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

#define AFX_IDW_RIBBON_BAR     0xE806  // CMFCRibbonBar
#define AFX_RIBBON_CAPTION_BUTTONS 3

#include "afxcontrolbarutil.h"

#include "afxpane.h"
#include "afxribbonbutton.h"
#include "afxribbonquickaccesstoolbar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef RT_RIBBON
#define RT_RIBBON MAKEINTRESOURCE(28)
#endif

class CMFCRibbonCategory;
class CMFCRibbonBar;
class CMFCRibbonApplicationButton;
class CMFCToolBarMenuButton;
class CMFCRibbonMainPanel;
class CMFCRibbonRichEditCtrl;
class CMFCRibbonKeyTip;
class CMFCRibbonPanel;

#define AFX_RIBBON_PROFILE  _T("MFCRibbons")

/*============================================================================*/
// AFX_RibbonCategoryColor

enum AFX_RibbonCategoryColor
{
	AFX_CategoryColor_None,
	AFX_CategoryColor_Red,
	AFX_CategoryColor_Orange,
	AFX_CategoryColor_Yellow,
	AFX_CategoryColor_Green,
	AFX_CategoryColor_Blue,
	AFX_CategoryColor_Indigo,
	AFX_CategoryColor_Violet
};

/*============================================================================*/
// CMFCRibbonCaptionButton

class CMFCRibbonCaptionButton : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonCaptionButton)

	friend class CMFCRibbonBar;

public:
	BOOL IsMDIChildButton() const { return m_hwndMDIChild != NULL; }

protected:
	CMFCRibbonCaptionButton(UINT uiCmd = 0, HWND hwndMDIChild = NULL);

	virtual void OnDraw(CDC* pDC);
	virtual void OnLButtonUp(CPoint point);

	virtual CSize GetRegularSize(CDC* pDC);
	virtual CSize GetCompactSize(CDC* pDC) { return GetRegularSize(pDC); }
	virtual BOOL IsShowTooltipOnBottom() const { return FALSE; }
	virtual int AddToListBox(CMFCRibbonCommandsListBox* /*pWndListBox*/, BOOL /*bDeep*/) { return -1; }
	virtual BOOL IsCaptionButton() const { return TRUE; }

	HWND m_hwndMDIChild;
};

/*============================================================================*/
// CMFCRibbonContextCaption

class CMFCRibbonContextCaption : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonContextCaption)

	friend class CMFCRibbonBar;

public:
	AFX_RibbonCategoryColor	GetColor() const { return m_Color; }
	int GetRightTabX() const { return m_nRightTabX; }
	UINT GetContextID() const { return m_uiID; }

	int GetContextCategoryCount();
	void GetContextCategories(CArray<CMFCRibbonCategory*,CMFCRibbonCategory*>& arCategories);
	int GetContextCaptionIndex(CMFCRibbonContextCaption* pContextCaption);

protected:
	CMFCRibbonContextCaption(LPCTSTR lpszName, UINT uiID, AFX_RibbonCategoryColor clrContext);
	CMFCRibbonContextCaption();

	virtual void OnDraw(CDC* pDC);
	virtual void OnLButtonUp(CPoint point);

	// Accessibility:
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual BOOL OnSetAccData (long lVal);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	AFX_RibbonCategoryColor m_Color;
	UINT m_uiID;
	int  m_nRightTabX;
};

/*============================================================================*/
// CMFCRibbonBar window

#define AFX_RIBBONBAR_HIDE_ELEMENTS  0x0001
#define AFX_RIBBONBAR_HIDE_ALL       0x0002

class CMFCRibbonBar : public CPane
{
	friend class CMFCRibbonCategory;
	friend class CMFCRibbonCustomizePropertyPage;
	friend class CPreviewViewEx;
	friend class CMFCRibbonApplicationButton;
	friend class CMFCRibbonPanelMenuBar;
	friend class CFrameImpl;
	friend class CMFCRibbonEdit;
	friend class CMFCRibbonPanel;
	friend class CMFCRibbonConstructor;
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonTabsGroup;
	friend class CMFCRibbonTab;

