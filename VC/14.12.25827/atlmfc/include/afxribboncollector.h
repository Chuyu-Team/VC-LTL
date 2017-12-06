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

class CMFCRibbonCollector
{
public:
	enum XFlags
	{
		e_CollectUpdatedToolInfo = 0x01, // update tool tip info
		e_CollectRibbonBarIcons  = 0x02, // add icons of the panels or tab elements to the ribbon image
		e_CollectGroupImages     = 0x04, // add image of each group to the small image of category
		e_CollectHiddenElements  = 0x08, // collect hidden elements of the category
		e_CollectImagesID        = 0x10  // collect resource ID of the images (for category and group images)
	};

public:
	CMFCRibbonCollector(CMFCRibbonInfo& info, DWORD dwFlags = 0);
	virtual ~CMFCRibbonCollector();

public:
	virtual void CollectRibbonBar(const CMFCRibbonBar& bar);

protected:
	virtual CMFCRibbonInfo::XElement* CollectElement(const CMFCRibbonBaseElement& element);

	virtual void GetID(const CMFCRibbonBaseElement& element, CMFCRibbonInfo::XID& info);
	virtual void GetRibbonBarImages(const CMFCRibbonBar& bar, CMFCRibbonInfo::XRibbonBar& info);
	virtual void GetCategoryImages(const CMFCRibbonCategory& category, CMFCRibbonInfo::XCategory& info);
	virtual void GetCategoryImages(const CMFCRibbonCategory& category, CMFCRibbonInfo::XImage& infoSmall, CMFCRibbonInfo::XImage& infoLarge);
	virtual void GetElementImages(const CMFCRibbonBaseElement& element, CMFCRibbonInfo::XImage& info);

	virtual void CollectBaseElement(const CMFCRibbonBaseElement& element, CMFCRibbonInfo::XElement& info, BOOL bSubItems = TRUE);
	virtual void CollectElement(const CMFCRibbonBaseElement& element, CMFCRibbonInfo::XElement& info);
	virtual void CollectQATElements(const CMFCRibbonBar& bar, CMFCRibbonInfo::XRibbonBar& info);
	virtual void CollectPanel(const CMFCRibbonPanel& panel, CMFCRibbonInfo::XPanel& info);
	virtual void CollectCategory(const CMFCRibbonCategory& category, CMFCRibbonInfo::XCategory& info);
	virtual void CollectCategoryMain(const CMFCRibbonBar& bar, CMFCRibbonInfo::XCategoryMain& info);
	virtual void CollectRibbonBar(const CMFCRibbonBar& bar, CMFCRibbonInfo::XRibbonBar& info);

	virtual void CollectImageInfo(CMFCToolBarImages& ti, CMFCRibbonInfo::XImage& image, BOOL bCollectID);

	CMFCRibbonInfo& GetInfo() { return m_Info; }
	DWORD GetFlags() const { return m_dwFlags; }

private:
	CMFCRibbonInfo& m_Info;
	DWORD           m_dwFlags;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
