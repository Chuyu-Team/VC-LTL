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
#include "afxtoolbarmenubutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

extern AFX_IMPORT_DATA UINT AFX_WM_GETDOCUMENTCOLORS;

class CMFCColorMenuButton : public CMFCToolBarMenuButton  
{
	DECLARE_SERIAL(CMFCColorMenuButton)

public:
	CMFCColorMenuButton();
	CMFCColorMenuButton(UINT uiCmdID, LPCTSTR lpszText, CPalette* pPalette = NULL);
	virtual ~CMFCColorMenuButton();

	virtual void SetColor(COLORREF clr, BOOL bNotify = TRUE);
	COLORREF GetColor() const { return m_Color; }

	static COLORREF __stdcall GetColorByCmdID(UINT uiCmdID);
	static void __stdcall SetColorByCmdID(UINT uiCmdID, COLORREF color) { m_ColorsByID.SetAt(uiCmdID, color); }
	void SetColumnsNumber(int nColumns) { m_nColumns = nColumns; }

	void EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable = TRUE);
	void EnableOtherButton(LPCTSTR lpszLabel, BOOL bAltColorDlg = TRUE, BOOL bEnable = TRUE);
	void EnableDocumentColors(LPCTSTR lpszLabel, BOOL bEnable = TRUE);
	void EnableTearOff(UINT uiID, int nVertDockColumns = -1, int nHorzDockRows = -1);

	static void __stdcall SetColorName(COLORREF color, const CString& strName);

	COLORREF GetAutomaticColor() const { return m_colorAutomatic; }

	virtual BOOL OpenColorDialog(const COLORREF colorDefault, COLORREF& colorRes);
	virtual void OnChangeParentWnd(CWnd* pWndParent);

protected:
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE); 
	virtual CMFCPopupMenu* CreatePopupMenu();
	virtual BOOL IsEmptyMenuAllowed() const { return TRUE; }

	virtual void Serialize(CArchive& ar);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);

	void Initialize();

protected:
	COLORREF m_Color;  // Currently selected color
	COLORREF m_colorAutomatic;

	CArray<COLORREF, COLORREF> m_Colors;

	CPalette m_Palette;

	int m_nColumns;
	int m_nVertDockColumns;
	int m_nHorzDockRows;

	BOOL m_bIsAutomaticButton;
	BOOL m_bIsOtherButton;
	BOOL m_bIsDocumentColors;

	CString m_strAutomaticButtonLabel;
	CString m_strOtherButtonLabel;
	CString m_strDocumentColorsLabel;

	BOOL m_bStdColorDlg;

	AFX_IMPORT_DATA static CMap<UINT,UINT,COLORREF, COLORREF> m_ColorsByID;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
