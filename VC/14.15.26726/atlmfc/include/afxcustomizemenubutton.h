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
#include "afxtoolbarmenubutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_CUSTOMIZE_INTERNAL_ID ((UINT)-20)

class  CMFCCustomizeMenuButton : public CMFCToolBarMenuButton
{
	friend class CMFCPopupMenuBar;

	DECLARE_DYNCREATE(CMFCCustomizeMenuButton)

public:
	CMFCCustomizeMenuButton(UINT uiID,HMENU hMenu,int iImage,LPCTSTR lpszText = NULL,BOOL bUserButton = FALSE);
	CMFCCustomizeMenuButton();
	virtual ~CMFCCustomizeMenuButton();

	static BOOL __stdcall SetParentToolbar(CMFCToolBar* pToolBar)
	{
		m_pWndToolBar = pToolBar;
		return TRUE;
	}

	static CMFCToolBar* __stdcall GetParentToolbar()
	{
		return m_pWndToolBar;
	}

	void SetItemIndex(UINT uiIndex, BOOL bExist = TRUE, BOOL bAddSp = FALSE);

//Overridables
protected:
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClickMenuItem();
	virtual void DrawCheckBox(CDC* pDC, const CRect& rect, BOOL bHighlight);

public:

	void SetSeparator()
	{
		bSeparator = TRUE;
		SetStyle(TBBS_DISABLED);
	}

	void RestoreRecentlyUsedState() { CMFCMenuBar::SetRecentlyUsedMenus(m_bRecentlyUsedOld); }

	// Save Resently Used State
	AFX_IMPORT_DATA static BOOL m_bRecentlyUsedOld;

	void EnableCustomization(BOOL bEnable = TRUE) { m_bIsEnabled = bEnable; }
	void SetSiblingsButton() { m_bBrothersBtn = TRUE; }

	AFX_IMPORT_DATA static CMap<UINT, UINT, int, int> m_mapPresentIDs;

	static BOOL __stdcall IsCommandExist(UINT uiCmdId);

protected:
	AFX_IMPORT_DATA static CMFCToolBar* m_pWndToolBar;

	UINT m_uiIndex;
	BOOL m_bShow;
	BOOL bSeparator;
	BOOL m_bExist;
	BOOL m_bAddSpr;
	BOOL m_bIsEnabled;
	BOOL m_bBrothersBtn;

protected:
	CString SearchCommandText(CMenu* pMenu, UINT in_uiCmd);
	void UpdateCustomizeButton();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
