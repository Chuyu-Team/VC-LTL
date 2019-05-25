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

#include "afxshellmanager.h"
#include "afxlistctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCShellTreeCtrl;

/*============================================================================*/
// CMFCShellListCtrl window

class CMFCShellListCtrl : public CMFCListCtrl
{
	friend class CMFCShellTreeCtrl;

	DECLARE_DYNAMIC(CMFCShellListCtrl)

// Construction
public:
	CMFCShellListCtrl();

// Attributes
public:
	enum
	{
		AFX_ShellList_ColumnName = 0,
		AFX_ShellList_ColumnSize = 1,
		AFX_ShellList_ColumnType = 2,
		AFX_ShellList_ColumnModified = 3,
	}
	AFX_ShellListColumns;

	const IShellFolder* GetCurrentShellFolder() const { return m_psfCurFolder; }
	LPITEMIDLIST GetCurrentItemIdList() const { return m_pidlCurFQ; }
	BOOL IsDesktop() const { return m_bIsDesktop; }

protected:
	IShellFolder* m_psfCurFolder;
	LPITEMIDLIST  m_pidlCurFQ;
	SHCONTF       m_nTypes;

	HWND m_hwndRelatedTree;
	BOOL m_bContextMenu;
	BOOL m_bIsDesktop;
	BOOL m_bNoNotify;

	AFX_IMPORT_DATA static IContextMenu2* m_pContextMenu2;

// Operations
public:
	BOOL GetItemPath(CString& strPath, int iItem) const;
	BOOL GetCurrentFolder(CString& strPath) const;
	BOOL GetCurrentFolderName(CString& strName) const;

	virtual HRESULT Refresh();
	virtual HRESULT DisplayFolder(LPCTSTR lpszPath);
	virtual HRESULT DisplayFolder(LPAFX_SHELLITEMINFO lpItemInfo);
	virtual HRESULT DisplayParentFolder();

	void SetItemTypes(SHCONTF nTypes);
	SHCONTF GetItemTypes() const { return m_nTypes; }

	void EnableShellContextMenu(BOOL bEnable = TRUE);

// Overrides
	virtual void OnSetColumns();
	virtual CString OnGetItemText(int iItem, int iColumn, LPAFX_SHELLITEMINFO pItem);
	virtual int OnGetItemIcon(int iItem, LPAFX_SHELLITEMINFO pItem);

	virtual void OnFormatFileSize(__int64 lFileSize, CString& str);
	virtual void OnFormatFileDate(const CTime& tmFile, CString& str);

	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);

protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CMFCShellListCtrl();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	HIMAGELIST GetShellImageList(BOOL bLarge);
	HRESULT LockCurrentFolder(LPAFX_SHELLITEMINFO pItemInfo);
	void ReleaseCurrFolder();

	virtual HRESULT EnumObjects(LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent);
	virtual void DoDefault(int iItem);

	BOOL InitList();
	CMFCShellTreeCtrl* GetRelatedTree() const;
};

extern AFX_IMPORT_DATA UINT AFX_WM_CHANGE_CURRENT_FOLDER;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
