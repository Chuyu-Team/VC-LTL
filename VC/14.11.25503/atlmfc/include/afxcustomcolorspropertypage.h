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
// CMFCCustomColorsPropertyPage dialog

class CMFCCustomColorsPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCCustomColorsPropertyPage)

// Construction
public:
	CMFCCustomColorsPropertyPage();
	CMFCColorDialog* m_pDialog;

	void Setup(BYTE R, BYTE G, BYTE B);

protected:
	enum { IDD = IDD_AFXBARRES_COLORPAGE_TWO };
	CMFCColorPickerCtrl m_wndLuminance;
	CMFCColorPickerCtrl m_wndColorPicker;
	int  m_r;
	int  m_b;
	int  m_g;
	UINT m_l;
	UINT m_h;
	UINT m_s;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnRGBChanged();
	afx_msg void OnHLSChanged();
	afx_msg void OnLuminance();
	afx_msg void OnColour();
	afx_msg void OnDoubleClickedColor();

	DECLARE_MESSAGE_MAP()

	BOOL m_bIsReady;
	BOOL m_bInUpdate;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
