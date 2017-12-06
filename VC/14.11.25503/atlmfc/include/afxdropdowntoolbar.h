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
#include "afxpopupmenubar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCDropDownToolbarButton;
class CMFCMenuBar;
class CMFCToolBarMenuButton;

class CMFCDropDownToolBar : public CMFCToolBar
{
	friend class CMFCDropDownToolbarButton;
	friend class CMFCDropDownFrame;

	DECLARE_SERIAL(CMFCDropDownToolBar)

public:
	CMFCDropDownToolBar() { m_bLocked = TRUE; }
	virtual ~CMFCDropDownToolBar() {}

	virtual BOOL OnSendCommand(const CMFCToolBarButton* pButton);

	virtual BOOL LoadBitmap(UINT uiResID, UINT uiColdResID = 0, UINT uiMenuResID = 0, BOOL /*bLocked*/ = FALSE, UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0)
	{
		return CMFCToolBar::LoadBitmap(uiResID, uiColdResID, uiMenuResID, TRUE /* Locked */, uiDisabledResID, uiMenuDisabledResID);
	}

	virtual BOOL LoadToolBar(UINT uiResID, UINT uiColdResID = 0, UINT uiMenuResID = 0, BOOL /*bLocked*/ = FALSE, UINT uiDisabledResID = 0, UINT uiMenuDisabledResID = 0, UINT uiHotResID = 0)
	{
		return CMFCToolBar::LoadToolBar(uiResID, uiColdResID, uiMenuResID, /*bLocked*/ TRUE, uiDisabledResID, uiMenuDisabledResID, uiHotResID);
	}

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CMFCDropDownFrame frame

class CMFCDropDownFrame : public CMiniFrameWnd
{
	friend CMFCDropDownToolbarButton;
	friend CMFCDropDownToolBar;

	DECLARE_SERIAL(CMFCDropDownFrame)

public:
	CMFCDropDownFrame();
	virtual ~CMFCDropDownFrame();

// Attributes
public:
	BOOL m_bAutoDestroyParent;

	void SetAutoDestroy(BOOL bAutoDestroy = TRUE) { m_bAutoDestroy = bAutoDestroy; }

protected:
	CMFCDropDownToolbarButton* m_pParentBtn;
	CMFCDropDownToolBar        m_wndToolBar;
	CMFCDropDownToolBar*       m_pWndOriginToolbar;

	int  m_x;
	int  m_y;
	BOOL m_bAutoDestroy;
	CString m_strCaption;

protected:
	AFX_IMPORT_DATA static CString m_strClassName;

// Operations
public:
	CMFCDropDownFrame* GetParentPopupMenu() const;
	CMFCMenuBar* GetParentMenuBar() const;

// Overrides
public:
	virtual BOOL Create(CWnd* pWndParent, int x, int y, CMFCDropDownToolBar* pWndOriginToolbar);
	virtual void RecalcLayout(BOOL bNotify = TRUE);

protected:
	virtual void PostNcDestroy();

// Implementation
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);

	DECLARE_MESSAGE_MAP()
};

class CMFCDropDownToolbarButton : public CMFCToolBarButton
{
	friend class CMFCDropDownFrame;

	DECLARE_SERIAL(CMFCDropDownToolbarButton)

public:
	CMFCDropDownToolbarButton();
	CMFCDropDownToolbarButton(LPCTSTR lpszName, CMFCDropDownToolBar* pToolBar);
	virtual ~CMFCDropDownToolbarButton();

// Overrides:
public:
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual BOOL OnClick(CWnd* pWnd, BOOL bDelay = TRUE);
	virtual BOOL OnClickUp();
	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual void OnCancelMode();
	virtual BOOL OnContextHelp(CWnd* pWnd) { return OnClick(pWnd, FALSE); }

	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL ExportToMenuButton(CMFCToolBarMenuButton& menuButton) const;

	void SetDefaultCommand(UINT uiCmd);
	BOOL DropDownToolbar(CWnd* pWnd);

	virtual BOOL IsExtraSize() const { return m_bExtraSize && !m_bInternalDraw; }
	virtual BOOL OnCustomizeMenu(CMenu* pMenu);

// Attributes:
public:
	BOOL IsDropDown() const { return m_pPopupMenu != NULL; }
	CMFCToolBar* GetDropDownToolBar() const { return m_pToolBar; }

	AFX_IMPORT_DATA static UINT m_uiShowBarDelay;

protected:
	CMFCDropDownToolBar* m_pToolBar;
	CString           m_strName;
	CMFCDropDownFrame*   m_pPopupMenu;

	UINT m_uiTimer;
	int  m_iSelectedImage;
	BOOL m_bInternalDraw;
	BOOL m_bLocalUserButton;

#ifdef _DEBUG

// diagnostics:
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

#endif
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
