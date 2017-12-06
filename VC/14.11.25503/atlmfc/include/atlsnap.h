// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSNAP_H__
#define __ATLSNAP_H__

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#pragma warning(disable: 4702) // unreachable code
#pragma warning(disable: 4127) // conditional expression constant
#endif //!_ATL_NO_PRAGMA_WARNINGS

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif


#include <MMC.h>
#include <CommCtrl.h>
#include <atlwin.h>

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "mmc.lib")
#pragma comment(lib, "comctl32.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS


#pragma pack(push,_ATL_PACKING)
namespace ATL
{

template <class T, bool bAutoDelete = true>
class ATL_NO_VTABLE CSnapInPropertyPageImpl :
	public CDialogImplBase
{
public:
	PROPSHEETPAGE m_psp;

	operator PROPSHEETPAGE*()
	{
		return &m_psp;
	}

// Construction
	CSnapInPropertyPageImpl(_In_opt_z_ LPCTSTR lpszTitle = NULL)
	{
		// initialize PROPSHEETPAGE struct
		memset(&m_psp, 0, sizeof(PROPSHEETPAGE));
		m_psp.dwSize = sizeof(PROPSHEETPAGE);
		m_psp.dwFlags = PSP_USECALLBACK;
		m_psp.hInstance = _AtlBaseModule.GetResourceInstance();
		m_psp.pszTemplate = MAKEINTRESOURCE(T::IDD);
		m_psp.pfnDlgProc = (DLGPROC)T::StartDialogProc;
		m_psp.pfnCallback = T::PropPageCallback;
		m_psp.lParam = (LPARAM)this;

		if(lpszTitle != NULL)
		{
			m_psp.pszTitle = lpszTitle;
			m_psp.dwFlags |= PSP_USETITLE;
		}
	}

	static UINT CALLBACK PropPageCallback(
		_In_ HWND hWnd,
		_In_ UINT uMsg,
		_In_ LPPROPSHEETPAGE ppsp)
	{
		UNREFERENCED_PARAMETER(hWnd);
		ATLASSERT(hWnd == NULL);
		if(uMsg == PSPCB_CREATE)
		{
			CDialogImplBase* pPage = (CDialogImplBase*)ppsp->lParam;
			_AtlWinModule.AddCreateWndData(&pPage->m_thunk.cd, pPage);
		}
		if (bAutoDelete && uMsg == PSPCB_RELEASE)
		{
			T* pPage = (T*)ppsp->lParam;
			delete pPage;
		}

		return 1;
	}

	HPROPSHEETPAGE Create()
	{
		ATLASSUME(m_psp.lParam == (LPARAM)this);
		return ::CreatePropertySheetPage(&m_psp);
	}

	BOOL EndDialog(int)
	{
		// do nothing here, calling ::EndDialog will close the whole sheet
		ATLASSERT(FALSE);
		return FALSE;
	}

// Operations
	void CancelToClose()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetParent() != NULL);

		::SendMessage(GetParent(), PSM_CANCELTOCLOSE, 0, 0L);
	}
	void SetModified(_In_ BOOL bChanged = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetParent() != NULL);

		if(bChanged)
			::SendMessage(GetParent(), PSM_CHANGED, (WPARAM)m_hWnd, 0L);
		else
			::SendMessage(GetParent(), PSM_UNCHANGED, (WPARAM)m_hWnd, 0L);
	}
	LRESULT QuerySiblings(
		_In_ WPARAM wParam,
		_In_ LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetParent() != NULL);

		return ::SendMessage(GetParent(), PSM_QUERYSIBLINGS, wParam, lParam);
	}

	typedef CSnapInPropertyPageImpl< T, bAutoDelete > thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
	END_MSG_MAP()

// Message handler
	LRESULT OnNotify(
		_In_ UINT /*uMsg*/,
		_In_ WPARAM /*wParam*/,
		_In_ LPARAM lParam,
		_Inout_ BOOL& bHandled)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		NMHDR* pNMHDR = (NMHDR*)lParam;

		// don't handle messages not from the page/sheet itself
		if(pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd))
		{
			bHandled = FALSE;
			return 1;
		}

		T* pT = (T*)this;
		LRESULT lResult = 0;
		// handle default
		switch(pNMHDR->code)
		{
		case PSN_SETACTIVE:
			lResult = pT->OnSetActive() ? 0 : -1;
			break;
		case PSN_KILLACTIVE:
			lResult = !pT->OnKillActive();
			break;
		case PSN_APPLY:
			lResult = pT->OnApply() ? PSNRET_NOERROR : PSNRET_INVALID_NOCHANGEPAGE;
			break;
		case PSN_RESET:
			pT->OnReset();
			break;
		case PSN_QUERYCANCEL:
			lResult = !pT->OnQueryCancel();
			break;
		case PSN_WIZNEXT:
			lResult = !pT->OnWizardNext();
			break;
		case PSN_WIZBACK:
			lResult = !pT->OnWizardBack();
			break;
		case PSN_WIZFINISH:
			lResult = !pT->OnWizardFinish();
			break;
		case PSN_HELP:
			lResult = pT->OnHelp();
			break;
		default:
			bHandled = FALSE;	// not handled
		}

		return lResult;
	}

// Overridables
	BOOL OnSetActive()
	{
		return TRUE;
	}
	BOOL OnKillActive()
	{
		return TRUE;
	}
	BOOL OnApply()
	{
		return TRUE;
	}
	void OnReset()
	{
	}
	BOOL OnQueryCancel()
	{
		return TRUE;    // ok to cancel
	}
	BOOL OnWizardBack()
	{
		return TRUE;
	}
	BOOL OnWizardNext()
	{
		return TRUE;
	}
	BOOL OnWizardFinish()
	{
		return TRUE;
	}
	BOOL OnHelp()
	{
		return TRUE;
	}
};

class CSnapInItem;
class CSnapInObjectRootBase;

class CObjectData
{
public:
	CSnapInItem* m_pItem;
	DATA_OBJECT_TYPES m_type;
};

class ATL_NO_VTABLE CSnapInItem
{
public:
	virtual ~CSnapInItem()
	{
	}
	STDMETHOD(Notify)(
		_In_ MMC_NOTIFY_TYPE event,
		_In_ LONG_PTR arg,
		_In_ LONG_PTR param,
		_Inout_opt_ IComponentData* pComponentData,
		_Inout_opt_ IComponent* pComponent,
		_In_ DATA_OBJECT_TYPES type) = 0;

	STDMETHOD(GetScopePaneInfo)(_Out_ SCOPEDATAITEM *pScopeDataItem) = 0;

	STDMETHOD(GetResultViewType)(
		_Outptr_result_z_ LPOLESTR *ppViewType,
		_Out_ long *pViewOptions) = 0;

	STDMETHOD(GetResultPaneInfo)(_Out_ RESULTDATAITEM *pResultDataItem) = 0;

	STDMETHOD(AddMenuItems)(
		_In_ LPCONTEXTMENUCALLBACK piCallback,
		_Inout_ long  *pInsertionAllowed,
		_In_ DATA_OBJECT_TYPES type) = 0;

	STDMETHOD(Command)(
		_In_ long lCommandID,
		_Inout_ CSnapInObjectRootBase* pObj,
		_In_ DATA_OBJECT_TYPES type) = 0;

	STDMETHOD(CreatePropertyPages)(
		_In_ LPPROPERTYSHEETCALLBACK lpProvider,
		_In_ LONG_PTR handle,
		_Inout_ IUnknown* pUnk,
		_In_ DATA_OBJECT_TYPES type) = 0;

