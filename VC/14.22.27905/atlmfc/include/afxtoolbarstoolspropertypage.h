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

#include "afxvslistbox.h"
#include "afxmenubutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarsCustomizeDialog;
class CMFCToolBarsToolsPropertyPage;
class CUserTool;

class CVSToolsListBox : public CVSListBox
{
public:
	CVSToolsListBox(CMFCToolBarsToolsPropertyPage* pParent) : m_pParent(pParent) {}

	virtual void OnSelectionChanged();
	virtual BOOL OnBeforeRemoveItem(int iItem);
	virtual void OnAfterAddItem(int iItem);
	virtual void OnAfterRenameItem(int iItem);
	virtual void OnAfterMoveItemUp(int iItem);
	virtual void OnAfterMoveItemDown(int iItem);

	CMFCToolBarsToolsPropertyPage* m_pParent;
};

/*============================================================================*/
// CMFCToolBarsToolsPropertyPage dialog

class CMFCToolBarsToolsPropertyPage : public CPropertyPage
{
	friend class CVSToolsListBox;

// Construction
public:
	CMFCToolBarsToolsPropertyPage();
	~CMFCToolBarsToolsPropertyPage();

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE7 };
	CMFCMenuButton m_wndInitialDirBtn;
	CMFCMenuButton m_wndArgumentsBtn;
	CEdit       m_wndArgumentsEdit;
	CEdit       m_wndInitialDirEdit;
	CEdit       m_wndCommandEdit;
	CButton     m_wndBrowseBtn;
	CVSToolsListBox  m_wndToolsList;
	CString     m_strCommand;
	CString     m_strArguments;
	CString     m_strInitialDirectory;

// Overrides
public:
	virtual void OnOK();
	virtual BOOL OnKillActive();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnBrowseCommand();
	afx_msg void OnUpdateTool();
	afx_msg void OnArgumentsOptions();
	afx_msg void OnInitialDirectoryOptions();

	DECLARE_MESSAGE_MAP()

	CUserTool* CreateNewTool();
	void EnableControls();

	CUserTool*         m_pSelTool;
	CMFCToolBarsCustomizeDialog* m_pParentSheet;

	CMenu m_menuArguments;
	CMenu m_menuInitialDir;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
