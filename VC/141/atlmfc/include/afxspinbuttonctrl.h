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
// CMFCSpinButtonCtrl window

class CMFCSpinButtonCtrl : public CSpinButtonCtrl
{
// Construction
public:
	CMFCSpinButtonCtrl();

// Attributes
protected:
	BOOL m_bIsButtonPressedUp;
	BOOL m_bIsButtonPressedDown;
	BOOL m_bIsButtonHighlightedUp;
	BOOL m_bIsButtonHighlightedDown;
	BOOL m_bTracked;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);

// Implementation
public:
	virtual ~CMFCSpinButtonCtrl();

protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
