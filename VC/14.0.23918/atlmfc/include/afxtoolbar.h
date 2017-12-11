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

#include "afxcontrolbarutil.h"
#include "afxtoolbardroptarget.h"
#include "afxtoolbardropsource.h"
#include "afxtoolbarimages.h"
#include "afxtoolbarbutton.h"
#include "afxcontrolbarimpl.h"
#include "afxcmdusagecount.h"
#include "afxcommandmanager.h"
#include "afxvisualmanager.h"
#include "afxglobals.h"

#include "afxbasetoolbar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarMenuButton;
class CMFCCustomizeButton;
class CSettingsStore;

//-----------------------------------------------
// CMFCToolBar LoadBitmap/LoadToolbar parameters:
//-----------------------------------------------
class CMFCToolBarInfo
{
public:
	CMFCToolBarInfo();

	UINT m_uiColdResID;          // Toolbar regular images
	UINT m_uiHotResID;           // Toolbar "hot" images
	UINT m_uiDisabledResID;      // Toolbar disabled images
	UINT m_uiLargeColdResID;     // Toolbar large regular images
	UINT m_uiLargeHotResID;      // Toolbar large "hot" images
	UINT m_uiLargeDisabledResID; // Toolbar large disabled images
	UINT m_uiMenuResID;          // Menu images
	UINT m_uiMenuDisabledResID;  // Menu disabled images
};

//----------------------------------
// Toolbar notification messages:
//----------------------------------
extern AFX_IMPORT_DATA UINT AFX_WM_TOOLBARMENU;
extern AFX_IMPORT_DATA UINT AFX_WM_CUSTOMIZETOOLBAR;
extern AFX_IMPORT_DATA UINT AFX_WM_CREATETOOLBAR;
extern AFX_IMPORT_DATA UINT AFX_WM_DELETETOOLBAR;
extern AFX_IMPORT_DATA UINT AFX_WM_CUSTOMIZEHELP;
extern AFX_IMPORT_DATA UINT AFX_WM_RESETTOOLBAR;
extern AFX_IMPORT_DATA UINT AFX_WM_RESETMENU;
extern AFX_IMPORT_DATA UINT AFX_WM_SHOWREGULARMENU;
extern AFX_IMPORT_DATA UINT AFX_WM_RESETCONTEXTMENU;
extern AFX_IMPORT_DATA UINT AFX_WM_RESETKEYBOARD;
extern AFX_IMPORT_DATA UINT AFX_WM_RESETRPROMPT;

extern const UINT AFX_ACCELERATOR_TIMER_DELAY;

#define AFX_TOOLBAR_LINE_OFFSET 5

class CMFCToolBar : public CMFCBaseToolBar
{
	friend class CMFCToolBarDropTarget;
	friend class CMFCToolBarsListPropertyPage;
	friend class CMFCToolBarsCommandsListBox;
	friend class CMFCCustomizeButton;
	friend class CMFCCustomizeMenuButton;
	friend class CMFCToolTipCtrl;

	DECLARE_SERIAL(CMFCToolBar)

