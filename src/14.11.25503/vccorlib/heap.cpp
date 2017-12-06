//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma hdrstop

#pragma warning(disable: 4672 4673)

namespace Platform { namespace Details {

	volatile HANDLE __abi_heap = nullptr;
	volatile long __abi_heap_objectcount = 0;
	volatile long __abi_heap_object_counter = 0;
	volatile LPCRITICAL_SECTION __abi_heap_critsec = nullptr;

	bool  __abi_firstAlloc = true;
	bool  __abi_is_global_oom_init = false;
	void* __abi_oom_controlblock = nullptr;
	IUnknown* __abi_oom_singleton = nullptr;

	::Platform::Details::HeapAllocationTrackingLevel __abi_heap_trackingLevel = ::Platform::Details::HeapAllocationTrackingLevel::NoTracking;

	size_t __abi_heap_break_on_id_new = 0;
	size_t __abi_heap_break_on_id_delete = 0;

	struct __abi_Mem_block
	{
		__abi_Mem_block     *previous;
		__abi_Mem_block     *next;
		size_t        num_bytes;
		size_t        alloc_id;
		bool          allocated;
		union
		{
			double      to_force_align;
#pragma warning(push)
#pragma warning(disable: 4200)
			char        start_of_alloc[];
#pragma warning(pop)
		};
	};

	__abi_Mem_block* __abi_heap_chain = nullptr;

	LPCRITICAL_SECTION __abi_heap_getcritsec()
	{
// Prefast error 28112: Mixing access with/without Interlocked functions can be dangerous but this code just tests for NULL
_Benign_race_begin_
		if (__abi_heap_critsec)
		{
			return __abi_heap_critsec;
		}
_Benign_race_end_

		LPCRITICAL_SECTION critsec = new CRITICAL_SECTION();
		if (!critsec)
		{
			throw ref new OutOfMemoryException();
		}

		::InitializeCriticalSectionEx(critsec, 0, 0);

		void* previous = InterlockedCompareExchangePointer((void* volatile *)&__abi_heap_critsec, critsec, nullptr);
		if (previous)
		{
			DeleteCriticalSection(critsec);
			delete critsec;
			return (LPCRITICAL_SECTION)previous;
		}

		return critsec;
	}

	static void* HeapAllocateHelper(SizeT size, SizeT alignment, SizeT offset)
	{
		__abi_firstAlloc = false;

		if (!__abi_heap)
		{
			HANDLE heap = ::GetProcessHeap();
			if (!heap)
			{
				throw ref new ::Platform::COMException(HRESULT_FROM_WIN32(GetLastError()));
			}

			InterlockedCompareExchangePointer(&__abi_heap, heap, nullptr);
		}

		switch (__abi_heap_trackingLevel)
		{
		case ::Platform::Details::HeapAllocationTrackingLevel::NoTracking:
			{
				LPVOID pMem = (alignment == 0)
					? HeapAlloc(__abi_heap, 0, size)
					: _aligned_offset_malloc(size, alignment, offset);
				if (pMem == nullptr)
				{
					throw reinterpret_cast<Platform::OutOfMemoryException^>(Platform::Details::__abi_oom_singleton);
				}

				memset(pMem, 0, size);

				return pMem;
			}
			break;

		case ::Platform::Details::HeapAllocationTrackingLevel::ObjectCount:
			{
				LPVOID pMem = (alignment == 0)
					? HeapAlloc(__abi_heap, 0, size)
					: _aligned_offset_malloc(size, alignment, offset);
				if (pMem == nullptr)
				{
					throw reinterpret_cast<Platform::OutOfMemoryException^>(Platform::Details::__abi_oom_singleton);
				}

				memset(pMem, 0, size);
				InterlockedIncrement(&__abi_heap_objectcount);
				return pMem;
			}
			break;

		case ::Platform::Details::HeapAllocationTrackingLevel::ObjectInstances:
		case ::Platform::Details::HeapAllocationTrackingLevel::BreakOnErrors:
			{
				size_t memBlockSize = offsetof(__abi_Mem_block, start_of_alloc);
				size += memBlockSize;
				LPVOID pMem = (alignment == 0)
					? HeapAlloc(__abi_heap, 0, size)
					: _aligned_offset_malloc(size, alignment, offset + memBlockSize);
				__abi_Mem_block* ptr = reinterpret_cast<__abi_Mem_block*>(pMem);
				if (ptr == nullptr)
				{
					throw reinterpret_cast<Platform::OutOfMemoryException^>(Platform::Details::__abi_oom_singleton);
				}

				memset(pMem, 0, size);
				InterlockedIncrement(&__abi_heap_objectcount);

				ptr->alloc_id = InterlockedIncrement(&__abi_heap_object_counter);
				if (ptr->alloc_id == __abi_heap_break_on_id_new)
				{
					OutputDebugStringW(L"Object matching Heap::BreakOnAllocationId value is being created\r\n");
					__debugbreak();
				}

				ptr->allocated = true;

				LPCRITICAL_SECTION cs = __abi_heap_getcritsec();
				EnterCriticalSection(cs);

				if (__abi_heap_chain)
				{
					__abi_heap_chain->previous = ptr;
				}

				ptr->previous = nullptr;
				ptr->next = __abi_heap_chain;

				__abi_heap_chain = ptr;

				LeaveCriticalSection(cs);

				return &ptr->start_of_alloc[0];
			}
		}

		// Should never happen:
		return nullptr;
	}