	DECLARE_DYNAMIC(CMFCRibbonBar)

// Construction
public:
	CMFCRibbonBar(BOOL bReplaceFrameCaption = TRUE);

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP, UINT nID = AFX_IDW_RIBBON_BAR);
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP, UINT nID = AFX_IDW_RIBBON_BAR);

	/// <summary>
	/// Loads a Ribbon Bar from application resources.</summary>
	/// <returns> TRUE if load succeeds; otherwise FALSE. </returns>
	/// <param name="uiXMLResID">Specifies resource ID of XML string with Ribbon Bar information.</param>
	/// <param name="lpszResType">Specifies type of the resource located at uiXMLResID.</param>
	/// <param name="hInstance">Handle to the module whose executable file contains the resource. If hModule is NULL, the system loads the resource from the module that was used to create the current process.</param>
	virtual BOOL LoadFromResource(UINT uiXMLResID, LPCTSTR lpszResType = RT_RIBBON, HINSTANCE hInstance = NULL);
	/// <summary>
	/// Loads a Ribbon Bar from application resources.</summary>
	/// <returns> TRUE if load succeeds; otherwise FALSE. </returns>
	/// <param name="lpszXMLResID">Specifies resource ID (in string form) with Ribbon Bar information.</param>
	/// <param name="lpszResType">Specifies type of the resource located at uiXMLResID.</param>
	/// <param name="hInstance">Handle to the module whose executable file contains the resource. If hModule is NULL, the system loads the resource from the module that was used to create the current process.</param>
	virtual BOOL LoadFromResource(LPCTSTR lpszXMLResID, LPCTSTR lpszResType = RT_RIBBON, HINSTANCE hInstance = NULL);
	/// <summary>
	/// Loads a Ribbon Bar from application resources.</summary>
	/// <returns> TRUE if load succeeds; otherwise FALSE. </returns>
	/// <param name="lpszXMLBuffer">A buffer with XML string to load the Ribbon Bar from.</param>
	virtual BOOL LoadFromBuffer(LPCTSTR lpszXMLBuffer);