	STDMETHOD(QueryPagesFor)(_In_ DATA_OBJECT_TYPES type) = 0;

	STDMETHOD(SetControlbar)(
		_Inout_ IControlbar  *pControlbar,
		_Inout_opt_ IExtendControlbar  *pExtendControlbar,
		_Inout_ CSimpleMap<UINT, IUnknown*>* pToolbarMap) = 0;

	STDMETHOD(ControlbarNotify)(
		_Inout_ IControlbar *pControlbar,
		_Inout_opt_ IExtendControlbar *pExtendControlbar,
		_Inout_ CSimpleMap<UINT, IUnknown*>* pToolbarMap,
		_In_ MMC_NOTIFY_TYPE event,
		_In_ LONG_PTR arg,
		_In_ LONG_PTR param,
		_Inout_ CSnapInObjectRootBase* pObj,
		_In_ DATA_OBJECT_TYPES type) = 0;

	STDMETHOD(GetScopeData)(_Outptr_ SCOPEDATAITEM  **pScopeDataItem) = 0;

	STDMETHOD(GetResultData)(_Outptr_ RESULTDATAITEM  **pResultDataItem) = 0;

	STDMETHOD(FillData)(
		_In_ CLIPFORMAT cf,
		_Inout_ LPSTREAM pStream) = 0;

	virtual void InitDataClass(
		_Inout_opt_ IDataObject* /*pDataObject*/,
		_Inout_opt_ CSnapInItem* /*pDefault*/)
	{
		ATLASSERT(0 && "Override this function in derived class");
	}

	static HRESULT GetDataClass(
		_Inout_ IDataObject* pDataObj,
		_Outptr_result_bytebuffer_maybenull_(sizeof(CSnapInItem)) CSnapInItem** ppItem,
		_Out_ DATA_OBJECT_TYPES* pType)
	{
		if (ppItem == NULL)
			return E_POINTER;
		if (pType == NULL)
			return E_POINTER;

		if (IS_SPECIAL_DATAOBJECT(pDataObj))
		{
			ATLTRACENOTIMPL(_T("CSnapInItem::GetDataClass"));
		}

		*ppItem = NULL;
		*pType = CCT_UNINITIALIZED;

		STGMEDIUM stgmedium = { TYMED_HGLOBAL, NULL };
		FORMATETC formatetc = { m_CCF_SNAPIN_GETOBJECTDATA,
			NULL,
			DVASPECT_CONTENT,
			-1,
			TYMED_HGLOBAL
		};

		stgmedium.hGlobal = GlobalAlloc(0, sizeof(CObjectData));
		if (stgmedium.hGlobal == NULL)
			return E_OUTOFMEMORY;

ATLPREFAST_SUPPRESS(6011)
		HRESULT hr = pDataObj->GetDataHere(&formatetc, &stgmedium);
ATLPREFAST_UNSUPPRESS()
		if (SUCCEEDED(hr))
		{
			CObjectData* pData = (CObjectData*)stgmedium.hGlobal;
			 *ppItem = pData->m_pItem;
			 *pType = pData->m_type;
		}

		GlobalFree(stgmedium.hGlobal);

		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE GetDataObject(
		_Outptr_ IDataObject** pDataObj,
		_In_ DATA_OBJECT_TYPES type) = 0;

	static void Init()
	{
		m_CCF_NODETYPE			= (CLIPFORMAT) RegisterClipboardFormat(_T("CCF_NODETYPE"));
		m_CCF_SZNODETYPE		= (CLIPFORMAT) RegisterClipboardFormat(_T("CCF_SZNODETYPE"));
		m_CCF_DISPLAY_NAME		= (CLIPFORMAT) RegisterClipboardFormat(_T("CCF_DISPLAY_NAME"));
		m_CCF_SNAPIN_CLASSID	= (CLIPFORMAT) RegisterClipboardFormat(_T("CCF_SNAPIN_CLASSID"));
		m_CCF_SNAPIN_GETOBJECTDATA	= (CLIPFORMAT) RegisterClipboardFormat(_T("CCF_GETOBJECTDATA"));
		m_CCF_MMC_MULTISELECT_DATAOBJECT	= (CLIPFORMAT) RegisterClipboardFormat(_T("CCF_MMC_MULTISELECT_DATAOBJECT"));
	}
public:
	static CLIPFORMAT m_CCF_NODETYPE;
	static CLIPFORMAT m_CCF_SZNODETYPE;
	static CLIPFORMAT m_CCF_DISPLAY_NAME;
	static CLIPFORMAT m_CCF_SNAPIN_CLASSID;
	static CLIPFORMAT m_CCF_SNAPIN_GETOBJECTDATA;
	static CLIPFORMAT m_CCF_MMC_MULTISELECT_DATAOBJECT;
};

class CSnapInObjectRootBase
{
public:
	CComPtr <IControlbar> m_spControlbar;
	CSimpleMap <UINT, IUnknown*> m_toolbarMap;
	const int m_nType;

