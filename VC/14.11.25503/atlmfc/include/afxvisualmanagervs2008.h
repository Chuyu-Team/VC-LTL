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

#include "afxvisualmanagervs2005.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCVisualManagerVS2008 : public CMFCVisualManagerVS2005
{
	DECLARE_DYNCREATE(CMFCVisualManagerVS2008)

public:
	CMFCVisualManagerVS2008();
	virtual ~CMFCVisualManagerVS2008();

	virtual void OnUpdateSystemColors();
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual void OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed);
	virtual void OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);

	virtual CRect GetMenuImageFrameOffset() const { return CRect(4, 2, 0, 2); }

	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	// Tabs support:
	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pWndTab);
	virtual BOOL OnEraseTabsFrame(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);

	virtual int GetMDITabsBordersSize()			{	return 4;		}
	virtual BOOL IsMDITabsTopEdge()				{	return FALSE;	}
	virtual BOOL AlwaysHighlight3DTabs() const	{	return TRUE;	}

	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);

	virtual void OnDrawTabResizeBar(CDC* pDC, CMFCBaseTabCtrl* pWndTab, BOOL bIsVert, CRect rect, CBrush* pbrFace, CPen* pPen);

	// Auto-hide buttons:
	virtual void OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton);

	virtual COLORREF OnFillCommandsListBackground(CDC* pDC, CRect rect, BOOL bIsSelected = FALSE);

	virtual AFX_SMARTDOCK_THEME GetSmartDockingTheme();

protected:
	BOOL m_bOSColors;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
