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

class CMFCToolBar;

class CMenuHash
{
public:
	CMenuHash();
	virtual ~CMenuHash();

	BOOL SaveMenuBar(HMENU hMenu, CMFCToolBar* pBar);
	BOOL LoadMenuBar(HMENU hMenu, CMFCToolBar* pBar);

	BOOL RemoveMenu(HMENU hMenu);
	void CleanUp();

	BOOL IsActive() const
	{
		return m_bIsActive;
	}

protected:
	CMap<HMENU, HMENU&, HANDLE, HANDLE&> m_StoredMenues;

	BOOL m_bIsActive;
};

extern CMenuHash afxMenuHash;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
