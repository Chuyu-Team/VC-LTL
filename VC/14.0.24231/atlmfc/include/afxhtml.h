// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXHTML_H__
#define __AFXHTML_H__

#pragma once

#ifndef __AFXDISP_H__
	#include <afxdisp.h>
#endif

#ifndef __exdisp_h__
	#include <exdisp.h>
#endif

#ifndef __mshtmhst_h__
#include <mshtmhst.h>
#endif

#ifndef __mshtml_h__
#include <mshtml.h>
#endif

#ifndef __mshtmcid_h__
#include <mshtmcid.h>
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif 

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#pragma warning(push)
#pragma warning(disable:4571)//catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions

/////////////////////////////////////////////////////////////////////////////
// AFXHTML - MFC Visual HTML classes

// Classes declared in this file

//CObject
	//CCmdTarget;
		//CWnd,CHtmlEditCtrlBase<CHtmlEditCtrl>
			class CHtmlEditCtrl;

//CObject
	//CCmdTarget;
		//CWnd
			//CView
				//CScrollView
					//CFormView
						class CHtmlView;

//CObject
	//CCmdTarget;
		//CWnd
			//CView
				//CScrollView
					//CFormView,CHtmlEditCtrlBase<CHtmlEditView>
						class CHtmlEditView;

//CObject
	//CCmdTarget;
		//CDocument
			class CHtmlEditDoc;


#undef AFX_DATA
#define AFX_DATA AFX_OLE_DATA

/*============================================================================*/
// CHtmlView

class CHtmlView : public CFormView
{
protected: // create from serialization only
	CHtmlView();
	DECLARE_DYNCREATE(CHtmlView)
	DECLARE_EVENTSINK_MAP()

// Attributes
public:
	CString GetType() const;
	long GetLeft() const;
	void SetLeft(long nNewValue);
	long GetTop() const;
	void SetTop(long nNewValue);
	long GetHeight() const;
	void SetHeight(long nNewValue);
	long GetWidth() const;
	void SetWidth(long nNewValue);
	void SetVisible(BOOL bNewValue);
	BOOL GetVisible() const;
	CString GetLocationName() const;
	READYSTATE GetReadyState() const;
	BOOL GetOffline() const;
	void SetOffline(BOOL bNewValue);
	BOOL GetSilent() const;
	void SetSilent(BOOL bNewValue);
	BOOL GetTopLevelContainer() const;
	CString GetLocationURL() const;
	BOOL GetBusy() const;
	LPDISPATCH GetApplication() const;
	LPDISPATCH GetParentBrowser() const;
	LPDISPATCH GetContainer() const;
	LPDISPATCH GetHtmlDocument() const;
	CString GetFullName() const;
	int GetToolBar() const;
	void SetToolBar(int nNewValue);
	BOOL GetMenuBar() const;
	void SetMenuBar(BOOL bNewValue);
	BOOL GetFullScreen() const;
	void SetFullScreen(BOOL bNewValue);
	OLECMDF QueryStatusWB(OLECMDID cmdID) const;
	BOOL GetRegisterAsBrowser() const;
	void SetRegisterAsBrowser(BOOL bNewValue);
	BOOL GetRegisterAsDropTarget() const;
	void SetRegisterAsDropTarget(BOOL bNewValue);
	BOOL GetTheaterMode() const;
	void SetTheaterMode(BOOL bNewValue);
	BOOL GetAddressBar() const;
	void SetAddressBar(BOOL bNewValue);
	BOOL GetStatusBar() const;
	void SetStatusBar(BOOL bNewValue);

// Operations
public:
	void GoBack();
	void GoForward();
	void GoHome();
	void GoSearch();
	void Navigate(LPCTSTR URL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
	void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL,	LPCTSTR lpszHeaders = NULL,
		LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
		CByteArray& baPostedData,
		LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL);
	void Refresh();
	void Refresh2(int nLevel);
	void Stop();
	void PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
	void PutProperty(LPCTSTR lpszPropertyName, double dValue);
	void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue);
	void PutProperty(LPCTSTR lpszPropertyName, long lValue);
	void PutProperty(LPCTSTR lpszPropertyName, short nValue);
	BOOL GetProperty(LPCTSTR lpszProperty, CString& strValue);
	COleVariant GetProperty(LPCTSTR lpszProperty);
	void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn,
		VARIANT* pvaOut);
	virtual BOOL GetSource(CString& strRef);
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);

	HRESULT QueryFormsCommand(DWORD dwCommandID, BOOL* pbSupported,
								BOOL* pbEnabled, BOOL* pbChecked);
	HRESULT ExecFormsCommand(DWORD dwCommandID, VARIANT* pVarIn,
								VARIANT* pVarOut);

