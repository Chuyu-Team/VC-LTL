// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLXML_H__
#define __ATLXML_H__

#pragma once

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#include <MsXml6.h>

#pragma warning (push)
#pragma pack(push,_ATL_PACKING)

#define IF_NOT_VALID_RETURN_EPTR if (!IsValid()) { return E_POINTER; }
#define HR_SUCCEEDED_OK(hr) (SUCCEEDED(hr) && hr == S_OK)
#define IF_HR_INVALID_RETURN_HR(hr) if (!HR_SUCCEEDED_OK(hr)) { return hr; }
#define IF_HR_INVALID_RETURN_BOOL(hr) if (!SUCCEEDED(hr) || hr == S_FALSE) { return FALSE; }
#define IF_NULL_RETURN_INVALID(ptr) if (ptr == NULL) { return E_INVALIDARG; }

namespace ATL
{

class CXMLDocument;

template<class T>
class CXMLNode
{
public:
	CXMLNode()
	{
	}
	CXMLNode(_In_ T* node) : m_pNode(node)
	{
	}
	CXMLNode(_In_ CXMLNode<T>& node) : m_pNode(node)
	{
	}
	virtual ~CXMLNode()
	{
	}

	inline BOOL IsValid() const
	{
		return m_pNode != NULL;
	}

	HRESULT GetNodeType(_Out_ DOMNodeType& val)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_nodeType(&val);
	}

	HRESULT GetNodeName(_Inout_ CString& strName)
	{
		IF_NOT_VALID_RETURN_EPTR;

		BSTR bstr = NULL;
		HRESULT hr = Get()->get_nodeName(&bstr);
		if (HR_SUCCEEDED_OK(hr))
		{
			strName = CStringW(bstr);
		}

		if (bstr != NULL)
		{
			::SysFreeString(bstr);
		}

		return hr;
	}

	HRESULT GetXML(_Inout_ CString& strXML)
	{
		IF_NOT_VALID_RETURN_EPTR;

		BSTR bstr = NULL;
		HRESULT hr = Get()->get_xml(&bstr);
		if (HR_SUCCEEDED_OK(hr))
		{
			strXML = CStringW(bstr);
		}

		if (bstr != NULL)
		{
			::SysFreeString(bstr);
		}

		return hr;
	}

	HRESULT GetName(_Inout_ CString& strName)
	{
		IF_NOT_VALID_RETURN_EPTR;

		BSTR bstr = NULL;
		HRESULT hr = Get()->get_baseName(&bstr);
		if (HR_SUCCEEDED_OK(hr))
		{
			strName = CStringW(bstr);
		}

		if (bstr != NULL)
		{
			::SysFreeString(bstr);
		}

		return hr;
	}

	HRESULT GetText(_Inout_ CString& strText)
	{
		IF_NOT_VALID_RETURN_EPTR;

		BSTR bstr = NULL;
		HRESULT hr = Get()->get_text(&bstr);
		if (HR_SUCCEEDED_OK(hr))
		{
			strText = CStringW(bstr);
		}

		if (bstr != NULL)
		{
			::SysFreeString(bstr);
		}

		return hr;
	}

