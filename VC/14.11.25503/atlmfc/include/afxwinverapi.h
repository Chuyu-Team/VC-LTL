// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXWINVERAPI_H__
#define __AFXWINVERAPI_H__

#pragma once

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif 

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

// This file contains declarations of wrappers for methods used
// in MFC that are only available in later versions of Windows.

// When the minimum supported version of Windows is increased, the
// implementations of these methods just call the real Windows APIs.

// Minimum supported versions of Windows:
// Windows XP SP2 for x86 and x64, Windows 8 for ARM

#if defined(_M_IX86) || defined(_M_X64)
#define _MFC_NTDDI_MIN NTDDI_WINXPSP2
#else
#define _MFC_NTDDI_MIN NTDDI_WIN8
#endif

// Use this macro for loading a local cached function from a DLL that is known to be loaded (e.g. KERNEL32)
#define IFDYNAMICGETCACHEDFUNCTIONFORMFC(libraryname, functionname, functionpointer) \
	static volatile auto functionpointer##_cache = reinterpret_cast<decltype(::functionname)*>(NULL); \
	auto functionpointer = reinterpret_cast<decltype(::functionname)*>(functionpointer##_cache); \
	if (functionpointer == reinterpret_cast<decltype(::functionname)*>(NULL)) \
	{ \
		HINSTANCE hLibrary = GetModuleHandleW(libraryname); \
		if (hLibrary != NULL) \
		{ \
			functionpointer = reinterpret_cast<decltype(::functionname)*>(::GetProcAddress(hLibrary, #functionname)); \
			functionpointer##_cache = reinterpret_cast<decltype(::functionname)*>(::EncodePointer((PVOID)functionpointer)); \
		} \
	} \
	else \
	{ \
		functionpointer = reinterpret_cast<decltype(::functionname)*>(::DecodePointer((PVOID)functionpointer)); \
	} \
	if (functionpointer != reinterpret_cast<decltype(::functionname)*>(NULL))

// Use this macro for loading a local cached function from a DLL that may not already be loaded (e.g. DWMAPI)
#define IFDYNAMICLOADCACHEDFUNCTIONFORMFC(libraryname, functionname, functionpointer) \
	static volatile auto functionpointer##_cache = reinterpret_cast<decltype(::functionname)*>(NULL); \
	auto functionpointer = reinterpret_cast<decltype(::functionname)*>(functionpointer##_cache); \
	if (functionpointer == reinterpret_cast<decltype(::functionname)*>(NULL)) \
	{ \
		HINSTANCE hLibrary = AtlLoadSystemLibraryUsingFullPath(libraryname); \
		if (hLibrary != NULL) \
		{ \
			functionpointer = reinterpret_cast<decltype(::functionname)*>(::GetProcAddress(hLibrary, #functionname)); \
			functionpointer##_cache = reinterpret_cast<decltype(::functionname)*>(::EncodePointer((PVOID)functionpointer)); \
		} \
	} \
	else \
	{ \
		functionpointer = reinterpret_cast<decltype(::functionname)*>(::DecodePointer((PVOID)functionpointer)); \
	} \
	if (functionpointer != reinterpret_cast<decltype(::functionname)*>(NULL))

int _AfxCompareStringEx(__in_opt LPCWSTR lpLocaleName, __in DWORD dwCmpFlags, __in LPCWSTR lpString1, __in int cchCount1, __in LPCWSTR lpString2, __in int cchCount2);

// Wrappers for Win32 APIs that are only supported on higher versions of Windows
int _AfxGetLocaleInfoEx(LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData);
BOOL _AfxGetThreadPreferredUILanguages(__in DWORD dwFlags, __out PULONG pulNumLanguages, __out_opt PZZWSTR pwszLanguagesBuffer, __inout PULONG pcchLanguagesBuffer);

HRESULT _AfxRegisterApplicationRestart(__in_opt PCWSTR pwzCommandline, __in DWORD dwFlags);
HRESULT _AfxRegisterApplicationRecoveryCallback(__in APPLICATION_RECOVERY_CALLBACK pRecoveryCallback, __in_opt PVOID pvParameter, __in DWORD dwPingInterval, __in DWORD dwFlags);
HRESULT _AfxApplicationRecoveryInProgress(__out PBOOL pbCanceled);
VOID    _AfxApplicationRecoveryFinished(__in BOOL bSuccess);

BOOL _AfxChangeWindowMessageFilter(__in UINT message, __in DWORD dwFlag);

HRESULT _AfxSHCreateItemFromParsingName(__in PCWSTR pszPath, __in IBindCtx *pbc, __in REFIID riid, __out void **ppv);
HRESULT _AfxSHGetKnownFolderPath(__in REFKNOWNFOLDERID rfid, __in DWORD dwFlags, __in_opt HANDLE hToken, __out PWSTR *ppszPath);

BOOL _AfxInitNetworkAddressControl();

HRESULT _AfxDrawThemeTextEx(__in HTHEME hTheme, __in HDC hdc, __in int iPartId, __in int iStateId, __in LPCWSTR pszText, __in int iCharCount, __in DWORD dwFlags, __inout LPRECT pRect, __in const DTTOPTS *pOptions);

#if (NTDDI_VERSION >= NTDDI_VISTA)
HRESULT      _AfxBufferedPaintInit(void);
HRESULT      _AfxBufferedPaintUnInit(void);
HPAINTBUFFER _AfxBeginBufferedPaint(HDC hdcTarget, const RECT *prcTarget, BP_BUFFERFORMAT dwFormat, __in BP_PAINTPARAMS *pPaintParams, __out HDC *phdc);
HRESULT      _AfxEndBufferedPaint(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);
#endif // (NTDDI_VERSION >= NTDDI_VISTA)

BOOL    _AfxDwmDefWindowProc(__in HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, __out LRESULT *plResult);
HRESULT _AfxDwmExtendFrameIntoClientArea(HWND hWnd, __in const MARGINS *pMarInset);
HRESULT _AfxDwmIsCompositionEnabled(__out BOOL *pfEnabled);
HRESULT _AfxDwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, __in LPCVOID pvAttribute, DWORD cbAttribute);
HRESULT _AfxDwmSetIconicThumbnail(__in HWND hwnd, __in HBITMAP hbmp, __in DWORD dwSITFlags);
HRESULT _AfxDwmInvalidateIconicBitmaps(__in  HWND hwnd);
HRESULT _AfxDwmSetIconicLivePreviewBitmap(HWND hwnd, HBITMAP hbmp, __in_opt POINT *pptClient, DWORD dwSITFlags);

HRESULT _AfxPSGetPropertyDescriptionListFromString(__in LPCWSTR pszPropList, __in REFIID riid, __out void **ppv);

BOOL    _AfxIsTaskDialogSupported();
#if (NTDDI_VERSION >= NTDDI_VISTA)
HRESULT _AfxTaskDialogIndirect(_In_ const TASKDIALOGCONFIG *pTaskConfig, _Out_opt_ int *pnButton, _Out_opt_ int *pnRadioButton, _Out_opt_ BOOL *pfVerificationFlagChecked);
#endif // (NTDDI_VERSION >= NTDDI_VISTA)

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFXWINVERAPI_H__

/////////////////////////////////////////////////////////////////////////////
