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

/*============================================================================*/
// CMFCHeaderCtrl window

class CMFCHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CMFCHeaderCtrl)

// Construction
public:
	CMFCHeaderCtrl();

// Attributes
public:
	int GetSortColumn() const;
	BOOL IsAscending() const;
	int GetColumnState(int iColumn) const; // Returns: 0 - not not sorted, -1 - descending, 1 - ascending

	BOOL IsMultipleSort() const { return m_bMultipleSort; }
	BOOL IsDialogControl() const { return m_bIsDlgControl; }

protected:
	CMap<int,int,int,int> m_mapColumnsStatus; // -1, 1, 0
	BOOL  m_bIsMousePressed;
	BOOL  m_bMultipleSort;
	BOOL  m_bAscending;
	BOOL  m_bTracked;
	BOOL  m_bIsDlgControl;
	int   m_nHighlightedItem;
	HFONT m_hFont;

// Operations
public:
	void SetSortColumn(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	void RemoveSortColumn(int iColumn);
	void EnableMultipleSort(BOOL bEnable = TRUE);

// Overrides
protected:
	virtual void PreSubclassWindow();
	virtual void OnDrawItem(CDC* pDC, int iItem, CRect rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnFillBackground(CDC* pDC);
	virtual void OnDrawSortArrow(CDC* pDC, CRect rectArrow);

// Implementation
public:
	virtual ~CMFCHeaderCtrl();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseLeave();
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	DECLARE_MESSAGE_MAP()

	void CommonInit();
	CFont* SelectFont(CDC *pDC);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
