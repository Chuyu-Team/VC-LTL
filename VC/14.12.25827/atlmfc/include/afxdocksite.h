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

class CDockingPanesRow;
class CDockingManager;

#pragma warning(push)
#pragma warning(disable : 4100 34)

class CDockSite : public CBasePane
{
	DECLARE_DYNCREATE(CDockSite)

// Construction
public:
	CDockSite();

// Attributes
public:

	bool IsLastRow(CDockingPanesRow* pRow) const;
	BOOL IsRectWithinDockSite(CRect rect, CPoint& ptDelta);
	virtual BOOL IsResizable() const {return FALSE;}
	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL IsDragMode() const;
	virtual BOOL IsAccessibilityCompatible() { return FALSE; }


	virtual BOOL DoesAllowDynInsertBefore() const { return FALSE; }
	const CObList& GetPaneList() const { return m_lstControlBars; }
	const CObList& GetDockSiteRowsList() const { return m_lstDockBarRows; }
	
// Operations
public:
	virtual CDockingPanesRow* CreateRow(CDockSite* pParentDockBar, int nOffset, int nRowHeight);

	virtual void DockPane(CPane* pWnd, AFX_DOCK_METHOD dockMethod, LPCRECT lpRect = NULL);
	virtual BOOL DockPaneLeftOf(CPane* pBarToDock, CPane* pTargetBar);

	virtual void RemovePane(CPane* pWnd, AFX_DOCK_METHOD dockMethod);

	virtual BOOL MovePane(CPane* pWnd, UINT nFlags, CPoint ptOffset);

	virtual void OnInsertRow(POSITION pos);
	virtual void OnRemoveRow(POSITION pos, BOOL bByShow = FALSE);
	virtual int  OnResizeRow(CDockingPanesRow* pRowToResize, int nOffset);
	virtual void OnShowRow(POSITION pos, BOOL bShow);

	virtual void OnSizeParent(CRect& rectAvailable, UINT nSide, BOOL bExpand, int nOffset) {}
	virtual void RepositionPanes(CRect& rectNewClientArea);
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	CDockingPanesRow* AddRow(POSITION pos, int nHeight);
	void RemoveRow(CDockingPanesRow* pRow);
	void ShowRow(CDockingPanesRow* pRow, BOOL bShow, BOOL bAdjustLayout);

	void ResizeDockSite(int nNewWidth, int nNewHeight);
	int ResizeRow(CDockingPanesRow* pRow, int nNewSize, BOOL bAdjustLayout = TRUE);

	void SwapRows(CDockingPanesRow* pFirstRow, CDockingPanesRow* pSecondRow);

	virtual void FixupVirtualRects();

	virtual BOOL ShowPane(CBasePane* pBar, BOOL bShow, BOOL bDelay, BOOL bActivate);

	virtual CPane* PaneFromPoint(CPoint pt);

	BOOL ReplacePane(CPane* pOldBar, CPane* pNewBar);
	void AlignDockSite(const CRect& rectToAlignBy, CRect& rectResult, BOOL bMoveImmediately);

	int FindRowIndex(CDockingPanesRow* pRow);

	virtual void AdjustLayout();
	virtual void AdjustDockingLayout();
	virtual UINT GetDockSiteID() const {return m_nDockBarID;}

	CPane* FindPaneByID(UINT nID);

public:
	static int __stdcall RectSideFromPoint(const CRect& rect, const CPoint& point);
	virtual BOOL OnSetWindowPos(const CWnd* pWndInsertAfter, const CRect& rectWnd, UINT nFlags);

// Overrides
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, DWORD dwControlBarStyle, CCreateContext* pContext = NULL);
	virtual BOOL CreateEx(DWORD dwStyleEx, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, DWORD dwControlBarStyle, CCreateContext* pContext = NULL);

protected:
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);

// Implementation
public:
	virtual ~CDockSite();

protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNcDestroy();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

	CDockingPanesRow* RowFromPoint(CPoint pt, bool& bOuterRow) const;
	CDockingPanesRow* RowFromPane(CBasePane* pBar) const;
	CDockingPanesRow* FindNextVisibleRow(POSITION& pos, BOOL bForward = TRUE);
	void ResizeDockSiteByOffset(int nOffset, BOOL bAdjustLayout = TRUE);
	CDockingPanesRow* FindRowByRect(CRect rectRow);

protected:
	CObList m_lstControlBars;
	CObList m_lstDockBarRows;
	UINT    m_nDockBarID; // for compatibility with MFC
};

#pragma warning(pop)

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
