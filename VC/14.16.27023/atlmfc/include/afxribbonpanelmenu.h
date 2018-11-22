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
#include "afxpopupmenubar.h"
#include "afxribbonpanel.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonButton;
class CMFCRibbonGallery;
class CMFCRibbonCategory;

/*============================================================================*/
// CMFCRibbonPanelMenuBar window

class CMFCRibbonPanelMenuBar : public CMFCPopupMenuBar
{
	DECLARE_DYNAMIC(CMFCRibbonPanelMenuBar)

	friend class CMFCRibbonBaseElement;
	friend class CMFCRibbonPanelMenu;
	friend class CMFCRibbonMiniToolBar;
	friend class CMFCRibbonQuickAccessCustomizeButton;

// Construction
public:
	CMFCRibbonPanelMenuBar(CMFCRibbonPanel* pPanel);
	CMFCRibbonPanelMenuBar(CMFCRibbonBar* pRibbonBar, const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons, BOOL bIsFloatyMode = FALSE);
	CMFCRibbonPanelMenuBar(CMFCRibbonGallery* pPaletteButton);
	CMFCRibbonPanelMenuBar(CMFCRibbonCategory* pCategory, CSize size);

protected:
	CMFCRibbonPanelMenuBar();
	void CommonInit();

// Attributes
public:
	CMFCRibbonBar* GetTopLevelRibbonBar() const;
	CMFCRibbonPanel* GetPanel() { return m_pPanel; }
	CMFCRibbonCategory* GetCategory() { return m_pCategory; }

	void SetMenuMode(BOOL bIsMenuMode = TRUE) { m_bIsMenuMode = bIsMenuMode; }
	BOOL IsMenuMode() const { return m_bIsMenuMode; }

	void SetDefaultMenuLook(BOOL bIsDefaultMenuLook = TRUE) { m_bIsDefaultMenuLook = bIsDefaultMenuLook; }
	BOOL IsDefaultMenuLook() const { return m_bIsDefaultMenuLook; }

	BOOL IsMainPanel() const { return m_pPanel != NULL && m_pPanel->IsMainPanel(); }
	BOOL IsQATPopup() const { return m_bIsQATPopup; }
	BOOL IsRibbonMiniToolBar() const { return m_bIsFloaty; }
	BOOL IsCategoryPopup() const { return m_pCategory != NULL; }

	CMFCRibbonButton* GetDroppedDown() const;
	CMFCRibbonBaseElement* HitTest(CPoint point) const;
	int HitTestEx(CPoint point) const;
	/// <summary>
	/// Returns a focused element. </summary>
	/// <returns> A pointer to a focused element or NULL.</returns>
	CMFCRibbonBaseElement* GetFocused() const;

protected:
	BOOL m_bTracked;
	BOOL m_bSimpleMode;
	BOOL m_bIsMenuMode;
	BOOL m_bIsDefaultMenuLook;
	BOOL m_bIsQATPopup;
	BOOL m_bCustomizeMenu;
	BOOL m_bIsFloaty;
	BOOL m_bHasKeyTips;
	BOOL m_bSetKeyTips;
	BOOL m_bAutoCommandTimer;

	CSize  m_sizePrefered;
	CSize  m_sizeCategory;
	CRect  m_rectAutoCommand;
	CPoint m_ptStartMenu;

	CScrollBar          m_wndScrollBarVert;
	CMFCRibbonBar*         m_pRibbonBar;
	CMFCRibbonPanel*       m_pPanel;
	CMFCRibbonPanel*       m_pPanelOrigin;
	CMFCRibbonButton*      m_pDelayedCloseButton;
	CMFCRibbonButton*      m_pDelayedButton;
	CMFCRibbonCategory*    m_pCategory;
	CMFCRibbonBaseElement* m_pPressed;

// Operations
public:
	void PopTooltip();
	void SetActive(BOOL bIsActive);

	CMFCRibbonBaseElement* FindByOrigin(CMFCRibbonBaseElement* pOrigin) const;

// Overrides
	virtual void OnClickButton(CMFCRibbonButton* pButton, CPoint point);
	virtual void OnChangeHighlighted(CMFCRibbonBaseElement* pHighlighted);

