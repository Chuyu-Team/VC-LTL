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

class CMFCAutoHideBar;
class CDockablePane;

#include "afxcontrolbarutil.h"
#include "afxautohidebar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCAutoHideButton : public CObject
{
	DECLARE_DYNCREATE(CMFCAutoHideButton)
public:
	CMFCAutoHideButton();
	virtual ~CMFCAutoHideButton();

	virtual BOOL Create(CMFCAutoHideBar* pParentBar, CDockablePane* pAutoHideWnd, DWORD dwAlignment);

	void ShowAttachedWindow(BOOL bShow);

	void BringToTop() {m_bTop = TRUE;}
	void Move(int nOffset);

	virtual void ShowButton(BOOL bShow); 
	virtual BOOL IsVisible() const {return m_bVisible;}

	/// <summary>
	/// Highlights auto hide button.</summary>
	/// <param name="bHighlight">Specifies the new auto hide button state: TRUE - button is highlighted, FALSE- button is not highlighted.</param>
	virtual void HighlightButton(BOOL bHighlight);
	/// <summary>
	/// Returns highlight state of auto hide button.</summary>
	/// <returns> Returns TRUE if auto hide button is highlighted; otherwise FALSE.</returns>
	virtual BOOL IsHighlighted() const { return m_bHighlighted; }

	BOOL IsTop() const {return m_bTop;}
	BOOL IsActive() const { return m_pParentBar != NULL && m_pParentBar->m_bActiveInGroup; }

	virtual void OnDraw(CDC* pDC);
	virtual void OnFillBackground(CDC* pDC, CRect rect);
	virtual void OnDrawBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize);

	virtual void UnSetAutoHideMode(CDockablePane* pFirstBarInGroup);

	CSize GetSize() const;
	CRect GetRect() const {return m_rect;}
	virtual CSize GetTextSize() const;

	BOOL IsHorizontal() const;

	DWORD GetAlignment() const { return m_dwAlignment; }

	CDockablePane* GetAutoHideWindow() const {return m_pAutoHideWindow;}
	CMFCAutoHideBar* GetParentToolBar() {return m_pParentBar;}

	void ReplacePane(CDockablePane* pNewBar);

	AFX_IMPORT_DATA static int m_nBorderSize;
	AFX_IMPORT_DATA static int m_nMarginSize;
	AFX_IMPORT_DATA static int m_nTextMargin;
	AFX_IMPORT_DATA static int m_nTextSizeNoIcon;

	AFX_IMPORT_DATA static BOOL m_bOverlappingTabs;

protected:
	BOOL  m_bTop;
	BOOL  m_bVisible;
	BOOL  m_bHighlighted;
	DWORD m_dwAlignment;
	int   m_nOffset;  // offset from the left bound of the parent bar for the horz buttons and from left bound for vert buttons

	CRect m_rect;
	CMFCAutoHideBar* m_pParentBar;
	CDockablePane* m_pAutoHideWindow;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