// Overrides
public:
	virtual BOOL CreateControlSite(COleControlContainer* pContainer, 
	   COleControlSite** ppSite, UINT nID, REFCLSID clsid);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnFilePrint();

	// DocHostUIHandler overrideables
	virtual HRESULT OnShowContextMenu(DWORD dwID, LPPOINT ppt,
		LPUNKNOWN pcmdtReserved, LPDISPATCH pdispReserved);
	virtual HRESULT OnGetExternal(LPDISPATCH *lppDispatch);
	virtual HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo);
	virtual HRESULT OnShowUI(DWORD dwID,
		LPOLEINPLACEACTIVEOBJECT pActiveObject,
		LPOLECOMMANDTARGET pCommandTarget, LPOLEINPLACEFRAME pFrame,
		LPOLEINPLACEUIWINDOW pDoc);
	virtual HRESULT OnHideUI();
	virtual HRESULT OnUpdateUI();
	virtual HRESULT OnEnableModeless(BOOL fEnable);
	virtual HRESULT OnDocWindowActivate(BOOL fActivate);
	virtual HRESULT OnFrameWindowActivate(BOOL fActivate);
	virtual HRESULT OnResizeBorder(LPCRECT prcBorder,
		LPOLEINPLACEUIWINDOW pUIWindow, BOOL fFrameWindow);
	virtual HRESULT OnTranslateAccelerator(LPMSG lpMsg,
		const GUID* pguidCmdGroup, DWORD nCmdID);
	virtual HRESULT OnGetOptionKeyPath(LPOLESTR* pchKey, DWORD dwReserved);
	virtual HRESULT OnFilterDataObject(LPDATAOBJECT pDataObject,
		LPDATAOBJECT* ppDataObject);
	virtual HRESULT OnTranslateUrl(DWORD dwTranslate,
		OLECHAR* pchURLIn, OLECHAR** ppchURLOut);
	virtual HRESULT OnGetDropTarget(LPDROPTARGET pDropTarget,
		LPDROPTARGET* ppDropTarget);

	// Events
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	virtual void OnProgressChange(long nProgress, long nProgressMax);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnDownloadBegin();
	virtual void OnDownloadComplete();
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnPropertyChange(LPCTSTR lpszProperty);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnQuit();
	virtual void OnVisible(BOOL bVisible);
	virtual void OnToolBar(BOOL bToolBar);
	virtual void OnMenuBar(BOOL bMenuBar);
	virtual void OnStatusBar(BOOL bStatusBar);
	virtual void OnFullScreen(BOOL bFullScreen);
	virtual void OnTheaterMode(BOOL bTheaterMode);
	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);

	/// <summary>
	/// Called by the framework when it needs to obtain a bitmap to be displayed on Windows 7 tab thumbnail, 
	/// or on the client for application peek. </summary>
	/// <remarks>
	/// Calls OleDraw to display HTML elements.</remarks>
	/// <param name="dc"> Specifies the device context.</param>
	/// <param name="rect"> Specifies the bounding rectangle of area to render.</param>
	/// <param name="szRequiredThumbnailSize"> Specifies the size of target thumbnail. Should be ignored if bIsThumbnail is FALSE.</param>
	/// <param name="bIsThumbnail"> Specifies whether this method is called for iconic thumbnail or live preview (peek).</param>
	/// <param name="bAlphaChannelSet"> Output parameter. Set it to TRUE if your implementation initializes alpha channel of a bitmap
	/// selected in dc.</param> 
	virtual void OnDrawIconicThumbnailOrLivePreview(CDC& dc, CRect rect, CSize szRequiredThumbnailSize, BOOL bIsThumbnail, BOOL& bAlphaChannelSet);

// Implementation
public:
	virtual ~CHtmlView();
	virtual void OnDraw(CDC* pDC);
	CWnd m_wndBrowser;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	::ATL::CComPtr<IWebBrowser2> m_pBrowserApp;

// Event reflectors (not normally overridden)
protected:
	virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
		VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
		VARIANT* Headers, VARIANT_BOOL* Cancel);
	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	virtual void NavigateError(LPDISPATCH pDisp, VARIANT* pvURL,
		VARIANT* pvFrame, VARIANT* pvStatusCode, VARIANT_BOOL* pvbCancel);

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

// CStreamOnCString
// This is a helper class used by the CHtmlEditCtrlBase class to stream
// HTML into and out of the web browser control.
class CStreamOnCString : public IStream
{
public:
// Construction
	CStreamOnCString()
	{
		m_current_index = 0;
	}

	CStreamOnCString(LPCTSTR szData):
		m_strStream(szData)
	{
		m_current_index = 0;
	}

	virtual ~CStreamOnCString() {}

	// Copies the data currently held in this
	// object into a CString
	BOOL CopyData(CString& target)
	{
		target = m_strStream;
		return TRUE;
	}

