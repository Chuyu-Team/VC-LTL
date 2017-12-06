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

#include "afxcontrolbarutil.h"
#include "afxpane.h"
#include "afxmenuimages.h"
#include "afxpanecontainer.h"
#include "afxcaptionbutton.h"
#include "afxmultipaneframewnd.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_CONTROLBAR_BUTTONS_NUM 3  // Hide + Expand + Menu

class CDockablePane;
class CTabbedPane;
class CPaneContainerManager;

/*============================================================================*/
// CDockablePane control bar styles

#define AFX_SCBS_EDGELEFT       0x00000001
#define AFX_SCBS_EDGERIGHT      0x00000002
#define AFX_SCBS_EDGETOP        0x00000004
#define AFX_SCBS_EDGEBOTTOM     0x00000008
#define AFX_SCBS_EDGEALL        0x0000000F
#define AFX_SCBS_SHOWEDGES      0x00000010
#define AFX_SCBS_GRIPPER        0x00000020

static const DWORD AFX_DEFAULT_DOCKING_PANE_STYLE = AFX_CBRS_FLOAT | AFX_CBRS_CLOSE | AFX_CBRS_RESIZE | AFX_CBRS_AUTOHIDE;

class CTabbedPane;
class CPaneDivider;
class CMFCAutoHideBar;
class CMFCAutoHideButton;

extern AFX_IMPORT_DATA UINT AFX_WM_ON_PRESS_CLOSE_BUTTON;

class CDockablePane : public CPane
{
	friend class CDockingManager;
	friend class CPaneFrameWnd;
	friend class CMultiPaneFrameWnd;
	friend class CBaseTabbedPane;
	friend class CMDIClientAreaWnd;