	//--------------
	// Construction:
	//--------------
public:
	CMFCToolBar();
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE, UINT nID = AFX_IDW_TOOLBAR);
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE,
		CRect rcBorders = CRect(1, 1, 1, 1), UINT nID = AFX_IDW_TOOLBAR);

	//----------------------------------------------------------------
	// Set toolbar buttons image resources.
	// You should use SINGLE CMFCToolBarImages for ALL your toolbars!
	//----------------------------------------------------------------
	static BOOL __stdcall SetUserImages(CMFCToolBarImages* pUserImages);
	virtual void ResetImages();	// Reset all toolbar images exept user-derfined to the default

	//------------------------------
	// Clear all images except user:
	//------------------------------
	static void __stdcall ResetAllImages();

	//--------------------------------
	// Dimension manipulation methods:
	//--------------------------------
	static void __stdcall SetSizes(SIZE sizeButton, SIZE sizeImage);
	static void __stdcall SetMenuSizes(SIZE sizeButton, SIZE sizeImage);
	static CSize __stdcall GetMenuImageSize();
	static CSize __stdcall GetMenuButtonSize();

	/// <summary>
	/// Sets the sizes of locked buttons and locked images on the toolbar. </summary>
	/// <param name="sizeButton">Specifies the size of locked toolbar buttons. </param>
	/// <param name="sizeImage">Specifies the size of locked toolbar images. </param>
	/// <param name="bDontScale">Specifies whether to scale or not locked toolbar images in high DPI mode.</param>
	void SetLockedSizes(SIZE sizeButton, SIZE sizeImage, BOOL bDontScale = FALSE);
	void SetHeight(int cyHeight);

	//-----------------
	// Toolbar context:
	//-----------------
	virtual BOOL LoadBitmap(UINT uiResID, UINT uiColdResID = 0, UINT uiMenuResID = 0, BOOL bLocked = FALSE, UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0);
	virtual BOOL LoadToolBar(UINT uiResID, UINT uiColdResID = 0, UINT uiMenuResID = 0, BOOL bLocked = FALSE,
		UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0,  UINT uiHotResID = 0);
	virtual BOOL LoadBitmapEx(CMFCToolBarInfo& params, BOOL bLocked = FALSE);
	virtual BOOL LoadToolBarEx(UINT uiToolbarResID, CMFCToolBarInfo& params, BOOL bLocked = FALSE);

	//----------------------------
	// Toolbar buttons add/remove:
	//----------------------------
	virtual BOOL SetButtons(const UINT* lpIDArray, int nIDCount, BOOL bRemapImages = TRUE);

	virtual int InsertButton(const CMFCToolBarButton& button, INT_PTR iInsertAt = -1);
	virtual int InsertSeparator(INT_PTR iInsertAt = -1);

	int ReplaceButton(UINT uiCmd, const CMFCToolBarButton& button, BOOL bAll = FALSE);
	
	virtual BOOL RemoveButton(int iIndex);
	virtual void RemoveAllButtons();

	static BOOL __stdcall IsLastCommandFromButton(CMFCToolBarButton* pButton);
	static BOOL __stdcall AddToolBarForImageCollection(UINT uiResID, UINT uiBmpResID = 0, UINT uiColdResID = 0,
		UINT uiMenuResID = 0, UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0);

	static void __stdcall SetNonPermittedCommands(CList<UINT, UINT>& lstCommands);
	static BOOL __stdcall IsCommandPermitted(UINT uiCmd) { return m_lstUnpermittedCommands.Find(uiCmd) == NULL; }

	static void __stdcall SetBasicCommands(CList<UINT, UINT>& lstCommands);
	static void __stdcall AddBasicCommand(UINT uiCmd);
	
	static BOOL __stdcall IsBasicCommand(UINT uiCmd) { return m_lstBasicCommands.Find(uiCmd) != NULL; }
	static const CList<UINT, UINT>& __stdcall GetBasicCommands() { return m_lstBasicCommands; }

	static BOOL __stdcall IsCommandRarelyUsed(UINT uiCmd);

	static void __stdcall AddCommandUsage(UINT uiCommand);
	static BOOL __stdcall SetCommandUsageOptions(UINT nStartCount, UINT nMinUsagePercentage = 5);

	virtual int GetRowHeight() const;

	virtual int GetColumnWidth() const
	{
		return m_bMenuMode ? m_sizeMenuButton.cx > 0 ? m_sizeMenuButton.cx : m_sizeButton.cx : GetButtonSize().cx;
	}

	virtual BOOL IsButtonExtraSizeAvailable() const { return TRUE; }

	static void __stdcall SetHelpMode(BOOL bOn = TRUE);
	virtual void Deactivate();
	virtual void RestoreFocus();

	void SetToolBarBtnText(UINT nBtnIndex, LPCTSTR szText = NULL, BOOL bShowText = TRUE, BOOL bShowImage = TRUE);

	virtual BOOL CanFocus() const { return FALSE; }

	void EnableLargeIcons(BOOL bEnable);

	static void __stdcall SetLargeIcons(BOOL bLargeIcons = TRUE);
	static BOOL __stdcall IsLargeIcons() { return m_bLargeIcons; }

	static void __stdcall AutoGrayInactiveImages(BOOL bEnable = TRUE, int nGrayImagePercentage = 0, BOOL bRedrawAllToolbars = TRUE);
	static BOOL __stdcall IsAutoGrayInactiveImages() { return m_bAutoGrayInactiveImages; }

	CSize GetButtonSize() const
	{
		CSize size = m_bLocked ? (m_bLargeIconsAreEnbaled ? m_sizeCurButtonLocked : m_sizeButtonLocked) : (m_bLargeIconsAreEnbaled ? m_sizeCurButton : m_sizeButton);
		if (IsButtonExtraSizeAvailable())
		{
			size += CMFCVisualManager::GetInstance()->GetButtonExtraBorder();
		}
		return size;
	}

	CSize GetImageSize() const
	{
		return m_bLocked ? m_bLargeIconsAreEnbaled ? m_sizeCurImageLocked : m_sizeImageLocked : m_bLargeIconsAreEnbaled ? m_sizeCurImage : m_sizeImage;
	}

	CSize GetLockedImageSize() const
	{
		if (!m_bLocked)
		{
			ASSERT(FALSE);
			return CSize(0, 0);
		}
		return m_sizeImageLocked;
	}

	BOOL IsButtonHighlighted(int iButton) const;

	//------------
	// Attributes:
	//------------
