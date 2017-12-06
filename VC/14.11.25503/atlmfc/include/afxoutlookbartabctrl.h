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
#include "afxribbonres.h"
#include "afxbasetabctrl.h"
#include "afxbutton.h"
#include "afxtoolbar.h"
#include "afxtoolbarslistcheckbox.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCOutlookBarTabCtrl;

class CMFCOutlookBarScrollButton : public CMFCButton
{
	virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
};

class CMFCOutlookBarToolBar : public CMFCToolBar
{
	friend class CMFCOutlookBarTabCtrl;

	DECLARE_DYNAMIC(CMFCOutlookBarToolBar)

	CMFCOutlookBarToolBar(CMFCOutlookBarTabCtrl* pParentBar);

	virtual BOOL LoadState(LPCTSTR /*lpszProfileName*/ = NULL, int /*nIndex*/ = -1, UINT /*uiID*/ = (UINT) -1)	{	return TRUE;	}
	virtual BOOL SaveState(LPCTSTR /*lpszProfileName*/ = NULL, int /*nIndex*/ = -1, UINT /*uiID*/ = (UINT) -1)	{	return TRUE;	}

	virtual BOOL OnSendCommand(const CMFCToolBarButton* pButton);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	virtual BOOL OnUserToolTip(CMFCToolBarButton* pButton, CString& strTTText) const;

	virtual BOOL IsTabbed() const { return FALSE; }

	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }
	virtual BOOL AllowShowOnList() const { return FALSE; }
	virtual void AdjustLocations();
	virtual void OnCustomizeMode(BOOL bSet);

	CMap<int, int, int, int> m_TabButtons;
	CMFCOutlookBarTabCtrl*             m_pParentBar;

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CMFCOutlookBarTabCtrl window

class CMFCOutlookBarTabCtrl : public CMFCBaseTabCtrl
{
	friend class COutlookOptionsDlg;

	DECLARE_DYNCREATE(CMFCOutlookBarTabCtrl)

// Construction
public:
	CMFCOutlookBarTabCtrl();

// Attributes
public:
	virtual BOOL IsPtInTabArea(CPoint point) const;
	virtual void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const;

	int GetBorderSize() const { return m_nBorderSize; }
	void SetBorderSize(int nBorderSize);

	static void __stdcall EnableAnimation(BOOL bEnable = TRUE);
	static BOOL __stdcall IsAnimation() { return m_bEnableAnimation; }

	void SetPageButtonTextAlign(UINT uiAlign, BOOL bRedraw = TRUE); // TA_LEFT, TA_CENTER or TA_RIGHT

	BOOL IsMode2003() const;

	int GetVisiblePageButtons() const { return m_nVisiblePageButtons; }
	void SetVisiblePageButtons(int nVisiblePageButtons) { m_nVisiblePageButtons = nVisiblePageButtons; }

	virtual void OnShowMorePageButtons();
	virtual void OnShowFewerPageButtons();
	virtual BOOL CanShowMorePageButtons() const;
	virtual BOOL CanShowFewerPageButtons() const;
	virtual void OnShowOptions();

	virtual void OnChangeTabs();

	BOOL SetToolbarImageList(UINT uiID, int cx, COLORREF clrTransp = RGB(255, 0, 255));

// Operations
public:
	void AddControl(CWnd* pWndCtrl, LPCTSTR lpszName, int nImageID = -1, BOOL bDetachable = TRUE,
		DWORD dwControlBarStyle = AFX_CBRS_FLOAT | AFX_CBRS_CLOSE | AFX_CBRS_RESIZE | AFX_CBRS_AUTOHIDE);

	virtual void RecalcLayout();
	virtual void EnableInPlaceEdit(BOOL bEnable)
	{
		ASSERT_VALID(this);
		m_bIsInPlaceEdit = bEnable;
	}

	virtual void CalcRectEdit(CRect& rectEdit)
	{
		ASSERT_VALID(this);
		rectEdit.DeflateRect(1, 1);
	}

	void EnableScrollButtons(BOOL bEnable = TRUE, BOOL bIsUp = TRUE, BOOL bIsDown = TRUE);

// Overrides
	virtual BOOL IsTabDetachable(int iTab) const;

public:
	virtual BOOL Create(const CRect& rect, CWnd* pParentWnd, UINT nID);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

// Implementation
public:
	virtual ~CMFCOutlookBarTabCtrl();

	virtual BOOL SetActiveTab(int iTab);
	virtual CWnd* FindTargetWnd(const CPoint& pt);

protected:
	virtual int GetTabNumberToDetach(int nTabNum = -1) const;

	void DrawTabButton(CDC& dc, int iButtonIdx, BOOL bDrawPressedButton = TRUE);
	void RebuildToolBar();

	virtual void OnScrollUp() {}
	virtual void OnScrollDown() {}

	// Generated message map functions
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnToolbarCommand(UINT id);
	afx_msg void OnUpdateToolbarCommand(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

	int   m_nBorderSize;
	int   m_nVisiblePageButtons;
	int   m_nMaxVisiblePageButtons;
	BOOL  m_bIsTracking;
	BOOL  m_bDontAdjustLayout;
	BOOL  m_bScrollButtons;
	UINT  m_nPageButtonTextAlign;
	CRect m_rectWndArea;
	CRect m_rectCaption;
	CRect m_rectSplitter;
	CSize m_sizeToolbarImage;

	CImageList           m_imagesToolbar;
	CMFCOutlookBarToolBar   m_wndToolBar;
	CMFCOutlookBarScrollButton m_btnUp;
	CMFCOutlookBarScrollButton m_btnDown;

	AFX_IMPORT_DATA static BOOL m_bEnableAnimation;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
