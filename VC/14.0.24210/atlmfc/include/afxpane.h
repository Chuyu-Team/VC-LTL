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
#include "afxbasepane.h"
#include "afxrecentdocksiteinfo.h"
#include "afxdragframeimpl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#pragma warning(push)
#pragma warning(disable : 4100 34)

#define AFX_CONTROL_BAR_PROFILE  _T("Panes")

/*============================================================================*/
// CPane window

class CDockSite;
class CFrameWndEx;
class CDockingPanesRow;
class CPaneFrameWnd;
class CDockingManager;


// CS - change state status
enum AFX_CS_STATUS
{
	CS_NOTHING,
	CS_DOCK_IMMEDIATELY,
	CS_DELAY_DOCK,
	CS_DELAY_DOCK_TO_TAB
};


static const DWORD AFX_DEFAULT_PANE_STYLE = AFX_CBRS_FLOAT | AFX_CBRS_CLOSE;
static const int AFX_DEFAULT_TOOLBAR_STYLE = (WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_HIDE_INPLACE);


/*============================================================================*/
// CPane window

class CPane : public CBasePane
{
	friend class CPaneFrameWnd;
	friend class CMultiPaneFrameWnd;

	DECLARE_DYNCREATE(CPane)

// Construction
protected:
	CPane();

// Attributes
public:

	virtual BOOL CanBeDocked(CBasePane* pDockBar) const;
	virtual BOOL IsResizable() const { return FALSE; }
	virtual BOOL IsDragMode() const { return m_bDragMode; }
	virtual void GetMinSize(CSize& size) const { size = m_sizeMin; }

	AFX_IMPORT_DATA static BOOL m_bHandleMinSize;

	void SetMinSize(const CSize& size) { m_sizeMin = size; }
	virtual int GetResizeStep() const { return -1; }

	CPoint GetHotSpot() const { return m_dragFrameImpl.m_ptHot; }
	CPoint GetClientHotSpot() const { return m_ptClientHotSpot; }
	void SetClientHotSpot(const CPoint& ptNew) { m_ptClientHotSpot = ptNew; }

	virtual BOOL IsInFloatingMultiPaneFrameWnd() const { return FALSE; }

	virtual int GetAvailableExpandSize() const;
	virtual int GetAvailableStretchSize() const;
	
	virtual BOOL DoesAllowSiblingBars() const { return !m_bExclusiveRow; }

	bool IsLeftOf(CRect rect, bool bWindowRect = true) const;

	virtual BOOL IsTabbed() const;
	virtual void CopyState(CPane* pOrgBar);

	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const;

	CDockingPanesRow* GetDockSiteRow() const { return m_pDockBarRow; }
	void SetDockSiteRow(CDockingPanesRow* pRow) { m_pDockBarRow = pRow; }

	void GetVirtualRect(CRect& rectVirtual) const;
	void SetVirtualRect(const CRect& rect, BOOL bMapToParent = TRUE);
	void UpdateVirtualRect();
	void UpdateVirtualRect(CPoint ptOffset);
	void UpdateVirtualRect(CSize sizeNew);
	
	bool m_bDisableMove;
	BOOL m_bFirstInGroup;
	BOOL m_bLastInGroup;
	BOOL m_bActiveInGroup;

	virtual void SetActiveInGroup(BOOL bActive);

	virtual void SetExclusiveRowMode(BOOL bExclusive = TRUE) { m_bExclusiveRow = bExclusive; }
	virtual BOOL GetExclusiveRowMode() const { return m_bExclusiveRow; }

// Operations
public:
	virtual void PrepareToDock(CDockSite* pDockBar, AFX_DOCK_METHOD dockMethod);
	virtual BOOL DockPane(CBasePane* pDockBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);
	virtual BOOL DockByMouse(CBasePane* pDockBar);
	virtual BOOL DockToFrameWindow(DWORD dwAlignment, LPCRECT lpRect = NULL, DWORD dwDockFlags = DT_DOCK_LAST, 
		CBasePane* pRelativeBar = NULL, int nRelativeIndex = -1, BOOL bOuterEdge = FALSE)
	{
		return FALSE;
	}

	virtual BOOL FloatPane(CRect rectFloat, AFX_DOCK_METHOD dockMethod = DM_UNKNOWN, bool bShow = true);
	virtual CSize CalcSize(BOOL bVertDock) {return CSize(0, 0);}
	
	BOOL MoveByAlignment(DWORD dwAlignment, int nOffset);
	virtual CSize MovePane(CRect rectNew, BOOL bForceMove, HDWP& hdwp);
	virtual int  StretchPaneDeferWndPos(int nStretchSize, HDWP& hdwp);

	virtual CPane* DockPaneStandard(BOOL& bWasDocked)
	{
		bWasDocked = FALSE;
		return this;
	}
	virtual AFX_CS_STATUS IsChangeState(int nOffset, CBasePane** ppTargetBar) const;

	virtual CPaneFrameWnd* CreateDefaultMiniframe(CRect rectInitial);

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual void SetDockState(CDockingManager* pDockManager);
	virtual void OnProcessDblClk();

