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
#include "afxframeimpl.h"
#include "afxmdiclientareawnd.h"
#include "afxpopupmenu.h"
#include "afxdockingmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class COleClientItem;
class CMFCPopupMenu;
class CMFCToolBar;
class CMFCToolBarMenuButton;
class CMFCToolBarButton;

static const DWORD AFX_MDI_CREATE_VERT_GROUP = 0x001;
static const DWORD AFX_MDI_CREATE_HORZ_GROUP = 0x002;
static const DWORD AFX_MDI_CAN_MOVE_PREV     = 0x004;
static const DWORD AFX_MDI_CAN_MOVE_NEXT     = 0x008;
static const DWORD AFX_MDI_CAN_BE_DOCKED     = 0x010;

/*============================================================================*/
// CMDIFrameWndEx frame

class CMDIFrameWndEx : public CMDIFrameWnd
{
	friend class CMDIClientAreaWnd;
	friend class CMFCToolBar;
	friend class CMFCMenuBar;
	friend class CWinAppEx;
	friend class CMFCPopupMenu;
	friend class CMFCToolBarsCustomizeDialog;
	friend class CPaneFrameWnd;
	friend class CMDIChildWndEx;
	friend class CGlobalUtils;
	friend class CMFCTabCtrl;
	friend class CMFCRibbonButton;
	friend class CFullScreenImpl;

	DECLARE_DYNCREATE(CMDIFrameWndEx)

protected:
	CMDIFrameWndEx(); // protected constructor used by dynamic creation

// Attributes
public:
	BOOL IsMenuBarAvailable() const { return m_Impl.GetMenuBar() != NULL; }
	const CMFCMenuBar* GetMenuBar() const { return m_Impl.GetMenuBar(); }

	UINT GetDefaultResId() const { return m_Impl.m_nIDDefaultResource; }
	CMFCPopupMenu*	GetActivePopup() const { return CMFCPopupMenu::m_pActivePopupMenu; }
	const CObList& GetTearOffBars() const { return m_Impl.m_listTearOffToolbars; }

	void SetPrintPreviewFrame(CFrameWnd* pWnd)
	{
		if (pWnd != NULL)
		{
			ASSERT_VALID(pWnd);
		}
		m_pPrintPreviewFrame = pWnd;
	}

	BOOL IsPrintPreview() { return m_Impl.IsPrintPreview(); }
	CMFCRibbonBar* GetRibbonBar() { return m_Impl.GetRibbonBar(); }
	void ActiveItemRecalcLayout();

	AFX_IMPORT_DATA static BOOL m_bDisableSetRedraw;

protected:
	CString m_strWindowsDlgMenuText;
	HMENU   m_hmenuWindow;

	UINT m_uiWindowsDlgMenuId;
	BOOL m_bIsTlbCustMode;
	BOOL m_bContextHelp;
	BOOL m_bDoSubclass;
	BOOL m_bShowWindowsDlgAlways;
	BOOL m_bShowWindowsDlgHelpButton;
	BOOL m_bCanConvertControlBarToMDIChild;
	BOOL m_bWasMaximized;
	BOOL m_bIsMinimized;
	BOOL m_bClosing;
	int  m_nFrameID; // need for uniqueness when saving state in MTI 

	// workaround for Pront Preview bug in VS 7.1
	CFrameWnd*         m_pPrintPreviewFrame;
	CDockingManager       m_dockManager; // THE DockManager
	CFrameImpl      m_Impl;
	CMDIClientAreaWnd m_wndClientArea;

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
	
	//-------------------- MDI Tab Groups interface ---
	DWORD GetMDITabsContextMenuAllowedItems() { return m_wndClientArea.GetMDITabsContextMenuAllowedItems(); }
	BOOL IsMemberOfMDITabGroup(CWnd* pWnd) { return m_wndClientArea.IsMemberOfMDITabGroup(pWnd); }

