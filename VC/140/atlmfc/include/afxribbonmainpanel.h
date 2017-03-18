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

#include "afxribbonpanel.h"
#include "afxribbonbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonMainPanelButton;

class CMFCRibbonMainPanel : public CMFCRibbonPanel
{
	friend class CMFCRibbonApplicationButton;
	friend class CMFCRibbonCollector;

	DECLARE_DYNCREATE(CMFCRibbonMainPanel)

// Construction
public:
	CMFCRibbonMainPanel();
	virtual ~CMFCRibbonMainPanel();

// Operations
public:
	virtual void Add(CMFCRibbonBaseElement* pElem);
	void AddToBottom(CMFCRibbonMainPanelButton* pElem);

	void AddRecentFilesList(LPCTSTR lpszLabel, int nWidth = 300);
	void AddToRight(CMFCRibbonBaseElement* pElem, int nWidth = 300);

protected:
	int GetMenuElements() const;
	void DrawApplicationButton(CDC* pDC, CWnd* pWnd);

// Overrides
protected:

	virtual void RecalcWidths(CDC* pDC, int nHeight);
	virtual void Reposition(CDC* pDC, const CRect& rect);
	virtual void DoPaint(CDC* pDC);

	virtual BOOL IsFixedSize() const { return TRUE; }

	virtual void CopyFrom(CMFCRibbonPanel& src);
	virtual BOOL GetPreferedMenuLocation(CRect& rect);

	virtual BOOL IsMainPanel() const { return TRUE; }

	virtual void OnDrawMenuBorder(CDC* pDC, CMFCRibbonPanelMenuBar* pMenuBar);
	virtual CMFCRibbonBaseElement* MouseButtonDown(CPoint point);

// Attributes:
public:
	CRect GetCommandsFrame() const;

protected:
	int   m_nBottomElementsNum;
	int   m_nTopMargin;
	int   m_nRightPaneWidth;
	CRect m_rectMenuElements;

	CMFCRibbonApplicationButton*  m_pMainButton;
	CMFCRibbonBaseElement* m_pElemOnRight;
};

class CMFCRibbonMainPanelButton : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonMainPanelButton)

// Construction
public:
	CMFCRibbonMainPanelButton();

	CMFCRibbonMainPanelButton(UINT nID, LPCTSTR lpszText, int nSmallImageIndex);
	CMFCRibbonMainPanelButton(UINT nID, LPCTSTR lpszText, HICON hIcon);

	virtual ~CMFCRibbonMainPanelButton();

// Overrides
protected:
	virtual COLORREF OnFillBackground(CDC* pDC);
	virtual void OnDrawBorder(CDC* pDC);
};

class CMFCRibbonRecentFilesList : public CMFCRibbonButtonsGroup
{
	DECLARE_DYNCREATE(CMFCRibbonRecentFilesList)

public:
	CMFCRibbonRecentFilesList(LPCTSTR lpszLabel = NULL)
	{
		SetText(lpszLabel == NULL ? _T("") : lpszLabel);
	}

	void FillList();

protected:
	virtual void OnAfterChangeRect(CDC* pDC);
	virtual CSize GetRegularSize(CDC* pDC);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnMenuKey(UINT nUpperChar);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