	// Sets the value of 
	BOOL SetData(LPCTSTR szData)
	{
		_ATLTRY
		{
			m_strStream = szData;
		}
		_ATLCATCHALL()
		{
			return FALSE;
		}
		return TRUE;
	}

// Implementation
	STDMETHOD(QueryInterface)(REFIID iid, __RPC__deref_out void **ppUnk)
	{
		if (ppUnk == NULL) 
		{ 
			return E_POINTER; 
		}
		if (::InlineIsEqualGUID(iid, IID_IUnknown) ||
			::InlineIsEqualGUID(iid, IID_IStream) ||
			::InlineIsEqualGUID(iid, IID_ISequentialStream))
		{
			*ppUnk = (void*)this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef( void) 
	{
		return (ULONG)1;
	}

	ULONG STDMETHODCALLTYPE Release( void) 
	{
		return (ULONG)1;
	}

#if !defined(_CSTRING_DISABLE_NARROW_WIDE_CONVERSION)
	STDMETHOD(Read)(_Out_writes_bytes_to_(cb, *pcbRead) void *pv, ULONG cb, _Out_opt_ ULONG *pcbRead)
	{
		if (pcbRead)
			*pcbRead = 0;
		if (m_strAnsi.GetLength() == 0)
			m_strAnsi = m_strStream;

		if (!pv)
			return E_POINTER;

		unsigned int length = m_strAnsi.GetLength();
		char *pStream = m_strAnsi.GetBuffer();
		if (!pStream)
			return E_UNEXPECTED;

		UINT_PTR uSum = (UINT_PTR)pStream + length;
		if (uSum < static_cast<UINT_PTR>(length))
			return E_UNEXPECTED;
		char *pEnd = reinterpret_cast<char*>(uSum);

		uSum = (UINT_PTR)pStream + m_current_index;
		if (uSum < m_current_index)
			return E_UNEXPECTED;
		char *pStart = reinterpret_cast<char*>(uSum);

		if (pStart >= pEnd)
			return S_FALSE; // no more data to read
		
		int bytes_left = (int)(pEnd-pStart);
		int bytes_to_copy = (int)__min(bytes_left, (int)cb);
		if (bytes_to_copy <= 0)
		{
			// reset members so this stream can be used again
			m_current_index = 0;
			m_strAnsi.Empty();
			return S_FALSE;
		}

		::ATL::Checked::memcpy_s(pv, cb, (void*)uSum, bytes_to_copy);
		if (pcbRead)
			*pcbRead = (ULONG)bytes_to_copy;
		m_current_index += bytes_to_copy;
		m_strAnsi.ReleaseBuffer(0);
		return S_OK;
	}

	STDMETHOD(Write)(_In_reads_bytes_(cb) const void *pv, ULONG cb, _Out_opt_ ULONG *pcbWritten)
	{
		if (pcbWritten)
			*pcbWritten = 0;
		try
		{
#ifdef _UNICODE
			int flags = IS_TEXT_UNICODE_UNICODE_MASK;
			if (0 != IsTextUnicode((LPVOID)pv, cb, &flags))
			{
				// compiling UNICODE and got a UNICODE buffer
				UINT nCharsToSkip = flags & IS_TEXT_UNICODE_SIGNATURE ? 1 : 0;
				m_strStream.Append((wchar_t*)( ((wchar_t*)pv)+nCharsToSkip), (cb/sizeof(wchar_t))-nCharsToSkip);
				if (pcbWritten)
					*pcbWritten = cb-nCharsToSkip*sizeof(wchar_t);
			}
			else
			{
				// compiling UNICODE and got an ansi buffer
				// convert ansi buffer to UNICODE buffer
				CStringW strTemp((LPCSTR)pv, cb);
				m_strStream.Append(strTemp);
				if (pcbWritten)
					*pcbWritten = cb;
				
			}
#else
			int flags = IS_TEXT_UNICODE_UNICODE_MASK;
			if (0 != IsTextUnicode((LPVOID)pv, cb, &flags))
			{
				// compiling ANSI and got a UNICODE buffer
				UINT nCharsToSkip = flags & IS_TEXT_UNICODE_SIGNATURE ? 1 : 0;
				CStringA strTemp((const wchar_t*)pv + nCharsToSkip, (cb/sizeof(wchar_t))- nCharsToSkip);
				m_strStream.Append(strTemp);
				if (pcbWritten)
					*pcbWritten = cb-nCharsToSkip*sizeof(wchar_t);
			}
			else
			{
				// compiling ANSI and got an ANSI buffer
				m_strStream.Append((char*)pv, cb);
				if (pcbWritten)
					*pcbWritten = cb;
			}
#endif
		}
		catch (CMemoryException* e)
		{
			e->Delete();
			return E_OUTOFMEMORY;
		}
		return S_OK;
	}
#endif

	STDMETHOD(Seek)(LARGE_INTEGER , DWORD , _Out_opt_ ULARGE_INTEGER *plibNewPosition)
	{
		if (plibNewPosition)
		{
			plibNewPosition->QuadPart = 0;
		}

		return S_OK;
	}

	STDMETHOD(SetSize)(ULARGE_INTEGER )
	{
		return S_OK;
	}

	STDMETHOD(CopyTo)(_In_ IStream *, ULARGE_INTEGER , _Out_opt_ ULARGE_INTEGER *,
		_Out_opt_ ULARGE_INTEGER *)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(Commit)(DWORD )
	{
		return E_NOTIMPL;
	}

	STDMETHOD(Revert)( void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(LockRegion)(ULARGE_INTEGER , ULARGE_INTEGER , DWORD )
	{
		return E_NOTIMPL;
	}

	STDMETHOD(UnlockRegion)(ULARGE_INTEGER , ULARGE_INTEGER ,
		DWORD )
	{
		return E_NOTIMPL;
	}

	STDMETHOD(Stat)(__RPC__out STATSTG *, DWORD )
	{
		return E_NOTIMPL;
	}

	STDMETHOD(Clone)(__RPC__deref_out_opt IStream **)
	{
		return E_NOTIMPL;
	}

protected:
	CString m_strStream;
	CStringA m_strAnsi;
	UINT m_current_index;

};

/*============================================================================*/
// CHtmlEditCtrlBase

template <class T>
class CHtmlEditCtrlBase 
{
public:
	BOOL SetDesignMode(BOOL bMode) const
	{
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spDoc;
		pT->GetDHtmlDocument(&spDoc);
		if (spDoc)
		{
			if (bMode)
				return S_OK == spDoc->put_designMode(CComBSTR(L"On")) ? TRUE : FALSE;
			else
				return S_OK == spDoc->put_designMode(CComBSTR(L"Off")) ? TRUE : FALSE;
		}
		return FALSE;
	}

	HRESULT ExecCommand(const GUID *pGuid, long cmdID, long cmdExecOpt, VARIANT* pInVar=NULL, VARIANT* pOutVar=NULL) const
	{
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spDoc;
		HRESULT hr = E_FAIL;
		pT->GetDHtmlDocument(&spDoc);
		if (spDoc)
		{
			CComQIPtr<IOleCommandTarget> spCmdTarg = spDoc;
			if (spCmdTarg)
			{
				hr = spCmdTarg->Exec(pGuid, cmdID, cmdExecOpt, pInVar , pOutVar);
			}
			else
				hr = E_NOINTERFACE;
		}
		return hr;
	}
	HRESULT ExecCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar=NULL, VARIANT* pOutVar=NULL) const
	{
		return ExecCommand(&CGID_MSHTML, cmdID, cmdExecOpt, 
					pInVar , pOutVar );
	}

	long QueryStatus(long cmdID) const
	{
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spDoc;
		pT->GetDHtmlDocument(&spDoc);
		if (spDoc)
		{
			CComQIPtr<IOleCommandTarget> spCmdTarg = spDoc;
			if (spCmdTarg)
			{
				OLECMD ocmd = {cmdID, 0};
				if (S_OK == spCmdTarg->QueryStatus(&CGID_MSHTML, 1, &ocmd, NULL))
					return ocmd.cmdf;
			}
		}
		// if an error occurred, returning 0 means the command is disabled
		return 0;
	}

	HRESULT GetEvent(IHTMLEventObj **ppEventObj) const
	{
		::ATL::CComPtr<IHTMLWindow2> sphtmlWnd;
		::ATL::CComPtr<IHTMLDocument2> sphtmlDoc;

		if (!ppEventObj)
			return E_POINTER;

		*ppEventObj = NULL;
		HRESULT hr = E_FAIL;

		if (GetDocument(&sphtmlDoc))
		{
			hr = sphtmlDoc->get_parentWindow(&sphtmlWnd);
			if(hr == S_OK)
			{
				hr = sphtmlWnd->get_event(ppEventObj);
			}
		}
		return hr;
	}

	HRESULT GetEventSrcElement(IHTMLElement **ppSrcElement) const
	{
		::ATL::CComPtr<IHTMLEventObj> spEventObj;
		if (!ppSrcElement)
			return E_POINTER;

		*ppSrcElement = NULL;
		HRESULT hr = GetEvent(&spEventObj);
		if (hr == S_OK)
		{
			hr = spEventObj->get_srcElement(ppSrcElement);
		}
		return hr;
	}

	HRESULT GetDocument(IHTMLDocument2** ppDoc) const
	{	
		const T* pT = static_cast<const T*>(this);
		return pT->GetDHtmlDocument(ppDoc) ? S_OK : E_NOINTERFACE;
	}

	HRESULT NewDocument() const
	{
		HRESULT hr = E_FAIL;
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spHTMLDocument;
		CComQIPtr<IPersistStreamInit> spPSI;
		CStreamOnCString stream;

		pT->GetDHtmlDocument(&spHTMLDocument);
		if (spHTMLDocument)
		{
			spPSI = spHTMLDocument;
			if (spPSI)
			{
				hr = spPSI->InitNew();
			}
		}
		return hr;
	}

	HRESULT GetDocumentHTML(CString& szHTML) const
	{
		HRESULT hr = E_NOINTERFACE;
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spHTMLDocument;
		CComQIPtr<IPersistStreamInit> spPSI;
		CStreamOnCString stream;

		pT->GetDHtmlDocument(&spHTMLDocument);
		if (spHTMLDocument)
		{
			spPSI = spHTMLDocument;
			if (spPSI)
			{
				hr = spPSI->Save(static_cast<IStream*>(&stream), FALSE);
				if (hr == S_OK)
				{
					if (!stream.CopyData(szHTML))
						hr = E_OUTOFMEMORY;
				}
			}
		}
		return hr;
	}

	HRESULT SetDocumentHTML(LPCTSTR szHTML) const
	{
		HRESULT hr = E_NOINTERFACE;
		const T* pT = static_cast<const T*>(this);
		CStreamOnCString stream(szHTML);
		::ATL::CComPtr<IHTMLDocument2> spHTMLDocument;
		CComQIPtr<IPersistStreamInit> spPSI;

		pT->GetDHtmlDocument(&spHTMLDocument);
		if (spHTMLDocument)
		{
			spPSI = spHTMLDocument;
			if (spPSI)
				hr = spPSI->Load(static_cast<IStream*>(&stream));
		}
		return hr;
	}

	HRESULT GetIsDirty() const
	{
		HRESULT hr = E_NOINTERFACE;
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spHTMLDocument;
		CComQIPtr<IPersistStreamInit> spPSI;

		pT->GetDHtmlDocument(&spHTMLDocument);
		if (spHTMLDocument)
		{
			spPSI = spHTMLDocument;
			if (spPSI)
				hr = spPSI->IsDirty();
		}
		return hr;
	}

	HRESULT GetDocumentTitle(CString& szTitle) const
	{
		HRESULT hr = E_FAIL;
		const T* pT = static_cast<const T*>(this);
		::ATL::CComPtr<IHTMLDocument2> spHTMLDocument;
		CComBSTR bstrTitle;
		pT->GetDHtmlDocument(&spHTMLDocument);
		if (!spHTMLDocument)
			hr = E_NOINTERFACE;
		else
		{
			hr = spHTMLDocument->get_title(&bstrTitle);
			if (hr == S_OK)
				szTitle = COLE2CT(bstrTitle);
		}
		return hr;
	}

	HRESULT GetBlockFormatNames(CStringArray &sa) const
	{
		CComVariant vaRet;
		HRESULT hr = E_FAIL;
		long lStatus = QueryStatus(IDM_GETBLOCKFMTS);
		if (lStatus & OLECMDF_ENABLED || lStatus & OLECMDF_LATCHED)
		{
			if (S_OK == ExecCommand(IDM_GETBLOCKFMTS, OLECMDEXECOPT_DODEFAULT, NULL, &vaRet))
			{
				if(vaRet.vt & VT_ARRAY)
				{
					// IHTMLDocument should be returning VT_ARRAY | VT_BSTR in the variant, so that it can
					// be properly cleared.  Assume if VT_ARRAY is set, that VT_BSTR should be set also.
					vaRet.vt |= VT_BSTR;

					SAFEARRAY *psa = vaRet.parray;

					long lBound = 0,uBound = 0;
					if(S_OK == SafeArrayGetLBound(psa,1,&lBound) &&
					   S_OK == SafeArrayGetUBound(psa,1,&uBound) )
					{
						for(long i=lBound; i<=uBound; i++)
						{
							CComBSTR bstrElem;
							if( (S_OK == SafeArrayGetElement(psa, &i, &bstrElem) ))
							{
								sa.Add(CString(bstrElem));
							}
						}
						hr = S_OK;
					}
				}
			}
		}

		if (vaRet.vt == VT_ERROR)
			hr = V_ERROR(&vaRet);

		return hr;
	}

	HRESULT SetForeColor(LPCTSTR szColor) const
	{
		return ExecHelperSetVal(IDM_FORECOLOR, szColor);
	}

	HRESULT SetForeColor(int nColor) const
	{
		return ExecHelperSetVal(IDM_FORECOLOR, nColor, OLECMDF_SUPPORTED|OLECMDF_ENABLED);
	}

	HRESULT GetForeColor(int &nColor)
	{
		return ExecHelperGetVal(IDM_FORECOLOR, nColor, OLECMDF_ENABLED|OLECMDF_SUPPORTED);
	}

	HRESULT GetBackColor(int& nColor) const
	{
		return ExecHelperGetVal(IDM_BACKCOLOR, nColor, OLECMDF_ENABLED|OLECMDF_SUPPORTED);
	}

	HRESULT SetBackColor(LPCTSTR szColor) const
	{
		return ExecHelperSetVal(IDM_BACKCOLOR, szColor, OLECMDF_ENABLED|OLECMDF_SUPPORTED);
	}

	HRESULT SetBackColor(int nColor) const
	{
		return ExecHelperSetVal(IDM_BACKCOLOR, nColor, OLECMDF_ENABLED|OLECMDF_SUPPORTED);
	}

	HRESULT SetDefaultComposeSettings(LPCTSTR szFontName=NULL,
									  unsigned short nFontSize=3,
									  COLORREF crFontColor=0xFF000000,
									  COLORREF crFontBgColor=0xFF000000,
									  bool bBold = false,
									  bool bItalic = false,
									  bool bUnderline = false) const
	{

		HRESULT hr = ExecHelperSetVal(IDM_HTMLEDITMODE, true);
		if (FAILED(hr))
			return hr;

		CString strBuffer;
		TCHAR szFontColor[12];
		TCHAR szBgColor[12];
		if(crFontColor == 0xFF000000)
			szFontColor[0] = 0;
		else
			if (0 > _sntprintf(szFontColor, 12, _T("%d.%d.%d"), GetRValue(crFontColor),
					GetGValue(crFontColor), GetBValue(crFontColor)))
					return E_UNEXPECTED;

		if (crFontBgColor == 0xFF000000)
			szBgColor[0] = 0;
		else
			if (0 > _sntprintf(szBgColor, 12, _T("%d.%d.%d"), GetRValue(crFontBgColor),
					GetGValue(crFontBgColor), GetBValue(crFontBgColor)))
					return E_UNEXPECTED;

		_ATLTRY
		{
		strBuffer.Format(_T("%d,%d,%d,%d,%Ts,%Ts,%Ts"),
						bBold ? 1 : 0,
						bItalic ? 1 : 0,
						bUnderline ? 1 : 0,
						nFontSize,
						szFontColor,
						szBgColor,
						szFontName);
		}
		_ATLCATCHALL()
		{
			return E_UNEXPECTED;
		}
		return ExecHelperSetVal(IDM_COMPOSESETTINGS, (LPCTSTR)strBuffer);
	}

	HRESULT GetBlockFormat(CString& strFormat) const
	{
		return ExecHelperGetVal(IDM_BLOCKFMT, strFormat, OLECMDF_SUPPORTED|OLECMDF_ENABLED);
	}

	HRESULT SetBlockFormat(LPCTSTR szFormat) const
	{
		return ExecHelperSetVal(IDM_BLOCKFMT, szFormat);
	}

	HRESULT GetFontFace(CString& strFace) const
	{
		return ExecHelperGetVal(IDM_FONTNAME, strFace, OLECMDF_ENABLED|OLECMDF_SUPPORTED);
	}

	HRESULT SetFontFace(LPCTSTR szFace) const
	{
		return ExecHelperSetVal(IDM_FONTNAME, szFace);
	}

	HRESULT IE50Paste(LPCTSTR szData) const
	{
		return ExecHelperSetVal(IDM_IE50_PASTE, szData);
	}

	HRESULT GetBookMark(CString& strAnchor) const
	{
		return ExecHelperGetVal(IDM_BOOKMARK, strAnchor);
	}

	HRESULT SetBookMark(LPCTSTR szAnchorName) const
	{
		return ExecHelperSetVal(IDM_BOOKMARK, szAnchorName,OLECMDF_SUPPORTED);
	}

	HRESULT SetOverwriteMode(bool bMode) const
	{
		return ExecHelperSetVal(IDM_OVERWRITE, bMode);
	}

	HRESULT Is1DElement(bool& bValue) const
	{
		return ExecHelperGetVal(IDM_1D_ELEMENT, bValue);
	}

	HRESULT Is2DElement(bool& bValue) const
	{
		return ExecHelperGetVal(IDM_2D_ELEMENT, bValue);
	}

	HRESULT GetFontSize(short& nSize) const
	{
		return ExecHelperGetVal(IDM_FONTSIZE, nSize);
	}

	HRESULT SetFontSize(unsigned short size) const
	{
		return ExecHelperSetVal(IDM_FONTSIZE, (short)size);
	}

	HRESULT GetFrameZone(short& nZone) const
	{
		return ExecHelperGetVal(IDM_GETFRAMEZONE, nZone);
	}

	HRESULT SetCSSEditingLevel(short nLevel) const
	{
		return ExecHelperSetVal(IDM_CSSEDITING_LEVEL, nLevel, 0);
	}

	HRESULT HyperLink(LPCTSTR szUrl = NULL) const
	{
		if (szUrl)
			return ExecHelperSetVal(IDM_HYPERLINK, szUrl, OLECMDF_SUPPORTED,
									OLECMDEXECOPT_DONTPROMPTUSER);
		return ExecHelperNN(IDM_HYPERLINK, OLECMDF_SUPPORTED, OLECMDEXECOPT_DODEFAULT);
	}

	HRESULT Image(LPCTSTR szUrl = NULL) const
	{
		if (szUrl)
			return ExecHelperSetVal(IDM_IMAGE, szUrl, OLECMDF_SUPPORTED,
									OLECMDEXECOPT_DONTPROMPTUSER);
		return ExecHelperNN(IDM_IMAGE, OLECMDF_SUPPORTED, OLECMDEXECOPT_DODEFAULT);
	}

	HRESULT OrderList(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_ORDERLIST, szId);
	}

	HRESULT UnorderList(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_UNORDERLIST, szId);
	}

