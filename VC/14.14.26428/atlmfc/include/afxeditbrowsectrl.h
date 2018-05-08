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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCEditBrowseCtrl window

class CMFCEditBrowseCtrl : public CEdit
{
	DECLARE_DYNAMIC(CMFCEditBrowseCtrl)

// Construction
public:
	CMFCEditBrowseCtrl();

// Attributes
public:
	enum BrowseMode
	{
		BrowseMode_None,
		BrowseMode_Default,
		BrowseMode_File,
		BrowseMode_Folder,
	};

	CMFCEditBrowseCtrl::BrowseMode GetMode() const { return m_Mode; }

protected:
	int  m_nBrowseButtonWidth;
	BOOL m_bIsButtonPressed;
	BOOL m_bIsButtonHighlighted;
	BOOL m_bIsButtonCaptured;
	BOOL m_bDefaultImage;

	CRect   m_rectBtn;
	CSize   m_sizeImage;
	CString m_strDefFileExt;
	CString m_strFileFilter;
	DWORD m_dwFileDialogFlags;
	CString m_strBrowseFolderTitle;
	UINT    m_ulBrowseFolderFlags;

	BrowseMode m_Mode;
	CImageList m_ImageBrowse;

// Operations
public:
	void EnableBrowseButton(BOOL bEnable = TRUE);
	void EnableFileBrowseButton(LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFilter = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	void EnableFolderBrowseButton(LPCTSTR lpszBrowseFolderTitle = NULL, UINT ulBrowseFolderFlags = BIF_RETURNONLYFSDIRS);

	void SetBrowseButtonImage(HICON hIcon, BOOL bAutoDestroy = TRUE);
	void SetBrowseButtonImage(HBITMAP hBitmap, BOOL bAutoDestroy = TRUE);
	void SetBrowseButtonImage(UINT uiBmpResId);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnBrowse();
	virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsButtonHot);
	virtual void OnChangeLayout();
	virtual void OnAfterUpdate();
	/// <summary>
	/// Called by the framework when an illegal file name was entered in the edit control.</summary>
	/// <param name="strFileName">Specifies the illegal file name.</param>
	/// <returns> Should return FALSE if this file name can't be passed further to the file dialog. In this case a focus set back to the edit control
	/// and the user should continue editing. The default implementation displays a message box telling the user about illegal file name and returns FALSE.
	/// You can override this method, correct the file name and return TRUE for further processing.</returns>
	virtual BOOL OnIllegalFileName(CString& strFileName);

// Implementation
public:
	virtual ~CMFCEditBrowseCtrl();

protected:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void SetInternalImage();
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
