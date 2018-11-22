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
#include "afxpane.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCBaseToolBar window

class CMFCBaseToolBar : public CPane
{
	DECLARE_DYNCREATE(CMFCBaseToolBar)

// Construction
public:
	CMFCBaseToolBar();

// Attributes
public:
	virtual AFX_DOCK_TYPE GetDockingMode() const { return DT_IMMEDIATE; }

// Operations
public:

	virtual void OnAfterChangeParent(CWnd* pWndOldParent);
	virtual void GetMinSize(CSize& size) const  
	{
		if (IsHorizontal())
		{
			size.cx = 40; size.cy = 16;
		}
		else
		{
			size.cy = 40; size.cx = 16;
		}
	}

// Implementation
public:
	virtual ~CMFCBaseToolBar();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void OnAfterStretch(int nStretchSize);
	virtual BOOL OnShowControlBarMenu(CPoint /*point*/) { return FALSE; }
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
