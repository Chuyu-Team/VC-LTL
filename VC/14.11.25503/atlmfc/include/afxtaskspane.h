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
#include "afxcaptionmenubutton.h"
#include "afxtoolbar.h"
#include "afxtaskspaneframewnd.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCTasksPane additional classes

class CMFCTasksPane;

class CMFCTasksPanePropertyPage : public CObject
{
public:
	CMFCTasksPanePropertyPage(LPCTSTR lpszName, CMFCTasksPane *pTaskPane)
	{
		m_strName = lpszName;
		m_pTaskPane = pTaskPane;
	}

	virtual ~CMFCTasksPanePropertyPage()
	{
		m_pTaskPane = NULL;
	}

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	CString     m_strName;
	CMFCTasksPane* m_pTaskPane;
};

class CMFCTasksPaneTaskGroup : public CObject
{
public:
	CMFCTasksPaneTaskGroup(LPCTSTR lpszName, BOOL bIsBottom, BOOL bIsSpecial = FALSE, BOOL bIsCollapsed = FALSE, CMFCTasksPanePropertyPage* pPage = NULL, HICON hIcon = NULL)
	{
		m_pPage = pPage;
		m_strName =  lpszName;
		m_bIsBottom = bIsBottom;
		m_bIsSpecial = bIsSpecial;
		m_rect.SetRectEmpty();
		m_rectGroup.SetRectEmpty();
		m_bIsCollapsed = bIsCollapsed;
		m_hIcon = hIcon;
		m_sizeIcon = CSize(0, 0);

		m_clrText    = (COLORREF)-1;
		m_clrTextHot = (COLORREF)-1;

		ICONINFO iconInfo;
		::ZeroMemory(&iconInfo, sizeof(iconInfo));
		::GetIconInfo(m_hIcon, &iconInfo);
		
		BITMAP bm;
		::ZeroMemory(&bm, sizeof(bm));
		::GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

		m_sizeIcon = CSize(bm.bmWidth, bm.bmHeight);

		::DeleteObject(iconInfo.hbmColor);
		::DeleteObject(iconInfo.hbmMask);
	}

	virtual ~CMFCTasksPaneTaskGroup()
	{
		while (!m_lstTasks.IsEmpty())
		{
			delete m_lstTasks.RemoveHead();
		}
		m_pPage = NULL;
	}

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	CMFCTasksPanePropertyPage* m_pPage;
	CString  m_strName;
	CObList  m_lstTasks;
	BOOL     m_bIsBottom;
	BOOL     m_bIsSpecial;
	BOOL     m_bIsCollapsed;
	CRect    m_rect;
	CRect    m_rectGroup;
	CSize    m_sizeIcon;
	HICON    m_hIcon;
	COLORREF m_clrText;
	COLORREF m_clrTextHot;
};

class CMFCTasksPaneTask : public CObject
{
public:
	CMFCTasksPaneTask(CMFCTasksPaneTaskGroup* pGroup, LPCTSTR lpszName, int nIcon, UINT uiCommandID, DWORD_PTR dwUserData = 0, HWND hwndTask = NULL, BOOL bAutoDestroyWindow = FALSE, int nWindowHeight = 0)
	{
		m_pGroup = pGroup;
		m_strName = lpszName == NULL ? _T("") : lpszName;
		m_nIcon = nIcon;
		m_uiCommandID = uiCommandID;
		m_dwUserData = dwUserData;
		m_hwndTask = hwndTask;
		m_bAutoDestroyWindow = bAutoDestroyWindow;
		m_nWindowHeight = nWindowHeight;
		m_bVisible = TRUE;
		m_bEnabled = TRUE;
		m_bIsSeparator = lpszName == NULL;
		m_clrText = (COLORREF)-1;
		m_clrTextHot = (COLORREF)-1;
		m_bIsBold = FALSE;

		m_rect.SetRectEmpty();
	}

