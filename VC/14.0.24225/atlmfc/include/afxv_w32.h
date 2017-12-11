// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// afxv_w32.h - target version/configuration control for Win32

#pragma once

#ifdef _WINDOWS_
	#error WINDOWS.H already included.  MFC apps must not #include <windows.h>
#endif

// STRICT is the only supported option (NOSTRICT is no longer supported)
#ifndef STRICT
#define STRICT 1
#endif

// WinSDKVer.h contains the definition for _WIN32_WINNT_MAXVER (and other maximums).
#include <winsdkver.h>

#ifndef _WIN32_WINNT
#ifdef WINVER
#define _WIN32_WINNT WINVER
#else
#pragma message("_WIN32_WINNT not defined. Defaulting to _WIN32_WINNT_MAXVER (see WinSDKVer.h)")
#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif
#endif

// SDKDDKVer.h will set any of WINVER, NTDDI_VERSION and _WIN32_IE that are yet unset.
#include <sdkddkver.h>

#if (WINVER < 0x0501)
#error MFC does not support WINVER less than 0x0501.  Please change the definition of WINVER in your project properties or precompiled header.
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WINXP)
#error MFC does not support _WIN32_WINNT less than _WIN32_WINNT_WINXP.  Please change the definition of _WIN32_WINNT in your project properties or precompiled header.
#endif

#if (NTDDI_VERSION < NTDDI_WINXP)
#error MFC does not support NTDDI_VERSION less than NTDDI_WINXP.  Please change the definition of NTDDI_VERSION in your project properties or precompiled header.
#endif

// certain parts of WINDOWS.H are necessary
#undef NOKERNEL
#undef NOGDI
#undef NOUSER
#undef NODRIVERS
#undef NOLOGERROR
#undef NOPROFILER
#undef NOMEMMGR
#undef NOLFILEIO
#undef NOOPENFILE
#undef NORESOURCE
#undef NOATOM
#undef NOLANGUAGE
#undef NOLSTRING
#undef NODBCS
#undef NOKEYBOARDINFO
#undef NOGDICAPMASKS
#undef NOCOLOR
#undef NOGDIOBJ
#undef NODRAWTEXT
#undef NOTEXTMETRIC
#undef NOSCALABLEFONT
#undef NOBITMAP
#undef NORASTEROPS
#undef NOMETAFILE
#undef NOSYSMETRICS
#undef NOSYSTEMPARAMSINFO
#undef NOMSG
#undef NOWINSTYLES
#undef NOWINOFFSETS
#undef NOSHOWWINDOW
#undef NODEFERWINDOWPOS
#undef NOVIRTUALKEYCODES
#undef NOKEYSTATES
#undef NOWH
#undef NOMENUS
#undef NOSCROLL
#undef NOCLIPBOARD
#undef NOICONS
#undef NOMB
#undef NOSYSCOMMANDS
#undef NOMDI
#undef NOCTLMGR
#undef NOWINMESSAGES

#ifndef WIN32
#define WIN32
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE         // UNICODE is used by Windows headers
#endif
#endif

#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
#endif
#endif

#ifdef VC_EXTRALEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NORPC

#ifndef NO_ANSIUNI_ONLY
#ifdef _UNICODE
#define UNICODE_ONLY
#else
#define ANSI_ONLY
#endif
#endif //!NO_ANSIUNI_ONLY

#endif //VC_EXTRALEAN

/////////////////////////////////////////////////////////////////////////////
// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the AFX include files

#pragma warning(push)
#pragma warning(disable: 4311 4312)
#pragma warning(disable: 4201)  // winnt.h uses nameless structs

// Don't include winsock.h
#pragma push_macro("_WINSOCKAPI_")
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#include <windows.h>

#pragma pop_macro("_WINSOCKAPI_")

#pragma warning(pop)

// Mouse message MFC is interested in
#ifndef AFX_WM_MOUSELAST
#define AFX_WM_MOUSELAST 0x0209
#endif

#include <zmouse.h>

struct HKEY__;
typedef struct HKEY__ *HKEY;

#ifndef _INC_COMMCTRL
	#include <commctrl.h>

	// Note: We must avoid using TB_ADDBUTTONW and TB_INSERTBUTTONW
	//       in the Unicode build or else MFCXXU.DLL will not be
	//       compatible with pre-IE4 versions of COMCTL32.DLL.
	#ifdef TB_ADDBUTTONSA
		#undef TB_ADDBUTTONS
		#define TB_ADDBUTTONS TB_ADDBUTTONSA
	#endif

	#ifdef TB_INSERTBUTTONA
		#undef TB_INSERTBUTTON
		#define TB_INSERTBUTTON TB_INSERTBUTTONA
	#endif
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifndef EXPORT
#define EXPORT
#endif

#ifndef _INC_TCHAR
	#include <tchar.h>      // used for ANSI v.s. UNICODE abstraction
#endif
#ifdef _MBCS
#ifndef _INC_MBCTYPE
	#include <mbctype.h>
#endif
#ifndef _INC_MBSTRING
	#include <mbstring.h>
#endif
#endif

#ifdef _WIN64
#define _AFX_NO_CTL3D_SUPPORT
#endif

/////////////////////////////////////////////////////////////////////////////
// Now for the Windows API specific parts

// WM_CTLCOLOR for 16 bit API compatability
#define WM_CTLCOLOR     0x0019

// Win32 uses macros with parameters for this, which breaks C++ code.
#ifdef GetWindowTask
#undef GetWindowTask
AFX_INLINE HTASK GetWindowTask(HWND hWnd)
	{ return (HTASK)(DWORD_PTR)::GetWindowThreadProcessId(hWnd, NULL); }
#endif

// Win32 uses macros with parameters for this, which breaks C++ code.
#ifdef GetNextWindow
#undef GetNextWindow
AFX_INLINE HWND GetNextWindow(HWND hWnd, UINT nDirection)
	{ return ::GetWindow(hWnd, nDirection); }
#endif

// Avoid mapping CToolBar::DrawState to DrawState[A/W]
#ifdef DrawState
#undef DrawState
AFX_INLINE BOOL WINAPI DrawState(HDC hdc, HBRUSH hbr, DRAWSTATEPROC lpOutputFunc,
	LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT fuFlags)
#ifdef UNICODE
	{ return ::DrawStateW(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy,
		fuFlags); }
#else
	{ return ::DrawStateA(hdc, hbr, lpOutputFunc, lData, wData, x, y, cx, cy,
		fuFlags); }
#endif
#endif

// FreeResource is not required on Win32 platforms
#undef FreeResource
AFX_INLINE BOOL WINAPI FreeResource(_In_ HGLOBAL) { return TRUE; }
// UnlockResource is not required on Win32 platforms
#undef UnlockResource
AFX_INLINE int WINAPI UnlockResource(HGLOBAL) { return 0; }

/////////////////////////////////////////////////////////////////////////////