	CSnapInObjectRootBase(_In_ int n = 0) : m_nType(n)
	{
	}
ATLPREFAST_SUPPRESS(6387)
	HRESULT GetDataClass(
		_In_ IDataObject* pDataObject,
		_Outptr_result_maybenull_ CSnapInItem** ppItem,
		_Out_ DATA_OBJECT_TYPES* pType)
	{
		return CSnapInItem::GetDataClass(pDataObject, ppItem, pType);
	}
ATLPREFAST_UNSUPPRESS()
};


template <int n, class ComponentData>
class CSnapInObjectRoot :
	public CSnapInObjectRootBase
{
public :
	CSnapInObjectRoot() : CSnapInObjectRootBase(n)
	{
		m_pComponentData = NULL;
	}
	ComponentData* m_pComponentData;
};

#define EXTENSION_SNAPIN_DATACLASS(dataClass) dataClass m_##dataClass;

#define BEGIN_EXTENSION_SNAPIN_NODEINFO_MAP(classname) \
	HRESULT GetDataClass( \
		_In_ IDataObject* pDataObject, \
		_Outptr_ ATL::CSnapInItem** ppItem, \
		_Out_ DATA_OBJECT_TYPES* pType) \
	{ \
		if (ppItem == NULL) \
			return E_POINTER; \
		if (pType == NULL) \
			return E_POINTER; \
\
		*ppItem = NULL; \
\
		*pType = CCT_UNINITIALIZED; \
\
		STGMEDIUM stgmedium = { TYMED_HGLOBAL, NULL }; \
		FORMATETC formatetc = { ATL::CSnapInItem::m_CCF_NODETYPE, \
			NULL, \
			DVASPECT_CONTENT, \
			-1, \
			TYMED_HGLOBAL \
		}; \
\
		stgmedium.hGlobal = GlobalAlloc(0, sizeof(GUID)); \
		if (stgmedium.hGlobal == NULL) \
			return E_OUTOFMEMORY; \
\
		HRESULT hr = pDataObject->GetDataHere(&formatetc, &stgmedium); \
		if (FAILED(hr)) \
		{ \
			GlobalFree(stgmedium.hGlobal); \
			return hr; \
		} \
\
		GUID guid; \
		Checked::memcpy_s(&guid, sizeof(GUID), stgmedium.hGlobal, sizeof(GUID)); \
\
		GlobalFree(stgmedium.hGlobal); \
		hr = S_OK;

#define EXTENSION_SNAPIN_NODEINFO_ENTRY(dataClass) \
		if (IsEqualGUID(guid, *(GUID*)m_##dataClass.GetNodeType())) \
		{ \
			*ppItem = m_##dataClass.GetExtNodeObject(pDataObject, &m_##dataClass); \
			ATLASSERT(*ppItem != NULL); \
			(*ppItem)->InitDataClass(pDataObject, &m_##dataClass); \
			return hr; \
		}

#define END_EXTENSION_SNAPIN_NODEINFO_MAP() \
			return ATL::CSnapInItem::GetDataClass(pDataObject, ppItem, pType); \
	};

class ATL_NO_VTABLE CSnapInDataObjectImpl :
	public IDataObject,
	public CComObjectRoot
{
public:
	virtual ~CSnapInDataObjectImpl()
	{
	}
	BEGIN_COM_MAP(CSnapInDataObjectImpl)
		COM_INTERFACE_ENTRY(IDataObject)
	END_COM_MAP()
	STDMETHOD(GetData)(
		_In_opt_ FORMATETC * /*pformatetcIn*/,
		_In_opt_ STGMEDIUM * /*pmedium*/)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::GetData\n"));
	}

	STDMETHOD(GetDataHere)(
		_In_ FORMATETC* pformatetc,
		_In_ STGMEDIUM* pmedium)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("SnapInDataObjectImpl::GetDataHere\n"));
		if (pmedium == NULL)
			return E_POINTER;

		HRESULT hr = DV_E_TYMED;
		// Make sure the type medium is HGLOBAL
		if (pmedium->tymed == TYMED_HGLOBAL)
		{
			// Create the stream on the hGlobal passed in
			CComPtr<IStream> spStream;
			hr = CreateStreamOnHGlobal(pmedium->hGlobal, FALSE, &spStream);
			if (SUCCEEDED(hr))
				if (pformatetc->cfFormat == CSnapInItem::m_CCF_SNAPIN_GETOBJECTDATA)
				{
					hr = DV_E_CLIPFORMAT;
					ULONG uWritten;
					hr = spStream->Write(&m_objectData, sizeof(CObjectData), &uWritten);
				}
				else
					hr = m_objectData.m_pItem->FillData(pformatetc->cfFormat, spStream);
		}
		return hr;
	}

	STDMETHOD(QueryGetData)(_In_opt_ FORMATETC* /* pformatetc */)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::QueryGetData\n"));
	}
	STDMETHOD(GetCanonicalFormatEtc)(
		_In_opt_ FORMATETC* /* pformatectIn */,
		_In_opt_ FORMATETC* /* pformatetcOut */)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::GetCanonicalFormatEtc\n"));
	}
	STDMETHOD(SetData)(
		_In_opt_ FORMATETC* /* pformatetc */,
		_In_opt_ STGMEDIUM* /* pmedium */,
		BOOL /* fRelease */)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::SetData\n"));
	}
	STDMETHOD(EnumFormatEtc)(
		DWORD /* dwDirection */,
		_In_ IEnumFORMATETC** /* ppenumFormatEtc */)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::EnumFormatEtc\n"));
	}
	STDMETHOD(DAdvise)(
		_In_opt_ FORMATETC * /*pformatetc*/,
		DWORD /*advf*/,
		_Inout_opt_ IAdviseSink * /*pAdvSink*/,
		_Out_opt_ DWORD* /*pdwConnection*/)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::SetData\n"));
	}
	STDMETHOD(DUnadvise)(DWORD /*dwConnection*/)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::SetDatan\n"));
	}
	STDMETHOD(EnumDAdvise)(_In_ IEnumSTATDATA ** /*ppenumAdvise*/)
	{
		ATLTRACENOTIMPL(_T("SnapInDataObjectImpl::SetData\n"));
	}

	CObjectData m_objectData;
};


template <class T, class Component>
class ATL_NO_VTABLE IComponentDataImpl :
	public IComponentData
{
public :
	IComponentDataImpl()
	{
		m_pNode = NULL;
	}

	STDMETHOD(Initialize)(_Inout_ LPUNKNOWN pUnknown)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentDataImpl::Initialize\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(pUnknown != NULL);
		if (pUnknown == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IComponentData::Initialize called with pUnknown == NULL\n"));
		else
		{
			hr = pUnknown->QueryInterface(&m_spConsole);
			if (FAILED(hr))
				ATLTRACE(atlTraceSnapin, 0, _T("QI for IConsole failed\n"));
		}

		return hr;
	}

	STDMETHOD(CreateComponent)(_Outptr_ LPCOMPONENT *ppComponent)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentDataImpl::CreateComponent\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(ppComponent != NULL);
		if (ppComponent == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IComponentData::CreateComponent called with ppComponent == NULL\n"));
		else
		{
			*ppComponent = NULL;

			CComObject< Component >* pComponent;
			hr = CComObject< Component >::CreateInstance(&pComponent);
			ATLASSUME(SUCCEEDED(hr));
			if (FAILED(hr))
				ATLTRACE(atlTraceSnapin, 0, _T("IComponentData::CreateComponent : Could not create IComponent object\n"));
			else
			{
				pComponent->m_pComponentData = static_cast<T*>(this);
				hr = pComponent->QueryInterface(__uuidof(IComponent), (void**)ppComponent);
			}
		}
		return hr;
	}

	STDMETHOD(Notify)(
		_Inout_ LPDATAOBJECT lpDataObject,
		_In_ MMC_NOTIFY_TYPE event,
		_In_ LONG_PTR arg,
		_In_ LONG_PTR param)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentDataImpl::Notify\n"));
		ATLASSERT(lpDataObject != NULL && _T("Override Notify in derived class handle notifications for which lpDataObject == NULL"));
		HRESULT hr = E_POINTER;

		ATLASSERT(lpDataObject != NULL);
		if (lpDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IComponentData::Notify called with lpDataObject == NULL\n"));
		else
		{
			T* pT = static_cast<T*>(this);
			CSnapInItem* pItem;
			DATA_OBJECT_TYPES type;
			hr = pT->m_pComponentData->GetDataClass(lpDataObject, &pItem, &type);
			ATLASSERT(SUCCEEDED(hr));
			if (SUCCEEDED(hr))
			{
				hr = pItem->Notify(event, arg, param, pT, NULL, type);
			}
		}
		return hr;
	}

	STDMETHOD(Destroy)(void)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentDataImpl::Destroy\n"));

		T* pT = static_cast<T*>(this);
		if (pT->m_spControlbar != NULL)
		{
			int n = pT->m_toolbarMap.GetSize();
			for (int i = 0; i < n; i++)
			{
				IToolbar* pToolbar = (IToolbar*)pT->m_toolbarMap.GetValueAt(i);
				 if (pToolbar != NULL)
				 {
					pT->m_spControlbar->Detach(pToolbar);
					pToolbar->Release();
				 }
			}
		}
		pT->m_toolbarMap.RemoveAll();

		m_spConsole.Release();
		return S_OK;
	}

	STDMETHOD(QueryDataObject)(
		_In_ LONG_PTR cookie,
		_In_ DATA_OBJECT_TYPES type,
		_Outptr_ LPDATAOBJECT *ppDataObject)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentDataImpl::QueryDataObject\n"));
		HRESULT hr = E_POINTER;

		ATLASSERT(ppDataObject != NULL);
		if (ppDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IComponentData::QueryDataObject called with ppDataObject == NULL\n"));
		else
		{
			*ppDataObject = NULL;

			CSnapInItem* pItem = (CSnapInItem*) cookie;
			if (cookie == NULL)
				pItem = m_pNode;

			hr = pItem->GetDataObject(ppDataObject, type);
		}
		return hr;
	}

	STDMETHOD(GetDisplayInfo)(_Inout_ SCOPEDATAITEM *pScopeDataItem)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentDataImpl::GetDisplayInfo\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(pScopeDataItem != NULL);
		if (pScopeDataItem == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IComponentData::GetDisplayInfo called with pScopeDataItem == NULL\n"));
		else
		{
			CSnapInItem* pItem= (CSnapInItem*) pScopeDataItem->lParam;
			if (pItem == NULL)
				pItem = m_pNode;

			hr = E_UNEXPECTED;
			if (pItem != NULL)
				hr = pItem->GetScopePaneInfo(pScopeDataItem);
		}
		return hr;
	}

	STDMETHOD(CompareObjects)(
		_In_opt_ LPDATAOBJECT /*lpDataObjectA*/,
		_In_opt_ LPDATAOBJECT /*lpDataObjectB*/)
	{
		ATLTRACENOTIMPL(_T("IComponentDataImpl::CompareObjects\n"));
	}

	CComPtr<IConsole> m_spConsole;
	CSnapInItem* m_pNode;
};


