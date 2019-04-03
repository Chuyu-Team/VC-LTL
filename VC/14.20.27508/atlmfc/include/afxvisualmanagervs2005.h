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

#include "afxvisualmanageroffice2003.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCVisualManagerVS2005 : public CMFCVisualManagerOffice2003
{
	DECLARE_DYNCREATE(CMFCVisualManagerVS2005)

public:
	CMFCVisualManagerVS2005();
	virtual ~CMFCVisualManagerVS2005();

	virtual void OnUpdateSystemColors();
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual void OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID = -1);

	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);

	virtual BOOL HasOverlappedAutoHideButtons() const { return m_bRoundedAutohideButtons; }

	virtual void OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton);
	virtual void OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton);

	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	virtual void OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton);

	virtual int GetMDITabsBordersSize() { return 1; }
	virtual int GetDockingTabsBordersSize();

	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);

	virtual void OnDrawToolBoxFrame(CDC* pDC, const CRect& rect);

	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);
	AFX_IMPORT_DATA static BOOL m_bRoundedAutohideButtons;

protected:
	int CreateAutoHideButtonRegion(CRect rect, DWORD dwAlignment, LPPOINT& points);

	COLORREF   m_colorActiveTabBorder;
	CPen       m_penActiveTabBorder;
	CBrush     m_brMenuButtonDroppedDown;
	CBrush     m_brMenuItemCheckedHighlight;
	WinXpTheme m_CurrAppTheme;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