	HRESULT AddToGlyphTable(LPCTSTR szTag,
							LPCTSTR szImgUrl,
							unsigned short nTagType,
							unsigned short nAlignment,
							unsigned short nPosInfo,
							unsigned short nDirection,
							unsigned int nImgWidth,
							unsigned int nImgHeight) const
	{
		// Check Paramters
		if (!szTag ||
			!szImgUrl ||
			nTagType > 2 ||
			nAlignment > 3 ||
			nPosInfo > 3 ||
			nDirection > 4)
			return E_INVALIDARG;

		CString strGlyphEntry;
		_ATLTRY
		{
			strGlyphEntry.Format(_T("%%%%%Ts^^%%%%%Ts^^%%%%%d^^%%%%%d^^%%%%%d^^")
								 _T("%%%%%d^^%%%%%d^^%%%%%d^^%%%%%d^^%%%%%d^^**"),
								 szTag, szImgUrl, nTagType, nAlignment, nPosInfo,
								 nDirection, nImgWidth, nImgHeight, nImgWidth, nImgHeight);
		}
		_ATLCATCHALL()
		{
			return E_UNEXPECTED;
		}
		CComVariant vIn((LPCTSTR)strGlyphEntry);
		return ExecCommand(IDM_ADDTOGLYPHTABLE, OLECMDEXECOPT_DODEFAULT, &vIn);
	}