	HRESULT GetOwnerDocument(_Inout_ CXMLDocument& document)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_ownerDocument(document);
	}

	HRESULT GetParentNode(_Inout_ CXMLNode<IXMLDOMNode>& node)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_parentNode(node);
	}

	HRESULT AppendChild(_In_ IXMLDOMNode* child)
	{
		IF_NOT_VALID_RETURN_EPTR;
		IF_NULL_RETURN_INVALID(child);

		CXMLNode<IXMLDOMNode> node;
		HRESULT hr = Get()->appendChild(child, node);

		return hr;
	}

	HRESULT SelectNode(
		_In_z_ LPCTSTR pszSelect,
		_Inout_ CXMLNode<IXMLDOMNode>& node)
	{
		IF_NOT_VALID_RETURN_EPTR;
		IF_NULL_RETURN_INVALID(pszSelect);

		CString strSelect = pszSelect;
		BSTR bstrSelect = strSelect.AllocSysString();
		HRESULT hr = Get()->selectSingleNode(bstrSelect, node);
		::SysFreeString(bstrSelect);

		return hr;
	}

	HRESULT SelectNodes(
		_In_z_ LPCTSTR pszSelect,
		_Outptr_result_maybenull_ IXMLDOMNodeList** nodes)
	{
		IF_NOT_VALID_RETURN_EPTR;
		IF_NULL_RETURN_INVALID(pszSelect);
		IF_NULL_RETURN_INVALID(nodes);

		CString strSelect = pszSelect;
		BSTR bstrSelect = strSelect.AllocSysString();
		HRESULT hr = Get()->selectNodes(bstrSelect, nodes);
		::SysFreeString(bstrSelect);

		return hr;
	}

	HRESULT HasChildren(_Out_ BOOL& bHasChildren)
	{
		IF_NOT_VALID_RETURN_EPTR;

		bHasChildren = FALSE;

		VARIANT_BOOL var = VARIANT_FALSE;
		HRESULT hr = Get()->hasChildNodes(&var);
		if (HR_SUCCEEDED_OK(hr))
		{
			bHasChildren = (BOOL)(var == VARIANT_TRUE);
		}

		return hr;
	}

	HRESULT GetFirstChild(_Inout_ CXMLNode<IXMLDOMNode>& child)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_firstChild(child);
	}

	HRESULT GetLastChild(_Inout_ CXMLNode<IXMLDOMNode>& child)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_lastChild(child);
	}

	HRESULT GetPrevSibling(_Inout_ CXMLNode<IXMLDOMNode>& sibling)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_previousSibling(sibling);
	}

	HRESULT GetNextSibling(_Inout_ CXMLNode<IXMLDOMNode>& sibling)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_nextSibling(sibling);
	}

	inline operator T* ()
	{
		return (T*)m_pNode;
	}
	inline operator T** ()
	{
		return &m_pNode;
	}
	inline CComQIPtr<T>& Get()
	{
		return m_pNode;
	}

private:
	CComQIPtr<T> m_pNode;
};

