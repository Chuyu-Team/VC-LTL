//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <agile.h>
#include <memory>
#include <roerrorapi.h>
#pragma hdrstop

namespace Platform { namespace Details {

	typedef struct {
		SRWLOCK targetsLock;
		SRWLOCK addRemoveLock;
	} EventLockInternal;

	static_assert(sizeof(EventLock) == sizeof(EventLockInternal), "EventLock struct size must be the same as EventLockInternal");

	typedef ::Platform::Agile< ::Platform::Object, true> TDelegate;

	class EventTargetArray : public ::Microsoft::WRL::RuntimeClass<IInspectable>
	{
		InspectableClass(nullptr, BaseTrust)
	public:
		EventTargetArray() throw()
		{
		}

		HRESULT RuntimeClassInitialize(size_t count) throw()
		{
			_begin = new(std::nothrow) std::shared_ptr<TDelegate>[count];
			if (_begin == nullptr)
			{
				::RoOriginateError(E_OUTOFMEMORY, nullptr);
				return E_OUTOFMEMORY;
			}

			_end = _begin;
			return S_OK;
		}

		~EventTargetArray() throw()
		{
			delete [] _begin;
		}

		unsigned int GetSize() throw()
		{
			return static_cast<unsigned int>(_end - _begin);
		}

		void* GetAt(unsigned int index, __int64* token)
		{
			std::shared_ptr<TDelegate> *_curr = _begin + index;

			*token = reinterpret_cast<size_t>(reinterpret_cast<void*>(_curr->get()));

			::Platform::Object^ obj = _curr->get()->Get();

			auto ret = reinterpret_cast<void*>(const_cast< ::Platform::Object^>(obj));
			*(__abi_IUnknown**)(&obj) = nullptr;
			return ret;
		}

		std::shared_ptr<TDelegate>* Begin() throw()
		{
			return _begin;
		}

		std::shared_ptr<TDelegate>* End() throw()
		{
			return _end;
		}

		__int64 AddTail(_In_ std::shared_ptr<TDelegate>* element) throw()
		{
			*_end = *element;
			_end++;
			return reinterpret_cast<size_t>(reinterpret_cast<void*>(element->get()));
		}

		static __int64 ExtractCookie(std::shared_ptr<TDelegate> *ptr)
		{
			return reinterpret_cast<size_t>(reinterpret_cast<void*>(ptr->get()));
		}
	private:
		std::shared_ptr<TDelegate> *_begin;
		std::shared_ptr<TDelegate> *_end;
	};

	CPPCLI_FUNC void __stdcall EventSourceInitialize(void** targets)
	{
		*targets = nullptr;
	}

	CPPCLI_FUNC void __stdcall EventSourceUninitialize(void** targets)
	{
		// Release targets array
		::Platform::Object^* targetsList = reinterpret_cast<::Platform::Object^*>(targets);
		*targetsList = nullptr;
	}

	CPPCLI_FUNC void* __stdcall EventSourceGetTargetArray(void* targets, EventLock* lock)
	{
		auto lockData = reinterpret_cast<EventLockInternal*>(lock);

		auto targetsPointerLock = ::Microsoft::WRL::Wrappers::SRWLock::LockShared(&lockData->targetsLock);
		// AddRef on targetsList pointer must happen on lock
		auto targetsList = reinterpret_cast< ::Platform::Object^>(targets);
		return __detach_as_voidptr(reinterpret_cast<void**>(&targetsList));
	}


	CPPCLI_FUNC unsigned int __stdcall EventSourceGetTargetArraySize(void* targets)
	{
		auto arr = reinterpret_cast<EventTargetArray*>(targets);
		return arr->GetSize();
	}

