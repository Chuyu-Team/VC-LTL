// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATL_HANDLER__
#define __ATL_HANDLER__

#pragma once

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#include <filter.h>

#ifndef REFPROPERTYKEY
#include <propkeydef.h>
#endif

#include <atlstr.h>

#ifndef _AFXDLL
// afx.h already defined POSITION type
#ifndef __AFX_H__
#include <atlcoll.h>
#endif //__AFX_H__
#ifndef ATLASSERT
#include <atldef.h>
#endif
#endif

#include <thumbcache.h>

namespace ATL
{

interface IFilterChunkValue
{
protected:
	IFilterChunkValue ()
	{
	}

public:
	virtual ~IFilterChunkValue ()
	{
	}

	virtual void Clear () = 0;
	virtual BOOL IsValid() const = 0;
	virtual HRESULT GetValue(_Outptr_ PROPVARIANT **ppPropVariant) = 0;
	virtual PROPVARIANT GetValueNoAlloc () = 0;
	virtual CString& GetString() = 0;
	virtual HRESULT CopyChunk(_Inout_ STAT_CHUNK *pStatChunk) = 0;
	virtual CHUNKSTATE GetChunkType() const = 0;
	virtual REFGUID GetChunkGUID() const = 0;
	virtual DWORD GetChunkPID() const = 0;

	// set the property by key to a unicode string
	virtual HRESULT SetTextValue(
		_In_ REFPROPERTYKEY pkey,
		_In_z_ LPCTSTR pszValue,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set the property by key to a bool
	virtual HRESULT SetBoolValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ BOOL bVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set the property by key to a variant bool
	virtual HRESULT SetBoolValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ VARIANT_BOOL bVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set the property by key to an int
	virtual HRESULT SetIntValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ int nVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set the property by key to a long
	virtual HRESULT SetLongValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ long lVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set the property by key to a dword
	virtual HRESULT SetDwordValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ DWORD dwVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set property by key to an int64
	virtual HRESULT SetInt64Value(
		_In_ REFPROPERTYKEY pkey,
		_In_ __int64 nVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	// set property by key to a SystemTime
	virtual HRESULT SetSystemTimeValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ const SYSTEMTIME &systemTime,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale=0,
		_In_ DWORD cwcLenSource=0,
		_In_ DWORD cwcStartSource=0,
		_In_ CHUNK_BREAKTYPE chunkBreakType=CHUNK_NO_BREAK) = 0;

	// set Property by key to a filetime
	virtual HRESULT SetFileTimeValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ FILETIME dtVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK) = 0;

	virtual void CopyFrom (_Inout_ IFilterChunkValue* pValue) = 0;
};

interface IDocument
{
protected:
	IDocument() : m_refCount(0)
	{
	}

public:
	virtual ~IDocument()
	{
	}

	virtual LONG AddRef()
	{
		return InterlockedIncrement(&m_refCount);
	}

	virtual LONG Release()
	{
#ifdef _AFXDLL
		ASSERT(m_refCount > 0);
#else
		ATLASSERT(m_refCount > 0);
#endif
		if (InterlockedDecrement(&m_refCount) == 0)
		{
			delete this;
			return 0;
		}

		return m_refCount;
	}

	virtual HRESULT LoadFromStream(
		_Inout_ IStream* pStream,
		_In_ DWORD grfMode) = 0;
	virtual void InitializeSearchContent() = 0;
	virtual void BeginReadChunks() = 0;

	virtual _Success_(return != NULL) BOOL GetThumbnail(
		_In_ UINT cx,
		_Out_ HBITMAP* phbmp,
		_Out_ WTS_ALPHATYPE* pdwAlpha) = 0;

	virtual void ClearChunkList() = 0;
	virtual BOOL SetChunkValue(
		_In_ IFilterChunkValue* value) = 0;
	virtual _Success_(return != FALSE) BOOL ReadNextChunkValue(
		_Outptr_result_maybenull_ IFilterChunkValue** value) = 0;
	virtual void RemoveChunk(
		_In_ REFCLSID guid,
		_In_ DWORD pid) = 0;
	virtual POSITION FindChunk(
		_In_ REFCLSID guid,
		_In_ DWORD pid) = 0;

	virtual LPVOID GetContainer() const = 0;

protected:
	volatile LONG m_refCount;
};

#ifdef _MFC_VER
#define IPreviewCtrl CMFCPreviewCtrlImpl
#else
interface IPreviewCtrl
{
protected:
	IPreviewCtrl()
	{
	}

public:
	virtual ~IPreviewCtrl()
	{
	}

	void SetDocument(_In_ IDocument* pDocument)
	{
		m_pDocument = pDocument;
	}

	IDocument* GetDocument()
	{
		return m_pDocument;
	}

	virtual BOOL Create(
		_In_ HWND hWndParent,
		_In_ const RECT* prc) = 0;
	virtual void Destroy() = 0;
	virtual void SetHost(_In_ HWND hWndParent) = 0;
	virtual void Focus() = 0;
	virtual void Redraw() = 0;
	virtual void SetRect(
		_In_ const RECT* prc,
		_In_ BOOL bRedraw) = 0;
	virtual void SetPreviewVisuals(
		_In_ COLORREF clrBack,
		_In_ COLORREF clrText,
		_In_ const LOGFONTW *plf) = 0;

protected:
	IDocument* m_pDocument;
};
#endif

} //namespace ATL
#endif // __ATL_HANDLER__
