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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CBasePane;
class CPane;
class CDockablePane;
class CPaneDivider;

class CPaneContainer : public CObject  
{
	friend class CPaneContainerManager;
	friend class CRecentPaneContainerInfo;

	DECLARE_DYNAMIC(CPaneContainer);
public:
	CPaneContainer(CPaneContainerManager* pManager = NULL, CDockablePane* pLeftBar = NULL, CDockablePane* pRightBar = NULL, CPaneDivider* pSlider = NULL);
	virtual ~CPaneContainer();

	CDockablePane* AddPane(CDockablePane* pBar);
	BOOL AddSubPaneContainer(CPaneContainer* pContainer, BOOL bRightNodeNew);

	enum BC_FIND_CRITERIA
	{
		BC_FIND_BY_LEFT_BAR,
		BC_FIND_BY_RIGHT_BAR,
		BC_FIND_BY_SLIDER,
		BC_FIND_BY_CONTAINER
	};

	CPaneContainer* FindSubPaneContainer(const CObject* pObject, BC_FIND_CRITERIA findCriteria);

	BOOL IsEmpty() const;
	BOOL IsLeftPartEmpty(BOOL bCheckVisibility = FALSE) const;
	BOOL IsRightPartEmpty(BOOL bCheckVisibility = FALSE) const;

	BOOL IsVisible() const;

	int  GetNodeCount() const;
	int  GetTotalReferenceCount() const;

	const CDockablePane* GetLeftPane() const { return m_pBarLeftTop; }
	const CDockablePane* GetRightPane() const { return m_pBarRightBottom; }
	const CPaneDivider* GetPaneDivider() const { return m_pSlider; }
	const CPaneContainer* GetLeftPaneContainer() const { return m_pLeftContainer; }
	const CPaneContainer* GetRightPaneContainer() const { return m_pRightContainer; }

	DWORD GetRecentPaneDividerStyle() const { return m_dwRecentSliderStyle; }
	CRect GetRecentPaneDividerRect() const { return m_rectRecentSlider; }

	CPaneContainer* GetParentPaneContainer() const { return m_pParentContainer; }

	void SetPane(CDockablePane* pBar, BOOL bLeft);
	void SetPaneContainer(CPaneContainer* pContainer, BOOL bLeft);
	void SetPaneDivider(CPaneDivider* pSlider) { m_pSlider = pSlider; }

	BOOL IsLeftPaneContainer() const;
	BOOL IsLeftPane(CDockablePane* pBar) const;

	void SetParentPaneContainer(CPaneContainer* p) { m_pParentContainer = p; }

	BOOL SetUpByID(UINT nID, CDockablePane* pBar);
	CDockablePane* FindTabbedPane(UINT nID);

// Overrides
	virtual void GetWindowRect(CRect& rect, BOOL bIgnoreVisibility = FALSE) const;
	virtual void GetMinSize(CSize& size) const;
	virtual void GetMinSizeLeft(CSize& size) const;
	virtual void GetMinSizeRight(CSize& size) const;
	virtual int  GetResizeStep() const;

	virtual void OnShowPane(CDockablePane* pBar, BOOL bShow);

	virtual void RemovePane(CDockablePane* pBar);
	virtual void DeletePane(CDockablePane* pBar, BC_FIND_CRITERIA barType);
	virtual void StoreRecentDockSiteInfo(CDockablePane* pBar);

	void OnDeleteHidePane(CDockablePane* pBar, BOOL bHide);

	virtual void Move(CPoint ptNewLeftTop);

	virtual int StretchPaneContainer(int nOffset, BOOL bStretchHorz, BOOL bLeftBar, BOOL bMoveSlider, HDWP& hdwp);
	virtual int OnMoveInternalPaneDivider(int nOffset, HDWP& hdwp);
	virtual void ResizePane(int nOffset, CPane* pBar, CPaneContainer* pContainer, BOOL bHorz, BOOL bLeftBar, HDWP& hdwp);
	virtual int CalcAvailablePaneSpace(int nRequiredOffset, CPane* pBar, CPaneContainer* pContainer, BOOL bLeftBar);
	virtual CSize CalcAvailableSpace(CSize sizeStretch, BOOL bLeftBar);

	virtual void Resize(CRect rect, HDWP& hdwp, BOOL bRedraw = FALSE);
	virtual void ResizePartOfPaneContainer(int nOffset, BOOL bLeftPart, HDWP& hdwp);

	void SetRecentPercent(int nRecentPercent) { m_nRecentPercent = nRecentPercent; };
	int GetRecentPercent() { return m_nRecentPercent; }
	void CalculateRecentSize();

	void CheckPaneDividerVisibility();
	void RemoveNonValidPanes();

	BOOL IsDisposed() const { return m_bDisposed; }

	void AddRef();
	DWORD Release();
	LONG GetRefCount() { return m_dwRefCount; }

	void Serialize(CArchive& ar);
	void ReleaseEmptyPaneContainer();

	virtual CPaneContainer* Copy(CPaneContainer* pParentContainer);

	CList<UINT, UINT>* GetAssociatedSiblingPaneIDs(CDockablePane* pBar);

	AFX_IMPORT_DATA static BOOL m_bMaintainPercentage;
	AFX_IMPORT_DATA static BOOL m_bRetainInternalSliderPosition;

protected:
	CDockablePane* m_pBarLeftTop;
	CDockablePane* m_pBarRightBottom;
	CPaneDivider*      m_pSlider;

	CPaneContainer* m_pLeftContainer;
	CPaneContainer* m_pRightContainer;
	CPaneContainer* m_pParentContainer;

	CPaneContainerManager* m_pContainerManager;

	int  m_nRecentPercent;
	UINT m_nSavedLeftBarID;
	UINT m_nSavedRightBarID;
	UINT m_nSavedSliderID;
	LONG m_dwRefCount;

	BOOL m_bIsRecentSliderHorz;
	BOOL m_bSavedSliderVisibility;
	BOOL m_bDisposed;

	DWORD m_dwRecentSliderStyle;
	CRect m_rectRecentSlider;
	CRect m_rectSavedSliderRect;

	CList<UINT, UINT> m_lstSavedSiblingBarIDsLeft;
	CList<UINT, UINT> m_lstSavedSiblingBarIDsRight;

protected:
	void AddNode(CPaneContainer* pContainer);
	void CleanUp();
	
	void SetPaneContainerManager(CPaneContainerManager* p, BOOL bDeep = FALSE);

	void MoveWnd(CWnd* pWnd, int nOffset, BOOL bHorz);

	void SaveTabbedPane(CArchive& ar, CDockablePane* pBar);
	CDockablePane* LoadTabbedPane(CArchive& ar, CList<UINT, UINT>& lstBarIDs);
	void FreeReleasedPaneContainer();

	BOOL IsPaneDividerHorz() const;
};

class CPaneContainerGC
{
	CObList m_lstDisposedContainers;

public:
	void AddPaneContainer(CPaneContainer* p)
	{
		m_lstDisposedContainers.AddTail(p);
	}

	void CleanUp()
	{
		while (!CPaneContainerGC::m_lstDisposedContainers.IsEmpty())
		{
			delete m_lstDisposedContainers.RemoveHead();
		}
	}

	~CPaneContainerGC()
	{
		CPaneContainerGC::CleanUp();
	}
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
