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
#include "afxpopupmenubar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarMenuButton;
class CMFCMenuBar;
class CMFCToolBarsMenuPropertyPage;
class CMFCRibbonBaseElement;
class CMFCShadowWnd;

/*============================================================================*/
// CMFCPopupMenu frame

class CMFCPopupMenu : public CMiniFrameWnd
{
	friend class CMFCCustomizeButton;
	friend class CMFCToolBarMenuButton;
	friend class CMFCToolBarsMenuPropertyPage;
	friend class CFrameWndEx;
	friend class CMDIFrameWndEx;
	friend class COleIPFrameWndEx;
	friend class COleDocIPFrameWndEx;
	friend class CFrameImpl;
	friend class CDialogImpl;
	friend class CContextMenuManager;
	friend class CMFCRibbonBaseElement;
	friend class CMFCPopupMenuBar;
	friend class CMFCRibbonBar;
	friend class CMFCRibbonMiniToolBar;
	friend class CMFCRibbonPanelMenuBar;

	DECLARE_SERIAL(CMFCPopupMenu)

public:
	CMFCPopupMenu();
	virtual ~CMFCPopupMenu();

protected:
	CMFCPopupMenu(CMFCToolBarsMenuPropertyPage* pCustPage, LPCTSTR lpszTitle); // For customization only

// Attributes
public:
	enum ANIMATION_TYPE
	{
		NO_ANIMATION,
		UNFOLD,
		SLIDE,
		FADE,
		SYSTEM_DEFAULT_ANIMATION = 999,
	};

	enum MENUAREA_TYPE
	{
		MENU,
		SHADOW_RIGHT,
		SHADOW_BOTTOM,
		LOGO,
		OUTSIDE,
		TEAROFF_CAPTION
	};

	enum DROP_DIRECTION
	{
		DROP_DIRECTION_NONE,
		DROP_DIRECTION_BOTTOM,
		DROP_DIRECTION_TOP,
		DROP_DIRECTION_RIGHT,
		DROP_DIRECTION_LEFT,
	};

	enum LOGO_LOCATION
	{
		MENU_LOGO_LEFT,
		MENU_LOGO_RIGHT,
		MENU_LOGO_TOP,
		MENU_LOGO_BOTTOM,
	};

	enum QUICK_CUSTOMIZE_TYPE
	{
		QUICK_CUSTOMIZE_PANE,
		QUICK_CUSTOMIZE_ADDREMOVE,
		QUICK_CUSTOMIZE_NONE,
	};

	enum MENU_RESIZE_TYPE
	{
		MENU_RESIZE_BOTTOM,
		MENU_RESIZE_BOTTOM_RIGHT,
		MENU_RESIZE_TOP,
		MENU_RESIZE_TOP_RIGHT,
	};

	BOOL m_bAutoDestroyParent;

	HMENU GetHMenu() const { return m_hMenu; }
	virtual CMFCPopupMenuBar* GetMenuBar() { return &m_wndMenuBar; }

	void SetAutoDestroy(BOOL bAutoDestroy = TRUE) { m_bAutoDestroy = bAutoDestroy; }

	CMFCToolBarMenuButton* GetParentButton() const { return m_pParentBtn; }
	CMFCRibbonBaseElement* GetParentRibbonElement() const { return m_pParentRibbonElement; }

	static void __stdcall SetForceShadow(BOOL bValue) { m_bForceShadow = bValue; }
	static BOOL __stdcall GetForceShadow() { return m_bForceShadow; }

	static void __stdcall SetForceMenuFocus(BOOL bValue) { m_bForceMenuFocus = bValue; }
	static BOOL __stdcall GetForceMenuFocus() { return m_bForceMenuFocus; }

