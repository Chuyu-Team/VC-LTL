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
#include "afxglobals.h"
#include "afxribbonbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonCategory;
class CMFCRibbonBaseElement;
class CMFCRibbonPanel;
class CMFCRibbonGallery;

/*============================================================================*/
// CMFCRibbonLaunchButton

class CMFCRibbonLaunchButton : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonLaunchButton)

	CMFCRibbonLaunchButton();

	virtual void OnDraw(CDC* pDC);
	virtual CSize GetRegularSize(CDC* pDC);
	virtual void OnClick(CPoint point);
	virtual CRect GetKeyTipRect(CDC* pDC, BOOL bIsMenu);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	CMFCRibbonPanel* m_pParentPanel;
};

/*============================================================================*/
// CMFCRibbonDefaultPanelButton

class CMFCRibbonDefaultPanelButton : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonDefaultPanelButton)

	friend class CMFCRibbonCategory;

	CMFCRibbonDefaultPanelButton(CMFCRibbonPanel* pPanel = NULL);

	virtual void OnDraw(CDC* pDC);
	virtual void OnLButtonDown(CPoint point);
	virtual void OnShowPopupMenu();
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	virtual CString GetToolTipText() const { return m_rect.IsRectEmpty() ? CString() : m_strText; }
	virtual void OnClick(CPoint /*point*/) { OnShowPopupMenu(); }

	virtual CSize GetImageSize(RibbonImageType /*type*/) const
	{
		CSize sizeIcon(16, 16);

		if (GetGlobalData()->GetRibbonImageScale() != 1.)
		{
			sizeIcon.cx = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * sizeIcon.cx);
			sizeIcon.cy = (int)(.5 + GetGlobalData()->GetRibbonImageScale() * sizeIcon.cy);
		}

		return sizeIcon;
	}

	virtual BOOL IsDefaultPanelButton() const { return TRUE; }

	virtual void DrawImage(CDC* pDC, RibbonImageType type, CRect rectImage);
	virtual BOOL CanBeAddedToQuickAccessToolBar() const { return TRUE; }

	virtual void CopyFrom(const CMFCRibbonBaseElement& s);
	virtual void OnDrawOnList(CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);
	virtual BOOL OnKey(BOOL bIsMenuKey);
	virtual CMFCRibbonPanel* GetParentPanel() const { return m_pPanel; }

	HICON         m_hIcon;
	CMFCRibbonPanel* m_pPanel;
};

/*============================================================================*/
// CMFCRibbonPanel

class CMFCRibbonPanel : public CMFCBaseAccessibleObject  
{
	DECLARE_DYNCREATE(CMFCRibbonPanel)

	friend class CMFCRibbonCategory;
	friend class CMFCRibbonPanelMenuBar;
	friend class CMFCRibbonPanelMenu;
	friend class CMFCRibbonBar;
	friend class CMFCRibbonConstructor;

// Construction
protected:
	CMFCRibbonPanel(LPCTSTR lpszName = NULL, HICON hIcon = NULL);
	CMFCRibbonPanel(CMFCRibbonGallery* pPaletteButton);

	virtual void CopyFrom(CMFCRibbonPanel& src);

	void CommonInit(LPCTSTR lpszName = NULL, HICON hIcon = NULL);

public:
	virtual ~CMFCRibbonPanel();

// Attributes:
public:
	LPCTSTR GetName() const { return m_strName; }
	CMFCRibbonButton& GetDefaultButton() { return m_btnDefault; }

	void EnableLaunchButton(UINT uiCmdID, int nIconIndex = -1, LPCTSTR lpszKeys = NULL);
	CMFCRibbonButton& GetLaunchButton() { return m_btnLaunch; }

	BOOL IsHighlighted() const { return m_bIsHighlighted; }
	BOOL IsCollapsed() const; 	BOOL IsMenuMode() const { return m_bMenuMode; }
	virtual BOOL IsMainPanel() const { return FALSE; }
	const CRect& GetRect() const { return m_rect; }

	DWORD_PTR GetData() const { return m_dwData; }
	void SetData(DWORD_PTR dwData) { m_dwData = dwData; }

	CMFCRibbonCategory* GetParentCategory() const { return m_pParent; }
	CMFCRibbonPanelMenuBar* GetParentMenuBar() const { return m_pParentMenuBar; }
	CMFCRibbonBaseElement* GetParentButton() const;

