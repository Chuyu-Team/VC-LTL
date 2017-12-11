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

#include "afxcontrolbarutil.h"
#include "afxdialogex.h"
#include "afxcolorbar.h"
#include "afximagepaintarea.h"
#include "afxtoolbar.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCImageEditorPaletteBar : public CMFCToolBar
{
	virtual BOOL IsButtonExtraSizeAvailable() const
	{
		return FALSE;
	}

	virtual int GetRowHeight() const
	{
		return GetButtonSize().cy;
	}
};

/*============================================================================*/
// CMFCImageEditorDialog dialog

class CMFCImageEditorDialog : public CDialogEx
{
	friend CMFCImagePaintArea;

// Construction
public:
	CMFCImageEditorDialog(CBitmap* pBitmap, CWnd* pParent = NULL, int nBitsPixel = -1);   // standard constructor

// Dialog Data
	enum { IDD = IDD_AFXBARRES_IMAGE_EDITOR };
	CStatic m_wndColorPickerLocation;
	CStatic m_wndPaletteBarLocation;
	CStatic m_wndPreview;
	CMFCImagePaintArea m_wndLargeDrawArea;

// Operations:
protected:
	BOOL OnPickColor(COLORREF color);

	void SetMode(CMFCImagePaintArea::IMAGE_EDIT_MODE mode) { m_wndLargeDrawArea.SetMode(mode); }
	CMFCImagePaintArea::IMAGE_EDIT_MODE GetMode() const { return m_wndLargeDrawArea.GetMode(); }

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnToolClear();
	afx_msg void OnToolCopy();
	afx_msg void OnToolPaste();
	afx_msg void OnUpdateToolPaste(CCmdUI* pCmdUI);
	afx_msg void OnToolEllipse();
	afx_msg void OnToolFill();
	afx_msg void OnToolLine();
	afx_msg void OnToolPen();
	afx_msg void OnToolPick();
	afx_msg void OnToolRect();
	afx_msg void OnUpdateToolEllipse(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolFill(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolPen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolPick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolRect(CCmdUI* pCmdUI);
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM);
	afx_msg void OnColors();

	DECLARE_MESSAGE_MAP()

// Attributes:
	CBitmap* m_pBitmap;
	CSize    m_sizeImage;
	int      m_nBitsPixel;

	CRect m_rectPreviewFrame;
	CRect m_rectPreviewImage;

	CMFCColorBar m_wndColorBar;
	CMFCImageEditorPaletteBar m_wndPaletteBar;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