template <class T>
class ATL_NO_VTABLE IComponentImpl :
	public IComponent
{
public:
	STDMETHOD(Initialize)(_Inout_ LPCONSOLE lpConsole)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentImpl::Initialize\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(lpConsole != NULL);
		if (lpConsole == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("lpConsole is NULL\n"));
		else
		{
			m_spConsole = lpConsole;

			CComPtr<IHeaderCtrl> spHeaderCtrl;
			hr = m_spConsole.QueryInterface(&spHeaderCtrl);
			if (FAILED(hr))
				ATLTRACE(atlTraceSnapin, 0, _T("QI for IHeaderCtrl failed\n"));
			else
			{
				hr = m_spConsole->SetHeader(spHeaderCtrl);
				if (FAILED(hr))
					ATLTRACE(atlTraceSnapin, 0, _T("IConsole::SetHeader failed (HRESULT = %x)\n"), hr);
			}
		}
		return hr;
	}

	STDMETHOD(Notify)(
		_Inout_ LPDATAOBJECT lpDataObject,
		_In_ MMC_NOTIFY_TYPE event,
		_In_ LONG_PTR arg,
		_In_ LONG_PTR param)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentImpl::Notify\n"));
		ATLASSERT(lpDataObject != NULL && _T("Override Notify in derived class handle notifications for which lpDataObject == NULL"));
		HRESULT hr = E_POINTER;

		if (lpDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IComponent::Notify called with lpDataObject==NULL \n"));
		else
		{
			T* pT = static_cast<T*>(this);
			CSnapInItem* pItem;
			DATA_OBJECT_TYPES type;
			// Make sure that the object is derived from CSnapInObjectRoot
			hr = pT->m_pComponentData->GetDataClass(lpDataObject, &pItem, &type);
			if (SUCCEEDED(hr))
			{
				hr = pItem->Notify(event, arg, param, NULL, pT, type);
			}
		}
		return hr;
	}

	STDMETHOD(Destroy)(_In_ LONG_PTR /*cookie*/)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentImpl::Destroy\n"));

		T* pT = static_cast<T*>(this);

		if (pT->m_spControlbar != NULL)
		{
			int n = pT->m_toolbarMap.GetSize();
			for (int i = 0; i < n; i++)
			{
				IToolbar* pToolbar = (IToolbar*)pT->m_toolbarMap.GetValueAt(i);
				 if (pToolbar != NULL)
				 {
					pT->m_spControlbar->Detach(pToolbar);
					pToolbar->Release();
				 }
			}
		}
		pT->m_toolbarMap.RemoveAll();

		m_spConsole->SetHeader(NULL);
		m_spConsole.Release();
		return S_OK;
	}

	STDMETHOD(QueryDataObject)(
		_In_ LONG_PTR cookie,
		_In_ DATA_OBJECT_TYPES type,
		_Outptr_ LPDATAOBJECT *ppDataObject)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentImpl::QueryDataObject\n"));

		ATLASSERT(ppDataObject != NULL);
		if (ppDataObject == NULL)
		{
			ATLTRACE(atlTraceSnapin, 0, _T("IComponent::QueryDataObject called with ppDataObject==NULL \n"));
			return E_POINTER;
		}

		if (cookie == NULL)
		{
			ATLTRACE(atlTraceSnapin, 0, _T("IComponent::QueryDataObject called with cookie==NULL \n"));
			return E_UNEXPECTED;
		}

		*ppDataObject = NULL;
		if (cookie == MMC_MULTI_SELECT_COOKIE)
		{
			ATLTRACE(atlTraceSnapin, 0, _T("Override QueryDataObject to handle multiselect\n"));
			return E_UNEXPECTED;
		}

		CSnapInItem* pItem = (CSnapInItem*) cookie;
		return pItem->GetDataObject(ppDataObject, type);
	}

	STDMETHOD(GetResultViewType)(
		_In_ LONG_PTR cookie,
		_Outptr_result_z_ LPOLESTR *ppViewType,
		_Out_ long *pViewOptions)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentImpl::GetResultViewType\n"));

		HRESULT hr = E_FAIL;
		if (cookie == NULL)
		{
			T* pT = static_cast<T*> (this);
			ATLENSURE_RETURN( pT->m_pComponentData != NULL );
			ATLENSURE_RETURN( pT->m_pComponentData->m_pNode != NULL );
			hr = pT->m_pComponentData->m_pNode->GetResultViewType(ppViewType, pViewOptions);
		}
		else
		{
			CSnapInItem* pItem = (CSnapInItem*)cookie;
			hr = pItem->GetResultViewType(ppViewType, pViewOptions);
		}
		return hr;
	}

	STDMETHOD(GetDisplayInfo)(_Inout_ RESULTDATAITEM *pResultDataItem)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IComponentImpl::GetDisplayInfo\n"));

		ATLASSERT(pResultDataItem != NULL);
		if (pResultDataItem == NULL)
		{
			ATLTRACE(atlTraceSnapin, 0, _T("IComponent::GetDisplayInfo called with pResultDataItem==NULL\n"));
			return E_POINTER;
		}

		CSnapInItem* pItem = (CSnapInItem*) pResultDataItem->lParam;

		if (pItem == NULL)
		{
			ATLTRACE(atlTraceSnapin, 0, _T("Invalid Item\n"));
			return E_UNEXPECTED;
		}
		return pItem->GetResultPaneInfo(pResultDataItem);
	}

	STDMETHOD(CompareObjects)(
		_In_opt_ LPDATAOBJECT /*lpDataObjectA*/,
		_In_opt_ LPDATAOBJECT /*lpDataObjectB*/)
	{
		ATLTRACENOTIMPL(_T("IComponentImpl::CompareObjects\n"));
	}

	CComPtr<IConsole> m_spConsole;
};

template <class T, class D>
class ATL_NO_VTABLE IResultDataCompareImpl :
	public IResultDataCompare
{
public:
	STDMETHOD(Compare)(long /*lUserParam*/,
		_In_ long /*cookieA*/,
		_In_ long /*cookieB*/,
		_In_opt_ int* /*pnResult*/)
	{
		ATLTRACENOTIMPL(_T("IResultDataCompareImpl::Compare"));
	}
};


