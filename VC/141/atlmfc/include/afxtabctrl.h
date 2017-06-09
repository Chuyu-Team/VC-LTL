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

#ifndef NO_TAB_WND_INFO

#include "afxcontrolbarutil.h"
#include "afxbasepane.h"
#include "afxbasetabctrl.h"
#include "afxbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCTabCtrl window

class CMFCTabInfo;

extern AFX_IMPORT_DATA UINT AFX_WM_GETDRAGBOUNDS;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_DRAGCOMPLETE;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_TABGROUPMOUSEMOVE;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_CANCELTABMOVE;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_MOVETABCOMPLETE;

class CMFCTabButton : public CMFCButton
{
	virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);

public:
	void SetPressed(BOOL bOn)
	{
		m_bPushed = bOn;
		m_bHighlighted = bOn;
		RedrawWindow();
	}
};

class CMFCTabCtrl : public CMFCBaseTabCtrl
{
	friend class CMDIClientAreaWnd;

	DECLARE_DYNCREATE(CMFCTabCtrl)

// Construction
public:
	CMFCTabCtrl();

	enum Style
	{
		STYLE_3D                      = 0,
		STYLE_FLAT                    = 1,
		STYLE_FLAT_SHARED_HORZ_SCROLL = 2,
		STYLE_3D_SCROLLED             = 3,
		STYLE_3D_ONENOTE              = 4,
		STYLE_3D_VS2005               = 5,
		STYLE_3D_ROUNDED              = 6,
		STYLE_3D_ROUNDED_SCROLL       = 7,
	};

	enum ResizeMode
	{
		RESIZE_NO    = 0,
		RESIZE_VERT  = 1,
		RESIZE_HORIZ = 2,
	};

// Operations
public:
	// Create methods:
	BOOL Create(Style style, const RECT& rect, CWnd* pParentWnd, UINT nID, Location location = LOCATION_BOTTOM, BOOL bCloseBtn = FALSE);
	virtual BOOL SetImageList(UINT uiID, int cx = 15, COLORREF clrTransp = RGB(255, 0, 255));
	virtual BOOL SetImageList(HIMAGELIST hImageList);

	BOOL ModifyTabStyle(Style style);

	virtual void RecalcLayout();

	void EnableActiveTabCloseButton(BOOL bEnable = TRUE);
	virtual BOOL IsActiveTabCloseButton() const { return m_bActiveTabCloseButton; }
	
	void EnableTabDocumentsMenu(BOOL bEnable = TRUE);
	BOOL IsTabDocumentsMenu() const { return m_bTabDocumentsMenu; }

	virtual void EnableInPlaceEdit(BOOL bEnable);
	virtual void CalcRectEdit(CRect& rectEdit)
	{
		ASSERT_VALID(this);
		rectEdit.DeflateRect(m_nTabsHeight / 2, 1);
	}

	virtual BOOL SetActiveTab(int iTab);
	virtual BOOL EnsureVisible(int iTab);

	// Scrolling methods:
	BOOL SynchronizeScrollBar(SCROLLINFO* pScrollInfo = NULL);
	CScrollBar* GetScrollBar() { return m_bSharedScroll ? &m_wndScrollWnd : NULL; }

	virtual void HideSingleTab(BOOL bHide = TRUE);
	void HideActiveWindowHorzScrollBar();

	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	static HICON __stdcall GetDocumentIcon(UINT nCmdID);

	virtual void OnDraw(CDC* pDC);

protected:
	virtual CWnd* FindTargetWnd(const CPoint& pt);

	virtual void AdjustTabs();
	virtual void AdjustTabsScroll();
	virtual void AdjustWndScroll();

	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void SetTabsHeight();
	void ReposButtons(CPoint pt, CSize sizeButton, BOOL bHide, int nButtonMargin);

	void UpdateScrollButtonsState();
	void SetScrollButtons();

// Attributes
public:
	virtual BOOL IsPtInTabArea(CPoint point) const;
	virtual BOOL AllowDestroyEmptyTabbedPane() const {return TRUE;}
	virtual int GetTabFromPoint(CPoint& pt) const;

	void SetFlatFrame(BOOL bFlat = TRUE, BOOL bRepaint = TRUE)
	{
		if (m_bFlatFrame != bFlat)
		{
			m_bFlatFrame = bFlat;
			if (bRepaint && GetSafeHwnd() != NULL)
			{
				Invalidate();
				UpdateWindow();
			}
		}
	}

	BOOL IsFlatFrame() const { return m_bFlatFrame; }
	void HideInactiveWindow(BOOL bHide = TRUE) { m_bHideInactiveWnd = bHide; }
	void AutoSizeWindow(BOOL bAutoSize = TRUE) { m_bAutoSizeWindow = bAutoSize; }
	void GetWndArea(CRect& rect) const { rect = m_rectWndArea; }

	void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const
	{
		rectTabAreaTop.SetRectEmpty();
		rectTabAreaBottom.SetRectEmpty();
		(m_location == LOCATION_TOP) ? rectTabAreaTop = m_rectTabsArea : rectTabAreaBottom = m_rectTabsArea;
	}

	virtual void GetTabsRect(CRect& rect) const { rect = m_rectTabsArea; }

