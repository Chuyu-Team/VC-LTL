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
#include "afxtoolbarimages.h"
#include "afxtoolbarbutton.h"
#include "afxcmdusagecount.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CCommandManager;

CCommandManager* AFX_CDECL GetCmdMgr();
#define afxCommandManager GetCmdMgr()

class CCommandManager
{
friend class _STATIC_CREATOR_;
protected:
	CCommandManager();

public:
	virtual ~CCommandManager();

	void CleanUp(); // Final cleaning

	// Image functions
	void SetCmdImage(UINT uiCmd, int iImage, BOOL bUserImage);
	int  GetCmdImage(UINT uiCmd, BOOL bUserImage = FALSE) const;
	void ClearCmdImage(UINT uiCmd);
	void ClearUserCmdImages();
	void ClearAllCmdImages();

	void EnableMenuItemImage(UINT uiCmd, BOOL bEnable = TRUE, int nUserImage = -1);
	
	BOOL IsMenuItemWithoutImage(UINT uiCmd) const { return m_lstCommandsWithoutImages.Find(uiCmd) != NULL; }
	int GetMenuUserImage(UINT uiMenuCmd) const
	{
		int iImage = -1;
		m_mapMenuUserImages.Lookup(uiMenuCmd, iImage);

		return iImage;
	}

	BOOL LoadState(LPCTSTR lpszProfileName);
	BOOL SaveState(LPCTSTR lpszProfileName);

protected:
	CMap<UINT, UINT, int, int> m_CommandIndex;
	CMap<UINT, UINT, int, int> m_CommandIndexUser;
	CMap<UINT, UINT, int, int> m_DefaultImages;

	// List of menu items where image appearance is disabled:
	CList<UINT,UINT>           m_lstCommandsWithoutImages;

	// List of menu items with user images:
	CMap<UINT,UINT,int,int>    m_mapMenuUserImages;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