	static void HeapFreeHelper(void* p, bool isAligned, void* pOriginalObjectPtr)
	{
		switch (__abi_heap_trackingLevel)
		{
		case ::Platform::Details::HeapAllocationTrackingLevel::NoTracking:
			{
				isAligned ? _aligned_free(p) : HeapFree(__abi_heap, 0, p);
			}
			break;

		case ::Platform::Details::HeapAllocationTrackingLevel::ObjectCount:
			{
				isAligned ? _aligned_free(p) : HeapFree(__abi_heap, 0, p);
				InterlockedDecrement(&__abi_heap_objectcount);
			}
			break;

		case ::Platform::Details::HeapAllocationTrackingLevel::ObjectInstances:
			{
				// Global out-of-memory singleton is special. It's not registered in the heap tracker since it's created before giving
				// the user the chance to set the tracking level.
				if ( (pOriginalObjectPtr == Platform::Details::__abi_oom_singleton) || (pOriginalObjectPtr == Platform::Details::__abi_oom_controlblock) )
				{
					isAligned ? _aligned_free(p) : HeapFree(__abi_heap, 0, p);
					break;
				}

				__abi_Mem_block *pBlock = reinterpret_cast<__abi_Mem_block*>( (char*)p - offsetof(__abi_Mem_block, start_of_alloc) );

				LPCRITICAL_SECTION cs = __abi_heap_getcritsec();
				EnterCriticalSection(cs);

				if (Platform::Details::__abi_heap_chain == pBlock)
				{
					Platform::Details::__abi_heap_chain = pBlock->next;
				}

				if (pBlock->next)
				{
					pBlock->next->previous = pBlock->previous;
				}

				if (pBlock->previous)
				{
					pBlock->previous->next = pBlock->next;
				}

				LeaveCriticalSection(cs);

				if (pBlock->alloc_id == __abi_heap_break_on_id_delete)
				{
					OutputDebugStringW(L"Object matching Heap::BreakOnFreeId value is being freed\r\n");
					__debugbreak();
				}

				isAligned ? _aligned_free(pBlock) : HeapFree(__abi_heap, 0, pBlock);
				InterlockedDecrement(&__abi_heap_objectcount);
			}
			break;

		case ::Platform::Details::HeapAllocationTrackingLevel::BreakOnErrors:
			{
				// Global out-of-memory singleton is special. It's not registered in the heap tracker since it's created before giving
				// the user the chance to set the tracking level.
				if ( (pOriginalObjectPtr == Platform::Details::__abi_oom_singleton) || (pOriginalObjectPtr == Platform::Details::__abi_oom_controlblock) )
				{
					break;
				}

				__abi_Mem_block *pBlock = reinterpret_cast<__abi_Mem_block*>( (char*)p - offsetof(__abi_Mem_block, start_of_alloc) );

				LPCRITICAL_SECTION cs = __abi_heap_getcritsec();
				EnterCriticalSection(cs);

				if (!pBlock->allocated)
				{
					OutputDebugStringW(L"Attempted double-free of pointer\r\n");
					__debugbreak();
				}

				pBlock->allocated = false;

				LeaveCriticalSection(cs);

				InterlockedDecrement(&__abi_heap_objectcount);

				if (pBlock->alloc_id == __abi_heap_break_on_id_delete)
				{
					OutputDebugStringW(L"Object matching Heap::BreakOnFreeId value is being freed\r\n");
					__debugbreak();
				}
			}
		}
	}

