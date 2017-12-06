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

#include "afxtoolbareditboxbutton.h"
#include "afxspinbuttonctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarSpinEditBoxButton : public CMFCToolBarEditBoxButton
{
	DECLARE_SERIAL(CMFCToolBarSpinEditBoxButton)

// Construction
public:
	CMFCToolBarSpinEditBoxButton();
	CMFCToolBarSpinEditBoxButton(UINT uiID, int iImage, DWORD dwStyle = ES_AUTOHSCROLL, int iWidth = 0);
	virtual ~CMFCToolBarSpinEditBoxButton();

// Operations
public:
	void SetRange(int nMin, int nMax);
	void GetRange(int& nMin, int& nMax);

	CSpinButtonCtrl* GetSpinControl() { return &m_wndSpin; }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

protected:
	void Init();

// Attributes
protected:
	CMFCSpinButtonCtrl m_wndSpin;
	int m_nMin;
	int m_nMax;

// Overrides
protected:
	virtual CEdit* CreateEdit(CWnd* pWndParent, const CRect& rect);
	virtual void OnMove();
	virtual void GetEditBorder(CRect& rectBorder);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& str);
	virtual void OnShowEditbox(BOOL bShow);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
