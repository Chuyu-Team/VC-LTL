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

#ifndef __AFXOLE_H__
#include <afxole.h>
#endif

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCToolBarDropSource command target

class CMFCToolBarDropSource : public COleDropSource
{
public:
	CMFCToolBarDropSource();
	virtual ~CMFCToolBarDropSource();

// Attributes
public:
	BOOL    m_bDeleteOnDrop;
	BOOL    m_bEscapePressed;
	BOOL    m_bDragStarted;
	HCURSOR m_hcurDelete;
	HCURSOR m_hcurMove;
	HCURSOR m_hcurCopy;

// Overrides
public:
	virtual SCODE GiveFeedback(DROPEFFECT dropEffect);
	virtual SCODE QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState);
	virtual BOOL OnBeginDrag(CWnd* pWnd);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
