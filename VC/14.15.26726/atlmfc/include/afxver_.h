// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// afxver_.h - target version/configuration control

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Master version numbers

#define _AFX     1      // Microsoft Application Framework Classes
#ifndef _MFC_VER
#define _MFC_VER 0x0E00 // Microsoft Foundation Classes version 14.00
#endif

#ifndef _MFC_FILENAME_VER
#define _MFC_FILENAME_VER "140"
#endif

/////////////////////////////////////////////////////////////////////////////
// turn off reference tracking for certain often used symbols

#pragma component(browser, off, references, "ASSERT")
#pragma component(browser, off, references, "AfxAssertFailedLine")
#pragma component(browser, off, references, "AfxDebugBreak")
#pragma component(browser, off, references, "BOOL")
#pragma component(browser, off, references, "BYTE")
#pragma component(browser, off, references, "DECLSPEC_IMPORT")
#pragma component(browser, off, references, "DWORD")
#pragma component(browser, off, references, "FALSE")
#pragma component(browser, off, references, "FAR")
#pragma component(browser, off, references, "LPSTR")
#pragma component(browser, off, references, "LPTSTR")
#pragma component(browser, off, references, "LPCSTR")
#pragma component(browser, off, references, "LPCTSTR")
#pragma component(browser, off, references, "NULL")
#pragma component(browser, off, references, "PASCAL")
#pragma component(browser, off, references, "THIS_FILE")
#pragma component(browser, off, references, "TRUE")
#pragma component(browser, off, references, "UINT")
#pragma component(browser, off, references, "WINAPI")
#pragma component(browser, off, references, "WORD")

/////////////////////////////////////////////////////////////////////////////
// For target version (one of)
//   _CUSTOM   : for custom configurations (causes afxv_cfg.h to be included)
//
// Additional build options:
//  _DEBUG              debug versions (full diagnostics)
//  _AFXDLL             use shared MFC DLL
//  _AFXEXT             extension DLL version, implies _AFXDLL
//  _USRDLL             create regular DLL (_AFXDLL is valid too)
//

#ifndef _DEBUG
#ifndef _AFX_DISABLE_INLINES
	#define _AFX_ENABLE_INLINES
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Special configurations

// _AFXEXT implies _AFXDLL
#if defined(_AFXEXT) && !defined(_AFXDLL)
	#define _AFXDLL
#endif

#if defined(_AFXDLL) && !defined(_DLL)
	#error Please use the /MD switch for _AFXDLL builds
#endif

/////////////////////////////////////////////////////////////////////////////
// special include files

#ifndef AFX_INLINE
	#define AFX_INLINE inline /*__forceinline*/
#endif

// setup packing value
#define _AFX_PACKING    8   // MFC packs structs at 8 bytes

#include <afxv_w32.h>

#ifdef _M_IX86
	#define _AFX_MINREBUILD
#endif

#ifdef _AFXDLL
	#include <afxv_dll.h>
#endif

// Define this virtual key for use by status bar
#ifndef VK_KANA
#define VK_KANA             0x15
#endif

/////////////////////////////////////////////////////////////////////////////
// Special AfxDebugBreak: used to break into debugger at critical times

#ifndef AfxDebugBreak
#define AfxDebugBreak() __debugbreak()
#endif

#ifndef _DEBUG
#ifdef AfxDebugBreak
#undef AfxDebugBreak
#endif
#define AfxDebugBreak()
#endif  // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Standard preprocessor symbols if not already defined
/////////////////////////////////////////////////////////////////////////////

// SIZE_T_MAX is used by the collection classes
#ifndef SIZE_T_MAX
	#define SIZE_T_MAX  UINT_MAX
#endif

// PASCAL is used for static member functions
#ifndef PASCAL
	#define PASCAL  __stdcall
#endif

// FASTCALL is used for static member functions with little or no params
#ifndef FASTCALL
	#define FASTCALL __fastcall
#endif

// CDECL and EXPORT are defined in case WINDOWS.H doesn't
#ifndef CDECL
	#define CDECL __cdecl
#endif

#ifndef EXPORT
	#define EXPORT
#endif

