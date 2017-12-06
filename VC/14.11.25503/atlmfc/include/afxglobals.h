// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//
#pragma once

#include "afxwin.h"
#include "afxcontrolbarutil.h"

#include "afxaccessibility.h"
#include <oleacc.h>

#if (NTDDI_VERSION >= NTDDI_WIN7)
#include <ShObjIdl.h>
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// Auxiliary System/Screen metrics

enum AFX_DOCK_TYPE
{
	DT_UNDEFINED = 0,    // inherit from application
	DT_IMMEDIATE = 1,    // control bar torn off immediately and follows the mouse
	DT_STANDARD  = 2,    // user drags a frame
	DT_SMART     = 0x80  // smart docking style
};

// autohide sliding modes
static const UINT AFX_AHSM_MOVE    = 1;
static const UINT AFX_AHSM_STRETCH = 2;

#define AFX_AUTOHIDE_LEFT   0x0001
#define AFX_AUTOHIDE_RIGHT  0x0002
#define AFX_AUTOHIDE_TOP    0x0004
#define AFX_AUTOHIDE_BOTTOM 0x0008

typedef HANDLE AFX_HPAINTBUFFER;  // handle to a buffered paint context

typedef struct _AFX_MARGINS {
	int cxLeftWidth;
	int cxRightWidth;
	int cyTopHeight;
	int cyBottomHeight;
} AFX_MARGINS;

class CMFCToolBarImages;

struct AFX_GLOBAL_DATA
{
	friend class CMemDC;

	BOOL m_bInitialized;	// Has the singleton afxGlobalData structure been initialized?
	BOOL m_bUseSystemFont;	// Use system font for menu/toolbar/ribbons
	BOOL m_bDontReduceFontHeight; // Do not reduce font height in UpdateFonts
	BOOL m_bInSettingChange;

	// solid brushes with convenient gray colors and system colors
	HBRUSH hbrBtnHilite, hbrBtnShadow;
	HBRUSH hbrWindow;

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
	COLORREF clrBtnText, clrWindowFrame;
	COLORREF clrBtnDkShadow, clrBtnLight;
	COLORREF clrGrayedText;
	COLORREF clrHilite;
	COLORREF clrTextHilite;
	COLORREF clrHotLinkNormalText;
	COLORREF clrHotLinkHoveredText;
	COLORREF clrHotLinkVisitedText;

	COLORREF clrBarWindow;
	COLORREF clrBarFace;
	COLORREF clrBarShadow, clrBarHilite;
	COLORREF clrBarDkShadow, clrBarLight;
	COLORREF clrBarText;

	COLORREF clrWindow;
	COLORREF clrWindowText;

	COLORREF clrCaptionText;
	COLORREF clrMenuText;
	COLORREF clrActiveCaption;
	COLORREF clrInactiveCaption;
	COLORREF clrInactiveCaptionText;
	///<summary>
	/// Specifies gradient color of active caption. Generally used for docking panes. </summary>
	COLORREF clrActiveCaptionGradient;
	///<summary>
	/// Specifies gradient color of inactive active caption. Generally used for docking panes. </summary>
	COLORREF clrInactiveCaptionGradient;

	COLORREF clrActiveBorder;
	COLORREF clrInactiveBorder;

	CBrush brBtnFace;
	CBrush brHilite;
	CBrush brLight;
	CBrush brBlack;
	CBrush brActiveCaption;
	CBrush brInactiveCaption;
	CBrush brWindow;

	CBrush brBarFace;

	CPen penHilite;
	CPen penBarFace;
	CPen penBarShadow;

	// Library cursors:
	HCURSOR m_hcurStretch;
	HCURSOR m_hcurStretchVert;
	HCURSOR m_hcurHand;
	HCURSOR m_hcurSizeAll;
	HCURSOR m_hcurMoveTab;
	HCURSOR m_hcurNoMoveTab;

	HCURSOR GetHandCursor();

	HICON m_hiconTool;
	HICON m_hiconLink;
	HICON m_hiconColors;

	// Shell icon sizes:
	CSize m_sizeSmallIcon;

