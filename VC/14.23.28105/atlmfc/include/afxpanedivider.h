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
#include "afxbasepane.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CPaneContainerManager;
class CPane;
class CDockablePane;
class CPaneContainer;
class CPaneTrackingWnd;

/*============================================================================*/
// CPaneDivider window

class CPaneDivider : public CBasePane
{
	friend class CDockablePane;

	DECLARE_DYNCREATE(CPaneDivider)

// Construction
public:
	CPaneDivider();
	CPaneDivider(BOOL bDefaultSlider, CWnd* pParent = NULL);

	void Init(BOOL bDefaultSlider = FALSE, CWnd* pParent = NULL);

	enum SS_STYLE
	{
		SS_HORZ = 0x1,
		SS_VERT = 0x2
	};

// Attributes
public:
	BOOL IsHorizontal() const { return m_dwDividerStyle & SS_HORZ; }
	int GetWidth() const  { return m_nWidth; }
	static int __stdcall GetDefaultWidth() { return m_nDefaultWidth; }
	BOOL IsDefault() const { return m_bDefaultDivider; }
	BOOL IsAutoHideMode() const { return m_bAutoHideMode; }

	DWORD GetPaneDividerStyle() const { return m_dwDividerStyle; }

	virtual BOOL DoesAllowDynInsertBefore() const;

	AFX_IMPORT_DATA static int m_nDefaultWidth;
	AFX_IMPORT_DATA static CRuntimeClass* m_pContainerManagerRTC;
	AFX_IMPORT_DATA static CRuntimeClass* m_pSliderRTC;

// Operations
public:

	void ShowWindow(int nCmdShow);

	virtual void Move(CPoint& ptOffset, BOOL bAdjustLayout = TRUE);
	virtual void AddPane(CDockablePane* pBar);

	virtual CDockablePane* AddRecentPane(CDockablePane* pBar);

	virtual BOOL InsertPane(CDockablePane* pBarToInsert, CDockablePane* pTargetBar, DWORD dwAlignment, LPCRECT lpRect = NULL);
	virtual BOOL ReplacePane(CDockablePane* pBarToReplace, CDockablePane* pBarToReplaceWith);
	virtual void RemovePane(CDockablePane* pBar);
	virtual void RepositionPanes(CRect& rectNew, HDWP& hdwp);
	virtual BOOL AddPaneContainer(CPaneContainerManager& barContainerManager, BOOL bOuterEdge);
	virtual BOOL AddPaneContainer(CDockablePane* pTargetBar, CPaneContainerManager& barContainerManager, DWORD dwAlignment);

	virtual BOOL CheckVisibility();

	virtual void OnShowPane(CDockablePane* pBar, BOOL bShow);

	virtual BOOL DoesContainFloatingPane();
	virtual void CalcExpectedDockedRect(CWnd* pWndToDock, CPoint ptMouse, CRect& rectResult, BOOL& bDrawTab, CDockablePane** ppTargetBar);

	void StoreRecentDockSiteInfo(CDockablePane* pBar);
	void StoreRecentTabRelatedInfo(CDockablePane* pDockingBar, CDockablePane* pTabbedBar);

	void SetPaneContainerManager(CPaneContainerManager* p) {m_pContainerManager = p;}
	void ReleaseEmptyPaneContainers();

	void SetAutoHideMode(BOOL bMode) { m_bAutoHideMode = bMode; }

	const CBasePane* GetFirstPane() const;

	void Serialize(CArchive& ar);
	CDockablePane* FindTabbedPane(UINT nID);
	CPaneContainer* FindPaneContainer(CDockablePane* pBar, BOOL& bLeftBar);
	CRect GetRootContainerRect();

	virtual void NotifyAboutRelease();

	void GetPanes(CObList& lstBars);
	void GetPaneDividers(CObList& lstSliders);

// Overrides
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual BOOL CreateEx(DWORD dwStyleEx, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext);

// Implementation
public:
	virtual ~CPaneDivider();
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCancelMode();

	DECLARE_MESSAGE_MAP()

protected:
	void SetDefaultMode(BOOL bMode) {m_bDefaultDivider = bMode;}
	virtual void StopTracking(BOOL bMoveSlider);

protected:
	UINT   m_nID;
	DWORD m_dwDividerStyle;
	int   m_nWidth;
	int   m_nMinOffset;
	int   m_nMaxOffset;
	int   m_nStep;
	bool  m_bCaptured;
	BOOL  m_bDefaultDivider; // true if this slider is created on the "edge" of one or several containers
	BOOL  m_bAutoHideMode;

	CRect  m_rectLastDragRect;
	CRect  m_rectDragBounds;
	CPoint m_ptHotTotal;
	CPoint m_ptHotDrag;
	CWnd*  m_pParentWndForSerialize;
	CPaneContainerManager* m_pContainerManager;
	CPaneTrackingWnd* m_pWndTrack;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
