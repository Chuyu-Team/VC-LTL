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

#include "afxvisualmanagerwindows.h"
#include "afxvisualmanageroffice2007.h"
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

class CMFCVisualManagerWindows7 : public CMFCVisualManagerWindows
{
	DECLARE_DYNCREATE(CMFCVisualManagerWindows7)

public:
	static BOOL __stdcall SetStyle(LPCTSTR lpszPath = NULL);
	static void __stdcall SetResourceHandle(HINSTANCE hinstRes);
	static void __stdcall CleanStyle();

protected:
	static CString __stdcall MakeResourceID(LPCTSTR lpszID);
	static CString __stdcall GetStyleResourceID();

	BOOL CanDrawImage() const
	{
		return GetGlobalData()->m_nBitsPerPixel > 8 && !GetGlobalData()->IsHighContrastMode() && m_bLoaded;
	}

public:
	CMFCVisualManagerWindows7();
	virtual ~CMFCVisualManagerWindows7();

	virtual BOOL IsOwnerDrawMenuCheck();
	virtual BOOL IsHighlightWholeMenuItem();

	virtual BOOL OnNcActivate(CWnd* pWnd, BOOL bActive);
	virtual BOOL OnNcPaint(CWnd* pWnd, const CObList& lstSysButtons, CRect rectRedraw);

	BOOL IsRibbonPresent(CWnd* pWnd) const;
	CMFCRibbonBar*	GetRibbonBar(CWnd* pWnd) const;

	virtual void DrawSeparator(CDC* pDC, const CRect& rect, CPen& pen1, CPen& pen2, BOOL bHorz);
    virtual void DrawNcBtn(CDC* pDC, const CRect& rect, UINT nButton, AFX_BUTTON_STATE state, BOOL bSmall, BOOL bActive, BOOL bMDI = FALSE);
	virtual void DrawNcText(CDC* pDC, CRect& rect, const CString& strTitle, BOOL bActive, BOOL bIsRTL, BOOL bTextCenter, BOOL bGlass = FALSE, int nGlassGlowSize = 0, COLORREF clrGlassText = (COLORREF)-1);

	virtual void OnUpdateSystemColors();
	virtual void CleanUp();

	virtual void OnHighlightMenuItem(CDC *pDC, CMFCToolBarMenuButton* pButton, CRect rect, COLORREF& clrText);
	virtual void OnDrawMenuBorder(CDC* pDC, CMFCPopupMenu* pMenu, CRect rect);
	virtual void OnDrawMenuCheck(CDC* pDC, CMFCToolBarMenuButton* pButton, CRect rect, BOOL bHighlight, BOOL bIsRadio);
	virtual void OnFillMenuImageRect(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual COLORREF OnDrawMenuLabel(CDC* pDC, CRect rect);

	virtual void OnFillButtonInterior(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);
	virtual void OnDrawButtonBorder(CDC* pDC, CMFCToolBarButton* pButton, CRect rect, CMFCVisualManager::AFX_BUTTON_STATE state);

	virtual void OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rectCaption, CRect rectText);
    virtual void OnDrawRibbonCaptionButton(CDC* pDC, CMFCRibbonCaptionButton* pButton);

	virtual void OnDrawRibbonCategory(CDC* pDC, CMFCRibbonCategory* pCategory, CRect rectCategory);

	virtual void OnDrawRibbonGalleryButton(CDC* pDC, CMFCRibbonGalleryIcon* pButton);

	virtual COLORREF OnDrawRibbonCategoryCaption(CDC* pDC, CMFCRibbonContextCaption* pContextCaption);
	virtual COLORREF OnDrawRibbonStatusBarPane(CDC* pDC, CMFCRibbonStatusBar* pBar, CMFCRibbonStatusBarPane* pPane);

	virtual COLORREF OnDrawRibbonCategoryTab(CDC* pDC, CMFCRibbonTab* pTab, BOOL bIsActive);
	virtual COLORREF OnDrawRibbonPanel(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectPanel, CRect rectCaption);

