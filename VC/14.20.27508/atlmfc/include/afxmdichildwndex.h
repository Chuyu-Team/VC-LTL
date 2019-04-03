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
#include "afxframeimpl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMDIFrameWndEx;
class CMDIChildWndEx;

class CMDITabProxyWnd : public CWnd
{
	DECLARE_DYNCREATE(CMDITabProxyWnd)
public:
	CMDITabProxyWnd();
	virtual ~CMDITabProxyWnd();

	void SetRelatedMDIChildFrame(CMDIChildWndEx* pRelatedMDIFrame);
	HBITMAP GetClientBitmap(int nWidth, int nHeight, BOOL bIsThumbnail);

protected:
	afx_msg BOOL OnSendIconicThumbnail(int cx, int cy);
	afx_msg BOOL OnSendIconicLivePreviewBitmap();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	BOOL IsMDIChildActive();

	CMDIChildWndEx* m_pRelatedMDIChildFrame;
};
/*============================================================================*/
// CMDIChildWndEx frame

class CMDIChildWndEx : public CMDIChildWnd
{
	friend class CMDIClientAreaWnd;
	friend class CMDIFrameWndEx;
	friend class CMDITabProxyWnd;

	DECLARE_DYNCREATE(CMDIChildWndEx)
protected:
	CMDIChildWndEx(); // protected constructor used by dynamic creation

// Attributes
public:
	AFX_IMPORT_DATA static BOOL  m_bEnableFloatingBars;
	AFX_IMPORT_DATA static DWORD m_dwExcludeStyle;
	/// <summary>
	/// A combination of flags, which is passed by the framework to the SetTaskbarTabProperties method,
	/// when a tab (MDI child) is being registered with Windows 7 taskbar tabs.
	/// The default combination is STPF_USEAPPTHUMBNAILWHENACTIVE | STPF_USEAPPPEEKWHENACTIVE.</summary>
	AFX_IMPORT_DATA static DWORD m_dwDefaultTaskbarTabPropertyFlags;

	BOOL IsTabbedPane() const { return m_pTabbedControlBar != NULL; }
	CDockablePane* GetTabbedPane() const { return m_pTabbedControlBar; }

	CMDIFrameWndEx* GetMDIFrameWndEx() const { return m_pMDIFrame; }

protected:
	// ---- MDITabGroup+
	CMFCTabCtrl* m_pRelatedTabGroup;
	// ---- MDITabGroup-

	BOOL  m_bToBeDestroyed;
	BOOL  m_bWasMaximized;
	BOOL  m_bIsMinimized;
	CRect m_rectOriginal;
	// set during OnMDIActivate to prevent unnecessary 
	// RecalcLayout in CMDIFrameWnd in OLE InPlace mode
	BOOL  m_bActivating;
	BOOL  m_bInOnCreate;

	CFrameImpl      m_Impl;
	CDockingManager m_dockManager;
	CDockablePane*  m_pTabbedControlBar;
	CMDIFrameWndEx* m_pMDIFrame;

	CMDITabProxyWnd m_tabProxyWnd;
	BOOL            m_bTabRegistered;
	BOOL            m_bEnableTaskbarThumbnailClip;

// Operations
public:
	// ---- MDITabGroup+
	CMFCTabCtrl* GetRelatedTabGroup() { return m_pRelatedTabGroup; }
	void SetRelatedTabGroup(CMFCTabCtrl* p) { m_pRelatedTabGroup = p; }
	// ---- MDITabGroup-

	void AddDockSite();
	BOOL AddPane(CBasePane* pControlBar, BOOL bTail = TRUE);
	BOOL InsertPane(CBasePane* pControlBar, CBasePane* pTarget, BOOL bAfter = TRUE);
	void RemovePaneFromDockManager(CBasePane* pControlBar, BOOL bDestroy, BOOL bAdjustLayout, BOOL bAutoHide, CBasePane* pBarReplacement);
	void DockPane(CBasePane* pBar, UINT nDockBarID = 0, LPCRECT lpRect = NULL);

	void AddTabbedPane(CDockablePane* pControlBar);

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

