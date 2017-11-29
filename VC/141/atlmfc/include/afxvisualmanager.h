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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CBasePane;
class CMFCPopupMenu;
class CMFCBaseToolBar;
class CMFCToolBar;
class CMFCToolBarButton;
class CMFCToolBarMenuButton;
class CMFCCaptionButton;
class CMFCCaptionMenuButton;
class CMFCBaseTabCtrl;
class CMFCToolBarComboBoxButton;
class CMFCStatusBar;
class CMFCStatusBarPaneInfo;
class CMFCButton;
class CPaneFrameWnd;
class CMFCCaptionBar;
class CMFCHeaderCtrl;
class CMFCToolBarEditBoxButton;
class CDockablePane;
class CMFCTasksPaneTask;
class CMFCTasksPaneTaskGroup;
class CPaneDivider;
class CSplitterWndEx;
class CMFCAutoHideButton;
class CMFCEditBrowseCtrl;
class CMFCSpinButtonCtrl;
class CMFCDesktopAlertWnd;
class CMFCDesktopAlertWndButton;
class CMFCPropertyGridCtrl;
class CMFCRibbonBar;
class CMFCRibbonCategory;
class CMFCRibbonPanel;
class CMFCRibbonButton;
class CMFCRibbonTab;
class CMFCRibbonCaptionButton;
class CMFCRibbonButtonsGroup;
class CMFCRibbonLaunchButton;
class CMFCRibbonRichEditCtrl;
class CMFCRibbonMainPanel;
class CMFCRibbonLabel;
class CMFCRibbonGallery;
class CMFCRibbonGalleryIcon;
class CMFCRibbonContextCaption;
class CMFCToolTipInfo;
class CMFCRibbonStatusBar;
class CMFCRibbonStatusBarPane;
class CMFCRibbonSlider;
class CMFCRibbonProgressBar;
class CMFCRibbonPanelMenuBar;
class CMFCRibbonPanelMenu;
class CMFCRibbonSeparator;
class CMFCRibbonBaseElement;
class CMFCPropertySheet;
class CMFCRibbonCheckBox;
class CMFCRibbonLinkCtrl;
class CMFCRibbonColorButton;
class CRibbonCategoryScroll;
class CMFCToolTipCtrl;

#define	AFX_SPIN_PRESSEDUP       0x0001
#define	AFX_SPIN_PRESSEDDOWN     0x0002
#define	AFX_SPIN_HIGHLIGHTEDUP   0x0004
#define	AFX_SPIN_HIGHLIGHTEDDOWN 0x0008
#define	AFX_SPIN_DISABLED        0x0010

#ifndef _UXTHEME_H_

// From uxtheme.h:
typedef HANDLE HTHEME;          // handle to a section of theme data for class

#endif // THEMEAPI

typedef HTHEME (__stdcall * OPENTHEMEDATA)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (__stdcall * CLOSETHEMEDATA)(HTHEME hTheme);
typedef HRESULT (__stdcall * DRAWTHEMEBACKGROUND)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
typedef HRESULT (__stdcall * GETTHEMECOLOR)(HTHEME hTheme, int iPartId, int iStateId, int iPropId, OUT COLORREF *pColor);
typedef COLORREF (__stdcall * GETTHEMESYSCOLOR)(HTHEME hTheme, int iColorId);

typedef HRESULT (__stdcall * GETCURRENTTHEMENAME)(OUT LPWSTR pszThemeFileName, int cchMaxNameChars, OUT OPTIONAL LPWSTR pszColorBuff,
	int cchMaxColorChars, OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);

typedef HTHEME (__stdcall * GETWINDOWTHEME)(HWND hWnd);

class CMFCBaseVisualManager : public CObject
{
public:
	CMFCBaseVisualManager();
	virtual ~CMFCBaseVisualManager();

	enum WinXpTheme
	{
		WinXpTheme_None = -1,
		WinXpTheme_NonStandard,
		WinXpTheme_Blue,
		WinXpTheme_Olive,
		WinXpTheme_Silver
	};

	virtual BOOL DrawPushButton(CDC* pDC, CRect rect, CMFCButton* pButton, UINT uiState);

	virtual BOOL DrawStatusBarProgress(CDC* pDC, CMFCStatusBar* pStatusBar, CRect rectProgress, int nProgressTotal, int nProgressCurr,
		COLORREF clrBar, COLORREF clrProgressBarDest, COLORREF clrProgressText, BOOL bProgressText);

