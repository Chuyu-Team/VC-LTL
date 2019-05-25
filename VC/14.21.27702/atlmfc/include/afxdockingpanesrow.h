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

class CDockSite;
class CPane;

/*============================================================================*/
// CDockSite window

enum AFX_ROW_ALIGNMENT
{
	AFX_ROW_ALIGN_TOP = 0,
	AFX_ROW_ALIGN_BOTTOM = 1
};

class CDockingPanesRow : public CObject
{
	DECLARE_DYNAMIC(CDockingPanesRow);

public:
	CDockingPanesRow(CDockSite* pParentDockBar, int nOffset, int nHeight);
	virtual ~CDockingPanesRow();

	virtual BOOL Create() { return TRUE; }

	virtual void GetAvailableSpace(CRect& rect);
	virtual int  GetVisibleCount();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual void AddPane(CPane* pControlBar, AFX_DOCK_METHOD dockMethod, LPCRECT lpRect = NULL, BOOL bAddLast = FALSE);
	virtual void AddPaneFromRow(CPane* pControlBar, AFX_DOCK_METHOD dockMethod);
	virtual void RemovePane(CPane* pControlBar);

	virtual void ArrangePanes(int nMargin, int nSpacing);

	virtual int  ResizeByPaneDivider(int /*nOffset*/) { return 0; } // can'te be resized by slider
	virtual int  Resize(int nOffset);
	virtual void Move(int nOffset);

	virtual void ArrangePanes(CPane* pInitialBar);
	virtual void RepositionPanes(CRect& rectNewParentBarArea, UINT nSide = (UINT)-1, BOOL bExpand = FALSE, int nOffset = 0);
	virtual BOOL IsEmpty() const { return m_lstControlBars.IsEmpty(); }

	virtual void UpdateVisibleState(BOOL bDelay);
	virtual void ShowDockSiteRow(BOOL bShow, BOOL bDelay);
	
	void MovePane(CPane* pControlBar, CPoint ptOffset, BOOL bSwapControlBars, HDWP& hdwp);
	void MovePane(CPane* pControlBar, CRect rectTarget, HDWP& hdwp);
	void MovePane(CPane* pControlBar, int nOffset, bool bForward, HDWP& hdwp);
	void MovePane(CPane* pControlBar, int nAbsolutOffset, HDWP& hdwp);
	void RedrawAll();

	BOOL HasPane(CBasePane* pControlBar) { return m_lstControlBars.Find(pControlBar) != NULL; }

	virtual BOOL ReplacePane(CPane* pBarOld, CPane* pBarNew);
	virtual BOOL ShowPane(CPane* pControlBar, BOOL bShow, BOOL bDelay = FALSE);

	virtual void OnResizePane(CBasePane* pControlBar);

	void FixupVirtualRects(bool bMoveBackToVirtualRect, CPane* pBarToExclude = NULL);
	void ExpandStretchedPanes();
	void ExpandStretchedPanesRect();

	virtual BOOL IsVisible() const { return m_bVisible ;}

	int GetRowHeight() const { return m_nRowHeight; }
	int GetRowOffset() const { return m_nRowOffset; }

	int GetPaneCount() const { return (int) m_lstControlBars.GetCount(); }
	void GetWindowRect(CRect& rect) const;
	void GetClientRect(CRect& rect) const;
	void ScreenToClient(CRect& rect) const;

	bool IsHorizontal() const { return (m_dwRowAlignment & CBRS_ALIGN_TOP || m_dwRowAlignment & CBRS_ALIGN_BOTTOM); }

	DWORD GetRowAlignment() const { return m_dwRowAlignment; }
	int GetID() const { return 0; }
	CDockSite* GetDockSite() const { return m_pParentDockBar; }
	const CObList& GetPaneList() const { return m_lstControlBars; }

	virtual int GetAvailableLength(BOOL bUseVirtualRect = FALSE) const;

	void SetExtra(int nExtraSpace, AFX_ROW_ALIGNMENT rowExtraAlign)
	{
		m_nExtraSpace = nExtraSpace;
		m_nExtraAlignment = rowExtraAlign;
	}
	int GetExtraSpace() const { return m_nExtraSpace; }

	// exclusive row does not allows more than one bar that does allow sibling bars
	virtual BOOL IsExclusiveRow() const;

	void GetGroupFromPane(CPane* pBar, CObList& lst);
	int  GetMaxPaneSize(BOOL bSkipHiddenBars = TRUE) const;

	BOOL m_bIgnoreBarVisibility;

protected:
	BOOL m_bVisible;
	int  m_nRowHeight;
	int  m_nRowSavedHeight;
	int  m_nMinHeight;
	int  m_nRowOffset;
	int  m_nExtraSpace; // specifies how the row is bigger than its bars
	int  m_nExtraAlignment; // how the bars are aligned if extra space
	CObList         m_lstControlBars;
	DWORD           m_dwRowAlignment;
	CDockSite* m_pParentDockBar;

	BOOL CheckPanes(CRect& rectCurrentBar, CPane* pCurrentBar, bool bForward, CPoint ptOffset, BOOL bSwapControlBars, HDWP& hdwp);
	CPane* FindFirstVisiblePane(BOOL bForward);

protected:
	virtual void OnInsertPane(CPane* pControlBar);
	void AdjustPaneToRowArea(CPane* pBar, const CRect& rectRow, HDWP& hdwp);
	int CalcLastPaneOffset();

	void BeginTrans();
	void CommitTrans();
	int GetAvailableLengthRect();
	int StretchPaneRect(CPane* pBar, int nStretchSize);
	void ArrangePanesRect(CPane* pInitialBar);
	void ResolveIntersectionRect(CPane* pBar, bool bForward);
	int GetOutOfBoundsOffsetRect(CPane* pBar, BOOL bLeftTopBound);
	void ShiftPanesRect(CPane* pControlBar, int nOffset, BOOL bForward);
	void MovePaneRect(CPane* pControlBar, int nOffset, bool bForward);
	
private:
	void OffsetFromRect(const CRect& rect, CPoint& pt, bool bForward);
	BOOL IsEnoughSpaceToMove(CPane* pControlBar, bool bForward, int& nAllowedOffset);
	void MoveTrailingPanes(POSITION posStart, CPoint ptOffset, bool bForward, CPane* pBarToSkip, HDWP& hdwp);
	void ResolveIntersection(CPane* pBar, bool bForward, HDWP& hdwp);
	int  GetOutOfBoundsOffset(CPane* pBar, BOOL bLeftTopBound);
	void ShiftPanes(CPane* pControlBar, int nOffset, BOOL bForward);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