	virtual ~CMFCTasksPaneTask()
	{
		if (m_hwndTask != NULL && m_bAutoDestroyWindow)
		{
			CWnd* pWnd = CWnd::FromHandlePermanent(m_hwndTask);
			if (pWnd != NULL)
			{
				pWnd->DestroyWindow();
				delete pWnd;
			}
			else
			{
				::DestroyWindow(m_hwndTask);
			}
		}
		m_pGroup = NULL;
	}

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	CMFCTasksPaneTaskGroup* m_pGroup;
	CString  m_strName;
	CRect    m_rect;
	int      m_nIcon;
	int      m_nWindowHeight;
	UINT     m_uiCommandID;
	DWORD_PTR m_dwUserData;
	HWND     m_hwndTask;
	BOOL     m_bAutoDestroyWindow;
	BOOL     m_bVisible;
	BOOL     m_bEnabled;
	BOOL     m_bIsSeparator;
	BOOL     m_bIsBold; // For labels only
	COLORREF m_clrText;
	COLORREF m_clrTextHot;
};

class CMFCTasksPaneToolBar : public CMFCToolBar
{
	friend class CMFCTasksPane;

	DECLARE_SERIAL(CMFCTasksPaneToolBar)

// Overrides
public:
	CMFCTasksPaneToolBar()
	{
		m_pBtnBack = NULL;
		m_pBtnForward = NULL;
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }
	virtual BOOL IsButtonExtraSizeAvailable() const { return FALSE; }

	void UpdateMenuButtonText(const CString& str);
	void UpdateButtons();

protected:
	virtual void AdjustLayout();
	virtual void AdjustLocations();
	virtual BOOL OnUserToolTip(CMFCToolBarButton* pButton, CString& strTTText) const;

	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	CMFCToolBarMenuButton* m_pBtnBack;
	CMFCToolBarMenuButton* m_pBtnForward;
};

/*============================================================================*/
// CMFCTasksPane window

class CMFCTasksPane : public CDockablePane
{
	DECLARE_SERIAL(CMFCTasksPane);

// Construction
public:
	CMFCTasksPane();

// Attributes
protected:
	BOOL m_bCanCollapse;
	BOOL m_bUseNavigationToolbar;
	BOOL m_bHistoryMenuButtons;
	BOOL m_bUseScrollButtons;
	BOOL m_bAnimationEnabled;
	BOOL m_bOffsetCustomControls;
	BOOL m_bMenuBtnPressed;
	BOOL m_bWrapTasks;
	BOOL m_bWrapLabels;

	int m_iActivePage;
	int m_iScrollMode;   // -1 - Up, 0 - None, 1 - Down
	int m_iScrollBtnHeight;
	int m_nVertScrollOffset;
	int m_nVertScrollTotal;
	int m_nVertScrollPage;
	int m_nRowHeight;
	int m_nAnimGroupExtraHeight;
	int m_nVertMargin;
	int m_nHorzMargin;
	int m_nGroupVertOffset;
	int m_nGroupCaptionHeight;
	int m_nGroupCaptionHorzOffset;
	int m_nGroupCaptionVertOffset;
	int m_nTasksHorzOffset;
	int m_nTasksIconHorzOffset;
	int m_nTasksIconVertOffset;

	const int m_nMaxHistory;
	UINT      m_uiToolbarBmpRes;

	HFONT  m_hFont;
	CFont  m_fontBold;
	CFont  m_fontBoldUnderline;
	CFont  m_fontUnderline;
	CSize  m_sizeIcon;
	CSize  m_sizeToolbarImage;
	CSize  m_sizeToolbarButton;
	CSize  m_sizeAnim;
	CRect  m_rectToolbar;
	CRect  m_rectScrollUp;
	CRect  m_rectScrollDn;
	CRect  m_rectTasks;
	CMenu  m_menuOther;
	CMFCTasksPaneTask* m_pHotTask;
	CMFCTasksPaneTask* m_pClickedTask;

	CObList m_lstTasksPanes;
	CObList m_lstTaskGroups;
	CString m_strCaption;

	CScrollBar m_wndScrollVert; // Vertical scroll bar
	CImageList m_lstIcons;

	CArray<int, int> m_arrHistoryStack;

	CMFCTasksPaneTaskGroup* m_pHotGroupCaption;
	CMFCTasksPaneTaskGroup* m_pClickedGroupCaption;
	CMFCTasksPaneTaskGroup* m_pAnimatedGroup;

	CMFCTasksPaneToolBar m_wndToolBar;

	AFX_IMPORT_DATA static clock_t m_nLastAnimTime;
	AFX_IMPORT_DATA static const int m_nAnimTimerDuration;
	AFX_IMPORT_DATA static const int m_nScrollTimerDuration;

// Operations
public:
	BOOL SetIconsList(UINT uiImageListResID, int cx, COLORREF clrTransparent = RGB(255, 0, 255));
	void SetIconsList(HIMAGELIST hIcons);