template <class T>
class ATL_NO_VTABLE IExtendContextMenuImpl :
	public IExtendContextMenu
{
public:
	STDMETHOD(AddMenuItems)(
		_Inout_ LPDATAOBJECT pDataObject,
		_In_ LPCONTEXTMENUCALLBACK piCallback,
		_Inout_ long *pInsertionAllowed)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IExtendContextMenuImpl::AddMenuItems\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(pDataObject != NULL);
		if (pDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IExtendContextMenu::AddMenuItems called with pDataObject==NULL\n"));
		else
		{
			T* pT = static_cast<T*>(this);
			CSnapInItem* pItem;
			DATA_OBJECT_TYPES type;
			hr = pT->m_pComponentData->GetDataClass(pDataObject, &pItem, &type);

			if (SUCCEEDED(hr))
			{
				hr = pItem->AddMenuItems(piCallback, pInsertionAllowed, type);
			}
		}
		return hr;
	}

	STDMETHOD(Command)(
		_In_ long lCommandID,
		_Inout_ LPDATAOBJECT pDataObject)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IExtendContextMenuImpl::Command\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(pDataObject != NULL);
		if (pDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IExtendContextMenu::Command called with pDataObject==NULL\n"));
		else
		{
			T* pT = static_cast<T*>(this);
			CSnapInItem* pItem;
			DATA_OBJECT_TYPES type;
			hr = pT->m_pComponentData->GetDataClass(pDataObject, &pItem, &type);

			if (SUCCEEDED(hr))
			{
				hr = pItem->Command(lCommandID, (CSnapInObjectRootBase*)pT, type);
			}
		}
		return hr;
	}
};

template<class T>
class ATL_NO_VTABLE IExtendPropertySheetImpl :
	public IExtendPropertySheet
{
public:
	STDMETHOD(CreatePropertyPages)(
		_In_ LPPROPERTYSHEETCALLBACK lpProvider,
		_In_ LONG_PTR handle,
		_Inout_ LPDATAOBJECT pDataObject)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IExtendPropertySheetImpl::CreatePropertyPages\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(pDataObject != NULL);
		if (pDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IExtendPropertySheetImpl::CreatePropertyPages called with pDataObject==NULL\n"));
		else
		{
			T* pT = static_cast<T*>(this);
			CSnapInItem* pItem;
			DATA_OBJECT_TYPES type;
			hr = pT->m_pComponentData->GetDataClass(pDataObject, &pItem, &type);

			if (SUCCEEDED(hr))
			{
				hr = pItem->CreatePropertyPages(lpProvider, handle, this, type);
			}
		}

		return hr;
	}

	STDMETHOD(QueryPagesFor)(_Inout_ LPDATAOBJECT pDataObject)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IExtendPropertySheetImpl::QueryPagesFor\n"));

		HRESULT hr = E_POINTER;

		ATLASSERT(pDataObject != NULL);
		if (pDataObject == NULL)
			ATLTRACE(atlTraceSnapin, 0, _T("IExtendPropertySheetImpl::QueryPagesFor called with pDataObject==NULL\n"));
		else
		{
			T* pT = static_cast<T*>(this);
			CSnapInItem* pItem;
			DATA_OBJECT_TYPES type;
			hr = pT->m_pComponentData->GetDataClass(pDataObject, &pItem, &type);

			if (SUCCEEDED(hr))
			{
				hr = pItem->QueryPagesFor(type);
			}
		}
		return hr;
	}
};

template <class T>
class ATL_NO_VTABLE IExtendControlbarImpl :
	public IExtendControlbar
{
public:
	STDMETHOD(SetControlbar)(_In_opt_ LPCONTROLBAR pControlbar)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IExtendControlbarImpl::SetControlbar\n"));
		T* pT = static_cast<T*>(this);

		if (pT->m_spControlbar != NULL)
		{
			int n = pT->m_toolbarMap.GetSize();
			for (int i = 0; i < n; i++)
			{
				IToolbar* pToolbar = (IToolbar*)pT->m_toolbarMap.GetValueAt(i);
				 if (pToolbar != NULL)
				 {
					pT->m_spControlbar->Detach(pToolbar);
					pToolbar->Release();
				 }
			}
		}
		pT->m_toolbarMap.RemoveAll();

		pT->m_spControlbar = pControlbar;
		return S_OK;
	}

	STDMETHOD(ControlbarNotify)(
		_In_ MMC_NOTIFY_TYPE event,
		_In_ LONG_PTR arg,
		_In_ LONG_PTR param)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("IExtendControlbarImpl::ControlbarNotify\n"));

		CSnapInItem* pItem = NULL;
		DATA_OBJECT_TYPES type = CCT_UNINITIALIZED;
		HRESULT hr = S_FALSE;
		T* pT = static_cast<T*>(this);

		if (event == MMCN_BTN_CLICK)
			hr = pT->m_pComponentData->GetDataClass((IDataObject*) arg, &pItem, &type);
		else if (event == MMCN_SELECT)
		{
			hr = pT->m_pComponentData->GetDataClass((IDataObject*) param, &pItem, &type);
			const BOOL bSelect = (BOOL) HIWORD (arg);
			if (bSelect)
			{
				int n = pT->m_toolbarMap.GetSize();
				for (int i = 0; i < n; i++)
				{
					IToolbar* pToolbar = (IToolbar*)pT->m_toolbarMap.GetValueAt(i);
					 if (pToolbar != NULL)
						pT->m_spControlbar->Detach(pToolbar);
				}
			}
		}

		if (SUCCEEDED(hr) && pItem != NULL)
			hr = pItem->ControlbarNotify(pT->m_spControlbar, this, &(pT->m_toolbarMap), event, arg, param, (CSnapInObjectRootBase*) pT, type);

		return hr;
	}
};

#define SNAPINMENUID(id) \
public: \
	static const UINT GetMenuID() \
	{ \
		static const UINT IDMENU = id; \
		return id; \
	}

#define EXT_SNAPINMENUID(id) \
public: \
	static const UINT GetMenuID() \
	{ \
		static const UINT IDMENU = id; \
		return id; \
	}

#define BEGIN_SNAPINCOMMAND_MAP(theClass, bIsExtension) \
	HRESULT ProcessCommand(UINT nID, \
		_Out_ bool& bHandled, \
		_In_opt_ ATL::CSnapInObjectRootBase* pObj, \
		_In_ DATA_OBJECT_TYPES type) \
	{ \
			bHandled = true; \
			HRESULT hr = S_OK;

#define SNAPINCOMMAND_ENTRY(id, func) \
		if (id == nID) \
		{ \
			hr = func(bHandled, pObj); \
			if (bHandled) \
				return hr; \
		}

#define SNAPINCOMMAND_RANGE_ENTRY(id1, id2, func) \
		if (nID >= id1 && nID <= id2) \
		{ \
			hr = func(nID, bHandled, pObj); \
			if (bHandled) \
				return hr; \
		}

#define CHAIN_SNAPINCOMMAND_MAP(theChainClass) \
	{ \
		hr = theChainClass::ProcessCommand(nID, bHandled, pObj, type); \
		if (bHandled) \
			return hr; \
	}

#define END_SNAPINCOMMAND_MAP() \
			return hr; \
	}

struct CSnapInToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]

	WORD* items()
	{
		return (WORD*)(this+1);
	}
};

#define RT_TOOLBAR  MAKEINTRESOURCE(241)

