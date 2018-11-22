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

#ifndef _SHLOBJ_H_
#include <ShlObj.h>
#endif

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

typedef struct _AFX_SHELLITEMINFO
{
	LPSHELLFOLDER  pParentFolder;
	LPITEMIDLIST   pidlFQ;
	LPITEMIDLIST   pidlRel;

	_AFX_SHELLITEMINFO()
	{
		pParentFolder = NULL;
		pidlFQ = NULL;
		pidlRel = NULL;
	}
}
AFX_SHELLITEMINFO, FAR *LPAFX_SHELLITEMINFO;

class CShellManager : public CObject
{
	friend class CMFCShellListCtrl;

// Construction
public:
	CShellManager();
	virtual ~CShellManager();

// Operations
public:
	BOOL BrowseForFolder(CString& strOutFolder, CWnd* pWndParent = NULL, LPCTSTR lplszInitialFolder = NULL,
		LPCTSTR lpszTitle = NULL, UINT ulFlags = BIF_RETURNONLYFSDIRS, LPINT piFolderImage = NULL);

	LPITEMIDLIST CreateItem(UINT cbSize);
	void FreeItem(LPITEMIDLIST pidl);

	UINT GetItemCount(LPCITEMIDLIST pidl);
	UINT GetItemSize(LPCITEMIDLIST pidl);

	LPITEMIDLIST ConcatenateItem(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
	LPITEMIDLIST CopyItem(LPCITEMIDLIST pidlSource);

	LPITEMIDLIST GetNextItem(LPCITEMIDLIST pidl);
	int GetParentItem(LPCITEMIDLIST lpidl, LPITEMIDLIST& lpidlParent);

	HRESULT ItemFromPath(LPCTSTR lpszPath, LPITEMIDLIST& pidl);

protected:
	static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

// Attributes
protected:
	LPMALLOC m_pMalloc;
	LPCTSTR  m_lpszInitialPath; // Valid in BrowseForFolder only
};

AFX_IMPORT_DATA extern CShellManager* afxShellManager;

extern AFX_IMPORT_DATA UINT AFX_WM_ON_AFTER_SHELL_COMMAND;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