	void RecalcLayout(BOOL bRedraw = TRUE);

	// --------------
	// Pages support:
	// --------------
	int AddPage(LPCTSTR lpszPageLabel);
	void RemovePage(int nPageIdx);
	void RemoveAllPages();

	int GetPagesCount() const { return (int) m_lstTasksPanes.GetCount(); }

	void SetActivePage(int nPageIdx);
	int GetActivePage() const { return m_arrHistoryStack[m_iActivePage]; }

	void GetPreviousPages(CStringList& lstPrevPages) const;
	void GetNextPages(CStringList&  lstNextPages) const;

	void SetCaption(LPCTSTR lpszName);
	void SetPageCaption(int nPageIdx, LPCTSTR lpszName);

	BOOL GetPageByGroup(int nGroup, int &nPage) const;

	virtual void OnPressBackButton();
	virtual void OnPressForwardButton();
	virtual void OnPressHomeButton();
	virtual void OnPressOtherButton(CMFCCaptionMenuButton* pbtn, CWnd* pWndOwner);
	BOOL IsBackButtonEnabled() const { return m_iActivePage > 0; }
	BOOL IsForwardButtonEnabled() const { return m_iActivePage < m_arrHistoryStack.GetUpperBound(); }

	virtual BOOL IsAccessibilityCompatible() { return TRUE; }
	virtual BOOL OnSetAccData(long lVal);

	// --------------
	// Group support:
	// --------------
	int AddGroup(int nPageIdx, LPCTSTR lpszGroupName, BOOL bBottomLocation = FALSE, BOOL bSpecial = FALSE, HICON hIcon = NULL);
	int AddGroup(LPCTSTR lpszGroupName, BOOL bBottomLocation = FALSE, BOOL bSpecial = FALSE, HICON hIcon = NULL)
	{
		return AddGroup(0, lpszGroupName, bBottomLocation, bSpecial, hIcon);
	}
	void RemoveGroup(int nGroup);
	void RemoveAllGroups(int nPageIdx = 0);

	BOOL SetGroupName(int nGroup, LPCTSTR lpszGroupName);
	BOOL SetGroupTextColor(int nGroup, COLORREF color, COLORREF colorHot = (COLORREF)-1);
	BOOL CollapseGroup(CMFCTasksPaneTaskGroup* pGroup, BOOL bCollapse = TRUE);

	BOOL CollapseGroup(int nGroup, BOOL bCollapse = TRUE) { return CollapseGroup(GetTaskGroup(nGroup), bCollapse); }

	void CollapseAllGroups(BOOL bCollapse = TRUE);
	void CollapseAllGroups(int nPageIdx, BOOL bCollapse);

	void EnableGroupCollapse(BOOL bEnable)
	{
		if (!bEnable)
		{
			CollapseAllGroups(FALSE);
		}
		m_bCanCollapse = bEnable;
	}
	BOOL IsGroupCollapseEnabled() const { return m_bCanCollapse; }

	CMFCTasksPaneTaskGroup* GetTaskGroup(int nGroup) const;
	BOOL GetGroupLocation(CMFCTasksPaneTaskGroup* pGroup, int &nGroup) const;
	int GetGroupCount() const { return (int) m_lstTaskGroups.GetCount(); }

	// -------------
	// Task support:
	// -------------
	int AddTask(int nGroup, LPCTSTR lpszTaskName, int nTaskIcon = -1, UINT uiCommandID = 0, DWORD_PTR dwUserData = 0);
	int AddSeparator(int nGroup) { return AddTask(nGroup, NULL); }

	BOOL SetTaskName(int nGroup, int nTask, LPCTSTR lpszTaskName);
	BOOL SetTaskTextColor(int nGroup, int nTask, COLORREF color, COLORREF colorHot = (COLORREF)-1);
	BOOL ShowTask(int nGroup, int nTask, BOOL bShow = TRUE, BOOL bRedraw = TRUE);
	BOOL ShowTaskByCmdId(UINT uiCommandID, BOOL bShow = TRUE, BOOL bRedraw = TRUE);
	BOOL RemoveTask(int nGroup, int nTask, BOOL bRedraw = TRUE);
	void RemoveAllTasks(int nGroup);
	BOOL GetTaskLocation(UINT uiCommandID, int& nGroup, int& nTask) const;
	BOOL GetTaskLocation(HWND hwndTask, int& nGroup, int& nTask) const;
	CMFCTasksPaneTask* GetTask(int nGroup, int nTask) const;
	BOOL GetTaskLocation(CMFCTasksPaneTask* pTask, int &nGroup, int& nTask) const;