class CXMLDocument :
	public CXMLNode<IXMLDOMDocument>
{
public:
	CXMLDocument()
	{
		EmptyErrorInfo();
		m_bComInitialized = FALSE;
	}
	virtual ~CXMLDocument()
	{
		if (m_bComInitialized)
		{
			CoUninitialize();
		}
	}

	HRESULT Create(
		_In_z_ LPCTSTR pszVersion,
		_In_z_ LPCTSTR pszEncoding,
		_In_ BOOL bStandalone,
		_In_z_ LPCTSTR pszRoot)
	{
		EmptyErrorInfo();
		IF_NULL_RETURN_INVALID(pszVersion);

		HRESULT hr = Initialize();
		if (!SUCCEEDED(hr))
		{
			return hr;
		}

		CXMLNode<IXMLDOMProcessingInstruction> proc;

		CString strData(_T("version=\""));
		CString strVersion = pszVersion;
		strData += strVersion + _T("\"");

		if (pszEncoding != NULL)
		{
			CString strEncoding = pszEncoding;
			CString strStandalone = bStandalone ? _T("\"yes\"") : _T("\"no\"");
			strData += _T(" encoding=\"") + strEncoding + _T("\" standalone=" + strStandalone);
		}

		BSTR bstr1 = T2BSTR(_T("xml"));
		BSTR bstr2 = strData.AllocSysString();

		hr = Get()->createProcessingInstruction(bstr1, bstr2, proc);

		::SysFreeString(bstr1);
		::SysFreeString(bstr2);

		IF_HR_INVALID_RETURN_HR(hr);

		hr = AppendChild(proc);
		IF_HR_INVALID_RETURN_HR(hr);

		if (pszRoot != NULL)
		{
			CXMLNode<IXMLDOMElement> root;
			hr = CreateElement(pszRoot, root);
			IF_HR_INVALID_RETURN_HR(hr);

			hr = AppendChild(root);
			IF_HR_INVALID_RETURN_HR(hr);
		}

		return hr;
	}

	HRESULT Load(_In_z_ LPCTSTR pszFile)
	{
		EmptyErrorInfo();
		IF_NULL_RETURN_INVALID(pszFile);

		HRESULT hr = Initialize();
		IF_HR_INVALID_RETURN_HR(hr);
		IF_NOT_VALID_RETURN_EPTR;

		CString strFile = pszFile;
		VARIANT_BOOL varBool = VARIANT_FALSE;
		VARIANT var;
		V_VT(&var) = VT_BSTR;
		V_BSTR(&var) = strFile.AllocSysString();

		hr = Get()->load(var, &varBool);

		::VariantClear(&var);

		if (HR_SUCCEEDED_OK(hr) && varBool == VARIANT_FALSE)
		{
			hr = S_FALSE;
		}

		if (!HR_SUCCEEDED_OK(hr))
		{
			UpdateErrorInfo();

			return hr;
		}

		return PostLoad();
	}

	HRESULT Load(_Inout_ IStream* pStream)
	{
		EmptyErrorInfo();
		IF_NULL_RETURN_INVALID(pStream);

		HRESULT hr = Initialize();
		IF_HR_INVALID_RETURN_HR(hr);
		IF_NOT_VALID_RETURN_EPTR;

		VARIANT_BOOL varBool = VARIANT_FALSE;
		VARIANT var;
		V_VT(&var) = VT_UNKNOWN;
		V_UNKNOWN(&var) = pStream;
		V_UNKNOWN(&var)->AddRef();

		hr = Get()->load(var, &varBool);

		::VariantClear(&var);

		if (HR_SUCCEEDED_OK(hr) && varBool == VARIANT_FALSE)
		{
			hr = S_FALSE;
		}

		if (!HR_SUCCEEDED_OK(hr))
		{
			UpdateErrorInfo();

			return hr;
		}

		return PostLoad();
	}

	HRESULT Load(
		_In_reads_bytes_(size) LPBYTE lpBuffer,
		_In_ UINT size)
	{
		EmptyErrorInfo();
		IF_NULL_RETURN_INVALID(lpBuffer);
		IF_NULL_RETURN_INVALID(size);

		HRESULT hr = Initialize();
		IF_HR_INVALID_RETURN_HR(hr);
		IF_NOT_VALID_RETURN_EPTR;

		hr = E_FAIL;

		HGLOBAL hGlobal = ::GlobalAlloc(GHND, size);
		if (hGlobal != NULL)
		{
			LPVOID lpVoid = ::GlobalLock(hGlobal);

			if (lpVoid != NULL)
			{
ATLPREFAST_SUPPRESS(6386)
				memcpy(lpVoid, lpBuffer, size);
ATLPREFAST_UNSUPPRESS()

				::GlobalUnlock(hGlobal);

				IStream* pStream = NULL;
				if (SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream)))
				{
					hr = Load(pStream);
				}

ATLPREFAST_SUPPRESS(6102)
				if (pStream != NULL)
				{
					pStream->Release();
				}
ATLPREFAST_UNSUPPRESS()
            }

			::GlobalFree(hGlobal);
		}

		return hr;
	}

	HRESULT LoadXML(_In_z_ LPCTSTR pszXML)
	{
		EmptyErrorInfo();
		IF_NULL_RETURN_INVALID(pszXML);

		HRESULT hr = Initialize();
		IF_HR_INVALID_RETURN_HR(hr);
		IF_NOT_VALID_RETURN_EPTR;

		VARIANT_BOOL varBool = VARIANT_FALSE;
		CString strXML = pszXML;
		BSTR bstr = strXML.AllocSysString();
		hr = Get()->loadXML(bstr, &varBool);
		::SysFreeString(bstr);

		if (HR_SUCCEEDED_OK(hr) && varBool == VARIANT_FALSE)
		{
			hr = S_FALSE;
		}

		if (!HR_SUCCEEDED_OK(hr))
		{
			UpdateErrorInfo();

			return hr;
		}

		return PostLoad();
	}

	HRESULT Save(_In_z_ LPCTSTR pszFile)
	{
		IF_NOT_VALID_RETURN_EPTR;
		IF_NULL_RETURN_INVALID(pszFile);

		CString strFile = pszFile;
		VARIANT var;
		V_VT(&var) = VT_BSTR;
		V_BSTR(&var) = strFile.AllocSysString();

		HRESULT hr = Get()->save(var);

		::VariantClear(&var);

		return hr;
	}

	HRESULT Save(_Inout_ IStream* pStream)
	{
		IF_NOT_VALID_RETURN_EPTR;
		IF_NULL_RETURN_INVALID(pStream);

		VARIANT var;
		V_VT(&var) = VT_UNKNOWN;
		V_UNKNOWN(&var) = pStream;
		V_UNKNOWN(&var)->AddRef();

		HRESULT hr = Get()->save(var);

		::VariantClear(&var);

		return hr;
	}

	HRESULT Save(
		_Outptr_result_buffer_maybenull_(size) LPBYTE* lpBuffer,
		_Out_ UINT& size)
	{
		IF_NOT_VALID_RETURN_EPTR;
		IF_NULL_RETURN_INVALID(lpBuffer);

		size = 0;
		*lpBuffer = NULL;

		HRESULT hr = E_FAIL;

		HGLOBAL hGlobal = ::GlobalAlloc(GHND, 0);
		if (hGlobal != NULL)
		{
			IStream* pStream = NULL;
			if (SUCCEEDED(::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream)))
			{
				if (SUCCEEDED(Save(pStream)))
				{
					STATSTG stat = {0};
					if (SUCCEEDED(pStream->Stat(&stat, STATFLAG_NONAME)))
                    {
    					size = (UINT)stat.cbSize.QuadPart;
	    				if (size > 0)
		    			{
			    			*lpBuffer = _ATL_NEW BYTE[size];
				    		if (*lpBuffer != NULL)
					    	{
						    	LARGE_INTEGER dlibMove = {0};
    							pStream->Seek(dlibMove, STREAM_SEEK_SET, NULL);
                                hr = pStream->Read(*lpBuffer, size, NULL);
		    				}
			    			else
				    		{
						    	hr = E_OUTOFMEMORY;
					    	}
                        }
					}
				}
			}

ATLPREFAST_SUPPRESS(6102)
			if (pStream != NULL)
			{
				pStream->Release();
			}
ATLPREFAST_UNSUPPRESS()

			::GlobalFree(hGlobal);
		}

		return hr;
	}

	HRESULT CreateElement(
		_In_z_ LPCTSTR pszName,
		_Inout_ CXMLNode<IXMLDOMElement>& val)
	{
		IF_NOT_VALID_RETURN_EPTR;

		CString strName = pszName;
		BSTR bstr = strName.AllocSysString();
		HRESULT hr = Get()->createElement(bstr, val);
		::SysFreeString(bstr);

		return hr;
	}

	HRESULT CreateText(
		_In_z_ LPCTSTR pszText,
		_Inout_ CXMLNode<IXMLDOMText>& val)
	{
		IF_NOT_VALID_RETURN_EPTR;

		CString strText = pszText;
		BSTR bstr = strText.AllocSysString();
		HRESULT hr = Get()->createTextNode(bstr, val);
		::SysFreeString(bstr);

		return hr;
	}

	HRESULT CreateElementWithText(
		_In_z_ LPCTSTR pszName,
		_In_z_ LPCTSTR pszText,
		_Inout_ CXMLNode<IXMLDOMElement>& node)
	{
		IF_NOT_VALID_RETURN_EPTR;

		HRESULT hr = CreateElement(pszName, node);
		IF_HR_INVALID_RETURN_HR(hr);

		CXMLNode<IXMLDOMText> element;
		hr = CreateText(pszText, element);
		IF_HR_INVALID_RETURN_HR(hr);

		return node.AppendChild(element);
	}

	HRESULT GetDocumentElement(_Inout_ CXMLNode<IXMLDOMElement>& document)
	{
		IF_NOT_VALID_RETURN_EPTR;

		return Get()->get_documentElement(document);
	}

	const CString& GetErrorReason() const
	{
		return m_strErrorReason;
	}
	long GetErrorLine() const
	{
		return m_nErrorLine;
	}
	long GetErrorLinePos() const
	{
		return m_nErrorLinePos;
	}

