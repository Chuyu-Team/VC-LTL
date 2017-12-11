// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.


#ifndef __ATLSYNC_H__
#define __ATLSYNC_H__

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(push)
#pragma warning(disable: 4512)  // assignment operator could not be generated
#endif  // !_ATL_NO_PRAGMA_WARNINGS

#include <atlbase.h>

#pragma pack(push,_ATL_PACKING)

namespace ATL
{

class CCriticalSection :
	public CRITICAL_SECTION
{
public:
	CCriticalSection();
	explicit CCriticalSection(_In_ ULONG nSpinCount);

	~CCriticalSection() throw();

	// Acquire the critical section
	_Acquires_lock_(*this)
	void Enter();
	// Release the critical section
	_Releases_lock_(*this)
	void Leave() throw();

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Set the spin count for the critical section
	ULONG SetSpinCount(_In_ ULONG nSpinCount) throw();
#endif
	// Attempt to acquire the critical section
	_When_(return != 0, _Acquires_lock_(*this))
	BOOL TryEnter() throw();
};

class CEvent :
	public CHandle
{
public:
	CEvent() throw();
	CEvent(_Inout_ CEvent& h) throw();
	CEvent(
		_In_ BOOL bManualReset,
		_In_ BOOL bInitialState);
	CEvent(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ BOOL bManualReset,
		_In_ BOOL bInitialState,
		_In_opt_z_ LPCTSTR pszName);
	explicit CEvent(_In_ HANDLE h) throw();

	// Create a new event
	BOOL Create(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ BOOL bManualReset,
		_In_ BOOL bInitialState,
		_In_opt_z_ LPCTSTR pszName) throw();
	// Open an existing named event
	BOOL Open(
		_In_ DWORD dwAccess,
		_In_ BOOL bInheritHandle,
		_In_z_ LPCTSTR pszName) throw();
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Pulse the event (signals waiting objects, then resets)
	BOOL Pulse() throw();
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// Set the event to the non-signaled state
	BOOL Reset() throw();
	// Set the event to the signaled state
	BOOL Set() throw();
};

class CMutex :
	public CHandle
{
public:
	CMutex() throw();
	CMutex(_Inout_ CMutex& h) throw();
	explicit CMutex(_In_ BOOL bInitialOwner);
	CMutex(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ BOOL bInitialOwner,
		_In_opt_z_ LPCTSTR pszName);
	explicit CMutex(_In_ HANDLE h) throw();

	// Create a new mutex
	BOOL Create(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ BOOL bInitialOwner,
		_In_opt_z_ LPCTSTR pszName) throw();
	// Open an existing named mutex
	BOOL Open(
		_In_ DWORD dwAccess,
		_In_ BOOL bInheritHandle,
		_In_z_ LPCTSTR pszName) throw();
	// Release ownership of the mutex
	_Releases_lock_(this->m_h)
	BOOL Release() throw();
};

class CSemaphore :
	public CHandle
{
public:
	CSemaphore() throw();
	CSemaphore(_Inout_ CSemaphore& h) throw();
	CSemaphore(
		_In_ LONG nInitialCount,
		_In_ LONG nMaxCount);
	CSemaphore(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ LONG nInitialCount,
		_In_ LONG nMaxCount,
		_In_opt_z_ LPCTSTR pszName );
	explicit CSemaphore(_In_ HANDLE h) throw();

	// Create a new semaphore
	BOOL Create(
		_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
		_In_ LONG nInitialCount,
		_In_ LONG nMaxCount,
		_In_opt_z_ LPCTSTR pszName) throw();
	// Open an existing named semaphore
	BOOL Open(
		_In_ DWORD dwAccess,
		_In_ BOOL bInheritHandle,
		_In_z_ LPCTSTR pszName) throw();
	// Increase the count of the semaphore
	BOOL Release(
		_In_ LONG nReleaseCount = 1,
		_Out_opt_ LONG* pnOldCount = NULL) throw();
};

class CMutexLock
{
public:
	_Post_same_lock_(mtx, this->m_mtx)
	_When_(bInitialLock != 0, _Acquires_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked != 0))
	_When_(bInitialLock == 0, _Post_satisfies_(this->m_bLocked == 0))
	CMutexLock(
		_Inout_ CMutex& mtx,
		_In_ bool bInitialLock = true);
	_When_(this->m_bLocked != 0, _Requires_lock_held_(this->m_mtx) _Releases_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked == 0))
	~CMutexLock() throw();

	_Acquires_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked != 0)
	void Lock();

	_Releases_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked == 0)
	void Unlock() throw();

// Implementation
private:
	CMutex& m_mtx;
	bool m_bLocked;

