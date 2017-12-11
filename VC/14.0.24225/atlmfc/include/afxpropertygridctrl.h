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

#include "comdef.h"
#include "afxcontrolbarutil.h"

#include "afxglobals.h"

#include "afxcolorpopupmenu.h"
#include "afxheaderctrl.h"
#include "afxpropertygridtooltipctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCPropertyGridProperty object

class CMFCPropertyGridProperty : public CObject
{
	DECLARE_DYNAMIC(CMFCPropertyGridProperty)

	friend class CMFCPropertyGridCtrl;

// Construction
public:
	// Group constructor
	CMFCPropertyGridProperty(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);

	// Simple property
	CMFCPropertyGridProperty(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);

	virtual ~CMFCPropertyGridProperty();

	enum ClickArea
	{
		ClickExpandBox,
		ClickName,
		ClickValue,
		ClickDescription
	};

// Operations:
public:
	int GetExpandedSubItems(BOOL bIncludeHidden = TRUE) const;
	BOOL AddSubItem(CMFCPropertyGridProperty* pProp);
	BOOL RemoveSubItem(CMFCPropertyGridProperty*& pProp, BOOL bDelete = TRUE);

	BOOL AddOption(LPCTSTR lpszOption, BOOL bInsertUnique = TRUE);
	void RemoveAllOptions();

	int GetOptionCount() const;
	LPCTSTR GetOption(int nIndex) const;

	CMFCPropertyGridProperty* HitTest(CPoint point, CMFCPropertyGridProperty::ClickArea* pnArea = NULL);

	void Expand(BOOL bExpand = TRUE);
	void Redraw();

	void EnableSpinControl(BOOL bEnable = TRUE, int nMin = 0, int nMax = 0);

	virtual void ResetOriginalValue();

	void Show(BOOL bShow = TRUE, BOOL bAdjustLayout = TRUE);

protected:
	void Init();
	void SetFlags();
	void SetOwnerList(CMFCPropertyGridCtrl* pWndList);
	void Reposition(int& y);
	void AddTerminalProp(CList<CMFCPropertyGridProperty*, CMFCPropertyGridProperty*>& lstProps);

	BOOL IsSubItem(CMFCPropertyGridProperty* pProp) const;
	CMFCPropertyGridProperty* FindSubItemByData(DWORD_PTR dwData) const;

	void ExpandDeep(BOOL bExpand = TRUE);
	void SetModifiedFlag();

// Overrides
public:
	virtual void OnDrawName(CDC* pDC, CRect rect);
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual void OnDrawExpandBox(CDC* pDC, CRect rectExpand);
	virtual void OnDrawButton(CDC* pDC, CRect rectButton);
	virtual void OnDrawDescription(CDC* pDC, CRect rect);

	virtual CString FormatProperty();

	virtual BOOL OnUpdateValue();
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
	virtual CSpinButtonCtrl* CreateSpinControl(CRect rectSpin);

	virtual BOOL OnEndEdit();

	virtual void OnClickButton(CPoint point);
	virtual BOOL OnClickValue(UINT uiMsg, CPoint point);
	virtual BOOL OnDblClk(CPoint point);

	virtual void OnSelectCombo();
	virtual void OnCloseCombo();

	virtual BOOL OnSetCursor() const;
	virtual BOOL PushChar(UINT nChar);

	virtual CString GetNameTooltip();
	virtual CString GetValueTooltip();

	virtual void OnClickName(CPoint /*point*/) {}
	virtual void OnRClickName(CPoint /*point*/) {}
	virtual void OnRClickValue(CPoint /*point*/, BOOL /*bSelChanged*/) {}

	virtual void OnPosSizeChanged(CRect /*rectOld*/) {}

	virtual void OnSetSelection(CMFCPropertyGridProperty* /*pOldSel*/) {}
	virtual void OnKillSelection(CMFCPropertyGridProperty* /*pNewSel*/) {}

	virtual void AdjustButtonRect();
	virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);

