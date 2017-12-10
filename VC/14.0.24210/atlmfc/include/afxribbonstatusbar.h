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
#include "afxribbonbar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonBaseElement;

/*============================================================================*/
// CMFCRibbonStatusBar window

class CMFCRibbonStatusBar : public CMFCRibbonBar
{
	friend class CFrameImpl;

	DECLARE_DYNAMIC(CMFCRibbonStatusBar)

// Construction
public:
	CMFCRibbonStatusBar();

	BOOL Create(CWnd* pParentWnd,DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT nID = AFX_IDW_STATUS_BAR);
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT nID = AFX_IDW_STATUS_BAR);

// Attributes
public:
	virtual BOOL GetExtendedArea(CRect& rect) const;

	// Returns a free space for dynamic panes
	int GetSpace() const { return m_cxFree; }

	BOOL IsBottomFrame() const { return m_bBottomFrame; }

protected:
	int   m_cxSizeBox;
	int   m_cxFree;
	CRect m_rectSizeBox;
	CRect m_rectResizeBottom;
	CRect m_rectInfo;
	BOOL  m_bBottomFrame;

	CString      m_strInfo;
	CStringArray m_arElementLabels;
	CStringArray m_arExElementLabels;

	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arElements;
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arExElements;
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arCustomizeItems;
	CList<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>  m_lstDynElements;

// Operations
public:
	void AddElement(CMFCRibbonBaseElement* pElement, LPCTSTR lpszLabel, BOOL bIsVisible = TRUE);
	void AddExtendedElement(CMFCRibbonBaseElement* pElement, LPCTSTR lpszLabel, BOOL bIsVisible = TRUE);
	void AddSeparator();

	void SetInformation(LPCTSTR lpszInfo); // lpszInfo == NULL - remove information
	BOOL IsInformationMode() const { return !m_strInfo.IsEmpty(); }

	void AddDynamicElement(CMFCRibbonBaseElement* pElement);

	BOOL RemoveElement(UINT uiID);
	void RemoveAll();

	int GetCount() const;
	int GetExCount() const;

	CMFCRibbonBaseElement* GetElement(int nIndex);
	CMFCRibbonBaseElement* GetExElement(int nIndex);

	CMFCRibbonBaseElement* FindByID(UINT uiCmdID, BOOL /*bVisibleOnly*/ = TRUE) { return FindElement(uiCmdID); }
	CMFCRibbonBaseElement* FindElement(UINT uiID);

	BOOL IsExtendedElement(CMFCRibbonBaseElement* pElement) const;

// Overrides
public:
	virtual void RecalcLayout();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual BOOL IsMainRibbonBar() const { return FALSE; }
	virtual BOOL IsShowGroupBorder(CMFCRibbonButtonsGroup* /*pGroup*/) const { return TRUE; }

	virtual CMFCRibbonBaseElement* HitTest(CPoint point, BOOL bCheckActiveCategory = FALSE, BOOL bCheckPanelCaption = FALSE);
	virtual CMFCRibbonBaseElement* GetDroppedDown();

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void OnPaneContextMenu(CWnd* pParentFrame, CPoint point);

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual void OnRTLChanged(BOOL bIsRTL);
	virtual void OnDrawInformation(CDC* pDC, CString& strInfo, CRect rectInfo);

	virtual HRESULT get_accChildCount(long *pcountChildren);

// Implementation
public:
	virtual ~CMFCRibbonStatusBar();

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnUpdateShadows(WPARAM,LPARAM);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()

	void CleanUpCustomizeItems();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
