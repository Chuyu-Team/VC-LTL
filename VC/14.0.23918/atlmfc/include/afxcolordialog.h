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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCColorPropertySheet;

#include "afxcontrolbarutil.h"
#include "afxdialogex.h"
#include "afxribbonres.h"
#include "afxcolorpickerctrl.h"
#include "afxbutton.h"
#include "afxstandardcolorspropertypage.h"
#include "afxcustomcolorspropertypage.h"

/*============================================================================*/
// CMFCColorDialog dialog

class CMFCColorDialog : public CDialogEx
{
// Construction

public:
	CMFCColorDialog(COLORREF clrInit = 0, DWORD dwFlags = 0 /* reserved */, CWnd* pParentWnd = NULL, HPALETTE hPal = NULL);
	virtual ~CMFCColorDialog();

	void SetCurrentColor(COLORREF rgb);
	void SetNewColor(COLORREF rgb);

	COLORREF GetColor() const { return m_NewColor; }

	void SetPageTwo(BYTE R, BYTE G, BYTE B);
	void SetPageOne(BYTE R, BYTE G, BYTE B);

	CPalette* GetPalette() const { return m_pPalette; }

// Dialog Data
	enum { IDD = IDD_AFXBARRES_COLOR_DLG };

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

// Implementation
protected:
	CMFCColorPropertySheet* m_pPropSheet;

	CMFCStandardColorsPropertyPage* m_pColourSheetOne;
	CMFCCustomColorsPropertyPage* m_pColourSheetTwo;
	CPalette*    m_pPalette;

	COLORREF m_CurrentColor;
	COLORREF m_NewColor;
	HCURSOR  m_hcurPicker;

	BOOL m_bIsMyPalette;
	BOOL m_bPickerMode;

	CMFCButton          m_btnColorSelect;
	CStatic             m_wndStaticPlaceHolder;
	CMFCColorPickerCtrl m_wndColors;

	virtual BOOL OnInitDialog();

	afx_msg void OnDestroy();
	afx_msg void OnSysColorChange();
	afx_msg void OnColorSelect();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

	void RebuildPalette();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
