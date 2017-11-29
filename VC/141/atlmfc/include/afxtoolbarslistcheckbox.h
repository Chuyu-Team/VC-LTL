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

/*============================================================================*/
// CMFCToolBarsListCheckBox window

class CMFCToolBarsListCheckBox : public CCheckListBox
{
// Construction
public:
	CMFCToolBarsListCheckBox();

// Attributes
protected:
	CArray<BOOL, BOOL> m_arCheckData;

// Operations
public:
	void EnableCheck(int nIndex, BOOL bEnable = TRUE);
	BOOL IsCheckEnabled(int nIndex) const;

// Implementation
public:
	virtual ~CMFCToolBarsListCheckBox();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	LRESULT OnLBAddString(WPARAM wParam, LPARAM lParam);
	LRESULT OnLBInsertString(WPARAM wParam, LPARAM lParam);
	LRESULT OnLBResetContent(WPARAM wParam, LPARAM lParam);
	LRESULT OnLBDeleteString(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void OnNewString(int iIndex);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
