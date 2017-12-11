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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CDockingManager;
class CPane;
class CDockablePane;
class CBaseTabbedPane;

class CMFCDragFrameImpl
{
public:
	CMFCDragFrameImpl();
	virtual ~CMFCDragFrameImpl();

	void Init(CWnd* pDraggedWnd);
	void MoveDragFrame(BOOL bForceMove = FALSE); 
	void EndDrawDragFrame(BOOL bClearInternalRects = TRUE);

	void PlaceTabPreDocking(CBaseTabbedPane* pTabbedBar, BOOL bFirstTime);
	void PlaceTabPreDocking(CWnd* pCBarToPlaceOn);
	void RemoveTabPreDocking(CDockablePane* pOldTargetBar = NULL);

	CPoint  m_ptHot;
	CRect   m_rectDrag;
	CRect   m_rectExpectedDocked;
	BOOL    m_bDockToTab;
	BOOL    m_bDragStarted;
	int     m_nInsertedTabID;

	CDockablePane* m_pFinalTargetBar;
	CDockablePane* m_pOldTargetBar;

	void ResetState();

protected:
	void DrawDragFrame(LPCRECT lpRectOld, LPCRECT lpRectNew, BOOL bFirstTime = FALSE, int nNewThickness = 4, int nOldThickness = 4);
	void DrawFrameTab(CDockablePane* pTargetBar, BOOL bErase);

	int                 m_nOldThickness;
	CWnd*               m_pDraggedWnd;
	CDockingManager*       m_pDockManager;
	CDockablePane* m_pTargetBar;
	CDockablePane* m_pWndDummy;
	BOOL                m_bFrameTabDrawn;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
