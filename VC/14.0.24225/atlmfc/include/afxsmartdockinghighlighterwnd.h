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

/*============================================================================*/
// CSmartDockingHighlighterWnd window

class CSmartDockingHighlighterWnd : public CWnd
{
	friend class CSmartDockingManager;

	// Construction
public:
	CSmartDockingHighlighterWnd();
	void Create(CWnd* pwndOwner);

	void SetDockingWnd(CWnd* pDockingWnd)
	{
		m_pDockingWnd = pDockingWnd;
	}

	void ShowAt(CRect rect);
	void ShowTabbedAt(CRect rect, CRect rectTab);
	void Hide();

	// Attributes
protected:
	CWnd* m_pWndOwner;
	CWnd* m_pDockingWnd;
	CRect m_rectLast;
	CRect m_rectTab;
	BOOL  m_bTabbed;
	BOOL  m_bShown;
	BOOL  m_bUseThemeColorInShading;

	// Implementation
public:
	virtual ~CSmartDockingHighlighterWnd();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