class CSnapInToolbarInfo
{
public:
	void __stdcall CleanUp(_In_ DWORD_PTR /*dw*/)
	{
		if (m_pStrToolTip)
		{
			for (UINT i = 0; i < m_nButtonCount; i++)
			{
				delete m_pStrToolTip[i];
				m_pStrToolTip[i] = NULL;
			}
			delete [] m_pStrToolTip;
			m_pStrToolTip = NULL;
		}

		if (m_pStrButtonText)
		{
			for (UINT i = 0; i < m_nButtonCount; i++)
			{
				delete m_pStrButtonText[i];
				m_pStrButtonText[i] = NULL;
			}

			delete [] m_pStrButtonText;
			m_pStrButtonText = NULL;
		}
		if (m_pnButtonID)
		{
			delete m_pnButtonID;
			m_pnButtonID = NULL;
		}

		m_nButtonCount = 0;
	}

	OLECHAR** m_pStrToolTip;
	OLECHAR** m_pStrButtonText;
	UINT* m_pnButtonID;
	UINT m_idToolbar;
	UINT m_nButtonCount;
};

#define BEGIN_SNAPINTOOLBARID_MAP(theClass) \
public: \
	static ATL::CSnapInToolbarInfo* GetToolbarInfo() \
	{ \
		static ATL::CSnapInToolbarInfo m_toolbarInfo[] = \
		{

#define SNAPINTOOLBARID_ENTRY(id) \
			{ NULL, NULL, NULL, id, 0},

#define END_SNAPINTOOLBARID_MAP() \
			{ NULL, NULL, NULL, 0, 0} \
		}; \
		return m_toolbarInfo; \
	}

