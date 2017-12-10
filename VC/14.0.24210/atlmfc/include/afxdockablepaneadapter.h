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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CDockablePaneAdapter window

class CDockablePaneAdapter : public CDockablePane
{
	DECLARE_SERIAL(CDockablePaneAdapter);

// Construction
public:
	CDockablePaneAdapter();

// Attributes
public:
	CRect m_rectInitial;
	DWORD m_dwEnabledAlignmentInitial;

// Operations
public:
	virtual BOOL SetWrappedWnd(CWnd* pWnd);
	virtual CWnd* GetWrappedWnd() const { return m_pWnd; }

	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

// Implementation
public:
	virtual ~CDockablePaneAdapter();

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

protected:
	CWnd* m_pWnd;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
