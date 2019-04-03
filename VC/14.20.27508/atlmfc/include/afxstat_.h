// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXSTATE_H__
#define __AFXSTATE_H__

#pragma once


#ifndef __ATLTYPES_H__
#include <atltypes.h>
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef __AFXTLS_H__
	#include <afxtls_.h>
#endif

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/////////////////////////////////////////////////////////////////////////////
// _AFX_DEBUG_STATE

#ifdef _DEBUG

class _AFX_DEBUG_STATE : public CNoTrackObject
{
public:
	_AFX_DEBUG_STATE();
	virtual ~_AFX_DEBUG_STATE();
};

EXTERN_PROCESS_LOCAL(_AFX_DEBUG_STATE, afxDebugState)

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// _AFX_WIN_STATE

#undef AFX_DATA
#define AFX_DATA

class _AFX_WIN_STATE : public CNoTrackObject
{
public:
	// printing abort
	BOOL m_bUserAbort;
};

EXTERN_PROCESS_LOCAL(_AFX_WIN_STATE, _afxWinState)

/////////////////////////////////////////////////////////////////////////////
// Type library cache - AFX_INTERNAL

struct ITypeInfo;
typedef __RPC_unique_pointer ITypeInfo* LPTYPEINFO;

struct ITypeLib;
typedef __RPC_unique_pointer ITypeLib* LPTYPELIB;

typedef struct _GUID GUID;
#ifndef _REFCLSID_DEFINED
#define REFGUID const GUID &
#endif

class CTypeLibCache
{
public:
	CTypeLibCache() : m_lcid((LCID)-1), m_ptlib(NULL), m_ptinfo(NULL), m_cRef(0) {}
	void Lock();
	void Unlock();
	BOOL Lookup(LCID lcid, LPTYPELIB* pptlib);
	void Cache(LCID lcid, LPTYPELIB ptlib);
	BOOL LookupTypeInfo(LCID lcid, REFGUID guid, LPTYPEINFO* pptinfo);
	void CacheTypeInfo(LCID lcid, REFGUID guid, LPTYPEINFO ptinfo);
	const GUID* m_pTypeLibID;

protected:
	LCID m_lcid;
	LPTYPELIB m_ptlib;
	GUID m_guidInfo;
	LPTYPEINFO m_ptinfo;
	long m_cRef;
};

/////////////////////////////////////////////////////////////////////////////
// AFX_MODULE_STATE : portion of state that is pushed/popped

// forward references required for AFX_MODULE_THREAD_STATE definition
class CWinThread;
class CHandleMap;
class CFrameWnd;
class CToolTipCtrl;
class CControlBar;
class CWnd;

#ifndef _PNH_DEFINED
typedef int (__cdecl * _PNH)( size_t );
#define _PNH_DEFINED
#endif

template<class TYPE>
class CEmbeddedButActsLikePtr
{
public:
	AFX_INLINE TYPE* operator->() { return &m_data; }
	AFX_INLINE operator TYPE*() { return &m_data; }
	TYPE m_data;
};

// AFX_MODULE_THREAD_STATE (local to thread *and* module)
class AFX_MODULE_THREAD_STATE : public CNoTrackObject
{
public:
	AFX_MODULE_THREAD_STATE();
	virtual ~AFX_MODULE_THREAD_STATE();

	// current CWinThread pointer
	CWinThread* m_pCurrentWinThread;

	// list of CFrameWnd objects for thread
	CTypedSimpleList<CFrameWnd*> m_frameList;

	// temporary/permanent map state
	DWORD m_nTempMapLock;           // if not 0, temp maps locked
	CHandleMap* m_pmapHWND;
	CHandleMap* m_pmapHMENU;
	CHandleMap* m_pmapHDC;
	CHandleMap* m_pmapHGDIOBJ;
	CHandleMap* m_pmapHIMAGELIST;

	// thread-local MFC new handler (separate from C-runtime)
	_PNH m_pfnNewHandler;

	// WinSock specific thread state
	HWND m_hSocketWindow;
#ifdef _AFXDLL
	CEmbeddedButActsLikePtr<CMapPtrToPtr> m_pmapSocketHandle;
	CEmbeddedButActsLikePtr<CMapPtrToPtr> m_pmapDeadSockets;
	CEmbeddedButActsLikePtr<CPtrList> m_plistSocketNotifications;
#else
	CMapPtrToPtr* m_pmapSocketHandle;
	CMapPtrToPtr* m_pmapDeadSockets;
	CPtrList* m_plistSocketNotifications;
#endif

