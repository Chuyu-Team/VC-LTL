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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonBar;
class CMFCRibbonBaseElement;
class CMFCRibbonCategory;
class CMFCRibbonSeparator;

/*============================================================================*/
// CMFCRibbonCommandsListBox window

class CMFCRibbonCommandsListBox : public CListBox
{
// Construction
public:
	CMFCRibbonCommandsListBox(	CMFCRibbonBar* pRibbonBar, BOOL bIncludeSeparator = TRUE, BOOL bDrawDefaultIcon = FALSE, BOOL bCommandsOnly = FALSE);

// Attributes
public:
	CMFCRibbonBaseElement* GetSelected() const;
	CMFCRibbonBaseElement* GetCommand(int nIndex) const;
	int GetCommandIndex(UINT uiID) const;

	BOOL CommandsOnly() const { return m_bCommandsOnly; }

protected:
	int						m_nTextOffset;
	BOOL					m_bDrawDefaultIcon;
	BOOL					m_bCommandsOnly;
	CMFCRibbonBar*			m_pRibbonBar;
	CMFCRibbonSeparator*	m_pSeparator;

// Operations
public:
	void FillFromCategory(CMFCRibbonCategory* pCategory);
	void FillFromIDs(const CList<UINT,UINT>& lstCommands, BOOL bDeep);
	void FillFromArray(const CArray<CMFCRibbonBaseElement*, CMFCRibbonBaseElement*>& arElements, BOOL bDeep, BOOL bIgnoreSeparators);
	void FillAll();

	BOOL AddCommand(CMFCRibbonBaseElement* pCmd, BOOL bSelect = TRUE, BOOL bDeep = TRUE);

// Implementation
public:
	virtual ~CMFCRibbonCommandsListBox();

protected:
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