	virtual int GetTabsHeight() const
	{
		if (!m_bSharedScroll)
		{
			int nVisibleTabs = GetVisibleTabsNum();
			if ((m_bHideSingleTab && nVisibleTabs <= 1) || (m_bHideNoTabs && nVisibleTabs == 0))
			{
				return 0;
			}
		}
		return m_nTabsHeight;
	}

	BOOL IsSharedScroll() const { return m_bSharedScroll; }

	virtual BOOL IsFlatTab() const { return m_bFlat; }
	virtual BOOL IsVS2005Style() const { return m_bIsVS2005Style; }
	virtual BOOL IsOneNoteStyle() const { return m_bIsOneNoteStyle; }
	virtual BOOL IsLeftRightRounded() const { return m_bLeftRightRounded; }

	void HideNoTabs(BOOL bHide = TRUE);

	void SetActiveTabBoldFont(BOOL bIsBold = TRUE);
	BOOL IsActiveTabBoldFont() const { return m_bIsActiveTabBold; }

	void SetDrawFrame(BOOL bDraw = TRUE);
	BOOL IsDrawFrame() const { return m_bDrawFrame; }

	AFX_IMPORT_DATA static BOOL m_bEnableActivate;

	virtual int GetFirstVisibleTabNum() const
	{
		// For m_bIsOneNoteStyle only!
		if (m_arTabIndices.GetSize() != m_arTabs.GetSize() || m_arTabIndices.GetSize() == 0)
		{
			return m_nFirstVisibleTab;
		}

		return m_arTabIndices[0];
	}

	virtual void SwapTabs(int nFisrtTabID, int nSecondTabID);
	virtual void MoveTab(int nSource, int nDest);

	virtual void OnShowTabDocumentsMenu(CPoint point);

	void SetTabMaxWidth(int nTabMaxWidth);
	int GetTabMaxWidth() const { return m_nTabMaxWidth; }

	void SetResizeMode(ResizeMode resizeMode);
	ResizeMode GetResizeMode() const { return m_ResizeMode; }

	void StopResize(BOOL bCancel);
	virtual BOOL IsMDITabGroup() const;
	void ActivateMDITab(int nTab = -1);
	void SetActiveInMDITabGroup(BOOL bActive)  {m_bIsActiveInMDITabGroup = bActive;}
	BOOL IsActiveInMDITabGroup() const {return m_bIsActiveInMDITabGroup;}

protected:
	BOOL m_bFlat;                 // Is flat (Excel-like) mode
	BOOL m_bIsOneNoteStyle;       // Is OneNote-like mode
	BOOL m_bIsVS2005Style;        // Is VS.NET 2005 MDI-like mode
	BOOL m_bLeftRightRounded;     // Is VS.NET 2005 two-side rounded tab
	BOOL m_bSharedScroll;         // Have a scrollbar shared with active window
	BOOL m_bScroll;               // Scroll buttons
	BOOL m_bCloseBtn;             // Close button
	BOOL m_bTrackSplitter;        // Is splitter tracked?
	BOOL m_bFlatFrame;            // Is frame flat?
	BOOL m_bAutoSizeWindow;       // Auto-resize tab widnows
	BOOL m_bTransparent;
	BOOL m_bTopEdge;
	BOOL m_bDrawFrame;            // Draw frame around window area
	BOOL m_bHideNoTabs;           // Hide tabs are when no tabs are available
	BOOL m_bIsActiveTabBold;      // Active tab text is drawing bold
	BOOL m_bActiveTabCloseButton; // Draw "Close" button on the active tab
	BOOL m_bTabDocumentsMenu;
	BOOL m_bHiddenDocuments;
	BOOL m_bIsActiveInMDITabGroup;
	BOOL m_bResize;

	int m_nScrollBarRight;  // Scrollbar right side
	int m_nTabsHorzOffset;
	int m_nTabsHorzOffsetMax;
	int m_nTabsTotalWidth;
	int m_nFirstVisibleTab;
	int m_nHorzScrollWidth; // Shared scrollbar width
	int m_nTabMaxWidth;

	CRect m_rectTabSplitter; // Splitter between tabs and scrollbar
	CRect m_rectCurrClip;
	CRect m_rectTabsArea;    // Tabs area
	CRect m_rectWndArea;     // Child window area
	CRect m_rectResize;
	CRect m_rectResizeDrag;
	CRect m_rectResizeBounds;

	CScrollBar m_wndScrollWnd; // Active window horizontal scroll bar
	CMFCTabButton m_btnScrollLeft;
	CMFCTabButton m_btnScrollRight;
	CMFCTabButton m_btnScrollFirst;
	CMFCTabButton m_btnScrollLast;
	CMFCTabButton m_btnClose;
	ResizeMode m_ResizeMode;

	CList<HWND, HWND> m_lstButtons;

	AFX_IMPORT_DATA static CMap<UINT,UINT,HICON,HICON> m_mapDocIcons;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM, LPARAM lParam, LRESULT* pResult);
	virtual void Draw3DTab(CDC* pDC, CMFCTabInfo* pTab, BOOL bActive);
	virtual void DrawFlatTab(CDC* pDC, CMFCTabInfo* pTab, BOOL bActive);
	virtual void DrawResizeDragRect(CRect& rectNew, CRect& rectOld);

// Implementation
public:
	virtual ~CMFCTabCtrl();

protected:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CMFCTabCtrl notification messages:
extern AFX_IMPORT_DATA UINT AFX_WM_ON_HSCROLL;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#endif // NO_TAB_WND_INFO
