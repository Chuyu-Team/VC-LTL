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
#include "afxbutton.h"
#include "afxeditbrowsectrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_VSLISTBOX_BTN_NEW    0x0001
#define AFX_VSLISTBOX_BTN_DELETE 0x0002
#define AFX_VSLISTBOX_BTN_UP     0x0004
#define AFX_VSLISTBOX_BTN_DOWN   0x0008

#define AFX_VSLISTBOX_BTN_NEW_ID    (UINT)(-11)
#define AFX_VSLISTBOX_BTN_DELETE_ID (UINT)(-12)
#define AFX_VSLISTBOX_BTN_UP_ID     (UINT)(-13)
#define AFX_VSLISTBOX_BTN_DOWN_ID   (UINT)(-14)

/*============================================================================*/
// CVSListBoxBase window - edit list control abstract base class
// 

class CVSListBoxBase : public CStatic
{
	DECLARE_DYNAMIC(CVSListBoxBase)

// Construction
protected:
	CVSListBoxBase();

	CList<CMFCButton*, CMFCButton*> m_lstButtons;
	CList<DWORD, DWORD>     m_lstKeyAccell;
	CMap<int,int,UINT,UINT> m_mapButtonIDs;
	UINT m_uiStandardBtns;
	BOOL m_bNewItem;
	BOOL m_bIsActualDelete;	// Indicated that Items is really deletd, not moved
	BOOL m_bBrowseButton;
	BOOL m_bGrayDisabledButtons;
	BOOL m_bDefaultCaption;
	CSize m_sizeButton;
	CRect m_rectCaption;
	CFont m_font;
	CString m_strCaption;

// Operations
public:
	BOOL SetStandardButtons(UINT uiBtns = AFX_VSLISTBOX_BTN_NEW | AFX_VSLISTBOX_BTN_DELETE | AFX_VSLISTBOX_BTN_UP | AFX_VSLISTBOX_BTN_DOWN);
	BOOL AddButton(UINT uiImageResId, LPCTSTR lpszTooltip = NULL, WORD wKeyAccelerator = 0, BYTE fVirt = 0, UINT uiButtonID = 0);
	BOOL EnableButton(int iButtonNum, BOOL bEnable = TRUE);

	int GetButtonsCount() const { return(int) m_lstButtons.GetCount(); }
	BOOL IsCreatingNewItem() const { return m_bNewItem; }

	UINT GetButtonID(int iButtonNum) const;
	int GetButtonNum(UINT uiID) const;

	// Enable/disable "Browse" button (appear right of edit control)
	void EnableBrowseButton(BOOL bEnable = TRUE);
	BOOL IsBrowseButton() const { return m_bBrowseButton; }

	// Allow to draw disabled button grayed. By default, the disabled button will be drawn with the regular look
	void SetGrayDisabledButtons(BOOL bOn = TRUE);
	BOOL IsGrayDisabledButtons() const { return m_bGrayDisabledButtons; }

	// These methods should be implemented in the actual edit list:
	virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1) = 0;
	virtual BOOL RemoveItem(int iIndex) = 0;
	virtual int GetCount() const = 0;
	virtual int GetSelItem() const = 0;
	virtual BOOL SelectItem(int iItem) = 0;
	virtual CString GetItemText(int iIndex) const = 0;
	virtual void SetItemText(int iIndex, const CString& strText) = 0;
	virtual DWORD_PTR GetItemData(int iIndex) const = 0;
	virtual void SetItemData(int iIndex, DWORD_PTR dwData) = 0;
	virtual BOOL EditItem(int iIndex) = 0;

protected:
	void Init();
	int GetStdButtonNum(UINT uiStdBtn) const;
	void AdjustLayout();

// Overrides
public:
	virtual void OnClickButton(int iButton);
	virtual void OnKey(WORD wKey, BYTE fFlags);
	virtual void OnEndEditLabel(LPCTSTR lpszLabel);
	virtual int OnGetImage(LV_ITEM* /*pItem*/) { return -1; }
	
	virtual void OnSelectionChanged() {}

	// "Standard" action overrides
	virtual BOOL OnBeforeRemoveItem(int /*iItem*/) { return TRUE; }
	virtual void OnAfterAddItem(int /*iItem*/) {}
	virtual void OnAfterRenameItem(int /*iItem*/) {}
	virtual void OnAfterMoveItemUp(int /*iItem*/) {}
	virtual void OnAfterMoveItemDown(int /*iItem*/) {}

	virtual void OnBrowse() {}

protected:
	virtual void CreateNewItem();

	virtual HWND GetListHwnd() const = 0;
	virtual CWnd* OnCreateList() = 0;
	virtual void OnSizeList() = 0;

	virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CVSListBoxBase();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg HFONT OnGetFont();
	afx_msg int OnSetText(LPCTSTR lpszText);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CVSListBoxEditCtrl window

class CVSListBoxEditCtrl : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(CVSListBoxEditCtrl)

// Construction
public:
	CVSListBoxEditCtrl();

// Attributes
protected:
	BOOL m_bLocked;
	CVSListBoxBase* m_pParentList;

// Operations
public:
	void LockSize(CVSListBoxBase* pParent, BOOL bLock = TRUE);

// Implementation
public:
	virtual ~CVSListBoxEditCtrl();

protected:
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	virtual void OnBrowse()
	{
		if (m_pParentList != NULL)
		{
			HWND hwndParentList = m_pParentList->GetSafeHwnd();
			m_pParentList->OnBrowse();

			if (::IsWindow(hwndParentList))
			{
				::SetFocus(hwndParentList);
			}
		}
	}
};

/*============================================================================*/
// CVSListBox window

class CVSListBox : public CVSListBoxBase
{
	DECLARE_DYNAMIC(CVSListBox)

// Construction
public:
	CVSListBox();

// Attributes
public:
	CListCtrl* m_pWndList; // Embedded list control

protected:
	CPoint        m_ptClick;
	CVSListBoxEditCtrl m_wndEdit;

// Operations
public:
	virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1);
	virtual BOOL RemoveItem(int iIndex);
	virtual int GetCount() const;

	virtual int GetSelItem() const;
	virtual BOOL SelectItem(int iItem);

	virtual CString GetItemText(int iIndex) const;
	virtual DWORD_PTR GetItemData(int iIndex) const;
	virtual void SetItemData(int iIndex, DWORD_PTR dwData);

	virtual BOOL EditItem(int iIndex);

protected:
	virtual HWND GetListHwnd() const { return m_pWndList->GetSafeHwnd(); }

	virtual CWnd* OnCreateList();
	virtual void OnSizeList();
	virtual void SetItemText(int iIndex, const CString& strText);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CVSListBox();

protected:
	afx_msg void OnKeyDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
