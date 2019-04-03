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
#include "afxbaseribbonelement.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_RIBBON_IMAGE_AUTO 9999

class CMFCRibbonButton : public CMFCRibbonBaseElement
{
	friend class CMFCRibbonPanel;
	friend class CMFCRibbonPanelMenuBar;
	friend class CMFCToolTipCtrl;
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonConstructor;

	DECLARE_DYNCREATE(CMFCRibbonButton)

// Construction
public:
	CMFCRibbonButton();
	CMFCRibbonButton(UINT nID, LPCTSTR lpszText, int nSmallImageIndex = -1, int nLargeImageIndex = -1, BOOL bAlwaysShowDescription = FALSE);
	CMFCRibbonButton(UINT nID, LPCTSTR lpszText, HICON hIcon, BOOL bAlwaysShowDescription = FALSE, HICON hIconSmall = NULL, BOOL bAutoDestroyIcon = FALSE, BOOL bAlphaBlendIcon = FALSE);

	virtual ~CMFCRibbonButton();

// Attributes
public:
	virtual void SetText(LPCTSTR lpszText);
	virtual void SetDescription(LPCTSTR lpszText);

	virtual BOOL IsDefaultPanelButton() const { return FALSE; }
	virtual BOOL IsApplicationButton() const { return FALSE; }

	void SetMenu(HMENU hMenu, BOOL bIsDefaultCommand = FALSE, BOOL bRightAlign = FALSE);
	void SetMenu(UINT uiMenuResID, BOOL bIsDefaultCommand = FALSE, BOOL bRightAlign = FALSE);
	HMENU GetMenu() const { return m_hMenu; }

	void SetDefaultCommand(BOOL bSet = TRUE) { m_bIsDefaultCommand = bSet; }
	BOOL IsDefaultCommand() const { return m_bIsDefaultCommand; }

	void SetRightAlignMenu(BOOL bSet = TRUE) { m_bRightAlignMenu = bSet; }
	BOOL IsRightAlignMenu() const { return m_bRightAlignMenu; }

	virtual BOOL HasMenu() const { return m_hMenu != NULL || m_arSubItems.GetSize() > 0; }

	virtual BOOL IsCommandAreaHighlighted() const { return m_bIsCommandHighlighted && IsHighlighted(); }
	virtual BOOL IsMenuAreaHighlighted() const { return m_bIsMenuHighlighted && IsHighlighted(); }

	CRect GetMenuRect() const { return m_rectMenu; }
	CRect GetCommandRect() const { return m_rectCommand; }

	BOOL IsMenuOnBottom() const { return m_bMenuOnBottom; }
	virtual BOOL IsAlwaysDrawBorder() const { return m_bForceDrawBorder; }

	virtual BOOL CanBeStretched();

	BOOL IsSingleLineText() const { return m_sizeTextBottom.cy == m_sizeTextRight.cy; }
	int GetTextRowHeight() const { return m_sizeTextRight.cy; }

	const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& GetSubItems() const { return m_arSubItems; }

	int GetImageIndex(BOOL bLargeImage) const { return bLargeImage ? m_nLargeImageIndex : m_nSmallImageIndex; }
	void SetImageIndex(int nIndex, BOOL bLargeImage)
	{
		if (bLargeImage)
		{
			m_nLargeImageIndex = nIndex;
		}
		else
		{
			m_nSmallImageIndex = nIndex;
		}
	}

	BOOL IsLargeImage() const { return m_bIsLargeImage; }
	HICON GetIcon(BOOL bLargeIcon = TRUE) const { return bLargeIcon ? m_hIcon : m_hIconSmall; }

	virtual BOOL IsAlwaysLargeImage () const { return m_bIsAlwaysLarge || (m_nLargeImageIndex >= 0 && m_nSmallImageIndex < 0); }
	void SetAlwaysLargeImage(BOOL bSet = TRUE) { m_bIsAlwaysLarge = bSet; }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	AFX_IMPORT_DATA static BOOL m_bUseMenuHandle;

// Operations
public:
	void AddSubItem(CMFCRibbonBaseElement* pSubItem, int nIndex = -1);
	int FindSubItemIndexByID(UINT uiID) const;
	BOOL RemoveSubItem(int nIndex);
	void RemoveAllSubItems();