	VCCORLIB_API void* Heap::Allocate(SizeT size)
	{
		return HeapAllocateHelper(size, 0, 0);
	}

	class HeapMemoryHelper
	{
		void* __memory;
	public:
		HeapMemoryHelper(void* __mem) :
			__memory(__mem)
		{
		}
		~HeapMemoryHelper()
		{
			if (__memory != nullptr)
			{
				Details::Heap::Free(__memory);
			}
		}
		void* Get()
		{
			return __memory;
		}
		void* Detach()
		{
			void* __ret = __memory;
			__memory = nullptr;
			return __ret;
		}
	};

	VCCORLIB_API void* Details::Heap::Allocate(::Platform::SizeT __rcOffset, ::Platform::SizeT __sizeArg)
	{
		const static int __singleAllocationThreshold = sizeof(void*) * 16;
		if (__sizeArg <= __singleAllocationThreshold)
		{
#ifdef  _WIN64
			const auto __sizeControlBlock = (sizeof(ControlBlock)) + (((sizeof(ControlBlock) % 16) != 0) ? 16 - (sizeof(ControlBlock) % 16) : 0);
#else
			const auto __sizeControlBlock = (sizeof(ControlBlock)) + (((sizeof(ControlBlock) % 8) != 0) ? 8 - (sizeof(ControlBlock) % 8) : 0);
#endif
			auto __memory = Details::Heap::Allocate(__sizeControlBlock + __sizeArg);

			// Initialize Control Block
			ControlBlock* __controlBlock = reinterpret_cast<ControlBlock*>(__memory);
			__controlBlock->InitControlBlock(reinterpret_cast<::default::uint8*>(__memory) + __sizeControlBlock, true);

			// Wire control block in object
			*reinterpret_cast<ControlBlock**>
				(reinterpret_cast<::default::uint8*>(__memory) + __sizeControlBlock + __rcOffset) = __controlBlock;

			return reinterpret_cast<::default::uint8*>(__memory) + __sizeControlBlock;
		}
		else
		{
			const auto __sizeControlBlock = sizeof(ControlBlock);
			HeapMemoryHelper __controlBlockMemory(Details::Heap::Allocate(__sizeControlBlock));
			HeapMemoryHelper __objectMemory(Details::Heap::Allocate(__sizeArg));

			// Initialize Control Block
			ControlBlock* __controlBlock = reinterpret_cast<ControlBlock*>(__controlBlockMemory.Get());
			__controlBlock->InitControlBlock(reinterpret_cast<::default::uint8*>(__objectMemory.Get()), false);

			// Wire control block in object
			*reinterpret_cast<ControlBlock**>
				(reinterpret_cast<::default::uint8*>(__objectMemory.Get()) + __rcOffset) = __controlBlock;

			__controlBlockMemory.Detach();
			return __objectMemory.Detach();
		}
	}

