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
#include "afxribbonbutton.h"
#include "afxtoolbarimages.h"
#include "afxtoolbarmenubutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonGalleryIcon;
class CMFCPopupMenu;

class CMFCRibbonGallery : public CMFCRibbonButton
{
	friend class CMFCRibbonGalleryIcon;
	friend class CMFCRibbonPanelMenuBar;
	friend class CMFCRibbonPanel;
	friend class CMFCRibbonGalleryMenuButton;
	friend class CMFCRibbonCollector;
	friend class CMFCRibbonConstructor;

	DECLARE_DYNCREATE(CMFCRibbonGallery)

// Construction
public:
	CMFCRibbonGallery();
	CMFCRibbonGallery(UINT nID, LPCTSTR lpszText, int nSmallImageIndex, int nLargeImageIndex, CMFCToolBarImages& imagesPalette);
	CMFCRibbonGallery(UINT nID, LPCTSTR lpszText, int nSmallImageIndex, int nLargeImageIndex, UINT uiImagesPaletteResID = 0, int cxPaletteImage = 0);

	// Owner-draw icons constructor:
	CMFCRibbonGallery(UINT nID, LPCTSTR lpszText, int nSmallImageIndex, int nLargeImageIndex, CSize sizeIcon,
		int nIconsNum,					// If 0 - you should call AddGroup later
		BOOL bDefaultButtonStyle = TRUE);// If FALSE - you need to implement your own border/content drawing

	virtual ~CMFCRibbonGallery();

// Attributes
public:
	void SetPaletteID(UINT nID) { m_nPaletteID = nID; }
	int GetPaletteID() const { return m_nPaletteID; }

	void SetButtonMode(BOOL bSet = TRUE) { m_bIsButtonMode = bSet; }
	BOOL IsButtonMode() const { return m_bIsButtonMode; }

	void SelectItem(int nItemIndex);
	int GetSelectedItem() const { return m_nSelected; }

	void EnableMenuResize(BOOL bEnable = TRUE, BOOL bVerticalOnly = FALSE)
	{
		m_bEnableMenuResize = bEnable;
		m_bMenuResizeVertical = bVerticalOnly;
	}

	BOOL IsMenuResizeEnabled() const { return m_bEnableMenuResize; }
	BOOL IsMenuResizeVertical() const { return m_bMenuResizeVertical; }

	void SetIconsInRow(int nIconsInRow) { m_nIconsInRow = nIconsInRow; }
	int GetIconsInRow() const { return m_nIconsInRow; }

	void SetItemToolTip(int nItemIndex, LPCTSTR lpszToolTip);
	LPCTSTR GetItemToolTip(int nItemIndex) const;
	void RemoveItemToolTips();

	static int __stdcall GetLastSelectedItem(UINT uiCmdID);

	BOOL IsMenuSideBar() const { return m_bMenuSideBar; }
	void EnableMenuSideBar(BOOL bEnable = TRUE) { m_bMenuSideBar = bEnable; }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data)
	{
		CMFCRibbonButton::SetACCData(pParent, data);
		data.m_nAccRole = ROLE_SYSTEM_BUTTONDROPDOWNGRID;
		return TRUE;
	}

// Operations:
public:
	void AddGroup(LPCTSTR lpszGroupName, UINT uiImagesPaletteResID, int cxPaletteImage);
	void AddGroup(LPCTSTR lpszGroupName, CMFCToolBarImages& imagesGroup);

	// For user-defined icons only!
	void AddGroup(LPCTSTR lpszGroupName, int nIconsNum);

	void SetGroupName(int nGroupIndex, LPCTSTR lpszGroupName);
	LPCTSTR GetGroupName(int nGroupIndex) const;

	void SetPalette(CMFCToolBarImages& imagesPalette);
	void SetPalette(UINT uiImagesPaletteResID, int cxPaletteImage);

	virtual void Clear();
	void RedrawIcons();

	void AddSubItem(CMFCRibbonBaseElement* pSubItem, int nIndex = -1, BOOL bOnTop = FALSE);

