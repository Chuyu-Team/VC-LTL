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
#include "afxribbonres.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMDIFrameWndEx;

extern AFX_IMPORT_DATA UINT AFX_WM_WINDOW_HELP;

/*============================================================================*/
// CMFCWindowsManagerDialog dialog

class CMFCWindowsManagerDialog : public CDialog
{
// Construction
public:
	CMFCWindowsManagerDialog(CMDIFrameWndEx* pMDIFrame, BOOL bHelpButton = FALSE);   // standard constructor

// Dialog Data
	enum { IDD = IDD_AFXBARRES_WINDOWS_DLG };
	CListBox m_wndList;

// Overrides
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	void MDIMessage(UINT uMsg,WPARAM flag);
	void FillWindowList(void);
	void SelActive(void);
	void UpdateButtons(void);

	CMDIFrameWndEx* m_pMDIFrame;
	const BOOL          m_bHelpButton;
	BOOL                m_bMDIActions;
	CList<HWND,HWND>    m_lstCloseDisabled;
	CList<HWND,HWND>    m_lstSaveDisabled;

	afx_msg void OnActivate();
	afx_msg void OnSave();
	afx_msg void OnClose();
	afx_msg void OnCascade();
	afx_msg void OnTilehorz();
	afx_msg void OnTilevert();
	afx_msg void OnMinimize();
	afx_msg void OnSelchangeList();
	afx_msg void OnWindowHelp();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