// Private to prevent accidental use
	CMutexLock(_In_ const CMutexLock&) throw();
	CMutexLock& operator=(_In_ const CMutexLock&) throw();
};

inline CCriticalSection::CCriticalSection()
{
	if (!::ATL::_AtlInitializeCriticalSectionEx( this, 0, 0 ))
	{
		AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
	}
}

inline CCriticalSection::CCriticalSection(_In_ ULONG nSpinCount)
{
	if (!::ATL::_AtlInitializeCriticalSectionEx( this, nSpinCount, 0 ))
	{
		AtlThrow(HRESULT_FROM_WIN32(GetLastError()));
	}
}

inline CCriticalSection::~CCriticalSection() throw()
{
	::DeleteCriticalSection( this );
}

_Acquires_lock_(*this)
inline void CCriticalSection::Enter()
{
	::EnterCriticalSection( this );
}

_Releases_lock_(*this)
inline void CCriticalSection::Leave() throw()
{
	::LeaveCriticalSection( this );
}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline ULONG CCriticalSection::SetSpinCount(_In_ ULONG nSpinCount) throw()
{
	return( ::SetCriticalSectionSpinCount( this, nSpinCount ) );
}

#endif

_When_(return != 0, _Acquires_lock_(*this))
inline BOOL CCriticalSection::TryEnter() throw()
{
	return( ::TryEnterCriticalSection( this ) );
}

inline CEvent::CEvent() throw()
{
}

inline CEvent::CEvent(_Inout_ CEvent& hEvent) throw() :
	CHandle( hEvent )
{
}

inline CEvent::CEvent(
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState)
{
	BOOL bSuccess;

	bSuccess = Create( NULL, bManualReset, bInitialState, NULL );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}

inline CEvent::CEvent(
	_In_opt_ LPSECURITY_ATTRIBUTES pAttributes,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_z_ LPCTSTR pszName)
{
	BOOL bSuccess;

	bSuccess = Create( pAttributes, bManualReset, bInitialState, pszName );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}


inline CEvent::CEvent(_In_ HANDLE h) throw() :
	CHandle( h )
{
}

inline BOOL CEvent::Create(
	_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	m_h = ::CreateEvent( pSecurity, bManualReset, bInitialState, pszName );
#else
	DWORD dwFlags = 0;

	if (bManualReset)
		dwFlags |= CREATE_EVENT_MANUAL_RESET;

	if (bInitialState)
		dwFlags |= CREATE_EVENT_INITIAL_SET;

	m_h = ::CreateEventEx( pSecurity, pszName, dwFlags, EVENT_ALL_ACCESS );
#endif

	return( m_h != NULL );
}

inline BOOL CEvent::Open(
	_In_ DWORD dwAccess,
	_In_ BOOL bInheritHandle,
	_In_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

	m_h = ::OpenEvent( dwAccess, bInheritHandle, pszName );
	return( m_h != NULL );
}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
inline BOOL CEvent::Pulse() throw()
{
	ATLASSUME( m_h != NULL );

	return( ::PulseEvent( m_h ) );
}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline BOOL CEvent::Reset() throw()
{
	ATLASSUME( m_h != NULL );

	return( ::ResetEvent( m_h ) );
}

inline BOOL CEvent::Set() throw()
{
	ATLASSUME( m_h != NULL );

	return( ::SetEvent( m_h ) );
}


inline CMutex::CMutex() throw()
{
}

inline CMutex::CMutex(_Inout_ CMutex& hMutex) throw() :
	CHandle( hMutex )
{
}

inline CMutex::CMutex(_In_ BOOL bInitialOwner)
{
	BOOL bSuccess;

	bSuccess = Create( NULL, bInitialOwner, NULL );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}

inline CMutex::CMutex(
	_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
	_In_ BOOL bInitialOwner,
	_In_opt_z_ LPCTSTR pszName)
{
	BOOL bSuccess;

	bSuccess = Create( pSecurity, bInitialOwner, pszName );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}

inline CMutex::CMutex(_In_ HANDLE h) throw() :
	CHandle( h )
{
}

inline BOOL CMutex::Create(
	_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
	_In_ BOOL bInitialOwner,
	_In_opt_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	m_h = ::CreateMutex( pSecurity, bInitialOwner, pszName );
#else
	m_h = ::CreateMutexEx( pSecurity, pszName, bInitialOwner? CREATE_MUTEX_INITIAL_OWNER : 0, MUTEX_ALL_ACCESS );
#endif
	return( m_h != NULL );
}

inline BOOL CMutex::Open(
	_In_ DWORD dwAccess,
	_In_ BOOL bInheritHandle,
	_In_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

	m_h = ::OpenMutex( dwAccess, bInheritHandle, pszName );
	return( m_h != NULL );
}