	BOOL IsJustifyColumns() const { return m_bJustifyColumns; }
	void SetJustifyColumns(BOOL bSet = TRUE) { m_bJustifyColumns = bSet; }

	void SetKeys(LPCTSTR lpszKeys);
	int GetCaptionHeight() const { return m_rectCaption.Height(); }

	BOOL IsCenterColumnVert() const { return m_bCenterColumnVert; }
	void SetCenterColumnVert(BOOL bSet = TRUE) { m_bCenterColumnVert = bSet; }

	/// <summary>
	/// Indicates whether the parent ribbon has Windows 7-style look (small rectangular application button)</summary>
	/// <returns> 
	/// TRUE if the parent ribbon has Windows 7-style look; otherwise FALSE.</returns>
	BOOL IsWindows7Look() const;

// Operations
public:
	virtual CMFCRibbonBaseElement* HitTest(CPoint point, BOOL bCheckPanelCaption = FALSE);
	virtual int HitTestEx(CPoint point) const;
	virtual int GetIndex(CMFCRibbonBaseElement* pElem) const;
	virtual BOOL GetPreferedMenuLocation(CRect& /*rect*/) { return FALSE; }

	BOOL HasElement(const CMFCRibbonBaseElement* pElem) const;

	virtual void RecalcWidths(CDC* pDC, int nHeight);

	virtual void Add(CMFCRibbonBaseElement* pElem);
	virtual void AddSeparator();

	virtual BOOL Insert(CMFCRibbonBaseElement* pElem, int nIndex);
	virtual BOOL InsertSeparator(int nIndex);

	CMFCRibbonButtonsGroup* AddToolBar(UINT uiToolbarResID, UINT uiColdResID = 0, UINT uiHotResID = 0, UINT uiDisabledResID = 0);

	CMFCRibbonBaseElement* GetElement(int nIndex) const;
	CMFCRibbonBaseElement* FindByID(UINT uiCmdID) const;
	CMFCRibbonBaseElement* FindByData(DWORD_PTR dwData) const;

	BOOL SetElementMenu(UINT uiCmdID, HMENU hMenu, BOOL bIsDefautCommand = FALSE, BOOL bRightAlign = FALSE);
	BOOL SetElementMenu(UINT uiCmdID, UINT uiMenuResID, BOOL bIsDefautCommand = FALSE, BOOL bRightAlign = FALSE);

	BOOL Replace(int nIndex, CMFCRibbonBaseElement* pElem);
	BOOL ReplaceByID(UINT uiCmdID, CMFCRibbonBaseElement* pElem);

	CMFCRibbonBaseElement* SetElementRTC(int nIndex, CRuntimeClass* pRTC);
	CMFCRibbonBaseElement* SetElementRTCByID(UINT uiCmdID, CRuntimeClass* pRTC);

	int GetCount() const;

	BOOL Remove(int nIndex, BOOL bDelete = TRUE);
	void RemoveAll();

	void GetElements(CArray <CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);
	void GetItemIDsList(CList<UINT,UINT>& lstItems) const;
	void GetElementsByID(UINT uiCmdID, CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);
	/// <summary>
	/// Obtains an array of visible elements.</summary>
	/// <param name="arElements"> When the function returns this parameter contains an array of visible elements.</param>
	void GetVisibleElements(CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);

	CMFCRibbonBaseElement* GetDroppedDown() const;
	CMFCRibbonBaseElement* GetHighlighted() const;
	CMFCRibbonBaseElement* GetPressed() const;
	/// <summary>
	/// Returns a focused element. </summary>
	/// <returns> A pointer to a focused element or NULL.</returns>
	CMFCRibbonBaseElement* GetFocused() const;

	/// <summary>
	/// Sets focus to the specified Ribbon element.</summary>
	/// <param name="pNewFocus"> A pointer to a Ribbon element that receives focus.</param>
	void SetFocused(CMFCRibbonBaseElement* pNewFocus);

	CMFCRibbonPanelMenu* ShowPopup(CMFCRibbonDefaultPanelButton* pButton = NULL);

	/// <summary> 
	/// Scrolls the gallery to make the specified Ribbon element visible.</summary>
	/// <param name="pItem"> A pointer to a Ribbon element to show.</param>
	void MakeGalleryItemVisible(CMFCRibbonBaseElement* pItem);
	/// <summary> 
	/// Returns a bounding rectangle of Gallery element.</summary> 
	/// <returns> Size and position of Gallery element within this panel.</returns>
	CRect GetGalleryRect();

// Overrides
public:
	virtual BOOL OnKey(UINT nChar);

protected:
	virtual int GetHeight(CDC* pDC) const;