	CPPCLI_FUNC void* __stdcall EventSourceGetTargetArrayEvent(void* targets, unsigned int index, const void* iid, __int64* token)
	{
		auto arr = reinterpret_cast<EventTargetArray*>(targets);
		Microsoft::WRL::ComPtr<IUnknown> element;
		element.Attach(reinterpret_cast<IUnknown*>(arr->GetAt(index, token)));

		Microsoft::WRL::ComPtr<IUnknown> delegateObj;
		long hr = element.AsIID(*reinterpret_cast<const IID*>(iid), &delegateObj);
		 __abi_ThrowIfFailed(hr);

		 return delegateObj.Detach();
	}

	CPPCLI_FUNC ::Windows::Foundation::EventRegistrationToken __stdcall EventSourceAdd(void** targets, EventLock* lock, ::Platform::Delegate^ delegateInterface)
	{
		// lock add/remove
		//    pNewList = _targets
		//    pNewList[last] = delegateInterface
		//    lock pointer exhange
		//        pTmpList = _targets
		//        _targets = pNewList
		//    unlock pointer exhange
		// unlock add/remove

		::Windows::Foundation::EventRegistrationToken token = { 0 };
		auto lockData = reinterpret_cast<EventLockInternal*>(lock);

		// Make sure that delegate interface pointer is not null
		if (delegateInterface == nullptr)
		{
			__abi_WinRTraiseInvalidArgumentException();
		}

		std::shared_ptr<TDelegate> delegateObj;
		try {
			delegateObj = std::make_shared<TDelegate>(delegateInterface);
		} catch (const ::std::bad_alloc&) {
			throw ref new ::Platform::OutOfMemoryException();
		}

		// This must be defined outside of the scope where the addRemoveLock is taken to ensure that it's destructor is called after the lock is released
		Microsoft::WRL::ComPtr<Details::EventTargetArray> pTempList;

		{
			// lock scope for addRemoveLock
			// We are doing "copy to new list and add" so as not to disturb the list that may be
			// currently undergoing a walk and fire (invoke).

			// The addRemoveLock_ prevents multiple threads from doing simultaneous adds.
			// An invoke may be occurring during an add or remove operation.
			auto addRemovePointerLock = ::Microsoft::WRL::Wrappers::SRWLock::LockExclusive(&lockData->addRemoveLock);

			// Casting void* _targets to array type
			auto targets_ = reinterpret_cast<EventTargetArray*>(*targets);

			Microsoft::WRL::ComPtr<Details::EventTargetArray> pNewList;

			// Allocate event array
			HRESULT hr = Microsoft::WRL::MakeAndInitialize<EventTargetArray>(pNewList.GetAddressOf(), targets_ == nullptr ? 1 : targets_->GetSize() + 1);
			// Make sure allocation succeeded
			if (FAILED(hr))
			{
				throw ::Platform::Exception::CreateException(hr);
			}

			// The list may not exist if nobody has registered
			if (targets_)
			{
				for (auto element = targets_->Begin(); element != targets_->End(); element++)
				{
					pNewList->AddTail(element);
				}
			}

			// AddTail operation will take a reference which will result in
			// this function adding one reference count on delegateInterface.
			// Get unique token value
			token.Value = pNewList->AddTail(&delegateObj);

			{
				// lock scope for targetsPointerLock
				// The targetsPointerLock_ protects the exchanging of the new list (with the addition)
				// for the old list (which could be used currently for firing events)
				auto targetsPointerLock = ::Microsoft::WRL::Wrappers::SRWLock::LockExclusive(&lockData->targetsLock);

				// We move targets_ to pTempList so that we can actually delete the list while
				// not holding any locks. The InvokeAll method may still have a reference to targets_ so
				// even when pTempList releases, this might not delete what was in targets_.
				pTempList.Attach(targets_);

				// We're done with pNewList, so just move it to targets_.
				targets_ = pNewList.Detach();

				// Assigning back local pointer to global data
				*targets = reinterpret_cast<void*>(targets_);

			} // end lock scope for targetsPointerLock
		} // end lock scope for addRemoveLock

		// Destroys pTempList here (this is the old targets_) end lock scope for addRemoveLock

		return token;
	}

