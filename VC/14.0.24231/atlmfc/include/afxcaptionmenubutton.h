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
#include "afxcaptionbutton.h"
#include "afxmenuimages.h"
#include "afxvisualmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCCaptionMenuButton : public CMFCCaptionButton
{
public:
	CMFCCaptionMenuButton();
	CMFCCaptionMenuButton(UINT nHit, BOOL bLeftAlign = FALSE);
	virtual ~CMFCCaptionMenuButton();

// Attributes
public:
	int  m_nMenuResult;
	BOOL m_bOSMenu;       // User standard Windows menu either than library
	BOOL m_bMenuIsActive;

// Operations
public:
	void ShowMenu(HMENU hMenu, CWnd* pWnd);
	virtual void OnDraw(CDC* pDC, BOOL bActive, BOOL bHorz = TRUE, BOOL bMaximized = TRUE, BOOL bDisabled = FALSE);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