	HRESULT EmptyGlyphTable() const
	{
		return ExecHelperNN(IDM_EMPTYGLYPHTABLE);
	}

	HRESULT Button(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_BUTTON, szId);
	}

	HRESULT CheckBox(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_CHECKBOX, szId);
	}

	HRESULT DropDownBox(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_DROPDOWNBOX, szId);
	}

	HRESULT HorizontalLine(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_HORIZONTALLINE, szId);
	}

	HRESULT Iframe(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_IFRAME, szId);
	}

	HRESULT InsFieldSet(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSFIELDSET, szId);
	}

	HRESULT InsInputButton(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTBUTTON, szId);
	}

	HRESULT InsInputHidden(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTHIDDEN, szId);
	}

	HRESULT InsInputImage(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTIMAGE, szId);
	}

	HRESULT InsInputPassword(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTPASSWORD, szId);
	}

	HRESULT InsInputReset(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTRESET, szId);
	}

	HRESULT InsInputSubmit(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTSUBMIT, szId);
	}

	HRESULT InsInputUpload(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_INSINPUTUPLOAD, szId);
	}

	HRESULT ListBox(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_LISTBOX, szId);
	}

	HRESULT Marquee(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_MARQUEE, szId);
	}

	HRESULT Paragraph(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_PARAGRAPH, szId);
	}

	HRESULT RadioButton(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_RADIOBUTTON, szId);
	}

	HRESULT SaveAs(LPCTSTR szPath = NULL) const
	{
		if (szPath)
		{
			return ExecHelperSetVal(IDM_SAVEAS, szPath, OLECMDF_SUPPORTED,
									OLECMDEXECOPT_DONTPROMPTUSER);
		}
		else
		{
			return ExecHelperNN(IDM_SAVEAS, OLECMDF_SUPPORTED, OLECMDEXECOPT_DODEFAULT);
		}
	}

	HRESULT TextArea(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_TEXTAREA, szId);
	}

	HRESULT TextBox(LPCTSTR szId = NULL) const
	{
		return ExecHelperSetVal(IDM_TEXTBOX, szId);
	}

	HRESULT GetAbsolutePosition(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_ABSOLUTE_POSITION) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetAbsolutePosition(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_ABSOLUTE_POSITION, bNewValue );
	}


	HRESULT Set2DPosition(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_2D_POSITION, bNewValue, 0);
	}

	HRESULT SetAtomicSelection(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_ATOMICSELECTION, bNewValue, 0);
	}

	HRESULT SetAutoURLDetectMode(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_AUTOURLDETECT_MODE, bNewValue, 0);
	}

	HRESULT SetDisableEditFocusUI(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_DISABLE_EDITFOCUS_UI, bNewValue, 0);
	}

	HRESULT SetIE5PasteMode(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_IE50_PASTE_MODE, bNewValue, 0);
	}

	HRESULT SetLiveResize(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_LIVERESIZE, bNewValue, 0);
	}

	HRESULT SetMultiSelect(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_MULTIPLESELECTION, bNewValue, 0);
	}

	HRESULT SetOverrideCursor(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_OVERRIDE_CURSOR, bNewValue, 0);
	}

	HRESULT SetRespectVisInDesign(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_RESPECTVISIBILITY_INDESIGN, bNewValue, 0);
	}

	HRESULT GetShowAlignedSiteTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWALIGNEDSITETAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowAlignedSiteTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWALIGNEDSITETAGS, bNewValue);
	}

	HRESULT GetShowAllTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWALLTAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowAllTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWALLTAGS, bNewValue);
	}

	HRESULT GetShowAreaTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWAREATAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowAreaTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWAREATAGS, bNewValue);
	}

	HRESULT GetShowCommentTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWCOMMENTTAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowCommentTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWCOMMENTTAGS, bNewValue);
	}

	HRESULT GetShowMiscTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWMISCTAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowMiscTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWMISCTAGS, bNewValue);
	}

	HRESULT GetShowScriptTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWSCRIPTTAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowScriptTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWSCRIPTTAGS, bNewValue);
	}

	HRESULT GetShowStyleTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWSTYLETAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowStyleTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWSTYLETAGS, bNewValue);
	}

	HRESULT GetShowUnknownTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWUNKNOWNTAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowUnknownTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWUNKNOWNTAGS, bNewValue);
	}

	HRESULT GetShowBRTags(bool &bCurValue) const
	{
		bCurValue = (QueryStatus(IDM_SHOWWBRTAGS) & OLECMDF_LATCHED) != 0;
		return S_OK;
	}

	HRESULT SetShowBRTags(bool bNewValue) const
	{
		return ExecHelperSetVal(IDM_SHOWWBRTAGS, bNewValue);
	}

	HRESULT PrintDocument() const
	{
		return ExecHelperNN(IDM_PRINT);
	}

	HRESULT PrintDocument(LPCTSTR szPrintTemplate) const
	{
		return ExecHelperSetVal(IDM_PRINT, szPrintTemplate);
	}

	HRESULT PrintDocument(bool bShowPrintDialog) const
	{
		return ExecHelperSetVal(IDM_PRINT, (short)(bShowPrintDialog ? 1 : 0));
	}

	HRESULT PrintPreview() const
	{
		return ExecHelperNN(IDM_PRINTPREVIEW);
	}

	HRESULT PrintPreview(LPCTSTR szPrintTemplate) const
	{
		return ExecHelperSetVal(IDM_PRINTPREVIEW, szPrintTemplate);
	}

	HRESULT Bold() const
	{
		return ExecHelperNN(IDM_BOLD);
	}

	HRESULT Copy() const
	{
		return ExecHelperNN(IDM_COPY);
	}

	HRESULT Cut() const
	{
		return ExecHelperNN(IDM_CUT);
	}

	HRESULT Delete() const
	{
		return ExecHelperNN(IDM_DELETE);
	}

	HRESULT Indent() const
	{
		return ExecHelperNN(IDM_INDENT);
	}

	HRESULT Italic() const
	{
		return ExecHelperNN(IDM_ITALIC);
	}

	HRESULT JustifyCenter() const
	{
		return ExecHelperNN(IDM_JUSTIFYCENTER);
	}

	HRESULT JustifyLeft() const
	{
		return ExecHelperNN(IDM_JUSTIFYLEFT);
	}

	HRESULT JustifyRight() const
	{
		return ExecHelperNN(IDM_JUSTIFYRIGHT);
	}

	HRESULT Outdent() const
	{
		return ExecHelperNN(IDM_OUTDENT);
	}

	HRESULT Paste() const
	{
		return ExecHelperNN(IDM_PASTE);
	}

	HRESULT RemoveFormat() const
	{
		return ExecHelperNN(IDM_REMOVEFORMAT);
	}

	HRESULT SelectAll() const
	{
		return ExecHelperNN(IDM_SELECTALL);
	}

	HRESULT Underline() const
	{
		return ExecHelperNN(IDM_UNDERLINE);
	}

	HRESULT Unlink() const
	{
		return ExecHelperNN(IDM_UNLINK);
	}

	HRESULT ClearSelection() const
	{
		return ExecHelperNN(IDM_CLEARSELECTION);
	}

	HRESULT Font() const
	{
		return ExecHelperNN(IDM_FONT);
	}

	HRESULT RefreshDocument() const
	{
		return ExecHelperNN(IDM_REFRESH, OLECMDF_SUPPORTED);
	}

	HRESULT UnBookmark() const
	{
		return ExecHelperNN(IDM_UNBOOKMARK);
	}

