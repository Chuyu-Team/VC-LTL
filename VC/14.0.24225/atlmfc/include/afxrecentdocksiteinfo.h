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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CPaneContainer;
class CDockSite;
class CDockingPanesRow;
class CPaneDivider;
class CPaneFrameWnd;
class CPane;
class CDockablePane;

class CRecentPaneContainerInfo : public CObject
{
public:
	CRecentPaneContainerInfo();
	virtual ~CRecentPaneContainerInfo();

	CRecentPaneContainerInfo& operator= (CRecentPaneContainerInfo& src);

	void Init();

	virtual void StoreDockInfo(CPaneContainer* pRecentContainer, CDockablePane* pBar, CDockablePane* pTabbedBar = NULL);
	virtual void SetInfo(CRecentPaneContainerInfo& srcInfo);

public:
	
	CRect m_rectDockedRect;   // in client coordinates
	int   m_nRecentPercent;   // what percent CB took in container
	BOOL  m_bIsRecentLeftBar; // is CB was left in its container

	CList<HWND,HWND> m_lstRecentListOfBars; // need to remember a list of siblings bar in container manager to place the CB back when docked again
	CPaneContainer*   m_pRecentBarContainer; // recent container of CB
	CPaneContainer*   m_pRecentContainerOfTabWnd;
};


class CRecentDockSiteInfo : public CObject
{
public:
	CRecentDockSiteInfo(CPane* pBar);
	virtual ~CRecentDockSiteInfo();

	CRecentDockSiteInfo& operator= (CRecentDockSiteInfo& src);

	void Init();
	void CleanUp();

	virtual void StoreDockInfo(CPaneContainer* pRecentContainer, CDockablePane* pTabbedBar = NULL);
	virtual void SetInfo(BOOL bForSlider, CRecentDockSiteInfo& srcInfo);

	CPaneContainer* GetRecentPaneContainer(BOOL bForSlider);
	CPaneContainer* GetRecentTabContainer(BOOL bForSlider);
	CRect& GetRecentDockedRect(BOOL bForSlider);
	int GetRecentDockedPercent(BOOL bForSlider);
	BOOL IsRecentLeftPane(BOOL bForSlider);
	void SaveListOfRecentPanes(CList<HWND,HWND>& lstOrg, BOOL bForSlider);
	CList<HWND, HWND>& GetRecentListOfPanes(BOOL bForSlider);
	CPaneDivider* GetRecentDefaultPaneDivider();

public:
	CRect m_rectRecentFloatingRect; // in screen coordinates
	DWORD m_dwRecentAlignmentToFrame; // to what frame side control bar was docked

	int   m_nRecentTabNumber; // recent position on tab window
	int   m_nRecentRowIndex;  // if CB was resided on a dock bar what row it occupied (zero-based)
	                          // it will be used instead of m_pRecentDockBarRow if this pointer is invalid (the row was deleted)

	CDockSite*    m_pRecentDockBar;    // what dock bar CB was docked on
	CDockingPanesRow* m_pRecentDockBarRow; // a pointer to dock bar row

	HWND m_hRecentDefaultSlider; // recent default slider of CB
	HWND m_hRecentMiniFrame;

	CRecentPaneContainerInfo m_recentSliderInfo;
	CRecentPaneContainerInfo m_recentMiniFrameInfo;

protected:
	CPane* m_pBar;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