	virtual BOOL GetToolbarButtonToolTipText(CMFCToolBarButton* /*pButton*/, CString& /*strTTText*/) { return FALSE; }

	BOOL DockPaneLeftOf(CPane* pBar, CPane* pLeftOf);

	/// <summary>
	/// Registers MDI child with Windows 7 taskbar tabs.</summary>
	/// <param name="pWndBefore">A pointer to MDI child window whose thumbnail is inserted to the left of. 
	/// This window must already be registered through RegisterTaskbarTab. If this value is NULL, the new thumbnail is added to the end of the list.</param>
	virtual void RegisterTaskbarTab(CMDIChildWndEx* pWndBefore = NULL);

	/// <summary>
	/// Returns TRUE if MDI child was successfully registered with Windows 7 taskbar tabs.</summary>
	/// <returns>
	/// TRUE if MDI child is registered with Windows 7 taskbar tabs; otherwise FALSE.</returns>
	BOOL IsRegisteredWithTaskbarTabs();

	/// <summary>
	/// Invalidates iconic bitmap representation of MDI child.</summary>
	/// <remarks> Should be called when the live content or size of MDI child has changed</remarks>
	/// <returns> Returns FALSE is Windows 7 taskbar support is disabled or the MDI child
	/// is not registered with Windows 7 taskbar tabs; otherwise returns TRUE.</returns>
	BOOL InvalidateIconicBitmaps();

	/// <summary>
	/// Tells whether the MDI child can appear on Windows 7 taskbar tabs.</summary>
	/// <remarks> Returns TRUE if MDI child can show on Windows 7 taskbar tabs.</remarks>
	/// <returns> Returns TRUE if MDI child can show on Windows 7 taskbar tabs, 
	/// or FALSE if MDI child can't be shown on Windows 7 taskbar tabs.</returns>
	BOOL IsTaskbarTabsSupportEnabled();

	/// <summary>
	/// Inserts MDI child before specified window on Windows 7 taskbar tabs.</summary>
	/// <param name="pWndBefore">A pointer to MDI child window whose thumbnail is inserted to the left of. 
	/// This window must already be registered through RegisterTaskbarTab. If this value is NULL, the new thumbnail is added to the end of the list.</param>
	void SetTaskbarTabOrder(CMDIChildWndEx* pWndBefore = NULL);

	/// <summary>
	/// Activates corresponding Windows 7 taskbar tab.</summary>
	void SetTaskbarTabActive();

	/// <summary>
	/// Sets properties for a Windows 7 taskbar tab.</summary>
	/// <param name="dwFlags">A combination of STPFLAG values. See ITaskbarList4::SetTabProperties for details.</param>
	void SetTaskbarTabProperties(DWORD dwFlags);

	/// <summary> 
	/// Removes MDI child from Windows 7 taskbar tabs.</summary>
	/// <param name="bCheckRegisteredMDIChildCount">Specifies whether this function needs to check the number of MDI children registered with MDI tabs.
	/// If this number is 0, then this function removes clipping rectangle from application's taskbar thumbnail.</param>
	void UnregisterTaskbarTab(BOOL bCheckRegisteredMDIChildCount = TRUE);

	/// <summary> 
	/// Returns tab proxy window actually registered with Windows 7 taskbar tabs.</summary>
	/// <returns> A pointer to CMDITabProxyWnd object, which is registered with Windows 7 taskbar tabs.</returns>
	CMDITabProxyWnd* GetTabProxyWnd() { return &m_tabProxyWnd;}

	/// <summary> 
	/// Enables or disables automatic selection of a portion of a window's client area to display 
	/// as that window's thumbnail in the taskbar. </summary>
	/// <param name="bEnable"> Specifies whether to enable (TRUE), or disable (FALSE) automatic selection of a portion of a window's client area to display.</param>
	void EnableTaskbarThumbnailClipRect(BOOL bEnable = TRUE);

	/// <summary> 
	/// Tells whether automatic selection of a portion of a window's client area to display 
	/// as that window's thumbnail in the taskbar is enabled or disabled. </summary>
	/// <returns>
	/// Returns TRUE if automatic selection of a portion of a window's client area to display is enabled; otherwise FALSE.</returns>
	BOOL IsTaskbarThumbnailClipRectEnabled() const { return m_bEnableTaskbarThumbnailClip; }

// Overrides

