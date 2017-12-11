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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CPane;

class CMFCControlBarImpl
{
	friend class CMFCToolBar;
	friend class CMFCReBar;

public:
	CMFCControlBarImpl(CPane* pBar);
	virtual ~CMFCControlBarImpl();

	void GetGripperRect(CRect& rectGripper, BOOL bClientCoord = FALSE);

// Attributes:
protected:
	CPane* m_pBar;

// Operations:
protected:
	void CalcNcSize(NCCALCSIZE_PARAMS FAR* lpncsp);
	void DrawNcArea();
	BOOL GetBackgroundFromParent(CDC* pDC);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
