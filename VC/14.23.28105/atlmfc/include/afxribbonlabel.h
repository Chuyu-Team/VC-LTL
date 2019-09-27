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

#include "afxribbonbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonLabel : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonLabel)

// Construction:
public:
	CMFCRibbonLabel(LPCTSTR lpszText, BOOL bIsMultiLine = FALSE);
	virtual ~CMFCRibbonLabel();

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

protected:
	CMFCRibbonLabel();

// Overrides
protected:
	virtual void OnDraw(CDC* pDC);
	virtual void OnCalcTextSize(CDC* pDC);
	virtual void OnAfterChangeRect(CDC* pDC);
	virtual void OnLButtonUp(CPoint /*point*/) {}
	virtual CSize GetIntermediateSize(CDC* pDC);
	
	virtual CSize GetRegularSize(CDC* pDC) { return GetIntermediateSize(pDC); }
	virtual CSize GetCompactSize(CDC* pDC) { return GetIntermediateSize(pDC); }

	virtual BOOL IsTabStop() const { return FALSE; }
	virtual BOOL CanBeAddedToQuickAccessToolBar() const { return FALSE; }
	virtual int AddToListBox(CMFCRibbonCommandsListBox* /*pWndListBox*/, BOOL /*bDeep*/) { return -1; }
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
