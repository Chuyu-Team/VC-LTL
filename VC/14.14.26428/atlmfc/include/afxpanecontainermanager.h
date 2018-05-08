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
#include "afxpanedivider.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CPane;
class CPaneContainer;

class CPaneContainerManager : public CObject
{
	friend class CPaneContainer;
	friend class CMultiPaneFrameWnd;
	friend class CRecentPaneContainerInfo;

	DECLARE_DYNCREATE(CPaneContainerManager)
public:
	CPaneContainerManager();
	virtual ~CPaneContainerManager();

	virtual BOOL Create(CWnd* pParentWnd, CPaneDivider* pDefaultSlider, CRuntimeClass* pContainerRTC = NULL);
	void SetPaneContainerRTC(CRuntimeClass* pContainerRTC) { m_pContainerRTC = pContainerRTC; }

	CRuntimeClass* GetPaneContainerRTC() const { return m_pContainerRTC; }

	virtual BOOL AddPaneContainerManager(CPaneContainerManager& srcManager, BOOL bOuterEdge);
	virtual BOOL AddPaneContainerManager(CDockablePane* pTargetControlBar, DWORD dwAlignment, CPaneContainerManager& srcManager, BOOL bCopy);
	virtual BOOL AddPaneContainerManagerToDockablePane(CDockablePane* pTargetControlBar, CPaneContainerManager& srcManager);
	virtual void AddPane(CDockablePane* pControlBarToAdd);
	virtual CDockablePane* AddPaneToRecentPaneContainer(CDockablePane* pBarToAdd, CPaneContainer* pRecentContainer);
	virtual BOOL InsertPane(CDockablePane* pControlBarToInsert, CDockablePane* pTargetControlBar,
		DWORD dwAlignment, LPCRECT lpRect = NULL, AFX_DOCK_METHOD dockMethod = DM_UNKNOWN);

	virtual int  OnPaneDividerMove(CPaneDivider* pSlider, UINT uFlags, int nOffset, HDWP& hdwp);
	virtual void GetWindowRect(CRect& rect) const;
	virtual void GetAvailableSpace(CRect& rect) const;
	virtual BOOL RemovePaneFromPaneContainer(CDockablePane* pControlBar);
	virtual void ResizePaneContainers(UINT nSide, BOOL bExpand, int nOffset, HDWP& hdwp);
	virtual void ResizePaneContainers(CRect rect, HDWP& hdwp);
	virtual BOOL ReplacePane(CDockablePane* pBarOld, CDockablePane* pBarNew);
	virtual void GetMinMaxOffset(CPaneDivider* pSlider, int& nMinOffset, int& nMaxOffset, int& nStep);

	virtual BOOL OnShowPane(CDockablePane* pBar, BOOL bShow);
	virtual BOOL IsRootPaneContainerVisible() const;

	virtual void GetMinSize(CSize& size);
	virtual int  GetVisiblePaneCount() const;
	virtual void EnableGrippers(BOOL bEnable);
	virtual void HideAll();

	virtual BOOL DoesContainFloatingPane();
	virtual BOOL DoesAllowDynInsertBefore() const;

	virtual CDockablePane* PaneFromPoint(CPoint point, int nSensitivity, BOOL bExactBar, BOOL& bIsTabArea, BOOL& bCaption);
	virtual BOOL CheckForMiniFrameAndCaption(CPoint point, CDockablePane** ppTargetControlBar);

	virtual CBasePane* GetFirstPane() const;
	virtual CWnd* GetDockSiteFrameWnd() { return m_pDockSite; }
	virtual CWnd* GetFirstVisiblePane() const;

	virtual BOOL CanBeAttached() const;

	CPaneDivider* GetDefaultPaneDivider() const {return m_pDefaultSlider;}

	void ReleaseEmptyPaneContainers();
	void AddPaneToList(CDockablePane* pControlBarToAdd); 
	BOOL IsEmpty() const;

	int GetTotalRefCount() const;
	int GetPaneCount() const { return(int)m_lstControlBars.GetCount(); }
	int GetNodeCount() const;

	virtual void SetResizeMode(BOOL bResize);
	virtual void RemovePaneDivider(CPaneDivider* pSlider);
	virtual void StoreRecentDockSiteInfo(CDockablePane* pBar);

	BOOL IsAutoHideMode() const { return (m_pDefaultSlider != NULL && m_pDefaultSlider->IsAutoHideMode()); }

	void SetDefaultPaneDividerForPanes(CPaneDivider* pSlider);
	void Serialize(CArchive& ar);
	CDockablePane* FindTabbedPane(UINT nID);
	virtual CPaneContainer* FindPaneContainer(CDockablePane* pBar, BOOL& bLeftBar);

	void AddPanesToList(CObList* plstControlBars, CObList* plstSliders);
	void RemoveAllPanesAndPaneDividers();

	void CalcRects(CRect& rectOriginal, CRect& rectInserted, CRect& rectSlider, DWORD& dwSliderStyle, DWORD dwAlignment, CSize sizeMinOriginal, CSize sizeMinInserted);
	void NotifyPaneDivider();

	void RemoveNonValidPanes();
	BOOL CheckAndRemoveNonValidPane(CWnd* pWnd);

protected:
	CPaneDivider* CreatePaneDivider(CRect rectSlider, DWORD dwSliderStyle, int nSliderID = -1);
	UINT FindPane(CPoint pt, CPane** ppBar, POSITION& posRet);
	UINT FindPane(CRect rect, CPane** ppBar, POSITION& posRet);
	BOOL AddPaneAndPaneDivider(CDockablePane* pBarOriginal, CDockablePane* pBarToInsert, POSITION posNearestBar, DWORD dwAlignment);
	BOOL AddPaneAndPaneContainer(CDockablePane* pBarOriginal, CPaneContainer* pContainerToInsert, DWORD dwAlignment);

	CObList  m_lstControlBars;
	CObList  m_lstSliders;

	CPaneContainer* m_pRootContainer;
	CRuntimeClass* m_pContainerRTC;
	CPaneDivider* m_pDefaultSlider;

	BOOL  m_bDestroyRootContainer;
	CWnd* m_pDockSite;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
