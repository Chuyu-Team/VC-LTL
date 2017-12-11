// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATL_HANDLER_IMPL__
#define __ATL_HANDLER_IMPL__

#pragma once

#if (NTDDI_VERSION < NTDDI_VISTA) // min Windows Vista required
#error Preview/Thumbnail/Search handlers are not supported on Windows versions prior to Vista.
#endif

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#endif //!_ATL_NO_PRAGMA_WARNINGS

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#include <Filterr.h>
#include <atlhandler.h>
#include <atlcoll.h>
#include <shobjidl.h>
#include <propsys.h>
#include <thumbcache.h>

#pragma pack(push,_ATL_PACKING)
namespace ATL
{

class ATL_NO_VTABLE CThumbnailProviderImpl :
	public IInitializeWithStream,
	public IThumbnailProvider
{
public:
	CThumbnailProviderImpl() : m_pStream(NULL), m_pDocument(NULL)
	{
	}

	virtual ~CThumbnailProviderImpl()
	{
	}

	void FinalRelease()
	{
		ReleaseAll();
	}

ATLPREFAST_SUPPRESS(6387 28196)
	//IThumbnailProvider implementation
	STDMETHOD (GetThumbnail)(
		UINT cx,
		__RPC__deref_out_opt HBITMAP *phbmp,
		__RPC__out WTS_ALPHATYPE *pdwAlpha)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if ((phbmp == NULL) || (pdwAlpha == NULL))
		{
			return E_POINTER;
		}

		*phbmp = NULL;
		*pdwAlpha = WTSAT_UNKNOWN;

		m_pDocument = CreateDocument();
		if (m_pDocument == NULL)
		{
			return E_POINTER;
		}

		m_pDocument->AddRef();

		HRESULT hr = LoadDocumentFromStream(m_pStream, m_grfMode);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = GetBitmap(cx, phbmp, pdwAlpha);

		ReleaseAll();
		return hr;
	}
ATLPREFAST_UNSUPPRESS()

	//IInitializeWithStream implementation
	STDMETHOD (Initialize)(
		_Inout_ IStream *pStream,
		_In_ DWORD grfMode)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		// Initialize can be called more than once, so release existing valid m_pStream
		if (m_pStream != NULL)
		{
			m_pStream->Release();
		}

		m_pStream = pStream;
		m_pStream->AddRef();

		m_grfMode = grfMode | STGM_SHARE_EXCLUSIVE;
		return S_OK;
	}

protected:
	virtual IDocument* CreateDocument()
	{
		return NULL;
	}

	virtual HRESULT LoadDocumentFromStream(
		_Inout_ IStream* pStream,
		_In_ DWORD grfMode)
	{
		if (m_pDocument == NULL)
		{
			return E_POINTER;
		}
		return m_pDocument->LoadFromStream(pStream, grfMode);
	}

	virtual HRESULT GetBitmap(
		_In_ UINT /* cx */,
		_In_opt_ HBITMAP * /* phbmp */,
		_In_opt_ WTS_ALPHATYPE * /* pdwAlpha */)
	{
		return E_NOTIMPL;
	}

	void ReleaseAll()
	{
		if (m_pStream != NULL)
		{
			m_pStream->Release();
			m_pStream = NULL;
		}

		if (m_pDocument != NULL)
		{
			m_pDocument->Release();
			m_pDocument = NULL;
		}
	}
protected:
	IStream*   m_pStream;
	IDocument* m_pDocument;
	DWORD      m_grfMode;

};

