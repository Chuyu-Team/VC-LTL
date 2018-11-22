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

class CMFCFontInfo;

/*============================================================================*/
// CMFCFontComboBox window

class CMFCFontComboBox : public CComboBox
{
// Construction
public:
	CMFCFontComboBox();

// Attributes
public:
	AFX_IMPORT_DATA static BOOL m_bDrawUsingFont;

protected:
	CImageList m_Images;
	BOOL       m_bToolBarMode;

// Operations
public:
	BOOL Setup(int nFontType = DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE, BYTE nCharSet = DEFAULT_CHARSET, BYTE nPitchAndFamily = DEFAULT_PITCH);
	BOOL SelectFont(CMFCFontInfo* pDesc);
	BOOL SelectFont(LPCTSTR lpszName, BYTE nCharSet = DEFAULT_CHARSET);
	
	CMFCFontInfo* GetSelFont() const;

protected:
	void Init();
	void CleanUp();

// Overrides
public:
	int DeleteString(UINT nIndex);

	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void PreSubclassWindow();

// Implementation
public:
	virtual ~CMFCFontComboBox();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