	virtual BOOL DrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted);
	virtual BOOL DrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted);

	virtual void FillReBarPane(CDC* pDC, CBasePane* pBar, CRect rectClient);

	virtual BOOL DrawCheckBox(CDC *pDC, CRect rect, BOOL bHighlighted, int nState, BOOL bEnabled, BOOL bPressed);
	virtual BOOL DrawRadioButton(CDC *pDC, CRect rect, BOOL bHighlighted, BOOL bChecked, BOOL bEnabled, BOOL bPressed);

	virtual WinXpTheme GetStandardWindowsTheme();

protected:
	void UpdateSystemColors();
	void CleanUpThemes();

	HTHEME m_hThemeWindow;
	HTHEME m_hThemeToolBar;
	HTHEME m_hThemeRebar;
	HTHEME m_hThemeButton;
	HTHEME m_hThemeStatusBar;
	HTHEME m_hThemeComboBox;
	HTHEME m_hThemeProgress;
	HTHEME m_hThemeHeader;
	HTHEME m_hThemeScrollBar;
	HTHEME m_hThemeExplorerBar;
	HTHEME m_hThemeTree;
	HTHEME m_hThemeStartPanel;
	HTHEME m_hThemeTaskBand;
	HTHEME m_hThemeTaskBar;
	HTHEME m_hThemeSpin;
	HTHEME m_hThemeTab;
	HTHEME m_hThemeToolTip;
	HTHEME m_hThemeTrack;
	HTHEME m_hThemeMenu;
};

class CMFCVisualManager : public CMFCBaseVisualManager
{
	friend struct AFX_GLOBAL_DATA;

	DECLARE_DYNCREATE(CMFCVisualManager)

public:
	CMFCVisualManager(BOOL bTemporary = FALSE);
	virtual ~CMFCVisualManager();

	static void __stdcall SetDefaultManager(CRuntimeClass* pRTI);