	// Next methods used by MDI tabs:
	virtual CString GetFrameText() const;
	virtual HICON GetFrameIcon() const;

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	virtual BOOL CanShowOnMDITabs();
	virtual BOOL CanShowOnWindowsList() { return TRUE; }
	virtual BOOL IsReadOnly() { return FALSE; }
	virtual BOOL IsTabbedMDIChild();

	/// <summary>
	/// Updates Windows 7 taskbar tab icon.</summary>
	/// <param name="hIcon"> A handle to an icon to display on Windows 7 taskbar tab.</param>
	virtual void UpdateTaskbarTabIcon(HICON hIcon);
	/// <summary> 
	/// Tells the framework whether this MDI child can be displayed on Windows 7 taskbar tabs.</summary>
	/// <remarks>
	/// Override this method in a derived class and return FALSE to disable appearance of this MDI child
	/// on Windows 7 taskbar tabs.</remarks>
	/// <returns>
	/// Should return TRUE if the content of MDI child can be displayed on Windows 7 taskbar thumbnails.</returns>
	virtual BOOL CanShowOnTaskBarTabs() { return TRUE; }

	/// <summary> 
	/// Called by the framework when it needs to obtain a bitmap for iconic thumbnail of MDI child.</summary>
	/// <returns> 
	/// If processed, should return a handle to a valid 32bpp bitmap, otherwise NULL.</returns>
	/// <remarks>
	/// Override this method in a derived class an return a valid 32bpp bitmap for iconic thumbnail of MDI child.
	/// This method is called only when MDI child is displayed on Windows 7 taskbar tabs.
	/// If you return NULL, MFC calls default handlers and obtains bitmaps using PrintClient.</remarks>
	/// <param name="nWidth"> Specifies the width of required bitmap.</param>
	/// <param name="nHeight"> Specifies the height of required bitmap.</param>
	virtual HBITMAP OnGetIconicThumbnail(int nWidth, int nHeight) 
	{ 
		UNREFERENCED_PARAMETER(nWidth);
		UNREFERENCED_PARAMETER(nHeight);
		return NULL; 
	}

	/// <summary> 
	/// Called by the framework when it needs to obtain a bitmap for live preview of MDI child.</summary>
	/// <returns> 
	/// If processed, should return a handle to a valid 32bpp bitmap, otherwise NULL.</returns>
	/// <remarks>
	/// Override this method in a derived class an return a valid 32bpp bitmap for live preview of MDI child.
	/// This method is called only when MDI child is displayed on Windows 7 taskbar tabs.
	/// If you return NULL, MFC calls default handlers and obtains bitmaps using PrintClient or PrintWindow.</remarks>
	/// <param name="bIsMDIChildActive"> This parameter is TRUE if the bitmap is requested for MDI child, which is currently active and the main window is not minimized. 
	/// The default processing in this case takes a snapshot of the main window.</param>
	/// <param name="ptLocation"> Specifies the location of bitmap in the main (top level) window client coordinated. This point should be provided by callee.</param>
	virtual HBITMAP OnGetIconicLivePreviewBitmap(BOOL bIsMDIChildActive, CPoint& ptLocation) 
	{
		UNREFERENCED_PARAMETER(bIsMDIChildActive);
		UNREFERENCED_PARAMETER(ptLocation);
		return NULL; 
	}

	/// <summary> 
	/// Called by the framework when it needs to stretch a bitmap for Windows 7 taskbar tab thumbnail preview of MDI child.</summary>
	/// <returns>Should return TRUE if overridden and processed in a derived class.</returns> 
	/// <param name="hBmpDst">A handle to a destination bitmap.</param>
	/// <param name="rectDst">Specifies the destination rectangle.</param>
	/// <param name="hBmpSrc">A handle to a source bitmap.</param>
	/// <param name="rectSrc">Specifies the source rectangle.</param>
	virtual BOOL OnTaskbarTabThumbnailStretch(HBITMAP hBmpDst, const CRect& rectDst, HBITMAP hBmpSrc, const CRect& rectSrc);