	void MDITabMoveToNextGroup(BOOL bNext = TRUE) { m_wndClientArea.MDITabMoveToNextGroup(bNext); }
	void MDITabNewGroup(BOOL bVert = TRUE) { m_wndClientArea.MDITabNewGroup(bVert); }
	const CObList& GetMDITabGroups() const { return m_wndClientArea.GetMDITabGroups(); }
	
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bTabDrop);
	//-------------------------------------------------

	BOOL DockPaneLeftOf(CPane* pBar, CPane* pLeftOf);

	void SetupToolbarMenu(CMenu& menu, const UINT uiViewUserToolbarCmdFirst, const UINT uiViewUserToolbarCmdLast)
	{
		m_Impl.SetupToolbarMenu(menu, uiViewUserToolbarCmdFirst, uiViewUserToolbarCmdLast);
	}

	// Enable built-in CMFCWindowsManagerDialog dialog. When bShowAlways is TRUE, show windows dialog everytime;
	// otherwise only instead of the standard "Windows..." dialog (appears if more than 9 windows are open).
	void EnableWindowsDialog(UINT uiMenuId, LPCTSTR lpszMenuText, BOOL bShowAlways = FALSE, BOOL bShowHelpButton = FALSE);
	void EnableWindowsDialog(UINT uiMenuId, UINT uiMenuTextResId, BOOL bShowAlways = FALSE, BOOL bShowHelpButton = FALSE);
	void ShowWindowsDialog();
	
	// Enable/Disable MDI child tabs:
	void EnableMDITabs(BOOL bEnable = TRUE, BOOL bIcons = TRUE, CMFCTabCtrl::Location tabLocation = CMFCTabCtrl::LOCATION_BOTTOM, BOOL bTabCloseButton = FALSE,
		CMFCTabCtrl::Style style = CMFCTabCtrl::STYLE_3D_SCROLLED, BOOL bTabCustomTooltips = FALSE, BOOL bActiveTabCloseButton = FALSE);
	void EnableMDITabbedGroups(BOOL bEnable, const CMDITabInfo& params);

	BOOL AreMDITabs(int* pnMDITabsType = NULL) const
	{
		if (pnMDITabsType != NULL)
		{
			*pnMDITabsType = 0;
			if (m_wndClientArea.m_bTabIsEnabled)
			{
				*pnMDITabsType = 1;
			}
			else if (m_wndClientArea.IsMDITabbedGroup())
			{
				*pnMDITabsType = 2;
			}
		}
		return m_wndClientArea.m_bTabIsEnabled || m_wndClientArea.IsMDITabbedGroup();
	}

	BOOL IsMDITabbedGroup() const { return m_wndClientArea.IsMDITabbedGroup(); }
	CMFCTabCtrl& GetMDITabs() { return m_wndClientArea.GetMDITabs(); }

	BOOL IsFullScreen() const { return m_Impl.m_FullScreenMgr.IsFullScreen(); }
	void ShowFullScreen() { m_Impl.m_FullScreenMgr.ShowFullScreen(); }
	void EnableFullScreenMainMenu(BOOL bEnableMenu) { m_Impl.m_FullScreenMgr.EnableMainMenu(bEnableMenu); }
	void EnableFullScreenMode(UINT uiFullScreenCmd) { m_Impl.m_FullScreenMgr.SetFullScreenID(uiFullScreenCmd); }

	void EnableLoadDockState(BOOL bEnable = TRUE) { m_Impl.m_bLoadDockState = bEnable; }

	void EnableMDITabsLastActiveActivation(BOOL bLastActiveTab = TRUE)
	{
		m_wndClientArea.EnableMDITabsLastActiveActivation(bLastActiveTab);
	}

	void EnablePaneMenu(BOOL bEnable, UINT uiCustomizeCmd, const CString& strCustomizeLabel, UINT uiViewToolbarsMenuEntryID,
		BOOL bContextMenuShowsToolbarsOnly = FALSE, BOOL bViewMenuShowsToolbarsOnly = FALSE)
	{
		m_dockManager.EnablePaneContextMenu(bEnable, uiCustomizeCmd, strCustomizeLabel, bContextMenuShowsToolbarsOnly);
		m_Impl.SetControlbarsMenuId(uiViewToolbarsMenuEntryID, bViewMenuShowsToolbarsOnly);
	}

	UINT GetPaneMenuEntryID() const { return m_Impl.m_uiControlbarsMenuEntryID; }

	void UpdateCaption() { m_Impl.UpdateCaption(); }

	virtual CMDIChildWndEx* ControlBarToTabbedDocument(CDockablePane* pBar);
	virtual BOOL TabbedDocumentToControlBar(CMDIChildWndEx* pMDIChildWnd);

	void RegisterAllMDIChildrenWithTaskbar(BOOL bRegister = TRUE);
	void UpdateMDITabbedBarsIcons();
	int GetRegisteredWithTaskBarMDIChildCount();

	virtual BOOL CanConvertControlBarToMDIChild() { return m_bCanConvertControlBarToMDIChild; }

protected:
	virtual BOOL OnSetMenu(HMENU hmenu);

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
	void SetDockState(const CDockState& state) { m_Impl.SetDockState(state); }
	CDockingManager* GetDockingManager() { return &m_dockManager; }

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual HMENU GetWindowMenuPopup(HMENU hMenuBar);
	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);

	BOOL ShowPopupMenu(CMFCPopupMenu* pMenuPopup);

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
	virtual BOOL OnEraseMDIClientBackground(CDC* /*pDC*/) { return FALSE; }

	virtual void OnSizeMDIClient(const CRect& /*rectOld*/, const CRect& /*rectNew*/) {}
	virtual void OnDrawMenuLogo(CDC* /*pDC*/, CMFCPopupMenu* /*pMenu*/, const CRect& /*rectLogo*/) {}
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);

	virtual BOOL OnTearOffMenu(CMFCPopupMenu* /*pMenuPopup*/, CPane* /*pBar*/) { return TRUE; }

	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	virtual BOOL OnShowPanes(BOOL bShow);
	virtual BOOL NegotiateBorderSpace( UINT nBorderCmd, LPRECT lpRectBorder );

	virtual void AdjustClientArea();
	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* /*pWnd*/) { return TRUE; }
	virtual BOOL OnCloseDockingPane(CDockablePane* /*pWnd*/) { return TRUE; }

	virtual BOOL LoadMDIState(LPCTSTR lpszProfileName);
	virtual BOOL SaveMDIState(LPCTSTR lpszProfileName);

	virtual CMDIChildWndEx* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* pObj);
	virtual CMDIChildWndEx* CreateNewWindow(LPCTSTR lpcszDocName, CObject* pObj);

// Implementation
protected:
	virtual ~CMDIFrameWndEx();
	virtual COleClientItem* GetInPlaceActiveItem();

	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg LRESULT OnToolbarCreateNew(WPARAM,LPARAM);
	afx_msg LRESULT OnToolbarDelete(WPARAM,LPARAM);
	afx_msg void OnContextHelp();
	afx_msg void OnUpdatePaneMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnPaneCheck(UINT nID);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg void OnExitSizeMove();
	afx_msg void OnWindowNew();
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);
	afx_msg int OnSetText(LPCTSTR lpszText);
	afx_msg LRESULT OnChangeVisualManager(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPostPreviewFrame(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCompositionChanged();
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM lEventData);
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnAfterTaskbarActivate(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
