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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarButton;
class CMFCToolBarImages;

/*============================================================================*/
// CMFCToolBarButtonsListButton window

class CMFCToolBarButtonsListButton : public CButton
{
// Construction
public:
	CMFCToolBarButtonsListButton();

// Operations
public:
	void SetImages(CMFCToolBarImages* pImages);
	void AddButton(CMFCToolBarButton* pButton);
	void RemoveButtons();

	CMFCToolBarButton* GetSelectedButton() const
	{
		return m_pSelButton;
	}

	BOOL SelectButton(int iImage);
	void EnableDragFromList(BOOL bEnable = TRUE)
	{
		m_bEnableDragFromList = bEnable;
	}

protected:
	CMFCToolBarButton* HitTest(POINT point) const;
	void SelectButton(CMFCToolBarButton* pButton);
	void RebuildLocations();
	void RedrawSelection();

// Attributes
protected:
	CObList         m_Buttons;    // CMFCToolBarButton list
	CMFCToolBarImages* m_pImages;
	CSize           m_sizeButton;
	CScrollBar      m_wndScrollBar;
	CMFCToolBarButton* m_pSelButton;
	int             m_iScrollOffset;
	int             m_iScrollTotal;
	int             m_iScrollPage;
	BOOL            m_bInited;
	BOOL            m_bEnableDragFromList;

// Overrides
public:
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

// Implementation
public:
	virtual ~CMFCToolBarButtonsListButton();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
