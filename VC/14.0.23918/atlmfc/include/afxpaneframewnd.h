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
#include "afxtoolbar.h"
#include "afxcaptionbutton.h"
#include "afxdragframeimpl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#pragma warning(push)
#pragma warning(disable : 4100 34)

/*============================================================================*/
// CPaneFrameWnd window

class CPane;
class CBasePane;
class CDockablePane;
class CMFCPopupMenu;

#define AFX_CAPTION_BTN_PIN       0x1
#define AFX_CAPTION_BTN_CLOSE     0x2
#define AFX_CAPTION_BTN_MENU      0x4
#define AFX_CAPTION_BTN_MAXIMIZE  0x8
#define AFX_CAPTION_BTN_CUSTOMIZE 0x10

enum AFX_PREDOCK_STATE
{
	PDS_NOTHING,
	PDS_DOCK_REGULAR,
	PDS_DOCK_TO_TAB
};

extern AFX_IMPORT_DATA UINT AFX_WM_CHECKEMPTYMINIFRAME;

class CPaneFrameWnd : public CWnd
{
	DECLARE_SERIAL(CPaneFrameWnd)
	friend class CPane;
	friend class CDockingManager;
	friend class CMFCBaseTabCtrl;
	friend class CMFCPopupMenu;

// Construction
public:
	CPaneFrameWnd();

// Attributes
public:
	CPoint GetHotPoint() const { return m_dragFrameImpl.m_ptHot; }
	void SetHotPoint(CPoint& ptNew) { m_dragFrameImpl.m_ptHot = ptNew; }
	BOOL IsCaptured() const { return m_bCaptured; }

	BOOL IsDelayShow() const { return m_bDelayShow; }
	void SetDelayShow(BOOL bDelayShow) { m_bDelayShow = bDelayShow; }

	virtual BOOL CanBeDockedToPane(const CDockablePane* pDockingBar) const;
	virtual BOOL IsRollDown() const;
	virtual BOOL IsRollUp() const;

	virtual int GetCaptionHeight() const { return m_nCaptionHeight; }

	virtual BOOL CanBeAttached() const
	{
		CPane* pBar = DYNAMIC_DOWNCAST(CPane, GetPane());
		if (pBar != NULL)
		{
			return pBar->CanBeAttached();
		}
		return FALSE;
	}

	bool m_bBlockMove;

	AFX_IMPORT_DATA static int m_nCaptionButtonMargin;
	AFX_IMPORT_DATA static int m_nCaptionButtonSpacing;
	AFX_IMPORT_DATA static const int m_nToolbarBorderSize;
	AFX_IMPORT_DATA static UINT m_nRollTimeOut;
	AFX_IMPORT_DATA static BOOL m_bUseSaveBits;

	void SetDockingManager(CDockingManager* pManager) { m_pDockManager = pManager; }
	CDockingManager* GetDockingManager() const { return m_pDockManager; }


	CWnd* GetParent() { return CWnd::FromHandlePermanent(m_hParentWnd); }

// Operations
public:
	virtual void AddPane(CBasePane* pWnd);
	virtual void RemovePane(CBasePane* pWnd, BOOL bDestroy = FALSE, BOOL bNoDelayedDestroy = FALSE);
	virtual void ReplacePane(CBasePane* pBarOrg, CBasePane* pBarReplaceWith);
	static BOOL __stdcall AddRemovePaneFromGlobalList(CBasePane* pWnd, BOOL bAdd);
	static void __stdcall RedrawAll();

	virtual CDockablePane* DockPane(BOOL& bWasDocked);
	virtual void CheckGripperVisibility() {}

	virtual void SetCaptionButtons(DWORD dwButtons);

	virtual BOOL SetPreDockState(AFX_PREDOCK_STATE preDockState, CBasePane* pBarToDock = NULL, AFX_DOCK_METHOD dockMethod = DM_MOUSE);

	AFX_PREDOCK_STATE GetPreDockState() { return m_preDockStateCurr; }

	virtual void OnMovePane(CPane* pBar, CPoint ptOffset);
	virtual void OnDockToRecentPos();
	virtual BOOL OnBeforeDock();

	virtual int GetPaneCount() const {return (CWnd::FromHandlePermanent(m_hEmbeddedBar) != NULL) ? 1 : 0;}
	virtual int GetVisiblePaneCount() const 
	{
		if (IsWindow(m_hEmbeddedBar) && (::GetWindowLong(m_hEmbeddedBar, GWL_STYLE) & WS_VISIBLE))
		{
			return 1;
		}
		return 0;
	}

	virtual CWnd* GetPane() const;
	virtual CWnd* GetFirstVisiblePane() const;

	virtual AFX_DOCK_TYPE GetDockingMode() const;
	virtual void SizeToContent();

	virtual CString GetCaptionText();
	virtual void CalcBorderSize(CRect& rectBorderSize) const;
	virtual void CalcExpectedDockedRect(CWnd* pWndToDock, CPoint ptMouse, CRect& rectResult, BOOL& bDrawTab, CDockablePane** ppTargetBar)
	{
		bDrawTab = FALSE;
		rectResult.SetRectEmpty();
	}

	virtual CBasePane* PaneFromPoint(CPoint point, int nSensitivity, BOOL bCheckVisibility);

	virtual void GetCaptionRect(CRect& rectCaption) const;

	virtual void OnPaneRecalcLayout();
	virtual void AdjustLayout();

	void SetDockingTimer(UINT nTimeOut);
	void KillDockingTimer();

