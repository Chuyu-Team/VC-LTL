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

#include "afxbutton.h"
#include "afxdesktopalertdialog.h"
#include "afxpopupmenu.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCDesktopAlertWndButton window

extern AFX_IMPORT_DATA UINT AFX_WM_ON_CLOSEPOPUPWINDOW;

class CMFCDesktopAlertWndButton : public CMFCButton
{
	friend class CMFCDesktopAlertWnd;

public:
	CMFCDesktopAlertWndButton()
	{
		m_bIsCaptionButton = FALSE;
		m_bIsCloseButton = FALSE;
	}

	BOOL IsCaptionButton() const { return m_bIsCaptionButton; }
	BOOL IsCloseButton() const { return m_bIsCloseButton; }

protected:
	virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);

	BOOL m_bIsCaptionButton;
	BOOL m_bIsCloseButton;
};

/*============================================================================*/
// CMFCDesktopAlertWnd window

class CMFCDesktopAlertWnd : public CWnd
{
	friend CMFCDesktopAlertDialog;

	DECLARE_DYNAMIC(CMFCDesktopAlertWnd)

// Construction
public:
	CMFCDesktopAlertWnd();

// Attributes
public:

	CMFCPopupMenu::ANIMATION_TYPE GetAnimationType() { return m_AnimationType; }
	void SetAnimationType(CMFCPopupMenu::ANIMATION_TYPE type) { m_AnimationType = type; }

	void SetAnimationSpeed(UINT nSpeed);
	UINT GetAnimationSpeed() const { return m_AnimationSpeed; }

	void SetTransparency(BYTE nTransparency)
	{
		if (nTransparency < 0 || nTransparency > 255)
		{
			ASSERT(FALSE);
			return;
		}
		m_nTransparency = nTransparency;
	}
	BYTE GetTransparency() const { return m_nTransparency; }

	void SetSmallCaption(BOOL bSmallCaption = TRUE) { m_bSmallCaption = bSmallCaption; }
	BOOL HasSmallCaption() const { return m_bSmallCaption; }

	// times in milliseconds
	void SetAutoCloseTime(int nTime) { m_nAutoCloseTime = nTime; }
	int GetAutoCloseTime() const { return m_nAutoCloseTime; }

	CPoint GetLastPos() const { return 	m_ptLastPos; }

protected:
	CWnd*  m_pWndOwner;
	CPoint m_ptLastPos;
	CPoint m_ptStartMove;

	BOOL m_bMoving;
	BOOL m_bIsActive;
	BOOL m_bHasCloseButton;
	BOOL m_bSmallCaption;

	UINT m_uiDlgResID;
	int  m_nAutoCloseTime; // milliseconds
	int  m_nBtnMarginVert;
	int  m_nBtnMarginHorz;
	BYTE m_nTransparency; // (0..255)

	HMENU m_hMenu;

	CMFCDesktopAlertDialog*      m_pWndDlg;
	CMFCDesktopAlertWndButton m_btnClose;
	CMFCDesktopAlertWndButton m_btnMenu;

	//----------------------
	// Animation attributes:
	//----------------------
	CMFCPopupMenu::ANIMATION_TYPE m_AnimationType;

	UINT m_AnimationSpeed;
	UINT m_nAnimationAlpha;
	BOOL m_bAnimationIsDone;
	BOOL m_bIsAnimRight;
	BOOL m_bIsAnimDown;
	BOOL m_bFadeOutAnimation;
	int  m_iFadePercent;
	int  m_iFadeStep;
	int  m_nAnimStepX;
	int  m_nAnimStepY;

	CSize   m_AnimSize;
	CSize   m_FinalSize;
	CBitmap m_bmpScreenSrc;
	CBitmap m_bmpScreenDst;
	CBitmap m_bmpScreenTmp;

	COLORREF* m_cFadeSrcBits;
	COLORREF* m_cFadeDstBits;
	COLORREF* m_cFadeTmpBits;

// Operations
public:
	BOOL ProcessCommand(HWND hwnd);

protected:
	CRect GetCaptionRect();
	void StartAnimation(BOOL bShow = TRUE);
	void DrawAnimation(CDC* pPaintDC);

	BOOL CommonCreate(CPoint ptPos, CMFCDesktopAlertWndInfo* pParams = NULL);

	void StartWindowMove();

	CMFCPopupMenu::ANIMATION_TYPE GetActualAnimationType()
	{
		if (m_AnimationType == CMFCPopupMenu::SYSTEM_DEFAULT_ANIMATION)
		{
			if (GetGlobalData()->m_bMenuAnimation)
			{
				return GetGlobalData()->m_bMenuFadeEffect ? CMFCPopupMenu::FADE : CMFCPopupMenu::SLIDE;
			}
			else
			{
				return CMFCPopupMenu::NO_ANIMATION;
			}
		}

		return m_AnimationType;
	}

// Overrides
public:
	virtual BOOL Create(CWnd* pWndOwner, UINT uiDlgResID, HMENU hMenu = NULL, CPoint ptPos = CPoint (-1, -1), CRuntimeClass* pRTIDlgBar = RUNTIME_CLASS(CMFCDesktopAlertDialog));
	virtual BOOL Create(CWnd* pWndOwner, CMFCDesktopAlertWndInfo& params, HMENU hMenu = NULL, CPoint ptPos = CPoint (-1, -1));
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	virtual CSize GetDialogSize();
	virtual BOOL OnBeforeShow(CPoint& /*ptPos*/) { return TRUE; }
	virtual void OnDraw(CDC* pDC);
	virtual int GetCaptionHeight();

	virtual BOOL OnClickLinkButton(UINT /*uiCmdID*/) { return FALSE; }

// Implementation
public:
	virtual ~CMFCDesktopAlertWnd();

protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNcDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
