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
#include "afxdialogex.h"
#include "afxribbonres.h"
#include "afxbutton.h"
#include "afxlistctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarsCustomizeDialog;
class CMFCToolBarButton;

/*============================================================================*/
// CMFCKeyMapDialog dialog

class CMFCKeyMapDialog : public CDialogEx
{
// Construction
public:
	CMFCKeyMapDialog(CFrameWnd* pWndParentFrame, BOOL bEnablePrint = FALSE);   // standard constructor
	~CMFCKeyMapDialog();

// Dialog Data
	enum { IDD = IDD_AFXBARRES_KEYMAP };
	CStatic         m_wndAccelLabel;
	CMFCListCtrl m_KeymapList;
	CComboBox       m_wndCategoryList;
	CStatic         m_wndViewIcon;
	CComboBox       m_wndViewTypeList;
	CMFCButton   m_ButtonPrint;
	CMFCButton   m_ButtonCopy;

	CFrameWnd*      m_pWndParentFrame;
	HACCEL          m_hAccelTable;
	LPACCEL         m_lpAccel;
	int             m_nAccelSize;
	CMFCToolBarsCustomizeDialog* m_pDlgCust;
	int             m_nSortedCol;
	BOOL            m_bSortAscending;
	const BOOL      m_bEnablePrint;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
	virtual void CopyKeyMap();
	virtual void PrintKeyMap();

	virtual int OnPrintHeader(CDC& dc, int nPage, int cx) const;
	virtual int OnPrintItem(CDC& dc, int nItem, int y, int cx, BOOL bCalcHeight) const;

	virtual CString FormatItem(int nItem) const;

	virtual void OnSetColumns();
	virtual void OnInsertItem(CMFCToolBarButton* pButton, int nItem);
	virtual void SetColumnsWidth();

	virtual CString GetCommandKeys(UINT uiCmdID) const;

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeViewType();
	afx_msg void OnSelchangeCategory();
	afx_msg void OnCopy();
	afx_msg void OnPrint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