protected:
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);
	virtual CComboBox* CreateCombo(CWnd* pWndParent, CRect rect);
	virtual void OnDestroyWindow();

	virtual BOOL OnKillFocus(CWnd* /*pNewWnd*/) { return TRUE; }
	virtual BOOL OnEditKillFocus() { return TRUE; }

	virtual BOOL HasButton() const;

	virtual BOOL IsProcessFirstClick() const { return TRUE; }
	virtual BOOL HasValueField() const { return TRUE; }

	virtual BOOL TextToVar(const CString& strText);
	virtual BOOL IsValueChanged() const;

	virtual BOOL OnActivateByTab();
	virtual BOOL OnRotateListValue(BOOL bForward);

// Attributes
public:
	LPCTSTR GetName() const { return m_strName; }
	void SetName(LPCTSTR lpszName, BOOL bRedraw = TRUE);

	virtual const COleVariant& GetValue() const { return m_varValue; }
	virtual void SetValue(const COleVariant& varValue);

	const COleVariant& GetOriginalValue() const { return m_varValueOrig; }
	virtual void SetOriginalValue(const COleVariant& varValue);

	const CString& GetDescription() const { return m_strDescr; }
	void SetDescription(const CString& strDescr) { m_strDescr = strDescr; }

	DWORD_PTR GetData() const { return m_dwData; }
	void SetData(DWORD_PTR dwData) { m_dwData = dwData; }

	BOOL IsGroup() const { return m_bGroup; }
	BOOL IsExpanded() const { return m_bExpanded; }

	BOOL IsParentExpanded() const;

	virtual BOOL IsSelected() const;
	int GetHierarchyLevel() const;

	void Enable(BOOL bEnable = TRUE);
	BOOL IsEnabled() const { return m_bEnabled; }

	void AllowEdit(BOOL bAllow = TRUE)
	{
		ASSERT(m_varValue.vt != VT_BOOL);
		m_bAllowEdit = bAllow;
	}
	BOOL IsAllowEdit() const { return m_bAllowEdit; }

	CRect GetRect() const { return m_Rect; }

	int GetSubItemsCount() const { return (int) m_lstSubItems.GetCount(); }

	CMFCPropertyGridProperty* GetSubItem(int nIndex) const;

	CMFCPropertyGridProperty* GetParent() const { return m_pParent; }
	BOOL IsInPlaceEditing() const { return m_bInPlaceEdit; }
	BOOL IsModified() const { return m_bIsModified; }
	BOOL IsVisible() const { return m_bIsVisible; }

protected:
	COleVariant m_varValue;     // Property value
	COleVariant m_varValueOrig; // Property original value

	DWORD_PTR m_dwData;   // User-defined data
	DWORD     m_dwFlags;  // Property flags

	CRect m_Rect;       // Property rectangle (in the prop.list coordinates)
	CRect m_rectButton; // Drop down/open button rectangle

	BOOL m_bButtonIsDown;    // Is button pressed?
	BOOL m_bButtonIsFocused; // Is button focused?
	BOOL m_bInPlaceEdit;     // Is in InPalce editing mode
	BOOL m_bGroup;           // Is property group?
	BOOL m_bExpanded;        // Is property expanded (for groups only)
	BOOL m_bEnabled;         // Is propperty enabled?
	BOOL m_bAllowEdit;       // Is property editable?
	BOOL m_bIsValueList;     // This is a value list group?
	BOOL m_bNameIsTruncated;
	BOOL m_bValueIsTruncated;
	BOOL m_bIsModified;      // Is property modified
	BOOL m_bIsVisible;       // Is property visible

	int m_nMinValue;
	int m_nMaxValue;

	CString m_strName;        // Property name
	CString m_strDescr;       // Property description
	CString m_strEditMask;    // Property edit mask (see CMFCMaskedEdit for description)
	CString m_strEditTempl;   // Property edit template (see CMFCMaskedEdit for description)
	CString m_strValidChars;  // Property edit valid chars (see CMFCMaskedEdit for description)
	CStringList m_lstOptions; // List of combobox items

	CWnd*              m_pWndInPlace; // Pointer to InPlace editing window
	CComboBox*         m_pWndCombo;   // Pointer to combbox
	CSpinButtonCtrl*   m_pWndSpin;    // Pointer to spin button
	CMFCPropertyGridCtrl*     m_pWndList;    // Pointer to the PropertyList window
	CMFCPropertyGridProperty* m_pParent;     // Parent property (NULL for top-level properties)

	CList<CMFCPropertyGridProperty*, CMFCPropertyGridProperty*> m_lstSubItems; // Sub-properies list
};