protected:
	_Check_return_ HRESULT Initialize()
	{
		if (IsValid())
		{
			return S_OK;
		}

		HRESULT hr = CoInitialize(NULL);

		if (SUCCEEDED(hr))
		{
			m_bComInitialized = TRUE;

			hr = Get().CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER);
			IF_HR_INVALID_RETURN_HR(hr);

			hr = Get()->put_async(VARIANT_FALSE);

			if (SUCCEEDED(hr))
			{
				hr = Get()->put_validateOnParse(VARIANT_FALSE);
			}

			if (SUCCEEDED(hr))
			{
				hr = Get()->put_resolveExternals(VARIANT_FALSE);
			}

			if (SUCCEEDED(hr))
			{
				hr = Get()->put_preserveWhiteSpace(VARIANT_TRUE);
			}

			if (FAILED(hr))
			{
				Get().Release();
			}
		}

		return hr;
	}

	HRESULT PostLoad()
	{
		IF_NOT_VALID_RETURN_EPTR;

		CComQIPtr<IXMLDOMDocument2> doc2;
		HRESULT hr = Get()->QueryInterface(&doc2);

		IF_HR_INVALID_RETURN_HR(hr);

		BSTR bstr = T2BSTR(_T("SelectionLanguage"));
		VARIANT var;
		V_VT(&var) = VT_BSTR;
		V_BSTR(&var) = T2BSTR(_T("XPath"));

		hr = doc2->setProperty(bstr, var);

		::SysFreeString(bstr);
		::VariantClear(&var);

		return hr;
	}

	HRESULT UpdateErrorInfo()
	{
		IF_NOT_VALID_RETURN_EPTR;

		EmptyErrorInfo();

		CComPtr<IXMLDOMParseError> pXMLErr;
		HRESULT hr = Get()->get_parseError(&pXMLErr);

		IF_HR_INVALID_RETURN_HR(hr);

		BSTR bstrErr = NULL;
		if (SUCCEEDED(pXMLErr->get_reason(&bstrErr)))
		{
			m_strErrorReason = CStringW(bstrErr);
		}
		if (bstrErr)
		{
			::SysFreeString(bstrErr);
		}

		if (!SUCCEEDED(pXMLErr->get_line(&m_nErrorLine)))
		{
			m_nErrorLine = 1;
		}

		if (!SUCCEEDED(pXMLErr->get_linepos(&m_nErrorLinePos)))
		{
			m_nErrorLinePos = 1;
		}

		return hr;
	}

	void EmptyErrorInfo()
	{
		m_strErrorReason.Empty();
		m_nErrorLine    = -1;
		m_nErrorLinePos = -1;
	}

protected:
	CString m_strErrorReason;
	long    m_nErrorLine;
	long    m_nErrorLinePos;
	BOOL    m_bComInitialized;
};

}	// namespace ATL

#pragma pack(pop)
#pragma warning (pop)

#ifndef _ATL_NO_AUTOMATIC_NAMESPACE
using namespace ATL;
#endif //!_ATL_NO_AUTOMATIC_NAMESPACE

#endif	// __ATLXML_H__
