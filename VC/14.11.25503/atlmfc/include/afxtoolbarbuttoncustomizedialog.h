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

#include "afxtoolbarbuttonslistbutton.h"
#include "afxribbonres.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarImages;
class CUserTool;

/*============================================================================*/
// CMFCToolBarButtonCustomizeDialog dialog

class CMFCToolBarButtonCustomizeDialog : public CDialog
{
// Construction
public:
	CMFCToolBarButtonCustomizeDialog(CMFCToolBarButton* pButton, CMFCToolBarImages* pImages, CWnd* pParent = NULL, int iStartImage = 0, BOOL bMenuMode = FALSE); // standard constructor
	virtual ~CMFCToolBarButtonCustomizeDialog();

// Dialog Data
	enum { IDD = IDD_AFXBARRES_BUTTON_PROPS };
	CButton      m_wndDefautImageBtn;
	CButton      m_wndUserImageBtn;
	CStatic      m_wndDefaultImageArea;
	CEdit        m_wndButtonText;
	CButton      m_wndAddImage;
	CMFCToolBarButtonsListButton m_wndButtonList;
	CButton      m_wndEditImage;
	CString      m_strButtonText;
	CString      m_strButtonDescr;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	afx_msg void OnAddImage();
	afx_msg void OnEditImage();
	afx_msg void OnImageList();
	afx_msg void OnImage();
	afx_msg void OnImageText();
	afx_msg void OnText();
	afx_msg void OnUserImage();
	afx_msg void OnDefaultImage();
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
	virtual BOOL OnInitDialog();

// Operations:
protected:
	void RebuildImageList();
	void EnableControls();

// Attributes:
protected:
	CMFCToolBarButton* m_pButton;
	CMFCToolBarImages* m_pImages;

	int m_iStartImage;
	int m_iSelImage;

	BOOL m_bImage;
	BOOL m_bText;
	BOOL m_bMenuMode;
	BOOL m_bUserButton;

	CObList    m_Buttons;
	CRect      m_rectDefaultImage;
	CString    m_strAccel;
	CUserTool* m_pUserTool;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
