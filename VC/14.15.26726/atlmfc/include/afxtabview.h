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
#include "afxtabctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CTabView view

class CTabView : public CView
{
protected:
	CTabView(); // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTabView)

// Attributes
public:
	CMFCTabCtrl& GetTabControl() { return m_wndTabs; }

protected:
	CMFCTabCtrl m_wndTabs;
	BOOL    m_bIsReady;
	int     m_nFirstActiveTab;

// Operations
public:
	int AddView(CRuntimeClass* pViewClass, const CString& strViewLabel, int iIndex = -1, CCreateContext* pContext = NULL);
	int FindTab(HWND hWndView) const;

	BOOL RemoveView(int iTabNum);
	BOOL SetActiveView(int iTabNum);
	CView* GetActiveView() const; // active view or NULL

// Overrides
public:
	virtual void OnInitialUpdate();

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

	virtual void OnActivateView(CView* /*pView*/) {}
	virtual BOOL IsScrollBar() const { return FALSE; }

// Implementation
protected:
	virtual ~CTabView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnChangeActiveTab(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
