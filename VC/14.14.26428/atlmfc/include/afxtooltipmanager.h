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
#include "afxtooltipctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#define AFX_TOOLTIP_TYPE_ALL        0xFFFF

#define AFX_TOOLTIP_TYPE_DEFAULT    0x0001
#define AFX_TOOLTIP_TYPE_TOOLBAR    0x0002
#define AFX_TOOLTIP_TYPE_TAB        0x0004
#define AFX_TOOLTIP_TYPE_MINIFRAME  0x0008
#define AFX_TOOLTIP_TYPE_DOCKBAR    0x0010
#define AFX_TOOLTIP_TYPE_EDIT       0x0020
#define	AFX_TOOLTIP_TYPE_BUTTON     0x0040
#define	AFX_TOOLTIP_TYPE_TOOLBOX    0x0080
#define AFX_TOOLTIP_TYPE_PLANNER    0x0100
#define AFX_TOOLTIP_TYPE_RIBBON     0x0200
#define	AFX_TOOLTIP_TYPE_CAPTIONBAR 0x0400

#define AFX_TOOLTIP_TYPES           11

class CTooltipManager : public CObject
{
public:

	static BOOL __stdcall CreateToolTip(CToolTipCtrl*& pToolTip, CWnd* pWndParent, UINT nType);
	static void __stdcall DeleteToolTip(CToolTipCtrl*& pToolTip);
	static void __stdcall SetTooltipText(TOOLINFO* pTI, CToolTipCtrl* pToolTip, UINT nType, const CString strText, LPCTSTR lpszDescr = NULL);

	CTooltipManager();
	virtual ~CTooltipManager();

	void SetTooltipParams(UINT nTypes, CRuntimeClass* pRTC = RUNTIME_CLASS(CMFCToolTipCtrl), CMFCToolTipInfo* pParams = NULL);
	void UpdateTooltips();

protected:
	BOOL CreateToolTipObject(CToolTipCtrl*& pToolTip, UINT nType);

	CMFCToolTipInfo    m_Params[AFX_TOOLTIP_TYPES];
	CRuntimeClass*    m_pRTC[AFX_TOOLTIP_TYPES];
	CList<HWND, HWND> m_lstOwners;
};

extern CTooltipManager* afxTooltipManager;
extern AFX_IMPORT_DATA UINT AFX_WM_UPDATETOOLTIPS;

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
