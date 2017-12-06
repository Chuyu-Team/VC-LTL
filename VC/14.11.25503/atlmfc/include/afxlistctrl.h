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
#include "afxheaderctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCListCtrl window

class CMFCListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMFCListCtrl)

// Construction
public:
	CMFCListCtrl();

// Attributes
public:
	virtual CMFCHeaderCtrl& GetHeaderCtrl() { return m_wndHeader; }

	// Mark sorted column by background color
	void EnableMarkSortedColumn(BOOL bMark = TRUE, BOOL bRedraw = TRUE);

protected:
	CMFCHeaderCtrl m_wndHeader;
	COLORREF m_clrSortedColumn;
	int      m_iSortedColumn;
	BOOL     m_bAscending;
	BOOL     m_bMarkSortedColumn;
	HFONT    m_hOldFont;

// Operations
public:
	// Sorting operations:
	virtual void Sort(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	void SetSortColumn(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	void RemoveSortColumn(int iColumn);
	void EnableMultipleSort(BOOL bEnable = TRUE);
	BOOL IsMultipleSort() const;

// Overrides
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);

	// Support for individual cells text/background colors:
	virtual COLORREF OnGetCellTextColor(int /*nRow*/, int /*nColum*/) { return GetTextColor(); }
	virtual COLORREF OnGetCellBkColor(int /*nRow*/, int /*nColum*/) { return GetBkColor(); }
	virtual HFONT OnGetCellFont(int /*nRow*/, int /*nColum*/, DWORD /*dwData*/ = 0) { return NULL; }

protected:
	virtual void PreSubclassWindow();

// Implementation
public:
	virtual ~CMFCListCtrl();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);

	DECLARE_MESSAGE_MAP()

	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	BOOL InitList();
	void InitColors();

	virtual void InitHeader();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
