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
// CMFCPropertyGridToolTipCtrl window

class CMFCPropertyGridToolTipCtrl : public CWnd
{
	DECLARE_DYNAMIC(CMFCPropertyGridToolTipCtrl)

// Construction
public:
	CMFCPropertyGridToolTipCtrl();

// Attributes
public:
	void SetTextMargin(int nTextMargin) { m_nTextMargin = nTextMargin; }
	void GetLastRect(CRect& rect) const { rect = m_rectLast; }

protected:
	AFX_IMPORT_DATA static CString m_strClassName;

	CString m_strText;
	CRect   m_rectLast;
	int     m_nTextMargin;
	HFONT   m_hFont;
	CWnd*   m_pWndParent;

// Operations
public:
	void Track(CRect rect, const CString& strText);
	void Hide();
	void Deactivate();

// Overrides
public:
	virtual BOOL Create(CWnd* pWndParent = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCPropertyGridToolTipCtrl();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
