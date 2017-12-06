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
#include "afxtoolbarbutton.h"
#include "afxeditbrowsectrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarEditBoxButton : public CMFCToolBarButton
{
	friend class CMFCToolBarEditCtrl;

	DECLARE_SERIAL(CMFCToolBarEditBoxButton)

public:
	CMFCToolBarEditBoxButton();
	CMFCToolBarEditBoxButton(UINT uiID, int iImage, DWORD dwStyle = ES_AUTOHSCROLL, int iWidth = 0);
	virtual ~CMFCToolBarEditBoxButton();

// Overrides:
	virtual CEdit* CreateEdit(CWnd* pWndParent, const CRect& rect);

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick(CWnd* pWnd, BOOL bDelay = TRUE);
	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual void OnMove();
	virtual void OnSize(int iSize);
	virtual HWND GetHwnd() { return m_pWndEdit->GetSafeHwnd(); }
	virtual BOOL NotifyCommand(int iNotifyCode);
	
	virtual BOOL CanBeStretched() const { return TRUE; }
	virtual void OnAddToCustomizePage();
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);
	virtual BOOL HaveHotBorder() const { return m_pWndEdit->GetSafeHwnd() == NULL || (m_pWndEdit->GetStyle() & WS_VISIBLE) == 0; }

	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);

	virtual void OnShow(BOOL bShow);
	virtual void SetContents(const CString& sContents);

	virtual const CRect GetInvalidateRect() const;
	virtual void SetStyle(UINT nStyle);

	virtual void GetEditBorder(CRect& rectBorder);

	virtual BOOL OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& str);

	virtual void OnGlobalFontsChanged();

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	static void __stdcall SetFlatMode(BOOL bFlat = TRUE) { m_bFlat = bFlat; }
	static BOOL __stdcall IsFlatMode() { return m_bFlat; }

protected:
	void Initialize();
	void SetHotEdit(BOOL bHot);

	virtual void OnShowEditbox(BOOL /*bShow*/) {}

// Attributes:
public:
	CEdit* GetEditBox() const { return m_pWndEdit; }

	void SetContextMenuID(UINT uiResID) { m_uiMenuResID = uiResID; }
	UINT GetContextMenuID() { return m_uiMenuResID; }

	static CMFCToolBarEditBoxButton* __stdcall GetByCmd(UINT uiCmd);
	static BOOL __stdcall SetContentsAll(UINT uiCmd, const CString& strContents);
	static CString __stdcall GetContentsAll(UINT uiCmd);

protected:
	int     m_iWidth;
	DWORD   m_dwStyle;
	CEdit*  m_pWndEdit;
	CString m_strContents;
	BOOL    m_bChangingText;
	BOOL    m_bHorz;
	BOOL    m_bIsHotEdit;
	UINT    m_uiMenuResID;

	AFX_IMPORT_DATA static BOOL m_bFlat;
};

/*============================================================================*/
// CMFCToolBarEditCtrl

class CMFCToolBarEditCtrl : public CMFCEditBrowseCtrl
{
// Construction
public:
	CMFCToolBarEditCtrl(CMFCToolBarEditBoxButton& edit);

// Attributes
protected:
	CMFCToolBarEditBoxButton& m_buttonEdit;
	BOOL m_bTracked;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCToolBarEditCtrl();

protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
