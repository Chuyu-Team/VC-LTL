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

#include "afxribbonres.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCMousePropertyPage dialog

class CMFCMousePropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCMousePropertyPage)

// Construction
public:
	CMFCMousePropertyPage();
	~CMFCMousePropertyPage();

// Operations:
protected:
	void EnableDblClkControls(BOOL bEnable = TRUE);
	BOOL SelectCommand(UINT uiCmd);

// Attributes:
protected:
	int        m_iCurrViewId;
	CImageList m_ViewsImages;

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE4 };
	CListCtrl m_wndListOfViews;
	CListBox  m_wndListOfCommands;
	CStatic   m_wndCommandDescription;
	CString   m_strCommandDescription;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnNoDblcliick();
	afx_msg void OnUseDblcliick();
	afx_msg void OnSelchangeListOfCommands();
	afx_msg void OnItemchangedListViews(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