	virtual BOOL OnBeforeFloat(CRect& rectFloat, AFX_DOCK_METHOD dockMethod);
	virtual void OnAfterFloat();

	virtual void UndockPane(BOOL bDelay = FALSE);

// Overrides
public:
	virtual BOOL Create(LPCTSTR lpszClassName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwControlBarStyle = AFX_DEFAULT_PANE_STYLE, CCreateContext* pContext = NULL);
	virtual BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszClassName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwControlBarStyle = AFX_DEFAULT_PANE_STYLE, CCreateContext* pContext = NULL);

	virtual void OnBeforeChangeParent(CWnd* pWndNewParent, BOOL bDelay = FALSE);
	virtual void OnAfterChangeParent(CWnd* pWndOldParent);

	virtual void RecalcLayout();
	virtual CSize CalcAvailableSize(CRect rectRequired);

	void SetMiniFrameRTC(CRuntimeClass* pClass)	{m_pMiniFrameRTC = pClass;}

	virtual void AdjustSizeImmediate(BOOL bRecalcLayout = TRUE);

	virtual void ToggleAutoHide()	{}
	virtual void OnPressCloseButton();

	virtual BOOL CanBeTabbedDocument() const;
	virtual void ConvertToTabbedDocument(BOOL bActiveTabOnly = TRUE);

public:
	virtual BOOL OnShowControlBarMenu(CPoint point);
	
protected:
	virtual BOOL OnBeforeShowPaneMenu(CMenu& /*menu*/) { return TRUE; }
	virtual BOOL OnAfterShowPaneMenu(int nMenuResult) { return TRUE; }

	virtual void OnContinueMoving();
	virtual void OnDrawDragRect(LPCRECT /*lprectNew*/, LPCRECT /*lprectOld*/) {}
	
	virtual BOOL OnBeforeDock(CBasePane** ppDockBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);
	virtual void OnAfterDock(CBasePane* /*pBar*/, LPCRECT /*lpRect*/, AFX_DOCK_METHOD /*dockMethod*/) {}

	virtual void OnBeforeStretch(int& /*nStretchSize*/) {}
	virtual void OnAfterStretch(int /*nStretchSize*/) {}
	
	virtual void RemoveFromMiniframe(CWnd* pNewParent, AFX_DOCK_METHOD dockMethod);
	virtual void SetDragMode(BOOL bOnOff);

	virtual BOOL Dock(CBasePane* pDockBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);

	virtual void EnterDragMode(BOOL bChangeHotPoint);
	virtual void OnRTLChanged(BOOL bIsRTL);

	// Implementation
public:
	virtual ~CPane();
	
	//---------------------------------------------------------------------------------------------
	// FOR COMPATIBILITY with MFC's CControlBar:
	//----------------------------------------------------------------------------------------------
	void SetBorders(int cxLeft = 0, int cyTop = 0, int cxRight = 0, int cyBottom = 0);
	void SetBorders(LPCRECT lpRect);
	CRect GetBorders() const;

	// array of elements
	int m_nCount;
	void* m_pData;        // m_nCount elements - type depends on derived class

	// info about bar (for status bar and toolbar)
	int m_cxLeftBorder, m_cxRightBorder;
	int m_cyTopBorder, m_cyBottomBorder;
	int m_cxDefaultGap;         // default gap value
	int m_nMRUWidth;

	void CalcInsideRect(CRect& rect, BOOL bHorz) const; // adjusts borders etc
	BOOL AllocElements(int nElements, int cbElement);

	//----------------------------------------------------------------------------------------------
	// Generated message map functions
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNcDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);

	DECLARE_MESSAGE_MAP()

	CRect  m_rectVirtual;
	CRect  m_rectDragImmediate;
	CSize  m_sizeMin;
	CPoint m_ptClientHotSpot;
	bool   m_bCaptured;
	bool   m_bDisableChangeHot;
	bool   m_bDblClick;
	BOOL   m_bDragMode;
	BOOL   m_bExclusiveRow;
	BOOL   m_bPinState;
	UINT   m_nID;
	HWND   m_hwndMiniFrameToBeClosed;

	CRuntimeClass* m_pMiniFrameRTC;
	CMFCDragFrameImpl m_dragFrameImpl;

//-----------------------------------------------
//  Recent dock/float state
//-----------------------------------------------
public:
	CRecentDockSiteInfo m_recentDockInfo;
	CRect m_rectSavedDockedRect; // in client coordinates

	BOOL m_bRecentFloatingState;
	BOOL m_bWasFloatingBeforeMove;
	BOOL m_bWasFloatingBeforeTabbed;

	void CalcRecentDockedRect();

protected:
	virtual void StoreRecentDockSiteInfo();
//-----------------------------------------------
};

_inline BOOL CPane::CanBeDocked(CBasePane* pDockBar) const
{
	ASSERT_VALID(this);
	return ((GetEnabledAlignment() & pDockBar->GetCurrentAlignment()) != 0);
}

#pragma warning(pop)

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
