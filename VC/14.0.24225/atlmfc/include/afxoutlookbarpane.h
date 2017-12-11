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
#include "afxtoolbar.h"
#include "afxbutton.h"
#include "afxtoolbarimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCOutlookBarPane window

class CMFCOutlookBarPane : public CMFCToolBar
{
	friend class CMFCVisualManager;
	friend class CMFCOutlookBarTabCtrl;

	DECLARE_SERIAL(CMFCOutlookBarPane)

// Construction
public:
	CMFCOutlookBarPane();

// Attributes
public:

// Operations
public:
	//--------------------
	// Add/remove buttons:
	//--------------------

	BOOL AddButton(UINT uiImage, LPCTSTR lpszLabel, UINT iIdCommand, int iInsertAt = -1);
	BOOL AddButton(UINT uiImage, UINT uiLabel, UINT iIdCommand, int iInsertAt = -1);
	BOOL AddButton(LPCTSTR szBmpFileName, LPCTSTR szLabel, UINT iIdCommand, int iInsertAt = -1);

	BOOL AddButton(HBITMAP hBmp, LPCTSTR lpszLabel, UINT iIdCommand, int iInsertAt = -1);
	BOOL AddButton(HICON hIcon, LPCTSTR lpszLabel, UINT iIdCommand, int iInsertAt = -1, BOOL bAlphaBlend = FALSE);

	BOOL RemoveButton(UINT iIdCommand);
	BOOL RemoveButtonByIndex(int nIndex);

	void ClearAll();
	void SetDefaultState();

	//--------------
	// General look:
	//--------------
	void SetTextColor(COLORREF clrRegText, COLORREF clrSelText = 0/* Obsolete*/);
	void SetTransparentColor(COLORREF color);
	void SetBackImage(UINT uiImageID);
	void SetBackColor(COLORREF color);

	// Set extra space between buttons
	void SetExtraSpace(int nSpace) { m_nExtraSpace = nSpace; }
	virtual BOOL CanBeAttached() const { return TRUE; }

	virtual AFX_CS_STATUS IsChangeState(int nOffset, CBasePane** ppTargetBar) const;
	virtual BOOL Dock(CBasePane* pDockBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);
	virtual BOOL OnBeforeFloat(CRect& rectFloat, AFX_DOCK_METHOD dockMethod);

	virtual BOOL RestoreOriginalState();
	virtual BOOL SmartUpdate(const CObList& lstPrevButtons);

	virtual BOOL CanBeRestored() const { return !m_OrigButtons.IsEmpty(); }

	virtual void RemoveAllButtons();

protected:
	BOOL InternalAddButton(int iImageIndex, LPCTSTR szLabel, UINT iIdCommand, int iInsertAt = -1);
	int	 AddBitmapImage(HBITMAP hBitmap);

	void ScrollUp();
	void ScrollDown();
	void ScrollPageUp();
	void ScrollPageDown();

	void CopyButtonsList(const CObList& lstSrc, CObList& lstDst);

// Overrides
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	virtual void AdjustLocations();
	virtual void DoPaint(CDC* pDC);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual CMFCToolBarButton* CreateDroppedButton(COleDataObject* pDataObject);

	virtual BOOL EnableContextMenuItems(CMFCToolBarButton* pButton, CMenu* pPopup);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnEraseWorkArea(CDC* pDC, CRect rectWorkArea);

	virtual DWORD GetCurrentAlignment() const { return (m_dwStyle & CBRS_ALIGN_ANY) & ~CBRS_ORIENT_HORZ; }

	virtual BOOL AllowShowOnList() const { return FALSE; }
	virtual BOOL CanFloat() const { return FALSE; }

	virtual void AddRemoveSeparator(const CMFCToolBarButton* /*pButton*/, const CPoint& /*ptStart*/, const CPoint& /*ptDrop*/) {}
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }

public:
	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = AFX_DEFAULT_TOOLBAR_STYLE, UINT uiID = (UINT)-1, DWORD dwControlBarStyle = 0);

// Implementation
public:
	virtual ~CMFCOutlookBarPane();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNcDestroy();

	DECLARE_MESSAGE_MAP()

public:
	COLORREF GetRegularColor() const { return m_clrRegText; }
	BOOL IsDrawShadedHighlight() const { return m_bDrawShadedHighlight; }
	BOOL IsBackgroundTexture() const { return m_bmpBack.GetCount() != 0; }
	void EnablePageScrollMode(BOOL bPageScroll = TRUE) { m_bPageScrollMode = bPageScroll; }

// Attributes:
protected:
	COLORREF m_clrRegText;
	COLORREF m_clrTransparentColor;
	COLORREF m_clrBackColor;

	CMFCToolBarImages m_bmpBack;
	CMFCButton  m_btnUp;
	CMFCButton  m_btnDown;

	UINT m_uiBackImageId;
	int  m_nSize;           // Width or Height, orientation dependable
	int  m_iScrollOffset;
	int  m_iFirstVisibleButton;
	int  m_nExtraSpace;

	HWND m_hRecentOutlookWnd;
	BOOL m_bScrollDown;
	BOOL m_bDrawShadedHighlight;
	BOOL m_bDontAdjustLayout;
	BOOL m_bPageScrollMode;

	AFX_IMPORT_DATA static CSize          m_csImage;
	AFX_IMPORT_DATA static CMFCToolBarImages m_Images;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