template <class T>
class ATL_NO_VTABLE CPreviewHandlerImpl :
	public IObjectWithSiteImpl<T>,
	public IPreviewHandler,
	public IOleWindow,
	public IInitializeWithStream,
	public IPreviewHandlerVisuals
{
public:
	CPreviewHandlerImpl(void) :
		m_pPreviewCtrl(NULL), m_pStream(NULL),
		m_pFrame(NULL), m_hWndHost(NULL),
		m_pDocument(NULL), m_clrBackColor(RGB(255, 255, 255)),
		m_clrTextColor(0), m_pLogFont(NULL)
	{
		::SetRectEmpty(&m_rectHost);
	}
	virtual ~CPreviewHandlerImpl()
	{
	}

	void FinalRelease()
	{
		ReleaseStream();

		if (m_pPreviewCtrl != NULL)
		{
			m_pPreviewCtrl->Destroy();
			m_pPreviewCtrl = NULL;
		}

		if (m_pFrame != NULL)
		{
			m_pFrame->Release();
			m_pFrame = NULL;
		}

		if (m_pDocument != NULL)
		{
			m_pDocument->Release();
			m_pDocument = NULL;
		}

		m_hWndHost = NULL;
		SetRectEmpty(&m_rectHost);
	}

	//IPreviewHandler implementation
	STDMETHOD (SetWindow)(
		_In_ HWND hwnd,
		_In_ const RECT *prc)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		m_hWndHost = hwnd;
		CopyRect(&m_rectHost, prc);

		if (m_pPreviewCtrl != NULL)
		{
			m_pPreviewCtrl->SetHost(hwnd);
			m_pPreviewCtrl->SetRect(prc, FALSE);
		}

		return S_OK;
	}

	STDMETHOD (SetRect)(_In_ const RECT *prc)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		CopyRect(&m_rectHost, prc);

		if (m_pPreviewCtrl != NULL)
		{
			m_pPreviewCtrl->SetRect(prc, TRUE);
		}

		return S_OK;
	}

	IFACEMETHODIMP DoPreview()
	{
		if (m_pPreviewCtrl == NULL)
		{
#ifdef _AFXDLL
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif

#ifdef AFX_PREVIEW_STANDALONE
			m_pDocument = CreateDocument();

			if (m_pDocument == NULL)
			{
				return E_FAIL;
			}

			m_pDocument->AddRef();
			m_pPreviewCtrl = CreatePreviewControl();

			if (m_pPreviewCtrl == NULL || !m_pPreviewCtrl->Create(m_hWndHost, &m_rectHost))
			{
				return E_FAIL;
			}

			m_pPreviewCtrl->SetDocument(m_pDocument);
#else
			if (FAILED(InitializeDocumentPreview(m_hWndHost, &m_rectHost)))
			{
				return E_FAIL;
			}
#endif

			if (m_pPreviewCtrl != NULL)
			{
				m_pPreviewCtrl->SetPreviewVisuals(m_clrBackColor, m_clrTextColor, m_pLogFont);
			}

			// support for loading from archive
			LoadDocumentFromStream(m_pStream);

			// we can release stream now, otherwise the file remains locked for a long time
			ReleaseStream();
		}

		if (m_pPreviewCtrl != NULL)
		{
			m_pPreviewCtrl->Redraw();
		}

		return S_OK;
	}

	IFACEMETHODIMP Unload()
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		ReleaseStream();

		if (m_pPreviewCtrl != NULL)
		{
			m_pPreviewCtrl->Destroy();
			m_pPreviewCtrl = NULL;
		}

		if (m_pDocument != NULL)
		{
			m_pDocument->Release();
			m_pDocument = NULL;
		}

		if (m_pFrame != NULL)
		{
			m_pFrame->Release();
			m_pFrame = NULL;
		}

		return S_OK;
	}

	IFACEMETHODIMP SetFocus()
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_FALSE;

		if (m_pPreviewCtrl != NULL)
		{
			m_pPreviewCtrl->Focus();
			hr = S_OK;
		}

		return hr;
	}

	STDMETHOD (QueryFocus)(_Out_ HWND *phwnd)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_OK;

		*phwnd = ::GetFocus();
		if (*phwnd == NULL)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		return hr;
	}

	STDMETHOD (TranslateAccelerator)(_In_ MSG *pmsg)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_FALSE;

		if (m_pFrame != NULL)
		{
			// If your previewer has multiple tab stops, you will need to do appropriate first/last child checking.
			// This sample previewer has no tabstops, so we want to just forward this message out
			hr = m_pFrame->TranslateAccelerator(pmsg);
		}

		return hr;
	}

	// IPreviewHandlerVisuals implementation
	STDMETHOD (SetBackgroundColor)(_In_ COLORREF color)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		m_clrBackColor = color;
		return S_OK;
	}

	STDMETHOD (SetFont)(_In_ const LOGFONTW *plf)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		m_pLogFont = (LOGFONTW*) plf;
		return S_OK;
	}

	STDMETHOD (SetTextColor)(_In_ COLORREF color)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		m_clrTextColor = color;
		return S_OK;
	}

	// IObjectWithSite override
	STDMETHOD (SetSite)(_Inout_opt_ IUnknown *punkSite)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		__super::SetSite(punkSite);

		// Clean up old frame
		if (m_pFrame != NULL)
		{
			m_pFrame->Release();
			m_pFrame = NULL;
		}

		// Get the new frame
		if (m_spUnkSite)
		{
			m_spUnkSite->QueryInterface(&m_pFrame);
		}

		return S_OK;
	}

	// IOleWindow
	STDMETHOD (GetWindow)(_Out_ HWND *phwnd)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		*phwnd = m_hWndHost;
		return S_OK;
	}

	STDMETHOD (ContextSensitiveHelp)(_In_ BOOL)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return E_NOTIMPL;
	}

	//IInitializeWithStream implementation
	STDMETHOD (Initialize)(
		_Inout_ IStream *pStream,
		_In_ DWORD grfMode)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		// Initialize can be called more than once, so release existing valid m_pStream
		ReleaseStream();

		m_pStream = pStream;
		m_pStream->AddRef();

		m_grfMode = grfMode;
		return S_OK;
	}