	CPPCLI_FUNC void __stdcall EventSourceRemove(void** targets, EventLock* lock, ::Windows::Foundation::EventRegistrationToken token)
	{
		// lock add/remove
		//    pNewList = _targets - delegate[token]
		//    lock pointer exhange
		//        pTmpList = _targets
		//        _targets = pNewList
		//    unlock pointer exhange
		// unlock add/remove
		auto lockData = reinterpret_cast<EventLockInternal*>(lock);

		// Used for deleting the current array without holding the addRemoveLock.
		Microsoft::WRL::ComPtr<EventTargetArray> pTempList;
		{ // lock scope for addRemoveLock_
			// The addRemoveLock_ prevents multiple threads from doing simultaneous adds/removes.
			// An invoke may be occurring during an add or remove operation.
			auto addRemovePointerLock = ::Microsoft::WRL::Wrappers::SRWLock::LockExclusive(&lockData->addRemoveLock);

			// Casting void* _targets to array type
			auto targets_ = reinterpret_cast<EventTargetArray*>(*targets);

			if (targets_ == nullptr)
			{
				return; // List is currently empty - thus token wasn't found, just return
			}

			Microsoft::WRL::ComPtr<EventTargetArray> pNewList;
			size_t availableSlots = targets_->GetSize() - 1;
			bool removed = false;
			// If one element in the array
			if (availableSlots == 0)
			{
				if (EventTargetArray::ExtractCookie(targets_->Begin()) == token.Value)
				{
					removed = true;
				}
			}
			else
			{
				// Instantiate EventTargetArray
				HRESULT hr = ::Microsoft::WRL::MakeAndInitialize<EventTargetArray>(pNewList.GetAddressOf(), availableSlots);
				if (FAILED(hr))
				{
					throw ::Platform::Exception::CreateException(hr);
				}

				for (auto element = targets_->Begin(); element != targets_->End(); element++)
				{
					if (!removed && token.Value == EventTargetArray::ExtractCookie(element))
					{
						removed = true;
						continue;
					}

					// The ComPtr<TDelegateInterface> contained in p is assigned to a
					// ComPtr<TDelegateInterface> of a new node in pNewList. The net result is
					// an addref on the interface.
					if (availableSlots == 0)
					{
						// We don't have any availableSlots left in the target array, hence every item was copied
						// from the source array.
						// This means we didn't find the item in the list - just return.
						__WRL_ASSERT__(!removed && "Attempt to remove token that was not added to this EventSource<>");
						break;
					}

					// Copy every registrant from old list except the item being removed
					// The ComPtr<TDelegateInterface> contained in p is assigned to a
					// ComPtr<TDelegateInterface> of a new node in pNewList. The net result is
					// an addref on the interface.
					pNewList->AddTail(element);
					availableSlots--;
				}
			}

			if (removed)
			{
				// lock scope for targetsPointerLock
				// The targetsPointerLock_ protects the exchanging of the new list (with the removal)
				// for the old list (which could be used currently for firing events)
				auto targetsPointerLock = ::Microsoft::WRL::Wrappers::SRWLock::LockExclusive(&lockData->targetsLock);

				// We move targets_ to pTempList so that we can actually delete the list while
				// not holding any locks. The InvokeAll method may still have a reference to targets_ so
				// even when pTempList releases, this might not delete what was in targets_.
				pTempList.Attach(targets_);

				// We still have some items left inside pNewList, so move it to targets_.
				targets_ = pNewList.Detach();

				// Assigning back local pointer to global data
				*targets = reinterpret_cast<void*>(targets_);

				// If we don't have any items added, the Details::Move(targets_) above already set targets_ to nullptr. The result
				// is that now when pTempList destructs, it will cause what used to be targets_ to be freed.

			} // end lock scope for targetsPointerLock

		} // end lock scope for addRemoveLock

		// Destroys pTempList here (this is the old targets_)
	}

} } // namespace Platform::Details
