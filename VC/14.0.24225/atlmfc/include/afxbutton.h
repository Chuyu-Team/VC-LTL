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
#include "afxmenuimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCButton window

class CMFCButton : public CButton
{
	DECLARE_DYNCREATE(CMFCButton)

// Construction
public:
	CMFCButton();

// Attributes
public:
	enum FlatStyle
	{
		BUTTONSTYLE_3D,
		BUTTONSTYLE_FLAT,
		BUTTONSTYLE_SEMIFLAT,
		BUTTONSTYLE_NOBORDERS
	};

	enum AlignStyle
	{
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};
	
	FlatStyle  m_nFlatStyle;
	AlignStyle m_nAlignStyle;

	BOOL m_bRightImage; // Is image on the right side?
	BOOL m_bTopImage;  // Is image on top?
	BOOL m_bTransparent;
	BOOL m_bDrawFocus;
	BOOL m_bHighlightChecked;
	BOOL m_bGrayDisabled;
	BOOL m_bResponseOnButtonDown;
	BOOL m_bDontUseWinXPTheme;
	BOOL m_bWasDblClk;

	CToolTipCtrl& GetToolTipCtrl()
	{
		if (m_pToolTip == NULL)
		{
			static CToolTipCtrl _afxEmptyToolTip;
			return _afxEmptyToolTip;
		}
		return (*m_pToolTip);
	}

	BOOL IsPressed() const { return m_bPushed && m_bHighlighted; }
	BOOL IsPushed() const { return m_bPushed; }
	BOOL IsHighlighted() const { return m_bHighlighted; }
	BOOL IsCheckBox() const { return m_bCheckButton; }
	BOOL IsRadioButton() const { return m_bRadioButton; }
	BOOL IsAutoCheck() const { return m_bAutoCheck; }
	BOOL IsChecked() const { return  m_bChecked; }

protected:
	BOOL m_bPushed;
	BOOL m_bClickiedInside;
	BOOL m_bHighlighted;
	BOOL m_bCaptured;
	BOOL m_bHover;
	BOOL m_bChecked;
	BOOL m_bCheckButton;
	BOOL m_bRadioButton;
	BOOL m_bAutoCheck;
	BOOL m_bAutoDestroyImage;
	BOOL m_bFullTextTooltip;
	BOOL m_bDelayFullTextTooltipSet;
	int  m_nAutoRepeatTimeDelay;

	CSize m_sizeImage;
	CSize m_sizePushOffset;

	CMFCToolBarImages m_Image;
	CMFCToolBarImages m_ImageHot;
	CMFCToolBarImages m_ImageDisabled;
	CMFCToolBarImages m_ImageChecked;
	CMFCToolBarImages m_ImageCheckedHot;
	CMFCToolBarImages m_ImageCheckedDisabled;

	CMenuImages::IMAGES_IDS  m_nStdImageId;
	CMenuImages::IMAGES_IDS  m_nStdImageDisabledId;
	CMenuImages::IMAGE_STATE m_StdImageState;

	CToolTipCtrl* m_pToolTip;

	HFONT   m_hFont;
	HCURSOR m_hCursor;

	COLORREF m_clrRegular;
	COLORREF m_clrHover;
	COLORREF m_clrFace;

	AFX_IMPORT_DATA static BOOL m_bWinXPTheme;
	AFX_IMPORT_DATA static BOOL m_bWinXPThemeWasChecked;

// Operations
public:
	void SetImage(HICON hIcon, BOOL bAutoDestroy = TRUE, HICON hIconHot = NULL, HICON hIconDisabled = NULL, BOOL bAlphaBlend = FALSE);
	void SetImage(HBITMAP hBitmap, BOOL bAutoDestroy = TRUE, HBITMAP hBitmapHot = NULL, BOOL bMap3dColors = TRUE, HBITMAP hBitmapDisabled = NULL);
	void SetImage(UINT uiBmpResId, UINT uiBmpHotResId = 0, UINT uiBmpDsblResID = 0);

	void SetCheckedImage(HICON hIcon, BOOL bAutoDestroy = TRUE, HICON hIconHot = NULL, HICON hIconDisabled = NULL, BOOL bAlphaBlend = FALSE);
	void SetCheckedImage(HBITMAP hBitmap, BOOL bAutoDestroy = TRUE, HBITMAP hBitmapHot = NULL, BOOL bMap3dColors = TRUE, HBITMAP hBitmapDisabled = NULL);
	void SetCheckedImage(UINT uiBmpResId, UINT uiBmpHotResId = 0, UINT uiBmpDsblResID = 0);

	void SetStdImage(CMenuImages::IMAGES_IDS id, CMenuImages::IMAGE_STATE state = CMenuImages::ImageBlack, CMenuImages::IMAGES_IDS idDisabled = (CMenuImages::IMAGES_IDS) 0);

	void SetTooltip(LPCTSTR lpszToolTipText); // lpszToolTip == NULL: disable tooltip

	void SetMouseCursor(HCURSOR hcursor);
	void SetMouseCursorHand();

	void SetTextColor(COLORREF clrText) { m_clrRegular = clrText; }
	void SetTextHotColor(COLORREF clrTextHot) { m_clrHover = clrTextHot; }

	void SetFaceColor(COLORREF crFace, BOOL bRedraw = TRUE);

	void EnableMenuFont(BOOL bOn = TRUE, BOOL bRedraw = TRUE);
	void EnableFullTextTooltip(BOOL bOn = TRUE);

	void SetAutorepeatMode(int nTimeDelay = 500 /* ms */);
	BOOL IsAutorepeatCommandMode() const { return m_nAutoRepeatTimeDelay > 0; }

	// Allow to draw button borders using current WinXP theme.
	// CMFCVisualManagerWindows should be active visual manager:
	static void __stdcall EnableWindowsTheming(BOOL bEnable = TRUE);
	static BOOL __stdcall IsWindowsThemingEnabled() { return m_bWinXPTheme; }

// Overrides
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMFCButton();
	virtual void CleanUp();

	virtual CSize SizeToContent(BOOL bCalcOnly = FALSE);
	virtual void OnDrawParentBackground(CDC* pDC, CRect rectClient);

protected:
	virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
	virtual void OnDrawFocusRect(CDC* pDC, const CRect& rectClient);
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);

	virtual void OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState);

	virtual CFont* SelectFont(CDC* pDC);

	virtual int GetImageHorzMargin() const;
	virtual int GetVertMargin() const;

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCancelMode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg LRESULT OnGetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnSetImage(WPARAM, LPARAM);
	afx_msg LRESULT OnGetImage(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void InitStyle(DWORD dwStyle);

	BOOL CheckNextPrevRadioButton(BOOL bNext);
	void UncheckRadioButtonsInGroup();

	void SetImageInternal(HICON hIcon, BOOL bAutoDestroy, HICON hIconHot, BOOL bChecked, HICON hIconDisabled = NULL, BOOL bAlphaBlend = FALSE);
	void SetImageInternal(HBITMAP hBitmap, BOOL bAutoDestroy, HBITMAP hBitmapHot, BOOL bMap3dColors, BOOL bChecked, HBITMAP hBitmapDisabled = NULL);
	void SetImageInternal(UINT uiBmpResId, UINT uiBmpHotResId, BOOL bChecked, UINT uiBmpDsblResId = 0);

	void ClearImages(BOOL bChecked);
	void DrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
