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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCReBarState
{
private:
	static BOOL CALLBACK LoadRebarStateProc(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK SaveRebarStateProc(HWND hwnd, LPARAM lParam);

public:
	static void __stdcall LoadState(CString& strKey, CFrameWnd* pFrrame);
	static void __stdcall SaveState(CString& strKey, CFrameWnd* pFrrame);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