	virtual void AdjustLocations();
	virtual CSize CalcSize(BOOL bVertDock);
	virtual void DoPaint(CDC* pDC);

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void CloseDelayedSubMenu();
	virtual void RestoreDelayedSubMenu();

	virtual BOOL OnKey(UINT nChar);
	virtual BOOL OnSetAccData(long lVal);

	virtual BOOL IsRibbonPanel() const { return TRUE; }
	virtual BOOL IsRibbonPanelInRegularMode() const { return !m_bIsMenuMode && !IsMainPanel(); }

protected:
	virtual void OnDrawMenuBorder(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCRibbonPanelMenuBar();

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	void AddButtons(CMFCRibbonBar* pRibbonBar, const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>&	arButtons, BOOL bFloatyMode);
	void SetPreferedSize(CSize size);
};

/*============================================================================*/
// CMFCRibbonPanelMenu window

class CMFCRibbonPanelMenu : public CMFCPopupMenu
{
	friend class CMFCRibbonPanelMenuBar;
	friend class CMFCRibbonPanel;

	DECLARE_DYNAMIC(CMFCRibbonPanelMenu)

// Construction
public:
	CMFCRibbonPanelMenu(CMFCRibbonPanel* pPanel);
	CMFCRibbonPanelMenu(CMFCRibbonBar* pRibbonBar, const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons, BOOL bIsFloatyMode = FALSE);
	CMFCRibbonPanelMenu(CMFCRibbonGallery* pPaletteButton);
	CMFCRibbonPanelMenu(CMFCRibbonCategory* pCategory, CSize size);

protected:
	CMFCRibbonPanelMenu();

// Attributes
public:
	void SetMenuMode(BOOL bIsMenuMode = TRUE) { m_wndRibbonBar.SetMenuMode(bIsMenuMode); }
	BOOL IsMenuMode() const { return m_wndRibbonBar.IsMenuMode(); }

	void SetDefaultMenuLook(BOOL bIsDefaultMenuLook = TRUE) { m_wndRibbonBar.SetDefaultMenuLook(bIsDefaultMenuLook); }
	BOOL IsDefaultMenuLook() const { return m_wndRibbonBar.IsDefaultMenuLook(); }

	CMFCRibbonPanel* GetPanel() const { return m_wndRibbonBar.m_pPanel; }
	CMFCRibbonCategory* GetCategory() const { return m_wndRibbonBar.m_pCategory; }
	void SetPreferedSize(CSize size) { m_wndRibbonBar.SetPreferedSize(size); }

	CMFCRibbonPanelMenuBar* GetParentRibbonMenuBar() const;

	void EnableCustomizeMenu(BOOL bEnable = TRUE) { m_wndRibbonBar.m_bCustomizeMenu = bEnable; }
	BOOL IsCustomizeMenu() const { return m_wndRibbonBar.m_bCustomizeMenu; }

protected:
	CMFCRibbonPanelMenuBar	m_wndRibbonBar;
	BOOL				m_bForceClose;

// Operations
public:
	virtual void SetActive(BOOL /*bIsActive*/) {}

	virtual CMFCPopupMenuBar* GetMenuBar() { return &m_wndRibbonBar; }

	virtual BOOL IsAlwaysClose() const;

	CMFCRibbonBaseElement* FindByOrigin(CMFCRibbonBaseElement* pOrigin) const { return m_wndRibbonBar.FindByOrigin(pOrigin); }

	virtual BOOL IsScrollUpAvailable();
	virtual BOOL IsScrollDnAvailable();

	virtual BOOL DefaultMouseClickOnClose() const { return m_wndRibbonBar.IsRibbonMiniToolBar() && !m_wndRibbonBar.IsQATPopup(); }

protected:
	virtual void DoPaint(CDC* pPaintDC);
	virtual int GetBorderSize() const;

// Implementation
public:
	virtual ~CMFCRibbonPanelMenu();

protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