/*============================================================================*/
// CMFCPropertyGridColorProperty object

class CMFCPropertyGridColorProperty : public CMFCPropertyGridProperty
{
	friend class CMFCPropertyGridCtrl;

	DECLARE_DYNAMIC(CMFCPropertyGridColorProperty)

// Construction
public:
	CMFCPropertyGridColorProperty(const CString& strName, const COLORREF& color, CPalette* pPalette = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CMFCPropertyGridColorProperty();

// Overrides
public:
	virtual void OnDrawValue(CDC* pDC, CRect rect);
	virtual void OnClickButton(CPoint point);
	virtual BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL OnUpdateValue();
	virtual CString FormatProperty();
	virtual void SetOriginalValue(const COleVariant& varValue);

protected:
	virtual BOOL OnKillFocus(CWnd* pNewWnd) { return pNewWnd->GetSafeHwnd() != m_pPopup->GetSafeHwnd(); }
	virtual BOOL OnEditKillFocus() { return m_pPopup == NULL; }
	virtual BOOL IsValueChanged() const { return m_Color != m_ColorOrig; }

	virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin);
	virtual void ResetOriginalValue();

// Attributes
public:
	COLORREF GetColor() const { return m_Color; }
	void SetColor(COLORREF color);

	// Color popup attributes:
	void EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable = TRUE);
	void EnableOtherButton(LPCTSTR lpszLabel, BOOL bAltColorDlg = TRUE, BOOL bEnable = TRUE);
	void SetColumnsNumber(int nColumnsNumber);

// Attributes
protected:
	COLORREF     m_Color;          // Color value
	COLORREF     m_ColorOrig;      // Original color value
	COLORREF     m_ColorAutomatic; // Automatic (default) color value
	CString      m_strAutoColor;   // Atomatic color label
	CString      m_strOtherColor;  // Alternative color label
	CMFCColorPopupMenu* m_pPopup;

	CArray<COLORREF, COLORREF> m_Colors;

	int  m_nColumnsNumber; // Number of columns in dropped-down colors list
	BOOL m_bStdColorDlg;   // Use standard Windows color dialog
};

/*============================================================================*/
// CMFCPropertyGridFileProperty object

class CMFCPropertyGridFileProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridFileProperty)

// Construction
public:

	CMFCPropertyGridFileProperty(const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = NULL);
	CMFCPropertyGridFileProperty(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CMFCPropertyGridFileProperty();

// Overrides
public:
	virtual void OnClickButton(CPoint point);

// Attributes
protected:
	BOOL    m_bIsFolder;

	// File open dialog atributes:
	BOOL    m_bOpenFileDialog; // TRUE - use "File Open/Save" diaog; otherwise - folder selection dialog
	DWORD   m_dwFileOpenFlags;
	CString m_strDefExt;
	CString m_strFilter;
};

/*============================================================================*/
// CMFCPropertyGridFontProperty object

class CMFCPropertyGridFontProperty : public CMFCPropertyGridProperty
{
	DECLARE_DYNAMIC(CMFCPropertyGridFontProperty)

// Construction
public:
	CMFCPropertyGridFontProperty(	const CString& strName, LOGFONT& lf, DWORD dwFontDialogFlags = CF_EFFECTS | CF_SCREENFONTS, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, COLORREF color = (COLORREF)-1);
	virtual ~CMFCPropertyGridFontProperty();

// Overrides
public:
	virtual void OnClickButton(CPoint point);
	virtual CString FormatProperty();

// Attributes
public:
	LPLOGFONT GetLogFont() { return &m_lf; }
	COLORREF GetColor() const { return m_Color; }

protected:
	LOGFONT   m_lf;
	LOGFONT   m_lfOrig;
	DWORD_PTR m_dwFontDialogFlags;
	COLORREF  m_Color;