	virtual BOOL IsFixedSize() const { return FALSE; }

	virtual void DoPaint(CDC* pDC);
	virtual void Reposition(CDC* pDC, const CRect& rect);
	virtual void RepositionMenu(CDC* pDC, const CRect& rect);
	virtual void ReposPalette(CDC* pDC, const CRect& rect);
	virtual void OnUpdateCmdUI(CMFCRibbonCmdUI* pCmdUI, CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL NotifyControlCommand(BOOL bAccelerator, int nNotifyCode, WPARAM wParam, LPARAM lParam);
	virtual void OnAfterChangeRect(CDC* pDC);
	virtual void OnShow(BOOL bShow);
	virtual void RedrawElement(CMFCRibbonBaseElement* pElem);
	virtual void OnDrawMenuBorder(CDC* /*pDC*/, CMFCRibbonPanelMenuBar* /*pMenuBar*/) {}
	virtual void OnRTLChanged(BOOL bIsRTL);

	// Accessibility:
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	virtual BOOL OnSetAccData(long lVal);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

// Operations
protected:
	int GetMinWidth(CDC* pDC);
	void CenterElementsInColumn(int nFirstInColumnIndex, int nLastInColumnIndex, int nCaptionHeight);
	void JustifyElementsInColumn(int nFirstInColumnIndex, int nLastInColumnIndex);

	CSize GetCaptionSize(CDC* pDC) const;
	int CalcTotalWidth();

	void ShowDefaultButton(CDC* pDC);

	virtual void Highlight(BOOL bHighlight, CPoint point);
	virtual CMFCRibbonBaseElement* MouseButtonDown(CPoint point);
	virtual void MouseButtonUp(CPoint point);
	void CancelMode();

	CWnd* GetParentWnd() const;

	CMFCRibbonBaseElement* GetFirstTabStop() const;
	CMFCRibbonBaseElement* GetLastTabStop() const;

	void CleanUpSizes();
	void OnDrawPaletteMenu(CDC* pDC);
	void ScrollPalette(int nScrollOffset, BOOL bIsDelta = FALSE);
	CSize GetPaltteMinSize() const;

// Attributes
protected:
	BOOL m_bShowCaption;
	BOOL m_bForceCollpapse;
	BOOL m_bIsHighlighted;
	BOOL m_bIsCalcWidth;
	BOOL m_bAlignByColumn;
	BOOL m_bCenterColumnVert;
	BOOL m_bMenuMode;
	BOOL m_bIsDefaultMenuLook;
	BOOL m_bFloatyMode;
	BOOL m_bIsQATPopup;
	BOOL m_bIsFirst;
	BOOL m_bIsLast;
	BOOL m_bJustifyColumns;
	BOOL m_bSizeIsLocked;
	BOOL m_bScrollDnAvailable;
	BOOL m_bTruncateCaption;
	BOOL m_bMouseIsDown;

	int m_nCurrWidthIndex; // Index of the current width
	int m_nFullWidth;      // Full width
	int m_nRows;           // Rows number (m_bAlignByColumn == FALSE)
	int m_nXMargin;
	int m_nYMargin;
	int m_nScrollOffset;

	CRect m_rectCaption;
	CRect m_rect;          // Panel location
	CRect m_rectMenuAreaTop;
	CRect m_rectMenuAreaBottom;

	CString   m_strName;   // Panel name
	DWORD_PTR m_dwData;    // User-defined data

	CScrollBar*               m_pScrollBar;
	CMFCRibbonCategory*          m_pParent;        // Parent category
	CMFCRibbonPanelMenuBar*      m_pParentMenuBar; // Parent menu bar
	CMFCRibbonGallery*     m_pPaletteButton;
	CMFCRibbonLaunchButton       m_btnLaunch;
	CMFCRibbonBaseElement*       m_pHighlighted;   // Highlighted 
	CMFCRibbonDefaultPanelButton m_btnDefault;     // Default panel button

	CMap<int,int,int,int> m_mapNonOptWidths; // Optimized Width <-> Full Width
	CArray<int,int>       m_arWidths;        // All possible widths
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arElements;

	AFX_IMPORT_DATA static UINT m_nNextPanelID;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