	CSize DrawBottomText(CDC* pDC, BOOL bCalcOnly);

protected:
	void CommonInit();
	void FillWindowList();
	int GetGroupButtonExtraWidth();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);

	virtual CSize GetRegularSize(CDC* pDC);
	virtual CSize GetCompactSize(CDC* pDC);
	virtual CSize GetIntermediateSize(CDC* pDC);

	virtual BOOL HasLargeMode() const { return GetImageSize(RibbonImageLarge) != CSize(0, 0); }
	virtual BOOL HasIntermediateMode() const { return !m_strText.IsEmpty(); }
	virtual BOOL HasCompactMode() const { return GetImageSize(RibbonImageSmall) != CSize(0, 0); }

	virtual void DrawImage(CDC* pDC, RibbonImageType type, CRect rectImage);
	virtual CSize GetImageSize(RibbonImageType type) const;

	virtual void OnCalcTextSize(CDC* pDC);
	virtual void CleanUpSizes() { m_sizeTextRight = m_sizeTextBottom = CSize(0, 0); }

	virtual void SetParentCategory(CMFCRibbonCategory* pParent);

	virtual void ClosePopupMenu();
	virtual CString GetToolTipText() const;

	virtual int DrawRibbonText(CDC* pDC, const CString& strText, CRect rectText, UINT uiDTFlags, COLORREF clrText = (COLORREF)-1);

	virtual void OnClick(CPoint point);
	virtual void OnAccDefaultAction();

	virtual BOOL IsDrawTooltipImage() const { return TRUE; }
	virtual BOOL IsPopupDefaultMenuLook() const { return TRUE; }

	virtual COLORREF OnFillBackground(CDC* pDC);
	virtual void OnDrawBorder(CDC* pDC);

protected:
	virtual void OnDrawOnList(CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);
	virtual int AddToListBox(CMFCRibbonCommandsListBox* pWndListBox, BOOL bDeep);

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);
	virtual void OnMouseMove(CPoint point);

	virtual void OnShowPopupMenu();

	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void SetOriginal(CMFCRibbonBaseElement* pOriginal);

	virtual CMFCRibbonBaseElement* FindByID(UINT uiCmdID);
	virtual CMFCRibbonBaseElement* FindByData(DWORD_PTR dwData);

	virtual void GetElementsByID(UINT uiCmdID, CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);
	virtual void GetElements(CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements);

	virtual int GetTextOffset() const { return m_szMargin.cx; }

	virtual void SetParentRibbonBar(CMFCRibbonBar* pRibbonBar);
	virtual CRect GetKeyTipRect(CDC* pDC, BOOL bIsMenu);
	virtual BOOL OnKey(BOOL bIsMenuKey);

	virtual void OnAfterChangeRect(CDC* pDC);

	virtual void SetMargin(const CSize& sizeMargin) { m_szMargin = sizeMargin; }

// Attributes
protected:
	CSize m_sizeTextRight;
	CSize m_sizeTextBottom;
	CSize m_szMargin;
	CRect m_rectMenu;
	CRect m_rectCommand;

	int m_nSmallImageIndex;
	int m_nLargeImageIndex;
	int m_nMenuArrowMargin;
	int m_nWindowsMenuItems;
	int m_nWrapIndex;

	HMENU m_hMenu;
	HICON m_hIcon;
	HICON m_hIconSmall;

	BOOL m_bRightAlignMenu;
	BOOL m_bIsDefaultCommand;
	BOOL m_bAutodestroyMenu;
	BOOL m_bMenuOnBottom;
	BOOL m_bIsMenuHighlighted;
	BOOL m_bIsCommandHighlighted;
	BOOL m_bAutoDestroyIcon;
	BOOL m_bAlphaBlendIcon;
	BOOL m_bForceDrawBorder;
	BOOL m_bToBeClosed;
	BOOL m_bAlwaysShowDescription;
	BOOL m_bIsLargeImage;
	BOOL m_bCreatedFromMenu;
	BOOL m_bIsWindowsMenu;

	CArray<int,int> m_arWordIndexes;
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arSubItems;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