	static CMFCPopupMenu::ANIMATION_TYPE __stdcall GetAnimationType(BOOL bNoSystem = FALSE)
	{
		if (m_AnimationType == SYSTEM_DEFAULT_ANIMATION && !bNoSystem)
		{
			if (GetGlobalData()->m_bMenuAnimation)
			{
				return GetGlobalData()->m_bMenuFadeEffect ? FADE : SLIDE; 
			}
			else
			{
				return CMFCPopupMenu::NO_ANIMATION;
			}
		}

		return m_AnimationType;
	}

	static void __stdcall SetAnimationType(CMFCPopupMenu::ANIMATION_TYPE type) { m_AnimationType = type; }
	static void __stdcall SetAnimationSpeed(UINT nElapse);
	static UINT __stdcall GetAnimationSpeed() { return m_AnimationSpeed; }
	static void __stdcall EnableMenuSound(BOOL bEnable = TRUE) { m_bMenuSound = bEnable; }
	static UINT __stdcall IsMenuSound() { return m_bMenuSound; }

	static void __stdcall AlwaysShowEmptyToolsEntry(BOOL bShow = TRUE) { m_bAlwaysShowEmptyToolsEntry = bShow; }
	static BOOL __stdcall IsAlwaysShowEmptyToolsEntry() { return m_bAlwaysShowEmptyToolsEntry; }

	static void __stdcall SetSendMenuSelectMsg(BOOL bSet = TRUE) { m_bSendMenuSelectMsg = bSet; }
	static BOOL __stdcall IsSendMenuSelectMsg() { return m_bSendMenuSelectMsg; }

	void EnableResize(CSize sizeMinResize);
	void EnableVertResize(int nMinResize);

	BOOL IsResizeble() const { return m_bIsResizable; }
	BOOL IsScrollable() const { return m_bScrollable; }
	void EnableScrolling(BOOL /*bEnable*/ = TRUE) { /* This method is obsolete. */ }

	void EnableMenuLogo(int iLogoSize, LOGO_LOCATION nLogoLocation = MENU_LOGO_LEFT);
	void SetMaxWidth(int iMaxWidth);

	CWnd* GetMessageWnd() const { return m_pMessageWnd; }
	void SetMessageWnd(CWnd* pMsgWnd) { m_pMessageWnd = pMsgWnd; }

	BOOL PostCommand(UINT uiCommandID);

	void MoveTo(const CPoint& pt)
	{
		m_ptLocation = pt;
		RecalcLayout();
	}

	virtual BOOL InCommand()
	{
		CMFCPopupMenuBar* pBar = GetMenuBar();
		return pBar == NULL ? FALSE : pBar->m_bInCommand;
	}

	static CMFCPopupMenu* __stdcall GetActiveMenu() { return m_pActivePopupMenu; }
	static CMFCPopupMenu* __stdcall GetSafeActivePopupMenu();

	DROP_DIRECTION GetDropDirection() const { return m_DropDirection; }

	void SetRightAlign(BOOL bRightAlign = TRUE) { m_bRightAlign = bRightAlign; }
	BOOL IsRightAlign() const { return m_bRightAlign; }

	BOOL IsQuickCustomize() { return m_bQuickCusomize; }
	void SetQuickCustomizeType(QUICK_CUSTOMIZE_TYPE Type) { m_QuickType = Type; }
	QUICK_CUSTOMIZE_TYPE GetQuickCustomizeType() const { return m_QuickType; }
	BOOL IsCustomizePane() { return m_QuickType == QUICK_CUSTOMIZE_PANE; }

	BOOL IsShown() const { return m_bShown; }
	virtual BOOL IsIdle() const { return m_bAnimationIsDone && m_iScrollMode == 0; }

	virtual CWnd* GetParentArea(CRect& rectParentBtn);
	void SetParentRibbonElement(CMFCRibbonBaseElement* pElem);

	virtual BOOL IsAlwaysClose() const { return FALSE; }

	BOOL HasBeenResized() const { return m_bHasBeenResized; }
	virtual BOOL DefaultMouseClickOnClose() const { return TRUE; }

protected:
	AFX_IMPORT_DATA static CMFCPopupMenu* m_pActivePopupMenu;
	AFX_IMPORT_DATA static BOOL        m_bForceMenuFocus;  // Menu takes a focus when activated

