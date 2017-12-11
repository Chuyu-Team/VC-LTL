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

#ifndef __AFXMT_H__
	#include <afxmt.h>
#endif

#include "afxcontrolbarutil.h"
#include "afxglobals.h"
#include "afxaccessibility.h"

#pragma warning(push)
#pragma warning(disable : 4100 34)

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CPaneDivider;
class CPaneFrameWnd;
class CDockSite;
class CDockingPanesRow;
class CTabbedPane;
class CMFCBaseTabCtrl;
class CBaseTabbedPane;

// DM - dock method
enum AFX_DOCK_METHOD
{
	DM_UNKNOWN,
	DM_MOUSE,
	DM_DBL_CLICK,
	DM_SHOW,
	DM_RECT,
	DM_STANDARD
};

// DT - dock type
enum AFX_DOCK_TYPES
{
	DT_DOCK_FIRST,
	DT_DOCK_LAST,
	DT_DOCK_BEFORE,
	DT_DOCK_AFTER,
	DT_DOCK_BY_INDEX
};

static const DWORD AFX_CBRS_FLOAT        = 0x1;
static const DWORD AFX_CBRS_AUTOHIDE     = 0x2;
static const DWORD AFX_CBRS_RESIZE       = 0x4;
static const DWORD AFX_CBRS_CLOSE        = 0x8;
static const DWORD AFX_CBRS_AUTO_ROLLUP  = 0x10;

static const DWORD AFX_CBRS_REGULAR_TABS = 0x20;
static const DWORD AFX_CBRS_OUTLOOK_TABS = 0x40;

/*============================================================================*/
// CBasePane window

class CBasePane : public CWnd
{
	friend class CDockingManager;
	friend class CMDIChildWndEx;
	friend class CMFCTabCtrl;

DECLARE_DYNAMIC(CBasePane)

// Construction
public:
	CBasePane();

// Attributes
public:
	virtual BOOL IsHorizontal() const;
	virtual BOOL IsDocked() const;
	virtual BOOL IsTabbed() const;
	virtual BOOL IsFloating() const { return !IsDocked(); }
	virtual BOOL IsMDITabbed() const;
	virtual BOOL IsResizable() const { return TRUE; }
	virtual BOOL IsVisible() const;
	virtual BOOL DoesAllowDynInsertBefore() const { return CanFloat(); }
	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL CanBeDocked(CBasePane* /*pDockBar*/) const { return FALSE; }
	virtual BOOL CanBeAttached() const { return FALSE; }
	virtual BOOL CanBeTabbedDocument() const { return FALSE; }
	virtual DWORD GetCurrentAlignment() const;
	virtual DWORD GetEnabledAlignment() const { return m_dwEnabledAlignment; }
	virtual CWnd* GetDockSiteFrameWnd() const;
	virtual CDockSite* GetParentDockSite() const { return m_pParentDockBar; }
	virtual int GetCaptionHeight() const { return 0; }
	virtual BOOL IsInFloatingMultiPaneFrameWnd() const { return FALSE; }
	
	virtual BOOL GetRecentVisibleState() const { return m_bRecentVisibleState ;}
	virtual BOOL IsRestoredFromRegistry() const { return m_bIsRestoredFromRegistry; }

	// use when loading from registry was partial (due to exception) to clean the state
	virtual void SetRestoredFromRegistry(BOOL bState) {m_bIsRestoredFromRegistry = bState;}

	virtual AFX_DOCK_TYPE GetDockingMode() const;
	virtual HICON GetPaneIcon(BOOL bBigIcon) {return CWnd::GetIcon(bBigIcon);}

	BOOL IsPaneVisible() const { return ((GetStyle() & WS_VISIBLE) != 0); }

	void SetDockingMode(AFX_DOCK_TYPE dockModeNew) { m_dockMode = dockModeNew; }
	void SetMDITabbed(BOOL bValue) { m_bIsMDITabbed = bValue; }