protected:
	virtual HRESULT InitializeDocumentPreview(
		_In_ HWND /* hWnd */,
		_In_opt_ RECT* /* prc */)
	{
		return E_NOTIMPL;
	}

	virtual IPreviewCtrl* CreatePreviewControl()
	{
		return NULL;
	}

	virtual IDocument* CreateDocument()
	{
		return NULL;
	}

	virtual HRESULT LoadDocumentFromStream(_Inout_ IStream* pStream)
	{
		if (m_pDocument == NULL)
		{
			return E_POINTER;
		}

		return m_pDocument->LoadFromStream(pStream, m_grfMode);
	}

	void ReleaseStream()
	{
		if (m_pStream != NULL)
		{
			m_pStream->Release();
			m_pStream = NULL;
		}
	}

protected:
	IPreviewCtrl*          m_pPreviewCtrl;
	IStream*               m_pStream;
	DWORD                  m_grfMode;
	IPreviewHandlerFrame*  m_pFrame;
	HWND                   m_hWndHost;
	RECT                   m_rectHost;

	COLORREF               m_clrBackColor;
	COLORREF               m_clrTextColor;
	LOGFONTW*              m_pLogFont;

	IDocument*             m_pDocument;
};

class ATL_NO_VTABLE CSearchFilterImpl :
	public IPersistStream,
	public IPersistFile,
	public IFilter
{
public:

	CSearchFilterImpl(void) :
		m_pStream(NULL), m_pDocument(NULL),
		m_dwChunkId(0), m_iText(0), m_pCurrentChunk(NULL)
	{
		m_hEventLog = NULL;
		m_bEnableEventLog = FALSE;
	}

	virtual ~CSearchFilterImpl()
	{
	}

	void FinalRelease()
	{
		ReleaseAll();

		if (m_hEventLog != NULL)
		{
			CloseEventLog(m_hEventLog);
			m_hEventLog = NULL;
		}
	}

public:
	// IFilter implementation
	SCODE STDMETHODCALLTYPE Init(
		ULONG /* grfFlags */,
		ULONG /* cAttributes */,
		const FULLPROPSPEC * /* aAttributes */,
		ULONG * /* pFlags */)
	{

#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_OK;

		if (m_hEventLog == NULL && m_bEnableEventLog)
		{
			m_hEventLog = OpenEventLog(NULL, L"Search Handler");
		}

		if (m_bEnableEventLog)
		{
			PCWSTR pszMsg = L"IFilter::Init entered.";
			ReportEvent(m_hEventLog, EVENTLOG_SUCCESS, 0, 1, NULL, 1, 0, &pszMsg, NULL);
		}

		m_pDocument = CreateDocument();
		if (m_pDocument == NULL)
		{
			return E_FAIL;
		}

		m_pDocument->AddRef();
		hr = LoadDocumentFromStream(m_pStream, STGM_READ | STGM_SHARE_EXCLUSIVE);

		if (FAILED(hr))
		{
			if (m_bEnableEventLog)
			{
				PCWSTR pszMsg = L"IFilter::Init (LoadDocumentFromStream) failed.";
				ReportEvent(m_hEventLog, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, sizeof (DWORD), &pszMsg, &hr);
			}
			return hr;
		}

		m_pDocument->InitializeSearchContent();
		m_pDocument->BeginReadChunks();

		return S_OK;
	}

	SCODE STDMETHODCALLTYPE GetChunk(STAT_CHUNK *pStat)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_OK;

		try
		{
			// Get the chunk from the derived class.  A return of S_FALSE indicates the
			// chunk should be skipped and we should try to get the next chunk.

			int cIterations = 0;
			hr = S_FALSE;

			while (S_FALSE == hr)
			{
				pStat->idChunk = m_dwChunkId;

				hr = S_OK;
				if (!m_pDocument->ReadNextChunkValue(&m_pCurrentChunk))
				{
					hr = FILTER_E_END_OF_CHUNKS;
				}
				++cIterations;
			}

			if (hr == S_OK)
			{
				if (m_pCurrentChunk != NULL && m_pCurrentChunk->IsValid())
				{
					// copy out the STAT_CHUNK
					m_pCurrentChunk->CopyChunk(pStat);

					// and set the id to be the sequential chunk
					pStat->idChunk = ++m_dwChunkId;
				}
				else
				{
					hr = E_INVALIDARG;
				}
			}
		}
		catch (CAtlException e)
		{
			ATLTRACE2(atlTraceGeneral, 4, L"CMFCWSFilterImpl::GetChunk ATL exception! hr = 0x%x", e.m_hr);
			hr = e.m_hr;
		}

		if (m_bEnableEventLog)
		{
			PCWSTR pszMsg = L"IFilter::GetChunk.";
			ReportEvent(m_hEventLog, EVENTLOG_SUCCESS, 0, 2, NULL, 1, sizeof (DWORD), &pszMsg, &hr);
		}

		return hr;
	}

	SCODE STDMETHODCALLTYPE GetText(
		ULONG *pcwcBuffer,
		WCHAR *awcBuffer)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_OK;

		if ((pcwcBuffer == NULL) || (*pcwcBuffer == 0))
		{
			return E_INVALIDARG;
		}

		if (m_pCurrentChunk == NULL || !m_pCurrentChunk->IsValid())
		{
			return FILTER_E_NO_MORE_TEXT;
		}

		if (m_pCurrentChunk->GetChunkType() != CHUNK_TEXT)
		{
			return FILTER_E_NO_TEXT;
		}

		try
		{
			int cchTotal = m_pCurrentChunk->GetString().GetLength();
			int cchLeft = cchTotal - m_iText;
			int cchToCopy = min((int)*pcwcBuffer - 1, cchLeft);

			if (cchToCopy > 0)
			{

				PCWSTR psz = ((PCWSTR)m_pCurrentChunk->GetString()) + m_iText;
				// copy the chars
				wcsncpy_s(awcBuffer, *pcwcBuffer, psz, cchToCopy);
				// null terminate it
				awcBuffer[cchToCopy] = '\0';

				// set how much data is copied
				*pcwcBuffer = cchToCopy;

				// remember we copied it
				m_iText += cchToCopy;
				cchLeft -= cchToCopy;

				if (cchLeft == 0)
				{
					hr = FILTER_S_LAST_TEXT;
				}
			}
			else
			{
				hr = FILTER_E_NO_MORE_TEXT;
			}
		}
		catch (CAtlException e)
		{
			ATLTRACE2(atlTraceGeneral, 4, L"CMFCWSFilterImpl::GetText ATL exception! hr = 0x%x", e.m_hr);
			hr = e.m_hr;
		}

		if (m_bEnableEventLog)
		{
			PCWSTR pszMsg = L"IFilter::GetText.";
			ReportEvent(m_hEventLog, EVENTLOG_SUCCESS, 0, 3, NULL, 1, sizeof(DWORD), &pszMsg, &hr);
		}

		return hr;
	}

	SCODE STDMETHODCALLTYPE GetValue(PROPVARIANT **ppPropValue)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = S_OK;

		// if this is not a value chunk they shouldn't be calling this
		if (m_pCurrentChunk == NULL || m_pCurrentChunk->GetChunkType() != CHUNK_VALUE)
		{
			return FILTER_E_NO_MORE_VALUES;
		}

		try
		{
			if (m_pCurrentChunk->IsValid())
			{
				// return the value of this chunk as a PROPVARIANT ( they own freeing it properly )
				hr = m_pCurrentChunk->GetValue(ppPropValue);
				m_pCurrentChunk->Clear();
			}
			else
			{
				// we have already return the value for this chunk, go away
				hr = FILTER_E_NO_MORE_VALUES;
			}
		}
		catch (CAtlException e)
		{
			ATLTRACE2(atlTraceGeneral, 4, L"CMFCWSFilterImpl::GetValue ATL exception! hr = 0x%x", e.m_hr);
			hr = e.m_hr;
		}

		if (m_bEnableEventLog)
		{
			PCWSTR pszMsg = L"IFilter::GetValue.";
			ReportEvent(m_hEventLog, EVENTLOG_SUCCESS, 0, 4, NULL, 1, sizeof(DWORD), &pszMsg, &hr);
		}

		return hr;
	}

	SCODE STDMETHODCALLTYPE BindRegion(
		FILTERREGION /* origPos */,
		REFIID /* riid */,
		void ** /* ppunk */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		ATLTRACE2(atlTraceGeneral, 4, L"BindRegion: not implemented");
		return E_NOTIMPL;
	}

	// IPersistStream implementation
	// IPersistStream::Load() is used by WS 3.x and above for the index
	// search case when this filter is loaded into the filter host process
	STDMETHODIMP Load(__RPC__in_opt IStream *pStream)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_hEventLog == NULL)
		{
			m_hEventLog = OpenEventLog(NULL, L"Search Handler");
		}

		if (pStream == NULL)
		{
			return E_FAIL;
		}

		// Initialize can be called more than once, so release existing valid m_pStream
		if (m_pStream != NULL)
		{
			m_pStream->Release();
		}

		m_pStream = pStream;
		m_pStream->AddRef();

		if (m_bEnableEventLog)
		{
			PCWSTR pszMsg = L"IPersistStream::Load entered.";
			ReportEvent(m_hEventLog, EVENTLOG_SUCCESS, 0, 0, NULL, 1, 0, &pszMsg, NULL);
		}

		return S_OK;
	};

	STDMETHODIMP Save(
		__RPC__in_opt IStream * /* pStm */,
		BOOL /* fClearDirty */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return E_NOTIMPL; 
	};

	STDMETHODIMP GetSizeMax(_Out_ ULARGE_INTEGER * /* pcbSize */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return E_NOTIMPL;
	};

	STDMETHODIMP IsDirty()
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return S_FALSE;
	}

	STDMETHODIMP GetClassID(_In_opt_ CLSID * /* pClassID */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		ATLTRACE2(atlTraceGeneral, 4, L"GetClassID: must be implemented in a derived class.");
		return E_NOTIMPL;
	}

	// IPersistFile implementation
	STDMETHODIMP Load(
		_In_z_ LPCWSTR pszFileName,
		_In_ DWORD dwMode)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		HRESULT hr = SHCreateStreamOnFileEx(pszFileName, dwMode, 0, FALSE, NULL, &m_pStream);
		return hr;
	};

	// these methods are never called by WS
	STDMETHODIMP Save(
		_In_opt_z_ LPCWSTR /* pszFileName */,
		_In_ BOOL /* fRemember */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return E_NOTIMPL;
	};

	STDMETHODIMP SaveCompleted(
		_In_opt_z_ LPCWSTR /* pszFileName */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return E_NOTIMPL;
	};

	STDMETHODIMP GetCurFile(
		_In_opt_ LPWSTR* /* pszFileName */)
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		return E_NOTIMPL;
	};

