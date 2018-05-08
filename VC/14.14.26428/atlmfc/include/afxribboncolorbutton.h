// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#include "afxcontrolbarutil.h"
#include "afxcolorbar.h"
#include "afxribbonpalettegallery.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonColorButton : public CMFCRibbonGallery  
{
	friend class CMFCColorBar;
	friend class CMFCRibbonColorMenuButton;
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonConstructor;

	DECLARE_DYNCREATE(CMFCRibbonColorButton)

// Construction:
public:
	CMFCRibbonColorButton();
	CMFCRibbonColorButton(UINT nID, LPCTSTR lpszText, int nSmallImageIndex, COLORREF color = RGB(0, 0, 0));
	CMFCRibbonColorButton(UINT nID, LPCTSTR lpszText, BOOL bSimpleButtonLook, int nSmallImageIndex, int nLargeImageIndex, COLORREF color = RGB(0, 0, 0));

	virtual ~CMFCRibbonColorButton();

protected:
	void CommonInit();

// Attributes:
public:
	void EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable = TRUE, LPCTSTR lpszToolTip = NULL, BOOL bOnTop = TRUE, BOOL bDrawBorder = FALSE);
	void EnableOtherButton(LPCTSTR lpszLabel, LPCTSTR lpszToolTip = NULL);
	
	void SetDocumentColors(LPCTSTR lpszLabel, CList<COLORREF,COLORREF>& lstColors);

	static void __stdcall SetColorName(COLORREF color, const CString& strName) { CMFCColorBar::SetColorName(color, strName); }
	COLORREF GetAutomaticColor() const { return m_ColorAutomatic; }
	void SetColor(COLORREF color) { m_Color = color; }
	COLORREF GetColor() const { return m_Color; }

	void SetPalette(CPalette* pPalette);

	int GetColumns() const { return m_nIconsInRow; }
	void SetColumns(int nColumns) { m_nPanelColumns = m_nIconsInRow = nColumns; }

	COLORREF GetHighlightedColor() const;

	void SetColorBoxSize(CSize sizeBox);
	CSize GetColorBoxSize() const { return m_sizeBox; }

	virtual BOOL IsSimpleButtonLook() const { return m_bSimpleButtonLook; }

protected:
	virtual BOOL HasMenu() const { return TRUE; }

// Opeartions:
public:
	void AddColorsGroup(LPCTSTR lpszName, const CList<COLORREF,COLORREF>& lstColors, BOOL bContiguousColumns = FALSE);
	void RemoveAllColorGroups();

	void UpdateColor(COLORREF color);

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

protected:
	COLORREF GetColorByIndex(int nIconIndex) const;

// Overrides
protected:
	virtual void OnDrawPaletteIcon(CDC* pDC, CRect rectIcon, int nIconIndex, CMFCRibbonGalleryIcon* pIcon, COLORREF clrText);
	virtual void DrawImage(CDC* pDC, RibbonImageType type, CRect rectImage);
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void OnShowPopupMenu();
	virtual BOOL OnClickPaletteSubItem(CMFCRibbonButton* pButton, CMFCRibbonPanelMenuBar* pMenuBar);
	virtual void OnClickPaletteIcon(CMFCRibbonGalleryIcon* pIcon);
	virtual void NotifyHighlightListItem(int nIndex);
	virtual CString GetIconToolTip(const CMFCRibbonGalleryIcon* pIcon) const;

	virtual int GetGroupOffset() const { return 2; }

	COLORREF m_Color; // Currently selected color
	COLORREF m_ColorHighlighted;
	COLORREF m_ColorAutomatic;

	CArray<COLORREF, COLORREF> m_Colors;
	CArray<COLORREF, COLORREF> m_DocumentColors;
	CArray<LPARAM, LPARAM>     m_arContColumnsRanges;

	BOOL m_bIsAutomaticButton;
	BOOL m_bIsAutomaticButtonOnTop;
	BOOL m_bIsAutomaticButtonBorder;
	BOOL m_bIsOtherButton;
	BOOL m_bHasGroups;
	BOOL m_bSimpleButtonLook;

	CString m_strAutomaticButtonLabel;
	CString m_strAutomaticButtonToolTip;
	CString m_strOtherButtonLabel;
	CString m_strOtherButtonToolTip;
	CString m_strDocumentColorsLabel;

	CMFCRibbonButton* m_pOtherButton;
	CMFCRibbonButton* m_pAutoButton;

	CSize m_sizeBox;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
