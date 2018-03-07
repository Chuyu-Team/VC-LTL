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
#include "afxribbonbutton.h"
#include "afxtoolbarimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonStatusBarPane : public CMFCRibbonButton  
{
	friend class CMFCRibbonStatusBar;

	DECLARE_DYNCREATE(CMFCRibbonStatusBarPane)

// Construction
public:
	CMFCRibbonStatusBarPane();

	CMFCRibbonStatusBarPane(
		UINT     nCmdID,                     // Pane command id
		LPCTSTR  lpszText,                   // Pane label
		BOOL     bIsStatic = FALSE,          // Pane is static (non-clickable)
		HICON    hIcon = NULL,               // Pane icon
		LPCTSTR  lpszAlmostLargeText = NULL, // The almost large text in pane
		BOOL     bAlphaBlendIcon = FALSE);	 // The icon is 32 bit

	CMFCRibbonStatusBarPane(
		UINT     nCmdID,                    // Pane command id
		LPCTSTR  lpszText,                  // Pane label
		HBITMAP  hBmpAnimationList,         // Pane animation list
		int      cxAnimation = 16,          // Animation frame width
		COLORREF clrTrnsp= RGB(192,192,192),// Transparent color
		HICON    hIcon = NULL,              // Pane icon
		BOOL     bIsStatic = FALSE,         // Pane is static (non-clickable)
		BOOL     bAlphaBlendIcon = FALSE);	 // The icon is 32 bit

	CMFCRibbonStatusBarPane(
		UINT     nCmdID,                    // Pane command id
		LPCTSTR  lpszText,                  // Pane label
		UINT     uiAnimationListResID,      // Pane animation list
		int      cxAnimation = 16,          // Animation frame width
		COLORREF clrTrnsp= RGB(192,192,192),// Transparent color
		HICON    hIcon = NULL,              // Pane icon
		BOOL     bIsStatic = FALSE,         // Pane is static (non-clickable)
		BOOL     bAlphaBlendIcon = FALSE);	 // The icon is 32 bit

	virtual ~CMFCRibbonStatusBarPane();

// Attributes
public:
	// Is located on status bar extended (right) area?
	BOOL IsExtended() const { return m_bIsExtended; }

	LPCTSTR GetAlmostLargeText() const { return m_strAlmostLargeText; }
	void SetAlmostLargeText(LPCTSTR lpszAlmostLargeText)
	{
		m_strAlmostLargeText = (lpszAlmostLargeText == NULL) ? _T("") : lpszAlmostLargeText;
		CleanUpSizes();
	}

	int GetTextAlign() const { return m_nTextAlign; }
	void SetTextAlign(int nAlign) { m_nTextAlign = nAlign; }
	BOOL IsAnimation() const { return m_nAnimTimerID != 0; }

protected:
	BOOL m_bIsStatic;             // Pane is static (non-clickable)
	BOOL m_bIsExtended;           // Is located on status bar extended (right) area
	BOOL m_bIsTextTruncated;      // Is text truncated?
	int  m_nTextAlign;            // TA_LEFT, TA_CENTER or TA_RIGHT
	int  m_nAnimationIndex;       // Current animation frame number
	UINT m_nAnimTimerID;          // Animation timer ID
	UINT m_nAnimationDuration;    // Animation duaration
	DWORD m_dwAnimationStartTime; // Animation start time
	CString m_strAlmostLargeText; // The almost large text in pane
	CMFCToolBarImages m_AnimImages;  // List of animation frames

	static CMap<UINT,UINT,CMFCRibbonStatusBarPane*,CMFCRibbonStatusBarPane*> m_mapAnimations;
	static CCriticalSection m_CriticalSection; // For multi-theard applications

// Operations
public:
	void SetAnimationList(	HBITMAP hBmpAnimationList, int cxAnimation = 16, COLORREF clrTransp = RGB(192, 192, 192));
	BOOL SetAnimationList(	UINT uiAnimationListResID, int cxAnimation = 16, COLORREF clrTransp = RGB(192, 192, 192));

	void StartAnimation(UINT nFrameDelay = 500, UINT nDuration = -1); // ms
	void StopAnimation();

protected:
	void CommonInit();

	static VOID CALLBACK AnimTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

// Overrides
public:
	virtual COLORREF OnFillBackground(CDC* pDC);
	virtual void OnDrawBorder(CDC* /*pDC*/) {}

protected:
	virtual int GetTextOffset() const { return m_szMargin.cx / 2; }
	virtual void OnCalcTextSize(CDC* pDC);
	virtual int DrawRibbonText(CDC* pDC, const CString& strText, CRect rectText, UINT uiDTFlags, COLORREF clrText = (COLORREF)-1);
	virtual CSize GetIntermediateSize(CDC* pDC);
	virtual void OnDraw(CDC* pDC);

	virtual CString GetToolTipText() const;

	virtual BOOL CanBeStretched() { return TRUE; }

	virtual void CopyFrom(const CMFCRibbonBaseElement& src);
	virtual void OnFinishAnimation() {}

	virtual void OnRTLChanged(BOOL bIsRTL);

	virtual BOOL IsDrawTooltipImage() const { return m_AnimImages.GetCount() <= 0; }
	virtual void SetMargin(const CSize& /*sizeMargin*/) { m_szMargin = CSize(9, 0); }

	virtual BOOL HasLargeMode() const { return FALSE; }
	virtual BOOL HasIntermediateMode() const { return TRUE; }
	virtual BOOL HasCompactMode() const { return FALSE; }
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
