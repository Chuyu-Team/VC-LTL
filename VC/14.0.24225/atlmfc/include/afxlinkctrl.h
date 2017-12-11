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

#include "afxbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCLinkCtrl window

class CMFCLinkCtrl : public CMFCButton
{
	DECLARE_DYNAMIC(CMFCLinkCtrl)

// Construction
public:
	CMFCLinkCtrl();

// Attributes
public:
	BOOL m_bMultilineText;
	BOOL m_bAlwaysUnderlineText;
	BOOL m_bDefaultClickProcess;
	BOOL m_bVisited;

protected:
	CString m_strURL;
	CString m_strPrefix;

// Operations
public:
	void SetURL(LPCTSTR lpszURL);          // By default, window text will be used
	void SetURLPrefix(LPCTSTR lpszPrefix); // For example "mailto:"

	virtual CSize SizeToContent(BOOL bVCenter = FALSE, BOOL bHCenter = FALSE);

protected:
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
	virtual void OnDrawFocusRect(CDC* pDC, const CRect& rectClient);

// Overrides
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCLinkCtrl();

protected:
	afx_msg BOOL OnClicked();
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