	int GetTaskCount(int nGroup) const
	{
		ASSERT(nGroup >= 0);
		ASSERT(nGroup < m_lstTaskGroups.GetCount());
		CMFCTasksPaneTaskGroup* pGroup = GetTaskGroup(nGroup);
		ASSERT_VALID(pGroup);
		return (int) pGroup->m_lstTasks.GetCount();
	}
	
	int AddWindow(int nGroup, HWND hwndTask, int nWndHeight, BOOL bAutoDestroyWindow = FALSE, DWORD_PTR dwUserData = 0);
	BOOL SetWindowHeight(int nGroup, HWND hwndTask, int nWndHeight);
	BOOL SetWindowHeight(HWND hwndTask, int nWndHeight);

	int AddLabel(int nGroup, LPCTSTR lpszLabelName, int nTaskIcon = -1, BOOL bIsBold = FALSE)
	{
		int nTask = AddTask(nGroup, lpszLabelName, nTaskIcon);
		if (nTask == -1 || !bIsBold)
		{
			return nTask;
		}

		POSITION pos = m_lstTaskGroups.FindIndex(nGroup);
		if (pos == NULL)
		{
			ASSERT(FALSE);
			return -1;
		}

		CMFCTasksPaneTaskGroup* pGroup = (CMFCTasksPaneTaskGroup*) m_lstTaskGroups.GetAt(pos);
		ASSERT_VALID(pGroup);

		CMFCTasksPaneTask* pTask = (CMFCTasksPaneTask*)pGroup->m_lstTasks.GetTail();
		if (pTask != NULL)
		{
			pTask->m_bIsBold = bIsBold;
		}

		return nTask;
	}

	int AddMRUFilesList(int nGroup, int nMaxFiles = 4);

	// --------
	// Margins:
	// --------
	int GetVertMargin() const { return m_nVertMargin; }
	void SetVertMargin(int n = -1)
	{
		ASSERT(n >= -1);
		m_nVertMargin = n;
	}

	int GetHorzMargin() const { return m_nHorzMargin; }
	void SetHorzMargin(int n = -1)
	{
		ASSERT(n >= -1);
		m_nHorzMargin = n;
	}

	int GetGroupVertOffset() const { return m_nGroupVertOffset; }
	void SetGroupVertOffset(int n = -1)
	{
		ASSERT(n >= -1);
		m_nGroupVertOffset = n;
	}

	int GetGroupCaptionHeight() const { return m_nGroupCaptionHeight; }
	void SetGroupCaptionHeight(int n = -1)
	{
		ASSERT(n >= -1);
		m_nGroupCaptionHeight = n;
	}

	int GetGroupCaptionHorzOffset() const { return m_nGroupCaptionHorzOffset; }
	void SetGroupCaptionHorzOffset(int n = -1)
	{
		ASSERT(n >= -1);
		m_nGroupCaptionHorzOffset = n;
	}

	int GetGroupCaptionVertOffset() const { return m_nGroupCaptionVertOffset; }
	void SetGroupCaptionVertOffset(int n = -1)
	{
		ASSERT(n >= -1);
		m_nGroupCaptionVertOffset = n;
	}

	int GetTasksHorzOffset() const { return m_nTasksHorzOffset; }
	void SetTasksHorzOffset(int n = -1)
	{
		ASSERT(n >= -1);
		m_nTasksHorzOffset = n;
	}

	int GetTasksIconHorzOffset() const { return m_nTasksIconHorzOffset; }
	void SetTasksIconHorzOffset(int n = -1)
	{
		ASSERT(n >= -1);
		m_nTasksIconHorzOffset = n;
	}

	int GetTasksIconVertOffset() const { return m_nTasksIconVertOffset; }
	void SetTasksIconVertOffset(int n = -1)
	{
		ASSERT(n >= -1);
		m_nTasksIconVertOffset = n;
	}

	void EnableOffsetCustomControls(BOOL bEnable) { m_bOffsetCustomControls = bEnable; }

	// ---------
	// Behavior:
	// ---------
	void EnableScrollButtons(BOOL bEnable = TRUE) { m_bUseScrollButtons = bEnable; }