protected:
	void CommonInit();
	void CreateIcons();
	void RemoveAll();
	void RebuildIconLocations();
	
	BOOL IsButtonLook() const
	{
		return	m_bQuickAccessMode || m_bFloatyMode || m_bIsButtonMode || m_bIsCollapsed || m_pParentGroup != NULL || m_nIcons == 0;
	}

	virtual BOOL PreClickPaletteIcon(CMFCRibbonGalleryIcon* /*pIcon*/, const CPoint& /*point*/) { return TRUE; }
	virtual void OnClickPaletteIcon(CMFCRibbonGalleryIcon* pIcon);
	void GetMenuItems(CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arButtons);

	virtual CMFCRibbonBaseElement* HitTest(CPoint point);
	virtual CMFCRibbonBaseElement* GetPressed();
	virtual CMFCRibbonBaseElement* GetHighlighted();

	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void SetParentCategory(CMFCRibbonCategory* pParent);

	virtual void OnShowPopupMenu();

	int GetMenuRowHeight() const;

	virtual UINT GetNotifyID()
	{
		if (m_bNotifyPaletteID)
		{
			SetNotifyParentID(FALSE);
			return m_nPaletteID;
		}
		else
		{
			return m_nID;
		}
	}

	virtual BOOL OnKey(BOOL bIsMenuKey);
	virtual CRect GetKeyTipRect(CDC* pDC, BOOL bIsMenu);

	virtual CSize GetIconSize() const;

	virtual CString GetIconToolTip(const CMFCRibbonGalleryIcon* pIcon) const;
	virtual CString GetIconDescription(const CMFCRibbonGalleryIcon* pIcon) const;

	virtual BOOL CanBeStretchedHorizontally()
	{
		return !IsButtonMode() && !m_bIsCollapsed;
	}

	virtual void SetInitialMode(BOOL bOneRow = FALSE)
	{
		CMFCRibbonButton::SetInitialMode(bOneRow);

		m_bResetColumns = TRUE;
		m_bIsCollapsed = FALSE;
	}

	virtual BOOL HasLargeMode() const
	{
		return !m_bIsButtonMode || CMFCRibbonButton::HasLargeMode();
	}

	virtual void StretchHorizontally()
	{
		if (m_bSmallIcons || m_nPanelColumns <= 3)
		{
			m_bIsCollapsed = TRUE;
		}
		else
		{
			m_nPanelColumns--;
		}
	}

	virtual BOOL OnClickPaletteSubItem(CMFCRibbonButton* /*pButton*/, CMFCRibbonPanelMenuBar* /*pMenuBar*/) { return FALSE; }
	virtual BOOL IsItemMenuLook() const { return FALSE; }

// Overrides
public:
	virtual void OnDraw(CDC* pDC);

	virtual CSize GetRegularSize(CDC* pDC);
	virtual CSize GetCompactSize(CDC* pDC);

	virtual void OnAfterChangeRect(CDC* pDC);

	virtual BOOL HasMenu() const { return m_nIcons > 0; }

	virtual CMFCRibbonBaseElement* GetDroppedDown();
	virtual void OnEnable(BOOL bEnable);

	virtual void OnRTLChanged(BOOL bIsRTL);

	virtual int GetGroupOffset() const { return 0; }

	virtual void OnSetFocus(BOOL bSet);

protected:
	virtual void OnDrawPaletteIcon(CDC* pDC, CRect rectIcon, int nIconIndex, CMFCRibbonGalleryIcon* pIcon, COLORREF clrText);

// Attributes
protected:
	CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> m_arIcons;

	CStringArray    m_arGroupNames;
	CArray<int,int> m_arGroupLen;
	CMFCToolBarImages  m_imagesPalette;
	CStringArray    m_arToolTips;

	UINT m_nPaletteID;

	int m_nImagesInRow;
	int m_nPanelColumns;
	int m_nImagesInColumn;
	int m_nScrollOffset;
	int m_nScrollTotal;
	int m_nSelected;
	int m_nIconsInRow;
	int m_nIcons;

	BOOL m_bIsButtonMode;
	BOOL m_bNotifyPaletteID;
	BOOL m_bSmallIcons;
	BOOL m_bEnableMenuResize;
	BOOL m_bMenuResizeVertical;
	BOOL m_bIsOwnerDraw;
	BOOL m_bDefaultButtonStyle;
	BOOL m_bMenuSideBar;
	BOOL m_bIsCollapsed;
	BOOL m_bResetColumns;

	AFX_IMPORT_DATA static CMap<UINT,UINT,int,int> m_mapSelectedItems;

	void SetNotifyParentID(BOOL bSet);
};