// Operations
public:
	/// <summary>
	/// Enable/disable ribbon Windows 7-style look (small rectangular application button)</summary>
	/// <param name="bWindows7Look">TRUE - set Windows 7-style look; FALSE - otherwise.</param>
	/// <param name="bRecalc">TRUE - recalculate the ribbon layout; FALSE - otherwise.</param>
	void SetWindows7Look(BOOL bWindows7Look, BOOL bRecalc = TRUE);

	/// <summary>
	/// Indicates whether the ribbon has Windows 7-style look (small rectangular application button)</summary>
	/// <returns> 
	/// TRUE if the ribbon has Windows 7-style look; otherwise FALSE.</returns>
	BOOL IsWindows7Look() const
	{
		return m_bWindows7Look;
	}

	virtual void RecalcLayout();

	//----------------------------------------------------------------------
	// Enable/disable ribbon main button (large rounded button on top left):
	//----------------------------------------------------------------------
	void SetApplicationButton(CMFCRibbonApplicationButton* pButton, CSize sizeButton);

	//--------------------------
	// Ribbon categories (tabs):
	//--------------------------
	CMFCRibbonMainPanel* AddMainCategory(LPCTSTR lpszName, UINT uiSmallImagesResID, UINT uiLargeImagesResID,
		CSize sizeSmallImage = CSize(16, 16), CSize sizeLargeImage = CSize(32, 32), CRuntimeClass* pRTI = NULL);
	CMFCRibbonCategory* AddCategory(LPCTSTR lpszName, UINT uiSmallImagesResID, UINT uiLargeImagesResID,
		CSize sizeSmallImage = CSize(16, 16), CSize sizeLargeImage = CSize(32, 32), int nInsertAt = -1, CRuntimeClass* pRTI = NULL);
	CMFCRibbonCategory* AddContextCategory(LPCTSTR lpszName, LPCTSTR lpszContextName, UINT uiContextID, AFX_RibbonCategoryColor clrContext,
		UINT uiSmallImagesResID, UINT uiLargeImagesResID, CSize sizeSmallImage = CSize(16, 16), CSize sizeLargeImage = CSize(32, 32), CRuntimeClass* pRTI = NULL);
	CMFCRibbonCategory* AddQATOnlyCategory(LPCTSTR lpszName, UINT uiSmallImagesResID, CSize sizeSmallImage = CSize(16, 16));
	CMFCRibbonCategory* AddPrintPreviewCategory();

	void EnablePrintPreview(BOOL bEnable = TRUE);
	BOOL IsPrintPreviewEnabled() const { return m_bIsPrintPreview; }

	int GetCategoryCount() const;
	int GetVisibleCategoryCount() const;
	CMFCRibbonCategory* GetCategory(int nIndex) const;
	int GetCategoryIndex(CMFCRibbonCategory* pCategory) const;

	void ShowCategory(int nIndex, BOOL bShow = TRUE);
	void ShowContextCategories(UINT uiContextID, BOOL bShow = TRUE);
	BOOL HideAllContextCategories();
	BOOL ActivateContextCategory(UINT uiContextID);

	BOOL RemoveCategory(int nIndex);
	void RemoveAllCategories();

	virtual BOOL SetActiveCategory(CMFCRibbonCategory* pCategory, BOOL bForceRestore = FALSE);
	CMFCRibbonCategory* GetActiveCategory() const { return m_pActiveCategory; }

	int FindCategoryIndexByData(DWORD dwData) const;
	BOOL GetContextName(UINT uiContextID, CString& strName) const;

	//-------------------------------
	// Ribbon elements direct access:
	//-------------------------------
	CMFCRibbonBaseElement* FindByID(UINT uiCmdID, BOOL bVisibleOnly = TRUE, BOOL bExcludeQAT = FALSE) const;
	CMFCRibbonBaseElement* FindByData(DWORD_PTR dwData, BOOL bVisibleOnly = TRUE) const;

	BOOL SetElementKeys(UINT uiCmdID, LPCTSTR lpszKeys, LPCTSTR lpszMenuKeys = NULL);

	void GetElementsByID(UINT uiCmdID, CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons);
	void GetVisibleElements(CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons);

	void SetQuickAccessDefaultState(const CMFCRibbonQuickAccessToolBarDefaultState& state);
	void SetQuickAccessCommands(const CList<UINT,UINT>& lstCommands, BOOL bRecalcLayout = TRUE);
	void GetQuickAccessCommands(CList<UINT,UINT>& lstCommands);

	//--------------------------------------------------
	// Additional elements located on the right of tabs:
	//--------------------------------------------------
	void AddToTabs(CMFCRibbonBaseElement* pElement);
	void RemoveAllFromTabs();

	//------------------
	// Tooltips support:
	//------------------
	void EnableToolTips(BOOL bEnable = TRUE, BOOL bEnableDescr = TRUE);
	
	BOOL IsToolTipEnabled() const { return m_bToolTip; }
	BOOL IsToolTipDescrEnabled() const { return m_bToolTipDescr; }

	int GetTooltipFixedWidthRegular() const { return m_nTooltipWidthRegular; }
	int GetTooltipFixedWidthLargeImage() const { return m_nTooltipWidthLargeImage; }

	void SetTooltipFixedWidth(int nWidthRegular, int nWidthLargeImage);	// 0 - set variable size

	//-----------------
	// Key tip support:
	//-----------------
	void EnableKeyTips(BOOL bEnable = TRUE);

	BOOL IsKeyTipEnabled() const { return m_bKeyTips; }

	void GetItemIDsList(CList<UINT,UINT>& lstItems, BOOL bHiddenOnly = FALSE) const;

	void ToggleMimimizeState();

	BOOL OnSysKeyDown(CFrameWnd* pFrameWnd, WPARAM wParam, LPARAM lParam);
	BOOL OnSysKeyUp(CFrameWnd* pFrameWnd, WPARAM wParam, LPARAM lParam);

	void EnableSingleLevelAccessibilityMode(BOOL bEnable = TRUE)
	{
		m_bSingleLevelAccessibilityMode = bEnable;
	}

	BOOL IsSingleLevelAccessibilityMode() const
	{
		return m_bSingleLevelAccessibilityMode;
	}

	//--------------------
	// Save ribbon to XML:
	//--------------------
	/// <summary> Saves the Ribbon Bar to XML file.
	/// </summary>
	/// <param name="lpszFilePath">Specifies the output file.</param>
	/// <returns>TRUE if succeeds; othewise FALSE.</returns>
	BOOL SaveToXMLFile(LPCTSTR lpszFilePath) const;
	/// <summary> Saves the Ribbon Bar to a buffer.
	/// </summary>
	/// <param name="ppBuffer">When this function returns, ppBuffer points to a buffer allocated by this method and 
	/// containing Ribbon Bar information in XML format.</param>
	/// <returns>TRUE if succeeds; othewise FALSE.</returns>
	UINT SaveToXMLBuffer(LPBYTE* ppBuffer) const;