	void EnableNavigationToolbar(BOOL bEnable = TRUE, UINT uiToolbarBmpRes = 0, CSize sizeToolbarImage = CSize(0, 0), CSize sizeToolbarButton = CSize(0, 0));
	BOOL IsNavigationToolbarEnabled() const { return m_bUseNavigationToolbar; }

	void EnableAnimation(BOOL bEnable = TRUE) { m_bAnimationEnabled = bEnable; }
	BOOL IsAnimationEnabled() const { return m_bAnimationEnabled; }

	void EnableHistoryMenuButtons(BOOL bEnable = TRUE);
	BOOL IsHistoryMenuButtonsEnabled() const { return m_bHistoryMenuButtons; }

	void EnableWrapTasks(BOOL bEnable = TRUE) { m_bWrapTasks = bEnable; }
	BOOL IsWrapTasksEnabled() const { return m_bWrapTasks; }

	void EnableWrapLabels(BOOL bEnable = TRUE) { m_bWrapLabels = bEnable; }
	BOOL IsWrapLabelsEnabled() const { return m_bWrapLabels; }

// Overrides
public:
	virtual void OnClickTask(int nGroupNumber, int nTaskNumber, UINT uiCommandID, DWORD_PTR dwUserData);

public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual void Serialize(CArchive& ar);
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual CPaneFrameWnd* CreateDefaultMiniframe(CRect rectInitial);

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void ShowCommandMessageString(UINT uiCmdId);

	virtual void Update();
	virtual BOOL IsToolBox() const { return FALSE; }

protected:
	virtual void OnFillBackground(CDC* pDC, CRect rectFill);
	virtual void OnDrawTasks(CDC* pDC, CRect rectWorkArea);
	virtual void OnActivateTasksPanePage() {}
	virtual void SetCaptionButtons();
	virtual void DrawCaption(CDC* pDC, CRect rectCaption);
	virtual void OnPressButtons(UINT nHit);

	virtual void OnTrackCaptionButtons(CPoint point);
	virtual void StopCaptionButtonsTracking();

	virtual BOOL GetMRUFileName(CRecentFileList* pRecentFileList, int nIndex, CString &strName);
	virtual CSize GetTasksGroupBorders() const;

	virtual void ScrollChild(HWND /*hwndTask*/, int /*nScrollValue*/) {}

// Implementation
public:
	virtual ~CMFCTasksPane();

	HMENU CreateMenu() const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg BOOL OnNeedTipText(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg void OnBack();
	afx_msg void OnForward();
	afx_msg void OnHome();
	afx_msg void OnClose();
	afx_msg void OnOther();
	afx_msg void OnUpdateBack(CCmdUI* pCmdUI);
	afx_msg void OnUpdateForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClose(CCmdUI* pCmdUI);
	afx_msg int OnSetText(LPCTSTR lpszText);

	DECLARE_MESSAGE_MAP()

	virtual void DoPaint(CDC* pDC);
	virtual int ReposTasks(BOOL bCalcHeightOnly = FALSE);
	void CreateFonts();
	HFONT SetFont(CDC* pDC);
	void SetScrollSizes();
	void AdjustScroll();
	void RebuildMenu();
	void ChangeActivePage(int nNewPageHistoryIdx, int nOldPageHistoryIdx);
	void SaveHistory(int nPageIdx);
	BOOL CreateNavigationToolbar();
	void UpdateCaption();

	BOOL IsScrollUpAvailable() { return m_nVertScrollOffset > 0; }

	BOOL IsScrollDnAvailable() { return m_nVertScrollOffset <= m_nVertScrollTotal - m_nVertScrollPage && m_nVertScrollTotal > 0; }

	BOOL ForceShowNavToolbar() const
	{
		BOOL bIsAttached = GetParentTabbedPane() != NULL;
		CPaneFrameWnd* pMiniFrame = GetParentMiniFrame(TRUE);
		BOOL bNonTasksPaneMiniFrame = pMiniFrame != NULL && !pMiniFrame->IsKindOf( RUNTIME_CLASS(CMFCTasksPaneFrameWnd));
		return bIsAttached || bNonTasksPaneMiniFrame;
	}

	CMFCTasksPaneTask* TaskHitTest(CPoint pt) const;
	virtual CMFCTasksPaneTaskGroup* GroupCaptionHitTest(CPoint pt) const;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
