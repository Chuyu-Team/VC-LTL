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

#include "afxribbonpalettegallery.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonUndoButton : public CMFCRibbonGallery  
{
	DECLARE_DYNCREATE(CMFCRibbonUndoButton)

// Construction
public:
	CMFCRibbonUndoButton();
	CMFCRibbonUndoButton(UINT nID, LPCTSTR lpszText, int nSmallImageIndex = -1, int nLargeImageIndex = -1);
	CMFCRibbonUndoButton(UINT nID, LPCTSTR lpszText, HICON hIcon);

	virtual ~CMFCRibbonUndoButton();

// Attributes
public:
	virtual BOOL HasMenu() const { return TRUE; }
	int GetActionNumber() const { return m_nActionNumber; }

protected:
	int          m_nActionNumber;
	CSize        m_sizeMaxText;
	CString      m_strCancel;
	CString      m_strUndoOne;
	CString      m_strUndoFmt;
	CStringArray m_arLabels;

// Overrides:
protected:
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void OnClick(CPoint point);
	virtual CSize GetIconSize() const;
	virtual void OnDrawPaletteIcon(CDC* pDC, CRect rectIcon, int nIconIndex, CMFCRibbonGalleryIcon* pIcon, COLORREF clrText);
	virtual void NotifyHighlightListItem(int nIndex);
	virtual BOOL OnClickPaletteSubItem(CMFCRibbonButton* pButton, CMFCRibbonPanelMenuBar* pMenuBar);
	virtual void OnShowPopupMenu();

	void CommonInit();

// Operations
public:
	void AddUndoAction(LPCTSTR lpszLabel);
	void CleanUpUndoList();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
