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

class CMFCRibbonSlider : public CMFCRibbonBaseElement
{
	DECLARE_DYNCREATE(CMFCRibbonSlider)

// Construction
public:
	CMFCRibbonSlider();
	CMFCRibbonSlider(UINT nID, int nWidth = 100);

	virtual ~CMFCRibbonSlider();

// Attributes
public:
	void SetRange(int nMin, int nMax);

	int GetRangeMin() const { return m_nMin; }
	int GetRangeMax() const { return m_nMax; }

	int GetWidth() const { return m_nWidth; }

	void SetPos(int nPos, BOOL bRedraw = TRUE);

	int GetPos() const { return m_nPos; }

	int GetZoomIncrement() const { return m_nZoomIncrement; }
	void SetZoomIncrement(int nZoomIncrement);

	void SetZoomButtons(BOOL bSet = TRUE);
	BOOL HasZoomButtons() const { return m_bZoomButtons; }

// Operations
protected:
	void CommonInit();
	void SetThumbRect();
	int GetPosFromPoint(int x);
	int GetHitTest(CPoint point) const;

	void NotifyCommand();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);
	virtual CSize GetRegularSize(CDC* pDC);

protected:
	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void OnAfterChangeRect(CDC* pDC);

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);
	virtual void OnMouseMove(CPoint point);

	virtual BOOL IsAutoRepeatMode(int& nDelay /* ms */) const;
	virtual BOOL OnAutoRepeat();

	virtual void OnDrawOnList(CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

// Attributes
protected:
	int  m_nMin;
	int  m_nMax;
	int  m_nPos;
	int  m_nZoomIncrement;
	int  m_nWidth;
	int  m_nHighlighted;	// -1 - none, 0 - thumb, 1 -Zoom In, 2 - Zoom Out
	int  m_nPressed;		// -1 - none, 0 - thumb, 1 -Zoom In, 2 - Zoom Out
	BOOL m_bZoomButtons;

	CRect m_rectZoomOut;
	CRect m_rectZoomIn;
	CRect m_rectSlider;
	CRect m_rectThumb;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
