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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBar;
class CMFCToolBarButton;
class CMFCToolBarImages;
class CMFCRibbonButton;

/*============================================================================*/
// CMFCToolTipInfo

class CMFCToolTipInfo
{
public:
	BOOL m_bBalloonTooltip;
	BOOL m_bDrawIcon;
	BOOL m_bDrawDescription;
	BOOL m_bRoundedCorners;	// Obsolete
	BOOL m_bBoldLabel;
	BOOL m_bDrawSeparator;
	BOOL m_bVislManagerTheme;
	int  m_nMaxDescrWidth;
	int  m_nGradientAngle;	// 0 - 360, -1 -default (vertical)
	COLORREF m_clrFill;
	COLORREF m_clrFillGradient;
	COLORREF m_clrText;
	COLORREF m_clrBorder;

	CMFCToolTipInfo()
	{
		m_bBalloonTooltip   = FALSE;
		m_bDrawIcon         = TRUE;
		m_bDrawDescription  = TRUE;
		m_bRoundedCorners   = FALSE;
		m_bBoldLabel        = TRUE;
		m_bDrawSeparator    = TRUE;
		m_bVislManagerTheme = FALSE;
		m_nMaxDescrWidth    = 200;
		m_nGradientAngle    = -1;
		m_clrFill           = (COLORREF)-1;
		m_clrFillGradient   = (COLORREF)-1;
		m_clrText           = (COLORREF)-1;
		m_clrBorder         = (COLORREF)-1;
	}

	CMFCToolTipInfo& operator= (CMFCToolTipInfo& src)
	{
		m_bBalloonTooltip   = src.m_bBalloonTooltip;
		m_bDrawIcon         = src.m_bDrawIcon;
		m_bDrawDescription  = src.m_bDrawDescription;
		m_bRoundedCorners   = src.m_bRoundedCorners;
		m_bBoldLabel        = src.m_bBoldLabel;
		m_bDrawSeparator    = src.m_bDrawSeparator;
		m_bVislManagerTheme = src.m_bVislManagerTheme;
		m_nMaxDescrWidth    = src.m_nMaxDescrWidth;
		m_nGradientAngle    = src.m_nGradientAngle;
		m_clrFill           = src.m_clrFill;
		m_clrFillGradient   = src.m_clrFillGradient;
		m_clrText           = src.m_clrText;
		m_clrBorder         = src.m_clrBorder;

		return *this;
	}
};

/*============================================================================*/
// CMFCToolTipCtrl window

class CMFCToolTipCtrl : public CToolTipCtrl
{
	DECLARE_DYNCREATE(CMFCToolTipCtrl)

// Construction
public:
	CMFCToolTipCtrl(CMFCToolTipInfo* pParams = NULL);

// Attributes
public:
	void SetFixedWidth(int nWidthRegular, int nWidthLargeImage)
	{
		m_nFixedWidthRegular = nWidthRegular;
		m_nFixedWidthWithImage = nWidthLargeImage;
	}

protected:
	CMFCToolBar* m_pToolBar;
	CMFCToolBarImages* m_pToolBarImages;
	CMFCToolBarButton* m_pHotButton;
	CMFCRibbonButton*  m_pRibbonButton;
	CMFCToolTipInfo  m_Params;

	int     m_nRibbonImageType;
	CString m_strDescription;
	CSize   m_sizeImage;
	CSize	m_sizeCornerRadius;
	CPoint  m_ptMargin;
	CPoint  m_ptLocation;

	int	m_nFixedWidthRegular;
	int	m_nFixedWidthWithImage;

// Operations
public:
	void SetParams(CMFCToolTipInfo* pParams); // NULL - default params
	const CMFCToolTipInfo& GetParams() const { return m_Params; }

	void SetLocation(CPoint pt);
	void SetHotRibbonButton(CMFCRibbonButton* pRibbonButton);

// Overrides
	virtual void SetDescription(const CString strDescription);

	virtual CSize GetIconSize();
	virtual void OnFillBackground(CDC* pDC, CRect rect, COLORREF& clrText, COLORREF& clrLine);

	virtual void OnDrawBorder(CDC* pDC, CRect rect, COLORREF clrLine);
	virtual BOOL OnDrawIcon(CDC* pDC, CRect rectImage);
	virtual CSize OnDrawLabel(CDC* pDC, CRect rect, BOOL bCalcOnly);
	virtual CSize OnDrawDescription(CDC* pDC, CRect rect, BOOL bCalcOnly);
	virtual void OnDrawSeparator(CDC* pDC, int x1, int x2, int y);

// Implementation
public:
	virtual ~CMFCToolTipCtrl();

protected:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShow(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPop(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

	void GetHotButton();
	int GetFixedWidth ();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
