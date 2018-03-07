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

#include "afxribbonedit.h"
#include "afxtoolbarfontcombobox.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonRichEditCtrl;

/*============================================================================*/
// CMFCRibbonComboBox

class CMFCRibbonComboBox : public CMFCRibbonEdit
{
	friend class CMFCDropDownListBox;
	friend class CMFCRibbonConstructor;

	DECLARE_DYNCREATE(CMFCRibbonComboBox)

// Construction:
public:
	CMFCRibbonComboBox(UINT nID, BOOL bHasEditBox = TRUE, int nWidth = -1, LPCTSTR lpszLabel = NULL, int nImage = -1);
	virtual ~CMFCRibbonComboBox();

protected:
	CMFCRibbonComboBox();

// Attributes:
public:
	void EnableDropDownListResize(BOOL bEnable = FALSE) { m_bResizeDropDownList = bEnable; }
	BOOL IsResizeDropDownList() const { return m_bResizeDropDownList; }

// Operations:
public:
	virtual INT_PTR AddItem(LPCTSTR lpszItem, DWORD_PTR dwData = 0);
	INT_PTR GetCount() const { return m_lstItems.GetCount(); }

	LPCTSTR GetItem(int iIndex) const;
	DWORD_PTR GetItemData(int iIndex) const;

	int GetCurSel() const { return m_iSelIndex; }

	void RemoveAllItems();

	BOOL SelectItem(int iIndex);
	BOOL SelectItem(DWORD_PTR dwData);
	BOOL SelectItem(LPCTSTR lpszText);

	BOOL DeleteItem(int iIndex);
	BOOL DeleteItem(DWORD_PTR dwData);
	BOOL DeleteItem(LPCTSTR lpszText);

	int FindItem(LPCTSTR lpszText) const;

	int GetDropDownHeight() const { return m_nDropDownHeight; }
	void SetDropDownHeight(int nHeight);

	BOOL HasEditBox() const { return m_bHasEditBox; }

// Overrides
public:
	virtual CSize GetIntermediateSize(CDC* pDC);
	virtual void OnSelectItem(int nItem);

protected:
	virtual void OnDraw(CDC* pDC);

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);

	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual BOOL HasMenu() const { return TRUE; }
	virtual BOOL CanBeStretched() { return FALSE; }

	virtual void OnAfterChangeRect(CDC* pDC);

	virtual void DropDownList();
	void CommonInit();

	virtual void ClearData();

	virtual BOOL OnDrawDropListItem(CDC* pDC, int nIndex, CMFCToolBarMenuButton* pItem, BOOL bHighlight);
	virtual CSize OnGetDropListItemSize(CDC* pDC, int nIndex, CMFCToolBarMenuButton* pItem, CSize sizeDefault);

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

// Attributes:
protected:
	CStringList                 m_lstItems;
	CList<DWORD_PTR, DWORD_PTR> m_lstItemData;

	int m_iSelIndex;
	int m_nDropDownHeight;

	BOOL m_bHasEditBox;
	BOOL m_bResizeDropDownList;
};

/*============================================================================*/
// CMFCRibbonFontComboBox

class CMFCRibbonFontComboBox : public CMFCRibbonComboBox  
{
	DECLARE_DYNCREATE(CMFCRibbonFontComboBox)

// Construction:
public:
	CMFCRibbonFontComboBox(UINT nID, int nFontType = DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE,
		BYTE nCharSet = DEFAULT_CHARSET, BYTE nPitchAndFamily = DEFAULT_PITCH, int nWidth = -1);

	virtual ~CMFCRibbonFontComboBox();

protected:
	CMFCRibbonFontComboBox();

// Operations:
public:
	BOOL SetFont(LPCTSTR lpszName, BYTE nCharSet = DEFAULT_CHARSET, BOOL bExact = FALSE);
	const CMFCFontInfo* GetFontDesc(int iIndex = -1) const
	{
		return (CMFCFontInfo*) GetItemData(iIndex);
	}

	void BuildFonts(int nFontType = DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE, BYTE nCharSet = DEFAULT_CHARSET, BYTE nPitchAndFamily = DEFAULT_PITCH);
	void RebuildFonts();

	/// <summary>
	/// Returns which font types to display in the combo box. Valid options are DEVICE_FONTTYPE, RASTER_FONTTYPE, and TRUETYPE_FONTTYPE, or any bitwise combination thereof.</summary>
	/// <returns> 
	/// Font types ( see EnumFontFamProc in the Windows SDK documentation).</returns>
	int GetFontType() const { return m_nFontType; }

	/// <summary>
	/// Returns the specified character set.</summary>
	/// <returns> 
	/// Characters set (see LOGFONT in the Windows SDK documentation).</returns>
	BYTE GetCharSet() const { return m_nCharSet; }

	/// <summary>
	/// Returns the pitch and the family of the fonts that are displayed in the combo box.</summary>
	/// <returns> 
	/// Pitch and the family (see LOGFONT in the Windows SDK documentation).</returns>
	BYTE GetPitchAndFamily() const { return m_nPitchAndFamily; }

protected:
	int GetFontsCount(LPCTSTR lpszName, const CObList& lstFonts);

// Overrides
protected:
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void ClearData();
	virtual void DropDownList();

	virtual BOOL OnDrawDropListItem(CDC* pDC, int nIndex, CMFCToolBarMenuButton* pItem, BOOL bHighlight);
	virtual CSize OnGetDropListItemSize(CDC* pDC, int nIndex, CMFCToolBarMenuButton* pItem, CSize sizeDefault);

// Attributes:
public:
	AFX_IMPORT_DATA static BOOL m_bDrawUsingFont;

protected:
	int  m_nFontType;
	BYTE m_nCharSet;
	BYTE m_nPitchAndFamily;

	CImageList m_Images;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