	/// <summary> 
	/// Called by the framework when it needs to obtain a child window (usually a view or splitter window) to be displayed on Windows 7 taskbar tab thumbnail.</summary>
	/// <returns>
	/// Should return a valid pointer to a CWnd object, whose preview should be displayed on 
	/// Windows 7 taskbar tab related to this MDI child. The default implementation returns
	/// a child window of this MDI child with AFX_IDW_PANE_FIRST control ID (which is usually a CView-derived class).</returns>
	virtual CWnd* GetTaskbarPreviewWnd();

	/// <summary> 
	/// Called by the framework when the user presses close button on Taskbar tab thumbnail..</summary>
	virtual void OnPressTaskbarThmbnailCloseButton();

	/// <summary> 
	/// Called by the framework when the Taskbar tab thumbnail should process WM_ACTIVATE message.</summary>
	/// <remarks>
	/// The default implementation activates related MDI child frame. </remarks>
	/// <param name="nState">Specifies whether the CWnd is being activated or deactivated.</param>
	/// <param name="pWndOther">Pointer to the CWnd being activated or deactivated. The pointer can be NULL, and it may be temporary.</param>
	/// <param name="bMinimized">Specifies the minimized state of the CWnd being activated or deactivated. A value of TRUE indicates the window is minimized.</param>
	virtual void OnTaskbarTabThumbnailActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

	/// <summary> 
	/// Called by the framework when the Taskbar tab thumbnail should process WM_MOUSEACTIVATE message.</summary>
	/// <remarks>
	/// The default implementation activates related MDI child frame. </remarks>
	/// <param name="pDesktopWnd">Specifies a pointer to the top-level parent window of the window being activated. The pointer may be temporary and should not be stored.</param>
	/// <param name="nHitTest">Specifies the hit-test area code. A hit test is a test that determines the location of the cursor.</param>
	/// <param name="message">Specifies the mouse message number.</param>
	virtual int OnTaskbarTabThumbnailMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	/// <summary> 
	/// Called by the framework when it needs to select a portion of a window's client area to display 
	/// as that window's thumbnail in the taskbar.</summary>
	/// <returns> A rectangle in windows coordinates. This rectangle will be mapped to the client area of top level frame. 
	/// The rectangle should be empty to clear the clipping rectangle</returns>
	virtual CRect GetTaskbarThumbnailClipRect() const;

	CDockingManager* GetDockingManager() { return &m_dockManager; }

	virtual LPCTSTR GetDocumentName(CObject** pObj);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	/// <summary>
	/// Called internally by the framework to set clipping rectangle to select a portion of a window's client area to display 
	/// as that window's thumbnail in the taskbar.</summary>
	/// <param name="rect">Specifies the new clipping rectangle. If rectangle is empty or null, the clipping is removed.</param>
	/// <returns>Returns TRUE if the functions succeeds; otherwise FALSE.</returns>
	virtual BOOL SetTaskbarThumbnailClipRect(CRect rect);

// Implementation
protected:
	virtual ~CMDIChildWndEx();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnSetText(LPCTSTR lpszText);
	afx_msg HICON OnSetIcon(BOOL bIsLarge, HICON hIcon);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnChangeVisualManager(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
#if (NTDDI_VERSION >= NTDDI_WIN7)
	afx_msg void OnSendIconicThumbnail(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSendIconicLivePreviewBitmap(WPARAM wParam, LPARAM lParam);
#endif

	DECLARE_MESSAGE_MAP()

	void AdjustClientArea();
	/// <summary>
	/// Called internally by the framework to activate top level frame when the application should be activated 
	/// from a taskbar tab.</summary>
	virtual void ActivateTopLevelFrame();

	/// <summary>
	/// Called internally by the framework to set text of taskbar tab.</summary>
	/// <param name="lpcszDefaultText"> Specifies a text to set. Current implementation adds this text to application name if the MDI child has FWS_ADDTOTITLE style. </param>
	virtual void SetTaskbarTabText(LPCTSTR lpcszDefaultText);
};

extern AFX_IMPORT_DATA UINT AFX_WM_AFTER_TASKBAR_ACTIVATE;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