	CWnd*   m_pMessageWnd;
	CPoint  m_ptLocation;
	CPoint  m_ptLocationInitial;
	CSize   m_FinalSize;
	CString m_strCaption;

	CMFCToolBarsMenuPropertyPage* const    m_pMenuCustomizationPage;
	CMFCToolBarMenuButton* m_pParentBtn;
	CMFCPopupMenuBar       m_wndMenuBar;
	DROP_DIRECTION      m_DropDirection;
	HMENU               m_hMenu;

	BOOL m_bAutoDestroy;
	BOOL m_bTrackMode;
	BOOL m_bHasBeenResized;
	BOOL m_bRightAlign;
	BOOL m_bShown;
	BOOL m_bTobeDstroyed;

	int m_nMenuBarHeight;
	int m_iMaxWidth;
	int m_iLogoWidth;
	int m_nLastCommandID;

	LOGO_LOCATION m_nLogoLocation;

	AFX_IMPORT_DATA static int m_nMinWidth;
	AFX_IMPORT_DATA static BOOL m_bMenuSound;
	AFX_IMPORT_DATA static BOOL m_bAlwaysShowEmptyToolsEntry;
	AFX_IMPORT_DATA static BOOL m_bSendMenuSelectMsg;

	//----------------------
	// Animation attributes:
	//----------------------
	AFX_IMPORT_DATA static ANIMATION_TYPE m_AnimationType;
	AFX_IMPORT_DATA static UINT           m_AnimationSpeed;

	CSize m_AnimSize;
	BOOL  m_bIsAnimRight;
	BOOL  m_bIsAnimDown;
	BOOL  m_bAnimationIsDone;
	BOOL  m_bDisableAnimation;

	//----------------------
	// Scrolling attributes:
	//----------------------
	BOOL m_bScrollable;
	BOOL m_bShowScrollBar;

	CRect m_rectScrollUp;
	CRect m_rectScrollDn;

	int m_iScrollMode;  // -1 - Up, 0 - None, 1 - Down
	int m_iScrollBtnHeight;
	int m_nMaxHeight;
	int m_iFadePercent;

	CScrollBar m_wndScrollBarVert;
	CBitmap    m_bmpScreenSrc;
	CBitmap    m_bmpScreenDst;
	CBitmap    m_bmpScreenTmp;

	COLORREF* m_cFadeSrcBits;
	COLORREF* m_cFadeDstBits;
	COLORREF* m_cFadeTmpBits;

	//-------------------
	// Shadow attributes:
	//-------------------
	int				m_iShadowSize;
	CBitmap			m_bmpShadowRight;				// Saved shadow butmaps
	CBitmap			m_bmpShadowBottom;
	CMFCShadowWnd*	m_pWndShadow;					// layered shadow


	AFX_IMPORT_DATA static BOOL m_bForceShadow;     // when TRUE paints shadows even outside main-frame

	//---------------------
	// Tear-off attributes:
	//---------------------
	BOOL         m_bTearOffTracking;
	BOOL         m_bIsTearOffCaptionActive;
	CRect        m_rectTearOffCaption;
	CToolTipCtrl m_wndToolTip;

	//-------------------
	// Resize attributes:
	//-------------------
	BOOL  m_bResizeTracking;
	BOOL  m_bWasResized;
	BOOL  m_bIsResizeBarOnTop;
	BOOL  m_bIsResizable;
	CSize m_sizeMinResize;
	CSize m_sizeCurrent;
	CRect m_rectResize;

	//------------------------
	// Quick Cusomization flags
	//------------------------
	BOOL  m_bQuickCusomize;
	BOOL  m_bEscClose;
	QUICK_CUSTOMIZE_TYPE m_QuickType;

