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
#include "afxpaneframewnd.h"
#include "afxpanecontainermanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMultiPaneFrameWnd window

class CMultiPaneFrameWnd : public CPaneFrameWnd
{
	DECLARE_SERIAL(CMultiPaneFrameWnd)
// Construction
public:
	CMultiPaneFrameWnd();

// Attributes
public:
	CPaneContainerManager& GetPaneContainerManager() { return m_barContainerManager; }
	virtual BOOL CanBeDockedToPane(const CDockablePane* pDockingBar) const;

// Operations
public:
	virtual void AddPane(CBasePane* pWnd);
	virtual void RemovePane(CBasePane* pBar, BOOL bDestroy = FALSE, BOOL bNoDelayedDestroy = TRUE);
	virtual void ReplacePane(CBasePane* pBarOrg, CBasePane* pBarReplaceWith);
	virtual BOOL DockFrame(CPaneFrameWnd* pDockedFrame, AFX_DOCK_METHOD dockMethod);
	virtual BOOL DockPane(CDockablePane* pDockedBar);

	virtual int GetPaneCount() const { return m_barContainerManager.GetPaneCount(); }
	virtual int GetVisiblePaneCount() const { return m_barContainerManager.GetVisiblePaneCount(); }
	virtual CWnd* GetFirstVisiblePane() const { return m_barContainerManager.GetFirstVisiblePane(); }
	virtual CWnd* GetPane() const;

	virtual BOOL CanBeAttached() const { return m_barContainerManager.CanBeAttached(); }

	virtual void CloseMiniFrame();

	virtual BOOL SetPreDockState(AFX_PREDOCK_STATE preDockState, CBasePane* pBarToDock = NULL, AFX_DOCK_METHOD dockMethod = DM_MOUSE);

	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, UINT uiID = (UINT) -1);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, UINT uiID = (UINT) -1);

	virtual void Serialize(CArchive& ar);
	virtual void SetDockState(CDockingManager* pDockManager);
	virtual void StoreRecentDockSiteInfo(CPane* pBar);
	virtual void StoreRecentTabRelatedInfo(CDockablePane* pDockingBar, CDockablePane* pTabbedBar);

	virtual BOOL AddRecentPane(CDockablePane* pBar);
	virtual void DockRecentPaneToMainFrame(CDockablePane* pBar);

	virtual void OnShowPane(CDockablePane* pBar, BOOL bShow);

	virtual BOOL InsertPane(CBasePane* pControlBar, CBasePane* pTarget, BOOL bAfter);

	virtual void CalcExpectedDockedRect(CWnd* pWndToDock, CPoint ptMouse, CRect& rectResult, BOOL& bDrawTab, CDockablePane** ppTargetBar);
	virtual CBasePane* PaneFromPoint(CPoint point, int nSensitivity, BOOL bCheckVisibility);

	virtual void RemoveNonValidPanes();

	virtual void OnSetRollUpTimer();
	virtual void OnKillRollUpTimer();

	virtual void AdjustPaneFrames();

	virtual void OnDockToRecentPos();
	virtual void ConvertToTabbedDocument();

	void SetLastFocusedPane(HWND hwnd) {m_hWndLastFocused = hwnd;}

// Implementation
public:
	virtual ~CMultiPaneFrameWnd();

	virtual void CheckGripperVisibility();
	virtual void OnPaneRecalcLayout();
	virtual void AdjustLayout() {}
	virtual CString GetCaptionText();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnCheckEmptyState(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()

protected:
	virtual void SaveRecentFloatingState();
	virtual CDockablePane* DockPaneStandard(BOOL& bWasDocked);

protected:
	CPaneContainerManager m_barContainerManager;
	HWND                 m_hWndLastFocused;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
