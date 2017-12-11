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
#include "afxacceleratorkey.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCAcceleratorKeyAssignCtrl window

class CMFCAcceleratorKeyAssignCtrl : public CEdit
{
// Construction
public:
	CMFCAcceleratorKeyAssignCtrl();

// Attributes
public:
	BOOL IsKeyDefined() const { return m_bIsDefined; }
	BOOL IsFocused() const { return m_bIsFocused; }
	ACCEL const* GetAccel() const { return &m_Accel; }

protected:
	BOOL  m_bIsDefined;
	ACCEL m_Accel;
	CMFCAcceleratorKey m_Helper;
	BOOL  m_bIsFocused;

// Operations
public:
	void ResetKey();

protected:
	void SetAccelFlag(BYTE bFlag, BOOL bOn);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCAcceleratorKeyAssignCtrl();

protected:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