	// Toolbar and menu fonts:
	CFont fontRegular;
	CFont fontTooltip;
	CFont fontBold;
	CFont fontDefaultGUIBold;
	CFont fontUnderline;
	CFont fontDefaultGUIUnderline;
	CFont fontVert;
	CFont fontVertCaption;
	CFont fontSmall;
	CFont fontMarlett;	// Standard Windows menu symbols
	CRect m_rectVirtual;

	///<summary>
	/// Indicates whether the application is being executed under Windows 7 OS or higher</summary>
	BOOL  bIsWindows7;
	BOOL  bIsRemoteSession;

	BOOL  m_bIsBlackHighContrast;
	BOOL  m_bIsWhiteHighContrast;
	BOOL  m_bUseBuiltIn32BitIcons;
	BOOL  m_bMenuAnimation;
	BOOL  m_bMenuFadeEffect;
	BOOL  m_bIsRTL;
	BOOL  m_bEnableAccessibility;

	BOOL m_bUnderlineKeyboardShortcuts;
	BOOL m_bSysUnderlineKeyboardShortcuts;

	BOOL m_bRefreshAutohideBars;

	int   m_nBitsPerPixel;
	int   m_nDragFrameThicknessFloat;
	int   m_nDragFrameThicknessDock;
	int   m_nAutoHideToolBarSpacing;
	int   m_nAutoHideToolBarMargin;
	int   m_nCoveredMainWndClientAreaPercent;
	int   m_nMaxToolTipWidth;
	int   m_nShellAutohideBars;

// Implementation
	AFX_GLOBAL_DATA();
	~AFX_GLOBAL_DATA();

	void Initialize();
	void UpdateSysColors();
	void UpdateFonts();
	void OnSettingChange();

	BOOL SetMenuFont(LPLOGFONT lpLogFont, BOOL bHorz);

	int GetTextHeight(BOOL bHorz = TRUE)
	{
		return bHorz ? m_nTextHeightHorz : m_nTextHeightVert;
	}

	int GetTextWidth(BOOL bHorz = TRUE)
	{
		return bHorz ? m_nTextWidthHorz : m_nTextWidthVert;
	}

	double GetRibbonImageScale()
	{
		return m_bIsRibbonImageScale ? m_dblRibbonImageScale : 1.;
	}

	void EnableRibbonImageScale(BOOL bEnable = TRUE)
	{
		m_bIsRibbonImageScale = bEnable;
	}

	BOOL IsRibbonImageScaleEnabled()
	{
		return m_bIsRibbonImageScale;
	}

	BOOL DrawParentBackground(CWnd* pWnd, CDC* pDC, LPRECT lpRect = NULL);
	void CleanUp();

	COLORREF GetColor(int nColor);

