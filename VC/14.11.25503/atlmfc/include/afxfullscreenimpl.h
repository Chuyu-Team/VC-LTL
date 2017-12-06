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

class CMFCToolBar;
class CFrameImpl;

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CFullScreenImpl
{
public:
	CFullScreenImpl(CFrameImpl* pFrameImpl);
	virtual ~CFullScreenImpl();

	void ShowFullScreen();

	void ShowFullScreen(CFrameWnd* pFrame);
	void RestoreState(CFrameWnd* pFrame);

	CRect GetFullScreenRect() const { return m_rectFullScreenWindow; }

	BOOL IsFullScreen() const { return m_bFullScreen; }

	void EnableMainMenu(BOOL bShow = TRUE)
	{
		m_bShowMenu = bShow;
	}
	void SetFullScreenID(UINT uiFullScreenID)
	{
		m_uiFullScreenID = uiFullScreenID;
	}

	void EnableTabsArea(BOOL bShowTabs)
	{
		m_bTabsArea = bShowTabs;
	}

	void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);

protected:
	CRect           m_rectFullScreenWindow;
	CMFCToolBar* m_pwndFullScreenBar;
	BOOL            m_bFullScreen;
	BOOL            m_bShowMenu;
	CRect           m_rectFramePrev;
	CFrameImpl*  m_pImpl;
	UINT            m_uiFullScreenID;
	BOOL            m_bTabsArea;
	CString         m_strRegSection;

protected:
	void UndockAndHidePanes(CFrameWnd* pFrame);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
