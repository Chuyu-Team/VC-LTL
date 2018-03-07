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

#include "afxframeimpl.h"
#include "afxcontrolbarutil.h"
#include "afxpopupmenu.h"

#include "afxdockingmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCMenuBar;
class CMFCToolBarMenuButton;
class CMFCToolBarButton;

/*============================================================================*/
// CFrameWndEx frame

class CFrameWndEx : public CFrameWnd
{
	friend class CMFCToolBar;
	friend class CMFCMenuBar;
	friend class CWinAppEx;
	friend class CMFCPopupMenu;
	friend class CPaneFrameWnd;

	DECLARE_DYNCREATE(CFrameWndEx)

protected:
	CFrameWndEx();           // protected constructor used by dynamic creation

// Attributes
public:
	BOOL IsMenuBarAvailable() const { return m_Impl.GetMenuBar() != NULL; }
	const CMFCMenuBar* GetMenuBar() const { return m_Impl.GetMenuBar(); }

	UINT GetDefaultResId() const { return m_Impl.m_nIDDefaultResource; }

	CMFCPopupMenu* GetActivePopup() const { return CMFCPopupMenu::m_pActivePopupMenu; }
	const CObList& GetTearOffBars() const { return m_Impl.m_listTearOffToolbars; }

	CMFCRibbonBar* GetRibbonBar() { return m_Impl.GetRibbonBar(); }

protected:
	BOOL  m_bIsTlbCustMode;
	BOOL m_bContextHelp;
	BOOL m_bWasMaximized;
	BOOL m_bIsMinimized;
	HMENU m_hmenuWindow;
	CFrameImpl m_Impl;
	CDockingManager  m_dockManager; // THE DockManager

	// workaround for Print Preview bug in VS 7.1
	CFrameWnd* m_pPrintPreviewFrame;

// Operations
public:
	//------------------ dockmanager layer ------------
	void AddDockSite();
	BOOL AddPane(CBasePane* pControlBar, BOOL bTail = TRUE);
	BOOL InsertPane(CBasePane* pControlBar, CBasePane* pTarget, BOOL bAfter = TRUE);
	void RemovePaneFromDockManager(CBasePane* pControlBar, BOOL bDestroy, BOOL bAdjustLayout, BOOL bAutoHide, CBasePane* pBarReplacement);
	void DockPane(CBasePane* pBar, UINT nDockBarID = 0, LPCRECT lpRect = NULL);

	CBasePane* PaneFromPoint(CPoint point, int nSensitivity, bool bExactBar, CRuntimeClass* pRTCBarType) const;
	CBasePane* PaneFromPoint(CPoint point, int nSensitivity, DWORD& dwAlignment, CRuntimeClass* pRTCBarType) const;
	BOOL IsPointNearDockSite(CPoint point, DWORD& dwBarAlignment, BOOL& bOuterEdge) const;
	virtual void AdjustDockingLayout(HDWP hdwp = NULL);
	BOOL EnableDocking(DWORD dwDockStyle);
	BOOL EnableAutoHidePanes(DWORD dwDockStyle);

	CBasePane* GetPane(UINT nID);
	void ShowPane(CBasePane* pBar, BOOL bShow, BOOL bDelay, BOOL bActivate);
	
	virtual BOOL OnMoveMiniFrame(CWnd* pFrame);
	virtual void RecalcLayout(BOOL bNotify = TRUE);

	BOOL DockPaneLeftOf(CPane* pBar, CPane* pLeftOf);

	void SetupToolbarMenu(CMenu& menu, const UINT uiViewUserToolbarCmdFirst, const UINT uiViewUserToolbarCmdLast)
	{
		m_Impl.SetupToolbarMenu(menu, uiViewUserToolbarCmdFirst, uiViewUserToolbarCmdLast);
	}

	void SetDockState(const CDockState& state) { m_Impl.SetDockState(state); }

	BOOL IsFullScreen() const { return m_Impl.m_FullScreenMgr.IsFullScreen(); }
	void ShowFullScreen() { m_Impl.m_FullScreenMgr.ShowFullScreen(); }
	void EnableFullScreenMainMenu(BOOL bEnableMenu) { m_Impl.m_FullScreenMgr.EnableMainMenu(bEnableMenu); }
	void EnableFullScreenMode(UINT uiFullScreenCmd) { m_Impl.m_FullScreenMgr.SetFullScreenID(uiFullScreenCmd); }

	void EnableLoadDockState(BOOL bEnable = TRUE) { m_Impl.m_bLoadDockState = bEnable; }

	CDockingManager* GetDockingManager() { return &m_dockManager; }