	BOOL SetLayeredAttrib(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

	BOOL Is32BitIcons() const
	{
		return m_bUseBuiltIn32BitIcons && m_nBitsPerPixel >= 16 && !m_bIsBlackHighContrast && !m_bIsWhiteHighContrast;
	}

	BOOL IsHighContrastMode() const
	{
		return m_bIsWhiteHighContrast || m_bIsBlackHighContrast;
	}

	BOOL IsAccessibilitySupport() const
	{
		return m_bEnableAccessibility;
	}

	/// <summary>
	/// Determines positions of Shell auto hide bars.</summary>
	/// <returns> An integer value with encoded flags that specify positions of auto hide bars.
	/// It may combine the following values: AFX_AUTOHIDE_BOTTOM, AFX_AUTOHIDE_TOP, AFX_AUTOHIDE_LEFT,
	/// AFX_AUTOHIDE_RIGHT.</returns>
	int GetShellAutohideBars()
	{
		if (m_bRefreshAutohideBars)
		{
			m_bRefreshAutohideBars = FALSE;

			APPBARDATA abd;
			ZeroMemory(&abd, sizeof(APPBARDATA));
			abd.cbSize = sizeof(APPBARDATA);

			abd.uEdge = ABE_BOTTOM;
			if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
			{
				m_nShellAutohideBars |= AFX_AUTOHIDE_BOTTOM;
			}

			abd.uEdge = ABE_TOP;
			if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
			{
				m_nShellAutohideBars |= AFX_AUTOHIDE_TOP;
			}

			abd.uEdge = ABE_LEFT;
			if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
			{
				m_nShellAutohideBars |= AFX_AUTOHIDE_LEFT;
			}

			abd.uEdge = ABE_RIGHT;
			if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
			{
				m_nShellAutohideBars |= AFX_AUTOHIDE_RIGHT;
			}
		}

		return m_nShellAutohideBars;
	}

	void EnableAccessibilitySupport(BOOL bEnable = TRUE);

	CString RegisterWindowClass(LPCTSTR lpszClassNamePrefix);
	BOOL ExcludeTag(CString& strBuffer, LPCTSTR lpszTag, CString& strTag, BOOL bIsCharsList = FALSE);

	BOOL IsDwmCompositionEnabled();

	BOOL DrawTextOnGlass(HTHEME hTheme, CDC* pDC, int iPartId, int iStateId, CString strText,
		CRect rect, DWORD dwFlags, int nGlowSize = 0, COLORREF clrText = (COLORREF)-1);

	BOOL Resume();
	BOOL GetNonClientMetrics (NONCLIENTMETRICS& info);

	/// <summary>
	/// Creates and initializes a Shell item object from a parsing name.</summary>
	/// <param name="pszPath">[in] A pointer to a display name.</param>
	/// <param name="pbc">A pointer to a bind context that controls the parsing operation.</param>
	/// <param name="riid">A reference to an interface ID.</param>
	/// <param name="ppv">[out] When this function returns, contains the interface pointer requested in riid. This will typically be IShellItem or IShellItem2.</param>
	/// <returns>Returns S_OK if successful, or an error value otherwise. </returns>
	HRESULT ShellCreateItemFromParsingName(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv);

#if (NTDDI_VERSION >= NTDDI_WIN7)
	/// <summary>
	/// Creates and stores in the global data a pointer to ITaskBarList interface.</summary>
	/// <returns>A pointer to ITaskbarList interface if creation of a task bar list object succeeds, or NULL if creation fails or current
	/// Operation System is less than Windows 7.</returns>
	ITaskbarList* GetITaskbarList();

	/// <summary>
	/// Creates and stores in the global data a pointer to ITaskBarList3 interface.</summary>
	/// <returns>A pointer to ITaskbarList3 interface if creation creation of a task bar list object succeeds, or NULL if creation fails or current
	/// Operation System is less than Windows 7.</returns>
	ITaskbarList3* GetITaskbarList3();
#endif
	void ResetCheckCompositionFlag();

protected:

	void UpdateTextMetrics();
	HBITMAP CreateDitherBitmap(HDC hDC);

	int m_nTextHeightHorz;
	int m_nTextHeightVert;
	int m_nTextWidthHorz;
	int m_nTextWidthVert;

	double m_dblRibbonImageScale;
	BOOL   m_bIsRibbonImageScale;
	BOOL   m_bBufferedPaintInited;

	HINSTANCE m_hinstDwmapiDLL;
};

// Do not reference this structure directly inside of MFC.  Always use the accessor method
// GetGlobalData in order to ensure that the structure has been properly initialized.
// Initialization is not done during the construction of the afxGlobalData object because it requires
// calling a number of GDI32/USER32 APIs, which violates the best practices for DLLMain.
AFX_IMPORT_DATA extern AFX_GLOBAL_DATA afxGlobalData;

inline AFX_GLOBAL_DATA *GetGlobalData()
{
	if (!afxGlobalData.m_bInitialized)
	{
		afxGlobalData.Initialize();
		afxGlobalData.m_bInitialized = TRUE;
	}

	return &afxGlobalData;
}

#define AFX_IMAGE_MARGIN 4

// MFC Control bar compatibility
#define AFX_CX_BORDER   1
#define AFX_CY_BORDER   1

#define AFX_CX_GRIPPER  3
#define AFX_CY_GRIPPER  3
#define AFX_CX_BORDER_GRIPPER 2
#define AFX_CY_BORDER_GRIPPER 2

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