// Attributes
public:
	int GetCaptionHeight() const { return m_nCaptionHeight; }
	int GetCategoryHeight() const { return m_nCategoryHeight; }
	BOOL IsReplaceFrameCaption() const { return m_bReplaceFrameCaption; }
	CMFCRibbonApplicationButton* GetApplicationButton() const { return m_pMainButton; }
	CMFCRibbonCategory* GetMainCategory() const { return m_pMainCategory; }

	CMFCRibbonQuickAccessToolBar* GetQuickAccessToolbar () { return &m_QAToolbar; }

	virtual CMFCRibbonBaseElement* HitTest(CPoint point, BOOL bCheckActiveCategory = FALSE, BOOL bCheckPanelCaption = FALSE);

	//---------------------------------
	// Quick access toolbar attributes:
	//---------------------------------
	void SetQuickAccessToolbarOnTop(BOOL bOnTop);

	BOOL IsQuickAccessToolbarOnTop() const { return m_bQuickAccessToolbarOnTop && m_bReplaceFrameCaption; }
	CRect GetQuickAccessToolbarLocation() const { return m_QAToolbar.GetRect(); }
	CRect GetQATCommandsLocation() const { return m_QAToolbar.GetCommandsRect(); }

	BOOL IsQATEmpty() const { return (int) m_QAToolbar.m_arButtons.GetSize() <= 1; }
	CMFCRibbonBaseElement* GetQATDroppedDown() { return m_QAToolbar.GetDroppedDown(); }

	DWORD GetHideFlags() const { return m_dwHideFlags; }
	int GetTabTruncateRatio() const { return m_nTabTruncateRatio; }

	void SetMaximizeMode(BOOL bMax, CWnd* pWnd = NULL);
	void SetActiveMDIChild(CWnd* pWnd);

	virtual CMFCRibbonBaseElement* GetDroppedDown();
	/// <summary>
	/// Returns a focused element. </summary>
	/// <returns> A pointer to a focused element or NULL.</returns>
	virtual CMFCRibbonBaseElement* GetFocused();

	BOOL IsTransparentCaption() const { return m_bIsTransparentCaption; }
	int GetKeyboardNavigationLevel() const { return m_nKeyboardNavLevel; }

	void SetKeyboardNavigationLevel(CObject* pLevel, BOOL bSetFocus = TRUE);

	CObject* GetKeyboardNavLevelParent() const { return m_pKeyboardNavLevelParent; }
	CObject* GetKeyboardNavLevelCurrent() const { return m_pKeyboardNavLevelCurrent; }

	// Accessibility:
	virtual BOOL OnSetAccData(long lVal);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);

	CMFCRibbonTabsGroup* GetTabs() { return &m_Tabs;}

	LPDISPATCH GetAccessibleDispatch();
	BOOL IsCaptionButtons();
	CMFCBaseAccessibleObject* AccessibleObjectByIndex(long lVal);
	CMFCBaseAccessibleObject* AccessibleObjectFromPoint(CPoint point);
	int GetAccObjectCount();

