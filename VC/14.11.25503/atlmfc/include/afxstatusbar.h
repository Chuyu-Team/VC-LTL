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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/////////////////////////////////////////////////////////////////////////////

class CMFCStatusBarPaneInfo
{
public:	
	RECT        rect;          // pane rectangle
	UINT        nID;           // IDC of indicator: 0 => normal text area
	UINT        nStyle;        // style flags (SBPS_*)
	int         cxText;        // width of string area in pixels (on both sides there is a 1 pixel gap and a one pixel border, making a pane 4 pixels wider)
	int         cxIcon;        // width of icon area
	int         cyIcon;        // height of icon area
	COLORREF    clrText;       // text color
	COLORREF    clrBackground; // background color
	LPCTSTR     lpszText;      // text in the pane
	LPCTSTR     lpszToolTip;   // pane tooltip
	HIMAGELIST  hImage;        // pane icon or animation

	// Animation parameters
	int         nFrameCount;   // Number of animation frames
	int         nCurrFrame;    // Current frame

	// Progress bar properties
	long        nProgressCurr; // Current progress value
	long        nProgressTotal;// Total progress value (-1 - no progress bar)
	BOOL        bProgressText; // Display text: "x%"
	COLORREF    clrProgressBar;
	COLORREF    clrProgressBarDest;
	COLORREF    clrProgressText;

	CMFCStatusBarPaneInfo()
	{
		nID = 0;
		nStyle = 0;
		lpszText = NULL;
		lpszToolTip = NULL;
		clrText = (COLORREF)-1;
		clrBackground = (COLORREF)-1;
		hImage = NULL;
		cxIcon = 0;
		cyIcon = 0;
		rect = CRect(0, 0, 0, 0);
		nFrameCount = 0;
		nCurrFrame = 0;
		nProgressCurr = 0;
		nProgressTotal = -1;
		clrProgressBar = (COLORREF)-1;
		clrProgressBarDest = (COLORREF)-1;
		clrProgressText = (COLORREF)-1;
		bProgressText = FALSE;
	}
};

/*============================================================================*/
// CMFCStatusBar window

class CMFCStatusBar : public CPane
{
	DECLARE_DYNAMIC(CMFCStatusBar)

// Construction
public:
	CMFCStatusBar();
	BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT nID = AFX_IDW_STATUS_BAR);
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, UINT nID = AFX_IDW_STATUS_BAR);
	BOOL SetIndicators(const UINT* lpIDArray, int nIDCount);

// Attributes
public: 
	virtual BOOL DoesAllowDynInsertBefore() const { return FALSE; }

	// standard control bar things
	int CommandToIndex(UINT nIDFind) const;
	UINT GetItemID(int nIndex) const;
	void GetItemRect(int nIndex, LPRECT lpRect) const;

	// specific to CStatusBar
	void GetPaneText(int nIndex, CString& s) const;
	CString GetPaneText(int nIndex) const;
	
	virtual BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE);

	void GetPaneInfo(int nIndex, UINT& nID, UINT& nStyle, int& cxWidth) const;
	void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth);
	UINT GetPaneStyle(int nIndex) const;
	void SetPaneStyle(int nIndex, UINT nStyle);
	CString GetTipText(int nIndex) const;
	void SetTipText(int nIndex, LPCTSTR pszTipText);

	int GetCount() const { return m_nCount; }

	// specific to CMFCStatusBar
	int GetPaneWidth(int nIndex) const;
	void SetPaneWidth(int nIndex, int cx);

	void SetPaneIcon(int nIndex, HICON hIcon, BOOL bUpdate = TRUE);
	void SetPaneIcon(int nIndex, HBITMAP hBmp, COLORREF clrTransparent = RGB(255, 0, 255), BOOL bUpdate = TRUE);
	void SetPaneAnimation(int nIndex, HIMAGELIST hImageList, UINT nFrameRate = 500 /* ms */, BOOL bUpdate = TRUE);
	void SetPaneTextColor(int nIndex, COLORREF clrText = (COLORREF)-1 /* Default */, BOOL bUpdate = TRUE);
	void SetPaneBackgroundColor(int nIndex, COLORREF clrBackground = (COLORREF)-1 /* Default */, BOOL bUpdate = TRUE);

	void EnablePaneProgressBar(int nIndex, long nTotal = 100 /* -1 - disable */, BOOL bDisplayText = FALSE, /* display "x%" */
		COLORREF clrBar = -1, COLORREF clrBarDest = -1 /* for gradient fill */, COLORREF clrProgressText = -1);
	void SetPaneProgress(int nIndex, long nCurr, BOOL bUpdate = TRUE);
	long GetPaneProgress(int nIndex) const;

	void InvalidatePaneContent(int nIndex);
	void EnablePaneDoubleClick(BOOL bEnable = TRUE);

	virtual BOOL GetExtendedArea(CRect& rect) const;

	void SetDrawExtendedArea(BOOL bSet = TRUE) { m_bDrawExtendedArea = bSet; }
	BOOL GetDrawExtendedArea() const { return m_bDrawExtendedArea; }

// Implementation
public:
	virtual ~CMFCStatusBar();
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HFONT m_hFont;
	int   m_cxSizeBox;        // style size box in corner
	CRect m_rectSizeBox;
	BOOL  m_bPaneDoubleClick;
	BOOL  m_bDrawExtendedArea;

	inline CMFCStatusBarPaneInfo* _GetPanePtr(int nIndex) const;
	CMFCStatusBarPaneInfo* HitTest(CPoint pt) const;

	HFONT GetCurrentFont() const;

// Overrides:
protected:
	virtual void OnDrawPane(CDC* pDC, CMFCStatusBarPaneInfo* pPane);
	virtual void RecalcLayout();
	virtual void DoPaint(CDC* pDC);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }
	virtual BOOL HideInPrintPreviewMode() const { return FALSE; }

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg int OnSetText(LPCTSTR lpszText);
	afx_msg int OnGetText(int nMaxChars, LPTSTR lpszText);
	afx_msg UINT OnGetTextLength();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
