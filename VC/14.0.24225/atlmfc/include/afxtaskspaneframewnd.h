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
#include "afxpaneframewnd.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_CAPTION_BTN_LEFT   0x10
#define AFX_CAPTION_BTN_RIGHT  0x20
#define AFX_CAPTION_BTN_TPMENU 0x40

class CMFCTasksPaneFrameWnd : public CPaneFrameWnd
{
	DECLARE_SERIAL(CMFCTasksPaneFrameWnd)
public:
	CMFCTasksPaneFrameWnd();
	virtual ~CMFCTasksPaneFrameWnd();

// Operations
public:
	virtual void SetCaptionButtons(DWORD dwButtons);
	void SetTaskPaneCaptionButtons();

	virtual void CalcBorderSize(CRect& rectBorderSize) const;

protected:
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

protected:
	virtual void AddButton(UINT nHit);
	virtual void OnTrackCaptionButtons(CPoint point);
	virtual void StopCaptionButtonsTracking();

	virtual void OnDrawBorder(CDC* pDC);
	virtual void OnDrawCaptionButtons(CDC* pDC);
	virtual void OnPressButtons(UINT nHit);
	
	BOOL m_bMenuBtnPressed;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