	DECLARE_SERIAL(CDockablePane);

public:
	CDockablePane();
	virtual ~CDockablePane();

// Overrides
	public:
	virtual BOOL Create(LPCTSTR lpszCaption, CWnd* pParentWnd, const RECT& rect, BOOL bHasGripper, UINT nID, DWORD dwStyle, DWORD dwTabbedStyle = AFX_CBRS_REGULAR_TABS, DWORD dwControlBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE, CCreateContext* pContext = NULL);
	virtual BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszCaption, CWnd* pParentWnd, const RECT& rect, BOOL bHasGripper, UINT nID, DWORD dwStyle, DWORD dwTabbedStyle = AFX_CBRS_REGULAR_TABS, DWORD dwControlBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	virtual BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, CSize sizeDefault, BOOL bHasGripper, UINT nID, 
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_HIDE_INPLACE, DWORD dwTabbedStyle = AFX_CBRS_REGULAR_TABS, DWORD dwControlBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE);

	virtual void OnUpdateCmdUI(class CFrameWnd *pTarget, int bDisableIfNoHndler);
	virtual BOOL IsDocked() const;
	virtual int GetCaptionHeight() const;

	virtual void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const
	{
		rectTabAreaTop.SetRectEmpty();
		rectTabAreaBottom.SetRectEmpty();
	}

	virtual BOOL IsTabLocationBottom() const { return TRUE; }

	virtual void OnBeforeChangeParent(CWnd* pWndNewParent, BOOL bDelay = FALSE);
	virtual void OnAfterChangeParent(CWnd* pWndOldParent);

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual int HitTest(CPoint point, BOOL bDetectCaption = FALSE);

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual BOOL IsResizable() const { return TRUE; }
	virtual BOOL IsInFloatingMultiPaneFrameWnd() const;

	virtual BOOL CanAcceptPane(const CBasePane* pBar) const
	{
		if (pBar == NULL)
		{
			return FALSE;
		}

		return (pBar->IsKindOf(RUNTIME_CLASS(CDockablePane)) && (IsDocked() || IsInFloatingMultiPaneFrameWnd()));
	}

	virtual BOOL CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const;

	virtual BOOL CanBeAttached() const { return TRUE; }
	virtual BOOL IsAutoHideMode() const { return m_bPinState; }
	virtual BOOL HasAutoHideMode() const { return TRUE; }
	virtual BOOL IsAutohideAllEnabled() const { return m_bEnableAutoHideAll; }
	void EnableAutohideAll(BOOL bEnable = TRUE) { m_bEnableAutoHideAll = bEnable; }

	virtual BOOL IsAccessibilityCompatible() { return FALSE; }

	virtual BOOL IsVisible() const;
	virtual void Slide(BOOL bSlideOut, BOOL bUseTimer = TRUE);

	void SetAutoHideParents(CMFCAutoHideBar* pToolBar, CMFCAutoHideButton* pBtn);
	void SetResizeMode(BOOL bResize);

	virtual void ToggleAutoHide();

	static const CSize& __stdcall GetDragSensitivity() { return m_sizeDragSensitivity; }

	virtual CPane* DockPaneStandard(BOOL& bWasDocked);

	virtual AFX_CS_STATUS IsChangeState(int nOffset, CBasePane** ppTargetBar) const;
	virtual AFX_CS_STATUS GetDockingStatus(CPoint pt, int nSensitivity);

	virtual CDockablePane* AttachToTabWnd(CDockablePane* pTabControlBarAttachTo, AFX_DOCK_METHOD dockMethod, BOOL bSetActive = TRUE, CDockablePane** ppTabbedControlBar = NULL);
	virtual CTabbedPane* CreateTabbedPane();

	void SetDefaultPaneDivider(HWND hSliderWnd);
	CPaneDivider* GetDefaultPaneDivider() const;

	virtual BOOL DockToFrameWindow(DWORD dwAlignment, LPCRECT lpRect = NULL, DWORD dwDockFlags = DT_DOCK_LAST, CBasePane* pRelativeBar = NULL, int nRelativeIndex = -1, BOOL bOuterEdge = FALSE);
	virtual BOOL DockToWindow(CDockablePane* pTargetWindow, DWORD dwAlignment, LPCRECT lpRect = NULL);
	virtual BOOL DockPaneContainer(CPaneContainerManager& barContainerManager, DWORD dwAlignment, AFX_DOCK_METHOD dockMethod);

	virtual void UndockPane(BOOL bDelay = FALSE);

	virtual void OnAfterDockFromMiniFrame() { OnAfterDock(this, NULL, DM_UNKNOWN); }

	virtual void UnSetAutoHideMode (CDockablePane* pFirstBarInGroup);
	BOOL ReplacePane(CDockablePane* pBarToReplaceWith, AFX_DOCK_METHOD dockMethod, BOOL bRegisterWithFrame = FALSE);

	void RemoveFromDefaultPaneDivider();

	virtual void ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate/* = TRUE*/);
	virtual BOOL DockToRecentPos() { return Dock(NULL, NULL, DM_SHOW); }

	virtual void EnableGripper(BOOL bEnable);

	virtual CMFCAutoHideBar* SetAutoHideMode(BOOL bMode, DWORD dwAlignment, CMFCAutoHideBar* pCurrAutoHideBar = NULL, BOOL bUseTimer = TRUE);

	static CPaneDivider* __stdcall CreateDefaultPaneDivider(DWORD dwAlignment, CWnd* pParent, CRuntimeClass* pSliderRTC = NULL);

	AFX_IMPORT_DATA static BOOL m_bHideInAutoHideMode;
	AFX_IMPORT_DATA static BOOL m_bDisableAnimation;

	virtual BOOL IsHideInAutoHideMode() const { return m_bHideInAutoHideMode; }
	virtual BOOL CanAutoHide() const;

	BOOL IsTracked() const { return m_bPrepareToFloat; }

	void SetLastPercentInPaneContainer(int n) { m_nLastPercent = n; }
	int  GetLastPercentInPaneContainer() const { return m_nLastPercent; }

	CRect GetAHRestoredRect() const {return m_rectRestored;}
	virtual UINT GetAHSlideMode() const { return m_ahSlideMode; }

	CMFCAutoHideBar* GetAutoHideToolBar() const { return m_pAutoHideBar; }
	CMFCAutoHideButton* GetAutoHideButton() { return m_pAutoHideButton; }

	virtual void Serialize(CArchive& ar);
	virtual void CopyState(CDockablePane* pOrgBar);

	virtual BOOL OnBeforeFloat(CRect& rectFloat, AFX_DOCK_METHOD dockMethod);

	void SetTabbedPaneRTC(CRuntimeClass* pRTC)
	{
		ASSERT(pRTC != NULL);
		m_pTabbedControlBarRTC = pRTC;
	}

	void SetRestoredDefaultPaneDivider(HWND hRestoredSlider) { m_hRestoredDefaultSlider = hRestoredSlider; }
	void RestoreDefaultPaneDivider() { SetDefaultPaneDivider(m_hRestoredDefaultSlider); }

	CRuntimeClass* GetTabbedPaneRTC() const { return m_pTabbedControlBarRTC; }

	virtual void ConvertToTabbedDocument(BOOL bActiveTabOnly = TRUE);

	AFX_IMPORT_DATA static int m_nTimeOutBeforeAutoHide;
	AFX_IMPORT_DATA static int m_nSlideSteps;