	// common controls thread state
	CToolTipCtrl* m_pToolTip;
	CWnd* m_pLastHit;       // last window to own tooltip
	INT_PTR m_nLastHit;         // last hittest code
	TOOLINFO* m_pLastInfo;    // last TOOLINFO structure
	INT_PTR m_nLastStatus;      // last flyby status message
	CControlBar* m_pLastStatus; // last flyby status control bar
};

// forward references required for AFX_MODULE_STATE definition
class CWinApp;
class COleObjectFactory;

class CWnd;

#ifdef _AFXDLL
class CDynLinkLibrary;
#endif

class COccManager;
class COleControlLock;

#ifndef _AFX_NO_DAO_SUPPORT
class _AFX_DAO_STATE;
#endif

class CComCtlWrapper;
class CCommDlgWrapper;

class CTypeLibCacheMap : public CMapPtrToPtr
{
public:
	virtual void RemoveAll(void* pExcept);
};


// AFX_MODULE_STATE (global data for a module)
class AFX_MODULE_STATE : public CNoTrackObject
{
public:
#ifdef _AFXDLL
	AFX_MODULE_STATE(BOOL bDLL, WNDPROC pfnAfxWndProc, DWORD dwVersion,
		BOOL bSystem = FALSE);
#else
	explicit AFX_MODULE_STATE(BOOL bDLL);
#endif
	~AFX_MODULE_STATE();

	CWinApp* m_pCurrentWinApp;
	HINSTANCE m_hCurrentInstanceHandle;
	HINSTANCE m_hCurrentResourceHandle;
	LPCTSTR m_lpszCurrentAppName;
	BYTE m_bDLL;    // TRUE if module is a DLL, FALSE if it is an EXE
	BYTE m_bSystem; // TRUE if module is a "system" module, FALSE if not
	BYTE m_bReserved[2]; // padding

	DWORD m_fRegisteredClasses; // flags for registered window classes

	// runtime class data
#ifdef _AFXDLL
	CRuntimeClass* m_pClassInit;
#endif
	CTypedSimpleList<CRuntimeClass*> m_classList;

	// OLE object factories
#ifdef _AFXDLL
	COleObjectFactory* m_pFactoryInit;
#endif
	CTypedSimpleList<COleObjectFactory*> m_factoryList;

	// number of locked OLE objects
	long m_nObjectCount;
	BOOL m_bUserCtrl;

	// AfxRegisterClass and AfxRegisterWndClass data

	CString m_strUnregisterList;

#ifdef _AFXDLL
	WNDPROC m_pfnAfxWndProc;
	DWORD m_dwVersion;  // version that module linked against
#endif

	// variables related to a given process in a module
	//  (used to be AFX_MODULE_PROCESS_STATE)
	void (PASCAL *m_pfnFilterToolTipMessage)(MSG*, CWnd*);

#ifdef _AFXDLL
	// CDynLinkLibrary objects (for resource chain)
	CTypedSimpleList<CDynLinkLibrary*> m_libraryList;

	// special case for MFCXXLLL.DLL (localized MFC resources)
	HINSTANCE m_appLangDLL;
#endif

	// OLE control container manager
	COccManager* m_pOccManager;
	// locked OLE controls
	CTypedSimpleList<COleControlLock*> m_lockList;

#ifndef _AFX_NO_DAO_SUPPORT
	_AFX_DAO_STATE* m_pDaoState;
#endif

	// Type library caches
	CTypeLibCache m_typeLibCache;
	CTypeLibCacheMap* m_pTypeLibCacheMap;

	// define thread local portions of module state
	CThreadLocal<AFX_MODULE_THREAD_STATE> m_thread;

	// bool indicating the return value of InitNetworkAddressControl() (from shell32.dll)
	BOOL m_bInitNetworkAddressControl;
	// bool indicating whether or not InitNetworkAddressControl() (from shell32.dll) has been called for CNetAddressCtrl
	BOOL m_bInitNetworkAddressControlCalled;
};

AFX_MODULE_STATE* AFXAPI AfxGetAppModuleState();
#ifdef _AFXDLL
AFX_MODULE_STATE* AFXAPI AfxSetModuleState(AFX_MODULE_STATE* pNewState) throw();
#endif
AFX_MODULE_STATE* AFXAPI AfxGetModuleState();
BOOL AFXAPI AfxIsModuleDll();
BOOL AFXAPI AfxInitCurrentStateApp();
AFX_MODULE_STATE* AFXAPI AfxGetStaticModuleState();
HINSTANCE AFXAPI AfxGetInstanceHandleHelper();

AFX_MODULE_THREAD_STATE* AFXAPI AfxGetModuleThreadState();

