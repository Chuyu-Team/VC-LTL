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

//--------------------------------------------------------
// The class CMultiDocTemplateEx is created for access
// to protected member 'uiMenuResId' of CMultiDocTemplate!
//--------------------------------------------------------

#pragma warning(push)
#pragma warning (disable : 4510 4610)

class CMultiDocTemplateEx : public CMultiDocTemplate
{
public:
	UINT GetResId () const { return m_nIDResource; }
};

#pragma warning(pop)

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
