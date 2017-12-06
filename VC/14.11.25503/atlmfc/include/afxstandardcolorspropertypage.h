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

#include "afxcolorpickerctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCColorDialog;

/*============================================================================*/
// CMFCStandardColorsPropertyPage dialog

class CMFCStandardColorsPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCStandardColorsPropertyPage)

// Construction
public:
	CMFCStandardColorsPropertyPage();

	CMFCColorDialog* m_pDialog;

// Dialog Data
	enum { IDD = IDD_AFXBARRES_COLOR_PAGE_ONE };
	CMFCColorPickerCtrl m_hexpicker;
	CMFCColorPickerCtrl m_hexpicker_greyscale;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	friend void hexfunc(COLORREF ref,DWORD ClientData);
	friend void hexfunc(COLORREF ref,void *ClientData);
	
	friend void hexgreyscalefunc(COLORREF ref,DWORD ClientData);
	friend void hexgreyscalefunc(COLORREF ref,void *ClientData);

	virtual BOOL OnInitDialog();

	afx_msg void OnGreyscale();
	afx_msg void OnHexColor();
	afx_msg void OnDoubleClickedColor();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

private:
	void AdjustControlWidth (CMFCColorPickerCtrl* pControl, int cx);
	int	m_nColorPickerOffset;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