// Implementation
	HRESULT ExecHelperNN(UINT nID,
						 long nMinSupportLevel = OLECMDF_SUPPORTED|OLECMDF_ENABLED,
						 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		long lStatus = QueryStatus(nID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
			hr = ExecCommand(nID, nExecOpt);
		return hr;
	}

	HRESULT ExecHelperSetVal(UINT nCmdID, 
							 LPCTSTR szID=NULL,
							 long nMinSupportLevel = OLECMDF_SUPPORTED|OLECMDF_ENABLED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		long lStatus = QueryStatus(nCmdID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			if (szID)
			{
				CComVariant vName(szID);
				return ExecCommand(nCmdID, nExecOpt, &vName);
			}
			else
				return ExecCommand(nCmdID, nExecOpt);
		}
		return E_FAIL;
	}

	HRESULT ExecHelperSetVal(UINT nCmdID,
							 bool bValue,
							 long nMinSupportLevel = OLECMDF_SUPPORTED|OLECMDF_ENABLED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		long lStatus = QueryStatus(nCmdID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			CComVariant vIn(bValue);
			hr = ExecCommand(nCmdID, nExecOpt, &vIn);
		}
		return hr;
	}

	HRESULT ExecHelperSetVal(UINT nCmdID,
							 short nNewVal,
							 long nMinSupportLevel = OLECMDF_SUPPORTED|OLECMDF_ENABLED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		long lStatus = QueryStatus(nCmdID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			CComVariant vIn((short)nNewVal);
			hr = ExecCommand(nCmdID, nExecOpt, &vIn);
		}
		return hr;
	}

	HRESULT ExecHelperSetVal(UINT nCmdID,
							 int nNewVal,
							 long nMinSupportLevel = OLECMDF_SUPPORTED|OLECMDF_ENABLED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		long lStatus = QueryStatus(nCmdID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			CComVariant vIn((int)nNewVal);
			hr = ExecCommand(nCmdID, nExecOpt, &vIn);
		}
		return hr;
	}

	HRESULT ExecHelperGetVal(UINT nCmdID,
							 bool &bValue,
							 long nMinSupportLevel = OLECMDF_SUPPORTED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		bValue = false;
		long lStatus = QueryStatus(nCmdID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			CComVariant vOut;
			hr = ExecCommand(nCmdID, nExecOpt, NULL, &vOut);
			if (hr == S_OK)
			{
				switch(vOut.vt)
				{
				case VT_BOOL:
					bValue = vOut.boolVal == VARIANT_TRUE ? true : false;
					break;
				case VT_NULL:
				case VT_EMPTY:
					break;
				case VT_ERROR:
					hr = V_ERROR(&vOut);
					break;
				default:
					hr = E_UNEXPECTED;
					break;
				}
			}
		}
		return hr;
	}

	HRESULT ExecHelperGetVal(UINT nCmdID,
							 short &nValue,
							 long nMinSupportLevel = OLECMDF_SUPPORTED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		int nTempVal;
		HRESULT hr = ExecHelperGetVal(nCmdID, nTempVal, nMinSupportLevel, nExecOpt);
		if (hr == S_OK)
			nValue = (short)nTempVal;
		return hr;
	}


	HRESULT ExecHelperGetVal(UINT nCmdID,
							 int &nValue,
							 long nMinSupportLevel = OLECMDF_SUPPORTED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		long lStatus = QueryStatus(nCmdID);
		nValue = 0;
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			CComVariant vOut;
			hr = ExecCommand(nCmdID, nExecOpt, NULL, &vOut);
			if (hr == S_OK)
			{
				switch (vOut.vt)
				{
				case VT_I4:
					nValue = vOut.lVal;
					break;
				case VT_I2:
					nValue = vOut.iVal;
					break;
				case VT_EMPTY:
				case VT_NULL:
					break;
				case VT_ERROR:
					hr = V_ERROR(&vOut);
					break;
				default:
					hr = E_UNEXPECTED;
					break;
				}
			}
		}
		return hr;
	}

	HRESULT ExecHelperGetVal(UINT nCmdID,
							 CString& strValue,
							 long nMinSupportLevel = OLECMDF_SUPPORTED,
							 long nExecOpt = OLECMDEXECOPT_DODEFAULT) const
	{
		HRESULT hr = E_FAIL;
		CComVariant vaRet;
		strValue.Empty();
		long lStatus = QueryStatus(nCmdID);
		if ((lStatus & nMinSupportLevel) == nMinSupportLevel)
		{
			hr = ExecCommand(nCmdID, nExecOpt, NULL, &vaRet);
			if (hr == S_OK)
			{
				switch(vaRet.vt)
				{
				case VT_BSTR:
					{
					CW2T szVal(vaRet.bstrVal);
					strValue = szVal;
					}
					break;
				case VT_NULL:
				case VT_EMPTY:
					break;
				case VT_ERROR:
					hr = V_ERROR(&vaRet);
					break;
				default:
					hr = E_UNEXPECTED;
					break;
				}
			}
		}
		return hr;
	}
}; //CHtmlEditCtrlBase

