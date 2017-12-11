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
#include "afxtoolbarbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCOutlookBarPane;

class CMFCOutlookBarPaneButton : public CMFCToolBarButton
{
	friend class CMFCOutlookBarPane;

	DECLARE_SERIAL(CMFCOutlookBarPaneButton)

public:
	CMFCOutlookBarPaneButton();
	virtual ~CMFCOutlookBarPaneButton();

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual void OnChangeParentWnd(CWnd* pWndParent);

	virtual BOOL CanBeDropped(CMFCToolBar* pToolbar);
	virtual void SetImage(int iImage);

protected:
	CMFCOutlookBarPane* m_pWndParentBar;
	CSize            m_sizeImage;
	BOOL             m_bIsWholeText;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
