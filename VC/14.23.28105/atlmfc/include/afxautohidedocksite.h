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
#include "afxdocksite.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCAutoHideBar;

/*============================================================================*/
// CAutoHideDockSite window

class CAutoHideDockSite : public CDockSite
{
	DECLARE_DYNCREATE(CAutoHideDockSite)
// Construction
public:
	CAutoHideDockSite();

// Attributes
public:
	void SetOffsetLeft(int nOffset) { m_nOffsetLeft = nOffset; }
	void SetOffsetRight(int nOffset) { m_nOffsetRight = nOffset; }

	void GetAlignRect(CRect& rect) const;
	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;

// Operations
public:
	virtual void DockPane(CPane* pWnd, AFX_DOCK_METHOD dockMethod, LPCRECT lpRect = NULL);
	virtual void RepositionPanes(CRect& rectNewClientArea);

	void UnSetAutoHideMode(CMFCAutoHideBar* pAutohideToolbar);

	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }

// Implementation
public:
	virtual ~CAutoHideDockSite();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	int m_nOffsetLeft;
	int m_nOffsetRight;

	AFX_IMPORT_DATA static int m_nExtraSpace;
};

/*============================================================================*/

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
