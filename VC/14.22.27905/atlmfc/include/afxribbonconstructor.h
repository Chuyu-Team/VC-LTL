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

#include "afxribboninfo.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCRibbonBar;

class CMFCRibbonConstructor
{
public:
	CMFCRibbonConstructor(const CMFCRibbonInfo& info);
	virtual ~CMFCRibbonConstructor();

public:
	virtual void ConstructRibbonBar(CMFCRibbonBar& bar) const;

protected:
	virtual CMFCRibbonBaseElement* CreateElement(const CMFCRibbonInfo::XElement& info) const;
	virtual CMFCRibbonPanel* CreatePanel(CMFCRibbonCategory& category, const CMFCRibbonInfo::XPanel& info) const;
	virtual CMFCRibbonCategory* CreateCategory(CMFCRibbonBar& bar, const CMFCRibbonInfo::XCategory& info) const;
	virtual CMFCRibbonCategory* CreateCategoryContext(CMFCRibbonBar& bar, const CMFCRibbonInfo::XContext& infoContext, const CMFCRibbonInfo::XCategory& info) const;
	virtual CMFCRibbonMainPanel* CreateCategoryMain(CMFCRibbonBar& bar, const CMFCRibbonInfo::XCategoryMain& info) const;
	virtual CMFCRibbonApplicationButton* CreateApplicationButton(CMFCRibbonBar& bar) const;

	virtual void SetID(CMFCRibbonBaseElement& element, const CMFCRibbonInfo::XID& info) const;
	virtual void SetIcon(CMFCRibbonButton& element, CMFCRibbonBaseElement::RibbonImageType type, const CMFCToolBarImages& images, BOOL bLargeIcon = FALSE) const;
	virtual void ConstructBaseElement(CMFCRibbonBaseElement& element, const CMFCRibbonInfo::XElement& info) const;
	virtual void ConstructElement(CMFCRibbonBaseElement& element, const CMFCRibbonInfo::XElement& info) const;
	virtual void ConstructQATElements(CMFCRibbonBar& bar, const CMFCRibbonInfo::XRibbonBar& info) const;
	virtual void ConstructTabElements(CMFCRibbonBar& bar, const CMFCRibbonInfo::XRibbonBar& info) const;
	virtual void ConstructPanel(CMFCRibbonPanel& panel, const CMFCRibbonInfo::XPanel& info) const;
	virtual void ConstructCategory(CMFCRibbonCategory& category, const CMFCRibbonInfo::XCategory& info) const;
	virtual void ConstructCategoryMain(CMFCRibbonBar& bar, const CMFCRibbonInfo::XCategoryMain& info) const;

	const CMFCRibbonInfo& GetInfo() const { return m_Info; }

private:
	const CMFCRibbonInfo&   m_Info;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
