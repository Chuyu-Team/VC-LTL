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

#ifndef __AFXOLE_H__
	#include <afxole.h>
#endif

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CAccessibilityData;
class CMFCToolBar;
class CMFCToolBarImages;
class CMFCToolBarMenuButton;

#define AFX_DUMMY_AMPERSAND_SEQUENCE  _T("\001\001")

class CMFCToolBarButton : public CObject
{
	friend class CMFCToolBarButtonsListButton;
	DECLARE_SERIAL(CMFCToolBarButton)

public:
	CMFCToolBarButton();
	CMFCToolBarButton(UINT uiID, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE, BOOL bLocked = FALSE);
	virtual ~CMFCToolBarButton();

// Operations:
public:
	//--------------------
	// Drag and drop stuff:
	//--------------------
	static CLIPFORMAT __stdcall GetClipboardFormat();
	static CMFCToolBarButton* __stdcall CreateFromOleData(COleDataObject* pDataObject);
	virtual BOOL PrepareDrag(COleDataSource& srcItem);

	//-----------------------------------------------------
	// Optional: if you want, that user may drag buttons 
	// between different applications, set your own format:
	//-----------------------------------------------------
	static void __stdcall SetClipboardFormatName(LPCTSTR lpszName);
	virtual BOOL CanBeDropped(CMFCToolBar* /*pToolbar*/) { return TRUE; }

	//-----------------------------------------------------------------
	//	Protected commands support. 
	//	Protected buttons will disabled in customization mode, so user 
	//	will be unable to drag/drop/change them.
	//-----------------------------------------------------------------
	static void __stdcall SetProtectedCommands(const CList<UINT, UINT>& lstCmds);
	static const CList<UINT, UINT>& __stdcall GetProtectedCommands() { return m_lstProtectedCommands; }

// Overrides:
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick(CWnd* /* pWnd */, BOOL /* bDelay = TRUE */) { return FALSE; }
	virtual BOOL OnClickUp() { return FALSE; }

	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual BOOL ExportToMenuButton(CMFCToolBarMenuButton& menuButton) const;
	virtual void OnMove() {}
	virtual void OnSize(int /*iSize*/) {}
	virtual HWND GetHwnd() { return NULL; }
	virtual BOOL CanBeStretched() const { return FALSE; }
	virtual BOOL NotifyCommand(int /*iNotifyCode*/) { return FALSE; }
	virtual void OnAddToCustomizePage() {}
	virtual HBRUSH OnCtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/) { return NULL; }
	virtual void OnDblClk(CWnd* /*pWnd*/) {}
	virtual BOOL CanBeStored() const { return TRUE; }
	virtual BOOL HaveHotBorder() const { return TRUE; }
	virtual void OnCancelMode() {}
	virtual void OnGlobalFontsChanged() {}
	virtual BOOL IsEditable() const { return !IsStandardCommand(m_nID) && (m_lstProtectedCommands.Find(m_nID) == NULL); }

	virtual BOOL OnContextHelp(CWnd* /* pWnd */) { return FALSE; }
	virtual BOOL OnCustomizeMenu(CMenu* /*pMenu*/) { return FALSE; }

	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);

	virtual BOOL IsDroppedDown() const { return FALSE; }
	virtual BOOL OnBeforeDrag() const { return TRUE; } // Drag is possible
	virtual BOOL OnBeforeDrop(CMFCToolBar* /*pTarget*/) { return TRUE; } // Drop is possible

	virtual BOOL OnToolHitTest(const CWnd* pWnd, TOOLINFO* pTI);
	virtual void SaveBarState() {}

	virtual void OnShow(BOOL /*bShow*/) {}

	virtual const CRect GetInvalidateRect() const { return m_rect; }
	virtual void SetStyle(UINT nStyle) { m_nStyle = nStyle; }

	virtual void ResetImageToDefault();
	virtual BOOL CompareWith(const CMFCToolBarButton& other) const;

	virtual void EnableWindow(BOOL bEnable = TRUE)
	{
		if (GetHwnd() != NULL)
		{
			::EnableWindow(GetHwnd(), bEnable);
		}
	}

	virtual BOOL IsWindowVisible()
	{
		CWnd* pWnd = GetHwnd() != NULL ? CWnd::FromHandle(GetHwnd()) : NULL;
		return (pWnd != NULL &&(pWnd->GetStyle() & WS_VISIBLE));
	}

	virtual BOOL IsOwnerOf(HWND hwnd)
	{
		return	GetHwnd() != NULL && (GetHwnd() == hwnd || ::IsChild(GetHwnd(), hwnd));
	}

	virtual BOOL HasFocus() const
	{
		HWND hwndBtn = ((CMFCToolBarButton*)this)->GetHwnd();
		return hwndBtn != NULL && (hwndBtn == ::GetFocus() || ::IsChild(hwndBtn, ::GetFocus()));
	}

	virtual BOOL OnGetCustomToolTipText(CString& /*strToolTip*/) { return FALSE; }

	virtual BOOL OnUpdateToolTip(CWnd* /*pWndParent*/, int /*iButtonIndex*/, CToolTipCtrl& /*wndToolTip*/, CString& /*str*/) { return FALSE; }

	virtual BOOL IsFirstInGroup() const;
	virtual BOOL IsLastInGroup() const;

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);
	virtual int GetAccCount();

