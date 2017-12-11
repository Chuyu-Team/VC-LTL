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
#include "afxmenubar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarMenuButton;

/*============================================================================*/
// CMFCPopupMenuBar command target

class CMFCPopupMenuBar : public CMFCToolBar
{
	friend class CMFCPopupMenu;
	friend class CMFCDropDownListBox;

	DECLARE_SERIAL(CMFCPopupMenuBar)

public:
	CMFCPopupMenuBar();
	virtual ~CMFCPopupMenuBar();

// Attributes
public:
	UINT GetDefaultMenuId() const { return m_uiDefaultMenuCmdId; }

	int GetOffset() const { return m_iOffset; }
	void SetOffset(int iOffset)
	{
		if (m_iOffset != iOffset)
		{
			m_iOffset = iOffset;
			AdjustLocations();
		}
	}

	virtual CSize GetCurrentMenuImageSize() const
	{
		if (m_pRelatedToolbar == NULL || !m_pRelatedToolbar->IsLocked())
		{
			return CMFCToolBar::GetMenuImageSize();
		}
		return m_pRelatedToolbar->GetLockedImageSize();
	}

	BOOL            m_bDisableSideBarInXPMode;
	CMFCToolBar* m_pRelatedToolbar;

	BOOL IsDropDownListMode() const { return m_bDropDownListMode; }

protected:
	UINT m_uiDefaultMenuCmdId;
	int  m_iOffset;	// rows
	int  m_xSeparatorOffsetLeft;
	int  m_xSeparatorOffsetRight;
	int  m_iMaxWidth;
	int  m_iMinWidth;
	BOOL m_bFirstClick;
	BOOL m_bFirstMove;
	BOOL m_bAreAllCommandsShown;
	BOOL m_bInCommand;
	BOOL m_bTrackMode;
	BOOL m_bIsClickOutsideItem;
	BOOL m_bPaletteMode;
	BOOL m_bPaletteRows;
	BOOL m_bDropDownListMode;
	BOOL m_nDropDownPageSize;
	BOOL m_bInScrollMode;
	BOOL m_bResizeTracking;

	CPoint                     m_ptCursor;
	CUIntArray                 m_arColumns;
	CMFCToolBarMenuButton*        m_pDelayedPopupMenuButton;
	CMFCToolBarMenuButton*        m_pDelayedClosePopupMenuButton;
	CMap<UINT, UINT,UINT,UINT> m_HiddenItemsAccel; // Accell. <==> Command

	AFX_IMPORT_DATA static UINT m_uiPopupTimerDelay;
	AFX_IMPORT_DATA static int  m_nLastCommandIndex;

// Operations
public:
	virtual BOOL ImportFromMenu(HMENU hMenu, BOOL bShowAllCommands = FALSE);
	virtual HMENU ExportToMenu() const;

	void StartPopupMenuTimer(CMFCToolBarMenuButton* pMenuButton, int nDelayFactor = 1);
	CMFCToolBar* FindDestintationToolBar(CPoint point);

	virtual void CloseDelayedSubMenu();
	virtual void RestoreDelayedSubMenu();

	BOOL LoadFromHash(HMENU hMenu);

	static int __stdcall GetLastCommandIndex() { return m_nLastCommandIndex; }
	BOOL IsPaletteMode() const { return m_bPaletteMode; }
	virtual BOOL IsRibbonPanel() const { return FALSE; }
	virtual BOOL IsRibbonPanelInRegularMode() const { return FALSE; }

	virtual void AdjustSizeImmediate(BOOL /*bRecalcLayout = TRUE*/) {}
	
	BOOL BuildOrigItems(UINT uiMenuResID);
	int GetGutterWidth();

public:
// Overrides
	virtual void SetButtonStyle(int nIndex, UINT nStyle);

	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);

protected:
	virtual BOOL OnSendCommand(const CMFCToolBarButton* pButton);
	virtual void AdjustLocations();
	virtual void DrawSeparator(CDC* pDC, const CRect& rect, BOOL bHorz);
	virtual CSize CalcSize(BOOL bVertDock);
	virtual BOOL AllowSelectDisabled() const { return CMFCMenuBar::IsHighlightDisabledItems(); }
	virtual BOOL AllowShowOnList() const { return FALSE; }
	virtual int FindDropIndex(const CPoint p, CRect& rectDrag) const;
	virtual void DrawDragCursor(CDC* pDC);
	virtual CMFCToolBarButton* CreateDroppedButton(COleDataObject* pDataObject);
	virtual void OnChangeHot(int iHot);
	virtual BOOL OnKey(UINT nChar);
	virtual BOOL OnSetDefaultButtonText(CMFCToolBarButton* pButton);
	virtual BOOL EnableContextMenuItems(CMFCToolBarButton* pButton, CMenu* pPopup);
	virtual void OnFillBackground(CDC* pDC);
	virtual void ShowCommandMessageString(UINT uiCmdId);

	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	virtual BOOL OnUserToolTip(CMFCToolBarButton* /*pButton*/, CString& /*strTTText*/) const { return FALSE; }

	virtual CWnd* GetCommandTarget() const;
	void InvokeMenuCommand(UINT uiCmdId, const CMFCToolBarButton* pMenuItem);

	void SetInCommand(BOOL bInCommand = TRUE);

	virtual BOOL TranslateChar(UINT /*nChar*/) { return FALSE; }
	virtual BOOL IsButtonExtraSizeAvailable() const { return FALSE; }

	virtual void AdjustLayout();

	virtual void OnCalcSeparatorRect(CMFCToolBarButton* pButton, CRect& rectSeparator, BOOL bHorz);
	virtual BOOL AllowAltCustomization() const { return FALSE; }
	virtual void OnAfterButtonDelete();

	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }
	virtual BOOL AlwaysSaveSelection() const { return FALSE; }

// Implementation
protected:
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnToolbarImageAndText();
	afx_msg void OnToolbarText();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
