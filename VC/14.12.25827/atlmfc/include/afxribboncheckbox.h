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

class CMFCRibbonCheckBox : public CMFCRibbonButton  
{
	DECLARE_DYNCREATE(CMFCRibbonCheckBox)

// Construction
public:
	CMFCRibbonCheckBox();
	CMFCRibbonCheckBox(UINT nID, LPCTSTR lpszText);

	virtual ~CMFCRibbonCheckBox();

// Overrides
public:
	virtual CSize GetIntermediateSize(CDC* pDC);

	virtual CSize GetCompactSize(CDC* pDC) { return GetIntermediateSize(pDC); }
	virtual CSize GetRegularSize(CDC* pDC) { return GetIntermediateSize(pDC); }

	virtual void OnDraw(CDC* pDC);
	virtual void OnDrawOnList(CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual BOOL OnDrawMenuImage(CDC* /*pDC*/, CRect /*rect*/) { return FALSE; }
	virtual BOOL IsDrawTooltipImage() const { return FALSE; }

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data)
	{
		CMFCRibbonButton::SetACCData(pParent, data);
		data.m_strAccDefAction = IsChecked() ? _T("Uncheck") : _T("Check");
		data.m_nAccRole = ROLE_SYSTEM_CHECKBUTTON;

		return TRUE;
	}
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
