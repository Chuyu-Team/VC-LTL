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
#include "afxpopupmenu.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonComboBox;

/*============================================================================*/
// CMFCDropDownListBox window

class CMFCDropDownListBox : public CMFCPopupMenu
{
	DECLARE_DYNAMIC(CMFCDropDownListBox)

// Construction
public:
	CMFCDropDownListBox();
	CMFCDropDownListBox(CWnd* pEditCtrl);
	CMFCDropDownListBox(CMFCRibbonComboBox* pRibbonCombo);

protected:
	void CommonInit();

// Attributes
public:
	void SetMaxHeight(int nMaxHeight) { m_nMaxHeight = nMaxHeight; }
	void SetMinWidth(int nMinWidth) { m_nMinWidth = nMinWidth; }

	int GetCount() const;

	int GetCurSel();
	int SetCurSel(int nSelect);

	// for listbox items
	void GetText(int nIndex, CString& rString) const;

	BOOL IsEditFocused() const { return m_pEditCtrl->GetSafeHwnd() == ::GetFocus(); }

// Operations
public:
	// manipulating listbox items
	void AddString(LPCTSTR lpszItem);
	void ResetContent();

	void Track(CPoint point, CWnd *pWndOwner = NULL);

// Overrides
	virtual void OnDrawItem(CDC* pDC, CMFCToolBarMenuButton* pItem, BOOL bHighlight);
	virtual CSize OnGetItemSize(CDC* pDC, CMFCToolBarMenuButton* pItem, CSize sizeDefault);
	virtual void OnChooseItem(UINT uidCmdID);
	virtual void OnChangeHot(int nHot);
	virtual BOOL DefaultMouseClickOnClose() const { return m_pRibbonCombo == NULL; }

// Implementation
public:
	virtual ~CMFCDropDownListBox();

protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	void HighlightItem(int nIndex);
	CMFCToolBarButton* GetItem(int nIndex) const;

	CMenu m_Menu;
	int   m_nCurSel;
	int   m_nMinWidth;
	CWnd* m_pEditCtrl;
	CMFCRibbonComboBox* m_pRibbonCombo;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
