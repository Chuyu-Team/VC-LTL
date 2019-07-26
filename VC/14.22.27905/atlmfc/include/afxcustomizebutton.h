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

class CMFCToolBar;

class CMFCCustomizeButton : public  CMFCToolBarMenuButton
{
	friend class CMFCToolBar;

	DECLARE_SERIAL(CMFCCustomizeButton)

public:
	CMFCCustomizeButton();
	CMFCCustomizeButton(int uiCustomizeCmdId, const CString& strCustomizeText);
	virtual ~CMFCCustomizeButton();

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual CMFCPopupMenu* CreatePopupMenu();

	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual BOOL IsEmptyMenuAllowed() const { return TRUE; }

	virtual void OnCancelMode();
	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);

	virtual BOOL IsEditable() const { return FALSE; }
	virtual BOOL CanBeStored() const { return FALSE; }

	int GetCustomizeCmdId() const { return m_iCustomizeCmdId; }

	CString GetCustomizeText() const { return m_strCustomizeText; }

	const CObList& GetInvisibleButtons() const { return m_lstInvisibleButtons; }

	CMFCToolBar* GetParentToolbar() { return m_pWndParentToolbar; }

	void SetDefaultDraw(BOOL bDefaultDraw = TRUE) { m_bDefaultDraw = bDefaultDraw; }
	BOOL IsDefaultDraw() const { return m_bDefaultDraw; }

	void SetExtraSize(int cx, int cy) { m_sizeExtra = CSize(cx, cy); }
	CSize GetExtraSize() const { return m_bIsPipeStyle ? m_sizeExtra : CSize(0, 0); }

	void SetPipeStyle(BOOL bOn = TRUE) { m_bIsPipeStyle = bOn; }
	BOOL IsPipeStyle() const { return m_bIsPipeStyle && !m_bOnRebar; }

	void AddInvisibleButton(CMFCToolBarButton* pButton) { m_lstInvisibleButtons.AddTail(pButton); }

	virtual BOOL InvokeCommand(CMFCPopupMenuBar* pMenuBar, const CMFCToolBarButton* pButton);

	void SetMenuRightAlign(BOOL bMenuRightAlign) { m_bMenuRightAlign = bMenuRightAlign; }
	BOOL IsMenuRightAlign() const { return m_bMenuRightAlign; }

public:
	AFX_IMPORT_DATA static BOOL m_bIgnoreLargeIconsMode;

protected:
	void CommonInit();

	int             m_iCustomizeCmdId;
	CSize           m_sizeExtra;
	CString         m_strCustomizeText;
	CMFCToolBar* m_pWndParentToolbar;
	BOOL            m_bIsEmpty;
	BOOL            m_bDefaultDraw;
	BOOL            m_bIsPipeStyle;        // Used in 2003 style only
	BOOL            m_bOnRebar;
	BOOL            m_bMenuRightAlign;
	CObList         m_lstInvisibleButtons; // List of invisible butons on the parent toolbar.
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
