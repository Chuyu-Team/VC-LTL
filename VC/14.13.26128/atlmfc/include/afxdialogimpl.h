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

class CMFCPopupMenu;

class CDialogImpl
{
	friend class CDialogEx;
	friend class CMFCPropertyPage;
	friend class CMFCPropertySheet;

protected:
	CDialogImpl(CWnd& dlg);
	virtual ~CDialogImpl();

	static LRESULT CALLBACK DialogMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

	void SetActiveMenu(CMFCPopupMenu* pMenu);

	BOOL ProcessMouseClick(POINT pt);
	BOOL ProcessMouseMove(POINT pt);

	BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNcActivate(BOOL& bActive);
	void OnActivate(UINT nState, CWnd* pWndOther);

	void OnDestroy();

	CWnd& m_Dlg;

	AFX_IMPORT_DATA static HHOOK           m_hookMouse;
	AFX_IMPORT_DATA static CDialogImpl* m_pMenuDlgImpl;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