	static CBasePane* __stdcall FindFloatingPaneByID(UINT nID);
	static CPaneFrameWnd* __stdcall FrameFromPoint(CPoint pt, int nSensitivity, CPaneFrameWnd* pFrameToExclude = NULL, BOOL bFloatMultiOnly = FALSE);

	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, UINT uiID = (UINT) -1);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, UINT uiID = (UINT) -1);

	virtual void Serialize(CArchive& ar);

	virtual void SetDockState(CDockingManager* pDockManager);
	virtual void StoreRecentDockSiteInfo(CPane* pBar);
	virtual void StoreRecentTabRelatedInfo(CDockablePane* pDockingBar, CDockablePane* pTabbedBar);

	CRect GetRecentFloatingRect() const {return m_rectRecentFloatingRect;}

	// does nothing - derived class should adjust container (if any)
	virtual void OnShowPane(CDockablePane* pBar, BOOL bShow) {}

	virtual LRESULT HitTest(CPoint point, BOOL bDetectCaption);

	virtual void RemoveNonValidPanes();

	void Pin(BOOL bPin = TRUE) { m_bPinned = bPin; }

	BOOL GetPinState() const { return m_bPinned; }

	BOOL StartTearOff(CMFCPopupMenu* pMenu);

	virtual void OnSetRollUpTimer();
	virtual void OnKillRollUpTimer();

	virtual void AdjustPaneFrames() {}
	virtual void ConvertToTabbedDocument();

// Overrides
public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext = NULL);
	virtual BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual void OnCapture(BOOL bOnOff);
	virtual void EnterDragMode(HWND hWndToDestroyOnRelease = NULL); 

	static void __stdcall GetPaneList(CObList& lstBars, CRuntimeClass* pRTCFilter = NULL, BOOL bIncludeTabs = FALSE);

// Implementation
public:
	virtual ~CPaneFrameWnd();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcDestroy();
	afx_msg void OnCancelMode();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnCheckEmptyState(WPARAM, LPARAM);
	afx_msg LRESULT OnFloatStatus(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg void OnExitSizeMove();
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

protected:
	void MoveDragFrame();
	BOOL MoveMiniFrame();
	virtual void OnDrawBorder(CDC* pDC);
	virtual void OnDrawCaptionButtons(CDC* pDC);

	virtual void SaveRecentFloatingState();

	virtual BOOL OnCloseMiniFrame();
	virtual void CloseMiniFrame();
	virtual void OnPressButtons(UINT nHit);

	virtual CDockablePane* DockPaneStandard(BOOL& bWasDocked);

	virtual void AddButton(UINT nHit);
	void RemoveButton(UINT nHit);
	void ShowButton(UINT nHit, BOOL bShow = TRUE);
	CMFCCaptionButton* FindButton(CPoint point) const;
	CMFCCaptionButton* FindButton(UINT uiHit) const;

	void ReplaceButton(UINT nHit, UINT nHitNew);
	void RemoveAllCaptionButtons();
	void ArrangeCaptionButtons();
	void SetCaptionButtonsToolTips();

	void RedrawCaptionButton(CMFCCaptionButton* pBtn);
	virtual void OnTrackCaptionButtons(CPoint point);
	virtual void StopCaptionButtonsTracking();

	CSize CalcAlignedRect(CRect& rectWnd, CDockSite* pDockBar = NULL);

	BOOL IsCustModeAndNotFloatingToolbar() const;
	int  GetCaptionButtonTotalWidth() const;

	virtual void CalcMinSize(CSize& sizeBase, MINMAXINFO FAR* lpMMI);
	virtual void OnCheckRollState();

	void SetRollUpTimer();
	void KillRollupTimer();

	virtual void RecalcCaptionHeight();
	void UpdateTooltips();

	void ForceAdjustLayout(CDockingManager* pDockManager);

protected:
	BOOL m_bActive;
	BOOL m_bIsMoving;
	BOOL m_bEnablePreDockState;
	BOOL m_bRolledUp;
	BOOL m_bPinned;
	BOOL m_bNoDelayedDestroy;
	BOOL m_bHostsToolbar;
	BOOL m_bDelayShow;
	bool m_bTabDragRectDisplayed;
	bool m_bCaptured;

	int  m_nHeightBeforeRollUp;
	UINT m_nCaptionHeight;
	UINT m_nHit; // Clicked button
	UINT m_nHot; // "Hot" button
	UINT m_nDockTimerID;
	UINT m_nRollTimerID;
	UINT m_nRestoredEmbeddedBarID;

	DWORD m_dwCaptionButtons;

	HWND m_hParentWnd;
	HWND m_hWndToDestroyOnRelease;
	HWND m_hLastFocusWnd;
	HWND m_hEmbeddedBar;

	CRect               m_rectRecentFloatingRect;  // loaded from archive for setting dock state
	CSize               m_sizeMinSize;
	CRect               m_rectRedraw;
	CPoint              m_ptHot;
	CObList             m_lstCaptionButtons;
	CToolTipCtrl*       m_pToolTip;
	CDockablePane* m_pPreDockBar;
	CMFCDragFrameImpl      m_dragFrameImpl;
	CDockingManager*       m_pDockManager; // docking manager related to embedded control bar's frame. Should be set on creation of miniframe

	AFX_PREDOCK_STATE m_preDockStateCurr;

	AFX_IMPORT_DATA static CMap<UINT,UINT,HWND,HWND> m_mapFloatingBars;
	AFX_IMPORT_DATA static CList<HWND, HWND> CPaneFrameWnd::m_lstFrames;
	AFX_IMPORT_DATA static CFrameWnd* m_pParentWndForSerialize; // should be set by caller before starting serialization
};

#pragma warning(pop)

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
