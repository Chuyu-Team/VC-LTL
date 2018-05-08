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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCImageEditorDialog;

/*============================================================================*/
// CMFCImagePaintArea window

class CMFCImagePaintArea : public CButton
{
// Construction
public:
	CMFCImagePaintArea(CMFCImageEditorDialog* pParentDlg);

// Attributes
public:
	enum IMAGE_EDIT_MODE
	{
		IMAGE_EDIT_MODE_PEN = 0,
		IMAGE_EDIT_MODE_FILL,
		IMAGE_EDIT_MODE_LINE,
		IMAGE_EDIT_MODE_RECT,
		IMAGE_EDIT_MODE_ELLIPSE,
		IMAGE_EDIT_MODE_COLOR
	};

	CRect     m_rectParentPreviewArea;

protected:
	COLORREF  m_rgbColor;
	CDC       m_memDC;
	CSize     m_sizeImage;
	CBitmap*  m_pBitmap;
	HCURSOR   m_hcurPen;
	HCURSOR   m_hcurFill;
	HCURSOR   m_hcurLine;
	HCURSOR   m_hcurRect;
	HCURSOR   m_hcurEllipse;
	HCURSOR   m_hcurColor;
	CSize     m_sizeCell;
	CPen      m_penDraw;
	CPen      m_penStretch;
	CRect     m_rectDraw;

	CMFCImageEditorDialog*  m_pParentDlg;
	IMAGE_EDIT_MODE m_Mode;

// Operations
public:
	void SetBitmap(CBitmap* pBitmap);
	void SetColor(COLORREF color);
	void SetMode(IMAGE_EDIT_MODE mode)
	{
		m_Mode = mode;
	}

	IMAGE_EDIT_MODE GetMode() const
	{
		return m_Mode;
	}

protected:
	void ScreenToBitmap(CPoint& point);
	void BitmapToClient(CRect& rect);

	void FloodFill(const CPoint& point);

// Overrides
protected:
	virtual void PreSubclassWindow();

// Implementation
public:
	virtual ~CMFCImagePaintArea();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	void DrawPixel(POINT point);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
