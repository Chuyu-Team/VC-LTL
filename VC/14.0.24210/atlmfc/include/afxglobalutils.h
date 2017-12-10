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

class CDockingManager;
class CBasePane;
class CDockablePane;
class CPaneContainerManager;

class CGlobalUtils
{
public:
	CGlobalUtils();
	virtual ~CGlobalUtils();

	BOOL CheckAlignment(CPoint point, CBasePane* pBar, int nSensitivity, const CDockingManager* pDockManager,
		BOOL bOuterEdge, DWORD& dwAlignment, DWORD dwEnabledDockBars = CBRS_ALIGN_ANY, LPCRECT lpRectBounds = NULL) const;

	CDockingManager* GetDockingManager(CWnd* pWnd);
	
	void FlipRect(CRect& rect, int nDegrees);
	DWORD GetOppositeAlignment(DWORD dwAlign);
	void SetNewParent(CObList& lstControlBars, CWnd* pNewParent, BOOL bCheckVisibility = TRUE);
	void CalcExpectedDockedRect(CPaneContainerManager& barContainerManager, CWnd* pWndToDock,
		CPoint ptMouse, CRect& rectResult, BOOL& bDrawTab, CDockablePane** ppTargetBar);
	BOOL GetPaneAndAlignFromPoint(CPaneContainerManager& barContainerManager, CPoint pt,
		CDockablePane** ppTargetControlBar, DWORD& dwAlignment, BOOL& bTabArea, BOOL& bCaption);
	void ForceAdjustLayout(CDockingManager* pDockManager, BOOL bForce = FALSE, BOOL bForceInvisible = FALSE);
	BOOL CanPaneBeInFloatingMultiPaneFrameWnd(CWnd* pWnd) const;
	void AdjustRectToWorkArea(CRect& rect, CRect* pRectDelta = NULL);
	BOOL CanBeAttached(CWnd* pWnd) const;

	BOOL StringFromCy(CString& str, CY& cy);
	BOOL CyFromString(CY& cy, LPCTSTR psz);
	BOOL StringFromDecimal(CString& str, DECIMAL& decimal);
	BOOL DecimalFromString(DECIMAL& decimal, LPCTSTR psz);

	HICON GetWndIcon(CWnd* pWnd);

	CSize GetSystemBorders(DWORD dwStyle);
	CSize GetSystemBorders(CWnd *pWnd);

	BOOL m_bDialogApp;
	BOOL m_bIsDragging;
};

AFX_IMPORT_DATA extern CGlobalUtils afxGlobalUtils;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