protected:
	int m_nTabsHeight;
	int m_nCategoryHeight;
	int m_nCategoryMinWidth;
	int m_nHighlightedTab;
	int m_nCaptionHeight;
	int m_nTabTruncateRatio;
	int m_nSystemButtonsNum;
	int m_nKeyboardNavLevel;
	int m_nCurrKeyChar;
	int m_nTooltipWidthRegular;
	int m_nTooltipWidthLargeImage;

	BOOL m_bRecalcCategoryHeight;
	BOOL m_bRecalcCategoryWidth;
	BOOL m_bTracked;
	BOOL m_bIsPrintPreview;
	BOOL m_bQuickAccessToolbarOnTop;
	BOOL m_bForceRedraw;
	BOOL m_bMaximizeMode;
	BOOL m_bAutoCommandTimer;
	BOOL m_bPrintPreviewMode;
	BOOL m_bIsTransparentCaption;
	BOOL m_bIsMaximized;
	BOOL m_bToolTip;
	BOOL m_bToolTipDescr;
	BOOL m_bKeyTips;
	BOOL m_bIsCustomizeMenu;
	BOOL m_bDontSetKeyTips;
	BOOL m_bAutoDestroyMainButton;

	const BOOL m_bReplaceFrameCaption;

	HFONT m_hFont;
	DWORD m_dwHideFlags;

	CMFCRibbonApplicationButton*  m_pMainButton;
	CMFCRibbonBaseElement* m_pHighlighted;
	CMFCRibbonBaseElement* m_pPressed;
	CMFCRibbonButtonsGroup m_TabElements;
	CMFCRibbonCategory*    m_pActiveCategory;
	CMFCRibbonCategory*    m_pActiveCategorySaved;
	CMFCRibbonCategory*    m_pMainCategory;
	CMFCRibbonCategory*    m_pPrintPreviewCategory;

	CArray<CMFCRibbonContextCaption*, CMFCRibbonContextCaption*> m_arContextCaptions;
	CArray<CMFCRibbonCategory*,CMFCRibbonCategory*> m_arCategories;
	CArray<CMFCRibbonKeyTip*,CMFCRibbonKeyTip*> m_arKeyElements;
	CArray<int,int> m_arVisibleCategoriesSaved;

	CRect m_rectCaption;
	CRect m_rectCaptionText;
	CRect m_rectSysButtons;
	CSize m_sizeMainButton;

	CToolTipCtrl* m_pToolTip;
	CObject*      m_pKeyboardNavLevelParent;
	CObject*      m_pKeyboardNavLevelCurrent;

	CMFCRibbonCaptionButton      m_CaptionButtons[AFX_RIBBON_CAPTION_BUTTONS];
	CMFCRibbonQuickAccessToolBar m_QAToolbar;
	BOOL							m_bSingleLevelAccessibilityMode;
	CMFCRibbonTabsGroup			m_Tabs;

	BOOL m_bWindows7Look;

// Overrides
public:
	virtual void OnClickButton(CMFCRibbonButton* pButton, CPoint point);
	virtual BOOL IsMainRibbonBar() const { return TRUE; }
	virtual BOOL IsShowGroupBorder(CMFCRibbonButtonsGroup* /*pGroup*/) const { return FALSE; }
	virtual void OnEditContextMenu(CMFCRibbonRichEditCtrl* pEdit, CPoint point);

	virtual void DWMCompositionChanged();

	virtual BOOL OnShowRibbonQATMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);

	virtual BOOL TranslateChar(UINT nChar);

	virtual void OnRTLChanged(BOOL bIsRTL);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void OnPaneContextMenu(CWnd* pParentFrame, CPoint point);

protected:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }

	virtual void OnFillBackground(CDC* pDC, CRect rectClient);

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual void OnSetPrintPreviewKeys(CMFCRibbonPanel* pPrintPanel, CMFCRibbonPanel* pZoomPanel, CMFCRibbonPanel* pPreviewPanel);
	virtual BOOL HideInPrintPreviewMode() const { return FALSE; }
	virtual void OnBeforeProcessKey(int& nChar);

	BOOL NavigateRibbon(int nChar);

	static CMFCRibbonBaseElement* __stdcall FindNextFocusedElement(int nChar, const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElems, CRect rectElems, CMFCRibbonBaseElement* pFocused, BOOL bIsScrollLeftAvailable, BOOL bIsScrollRightAvailable, int& nScroll);
	static CMFCRibbonBaseElement* __stdcall FindNearest(CPoint pt, const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons);

