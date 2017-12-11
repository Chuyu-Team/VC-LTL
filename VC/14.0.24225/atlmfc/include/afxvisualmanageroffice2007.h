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

#include "afxvisualmanageroffice2003.h"
#include "afxtoolbarimages.h"
#include "afxcontrolrenderer.h"
#include "afxtooltipctrl.h"
#include "afxglobals.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_RIBBON_CATEGORY_COLOR_COUNT 7

class CMFCVisualManagerBitmapCache
{
public:
	class CMFCVisualManagerBitmapCacheItem
	{
	public:

		CMFCVisualManagerBitmapCacheItem();
		virtual ~CMFCVisualManagerBitmapCacheItem();

		void AddImage(HBITMAP hBmp);
		void Cache(const CSize& size, CMFCControlRenderer& renderer);

		void Draw(CDC* pDC, CRect rect, int iImageIndex = 0, BYTE alphaSrc = 255);
		void DrawY(CDC* pDC, CRect rect, CSize sides, int iImageIndex = 0, BYTE alphaSrc = 255);

	protected:
		CMFCToolBarImages m_Images;
		BOOL m_bMirror;
	};

	typedef CArray<CMFCVisualManagerBitmapCacheItem*, CMFCVisualManagerBitmapCacheItem*> XCache;
	typedef CArray<CSize, CSize> XSizes;

public:

	CMFCVisualManagerBitmapCache();
	virtual ~CMFCVisualManagerBitmapCache();

	void Clear();

	int Cache(const CSize& size, CMFCControlRenderer& renderer);
	int CacheY(int height, CMFCControlRenderer& renderer);
	BOOL IsCached(const CSize& size) const;

	int FindIndex(const CSize& size) const;

	CMFCVisualManagerBitmapCacheItem* Get(const CSize& size);
	CMFCVisualManagerBitmapCacheItem* Get(int nIndex);

protected:
	XCache m_Cache;
	XSizes m_Sizes;
};

class CMFCVisualManagerOffice2007 : public CMFCVisualManagerOffice2003
{
	DECLARE_DYNCREATE(CMFCVisualManagerOffice2007)

protected:
	CMFCVisualManagerOffice2007();
	virtual ~CMFCVisualManagerOffice2007();

public:
	enum Style
	{
		Office2007_LunaBlue,
		Office2007_ObsidianBlack,
		Office2007_Aqua,
		Office2007_Silver
	};

	static BOOL __stdcall SetStyle(Style style, LPCTSTR lpszPath = NULL);
	static Style __stdcall GetStyle();
	static void __stdcall SetResourceHandle(HINSTANCE hinstRes);
	static void __stdcall CleanStyle();

	virtual void OnUpdateSystemColors();

	BOOL IsRibbonPresent(CWnd* pWnd) const;
	CMFCRibbonBar*	GetRibbonBar(CWnd* pWnd) const;

	virtual BOOL IsOwnerDrawCaption() { return CanDrawImage() && !GetGlobalData()->IsDwmCompositionEnabled(); }

	virtual BOOL OnNcActivate(CWnd* pWnd, BOOL bActive);
	virtual BOOL OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
	virtual BOOL OnSetWindowRegion(CWnd* pWnd, CSize sizeWindow);

	virtual CSize GetNcBtnSize(BOOL bSmall) const;

public:
	virtual BOOL IsOwnerDrawMenuCheck();
	virtual BOOL IsHighlightWholeMenuItem();

	virtual COLORREF GetStatusBarPaneTextColor(CMFCStatusBar* pStatusBar, CMFCStatusBarPaneInfo* pPane);
	virtual COLORREF GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual COLORREF GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled);
	virtual COLORREF GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton);

