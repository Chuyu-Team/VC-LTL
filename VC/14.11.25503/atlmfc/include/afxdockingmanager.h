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
#include "afxpane.h"
#include "afxsmartdockingmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CDockSite;
class CAutoHideDockSite;
class CMFCAutoHideBar;
class CPaneFrameWnd;
class CMultiPaneFrameWnd;
class CTabbedPane;
class CPaneDivider;
class CDockablePane;
class COleCntrFrameWndEx;

struct AFX_DOCKSITE_INFO
{
	DWORD          m_dwBarAlignment;
	CRuntimeClass* pDockBarRTC;
};

struct AFX_AUTOHIDE_DOCKSITE_SAVE_INFO
{
	AFX_AUTOHIDE_DOCKSITE_SAVE_INFO()
	{
		m_pSavedBar = NULL;
		m_rectBar.SetRectEmpty();
		m_nBarID = (UINT)-1;
		m_bIsVisible = FALSE;
		m_dwBarAlignment = 0;
		m_bFirstInGroup = TRUE;
		m_bLastInGroup = TRUE;
		m_bActiveInGroup = TRUE;
	}

	AFX_AUTOHIDE_DOCKSITE_SAVE_INFO(AFX_AUTOHIDE_DOCKSITE_SAVE_INFO& src)
	{
		Copy(src);
	}

	UINT  m_nBarID;
	DWORD m_dwBarAlignment;
	BOOL  m_bIsVisible;
	CRect m_rectBar;
	CList<UINT, UINT&> m_lstSiblingBars;

	BOOL m_bFirstInGroup;
	BOOL m_bLastInGroup;
	BOOL m_bActiveInGroup;

	void Serialize(CArchive& ar);
	AFX_AUTOHIDE_DOCKSITE_SAVE_INFO& operator= (AFX_AUTOHIDE_DOCKSITE_SAVE_INFO& src) { return Copy(src); }

	AFX_AUTOHIDE_DOCKSITE_SAVE_INFO& Copy(AFX_AUTOHIDE_DOCKSITE_SAVE_INFO& src)
	{
		m_lstSiblingBars.RemoveAll();
		m_nBarID = src.m_nBarID;
		m_dwBarAlignment = src.m_dwBarAlignment;
		m_bIsVisible = src.m_bIsVisible;
		m_rectBar = src.m_rectBar;
		m_lstSiblingBars.AddTail(&src.m_lstSiblingBars);
		m_bFirstInGroup = src.m_bFirstInGroup;
		m_bLastInGroup = src.m_bLastInGroup;
		m_bActiveInGroup = src.m_bActiveInGroup;
		return *this;
	}

	CDockablePane* m_pSavedBar; // must be set before serialize;
};

#define AFX_SD_MARKERS_NUM 5

class CSmartDockingInfo : public CObject
{
public:
	CSmartDockingInfo();

	void CopyTo(CSmartDockingInfo& params);

	CSize       m_sizeTotal;    // Smart docking group bounds dimension
	int         m_nCentralGroupOffset;
	COLORREF    m_clrTransparent;
	COLORREF    m_clrToneSrc;
	COLORREF    m_clrToneDest;
	COLORREF    m_clrBaseBackground;
	COLORREF    m_clrBaseBorder;
	UINT        m_uiMarkerBmpResID [AFX_SD_MARKERS_NUM];
	UINT        m_uiMarkerLightBmpResID [AFX_SD_MARKERS_NUM];
	UINT		m_uiBaseBmpResID;
	BOOL        m_bUseThemeColorInShading;
	BOOL		m_bIsAlphaMarkers;
};

class CDockingManager : public CObject
{
	friend class CMDIChildWndEx;
	friend class COleCntrFrameWndEx;
	friend struct AFX_GLOBAL_DATA;
	friend class CMFCVisualManager;
	friend class CFrameImpl;

public:
	CDockingManager();
	virtual ~CDockingManager();

	DWORD GetEnabledAutoHideAlignment() const { return m_dwEnabledSlideBars; }

	void DockPane(CBasePane* pBar, UINT nDockBarID = 0, LPCRECT lpRect = NULL);
	BOOL DockPaneLeftOf(CPane* pBarToDock, CPane* pTargetBar);

	BOOL IsPointNearDockSite(CPoint point, DWORD& dwBarAlignment, BOOL& bOuterEdge) const;

	BOOL Create(CFrameWnd* pParentWnd);
	BOOL EnableDocking(DWORD dwStyle);
	BOOL EnableAutoHidePanes(DWORD dwStyle);

