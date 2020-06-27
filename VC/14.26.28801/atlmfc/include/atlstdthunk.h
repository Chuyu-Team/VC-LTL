// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSTDTHUNK_H__
#define __ATLSTDTHUNK_H__

#pragma once

#pragma push_macro("new")
#undef new

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#if !defined(USE_ATL_THUNK2) && (defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64))
#define USE_ATL_THUNK2
#endif

#include <atlthunk.h>

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Thunks for __stdcall member functions

#if defined(_M_IX86)
PVOID __stdcall __AllocStdCallThunk(VOID);
VOID  __stdcall __FreeStdCallThunk(_In_opt_ PVOID);

#pragma pack(push,1)
struct _stdcallthunk
{
	DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
	DWORD   m_this;         //
	BYTE    m_jmp;          // jmp WndProc
	DWORD   m_relproc;      // relative jmp
	BOOL Init(
		_In_ DWORD_PTR proc,
		_In_opt_ void* pThis)
	{
		m_mov = 0x042444C7;  //C7 44 24 0C
		m_this = PtrToUlong(pThis);
		m_jmp = 0xe9;
		m_relproc = DWORD((INT_PTR)proc - ((INT_PTR)this+sizeof(_stdcallthunk)));
		// write block from data cache and
		//  flush from instruction cache
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
		return TRUE;
	}
	//some thunks will dynamically allocate the memory for the code
	void* GetCodeAddress()
	{
		return this;
	}
	_Ret_maybenull_ _Post_writable_byte_size_(sizeof(_stdcallthunk)) void* operator new(_In_ size_t)
	{
        return __AllocStdCallThunk();
    }
    void operator delete(_In_opt_ void* pThunk)
    {
        __FreeStdCallThunk(pThunk);
    }
};
#pragma pack(pop)

#elif defined(_M_X64)
PVOID __stdcall __AllocStdCallThunk(VOID);
VOID  __stdcall __FreeStdCallThunk(PVOID);
#pragma pack(push,2)
struct _stdcallthunk
{
    USHORT  RcxMov;         // mov rcx, pThis
    ULONG64 RcxImm;         //
    USHORT  RaxMov;         // mov rax, target
    ULONG64 RaxImm;         //
    USHORT  RaxJmp;         // jmp target
    BOOL Init(
		_In_ DWORD_PTR proc,
		_In_opt_ void *pThis)
    {
        RcxMov = 0xb948;          // mov rcx, pThis
        RcxImm = (ULONG64)pThis;  //
        RaxMov = 0xb848;          // mov rax, target
        RaxImm = (ULONG64)proc;   //
        RaxJmp = 0xe0ff;          // jmp rax
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
		return TRUE;
    }
	//some thunks will dynamically allocate the memory for the code
	void* GetCodeAddress()
	{
		return this;
	}
	_Ret_maybenull_ _Post_writable_byte_size_(sizeof(_stdcallthunk)) void* operator new(_In_ size_t)
	{
        return __AllocStdCallThunk();
    }
    void operator delete(_In_opt_ void* pThunk)
    {
        __FreeStdCallThunk(pThunk);
    }
};
#pragma pack(pop)
#elif defined (_M_THUMB)
// note this case must be before _M_ARM because _M_ARM is also defined
PVOID __stdcall __AllocStdCallThunk(VOID);
VOID  __stdcall __FreeStdCallThunk(PVOID);
#pragma pack(push,2)
struct _stdcallthunk
{
	USHORT	m_mov_r0[2];	// mov	r0, pThis
	USHORT	m_mov_pc[2];	// mov	pc, pFunc
	DWORD	m_pThis;
	DWORD	m_pFunc;
	BOOL Init(DWORD_PTR proc, void* pThis)
	{
		m_mov_r0[0] = 0xF8DF;
		m_mov_r0[1] = 0x0004;
 		m_mov_pc[0] = 0xF8DF;
		m_mov_pc[1] = 0xF004;
		m_pThis = (DWORD)pThis;
		m_pFunc = (DWORD)proc;
		// write block from data cache and
		//  flush from instruction cache
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
		return TRUE;
	}
	void* GetCodeAddress()
	{
		return (void *)((ULONG_PTR)this | 1);
	}
	void* operator new(size_t)
	{
		return __AllocStdCallThunk();
    }
	void operator delete(void* pThunk)
	{
		__FreeStdCallThunk(pThunk);
	}
};
#pragma pack(pop)
#elif defined(_M_ARM64)
PVOID __stdcall __AllocStdCallThunk(VOID);
VOID  __stdcall __FreeStdCallThunk(PVOID);
#pragma pack(push,4)
struct _stdcallthunk {
    ULONG   m_ldr_r16;      // ldr  x16, [pc, #24]
    ULONG   m_ldr_r0;       // ldr  x0, [pc, #12]
    ULONG   m_br;           // br   x16
    ULONG   m_pad;
    ULONG64	m_pThis;
    ULONG64	m_pFunc;
    BOOL Init(DWORD_PTR proc, void* pThis) {
        m_ldr_r16 = 0x580000D0;
        m_ldr_r0 = 0x58000060;
        m_br = 0xd61f0200;
        m_pThis = (ULONG64)pThis;
        m_pFunc = (ULONG64)proc;
        // write block from data cache and
        //  flush from instruction cache
        FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
        return TRUE;
    }
    void* GetCodeAddress() {
        return (void *)((ULONG_PTR)this);
    }
    void* operator new(size_t)
    {
        return __AllocStdCallThunk();
    }
    void operator delete(void* pThunk) {
        __FreeStdCallThunk(pThunk);
    }
};
#pragma pack(pop)
#elif defined(_ARM_)
#pragma pack(push,4)
struct _stdcallthunk // this should come out to 16 bytes
{
	DWORD	m_mov_r0;		// mov	r0, pThis
	DWORD	m_mov_pc;		// mov	pc, pFunc
	DWORD	m_pThis;
	DWORD	m_pFunc;
	BOOL Init(
		_In_ DWORD_PTR proc,
		_In_opt_ void* pThis)
	{
		m_mov_r0 = 0xE59F0000;
		m_mov_pc = 0xE59FF000;
		m_pThis = (DWORD)pThis;
		m_pFunc = (DWORD)proc;
		// write block from data cache and
		//  flush from instruction cache
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
		return TRUE;
	}
	void* GetCodeAddress()
	{
		return this;
	}
};
#pragma pack(pop)
#else
#error Only X86, X64, ARM, and ARM64 supported
#endif


