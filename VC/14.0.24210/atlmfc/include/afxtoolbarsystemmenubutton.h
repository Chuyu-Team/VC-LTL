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

#include "afxtoolbarmenubutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarSystemMenuButton : public CMFCToolBarMenuButton
{
	DECLARE_SERIAL(CMFCToolBarSystemMenuButton)

public:
	CMFCToolBarSystemMenuButton();
	CMFCToolBarSystemMenuButton(HMENU hSystemMenu, HICON hSystemIcon);
	virtual ~CMFCToolBarSystemMenuButton();

// Overrides:
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual void OnDblClk(CWnd* pWnd);
	virtual BOOL CanBeStored() const { return FALSE; }
	virtual BOOL HaveHotBorder() const { return FALSE; }
	virtual void Serialize(CArchive& /*ar*/) { ASSERT(FALSE); }
	virtual void OnCancelMode();

	virtual void CreateFromMenu(HMENU hMenu);
	virtual HMENU CreateMenu() const;

	virtual BOOL IsBorder() const { return FALSE; }
	virtual void OnAfterCreatePopupMenu();

/// Attributes:
public:
	HICON GetSysMenuIcon() const { return m_hSysMenuIcon; }
	HMENU GetSysMenu() const { return m_hSystemMenu; }

protected:
	HICON m_hSysMenuIcon;
	HMENU m_hSystemMenu;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
