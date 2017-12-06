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

#ifndef STATE_SYSTEM_NORMAL
#define STATE_SYSTEM_NORMAL  0x0000
#endif 

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

// CAccessibilityData

class CAccessibilityData
{
public:
	CAccessibilityData()
	{
		Clear();
	}

	void Clear()
	{
		m_nAccRole = 0;
		m_bAccState = 0;
		m_nAccHit = 0;
		m_rectAccLocation = CRect(0, 0, 0, 0);
		m_ptAccHit = CPoint(0, 0);

		m_strAccName.Empty();
		m_strAccValue.Empty();
		m_strDescription.Empty();
		m_strAccKeys.Empty();
		m_strAccHelp.Empty();
		m_strAccName.Empty();
		m_strAccDefAction.Empty();
	}

	CString m_strAccName;
	CString m_strAccValue;
	CString m_strDescription;
	CString m_strAccKeys;
	CString m_strAccHelp;
	CString m_strAccDefAction;

	int    m_nAccRole;
	UINT   m_bAccState;
	UINT   m_nAccHit;
	CRect  m_rectAccLocation;
	CPoint m_ptAccHit;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