_Releases_lock_(this->m_h)
inline BOOL CMutex::Release() throw()
{
	ATLASSUME( m_h != NULL );

	return( ::ReleaseMutex( m_h ) );
}

inline CSemaphore::CSemaphore() throw()
{
}

inline CSemaphore::CSemaphore(_Inout_ CSemaphore& hSemaphore) throw() :
	CHandle( hSemaphore )
{
}

inline CSemaphore::CSemaphore(
	_In_ LONG nInitialCount,
	_In_ LONG nMaxCount)
{
	BOOL bSuccess = Create( NULL, nInitialCount, nMaxCount, NULL );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}

inline CSemaphore::CSemaphore(
	_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
	_In_ LONG nInitialCount,
	_In_ LONG nMaxCount,
	_In_opt_z_ LPCTSTR pszName)
{
	BOOL bSuccess;

	bSuccess = Create( pSecurity, nInitialCount, nMaxCount, pszName );
	if( !bSuccess )
	{
		AtlThrowLastWin32();
	}
}

inline CSemaphore::CSemaphore(_In_ HANDLE h) throw() :
	CHandle( h )
{
}

inline BOOL CSemaphore::Create(
	_In_opt_ LPSECURITY_ATTRIBUTES pSecurity,
	_In_ LONG nInitialCount,
	_In_ LONG nMaxCount,
	_In_opt_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	m_h = ::CreateSemaphore( pSecurity, nInitialCount, nMaxCount, pszName);
#else
	m_h = ::CreateSemaphoreEx( pSecurity, nInitialCount, nMaxCount, pszName, 0, SEMAPHORE_ALL_ACCESS);
#endif
	return( m_h != NULL );
}

inline BOOL CSemaphore::Open(
	_In_ DWORD dwAccess,
	_In_ BOOL bInheritHandle,
	_In_z_ LPCTSTR pszName) throw()
{
	ATLASSUME( m_h == NULL );

	m_h = ::OpenSemaphore( dwAccess, bInheritHandle, pszName );
	return( m_h != NULL );
}

inline BOOL CSemaphore::Release(
	_In_ LONG nReleaseCount,
	_Out_opt_ LONG* pnOldCount) throw()
{
	ATLASSUME( m_h != NULL );

	return( ::ReleaseSemaphore( m_h, nReleaseCount, pnOldCount ) );
}


_Post_same_lock_(mtx, this->m_mtx)
_When_(bInitialLock != 0, _Acquires_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked != 0))
_When_(bInitialLock == 0, _Post_satisfies_(this->m_bLocked == 0))
inline CMutexLock::CMutexLock(
		_Inout_ CMutex& mtx,
		_In_ bool bInitialLock) :
	m_mtx( mtx ),
	m_bLocked( false )
{
	if( bInitialLock )
	{
#pragma warning(suppress : 28313) // The C28313 warning associated with the following line is spurious.
		Lock();
	}
}

_When_(this->m_bLocked != 0, _Requires_lock_held_(this->m_mtx) _Releases_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked == 0))
inline CMutexLock::~CMutexLock() throw()
{
	if( m_bLocked )
	{
		Unlock();
	}
}

_Acquires_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked != 0)
inline void CMutexLock::Lock()
{
	DWORD dwResult;

	ATLASSERT( !m_bLocked );
	dwResult = ::WaitForSingleObjectEx( m_mtx, INFINITE, FALSE );
	if( dwResult == WAIT_ABANDONED )
	{
#pragma warning(push)
#pragma warning(disable: 4302 4311) // 'conversion' : truncation
		ATLTRACE(atlTraceSync, 0, _T("Warning: abandoned mutex 0x%x\n"), 
			reinterpret_cast<int>(static_cast<HANDLE>(m_mtx)));
#pragma warning(pop)
	}
	_Analysis_assume_lock_held_(this->m_mtx);
	m_bLocked = true;
}

_Releases_lock_(this->m_mtx) _Post_satisfies_(this->m_bLocked == 0)
inline void CMutexLock::Unlock() throw()
{
	ATLASSUME( m_bLocked );

	_Analysis_assume_lock_held_((this->m_mtx).m_h);
	m_mtx.Release();
	//ATLASSERT in CMutexLock::Lock prevents calling Lock more than 1 time.
	_Analysis_assume_lock_released_(this->m_mtx);
	m_bLocked = false;
}

};  // namespace ATL

#pragma pack(pop)
#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(pop)
#endif  // !_ATL_NO_PRAGMA_WARNINGS

#endif  // __ATLSYNC_H__