public:
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual void OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton);
	virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	virtual void OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual COLORREF OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect,
		BOOL bIsPressed, BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);

	virtual int GetShowAllMenuItemsHeight(CDC* pDC, const CSize& sizeDefault);
	virtual void OnDrawShowAllMenuItems(CDC* pDC, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnDrawStatusBarPaneBorder(CDC* pDC, CMFCStatusBar* pBar, CRect rectPane, UINT uiID, UINT nStyle);
	virtual void OnDrawStatusBarSizeBox(CDC* pDC, CMFCStatusBar* pStatBar, CRect rectSizeBox);

	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, CMFCToolBarEditBoxButton* pButton);
	virtual void OnDrawTearOffCaption(CDC* pDC, CRect rect, BOOL bIsActive);
	virtual void OnDrawMenuResizeBar(CDC* pDC, CRect rect, int nResizeFlags);
	virtual void OnDrawMenuScrollButton(CDC* pDC, CRect rect, BOOL bIsScrollDown, BOOL bIsHighlited, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight);	

	virtual COLORREF OnDrawPropertySheetListItem(CDC* pDC, CMFCPropertySheet* pParent, CRect rect, BOOL bIsHighlihted, BOOL bIsSelected);

	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonSeparator(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state, BOOL bHorz);

	virtual void OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	virtual void OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed);
	virtual void OnDrawMenuCheck(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect, BOOL bHighlight, BOOL bIsRadio);
	
	virtual void OnDrawMenuItemButton(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rectButton, BOOL bHighlight, BOOL bDisabled);

	// MiniFrame:
	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);
	virtual void OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize);
	virtual void OnDrawFloatingToolbarBorder(	CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize);

	// tabs
	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack,
		COLORREF& clrHighlight, COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual BOOL OnEraseTabsFrame(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTabsButtonBorder(CDC* pDC, CRect& rect, CMFCButton* pButton, UINT uiState, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnFillTab(CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual BOOL AlwaysHighlight3DTabs() const { return CanDrawImage() ? TRUE : CMFCVisualManagerOffice2003::AlwaysHighlight3DTabs(); }
	virtual COLORREF GetTabTextColor(const CMFCBaseTabCtrl* pTabWnd, int iTab, BOOL bIsActive);
	virtual int GetTabHorzMargin(const CMFCBaseTabCtrl* pTabWnd);
	virtual void AdjustTabTextRect(CRect& rectTabText) { rectTabText.OffsetRect(-3, 0); }

	virtual void OnDrawCaptionBarInfoArea(CDC* pDC, CMFCCaptionBar* pBar, CRect rect);
	virtual COLORREF GetCaptionBarTextColor(CMFCCaptionBar* pBar);
	
	// Outlook bar:
	virtual void OnFillOutlookPageButton(CDC* pDC, const CRect& rect, BOOL bIsHighlighted, BOOL bIsPressed, COLORREF& clrText);
	virtual void OnDrawOutlookPageButtonBorder(CDC* pDC, CRect& rectBtn, BOOL bIsHighlighted, BOOL bIsPressed);
	virtual void OnDrawOutlookBarSplitter(CDC* pDC, CRect rectSplitter);
	virtual void OnFillOutlookBarCaption(CDC* pDC, CRect rectCaption, COLORREF& clrText);

	virtual COLORREF GetToolbarDisabledTextColor()
	{
		return CanDrawImage() ? m_clrToolBarBtnTextDisabled : CMFCVisualManagerOffice2003::GetToolbarDisabledTextColor();
	}

	virtual void OnDrawTasksGroupCaption(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);
	virtual void OnDrawTask(CDC* pDC, CMFCTasksPaneTask* pTask, CImageList* pIcons, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE);
	virtual void OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize,int iImage, BOOL bHilited);
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed,BOOL bEnabled);
	
	// Ribbon control:
	virtual COLORREF OnDrawRibbonTabsFrame(CDC* pDC, CMFCRibbonBar* pWndRibbonBar, CRect rectTab);
	
	virtual void OnDrawRibbonApplicationButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory);

	virtual COLORREF OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive);
	virtual COLORREF OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption);
	virtual void OnDrawRibbonCategoryScroll(CDC* pDC, CRibbonCategoryScroll* pScroll);

	virtual void OnDrawRibbonPanelCaption(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectCaption);
	virtual void OnDrawRibbonLaunchButton(CDC* pDC, CMFCRibbonLaunchButton* pButton, CMFCRibbonPanel* pPanel);

	virtual COLORREF OnFillRibbonButton(CDC* pDC, CMFCRibbonButton* pButton);

	virtual void OnFillRibbonEdit(CDC* pDC, CMFCRibbonRichEditCtrl* pEdit, CRect rect,
		BOOL bIsHighlighted, BOOL bIsPaneHighlighted, BOOL bIsDisabled, COLORREF& clrText, COLORREF& clrSelBackground, COLORREF& clrSelText);
	virtual void OnDrawRibbonButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonMenuCheckFrame(CDC* pDC, CMFCRibbonButton* pButton, CRect rect);
	virtual void OnDrawRibbonDefaultPaneButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonDefaultPaneButtonIndicator(CDC* pDC, CMFCRibbonButton* pButton, CRect rect, BOOL bIsSelected, BOOL bHighlighted);
	virtual void OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rectCaption, CRect rectText);

	virtual int GetRibbonQuickAccessToolBarChevronOffset() { return IsBeta1() ? 0 : 13; }
	virtual int GetRibbonQuickAccessToolBarRightMargin();

	virtual COLORREF GetRibbonQuickAccessToolBarTextColor(BOOL bDisabled = FALSE) { return bDisabled ? m_clrRibbonCategoryTextDisabled : m_clrRibbonCategoryText; }

	virtual void OnDrawRibbonCaptionButton(CDC* pDC, CMFCRibbonCaptionButton* pButton);

	virtual COLORREF OnDrawRibbonButtonsGroup(CDC* pDC, CMFCRibbonButtonsGroup* pGroup, CRect rectGroup);

	virtual void OnDrawDefaultRibbonImage(CDC* pDC, CRect rectImage, BOOL bIsDisabled = FALSE, BOOL bIsPressed = FALSE, BOOL bIsHighlighted = FALSE);
	virtual void OnDrawRibbonMainPanelFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual void OnFillRibbonMenuFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual void OnDrawRibbonRecentFilesFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);

	virtual COLORREF OnFillRibbonMainPanelButton(CDC* pDC, CMFCRibbonButton* pButton);

	virtual void OnDrawRibbonMainPanelButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonGalleryButton(CDC* pDC, CMFCRibbonGalleryIcon* pButton);
	virtual void OnDrawRibbonGalleryBorder(CDC* pDC, CMFCRibbonGallery* pButton, CRect rectBorder);

	virtual COLORREF OnDrawRibbonCategoryCaption(CDC* pDC, CMFCRibbonContextCaption* pContextCaption);
	virtual COLORREF OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane);
	
	virtual void OnDrawRibbonSliderZoomButton(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsZoomOut, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonSliderChannel(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect);
	virtual void OnDrawRibbonSliderThumb(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* pProgress, CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode);
	virtual void OnFillRibbonQuickAccessToolBarPopup(CDC* pDC, CMFCRibbonPanelMenuBar* pMenuBar, CRect rect);

	virtual int GetRibbonPopupBorderSize(const CMFCRibbonPanelMenu* pPopup) const;
	virtual BOOL IsLayeredRibbonKeyTip() const { return CanDrawImage(); }

	virtual void OnDrawRibbonKeyTip(CDC* pDC, CMFCRibbonBaseElement* pElement, CRect rect, CString str);
	virtual void OnDrawRibbonCheckBoxOnList(CDC* pDC, CMFCRibbonCheckBox* pCheckBox, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual COLORREF GetRibbonHyperlinkTextColor(CMFCRibbonLinkCtrl* pHyperLink);
	virtual COLORREF GetRibbonStatusBarTextColor(CMFCRibbonStatusBar* pStatusBar);

	virtual COLORREF GetRibbonEditBackgroundColor(CMFCRibbonRichEditCtrl* pEdit, BOOL bIsHighlighted, BOOL bIsPaneHighlighted, BOOL bIsDisabled);

	virtual void OnFillPopupWindowBackground(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd);
	virtual BOOL OnEraseMDIClientArea(CDC* pDC, CRect rectClient);

	// ToolTip
	virtual BOOL GetToolTipInfo(CMFCToolTipInfo& params, UINT nType = (UINT)(-1));

	virtual AFX_SMARTDOCK_THEME GetSmartDockingTheme();

protected:
	BOOL CanDrawImage() const
	{
		return GetGlobalData()->m_nBitsPerPixel > 8 && !GetGlobalData()->IsHighContrastMode() && m_bLoaded;
	}

	static CString __stdcall MakeResourceID(LPCTSTR lpszID);
	static CString __stdcall GetStyleResourceID(Style style);

	virtual void CleanUp();

	void DrawSeparator(CDC* pDC, const CRect& rect, BOOL bHorz);
	void DrawSeparator(CDC* pDC, const CRect& rect, CPen& pen1, CPen& pen2, BOOL bHorz);
	void DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI = FALSE);
	void DrawNcText(CDC* pDC, CRect& rect, const CString& strTitle, const CString& strDocument, BOOL bPrefix,
		BOOL bActive, BOOL bIsRTL, BOOL bTextCenter, BOOL bGlass = FALSE, int nGlassGlowSize = 0, COLORREF clrGlassText = (COLORREF)-1);
	void DrawNcCaption(CDC* pDC, CRect rectCaption, DWORD dwStyle, DWORD dwStyleEx, const CString& strTitle,
		const CString& strDocument, HICON hIcon, BOOL bPrefix, BOOL bActive, BOOL bTextCenter, const CObList& lstSysButtons);

	BOOL m_bLoaded;
	BOOL m_bNcTextCenter;
	BOOL m_bPlannerBlack;
	BOOL m_bToolTipParams;

	CSize m_szNcBtnSize[2];

	CMFCToolBarImages m_ToolBarGripper;
	CMFCToolBarImages m_ToolBarTear;
	CMFCToolBarImages m_StatusBarPaneBorder;
	CMFCToolBarImages m_StatusBarSizeBox;
	CMFCToolBarImages m_SysBtnClose[2];
	CMFCToolBarImages m_SysBtnRestore[2];
	CMFCToolBarImages m_SysBtnMaximize[2];
	CMFCToolBarImages m_SysBtnMinimize[2];
	CMFCToolBarImages m_PopupResizeBar_HV;
	CMFCToolBarImages m_PopupResizeBar_HVT;
	CMFCToolBarImages m_PopupResizeBar_V;
	CMFCToolBarImages m_MenuItemMarkerC;
	CMFCToolBarImages m_MenuItemMarkerR;
	CMFCToolBarImages m_RibbonPanelSeparator;
	CMFCToolBarImages m_RibbonBtnLaunchIcon;
	CMFCToolBarImages m_RibbonBtnDefaultImage;

	CMFCControlRenderer m_ctrlToolBarBorder;
	CMFCControlRenderer m_ctrlStatusBarBack;
	CMFCControlRenderer m_ctrlStatusBarBack_Ext;
	CMFCControlRenderer m_SysBtnBack[2];
	CMFCControlRenderer m_ctrlComboBoxBtn;
	CMFCControlRenderer m_ctrlMainBorder;
	CMFCControlRenderer m_ctrlMDIChildBorder;
	CMFCControlRenderer m_ctrlMainBorderCaption;
	CMFCControlRenderer m_ctrlPopupBorder;
	CMFCControlRenderer m_ctrlPopupResizeBar;
	CMFCControlRenderer m_ctrlMenuBarBtn;
	CMFCControlRenderer m_ctrlMenuItemBack;
	CMFCControlRenderer m_ctrlMenuItemShowAll;
	CMFCControlRenderer m_ctrlMenuHighlighted[2];
	CMFCControlRenderer m_ctrlMenuButtonBorder;
	CMFCControlRenderer m_ctrlMenuScrollBtn[2];
	CMFCControlRenderer m_ctrlToolBarBtn;
	CMFCControlRenderer m_ctrlTaskScrollBtn;
	CMFCControlRenderer m_ctrlTab3D[2];
	CMFCControlRenderer m_ctrlTabFlat[2];
	CMFCControlRenderer m_ctrlOutlookWndBar;
	CMFCControlRenderer m_ctrlOutlookWndPageBtn;
	CMFCControlRenderer m_ctrlRibbonCaptionQA;
	CMFCControlRenderer m_ctrlRibbonCaptionQA_Glass;
	CMFCControlRenderer m_ctrlRibbonCategoryBack;
	CMFCControlRenderer m_ctrlRibbonCategoryTab;
	CMFCControlRenderer m_ctrlRibbonCategoryTabSep;
	CMFCControlRenderer m_ctrlRibbonCategoryBtnPage[2];
	CMFCControlRenderer m_ctrlRibbonPanelBack_T;
	CMFCControlRenderer m_ctrlRibbonPanelBack_B;
	CMFCControlRenderer m_ctrlRibbonPanelQAT;
	CMFCControlRenderer m_ctrlRibbonMainPanel;
	CMFCControlRenderer m_ctrlRibbonMainPanelBorder;
	CMFCControlRenderer m_ctrlRibbonBtnMainPanel;
	CMFCControlRenderer m_ctrlRibbonBtnGroup_S;
	CMFCControlRenderer m_ctrlRibbonBtnGroup_F;
	CMFCControlRenderer m_ctrlRibbonBtnGroup_M;
	CMFCControlRenderer m_ctrlRibbonBtnGroup_L;
	CMFCControlRenderer m_ctrlRibbonBtnGroupMenu_F[2];
	CMFCControlRenderer m_ctrlRibbonBtnGroupMenu_M[2];
	CMFCControlRenderer m_ctrlRibbonBtnGroupMenu_L[2];
	CMFCControlRenderer m_ctrlRibbonBtn[2];
	CMFCControlRenderer m_ctrlRibbonBtnMenuH[2];
	CMFCControlRenderer m_ctrlRibbonBtnMenuV[2];
	CMFCControlRenderer m_ctrlRibbonBtnLaunch;
	CMFCControlRenderer m_RibbonBtnMain;
	CMFCControlRenderer m_ctrlRibbonBtnDefault;
	CMFCControlRenderer m_ctrlRibbonBtnDefaultIcon;
	CMFCControlRenderer m_ctrlRibbonBtnDefaultQATIcon;
	CMFCControlRenderer m_ctrlRibbonBtnDefaultQAT;
	CMFCControlRenderer m_ctrlRibbonBtnCheck;
	CMFCControlRenderer m_ctrlRibbonBtnPalette[3];
	CMFCControlRenderer m_ctrlRibbonBtnStatusPane;
	CMFCControlRenderer m_ctrlRibbonSliderThumb;
	CMFCControlRenderer m_ctrlRibbonSliderBtnPlus;
	CMFCControlRenderer m_ctrlRibbonSliderBtnMinus;
	CMFCControlRenderer m_ctrlRibbonProgressBack;
	CMFCControlRenderer m_ctrlRibbonProgressNormal;
	CMFCControlRenderer m_ctrlRibbonProgressNormalExt;
	CMFCControlRenderer m_ctrlRibbonProgressInfinity;
	CMFCControlRenderer m_ctrlRibbonBorder_QAT;
	CMFCControlRenderer m_ctrlRibbonBorder_Floaty;
	CMFCControlRenderer m_ctrlRibbonKeyTip;
	CMFCControlRenderer m_ctrlRibbonComboBoxBtn;
	CMFCControlRenderer m_ctrlRibbonContextPanelBack_T;
	CMFCControlRenderer m_ctrlRibbonContextPanelBack_B;
	CMFCControlRenderer m_ctrlRibbonContextSeparator;

	CPen   m_penTabFlatInner[2];
	CPen   m_penTabFlatOuter[2];
	CPen   m_penSeparator2;
	CPen   m_penSeparatorDark;
	CFont  m_AppCaptionFont;
	CBrush m_brMainClientArea;
	CBrush m_brGroupBackground;

	COLORREF m_clrAppCaptionActiveStart;
	COLORREF m_clrAppCaptionActiveFinish;
	COLORREF m_clrAppCaptionInactiveStart;
	COLORREF m_clrAppCaptionInactiveFinish;
	COLORREF m_clrAppCaptionActiveText;
	COLORREF m_clrAppCaptionInactiveText;
	COLORREF m_clrAppCaptionActiveTitleText;
	COLORREF m_clrAppCaptionInactiveTitleText;
	COLORREF m_clrMainClientArea;
	COLORREF m_clrMenuBarGradientLight;
	COLORREF m_clrMenuBarGradientDark;
	COLORREF m_clrMenuBarGradientVertLight;
	COLORREF m_clrMenuBarGradientVertDark;
	COLORREF m_clrMenuBarBtnText;
	COLORREF m_clrMenuBarBtnTextHighlighted;
	COLORREF m_clrMenuBarBtnTextDisabled;
	COLORREF m_clrToolBarBtnText;
	COLORREF m_clrToolBarBtnTextHighlighted;
	COLORREF m_clrToolBarBtnTextDisabled;
	COLORREF m_clrMenuText;
	COLORREF m_clrMenuTextHighlighted;
	COLORREF m_clrMenuTextDisabled;
	COLORREF m_clrStatusBarText;
	COLORREF m_clrStatusBarTextDisabled;
	COLORREF m_clrExtenedStatusBarTextDisabled;
	COLORREF m_clrEditBorder;
	COLORREF m_clrEditBorderDisabled;
	COLORREF m_clrEditBorderHighlighted;
	COLORREF m_clrEditSelection;
	COLORREF m_clrComboBorder;
	COLORREF m_clrComboBorderDisabled;
	COLORREF m_clrComboBorderPressed;
	COLORREF m_clrComboBorderHighlighted;
	COLORREF m_clrComboBtnStart;
	COLORREF m_clrComboBtnFinish;
	COLORREF m_clrComboBtnBorder;
	COLORREF m_clrComboBtnDisabledStart;
	COLORREF m_clrComboBtnDisabledFinish;
	COLORREF m_clrComboBtnBorderDisabled;
	COLORREF m_clrComboBtnPressedStart;
	COLORREF m_clrComboBtnPressedFinish;
	COLORREF m_clrComboBtnBorderPressed;
	COLORREF m_clrComboBtnHighlightedStart;
	COLORREF m_clrComboBtnHighlightedFinish;
	COLORREF m_clrComboBtnBorderHighlighted;
	COLORREF m_clrComboSelection;
	COLORREF m_clrHeaderNormalStart;
	COLORREF m_clrHeaderNormalFinish;
	COLORREF m_clrHeaderNormalBorder;
	COLORREF m_clrHeaderHighlightedStart;
	COLORREF m_clrHeaderHighlightedFinish;
	COLORREF m_clrHeaderHighlightedBorder;
	COLORREF m_clrHeaderPressedStart;
	COLORREF m_clrHeaderPressedFinish;
	COLORREF m_clrHeaderPressedBorder;
	COLORREF m_clrBarCaption;
	COLORREF m_clrMiniFrameCaption;
	COLORREF m_clrSeparator1;
	COLORREF m_clrSeparator2;
	COLORREF m_clrGroupText;
	COLORREF m_clrCaptionBarText;
	COLORREF m_clrTaskPaneGroupCaptionHighDark;
	COLORREF m_clrTaskPaneGroupCaptionHighLight;
	COLORREF m_clrTaskPaneGroupCaptionHighSpecDark;
	COLORREF m_clrTaskPaneGroupCaptionHighSpecLight;
	COLORREF m_clrTaskPaneGroupCaptionTextSpec;
	COLORREF m_clrTaskPaneGroupCaptionTextHighSpec;
	COLORREF m_clrTaskPaneGroupCaptionText;
	COLORREF m_clrTaskPaneGroupCaptionTextHigh;
	COLORREF m_clrTabFlatBlack;
	COLORREF m_clrTabFlatHighlight;
	COLORREF m_clrTabTextActive;
	COLORREF m_clrTabTextInactive;
	COLORREF m_clrOutlookPageTextNormal;
	COLORREF m_clrOutlookPageTextHighlighted;
	COLORREF m_clrOutlookPageTextPressed;
	COLORREF m_clrOutlookCaptionTextNormal;
	COLORREF m_clrRibbonCategoryText;
	COLORREF m_clrRibbonCategoryTextHighlighted;
	COLORREF m_clrRibbonCategoryTextDisabled;
	COLORREF m_clrRibbonPanelText;
	COLORREF m_clrRibbonPanelTextHighlighted;
	COLORREF m_clrRibbonPanelCaptionText;
	COLORREF m_clrRibbonPanelCaptionTextHighlighted;
	COLORREF m_clrRibbonKeyTipTextNormal;
	COLORREF m_clrRibbonKeyTipTextDisabled;
	COLORREF m_clrRibbonEdit;
	COLORREF m_clrRibbonEditDisabled;
	COLORREF m_clrRibbonEditHighlighted;
	COLORREF m_clrRibbonEditPressed;
	COLORREF m_clrRibbonEditBorder;
	COLORREF m_clrRibbonEditBorderDisabled;
	COLORREF m_clrRibbonEditBorderHighlighted;
	COLORREF m_clrRibbonEditBorderPressed;
	COLORREF m_clrRibbonEditSelection;
	COLORREF m_clrRibbonComboBtnStart;
	COLORREF m_clrRibbonComboBtnFinish;
	COLORREF m_clrRibbonComboBtnBorder;
	COLORREF m_clrRibbonComboBtnDisabledStart;
	COLORREF m_clrRibbonComboBtnDisabledFinish;
	COLORREF m_clrRibbonComboBtnBorderDisabled;
	COLORREF m_clrRibbonComboBtnPressedStart;
	COLORREF m_clrRibbonComboBtnPressedFinish;
	COLORREF m_clrRibbonComboBtnBorderPressed;
	COLORREF m_clrRibbonComboBtnHighlightedStart;
	COLORREF m_clrRibbonComboBtnHighlightedFinish;
	COLORREF m_clrRibbonComboBtnBorderHighlighted;
	COLORREF m_clrRibbonContextPanelText;
	COLORREF m_clrRibbonContextPanelTextHighlighted;
	COLORREF m_clrRibbonContextPanelCaptionText;
	COLORREF m_clrRibbonContextPanelCaptionTextHighlighted;
	COLORREF m_clrPlannerTodayCaption[4];
	COLORREF m_clrPlannerTodayBorder;
	COLORREF m_clrPlannerNcArea;
	COLORREF m_clrPlannerNcLine;
	COLORREF m_clrPlannerNcText;
	COLORREF m_clrPopupGradientLight;
	COLORREF m_clrPopupGradientDark;
	COLORREF m_clrRibbonHyperlinkInactive;
	COLORREF m_clrRibbonHyperlinkActive;
	COLORREF m_clrRibbonStatusbarHyperlinkInactive;
	COLORREF m_clrRibbonStatusbarHyperlinkActive;

	struct CMFCRibbonContextCategory
	{
		CMFCControlRenderer m_ctrlCaption;
		CMFCControlRenderer m_ctrlBack;
		CMFCControlRenderer m_ctrlTab;
		CMFCControlRenderer m_ctrlBtnDefault;
		COLORREF m_clrCaptionText;
		COLORREF m_clrText;
		COLORREF m_clrTextHighlighted;
		CMFCVisualManagerBitmapCache m_cacheBack;
		CMFCVisualManagerBitmapCache m_cacheBtnDefault;

		void CleanUp()
		{
			m_ctrlCaption.CleanUp();
			m_ctrlBack.CleanUp();
			m_ctrlTab.CleanUp();
			m_ctrlBtnDefault.CleanUp();
			m_cacheBack.Clear();
			m_cacheBtnDefault.Clear();
		}
	};

	CMFCRibbonContextCategory  m_ctrlRibbonContextCategory[AFX_RIBBON_CATEGORY_COLOR_COUNT];

	CMFCVisualManagerBitmapCache m_cacheRibbonContextPanelBack_T;
	CMFCVisualManagerBitmapCache m_cacheRibbonContextPanelBack_B;
	CMFCVisualManagerBitmapCache m_cacheRibbonCategoryBack;
	CMFCVisualManagerBitmapCache m_cacheRibbonPanelBack_T;
	CMFCVisualManagerBitmapCache m_cacheRibbonPanelBack_B;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnDefault;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_S;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_F;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_M;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_L;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroupMenu_F[2];
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroupMenu_M[2];
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroupMenu_L[2];

	int m_nType;

	CMFCToolTipInfo m_ToolTipParams;

	CMap<HWND, HWND, BOOL, BOOL> m_ActivateFlag;

	AFX_IMPORT_DATA static Style     m_Style;
	AFX_IMPORT_DATA static CString   m_strStylePrefix;
	AFX_IMPORT_DATA static HINSTANCE m_hinstRes;
	AFX_IMPORT_DATA static BOOL      m_bAutoFreeRes;

	BOOL IsBeta1() const { return m_nType == 10; }
	BOOL IsBeta() const { return m_nType < 20; }
	BOOL IsWindowActive(CWnd* pWnd) const;
	CSize GetSystemBorders(BOOL bRibbonPresent) const;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
