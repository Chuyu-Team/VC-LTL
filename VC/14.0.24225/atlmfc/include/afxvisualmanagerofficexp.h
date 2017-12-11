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
#include "afxvisualmanager.h"
#include "afxmenuimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCStatusBar;
class CBasePane;

class CMFCVisualManagerOfficeXP : public CMFCVisualManager
{
	DECLARE_DYNCREATE(CMFCVisualManagerOfficeXP)

protected:
	CMFCVisualManagerOfficeXP(BOOL bIsTemporary = FALSE);

public:
	virtual ~CMFCVisualManagerOfficeXP();

protected:
	virtual void OnUpdateSystemColors();

	virtual void OnDrawBarGripper(CDC* pDC, CRect rectGripper, BOOL bHorz, CBasePane* pBar);
	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);
	virtual void OnDrawPaneBorder(CDC* pDC, CBasePane* pBar, CRect& rect);
	virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	virtual void OnDrawMenuShadow(CDC* pDC, const CRect& rectClient, const CRect& rectExclude, int nDepth,
		int iMinBrightness, int iMaxBrightness, CBitmap* pBmpSaveBottom,  CBitmap* pBmpSaveRight, BOOL bRTL);
	virtual void OnDrawSeparator(CDC* pDC, CBasePane* pBar, CRect rect, BOOL bIsHoriz);
	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect);
	
	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnHighlightMenuItem(CDC*pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	virtual COLORREF GetHighlightedMenuItemTextColor(CMFCToolBarMenuButton* pButton);
	virtual void OnHighlightRarelyUsedMenuItems(CDC* pDC, CRect rectRarelyUsed);
	virtual void OnHighlightQuickCustomizeMenuButton(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect);
	virtual BOOL IsHighlightWholeMenuItem() { return TRUE; }

	virtual void OnEraseTabsArea(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnDrawTab(CDC* pDC, CRect rectTab, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);
	virtual void OnFillTab(CDC* pDC, CRect rectFill, CBrush* pbrFill, int iTab, BOOL bIsActive, const CMFCBaseTabCtrl* pTabWnd);

	virtual void OnDrawTabCloseButton(CDC* pDC, CRect rect, const CMFCBaseTabCtrl* pTabWnd, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnEraseTabsButton(CDC* pDC, CRect rect, CMFCButton* pButton, CMFCBaseTabCtrl* pWndTab);
	virtual void OnDrawTabsButtonBorder(CDC* pDC, CRect& rect, CMFCButton* pButton, UINT uiState, CMFCBaseTabCtrl* pWndTab);

	virtual COLORREF OnDrawPaneCaption(CDC* pDC, CDockablePane* pBar, BOOL bActive, CRect rectCaption, CRect rectButtons);

	virtual void OnDrawCaptionButton(CDC* pDC, CMFCCaptionButton* pButton, BOOL bActive, BOOL bHorz, BOOL bMaximized, BOOL bDisabled, int nImageID = -1);
	virtual void OnDrawCaptionButtonIcon(CDC* pDC, CMFCCaptionButton* pButton, CMenuImages::IMAGES_IDS id, BOOL bActive, BOOL bDisabled, CPoint ptImage);

	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawComboBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);
	virtual void OnDrawEditBorder(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsHighlighted, CMFCToolBarEditBoxButton* pButton);

	virtual COLORREF GetToolbarButtonTextColor(CMFCToolBarButton* pButton, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawTearOffCaption(CDC* pDC, CRect rect, BOOL bIsActive);
	virtual void OnDrawMenuResizeBar(CDC* pDC, CRect rect, int nResizeFlags);
	virtual void OnDrawMenuScrollButton(CDC* pDC, CRect rect, BOOL bIsScrollDown, BOOL bIsHighlited, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawSpinButtons(CDC* pDC, CRect rectSpin, int nState, BOOL bOrientation, CMFCSpinButtonCtrl* pSpinCtrl);

	virtual COLORREF OnFillCommandsListBackground(CDC* pDC, CRect rect, BOOL bIsSelected = FALSE);
	virtual void OnDrawMenuArrowOnCustomizeList(CDC* pDC, CRect rectCommand, BOOL bSelected);

	virtual void OnDrawMenuSystemButton(CDC* pDC, CRect rect, UINT uiSystemCommand, UINT nStyle, BOOL bHighlight);
	virtual void OnDrawStatusBarPaneBorder(CDC* pDC, CMFCStatusBar* pBar, CRect rectPane, UINT uiID, UINT nStyle);

	virtual int GetMenuImageMargin() const { return 3; }
	virtual int GetPopupMenuGap() const { return 0; }

	virtual void OnDrawButtonSeparator(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state, BOOL bHorz);

	virtual COLORREF OnFillMiniFrameCaption(CDC* pDC, CRect rectCaption, CPaneFrameWnd* pFrameWnd, BOOL bActive);
	virtual void OnDrawMiniFrameBorder(CDC* pDC, CPaneFrameWnd* pFrameWnd, CRect rectBorder, CRect rectBorderSize);
	virtual void OnDrawFloatingToolbarBorder(CDC* pDC, CMFCBaseToolBar* pToolBar, CRect rectBorder, CRect rectBorderSize);

	virtual COLORREF GetToolbarHighlightColor() { return m_clrHighlight; }

	virtual COLORREF OnFillCaptionBarButton(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, BOOL bIsPressed,
		BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);
	virtual void OnDrawCaptionBarButtonBorder(CDC* pDC, CMFCCaptionBar* pBar, CRect rect, BOOL bIsPressed,
		BOOL bIsHighlighted, BOOL bIsDisabled, BOOL bHasDropDownArrow, BOOL bIsSysButton);
	virtual void OnDrawCaptionBarInfoArea(CDC* pDC, CMFCCaptionBar* pBar, CRect rect);

	virtual COLORREF GetPropertyGridGroupColor(CMFCPropertyGridCtrl* pPropList);
	virtual COLORREF GetPropertyGridGroupTextColor(CMFCPropertyGridCtrl* pPropList);

	// Tasks pane:
	virtual void OnDrawTasksGroupCaption(CDC* pDC, CMFCTasksPaneTaskGroup* pGroup, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE, BOOL bCanCollapse = FALSE);

	virtual void OnFillTasksGroupInterior(CDC* pDC, CRect rect, BOOL bSpecial = FALSE);
	virtual void OnDrawTasksGroupAreaBorder(CDC* pDC, CRect rect, BOOL bSpecial = FALSE, BOOL bNoTitle = FALSE);
	virtual void OnDrawTask(CDC* pDC, CMFCTasksPaneTask* pTask, CImageList* pIcons, BOOL bIsHighlighted = FALSE, BOOL bIsSelected = FALSE);
	virtual void OnDrawScrollButtons(CDC* pDC, const CRect& rect, const int nBorderSize, int iImage, BOOL bHilited);

	virtual void OnDrawSplitterBorder(CDC* pDC, CSplitterWndEx* pSplitterWnd, CRect rect);
	virtual void OnDrawSplitterBox(CDC* pDC, CSplitterWndEx* pSplitterWnd, CRect& rect);

	// Edit box:
	virtual BOOL OnDrawBrowseButton(CDC* pDC, CRect rect, CMFCEditBrowseCtrl* pEdit, CMFCVisualManager::AFX_BUTTON_STATE state, COLORREF& clrText);

	virtual COLORREF GetAutoHideButtonTextColor(CMFCAutoHideButton* pButton);

	virtual void GetSmartDockingBaseGuideColors(COLORREF& clrBaseGroupBackground, COLORREF& clrBaseGroupBorder);

	virtual COLORREF OnDrawPropertySheetListItem(CDC* pDC, CMFCPropertySheet* pParent, CRect rect, BOOL bIsHighlihted, BOOL bIsSelected);

	// Popup window:
	virtual void OnDrawPopupWindowBorder(CDC* pDC, CRect rect);
	virtual COLORREF OnDrawPopupWindowCaption(CDC* pDC, CRect rectCaption, CMFCDesktopAlertWnd* pPopupWnd);
	virtual void OnErasePopupWindowButton(CDC* pDC, CRect rectClient, CMFCDesktopAlertWndButton* pButton);
	virtual void OnDrawPopupWindowButtonBorder(CDC* pDC, CRect rectClient, CMFCDesktopAlertWndButton* pButton);
	virtual void OnFillPopupWindowBackground(CDC* pDC, CRect rect);

	// Ribbon control:
	virtual COLORREF OnFillRibbonButton(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonCategoryScroll(CDC* pDC, CRibbonCategoryScroll* pScroll);
	virtual void OnDrawRibbonButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual void OnDrawRibbonMenuCheckFrame(CDC* pDC, CMFCRibbonButton* pButton, CRect rect);
	virtual void OnFillRibbonMenuFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual void OnDrawRibbonRecentFilesFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual COLORREF OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane);

	virtual void GetRibbonSliderColors(CMFCRibbonSlider* pSlider, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled, COLORREF& clrLine, COLORREF& clrFill);
	virtual void OnDrawRibbonQuickAccessToolBarSeparator(CDC* pDC, CMFCRibbonSeparator* pSeparator, CRect rect);

	virtual void OnDrawRibbonColorPaletteBox(CDC* pDC, CMFCRibbonColorButton* pColorButton, CMFCRibbonGalleryIcon* pIcon,
		COLORREF color, CRect rect, BOOL bDrawTopEdge, BOOL bDrawBottomEdge, BOOL bIsHighlighted, BOOL bIsChecked, BOOL bIsDisabled);


	COLORREF m_clrBarBkgnd;            // Control bar background color (expect menu bar)
	COLORREF m_clrMenuRarelyUsed;
	COLORREF m_clrMenuLight;           // Color of the light menu area
	COLORREF m_clrInactiveTabText;
	COLORREF m_clrHighlight;           // Highlighted toolbar/menu item color
	COLORREF m_clrHighlightDn;         // Highlighted and pressed toolbar item color
	COLORREF m_clrHighlightChecked;
	COLORREF m_clrPressedButtonBorder; // Used in derived classes
	COLORREF m_clrGripper;
	COLORREF m_clrSeparator;
	COLORREF m_clrPaneBorder;          // Status bar pane border
	COLORREF m_clrMenuBorder;          // Menu border
	COLORREF m_clrMenuItemBorder;      // Highlighted menu item border

	CBrush m_brGripperHorz;
	CBrush m_brGripperVert;
	CBrush m_brBarBkgnd;
	CBrush m_brMenuRarelyUsed;
	CBrush m_brMenuLight;
	CBrush m_brTabBack;
	CBrush m_brHighlight;
	CBrush m_brHighlightDn;
	CBrush m_brHighlightChecked;
	CBrush m_brFloatToolBarBorder;     // Floating toolbar border

	CPen m_penSeparator;
	CPen m_penMenuItemBorder;

	BOOL m_bConnectMenuToParent;
	BOOL m_bShdowDroppedDownMenuButton;
	BOOL m_bDrawLastTabLine;

	virtual void CreateGripperBrush();
	virtual void ExtendMenuButton(CMFCToolBarMenuButton* pMenuButton, CRect& rect);
	virtual COLORREF GetWindowColor() const;
	virtual void OnFillHighlightedArea(CDC* pDC, CRect rect, CBrush* pBrush, CMFCToolBarButton* pButton);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