	virtual BOOL IsValueChanged() const
	{
		return((memcmp(&m_lf, &m_lfOrig, sizeof(LOGFONT)) != 0) || (lstrcmp(m_lf.lfFaceName, m_lfOrig.lfFaceName) != 0));
	}

	virtual void ResetOriginalValue();
};

/*============================================================================*/
// CMFCPropertyGridCtrl window

#define AFX_PROPLIST_ID_INPLACE 3

extern AFX_IMPORT_DATA UINT AFX_WM_PROPERTY_CHANGED;

class CMFCPropertyGridCtrl : public CWnd
{
	DECLARE_DYNAMIC(CMFCPropertyGridCtrl)

	friend class CMFCPropertyGridProperty;
	friend class CMFCPropertyGridColorProperty;

// Construction
public:
	CMFCPropertyGridCtrl();

// Attributes
public:
	void EnableHeaderCtrl(BOOL bEnable = TRUE, LPCTSTR lpszLeftColumn = _T("Property"), LPCTSTR lpszRightColumn = _T("Value"));
	BOOL IsHeaderCtrl() const { return m_bHeaderCtrl; }

	void EnableDescriptionArea(BOOL bEnable = TRUE);
	BOOL IsDescriptionArea() const { return m_bDescriptionArea; }

	int GetDescriptionHeight() const { return m_nDescrHeight; }
	int GetDescriptionRows() const { return m_nDescrRows; }
	void SetDescriptionRows(int nDescRows);

	void SetAlphabeticMode(BOOL bSet = TRUE);
	BOOL IsAlphabeticMode() const { return m_bAlphabeticMode; }

	void SetVSDotNetLook(BOOL bSet = TRUE);
	BOOL IsVSDotNetLook() const { return m_bVSDotNetLook; }

	void MarkModifiedProperties(BOOL bMark = TRUE, BOOL bRedraw = TRUE);
	BOOL IsMarkModifiedProperties() const { return m_bMarkModifiedProperties; }

	void ResetOriginalValues(BOOL bRedraw = TRUE);

	void SetBoolLabels(LPCTSTR lpszTrue, LPCTSTR lpszFalse);
	void SetListDelimiter(TCHAR c);

	CRect GetListRect() const { return m_rectList; }

	int GetPropertyColumnWidth() const { return m_nLeftColumnWidth; }
	int GetHeaderHeight() const { return m_nHeaderHeight; }
	int GetRowHeight() const { return m_nRowHeight; }

	virtual CMFCHeaderCtrl& GetHeaderCtrl() { return m_wndHeader; }

	int GetLeftColumnWidth() const { return m_nLeftColumnWidth; }
	BOOL IsGroupNameFullWidth() const { return m_bGroupNameFullWidth; }

	void SetGroupNameFullWidth(BOOL bGroupNameFullWidth = TRUE, BOOL bRedraw = TRUE);

	BOOL IsShowDragContext() const { return m_bShowDragContext; }
	void SetShowDragContext(BOOL bShowDragContext = TRUE) { m_bShowDragContext = bShowDragContext; }

	// Use (COLORREF)-1 for the default color
	void SetCustomColors(COLORREF clrBackground, COLORREF clrText, COLORREF clrGroupBackground, COLORREF clrGroupText,
		COLORREF clrDescriptionBackground, COLORREF clrDescriptionText, COLORREF clrLine);
	void GetCustomColors(COLORREF& clrBackground, COLORREF& clrText, COLORREF& clrGroupBackground, COLORREF& clrGroupText,
		COLORREF& clrDescriptionBackground, COLORREF& clrDescriptionText, COLORREF& clrLine);