template <class T, BOOL bIsExtension = FALSE>
class ATL_NO_VTABLE CSnapInItemImpl :
	public CSnapInItem
{
public:
	CSnapInItemImpl()
	{
	}

	virtual ~CSnapInItemImpl()
	{
	}

public:

	STDMETHOD(Notify)(
		_In_ MMC_NOTIFY_TYPE /*event*/,
		_In_ LONG_PTR /*arg*/,
		_In_ LONG_PTR /*param*/,
		_Inout_opt_ IComponentData* /*pComponentData*/,
		_Inout_opt_ IComponent* /*pComponent*/,
		_In_ DATA_OBJECT_TYPES /*type*/)
	{
		ATLASSERT("Override Function in Derived Class");
		ATLTRACENOTIMPL(_T("CSnapInItemImpl::Notify"));
	}

	STDMETHOD(GetScopePaneInfo)(_In_ SCOPEDATAITEM * /*pScopeDataItem*/)
	{
		ATLTRACENOTIMPL(_T("CSnapInItemImpl::GetScopePaneInfo"));
	}

	STDMETHOD(GetResultViewType)(
		_Outptr_result_maybenull_z_ LPOLESTR *ppViewType,
		_Out_ long *pViewOptions)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("CSnapInItemImpl::GetResultViewType\n"));
		*ppViewType = NULL;
		*pViewOptions = MMC_VIEW_OPTIONS_NONE;
		return S_FALSE;
	}

	STDMETHOD(GetResultPaneInfo)(_In_opt_ RESULTDATAITEM * /*pResultDataItem*/)
	{
		ATLTRACENOTIMPL(_T("CSnapInItemImpl::GetResultPaneInfo"));
	}

	STDMETHOD(AddMenuItems)(
		_In_ LPCONTEXTMENUCALLBACK piCallback,
		_Inout_ long *pInsertionAllowed,
		_In_ DATA_OBJECT_TYPES /*type*/)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("CSnapInItemImpl::AddMenuItems\n"));
		T* pT = static_cast<T*>(this);

		if (!bIsExtension)
			pT->SetMenuInsertionFlags(true, pInsertionAllowed);

		UINT menuID = pT->GetMenuID();
		if (menuID == 0)
			return S_OK;

		HMENU hMenu = LoadMenu(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(menuID));
		long insertionID;
		if (hMenu)
		{
			USES_CONVERSION_EX;

			for (int i = 0; 1; i++)
			{
				HMENU hSubMenu = GetSubMenu(hMenu, i);
				if (hSubMenu == NULL)
					break;

				MENUITEMINFO menuItemInfo;
				memset(&menuItemInfo, 0, sizeof(menuItemInfo));
				menuItemInfo.cbSize = sizeof(menuItemInfo);

				switch (i)
				{
				case 0:
					if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_TOP) )
						continue;
					insertionID = CCM_INSERTIONPOINTID_PRIMARY_TOP;
					break;

				case 1:
					if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_NEW) )
						continue;
					if (bIsExtension)
						insertionID = CCM_INSERTIONPOINTID_3RDPARTY_NEW;
					else
						insertionID = CCM_INSERTIONPOINTID_PRIMARY_NEW;
					break;

				case 2:;
					if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_TASK) )
						continue;
					if (bIsExtension)
						insertionID = CCM_INSERTIONPOINTID_3RDPARTY_TASK;
					else
						insertionID = CCM_INSERTIONPOINTID_PRIMARY_TASK;
					break;
				case 3:;
					if (! (*pInsertionAllowed & CCM_INSERTIONALLOWED_VIEW) )
						continue;
					insertionID = CCM_INSERTIONPOINTID_PRIMARY_VIEW;
					break;
				default:
					{
						insertionID = 0;
						continue;
					}
					break;
				}

				menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
				menuItemInfo.fType = MFT_STRING;
				TCHAR szMenuText[128];

				for (int j = 0; 1; j++)
				{
					menuItemInfo.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
					menuItemInfo.fType = MFT_STRING;
					menuItemInfo.cch = 128;
					menuItemInfo.dwTypeData = szMenuText;
					TCHAR szStatusBar[256];

					if (!GetMenuItemInfo(hSubMenu, j, TRUE, &menuItemInfo))
						break;
					if (menuItemInfo.fType != MFT_STRING)
						continue;

					/* force termination -- should be unnecessary because we check return from GetMenuItemInfo */
					szMenuText[_countof(szMenuText)-1]=_T('\0');

					pT->UpdateMenuState(menuItemInfo.wID, szMenuText, &menuItemInfo.fState);
					LoadString(_AtlBaseModule.GetResourceInstance(), menuItemInfo.wID, szStatusBar, 256);

					OLECHAR wszStatusBar[256];
					OLECHAR wszMenuText[128];
					if(!ocscpy_s(wszMenuText, _countof(wszMenuText), CT2COLE(szMenuText)) ||
						!ocscpy_s(wszStatusBar, _countof(wszStatusBar), CT2COLE (szStatusBar)))
					{
						return E_FAIL;
					}

					CONTEXTMENUITEM contextMenuItem;
					contextMenuItem.strName = wszMenuText;
					contextMenuItem.strStatusBarText = wszStatusBar;
					contextMenuItem.lCommandID = menuItemInfo.wID;
					contextMenuItem.lInsertionPointID = insertionID;
					contextMenuItem.fFlags = menuItemInfo.fState;
					contextMenuItem.fSpecialFlags = 0;

					HRESULT hr = piCallback->AddItem(&contextMenuItem);
					DBG_UNREFERENCED_LOCAL_VARIABLE(hr);
					ATLASSERT(SUCCEEDED(hr));
				}
			}
			DestroyMenu(hMenu);
		}

		if (!bIsExtension)
			pT->SetMenuInsertionFlags(true, pInsertionAllowed);

		return S_OK;
	}

	STDMETHOD(Command)(
		_In_ long lCommandID,
		_Inout_opt_ CSnapInObjectRootBase* pObj,
		_In_ DATA_OBJECT_TYPES type)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("CSnapInItemImpl::Command\n"));
		bool bHandled = false;
		T* pT = static_cast<T*>(this);
		return pT->ProcessCommand(lCommandID, bHandled, pObj, type);
	}

	STDMETHOD(CreatePropertyPages)(
		_In_ LPPROPERTYSHEETCALLBACK /*lpProvider*/,
		_In_ LONG_PTR /*handle*/,
		_Inout_opt_ IUnknown* /*pUnk*/,
		_In_ DATA_OBJECT_TYPES /*type*/)
	{
		ATLASSERT("Override Function in Derived Class");
		ATLTRACENOTIMPL(_T("CSnapInItemImpl::CreatePropertyPages"));
	}

	STDMETHOD(QueryPagesFor)(_In_ DATA_OBJECT_TYPES /*type*/)
	{
		ATLASSERT("Override Function in Derived Class");
		ATLTRACENOTIMPL(_T("CSnapInItemImpl::QueryPagesFor"));
	}

	STDMETHOD(SetControlbar)(
		_Inout_ IControlbar *pControlbar,
		_Inout_opt_ IExtendControlbar* pExtendControlBar,
		_Inout_ CSimpleMap<UINT, IUnknown*>* pToolbarMap)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("CSnapInItemImpl::SetControlbar\n"));
		static bool m_bAddTermFunc = false;
		if (!m_bAddTermFunc)
		{
			m_bAddTermFunc = true;
			_pAtlModule->AddTermFunc(CleanUpToolbarInfo, 0);
		}

		T* pT = static_cast<T*>(this);

		CSnapInToolbarInfo* pInfo = pT->GetToolbarInfo();
		if (pInfo == NULL)
			return S_OK;

		USES_CONVERSION_EX;
		for( ; pInfo->m_idToolbar; pInfo++)
		{
			IToolbar* p = (IToolbar*) pToolbarMap->Lookup(pInfo->m_idToolbar);
			if (p != NULL)
				continue;

			HBITMAP hBitmap = LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(pInfo->m_idToolbar));
			if (hBitmap == NULL)
				return E_FAIL;

			HRSRC hRsrc = ::FindResource(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(pInfo->m_idToolbar), RT_TOOLBAR);
			if (hRsrc == NULL)
				return E_FAIL;

			HGLOBAL hGlobal = LoadResource(_AtlBaseModule.GetResourceInstance(), hRsrc);
			if (hGlobal == NULL)
				return E_FAIL;

			CSnapInToolBarData* pData = (CSnapInToolBarData*)LockResource(hGlobal);
			if (pData == NULL)
				return E_FAIL;
			ATLASSERT(pData->wVersion == 1);
			ATLASSERT(pData->wWidth == 16);
			ATLASSERT(pData->wHeight == 16);

			int nButtonCount = pData->wItemCount;
			if (pInfo->m_pnButtonID == NULL)
			{
				pInfo->m_pnButtonID = _ATL_NEW UINT[nButtonCount];
			}
			pInfo->m_nButtonCount=nButtonCount;

			if (pInfo->m_pnButtonID == NULL)
				continue;

			CAutoVectorPtr<MMCBUTTON> pButtons;
			if (!pButtons.Allocate(pData->wItemCount))
			{
				delete []pInfo->m_pnButtonID;
				pInfo->m_pnButtonID=NULL;
				continue;
			}

			if (pInfo->m_pStrToolTip == NULL)
			{
				pInfo->m_pStrToolTip = _ATL_NEW OLECHAR* [pData->wItemCount];
				if (pInfo->m_pStrToolTip)
					memset(pInfo->m_pStrToolTip, 0, sizeof(OLECHAR*) * pData->wItemCount);
			}

			if (pInfo->m_pStrToolTip == NULL)
			{
				delete []pInfo->m_pnButtonID;
				pInfo->m_pnButtonID=NULL;
				continue;
			}

			for (int i = 0, j = 0; i < pData->wItemCount; i++)
			{
				pInfo->m_pnButtonID[i] = pButtons[i].idCommand = pData->items()[i];
				if (pButtons[i].idCommand)
				{
					pButtons[i].nBitmap = j++;
					// get the statusbar string and allow modification of the button state
					TCHAR szStatusBar[512];
					LoadString(_AtlBaseModule.GetResourceInstance(), pButtons[i].idCommand, szStatusBar, 512);

					if (pInfo->m_pStrToolTip[i] == NULL)
					{
						pInfo->m_pStrToolTip[i] = _ATL_NEW OLECHAR[_tcslen(szStatusBar) + 1];
					}
					if (pInfo->m_pStrToolTip[i] == NULL)
						continue;
					if(!ocscpy_s(pInfo->m_pStrToolTip[i], (_tcslen(szStatusBar) + 1), CT2COLE(szStatusBar)))
					{
						return E_FAIL;
					}
					pButtons[i].lpTooltipText = pInfo->m_pStrToolTip[i];
					pButtons[i].lpButtonText = OLESTR("");
					pButtons[i].fsState = TBSTATE_ENABLED;
					pButtons[i].fsType = TBSTYLE_BUTTON;
					pT->SetToolbarButtonInfo(pButtons[i].idCommand, &pButtons[i].fsState, &pButtons[i].fsType);
				}
				else
				{
					pButtons[i].lpTooltipText = OLESTR("");
					pButtons[i].lpButtonText = OLESTR("");
					pButtons[i].fsType = TBSTYLE_SEP;
					pButtons[i].fsState = 0;
				}
			}

			CComPtr<IToolbar> pToolbar;
			HRESULT hr = pControlbar->Create(TOOLBAR, pExtendControlBar, reinterpret_cast<LPUNKNOWN*>(&pToolbar));
			if (SUCCEEDED(hr))
			{
				hr = pToolbar->AddBitmap(pData->wItemCount, hBitmap, pData->wWidth, pData->wHeight, RGB(192, 192, 192));
				if (SUCCEEDED(hr))
				{
					hr = pToolbar->AddButtons(pData->wItemCount, pButtons);
					if (SUCCEEDED(hr))
					{
						(*pToolbar).AddRef();
						pToolbarMap->Add(pInfo->m_idToolbar, pToolbar);
					}
				}
			}
		}
		return S_OK;
	}

	STDMETHOD(ControlbarNotify)(
		_Inout_ IControlbar *pControlbar,
		_Inout_opt_ IExtendControlbar *pExtendControlbar,
		_Inout_ CSimpleMap<UINT, IUnknown*>* pToolbarMap,
		_In_ MMC_NOTIFY_TYPE event,
		_In_ LONG_PTR arg,
		_In_ LONG_PTR param,
		_Inout_opt_ CSnapInObjectRootBase* pObj,
		_In_ DATA_OBJECT_TYPES type)
	{
		ATLTRACE(atlTraceSnapin, 2, _T("CSnapInItemImpl::ControlbarNotify\n"));
		T* pT = static_cast<T*>(this);

		SetControlbar(pControlbar, pExtendControlbar, pToolbarMap);

		if(event == MMCN_SELECT)
		{
			if (pControlbar == NULL)
				return S_OK;

			BOOL bSelect = (BOOL) HIWORD (arg);

			CSnapInToolbarInfo* pInfo = pT->GetToolbarInfo();
			if (pInfo == NULL)
				return S_OK;

			for(; pInfo->m_idToolbar; pInfo++)
			{
				IToolbar* pToolbar = (IToolbar*)pToolbarMap->Lookup(pInfo->m_idToolbar);
				if (pToolbar == NULL)
					continue;
				if (bSelect)
					pControlbar->Attach(TOOLBAR, pToolbar);
				for (UINT i = 0; i < pInfo->m_nButtonCount; i++)
				{
					if (pInfo->m_pnButtonID[i])
					{
						BOOL bEnable;
						if (bSelect)
							bEnable = pT->UpdateToolbarButton(pInfo->m_pnButtonID[i],
								ENABLED);
						else
							bEnable = FALSE;

						pToolbar->SetButtonState(pInfo->m_pnButtonID[i],
							ENABLED, bEnable
							);
						pToolbar->SetButtonState(pInfo->m_pnButtonID[i],
							CHECKED,
							pT->UpdateToolbarButton(pInfo->m_pnButtonID[i],
								CHECKED));
						pToolbar->SetButtonState(pInfo->m_pnButtonID[i],
							HIDDEN,
							pT->UpdateToolbarButton(pInfo->m_pnButtonID[i],
								HIDDEN));
						pToolbar->SetButtonState(pInfo->m_pnButtonID[i],
							INDETERMINATE,
							pT->UpdateToolbarButton(pInfo->m_pnButtonID[i],
								INDETERMINATE));
						pToolbar->SetButtonState(pInfo->m_pnButtonID[i],
							BUTTONPRESSED,
							pT->UpdateToolbarButton(pInfo->m_pnButtonID[i],
								BUTTONPRESSED));
					}
				}
			}
			return S_OK;
		}
		else if (event == MMCN_BTN_CLICK)
		{
			bool bHandled;
			return pT->ProcessCommand((UINT) param, bHandled, pObj, type);
		}

		return E_UNEXPECTED;
	}

	STDMETHOD(GetScopeData)(_Outptr_ SCOPEDATAITEM **pScopeDataItem)
	{
		if (pScopeDataItem == NULL)
			return E_FAIL;

		*pScopeDataItem = &m_scopeDataItem;
		return S_OK;
	}

	STDMETHOD(GetResultData)(_Outptr_ RESULTDATAITEM **pResultDataItem)
	{
		if (pResultDataItem == NULL)
			return E_FAIL;

		*pResultDataItem = &m_resultDataItem;
		return S_OK;
	}

	STDMETHOD(GetDataObject)(
		_COM_Outptr_ IDataObject** pDataObj,
		_In_ DATA_OBJECT_TYPES type)
	{
		ATLASSERT(pDataObj != NULL);
		if (pDataObj == NULL)
			return E_POINTER;
		*pDataObj = NULL;

		CComObject<CSnapInDataObjectImpl>* pData;
		HRESULT hr = CComObject<CSnapInDataObjectImpl>::CreateInstance(&pData);
		if (FAILED(hr))
			return hr;

		T* pT = static_cast<T*> (this);
		pData->m_objectData.m_pItem = pT;
		pData->m_objectData.m_type = type;

		hr = pData->QueryInterface(__uuidof(IDataObject), (void**)(pDataObj));
		return hr;
	}

	void UpdateMenuState(
		_In_ UINT /*id*/,
		_In_opt_z_ LPTSTR /*pBuf*/,
		_In_opt_ UINT * /*flags*/)
	{
		return;
	}

	void SetToolbarButtonInfo(
		_In_ UINT /*id*/,
		_Out_ BYTE *pfsState,
		_Out_ BYTE *pfsType)
	{
		*pfsState = TBSTATE_ENABLED;
		*pfsType = TBSTYLE_BUTTON;
	}

	BOOL UpdateToolbarButton(
		_In_ UINT /*id*/,
		_In_ BYTE fsState)
	{
		if (fsState == ENABLED)
			return TRUE;
		return FALSE;
	}

	HRESULT ProcessCommand(
		_In_ UINT nID,
		_In_ bool& /*bHandled*/,
		_In_opt_ CSnapInObjectRootBase* /*pObj*/,
		_In_ DATA_OBJECT_TYPES /*type*/)
	{
		UNREFERENCED_PARAMETER(nID);
		ATLTRACE(atlTraceSnapin, 2, _T("No handler for item with ID %d\n"), nID);
		return S_OK;
	}

	STDMETHOD (FillData)(
		_In_ CLIPFORMAT cf,
		_Inout_ LPSTREAM pStream)
	{
		HRESULT hr = DV_E_CLIPFORMAT;
		ULONG uWritten;

		T* pT = static_cast<T*> (this);

		if (cf == m_CCF_NODETYPE)
		{
			hr = pStream->Write(pT->GetNodeType(), sizeof(GUID), &uWritten);
			return hr;
		}

		if (cf == m_CCF_SZNODETYPE)
		{
			hr = pStream->Write(pT->GetSZNodeType(), (ocslen((OLECHAR*)pT->GetSZNodeType()) + 1 )* sizeof(OLECHAR), &uWritten);
			return hr;
		}

		if (cf == m_CCF_DISPLAY_NAME)
		{
			hr = pStream->Write(pT->GetDisplayName(), (ocslen((OLECHAR*)pT->GetDisplayName()) + 1) * sizeof(OLECHAR), &uWritten);
			return hr;
		}

		if (cf == m_CCF_SNAPIN_CLASSID)
		{
			hr = pStream->Write(pT->GetSnapInCLSID(), sizeof(GUID), &uWritten);
			return hr;
		}

		return hr;
	}

	static CSnapInToolbarInfo* GetToolbarInfo()
	{
		return NULL;
	}

	static void _stdcall CleanUpToolbarInfo(_In_ DWORD_PTR dw)
	{
		for (CSnapInToolbarInfo* pInfo = T::GetToolbarInfo(); pInfo->m_idToolbar != 0; pInfo++)
		{
			pInfo->CleanUp(dw);
		}
	}

	static const UINT GetMenuID()
	{
		return 0;
	}

	void SetMenuInsertionFlags(
		_In_ bool /*bBeforeInsertion*/,
		_In_opt_ long* /*pInsertionAllowed*/)
	{
	}

	void* GetNodeType()
	{
		return (void*)T::m_NODETYPE;
	}
	void* GetSZNodeType()
	{
		return (void*)T::m_SZNODETYPE;
	}

	void* GetDisplayName()
	{
		return (void*)T::m_SZDISPLAY_NAME;
	}

	void* GetSnapInCLSID()
	{
		return (void*)T::m_SNAPIN_CLASSID;
	}

	CComBSTR m_bstrDisplayName;
	SCOPEDATAITEM m_scopeDataItem;
	RESULTDATAITEM m_resultDataItem;
};

_declspec( selectany ) CLIPFORMAT CSnapInItem::m_CCF_NODETYPE = 0;
_declspec( selectany ) CLIPFORMAT CSnapInItem::m_CCF_SZNODETYPE = 0;
_declspec( selectany ) CLIPFORMAT CSnapInItem::m_CCF_DISPLAY_NAME = 0;
_declspec( selectany ) CLIPFORMAT CSnapInItem::m_CCF_SNAPIN_CLASSID = 0;
_declspec( selectany ) CLIPFORMAT CSnapInItem::m_CCF_SNAPIN_GETOBJECTDATA = 0;
_declspec( selectany ) CLIPFORMAT CSnapInItem::m_CCF_MMC_MULTISELECT_DATAOBJECT = 0;

}; //namespace ATL


#pragma pack(pop)
#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (pop)
#endif //!_ATL_NO_PRAGMA_WARNINGS

#endif //__ATLSNAP_H__