	virtual void OnDrawRibbonPanelCaption(CDC* pDC, CMFCRibbonPanel* pPanel, CRect rectCaption);
	virtual void OnDrawRibbonLaunchButton(CDC* pDC, CMFCRibbonLaunchButton* pButton, CMFCRibbonPanel* pPanel);
	virtual void OnDrawRibbonCategoryScroll(CDC* pDC, CRibbonCategoryScroll* pScroll);

	virtual COLORREF OnDrawRibbonTabsFrame(CDC* pDC, CMFCRibbonBar* pWndRibbonBar, CRect rectTab);
	virtual void OnDrawRibbonSliderZoomButton(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsZoomOut, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonSliderChannel(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect);
	virtual void OnDrawRibbonSliderThumb(CDC* pDC, CMFCRibbonSlider* pSlider, CRect rect, BOOL bIsHighlighted, BOOL bIsPressed, BOOL bIsDisabled);
	virtual void OnDrawRibbonProgressBar(CDC* pDC, CMFCRibbonProgressBar* pProgress, CRect rectProgress, CRect rectChunk, BOOL bInfiniteMode);

	virtual void OnDrawDefaultRibbonImage(CDC* pDC, CRect rectImage, BOOL bIsDisabled = FALSE, BOOL bIsPressed = FALSE, BOOL bIsHighlighted = FALSE);
	virtual void OnDrawRibbonApplicationButton(CDC* pDC, CMFCRibbonButton* pButton);

	virtual void OnFillBarBackground(CDC* pDC, CBasePane* pBar, CRect rectClient, CRect rectClip, BOOL bNCArea = FALSE);

	virtual void OnDrawStatusBarSizeBox(CDC* pDC, CMFCStatusBar* pStatBar, CRect rectSizeBox);
	virtual void OnDrawCheckBoxEx(CDC *pDC, CRect rect, int nState, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled);
	virtual void OnDrawRadioButton(CDC *pDC, CRect rect, BOOL bOn, BOOL bHighlighted, BOOL bPressed, BOOL bEnabled);

	virtual void OnDrawRibbonButtonBorder(CDC* pDC, CMFCRibbonButton* pButton);
	virtual COLORREF OnFillRibbonButton(CDC* pDC, CMFCRibbonButton* pButton);

	virtual void OnDrawRibbonMenuCheckFrame(CDC* pDC, CMFCRibbonButton* pButton, CRect rect);
	virtual void OnDrawRibbonDefaultPaneButton(CDC* pDC, CMFCRibbonButton* pButton);

	virtual void OnFillRibbonQuickAccessToolBarPopup(CDC* pDC, CMFCRibbonPanelMenuBar* pMenuBar, CRect rect);

	virtual void OnDrawRibbonRecentFilesFrame(CDC* pDC, CMFCRibbonMainPanel* pPanel, CRect rect);
	virtual void OnDrawComboDropButton(CDC* pDC, CRect rect, BOOL bDisabled, BOOL bIsDropped, BOOL bIsHighlighted, CMFCToolBarComboBoxButton* pButton);

	virtual int GetRibbonQuickAccessToolBarChevronOffset() { return 13; }
	virtual int GetRibbonQuickAccessToolBarRightMargin() { return 0; }
	virtual int GetRibbonPopupBorderSize(const CMFCRibbonPanelMenu* pPopup) const;
	virtual COLORREF GetRibbonEditBackgroundColor(CMFCRibbonRichEditCtrl* pEdit, BOOL bIsHighlighted, BOOL bIsPaneHighlighted, BOOL bIsDisabled);

protected:
	CFont	m_AppCaptionFont;
	CPen	m_penSeparatorDark;
	CPen	m_penSeparatorLight;

	BOOL	m_bLoaded;

	COLORREF m_clrRibbonBarBkgnd;
	CBrush   m_brRibbonBarBkgnd;
    COLORREF m_clrRibbonBarGradientLight;
    COLORREF m_clrRibbonBarGradientDark;

	COLORREF m_clrRibbonPanelCaptionText;
	COLORREF m_clrRibbonPanelCaptionTextHighlighted;

	COLORREF m_clrRibbonEdit;
	COLORREF m_clrRibbonEditDisabled;
	COLORREF m_clrRibbonEditHighlighted;
	COLORREF m_clrRibbonEditPressed;
	COLORREF m_clrRibbonEditBorder;
	COLORREF m_clrRibbonEditBorderDisabled;
	COLORREF m_clrRibbonEditBorderHighlighted;
	COLORREF m_clrRibbonEditBorderPressed;
	COLORREF m_clrRibbonEditSelection;

	CMFCControlRenderer m_ctrlRibbonComboBoxBtn;
	CMFCControlRenderer m_ctrlMenuItemBack;
    CMFCToolBarImages   m_MenuItemMarkerC;
    CMFCToolBarImages   m_MenuItemMarkerR;
	CMFCControlRenderer m_ctrlMenuHighlighted[2];

	CMFCControlRenderer m_ctrlRibbonCaptionQA;

	COLORREF            m_clrRibbonCategoryText;
	COLORREF            m_clrRibbonCategoryTextHighlighted;
	COLORREF            m_clrRibbonCategoryTextDisabled;
	CMFCControlRenderer m_ctrlRibbonCategoryBack;
	CMFCControlRenderer m_ctrlRibbonCategoryTab;
	CMFCControlRenderer m_ctrlRibbonCategoryTabSep;
	CMFCControlRenderer m_ctrlRibbonCategoryBtnPage[2];
	CMFCControlRenderer m_ctrlRibbonPanelBack;
	CMFCControlRenderer m_ctrlRibbonPanelBackSep;
	CMFCControlRenderer m_ctrlRibbonMainPanel;
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
	CMFCToolBarImages m_RibbonBtnLaunchIcon;
	CMFCControlRenderer m_ctrlRibbonBtnMain;
	CMFCControlRenderer m_ctrlRibbonSliderBtnPlus;
	CMFCControlRenderer m_ctrlRibbonSliderBtnMinus;
	CMFCToolBarImages	 m_RibbonBtnDefaultImage;
	CMFCControlRenderer m_ctrlRibbonBtnDefault;
	CMFCControlRenderer m_ctrlRibbonBtnDefaultIcon;
	CMFCControlRenderer m_ctrlRibbonBtnDefaultQAT;
	CMFCControlRenderer m_ctrlRibbonBtnStatusPane;
	CMFCControlRenderer m_ctrlRibbonBtnPalette[3];

	CMFCControlRenderer m_ctrlRibbonBorder_QAT;
	CMFCControlRenderer m_ctrlRibbonBorder_Panel;

	struct CMFCRibbonContextCategory
	{
		CMFCControlRenderer          m_ctrlCaption;
		CMFCControlRenderer          m_ctrlBack;
		CMFCControlRenderer          m_ctrlTab;
		CMFCVisualManagerBitmapCache m_cacheBack;
		COLORREF                     m_clrCaptionText;
		COLORREF                     m_clrText;
		COLORREF                     m_clrTextHighlighted;

		void CleanUp()
		{
			m_ctrlCaption.CleanUp();
			m_ctrlBack.CleanUp();
			m_ctrlTab.CleanUp();
			m_cacheBack.Clear();
		}
	};

	CMFCControlRenderer       m_ctrlRibbonContextSeparator;
	CMFCRibbonContextCategory m_ctrlRibbonContextCategory[AFX_RIBBON_CATEGORY_COLOR_COUNT];

	CMFCVisualManagerBitmapCache m_cacheRibbonCategoryBack;
	CMFCVisualManagerBitmapCache m_cacheRibbonPanelBack;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_S;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_F;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_M;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroup_L;
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroupMenu_F[2];
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroupMenu_M[2];
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnGroupMenu_L[2];
	CMFCVisualManagerBitmapCache m_cacheRibbonBtnDefault;

	AFX_IMPORT_DATA static CString   m_strStylePrefix;
	AFX_IMPORT_DATA static HINSTANCE m_hinstRes;
	AFX_IMPORT_DATA static BOOL      m_bAutoFreeRes;

	CMap<HWND, HWND, BOOL, BOOL> m_ActivateFlag;

	BOOL IsWindowActive(CWnd* pWnd) const;
	CSize GetSystemBorders(BOOL bRibbonPresent) const;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
