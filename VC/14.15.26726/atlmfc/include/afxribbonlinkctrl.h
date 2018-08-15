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

class CMFCRibbonLinkCtrl : public CMFCRibbonButton
{
	DECLARE_DYNCREATE(CMFCRibbonLinkCtrl)

// Construction:
public:
	CMFCRibbonLinkCtrl();
	CMFCRibbonLinkCtrl(UINT nID, LPCTSTR lpszText, LPCTSTR lpszLink);

	virtual ~CMFCRibbonLinkCtrl();

// Operations:
public:
	BOOL OpenLink();

// Overrides
public:
	virtual CSize GetRegularSize(CDC* pDC);
	virtual CSize GetCompactSize(CDC* pDC);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnDrawMenuImage(CDC* /*pDC*/, CRect /*rect*/) { return FALSE; }
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual CString GetToolTipText() const { return GetLink(); }

	virtual void OnMouseMove(CPoint point);

	virtual void OnSetIcon();
	virtual BOOL IsDrawTooltipImage() const { return FALSE; }
	
	LPCTSTR GetLink() const { return m_strLink; }
	void SetLink(LPCTSTR lpszLink);

protected:
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);
	virtual void OnAccDefaultAction();

// Attributes:
protected:
	CString m_strLink;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
