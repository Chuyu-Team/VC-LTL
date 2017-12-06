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

class CBasePane;
class CMFCToolBar;
class CMFCMenuBar;
class CMFCPopupMenu;
class CMFCRibbonBar;
class CMFCRibbonStatusBar;
class CMFCCaptionButtonEx;

#include "afxcontrolbarutil.h"
#include "afxdockingmanager.h"
#include "afxfullscreenimpl.h"
#include "afxvisualmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CFrameImpl
{
	friend class CMFCToolBar;
	friend class CMDIFrameWndEx;
	friend class CFrameWndEx;
	friend class COleIPFrameWndEx;
	friend class COleDocIPFrameWndEx;
	friend class CMFCMenuBar;
	friend class CWinAppEx;
	friend class CMFCPopupMenu;
	friend class CPaneFrameWnd;
	friend class CFullScreenImpl;
	friend class CMDIChildWndEx;
	friend class CMDIClientAreaWnd;

public:
	CFrameImpl(CFrameWnd* pFrame);
	virtual ~CFrameImpl();

	void SetDockingManager(CDockingManager* pManager) 
	{
		ASSERT_VALID(pManager); 
		m_pDockManager = pManager;
	}

	void GetFullScreenMinMaxInfo(MINMAXINFO FAR* lpMMI) { m_FullScreenMgr.OnGetMinMaxInfo(lpMMI); }
	static const CList<CFrameWnd*, CFrameWnd*>& __stdcall GetFrameList() { return m_lstFrames; }

	static void __stdcall AddFrame(CFrameWnd* pFrame);
	static void __stdcall RemoveFrame(CFrameWnd* pFrame);

	void OnChangeVisualManager();

// Attributes:
public:
	const CMFCMenuBar* GetMenuBar() const { return m_pMenuBar; }
	CMFCMenuBar* GetMenuBar() { return m_pMenuBar; }
	CMFCRibbonBar* GetRibbonBar() { return m_pRibbonBar; }

	void SetNewUserToolBarRTC(CRuntimeClass* pCustomUserToolBarRTC);

	void SetControlbarsMenuId(UINT uiViewMenuEntryID, BOOL bViewMenuShowsToolbarsOnly)
	{
		m_uiControlbarsMenuEntryID = uiViewMenuEntryID;
		m_bViewMenuShowsToolbarsOnly = bViewMenuShowsToolbarsOnly;
	}

	BOOL HasRegion () const
	{
		return m_bIsWindowRgn;
	}

	AFX_IMPORT_DATA static BOOL m_bControlBarExtraPixel;

protected:
	UINT m_uiUserToolbarFirst;
	UINT m_uiUserToolbarLast;
	UINT m_nIDDefaultResource;
	UINT m_nHotSysButton;
	UINT m_nHitSysButton;
	UINT m_uiControlbarsMenuEntryID;

	BOOL m_bCaptured;
	BOOL m_bIsWindowRgn;
	BOOL m_bHasBorder;
	BOOL m_bIsMDIChildFrame;
	BOOL m_bIsOleInPlaceActive;
	BOOL m_bHadCaption;
	BOOL m_bLoadDockState;
	BOOL m_bViewMenuShowsToolbarsOnly;
	BOOL m_bWindowPosChanging;

	HMENU              m_hDefaultMenu;
	CRect              m_rectRedraw;
	CObList            m_listUserDefinedToolbars;
	CObList            m_listTearOffToolbars;
	CObList            m_lstCaptionSysButtons;
	CString            m_strControlBarRegEntry;
	CMFCMenuBar*          m_pMenuBar;
	CFrameWnd*         m_pFrame;
	CDockingManager*      m_pDockManager;
	CFullScreenImpl m_FullScreenMgr;
	CRuntimeClass*     m_pCustomUserToolBarRTC;
	CMFCRibbonBar*        m_pRibbonBar;
	CMFCRibbonStatusBar*  m_pRibbonStatusBar;


// Operations
protected:
	static UINT __stdcall GetFreeCtrlBarID(UINT uiFirstID, UINT uiLastID, const CObList& lstCtrlBars);

	// User-defined toolbars:
	void InitUserToolbars(LPCTSTR lpszRegEntry, UINT uiUserToolbarFirst, UINT uiUserToolbarLast);
	void LoadUserToolbars();
	void SaveUserToolbars(BOOL bFrameBarsOnly = FALSE);

	const CMFCToolBar* CreateNewToolBar(LPCTSTR lpszName);
	BOOL DeleteToolBar(CMFCToolBar* pToolBar);

	// Tear-off toolbars:
	void AddTearOffToolbar(CBasePane* pToolBar);
	void RemoveTearOffToolbar(CBasePane* pToolBar);

	void LoadTearOffMenus();
	void SaveTearOffMenus(BOOL bFrameBarsOnly = FALSE);

	void OnCloseFrame();
	void OnLoadFrame();
	void RestorePosition(CREATESTRUCT& cs);

	void StoreWindowPlacement();

	BOOL IsDockStateValid(const CDockState& state);
	BOOL IsUserDefinedToolbar(const CMFCToolBar* pToolBar) const;

	void SetMenuBar(CMFCMenuBar* pMenuBar);
	CMFCToolBar* GetUserToolBarByIndex(int iIndex) const;

	void SetupToolbarMenu(	CMenu& menu, const UINT uiViewUserToolbarCmdFirst, const UINT uiViewUserToolbarCmdLast);

	BOOL ProcessKeyboard(int nKey, BOOL* pbProcessAccel = NULL);
	BOOL ProcessMouseClick(UINT uiMsg, POINT pt, HWND hwnd);
	BOOL ProcessMouseMove(POINT pt);
	BOOL ProcessMouseWheel(WPARAM wParam, LPARAM lParam);

	void FrameEnableDocking(CFrameWnd * pFrame, DWORD dwDockStyle);

	BOOL OnShowPopupMenu(CMFCPopupMenu* pMenuPopup, CFrameWnd* pWndFrame);
	BOOL OnShowCustomizePane(CMFCPopupMenu* /*pMenuPane*/, UINT uiToolbarID);

	BOOL OnMenuChar(UINT nChar);

	void DeactivateMenu();

	void SetDockState(const CDockState& state);
	void SaveDockState(LPCTSTR lpszSectionName = NULL);
	void LoadDockState(LPCTSTR lpszSectionName = NULL);

	BOOL LoadLargeIconsState();

	static BOOL __stdcall IsHelpKey(LPMSG lpMsg);

	void ShowQuickCustomizePane(CMFCPopupMenu* pMenuPopup);
	void AddDefaultButtonsToCustomizePane(CMFCPopupMenu* pMenuPane, UINT uiToolbarID);
	BOOL IsCustomizePane(const CMFCPopupMenu* pMenuPopup) const;

	CRect GetFullScreeen() { return m_FullScreenMgr.GetFullScreenRect(); }
	BOOL IsFullScreeen() const { return m_FullScreenMgr.IsFullScreen(); }
	void EnableFullScreenMainMenu(BOOL bShow = TRUE) { m_FullScreenMgr.EnableMainMenu(bShow); }
	BOOL IsPrintPreview();
	BOOL IsOwnerDrawCaption() { return CMFCVisualManager::GetInstance()->IsOwnerDrawCaption() && !m_bIsOleInPlaceActive; }

	void OnSetText(LPCTSTR lpszText);
	void OnActivateApp(BOOL bActive);
	void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	BOOL OnNcPaint();
	void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	BOOL OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	BOOL OnNcActivate(BOOL bActive);
	void UpdateCaption();
	CRect GetCaptionRect();
	UINT OnNcHitTest(CPoint point);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	void OnLButtonDown(CPoint point);
	void OnLButtonUp(CPoint point);
	void OnMouseMove(CPoint point);

	CMFCCaptionButtonEx* GetSysButton(UINT nHit);

	void SetHighlightedSysButton(UINT nHitTest);
	void OnTrackCaptionButtons(CPoint point);
	void StopCaptionButtonsTracking();
	void RedrawCaptionButton(CMFCCaptionButtonEx* pBtn);
	void OnCompositionChanged();

	AFX_IMPORT_DATA static CList<CFrameWnd*, CFrameWnd*> m_lstFrames;
};

extern AFX_IMPORT_DATA UINT AFX_WM_POSTSETPREVIEWFRAME;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