public: 
	BOOL IsLocked() const { return m_bLocked; }
	UINT GetResourceID() const { return m_uiOriginalResID; }
	virtual BOOL IsResourceChanged() const { return m_bResourceWasChanged; }
	virtual BOOL IsFloating() const { return m_bFloating; }

	static const CObList& __stdcall GetAllToolbars();

	// standard control bar things
	int CommandToIndex(UINT nIDFind, int iIndexFirst = 0) const;
	UINT GetItemID(int nIndex) const;
	
	virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
	virtual void GetInvalidateItemRect(int nIndex, LPRECT lpRect) const;

	UINT GetButtonStyle(int nIndex) const;
	virtual void SetButtonStyle(int nIndex, UINT nStyle);

	int GetCount() const;
	int ButtonToIndex(const CMFCToolBarButton* pButton) const;
	CMFCToolBarButton* GetButton(int iIndex) const;

	const CObList& GetAllButtons() const { return m_Buttons; }
	const CObList& GetOrigButtons() const { return m_OrigButtons; }
	const CObList& GetOrigResetButtons() const { return m_OrigResetButtons; }

	void SetOrigButtons(const CObList& lstOrigButtons);

	// Find all buttons specified by the given command ID from the all toolbars:
	static int __stdcall GetCommandButtons(UINT uiCmd, CObList& listButtons);
	
	static BOOL __stdcall SetCustomizeMode(BOOL bSet = TRUE);
	static BOOL __stdcall IsCustomizeMode() { return m_bCustomizeMode; }

	static BOOL __stdcall IsAltCustomizeMode() { return m_bAltCustomizeMode; }

	BOOL IsUserDefined() const;

	static CMFCToolBar* __stdcall FromHandlePermanent(HWND hwnd);

	static CMFCToolBarImages* __stdcall GetImages() { return &m_Images; }
	static CMFCToolBarImages* __stdcall GetColdImages() { return &m_ColdImages; }
	static CMFCToolBarImages* __stdcall GetDisabledImages() { return &m_DisabledImages; }
	static CMFCToolBarImages* __stdcall GetLargeImages() { return &m_LargeImages; }
	static CMFCToolBarImages* __stdcall GetLargeColdImages() { return &m_LargeColdImages; }
	static CMFCToolBarImages* __stdcall GetLargeDisabledImages() { return &m_LargeDisabledImages; }
	static CMFCToolBarImages* __stdcall GetMenuImages() { return &m_MenuImages; }
	static CMFCToolBarImages* __stdcall GetDisabledMenuImages() { return &m_DisabledMenuImages; }
	static CMFCToolBarImages* __stdcall GetUserImages() { return m_pUserImages; }

	CMFCToolBarImages* GetLockedImages()
	{
		if (!m_bLocked)
		{
			ASSERT(FALSE);
			return NULL;
		}
		return &m_ImagesLocked;
	}

	CMFCToolBarImages* GetLockedColdImages()
	{
		if (!m_bLocked)
		{
			ASSERT(FALSE);
			return NULL;
		}
		return &m_ColdImagesLocked;
	}

	CMFCToolBarImages* GetLockedDisabledImages()
	{
		if (!m_bLocked)
		{
			ASSERT(FALSE);
			return NULL;
		}
		return &m_DisabledImagesLocked;
	}
	
	CMFCToolBarImages* GetLockedMenuImages()
	{
		if (!m_bLocked)
		{
			ASSERT(FALSE);
			return NULL;
		}

		if (m_MenuImagesLocked.GetCount() > 0)
		{
			return &m_MenuImagesLocked;
		}

		if (m_ImagesLocked.GetCount() > 0)
		{
			return &m_ImagesLocked;
		}

		return NULL;
	}

	static int __stdcall GetDefaultImage(UINT uiID)
	{
		int iImage;
		if (m_DefaultImages.Lookup(uiID, iImage))
		{
			return iImage;
		}
		return -1;
	}

	int GetImagesOffset() const { return m_iImagesOffset; }

	CMFCToolBarButton* GetHighlightedButton() const;

	static void __stdcall SetHotTextColor(COLORREF clrText);
	static COLORREF __stdcall GetHotTextColor();

	void SetHotBorder(BOOL bShowHotBorder) { m_bShowHotBorder = bShowHotBorder; }
	BOOL GetHotBorder() const { return m_bShowHotBorder; }

	void SetGrayDisabledButtons(BOOL bGrayDisabledButtons) { m_bGrayDisabledButtons = bGrayDisabledButtons; }
	BOOL GetGrayDisabledButtons() const { return m_bGrayDisabledButtons; }

	//------------------------------------------------------
	// Enable/disable quick customization mode ("Alt+drag"):
	//------------------------------------------------------
	static void __stdcall EnableQuickCustomization(BOOL bEnable = TRUE) { m_bAltCustomization = bEnable; }

	virtual void EnableDocking(DWORD dwAlignment); 
	
	void EnableCustomizeButton(BOOL bEnable, int iCustomizeCmd, const CString& strCustomizeText, BOOL bQuickCustomize = TRUE);
	void EnableCustomizeButton(BOOL bEnable, int iCustomizeCmd, UINT uiCustomizeTextResId, BOOL bQuickCustomize = TRUE);

	BOOL IsExistCustomizeButton()
	{
		if(m_pCustomizeBtn == NULL)
		{
			return FALSE;
		}
		return TRUE;
	}

	CMFCCustomizeButton* GetCustomizeButton() { return m_pCustomizeBtn; }

	void EnableTextLabels(BOOL bEnable = TRUE);
	virtual BOOL AllowChangeTextLabels() const { return TRUE; }
	BOOL AreTextLabels() const { return m_bTextLabels; }

	virtual BOOL OnBeforeRemoveButton(CMFCToolBarButton* /*pButton*/, DROPEFFECT /*dropEffect*/) { return TRUE; }

	void SetMaskMode(BOOL bMasked) { m_bMasked = bMasked; }
	void SetPermament(BOOL bPermament = TRUE) { m_bPermament = bPermament; }

	BOOL GetIgnoreSetText() const { return m_bIgnoreSetText; }
	void SetIgnoreSetText(BOOL bValue) { m_bIgnoreSetText = bValue; }

	BOOL GetRouteCommandsViaFrame() { return m_bRouteCommandsViaFrame; }
	void SetRouteCommandsViaFrame(BOOL bValue) { m_bRouteCommandsViaFrame = bValue; }

	BOOL IsAddRemoveQuickCustomize() { return m_bQuickCustomize; }

	BOOL IsSibling() { return m_bHasBrother; }
	CMFCToolBar* GetSiblingToolBar() { return m_pBrotherToolBar; }

	void SetSiblingToolBar(CMFCToolBar* pBrotherToolbar);
	BOOL IsOneRowWithSibling();
	void SetOneRowWithSibling();
	void SetTwoRowsWithSibling();
	BOOL CanHandleSiblings();

	void EnableReflections(BOOL bEnable = TRUE) { m_bAllowReflections = bEnable; }

	static BOOL __stdcall GetShowTooltips() { return m_bShowTooltips; }
	static void __stdcall SetShowTooltips(BOOL bValue) { m_bShowTooltips = bValue; }

	HWND GetHwndLastFocus() const { return m_hwndLastFocus; }

	AFX_IMPORT_DATA static BOOL m_bDisableLabelsEdit;