	BOOL AddDockSite(const AFX_DOCKSITE_INFO& info, CDockSite** ppDockBar = NULL); 
	BOOL InsertDockSite(const AFX_DOCKSITE_INFO& info, DWORD dwAlignToInsertAfter, CDockSite** ppDockBar = NULL);

	BOOL AddPane(CBasePane* pWnd, BOOL bTail = TRUE, BOOL bAutoHide = FALSE, BOOL bInsertForOuterEdge = FALSE);
	BOOL InsertPane(CBasePane* pControlBar, CBasePane* pTarget, BOOL bAfter = TRUE);

	void CalcExpectedDockedRect(CWnd* pWnd, CPoint ptMouse, CRect& rectResult, BOOL& bDrawTab, CDockablePane** ppTargetBar);

	void RemovePaneFromDockManager(CBasePane* pWnd, BOOL bDestroy, BOOL bAdjustLayout, BOOL bAutoHide = FALSE, CBasePane* pBarReplacement = NULL);

	virtual CBasePane* PaneFromPoint(CPoint point, int nSensitivity, bool bExactBar = false,
		CRuntimeClass* pRTCBarType = NULL, BOOL bCheckVisibility = FALSE, const CBasePane* pBarToIgnore = NULL) const;
	virtual CBasePane* PaneFromPoint(CPoint point, int nSensitivity, DWORD& dwAlignment,
		CRuntimeClass* pRTCBarType = NULL, const CBasePane* pBarToIgnore = NULL) const;
	virtual CPaneFrameWnd* FrameFromPoint(CPoint pt, CPaneFrameWnd* pFrameToExclude, BOOL bFloatMultiOnly) const;

	virtual AFX_CS_STATUS DeterminePaneAndStatus(CPoint pt, int nSensitivity, DWORD dwEnabledAlignment,
		CBasePane** ppTargetBar, const CBasePane* pBarToIgnore, const CBasePane* pBarToDock);

	virtual BOOL AddMiniFrame(CPaneFrameWnd* pWnd);
	virtual BOOL RemoveMiniFrame(CPaneFrameWnd* pWnd);

	virtual CBasePane* FindPaneByID(UINT uBarID, BOOL bSearchMiniFrames = FALSE);
	virtual CDockSite* FindDockSite(DWORD dwAlignment, BOOL bOuter);

	virtual CDockSite* FindDockSiteByPane(CPane* pTargetBar);

	virtual void FixupVirtualRects();
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual void AdjustDockingLayout(HDWP hdwp = NULL);
	virtual BOOL OnMoveMiniFrame(CWnd* pFrame);