// UNALIGNED is used for unaligned data access (in CArchive mostly)
#if !defined(UNALIGNED)
#if defined(_M_X64)
#define UNALIGNED __unaligned
#else
#define UNALIGNED
#endif
#endif

// AFX_DEPRECATED is used for functions that should no longer be used
#ifndef AFX_DEPRECATED
#ifdef _AFX_DISABLE_DEPRECATED
	#define AFX_DEPRECATED(_Message)
#else
	#define AFX_DEPRECATED(_Message) __declspec(deprecated(_Message))
#endif
#endif

/*
 * Why are these ANSI functions now deprecated?
 *
 * AFX_ANSI_DEPRECATED is used to bring MFC into line with what messages and
 * modes Windows supports for its controls.
 *
 * Historically, all core controls could be created ANSI (CreateWindowA) or
 * UNICODE (CreateWindowW). /DUNICODE builds of MFC used CreateWindowW and
 * hence got UNICODE controls. But the set of functions and messages available
 * for the controls wasn't different between the two modes.
 *
 * [Side note: There are some exceptions to this rule, both in MFC and Win32].
 *
 * Windows XP came along and added comctrl version 6, which was not intended
 * to be binary or behaviour compatible with previous versions. One of these
 * incompatibilities was that the new controls are UNICODE from the ground up
 * and do not support ANSI mode. These controls also have the new (at the time)
 * Windows XP "themed" look and feel applied to them, which made them very
 * appealing to developers who wanted their apps to look up to date
 *
 * To ensure that the new controls didn't break existing apps, you have to opt
 * in to using comctrl version 6 (via a dependency entry in your Win32 XML
 * manifest).
 *
 * Unfortunately, even though these controls are not supported or documented
 * to work in ANSI mode, they do have some limited functionality in ANSI mode
 * (to support various complicated compatibility scenarios where controls are
 * mixed in a process by addins).
 *
 * As a result, after Windows XP shipped, some people took existing ANSI apps
 * (that call CreateWindowA) and applied XML manifests to them. This should
 * not have worked (and is not documented to work), but sometimes does
 * actually work.
 *
 * To further muddy the waters, VC7.0 applied the manifest indiscriminately
 * to all newly-created MFC apps (even ANSI ones), furthering the impression
 * that ANSI apps could use the themed controls. This was the source of quite
 * a few bugs in VC7.0. VC7.1 fixed this problem -- the manifest was only
 * applied to newly created projects when compiled Unicode (via a #ifdef in
 * the .rc file), and VC8 made it a bit simpler by using the new manifest
 * generation scheme.
 *
 * Another mistake we made in VC7 was to provide wrappers for the new
 * control's new messages that were available in ANSI builds. This again
 * allowed you to call some of these new methods on ANSI controls even though
 * several of them wouldn't even work.
 *
 * In VC9 we are fixing this issue and bringing our projects and wrappers into
 * line with the supported OS behaviour for Windows XP and Windows Vista.
 *
 * Messages and functions that are only supposed to work on UNICODE controls
 * are now only available on UNICODE controls. You will see a deprecation
 * message if you use one of these unsupported messages. If you want to use
 * the new control features that have been added since Windows XP, you'll need
 * to compile your MFC-based project as UNICODE, and set appropriate values
 * for the OS minimum version control macros (_WIN32_WINNT, _WIN32_IE,
 * _NTDDI_VERSION, etc).
 *
 */
#ifndef AFX_ANSI_DEPRECATED
#ifdef UNICODE
	#define AFX_ANSI_DEPRECATED
#else
	#define AFX_ANSI_DEPRECATED AFX_DEPRECATED( \
			"This MFC feature requires /DUNICODE to work correctly, because the underlying Windows control is only supported for UNICODE builds. " \
			"Previous versions of MFC incorrectly enabled this in ANSI/MBCS builds. " \
			"Please compile with /DUNICODE to use this feature. See product documentation for more information." \
			)
#endif
#endif


#if defined(_AFX_SECURE_NO_DEPRECATE) && !defined(_AFX_SECURE_NO_WARNINGS)
#define _AFX_SECURE_NO_WARNINGS
#endif

