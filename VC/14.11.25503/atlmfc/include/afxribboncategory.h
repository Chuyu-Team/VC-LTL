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
#include "afxbaseribbonelement.h"
#include "afxtoolbarimages.h"
#include "afxribbonbar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCRibbonTab

class CMFCRibbonCategory;

class CMFCRibbonTab : public CMFCRibbonBaseElement
{
	DECLARE_DYNAMIC(CMFCRibbonTab)

	friend class CMFCRibbonCategory;
	friend class CMFCRibbonBar;

	CMFCRibbonTab();
	virtual void OnDraw(CDC* pDC);
	virtual CSize GetRegularSize(CDC* pDC);
	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonDblClk(CPoint point);
	virtual CString GetToolTipText() const;
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual CRect GetKeyTipRect(CDC* pDC, BOOL bIsMenu);
	virtual BOOL OnKey(BOOL bIsMenuKey);
	virtual BOOL IsShowTooltipOnBottom() const { return FALSE; }
	virtual void Redraw();

	AFX_RibbonCategoryColor m_Color;
	BOOL m_bIsTruncated;
	int  m_nFullWidth;

public:
	BOOL IsSelected() const;

	// Accessibility:
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);
	virtual void OnAccDefaultAction();

	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
};

/*============================================================================*/
// CRibbonCategoryScroll

class CRibbonCategoryScroll : public CMFCRibbonButton
{
	friend class CMFCRibbonCategory;

	CRibbonCategoryScroll();

	virtual void OnMouseMove(CPoint point);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnAutoRepeat();

	virtual BOOL IsAutoRepeatMode(int& /*nDelay*/) const { return TRUE; }
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void OnClick(CPoint /*point*/) { OnAutoRepeat(); }

	BOOL	m_bIsLeft;

public:
	BOOL IsLeftScroll() const { return m_bIsLeft; }
};

/*============================================================================*/
// CMFCRibbonCategory

class CMFCRibbonPanel;
class CMFCRibbonBar;

class CMFCRibbonCategory : public CMFCBaseAccessibleObject
{
	friend class CMFCRibbonBar;
	friend class CMFCRibbonTab;
	friend class CMFCRibbonPanel;
	friend class CMFCRibbonPanelMenuBar;
	friend class CMFCRibbonBaseElement;
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonTabsGroup;

	DECLARE_DYNCREATE(CMFCRibbonCategory)

// Construction
protected:
	CMFCRibbonCategory();

	CMFCRibbonCategory(CMFCRibbonBar* pParentRibbonBar, LPCTSTR lpszName, UINT uiSmallImagesResID,
		UINT uiLargeImagesResID, CSize sizeSmallImage = CSize(16, 16), CSize sizeLargeImage = CSize(32, 32));

	void CommonInit(CMFCRibbonBar* pParentRibbonBar = NULL, LPCTSTR lpszName = NULL,
		UINT uiSmallImagesResID = 0, UINT uiLargeImagesResID = 0, CSize sizeSmallImage = CSize(0, 0), CSize sizeLargeImage = CSize(0, 0));

// Attributes
public:
	CMFCRibbonBar* GetParentRibbonBar() const { return m_pParentRibbonBar; }
	CMFCRibbonPanelMenuBar* GetParentMenuBar() const { return m_pParentMenuBar; }

	CMFCRibbonBaseElement* HitTest(CPoint point, BOOL bCheckPanelCaption = FALSE) const;
	CMFCRibbonBaseElement* HitTestScrollButtons(CPoint point) const;
	int HitTestEx(CPoint point) const;
	CMFCRibbonPanel* GetPanelFromPoint(CPoint point) const;

	/// <summary>
	/// Obtain a first visible element that belong to the ribbon category</summary>
	/// <returns> 
	/// Pointer to the first visible element; may be NULL if category doesn't have any visible elements</returns>
	CMFCRibbonBaseElement* GetFirstVisibleElement() const;

	/// <summary>
	/// Obtain a last visible element that belong to the ribbon category</summary>
	/// <returns> 
	/// Pointer to the last visible element; may be NULL if category doesn't have any visible elements</returns>
	CMFCRibbonBaseElement* GetLastVisibleElement() const;

	CRect GetTabRect() const { return m_Tab.m_rect; }
	CRect GetRect() const { return m_rect; }

	BOOL IsActive() const { return m_bIsActive; }

	CSize GetImageSize(BOOL bIsLargeImage) const;

	int GetImageCount(BOOL bIsLargeImage) const
	{
		return bIsLargeImage ? m_LargeImages.GetCount() : m_SmallImages.GetCount();
	}

	LPCTSTR GetName() const { return m_strName; }
	void SetName(LPCTSTR lpszName);

	DWORD_PTR GetData() const { return m_dwData; }
	void SetData(DWORD_PTR dwData) { m_dwData = dwData; }

	UINT GetContextID() const { return m_uiContextID; }

	void SetTabColor(AFX_RibbonCategoryColor color) { m_Tab.m_Color = color; }
	AFX_RibbonCategoryColor GetTabColor() const { return m_Tab.m_Color; }

	BOOL IsVisible() const { return m_bIsVisible; }

	CMFCRibbonBaseElement* GetDroppedDown();

	/// <summary>
	/// Returns a focused element. </summary>
	/// <returns> A pointer to a focused element or NULL if no elements are focused.</returns>
	CMFCRibbonBaseElement* GetFocused();

	/// <summary>
	/// Returns a highlighted element. </summary>
	/// <returns> A pointer to a highlighted element or NULL if no elements are highlighted.</returns>
	CMFCRibbonBaseElement* GetHighlighted();
	CMFCRibbonBaseElement* GetParentButton() const;