protected:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnSetText(LPCTSTR lpszText);
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL Dock(CBasePane* pTargetBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);
	virtual void OnContinueMoving() {} // do nothing - resizable control bar doesn't move after docking
	virtual void OnDrawDragRect(LPCRECT lprectNew, LPCRECT lprectOld);
	virtual void OnAfterDock(CBasePane* /*pBar*/, LPCRECT /*lpRect*/, AFX_DOCK_METHOD /*dockMethod*/); 
	
	virtual void EnterDragMode(BOOL bChangeHotPoint);
	virtual void StoreRecentDockSiteInfo();
	virtual void StoreRecentTabRelatedInfo();

	virtual void OnPressCloseButton();
	virtual void OnPressButtons(UINT /*nHit*/) {}

	virtual void DrawCaption(CDC* pDC, CRect rectCaption);
	virtual void AdjustPaneToPaneContainer(CPaneDivider* pSlider);

	void RedrawButton(const CMFCCaptionButton* pBtn);
	CMFCCaptionButton* FindButton(CPoint point) const;
	CMFCCaptionButton* FindButtonByHit(UINT nHit) const;
	void EnableButton(UINT nHit, BOOL bEnable = TRUE);
	BOOL IsButtonEnabled(UINT nHit) const;

	virtual void OnTrackCaptionButtons(CPoint point);
	virtual void StopCaptionButtonsTracking();

	virtual void SetCaptionButtons();
	void RemoveCaptionButtons();
	
	void UpdateTooltips();

	CRect m_rectCloseButton;

	/// how far the mouse is moved after NcLBtnDown before the bar becomes floating
	AFX_IMPORT_DATA static CSize m_sizeDragSensitivity;

	bool m_bPrepareToFloat;
	bool m_bReadyToFloat;
	int  m_cyGripper;
	int  m_nLastPercent;
	int  m_nBorderSize;
	BOOL m_bHasGripper; // indicates that the bar has caption (it may not be shown when floating)
	BOOL m_bActive;
	BOOL m_bEnableAutoHideAll;
	DWORD m_dwSCBStyle;
	CRect m_rectRedraw;

	// auto hide support
	BOOL     m_bPinState;
	UINT_PTR m_nAutoHideConditionTimerID;
	UINT_PTR m_nSlideTimer;
	int      m_nSlideStep;
	int      m_nSlideDelta;

	AFX_IMPORT_DATA static int m_nSlideDefaultTimeOut;

	CRect m_rectRestored;
	BOOL  m_bIsSliding;
	BOOL  m_bIsResizing;
	BOOL  m_bIsHiding;
	UINT  m_ahSlideMode;

	CMFCAutoHideButton*  m_pAutoHideButton;
	CMFCAutoHideBar* m_pAutoHideBar;

	virtual BOOL CheckAutoHideCondition();
	virtual BOOL CheckStopSlideCondition(BOOL bDirection);
	virtual void OnSlide(BOOL bSlideOut);

	void CalcRectForSliding(CRect& rect, CRect& rectSlider, BOOL bSlideDirection);
	void OffsetRectForSliding(CRect& rect, BOOL bSlideDirection, BOOL bIsRTL);
	CPoint CalcCorrectOffset(CRect rect, BOOL bIsRTL);

	CTypedPtrArray<CObArray, CMFCCaptionButton*> m_arrButtons;

	CRuntimeClass* m_pTabbedControlBarRTC;
	CToolTipCtrl*  m_pToolTip;

	HWND m_hDefaultSlider;
	HWND m_hRestoredDefaultSlider;
	UINT m_nHot;
	UINT m_nHit;
	BOOL m_bCaptionButtonsCaptured;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
