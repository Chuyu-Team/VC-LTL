// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXMT.H

#pragma once

#ifndef __AFXMT_H__
#error afxmt.inl requires afxmt.h to be included first
#endif

#ifdef _AFXMT_INLINE

/* Special considerations for this file
Classes CSyncObject, CSemaphore, CEvent, CSingleLock, CMultiLock and CCriticalSection 
have the same names as classes in the ::ATL namespace. 
Typically, the ATL headers perform using namespace ATL, meaning that references 
to these classes would be ambiguous.
We want it to be possible to include ATL headers before including this file.
Thus, all references to the classes listed above from anywhere within this file 
should be preceded by :: to ensure that they unambiguously refer to the MFC classes 
in the global namespace */

_AFXMT_INLINE ::CSyncObject::operator HANDLE() const
{ 
	return m_hObject;
}

_AFXMT_INLINE BOOL (::CSemaphore::Unlock())
{ 
	return Unlock(1, NULL); 
}

_AFXMT_INLINE BOOL (::CEvent::SetEvent())
{ 
	ASSERT(m_hObject != NULL); 
	
	return ::SetEvent(m_hObject); 	
}

_AFXMT_INLINE BOOL (::CEvent::PulseEvent())
{ 
	ASSERT(m_hObject != NULL); 
	
	return ::PulseEvent(m_hObject); 
}

_AFXMT_INLINE BOOL (::CEvent::ResetEvent())
{ 
	ASSERT(m_hObject != NULL); 
	
	return ::ResetEvent(m_hObject); 
}

_AFXMT_INLINE ::CSingleLock::~CSingleLock()
{ 
	Unlock(); 
}

_AFXMT_INLINE BOOL (::CSingleLock::IsLocked())
{ 
	return m_bAcquired; 
}

_AFXMT_INLINE BOOL (::CMultiLock::IsLocked(DWORD dwObject))
{ 
	ASSERT(dwObject < m_dwCount);
	
	return m_bLockedArray[dwObject]; 
}

_AFXMT_INLINE HRESULT (::CCriticalSection::Init())
{
	if (!InitializeCriticalSectionAndSpinCount(&m_sect, 0))
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	return S_OK;
}

_AFXMT_INLINE ::CCriticalSection::CCriticalSection() : CSyncObject(NULL)
{ 		
	HRESULT hr = Init();
	
	if (FAILED(hr))
	{
		AtlThrow(hr);
	}		
}

_AFXMT_INLINE ::CCriticalSection::operator CRITICAL_SECTION*()
{
	return (CRITICAL_SECTION*) &m_sect; 
}

_AFXMT_INLINE ::CCriticalSection::~CCriticalSection()
{ 
	::DeleteCriticalSection(&m_sect); 
}

_AFXMT_INLINE BOOL (::CCriticalSection::Lock())
{	
	::EnterCriticalSection(&m_sect); 

	return TRUE; 
}

_AFXMT_INLINE BOOL (::CCriticalSection::Lock(DWORD dwTimeout))
{ 
	ASSERT(dwTimeout == INFINITE); 
	(void)dwTimeout; 
	
	return Lock(); 
}

_AFXMT_INLINE BOOL (::CCriticalSection::Unlock())
{ 
	::LeaveCriticalSection(&m_sect); 
	
	return TRUE; 
}

#endif //_AFXMT_INLINE