	CMFCToolBarImages& GetSmallImages() { return m_SmallImages; }
	CMFCToolBarImages& GetLargeImages() { return m_LargeImages; }

	void SetKeys(LPCTSTR lpszKeys);

	/// <summary>
	/// Indicates whether the parent ribbon has Windows 7-style look (small rectangular application button)</summary>
	/// <returns> 
	/// TRUE if the parent ribbon has Windows 7-style look; otherwise FALSE.</returns>
	BOOL IsWindows7Look() const;

	CMFCRibbonTab* GetTab() { return &m_Tab; }

// Operations
public:
	CMFCRibbonPanel* AddPanel(LPCTSTR lpszPanelName, HICON hIcon = 0, CRuntimeClass* pRTI = NULL);
	BOOL RemovePanel (int nIndex, BOOL bDelete = TRUE);

	void SetCollapseOrder(const CArray<int, int>& arCollapseOrder);
	
	int GetPanelCount() const;
	CMFCRibbonPanel* GetPanel(int nIndex);

	int GetPanelIndex(const CMFCRibbonPanel* pPanel) const;

	int GetMaxHeight(CDC* pDC);

	CMFCRibbonBaseElement* FindByID(UINT uiCmdID, BOOL bVisibleOnly = TRUE) const;
	CMFCRibbonBaseElement* FindByData(DWORD_PTR dwData, BOOL bVisibleOnly = TRUE) const;

	CMFCRibbonPanel* HighlightPanel(CMFCRibbonPanel* pHLPanel, CPoint point);
	CMFCRibbonPanel* FindPanelWithElem(const CMFCRibbonBaseElement* pElement);

	void AddHidden(CMFCRibbonBaseElement* pElem);

	void GetElements(CArray <CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);
	void GetElementsByID(UINT uiCmdID, CArray <CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);

	/// <summary>
	/// Obtain all visible elements that belong to the ribbon category</summary>
	/// <param name="arElements">Array of all visible elements</param>
	void GetVisibleElements(CArray <CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);

	void GetItemIDsList(CList<UINT,UINT>& lstItems, BOOL bHiddenOnly = FALSE) const;

	int GetTextTopLine() const;

// Overrides
	virtual void RecalcLayout(CDC* pDC);
	virtual void OnDraw(CDC* pDC);
	virtual void OnCancelMode();
	virtual CMFCRibbonBaseElement* OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);
	virtual void OnMouseMove(CPoint point);
	virtual void OnUpdateCmdUI(CMFCRibbonCmdUI* pCmdUI, CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	virtual BOOL NotifyControlCommand(BOOL bAccelerator, int nNotifyCode, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnDrawImage(CDC* pDC, CRect rect, CMFCRibbonBaseElement* pElement, BOOL bIsLargeImage, BOOL nImageIndex, BOOL bCenter);

	virtual void CopyFrom(CMFCRibbonCategory& src);
	virtual void OnDrawMenuBorder(CDC* /*pDC*/, CMFCRibbonPanelMenuBar* /*pMenuBar*/)	{}
	virtual void OnRTLChanged(BOOL bIsRTL);

	virtual BOOL OnScrollHorz(BOOL bScrollLeft, int nScrollOffset = 0);
	virtual void ReposPanels(CDC* pDC);

	/// <summary>
	/// Called by the framework when a user presses a keyboard button.</summary>
	/// <param name="nChar">The virtual-key code for the key that a user pressed.</param>
	virtual BOOL OnKey(UINT nChar);

	//Accessibility
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual CWnd* GetParentWnd () const { return m_pParentRibbonBar; }
	virtual BOOL OnSetAccData (long lVal);
	virtual BOOL SetACCData (CWnd* pParent, CAccessibilityData& data);

// Implementation
public:
	virtual ~CMFCRibbonCategory();

protected:
	void SetActive(BOOL bIsActive = TRUE);
	void ShowElements(BOOL bShow = TRUE);

	void ShowFloating(CRect rectFloating);

	void RecalcPanelWidths(CDC* pDC);
	void CleanUpSizes();
	int GetMinWidth(CDC* pDC);

	BOOL SetPanelsLayout(int nWidth);
	void ResetPanelsLayout();

	void UpdateScrollButtons();
	void EnsureVisible(CMFCRibbonButton* pButton);

	BOOL m_bMouseIsPressed;
	BOOL m_bIsActive;
	BOOL m_bIsVisible;
	UINT m_uiContextID;
	int  m_nLastCategoryWidth;
	int  m_nLastCategoryOffsetY;

	DWORD_PTR             m_dwData;
	CRect                 m_rect;
	CString               m_strName;
	CMFCRibbonTab         m_Tab;
	CRibbonCategoryScroll m_ScrollLeft;
	CRibbonCategoryScroll m_ScrollRight;
	int                   m_nScrollOffset;
	CMFCRibbonBar*        m_pParentRibbonBar;

	CMFCRibbonPanelMenuBar* m_pParentMenuBar;

	int    m_nMinWidth;
	CArray<int, int>   m_arCollapseOrder;
	CArray<CMFCRibbonPanel*,CMFCRibbonPanel*> m_arPanels;

	clock_t  m_ActiveTime;

	//----------------------
	// Category image lists:
	//----------------------
	CMFCToolBarImages m_SmallImages;
	CMFCToolBarImages m_LargeImages;

	//---------------------------------
	// Category elements (non-visible):
	//---------------------------------
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arElements;

	UINT m_uiSmallImagesResID;
	UINT m_uiLargeImagesResID;

private:
	void NormalizeFloatingRect(CMFCRibbonBar* pRibbonBar, CRect& rectCategory);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
