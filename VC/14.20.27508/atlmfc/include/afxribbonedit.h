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

class CMFCRibbonRichEditCtrl;
class CMFCRibbonSpinButtonCtrl;

class CMFCRibbonEdit : public CMFCRibbonButton
{
	friend class CMFCRibbonRichEditCtrl;

	DECLARE_DYNCREATE(CMFCRibbonEdit)

// Construction:
public:
	CMFCRibbonEdit(UINT nID, int nWidth, LPCTSTR lpszLabel = NULL, int nImage = -1);
	virtual ~CMFCRibbonEdit();

protected:
	CMFCRibbonEdit();

// Attributes:
public:
	CString GetEditText() const { return m_strEdit; };

	void SetEditText(CString strText);

	void EnableSpinButtons(int nMin, int nMax);

	void SetTextAlign(int nAlign); // ES_LEFT, ES_CENTER or ES_RIGHT
	int GetTextAlign() const { return m_nAlign; }

	int GetWidth(BOOL bInFloatyMode = FALSE) const { return bInFloatyMode ? m_nWidthFloaty : m_nWidth; }
	void SetWidth(int nWidth, BOOL bInFloatyMode = FALSE);

// Overrides
public:
	virtual BOOL HasLargeMode() const { return FALSE; }
	virtual BOOL HasCompactMode() const { return TRUE; }

	virtual CSize GetIntermediateSize(CDC* pDC);
	virtual CSize GetCompactSize(CDC* pDC);

	virtual void Redraw();
	virtual void OnHighlight(BOOL bHighlight);
	virtual void OnSetFocus(BOOL bSet);
	virtual BOOL PreLMouseDown(CPoint point);

	virtual BOOL IsHighlighted() const { return m_bIsHighlighted || m_bIsEditFocused; }
	virtual BOOL HasFocus() const { return m_bIsEditFocused; }
	virtual BOOL HasSpinButtons() const { return m_bHasSpinButtons; }

	virtual int GetRangeMin() const { return m_nMin; }
	virtual int GetRangeMax() const { return m_nMax; }

	virtual void OnDraw(CDC* pDC);
	virtual void OnDrawOnList(CDC* pDC, CString strText, int nTextOffset, CRect rect, BOOL bIsSelected, BOOL bHighlighted);

	virtual void OnDrawLabelAndImage(CDC* pDC);

	virtual void OnLButtonDown(CPoint point);
	virtual void OnLButtonUp(CPoint point);

	virtual void CopyFrom(const CMFCRibbonBaseElement& src);

	virtual BOOL CanBeStretched() { return FALSE; }

	virtual void OnAfterChangeRect(CDC* pDC);
	virtual void OnShow(BOOL bShow);

	virtual CMFCRibbonRichEditCtrl* CreateEdit(CWnd* pWndParent, DWORD dwEditStyle);

	virtual void OnEnable(BOOL bEnable);
	virtual void DropDownList() {}
	virtual BOOL OnKey(BOOL bIsMenuKey);

	virtual void OnRTLChanged(BOOL bIsRTL);
	virtual void DestroyCtrl();

	virtual BOOL SetACCData(CWnd* pParent, CAccessibilityData& data);

// Operations:
protected:
	BOOL CreateSpinButton(CMFCRibbonRichEditCtrl* pWndEdit, CWnd* pWndParent);
	void CommonInit();
	void RepositionRibbonEditCtrl();


// Attributes:
protected:
	int  m_nWidth;
	int  m_nWidthFloaty;
	int  m_nMin;
	int  m_nMax;
	int  m_nAlign;
	int  m_nLabelImageWidth;

	BOOL m_bHasDropDownList;
	BOOL m_bHasSpinButtons;
	BOOL m_bIsEditFocused;
	BOOL m_bNotifyCommand;

	CString m_strEdit;
	CMFCRibbonRichEditCtrl* m_pWndEdit;
	CMFCRibbonSpinButtonCtrl* m_pWndSpin;
};

/*============================================================================*/
// CMFCRibbonRichEditCtrl

class CMFCRibbonRichEditCtrl : public CRichEditCtrl
{
	friend class CMFCRibbonEdit;

	DECLARE_DYNAMIC(CMFCRibbonRichEditCtrl)

// Construction
public:
	CMFCRibbonRichEditCtrl(CMFCRibbonEdit& edit);

// Attributes
public:
	CMFCRibbonEdit& GetOwnerRibbonEdit()
	{
		return m_edit;
	}

protected:
	CMFCRibbonEdit& m_edit;
	BOOL m_bTracked;
	BOOL m_bIsHighlighted;
	BOOL m_bIsContextMenu;
	CString m_strOldText;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCRibbonRichEditCtrl();

protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnChange();

	DECLARE_MESSAGE_MAP()

	BOOL ProcessClipboardAccelerators(UINT nChar);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