	virtual DWORD GetPaneStyle() const { return m_dwStyle; }
	virtual DWORD GetControlBarStyle() const { return m_dwControlBarStyle; }
	virtual BOOL CanBeClosed() const { return m_dwControlBarStyle & AFX_CBRS_CLOSE; }
	virtual BOOL CanFloat() const;
	virtual BOOL CanBeResized() const { return m_dwControlBarStyle & AFX_CBRS_RESIZE; }
	virtual BOOL CanAutoHide() const { return m_dwControlBarStyle & AFX_CBRS_AUTOHIDE; }
	virtual BOOL CanFocus() const { return TRUE; }

	virtual BOOL IsAutoHideMode() const { return FALSE; }

	CDockingPanesRow* GetPaneRow() {return m_pDockBarRow;}

	static BOOL __stdcall IsTooltipTopmost() { return m_bSetTooltipTopmost; }

	AFX_IMPORT_DATA static BOOL m_bSetTooltipTopmost;
	AFX_IMPORT_DATA static BOOL m_bMultiThreaded;

// Operations
public:
	virtual void SetPaneAlignment(DWORD dwAlignment)
	{
		m_dwStyle &= ~(CBRS_ALIGN_ANY);
		m_dwStyle |= dwAlignment;
	}

	virtual void SetPaneStyle(DWORD dwNewStyle) { m_dwStyle = dwNewStyle; }
	virtual void SetControlBarStyle(DWORD dwNewStyle) { m_dwControlBarStyle = dwNewStyle; }
	virtual void EnableDocking(DWORD dwAlignment) { m_dwEnabledAlignment = dwAlignment; }
	virtual void EnableGripper(BOOL bEnable) {}
	virtual void OnDrawCaption() {}
	virtual BOOL DockPane(CBasePane* pDockBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod) { return FALSE; }
	virtual BOOL FloatPane(CRect rectFloat, AFX_DOCK_METHOD dockMethod = DM_UNKNOWN, bool bShow = true) { return FALSE; }

	virtual BOOL DockToFrameWindow(DWORD dwAlignment, LPCRECT lpRect = NULL, DWORD dwDockFlags = DT_DOCK_LAST,
		CBasePane* pRelativeBar = NULL, int nRelativeIndex = -1, BOOL bOuterEdge = FALSE)
	{
		return FALSE;
	}
	
	virtual void OnMovePaneDivider(CPaneDivider* /*pSlider*/) {}
	virtual CSize StretchPane(int nLength, BOOL bVert) { return CSize(0, 0); }

	void DockPaneUsingRTTI(BOOL bUseDockSite);

	virtual void AdjustLayout() {}
	virtual void RecalcLayout() {}

	virtual CPaneFrameWnd* CreateDefaultMiniframe(CRect rectInitial) { return NULL; }
	virtual void OnRemoveFromMiniFrame(CPaneFrameWnd* pMiniFrame) {}

	virtual void OnBeforeChangeParent(CWnd* pWndNewParent, BOOL bDelay = FALSE) {}
	virtual void OnAfterChangeParent(CWnd* pWndOldParent) {}

	virtual void ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate/* = TRUE*/);

	virtual CPaneFrameWnd* GetParentMiniFrame(BOOL bNoAssert = FALSE) const;

	virtual void Serialize(CArchive& ar);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual HDWP MoveWindow(const CRect& rect, BOOL bRepaint = TRUE, HDWP hdwp = NULL);
	virtual HDWP SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags, HDWP hdwp = NULL);

	virtual void CopyState(CBasePane* pOrgBar);

	CMFCBaseTabCtrl* GetParentTabWnd(HWND& hWndTab) const;
	CBaseTabbedPane* GetParentTabbedPane() const;
	virtual void UndockPane(BOOL bDelay = FALSE) {}

	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/) {}

	const CRect& GetPaneRect() const { return m_rectBar; }
	void SetPaneRect(const CRect& rect) { m_rectBar = rect; }
	void FillWindowRect() { GetWindowRect(m_rectBar); }

	virtual void OnPaneContextMenu(CWnd* pParentFrame, CPoint point);

	virtual BOOL HideInPrintPreviewMode() const { return TRUE; }