	VCCORLIB_API void* Details::Heap::AlignedAllocate(::Platform::SizeT __rcOffset, ::Platform::SizeT __sizeArg, ::Platform::SizeT __alignment)
	{
		const static int __singleAllocationThreshold = sizeof(void*) * 16;
		if (__sizeArg <= __singleAllocationThreshold)
		{
#ifdef  _WIN64
			const auto __sizeControlBlock = (sizeof(ControlBlock)) + (((sizeof(ControlBlock) % 16) != 0) ? 16 - (sizeof(ControlBlock) % 16) : 0);
#else
			const auto __sizeControlBlock = (sizeof(ControlBlock)) + (((sizeof(ControlBlock) % 8) != 0) ? 8 - (sizeof(ControlBlock) % 8) : 0);
#endif
			auto __alignedSizeControlBlock = (__sizeControlBlock % __alignment) ?
				__sizeControlBlock : __sizeControlBlock + __alignment - (__sizeControlBlock % __alignment);
			auto __memory = Details::Heap::AlignedAllocate(__alignedSizeControlBlock + __sizeArg, __alignment);

			// Initialize Control Block
			ControlBlock* __controlBlock = reinterpret_cast<ControlBlock*>(__memory);
			__controlBlock->InitControlBlock(reinterpret_cast<::default::uint8*>(__memory) + __alignedSizeControlBlock, true, true);

			// Wire control block in object
			*reinterpret_cast<ControlBlock**>
				(reinterpret_cast<::default::uint8*>(__memory) + __alignedSizeControlBlock + __rcOffset) = __controlBlock;

			return reinterpret_cast<::default::uint8*>(__memory) + __alignedSizeControlBlock;
		}
		else
		{
			const auto __sizeControlBlock = sizeof(ControlBlock);
			HeapMemoryHelper __controlBlockMemory(Details::Heap::Allocate(__sizeControlBlock));
			HeapMemoryHelper __objectMemory(Details::Heap::AlignedAllocate(__sizeArg, __alignment));

			// Initialize Control Block
			ControlBlock* __controlBlock = reinterpret_cast<ControlBlock*>(__controlBlockMemory.Get());
			__controlBlock->InitControlBlock(reinterpret_cast<::default::uint8*>(__objectMemory.Get()), false, true);

			// Wire control block in object
			*reinterpret_cast<ControlBlock**>
				(reinterpret_cast<::default::uint8*>(__objectMemory.Get()) + __rcOffset) = __controlBlock;

			__controlBlockMemory.Detach();
			return __objectMemory.Detach();
		}
	}

	VCCORLIB_API void* Details::Heap::AllocateException(::Platform::SizeT __rcOffset, ::Platform::SizeT __sizeArg)
	{
		const auto __sizeControlBlock = sizeof(ControlBlock);
		HeapMemoryHelper __controlBlockMemory(Details::Heap::Allocate(__sizeControlBlock));
		HeapMemoryHelper __objectMemory(Details::Heap::AllocateException(__sizeArg));

		// Initialize Control Block
		ControlBlock* __controlBlock = reinterpret_cast<ControlBlock*>(__controlBlockMemory.Get());
		__controlBlock->InitControlBlock(reinterpret_cast<::default::uint8*>(__objectMemory.Get()), false, false, true);

		// Wire control block in object
		*reinterpret_cast<ControlBlock**>
			(reinterpret_cast<::default::uint8*>(__objectMemory.Get()) + __rcOffset) = __controlBlock;

		__controlBlockMemory.Detach();
		return __objectMemory.Detach();
	}

	VCCORLIB_API void* Details::Heap::AlignedAllocateException(::Platform::SizeT __rcOffset, ::Platform::SizeT __sizeArg, ::Platform::SizeT __alignment)
	{
		const auto __sizeControlBlock = sizeof(ControlBlock);
		HeapMemoryHelper __controlBlockMemory(Details::Heap::Allocate(__sizeControlBlock));
		HeapMemoryHelper __objectMemory(Details::Heap::AlignedAllocateException(__sizeArg, __alignment));

		// Initialize Control Block
		ControlBlock* __controlBlock = reinterpret_cast<ControlBlock*>(__controlBlockMemory.Get());
		__controlBlock->InitControlBlock(reinterpret_cast<::default::uint8*>(__objectMemory.Get()), false, true, true);

		// Wire control block in object
		*reinterpret_cast<ControlBlock**>
			(reinterpret_cast<::default::uint8*>(__objectMemory.Get()) + __rcOffset) = __controlBlock;

		__controlBlockMemory.Detach();
		return __objectMemory.Detach();
	}

	VCCORLIB_API void Heap::Free(void* p)
	{
		HeapFreeHelper(p, false, p);
	}

	VCCORLIB_API void* Heap::AlignedAllocate(SizeT size, SizeT alignment)
	{
		// Alignment must be nonzero so helper knows to use _aligned_offset_malloc

		if (alignment == 0)
		{
			alignment = 1;
		}

		return HeapAllocateHelper(size, alignment, 0);
	}

