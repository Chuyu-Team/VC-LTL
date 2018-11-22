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
#include "afxpopupmenu.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

static const int AFX_TEXT_MARGIN = 3;
static const int AFX_MENU_IMAGE_MARGIN = 2;

static const int AFX_MENU_GROUP_ID = ((UINT)-1) - 1;

class CMFCToolBarMenuButton : public CMFCToolBarButton
{
	friend class CMFCPopupMenu;
	friend class CMFCPopupMenuBar;
	friend class CMFCToolBar;
	friend class CMFCMenuBar;
	friend class CMFCRibbonStatusBarCustomizeButton;
	friend class CMFCRibbonButton;

	DECLARE_SERIAL(CMFCToolBarMenuButton)

public:
	CMFCToolBarMenuButton();
	CMFCToolBarMenuButton(const CMFCToolBarMenuButton& src);
	CMFCToolBarMenuButton(UINT uiID, HMENU hMenu, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE);
	virtual ~CMFCToolBarMenuButton();

// Operations:
protected:
	void Initialize();
	void Initialize(UINT uiID, HMENU hMenu, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE);
	void DrawMenuItem(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bCustomizeMode, BOOL bHighlight, BOOL bGrayDisabledButtons, BOOL bContentOnly = FALSE);
	void DrawDocumentIcon(CDC* pDC, const CRect& rectImage, HICON hIcon);

	static void __stdcall GetTextHorzOffsets(int& xOffsetLeft, int& xOffsetRight);

public:
	virtual BOOL OpenPopupMenu(CWnd* pWnd = NULL);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);

	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick(CWnd* pWnd, BOOL bDelay = TRUE);
	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual void CreateFromMenu(HMENU hMenu);
	virtual HMENU CreateMenu() const;
	virtual BOOL HaveHotBorder() const { return FALSE; }
	virtual void OnCancelMode();
	virtual BOOL OnContextHelp(CWnd* pWnd) { return OnClick(pWnd, FALSE); }

	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);
	virtual BOOL IsDroppedDown() const { return m_pPopupMenu != NULL; }

	virtual CMFCPopupMenu* CreatePopupMenu() { return new CMFCPopupMenu; }
	virtual void OnAfterCreatePopupMenu() {}

	virtual BOOL IsEmptyMenuAllowed() const { return FALSE; }

	virtual BOOL OnBeforeDrag() const;
	virtual void SaveBarState();

	void GetImageRect(CRect& rectImage);
	
	virtual void SetRadio();
	virtual void ResetImageToDefault();
	virtual BOOL CompareWith(const CMFCToolBarButton& other) const;

	virtual BOOL IsBorder() const { return TRUE; }

	virtual BOOL OnClickMenuItem() { return FALSE; } // Return TRUE for the custom process
	virtual BOOL IsExclusive() const { return FALSE; }
	virtual BOOL HasButton() const { return FALSE; }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

// Attributes:
public:
	const CObList& GetCommands() const { return m_listCommands; }
	CMFCPopupMenu* GetPopupMenu() const { return m_pPopupMenu; }

	void SetMenuOnly(BOOL bMenuOnly) { m_bMenuOnly = bMenuOnly; }
	virtual BOOL IsTearOffMenu() const { return m_uiTearOffBarID != 0; }
	virtual void SetTearOff(UINT uiBarID);

	// CMainFrame::OnDrawMenuImage will be called  even after default image was drawn
	AFX_IMPORT_DATA static BOOL m_bAlwaysCallOwnerDraw;

	void SetMessageWnd(CWnd* pWndMessage) { m_pWndMessage = pWndMessage; }

	void SetMenuPaletteMode(BOOL bMenuPaletteMode = TRUE, int nPaletteRows = 1);
	BOOL IsMenuPaletteMode() const { return m_bMenuPaletteMode; }

	int GetPaletteRows() const { return m_nPaletteRows; }

	void EnableQuickCustomize() { m_bQuickCustomMode = TRUE; }
	BOOL IsQuickMode() { return m_bQuickCustomMode; }
	BOOL IsClickedOnMenu() const { return m_bClickedOnMenu; }

protected:
	CObList     m_listCommands; // List of CMFCToolBarButton
	CMFCPopupMenu* m_pPopupMenu;

	BOOL m_bDrawDownArrow;
	BOOL m_bMenuMode;
	BOOL m_bDefault;
	BOOL m_bClickedOnMenu;
	BOOL m_bHorz;
	BOOL m_bToBeClosed;
	BOOL m_bMenuOnly; //JRG
	BOOL m_bIsRadio;
	BOOL m_bMenuPaletteMode;
	BOOL m_bQuickCustomMode;
	BOOL m_bShowAtRightSide;
	UINT m_uiTearOffBarID;
	int  m_nPaletteRows;

	CWnd* m_pWndMessage;
	CRect m_rectArrow;
	CRect m_rectButton;

public:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