// _AFX_INSECURE_DEPRECATE is used for deprecated, insecure functions.
#ifndef _AFX_INSECURE_DEPRECATE
#ifdef _AFX_SECURE_NO_WARNINGS
#define _AFX_INSECURE_DEPRECATE(_Message)
#else
#define _AFX_INSECURE_DEPRECATE(_Message) __declspec(deprecated(_Message))
#endif // _AFX_SECURE_NO_WARNINGS
#endif // _AFX_INSECURE_DEPRECATE

// AFXAPI is used on global public functions
#ifndef AFXAPI
	#define AFXAPI __stdcall
#endif

// AFXOLEAPI is used for some special OLE functions
#ifndef AFXOLEAPI
	#define AFXOLEAPI __stdcall
#endif

// AFX_CDECL is used for rare functions taking variable arguments
#ifndef AFX_CDECL
	#define AFX_CDECL __cdecl
#endif

// AFX_EXPORT is used for functions which need to be exported
#ifndef AFX_EXPORT
	#define AFX_EXPORT EXPORT
#endif

#ifndef AFX_STATIC
	#define AFX_STATIC extern
	#define AFX_STATIC_DATA extern __declspec(selectany)
#endif

// The following macros are used to enable export/import
// for data
#ifndef AFX_DATA_EXPORT
	#define AFX_DATA_EXPORT __declspec(dllexport)
#endif
#ifndef AFX_DATA_IMPORT
	#define AFX_DATA_IMPORT __declspec(dllimport)
#endif

#if !defined(_MFC_BLD) && defined(_AFXDLL)
	#define AFX_IMPORT_DATA __declspec(dllimport)
#else
	#define AFX_IMPORT_DATA
#endif

// for classes
#ifndef AFX_CLASS_EXPORT
	#define AFX_CLASS_EXPORT __declspec(dllexport)
#endif
#ifndef AFX_CLASS_IMPORT
	#define AFX_CLASS_IMPORT __declspec(dllimport)
#endif

// for global APIs
#ifndef AFX_API_EXPORT
	#define AFX_API_EXPORT __declspec(dllexport)
#endif
#ifndef AFX_API_IMPORT
	#define AFX_API_IMPORT __declspec(dllimport)
#endif

// This macro is used to reduce size requirements of some classes
#ifndef AFX_ALWAYS_VTABLE
#ifndef AFX_NOVTABLE
#if !defined(_DEBUG)
#define AFX_NOVTABLE __declspec(novtable)
#else
#define AFX_NOVTABLE
#endif
#endif
#endif

// for global data that should be in COMDATs (packaged data)
#ifndef AFX_COMDAT
#define AFX_COMDAT __declspec(selectany)
#endif

// The following macros are used on data declarations/definitions
//  (they are redefined for extension DLLs and the shared MFC DLL)
#define AFX_DATA
#define AFX_DATADEF
#define AFX_API

// used when building the "core" MFCXX.DLL
#ifndef AFX_CORE_DATA
	#define AFX_CORE_DATA
	#define AFX_CORE_DATADEF
#endif

// used when building the MFC/OLE support MFCOXX.DLL
#ifndef AFX_OLE_DATA
	#define AFX_OLE_DATA
	#define AFX_OLE_DATADEF
#endif

// used when building the MFC/DB support MFCDXX.DLL
#ifndef AFX_DB_DATA
	#define AFX_DB_DATA
	#define AFX_DB_DATADEF
#endif

// used when building the MFC/NET support MFCNXX.DLL
#ifndef AFX_NET_DATA
	#define AFX_NET_DATA
	#define AFX_NET_DATADEF
#endif

// used when building extension DLLs
#ifndef AFX_EXT_DATA
	#define AFX_EXT_DATA
	#define AFX_EXT_DATADEF
	#define AFX_EXT_CLASS
	#define AFX_EXT_API
#endif

// BASED_XXXX macros are provided for backward compatibility
#ifndef BASED_CODE
	#define BASED_CODE
#endif

#ifndef BASED_DEBUG
	#define BASED_DEBUG
#endif

#ifndef BASED_STACK
	#define BASED_STACK
#endif
/////////////////////////////////////////////////////////////////////////////