	COLORREF GetTextColor() const { return m_clrText == (COLORREF)-1 ? GetGlobalData()->clrWindowText : m_clrText; }
	COLORREF GetBkColor() const { return m_clrBackground == (COLORREF)-1 ? GetGlobalData()->clrWindow : m_clrBackground; }

	CFont& GetBoldFont() { return m_fontBold; }

	BOOL IsAlwaysShowUserToolTip() const { return m_bAlwaysShowUserTT; }
	void AlwaysShowUserToolTip(BOOL bShow = TRUE) { m_bAlwaysShowUserTT = bShow; }

	BOOL DrawControlBarColors() const { return m_bControlBarColors; }

protected:
	CToolTipCtrl        m_ToolTip;       // Tooltip control
	CMFCHeaderCtrl   m_wndHeader;     // Property list header control
	CMFCPropertyGridToolTipCtrl m_IPToolTip;     // Inplace tooltip control
	CScrollBar          m_wndScrollVert; // Vertical scroll bar

	BOOL m_bHeaderCtrl;             // Is header control visible?
	BOOL m_bDescriptionArea;        // Does description area enabled?
	BOOL m_bAlwaysShowUserTT;       // Always show user tooltips, even if in-place TT should be activated
	BOOL m_bAlphabeticMode;         // Use property list in alphabetic (non-"tree") mode
	BOOL m_bVSDotNetLook;           // Provide MS Visual Studio.NET look (gray groups, flat look)
	BOOL m_bMarkModifiedProperties; // Draw modified properties with bold

	CString m_strTrue;  // Customized boolean value (e.g. "Yes")
	CString m_strFalse; // Customized boolean value (e.g. "No")

	TCHAR m_cListDelimeter; // Customized list delimeter character

	HFONT m_hFont;    // Property list regular font
	CFont m_fontBold; // Property list bold font
	CRect m_rectList; // Properies area

	int m_nDescrHeight;        // Description area height
	int m_nDescrRows;          // Number of rows in description area
	int m_nTooltipsCount;      // Number of tooltip items
	int m_nEditLeftMargin;     // Edit control left margin
	int m_nBoldEditLeftMargin; // Edit control left margin (bold font)
	int m_nBorderSize;         // Control border size
	int m_nHeaderHeight;       // Header control height
	int m_nRowHeight;          // Height of the single row
	int m_nLeftColumnWidth;    // Width of the left ("Name") column
	int m_nVertScrollOffset;   // In rows
	int m_nVertScrollTotal;
	int m_nVertScrollPage;

	//-----------------------------------------------------------
	// Tracking attributes: used for the vertical and description 
	// area splitters tracking:
	//-----------------------------------------------------------
	CRect m_rectTrackHeader;
	CRect m_rectTrackDescr;

	BOOL m_bTracking;
	BOOL m_bTrackingDescr;
	BOOL m_bFocused;            // Control has focus
	BOOL m_bControlBarColors;   // Use colors of tye parent control bar
	BOOL m_bGroupNameFullWidth; // Display group name in the whole row
	BOOL m_bShowDragContext;    // Show context while dragging spliters

	CList<CMFCPropertyGridProperty*, CMFCPropertyGridProperty*> m_lstProps;         // List of top-level properties
	CList<CMFCPropertyGridProperty*, CMFCPropertyGridProperty*> m_lstTerminalProps; // List of terminal properties
	CMFCPropertyGridProperty*                            m_pSel;             // Current selection

	COLORREF m_clrGray; // Special gray color

	//---------------
	// Custom colors:
	//---------------
	COLORREF m_clrBackground;            // Control background color
	COLORREF m_clrText;                  // Control foreground color
	COLORREF m_clrGroupBackground;       // Group background text
	COLORREF m_clrGroupText;             // Group foreground text
	COLORREF m_clrDescriptionBackground; // Description background text
	COLORREF m_clrDescriptionText;       // Description foreground text
	COLORREF m_clrLine;                  // Color of the grid lines