protected:
	virtual IDocument* CreateDocument()
	{
		return NULL;
	}

	virtual HRESULT LoadDocumentFromStream(
		_Inout_ IStream* pStream,
		_In_ DWORD grfMode)
	{
		if (m_pDocument == NULL)
		{
			return E_POINTER;
		}

		return m_pDocument->LoadFromStream(pStream, grfMode);
	}

	void ReleaseAll()
	{
#ifdef _AFXDLL
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		if (m_pStream != NULL)
		{
			m_pStream->Release();
			m_pStream = NULL;
		}

		if (m_pDocument != NULL)
		{
			m_pDocument->Release();
			m_pDocument = NULL;
		}
	}

protected:
	IStream*                    m_pStream;
	IDocument*                  m_pDocument;

	IFilterChunkValue*          m_pCurrentChunk;    // the current chunk value
	DWORD                       m_dwChunkId;        // Current chunk id
	DWORD                       m_iText;            // index into ChunkValue

	HANDLE                      m_hEventLog;
	BOOL                        m_bEnableEventLog;
};

class CFilterChunkValueImpl : 
	public IFilterChunkValue
{
public:
	CFilterChunkValueImpl()
	{
		Clear();
	}

	~CFilterChunkValueImpl()
	{
		Clear();
	}

	virtual void Clear()
	{
		m_fIsValid = false;
		ZeroMemory(&m_chunk, sizeof(m_chunk));
		PropVariantInit(&m_propVariant);
		m_strValue.Empty();
	}

	virtual BOOL IsValid() const
	{
		return m_fIsValid;
	}

ATLPREFAST_SUPPRESS(6387)
	virtual HRESULT GetValue(_Outptr_ PROPVARIANT **ppPropVariant)
	{
		HRESULT hr = S_OK;

		*ppPropVariant = NULL;
		CComHeapPtr<PROPVARIANT> spPropVariant;

		// allocate a propvariant on the COM heap
		if (spPropVariant.Allocate())
		{
			// fill it with COMHeap copied data from our local non-com heap propvariant
			hr = PropVariantCopy(spPropVariant, &m_propVariant);
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}

		if (SUCCEEDED(hr))
		{
			// detach and return this as the value
			*ppPropVariant = spPropVariant.Detach();
		}
				
		return hr;
	}
ATLPREFAST_UNSUPPRESS()
	
	virtual PROPVARIANT GetValueNoAlloc()
	{
		return m_propVariant;
	}

	virtual CString& GetString()
	{
		return m_strValue;
	}

	virtual HRESULT CopyChunk(_Out_ STAT_CHUNK *pStatChunk)
	{
		*pStatChunk = m_chunk;
		return S_OK;
	}

	virtual CHUNKSTATE GetChunkType() const
	{
		return m_chunk.flags;
	}

	virtual REFGUID GetChunkGUID() const
	{
		return m_chunk.attribute.guidPropSet;
	}

	virtual DWORD GetChunkPID() const
	{
		return m_chunk.attribute.psProperty.propid;
	}

	// set the property by key to a unicode string
	virtual HRESULT SetTextValue(
		_In_ REFPROPERTYKEY pkey,
		_In_z_ LPCTSTR pszValue,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_strValue = pszValue;
			m_propVariant.vt = VT_LPWSTR;
			m_propVariant.pwszVal = (LPWSTR)(LPCWSTR)m_strValue;
			m_fIsValid = true;
		}

		return hr;
	}

	// set the property by key to a bool
	virtual HRESULT SetBoolValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ BOOL bVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		return SetBoolValue(pkey, bVal ? VARIANT_TRUE : VARIANT_FALSE, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
	}

	// set the property by key to a variant bool
	virtual HRESULT SetBoolValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ VARIANT_BOOL bVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_BOOL;
			m_propVariant.boolVal = bVal;
			m_fIsValid = true;
		}

		return hr;
	}

	// set the property by key to an int
	virtual HRESULT SetIntValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ int nVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_I4;
			m_propVariant.lVal = nVal;
			m_fIsValid = true;
		}

		return hr;
	}

	// set the property by key to a long
	virtual HRESULT SetLongValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ long lVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_I4;
			m_propVariant.lVal = lVal;
			m_fIsValid = true;
		}

		return hr;
	}

	// set the property by key to a dword
	virtual HRESULT SetDwordValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ DWORD dwVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_UI4;
			m_propVariant.ulVal = dwVal;
			m_fIsValid = true;
		}

		return hr;
	}

	// set property by key to an int64
	virtual HRESULT SetInt64Value(
		_In_ REFPROPERTYKEY pkey,
		_In_ __int64 nVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_I8;
			m_propVariant.hVal.QuadPart = nVal;
			m_fIsValid = true;
		}

		return hr;
	}

	// set property by key to a SystemTime
	virtual HRESULT SetSystemTimeValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ const SYSTEMTIME &systemTime,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_FILETIME;
			SystemTimeToFileTime(&systemTime, &m_propVariant.filetime);
			m_fIsValid = true;
		}

		return hr;
	}

	// set Property by key to a filetime
	virtual HRESULT SetFileTimeValue(
		_In_ REFPROPERTYKEY pkey,
		_In_ FILETIME dtVal,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		HRESULT hr = SetChunk(pkey, chunkType, locale, cwcLenSource, cwcStartSource, chunkBreakType);
		if (SUCCEEDED(hr))
		{
			m_propVariant.vt = VT_FILETIME;
			m_propVariant.filetime = dtVal;
			m_fIsValid = true;
		}

		return hr;
	}

	virtual void CopyFrom(_Inout_ IFilterChunkValue* pValue)
	{
		Clear();

		pValue->CopyChunk (&m_chunk);

		// NOTE this object does not contain a real property variant that was allocated with CoTaskMemAlloc
		m_propVariant = pValue->GetValueNoAlloc ();
		m_strValue = pValue->GetString ();

		// fix up the string ownership to member
		if (m_propVariant.vt == VT_LPWSTR)
		{
			m_propVariant.pwszVal = (LPWSTR)(LPCWSTR)m_strValue;
		}
		else if (m_propVariant.vt & VT_VECTOR)
		{
		}

		m_fIsValid = pValue->IsValid();
		return;
	}