/*============================================================================*/
// CHtmlEditCtrl
class CHtmlEditCtrl:
	public CWnd,
	public CHtmlEditCtrlBase<CHtmlEditCtrl>
{
public:
	// Construct
	CHtmlEditCtrl();

	// Create a new instance of the control. This will also call the 
	// contained WebBrowser control's Navigate method to load a default
	// document into the WebBrowser control.
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID, CCreateContext *pContext=NULL);

	// Retrieves the URL to a default document to load.
	virtual LPCTSTR GetStartDocument();

	// Retrieves the IHTMLDocument2 interface on the contained WebBrowser control's
	// currently loaded document.
	BOOL GetDHtmlDocument(IHTMLDocument2 **ppDocument) const;

// Implementation
public:
	// Destruct
	virtual ~CHtmlEditCtrl();

protected:
	DECLARE_EVENTSINK_MAP()
	void _OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL);

};


/*============================================================================*/
// CHtmlEditView
#define AFX_INVALID_DHTML_CMD_ID 0xFFFFFFFF
#define AFX_UI_ELEMTYPE_NORMAL	0
#define AFX_UI_ELEMTYPE_CHECBOX	1
#define AFX_UI_ELEMTYPE_RADIO	2


#define DECLARE_DHTMLEDITING_CMDMAP(className) \
public: \
	UINT GetDHtmlCommandMapping(UINT nIDWindowsCommand, BOOL&, UINT&); \
	BOOL ExecHandler(UINT nCmdID); \
	typedef void  (className##::*PFN_##className##_CMDHDLR)(UINT); \
	struct className##dhtml_cmd_mapping \
	{ \
		UINT windowsCommand; \
		UINT dhtmlCommand; \
		UINT nElementType; \
		PFN_##className##_CMDHDLR pfn; \
	}; \
	static className##dhtml_cmd_mapping m_commands[];\

#define DHTMLEDITING_CMD_ENTRY(cmdID, dhtmlcmdID)\
{cmdID, dhtmlcmdID, AFX_UI_ELEMTYPE_NORMAL, NULL}, 

#define DHTMLEDITING_CMD_ENTRY_FUNC(cmdID, dhtmlcmdID, member_func_name)\
{cmdID, dhtmlcmdID, AFX_UI_ELEMTYPE_NORMAL, member_func_name},

#define DHTMLEDITING_CMD_ENTRY_TYPE(cmdID, dhtmlcmdID, elemType)\
{cmdID, dhtmlcmdID, elemType, NULL}, 

#define DHTMLEDITING_CMD_ENTRY_FUNC_TYPE(cmdID, dhtmlcmdID, member_func_name, elemType)\
{cmdID, dhtmlcmdID, elemType, member_func_name},

#define BEGIN_DHTMLEDITING_CMDMAP(className) \
PTM_WARNING_DISABLE \
BOOL className##::ExecHandler(UINT nWindowsCommand)\
{\
	const className##dhtml_cmd_mapping *pMapping = className##::m_commands;\
	while (pMapping->dhtmlCommand != 0)\
	 {\
	 if (nWindowsCommand == pMapping->windowsCommand)\
	 {\
		(this->*pMapping->pfn)(pMapping->dhtmlCommand);\
		return TRUE;\
	 }\
	 pMapping++;\
	 }\
	return FALSE;\
}\
UINT className##::GetDHtmlCommandMapping(UINT nIDWindowsCommand, BOOL& bHasExecFunc, UINT& uiElemType)\
{ \
	const className##dhtml_cmd_mapping *pMapping = className##::m_commands;\
	 while (pMapping->dhtmlCommand != 0)\
	 {\
	 if (pMapping->windowsCommand == nIDWindowsCommand)\
	 {\
		 bHasExecFunc = pMapping->pfn != NULL ? TRUE : FALSE;\
		 uiElemType = pMapping->nElementType;\
		 return pMapping->dhtmlCommand;\
	 }\
	 pMapping++;\
	 }\
	return AFX_INVALID_DHTML_CMD_ID;\
}\
className##::className##dhtml_cmd_mapping className##::m_commands[] = {

#define END_DHTMLEDITING_CMDMAP()\
		{0, 0, 0, NULL} \
	}; \
	PTM_WARNING_RESTORE