#ifdef _AFXDLL
#define _AFX_CMDTARGET_GETSTATE() (m_pModuleState)
#else
#define _AFX_CMDTARGET_GETSTATE() (AfxGetModuleState())
#endif

/////////////////////////////////////////////////////////////////////////////
// macros & classes to manage pushing/popping the module state

#ifdef _AFXDLL
struct AFX_MAINTAIN_STATE
{
	explicit AFX_MAINTAIN_STATE(AFX_MODULE_STATE* pModuleState) throw();
	~AFX_MAINTAIN_STATE();

protected:
	AFX_MODULE_STATE* m_pPrevModuleState;
};
#endif

class _AFX_THREAD_STATE;
struct AFX_MAINTAIN_STATE2
{
	explicit AFX_MAINTAIN_STATE2(AFX_MODULE_STATE* pModuleState) throw();
	~AFX_MAINTAIN_STATE2();

protected:
#ifdef _AFXDLL
	AFX_MODULE_STATE* m_pPrevModuleState;
	_AFX_THREAD_STATE* m_pThreadState;
#endif
};
#define AFX_MANAGE_STATE_NO_INIT_MANAGED(p) AFX_MAINTAIN_STATE2 _ctlState(p);
#define AFX_MANAGE_STATE(p) _AfxInitManaged(); AFX_MANAGE_STATE_NO_INIT_MANAGED(p)

/////////////////////////////////////////////////////////////////////////////
// Thread global state

// forward references required for _AFX_THREAD_STATE definition
class CView;
class CToolTipCtrl;
class CControlBar;
class CPushRoutingFrame;
class CPushRoutingView;

#define _AFX_TEMP_CLASS_NAME_SIZE 96
class _AFX_THREAD_STATE : public CNoTrackObject
{
public:
	_AFX_THREAD_STATE();
	virtual ~_AFX_THREAD_STATE();

	// override for m_pModuleState in _AFX_APP_STATE
	AFX_MODULE_STATE* m_pModuleState;
	AFX_MODULE_STATE* m_pPrevModuleState;

	// memory safety pool for temp maps
	void* m_pSafetyPoolBuffer;    // current buffer

	// thread local exception context
	AFX_EXCEPTION_CONTEXT m_exceptionContext;

	// CWnd create, gray dialog hook, and other hook data
	CWnd* m_pWndInit;
	CWnd* m_pAlternateWndInit;      // special case commdlg hooking
	DWORD m_dwPropStyle;
	DWORD m_dwPropExStyle;
	HWND m_hWndInit;
	HHOOK m_hHookOldCbtFilter;
	HHOOK m_hHookOldMsgFilter;

	// message pump for Run
	MSG m_msgCur;                   // current message
	CPoint m_ptCursorLast;          // last mouse position
	UINT m_nMsgLast;                // last mouse message

#ifdef _DEBUG
		int m_nDisablePumpCount; // Diagnostic trap to detect illegal re-entrancy
#endif

	// other CWnd modal data
	MSG m_lastSentMsg;              // see CWnd::WindowProc
	HWND m_hTrackingWindow;         // see CWnd::TrackPopupMenu
	HMENU m_hTrackingMenu;
	TCHAR m_szTempClassName[_AFX_TEMP_CLASS_NAME_SIZE];    // see AfxRegisterWndClass
	HWND m_hLockoutNotifyWindow;    // see CWnd::OnCommand
	BOOL m_bInMsgFilter;

	// other framework modal data
	CView* m_pRoutingView;          // see CCmdTarget::GetRoutingView
   CPushRoutingView* m_pPushRoutingView;
	CFrameWnd* m_pRoutingFrame;     // see CCmdTarget::GetRoutingFrame
   CPushRoutingFrame* m_pPushRoutingFrame;

	// MFC/DB thread-local data
	BOOL m_bWaitForDataSource;

	// OLE control thread-local data
	CWnd* m_pWndPark;       // "parking space" window
	long m_nCtrlRef;        // reference count on parking window
	BOOL m_bNeedTerm;       // TRUE if OleUninitialize needs to be called
};

EXTERN_THREAD_LOCAL(_AFX_THREAD_STATE, _afxThreadState)

_AFX_THREAD_STATE* AFXAPI AfxGetThreadState();

/////////////////////////////////////////////////////////////////////////////

void AFXAPI AfxSetPerUserRegistration(BOOL bEnable);

BOOL AFXAPI AfxGetPerUserRegistration(void);

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#undef AFX_DATA
#define AFX_DATA

#endif //__AFXSTATE_H__

/////////////////////////////////////////////////////////////////////////////