protected:
	// set the locale for this chunk
	HRESULT SetChunk(
		_In_ REFPROPERTYKEY pkey,
		_In_ CHUNKSTATE chunkType = CHUNK_VALUE,
		_In_ LCID locale = 0,
		_In_ DWORD cwcLenSource = 0,
		_In_ DWORD cwcStartSource = 0,
		_In_ CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK)
	{
		Clear();

		// initialize the chunk
		m_chunk.attribute.psProperty.ulKind = PRSPEC_PROPID;
		m_chunk.attribute.psProperty.propid = pkey.pid;
		m_chunk.attribute.guidPropSet = pkey.fmtid;
		m_chunk.flags = chunkType;
		m_chunk.locale = locale == 0 ? GetUserDefaultLCID() : locale;
		m_chunk.cwcLenSource = cwcLenSource;
		m_chunk.cwcStartSource = cwcStartSource;
		m_chunk.breakType = chunkBreakType;

		return S_OK;
	}

	// member variables
private:
	BOOL        m_fIsValid;
	STAT_CHUNK  m_chunk;
	PROPVARIANT m_propVariant;
	CString     m_strValue;
};

class CAtlDocumentImpl : 
	public IDocument
{
public:
	virtual void BeginReadChunks()
	{
		m_posReadChunk = m_lstChunks.GetHeadPosition();
	}

	virtual void ClearChunkList()
	{
		m_posReadChunk = NULL;

		POSITION pos = m_lstChunks.GetHeadPosition();
		while (pos != NULL)
		{
			IFilterChunkValue* v = (IFilterChunkValue*) m_lstChunks.GetNext(pos);
			if (v != NULL)
			{
				delete v;
			}
		}

		m_lstChunks.RemoveAll();
	}

	virtual BOOL SetChunkValue(_Inout_ IFilterChunkValue* pValue)
	{
		if (pValue == NULL || !pValue->IsValid())
		{
			return false;
		}

		POSITION pos = FindChunk(pValue->GetChunkGUID(), pValue->GetChunkPID());

		if (pos == NULL)
		{
			m_lstChunks.AddTail(pValue);
		}

		return TRUE;
	}

	virtual _Success_(return != FALSE) BOOL ReadNextChunkValue(_Outptr_ IFilterChunkValue** ppValue)
	{
		if (m_posReadChunk == NULL || ppValue == NULL)
		{
			return FALSE;
		}

		*ppValue = (IFilterChunkValue*) m_lstChunks.GetNext(m_posReadChunk);
		return TRUE;
	}

	virtual void RemoveChunk(
		_In_ REFCLSID guid,
		_In_ DWORD pid)
	{
		POSITION pos = FindChunk(guid, pid);
		if (pos != NULL)
		{
			delete m_lstChunks.GetAt(pos);
			m_lstChunks.RemoveAt(pos);
		}
	}

	virtual POSITION FindChunk(
		_In_ REFCLSID guid,
		_In_ DWORD pid)
	{
		POSITION value = NULL;
		POSITION pos = m_lstChunks.GetHeadPosition();
		POSITION posPrev = NULL;
		while (pos != NULL)
		{
			posPrev = pos;
			IFilterChunkValue* v = (IFilterChunkValue*) m_lstChunks.GetNext(pos);
			if (IsEqualIID(v->GetChunkGUID(), guid) && v->GetChunkPID() == pid)
			{
				value = posPrev;
				break;
			}
		}

		return value;
	}

	ATLPREFAST_SUPPRESS(6101)
	_Success_(return != FALSE) BOOL GetThumbnail(
		_In_ UINT cx,
		_Out_ HBITMAP* phbmp,
		_Out_ WTS_ALPHATYPE* /* pdwAlpha */)
	{
		HDC hdc = ::GetDC(NULL);
		RECT rcBounds;

		SetRect(&rcBounds, 0, 0, cx, cx);

		HDC hDrawDC = CreateCompatibleDC(hdc);
		if (hDrawDC == NULL)
		{
			ReleaseDC(NULL, hdc);
			return FALSE;
		}

		HBITMAP hBmp = CreateCompatibleBitmap(hDrawDC, cx, cx);
		if (hBmp == NULL)
		{
			ReleaseDC(NULL, hdc);
			DeleteDC(hDrawDC);
			return FALSE;
		}

		HBITMAP hOldBitmap = (HBITMAP) SelectObject(hDrawDC, hBmp);

		// Here you need to draw the document's data
		OnDrawThumbnail(hDrawDC, &rcBounds);

		SelectObject(hDrawDC, hOldBitmap);

		DeleteDC(hDrawDC);
		ReleaseDC(NULL, hdc);

		*phbmp = hBmp;
		return TRUE;
	}
	ATLPREFAST_UNSUPPRESS()

	virtual void OnDrawThumbnail(
		_In_ HDC /* hDrawDC */,
		_In_ LPRECT /* lprcBounds */)
	{
	}

	virtual LPVOID GetContainer() const
	{
		return (LPVOID) this;
	}

protected:
	CAtlList<IFilterChunkValue*> m_lstChunks;
	POSITION                     m_posReadChunk;
};

} //namespace ATL

#pragma pack(pop)

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (pop)
#endif //!_ATL_NO_PRAGMA_WARNINGS

#endif // __ATL_HANDLER_IMPL__
