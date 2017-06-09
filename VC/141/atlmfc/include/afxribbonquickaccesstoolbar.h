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
#include "afxribbonbuttonsgroup.h"
#include "afxribbonbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCRibbonQuickAccessToolBar class

class CMFCRibbonQuickAccessCustomizeButton;

class CMFCRibbonQuickAccessToolBarDefaultState
{
	friend class CMFCRibbonQuickAccessToolBar;
	friend class CMFCRibbonBar;
	friend class CMFCRibbonCollector;

public:
	CMFCRibbonQuickAccessToolBarDefaultState();

	void AddCommand(UINT uiCmd, BOOL bIsVisible = TRUE);
	void RemoveAll();

	void CopyFrom(const CMFCRibbonQuickAccessToolBarDefaultState& src);

protected:
	CArray<UINT,UINT> m_arCommands;
	CArray<BOOL,BOOL> m_arVisibleState;
};

class CMFCRibbonQuickAccessToolBar : public CMFCRibbonButtonsGroup
{
	DECLARE_DYNCREATE(CMFCRibbonQuickAccessToolBar)

	friend class CMFCRibbonBar;
	friend class CMFCRibbonBaseElement;
	friend class CMFCRibbonCustomizePropertyPage;
	friend class CMFCRibbonCollector;

public:
	CMFCRibbonQuickAccessToolBar();
	virtual ~CMFCRibbonQuickAccessToolBar();

protected:
	void SetCommands(CMFCRibbonBar* pRibbonBar, const CList<UINT,UINT>& lstCommands, LPCTSTR lpszToolTip);
	void SetCommands(CMFCRibbonBar* pRibbonBar, const CList<UINT,UINT>& lstCommands, CMFCRibbonQuickAccessCustomizeButton* pCustButton);

	void GetCommands(CList<UINT,UINT>& lstCommands);
	void GetDefaultCommands(CList<UINT,UINT>& lstCommands);
	void ReplaceCommands(const CList<UINT,UINT>& lstCommands);
	void ResetCommands();

	int GetActualWidth() const;

	virtual CSize GetRegularSize(CDC* pDC);
	virtual void OnAfterChangeRect(CDC* pDC);

	virtual BOOL IsQuickAccessToolBar() const { return TRUE; }

	// Accessibility:
	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	virtual BOOL OnSetAccData(long lVal);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

	void Add(CMFCRibbonBaseElement* pElem);
	void Remove(CMFCRibbonBaseElement* pElem);

	void RebuildHiddenItems();
	CRect GetCommandsRect() const { return m_rectCommands; }
	void RebuildKeys();

protected:
	CMFCRibbonQuickAccessToolBarDefaultState m_DefaultState;
	CRect                  m_rectCommands;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
