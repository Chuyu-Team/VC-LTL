// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CKeyboardManager : public CObject
{
// Constructor/Destructor:
public:
	CKeyboardManager();
	virtual ~CKeyboardManager();

// Operattion:
public:
	BOOL LoadState(LPCTSTR lpszProfileName = NULL, CFrameWnd* pDefaultFrame = NULL);
	BOOL SaveState(LPCTSTR lpszProfileName = NULL, CFrameWnd* pDefaultFrame = NULL);

	BOOL UpdateAccelTable(CMultiDocTemplate* pTemplate, LPACCEL lpAccel, int nSize, CFrameWnd* pDefaultFrame = NULL);
	BOOL UpdateAccelTable(CMultiDocTemplate* pTemplate, HACCEL hAccelNew, CFrameWnd* pDefaultFrame = NULL);

	void ResetAll();

	static BOOL __stdcall FindDefaultAccelerator(UINT uiCmd, CString& str, CFrameWnd* pWndFrame, BOOL bIsDefaultFrame);
	static BOOL __stdcall IsKeyPrintable(const UINT nChar);
	static UINT __stdcall TranslateCharToUpper(const UINT nChar);
	static BOOL __stdcall IsKeyHandled(WORD nKey, BYTE fVirt, CFrameWnd* pWndFrame, BOOL bIsDefaultFrame);

	static void __stdcall ShowAllAccelerators(BOOL bShowAll = TRUE);
	static BOOL __stdcall IsShowAllAccelerators() { return m_bAllAccelerators; }

	static void __stdcall CleanUp();

protected:
	BOOL SaveAcceleratorState(LPCTSTR lpszProfileName, UINT uiResId, HACCEL hAccelTable);
	BOOL LoadAcceleratorState(LPCTSTR lpszProfileName, UINT uiResId, HACCEL& hAccelTable);
	
	static void __stdcall SetAccelTable(LPACCEL& lpAccel, HACCEL& hAccelLast, int& nSize, const HACCEL hAccelCur);

	AFX_IMPORT_DATA static LPACCEL m_lpAccelDefault;    // Main frame acceleration table 
	AFX_IMPORT_DATA static LPACCEL m_lpAccel;           // Child frame acceleration table
	AFX_IMPORT_DATA static int     m_nAccelDefaultSize;
	AFX_IMPORT_DATA static int     m_nAccelSize;
	AFX_IMPORT_DATA static HACCEL  m_hAccelDefaultLast;
	AFX_IMPORT_DATA static HACCEL  m_hAccelLast;
	AFX_IMPORT_DATA static BOOL    m_bAllAccelerators;
};

AFX_IMPORT_DATA extern CKeyboardManager* afxKeyboardManager;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
