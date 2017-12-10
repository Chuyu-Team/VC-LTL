// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#include "afxcontrolbarutil.h"
#include "afxtoolbarbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarMenuButton;
class CMFCToolBarComboBoxEdit;

class CMFCToolBarComboBoxButton : public CMFCToolBarButton
{
	friend class CMFCToolBarComboBoxEdit;
	friend class CMFCRibbonComboBox;

	DECLARE_SERIAL(CMFCToolBarComboBoxButton)

public:
	CMFCToolBarComboBoxButton();
	CMFCToolBarComboBoxButton(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWNLIST, int iWidth = 0);
	virtual ~CMFCToolBarComboBoxButton();

// Operations:
	virtual INT_PTR AddItem(LPCTSTR lpszItem, DWORD_PTR dwData = 0);
	virtual INT_PTR AddSortedItem(LPCTSTR lpszItem, DWORD_PTR dwData = 0);

	INT_PTR GetCount() const;
	LPCTSTR GetItem(int iIndex = -1) const;
	DWORD_PTR GetItemData(int iIndex = -1) const;
	int GetCurSel() const { return m_iSelIndex; }
	void RemoveAllItems();
	BOOL SelectItem(int iIndex, BOOL bNotify = TRUE);
	BOOL SelectItem(DWORD_PTR dwData);
	BOOL SelectItem(LPCTSTR lpszText);

	BOOL DeleteItem(int iIndex);
	BOOL DeleteItem(DWORD_PTR dwData);
	BOOL DeleteItem(LPCTSTR lpszText);

	int FindItem(LPCTSTR lpszText) const;
	void SetDropDownHeight(int nHeight);

// Overrides:
	virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
	virtual CMFCToolBarComboBoxEdit* CreateEdit(CWnd* pWndParent, const CRect& rect, DWORD dwEditStyle);

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick(CWnd* pWnd, BOOL bDelay = TRUE);
	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual void OnMove();
	virtual void OnSize(int iSize);
	virtual HWND GetHwnd() { return m_pWndCombo->GetSafeHwnd(); }
	virtual CEdit* GetEditCtrl() { return m_pWndEdit; }

	virtual void EnableWindow(BOOL bEnable = TRUE)
	{
		if (m_pWndCombo->GetSafeHwnd() != NULL)
		{
			m_pWndCombo->EnableWindow(bEnable);
		}

		if (m_pWndEdit->GetSafeHwnd() != NULL)
		{
			m_pWndEdit->EnableWindow(bEnable);
		}
	}

	virtual BOOL IsWindowVisible()
	{
		return ((m_pWndCombo->GetSafeHwnd() != NULL && m_pWndCombo->GetStyle() & WS_VISIBLE) || (m_pWndEdit->GetSafeHwnd() != NULL && m_pWndEdit->GetStyle() & WS_VISIBLE));
	}

	virtual BOOL IsOwnerOf(HWND hwnd)
	{
		if (m_pWndCombo->GetSafeHwnd() != NULL && (m_pWndCombo->GetSafeHwnd() == hwnd || ::IsChild(m_pWndCombo->GetSafeHwnd(), hwnd)))
		{
			return TRUE;
		}

		if (m_pWndEdit->GetSafeHwnd() != NULL && (m_pWndEdit->GetSafeHwnd() == hwnd || ::IsChild(m_pWndEdit->GetSafeHwnd(), hwnd)))
		{
			return TRUE;
		}

		return FALSE;
	}

	virtual BOOL NotifyCommand(int iNotifyCode);
	
	virtual BOOL CanBeStretched() const { return TRUE; }
	virtual void OnAddToCustomizePage();
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);
	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);

	virtual void DuplicateData() {}
	virtual void ClearData() {}

	virtual void OnShow(BOOL bShow);
	virtual BOOL ExportToMenuButton(CMFCToolBarMenuButton& menuButton) const;

	virtual void SetStyle(UINT nStyle);

	virtual int Compare(LPCTSTR lpszItem1, LPCTSTR lpszItem2);
	virtual void OnGlobalFontsChanged();

	virtual BOOL OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& str);

	virtual CString GetPrompt() const { return CString(); }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

protected:
	void Initialize();
	void AdjustRect();
	void SetHotEdit(BOOL bHot = TRUE);

// Attributes:
public:
	static void __stdcall SetFlatMode(BOOL bFlat = TRUE) { m_bFlat = bFlat; }
	static BOOL __stdcall IsFlatMode() { return m_bFlat; }

	static void __stdcall SetCenterVert(BOOL bCenterVert = TRUE) { m_bCenterVert = bCenterVert; }
	static BOOL __stdcall IsCenterVert() { return m_bCenterVert; }

	CComboBox* GetComboBox() const { return m_pWndCombo; }

	LPCTSTR GetText() const { return m_strEdit; }
	void SetText(LPCTSTR lpszText);

	void SetContextMenuID(UINT uiResID) { m_uiMenuResID = uiResID; }
	UINT GetContextMenuID() { return m_uiMenuResID; }

	static CMFCToolBarComboBoxButton* __stdcall GetByCmd(UINT uiCmd, BOOL bIsFocus = FALSE);
	static BOOL __stdcall SelectItemAll(UINT uiCmd, int iIndex);
	static BOOL __stdcall SelectItemAll(UINT uiCmd, DWORD_PTR dwData);
	static BOOL __stdcall SelectItemAll(UINT uiCmd, LPCTSTR lpszText);
	static int __stdcall GetCountAll(UINT uiCmd);
	static int __stdcall GetCurSelAll(UINT uiCmd);
	static LPCTSTR __stdcall GetItemAll(UINT uiCmd, int iIndex = -1);
	static DWORD_PTR __stdcall GetItemDataAll(UINT uiCmd, int iIndex = -1);
	static void* __stdcall GetItemDataPtrAll(UINT uiCmd, int iIndex = -1);
	static LPCTSTR __stdcall GetTextAll(UINT uiCmd);

	virtual BOOL HasFocus() const;

	BOOL IsRibbonButton() const { return m_bIsRibbon; }

protected:
	int   m_iWidth;
	int   m_iSelIndex;
	int   m_nDropDownHeight;
	BOOL  m_bHorz;
	BOOL  m_bIsHotEdit;
	BOOL  m_bIsRibbon;
	UINT  m_uiMenuResID;
	DWORD m_dwStyle;
	CRect m_rectCombo;
	CRect m_rectButton;

	CEdit*     m_pWndEdit;
	CComboBox* m_pWndCombo;
	CString    m_strEdit;
	CStringList m_lstItems;
	CList<DWORD_PTR, DWORD_PTR> m_lstItemData;

	AFX_IMPORT_DATA static BOOL m_bFlat;
	AFX_IMPORT_DATA static BOOL m_bCenterVert;
};

/*============================================================================*/
// CMFCToolBarComboBoxEdit

class CMFCToolBarComboBoxEdit : public CEdit
{
// Construction
public:
	CMFCToolBarComboBoxEdit(CMFCToolBarComboBoxButton& combo);

// Attributes
protected:
	CMFCToolBarComboBoxButton& m_combo;
	BOOL m_bTracked;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCToolBarComboBoxEdit();

	// Generated message map functions
protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChange();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
