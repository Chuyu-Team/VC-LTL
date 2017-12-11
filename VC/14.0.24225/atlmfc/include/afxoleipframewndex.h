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
class COleCntrFrameWndEx;

/*============================================================================*/
// COleIPFrameWndEx window

class COleIPFrameWndEx : public COleIPFrameWnd
{
	friend class CMFCMenuBar;
	friend class CMFCToolBar;
	friend class CWinAppEx;
	friend class CMFCPopupMenu;
	friend class CPaneFrameWnd;

	DECLARE_DYNCREATE(COleIPFrameWndEx)

// Construction
protected:
	COleIPFrameWndEx();           // protected constructor used by dynamic creation

// Attributes
public:
	BOOL IsMenuBarAvailable() const { return m_Impl.GetMenuBar() != NULL; }
	const CMFCMenuBar* GetMenuBar() const { return m_Impl.GetMenuBar(); }
	UINT GetDefaultResId() const { return m_Impl.m_nIDDefaultResource; }
	CMFCPopupMenu* GetActivePopup() const { return CMFCPopupMenu::m_pActivePopupMenu; }
	const CObList& GetTearOffBars() const { return m_Impl.m_listTearOffToolbars; }

	CFrameWnd* GetMainFrame() { return (CFrameWnd*) m_pMainFrame; }
	CFrameWnd* GetDockFrame() { return (CFrameWnd*) m_pDocFrame; }
	CDockingManager* GetDockingManager() { return &m_dockManager; }

	COleCntrFrameWndEx* GetContainerFrameWindow();

protected:
	BOOL  m_bIsTlbCustMode;
	BOOL  m_bContextHelp;
	HMENU m_hmenuWindow;
	HWND  m_hwndLastTopLevelFrame;
	CFrameImpl m_Impl;
	CDockingManager  m_dockManager;

// Operations
public:
	virtual BOOL OnShowPanes(BOOL bShow);

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
	//-------------------------------------------------
	
	BOOL DockPaneLeftOf(CPane* pBar, CPane* pLeftOf);
	void SetupToolbarMenu(CMenu& menu, const UINT uiViewUserToolbarCmdFirst, const UINT uiViewUserToolbarCmdLast)
	{
		m_Impl.SetupToolbarMenu(menu, uiViewUserToolbarCmdFirst, uiViewUserToolbarCmdLast);
	}

	void SetDockState(const CDockState& state) { m_Impl.SetDockState(state); }

	void EnablePaneMenu(BOOL bEnable, UINT uiCustomizeCmd, const CString& strCustomizeLabel, UINT uiViewToolbarsMenuEntryID,
		BOOL bContextMenuShowsToolbarsOnly = FALSE, BOOL bViewMenuShowsToolbarsOnly = FALSE)
	{
		m_dockManager.EnablePaneContextMenu(bEnable, uiCustomizeCmd, strCustomizeLabel, bContextMenuShowsToolbarsOnly);
		m_Impl.SetControlbarsMenuId(uiViewToolbarsMenuEntryID, bViewMenuShowsToolbarsOnly);
	}

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
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);

// Implementation
protected:
	virtual ~COleIPFrameWndEx();

	BOOL OnSetMenu(HMENU hmenu);
	BOOL ShowPopupMenu(CMFCPopupMenu* pMenuPopup);

public:
	virtual BOOL OnShowPopupMenu(CMFCPopupMenu* /*pMenuPopup*/) { return TRUE; }

	virtual BOOL OnShowCustomizePane(CMFCPopupMenu* pMenuPane, UINT uiToolbarID)
	{
		m_Impl.AddDefaultButtonsToCustomizePane(pMenuPane, uiToolbarID);
		return TRUE;
	}

	virtual void OnClosePopupMenu(CMFCPopupMenu* pMenuPopup);
	virtual BOOL OnDrawMenuImage(	CDC* /*pDC*/, const CMFCToolBarMenuButton* /*pMenuButton*/, const CRect& /*rectImage*/) { return FALSE; }
	virtual BOOL OnMenuButtonToolHitTest(CMFCToolBarButton* /*pButton*/, TOOLINFO* /*pTI*/) { return FALSE; }
	virtual BOOL GetToolbarButtonToolTipText(CMFCToolBarButton* /*pButton*/, CString& /*strTTText*/) { return FALSE; }

	virtual void OnDrawMenuLogo(CDC* /*pDC*/, CMFCPopupMenu* /*pMenu*/, const CRect& /*rectLogo*/){}

	virtual BOOL OnTearOffMenu(CMFCPopupMenu* /*pMenuPopup*/, CPane* /*pBar*/) { return TRUE; }
	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* /*pWnd*/) { return TRUE; }
	virtual BOOL OnCloseDockingPane(CDockablePane* /*pWnd*/) { return TRUE; }

protected:
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnSetMenu(WPARAM wp, LPARAM);
	afx_msg LRESULT OnToolbarCreateNew(WPARAM,LPARAM);
	afx_msg LRESULT OnToolbarDelete(WPARAM,LPARAM);
	afx_msg void OnContextHelp();
	afx_msg void OnUpdatePaneMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnPaneCheck(UINT nID);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
