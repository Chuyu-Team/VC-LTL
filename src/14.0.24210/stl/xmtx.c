/*	xmtx.c -- mutex support for VC++ */
#include "xmtx.h"
#include <awint.h>

 #if !_MULTI_THREAD
 #else /* !_MULTI_THREAD */
/* Win32 critical sections are recursive */

void  __CLRCALL_PURE_OR_CDECL _Mtxinit(_Rmtx *_Mtx)
	{	/* initialize mutex */
	__crtInitializeCriticalSectionEx(_Mtx, 4000, 0);
	}

void  __CLRCALL_PURE_OR_CDECL _Mtxdst(_Rmtx *_Mtx)
	{	/* delete mutex */
	DeleteCriticalSection(_Mtx);
	}

_RELIABILITY_CONTRACT
void  __CLRCALL_PURE_OR_CDECL _Mtxlock(_Rmtx *_Mtx)
	{	/* lock mutex */
#ifdef _M_CEE
	System::Threading::Thread::BeginThreadAffinity();
#endif /* _M_CEE */
	EnterCriticalSection(_Mtx);
	}

_RELIABILITY_CONTRACT
void  __CLRCALL_PURE_OR_CDECL _Mtxunlock(_Rmtx *_Mtx)
	{	/* unlock mutex */
	LeaveCriticalSection(_Mtx);
#ifdef _M_CEE
	System::Threading::Thread::EndThreadAffinity();
#endif /* _M_CEE */
	}
 #endif /* !_MULTI_THREAD */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