	CBrush m_brBackground;

// Operations
public:
	int AddProperty(CMFCPropertyGridProperty* pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	BOOL DeleteProperty(CMFCPropertyGridProperty*& pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE);
	void RemoveAll();

	CMFCPropertyGridProperty* GetProperty(int nIndex) const;
	int GetPropertyCount() const { return (int) m_lstProps.GetCount(); }

	CMFCPropertyGridProperty* FindItemByData(DWORD_PTR dwData, BOOL bSearchSubItems = TRUE) const;

	CMFCPropertyGridProperty* HitTest(CPoint pt, CMFCPropertyGridProperty::ClickArea* pnArea = NULL, BOOL bPropsOnly = FALSE) const;

	void SetCurSel(CMFCPropertyGridProperty* pProp, BOOL bRedraw = TRUE);
	CMFCPropertyGridProperty* GetCurSel() const { return m_pSel; }

	void EnsureVisible(CMFCPropertyGridProperty* pProp, BOOL bExpandParents = FALSE);
	void ExpandAll(BOOL bExpand = TRUE);

	virtual void CloseColorPopup();
	virtual void UpdateColor(COLORREF color);
	virtual void AdjustLayout();

// Overrides
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;

public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;

protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

public:
	virtual void OnChangeSelection(CMFCPropertyGridProperty* /*pNewSel*/, CMFCPropertyGridProperty* /*pOldSel*/) {}

	virtual BOOL EditItem(CMFCPropertyGridProperty* pProp, LPPOINT lptClick = NULL);
	virtual void OnClickButton(CPoint point);
	virtual BOOL EndEditItem(BOOL bUpdateData = TRUE);
	virtual BOOL ValidateItemData(CMFCPropertyGridProperty* /*pProp*/) { return TRUE; }

	virtual int OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const;
	virtual void InitHeader();

protected:
	virtual void Init();

	virtual void OnFillBackground(CDC* pDC, CRect rectClient);

	virtual void OnDraw(CDC* pDC);
	virtual void OnDrawBorder(CDC* pDC);
	virtual void OnDrawList(CDC* pDC);
	virtual void OnDrawDescription(CDC* pDC, CRect rect);

	virtual BOOL ProcessClipboardAccelerators(UINT nChar);
	virtual int CompareProps(const CMFCPropertyGridProperty* pProp1, const CMFCPropertyGridProperty* pProp2) const;
	virtual void NotifyAccessibility(CMFCPropertyGridProperty* pProp);

// Implementation
public:
	virtual ~CMFCPropertyGridCtrl();

	//----------------------
	// Accessibility support:
	//----------------------
	CMFCPropertyGridProperty* m_pAccProp;

// IAccessible
public:
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT get_accName(VARIANT varChild, BSTR *pszName);
	virtual HRESULT get_accValue(VARIANT varChild, BSTR *pszValue);
	virtual HRESULT get_accDescription(VARIANT varChild, BSTR *pszDescription);
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);
	virtual HRESULT get_accHelp(VARIANT varChild, BSTR *pszHelp);
	virtual HRESULT get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic);
	virtual HRESULT get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut);
	virtual HRESULT get_accFocus(VARIANT *pvarChild);
	virtual HRESULT get_accSelection(VARIANT *pvarChildren);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);

	virtual HRESULT accSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg void OnHeaderItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHeaderEndTrack(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpinDeltaPos(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUpdateSpin(WPARAM, LPARAM);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnSelectCombo();
	afx_msg void OnCloseCombo();
	afx_msg void OnEditKillFocus();
	afx_msg void OnComboKillFocus();
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnGetObject(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	//------------------
	// Internal helpres:
	//------------------
	HFONT SetCurrFont(CDC* pDC);
	void TrackHeader(int nOffset);
	void TrackDescr(int nOffset);
	void TrackToolTip(CPoint pt);

	void SetScrollSizes();

	int GetTotalItems(BOOL bIncludeHidden = TRUE) const;
	void ReposProperties();

	void CreateBoldFont();
	void CalcEditMargin();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