// Overrides
public:
	virtual BOOL CreateEx(DWORD dwStyleEx, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwControlBarStyle = 0, CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Accessibility
public:
	virtual BOOL OnSetAccData(long lVal);
	virtual BOOL IsAccessibilityCompatible() { return TRUE; }
	virtual void AccNotifyObjectFocusEvent(int /*iButton*/) {};

	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT get_accName(VARIANT varChild, BSTR *pszName);
	virtual HRESULT get_accDescription(VARIANT varChild, BSTR *pszDescription);
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);
	virtual HRESULT get_accHelp(VARIANT varChild, BSTR *pszHelp);
	virtual HRESULT get_accFocus(VARIANT *pvarChild);
	virtual HRESULT get_accSelection(VARIANT *pvarChildren);
	virtual HRESULT get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic);
	virtual HRESULT get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut);
	virtual HRESULT get_accValue(VARIANT varChild, BSTR *pszValue);
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);

	virtual HRESULT accSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CBasePane();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz)
	{
		CSize size;
		size.cx = (bStretch && bHorz ? 32767 : 0);
		size.cy = (bStretch && !bHorz ? 32767 : 0);
		return size;
	}

protected:
	virtual void DoPaint(CDC* pDC);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM lParam);
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg HICON OnSetIcon(BOOL bIsLarge, HICON hIcon);
	afx_msg LRESULT OnGetObject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	BOOL m_bVisible; // use this status instead of IsWindowVisible
	BOOL m_bRecentVisibleState;
	BOOL m_bIsRestoredFromRegistry;
	BOOL m_bIsDlgControl;
	BOOL m_bIsMDITabbed;
	BOOL m_bEnableIDChecking;

	DWORD m_dwEnabledAlignment;
	DWORD m_dwStyle;
	DWORD m_dwControlBarStyle;

	CWnd*              m_pDockSite;
	CRect              m_rectBar;
	CDockSite*    m_pParentDockBar;
	CDockingPanesRow* m_pDockBarRow;

	AFX_DOCK_TYPE m_dockMode;

	// Dialog bar-specific:
	LPTSTR m_lpszBarTemplateName;
	CSize  m_sizeDialog;

	// Accessibility attributes:
	CAccessibilityData m_AccData;

	static CCriticalSection m_CriticalSection;

public:
	//------------ for frame mapping ---------------------------
	void AddPane(CBasePane* pBar);
	BOOL InsertPane	(CBasePane* pControlBar, CBasePane* pTarget, BOOL bAfter = TRUE);
	void RemovePaneFromDockManager(CBasePane* pBar, BOOL bDestroy = TRUE, BOOL bAdjustLayout = FALSE, BOOL bAutoHide = FALSE, CBasePane* pBarReplacement = NULL);
	BOOL IsPointNearDockSite(CPoint point, DWORD& dwBarAlignment, BOOL& bOuterEdge) const;
	CBasePane* PaneFromPoint(CPoint point, int nSensitivity, bool bExactBar = false, CRuntimeClass* pRTCBarType = NULL) const;
	virtual void AdjustDockingLayout(HDWP hdwp = NULL);

	BOOL IsDialogControl() const { return m_bIsDlgControl; }

	CFont* SelectDefaultFont(CDC* pDC)
	{
		if (m_bIsDlgControl)
		{
			return(CFont*) pDC->SelectStockObject(DEFAULT_GUI_FONT);
		}
		else
		{
			return pDC->SelectObject(&(GetGlobalData()->fontRegular));
		}
	}
};

_inline BOOL CBasePane::IsHorizontal() const
{
	ASSERT_VALID(this);
	return(GetCurrentAlignment() & CBRS_ORIENT_HORZ);
}

_inline BOOL CBasePane::CanAcceptPane(const CBasePane* /* pBar */) const
{
	return FALSE;
}

#pragma warning(pop)

/*============================================================================*/

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
