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

#include "afxcontrolbarutil.h"
#include "afxbutton.h"
#include "afxcolorbar.h"

class CMFCColorPopupMenu;

/*============================================================================*/
// CMFCColorButton window

class CMFCColorButton : public CMFCButton
{
	friend class CMFCColorBar;

	DECLARE_DYNAMIC(CMFCColorButton)

// Construction
public:
	CMFCColorButton();

// Attributes
public:
	COLORREF GetColor() const { return m_Color; }
	void SetColor(COLORREF color /* -1 - automatic*/);

	void SetColumnsNumber(int nColumns) { m_nColumns = nColumns; }

	void EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable = TRUE);
	void EnableOtherButton(LPCTSTR lpszLabel, BOOL bAltColorDlg = TRUE, BOOL bEnable = TRUE);
	
	void SetDocumentColors(LPCTSTR lpszLabel, CList<COLORREF,COLORREF>& lstColors);

	static void __stdcall SetColorName(COLORREF color, const CString& strName) { CMFCColorBar::SetColorName(color, strName); }

	COLORREF GetAutomaticColor() const { return m_ColorAutomatic; }

	BOOL m_bEnabledInCustomizeMode;
	BOOL m_bAutoSetFocus;

protected:
	virtual void UpdateColor(COLORREF color);
	void RebuildPalette(CPalette* pPal);

protected:
	COLORREF m_Color;
	COLORREF m_ColorAutomatic;

	CArray<COLORREF, COLORREF> m_Colors;
	CList<COLORREF,COLORREF>   m_lstDocColors;

	int m_nColumns;
	CMFCColorPopupMenu* m_pPopup;

	CString m_strOtherText;
	CString m_strDocColorsText;
	CString m_strAutoColorText;

	BOOL m_bAltColorDlg;

	CPalette* m_pPalette;

// Operations
public:
	void SetPalette(CPalette* pPalette);

// Implementation
public:
	virtual ~CMFCColorButton();
	virtual CSize SizeToContent(BOOL bCalcOnly = FALSE);

protected:
	virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
	virtual void OnDrawFocusRect(CDC* pDC, const CRect& rectClient);
	
	virtual void OnShowColorPopup();

	BOOL IsDrawXPTheme() const;

protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSysColorChange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