	virtual void AdjustPaneFrames();

	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, UINT uiID = (UINT) -1);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, UINT uiID = (UINT) -1);
	virtual void SetDockState();

	virtual BOOL ShowPanes(BOOL bShow);
	virtual void ShowDelayShowMiniFrames(BOOL bShow);
	virtual void OnActivateFrame(BOOL bActivate);

	CRect GetClientAreaBounds() const { return m_rectClientAreaBounds; }
	CRect GetOuterEdgeBounds() const { return m_rectOuterEdgeBounds; }
	CFrameWnd* GetDockSiteFrameWnd() const { return m_pParentWnd; }

	CMFCAutoHideBar*  AutoHidePane(CDockablePane* pBar, CMFCAutoHideBar* pCurrAutoHideToolBar = NULL);
	void AlignAutoHidePane(CPaneDivider* pDefaultSlider, BOOL bIsVisible = TRUE);
	void BringBarsToTop(DWORD dwAlignment = 0, BOOL bExcludeDockedBars = TRUE);
	void SetAutohideZOrder(CDockablePane* pAHDockingBar);
	void HideAutoHidePanes(CDockablePane* pBarToExclude = NULL, BOOL bImmediately = FALSE);

	virtual BOOL AdjustRectToClientArea(CRect& rectResult, DWORD dwAlignment);

	void Serialize(CArchive& ar);

	static UINT m_nTimeOutBeforeToolBarDock;
	static UINT m_nTimeOutBeforeDockingBarDock;

	void SetPrintPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	BOOL IsPrintPreviewValid() const { return m_bIsPrintPreviewMode; }

	BOOL IsOLEContainerMode() const 
	{
		if (m_pParentWnd == NULL || m_pParentWnd->GetSafeHwnd() == NULL)
		{
			return FALSE;
		}
		return m_pParentWnd->m_pNotifyHook != NULL;
	}

	BOOL SendMessageToMiniFrames(UINT uMessage, WPARAM wParam = 0, LPARAM lParam = 0);
	BOOL ReplacePane(CDockablePane* pOriginalBar, CDockablePane* pNewBar);
	void ReleaseEmptyPaneContainers();

	void LockUpdate(BOOL bLock);

	void DisableRestoreDockState(BOOL bDisable = TRUE) { m_bDisableRestoreDockState = bDisable; }
	BOOL IsInAdjustLayout() const { return m_bAdjustingBarLayout; }

	void GetPaneList(CObList& lstBars, BOOL bIncludeAutohide = FALSE, CRuntimeClass* pRTCFilter = NULL, BOOL bIncludeTabs = FALSE);

	CSmartDockingManager* GetSmartDockingManagerPermanent() const { return m_pSDManager; }

	CSmartDockingManager* GetSmartDockingManager()  // consider returning NULL
	{
		if (m_bSDParamsModified)
		{
			// Smart docking manager should be re-created
			if (m_pSDManager != NULL)
			{
				delete m_pSDManager;
				m_pSDManager = NULL;
			}

			m_bSDParamsModified = FALSE;
		}

		// m_pParentWnd must already exist
		if (m_pSDManager == NULL)
		{
			m_pSDManager = new CSmartDockingManager;
			m_pSDManager->Create(m_pParentWnd);
		}

		return m_pSDManager;
	}

	void StartSDocking(CWnd* pDockingWnd)
	{
		CSmartDockingManager* pSDManager = GetSmartDockingManager();
		if (pSDManager != NULL)
		{
			pSDManager->SetOuterRect(m_rectOuterEdgeBounds);
			pSDManager->Start(pDockingWnd);
		}
	}

	void StopSDocking()
	{
		if (m_pSDManager != NULL)
		{
			m_pSDManager->Stop();
		}
	}

	void GetClientAreaBounds(CRect & rcClient)
	{
		rcClient = m_rectClientAreaBounds;
	}

	void ResortMiniFramesForZOrder();

	const CObList& GetMiniFrames() const
	{
		return m_lstMiniFrames;
	}

	void OnPaneContextMenu(CPoint point);
	BOOL ProcessPaneContextMenuCommand(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	void OnClosePopupMenu();

	void AddHiddenMDITabbedBar(CDockablePane* pBar);
	void RemoveHiddenMDITabbedBar(CDockablePane* pBar);

	void RedrawAllMiniFrames();

public:
	BOOL m_bEnableAdjustLayout;
	BOOL m_bLockUpdate;
	BOOL m_bSizeFrame;
	CRect m_rectInPlace;

	static AFX_DOCK_TYPE __stdcall GetDockingMode() { return m_dockModeGlobal; }

	/// <summary>
	/// Sets docking mode.</summary>
	/// <param name="dockMode">Specifies a docking mode to set. It should be one of the following 
	/// enumerated values: DT_IMMEDIATE, DT_STANDARD or combination of DT_IMMEDIATE and DT_SMART.</param>
	/// <param name="theme">Specifies the theme to be used for smart docking markers. It can be 
	/// one of the following enumerated values: AFX_SDT_DEFAULT, AFX_SDT_VS2005, AFX_SDT_VS2008.</param>
	static void __stdcall SetDockingMode(AFX_DOCK_TYPE dockMode, AFX_SMARTDOCK_THEME theme = AFX_SDT_DEFAULT);

	static void __stdcall SetSmartDockingParams(CSmartDockingInfo& params);
	static CSmartDockingInfo& __stdcall GetSmartDockingParams() { return m_SDParams; }

	/// <summary>
	/// A static method that returns a theme used to display smart docking markers.</summary>
	/// <returns> 
	/// Returns one of the following enumerated values: AFX_SDT_DEFAULT, AFX_SDT_VS2005, AFX_SDT_VS2008.</returns>
	static AFX_SMARTDOCK_THEME __stdcall GetSmartDockingTheme()
	{
		return m_SDTheme;
	}

	static void __stdcall EnableDockSiteMenu(BOOL bEnable = TRUE) { m_bDockBarMenu = bEnable; }
	static BOOL __stdcall IsDockSiteMenu() { return m_bDockBarMenu; }

	void EnablePaneContextMenu(BOOL bEnable, UINT uiCustomizeCmd, const CString& strCustomizeText, BOOL bToolbarsOnly = FALSE);
	void BuildPanesMenu(CMenu& menu, BOOL bToolbarsOnly);

	AFX_IMPORT_DATA static UINT m_ahSlideModeGlobal;
	AFX_IMPORT_DATA static int  m_nDockSensitivity;

	AFX_IMPORT_DATA static BOOL m_bRestoringDockState;
	AFX_IMPORT_DATA static BOOL m_bSavingState;
	AFX_IMPORT_DATA static BOOL m_bHideDockingBarsInContainerMode;
	AFX_IMPORT_DATA static BOOL m_bDisableRecalcLayout;
	AFX_IMPORT_DATA static BOOL m_bFullScreenMode;
	AFX_IMPORT_DATA static BOOL m_bIgnoreEnabledAlignment;

	AFX_IMPORT_DATA static CRuntimeClass* m_pAutoHideToolbarRTC;

protected:
	AFX_IMPORT_DATA static AFX_DOCK_TYPE m_dockModeGlobal;

	CObList m_lstControlBars;
	CObList m_lstAutoHideBars;

	CObList m_lstLoadedBars; // list of control bars restored from the archive
	CObList m_lstNonFloatingBars; // list of non-floating control bars restored from the archive
	CObList m_lstLoadedMiniFrames;

	// list of ahoto hide bar ids loaded from the archive
	CList<AFX_AUTOHIDE_DOCKSITE_SAVE_INFO, AFX_AUTOHIDE_DOCKSITE_SAVE_INFO&> m_lstLoadedAutoHideBarIDs;

	CObList m_lstMiniFrames; // list miniframes; these miniframes contain control bars that belong to this dock manager (and parent frame as well)

	CFrameWnd* m_pParentWnd;
	CRect m_rectDockBarBounds;
	CRect m_rectClientAreaBounds; // contains bounds of the client area and calculated as m_rectDockBarBounds minus rectangles of all dock bars
	CRect m_rectOuterEdgeBounds;  // contains rectangle of "outer edges" and calculated as m_rectDockBarBounds minus  rectangles of first nonresizable dock bars

	DWORD m_dwEnabledDockBars;
	DWORD m_dwEnabledSlideBars;

	CBasePane*    m_pLastTargetBar;
	CMultiPaneFrameWnd* m_pLastMultiMiniFrame;

	clock_t       m_clkLastTime;
	AFX_CS_STATUS m_statusLast;

	// Print preview memebers:
	BOOL    m_bIsPrintPreviewMode;
	CObList m_lstBarsHiddenInPreview;

	CPaneFrameWnd* m_pActiveSlidingWnd;

	BOOL m_bAdjustingBarLayout;
	BOOL m_bRecalcLayout;
	BOOL m_bDisableSetDockState; // should be disabled if load from registry failed
	BOOL m_bDisableRestoreDockState; // can be set by user to disable load dock state

	// Bars hidden on deactivation
	CList<HWND, HWND&> m_lstBarsHiddenOnDeactivate;
	// maintains a list of control bars hidden during in-place activation
	CList<HWND, HWND&> m_lstBarsHiddenForOLE;
	// maintains a list of control bars hidden during in-place activation
	CList<HWND, HWND&> m_lstHiddenMDITabbedBars;

	CSmartDockingManager* m_pSDManager;

	AFX_IMPORT_DATA static CSmartDockingInfo m_SDParams;
	AFX_IMPORT_DATA static BOOL m_bSDParamsModified;
	AFX_IMPORT_DATA static BOOL m_bDockBarMenu;
	AFX_IMPORT_DATA static AFX_SMARTDOCK_THEME m_SDTheme;

	// Map of control bars that appear in context menu:
	CMap<UINT,UINT,CPane*,CPane*> m_mapControlBarsInMenu;
	BOOL m_bControlBarsMenuIsShown;

	// Controlbar context menu attributes:
	BOOL m_bControlBarsContextMenu;
	BOOL m_bControlBarsContextMenuToolbarsOnly;
	BOOL m_bHiddenForOLE;
	UINT m_uiCustomizeCmd;
	CString m_strCustomizeText;

protected:
	void AlignByRect(const CRect& rectToAlignBy, CRect& rectResult, DWORD dwAlignment, BOOL bHorizontal, BOOL bResizable);
	void AlignPaneContainer(CRect& rectCurrBounds, POSITION& posNextBar, DWORD dwAlignment, BOOL bHorizontal);
	void CalcPaneOffset(CAutoHideDockSite* pBar);

	void HideForPrintPreview(const CObList& lstBars);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
