// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#include "afxcontrolbarutil.h"

#include "afxpropertysheet.h"
#include "afxpropertypage.h"
#include "afxbutton.h"
#include "afxribboncommandslistbox.h"
#include "afxribbonres.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonBar;
class CMFCRibbonCustomizeCategory;

/*============================================================================*/
// CMFCRibbonCustomizePropertyPage dialog

class CMFCRibbonCustomizePropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNCREATE(CMFCRibbonCustomizePropertyPage)

// Construction
public:
	CMFCRibbonCustomizePropertyPage(CMFCRibbonBar* pRibbonBar = NULL);
	~CMFCRibbonCustomizePropertyPage();

	void AddCustomCategory(LPCTSTR lpszName, const CList<UINT, UINT>& lstIDS);

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE8 };
	CButton m_wndAdd;
	CButton m_wndRemove;
	CComboBox m_wndCategoryCombo;
	CMFCButton m_wndUp;
	CMFCButton m_wndDown;
	CMFCRibbonCommandsListBox m_wndCommandsList;
	CMFCRibbonCommandsListBox m_wndQATList;
	int  m_nCategory;
	BOOL m_bQAToolbarOnBottom;

// Overrides
public:
	virtual void OnOK();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnSelendokCategoryCombo();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnToolbarReset();
	afx_msg void OnSelchangeQATCommands();
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomizeKeyboard();
	afx_msg void OnSelchangeCommandsList();

	DECLARE_MESSAGE_MAP()

	void MoveItem(BOOL bMoveUp);

	CMFCRibbonBar* m_pRibbonBar;
	CList<CMFCRibbonCustomizeCategory*,CMFCRibbonCustomizeCategory*> m_lstCustomCategories;
};

class CMFCRibbonCustomizeDialog : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CMFCRibbonCustomizeDialog)

public:
	CMFCRibbonCustomizeDialog(CWnd* pWndParent, CMFCRibbonBar* pRibbon);
	virtual ~CMFCRibbonCustomizeDialog();

protected:
	CMFCRibbonCustomizePropertyPage* m_pPage;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
