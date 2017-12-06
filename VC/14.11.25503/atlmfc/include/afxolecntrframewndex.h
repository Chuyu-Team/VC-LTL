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

#include "afxpriv.h"
#include "afxcontrolbarutil.h"
#include "afxdockingmanager.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// COleCntrFrameWndEx frame

class COleCntrFrameWndEx : public COleCntrFrameWnd
{
	friend class COleIPFrameWndEx;
	friend class COleDocIPFrameWndEx;

	DECLARE_DYNAMIC(COleCntrFrameWndEx)

public:
	COleCntrFrameWndEx(COleIPFrameWnd* pInPlaceFrame);

// Attributes
protected:
	CDockingManager m_dockManager;

// Operations
public:
	virtual BOOL OnShowPanes(BOOL bShow);

	void AddDockSite();
	BOOL AddPane(CBasePane* pControlBar, BOOL bTail = TRUE);
	BOOL InsertPane(CBasePane* pControlBar, CBasePane* pTarget, BOOL bAfter = TRUE);
	void RemovePaneFromDockManager(CBasePane* pControlBar, BOOL bDestroy, BOOL bAdjustLayout, BOOL bAutoHide, CBasePane* pBarReplacement);
	void DockPane(CBasePane* pBar, UINT nDockBarID = 0, LPCRECT lpRect = NULL);

	CBasePane* PaneFromPoint(CPoint point, int nSensitivity, bool bExactBar, CRuntimeClass* pRTCBarType) const;
	CBasePane* PaneFromPoint(CPoint point, int nSensitivity, DWORD& dwAlignment, CRuntimeClass* pRTCBarType) const;
	BOOL IsPointNearDockSite(CPoint point, DWORD& dwBarAlignment, BOOL& bOuterEdge) const;

	virtual void AdjustDockingLayout(HDWP hdwp = NULL);

	BOOL EnableDocking(DWORD dwDockStyle);
	BOOL EnableAutoHidePanes(DWORD dwDockStyle);

	CBasePane* GetPane(UINT nID);
	void ShowPane(CBasePane* pBar, BOOL bShow, BOOL bDelay, BOOL bActivate);

	virtual BOOL OnMoveMiniFrame(CWnd* pFrame);
	virtual void RecalcLayout(BOOL bNotify = TRUE);

	BOOL DockPaneLeftOf(CPane* pBar, CPane* pLeftOf);

	void CreateDockingManager() { m_dockManager.Create(this); }
	void AdjustClientArea();

// Overrides
	CDockingManager* GetDockingManager() {return &m_dockManager;}

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
protected:
	virtual ~COleCntrFrameWndEx();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDestroy();
	afx_msg void OnIdleUpdateCmdUI();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