class CHtmlEditView :
	public CHtmlView,
	public CHtmlEditCtrlBase<CHtmlEditView>
{
public:
	CHtmlEditView();

	// Retrieve the IHTMLDocument2 interface associated with the
	// document loaded in the contained WebBrowser control.
	BOOL GetDHtmlDocument(IHTMLDocument2 **ppDocument) const;

	// Create a new window object. This function will also call the
	// contained WebBrowser's Navigate method to load a default document.
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);

	// Retrieve the name of the default document for this view.
	virtual LPCTSTR GetStartDocument();

// Implementation
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual UINT GetDHtmlCommandMapping(UINT nIDWindowsCommand, BOOL& bHasExecFunc, UINT& uiElemType);
	virtual BOOL ExecHandler(UINT nCmdID);
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	void OnPaint();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CHtmlEditView)
	virtual ~CHtmlEditView();
};

class AFX_NOVTABLE CHtmlEditDoc : public CDocument
{
public:
	// Construction
	CHtmlEditDoc();

	// Get the CHtmlEditView associated with this document
	virtual CHtmlEditView* GetView() const;

	// Open a URL into the view associated with this document
	virtual BOOL OpenURL(LPCTSTR lpszURL);

	// See if the associated view's WebBrowser control contains a 
	// document that has been modified by the user.
	virtual BOOL IsModified();

// Implementation
public:
	virtual ~CHtmlEditDoc();
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszFileName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszFileName);
	virtual BOOL OnNewDocument();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	DECLARE_DYNCREATE(CHtmlEditDoc)
};

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#pragma warning(pop)

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXHTML_INLINE AFX_INLINE
#include <afxhtml.inl>
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFXHTML_H__

