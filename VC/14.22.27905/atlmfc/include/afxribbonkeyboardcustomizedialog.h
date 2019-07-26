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
#include "afxdialogex.h"
#include "afxribbonres.h"
#include "afxacceleratorkeyassignctrl.h"
#include "afxribboncommandslistbox.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonBaseElement;
class CMFCRibbonBar;

/*============================================================================*/
// CMFCRibbonKeyboardCustomizeDialog dialog

class CMFCRibbonKeyboardCustomizeDialog : public CDialogEx
{
// Construction
public:
	CMFCRibbonKeyboardCustomizeDialog(CMFCRibbonBar* pRibbonBar, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMFCRibbonKeyboardCustomizeDialog();

// Dialog Data
	enum { IDD = IDD_AFXBARRES_RIBBON_KEYBOARD_DLG };
	CString    m_strDescription;
	CString    m_strAssignedTo;
	CStatic    m_wndAssignedToTitle;
	CStatic    m_wndViewIcon;
	CButton    m_wndRemoveButton;
	CButton    m_wndAssignButton;
	CListBox   m_wndCurrentKeysList;
	CListBox   m_wndCategoryList;
	CComboBox  m_wndViewTypeList;
	CMFCAcceleratorKeyAssignCtrl m_wndNewKey;
	CMFCRibbonCommandsListBox m_wndCommandsList;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnAssign();
	afx_msg void OnSelchangeCategory();
	afx_msg void OnSelchangeCommandsList();
	afx_msg void OnSelchangeCurrentKeysList();
	afx_msg void OnRemove();
	afx_msg void OnResetAll();
	afx_msg void OnSelchangeViewType();
	afx_msg void OnUpdateNewShortcutKey();

	DECLARE_MESSAGE_MAP()

// Operations:
protected:
	void AddKeyEntry(LPACCEL pEntry);

// Attributes:
protected:
	HACCEL  m_hAccelTable;
	LPACCEL m_lpAccel;
	LPACCEL m_pSelEntry;
	int     m_nAccelSize;
	BOOL    m_bAutoSet;

	CFrameWnd*          m_pParentFrame;
	CMFCRibbonBar*         m_pRibbonBar;
	CMultiDocTemplate*  m_pSelTemplate;
	CMFCRibbonBaseElement* m_pSelButton;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
