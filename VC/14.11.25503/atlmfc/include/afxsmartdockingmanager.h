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

#include "afxsmartdockingguide.h"
#include "afxsmartdockinghighlighterwnd.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CSmartDockingManager : public CObject
{
public:
	CSmartDockingManager();
	virtual ~CSmartDockingManager();

	// create/destroy the manager
	void Create(CWnd* pwndOwner, CRuntimeClass* prtMarker = NULL, CRuntimeClass* prtCentralGroup = NULL);
	void Destroy();

	// start/stop smart-docking action
	void Start(CWnd* pDockingWnd);
	void Stop();

	// show/hide smart-docking markers (typically while Ctrl-key is held)
	void Show(BOOL bShow = TRUE);

	// get currently highlighted marker number
	CSmartDockingStandaloneGuide::SDMarkerPlace GetHighlightedGuideNo() const { return m_nHiliteSideNo; }

	BOOL IsStarted() { return m_bCreated && m_bStarted; }

	// react to mouse move
	void OnMouseMove(CPoint point);

	// react to mainframe's position change
	void OnPosChange();

	// set owner's outer docking rectangle (in screen coords)
	void SetOuterRect(CRect rcOuter);

	// move central group to new position
	// middle marker: nMiddleIsOn == -1 no change, 0 off, 1 on
	void MoveCentralGroup(CRect rect, int nMiddleIsOn = -1, DWORD dwEnabledAlignment = CBRS_ALIGN_ANY);
	void ShowCentralGroup(BOOL bShow = TRUE, DWORD dwEnabledAlignment = CBRS_ALIGN_ANY);

	// marker place
	void ShowPlaceAt(CRect rect);
	void HidePlace();
	void ShowTabbedPlaceAt(CRect rect, int nTabXOffset, int nTabWidth, int nTabHeight);

	CWnd* GetOwner() const { return m_pwndOwner; };

	void CauseCancelMode();

protected:
	// state flags
	BOOL   m_bStarted;
	BOOL   m_bCreated;
	BOOL   m_bShown;
	BOOL   m_bCentralGroupShown;
	CWnd*  m_pwndOwner;          // mainframe
	CWnd*  m_pDockingWnd;        // miniframe being docked
	DWORD  m_dwEnabledAlignment; // enabled alignment of embedded docking bar
	CRect  m_rcOuter;            // outer rectangle

	CSmartDockingHighlighterWnd m_wndPlaceMarker; // place marker
	CSmartDockingStandaloneGuide*        m_arMarkers [9];
	CSmartDockingGroupGuidesManager*  m_pCentralGroup;

	CSmartDockingStandaloneGuide::SDMarkerPlace m_nHiliteSideNo; // currently highlighted marker
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