public:
	// for changing button info
	void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
	void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);

	BOOL SetButtonText(int nIndex, LPCTSTR lpszText);
	CString GetButtonText( int nIndex ) const;
	void GetButtonText( int nIndex, CString& rString ) const;

	// Save/load toolbar state + buttons:
	void Serialize(CArchive& ar);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL RemoveStateFromRegistry(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	static BOOL __stdcall LoadParameters(LPCTSTR lpszProfileName = NULL);
	static BOOL __stdcall SaveParameters(LPCTSTR lpszProfileName = NULL);
	static BOOL __stdcall LoadLargeIconsState(LPCTSTR lpszProfileName = NULL);

	virtual BOOL CanBeRestored() const;
	virtual BOOL CanBeClosed() const { return !m_bPermament; }

	virtual BOOL RestoreOriginalState();
	virtual void OnReset() {}

	static void __stdcall ResetAll();
	static void __stdcall RedrawUnderlines();

	virtual void AdjustLayout();
	virtual int HitTest(CPoint point);
	virtual BOOL TranslateChar(UINT nChar);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	virtual BOOL PrevMenu();
	virtual BOOL NextMenu();
	BOOL ProcessCommand(CMFCToolBarButton* pButton);
	CMFCToolBarMenuButton* GetDroppedDownMenu(int* pIndex = NULL) const;

	BOOL SetHot(CMFCToolBarButton *pMenuButton);

	virtual BOOL OnSetDefaultButtonText(CMFCToolBarButton* pButton);

	BOOL IsDragButton(const CMFCToolBarButton* pButton) const { return pButton == m_pDragButton; }

	virtual void  OnFillBackground(CDC* /*pDC*/) {}
	virtual void OnGlobalFontsChanged();

	AFX_IMPORT_DATA static BOOL   m_bExtCharTranslation;
	AFX_IMPORT_DATA static double m_dblLargeImageRatio;  // large image stretch ratio (default: * 2)
	/// <summary> Specifies whether to scale or not toolbar images in high DPI mode.</summary>
	AFX_IMPORT_DATA static BOOL   m_bDontScaleImages;    // don't scale shared images in high DPI mode

	static void __stdcall CMFCToolBar::CleanUpImages();
	void CleanUpLockedImages();

	void AdjustSize();

	virtual BOOL OnUserToolTip(CMFCToolBarButton* pButton, CString& strTTText) const;
	virtual BOOL OnSetAccData(long lVal);

// Implementation
public:
	virtual ~CMFCToolBar();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual CSize CalcSize(BOOL bVertDock);
	int WrapToolBar(int nWidth, int nHeight = 32767, CDC* pDC = NULL, int nColumnWidth = -1, int nRowHeight = -1);

	virtual void OnChangeHot(int iHot);

	virtual CSize StretchPane(int nLength, BOOL bVert);

	CMFCToolBarButton* InvalidateButton(int nIndex);
	void UpdateButton(int nIndex);

	virtual void OnChangeVisualManager();

protected:
	virtual CSize CalcLayout(DWORD dwMode, int nLength = -1);
	void  SizeToolBar(int nLength, BOOL bVert = FALSE);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual int InsertButton(CMFCToolBarButton* pButton, int iInsertAt = -1);

	virtual BOOL DrawButton(CDC* pDC, CMFCToolBarButton* pButton, CMFCToolBarImages* pImages, BOOL bHighlighted, BOOL bDrawDisabledImages);
	virtual void DrawDragCursor(CDC* pDC);
	CMFCToolBarImages* GetImageList(CMFCToolBarImages& images, CMFCToolBarImages& imagesLocked, CMFCToolBarImages& largeImages, CMFCToolBarImages& largeImagesLocked) const;

	void RebuildAccelerationKeys();
	virtual CWnd* GetCommandTarget() const
	{
		// determine target of command update
		CFrameWnd* pTarget = (CFrameWnd*) GetOwner();
		if (pTarget == NULL || (m_bRouteCommandsViaFrame && !pTarget->IsFrameWnd()))
		{
			pTarget = AFXGetParentFrame(this);
		}

		return pTarget;
	}

	void UpdateTooltips();

	virtual void OnAfterFloat();
	virtual void OnAfterDock(CBasePane* /*pBar*/, LPCRECT /*lpRect*/, AFX_DOCK_METHOD /*dockMethod*/);
	virtual void OnBeforeChangeParent(CWnd* pWndNewParent, BOOL bDelay = FALSE);

	virtual void AccNotifyObjectFocusEvent(int iButton);
	virtual int AccGetButtonsCount();

	CMFCToolBarButton* AccGetButtonByChildId(long lVal);
	int AccGetChildIdByButtonIndex(int nButtonIndex);

	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);