#pragma pack(push,8)

// avoid ODR violations
#ifdef USE_ATL_THUNK2
typedef class CDynamicStdCallThunk2 CDynamicStdCallThunk; class CDynamicStdCallThunk2
#else
class CDynamicStdCallThunk
#endif
{
public:
#ifdef USE_ATL_THUNK2
	AtlThunkData_t* pThunk;
#else
	_stdcallthunk *pThunk;
#endif

#ifdef USE_ATL_THUNK2
	CDynamicStdCallThunk2()
#else
	CDynamicStdCallThunk()
#endif
	{
		pThunk = NULL;
	}

#ifdef USE_ATL_THUNK2
	~CDynamicStdCallThunk2()
#else
	~CDynamicStdCallThunk()
#endif
	{
		if (pThunk)
		{
#ifdef USE_ATL_THUNK2
			AtlThunk_FreeData(pThunk);
#else
			delete pThunk;
#endif
		}
	}

	BOOL Init(
		_In_ DWORD_PTR proc,
		_In_opt_ void *pThis)
	{
		if (pThunk == NULL)
		{
#ifdef USE_ATL_THUNK2
			pThunk = AtlThunk_AllocateData();
#else
			pThunk = new _stdcallthunk;
#endif
			if (pThunk == NULL)
			{
				return FALSE;
			}
		}

#ifdef USE_ATL_THUNK2
		AtlThunk_InitData(pThunk, (void*)proc, (size_t)pThis);
		return TRUE;
#else
		return pThunk->Init(proc, pThis);
#endif
	}


	void* GetCodeAddress()
	{
#ifdef USE_ATL_THUNK2
		return AtlThunk_DataToCode(pThunk);
#else
		return pThunk->GetCodeAddress();
#endif
	}
};

#pragma pack(pop)
typedef CDynamicStdCallThunk CStdCallThunk;

}   // namespace ATL


#pragma pop_macro("new")

#endif // __ATLSTDTHUNK_H__