	void SetPrintPreviewFrame(CFrameWnd* pWnd)
	{
		if (pWnd != NULL)
		{
			ASSERT_VALID(pWnd);
		}

		m_pPrintPreviewFrame = pWnd;
	}

	void EnablePaneMenu(BOOL bEnable, UINT uiCustomizeCmd, const CString& strCustomizeLabel, UINT uiViewToolbarsMenuEntryID,
		BOOL bContextMenuShowsToolbarsOnly = FALSE, BOOL bViewMenuShowsToolbarsOnly = FALSE)
	{
		m_dockManager.EnablePaneContextMenu(bEnable, uiCustomizeCmd, strCustomizeLabel, bContextMenuShowsToolbarsOnly);
		m_Impl.SetControlbarsMenuId(uiViewToolbarsMenuEntryID, bViewMenuShowsToolbarsOnly);
	}

	UINT GetPaneMenuEntryID() const { return m_Impl.m_uiControlbarsMenuEntryID; }

	void UpdateCaption() { m_Impl.UpdateCaption(); }
	BOOL IsPrintPreview() { return m_Impl.IsPrintPreview(); }

protected:

// Implementation:
	void InitUserToolbars(LPCTSTR lpszRegEntry, UINT uiUserToolbarFirst, UINT uiUserToolbarLast)
	{
		m_Impl.InitUserToolbars(lpszRegEntry, uiUserToolbarFirst, uiUserToolbarLast);
	}

	void LoadUserToolbars() { m_Impl.LoadUserToolbars(); }
	void SaveUserToolbars() { m_Impl.SaveUserToolbars(); }
	void LoadTearOffMenus() { m_Impl.LoadTearOffMenus(); }
	void SaveTearOffMenus() { m_Impl.SaveTearOffMenus(); }

	BOOL IsDockStateValid(const CDockState& state) { return m_Impl.IsDockStateValid(state); }
	CMFCToolBar* GetUserToolBarByIndex(int iIndex) const { return m_Impl.GetUserToolBarByIndex(iIndex); }

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);

// Implementation
protected:
	virtual ~CFrameWndEx();

	BOOL OnSetMenu(HMENU hmenu);
	BOOL ShowPopupMenu(CMFCPopupMenu* pMenuPopup);

	virtual void AdjustClientArea();
	virtual COleClientItem*	GetInPlaceActiveItem();

public:
	virtual BOOL OnShowPopupMenu(CMFCPopupMenu* /*pMenuPopup*/);

	virtual BOOL OnShowCustomizePane(CMFCPopupMenu* pMenuPane, UINT uiToolbarID)
	{
		m_Impl.AddDefaultButtonsToCustomizePane(pMenuPane, uiToolbarID);
		return TRUE;
	}

	virtual void OnClosePopupMenu(CMFCPopupMenu* pMenuPopup);
	virtual BOOL OnDrawMenuImage(CDC* pDC, const CMFCToolBarMenuButton* pMenuButton, const CRect& rectImage);

	virtual BOOL OnMenuButtonToolHitTest(CMFCToolBarButton* /*pButton*/, TOOLINFO* /*pTI*/) { return FALSE; }
	virtual BOOL GetToolbarButtonToolTipText(CMFCToolBarButton* /*pButton*/, CString& /*strTTText*/) { return FALSE; }
	virtual void OnDrawMenuLogo(CDC* /*pDC*/, CMFCPopupMenu* /*pMenu*/, const CRect& /*rectLogo*/) {}
	virtual BOOL OnTearOffMenu(CMFCPopupMenu* /*pMenuPopup*/, CPane* /*pBar*/) { return TRUE; }

	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);

	virtual BOOL OnShowPanes(BOOL bShow);
	virtual BOOL NegotiateBorderSpace( UINT nBorderCmd, LPRECT lpRectBorder);

	virtual void DelayUpdateFrameMenu(HMENU hMenuAlt);
	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* /*pWnd*/) { return TRUE; }
	virtual BOOL OnCloseDockingPane(CDockablePane* /*pWnd*/) { return TRUE; }

	void ActiveItemRecalcLayout();
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg LRESULT OnSetMenu(WPARAM wp, LPARAM);
	afx_msg LRESULT OnToolbarCreateNew(WPARAM,LPARAM);
	afx_msg LRESULT OnToolbarDelete(WPARAM,LPARAM);
	afx_msg void OnContextHelp();
	afx_msg void OnUpdatePaneMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnPaneCheck(UINT nID);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnExitSizeMove();
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);
	afx_msg int OnSetText(LPCTSTR lpszText);
	afx_msg LRESULT OnChangeVisualManager(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostPreviewFrame(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCompositionChanged();
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM lEventData);
	afx_msg void OnSysColorChange();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
