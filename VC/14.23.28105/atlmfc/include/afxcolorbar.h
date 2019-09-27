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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#include "afxcontrolbarutil.h"
#include "afxpopupmenubar.h"

class CMFCColorButton;
class CMFCPropertyGridCtrl;
class CMFCRibbonColorButton;

/*============================================================================*/
// CMFCColorBar window

class CMFCColorBar : public CMFCPopupMenuBar
{
	friend class CMFCColorPopupMenu;
	friend class CMFCColorMenuButton;
	friend class CMFCColorButton;
	friend class CMFCToolBarColorButton;
	friend class CMFCImageEditorDialog;
	friend class CMFCPropertyGridColorProperty;
	friend class CMFCRibbonColorButton;

	DECLARE_SERIAL(CMFCColorBar)

// Construction
public:
	CMFCColorBar();

	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID,
		CPalette* pPalette = NULL, // Palette color, NULL - standard 20 colors
		int nColumns = 0,          // Number of columns, -1 - default
		int nRowsDockHorz = 0,     // Number of rows for horz. docking
		int nColDockVert = 0);     // Number of columns for vert. docking

	virtual BOOL CreateControl(CWnd* pParentWnd, const CRect& rect, UINT nID, int nColumns = -1, CPalette* pPalette = NULL);

protected:
	CMFCColorBar(const CArray<COLORREF, COLORREF>& colors, COLORREF color, LPCTSTR lpszAutoColor, LPCTSTR lpszOtherColor, LPCTSTR lpszDocColors,
		CList<COLORREF,COLORREF>& lstDocColors, int nColumns, int nRowsDockHorz, int nColDockVert, COLORREF colorAutomatic, UINT nCommandID, CMFCColorButton* pParentBtn);
	CMFCColorBar(const CArray<COLORREF, COLORREF>& colors, COLORREF color, LPCTSTR lpszAutoColor, LPCTSTR lpszOtherColor, LPCTSTR lpszDocColors,
		CList<COLORREF,COLORREF>& lstDocColors, int nColumns, COLORREF colorAutomatic, UINT nCommandID, CMFCRibbonColorButton*	pParentRibbonBtn);
	CMFCColorBar(CMFCColorBar& src, UINT uiCommandID);

// Operations:
public:
	void SetDocumentColors(LPCTSTR lpszCaption, CList<COLORREF,COLORREF>& lstDocColors, BOOL bShowWhenDocked = FALSE);
	void ContextToSize(BOOL bSquareButtons = TRUE, BOOL bCenterButtons = TRUE);

// Attributes
public:
	void SetColor(COLORREF color);
	COLORREF GetColor() const { return m_ColorSelected; }
	static void __stdcall SetColorName(COLORREF color, const CString& strName) { m_ColorNames.SetAt(color, strName); }

	void EnableAutomaticButton(LPCTSTR lpszLabel, COLORREF colorAutomatic, BOOL bEnable = TRUE);
	void EnableOtherButton(LPCTSTR lpszLabel, BOOL bAltColorDlg = TRUE, BOOL bEnable = TRUE);

	UINT GetCommandID() const { return m_nCommandID; }
	void SetCommandID(UINT nCommandID) { m_nCommandID = nCommandID; }
	BOOL IsTearOff() const { return m_bIsTearOff; }

	void SetVertMargin(int nVertMargin);
	void SetHorzMargin(int nHorzMargin);

	int GetVertMargin() const { return m_nVertMargin; }
	int GetHorzMargin() { return m_nHorzMargin; }

	COLORREF GetHighlightedColor() const;

protected:
	int m_nNumColumns;
	int m_nNumRowsHorz;
	int m_nNumColumnsVert;
	int m_nHorzOffset;
	int m_nVertOffset;
	int m_nRowHeight;
	int m_nVertMargin;
	int m_nHorzMargin;

	BOOL m_bStdColorDlg;
	BOOL m_bIsEnabled;
	BOOL m_bIsTearOff;
	BOOL m_bShowDocColorsWhenDocked;
	BOOL m_bInternal;  // Used in customization

	UINT m_nCommandID;

	CArray<COLORREF, COLORREF> m_colors;
	CList<COLORREF,COLORREF>   m_lstDocColors;

	CSize               m_BoxSize;
	CPalette            m_Palette;
	COLORREF            m_ColorSelected; // (-1) - auto
	COLORREF            m_ColorAutomatic;
	CString             m_strAutoColor;
	CString             m_strOtherColor;
	CString             m_strDocColors;
	CMFCColorButton*    m_pParentBtn;
	CMFCPropertyGridCtrl*      m_pWndPropList;
	CMFCRibbonColorButton* m_pParentRibbonBtn;

	AFX_IMPORT_DATA static CMap<COLORREF,COLORREF,CString, LPCTSTR> m_ColorNames;

// Overrides
	virtual BOOL OnSendCommand(const CMFCToolBarButton* pButton);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void AdjustLocations();
	virtual CSize CalcSize(BOOL bVertDock);
	virtual void DoPaint(CDC* pDC);
	virtual BOOL OnKey(UINT nChar);
	virtual void OnChangeHot(int iHot);

	virtual void Serialize(CArchive& ar);
	virtual void ShowCommandMessageString(UINT uiCmdId);

	virtual BOOL AllowChangeTextLabels() const { return FALSE; }
	virtual BOOL AllowShowOnList() const { return TRUE; }

	virtual BOOL OpenColorDialog(const COLORREF colorDefault, COLORREF& colorRes);

	virtual void Rebuild();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCColorBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseLeave();

	DECLARE_MESSAGE_MAP()

	static int __stdcall InitColors(CPalette* pPalette, CArray<COLORREF, COLORREF>& arColors);
	static BOOL __stdcall CreatePalette(const CArray<COLORREF, COLORREF>& arColors, CPalette& palette);

	CSize GetColorGridSize(BOOL bVertDock) const;
	int GetExtraHeight(int nNumColumns) const;
	CPalette* SelectPalette(CDC* pDC);

	void SetPropList(CMFCPropertyGridCtrl* pWndList) { m_pWndPropList = pWndList; }
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
