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
#include "afxmenuimages.h"
#include "afxvisualmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_HTCLOSE       HTOBJECT	// To prevent standard Windows 98/2000 close tooltip
#define AFX_HTLEFTBUTTON  HTHELP + 2
#define AFX_HTRIGHTBUTTON HTHELP + 3
#define AFX_HTMENU        HTHELP + 4
#define AFX_HTMAXBUTTON   HTHELP + 5
#define AFX_HTMINBUTTON   HTHELP + 6

class CMFCCaptionButton : public CObject
{
public:
	CMFCCaptionButton();
	CMFCCaptionButton(UINT nHit, BOOL bLeftAlign = FALSE);
	virtual ~CMFCCaptionButton();

	void Move (const CPoint& ptTo, BOOL bHide = FALSE)
	{
		m_ptOrg = ptTo;
		m_bHidden = bHide;
	}

	virtual CRect GetRect () const
	{
		return CRect (m_ptOrg, m_bHidden ? CSize (0, 0) : GetSize ());
	}

	static CSize __stdcall GetSize ()
	{
		return CMenuImages::Size () + CSize (2 * m_nButtonMargin, 2 * m_nButtonMarginVert) +
			CMFCVisualManager::GetInstance ()->GetCaptionButtonExtraBorder ();
	}

	virtual void OnDraw (CDC* pDC, BOOL bActive, BOOL bHorz = TRUE, BOOL bMaximized = TRUE, BOOL bDisabled = FALSE);

	UINT GetHit () const;

	virtual CMenuImages::IMAGES_IDS GetIconID (BOOL bHorz, BOOL bMaximized = FALSE) const;

	void SetMiniFrameButton (BOOL bSet = TRUE) { m_bIsMiniFrameButton = bSet; }
	BOOL IsMiniFrameButton () const { return m_bIsMiniFrameButton; }

	BOOL m_bPushed;
	BOOL m_bFocused;
	BOOL m_bHidden;
	BOOL m_bEnabled;
	BOOL m_bDroppedDown;
	BOOL m_bLeftAlign;
	UINT m_nHit;

	COLORREF m_clrForeground;

	AFX_IMPORT_DATA static int m_nButtonMargin;
	AFX_IMPORT_DATA static int m_nButtonMarginVert;

protected:
	CPoint m_ptOrg;
	BOOL   m_bIsMiniFrameButton;
};

class CMFCCaptionButtonEx : public CMFCCaptionButton
{
public:
	CMFCCaptionButtonEx (UINT nHit);
	~CMFCCaptionButtonEx();

	void SetRect (const CRect& rect) { m_rect = rect; }
	virtual CRect GetRect () const { return m_rect; }

protected:
	CRect m_rect;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