	VCCORLIB_API void Heap::AlignedFree(void * p)
	{
		HeapFreeHelper(p, true, p);
	}

	VCCORLIB_API void* Heap::AllocateException(SizeT size)
	{
		// Allocate space for two pointers before the actual exception object.
		// The 2nd pointer (immediately before the object) will be used to point
		// to ExceptionInformation associated with the exception.  The space for
		// the first pointer is unused, and there just to keep the exception
		// object double-pointer aligned.

		size += 2 * sizeof(void *);

		void * pMem = Heap::Allocate(size);

		return reinterpret_cast<void **>(pMem) + 2;
	}

	VCCORLIB_API void Heap::FreeException(void* p)
	{
		void* pFree = reinterpret_cast<void **>(p) - 2;
		HeapFreeHelper(pFree, false, p);
	}

	VCCORLIB_API void* Heap::AlignedAllocateException(SizeT size, SizeT alignment)
	{
		// Allocate space for a single pointer before the actual exception object.
		// The pointer will be used to point to ExceptionInformation associated
		// with the exception.

		size += sizeof(void *);

		// Alignment must be nonzero so helper knows to use _aligned_offset_malloc

		if (alignment == 0)
		{
			alignment = 1;
		}

		void * pMem = HeapAllocateHelper(size, alignment, sizeof(void *));

		return reinterpret_cast<void **>(pMem) + 1;
	}

	VCCORLIB_API void Heap::AlignedFreeException(void * p)
	{
		void* pFree = reinterpret_cast<void **>(p) - 1;
		HeapFreeHelper(pFree, true, p);
	}

	VCCORLIB_API ::Platform::Details::HeapAllocationTrackingLevel Heap::TrackingLevel::get()
	{
		return __abi_heap_trackingLevel;
	}

	VCCORLIB_API void Heap::TrackingLevel::set(::Platform::Details::HeapAllocationTrackingLevel level)
	{
		if (!__abi_firstAlloc)
		{
			// Too late, we can't change it now, the objects will blow on on de-allocate.
			if ( (level == ::Platform::Details::HeapAllocationTrackingLevel::ObjectInstances) ||
				(level == ::Platform::Details::HeapAllocationTrackingLevel::BreakOnErrors) )
			{
				throw ref new FailureException();
			}
		}

		__abi_heap_trackingLevel = level;
	}

	VCCORLIB_API int Heap::CurrentAllocationId::get()
	{
		return __abi_heap_object_counter;
	}


	VCCORLIB_API int Heap::BreakOnAllocationId::get()
	{
		return static_cast<int>(__abi_heap_break_on_id_new);
	}

	VCCORLIB_API void Heap::BreakOnAllocationId::set(int id)
	{
		__abi_heap_break_on_id_new = id;
	}


	VCCORLIB_API int Heap::BreakOnFreeId::get()
	{
		return static_cast<int>(__abi_heap_break_on_id_delete);
	}

	VCCORLIB_API void Heap::BreakOnFreeId::set(int id)
	{
		__abi_heap_break_on_id_delete = id;
	}

	VCCORLIB_API int32 Heap::ObjectCount::get()
	{
		return __abi_heap_objectcount;
	}

	VCCORLIB_API void Heap::EnumerateAllocatedObjects(Platform::Details::HeapEntryHandler^ handler)
	{
		LPCRITICAL_SECTION cs = __abi_heap_getcritsec();
		EnterCriticalSection(cs);

		__abi_Mem_block *p;
		for (p = __abi_heap_chain; p != nullptr; p = p->next)
		{
			if (p->allocated)
			{
				ControlBlock* pCtlBlockStart = reinterpret_cast<ControlBlock*>(&p->start_of_alloc[0]);
				void* pObjStart = reinterpret_cast<void*>( pCtlBlockStart->__target );

				Object^* ppObj = reinterpret_cast<Object^*>(&pObjStart);
				handler(*ppObj, static_cast<int>(p->alloc_id));
			}
		}

		LeaveCriticalSection(cs);
	}
} } // Platform::Details