protected:
	void Initialize();
	void FillInterior(CDC* pDC, const CRect& rect, BOOL bHighlight, BOOL bMenuImage = FALSE);

// Attributes:
public:
	BOOL IsDrawText() const { return m_bText && !m_strText.IsEmpty(); }
	BOOL IsDrawImage() const { return m_bImage && GetImage() >= 0; }
	int GetImage() const { return m_bUserButton ? m_iUserImage : m_iImage; }

	virtual void SetImage(int iImage);
	virtual void SetRadio() {}

	BOOL IsLocked() const { return m_bLocked; }

	void SetRect(const CRect rect)
	{
		m_rect = rect;
		OnMove();
	}

	const CRect& Rect() const { return m_rect; }

	void Show(BOOL bShow)
	{
		if (m_bIsHidden != !bShow)
		{
			m_bIsHidden = !bShow;
			OnShow(bShow);
		}
	}

	BOOL IsHidden() const { return m_bIsHidden; }

	BOOL IsVisible() const { return m_bVisible; }
	void SetVisible(BOOL bShow = TRUE) { m_bVisible = bShow; }

	virtual BOOL IsExtraSize() const { return m_bExtraSize; }
	SIZE GetTextSize() const { return m_sizeText; } // Actual only if m_bTextBelow is TRUE
	BOOL IsHorizontal() const { return m_bHorz; }
	CWnd* GetParentWnd() const { return m_pWndParent; }

	BOOL    m_bUserButton;   // Is user-defined tool button?
	BOOL    m_bText;         // Draw text label
	BOOL    m_bImage;        // Draw image
	BOOL    m_bWrap;         // Wrap toolbar
	BOOL    m_bWholeText;    // Is whole text printed?
	BOOL    m_bTextBelow;    // Is text below image?
	BOOL    m_bDragFromCollection; // Button is dragged from collection
	UINT    m_nID;           // Command ID that this button sends
	UINT    m_nStyle;        // TBBS_ styles
	DWORD_PTR m_dwdItemData;   // User-defined data
	CString m_strText;       // Button text (for user-defined buttons only!)
	CString m_strTextCustom; // Text appear on the customization list

	// Run-time properties:
	AFX_IMPORT_DATA static CLIPFORMAT m_cFormat;
	AFX_IMPORT_DATA static CString    m_strClipboardFormatName;

	AFX_IMPORT_DATA static BOOL m_bWrapText; // Is toolbar text may be multi-lined?
	AFX_IMPORT_DATA static BOOL m_bUpdateImages;

protected:
	int m_iImage;        // index into bitmap of this button's picture
	int m_iUserImage;    // index into user's bitmap of this button's picture

	BOOL m_bLocked;      // Is buttons placed on the "locked" toolbar
	BOOL m_bIsHidden;    // Button rectangle is out of bar
	BOOL m_bDisableFill; // Disable interior fill
	BOOL m_bExtraSize;   // Is Visual Manager's extra size used?
	BOOL m_bHorz;        // Is located on the horizontal toolbar?
	BOOL m_bVisible;     // Is button  visible 

	CRect m_rect;        // Button location
	CSize m_sizeText;    // Actual only if m_bTextBelow is TRUE
	CWnd* m_pWndParent;  // Parent window

	AFX_IMPORT_DATA static CList<UINT, UINT> m_lstProtectedCommands; // Buttons are disabled in customization mode

// Diagnostics:
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