// Implementation
public:
	virtual ~CMFCRibbonBar();

	void PopTooltip();
	BOOL DrawMenuImage(CDC* pDC, const CMFCToolBarMenuButton* pMenuItem, const CRect& rectImage);

	virtual BOOL OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);

	void ForceRecalcLayout();
	void DeactivateKeyboardFocus(BOOL bSetFocus = TRUE);

	void ShowKeyTips(BOOL bRepos = FALSE);
	void HideKeyTips();

	int GetVisibleContextCaptionCount(); 
	void GetVisibleContextCaptions(CArray<int, int>* arCaptions);
	void GetVisibleContextCaptions(CArray<CMFCRibbonContextCaption*, CMFCRibbonContextCaption*>& arCaptions);

protected:
	void ShowSysMenu(const CPoint& point);
	void SetPrintPreviewMode(BOOL bSet = TRUE);

	CMFCRibbonContextCaption* FindContextCaption(UINT uiID) const;

	void UpdateToolTipsRect();
	BOOL ProcessKey(int nChar);

	void RemoveAllKeys();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg void OnMouseLeave();
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnPostRecalcLayout(WPARAM,LPARAM);
	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CMFCRibbonBar idle update through CMFCRibbonCmdUI class

class CMFCRibbonCmdUI : public CCmdUI
{
public:
	CMFCRibbonCmdUI();

	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetRadio(BOOL bOn = TRUE);

	CMFCRibbonBaseElement* m_pUpdated;
};

/*============================================================================*/
// CMFCRibbonApplicationButton

class CMFCRibbonApplicationButton : public CMFCRibbonButton
{
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonConstructor;

	DECLARE_DYNCREATE(CMFCRibbonApplicationButton);

public:
	CMFCRibbonApplicationButton() { }

	CMFCRibbonApplicationButton(UINT uiBmpResID) { SetImage(uiBmpResID); }
	CMFCRibbonApplicationButton(HBITMAP hBmp) { SetImage(hBmp); }

	void SetImage(UINT uiBmpResID);
	void SetImage(HBITMAP hBmp);
	void SetWindows7Image(UINT uiBmpResID);
	void SetWindows7Image(HBITMAP hBmp);

protected:
	virtual BOOL IsShowTooltipOnBottom() const { return FALSE; }
	virtual BOOL IsApplicationButton() const { return TRUE; }
	virtual BOOL CanBeAddedToQuickAccessToolBar() const { return FALSE; }
	virtual int AddToListBox(CMFCRibbonCommandsListBox* /*pWndListBox*/, BOOL /*bDeep*/) { return -1; }

	virtual CSize GetImageSize(RibbonImageType /*type*/) const 
	{
		ASSERT_VALID(this);
		return m_Image.GetImageSize();
	}

	virtual void DrawImage(CDC* pDC, RibbonImageType /*type*/, CRect rectImage);

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonDblClk(CPoint point);
	virtual void OnAccDefaultAction() { ShowMainMenu(); }
	virtual BOOL OnKey(BOOL bIsMenuKey);

	virtual BOOL IsDrawTooltipImage() const { return FALSE; }

	BOOL ShowMainMenu();

	// Accessibility:
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	CMFCToolBarImages m_Image;
	CMFCToolBarImages m_ImageWindows7;
};

extern AFX_IMPORT_DATA UINT AFX_WM_ON_CHANGE_RIBBON_CATEGORY;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_RIBBON_CUSTOMIZE;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_HIGHLIGHT_RIBBON_LIST_ITEM;
extern AFX_IMPORT_DATA UINT AFX_WM_ON_BEFORE_SHOW_RIBBON_ITEM_MENU;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
