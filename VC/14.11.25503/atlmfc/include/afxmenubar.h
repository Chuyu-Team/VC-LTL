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

#include "afxtoolbar.h"
#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarMenuButton;
class CMFCToolBarSystemMenuButton;
class CMFCToolBarMenuButtonsButton;
class CMFCToolBarComboBoxButton;
class CMFCToolBarsMenuPropertyPage;

/*============================================================================*/
// CMFCMenuBar window

class CMFCMenuBar : public CMFCToolBar
{
	friend class CMFCToolBarsOptionsPropertyPage;
	friend class CMFCToolBar;
	friend class CMFCPopupMenuBar;
	friend class CMFCToolBarsMenuPropertyPage;

	DECLARE_SERIAL(CMFCMenuBar)

// Construction
public:
	CMFCMenuBar();

// Attributes
public:
	static BOOL __stdcall IsShowAllCommands() { return m_bShowAllCommands || !m_bRecentlyUsedMenus || !m_UsageCount.HasEnoughInformation(); }
	static BOOL __stdcall IsRecentlyUsedMenus() { return m_bRecentlyUsedMenus; }
	static BOOL __stdcall IsShowAllCommandsDelay() { return m_bShowAllMenusDelay; }

	static void __stdcall SetShowAllCommands(BOOL bShowAllCommands = TRUE);
	static void __stdcall SetRecentlyUsedMenus(BOOL bOn = TRUE);

	static BOOL __stdcall SetMenuFont(LPLOGFONT lpLogFont, BOOL bHorz = TRUE);
	static const CFont& __stdcall GetMenuFont(BOOL bHorz = TRUE);

	static void __stdcall EnableMenuShadows(BOOL bEnable = TRUE) { m_bMenuShadows = bEnable; }
	static BOOL __stdcall IsMenuShadows() { return m_bMenuShadows; }
	static void __stdcall HighlightDisabledItems(BOOL bHighlight = TRUE) { m_bHighlightDisabledItems = bHighlight; }
	static BOOL __stdcall IsHighlightDisabledItems() { return m_bHighlightDisabledItems ; }

	BOOL m_bAutoDocMenus;     // Automatically load/save  document menus
	BOOL m_bClearHashOnClose; // Clear temporary files on close

	BOOL GetForceDownArrows() { return m_bForceDownArrows; }
	void SetForceDownArrows(BOOL bValue) { m_bForceDownArrows = bValue; }

	// Support for MS Office-like help combobox:
	void EnableHelpCombobox(UINT uiID /* 0 - disable */, LPCTSTR lpszPrompt = NULL, int nComboBoxWidth = 150);
	CMFCToolBarComboBoxButton* GetHelpCombobox();

	void SetMenuButtonRTC(CRuntimeClass* pMenuButtonRTC);

protected:
	CWnd* m_pParentWnd;
	HMENU m_hMenu;
	HMENU m_hDefaultMenu;
	HMENU m_hSysMenu;
	HICON m_hSysIcon;
	UINT  m_uiDefMenuResId;
	UINT  m_nHelpComboID;
	int   m_nSystemButtonsNum;
	int   m_nSystemButtonsNumSaved;
	int   m_nHelpComboWidth;
	BOOL  m_bMaximizeMode;
	BOOL  m_bHaveButtons;
	BOOL  m_bForceDownArrows;

	CSize   m_szSystemButton;	// System button size
	CString m_strHelpComboPrompt;

	CMFCToolBarsMenuPropertyPage*     m_pMenuPage;
	CRuntimeClass* m_pMenuButtonRTC;

	AFX_IMPORT_DATA static BOOL m_bShowAllCommands;
	AFX_IMPORT_DATA static BOOL m_bRecentlyUsedMenus;
	AFX_IMPORT_DATA static BOOL m_bShowAllMenusDelay;
	AFX_IMPORT_DATA static BOOL m_bMenuShadows;
	AFX_IMPORT_DATA static BOOL m_bHighlightDisabledItems;

// Operations
public:
	virtual void CreateFromMenu(HMENU hMenu, BOOL bDefaultMenu = FALSE, BOOL bForceUpdate = FALSE);
	void SetMaximizeMode(BOOL bMax, CWnd* pWnd = NULL, BOOL bRecalcLayout = TRUE);

	void SetDefaultMenuResId(UINT uiResId);
	UINT GetDefaultMenuResId() const { return m_uiDefMenuResId; }
	HMENU GetHMenu() const { return m_hMenu; }
	HMENU GetDefaultMenu() const { return m_hDefaultMenu; }

	CMFCToolBarButton* GetMenuItem(int iItem) const;
	CMFCToolBarSystemMenuButton* GetSystemMenu() const;
	CMFCToolBarMenuButtonsButton* GetSystemButton(UINT uiBtn, BOOL bByCommand = TRUE) const;
	int GetSystemButtonsCount() const { return m_nSystemButtonsNum; }

	int GetFloatPopupDirection(CMFCToolBarMenuButton* pButton);

protected:
	void RestoreMaximizeMode(BOOL bRecalcLayout = TRUE);

	static BOOL __stdcall FindMenuItemText(HMENU hMenu, const UINT nID, CString& strText);

	void CalcSysButtonSize();
	virtual BOOL BuildOrigItems(UINT uiMenuResID); // Required for the SmartResourceUpdate

// Overrides
public:
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE, UINT nID = AFX_IDW_MENUBAR);
	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE,
		CRect rcBorders = CRect(1, 1, 1, 1), UINT nID = AFX_IDW_MENUBAR);

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcLayout(DWORD dwMode, int nLength = -1);

	virtual int CalcMaxButtonHeight();

	virtual void AdjustLocations();
	virtual BOOL OnSendCommand(const CMFCToolBarButton* pButton);

	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual BOOL CanBeRestored() const { return TRUE; }
	virtual BOOL CanBeClosed() const { return FALSE; }
	virtual BOOL AllowChangeTextLabels() const { return FALSE; }
	virtual BOOL IsButtonExtraSizeAvailable() const { return FALSE; }
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }

	virtual void OnDefaultMenuLoaded(HMENU) {};
	virtual BOOL RestoreOriginalState();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual int GetRowHeight() const;

	virtual int GetColumnWidth() const
	{
		return m_bHaveButtons ? GetButtonSize().cx : m_sizeMenuButton.cx <= 0 ? m_sizeButton.cx - 2 : m_sizeMenuButton.cx - 2;
	}

	virtual BOOL OnSetDefaultButtonText(CMFCToolBarButton* pButton);
	virtual void OnChangeHot(int iHot);
	
	virtual int GetAvailableExpandSize() const { return m_bExclusiveRow ? 0xFFFF /*unlimited size*/ : CMFCToolBar::GetAvailableExpandSize(); }

protected:
	virtual int FindDropIndex(const CPoint point, CRect& rectDrag) const;
	virtual void ResetImages();

	virtual BOOL IsPureMenuButton(CMFCToolBarButton* pButton) const;
	static void __stdcall UpdateMDIChildrenMenus(CMultiDocTemplate* pTemplate);

	// Implementation
public:
	virtual ~CMFCMenuBar();

	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