/*============================================================================*/
// CMFCRibbonGalleryIcon

class CMFCRibbonGalleryIcon : public CMFCRibbonButton
{
	friend class CMFCRibbonGallery;
	friend class CMFCRibbonPanel;
	friend class CMFCRibbonUndoButton;

	DECLARE_DYNCREATE(CMFCRibbonGalleryIcon)

public:
	CMFCRibbonGalleryIcon(CMFCRibbonGallery* pOwner = NULL,
		int nIndex = 0);

public:
	BOOL IsFirst() const;
	BOOL IsLast() const;

	int GetIndex() const { return m_nIndex; }

	BOOL IsFirstInRow() const { return m_bIsFirstInRow; }
	BOOL IsLastInRow() const { return m_bIsLastInRow; }

	BOOL IsFirstInColumn() const { return m_bIsFirstInColumn; }
	BOOL IsLastInColumn() const { return m_bIsLastInColumn; }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

protected:
	virtual void OnDraw(CDC* pDC);
	virtual void OnClick(CPoint point);
	virtual void OnLButtonDown(CPoint point);
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual CSize GetRegularSize(CDC* pDC);
	virtual CSize GetCompactSize(CDC* pDC);
	virtual BOOL IsAutoRepeatMode(int& nDelay /* ms */) const;
	virtual BOOL OnAutoRepeat();
	virtual BOOL CanBeAddedToQuickAccessToolBar() const { return TRUE; }
	virtual BOOL OnAddToQAToolbar(CMFCRibbonQuickAccessToolBar& qat);

	virtual UINT GetQuickAccessToolBarID() const
	{
		if (m_pOwner != NULL)
		{
			ASSERT_VALID(m_pOwner);
			return m_pOwner->GetID();
		}

		return m_nID;
	}

	virtual CString GetToolTipText() const;
	virtual CString GetDescription() const;
	virtual void OnHighlight(BOOL bHighlight);

	virtual CWnd* GetParentWnd() const
	{
		ASSERT_VALID(this);

		if (m_pOwner != NULL)
		{
			ASSERT_VALID(m_pOwner);
			return m_pOwner->GetParentWnd();
		}

		return CMFCRibbonButton::GetParentWnd();
	}

	virtual BOOL IsGalleryIcon() const
	{
		ASSERT_VALID(this);

		if (m_pOwner != NULL)
		{
			ASSERT_VALID(m_pOwner);
			return !m_pOwner->IsItemMenuLook();
		}

		return TRUE;
	}

	CMFCRibbonGallery*	m_pOwner;

	int  m_nIndex;
	BOOL m_bIsFirstInRow;
	BOOL m_bIsLastInRow;
	BOOL m_bIsFirstInColumn;
	BOOL m_bIsLastInColumn;
};

/*============================================================================*/
// CMFCRibbonGalleryMenuButton

class CMFCRibbonGalleryMenuButton : public CMFCToolBarMenuButton
{
	DECLARE_DYNCREATE(CMFCRibbonGalleryMenuButton)

// Construction
protected:
	CMFCRibbonGalleryMenuButton();

public:
	CMFCRibbonGalleryMenuButton(UINT uiID, int iImage, LPCTSTR lpszText, CMFCToolBarImages& imagesPalette);
	CMFCRibbonGalleryMenuButton(UINT uiID, int iImage, LPCTSTR lpszText, UINT uiImagesPaletteResID = 0, int cxPaletteImage = 0);

	virtual ~CMFCRibbonGalleryMenuButton();

protected:
	void CommonInit();

// Attributes:
public:
	CMFCRibbonGallery& GetPalette() { return m_paletteButton; }

protected:
	CMFCRibbonGallery m_paletteButton;

// Overrides:
public:
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual CMFCPopupMenu* CreatePopupMenu();
	virtual BOOL IsEmptyMenuAllowed() const { return TRUE; }
	virtual BOOL HasButton() const { return TRUE; }
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
