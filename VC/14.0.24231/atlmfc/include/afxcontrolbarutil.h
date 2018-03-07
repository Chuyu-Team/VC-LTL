// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//

#pragma once

#ifndef __AFXCMN_H__
	#include <afxcmn.h> // MFC support for Windows Common Controls
#endif

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif

#include "afxusertoolsmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define _AFX_USING_CONTROL_BARS

inline BOOL IsStandardCommand(UINT uiCmd)
{
	return((uiCmd >= ID_FILE_MRU_FILE1 && uiCmd <= ID_FILE_MRU_FILE16) || // MRU commands,
		(uiCmd >= 0xF000 && uiCmd < 0xF1F0) ||                            // system commands,
		((int) uiCmd >= AFX_IDM_FIRST_MDICHILD) ||                        // windows commands
		(uiCmd >= ID_OLE_VERB_FIRST && uiCmd <= ID_OLE_VERB_LAST) ||      // OLE commands
		afxUserToolsManager != NULL && uiCmd == afxUserToolsManager->GetToolsEntryCmd());
}

void ControlBarCleanUp();

#ifdef _AFXDLL
void AfxGlobalsAddRef();
void AfxGlobalsRelease();
#endif

extern CFrameWnd* g_pTopLevelFrame;

inline void AFXSetTopLevelFrame(CFrameWnd* pFrame)
{
	g_pTopLevelFrame = pFrame;
}

inline CFrameWnd* AFXGetTopLevelFrame(const CWnd* pWnd)
{
	ASSERT_VALID(pWnd);
	return g_pTopLevelFrame == NULL ? pWnd->GetTopLevelFrame() : g_pTopLevelFrame;
}

CFrameWnd* AFX_CDECL AFXGetParentFrame(const CWnd* pWnd);

#define AFX_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define AFX_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

class CMemDC
{
public:
	AFX_IMPORT_DATA static BOOL m_bUseMemoryDC;

	CMemDC(CDC& dc, CWnd* pWnd);
	CMemDC(CDC& dc, const CRect& rect);

	virtual ~CMemDC();

	CDC& GetDC() { return m_bMemDC ? m_dcMem : m_dc; }
	BOOL IsMemDC() const { return m_bMemDC; }
	BOOL IsVistaDC() const { return m_hBufferedPaint != NULL; }

protected:
	CDC&     m_dc;
	BOOL     m_bMemDC;
	HANDLE   m_hBufferedPaint;
	CDC      m_dcMem;
	CBitmap  m_bmp;
	CBitmap* m_pOldBmp;
	CRect    m_rect;
};

// Windows Vista Notifications:
#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED        0x031E
#endif

#ifndef WM_DWMNCRENDERINGCHANGED
#define WM_DWMNCRENDERINGCHANGED        0x031F
#endif

#ifndef WM_DWMCOLORIZATIONCOLORCHANGED
#define WM_DWMCOLORIZATIONCOLORCHANGED  0x0320
#endif

#ifndef WM_DWMWINDOWMAXIMIZEDCHANGE
#define WM_DWMWINDOWMAXIMIZEDCHANGE     0x0321
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

// Smart docking theme:
enum AFX_SMARTDOCK_THEME
{
	AFX_SDT_DEFAULT = 0,	// Visual Manager-specific theme
	AFX_SDT_VS2005 = 1,		// VS 2005-style
	AFX_SDT_VS2008 = 2		// VS 2008-style
};


