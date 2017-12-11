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
#include "afxdockablepane.h"
#include "afxbasetabctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CBaseTabbedPane window

class CBaseTabbedPane : public CDockablePane
{
	DECLARE_DYNAMIC(CBaseTabbedPane)

// Construction
public:
	CBaseTabbedPane(BOOL bAutoDestroy = FALSE);

// Attributes
public:
	virtual CMFCBaseTabCtrl* GetUnderlyingWindow()
	{
		return m_pTabWnd;
	}

	virtual void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const = 0;
	virtual HICON GetPaneIcon(BOOL bBigIcon);

	void SetAutoDestroy(BOOL bAutoDestroy = TRUE)
	{
		// should be called after dynamic creation (by CreateObject)
		m_bAutoDestroy = bAutoDestroy;
	}

	virtual BOOL HasAutoHideMode() const
	{
		return FALSE;
	}

	virtual int GetTabsNum() const
	{
		if (m_pTabWnd != NULL)
		{
			return m_pTabWnd->GetTabsNum();
		}

		return 0;
	}

	virtual int GetVisibleTabsNum() const
	{
		if (m_pTabWnd != NULL)
		{
			return m_pTabWnd->GetVisibleTabsNum();
		}

		return 0;
	}

	virtual BOOL IsHideSingleTab() const
	{
		if (m_pTabWnd != NULL)
		{
			return m_pTabWnd->IsHideSingleTab();
		}

		return FALSE;
	}

	virtual BOOL CanFloat() const;
	virtual BOOL AllowDestroyEmptyTabbedPane() const {return TRUE;}

	const CArray<int,int>& GetDefaultTabsOrder()
	{
		if (m_arDefaultTabsOrder.GetSize() == 0)
		{
			FillDefaultTabsOrderArray();
		}

		return m_arDefaultTabsOrder;
	}

	void FillDefaultTabsOrderArray();

	virtual void GetMinSize(CSize& size) const;
	
// Operations
public:
	virtual BOOL AddTab(CWnd* pNewBar, BOOL bVisible = TRUE, BOOL bSetActive = TRUE, BOOL bDetachable = TRUE);
	
	virtual BOOL FloatTab(CWnd* pBar, int nTabID, AFX_DOCK_METHOD dockMethod, BOOL bHide = FALSE);
	virtual BOOL DetachPane(CWnd* pBar, BOOL bHide = FALSE);
	virtual BOOL RemovePane(CWnd* pBar);
	virtual BOOL ShowTab(CWnd* pBar, BOOL bShow, BOOL bDelay, BOOL bActivate);

	virtual CWnd* FindPaneByID(UINT uBarID);
	virtual CWnd* FindBarByTabNumber(int nTabNum, BOOL bGetWrappedBar = FALSE);

	virtual CMFCAutoHideBar* SetAutoHideMode(BOOL bMode, DWORD dwAlignment, CMFCAutoHideBar* pCurrAutoHideBar = NULL, BOOL bUseTimer = TRUE);


	virtual CWnd* GetFirstVisibleTab(int& iTabNum);
	virtual void RecalcLayout();

	virtual void ApplyRestoredTabInfo(BOOL bUseTabIndexes = FALSE);
	virtual void Serialize(CArchive& ar);
	virtual void SerializeTabWindow(CArchive& ar);
	virtual void SaveSiblingBarIDs(CArchive& ar);

	virtual void EnableSetCaptionTextToTabName(BOOL bEnable) { m_bSetCaptionTextToTabName = bEnable; }
	virtual BOOL CanSetCaptionTextToTabName() const { return m_bSetCaptionTextToTabName; }

	virtual void GetPaneList(CObList& lst, CRuntimeClass* pRTCFilter = NULL);
	virtual void ConvertToTabbedDocument(BOOL bActiveTabOnly = TRUE);

	// it should be static because it may be called before the tab bar has been created
	static  void __stdcall LoadSiblingPaneIDs(CArchive& ar, CList<UINT, UINT>& lstBarIDs);

// Implementation
public:
	virtual ~CBaseTabbedPane();

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL FloatPane(CRect rectFloat, AFX_DOCK_METHOD dockMethod = DM_UNKNOWN, bool bShow = true);

	virtual BOOL SaveState(LPCTSTR lpszProfileName, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

	virtual void StoreRecentDockSiteInfo();

	virtual BOOL Dock(CBasePane* pTargetBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);

	virtual void OnActivateTab(int /*iTabNum*/) {}

protected:
	BOOL m_bAutoDestroy;
	BOOL m_bSetCaptionTextToTabName;

	CMFCBaseTabCtrl*    m_pTabWnd;
	CArray<int,int> m_arDefaultTabsOrder;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
