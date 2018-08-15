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

#include "afxtoolbarimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CSmartDockingInfo;

class CSmartDockingStandaloneGuideWnd : public CWnd
{
// Construction
public:
	CSmartDockingStandaloneGuideWnd();
	virtual ~CSmartDockingStandaloneGuideWnd();

	BOOL Create(LPRECT pWndRect, HBITMAP hbmpFace, HRGN hrgnShape, CWnd* pwndOwner, BOOL bIsDefaultImage, BOOL bIsVert);
	BOOL Assign(HBITMAP hbmpFace, BOOL bRedraw);
	void Highlight(BOOL bSet);
	void UpdateLayered();

// Attributes
protected:
	HBITMAP  m_hbmpFace;
	COLORREF m_clrFrame;
	BOOL     m_bIsDefaultImage;
	BOOL     m_bIsHighlighted;
	BOOL     m_bIsVert;

// Overrides
protected:
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

class CSmartDockingStandaloneGuide : public CObject
{
public:
	enum SDMarkerPlace
	{
		sdNONE = -1,

		sdLEFT,
		sdRIGHT,
		sdTOP,
		sdBOTTOM,

		sdCLEFT,
		sdCRIGHT,
		sdCTOP,
		sdCBOTTOM,
		sdCMIDDLE,
	};

	CSmartDockingStandaloneGuide();
	virtual ~CSmartDockingStandaloneGuide();

	virtual void Create(SDMarkerPlace nSideNo, CWnd* pwndOwner);
	virtual void Destroy();

	virtual void Show(BOOL bShow = TRUE);        // show or hide marker
	virtual void AdjustPos(CRect rcHost);        // adjust marker position to the host rectangle (in screen coords)
	virtual void Highlight(BOOL bHiLite = TRUE); // highlight the marker
	virtual BOOL IsPtIn(CPoint point) const;     // discover if a point (in screen coords) lies within marker

protected:
	virtual void InitImages(CSmartDockingInfo& params);

	SDMarkerPlace  m_nSideNo;
	CSmartDockingStandaloneGuideWnd  m_wndBmp;
	CMFCToolBarImages m_Image;
	CMFCToolBarImages m_ImageHot;

	CRgn m_Rgn;
	int  m_cx;
	int  m_cy;
	BOOL m_bHiLited;
	BOOL m_bLayered;
	BOOL m_bIsDefaultImage;

	AFX_IMPORT_DATA static const int m_nLeftOffsetX;
	AFX_IMPORT_DATA static const int m_nRightOffsetX;
	AFX_IMPORT_DATA static const int m_nTopOffsetY;
	AFX_IMPORT_DATA static const int m_nBottomOffsetY;

	DECLARE_DYNCREATE(CSmartDockingStandaloneGuide)
};

class CSmartDockingGroupGuidesManager;

class CSmartDockingGroupGuidesWnd : public CWnd
{
	friend class CSmartDockingGroupGuidesManager;

protected:
	CSmartDockingGroupGuidesWnd();

public:
	void Update ();

	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

protected:
	CSmartDockingGroupGuidesManager* m_pCentralGroup;
	CBrush m_brBaseBackground;
	CBrush m_brBaseBorder;
};

class CSmartDockingGroupGuide : public CSmartDockingStandaloneGuide
{
public:
	CSmartDockingGroupGuide();
	virtual ~CSmartDockingGroupGuide();

	virtual void Create(SDMarkerPlace nSideNo, CWnd* pwndOwner);
	virtual void Destroy();

	virtual void Show(BOOL bShow = TRUE);        // show or hide marker
	virtual void AdjustPos(CRect rcHost);        // adjust marker position to the host rectangle (in screen coords)
	virtual void Highlight(BOOL bHiLite = TRUE); // highlight the marker
	virtual BOOL IsPtIn(CPoint point) const;     // discover if a point (in screen coords) lies within marker

	void SetVisible(BOOL bVisible = TRUE, BOOL bRedraw = TRUE);
	BOOL IsVisible() const { return m_bVisible; }

protected:
	virtual void Create(SDMarkerPlace nSideNo, CSmartDockingGroupGuidesManager* pCentralGroup);
	virtual void DestroyImages();

	virtual void Draw(CDC& dc, BOOL bAlpha = FALSE);

	CSmartDockingGroupGuidesManager* m_pCentralGroup;
	COLORREF m_clrFrame;
	CPen m_penFrame;
	CPen m_penHighlight;
	int  m_nOffsetX;
	int  m_nOffsetY;
	BOOL m_bVisible;

	friend class CSmartDockingGroupGuidesManager;
};

class CSmartDockingGroupGuidesManager : public CObject
{
public:
	CSmartDockingGroupGuidesManager();
	virtual ~CSmartDockingGroupGuidesManager();

	virtual void Create(CWnd* pwndOwner);
	virtual void Destroy();
	virtual void Show(BOOL bShow = TRUE);   // show or hide group

	// adjust group position to the host rectangle (in screen coords)
	// middle marker: nMiddleIsOn == -1 no change, 0 off, 1 on
	virtual BOOL AdjustPos(CRect rcHost, int nMiddleIsOn = -1);

	void ShowGuide(CSmartDockingStandaloneGuide::SDMarkerPlace nMarkerNo, BOOL bShow = TRUE, BOOL bRedraw = TRUE);

	// fetch markers' interfaces (0 - left, 1 - right, 2 - top, 3 - bottom, 4 - middle)
	virtual CSmartDockingGroupGuide* GetGuide(CSmartDockingStandaloneGuide::SDMarkerPlace nMarkerNo)
	{
		if (nMarkerNo < CSmartDockingStandaloneGuide::sdCLEFT || nMarkerNo > CSmartDockingStandaloneGuide::sdCMIDDLE)
		{
			ASSERT(FALSE);
			return NULL;
		}
		return &m_arMarkers [nMarkerNo - CSmartDockingStandaloneGuide::sdCLEFT];
	}

	void GetWindowRect(CRect& rect);

	DECLARE_DYNCREATE(CSmartDockingGroupGuidesManager)

protected:
	CSmartDockingGroupGuidesWnd m_Wnd;

	CSmartDockingGroupGuide m_arMarkers [5];

	CMFCToolBarImages m_Image;

	CRgn m_rgnBase;
	BOOL m_bCreated;
	BOOL m_bMiddleIsOn;
	BOOL m_bLayered;

	friend class CSmartDockingGroupGuidesWnd;
	friend class CSmartDockingGroupGuide;
	friend class CSmartDockingManager;

	void DrawCentralGroupGuides(CDC& dc, CBrush& brBaseBackground, CBrush& brBaseBorder, CRect rectClient);
};

// provide window class name for specified class style
template <DWORD WndStyle>
LPCTSTR GetSmartDockingWndClassName()
{
	return AfxRegisterWndClass(WndStyle);
}

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