	virtual BOOL IsWindowsThemingSupported() const { return FALSE; }
	static void __stdcall DestroyInstance(BOOL bAutoDestroyOnly = FALSE);

// Operations:
public:
	static void __stdcall RedrawAll();
	static void __stdcall AdjustToolbars();
	static void __stdcall AdjustFrames();

// Overrides:
public:
	virtual void OnUpdateSystemColors();

	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual void OnDrawPaneBorder(CDC* pDC, CBasePane* pBar, CRect& rect);
	virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	virtual void OnDrawMenuShadow(CDC* pDC, const CRect& rectClient, const CRect& rectExclude, int nDepth,
		int iMinBrightness, int iMaxBrightness, CBitmap* pBmpSaveBottom, CBitmap* pBmpSaveRight, BOOL bRTL);
	virtual void OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);
	virtual void OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID = -1);
	virtual void OnDrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight);
	virtual void OnDrawStatusBarPaneBorder(CDC* pDC, CMFCStatusBar* pBar, CRect rectPane, UINT uiID, UINT nStyle);
	virtual COLORREF GetStatusBarPaneTextColor(CMFCStatusBar* pStatusBar, CMFCStatusBarPaneInfo* pPane);

	virtual void OnDrawStatusBarProgress(CDC* pDC, CMFCStatusBar* /*pStatusBar*/, CRect rectProgress, int nProgressTotal,
		int nProgressCurr, COLORREF clrBar, COLORREF clrProgressBarDest, COLORREF clrProgressText, BOOL bProgressText);
	virtual void OnDrawStatusBarSizeBox(CDC* pDC, CMFCStatusBar* pStatBar, CRect rectSizeBox);

	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, CMFCToolBarEditBoxButton* pButton);
	virtual void OnDrawTearOffCaption(CDC* pDC, CRect rect, BOOL bIsActive);

	virtual void OnDrawMenuResizeBar(CDC* pDC, CRect rect, int nResizeFlags);
	virtual void OnDrawMenuScrollButton(CDC* pDC, CRect rect, BOOL bIsScrollDown, BOOL bIsHighlited, BOOL bIsPressed, BOOL bIsDisabled);

	enum AFX_BUTTON_STATE
	{
		ButtonsIsRegular,
		ButtonsIsPressed,
		ButtonsIsHighlighted,
	};

	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonSeparator(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state, BOOL bHorz);

	virtual void OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	virtual COLORREF GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton);
	virtual BOOL IsHighlightWholeMenuItem() { return FALSE; }
	
	virtual COLORREF GetMenuItemTextColor(CMFCToolBarMenuButton* pButton, BOOL bHighlighted, BOOL bDisabled);
	virtual void OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed);

	virtual void OnDrawMenuCheck(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect, BOOL bHighlight, BOOL bIsRadio);
	virtual void OnDrawMenuItemButton(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rectButton, BOOL bHighlight, BOOL bDisabled);

	virtual void OnFillMenuImageRect(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state) { OnFillButtonInterior(pDC, pButton, rect, state); }

	virtual BOOL IsOwnerDrawMenuCheck() { return FALSE; }

	virtual COLORREF GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual COLORREF GetToolbarDisabledColor() const { return (COLORREF)-1; }

	virtual BOOL IsToolbarRoundShape(CMFCToolBar* /*pToolBar*/) { return FALSE; }

	virtual COLORREF GetToolbarHighlightColor();
	virtual COLORREF GetToolbarDisabledTextColor();

	virtual int GetDockingPaneCaptionExtraHeight() const { return 0; }

	// Caption bar:
	virtual COLORREF GetCaptionBarTextColor(CMFCCaptionBar* pBar);
	virtual void OnDrawCaptionBarInfoArea(CDC* pDC, CMFCCaptionBar* pBar, CRect rect);
	virtual void OnDrawCaptionBarBorder(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, COLORREF clrBarBorder, BOOL bFlatBorder);
	virtual COLORREF OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, BOOL bIsPressed,
		BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);
	virtual void OnDrawCaptionBarButtonBorder(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, BOOL bIsPressed,
		BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);

	// Outlook bar:
	virtual void OnFillOutlookPageButton(CDC* pDC, const CRect& rect, BOOL bIsHighlighted, BOOL bIsPressed, COLORREF& clrText);
	virtual void OnDrawOutlookPageButtonBorder(CDC* pDC, CRect& rectBtn, BOOL bIsHighlighted, BOOL bIsPressed);
	virtual void OnDrawOutlookBarSplitter(CDC* pDC, CRect rectSplitter);
	virtual void OnFillOutlookBarCaption(CDC* pDC, CRect rectCaption, COLORREF& clrText);

	// Tab overrides:
	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnFillTab(CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTabContent(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd, COLORREF clrText);
	virtual void OnDrawTabCloseButton(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTabsButtonBorder(CDC* pDC, CRect& rect, CMFCButton* pButton, UINT uiState, CMFCBaseTabCtrl* pWndTab);
	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack, COLORREF& clrHighlight,
		COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	virtual BOOL OnEraseTabsFrame(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual BOOL IsHighlightOneNoteTabs() const { return FALSE; }
	virtual BOOL AlwaysHighlight3DTabs() const { return FALSE; }
	virtual COLORREF GetTabTextColor(const CMFCBaseTabCtrl* /*pTabWnd*/, int /*iTab*/, BOOL /*bIsActive*/) { return (COLORREF)-1; }
	virtual int GetTabHorzMargin(const CMFCBaseTabCtrl* /*pTabWnd*/) { return 0; }
	virtual void OnDrawTabResizeBar(CDC* pDC, CMFCBaseTabCtrl* pWndTab, BOOL bIsVert, CRect rect, CBrush* pbrFace, CPen* pPen);
	virtual void AdjustTabTextRect(CRect& /* rectTabText */) { }

	virtual int GetMDITabsBordersSize() { return -1; /* Default */ }
	virtual int GetDockingTabsBordersSize() { return -1; /* Default */ }
	virtual BOOL IsDockingTabHasBorder() { return TRUE; }

	// Customization dialog:
	virtual COLORREF OnFillCommandsListBackground(CDC* pDC, CRect rect, BOOL bIsSelected = FALSE);
	virtual void OnDrawMenuArrowOnCustomizeList(CDC* pDC, CRect rectCommand, BOOL bSelected);

	virtual CSize GetButtonExtraBorder() const { return CSize(0, 0); }
	virtual CSize GetCaptionButtonExtraBorder() const { return CSize(0, 0); }

	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);
	virtual void OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize);
	virtual void OnDrawFloatingToolbarBorder(	CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize);

	// Header control:
	virtual void OnFillHeaderCtrlBackground(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect rect);
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);
	virtual void OnDrawHeaderCtrlSortArrow(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsUp);
	void DoDrawHeaderSortArrow(CDC* pDC, CRect rect, BOOL bIsUp, BOOL bDlgCtrl);

	virtual COLORREF OnDrawPropertySheetListItem(CDC* pDC, CMFCPropertySheet* pParent, CRect rect, BOOL bIsHighlihted, BOOL bIsSelected);

	// Tasks pane:
	virtual void OnFillTasksPaneBackground(CDC* pDC, CRect rectWorkArea);

	virtual void OnDrawTasksGroupCaption(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);
	virtual void OnDrawTasksGroupIcon(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, int nIconHOffset = 5, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);

	virtual void OnFillTasksGroupInterior(CDC* pDC, CRect rect, BOOL bSpecial = FALSE);
	virtual void OnDrawTasksGroupAreaBorder(CDC* pDC, CRect rect, BOOL bSpecial = FALSE, BOOL bNoTitle = FALSE);
	virtual void OnDrawTask(CDC* pDC, CMFCTasksPaneTask* pTask, CImageList* pIcons, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE);

	virtual void OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize, int iImage, BOOL bHilited);
	virtual void OnDrawToolBoxFrame(CDC* pDC, const CRect& rect);

	// Slider
	virtual void OnDrawPaneDivider(CDC* pDC, CPaneDivider* pSlider, CRect rect, BOOL bAutoHideMode);

	// Property list:
	virtual void OnDrawExpandingBox(CDC* pDC, CRect rect, BOOL bIsOpened, COLORREF colorBox);
	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);
	virtual COLORREF GetPropertyGridGroupTextColor(CMFCPropertyGridCtrl* pPropList);

	// Splitter:
	virtual void OnDrawSplitterBorder(CDC* pDC, CSplitterWndEx* pSplitterWnd, CRect rect);
	virtual void OnDrawSplitterBox(CDC* pDC, CSplitterWndEx* pSplitterWnd, CRect& rect);
	virtual void OnFillSplitterBackground(CDC* pDC, CSplitterWndEx* pSplitterWnd, CRect rect);

	virtual void OnDrawCheckBox(CDC *pDC, CRect rect, BOOL bHighlighted, BOOL bChecked,BOOL bEnabled);
	virtual void OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled);
	virtual void OnDrawControlBorder(CWnd* pWndCtrl);

	// Auto-hide buttons:
	virtual BOOL HasOverlappedAutoHideButtons() const { return FALSE; }

	virtual void OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton);
	virtual void OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton);
	virtual COLORREF GetAutoHideButtonTextColor(CMFCAutoHideButton* pButton);

	// Edit box:
	virtual BOOL OnDrawBrowseButton(CDC* pDC, CRect rect, CMFCEditBrowseCtrl* pEdit, CMFCVisualManager::AFX_BUTTON_STATE state, COLORREF& clrText);

	// Spin control:
	virtual void OnDrawSpinButtons(CDC* pDC, CRect rectSpin, int nState, BOOL bOrientation, CMFCSpinButtonCtrl* pSpinCtrl);

	// Windows XP drawing methods:
	virtual BOOL DrawPushButtonWinXP(CDC* /*pDC*/, CRect /*rect*/, CMFCButton* /*pButton*/, UINT /*uiState*/) { return FALSE; }
	virtual BOOL DrawComboDropButtonWinXP(CDC* /*pDC*/, CRect /*rect*/, BOOL /*bDisabled*/, BOOL /*bIsDropped*/, BOOL /*bIsHighlighted*/) { return FALSE; }
	virtual BOOL DrawComboBorderWinXP(CDC* /*pDC*/, CRect /*rect*/, BOOL /*bDisabled*/, BOOL /*bIsDropped*/, BOOL /*bIsHighlighted*/) { return FALSE; }

	// Smart docking marker colors:
	virtual void GetSmartDockingBaseGuideColors(COLORREF& clrBaseGroupBackground, COLORREF& clrBaseGroupBorder);
	virtual COLORREF GetSmartDockingHighlightToneColor();
	virtual AFX_SMARTDOCK_THEME GetSmartDockingTheme() { return AFX_SDT_VS2005; }

	// Popup window:
	virtual void OnFillPopupWindowBackground(CDC* pDC, CRect rect);
	virtual void OnDrawPopupWindowBorder(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd);
	virtual void OnErasePopupWindowButton(CDC* pDC, CRect rectClient, CMFCDesktopAlertWndButton* pButton);
	virtual void OnDrawPopupWindowButtonBorder(CDC* pDC, CRect rectClient, CMFCDesktopAlertWndButton* pButton);
	virtual BOOL IsDefaultWinXPPopupButton(CMFCDesktopAlertWndButton* /*pButton*/) const { return FALSE; }

	// Window apperance:
	virtual BOOL IsOwnerDrawCaption() { return FALSE; }
	virtual BOOL OnSetWindowRegion(CWnd* pWnd, CSize sizeWindow);
	virtual void OnActivateApp(CWnd* pWnd, BOOL bActive);
	virtual BOOL OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);
	virtual BOOL OnNcActivate(CWnd* pWnd, BOOL bActive);
	virtual CSize GetNcBtnSize(BOOL bSmall) const;

	// Ribbon control:
	virtual COLORREF OnDrawRibbonTabsFrame(CDC* pDC, CMFCRibbonBar* pWndRibbonBar, CRect rectTab);
	virtual void OnDrawRibbonApplicationButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory);
	virtual COLORREF OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive);
	virtual void OnDrawRibbonCategoryScroll(CDC* pDC, CRibbonCategoryScroll* pScroll);
	virtual COLORREF OnDrawRibbonPanel(CDC* pDC,CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption);
	virtual void OnDrawRibbonPanelCaption(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectCaption);
	virtual void OnDrawRibbonLaunchButton(CDC* pDC, CMFCRibbonLaunchButton* pButton, CMFCRibbonPanel* pPanel);
	virtual void OnDrawRibbonDefaultPaneButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonDefaultPaneButtonContext(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonDefaultPaneButtonIndicator(CDC* pDC, CMFCRibbonButton* pButton, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual COLORREF OnFillRibbonButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnFillRibbonEdit(CDC* pDC, CMFCRibbonRichEditCtrl* pEdit, CRect rect, BOOL bIsHighlighted,
		BOOL bIsPaneHighlighted, BOOL bIsDisabled, COLORREF& clrText, COLORREF& clrSelBackground, COLORREF& clrSelText);

	virtual void OnDrawRibbonButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonMenuCheckFrame(CDC* pDC, CMFCRibbonButton* pButton, CRect rect);

	virtual COLORREF OnFillRibbonMainPanelButton(CDC* pDC, CMFCRibbonButton* pButton);

	virtual void OnDrawRibbonMainPanelButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rect,CRect rectText);

	virtual int GetRibbonQuickAccessToolBarChevronOffset() { return 0; }
	virtual int GetRibbonQuickAccessToolBarRightMargin() { return 0; }
	virtual COLORREF GetRibbonQuickAccessToolBarTextColor(BOOL bDisabled = FALSE) { return bDisabled ? GetToolbarDisabledTextColor() : (COLORREF)-1; }

	virtual void OnDrawRibbonCaptionButton(CDC* pDC, CMFCRibbonCaptionButton* pButton);
	virtual COLORREF OnDrawRibbonButtonsGroup(CDC* pDC, CMFCRibbonButtonsGroup* pGroup, CRect rectGroup);
	virtual void OnDrawDefaultRibbonImage(CDC* pDC, CRect rectImage, BOOL bIsDisabled = FALSE, BOOL bIsPressed = FALSE, BOOL bIsHighlighted = FALSE);
	virtual void OnDrawRibbonMainPanelFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);

	virtual void OnFillRibbonMenuFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);

	virtual void OnDrawRibbonRecentFilesFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual void OnDrawRibbonLabel(CDC* pDC, CMFCRibbonLabel* pLabel,CRect rect);
	virtual void OnDrawRibbonGalleryButton(CDC* pDC, CMFCRibbonGalleryIcon* pButton);
	virtual void OnDrawRibbonGalleryBorder(CDC* pDC, CMFCRibbonGallery* pButton, CRect rectBorder);

	virtual COLORREF RibbonCategoryColorToRGB(AFX_RibbonCategoryColor color);

	virtual COLORREF OnDrawRibbonCategoryCaption(CDC* pDC, CMFCRibbonContextCaption* pContextCaption);
	virtual COLORREF OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane);

	virtual void GetRibbonSliderColors(CMFCRibbonSlider* pSlider, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled, COLORREF& clrLine, COLORREF& clrFill);

	virtual void OnDrawRibbonSliderZoomButton(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsZoomOut, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);

	virtual void OnDrawRibbonSliderChannel(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect);
	virtual void OnDrawRibbonSliderThumb(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* pProgress, CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode);

	virtual void OnFillRibbonQuickAccessToolBarPopup(CDC* pDC, CMFCRibbonPanelMenuBar* pMenuBar, CRect rect);

	virtual int GetRibbonPopupBorderSize(const CMFCRibbonPanelMenu* /*pPopup*/) const { return m_nMenuBorderSize; }

	virtual void OnDrawRibbonQuickAccessToolBarSeparator(CDC* pDC, CMFCRibbonSeparator* pSeparator, CRect rect);

	virtual BOOL IsLayeredRibbonKeyTip() const { return FALSE; }
	virtual void OnDrawRibbonKeyTip(CDC* pDC, CMFCRibbonBaseElement* pElement, CRect rect, CString str);

	virtual void OnDrawRibbonCheckBoxOnList(CDC* pDC, CMFCRibbonCheckBox* pCheckBox, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual COLORREF GetRibbonHyperlinkTextColor(CMFCRibbonLinkCtrl* pHyperLink);
	virtual COLORREF GetRibbonStatusBarTextColor(CMFCRibbonStatusBar* pStatusBar);

	virtual COLORREF GetRibbonEditBackgroundColor(CMFCRibbonRichEditCtrl* pEdit, BOOL bIsHighlighted, BOOL bIsPaneHighlighted, BOOL bIsDisabled);

	virtual void OnDrawRibbonColorPaletteBox(CDC* pDC, CMFCRibbonColorButton* pColorButton, CMFCRibbonGalleryIcon* pIcon,
		COLORREF color, CRect rect, BOOL bDrawTopEdge, BOOL bDrawBottomEdge, BOOL bIsHighlighted, BOOL bIsChecked, BOOL bIsDisabled);

	const CPoint& GetRibbonMainImageOffset() const { return m_ptRibbonMainImageOffset; }

	// Vista support:
	virtual BOOL DrawTextOnGlass(CDC* pDC, CString strText, CRect rect, DWORD dwFlags, int nGlowSize = 0, COLORREF clrText = (COLORREF)-1);

	// MDI Client area
	virtual BOOL OnEraseMDIClientArea(CDC* pDC, CRect rectClient);

	// ToolTip
	virtual BOOL GetToolTipInfo(CMFCToolTipInfo& params, UINT nType = (UINT)(-1));
	virtual void OnFillToolTip (CDC* pDC, CMFCToolTipCtrl* pToolTip, CRect rect, COLORREF& clrText, COLORREF& clrLine);

// Attributes:
public:

	void SetMenuShadowDepth(int nDepth) { m_nMenuShadowDepth = nDepth; }
	int GetMenuShadowDepth() const { return m_nMenuShadowDepth; }
	
	static CMFCVisualManager* __stdcall GetInstance()
	{
		if (m_pVisManager != NULL)
		{
			ASSERT_VALID(m_pVisManager);
			return m_pVisManager;
		}

		if (m_pRTIDefault == NULL)
		{
			m_pVisManager = new CMFCVisualManager;
		}
		else
		{
			m_pVisManager = (CMFCVisualManager*) m_pRTIDefault->CreateObject();
			ASSERT_VALID(m_pVisManager);
		}

		m_pVisManager->m_bAutoDestroy = TRUE;
		m_pVisManager->OnUpdateSystemColors();

		return m_pVisManager;
	}

	BOOL IsMenuFlatLook() const { return m_bMenuFlatLook; }
	void SetMenuFlatLook(BOOL bMenuFlatLook = TRUE)
	{
		m_bMenuFlatLook = bMenuFlatLook;
		RedrawAll();
	}

	BOOL IsAutoDestroy() const { return m_bAutoDestroy; }

	void SetShadowHighlightedImage(BOOL bShadow = TRUE) { m_bShadowHighlightedImage = bShadow; }
	BOOL IsShadowHighlightedImage() const { return m_bShadowHighlightedImage; }

	void EnableToolbarButtonFill(BOOL bEnable = TRUE) { m_bEnableToolbarButtonFill = bEnable; }
	BOOL IsToolbarButtonFillEnabled() const { return m_bEnableToolbarButtonFill; }

	BOOL IsEmbossDisabledImage() const { return m_bEmbossDisabledImage; }
	void SetEmbossDisabledImage(BOOL bEmboss = TRUE) { m_bEmbossDisabledImage = bEmboss; }

	BOOL IsFadeInactiveImage() const { return m_bFadeInactiveImage; }
	void SetFadeInactiveImage(BOOL bFade = TRUE) { m_bFadeInactiveImage = bFade; }

	virtual int GetMenuImageMargin() const { return 2; }
	virtual int GetPopupMenuGap() const { return 1; }
	virtual CRect GetMenuImageFrameOffset() const { return CRect (2, 1, 0, 2); }

	// TasksPane:
	int GetTasksPaneVertMargin() const { return m_nVertMargin; }
	int GetTasksPaneHorzMargin() const { return m_nHorzMargin; }
	int GetTasksPaneGroupVertOffset() const { return m_nGroupVertOffset; }
	int GetTasksPaneGroupCaptionHeight() const { return m_nGroupCaptionHeight; }

	int GetTasksPaneGroupCaptionHorzOffset() const { return m_nGroupCaptionHorzOffset; }
	int GetTasksPaneGroupCaptionVertOffset() const { return m_nGroupCaptionVertOffset; }

	int GetTasksPaneTaskHorzOffset() const { return m_nTasksHorzOffset; }

	int GetTasksPaneIconHorzOffset() const { return m_nTasksIconHorzOffset; }
	int GetTasksPaneIconVertOffset() const { return m_nTasksIconVertOffset; }

	virtual int GetToolBarCustomizeButtonMargin() const { return 2; }
	virtual BOOL IsOffsetPressedButton() const { return TRUE; }
	virtual BOOL IsOfficeXPStyleMenus() const { return m_bOfficeXPStyleMenus; }
	virtual int GetPopupMenuBorderSize() const { return m_nMenuBorderSize; }

	BOOL IsFrameMenuCheckedItems() const { return m_bFrameMenuCheckedItems; }

	// "Show all menu items" methods:
	virtual int GetShowAllMenuItemsHeight(CDC* pDC, const CSize& sizeDefault);
	virtual void OnDrawShowAllMenuItems(CDC* pDC, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

protected:
	static CMFCVisualManager* __stdcall CreateVisualManager(CRuntimeClass* pVisualManager);

// Attributes:
protected:
	AFX_IMPORT_DATA static CRuntimeClass*  m_pRTIDefault;
	AFX_IMPORT_DATA static CMFCVisualManager* m_pVisManager;

	BOOL m_bMenuFlatLook; // Menu item is always still unpressed
	BOOL m_bShadowHighlightedImage;
	BOOL m_bEmbossDisabledImage;
	BOOL m_bFadeInactiveImage;
	BOOL m_bEnableToolbarButtonFill;
	BOOL m_bFrameMenuCheckedItems;
	BOOL m_bIsTemporary;
	BOOL m_bActiveCaptions;
	BOOL m_bOfficeXPStyleMenus;
	BOOL m_bAlwaysFillTab;
	BOOL m_b3DTabWideBorder;

	int m_nMenuShadowDepth;
	int m_nVertMargin;
	int m_nHorzMargin;
	int m_nGroupVertOffset;
	int m_nGroupCaptionHeight;
	int m_nGroupCaptionHorzOffset;
	int m_nGroupCaptionVertOffset;
	int m_nTasksHorzOffset;
	int m_nTasksIconHorzOffset;
	int m_nTasksIconVertOffset;
	int m_nMenuBorderSize;

	COLORREF m_clrMenuShadowBase;

	CPoint m_ptRibbonMainImageOffset;

private:
	BOOL m_bAutoDestroy;
};

extern AFX_IMPORT_DATA UINT AFX_WM_CHANGEVISUALMANAGER;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
