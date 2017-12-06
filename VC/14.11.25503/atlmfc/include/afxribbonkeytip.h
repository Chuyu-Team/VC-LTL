// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCRibbonKeyTip window

class CMFCRibbonBaseElement;

class CMFCRibbonKeyTip : public CWnd
{
// Construction
public:
	CMFCRibbonKeyTip(CMFCRibbonBaseElement* pElement, BOOL bIsMenu = FALSE);

// Attributes
public:
	CMFCRibbonBaseElement* GetElement() const { return m_pElement; }
	BOOL IsMenuKey() const { return m_bIsMenu; }

protected:
	CMFCRibbonBaseElement* m_pElement;
	CRect m_rectScreen;
	BOOL  m_bIsMenu;

	AFX_IMPORT_DATA static CString m_strClassName;

// Operations
public:
	BOOL Show(BOOL bRepos);
	void Hide();

protected:
	void UpdateMenuShadow();

// Implementation
public:
	virtual ~CMFCRibbonKeyTip();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
