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
#include "afxoutlookbar.h"
#include "afxoutlookbarpane.h"
#include "afxtabctrl.h"
#include "afxdialogimpl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCPropertySheet;
class CMFCPropertyPage;

/*============================================================================*/
// CMFCOutlookBarPaneList

class CMFCOutlookBarPaneList : public CMFCOutlookBarPane
{
	friend class CMFCPropertySheet;

	virtual BOOL OnSendCommand(const CMFCToolBarButton* pButton);
	void EnsureVisible(int iButton);

	CMFCPropertySheet* m_pParent;
};

class CMFCPropertySheetTabCtrl : public CMFCTabCtrl
{
	friend class CMFCPropertySheet;

	CMFCPropertySheetTabCtrl();

	virtual BOOL SetActiveTab(int iTab);

	CMFCPropertySheet* m_pParent;
};

class CMFCPropertySheetListBox : public CListBox
{
	friend class CMFCPropertySheet;

	CMFCPropertySheetListBox();

	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	DECLARE_MESSAGE_MAP()

	int  m_nHighlightedItem;
	BOOL m_bTracked;
	CMFCPropertySheet* m_pParent;
};

/*============================================================================*/
// CMFCPropertySheetCategoryInfo

class CMFCPropertySheetCategoryInfo : public CObject
{
	DECLARE_DYNAMIC(CMFCPropertySheetCategoryInfo)

	CMFCPropertySheetCategoryInfo(LPCTSTR lpszName, int nIcon, int nSelectedItem, const CMFCPropertySheetCategoryInfo* pParentCategory, CMFCPropertySheet& propSheet);
	virtual ~CMFCPropertySheetCategoryInfo();

	const CString m_strName;
	const int     m_nIcon;
	const int     m_nSelectedIcon;
	HTREEITEM     m_hTreeItem;
	HTREEITEM     m_hLastSelectedItem;

	CMFCPropertySheetCategoryInfo*   m_pParentCategory;
	CMFCPropertySheet& m_propSheet;

	CList<CMFCPropertySheetCategoryInfo*,CMFCPropertySheetCategoryInfo*>    m_lstSubCategories;
	CList<CMFCPropertyPage*, CMFCPropertyPage*> m_lstPages;
};

/*============================================================================*/
// CMFCPropertySheet

class CMFCPropertySheet : public CPropertySheet
{
	friend class CMFCPropertySheetCategoryInfo;

	DECLARE_DYNAMIC(CMFCPropertySheet)

// Construction
public:
	CMFCPropertySheet();
	CMFCPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMFCPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	enum PropSheetLook
	{
		PropSheetLook_Tabs,
		PropSheetLook_OutlookBar,
		PropSheetLook_Tree,
		PropSheetLook_OneNoteTabs,
		PropSheetLook_List,
	};

	// Should be called BEFORE DoModal or Create!
	void SetLook(PropSheetLook look, int nNavControlWidth = 100);
	PropSheetLook GetLook() const { return m_look; }

// Attributes
public:
	CMFCTabCtrl& GetTab() const; // for PropSheetLook_OneNoteTabs only
	int GetHeaderHeight() const { return m_nHeaderHeight; }
	int GetNavBarWidth() const;

protected:
	PropSheetLook  m_look;
	CMFCOutlookBarPaneList m_wndPane1;
	CMFCPropertySheetTabCtrl  m_wndTab;
	CMFCPropertySheetListBox m_wndList;
	CMFCOutlookBar    m_wndOutlookBar;
	CTreeCtrl      m_wndTree;
	CImageList     m_Icons;

	int  m_nBarWidth;
	int  m_nActivePage;
	int  m_nHeaderHeight;
	BOOL m_bAlphaBlendIcons;
	BOOL m_bIsInSelectTree;

	CList<CMFCPropertySheetCategoryInfo*,CMFCPropertySheetCategoryInfo*> m_lstTreeCategories;

	CDialogImpl m_Impl;

// Operations
public:
	BOOL SetIconsList(UINT uiImageListResID, int cx, COLORREF clrTransparent = RGB(255, 0, 255));
	void SetIconsList(HIMAGELIST hIcons);

	void AddPage(CPropertyPage* pPage);

	void RemovePage(CPropertyPage* pPage);
	void RemovePage(int nPage);
	void RemoveCategory(CMFCPropertySheetCategoryInfo* pCategory);

	// PropSheetLook_Tree methods:
	CMFCPropertySheetCategoryInfo* AddTreeCategory(LPCTSTR lpszLabel, int nIconNum = -1, int nSelectedIconNum = -1, const CMFCPropertySheetCategoryInfo* pParentCategory = NULL);
	void AddPageToTree(CMFCPropertySheetCategoryInfo* pCategory, CMFCPropertyPage* pPage, int nIconNum = -1, int nSelIconNum = -1);

	void EnablePageHeader(int nHeaderHeight);

// Overrides
	virtual void OnActivatePage(CPropertyPage* pPage);
	virtual CWnd* InitNavigationControl();
	virtual BOOL OnRemoveTreePage(CPropertyPage* pPage);
	
	virtual void OnDrawPageHeader(CDC* pDC, int nPage, CRect rectHeader);

protected:
	virtual BOOL IsLeftNavigationPane(HWND hWnd) const;
	virtual BOOL CanAddPageToDynamicLayout() const;

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCPropertySheet();

protected:
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnAfterActivatePage(WPARAM,LPARAM);
	afx_msg void OnSelectTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectList();

	DECLARE_MESSAGE_MAP()

	void InternalAddPage(int nTab);
	void AddCategoryToTree(CMFCPropertySheetCategoryInfo* pCategory);
	void CommonInit();
	int FindPageIndexInList(CPropertyPage* pPage);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
