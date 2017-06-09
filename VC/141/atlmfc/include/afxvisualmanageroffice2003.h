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

#include "afxvisualmanagerofficexp.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCVisualManagerOffice2003 : public CMFCVisualManagerOfficeXP
{
	DECLARE_DYNCREATE(CMFCVisualManagerOffice2003)

public:
	CMFCVisualManagerOffice2003();
	virtual ~CMFCVisualManagerOffice2003();

	static void __stdcall SetUseGlobalTheme(BOOL bUseGlobalTheme = TRUE);
	static BOOL __stdcall IsUseGlobalTheme() { return m_bUseGlobalTheme; }

	virtual BOOL IsWindowsThemingSupported() const { return m_hThemeWindow != NULL; }

	static void __stdcall SetStatusBarOfficeXPLook(BOOL bStatusBarOfficeXPLook = TRUE);
	static BOOL __stdcall IsStatusBarOfficeXPLook() { return m_bStatusBarOfficeXPLook; }

	static void __stdcall SetDefaultWinXPColors(BOOL bDefaultWinXPColors = TRUE);
	static BOOL __stdcall IsDefaultWinXPColorsEnabled() { return m_bDefaultWinXPColors; }

	virtual COLORREF GetBaseThemeColor();

	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual void OnDrawPaneBorder(CDC* pDC, CBasePane* pBar, CRect& rect);
	virtual void OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar);
	virtual void OnDrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnFillOutlookPageButton(CDC* pDC, const CRect& rect, BOOL bIsHighlighted, BOOL bIsPressed, COLORREF& clrText);
	virtual void OnDrawOutlookPageButtonBorder(CDC* pDC, CRect& rectBtn, BOOL bIsHighlighted, BOOL bIsPressed);
	virtual void OnDrawOutlookBarSplitter(CDC* pDC, CRect rectSplitter);
	virtual void OnFillOutlookBarCaption(CDC* pDC, CRect rectCaption, COLORREF& clrText);
	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);

	virtual int GetToolBarCustomizeButtonMargin() const { return 1; }
	virtual COLORREF GetToolbarDisabledColor() const { return m_clrToolbarDisabled; }

	virtual BOOL IsToolbarRoundShape(CMFCToolBar* pToolBar);

	virtual void OnHighlightQuickCustomizeMenuButton(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect);
	virtual COLORREF OnFillCommandsListBackground(CDC* pDC, CRect rect, BOOL bIsSelected = FALSE);

	virtual void DrawCustomizeButton(CDC* pDC, CRect rect, BOOL bIsHorz, CMFCVisualManager::AFX_BUTTON_STATE state, BOOL bIsCustomize, BOOL bIsMoreButtons);

	virtual void OnUpdateSystemColors();
	virtual void OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton);

	virtual BOOL IsOffsetPressedButton() const { return FALSE; }

	virtual int GetShowAllMenuItemsHeight(CDC* pDC, const CSize& sizeDefault);
	virtual void OnDrawShowAllMenuItems(CDC* pDC, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnDrawCaptionBarBorder(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, COLORREF clrBarBorder, BOOL bFlatBorder);
	virtual void OnDrawTearOffCaption(CDC* pDC, CRect rect, BOOL bIsActive);

	virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	virtual void OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed);

	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual BOOL OnEraseTabsFrame(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTabsButtonBorder(CDC* pDC, CRect& rect, CMFCButton* pButton, UINT uiState, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnFillTab(CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void GetTabFrameColors(const CMFCBaseTabCtrl* pTabWnd, COLORREF& clrDark, COLORREF& clrBlack,
		COLORREF& clrHighlight, COLORREF& clrFace, COLORREF& clrDarkShadow, COLORREF& clrLight, CBrush*& pbrFace, CBrush*& pbrBlack);
	virtual BOOL IsHighlightOneNoteTabs() const { return TRUE; }
	virtual BOOL IsDockingTabHasBorder() { return FALSE; }

	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);
	virtual COLORREF GetPropertyGridGroupTextColor(CMFCPropertyGridCtrl* pPropList);

	// Tasks pane:
	virtual void OnFillTasksPaneBackground(CDC* pDC, CRect rectWorkArea);

	virtual void OnDrawTasksGroupCaption(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);

	virtual void OnFillTasksGroupInterior(CDC* pDC, CRect rect, BOOL bSpecial = FALSE);
	virtual void OnDrawTasksGroupAreaBorder(CDC* pDC, CRect rect, BOOL bSpecial = FALSE, BOOL bNoTitle = FALSE);
	virtual void OnDrawTask(CDC* pDC, CMFCTasksPaneTask* pTask, CImageList* pIcons, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE);

	virtual void OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize, int iImage, BOOL bHilited);

	virtual void OnDrawStatusBarProgress(CDC* pDC, CMFCStatusBar* pStatusBar, CRect rectProgress, int nProgressTotal, int nProgressCurr,
		COLORREF clrBar, COLORREF clrProgressBarDest, COLORREF clrProgressText, BOOL bProgressText);
	virtual void OnDrawStatusBarPaneBorder(CDC* pDC, CMFCStatusBar* pBar, CRect rectPane, UINT uiID, UINT nStyle);

	virtual void OnFillHeaderCtrlBackground(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);

	// Auto-hide buttons:
	virtual void OnFillAutoHideButtonBackground(CDC* pDC, CRect rect, CMFCAutoHideButton* pButton);
	virtual void OnDrawAutoHideButtonBorder(CDC* pDC, CRect rectBounds, CRect rectBorderSize, CMFCAutoHideButton* pButton);

	virtual BOOL DrawPushButtonWinXP(CDC* pDC, CRect rect, CMFCButton* pButton, UINT uiState)
	{
		return DrawPushButton(pDC, rect, pButton, uiState);
	}

	virtual BOOL DrawComboDropButtonWinXP(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted)
	{
		return DrawComboDropButton(pDC, rect, bDisabled, bIsDropped, bIsHighlighted);
	}

	virtual BOOL DrawComboBorderWinXP(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted)
	{
		return DrawComboBorder(pDC, rect, bDisabled, bIsDropped, bIsHighlighted);
	}

	// Edit box:
	virtual BOOL OnDrawBrowseButton(CDC* pDC, CRect rect, CMFCEditBrowseCtrl* pEdit, CMFCVisualManager::AFX_BUTTON_STATE state, COLORREF& clrText);
	virtual void OnDrawControlBorder(CWnd* pWndCtrl);
	virtual void OnDrawExpandingBox(CDC* pDC, CRect rect, BOOL bIsOpened, COLORREF colorBox);

	virtual void GetSmartDockingBaseGuideColors(COLORREF& clrBaseGroupBackground, COLORREF& clrBaseGroupBorder);
	virtual COLORREF GetSmartDockingHighlightToneColor();

	virtual void OnDrawStatusBarSizeBox(CDC* pDC, CMFCStatusBar* pStatBar, CRect rectSizeBox);
	virtual void OnDrawHeaderCtrlBorder(CMFCHeaderCtrl* pCtrl, CDC* pDC, CRect& rect, BOOL bIsPressed, BOOL bIsHighlighted);

	// Popup window:
	virtual void OnFillPopupWindowBackground(CDC* pDC, CRect rect);
	virtual void OnDrawPopupWindowBorder(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd);
	virtual void OnErasePopupWindowButton(CDC* pDC, CRect rectClient, CMFCDesktopAlertWndButton* pButton);
	virtual void OnDrawPopupWindowButtonBorder(CDC* pDC, CRect rectClient, CMFCDesktopAlertWndButton* pButton);

	virtual COLORREF GetHighlightMenuItemColor() const { return m_clrHighlightMenuItem; }

	virtual void OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled);
	virtual BOOL GetToolTipInfo(CMFCToolTipInfo& params, UINT nType = (UINT)(-1));

	// Ribbon control:
	virtual COLORREF OnDrawRibbonCategoryTab(CDC* pDC,  CMFCRibbonTab* pTab, BOOL bIsActive);
	virtual COLORREF OnDrawRibbonButtonsGroup(CDC* pDC, CMFCRibbonButtonsGroup* pGroup, CRect rectGroup);
	virtual COLORREF OnDrawRibbonCategoryCaption(CDC* pDC, CMFCRibbonContextCaption* pContextCaption);
	virtual COLORREF OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane);
	virtual void OnDrawRibbonSliderZoomButton(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsZoomOut, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonSliderChannel(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect);
	virtual void OnDrawRibbonSliderThumb(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* pProgress, CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode);
	virtual void OnDrawRibbonQuickAccessToolBarSeparator(CDC* pDC, CMFCRibbonSeparator* pSeparator, CRect rect);

protected:
	WinXpTheme m_WinXPTheme;

	COLORREF m_clrBarGradientDark;
	COLORREF m_clrBarGradientLight;
	COLORREF m_clrToolBarGradientDark;
	COLORREF m_clrToolBarGradientLight;
	COLORREF m_clrToolbarDisabled;
	COLORREF m_clrToolBarGradientVertLight;
	COLORREF m_clrToolBarGradientVertDark;
	COLORREF m_clrCustomizeButtonGradientDark;
	COLORREF m_clrCustomizeButtonGradientLight;
	COLORREF m_clrToolBarBottomLine;
	COLORREF m_colorToolBarCornerTop;
	COLORREF m_colorToolBarCornerBottom;
	COLORREF m_clrHighlightMenuItem;
	COLORREF m_clrHighlightGradientLight;
	COLORREF m_clrHighlightGradientDark;
	COLORREF m_clrHighlightDnGradientLight;
	COLORREF m_clrHighlightDnGradientDark;
	COLORREF m_clrHighlightCheckedGradientLight;
	COLORREF m_clrHighlightCheckedGradientDark;
	COLORREF m_clrGripper;
	COLORREF m_clrCaptionBarGradientLight;
	COLORREF m_clrCaptionBarGradientDark;
	COLORREF m_clrTaskPaneGradientDark;
	COLORREF m_clrTaskPaneGradientLight;
	COLORREF m_clrTaskPaneGroupCaptionDark;
	COLORREF m_clrTaskPaneGroupCaptionLight;
	COLORREF m_clrTaskPaneGroupCaptionSpecDark;
	COLORREF m_clrTaskPaneGroupCaptionSpecLight;
	COLORREF m_clrTaskPaneGroupAreaLight;
	COLORREF m_clrTaskPaneGroupAreaDark;
	COLORREF m_clrTaskPaneGroupAreaSpecLight;
	COLORREF m_clrTaskPaneGroupAreaSpecDark;
	COLORREF m_clrTaskPaneGroupBorder;

	CPen   m_penBottomLine;
	CPen   m_penSeparatorLight;
	CPen   m_penTaskPaneGroupBorder;

	CBrush m_brTearOffCaption;
	CBrush m_brFace;

	BOOL   m_bIsStandardWinXPTheme;

	AFX_IMPORT_DATA static BOOL m_bUseGlobalTheme;
	AFX_IMPORT_DATA static BOOL m_bStatusBarOfficeXPLook;
	AFX_IMPORT_DATA static BOOL m_bDefaultWinXPColors;

	COLORREF GetThemeColor(HTHEME hTheme, int nIndex) const;

	virtual void ModifyGlobalColors();
	virtual COLORREF GetWindowColor() const;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
