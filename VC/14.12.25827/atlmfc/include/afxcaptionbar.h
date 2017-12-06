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

#include "afxbutton.h"
#include "afxtoolbarimages.h"
#include "afxpane.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCCaptionBar : public CPane
{
	DECLARE_DYNCREATE(CMFCCaptionBar)

public:
	CMFCCaptionBar();
	virtual ~CMFCCaptionBar();

	BOOL Create(DWORD dwStyle, CWnd* pParentWnd, UINT uID, int nHeight = -1, BOOL bIsMessageBarMode = FALSE);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysColorChange();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg void OnMouseLeave();
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

//Attributes
public:
	enum BarElement
	{
		ELEM_BUTTON,
		ELEM_TEXT,
		ELEM_ICON
	};

	enum BarElementAlignment
	{
		ALIGN_INVALID,
		ALIGN_LEFT,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	BOOL IsMessageBarMode() const { return m_bIsMessageBarMode; }
	void SetFlatBorder(BOOL bFlat = TRUE) { m_bFlatBorder = bFlat; }

	void SetButton(LPCTSTR lpszLabel, UINT uiCmdUI, BarElementAlignment btnAlignment = ALIGN_LEFT, BOOL bHasDropDownArrow = TRUE);
	void SetButtonToolTip(LPCTSTR lpszToolTip, LPCTSTR lpszDescription = NULL);
	void RemoveButton();
	void EnableButton(BOOL bEnable = TRUE);
	void SetButtonPressed(BOOL bPresed = TRUE);

	void SetIcon(HICON hIcon, BarElementAlignment iconAlignment = ALIGN_RIGHT);
	void RemoveIcon();

	void SetBitmap(HBITMAP hBitmap, COLORREF clrTransparent, BOOL bStretch = FALSE, BarElementAlignment bmpAlignment = ALIGN_RIGHT);
	void SetBitmap(UINT uiBmpResID, COLORREF clrTransparent, BOOL bStretch = FALSE, BarElementAlignment bmpAlignment = ALIGN_RIGHT);
	void RemoveBitmap();
	void SetImageToolTip(LPCTSTR lpszToolTip, LPCTSTR lpszDescription = NULL);

	void SetText(const CString& strText, BarElementAlignment textAlignment = ALIGN_RIGHT);
	void RemoveText();

	void SetBorderSize(int nSize) { m_nBorderSize = nSize; }
	int  GetBorderSize() const { return m_nBorderSize; }

	void SetMargin(int nMargin) { m_nMargin = nMargin; }
	int  GetMargin() const { return m_nMargin; }

	BarElementAlignment GetAlignment(BarElement elem);

	COLORREF m_clrBarText;
	COLORREF m_clrBarBackground;
	COLORREF m_clrBarBorder;

	CRect GetButtonRect() const { return m_rectButton; }

	virtual BOOL DoesAllowDynInsertBefore() const {return FALSE;}

protected:
// Overridables
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void OnDrawBackground(CDC* pDC, CRect rect);
	virtual void OnDrawBorder(CDC* pDC, CRect rect);
	virtual void OnDrawText(CDC* pDC, CRect rect, const CString& strText);
	virtual void OnDrawImage(CDC* pDC, CRect rect);
	virtual void OnDrawButton(CDC* pDC, CRect rect, const CString& strButton, BOOL bEnabled);

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual CSize GetTextSize(CDC* pDC, const CString& strText);

	virtual void AdjustLayout();
	virtual void RecalcLayout();

	virtual BOOL OnShowControlBarMenu(CPoint /*point*/) { return FALSE; }

	BOOL m_bIsMessageBarMode;
	CToolTipCtrl* m_pToolTip;

	//------------------
	// Image attributes:
	//------------------
	HICON               m_hIcon;
	CMFCToolBarImages      m_Bitmap;
	BOOL                m_bStretchImage;
	BarElementAlignment m_iconAlignment;
	CRect               m_rectImage;
	CString             m_strImageToolTip;
	CString             m_strImageDescription;

	//-----------------------
	// Text label attributes:
	//-----------------------
	HFONT               m_hFont;
	CString             m_strText;
	CStringArray        m_arTextParts;
	BarElementAlignment m_textAlignment;
	CRect               m_rectText;
	CRect               m_rectDrawText;
	BOOL                m_bTextIsTruncated;

	//-------------------
	// Button attributes:
	//-------------------
	CString             m_strBtnText;
	CString             m_strButtonToolTip;
	CString             m_strButtonDescription;
	UINT                m_uiBtnID;
	BarElementAlignment m_btnAlignnment;
	CRect               m_rectButton;
	BOOL                m_bIsBtnPressed;
	BOOL                m_bIsBtnHighlighted;
	BOOL                m_bIsBtnForcePressed;
	BOOL                m_bTracked;
	BOOL                m_bBtnEnabled;
	BOOL                m_bBtnHasDropDownArrow;
	BOOL                m_bFlatBorder;

	int                 m_nBorderSize;
	int                 m_nMargin;
	int                 m_nHorzElementOffset;
	int                 m_nDefaultHeight;
	int                 m_nCurrentHeight;

	//-------------------------
	// Close Button attributes:
	//-------------------------
	BOOL                m_bIsCloseBtnPressed;
	BOOL                m_bIsCloseBtnHighlighted;
	BOOL                m_bCloseTracked;
	CRect               m_rectClose;

	CSize GetImageSize() const;

	BOOL CheckRectangle(CRect& rectSrc, const CRect& rectOther, BOOL bLeftOf);
	void AdjustRectToMargin(CRect& rectSrc, const CRect& rectClient, int nMargin, BOOL bRetainSize = FALSE);

	void UpdateTooltips();

	BOOL IsImageSet() const { return (m_hIcon != NULL || m_Bitmap.GetCount() > 0); }
};



#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