	//-------------------
	// Ribbon attributes:
	//-------------------
	CMFCRibbonBaseElement* m_pParentRibbonElement;
	HWND                m_hwndConnectedFloaty;

// Operations
public:
	virtual void SaveState();

	CMFCPopupMenu* GetParentPopupMenu() const;
	CMFCToolBar* GetParentToolBar() const;

	CMFCToolBarMenuButton* GetSelItem();

	int InsertItem(const CMFCToolBarMenuButton& button, int iInsertAt = -1);
	int InsertSeparator(int iInsertAt = -1);

	int GetMenuItemCount() const;
	CMFCToolBarMenuButton* GetMenuItem(int iIndex) const;
	CMFCToolBarMenuButton* FindSubItemByCommand(UINT uiCmd) const;
	
	BOOL RemoveItem(int iIndex);
	void RemoveAllItems();

	void SetDefaultItem(UINT uiCmd) { GetMenuBar()->m_uiDefaultMenuCmdId = uiCmd; }

	static BOOL __stdcall ActivatePopupMenu(CFrameWnd* pTopFrame, CMFCPopupMenu* pPopupMenu);
	void CloseMenu(BOOL bSetFocusToBar = FALSE);
	void ShowAllCommands();

	BOOL HideRarelyUsedCommands() const;
	BOOL AreAllCommandsShown() const { return ((CMFCPopupMenu*) this)->GetMenuBar()->m_bAreAllCommandsShown; }

	MENUAREA_TYPE CheckArea(const CPoint& ptScreen) const;

	void UpdateShadow(LPRECT lprectScreen = NULL);
	static void __stdcall UpdateAllShadows(LPRECT lprectScreen = NULL); // Repaint all opened menu shadows

	void SetQuickMode();

	BOOL IsEscClose() { return m_bEscClose; }

	void TriggerResize();

protected:
	void Initialize();

	virtual BOOL IsScrollUpAvailable();
	virtual BOOL IsScrollDnAvailable();

	void CollapseSubmenus();
	void UpdateBottomWindows(BOOL bCheckOnly = FALSE);

	virtual void DoPaint(CDC* pPaintDC);
	virtual void DrawFade(CDC* pPaintDC);

	BOOL AdjustScroll(BOOL bForceMenuBarResize = FALSE);
	void SetScrollBar();

	virtual BOOL InitMenuBar();
	virtual BOOL TearOff(CPoint point);
	virtual CPane* CreateTearOffBar(CFrameWnd* pWndMain, UINT uiID, LPCTSTR lpszName);
	virtual void OnChooseItem(UINT /*uidCmdID*/) {}
	virtual void OnChangeHot(int /*nHot*/) {}

	BOOL NotifyParentDlg(BOOL bActivate);

	virtual BOOL IsRibbonMiniToolBar() const { return FALSE; }

	BOOL StartResize();

	virtual int GetBorderSize() const;

	static CMFCPopupMenu* __stdcall FindMenuWithConnectedFloaty();

// Overrides
public:
	virtual BOOL Create(CWnd* pWndParent, int x, int y, HMENU hMenu, BOOL bLocked = FALSE, BOOL bOwnMessage=FALSE); // Add: Alex Corazzin (2)
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// Implementation
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);

	DECLARE_MESSAGE_MAP()

	virtual HRESULT get_accName(VARIANT varChild, BSTR *pszName);
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);
};

class CMFCDisableMenuAnimation
{
public:
	CMFCDisableMenuAnimation ()
	{
		m_animType = CMFCPopupMenu::GetAnimationType ();
		CMFCPopupMenu::SetAnimationType (CMFCPopupMenu::NO_ANIMATION);
	}

	virtual ~CMFCDisableMenuAnimation ()
	{
		Restore ();
	}

	void Restore ()
	{
		CMFCPopupMenu::SetAnimationType (m_animType);
	}

protected:
	CMFCPopupMenu::ANIMATION_TYPE m_animType;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