protected:
	friend class CWinAppEx;

	AFX_IMPORT_DATA static CMFCToolBar* m_pSelToolbar;        // "Selected" toolbar in the customization mode

	AFX_IMPORT_DATA static CMFCToolBarImages m_Images;              // Shared toolbar images
	AFX_IMPORT_DATA static CMFCToolBarImages m_ColdImages;          // Shared toolbar "cold" images
	AFX_IMPORT_DATA static CMFCToolBarImages m_DisabledImages;      // Shared disabled images

	AFX_IMPORT_DATA static CMFCToolBarImages m_LargeImages;         // Shared toolbar large "hot" images
	AFX_IMPORT_DATA static CMFCToolBarImages m_LargeColdImages;     // Shared toolbar large "cold" images
	AFX_IMPORT_DATA static CMFCToolBarImages m_LargeDisabledImages; // Shared disabled large images

	AFX_IMPORT_DATA static CMFCToolBarImages m_MenuImages;
	AFX_IMPORT_DATA static CMFCToolBarImages m_DisabledMenuImages;

	AFX_IMPORT_DATA static BOOL m_bAutoGrayInactiveImages;
	AFX_IMPORT_DATA static int m_nGrayImagePercentage;

	AFX_IMPORT_DATA static CMFCToolBarImages* m_pUserImages;        // Shared user-defined images

	CMFCToolBarImages m_ImagesLocked;              // "Locked" toolbar images
	CMFCToolBarImages m_ColdImagesLocked;          // "Locked" toolbar "cold" images
	CMFCToolBarImages m_DisabledImagesLocked;      // "Locked" toolbar disabled images
	CMFCToolBarImages m_LargeImagesLocked;         // "Locked" toolbar large images
	CMFCToolBarImages m_LargeColdImagesLocked;     // "Locked" toolbar large "cold" images
	CMFCToolBarImages m_LargeDisabledImagesLocked; // "Locked" toolbar large disabled images
	CMFCToolBarImages m_MenuImagesLocked;          // "Locked" toolbar menu images
	CMFCToolBarImages m_DisabledMenuImagesLocked;  // "Locked" toolbar menu disabled images

	BOOL m_bLocked;
	BOOL m_bLargeIconsAreEnbaled;
	BOOL m_bMasked;
	BOOL m_bPermament;          // Can't be closed
	BOOL m_bTextLabels;         // Text labels below the image are available
	BOOL m_bDrawTextLabels;
	BOOL m_bResourceWasChanged; // Resource was changed since last session
	BOOL m_bLeaveFocus;         // Don't remove selection in the focused bars.
	BOOL m_bFloating;
	BOOL m_bNoDropTarget;
	BOOL m_bIsDragCopy;
	BOOL m_bStretchButton;
	BOOL m_bTracked;
	BOOL m_bMenuMode;
	BOOL m_bDisableControlsIfNoHandler;
	BOOL m_bRouteCommandsViaFrame;
	BOOL m_bDisableCustomize;
	BOOL m_bShowHotBorder;
	BOOL m_bGrayDisabledButtons;
	BOOL m_bIgnoreSetText;
	BOOL m_bQuickCustomize;
	BOOL m_bHasBrother;
	BOOL m_bElderBrother;
	BOOL m_bAllowReflections;
	BOOL m_bRoundShape;
	BOOL m_bInUpdateShadow;

	CMFCControlBarImpl m_Impl;

	int m_nMaxBtnHeight;  // Actual only if m_bTextLabels is TRUE
	int m_iButtonCapture; // index of button with capture (-1 => none)
	int m_iHighlighted;   // highlighted button index
	int m_iSelected;      // selected button index
	int m_iHot;
	int m_nTooltipsCount;
	int m_iDragIndex;
	int m_iImagesOffset;
	int m_nMaxLen;
	int m_iAccHotItem;

	AFX_IMPORT_DATA static CMap<UINT, UINT, int, int> m_DefaultImages;

	AFX_IMPORT_DATA static CSize m_sizeButton;      // original size of button
	AFX_IMPORT_DATA static CSize m_sizeImage;       // original size of glyph
	AFX_IMPORT_DATA static CSize m_sizeMenuButton;  // size of button on the menu
	AFX_IMPORT_DATA static CSize m_sizeMenuImage;   // size of image on the menu
	AFX_IMPORT_DATA static CSize m_sizeCurButton;   // size of button
	AFX_IMPORT_DATA static CSize m_sizeCurImage;    // size of glyph

	CSize m_sizeButtonLocked;    // original size of button of the locked toolbars
	CSize m_sizeImageLocked;     // original size of glyph of the locked toolbars
	CSize m_sizeCurButtonLocked; // size of button
	CSize m_sizeCurImageLocked;  // size of glyph

	BOOL m_bDontScaleLocked;     // don't scale locked images in high DPI mode
	CSize m_sizeLast;

	CObList m_Buttons;
	CObList m_OrigButtons;      // Original (not customized) items
	CObList m_OrigResetButtons; // Original (not customized) items after reset

	CMFCToolBarDropTarget	m_DropTarget;
	AFX_IMPORT_DATA static CMFCToolBarDropSource m_DropSource;

	CRect  m_rectDrag;
	CRect  m_rectTrack;
	CPen   m_penDrag;
	CPoint m_ptStartDrag;
	CPoint m_ptLastMouse;
	CWnd*  m_pWndLastCapture;

	CToolTipCtrl*     m_pToolTip;
	CMFCToolBarButton*   m_pDragButton;
	CMFCToolBar*   m_pBrotherToolBar;
	CMFCCustomizeButton* m_pCustomizeBtn;

	UINT m_uiOriginalResID; // Toolbar resource ID
	HWND m_hwndLastFocus;

	AFX_IMPORT_DATA static COLORREF m_clrTextHot;
	
	AFX_IMPORT_DATA static HHOOK m_hookMouseHelp; // Mouse hook for the help mode
	AFX_IMPORT_DATA static CMFCToolBar* m_pLastHookedToolbar;

	CMap<UINT, UINT&, CMFCToolBarButton*, CMFCToolBarButton*&> m_AccelKeys; // Keyboard acceleration keys

	AFX_IMPORT_DATA static BOOL m_bCustomizeMode;
	AFX_IMPORT_DATA static BOOL m_bAltCustomizeMode;
	AFX_IMPORT_DATA static BOOL m_bShowTooltips;
	AFX_IMPORT_DATA static BOOL m_bShowShortcutKeys;
	AFX_IMPORT_DATA static BOOL m_bLargeIcons;
	AFX_IMPORT_DATA static BOOL m_bAltCustomization;

	AFX_IMPORT_DATA static CList<UINT, UINT> m_lstUnpermittedCommands;
	AFX_IMPORT_DATA static CList<UINT, UINT> m_lstBasicCommands;

	AFX_IMPORT_DATA static CMFCCmdUsageCount m_UsageCount;

	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	virtual void DoPaint(CDC* pDC);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	virtual BOOL NotifyControlCommand(CMFCToolBarButton* pButton, BOOL bAccelerator, int nNotifyCode, WPARAM wParam, LPARAM lParam);

	virtual int FindDropIndex(const CPoint point, CRect& rectDrag) const;
	virtual void AdjustLocations();

	virtual BOOL OnSendCommand(const CMFCToolBarButton* /*pButton*/) { return FALSE; }

	virtual BOOL AllowSelectDisabled() const { return FALSE; }
	virtual BOOL AllowShowOnList() const { return TRUE; }
	virtual BOOL AlwaysSaveSelection() const { return FALSE; }

	virtual void DrawSeparator(CDC* pDC, const CRect& rect, BOOL bHorz);
	virtual CMFCToolBarButton* CreateDroppedButton(COleDataObject* pDataObject);
	virtual BOOL OnKey(UINT /*nChar*/) { return FALSE; }
	virtual void OnCustomizeMode(BOOL bSet);

	virtual BOOL EnableContextMenuItems(CMFCToolBarButton* pButton, CMenu* pPopup);
	virtual BOOL IsPureMenuButton(CMFCToolBarButton* /*pButton*/) const { return m_bMenuMode; }

	virtual void OnCalcSeparatorRect(CMFCToolBarButton* pButton, CRect& rectSeparator, BOOL bHorz);

	virtual void AddRemoveSeparator(const CMFCToolBarButton* pButton, const CPoint& ptStart, const CPoint& ptDrop);
	virtual void ShowCommandMessageString(UINT uiCmdId);

	static LRESULT CALLBACK ToolBarMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

	BOOL DropDownMenu(CMFCToolBarButton* pButton);

	virtual int CalcMaxButtonHeight();

	virtual BOOL AllowAltCustomization() const { return TRUE; }
	virtual void OnAfterButtonDelete();

	void SetRoundedRgn();
	void RedrawCustomizeButton();

	void UpdateImagesColor();

	// SmartUpdate methods:
	virtual void SaveOriginalState(CSettingsStore& reg);
	virtual BOOL LoadLastOriginalState(CSettingsStore& reg);
	virtual BOOL SmartUpdate(const CObList& lstPrevButtons);

	void SaveResetOriginalState(CSettingsStore& reg);
	BOOL LoadResetOriginalState(CSettingsStore& reg);

	BOOL RemoveResetStateButton(UINT uiCmdId);
	int  InsertResetStateButton(const CMFCToolBarButton& button, int iInsertAt);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnToolbarAppearance();
	afx_msg void OnToolbarDelete();
	afx_msg void OnToolbarImage();
	afx_msg void OnToolbarImageAndText();
	afx_msg void OnToolbarStartGroup();
	afx_msg void OnToolbarText();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnToolbarReset();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnCopyImage();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnToolbarNewMenu();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg LRESULT OnHelpHitTest(WPARAM,LPARAM);
	afx_msg LRESULT OnGetButtonCount(WPARAM,LPARAM);
	afx_msg LRESULT OnGetItemRect(WPARAM,LPARAM);
	afx_msg LRESULT OnGetButton(WPARAM,LPARAM);
	afx_msg LRESULT OnGetButtonText(WPARAM,LPARAM);
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg LRESULT OnPromptReset(WPARAM, LPARAM);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnUpdateToolTips(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
