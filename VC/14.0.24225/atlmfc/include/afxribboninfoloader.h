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

#ifndef RT_RIBBON
#define RT_RIBBON MAKEINTRESOURCE(28)
#endif

class CMFCRibbonInfoLoader  
{
public:
    CMFCRibbonInfoLoader(CMFCRibbonInfo& info);
    virtual ~CMFCRibbonInfoLoader();

    BOOL Load(UINT uiResID, LPCTSTR lpszResType = RT_RIBBON, HINSTANCE hInstance = NULL);
    BOOL Load(LPCTSTR lpszResID, LPCTSTR lpszResType = RT_RIBBON, HINSTANCE hInstance = NULL);

    virtual BOOL LoadFromBuffer(LPCTSTR lpszBuffer);
    virtual BOOL LoadFromBuffer(LPBYTE lpBuffer, UINT nSize);

    virtual BOOL LoadImage(CMFCRibbonInfo::XImage& image, BOOL bSingle);

protected:
    virtual BOOL LoadImage(const CMFCRibbonInfo::XID& id, CMFCToolBarImages& image, BOOL bSingle);

private:
    CMFCRibbonInfo& m_Info;
    HINSTANCE       m_hInstance;
};


#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
