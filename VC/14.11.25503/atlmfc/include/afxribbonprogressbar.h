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

class CMFCRibbonProgressBar : public CMFCRibbonBaseElement
{
	DECLARE_DYNCREATE(CMFCRibbonProgressBar)

public:
	CMFCRibbonProgressBar();
	CMFCRibbonProgressBar(UINT nID, int nWidth = 90, int nHeight = 22);

	virtual ~CMFCRibbonProgressBar();

// Attributes
public:
	void SetRange(int nMin, int nMax);

	int GetRangeMin() const { return m_nMin; }
	int GetRangeMax() const { return m_nMax; }

	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }

	void SetPos(int nPos, BOOL bRedraw = TRUE);
	int GetPos() const { return m_nPos; }

	void SetInfiniteMode(BOOL bSet = TRUE);
	BOOL IsInfiniteMode() const { return m_bInfiniteMode; }

// Operations
protected:
	void CommonInit();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);
	virtual CSize GetRegularSize(CDC* pDC);

protected:
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);

	virtual BOOL CanBeStretched() { return FALSE; }

	virtual void OnDrawOnList(CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);
	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

// Attributes
protected:
	int  m_nMin;
	int  m_nMax;
	int  m_nPos;
	int  m_nWidth;
	int  m_nHeight;
	BOOL m_bInfiniteMode;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
