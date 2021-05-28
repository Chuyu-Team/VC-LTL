// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLCTL_H__
#define __ATLCTL_H__

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#pragma warning(disable: 4702)  // unreachable code
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4127)  // conditional expression constant
#endif //!_ATL_NO_PRAGMA_WARNINGS

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#include <atlwin.h>

#include <ObjSafe.h>
#include <urlmon.h>

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "urlmon.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS

#pragma pack(push, _ATL_PACKING)

#define DECLARE_VIEW_STATUS(statusFlags) \
	DWORD _GetViewStatus() \
	{ \
		return statusFlags; \
	}

// Include GUIDs for the new stock property dialogs contained in the dll MSStkProp.DLL
#include "msstkppg.h"
#include "atliface.h"
#define CLSID_MSStockFont CLSID_StockFontPage
#define CLSID_MSStockColor CLSID_StockColorPage
#define CLSID_MSStockPicture CLSID_StockPicturePage

#define REFLECTOR_MAP_ID 69

struct ATL_DRAWINFO
{
	UINT cbSize;
	DWORD dwDrawAspect;
	LONG lindex;
	DVTARGETDEVICE* ptd;
	HDC hicTargetDev;
	HDC hdcDraw;
	LPCRECTL prcBounds; //Rectangle in which to draw
	LPCRECTL prcWBounds; //WindowOrg and Ext if metafile
	BOOL bOptimize;
	BOOL bZoomed;
	BOOL bRectInHimetric;
	SIZEL ZoomNum;      //ZoomX = ZoomNum.cx/ZoomNum.cy
	SIZEL ZoomDen;
};

namespace ATL
{

// Forward declarations
//
class ATL_NO_VTABLE CComControlBase;
template <class T, class WinBase> class CComControl;

//////////////////////////////////////////////////////////////////////////////
// CFirePropNotifyEvent


// Helper functions for safely communicating with objects who sink IPropertyNotifySink
class CFirePropNotifyEvent
{
public:
	// Ask any objects sinking the IPropertyNotifySink notification if it is ok to edit a specified property
	static HRESULT FireOnRequestEdit(
		_Inout_ IUnknown* pUnk,
		_In_ DISPID dispID)
	{
		CComQIPtr<IConnectionPointContainer, &__uuidof(IConnectionPointContainer)> pCPC(pUnk);
		if (!pCPC)
			return S_OK;
		CComPtr<IConnectionPoint> pCP;
		pCPC->FindConnectionPoint(__uuidof(IPropertyNotifySink), &pCP);
		if (!pCP)
			return S_OK;
		CComPtr<IEnumConnections> pEnum;

		if (FAILED(pCP->EnumConnections(&pEnum)))
			return S_OK;
		CONNECTDATA cd;
		while (pEnum->Next(1, &cd, NULL) == S_OK)
		{
			if (cd.pUnk)
			{
				HRESULT hr = S_OK;
				CComQIPtr<IPropertyNotifySink, &__uuidof(IPropertyNotifySink)> pSink(cd.pUnk);
#ifdef _DEBUG
				if (pSink == NULL)
				{
					ATLTRACE(atlTraceControls,2,_T("QI for IPropertyNotifySink failed in CFirePropNotifyEvent::FireOnRequestEdit\n"));
				}
#endif
				if (pSink != NULL)
					hr = pSink->OnRequestEdit(dispID);

				cd.pUnk->Release();
				if (hr == S_FALSE)
					return S_FALSE;
			}
		}
		return S_OK;
	}
	// Notify any objects sinking the IPropertyNotifySink notification that a property has changed
	static HRESULT FireOnChanged(
		_Inout_ IUnknown* pUnk,
		_In_ DISPID dispID)
	{
		CComQIPtr<IConnectionPointContainer, &__uuidof(IConnectionPointContainer)> pCPC(pUnk);
		if (!pCPC)
			return S_OK;
		CComPtr<IConnectionPoint> pCP;
		pCPC->FindConnectionPoint(__uuidof(IPropertyNotifySink), &pCP);
		if (!pCP)
			return S_OK;
		CComPtr<IEnumConnections> pEnum;

		if (FAILED(pCP->EnumConnections(&pEnum)))
			return S_OK;
		CONNECTDATA cd;
		while (pEnum->Next(1, &cd, NULL) == S_OK)
		{
			if (cd.pUnk)
			{
				CComQIPtr<IPropertyNotifySink, &__uuidof(IPropertyNotifySink)> pSink(cd.pUnk);
#ifdef _DEBUG
				if (pSink == NULL)
				{
					ATLTRACE(atlTraceControls,2,_T("QI for IPropertyNotifySink failed in CFirePropNotifyEvent::FireOnChanged\n"));
				}
#endif
				if (pSink != NULL)
					pSink->OnChanged(dispID);
				cd.pUnk->Release();
			}
		}
		return S_OK;
	}
};


//////////////////////////////////////////////////////////////////////////////
// CComControlBase

#pragma warning(push)
#pragma warning(disable:4512)

// Holds the essential data members for an ActiveX control and useful helper functions
class ATL_NO_VTABLE CComControlBase
{
public:
	typedef short AppearanceType;  // Override in derived class if your
		// m_nAppearance stock property isn't of type 'short'
public:
	CComControlBase(_Inout_ HWND& h) : m_hWndCD(h)
	{
		m_sizeExtent.cx = 2*2540;
		m_sizeExtent.cy = 2*2540;
		m_sizeNatural = m_sizeExtent;
		m_rcPos.left = 0;
		m_rcPos.right = 0;
		m_rcPos.top = 0;
		m_rcPos.bottom = 0;
		m_nFreezeEvents = 0;
		m_bNegotiatedWnd = 0;
		m_bWndLess = 0;
		m_bInPlaceActive = 0;
		m_bUIActive = 0;
		m_bUsingWindowRgn = 0;
		m_bInPlaceSiteEx = 0;
		m_bWindowOnly = 0;
		m_bRequiresSave = 0;
		m_bWasOnceWindowless = 0;
		m_bAutoSize = 0;
		m_bRecomposeOnResize = 0;
		m_bResizeNatural = 0;
		m_bDrawFromNatural = 0;
		m_bDrawGetDataInHimetric = 0;
	}
	virtual ~CComControlBase()
	{
		if (m_hWndCD != NULL)
			::DestroyWindow(m_hWndCD);
		ATLTRACE(atlTraceControls,2,_T("Control Destroyed\n"));
	}

// methods
public:
	// Control helper functions can go here non-virtuals only please

	// Mark the control 'dirty' so the container will save it
	void SetDirty(_In_ BOOL bDirty)
	{
		m_bRequiresSave = bDirty;
	}
	// Obtain the dirty state for the control
	BOOL GetDirty()
	{
		return m_bRequiresSave;
	}
	// Get the zoom factor (numerator & denominator) which is factor of the natural extent
	void GetZoomInfo(_Inout_ ATL_DRAWINFO& di);
	// Sends a notification that the moniker for the control has changed
	HRESULT SendOnRename(_Inout_ IMoniker *pmk)
	{
		HRESULT hRes = S_OK;
		if (m_spOleAdviseHolder)
			hRes = m_spOleAdviseHolder->SendOnRename(pmk);
		return hRes;
	}
	// Sends a notification that the control has just saved its data
	HRESULT SendOnSave()
	{
		HRESULT hRes = S_OK;
		if (m_spOleAdviseHolder)
			hRes = m_spOleAdviseHolder->SendOnSave();
		return hRes;
	}
	// Sends a notification that the control has closed its advisory sinks
	HRESULT SendOnClose()
	{
		HRESULT hRes = S_OK;
		if (m_spOleAdviseHolder)
			hRes = m_spOleAdviseHolder->SendOnClose();
		return hRes;
	}
	// Sends a notification that the control's data has changed
	HRESULT SendOnDataChange(_In_ DWORD advf = 0);
	// Sends a notification that the control's representation has changed
	HRESULT SendOnViewChange(_In_ DWORD dwAspect, _In_ LONG lindex = -1)
	{
		if (m_spAdviseSink)
			m_spAdviseSink->OnViewChange(dwAspect, lindex);
		return S_OK;
	}
	// Sends a notification to the container that the control has received focus
	LRESULT OnSetFocus(
		_In_ UINT /*uMsg*/,
		_In_ WPARAM /*wParam*/,
		_In_ LPARAM /*lParam*/,
		_Out_ BOOL& bHandled)
	{
		if (m_bInPlaceActive)
		{
			CComPtr<IOleObject> pOleObject;
			ControlQueryInterface(__uuidof(IOleObject), (void**)&pOleObject);
			if (pOleObject != NULL)
				pOleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, m_spClientSite, 0, m_hWndCD, &m_rcPos);
			CComQIPtr<IOleControlSite, &__uuidof(IOleControlSite)> spSite(m_spClientSite);
			if (m_bInPlaceActive && spSite != NULL)
				spSite->OnFocus(TRUE);
		}
		bHandled = FALSE;
		return 1;
	}
	LRESULT OnKillFocus(
		_In_ UINT /*uMsg*/,
		_In_ WPARAM /*wParam*/,
		_In_ LPARAM /*lParam*/,
		_Out_ BOOL& bHandled)
	{
		CComQIPtr<IOleControlSite, &__uuidof(IOleControlSite)> spSite(m_spClientSite);
		if (m_bInPlaceActive && spSite != NULL && !::IsChild(m_hWndCD, ::GetFocus()))
			spSite->OnFocus(FALSE);
		bHandled = FALSE;
		return 1;
	}
	LRESULT OnMouseActivate(
		_In_ UINT /*uMsg*/,
		_In_ WPARAM /*wParam*/,
		_In_ LPARAM /*lParam*/,
		_Out_ BOOL& bHandled)
	{
		BOOL bUserMode = TRUE;
		HRESULT hRet = GetAmbientUserMode(bUserMode);
		// UI activate if in user mode only
		// allow activation if we can't determine mode
		if (FAILED(hRet) || bUserMode)
		{
			CComPtr<IOleObject> pOleObject;
			ControlQueryInterface(__uuidof(IOleObject), (void**)&pOleObject);
			if (pOleObject != NULL)
				pOleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, m_spClientSite, 0, m_hWndCD, &m_rcPos);
		}
		bHandled = FALSE;
		return 1;
	}
	BOOL PreTranslateAccelerator(
		_In_opt_ LPMSG /*pMsg*/,
		_In_ HRESULT& /*hRet*/)
	{
		return FALSE;
	}

	HRESULT GetAmbientProperty(
		_In_ DISPID dispid,
		_Out_ VARIANT& var)
	{
		HRESULT hRes = E_FAIL;
		if (m_spAmbientDispatch.p != NULL)
			hRes = m_spAmbientDispatch.GetProperty(dispid, &var);
		return hRes;
	}
	HRESULT GetAmbientAppearance(_Out_ short& nAppearance)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_APPEARANCE, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_I2 || var.vt == VT_UI2 || var.vt == VT_I4 || var.vt == VT_UI4);
		if (SUCCEEDED(hRes))
		{
			if (var.vt == VT_EMPTY)
				hRes = E_FAIL;
			else
			{
				nAppearance = var.iVal;
			}
		}
		return hRes;
	}
	HRESULT GetAmbientBackColor(_Out_ OLE_COLOR& BackColor)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_BACKCOLOR, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_I4 || var.vt == VT_UI4);
		if (SUCCEEDED(hRes))
			BackColor = var.lVal;
		return hRes;
	}
	HRESULT GetAmbientDisplayName(_Inout_ _Outref_result_maybenull_ _Post_z_ BSTR& bstrDisplayName)
	{
		CComVariant var;

		if (bstrDisplayName)
		{
			SysFreeString(bstrDisplayName);
			bstrDisplayName = NULL;
		}

		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_DISPLAYNAME, var);
		if (SUCCEEDED(hRes))
		{
			if (var.vt != VT_BSTR)
				return E_FAIL;
			bstrDisplayName = var.bstrVal;
			var.vt = VT_EMPTY;
			var.bstrVal = NULL;
		}
		return hRes;
	}
ATLPREFAST_SUPPRESS(6387)
	HRESULT GetAmbientFont(_Outptr_result_maybenull_ IFont** ppFont)
	{
		// caller MUST Release the font!
		if (ppFont == NULL)
			return E_POINTER;
		*ppFont = NULL;
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_FONT, var);
		ATLASSERT(FAILED(hRes) || (var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH));
		if (SUCCEEDED(hRes) && var.pdispVal)
		{
			if (var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH)
				hRes = var.pdispVal->QueryInterface(__uuidof(IFont), (void**)ppFont);
			else
				hRes = DISP_E_BADVARTYPE;
		}
		return hRes;
	}
ATLPREFAST_UNSUPPRESS()

ATLPREFAST_SUPPRESS(6387)
	HRESULT GetAmbientFontDisp(_Outptr_result_maybenull_ IFontDisp** ppFont)
	{
		// caller MUST Release the font!
		if (ppFont == NULL)
			return E_POINTER;
		*ppFont = NULL;
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_FONT, var);
		ATLASSERT(FAILED(hRes) || (var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH));
		if (SUCCEEDED(hRes) && var.pdispVal)
		{
			if (var.vt == VT_UNKNOWN || var.vt == VT_DISPATCH)
				hRes = var.pdispVal->QueryInterface(__uuidof(IFontDisp), (void**)ppFont);
			else
				hRes = DISP_E_BADVARTYPE;
		}
		return hRes;
	}
ATLPREFAST_UNSUPPRESS()

	HRESULT GetAmbientForeColor(_Out_ OLE_COLOR& ForeColor)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_FORECOLOR, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_I4 || var.vt == VT_UI4);
		if (SUCCEEDED(hRes))
			ForeColor = var.lVal;
		return hRes;
	}
	HRESULT GetAmbientLocaleID(_Out_ LCID& lcid)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_LOCALEID, var);
		ATLASSERT(FAILED(hRes) || (var.vt == VT_UI4 || var.vt == VT_I4));
		if (SUCCEEDED(hRes))
			lcid = var.lVal;
		return hRes;
	}
	HRESULT GetAmbientScaleUnits(_Inout_ _Outref_result_maybenull_ _Post_z_ BSTR& bstrScaleUnits)
	{
		CComVariant var;

		if (bstrScaleUnits)
		{
			SysFreeString(bstrScaleUnits);
			bstrScaleUnits = NULL;
		}

		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_SCALEUNITS, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_BSTR);
		if (SUCCEEDED(hRes))
		{
			if (var.vt != VT_BSTR)
				return E_FAIL;
			bstrScaleUnits = var.bstrVal;
			var.vt = VT_EMPTY;
			var.bstrVal = NULL;
		}
		return hRes;
	}
	HRESULT GetAmbientTextAlign(_Out_ short& nTextAlign)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_TEXTALIGN, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_I2);
		if (SUCCEEDED(hRes))
		{
			if (var.vt == VT_EMPTY)
				hRes = E_FAIL;
			else
				nTextAlign = var.iVal;
		}
		return hRes;
	}
	HRESULT GetAmbientUserMode(_Out_ BOOL& bUserMode)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_USERMODE, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bUserMode = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientUIDead(_Out_ BOOL& bUIDead)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_UIDEAD, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bUIDead = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientShowGrabHandles(_Out_ BOOL& bShowGrabHandles)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_SHOWGRABHANDLES, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bShowGrabHandles = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientShowHatching(_Out_ BOOL& bShowHatching)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_SHOWHATCHING, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bShowHatching = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientMessageReflect(_Out_ BOOL& bMessageReflect)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_MESSAGEREFLECT, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bMessageReflect = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientAutoClip(_Out_ BOOL& bAutoClip)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_AUTOCLIP, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bAutoClip = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientDisplayAsDefault(_Out_ BOOL& bDisplaysDefault)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_DISPLAYASDEFAULT, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bDisplaysDefault = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientSupportsMnemonics(_Out_ BOOL& bSupportMnemonics)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_SUPPORTSMNEMONICS, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bSupportMnemonics = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}
	HRESULT GetAmbientPalette(_Out_ HPALETTE& hPalette)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_PALETTE, var);
#ifdef _WIN64
		ATLASSERT(FAILED(hRes) || var.vt == VT_I8 || var.vt == VT_UI8);
		if (SUCCEEDED(hRes))
			hPalette = reinterpret_cast<HPALETTE>(static_cast<LONG_PTR>(var.llVal));
#else
		ATLASSERT(FAILED(hRes) || var.vt == VT_I4 || var.vt == VT_UI4);
		if (SUCCEEDED(hRes))
			hPalette = reinterpret_cast<HPALETTE>(static_cast<LONG_PTR>(var.lVal));
#endif
		return hRes;
	}

	HRESULT GetAmbientCodePage(_Out_ ULONG& ulCodePage)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_CODEPAGE, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_UI4);
		if (SUCCEEDED(hRes))
			ulCodePage = var.ulVal;
		return hRes;
	}

	HRESULT GetAmbientCharSet(_Inout_ _Outref_result_maybenull_ _Post_z_ BSTR& bstrCharSet)
	{
		CComVariant var;

		if (bstrCharSet)
		{
			SysFreeString(bstrCharSet);
			bstrCharSet = NULL;
		}

		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_CHARSET, var);
		ATLASSERT(FAILED(hRes) || var.vt == VT_BSTR);
		if (SUCCEEDED(hRes))
		{
			if (var.vt != VT_BSTR)
				return E_FAIL;
			bstrCharSet = var.bstrVal;
			var.vt = VT_EMPTY;
			var.bstrVal = NULL;
		}
		return hRes;
	}

	HRESULT GetAmbientRightToLeft(_Out_ BOOL& bRightToLeft)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_RIGHTTOLEFT, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bRightToLeft = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}

	HRESULT GetAmbientTopToBottom(_Out_ BOOL& bTopToBottom)
	{
		CComVariant var;
		HRESULT hRes = GetAmbientProperty(DISPID_AMBIENT_TOPTOBOTTOM, var);
		if(SUCCEEDED(hRes))
		{
			ATLASSERT(var.vt == VT_BOOL);
			if(var.vt != VT_BOOL)
				return DISP_E_TYPEMISMATCH;
			bTopToBottom = (var.boolVal != ATL_VARIANT_FALSE) ? TRUE : FALSE;
		}
		return hRes;
	}

ATLPREFAST_SUPPRESS(6387)
	HRESULT InternalGetSite(
		_In_ REFIID riid,
		_Outptr_ void** ppUnkSite)
	{
		ATLASSERT(ppUnkSite != NULL);
		if (ppUnkSite == NULL)
			return E_POINTER;
		if (m_spClientSite == NULL)
		{
			*ppUnkSite = NULL;
			return E_FAIL;
		}
		return m_spClientSite->QueryInterface(riid, ppUnkSite);
	}
ATLPREFAST_UNSUPPRESS()

	BOOL DoesVerbUIActivate(_In_ LONG iVerb)
	{
		BOOL b = FALSE;
		switch (iVerb)
		{
			case OLEIVERB_UIACTIVATE:
			case OLEIVERB_PRIMARY:
				b = TRUE;
				break;
		}
		// if no ambient dispatch then in old style OLE container
		if (DoesVerbActivate(iVerb) && m_spAmbientDispatch.p == NULL)
			b = TRUE;
		return b;
	}

	BOOL DoesVerbActivate(_In_ LONG iVerb)
	{
		BOOL b = FALSE;
		switch (iVerb)
		{
			case OLEIVERB_UIACTIVATE:
			case OLEIVERB_PRIMARY:
			case OLEIVERB_SHOW:
			case OLEIVERB_INPLACEACTIVATE:
				b = TRUE;
				break;
		}
		return b;
	}

	BOOL SetControlFocus(_In_ BOOL bGrab);
	HRESULT IQuickActivate_QuickActivate(
		_In_ QACONTAINER *pQACont,
		_Inout_ QACONTROL *pQACtrl);
	HRESULT DoVerbProperties(
		_In_opt_ LPCRECT /* prcPosRect */,
		_In_ HWND hwndParent);
	HRESULT InPlaceActivate(
		_In_ LONG iVerb,
		_In_opt_ const RECT* prcPosRect = NULL);

	HRESULT IOleObject_SetClientSite(_Inout_opt_ IOleClientSite *pClientSite);
	HRESULT IOleObject_GetClientSite(_Outptr_result_maybenull_ IOleClientSite **ppClientSite);
	HRESULT IOleObject_Advise(
		_Inout_ IAdviseSink *pAdvSink,
		_Out_ DWORD *pdwConnection);
	HRESULT IOleObject_Close(_In_ DWORD dwSaveOption);
	HRESULT IOleObject_SetExtent(
		_In_ DWORD dwDrawAspect,
		_In_reads_bytes_(sizeof(SIZE)) SIZEL *psizel);
	HRESULT IOleInPlaceObject_InPlaceDeactivate(void);
	HRESULT IOleInPlaceObject_UIDeactivate(void);
	HRESULT IOleInPlaceObject_SetObjectRects(
		_In_ LPCRECT prcPos,
		_In_ LPCRECT prcClip);
	HRESULT IViewObject_Draw(
		_In_ DWORD dwDrawAspect,
		_In_ LONG lindex,
		_In_opt_ void *pvAspect,
		_In_opt_ DVTARGETDEVICE *ptd,
		_In_ HDC hicTargetDev,
		_In_ HDC hdcDraw,
		_In_opt_ LPCRECTL prcBounds,
		_In_opt_ LPCRECTL prcWBounds);
	HRESULT IDataObject_GetData(
		_In_ FORMATETC *pformatetcIn,
		_Out_ STGMEDIUM *pmedium);

	HRESULT FireViewChange();
	LRESULT OnPaint(
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_In_ BOOL& lResult);

	virtual HWND CreateControlWindow(
		_In_ HWND hWndParent,
		_In_ RECT& rcPos) = 0;
	virtual HRESULT ControlQueryInterface(
		_In_ const IID& iid,
		_Outptr_ void** ppv) = 0;
	virtual HRESULT OnDrawAdvanced(_Inout_ ATL_DRAWINFO& di);
	virtual HRESULT OnDraw(_In_ ATL_DRAWINFO& di)
	{
		::SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL ") _T(_ATL_VER_RBLD);
		::TextOut(di.hdcDraw, di.prcBounds->left + (di.prcBounds->right - di.prcBounds->left) / 2, di.prcBounds->top + (di.prcBounds->bottom - di.prcBounds->top) / 2, pszText, static_cast<int>(_tcslen(pszText)));

		return S_OK;
	}

// Attributes
public:
	CComPtr<IOleInPlaceSiteWindowless> m_spInPlaceSite;
	CComPtr<IDataAdviseHolder> m_spDataAdviseHolder;
	CComPtr<IOleAdviseHolder> m_spOleAdviseHolder;
	CComPtr<IOleClientSite> m_spClientSite;
	CComPtr<IAdviseSink> m_spAdviseSink;
	CComDispatchDriver m_spAmbientDispatch;

	SIZE m_sizeNatural; //unscaled size in himetric
	SIZE m_sizeExtent;  //current extents in himetric
	RECT m_rcPos; // position in pixels
	HWND& m_hWndCD;
	int m_nFreezeEvents; // count of freezes versus thaws
	unsigned m_bNegotiatedWnd:1;
	unsigned m_bWndLess:1;
	unsigned m_bInPlaceActive:1;
	unsigned m_bUIActive:1;
	unsigned m_bUsingWindowRgn:1;
	unsigned m_bInPlaceSiteEx:1;
	unsigned m_bWindowOnly:1;
	unsigned m_bRequiresSave:1;
	unsigned m_bWasOnceWindowless:1;
	unsigned m_bAutoSize:1; //SetExtent fails if size doesn't match existing
	unsigned m_bRecomposeOnResize:1; //implies OLEMISC_RECOMPOSEONRESIZE
	unsigned m_bResizeNatural:1;  //resize natural extent on SetExtent
	unsigned m_bDrawFromNatural:1; //instead of m_sizeExtent
	unsigned m_bDrawGetDataInHimetric:1; //instead of pixels

	DECLARE_VIEW_STATUS(VIEWSTATUS_OPAQUE)
};

#pragma warning(pop)

inline HRESULT CComControlBase::IQuickActivate_QuickActivate(
	_In_ QACONTAINER *pQACont,
	_Inout_ QACONTROL *pQACtrl)
{
	ATLASSERT(pQACont != NULL);
	ATLASSERT(pQACtrl != NULL);
	if (!pQACont || !pQACtrl)
		return E_POINTER;

	HRESULT hRes;
	ULONG uCB = pQACtrl->cbSize;
	memset(pQACtrl, 0, uCB);
	pQACtrl->cbSize = uCB;

	// get all interfaces we are going to need
	CComPtr<IOleObject> pOO;
	ControlQueryInterface(__uuidof(IOleObject), (void**)&pOO);
	CComPtr<IViewObjectEx> pVOEX;
	ControlQueryInterface(__uuidof(IViewObjectEx), (void**)&pVOEX);
	CComPtr<IPointerInactive> pPI;
	ControlQueryInterface(__uuidof(IPointerInactive), (void**)&pPI);
	CComPtr<IProvideClassInfo2> pPCI;
	ControlQueryInterface(__uuidof(IProvideClassInfo2), (void**)&pPCI);

	if (pOO == NULL || pVOEX == NULL)
		return E_FAIL;

	pOO->SetClientSite(pQACont->pClientSite);

	if (pQACont->pAdviseSink != NULL)
	{
		ATLTRACE(atlTraceControls,2,_T("Setting up IOleObject Advise\n"));
		pVOEX->SetAdvise(DVASPECT_CONTENT, 0, pQACont->pAdviseSink);
	}

	CComPtr<IConnectionPointContainer> pCPC;
	ControlQueryInterface(__uuidof(IConnectionPointContainer), (void**)&pCPC);

	if (pQACont->pPropertyNotifySink)
	{
		ATLTRACE(atlTraceControls,2,_T("Setting up PropNotify CP\n"));
		CComPtr<IConnectionPoint> pCP;
		if (pCPC != NULL)
		{
			hRes = pCPC->FindConnectionPoint(__uuidof(IPropertyNotifySink), &pCP);
			if (SUCCEEDED(hRes))
				pCP->Advise(pQACont->pPropertyNotifySink, &pQACtrl->dwPropNotifyCookie);
		}
	}

	if (pPCI)
	{
		GUID iidDefaultSrc;
		if (SUCCEEDED(pPCI->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID,
			&iidDefaultSrc)))
		{
			if (pQACont->pUnkEventSink)
			{
				ATLTRACE(atlTraceControls,2,_T("Setting up Default Out Going Interface\n"));
				CComPtr<IConnectionPoint> pCP;
				if (pCPC != NULL)
				{
					hRes = pCPC->FindConnectionPoint(iidDefaultSrc, &pCP);
					if (SUCCEEDED(hRes))
						pCP->Advise(pQACont->pUnkEventSink, &pQACtrl->dwEventCookie);
				}
			}
		}
	}
	// give information to container
	if (pOO != NULL)
		pOO->GetMiscStatus(DVASPECT_CONTENT, &pQACtrl->dwMiscStatus);

	if (pVOEX != NULL)
		pVOEX->GetViewStatus(&pQACtrl->dwViewStatus);

	if (pPI != NULL)
		pPI->GetActivationPolicy(&pQACtrl->dwPointerActivationPolicy);
	return S_OK;
}

inline BOOL CComControlBase::SetControlFocus(_In_ BOOL bGrab)
{
	if (m_bWndLess)
	{
		if (!m_bUIActive && bGrab)
			if (FAILED(InPlaceActivate(OLEIVERB_UIACTIVATE)))
				return FALSE;

		return (m_spInPlaceSite->SetFocus(bGrab) == S_OK);
	}
	else
	{
		// we've got a window.
		//
		if (m_bInPlaceActive)
		{
			HWND hwnd = (bGrab) ? m_hWndCD : ::GetParent(m_hWndCD);
			if (!m_bUIActive && bGrab)
				return SUCCEEDED(InPlaceActivate(OLEIVERB_UIACTIVATE));
			else
			{
				if (!::IsChild(hwnd, ::GetFocus()))
					::SetFocus(hwnd);
				return TRUE;
			}
		}
	}
	return FALSE;
}

inline HRESULT CComControlBase::DoVerbProperties(
	_In_opt_ LPCRECT /* prcPosRect */,
	_In_ HWND hwndParent)
{
	HRESULT hr = S_OK;
	CComQIPtr <ISpecifyPropertyPages, &__uuidof(ISpecifyPropertyPages)> spPages;
	CComQIPtr <IOleObject, &__uuidof(IOleObject)> spObj;
	CComQIPtr <IOleControlSite, &__uuidof(IOleControlSite)> spSite(m_spClientSite);

	if (spSite)
	{
		hr = spSite->ShowPropertyFrame();
		if (SUCCEEDED(hr))
			return hr;
	}

	CComPtr<IUnknown> pUnk;
	ControlQueryInterface(__uuidof(IUnknown), (void**)&pUnk);
	ATLASSERT(pUnk != NULL);
	CAUUID pages;
	spPages = pUnk;
	if (spPages)
	{
		hr = spPages->GetPages(&pages);
		if (SUCCEEDED(hr))
		{
			spObj = pUnk;
			if (spObj)
			{
				LPOLESTR szTitle = NULL;

				hr = spObj->GetUserType(USERCLASSTYPE_SHORT, &szTitle);
				_Analysis_assume_(SUCCEEDED(hr) || (szTitle = NULL) == NULL);

				LCID lcid;
				if (FAILED(GetAmbientLocaleID(lcid)))
					lcid = LOCALE_USER_DEFAULT;

				hr = OleCreatePropertyFrame(hwndParent, m_rcPos.top, m_rcPos.left, szTitle,
					1, &pUnk.p, pages.cElems, pages.pElems, lcid, 0, 0);

				CoTaskMemFree(szTitle);
			}
			else
			{
				hr = OLEOBJ_S_CANNOT_DOVERB_NOW;
			}
			CoTaskMemFree(pages.pElems);
		}
	}
	else
	{
		hr = OLEOBJ_S_CANNOT_DOVERB_NOW;
	}

	return hr;
}

inline HRESULT CComControlBase::InPlaceActivate(
	_In_ LONG iVerb,
	_In_opt_ const RECT* /*prcPosRect*/)
{
	HRESULT hr;

	if (m_spClientSite == NULL)
		return S_OK;

	CComPtr<IOleInPlaceObject> pIPO;
	ControlQueryInterface(__uuidof(IOleInPlaceObject), (void**)&pIPO);
	ATLASSERT(pIPO != NULL);

	if (!m_bNegotiatedWnd)
	{
		if (!m_bWindowOnly)
			// Try for windowless site
			hr = m_spClientSite->QueryInterface(__uuidof(IOleInPlaceSiteWindowless), (void **)&m_spInPlaceSite);

		if (m_spInPlaceSite)
		{
			m_bInPlaceSiteEx = TRUE;
			// CanWindowlessActivate returns S_OK or S_FALSE
			if ( m_spInPlaceSite->CanWindowlessActivate() == S_OK )
			{
				m_bWndLess = TRUE;
				m_bWasOnceWindowless = TRUE;
			}
			else
			{
				m_bWndLess = FALSE;
			}
		}
		else
		{
			m_spClientSite->QueryInterface(__uuidof(IOleInPlaceSiteEx), (void **)&m_spInPlaceSite);
			if (m_spInPlaceSite)
				m_bInPlaceSiteEx = TRUE;
			else
				hr = m_spClientSite->QueryInterface(__uuidof(IOleInPlaceSite), (void **)&m_spInPlaceSite);
		}
	}

	ATLASSUME(m_spInPlaceSite);
	if (!m_spInPlaceSite)
		return E_FAIL;

	m_bNegotiatedWnd = TRUE;

	if (!m_bInPlaceActive)
	{

		BOOL bNoRedraw = FALSE;
		if (m_bWndLess)
			m_spInPlaceSite->OnInPlaceActivateEx(&bNoRedraw, ACTIVATE_WINDOWLESS);
		else
		{
			if (m_bInPlaceSiteEx)
				m_spInPlaceSite->OnInPlaceActivateEx(&bNoRedraw, 0);
			else
			{
				hr = m_spInPlaceSite->CanInPlaceActivate();
				// CanInPlaceActivate returns S_FALSE or S_OK
				if (FAILED(hr))
					return hr;
				if ( hr != S_OK )
				{
				   // CanInPlaceActivate returned S_FALSE.
				   return( E_FAIL );
				}
				m_spInPlaceSite->OnInPlaceActivate();
			}
		}
	}

	m_bInPlaceActive = TRUE;

	// get location in the parent window,
	// as well as some information about the parent
	//
	OLEINPLACEFRAMEINFO frameInfo;
	RECT rcPos, rcClip;
	CComPtr<IOleInPlaceFrame> spInPlaceFrame;
	CComPtr<IOleInPlaceUIWindow> spInPlaceUIWindow;
	frameInfo.cb = sizeof(OLEINPLACEFRAMEINFO);
	HWND hwndParent;
	if (m_spInPlaceSite->GetWindow(&hwndParent) == S_OK)
	{
		hr = m_spInPlaceSite->GetWindowContext(&spInPlaceFrame,
			&spInPlaceUIWindow, &rcPos, &rcClip, &frameInfo);
		if (FAILED(hr))
			return hr;

		if (!m_bWndLess)
		{
			if (m_hWndCD)
				ShowWindow(m_hWndCD, SW_SHOW);
			else
			{
				HWND h = CreateControlWindow(hwndParent, rcPos);
				ATLASSERT(h != NULL);	// will assert if creation failed
				ATLASSERT(h == m_hWndCD);
				if(h == NULL)
					return E_FAIL;
			}
		}

		pIPO->SetObjectRects(&rcPos, &rcClip);
	}

	CComPtr<IOleInPlaceActiveObject> spActiveObject;
	ControlQueryInterface(__uuidof(IOleInPlaceActiveObject), (void**)&spActiveObject);

	// Gone active by now, take care of UIACTIVATE
	if (DoesVerbUIActivate(iVerb))
	{
		if (!m_bUIActive)
		{
			m_bUIActive = TRUE;
			hr = m_spInPlaceSite->OnUIActivate();
			if (FAILED(hr))
			{
				m_bUIActive = FALSE;
				return hr;
			}

			SetControlFocus(TRUE);
			// set ourselves up in the host.
			//
			if (spActiveObject)
			{
				if (spInPlaceFrame)
					spInPlaceFrame->SetActiveObject(spActiveObject, NULL);
				if (spInPlaceUIWindow)
					spInPlaceUIWindow->SetActiveObject(spActiveObject, NULL);
			}

			if (spInPlaceFrame)
				spInPlaceFrame->SetBorderSpace(NULL);
			if (spInPlaceUIWindow)
				spInPlaceUIWindow->SetBorderSpace(NULL);
		}
	}

	m_spClientSite->ShowObject();

	return S_OK;
}

inline HRESULT CComControlBase::SendOnDataChange(_In_ DWORD advf)
{
	HRESULT hRes = S_OK;
	if (m_spDataAdviseHolder)
	{
		CComPtr<IDataObject> pdo;
		if (SUCCEEDED(ControlQueryInterface(__uuidof(IDataObject), (void**)&pdo)))
			hRes = m_spDataAdviseHolder->SendOnDataChange(pdo, 0, advf);
	}
	return hRes;
}

inline HRESULT CComControlBase::IOleObject_SetClientSite(_Inout_opt_ IOleClientSite *pClientSite)
{
	ATLASSERT(pClientSite == NULL || m_spClientSite == NULL);
	m_spClientSite = pClientSite;
	m_spAmbientDispatch.Release();
	if (m_spClientSite != NULL)
	{
		m_spClientSite->QueryInterface(__uuidof(IDispatch),
			(void**) &m_spAmbientDispatch.p);
	}
	return S_OK;
}

inline HRESULT CComControlBase::IOleObject_GetClientSite(_Outptr_result_maybenull_ IOleClientSite **ppClientSite)
{
	ATLASSERT(ppClientSite);
	if (ppClientSite == NULL)
		return E_POINTER;

	*ppClientSite = m_spClientSite;
	if (m_spClientSite != NULL)
		m_spClientSite.p->AddRef();
	return S_OK;
}

inline HRESULT CComControlBase::IOleObject_Advise(
	_Inout_ IAdviseSink *pAdvSink,
	_Out_ DWORD *pdwConnection)
{
	HRESULT hr = S_OK;
	if (m_spOleAdviseHolder == NULL)
		hr = CreateOleAdviseHolder(&m_spOleAdviseHolder);
	if (SUCCEEDED(hr))
		hr = m_spOleAdviseHolder->Advise(pAdvSink, pdwConnection);
	return hr;
}

inline HRESULT CComControlBase::IOleObject_Close(_In_ DWORD dwSaveOption)
{
	if (m_hWndCD)
	{
		if (m_spClientSite)
			m_spClientSite->OnShowWindow(FALSE);
	}

	if (m_bInPlaceActive)
	{
		CComPtr<IOleInPlaceObject> pIPO;
		ControlQueryInterface(__uuidof(IOleInPlaceObject), (void**)&pIPO);
		ATLASSERT(pIPO != NULL);
		HRESULT hr = pIPO->InPlaceDeactivate();
		if (FAILED(hr))
			return hr;
		ATLASSERT(!m_bInPlaceActive);
	}
	if (m_hWndCD)
	{
		ATLTRACE(atlTraceControls,2,_T("Destroying Window\n"));
		if (::IsWindow(m_hWndCD))
			DestroyWindow(m_hWndCD);
		m_hWndCD = NULL;
	}

	// handle the save flag.
	//
	if ((dwSaveOption == OLECLOSE_SAVEIFDIRTY ||
		dwSaveOption == OLECLOSE_PROMPTSAVE) && m_bRequiresSave)
	{
		if (m_spClientSite)
			m_spClientSite->SaveObject();
		SendOnSave();
	}

	m_spInPlaceSite.Release();
	m_bNegotiatedWnd = FALSE;
	m_bWndLess = FALSE;
	m_bInPlaceSiteEx = FALSE;
	m_spAdviseSink.Release();
	return S_OK;
}

inline HRESULT CComControlBase::IOleInPlaceObject_InPlaceDeactivate(void)
{
	if (!m_bInPlaceActive)
		return S_OK;

	if(m_bUIActive) {
		CComPtr<IOleInPlaceObject> pIPO;
		ControlQueryInterface(__uuidof(IOleInPlaceObject), (void**)&pIPO);
		ATLENSURE(pIPO != NULL);
		pIPO->UIDeactivate();
	}

	m_bInPlaceActive = FALSE;

	// if we have a window, tell it to go away.
	//
	if (m_hWndCD)
	{
		ATLTRACE(atlTraceControls,2,_T("Destroying Window\n"));
		if (::IsWindow(m_hWndCD))
			DestroyWindow(m_hWndCD);
		m_hWndCD = NULL;
	}

	if (m_spInPlaceSite)
		m_spInPlaceSite->OnInPlaceDeactivate();

	return S_OK;
}

inline HRESULT CComControlBase::IOleInPlaceObject_UIDeactivate(void)
{
	// if we're not UIActive, not much to do.
	if (!m_bUIActive)
		return S_OK;

	m_bUIActive = FALSE;

	HWND hwndParent;
	// This call to GetWindow is a fix for Delphi
	if (m_spInPlaceSite)
	{
		if (m_spInPlaceSite->GetWindow(&hwndParent) == S_OK)
		{
			// notify frame windows, if appropriate, that we're no longer ui-active.
			CComPtr<IOleInPlaceFrame> spInPlaceFrame;
			CComPtr<IOleInPlaceUIWindow> spInPlaceUIWindow;
			OLEINPLACEFRAMEINFO frameInfo;
			frameInfo.cb = sizeof(OLEINPLACEFRAMEINFO);
			RECT rcPos, rcClip;

			m_spInPlaceSite->GetWindowContext(&spInPlaceFrame,
				&spInPlaceUIWindow, &rcPos, &rcClip, &frameInfo);
			if (spInPlaceUIWindow)
				spInPlaceUIWindow->SetActiveObject(NULL, NULL);
			if (spInPlaceFrame)
				spInPlaceFrame->SetActiveObject(NULL, NULL);
		}
		// we don't need to explicitly release the focus here since somebody
		// else grabbing the focus is what is likely to cause us to get lose it
		m_spInPlaceSite->OnUIDeactivate(FALSE);
	}
	return S_OK;
}

inline HRESULT CComControlBase::IOleInPlaceObject_SetObjectRects(
	_In_ LPCRECT prcPos,
	_In_ LPCRECT prcClip)
{
	if (prcPos == NULL || prcClip == NULL)
		return E_POINTER;

	m_rcPos = *prcPos;
	if (m_hWndCD)
	{
		// the container wants us to clip, so figure out if we really
		// need to
		//
		RECT rcIXect;
		BOOL b = IntersectRect(&rcIXect, prcPos, prcClip);
		HRGN tempRgn = NULL;
		if (b && !EqualRect(&rcIXect, prcPos))
		{
			OffsetRect(&rcIXect, -(prcPos->left), -(prcPos->top));
			tempRgn = CreateRectRgnIndirect(&rcIXect);
		}

		SetWindowRgn(m_hWndCD, tempRgn, TRUE);

		// set our control's location, but don't change it's size at all
		// [people for whom zooming is important should set that up here]
		//
		SIZEL size = {prcPos->right - prcPos->left, prcPos->bottom - prcPos->top};
		SetWindowPos(m_hWndCD, NULL, prcPos->left,
					 prcPos->top, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	return S_OK;
}

inline HRESULT CComControlBase::IOleObject_SetExtent(
	_In_ DWORD dwDrawAspect,
	_In_reads_bytes_(sizeof(SIZE)) SIZEL *psizel)
{
	if (dwDrawAspect != DVASPECT_CONTENT)
		return DV_E_DVASPECT;
	if (psizel == NULL)
		return E_POINTER;

	BOOL bSizeMatchesNatural =
		memcmp(psizel, &m_sizeNatural, sizeof(SIZE)) == 0;

	if (m_bAutoSize) //object can't do any other size
		return (bSizeMatchesNatural) ? S_OK : E_FAIL;

	BOOL bResized = FALSE;
	if (memcmp(psizel, &m_sizeExtent, sizeof(SIZE)) != 0)
	{
		m_sizeExtent = *psizel;
		bResized = TRUE;
	}
	if (m_bResizeNatural && !bSizeMatchesNatural)
	{
		m_sizeNatural = *psizel;
		bResized = TRUE;
	}

	if (m_bRecomposeOnResize && bResized)
	{
		SendOnDataChange();
		FireViewChange();
	}
	return S_OK;
}

inline HRESULT CComControlBase::IViewObject_Draw(
	_In_ DWORD dwDrawAspect,
	_In_ LONG lindex,
	_In_opt_ void *pvAspect,
	_In_opt_ DVTARGETDEVICE *ptd,
	_In_ HDC hicTargetDev,
	_In_ HDC hdcDraw,
	_In_opt_ LPCRECTL prcBounds,
	_In_opt_ LPCRECTL prcWBounds)
{
	ATLTRACE(atlTraceControls,2,_T("Draw dwDrawAspect=%x lindex=%d ptd=%x hic=%x hdc=%x\n"),
		dwDrawAspect, lindex, reinterpret_cast<ULONG_PTR>(ptd),
			reinterpret_cast<ULONG_PTR>(hicTargetDev), reinterpret_cast<ULONG_PTR>(hdcDraw));
#ifdef _DEBUG
	if (prcBounds == NULL)
		ATLTRACE(atlTraceControls,2,_T("\tprcBounds=NULL\n"));
	else
		ATLTRACE(atlTraceControls,2,_T("\tprcBounds=%d,%d,%d,%d\n"), prcBounds->left,
			prcBounds->top, prcBounds->right, prcBounds->bottom);
	if (prcWBounds == NULL)
		ATLTRACE(atlTraceControls,2,_T("\tprcWBounds=NULL\n"));
	else
		ATLTRACE(atlTraceControls,2,_T("\tprcWBounds=%d,%d,%d,%d\n"), prcWBounds->left,
			prcWBounds->top, prcWBounds->right, prcWBounds->bottom);
#endif

	if (prcBounds == NULL)
	{
		if (!m_bWndLess)
			return E_INVALIDARG;
		prcBounds = (RECTL*)&m_rcPos;
	}

	// support the aspects required for multi-pass drawing
	switch (dwDrawAspect)
	{
		case DVASPECT_CONTENT:
		case DVASPECT_OPAQUE:
		case DVASPECT_TRANSPARENT:
			break;
		default:
			ATLASSERT(FALSE);
			return DV_E_DVASPECT;
			break;
	}

	// make sure nobody forgets to do this
	if (ptd == NULL)
		hicTargetDev = NULL;

	BOOL bOptimize = FALSE;
	if (pvAspect && ((DVASPECTINFO *)pvAspect)->cb >= sizeof(DVASPECTINFO))
		bOptimize = (((DVASPECTINFO *)pvAspect)->dwFlags & DVASPECTINFOFLAG_CANOPTIMIZE);

	ATL_DRAWINFO di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	di.dwDrawAspect = dwDrawAspect;
	di.lindex = lindex;
	di.ptd = ptd;
	di.hicTargetDev = hicTargetDev;
	di.hdcDraw = hdcDraw;
	di.prcBounds = prcBounds;
	di.prcWBounds = prcWBounds;
	di.bOptimize = bOptimize;
	return OnDrawAdvanced(di);
}

inline HRESULT CComControlBase::IDataObject_GetData(
	_In_ FORMATETC *pformatetcIn,
	_Out_ STGMEDIUM *pmedium)
{
	if (pmedium == NULL)
		return E_POINTER;
	memset(pmedium, 0, sizeof(STGMEDIUM));
	ATLTRACE(atlTraceControls,2,_T("Format = %x\n"), pformatetcIn->cfFormat);
	ATLTRACE(atlTraceControls,2,_T("TYMED = %x\n"), pformatetcIn->tymed);

	if ((pformatetcIn->tymed & TYMED_MFPICT) == 0)
		return DATA_E_FORMATETC;

	SIZEL sizeMetric, size;
	if (m_bDrawFromNatural)
		sizeMetric = m_sizeNatural;
	else
		sizeMetric = m_sizeExtent;
	if (!m_bDrawGetDataInHimetric)
		AtlHiMetricToPixel(&sizeMetric, &size);
	else
		size = sizeMetric;
	RECTL rectl = {0 ,0, size.cx, size.cy};

	ATL_DRAWINFO di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	di.dwDrawAspect = DVASPECT_CONTENT;
	di.lindex = -1;
	di.ptd = NULL;
	di.hicTargetDev = NULL;
	di.prcBounds = &rectl;
	di.prcWBounds = &rectl;
	di.bOptimize = TRUE; //we do a SaveDC/RestoreDC
	di.bRectInHimetric = m_bDrawGetDataInHimetric;
	// create appropriate memory metafile DC
	di.hdcDraw = CreateMetaFile(NULL);

	// create attribute DC according to pformatetcIn->ptd

	SaveDC(di.hdcDraw);
	SetWindowOrgEx(di.hdcDraw, 0, 0, NULL);
	SetWindowExtEx(di.hdcDraw, rectl.right, rectl.bottom, NULL);
	OnDrawAdvanced(di);
	RestoreDC(di.hdcDraw, -1);

	HMETAFILE hMF = CloseMetaFile(di.hdcDraw);
	if (hMF == NULL)
		return E_UNEXPECTED;

	HGLOBAL hMem=GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, sizeof(METAFILEPICT));

	if (NULL==hMem)
	{
		DeleteMetaFile(hMF);
		return ResultFromScode(STG_E_MEDIUMFULL);
	}

	LPMETAFILEPICT pMF=(LPMETAFILEPICT)GlobalLock(hMem);
	ATLENSURE(pMF);
	pMF->hMF=hMF;
	pMF->mm=MM_ANISOTROPIC;
	pMF->xExt=sizeMetric.cx;
	pMF->yExt=sizeMetric.cy;
	GlobalUnlock(hMem);

	pmedium->tymed = TYMED_MFPICT;
	pmedium->hGlobal = hMem;
	pmedium->pUnkForRelease = NULL;

	return S_OK;
}

inline HRESULT CComControlBase::FireViewChange()
{
	if (m_bInPlaceActive)
	{
		// Active
		if (m_hWndCD != NULL)
			::InvalidateRect(m_hWndCD, NULL, TRUE); // Window based
		else if (m_bWndLess && m_spInPlaceSite != NULL)
			m_spInPlaceSite->InvalidateRect(NULL, TRUE); // Windowless
	}
	else // Inactive
		SendOnViewChange(DVASPECT_CONTENT);
	return S_OK;
}

inline void CComControlBase::GetZoomInfo(_Inout_ ATL_DRAWINFO& di)
{
	const RECTL& rcPos = *di.prcBounds;
	SIZEL sizeDen;
	if (m_bDrawFromNatural)
		sizeDen = m_sizeNatural;
	else
		sizeDen = m_sizeExtent;
	if (!di.bRectInHimetric)
		AtlHiMetricToPixel(&sizeDen, &sizeDen);
	SIZEL sizeNum = {rcPos.right-rcPos.left, rcPos.bottom-rcPos.top};
	di.ZoomNum.cx = sizeNum.cx;
	di.ZoomNum.cy = sizeNum.cy;
	di.ZoomDen.cx = sizeDen.cx;
	di.ZoomDen.cy = sizeDen.cy;
	if (sizeDen.cx == 0 || sizeDen.cy == 0 ||
		sizeNum.cx == 0 || sizeNum.cy == 0)
	{
		di.ZoomNum.cx = di.ZoomNum.cy = di.ZoomDen.cx = di.ZoomDen.cy = 1;
		di.bZoomed = FALSE;
	}
	else if (sizeNum.cx != sizeDen.cx || sizeNum.cy != sizeDen.cy)
		di.bZoomed = TRUE;
	else
		di.bZoomed = FALSE;
}

inline HRESULT CComControlBase::OnDrawAdvanced(_Inout_ ATL_DRAWINFO& di)
{
	BOOL bDeleteDC = FALSE;
	if (di.hicTargetDev == NULL)
	{
		di.hicTargetDev = AtlCreateTargetDC(di.hdcDraw, di.ptd);
		bDeleteDC = (di.hicTargetDev != di.hdcDraw);
	}
	RECTL rectBoundsDP = *di.prcBounds;
	BOOL bMetafile = GetDeviceCaps(di.hdcDraw, TECHNOLOGY) == DT_METAFILE;
	if (!bMetafile)
	{
		::LPtoDP(di.hdcDraw, (LPPOINT)&rectBoundsDP, 2);
		SaveDC(di.hdcDraw);
		SetMapMode(di.hdcDraw, MM_TEXT);
		SetWindowOrgEx(di.hdcDraw, 0, 0, NULL);
		SetViewportOrgEx(di.hdcDraw, 0, 0, NULL);
		di.bOptimize = TRUE; //since we save the DC we can do this
	}
	di.prcBounds = &rectBoundsDP;
	GetZoomInfo(di);

	HRESULT hRes = OnDraw(di);
	if (bDeleteDC)
		::DeleteDC(di.hicTargetDev);
	if (!bMetafile)
		RestoreDC(di.hdcDraw, -1);
	return hRes;
}

inline LRESULT CComControlBase::OnPaint(
	_In_ UINT /* uMsg */,
	_In_ WPARAM wParam,
	_In_ LPARAM /* lParam */,
	_In_ BOOL& /* lResult */)
{
	RECT rc;
	PAINTSTRUCT ps;

	HDC hdc = (wParam != 0) ? (HDC)wParam : ::BeginPaint(m_hWndCD, &ps);
	if (hdc == NULL)
		return 0;
	::GetClientRect(m_hWndCD, &rc);

	ATL_DRAWINFO di;
	memset(&di, 0, sizeof(di));
	di.cbSize = sizeof(di);
	di.dwDrawAspect = DVASPECT_CONTENT;
	di.lindex = -1;
	di.hdcDraw = hdc;
	di.prcBounds = (LPCRECTL)&rc;

	OnDrawAdvanced(di);
	if (wParam == 0)
		::EndPaint(m_hWndCD, &ps);
	return 0;
}


template <class T, class WinBase =  CWindowImpl< T > >
class ATL_NO_VTABLE CComControl :
	public CComControlBase,
	public WinBase
{
public:
	CComControl() : CComControlBase(this->m_hWnd) {}

	virtual HWND CreateControlWindow(
		_In_ HWND hWndParent,
		_In_ RECT& rcPos)
	{
		T* pT = static_cast<T*>(this);
		return pT->Create(hWndParent, rcPos);
	}

	HRESULT FireOnRequestEdit(_In_ DISPID dispID)
	{
		T* pT = static_cast<T*>(this);
		return T::__ATL_PROP_NOTIFY_EVENT_CLASS::FireOnRequestEdit(pT->GetUnknown(), dispID);
	}
	HRESULT FireOnChanged(_In_ DISPID dispID)
	{
		T* pT = static_cast<T*>(this);
		return T::__ATL_PROP_NOTIFY_EVENT_CLASS::FireOnChanged(pT->GetUnknown(), dispID);
	}

	virtual HRESULT ControlQueryInterface(
		_In_ const IID& iid,
		_Outptr_ void** ppv)
	{
		T* pT = static_cast<T*>(this);
		return pT->GetUnknown()->QueryInterface(iid, ppv);
	}

	int MessageBox(
		_In_z_ LPCTSTR lpszText,
		_In_opt_z_ LPCTSTR lpszCaption = _T(""),
		_In_ UINT nType = MB_OK)
	{
		if (::IsWindow(m_hWndCD))
			return ::MessageBox(m_hWndCD, lpszText, lpszCaption, nType);
		HWND hwndParent;
		if (m_spInPlaceSite && m_spInPlaceSite->GetWindow(&hwndParent) == S_OK)
			return ::MessageBox(hwndParent, lpszText, lpszCaption, nType);
		return ::MessageBox(NULL, lpszText, lpszCaption, nType);
	}

	typedef CComControl< T, WinBase >	thisClass;
	typedef WinBase						baseWinClass;
	BEGIN_MSG_MAP(thisClass)
		__if_not_exists(WinBase::m_wndReflector)
		{
		MESSAGE_HANDLER(WM_PAINT, CComControlBase::OnPaint)
		}
		MESSAGE_HANDLER(WM_SETFOCUS, CComControlBase::OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, CComControlBase::OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, CComControlBase::OnMouseActivate)
		__if_exists(WinBase::m_wndReflector)
		{
			CHAIN_MSG_MAP(baseWinClass)
		}
	END_MSG_MAP()
};

//////////////////////////////////////////////////////////////////////////////
// CComCompositeControl

#ifndef _ATL_NO_HOSTING
template <class T>
class CComCompositeControl :
	public CComControl< T, CAxDialogImpl< T > >
{
public:
	CComCompositeControl()
	{
		m_hbrBackground = NULL;
		m_hWndFocus = NULL;
	}
	virtual ~CComCompositeControl()
	{
		DeleteObject(m_hbrBackground);
	}
	HRESULT AdviseSinkMap(_In_ bool bAdvise)
	{
		if(!bAdvise && this->m_hWnd == NULL)
		{
			// window is gone, controls are already unadvised
			ATLTRACE(atlTraceControls, 1, _T("CComCompositeControl::AdviseSinkMap called after the window was destroyed\n"));
			return S_OK;
		}
		T* pT = static_cast<T*>(this);
		return AtlAdviseSinkMap(pT, bAdvise);
	}
	HBRUSH m_hbrBackground;
	HRESULT SetBackgroundColorFromAmbient()
	{
		if (m_hbrBackground != NULL)
		{
			DeleteObject(m_hbrBackground);
			m_hbrBackground = NULL;
		}
		OLE_COLOR clr;
		HRESULT hr = this->GetAmbientBackColor(clr);
		if (SUCCEEDED(hr))
		{
			COLORREF rgb;
			::OleTranslateColor(clr, NULL, &rgb);
			m_hbrBackground = ::CreateSolidBrush(rgb);
			EnumChildWindows(this->m_hWnd, (WNDENUMPROC)BackgroundColorEnumProc, (LPARAM) clr);
		}
		return hr;
	}
	static BOOL CALLBACK BackgroundColorEnumProc(_In_ HWND hwnd, _In_ LPARAM l)
	{
		CAxWindow wnd(hwnd);
		CComPtr<IAxWinAmbientDispatch> spDispatch;
		wnd.QueryHost(&spDispatch);
		if (spDispatch != NULL)
			spDispatch->put_BackColor((OLE_COLOR)l);
		return TRUE;
	}
	LRESULT OnDialogColor(
		_In_ UINT,
		_In_ WPARAM w,
		_In_ LPARAM,
		_In_ BOOL&)
	{
		HIGHCONTRAST contrastMode;
		memset(&contrastMode, 0, sizeof(HIGHCONTRAST));
		contrastMode.cbSize = sizeof(HIGHCONTRAST);

		if (SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &contrastMode, 0) &&
			(contrastMode.dwFlags & HCF_HIGHCONTRASTON) != 0)

			return this->DefWindowProc();

		HDC dc = (HDC) w;
		LOGBRUSH lb;
		::GetObject(m_hbrBackground, sizeof(lb), (void*)&lb);
		::SetBkColor(dc, lb.lbColor);
		return (LRESULT)m_hbrBackground;
	}
	HWND Create(
		_In_ HWND hWndParent,
		_In_ RECT& /*rcPos*/,
		_In_ LPARAM dwInitParam = NULL)
	{
		CComControl< T, CAxDialogImpl< T > >::Create(hWndParent, dwInitParam);
		if (this->m_hWnd != NULL)
		{
			SetBackgroundColorFromAmbient();
			this->ShowWindow(SW_SHOWNOACTIVATE);
		}
		return this->m_hWnd;
	}
	BOOL CalcExtent(_Inout_ SIZE& size)
	{
		HINSTANCE hInstance = _AtlBaseModule.GetResourceInstance();
		LPCTSTR lpTemplateName = MAKEINTRESOURCE(static_cast<T*>(this)->IDD);
		HRSRC hDlgTempl = FindResource(hInstance, lpTemplateName, RT_DIALOG);
		if (hDlgTempl == NULL)
			return FALSE;
		HGLOBAL hResource = LoadResource(hInstance, hDlgTempl);
		if (hResource == NULL)
			return FALSE;
		DLGTEMPLATE* pDlgTempl = (DLGTEMPLATE*)LockResource(hResource);
		if (pDlgTempl == NULL)
			return FALSE;
		AtlGetDialogSize(pDlgTempl, &size);
		AtlPixelToHiMetric(&size, &size);
		return TRUE;
	}
//Implementation
	BOOL PreTranslateAccelerator(
		_In_ LPMSG pMsg,
		_Out_ HRESULT& hRet)
	{
		hRet = S_OK;

		if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;
		// find a direct child of the dialog from the window that has focus
		HWND hWndCtl = ::GetFocus();
		if (this->IsChild(hWndCtl) && ::GetParent(hWndCtl) != this->m_hWnd)
		{
			do
			{
				hWndCtl = ::GetParent(hWndCtl);
			}
			while (::GetParent(hWndCtl) != this->m_hWnd);
		}
		// give controls a chance to translate this message
		if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) == 1)
			return TRUE;

		// special handling for keyboard messages
		LRESULT dwDlgCode = ::SendMessage(pMsg->hwnd, WM_GETDLGCODE, 0, 0);
		switch(pMsg->message)
		{
		case WM_CHAR:
			if(dwDlgCode == 0)	// no dlgcode, possibly an ActiveX control
				return FALSE;	// let the container process this
			break;
		case WM_KEYDOWN:
			switch(LOWORD(pMsg->wParam))
			{
			case VK_TAB:
				// prevent tab from looping inside of our dialog
				if((dwDlgCode & DLGC_WANTTAB) == 0)
				{
					HWND hWndFirstOrLast = ::GetWindow(this->m_hWnd, GW_CHILD);
					if (::GetKeyState(VK_SHIFT) >= 0)  // not pressed
						hWndFirstOrLast = this->GetNextDlgTabItem(hWndFirstOrLast, TRUE);
					if (hWndFirstOrLast == hWndCtl)
						return FALSE;
				}
				break;
			case VK_LEFT:
			case VK_UP:
			case VK_RIGHT:
			case VK_DOWN:
				// prevent arrows from looping inside of our dialog
				if((dwDlgCode & DLGC_WANTARROWS) == 0)
				{
					HWND hWndFirstOrLast = ::GetWindow(this->m_hWnd, GW_CHILD);
					if (pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN)	// going forward
						hWndFirstOrLast = this->GetNextDlgTabItem(hWndFirstOrLast, TRUE);
					if (hWndFirstOrLast == hWndCtl)
						return FALSE;
				}
				break;
			case VK_RETURN:
				break;
			case VK_EXECUTE:
			case VK_ESCAPE:
			case VK_CANCEL:
				// we don't want to handle these, let the container do it
				return FALSE;
			}
			break;
		}

		BOOL bRet;
		//Process accel msg
		if ( (pMsg->message == WM_SYSCHAR) || (pMsg->message == WM_SYSKEYDOWN) || (pMsg->message == WM_SYSKEYUP) )
		{
			T* pT = static_cast<T*>(this);

			CONTROLINFO ci;
			HRESULT hr = pT->GetControlInfo(&ci);
			if (SUCCEEDED(hr))
			{
				if (ci.cAccel > 0)
				{
					ACCEL* pAccel = _ATL_NEW ACCEL[ci.cAccel];
					if (pAccel == NULL)
					{
						//Out of memory, don't send to control site
						hRet = E_OUTOFMEMORY;
						return TRUE;
					}
					int cAccel = CopyAcceleratorTable(ci.hAccel, pAccel, ci.cAccel);
					ATLASSERT(cAccel == ci.cAccel);
					bRet = FALSE;	//Accel not processed (invalid)
					WORD fVert = (pMsg->message == WM_SYSCHAR) ? FALT : 0;
					WORD key = LOWORD(pMsg->wParam);
					for (int i = 0; i < cAccel; i++)
					{
						if (((pAccel[i].fVirt & ~FNOINVERT & ~FVIRTKEY) == fVert) &&
							((pAccel[i].key == _toupper(key)) || pAccel[i].key == _tolower(key)))
						{
							bRet = ::IsDialogMessage(this->m_hWnd, pMsg);	//Accel is valid, process
							break;
						}
					}
					delete [] pAccel;
				}
				else
					bRet = FALSE;	//No accels to process, let the container handle
			}
			else
			{
				bRet = ::IsDialogMessage(this->m_hWnd, pMsg);	//Backward compt. (not impl GetControlInfo)
			}
		}
		else
		{
			bRet = ::IsDialogMessage(this->m_hWnd, pMsg);	//Not an accelerator msg
		}

		if (bRet)
		{
			HWND hWndCtlNewFocus = ::GetFocus();
			if (this->IsChild(hWndCtlNewFocus))
				m_hWndFocus = hWndCtlNewFocus;
			else
				m_hWndFocus = NULL;
			if (this->IsChild(hWndCtlNewFocus) && ::GetParent(hWndCtlNewFocus) != this->m_hWnd)
			{
				do
				{
					hWndCtlNewFocus = ::GetParent(hWndCtlNewFocus);
				}
				while (::GetParent(hWndCtlNewFocus) != this->m_hWnd);
			}

			if (this->IsChild(hWndCtlNewFocus)  && this->IsChild(hWndCtl) && hWndCtl != hWndCtlNewFocus)
			{
				CComPtr<IUnknown> spUnknown;
				HRESULT hr = AtlAxGetControl(hWndCtl, &spUnknown);
				if (SUCCEEDED(hr))
				{
					CComPtr<IOleInPlaceObject> spIOleInPlaceObject;
					hr = spUnknown->QueryInterface(&spIOleInPlaceObject);
					if (SUCCEEDED(hr))
						spIOleInPlaceObject->UIDeactivate();
				}
			}
		}
		return bRet;
	}
	HRESULT IOleInPlaceObject_InPlaceDeactivate(void)
	{
		AdviseSinkMap(false); //unadvise
		return CComControl<T, CAxDialogImpl<T> >::IOleInPlaceObject_InPlaceDeactivate();
	}
	HRESULT IOleInPlaceObject_UIDeactivate(void)
	{
		if (m_hWndFocus != NULL)
		{
			HWND hWnd = m_hWndFocus;
			do
			{
				hWnd = ::GetParent(hWnd);
			}
			while (hWnd != NULL && ::GetParent(hWnd) != this->m_hWnd);

			if (hWnd != m_hWndFocus && hWnd != NULL)
			{
				CComPtr<IUnknown> spUnknown;
				HRESULT hr = AtlAxGetControl(hWnd, &spUnknown);
				if (SUCCEEDED(hr))
				{
					CComPtr<IOleInPlaceObject> spIOleInPlaceObject;
					hr = spUnknown->QueryInterface(&spIOleInPlaceObject);
					if (SUCCEEDED(hr))
						spIOleInPlaceObject->UIDeactivate();
				}
			}
		}
		m_hWndFocus = NULL;
		return CComControl<T, CAxDialogImpl<T> >::IOleInPlaceObject_UIDeactivate();
		return S_OK;
	}

	virtual HWND CreateControlWindow(
		_In_ HWND hWndParent,
		_In_ RECT& rcPos)
	{
		T* pT = static_cast<T*>(this);
		HWND h = pT->Create(hWndParent, rcPos);
		if (h != NULL)
			AdviseSinkMap(true);
		return h;
	}
	virtual HRESULT OnDraw(_In_ ATL_DRAWINFO& di)
	{
		if(!this->m_bInPlaceActive)
		{
			HPEN hPen = (HPEN)::GetStockObject(BLACK_PEN);
			HBRUSH hBrush = (HBRUSH)::GetStockObject(GRAY_BRUSH);
			::SelectObject(di.hdcDraw, hPen);
			::SelectObject(di.hdcDraw, hBrush);
			::Rectangle(di.hdcDraw, di.prcBounds->left, di.prcBounds->top, di.prcBounds->right, di.prcBounds->bottom);
			::SetTextColor(di.hdcDraw, ::GetSysColor(COLOR_WINDOWTEXT));
			::SetBkMode(di.hdcDraw, TRANSPARENT);
			::DrawText(di.hdcDraw, _T("ATL Composite Control"), -1, (LPRECT)di.prcBounds, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		return S_OK;
	}
	LRESULT OnInitDialog(
		_In_ UINT /*uMsg*/,
		_In_ WPARAM /*wParam*/,
		_In_ LPARAM /*lParam*/,
		_Out_  BOOL& bHandled)
	{
		// initialize controls in dialog with DLGINIT resource section
		this->ExecuteDlgInit(static_cast<T*>(this)->IDD);
		bHandled = TRUE;
		return 1;
	}
	// save HWND of child that last had focus
	LRESULT OnChildKillFocus(
		_In_ WORD /*wNotifyCode*/,
		_In_ WORD /*wID*/,
		_In_ HWND hWndCtl,
		_Out_ BOOL& bHandled)
	{
		m_hWndFocus = hWndCtl;
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnNMKillFocus(
		_In_ int /*idCtrl*/,
		_In_ LPNMHDR pnmh,
		_Out_ BOOL& bHandled)
	{
		m_hWndFocus = pnmh->hwndFrom;
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnSetFocus(
		_In_ UINT /*uMsg*/,
		_In_ WPARAM /*wParam*/,
		_In_ LPARAM /*lParam*/,
		_Out_ BOOL& bHandled)
	{
		// Call base class OnSetFocus so control is UI-activated.
		baseClass::OnSetFocus(0, 0, 0, bHandled);
		// Shift-tab, up or left arrow was pressed, set focus to last control.
		if ((GetKeyState(VK_SHIFT) < 0 && GetKeyState(VK_TAB) < 0) ||
			(GetKeyState(VK_UP) < 0) || (GetKeyState(VK_LEFT) < 0))
		{
			::SetFocus(::GetWindow(::GetWindow(this->m_hWnd, GW_CHILD), GW_HWNDLAST));
		}
		// Tab, down or right arrow was pressed, set focus to first control.
		else if (GetKeyState(VK_TAB) < 0 || GetKeyState(VK_DOWN) < 0 ||
				GetKeyState(VK_RIGHT) < 0)
		{
			::SetFocus(::GetWindow(this->m_hWnd, GW_CHILD));
		}
		else
		{
			if (!::IsWindow(m_hWndFocus) || !::IsChild(this->m_hWnd, m_hWndFocus))
				m_hWndFocus = ::GetWindow(this->m_hWnd, GW_CHILD);
			// set focus to last child window that had focus
			::SetFocus(m_hWndFocus);
		}

		bHandled = TRUE;
		return 0;
	}
	typedef CComControl< T, CAxDialogImpl< T > >	baseClass;
	LRESULT OnMouseActivate(
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Out_ BOOL& bHandled)
	{
		ATLTRACE(_T("CComCompositeControl::OnMouseActivate\n"));
		HWND hWndFocus = GetFocus();
		if (m_hWndFocus != NULL)
		{
			if (m_hWndFocus != this->m_hWnd && hWndFocus != m_hWndFocus)
			{
				HWND hWnd = m_hWndFocus;
				do
				{
					hWnd = ::GetParent(hWnd);
				}
				while (hWnd != NULL && ::GetParent(hWnd) != this->m_hWnd);

				if (hWnd != m_hWndFocus)
				{
					CComPtr<IUnknown> spUnknown;
					HRESULT hr = AtlAxGetControl(hWnd, &spUnknown);
					if (SUCCEEDED(hr))
					{
						CComPtr<IOleInPlaceObject> spIOleInPlaceObject;
						hr = spUnknown->QueryInterface(&spIOleInPlaceObject);
						if (SUCCEEDED(hr))
							spIOleInPlaceObject->UIDeactivate();
					}
				}
			}
		}
		if (this->IsChild(hWndFocus))
			m_hWndFocus = hWndFocus;
		else
			m_hWndFocus = NULL;

		return baseClass::OnMouseActivate(uMsg, wParam, lParam, bHandled);
	}

	BEGIN_MSG_MAP(CComCompositeControl< T >)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnDialogColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnDialogColor)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		COMMAND_CODE_HANDLER(EN_KILLFOCUS, OnChildKillFocus)
		COMMAND_CODE_HANDLER(BN_KILLFOCUS, OnChildKillFocus)
		COMMAND_CODE_HANDLER(LBN_KILLFOCUS, OnChildKillFocus)
		COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnChildKillFocus)
		NOTIFY_CODE_HANDLER(NM_KILLFOCUS, OnNMKillFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, baseClass::OnKillFocus)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	END_MSG_MAP()

	BEGIN_SINK_MAP(T)
	END_SINK_MAP()

	HWND m_hWndFocus;
};
#endif //_ATL_NO_HOSTING

// Forward declarations
//
template <class T> class IPersistStorageImpl;
template <class T> class IPersistPropertyBagImpl;
template <class T> class IOleControlImpl;
template <class T> class IRunnableObjectImpl;
template <class T> class IQuickActivateImpl;
template <class T> class IOleObjectImpl;
template <class T> class IPropertyPageImpl;
template <class T> class IPropertyPage2Impl;
template <class T> class IPerPropertyBrowsingImpl;
template <class T> class IViewObjectExImpl;
template <class T> class IOleWindowImpl;
template <class T> class IPointerInactiveImpl;
template <class T, class CDV> class IPropertyNotifySinkCP;
template <class T> class IBindStatusCallbackImpl;
template <class T, int nBindFlags> class CBindStatusCallback;


//////////////////////////////////////////////////////////////////////////////
// IOleControlImpl
template <class T>
class ATL_NO_VTABLE IOleControlImpl :
	public IOleControl
{
public:
	STDMETHOD(GetControlInfo)(_In_ LPCONTROLINFO /* pCI */)
	{
		ATLTRACENOTIMPL(_T("IOleControlImpl::GetControlInfo"));
	}
	STDMETHOD(OnMnemonic)(_In_ LPMSG /* pMsg */)
	{
		ATLTRACENOTIMPL(_T("IOleControlImpl::OnMnemonic"));
	}
	STDMETHOD(OnAmbientPropertyChange)(_In_ DISPID dispid)
	{
		UNREFERENCED_PARAMETER(dispid);
		ATLTRACE(atlTraceControls,2,_T("IOleControlImpl::OnAmbientPropertyChange\n"));
		ATLTRACE(atlTraceControls,2,_T(" -- DISPID = %d\n"), dispid);
		return S_OK;
	}
	STDMETHOD(FreezeEvents)(_In_ BOOL bFreeze)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleControlImpl::FreezeEvents\n"));
		if (bFreeze)
			pT->m_nFreezeEvents++;
		else
			pT->m_nFreezeEvents--;
		return S_OK;
	}
};


//////////////////////////////////////////////////////////////////////////////
// IQuickActivateImpl
template <class T>
class ATL_NO_VTABLE IQuickActivateImpl :
	public IQuickActivate
{
public:
	STDMETHOD(QuickActivate)(
		_In_ QACONTAINER *pQACont,
		_Inout_ QACONTROL *pQACtrl)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IQuickActivateImpl::QuickActivate\n"));
		__if_exists(T::m_clrForeColor)
		{
			pT->m_clrForeColor = pQACont->colorFore;
		}
		__if_exists(T::m_clrBackColor)
		{
			pT->m_clrBackColor = pQACont->colorBack;
		}
		__if_exists(T::m_nAppearance)
		{
			// If you've declared m_nAppearance as something other than
			// 'short', you'll need to typedef AppearanceType to that type
			// in your derived class T.
			pT->m_nAppearance = static_cast<typename T::AppearanceType>(pQACont->dwAppearance);
		}
		return pT->IQuickActivate_QuickActivate(pQACont, pQACtrl);
	}
	STDMETHOD(SetContentExtent)(_In_ LPSIZEL pSize)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IQuickActivateImpl::SetContentExtent\n"));
		return pT->IOleObjectImpl<T>::SetExtent(DVASPECT_CONTENT, pSize);
	}
	STDMETHOD(GetContentExtent)(_Out_ LPSIZEL pSize)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IQuickActivateImpl::GetContentExtent\n"));
		return pT->IOleObjectImpl<T>::GetExtent(DVASPECT_CONTENT, pSize);
	}
};

//////////////////////////////////////////////////////////////////////////////
// IOleObjectImpl
ATLPREFAST_SUPPRESS(6387)
template <class T>
class ATL_NO_VTABLE IOleObjectImpl :
	public IOleObject
{
public:
	STDMETHOD(SetClientSite)(_Inout_opt_ IOleClientSite *pClientSite)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::SetClientSite\n"));
		return pT->IOleObject_SetClientSite(pClientSite);
	}
	STDMETHOD(GetClientSite)(_Outptr_result_maybenull_ IOleClientSite **ppClientSite)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::GetClientSite\n"));
		return pT->IOleObject_GetClientSite(ppClientSite);
	}
	STDMETHOD(SetHostNames)(
		_In_opt_z_ LPCOLESTR /* szContainerApp */,
		_In_opt_z_ LPCOLESTR /* szContainerObj */)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::SetHostNames\n"));
		return S_OK;
	}
	STDMETHOD(Close)(_In_ DWORD dwSaveOption)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::Close\n"));
		return pT->IOleObject_Close(dwSaveOption);
	}
	STDMETHOD(SetMoniker)(
		_In_ DWORD /* dwWhichMoniker */,
		_In_opt_ IMoniker* /* pmk */)
	{
		ATLTRACENOTIMPL(_T("IOleObjectImpl::SetMoniker"));
	}
	STDMETHOD(GetMoniker)(
		_In_ DWORD /* dwAssign */,
		_In_ DWORD /* dwWhichMoniker */,
		_In_opt_ IMoniker** /* ppmk */)
	{
		ATLTRACENOTIMPL(_T("IOleObjectImpl::GetMoniker"));
	}
	STDMETHOD(InitFromData)(
		_In_ IDataObject* /* pDataObject */,
		_In_ BOOL /* fCreation */,
		_In_ DWORD /* dwReserved */)
	{
		ATLTRACENOTIMPL(_T("IOleObjectImpl::InitFromData"));
	}
	STDMETHOD(GetClipboardData)(
		_In_ DWORD /* dwReserved */,
		_In_opt_ IDataObject** /* ppDataObject */)
	{
		ATLTRACENOTIMPL(_T("IOleObjectImpl::GetClipboardData"));
	}

	// Helpers for DoVerb - Over-rideable in user class
	HRESULT DoVerbPrimary(
		_In_ LPCRECT prcPosRect,
		_In_ HWND hwndParent)
	{
		T* pT = static_cast<T*>(this);
		BOOL bDesignMode = FALSE;
		CComVariant var;
		// if container doesn't support this property
		// don't allow design mode
		HRESULT hRes = pT->GetAmbientProperty(DISPID_AMBIENT_USERMODE, var);
		if (SUCCEEDED(hRes) && var.vt == VT_BOOL && !var.boolVal)
			bDesignMode = TRUE;
		if (bDesignMode)
			return pT->DoVerbProperties(prcPosRect, hwndParent);
		return pT->DoVerbInPlaceActivate(prcPosRect, hwndParent);
	}
	HRESULT DoVerbShow(
		_In_ LPCRECT prcPosRect,
		_In_ HWND /* hwndParent */)
	{
		T* pT = static_cast<T*>(this);
		HRESULT hr;
		hr = pT->OnPreVerbShow();
		if (SUCCEEDED(hr))
		{
			hr = pT->InPlaceActivate(OLEIVERB_SHOW, prcPosRect);
			if (SUCCEEDED(hr))
				hr = pT->OnPostVerbShow();
		}
		return hr;
	}
	HRESULT DoVerbInPlaceActivate(
		_In_ LPCRECT prcPosRect,
		_In_ HWND /* hwndParent */)
	{
		T* pT = static_cast<T*>(this);
		HRESULT hr;
		hr = pT->OnPreVerbInPlaceActivate();
		if (SUCCEEDED(hr))
		{
			hr = pT->InPlaceActivate(OLEIVERB_INPLACEACTIVATE, prcPosRect);
			if (SUCCEEDED(hr))
				hr = pT->OnPostVerbInPlaceActivate();
			if (SUCCEEDED(hr))
				pT->FireViewChange();
		}
		return hr;
	}
	HRESULT DoVerbUIActivate(
		_In_ LPCRECT prcPosRect,
		_In_ HWND /* hwndParent */)
	{
		T* pT = static_cast<T*>(this);
		HRESULT hr = S_OK;
		if (!pT->m_bUIActive)
		{
			hr = pT->OnPreVerbUIActivate();
			if (SUCCEEDED(hr))
			{
				hr = pT->InPlaceActivate(OLEIVERB_UIACTIVATE, prcPosRect);
				if (SUCCEEDED(hr))
					hr = pT->OnPostVerbUIActivate();
			}
		}
		return hr;
	}
	HRESULT DoVerbHide(
		_In_opt_ LPCRECT /* prcPosRect */,
		_In_ HWND /* hwndParent */)
	{
		T* pT = static_cast<T*>(this);
		HRESULT hr;
		hr = pT->OnPreVerbHide();
		if (SUCCEEDED(hr))
		{
			pT->UIDeactivate();
			if (pT->m_hWnd)
				pT->ShowWindow(SW_HIDE);
			hr = pT->OnPostVerbHide();
		}
		return hr;
	}
	HRESULT DoVerbOpen(
		_In_opt_ LPCRECT /* prcPosRect */,
		_In_ HWND /* hwndParent */)
	{
		T* pT = static_cast<T*>(this);
		HRESULT hr;
		hr = pT->OnPreVerbOpen();
		if (SUCCEEDED(hr))
			hr = pT->OnPostVerbOpen();
		return hr;
	}
	HRESULT DoVerbDiscardUndo(
		_In_opt_ LPCRECT /* prcPosRect */,
		_In_ HWND /* hwndParent */)
	{
		T* pT = static_cast<T*>(this);
		HRESULT hr;
		hr = pT->OnPreVerbDiscardUndo();
		if (SUCCEEDED(hr))
			hr = pT->OnPostVerbDiscardUndo();
		return hr;
	}
	STDMETHOD(DoVerb)(
		_In_ LONG iVerb,
		_In_opt_ LPMSG /* pMsg */,
		_Inout_ IOleClientSite* pActiveSite,
		_In_ LONG /* lindex */,
		_In_ HWND hwndParent,
		_In_ LPCRECT lprcPosRect)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::DoVerb(%d)\n"), iVerb);
		ATLASSERT(pT->m_spClientSite);

		// We don't support getting a different site from the one passed into SetClientSite.
			if (!pT->m_spClientSite.IsEqualObject(pActiveSite))
		{
			return E_UNEXPECTED;
		}

		HRESULT hr;
		if (iVerb > 0)
		{
			pT->DoVerbPrimary(lprcPosRect, hwndParent);
			hr = OLEOBJ_S_INVALIDVERB;
		}
		else
		{
			hr = E_NOTIMPL;
			switch (iVerb)
			{
			case OLEIVERB_PRIMARY:
				hr = pT->DoVerbPrimary(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_SHOW:
				hr = pT->DoVerbShow(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_INPLACEACTIVATE:
				hr = pT->DoVerbInPlaceActivate(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_UIACTIVATE:
				hr = pT->DoVerbUIActivate(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_HIDE:
				hr = pT->DoVerbHide(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_OPEN:
				hr = pT->DoVerbOpen(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_DISCARDUNDOSTATE:
				hr = pT->DoVerbDiscardUndo(lprcPosRect, hwndParent);
				break;
			case OLEIVERB_PROPERTIES:
				hr = pT->DoVerbProperties(lprcPosRect, hwndParent);
			}
		}
		return hr;
	}
	STDMETHOD(EnumVerbs)(_Outptr_ IEnumOLEVERB **ppEnumOleVerb)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::EnumVerbs\n"));
		ATLASSERT(ppEnumOleVerb);
		if (!ppEnumOleVerb)
			return E_POINTER;
		return OleRegEnumVerbs(T::GetObjectCLSID(), ppEnumOleVerb);
	}
	STDMETHOD(Update)(void)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::Update\n"));
		return S_OK;
	}
	STDMETHOD(IsUpToDate)(void)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::IsUpToDate\n"));
		return S_OK;
	}
	STDMETHOD(GetUserClassID)(_Out_ CLSID *pClsid)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::GetUserClassID\n"));
		ATLASSERT(pClsid);
		if (!pClsid)
			return E_POINTER;
		*pClsid = T::GetObjectCLSID();
		return S_OK;
	}
	STDMETHOD(GetUserType)(
		_In_ DWORD dwFormOfType,
		_Outptr_result_z_ LPOLESTR *pszUserType)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::GetUserType\n"));
		return OleRegGetUserType(T::GetObjectCLSID(), dwFormOfType, pszUserType);
	}
	STDMETHOD(SetExtent)(
		_In_ DWORD dwDrawAspect,
		_In_ SIZEL *psizel)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::SetExtent\n"));
		return pT->IOleObject_SetExtent(dwDrawAspect, psizel);
	}
	STDMETHOD(GetExtent)(
		_In_ DWORD dwDrawAspect,
		_Out_ SIZEL *psizel)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::GetExtent\n"));
		if (dwDrawAspect != DVASPECT_CONTENT)
			return E_FAIL;
		if (psizel == NULL)
			return E_POINTER;
		*psizel = pT->m_sizeExtent;
		return S_OK;
	}
	STDMETHOD(Advise)(
		_Inout_ IAdviseSink *pAdvSink,
		_Out_ DWORD *pdwConnection)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::Advise\n"));
		return pT->IOleObject_Advise(pAdvSink, pdwConnection);
	}
	STDMETHOD(Unadvise)(_In_ DWORD dwConnection)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::Unadvise\n"));
		HRESULT hRes = E_FAIL;
		if (pT->m_spOleAdviseHolder != NULL)
			hRes = pT->m_spOleAdviseHolder->Unadvise(dwConnection);
		return hRes;
	}
	STDMETHOD(EnumAdvise)(_Outptr_ IEnumSTATDATA **ppenumAdvise)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::EnumAdvise\n"));
		ATLASSERT(ppenumAdvise != NULL);
		if (ppenumAdvise == NULL)
			return E_POINTER;
		*ppenumAdvise = NULL;
		HRESULT hRes = E_FAIL;
		if (pT->m_spOleAdviseHolder != NULL)
			hRes = pT->m_spOleAdviseHolder->EnumAdvise(ppenumAdvise);

		return hRes;
	}
	STDMETHOD(GetMiscStatus)(
		_In_ DWORD dwAspect,
		_Out_ DWORD *pdwStatus)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleObjectImpl::GetMiscStatus\n"));
		return OleRegGetMiscStatus(T::GetObjectCLSID(), dwAspect, pdwStatus);
	}
	STDMETHOD(SetColorScheme)(_In_opt_ LOGPALETTE* /* pLogpal */)
	{
		ATLTRACENOTIMPL(_T("IOleObjectImpl::SetColorScheme"));
	}
// Implementation
public:
	HRESULT OnPreVerbShow()
	{
		return S_OK;
	}
	HRESULT OnPostVerbShow()
	{
		return S_OK;
	}
	HRESULT OnPreVerbInPlaceActivate()
	{
		return S_OK;
	}
	HRESULT OnPostVerbInPlaceActivate()
	{
		return S_OK;
	}
	HRESULT OnPreVerbUIActivate()
	{
		return S_OK;
	}
	HRESULT OnPostVerbUIActivate()
	{
		return S_OK;
	}
	HRESULT OnPreVerbHide()
	{
		return S_OK;
	}
	HRESULT OnPostVerbHide()
	{
		return S_OK;
	}
	HRESULT OnPreVerbOpen()
	{
		return S_OK;
	}
	HRESULT OnPostVerbOpen()
	{
		return S_OK;
	}
	HRESULT OnPreVerbDiscardUndo()
	{
		return S_OK;
	}
	HRESULT OnPostVerbDiscardUndo()
	{
		return S_OK;
	}
};
ATLPREFAST_UNSUPPRESS()

//////////////////////////////////////////////////////////////////////////////
// IPropertyPageImpl
template <class T>
class ATL_NO_VTABLE IPropertyPageImpl :
	public IPropertyPage
{

public:
	void SetDirty(_In_ BOOL bDirty)
	{
		T* pT = static_cast<T*>(this);
		if (pT->m_bDirty != bDirty)
		{
			pT->m_bDirty = bDirty;
			pT->m_pPageSite->OnStatusChange(bDirty ? PROPPAGESTATUS_DIRTY : PROPPAGESTATUS_CLEAN);
		}
	}

	IPropertyPageImpl()
	{
		T* pT = static_cast<T*>(this);
		pT->m_pPageSite = NULL;
		pT->m_size.cx = 0;
		pT->m_size.cy = 0;
		pT->m_dwTitleID = 0;
		pT->m_dwHelpFileID = 0;
		pT->m_dwDocStringID = 0;
		pT->m_dwHelpContext = 0;
		pT->m_ppUnk = NULL;
		pT->m_nObjects = 0;
		pT->m_bDirty = FALSE;
		pT->m_hWnd = NULL;
	}

	virtual ~IPropertyPageImpl()
	{
		T* pT = static_cast<T*>(this);
		if (pT->m_pPageSite != NULL)
			pT->m_pPageSite->Release();

		for (UINT i = 0; i < m_nObjects; i++)
			pT->m_ppUnk[i]->Release();

		delete[] pT->m_ppUnk;
	}

	// IPropertyPage
	//
	STDMETHOD(SetPageSite)(_Inout_ IPropertyPageSite *pPageSite)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::SetPageSite\n"));

		if (!pPageSite && pT->m_pPageSite)
		{
			pT->m_pPageSite->Release();
			pT->m_pPageSite = NULL;
			return S_OK;
		}

		if (!pPageSite && !pT->m_pPageSite)
			return S_OK;

		if (pPageSite && pT->m_pPageSite)
		{
			ATLTRACE(atlTraceControls,2,_T("Error : setting page site again with non NULL value\n"));
			return E_UNEXPECTED;
		}

		pT->m_pPageSite = pPageSite;
		pT->m_pPageSite->AddRef();
		return S_OK;
	}
	STDMETHOD(Activate)(
		_In_ HWND hWndParent,
		_In_ LPCRECT pRect,
		_In_ BOOL /* bModal */)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::Activate\n"));

		if (pRect == NULL)
		{
			ATLTRACE(atlTraceControls,2,_T("Error : Passed a NULL rect\n"));
			return E_POINTER;
		}

		pT->m_hWnd = pT->Create(hWndParent);
		Move(pRect);

		m_size.cx = pRect->right - pRect->left;
		m_size.cy = pRect->bottom - pRect->top;

		return S_OK;

	}
	STDMETHOD(Deactivate)(void)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::Deactivate\n"));

		if (pT->m_hWnd)
		{
			ATLTRACE(atlTraceControls,2,_T("Destroying Dialog\n"));
			if (::IsWindow(pT->m_hWnd))
				pT->DestroyWindow();
			pT->m_hWnd = NULL;
		}

		return S_OK;

	}
	STDMETHOD(GetPageInfo)(_Inout_ PROPPAGEINFO *pPageInfo)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::GetPageInfo\n"));

		if (pPageInfo == NULL)
		{
			ATLTRACE(atlTraceControls,2,_T("Error : PROPPAGEINFO passed == NULL\n"));
			return E_POINTER;
		}

		HRSRC hRsrc = FindResource(_AtlBaseModule.GetResourceInstance(),
									MAKEINTRESOURCE(static_cast<T*>(this)->IDD), RT_DIALOG);
		if (hRsrc == NULL)
		{
			ATLTRACE(atlTraceControls,2,_T("Could not find resource template\n"));
			return E_UNEXPECTED;
		}

		HGLOBAL hGlob = LoadResource(_AtlBaseModule.GetResourceInstance(), hRsrc);
		if (hGlob == NULL)
		{
			ATLTRACE(atlTraceControls,2,_T("Could not load resource template\n"));
			return E_UNEXPECTED;
		}
		DLGTEMPLATE* pDlgTempl = (DLGTEMPLATE*)LockResource(hGlob);
		if (pDlgTempl == NULL)
		{
			ATLTRACE(atlTraceControls,2,_T("Could not load resource template\n"));
			return E_UNEXPECTED;
		}
		AtlGetDialogSize(pDlgTempl, &m_size, true);

		pPageInfo->cb = sizeof(PROPPAGEINFO);
		pPageInfo->pszTitle = LoadStringHelper(pT->m_dwTitleID);
		pPageInfo->size = m_size;
		pPageInfo->pszHelpFile = LoadStringHelper(pT->m_dwHelpFileID);
		pPageInfo->pszDocString = LoadStringHelper(pT->m_dwDocStringID);
		pPageInfo->dwHelpContext = pT->m_dwHelpContext;

		return S_OK;
	}

	STDMETHOD(SetObjects)(
		_In_ ULONG nObjects,
		_Inout_ _Deref_pre_opt_valid_ IUnknown **ppUnk)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::SetObjects\n"));

		if (ppUnk == NULL)
			return E_POINTER;

		if (pT->m_ppUnk != NULL && pT->m_nObjects > 0)
		{
			for (UINT iObj = 0; iObj < pT->m_nObjects; iObj++)
				pT->m_ppUnk[iObj]->Release();

			delete [] pT->m_ppUnk;
		}

		pT->m_ppUnk = _ATL_NEW IUnknown*[nObjects];

		if (pT->m_ppUnk == NULL)
			return E_OUTOFMEMORY;

		for (UINT i = 0; i < nObjects; i++)
		{
			ppUnk[i]->AddRef();
			pT->m_ppUnk[i] = ppUnk[i];
		}

		pT->m_nObjects = nObjects;

		return S_OK;
	}
	STDMETHOD(Show)(_In_ UINT nCmdShow)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::Show\n"));

		if (pT->m_hWnd == NULL)
			return E_UNEXPECTED;

		ShowWindow(pT->m_hWnd, nCmdShow);
		return S_OK;
	}
	STDMETHOD(Move)(_In_ LPCRECT pRect)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::Move\n"));

		if (pT->m_hWnd == NULL)
			return E_UNEXPECTED;

		if (pRect == NULL)
			return E_POINTER;

		MoveWindow(pT->m_hWnd, pRect->left, pRect->top, pRect->right - pRect->left,
				 pRect->bottom - pRect->top, TRUE);

		return S_OK;

	}
	STDMETHOD(IsPageDirty)(void)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::IsPageDirty\n"));
		return pT->m_bDirty ? S_OK : S_FALSE;
	}
	STDMETHOD(Apply)(void)
	{
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::Apply\n"));
		return S_OK;
	}
	STDMETHOD(Help)(_In_z_ LPCOLESTR pszHelpDir)
	{
		T* pT = static_cast<T*>(this);
		USES_CONVERSION_EX;

		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::Help\n"));
		CComBSTR szFullFileName(pszHelpDir);
		CComHeapPtr< OLECHAR > pszFileName(LoadStringHelper(pT->m_dwHelpFileID));
		if (pszFileName == NULL)
			return E_OUTOFMEMORY;
		HRESULT hr=szFullFileName.Append(OLESTR("\\"));
		if(FAILED(hr))
		{
			return hr;
		}
		hr=szFullFileName.Append(pszFileName);
		if(FAILED(hr))
		{
			return hr;
		}
		WinHelp(pT->m_hWnd, OLE2CT_EX_DEF(szFullFileName), HELP_CONTEXTPOPUP, NULL);
		return S_OK;
	}
	STDMETHOD(TranslateAccelerator)(_In_ MSG *pMsg)
	{
		ATLTRACE(atlTraceControls,2,_T("IPropertyPageImpl::TranslateAccelerator\n"));
		T* pT = static_cast<T*>(this);
		if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
			(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return S_FALSE;

		return (IsDialogMessage(pT->m_hWnd, pMsg)) ? S_OK : S_FALSE;
	}

	IPropertyPageSite* m_pPageSite;
	IUnknown** m_ppUnk;
	ULONG m_nObjects;
	SIZE m_size;
	UINT m_dwTitleID;
	UINT m_dwHelpFileID;
	UINT m_dwDocStringID;
	DWORD m_dwHelpContext;
	BOOL m_bDirty;

//methods
public:

	BEGIN_MSG_MAP(IPropertyPageImpl<T>)
		MESSAGE_HANDLER(WM_STYLECHANGING, OnStyleChange)
	END_MSG_MAP()

	LRESULT OnStyleChange(
		_In_ UINT,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_In_ BOOL&)
	{
		if (wParam == GWL_EXSTYLE)
		{
			LPSTYLESTRUCT lpss = (LPSTYLESTRUCT) lParam;
			lpss->styleNew |= WS_EX_CONTROLPARENT;
			return 0;
		}
		return 1;
	}

	LPOLESTR LoadStringHelper(_In_ UINT idRes)
	{
		const ATLSTRINGRESOURCEIMAGE* pString = AtlGetStringResourceImage(
			_AtlBaseModule.GetResourceInstance(), idRes);
		if (pString == NULL)
		{
			ATLTRACE(atlTraceControls,2,_T("Error : Failed to load string from res\n"));
			return NULL;
		}

		CComHeapPtr< OLECHAR > psz;

		psz.Allocate( pString->nLength+1 );
		if (psz != NULL)
		{
			Checked::memcpy_s(psz, (pString->nLength+1)*sizeof(OLECHAR), pString->achString, pString->nLength*sizeof(OLECHAR));
			psz[pString->nLength] = L'\0';
		}

		return psz.Detach();
	}
};


//////////////////////////////////////////////////////////////////////////////
// IPropertyPage2Impl
template <class T>
class ATL_NO_VTABLE IPropertyPage2Impl :
	public IPropertyPageImpl<T>
{
public:

	STDMETHOD(EditProperty)(_In_ DISPID /*dispID*/)
	{
		ATLTRACENOTIMPL(_T("IPropertyPage2Impl::EditProperty\n"));
	}
};



//////////////////////////////////////////////////////////////////////////////
// IPerPropertyBrowsingImpl
template <class T>
class ATL_NO_VTABLE IPerPropertyBrowsingImpl :
	public IPerPropertyBrowsing
{
public:
	// declare empty map in case derived classes doesn't want to specify one
	DECLARE_EMPTY_PROP_VAL_MAP()

	_Success_(return == S_OK)
	STDMETHOD(GetDisplayString)(
		_In_ DISPID dispID,
		_Outptr_result_maybenull_z_ BSTR *pBstr)
	{
		ATLTRACE(atlTraceControls,2,_T("IPerPropertyBrowsingImpl::GetDisplayString\n"));
		if (pBstr == NULL)
			return E_POINTER;

		T* pT = static_cast<T*>(this);
		*pBstr = NULL;
		CComVariant var;

		//---- get current value of property ----
		IDispatch *pdisp = NULL;
		pT->QueryInterface(__uuidof(IDispatch), (void **)&pdisp);
		if (! pdisp)
			return S_FALSE;
		HRESULT hr = CComDispatchDriver::GetProperty(pdisp, dispID, &var);
		pdisp->Release();
		if (FAILED(hr))
			return S_FALSE;

		//---- try finding a match in the PROP_VAL_MAP ----
		ATL_PROPVALMAP_ENTRY *valmap;
		int i, cnt;
		BSTR bstrSrc;

		valmap = pT->GetPropValMap(&cnt);
		if ((valmap) && (cnt))
		{
			for (i=0; i < cnt; i++)
			{
				if ((valmap[i].dispid == dispID) && (var == valmap[i].val))
				{
					bstrSrc = (BSTR)valmap[i].szDesc;
					*pBstr = SysAllocString(bstrSrc);
					if (*pBstr == NULL && bstrSrc != NULL)
						return E_OUTOFMEMORY;
					return S_OK;
				}
			}
		}

		//---- not in our PROP_VAL_MAP - let it get standard host treatment ----
		return S_FALSE;
	}

	STDMETHOD(MapPropertyToPage)(
		_In_ DISPID dispID,
		_Out_ CLSID *pClsid)
	{
		ATLTRACE(atlTraceControls,2,_T("IPerPropertyBrowsingImpl::MapPropertyToPage\n"));
		if (pClsid == NULL)
			return E_POINTER;

		const ATL_PROPMAP_ENTRY* pMap = T::GetPropertyMap();
		ATLASSERT(pMap != NULL);
		if(!pMap)
		{
			return E_FAIL;
		}
		for (int i = 0; pMap[i].pclsidPropPage != NULL; i++)
		{
			if (pMap[i].szDesc == NULL)
				continue;

			// reject data entry types
			if (pMap[i].dwSizeData != 0)
				continue;

			if (pMap[i].dispid == dispID)
			{
				ATLASSERT(pMap[i].pclsidPropPage != NULL);
				*pClsid = *(pMap[i].pclsidPropPage);
				// Does this property have a page?  CLSID_NULL means it does not
				if (InlineIsEqualGUID(*pMap[i].pclsidPropPage, CLSID_NULL))
					return PERPROP_E_NOPAGEAVAILABLE;
				return S_OK;
			}
		}
		*pClsid = CLSID_NULL;
		return E_INVALIDARG;
	}

	STDMETHOD(GetPredefinedStrings)(
		_In_ DISPID dispID,
		_Out_ CALPOLESTR *pCaStringsOut,
		_Out_ CADWORD *pCaCookiesOut)
	{
		ATL_PROPVALMAP_ENTRY *valmap;
		int i, cnt, matches, addcnt = 0;

		ATLTRACE(atlTraceControls,2,_T("IPerPropertyBrowsingImpl::GetPredefinedStrings\n"));
		if (pCaStringsOut == NULL || pCaCookiesOut == NULL)
			return E_POINTER;

		valmap = T::GetPropValMap(&cnt);
		if ((! valmap) || (! cnt))
			ATLTRACENOTIMPL(_T("IPerPropertyBrowsingImpl::GetPredefinedStrings"));

		//---- first pass thru - count matches ----
		matches = 0;
		for (i=0; i < cnt; i++)
		{
			if (dispID == valmap[i].dispid)
				matches++;
		}

		DWORD *pCookies = NULL;
		LPOLESTR *pStrings = NULL;

		//---- set up the collections to be returned ----
		pCookies = (DWORD *)::ATL::AtlCoTaskMemCAlloc(matches,static_cast<ULONG>(sizeof(DWORD)));
		if (! pCookies)
			goto outofmem;

		pStrings = (LPOLESTR *)::ATL::AtlCoTaskMemCAlloc(matches,static_cast<ULONG>(sizeof(LPOLESTR)));
		if (! pStrings)
			goto outofmem;

		//---- second pass thru - collect the items ----
		for (i=0; i < cnt; i++)
		{
			if (dispID == valmap[i].dispid)
			{
				LPCOLESTR src;
				LPOLESTR dst;

				// store cookie
				pCookies[addcnt] = i;
				// allocate and store string
				src = valmap[i].szDesc;
				size_t len = AtlStrLen(src)+1;
				if(len>ULONG_MAX)
				{
					goto outofmem;
				}
				dst = (LPOLESTR)::ATL::AtlCoTaskMemCAlloc(static_cast<ULONG>(len),static_cast<ULONG>(sizeof(WCHAR)));
				if (! dst)
					goto outofmem;

				if(!ocscpy_s(dst, len, src))
				{
					goto outofmem;
				}
				pStrings[addcnt] = dst;
				addcnt++;
			}
		}

		pCaCookiesOut->cElems = matches;
		pCaCookiesOut->pElems = pCookies;
		pCaStringsOut->cElems = matches;
		pCaStringsOut->pElems = pStrings;

		return S_OK;

outofmem:
		CoTaskMemFree(pCookies);
		if (pStrings)
		{
			for (i=0; i < addcnt; i++)
				CoTaskMemFree(pStrings[i]);
			CoTaskMemFree(pStrings);
		}

		return E_OUTOFMEMORY;
	}

	STDMETHOD(GetPredefinedValue)(
		_In_ DISPID /* dispID */,
		_In_ DWORD dwCookie,
		_Out_ VARIANT* pVarOut)
	{
		ATL_PROPVALMAP_ENTRY *valmap;
		int cnt, index;

		ATLTRACE(atlTraceControls,2,_T("IPerPropertyBrowsingImpl::GetPredefinedValue\n"));
		if (pVarOut == NULL)
			return E_POINTER;

		valmap = T::GetPropValMap(&cnt);
		if ((! valmap) || (! cnt))
			ATLTRACENOTIMPL(_T("IPerPropertyBrowsingImpl::GetPredefinedValue"));

		index = (int) dwCookie;
		if ((index < 0) || (index >= cnt))
			return E_INVALIDARG;

		return VariantCopy(pVarOut, &valmap[index].val);
	}
};

//////////////////////////////////////////////////////////////////////////////
// IViewObjectExImpl
template <class T>
class ATL_NO_VTABLE IViewObjectExImpl :
	public IViewObjectEx
{
public:
	STDMETHOD(Draw)(
		_In_ DWORD dwDrawAspect,
		_In_ LONG lindex,
		_In_opt_ void *pvAspect,
		_In_opt_ DVTARGETDEVICE *ptd,
		_In_ HDC hicTargetDev,
		_In_ HDC hdcDraw,
		_In_opt_ LPCRECTL prcBounds,
		_In_ LPCRECTL prcWBounds,
		/* _In_opt_ */ BOOL (__stdcall * /*pfnContinue*/)(DWORD_PTR dwContinue),
		_In_ DWORD_PTR /*dwContinue*/)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::Draw\n"));
		return pT->IViewObject_Draw(dwDrawAspect, lindex, pvAspect, ptd, hicTargetDev, hdcDraw,
			prcBounds, prcWBounds);
	}
	STDMETHOD(GetColorSet)(
		_In_ DWORD /* dwDrawAspect */,
		_In_ LONG /* lindex */,
		_In_opt_ void* /* pvAspect */,
		_In_opt_ DVTARGETDEVICE* /* ptd */,
		_In_ HDC /* hicTargetDev */,
		_In_opt_ LOGPALETTE** /* ppColorSet */)
	{
		ATLTRACENOTIMPL(_T("IViewObjectExImpl::GetColorSet"));
	}
	STDMETHOD(Freeze)(
		_In_ DWORD /* dwDrawAspect */,
		_In_ LONG /* lindex */,
		_In_opt_ void* /* pvAspect */,
		_In_ DWORD* /* pdwFreeze */)
	{
		ATLTRACENOTIMPL(_T("IViewObjectExImpl::Freeze"));
	}
	STDMETHOD(Unfreeze)(_In_ DWORD /* dwFreeze */)
	{
		ATLTRACENOTIMPL(_T("IViewObjectExImpl::Unfreeze"));
	}
	STDMETHOD(SetAdvise)(
		_In_ DWORD /* aspects */,
		_In_ DWORD /* advf */,
		_Inout_ IAdviseSink* pAdvSink)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::SetAdvise\n"));
		pT->m_spAdviseSink = pAdvSink;
		return S_OK;
	}
	STDMETHOD(GetAdvise)(
		_In_opt_  DWORD* /* pAspects */,
		_In_opt_  DWORD* /* pAdvf */,
		_Outptr_result_maybenull_ IAdviseSink** ppAdvSink)
	{
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::GetAdvise\n"));
		ATLASSERT(ppAdvSink != NULL);

		HRESULT hr = E_POINTER;
		if (ppAdvSink != NULL)
		{
			T* pT = static_cast<T*>(this);
			*ppAdvSink = pT->m_spAdviseSink;
			if (pT->m_spAdviseSink)
				pT->m_spAdviseSink.p->AddRef();
			hr = S_OK;
		}
		return hr;
	}

	// IViewObject2
	//
	STDMETHOD(GetExtent)(
		_In_ DWORD /* dwDrawAspect */,
		_In_ LONG /* lindex */,
		_In_ DVTARGETDEVICE* /* ptd */,
		_Out_ LPSIZEL lpsizel)
	{
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::GetExtent\n"));
		ATLASSERT(lpsizel != NULL);
		if (lpsizel == NULL)
			return E_POINTER;

		T* pT = static_cast<T*>(this);
		*lpsizel = pT->m_sizeExtent;
		return S_OK;
	}

	// IViewObjectEx
	//
	STDMETHOD(GetRect)(
		_In_ DWORD /* dwAspect */,
		_In_opt_ LPRECTL /* pRect */)
	{
		ATLTRACENOTIMPL(_T("IViewObjectExImpl::GetRect"));
	}

	STDMETHOD(GetViewStatus)(_Out_ DWORD* pdwStatus)
	{
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::GetViewStatus\n"));
		ATLASSERT(pdwStatus != NULL);
		if (pdwStatus == NULL)
			return E_POINTER;

		T* pT = static_cast<T*>(this);
		*pdwStatus = pT->_GetViewStatus();
		return S_OK;
	}
	STDMETHOD(QueryHitPoint)(
		_In_ DWORD dwAspect,
		_In_ LPCRECT pRectBounds,
		_In_ POINT ptlLoc,
		_In_ LONG /* lCloseHint */,
		_Out_ DWORD *pHitResult)
	{
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::QueryHitPoint\n"));
		ATLASSERT(pHitResult != NULL);
		if (pHitResult == NULL)
			return E_POINTER;

		if (dwAspect == DVASPECT_CONTENT)
		{
			*pHitResult = PtInRect(pRectBounds, ptlLoc) ? HITRESULT_HIT : HITRESULT_OUTSIDE;
			return S_OK;
		}
		*pHitResult = NULL;
		ATLTRACE(atlTraceControls,2,_T("Wrong DVASPECT\n"));
		return E_FAIL;
	}
	STDMETHOD(QueryHitRect)(
		_In_ DWORD dwAspect,
		_In_ LPCRECT pRectBounds,
		_In_ LPCRECT prcLoc,
		_In_ LONG /* lCloseHint */,
		_Out_ DWORD* pHitResult)
	{
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::QueryHitRect\n"));
		ATLASSERT(pHitResult != NULL);
		if (pHitResult == NULL)
			return E_POINTER;

		if (dwAspect == DVASPECT_CONTENT)
		{
			RECT rc;
			*pHitResult = UnionRect(&rc, pRectBounds, prcLoc) ? HITRESULT_HIT : HITRESULT_OUTSIDE;
			return S_OK;
		}
		*pHitResult = NULL;
		ATLTRACE(atlTraceControls,2,_T("Wrong DVASPECT\n"));
		return E_FAIL;
	}
	STDMETHOD(GetNaturalExtent)(
		_In_ DWORD dwAspect,
		_In_ LONG /* lindex */,
		_In_opt_ DVTARGETDEVICE* /* ptd */,
		_In_ HDC /* hicTargetDev */,
		_In_ DVEXTENTINFO* pExtentInfo ,
		_Out_ LPSIZEL psizel)
	{
		ATLTRACE(atlTraceControls,2,_T("IViewObjectExImpl::GetNaturalExtent\n"));
		ATLASSERT(pExtentInfo != NULL);
		ATLASSERT(psizel != NULL);
		if ((pExtentInfo == NULL) || (psizel == NULL))
			return E_POINTER;

		HRESULT hRes = E_FAIL;
		T* pT = static_cast<T*>(this);
		if (dwAspect == DVASPECT_CONTENT)
		{
			if (pExtentInfo->dwExtentMode == DVEXTENT_CONTENT)
			{
				*psizel = pT->m_sizeNatural;
				hRes = S_OK;
			}
		}
		return hRes;
	}

public:
};

//////////////////////////////////////////////////////////////////////////////
// IOleInPlaceObjectWindowlessImpl
//
template <class T>
class ATL_NO_VTABLE IOleInPlaceObjectWindowlessImpl :
	public IOleInPlaceObjectWindowless
{
public:
	// IOleWindow
	//

	// Change IOleInPlaceActiveObject::GetWindow as well
	STDMETHOD(GetWindow)(_Out_ HWND* phwnd)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceObjectWindowlessImpl::GetWindow\n"));
		T* pT = static_cast<T*>(this);
		HRESULT hRes = E_POINTER;

		if (pT->m_bWasOnceWindowless)
			return E_FAIL;

		if (phwnd != NULL)
		{
			*phwnd = pT->m_hWnd;
			hRes = (*phwnd == NULL) ? E_UNEXPECTED : S_OK;
		}
		return hRes;
	}
	STDMETHOD(ContextSensitiveHelp)(_In_ BOOL /* fEnterMode */)
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceObjectWindowlessImpl::ContextSensitiveHelp"));
	}

	// IOleInPlaceObject
	//
	STDMETHOD(InPlaceDeactivate)(void)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceObjectWindowlessImpl::InPlaceDeactivate\n"));
		return pT->IOleInPlaceObject_InPlaceDeactivate();
	}
	STDMETHOD(UIDeactivate)(void)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceObjectWindowlessImpl::UIDeactivate\n"));
		return pT->IOleInPlaceObject_UIDeactivate();
	}
	STDMETHOD(SetObjectRects)(
		_In_ LPCRECT prcPos,
		_In_ LPCRECT prcClip)
	{
		T* pT = static_cast<T*>(this);
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceObjectWindowlessImpl::SetObjectRects\n"));
		return pT->IOleInPlaceObject_SetObjectRects(prcPos, prcClip);
	}
	STDMETHOD(ReactivateAndUndo)(void)
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceObjectWindowlessImpl::ReactivateAndUndo"));
	}

	// IOleInPlaceObjectWindowless
	//
	STDMETHOD(OnWindowMessage)(
		_In_ UINT msg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Inout_ LRESULT *plResult)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceObjectWindowlessImpl::OnWindowMessage\n"));
		T* pT = static_cast<T*>(this);
		_ATL_MSG message(NULL, msg, wParam, lParam);
		const _ATL_MSG* pOldMsg = pT->m_pCurrentMsg;
		pT->m_pCurrentMsg = &message;
		BOOL b = pT->ProcessWindowMessage(pT->m_hWnd, msg, wParam, lParam, *plResult);
		// restore saved value for the current message
		ATLASSERT(pT->m_pCurrentMsg == &message);
		pT->m_pCurrentMsg = pOldMsg;
		return b ? S_OK : S_FALSE;
	}

	STDMETHOD(GetDropTarget)(_In_opt_ IDropTarget** /* ppDropTarget */)
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceObjectWindowlessImpl::GetDropTarget"));
	}
};


//////////////////////////////////////////////////////////////////////////////
// IOleInPlaceActiveObjectImpl
//
template <class T>
class ATL_NO_VTABLE IOleInPlaceActiveObjectImpl :
	public IOleInPlaceActiveObject
{
public:
	// IOleWindow
	//

	// Change IOleInPlaceObjectWindowless::GetWindow as well
	STDMETHOD(GetWindow)(_Out_ HWND *phwnd)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceActiveObjectImpl::GetWindow\n"));
		T* pT = static_cast<T*>(this);
		HRESULT hRes = E_POINTER;

		if (pT->m_bWasOnceWindowless)
			return E_FAIL;

		if (phwnd != NULL)
		{
			*phwnd = pT->m_hWnd;
			hRes = (*phwnd == NULL) ? E_UNEXPECTED : S_OK;
		}
		return hRes;
	}
	STDMETHOD(ContextSensitiveHelp)(_In_ BOOL /* fEnterMode */)
	{
		ATLTRACENOTIMPL(_T("IOleInPlaceActiveObjectImpl::ContextSensitiveHelp"));
	}

	// IOleInPlaceActiveObject
	//
	STDMETHOD(TranslateAccelerator)(_In_ LPMSG pMsg)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceActiveObjectImpl::TranslateAccelerator\n"));
		T* pT = static_cast<T*>(this);
		HRESULT hRet = S_OK;
		MSG msg = *pMsg;
		if (pT->PreTranslateAccelerator(&msg, hRet))
		{
			return hRet;
		}

		CComPtr<IOleControlSite> spCtlSite;
		hRet = pT->InternalGetSite(__uuidof(IOleControlSite), (void**)&spCtlSite);
		if (SUCCEEDED(hRet))
		{
			if (spCtlSite != NULL)
			{
				DWORD dwKeyMod = 0;
				if (::GetKeyState(VK_SHIFT) < 0)
					dwKeyMod += 1;	// KEYMOD_SHIFT
				if (::GetKeyState(VK_CONTROL) < 0)
					dwKeyMod += 2;	// KEYMOD_CONTROL
				if (::GetKeyState(VK_MENU) < 0)
					dwKeyMod += 4;	// KEYMOD_ALT
				hRet = spCtlSite->TranslateAccelerator(&msg, dwKeyMod);
			}
			else
				hRet = S_FALSE;
		}
		return (hRet == S_OK) ? S_OK : S_FALSE;
	}
	STDMETHOD(OnFrameWindowActivate)(_In_ BOOL /* fActivate */)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceActiveObjectImpl::OnFrameWindowActivate\n"));
		return S_OK;
	}
	STDMETHOD(OnDocWindowActivate)(_In_ BOOL fActivate)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceActiveObjectImpl::OnDocWindowActivate\n"));
		T* pT = static_cast<T*>(this);
		if (fActivate == FALSE)
			pT->IOleInPlaceObject_UIDeactivate();
		return S_OK;
	}
	STDMETHOD(ResizeBorder)(
		_In_ LPCRECT /* prcBorder */,
		_In_opt_ IOleInPlaceUIWindow* /* pUIWindow */,
		_In_ BOOL /* fFrameWindow */)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceActiveObjectImpl::ResizeBorder\n"));
		return S_OK;
	}
	STDMETHOD(EnableModeless)(_In_ BOOL /* fEnable */)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleInPlaceActiveObjectImpl::EnableModeless\n"));
		return S_OK;
	}
};

//////////////////////////////////////////////////////////////////////////////
// IPointerInactiveImpl
template <class T>
class ATL_NO_VTABLE IPointerInactiveImpl :
	public IPointerInactive
{
public:
	// IPointerInactive
	//
	STDMETHOD(GetActivationPolicy)(_In_opt_ DWORD* /*pdwPolicy*/)
	{
		ATLTRACENOTIMPL(_T("IPointerInactiveImpl::GetActivationPolicy"));
	}
	STDMETHOD(OnInactiveMouseMove)(
		_In_ LPCRECT /*pRectBounds*/,
		_In_ long /*x*/,
		_In_ long /*y*/,
		_In_ DWORD /*dwMouseMsg*/)
	{
		ATLTRACENOTIMPL(_T("IPointerInactiveImpl::OnInactiveMouseMove"));
	}
	STDMETHOD(OnInactiveSetCursor)(
		_In_ LPCRECT /*pRectBounds*/,
		_In_ long /*x*/,
		_In_ long /*y*/,
		_In_ DWORD /*dwMouseMsg*/,
		_In_ BOOL /*fSetAlways*/)
	{
		ATLTRACENOTIMPL(_T("IPointerInactiveImpl::OnInactiveSetCursor"));
	}
};

//////////////////////////////////////////////////////////////////////////////
// IRunnableObjectImpl
template <class T>
class ATL_NO_VTABLE IRunnableObjectImpl :
	public IRunnableObject
{
public:
	// IRunnableObject
	//
	STDMETHOD(GetRunningClass)(_Out_ LPCLSID lpClsid)
	{
		ATLTRACE(atlTraceControls,2,_T("IRunnableObjectImpl::GetRunningClass\n"));
		*lpClsid = GUID_NULL;
		return E_UNEXPECTED;
	}
	STDMETHOD(Run)(_In_opt_ LPBINDCTX)
	{
		ATLTRACE(atlTraceControls,2,_T("IRunnableObjectImpl::Run\n"));
		return S_OK;
	}
	virtual BOOL STDMETHODCALLTYPE IsRunning()
	{
		ATLTRACE(atlTraceControls,2,_T("IRunnableObjectImpl::IsRunning\n"));
		return TRUE;
	}
	STDMETHOD(LockRunning)(
		_In_ BOOL /*fLock*/,
		_In_ BOOL /*fLastUnlockCloses*/)
	{
		ATLTRACE(atlTraceControls,2,_T("IRunnableObjectImpl::LockRunning\n"));
		return S_OK;
	}
	STDMETHOD(SetContainedObject)(_In_ BOOL /*fContained*/)
	{
		ATLTRACE(atlTraceControls,2,_T("IRunnableObjectImpl::SetContainedObject\n"));
		return S_OK;
	}
};


//////////////////////////////////////////////////////////////////////////////
// IDataObjectImpl
ATLPREFAST_SUPPRESS(6387)
template <class T>
class ATL_NO_VTABLE IDataObjectImpl :
	public IDataObject
{
public:
	STDMETHOD(GetData)(
		_In_ FORMATETC *pformatetcIn,
		_Out_ STGMEDIUM *pmedium)
	{
		ATLTRACE(atlTraceControls,2,_T("IDataObjectImpl::GetData\n"));
		T* pT = (T*) this;
		return pT->IDataObject_GetData(pformatetcIn, pmedium);
	}
	STDMETHOD(GetDataHere)(
		_In_opt_ FORMATETC* /* pformatetc */,
		_In_opt_ /* _Inout_ */ STGMEDIUM* /* pmedium */)
	{
		ATLTRACENOTIMPL(_T("IDataObjectImpl::GetDataHere"));
	}
	STDMETHOD(QueryGetData)(_In_opt_ FORMATETC* /* pformatetc */)
	{
		ATLTRACENOTIMPL(_T("IDataObjectImpl::QueryGetData"));
	}
	STDMETHOD(GetCanonicalFormatEtc)(
		_In_opt_ FORMATETC* /* pformatectIn */,
		_In_opt_ FORMATETC* /* pformatetcOut */)
	{
		ATLTRACENOTIMPL(_T("IDataObjectImpl::GetCanonicalFormatEtc"));
	}
	STDMETHOD(SetData)(
		_In_opt_ FORMATETC* /* pformatetc */,
		_In_opt_ STGMEDIUM* /* pmedium */,
		_In_ BOOL /* fRelease */)
	{
		ATLTRACENOTIMPL(_T("IDataObjectImpl::SetData"));
	}
	STDMETHOD(EnumFormatEtc)(
		_In_ DWORD /* dwDirection */,
		_In_opt_ IEnumFORMATETC** /* ppenumFormatEtc */)
	{
		ATLTRACENOTIMPL(_T("IDataObjectImpl::EnumFormatEtc"));
	}
	STDMETHOD(DAdvise)(
		_In_ FORMATETC *pformatetc,
		_In_ DWORD advf,
		_Inout_ IAdviseSink *pAdvSink,
		_Out_ DWORD *pdwConnection)
	{
		ATLTRACE(atlTraceControls,2,_T("IDataObjectImpl::DAdvise\n"));
		T* pT = static_cast<T*>(this);
		HRESULT hr = S_OK;
		if (pT->m_spDataAdviseHolder == NULL)
			hr = CreateDataAdviseHolder(&pT->m_spDataAdviseHolder);

		if (hr == S_OK)
			hr = pT->m_spDataAdviseHolder->Advise((IDataObject*)this, pformatetc, advf, pAdvSink, pdwConnection);

		return hr;
	}
	STDMETHOD(DUnadvise)(_In_ DWORD dwConnection)
	{
		ATLTRACE(atlTraceControls,2,_T("IDataObjectImpl::DUnadvise\n"));
		T* pT = static_cast<T*>(this);
		HRESULT hr = S_OK;
		if (pT->m_spDataAdviseHolder == NULL)
			hr = OLE_E_NOCONNECTION;
		else
			hr = pT->m_spDataAdviseHolder->Unadvise(dwConnection);
		return hr;
	}

	STDMETHOD(EnumDAdvise)(_Outptr_ IEnumSTATDATA **ppenumAdvise)
	{
		ATLTRACE(atlTraceControls,2,_T("IDataObjectImpl::EnumDAdvise\n"));
		ATLASSERT(ppenumAdvise != NULL);
		if (ppenumAdvise == NULL)
			return E_POINTER;
		*ppenumAdvise = NULL;

		T* pT = static_cast<T*>(this);
		if (pT->m_spDataAdviseHolder != NULL)
			return pT->m_spDataAdviseHolder->EnumAdvise(ppenumAdvise);
		return E_FAIL;
	}
};
ATLPREFAST_UNSUPPRESS()

//////////////////////////////////////////////////////////////////////////////
// IPropertyNotifySinkCP
template <class T, class CDV = CComDynamicUnkArray >
class ATL_NO_VTABLE IPropertyNotifySinkCP :
	public IConnectionPointImpl<T, &IID_IPropertyNotifySink, CDV>
{
public:
	typedef CFirePropNotifyEvent __ATL_PROP_NOTIFY_EVENT_CLASS;
};


//////////////////////////////////////////////////////////////////////////////
// IObjectSafety
//
// 2nd template parameter is the supported safety e.g.
// INTERFACESAFE_FOR_UNTRUSTED_CALLER - safe for scripting
// INTERFACESAFE_FOR_UNTRUSTED_DATA   - safe for initialization from data
template <class T, DWORD dwSupportedSafety>
class ATL_NO_VTABLE IObjectSafetyImpl :
	public IObjectSafety
{
protected:
	virtual BOOL FAccessAllowed()
	{
		return TRUE;
	}
public:
	IObjectSafetyImpl() : m_dwCurrentSafety(0)
	{
		ATLSTATIC_ASSERT((dwSupportedSafety != 0 &&
				(~(INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA) & dwSupportedSafety) == 0),
					"Only INTERFACESAFE_FOR_UNTRUSTED_CALLER and/or INTERFACESAFE_FOR_UNTRUSTED_DATA are supported for dwSupportedSafety parameter.");
	}

	STDMETHOD(GetInterfaceSafetyOptions)(
		_In_ REFIID riid,
		_Out_ DWORD *pdwSupportedOptions,
		_Out_ DWORD *pdwEnabledOptions)
	{
		ATLTRACE(atlTraceControls,2,_T("IObjectSafetyImpl::GetInterfaceSafetyOptions\n"));
		T* pT = static_cast<T*>(this);
		if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
			return E_POINTER;

		HRESULT hr;
		IUnknown* pUnk = NULL;
		// Check if we support this interface
		hr = pT->GetUnknown()->QueryInterface(riid, (void**)&pUnk);
		if (SUCCEEDED(hr))
		{
			// We support this interface so set the safety options accordingly
			pUnk->Release();	// Release the interface we just acquired
			*pdwSupportedOptions = dwSupportedSafety;

			if (FAccessAllowed())
			{
				*pdwEnabledOptions   = m_dwCurrentSafety;
			}
			else
			{
				*pdwEnabledOptions   = 0;
			}
		}
		else
		{
			// We don't support this interface
			*pdwSupportedOptions = 0;
			*pdwEnabledOptions   = 0;
		}
		return hr;
	}
	STDMETHOD(SetInterfaceSafetyOptions)(
		_In_ REFIID riid,
		_In_ DWORD dwOptionSetMask,
		_In_ DWORD dwEnabledOptions)
	{
		ATLTRACE(atlTraceControls,2,_T("IObjectSafetyImpl::SetInterfaceSafetyOptions\n"));
		T* pT = static_cast<T*>(this);
		IUnknown* pUnk = NULL;

		// Check if we support the interface and return E_NOINTEFACE if we don't
		if (FAILED(pT->GetUnknown()->QueryInterface(riid, (void**)&pUnk)))
			return E_NOINTERFACE;
		pUnk->Release();	// Release the interface we just acquired

		// If we are asked to set options we don't support then fail
		if (dwOptionSetMask & ~dwSupportedSafety)
			return E_FAIL;

		DWORD dwNewSafety = (m_dwCurrentSafety & ~dwOptionSetMask) | (dwOptionSetMask & dwEnabledOptions);

		if (m_dwCurrentSafety == dwNewSafety)
		{
			return S_OK;
		}

		if (!FAccessAllowed())
		{
			return E_ACCESSDENIED;
		}
		// Set the safety options we have been asked to
		m_dwCurrentSafety = dwNewSafety;

		return S_OK;
	}
	DWORD m_dwCurrentSafety;
};

template <class T>
class ATL_NO_VTABLE IOleLinkImpl :
	public IOleLink
{
	STDMETHOD(SetUpdateOptions)(_In_ DWORD /* dwUpdateOpt */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::SetUpdateOptions"));
	}

	STDMETHOD(GetUpdateOptions)(_In_ DWORD* /* pdwUpdateOpt */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::GetUpdateOptions"));
	}

	STDMETHOD(SetSourceMoniker)(
		_Inout_opt_ IMoniker* /* pmk */,
		_In_ REFCLSID /* rclsid */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::SetSourceMoniker"));
	}

	STDMETHOD(GetSourceMoniker)(_In_opt_ IMoniker** /* ppmk */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::GetSourceMoniker"));
	};

	STDMETHOD(SetSourceDisplayName)(_In_opt_z_ LPCOLESTR /* pszStatusText */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::SetSourceDisplayName"));
	}

	STDMETHOD(GetSourceDisplayName)(
		_Outptr_result_maybenull_z_ LPOLESTR *ppszDisplayName)
	{
		ATLTRACE(atlTraceControls,2,_T("IOleLink::GetSourceDisplayName\n"));
		*ppszDisplayName = NULL;
		return E_FAIL;
	}

	STDMETHOD(BindToSource)(
		_In_ DWORD /* bindflags */,
		_In_opt_ IBindCtx* /* pbc */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::BindToSource\n"));
	};

	STDMETHOD(BindIfRunning)()
	{
		ATLTRACE(atlTraceControls,2,_T("IOleLinkImpl::BindIfRunning\n"));
		return S_OK;
	};

	STDMETHOD(GetBoundSource)(_In_opt_ IUnknown** /* ppunk */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::GetBoundSource"));
	};

	STDMETHOD(UnbindSource)()
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::UnbindSource"));
	};

	STDMETHOD(Update)(_In_opt_ IBindCtx* /* pbc */)
	{
		ATLTRACENOTIMPL(_T("IOleLinkImpl::Update"));
	};
};

ATLPREFAST_SUPPRESS(6011)
template <class T, int nBindFlags = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE>
class ATL_NO_VTABLE CBindStatusCallback :
	public CComObjectRootEx<typename T::_ThreadModel::ThreadModelNoCS>,
	public IBindStatusCallback
{
public:
	typedef void (T::*ATL_PDATAAVAILABLE)(CBindStatusCallback<T, nBindFlags>* pbsc, BYTE* pBytes, DWORD dwSize);
	typedef CBindStatusCallback<T, nBindFlags> thisClass;

BEGIN_COM_MAP(thisClass)
	COM_INTERFACE_ENTRY(IBindStatusCallback)
END_COM_MAP()

	CBindStatusCallback()
	{
		m_pT = NULL;
		m_pFunc = NULL;
	}
	virtual ~CBindStatusCallback()
	{
		ATLTRACE(atlTraceControls,2,_T("~CBindStatusCallback\n"));
	}

	STDMETHOD(OnStartBinding)(
		DWORD /*dwReserved*/,
		_In_opt_ IBinding *pBinding)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnStartBinding\n"));
		m_spBinding = pBinding;
		return S_OK;
	}

	STDMETHOD(GetPriority)(_Out_ LONG *pnPriority)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::GetPriority"));
		HRESULT hr = S_OK;
		if (pnPriority)
			*pnPriority = THREAD_PRIORITY_NORMAL;
		else
			hr = E_INVALIDARG;
		return S_OK;
	}

	STDMETHOD(OnLowResource)(DWORD /*reserved*/)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnLowResource"));
		return S_OK;
	}

	STDMETHOD(OnProgress)(
		ULONG /*ulProgress*/,
		ULONG /*ulProgressMax*/,
		ULONG /*ulStatusCode*/,
		_In_opt_z_ LPCWSTR /*szStatusText*/)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnProgress"));
		return S_OK;
	}

	STDMETHOD(OnStopBinding)(
		_In_ HRESULT hresult,
		_In_opt_z_ LPCWSTR /*szError*/)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnStopBinding\n"));
		// Pass NULL as the array of bytes to signify the end.
		// Pass the HRESULT for the dwSize parameter
		(m_pT->*m_pFunc)(this, NULL, hresult);
		m_spBinding.Release();
		m_spBindCtx.Release();
		m_spMoniker.Release();
		return S_OK;
	}

	STDMETHOD(GetBindInfo)(
		DWORD *pgrfBINDF,
		BINDINFO *pbindInfo)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::GetBindInfo\n"));

		if (pbindInfo==NULL || pbindInfo->cbSize==0 || pgrfBINDF==NULL)
			return E_INVALIDARG;

		*pgrfBINDF = nBindFlags;

		ULONG cbSize = pbindInfo->cbSize;		  // remember incoming cbSize
		memset(pbindInfo, 0, sizeof(*pbindInfo)); // zero out structure
		pbindInfo->cbSize = cbSize;				  // restore cbSize
		pbindInfo->dwBindVerb = BINDVERB_GET;	  // set verb

		return S_OK;
	}

	STDMETHOD(OnDataAvailable)(
		DWORD grfBSCF,
		DWORD dwSize,
		FORMATETC * /*pformatetc*/,
		STGMEDIUM *pstgmed)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnDataAvailable\n"));
		HRESULT hr = S_OK;

		// Get the Stream passed
		if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)
		{
			if (!m_spStream && pstgmed->tymed == TYMED_ISTREAM)
				m_spStream = pstgmed->pstm;
		}

		DWORD dwRead = dwSize - m_dwTotalRead; // Minimum amount available that hasn't been read
		DWORD dwActuallyRead = 0;            // Placeholder for amount read during this pull

		// If there is some data to be read then go ahead and read them
		if (m_spStream)
		{
			if (dwRead > 0)
			{
				BYTE* pBytes = _ATL_NEW BYTE[dwRead + 1];
				if (pBytes == NULL)
				{
					// Before returning m_spStream should be released if grfBSCF is BSCF_LASTDATANOTIFICATION
					hr = E_OUTOFMEMORY;
				}
				else
				{
					hr = m_spStream->Read(pBytes, dwRead, &dwActuallyRead);
					if (SUCCEEDED(hr))
					{
						pBytes[dwActuallyRead] = 0;
						if (dwActuallyRead>0)
						{
							(m_pT->*m_pFunc)(this, pBytes, dwActuallyRead);
							m_dwTotalRead += dwActuallyRead;
						}
					}
					delete[] pBytes;
				}
			}
		}

		if (BSCF_LASTDATANOTIFICATION & grfBSCF)
			m_spStream.Release();
		return hr;
	}

	STDMETHOD(OnObjectAvailable)(
		_In_ REFIID /*riid*/,
		_In_opt_ IUnknown * /*punk*/)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnObjectAvailable"));
		return S_OK;
	}

	HRESULT _StartAsyncDownload(
		_In_z_ BSTR bstrURL,
		_Inout_opt_ IUnknown* pUnkContainer,
		_In_ BOOL bRelative)
	{
		m_dwTotalRead = 0;
		m_dwAvailableToRead = 0;
		HRESULT hr = S_OK;
		CComQIPtr<IServiceProvider, &__uuidof(IServiceProvider)> spServiceProvider(pUnkContainer);
		CComPtr<IBindHost>	spBindHost;
		CComPtr<IStream>	spStream;
		CComPtr<IBindStatusCallback>	spPrevCallback;
		if (spServiceProvider)
			spServiceProvider->QueryService(SID_IBindHost, __uuidof(IBindHost), (void**)&spBindHost);

		if (spBindHost == NULL)
		{
			if (bRelative)
				return E_NOINTERFACE;  // relative asked for, but no IBindHost
			hr = CreateURLMoniker(NULL, bstrURL, &m_spMoniker);
			if (SUCCEEDED(hr))
				hr = CreateBindCtx(0, &m_spBindCtx);

			if (SUCCEEDED(hr))
				hr = RegisterBindStatusCallback(m_spBindCtx, static_cast<IBindStatusCallback*>(this), &spPrevCallback, 0L);
			else
				m_spMoniker.Release();

			if (SUCCEEDED(hr))
				hr = m_spMoniker->BindToStorage(m_spBindCtx, 0, __uuidof(IStream), (void**)&spStream);
		}
		else
		{
			hr = CreateBindCtx(0, &m_spBindCtx);
			if (SUCCEEDED(hr))
				hr = RegisterBindStatusCallback(m_spBindCtx, static_cast<IBindStatusCallback*>(this), &spPrevCallback, 0L);

			if (SUCCEEDED(hr))
			{
				if (bRelative)
					hr = spBindHost->CreateMoniker(bstrURL, m_spBindCtx, &m_spMoniker, 0);
				else
					hr = CreateURLMoniker(NULL, bstrURL, &m_spMoniker);
			}

			if (SUCCEEDED(hr))
			{
				hr = spBindHost->MonikerBindToStorage(m_spMoniker, m_spBindCtx, static_cast<IBindStatusCallback*>(this), __uuidof(IStream), (void**)&spStream);
				ATLTRACE(atlTraceControls,2,_T("Bound"));
			}
		}
		return hr;
	}

	HRESULT StartAsyncDownload(
		_In_ T* pT,
		_In_ ATL_PDATAAVAILABLE pFunc,
		_In_z_ BSTR bstrURL,
		_Inout_opt_ IUnknown* pUnkContainer = NULL,
		_In_ BOOL bRelative = FALSE)
	{
		m_pT = pT;
		m_pFunc = pFunc;
		return  _StartAsyncDownload(bstrURL, pUnkContainer, bRelative);
	}

	static HRESULT Download(
		_In_ T* pT,
		_In_ ATL_PDATAAVAILABLE pFunc,
		_In_z_ BSTR bstrURL,
		_Inout_opt_ IUnknown* pUnkContainer = NULL,
		_In_ BOOL bRelative = FALSE)
	{
		CComObject<CBindStatusCallback<T, nBindFlags> > *pbsc;
		HRESULT hRes = CComObject<CBindStatusCallback<T, nBindFlags> >::CreateInstance(&pbsc);
		if (FAILED(hRes))
			return hRes;

		return pbsc->StartAsyncDownload(pT, pFunc, bstrURL, pUnkContainer, bRelative);
	}

	CComPtr<IMoniker> m_spMoniker;
	CComPtr<IBindCtx> m_spBindCtx;
	CComPtr<IBinding> m_spBinding;
	CComPtr<IStream> m_spStream;
	T* m_pT;
	ATL_PDATAAVAILABLE m_pFunc;
	DWORD m_dwTotalRead;
	DWORD m_dwAvailableToRead;
};
ATLPREFAST_UNSUPPRESS()

#define IMPLEMENT_STOCKPROP(type, fname, pname, dispid) \
	HRESULT STDMETHODCALLTYPE put_##fname(_In_ type pname) \
	{ \
		__if_exists(T::m_##pname) \
		{ \
			ATLTRACE(ATL::atlTraceControls,2,_T("CStockPropImpl::put_%Ts\n"), #fname); \
			T* pT = (T*) this; \
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(dispid) == S_FALSE) \
				return S_FALSE; \
			pT->m_##pname = pname; \
			pT->m_bRequiresSave = TRUE; \
			if (pT->m_nFreezeEvents == 0) \
				pT->FireOnChanged(dispid); \
			__if_exists(T::On##fname##Changed) \
			{ \
				pT->On##fname##Changed(); \
			} \
			pT->FireViewChange(); \
			pT->SendOnDataChange(NULL); \
		} \
		return S_OK; \
	} \
	HRESULT STDMETHODCALLTYPE get_##fname(_Out_ type* p##pname) \
	{ \
		__if_exists(T::m_##pname) \
		{ \
			ATLTRACE(ATL::atlTraceControls,2,_T("CStockPropImpl::get_%Ts\n"), #fname); \
			ATLASSERT(p##pname != NULL); \
			if (p##pname == NULL) \
				return E_POINTER; \
			T* pT = (T*) this; \
			*p##pname = pT->m_##pname; \
		} \
		return S_OK; \
	}

#define IMPLEMENT_BOOL_STOCKPROP(fname, pname, dispid) \
	HRESULT STDMETHODCALLTYPE put_##fname(_In_ VARIANT_BOOL pname) \
	{ \
		__if_exists(T::m_##pname) \
		{ \
			ATLTRACE(ATL::atlTraceControls,2,_T("CStockPropImpl::put_%Ts\n"), #fname); \
			T* pT = (T*) this; \
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(dispid) == S_FALSE) \
				return S_FALSE; \
			pT->m_##pname = pname; \
			pT->m_bRequiresSave = TRUE; \
			if (pT->m_nFreezeEvents == 0) \
				pT->FireOnChanged(dispid); \
			__if_exists(T::On##fname##Changed) \
			{ \
				pT->On##fname##Changed(); \
			} \
			pT->FireViewChange(); \
			pT->SendOnDataChange(NULL); \
		} \
		return S_OK; \
	} \
	HRESULT STDMETHODCALLTYPE get_##fname(_Out_ VARIANT_BOOL* p##pname) \
	{ \
		__if_exists(T::m_##pname) \
		{ \
			ATLTRACE(ATL::atlTraceControls,2,_T("CStockPropImpl::get_%Ts\n"), #fname); \
			ATLASSERT(p##pname != NULL); \
			if (p##pname == NULL) \
				return E_POINTER; \
			T* pT = (T*) this; \
			*p##pname = pT->m_##pname ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE; \
		} \
		return S_OK; \
	}

#define IMPLEMENT_BSTR_STOCKPROP(fname, pname, dispid) \
	HRESULT STDMETHODCALLTYPE put_##fname(_In_z_ BSTR pname) \
	{ \
		__if_exists(T::m_##pname) \
		{ \
			ATLTRACE(ATL::atlTraceControls,2,_T("CStockPropImpl::put_%Ts\n"), #fname); \
			T* pT = (T*) this; \
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(dispid) == S_FALSE) \
				return S_FALSE; \
			SysFreeStringHelper(pT->m_##pname); \
			HRESULT hr=SysAllocStringHelper(pT->m_##pname,pname); \
			if (FAILED(hr) && pname != NULL) \
				return E_OUTOFMEMORY; \
			pT->m_bRequiresSave = TRUE; \
			if (pT->m_nFreezeEvents == 0) \
				pT->FireOnChanged(dispid); \
			__if_exists(T::On##fname##Changed) \
			{ \
				pT->On##fname##Changed(); \
			} \
			pT->FireViewChange(); \
			pT->SendOnDataChange(NULL); \
		} \
		return S_OK; \
	} \
	HRESULT STDMETHODCALLTYPE get_##fname(_Out_ BSTR* p##pname) \
	{ \
		__if_exists(T::m_##pname) \
		{ \
			ATLTRACE(ATL::atlTraceControls,2,_T("CStockPropImpl::get_%Ts\n"), #fname); \
			ATLASSERT(p##pname != NULL); \
			if (p##pname == NULL) \
				return E_POINTER; \
			T* pT = (T*) this; \
			*p##pname = SysAllocString(pT->m_##pname); \
			if (*p##pname == NULL && pT->m_##pname != NULL) \
				return E_OUTOFMEMORY; \
		} \
		return S_OK; \
	}


template < class T, class InterfaceName, const IID* piid = &_ATL_IIDOF(InterfaceName), const GUID* plibid = &CAtlModule::m_libid, WORD wMajor = 1,
WORD wMinor = 0, class tihclass = CComTypeInfoHolder>
class ATL_NO_VTABLE CStockPropImpl :
	public IDispatchImpl< InterfaceName, piid, plibid, wMajor, wMinor, tihclass >
{
public:
	// Font
	HRESULT STDMETHODCALLTYPE put_Font(_In_ IFontDisp* pFontDisp)
	{
		__if_exists(T::m_pFont)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::put_Font\n"));
			T* pT = (T*) this;
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(DISPID_FONT) == S_FALSE)
				return S_FALSE;
			pT->m_pFont = 0;
			if (pFontDisp)
			{
				CComQIPtr<IFont, &__uuidof(IFont)> p(pFontDisp);
				if (p)
				{
					CComPtr<IFont> pFont;
					p->Clone(&pFont);
					if (pFont)
					{
						pFont->QueryInterface(__uuidof(IFontDisp), (void**) &pT->m_pFont);
					}
				}
			}
			pT->m_bRequiresSave = TRUE;
			if (pT->m_nFreezeEvents == 0)
				pT->FireOnChanged(DISPID_FONT);
			__if_exists(T::OnFontChanged)
			{
				pT->OnFontChanged();
			}
			pT->FireViewChange();
			pT->SendOnDataChange(NULL);
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE putref_Font(_In_ IFontDisp* pFont)
	{
		__if_exists(T::m_pFont)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::putref_Font\n"));
			T* pT = (T*) this;
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(DISPID_FONT) == S_FALSE)
				return S_FALSE;
			pT->m_pFont = pFont;
			pT->m_bRequiresSave = TRUE;
			if (pT->m_nFreezeEvents == 0)
				pT->FireOnChanged(DISPID_FONT);
			__if_exists(T::OnFontChanged)
			{
				pT->OnFontChanged();
			}
			pT->FireViewChange();
			pT->SendOnDataChange(NULL);
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE get_Font(
		_Outptr_result_maybenull_ IFontDisp** ppFont)
	{
		__if_exists(T::m_pFont)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::get_Font\n"));
			ATLASSERT(ppFont != NULL);
			if (ppFont == NULL)
				return E_POINTER;

			T* pT = (T*) this;
			*ppFont = pT->m_pFont;
			if (*ppFont != NULL)
				(*ppFont)->AddRef();
		}
		return S_OK;
	}
	// Picture
	HRESULT STDMETHODCALLTYPE put_Picture(_In_ IPictureDisp* pPicture)
	{
		__if_exists(T::m_pPicture)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::put_Picture\n"));
			T* pT = (T*) this;
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(DISPID_PICTURE) == S_FALSE)
				return S_FALSE;
			pT->m_pPicture = 0;
			if (pPicture)
			{
				CComQIPtr<IPersistStream, &__uuidof(IPersistStream)> p(pPicture);
				if (p)
				{
					ULARGE_INTEGER l;
					p->GetSizeMax(&l);
					HGLOBAL hGlob = GlobalAlloc(GHND, l.LowPart);
					if (hGlob)
					{
						CComPtr<IStream> spStream;
						CreateStreamOnHGlobal(hGlob, TRUE, &spStream);
						if (spStream)
						{
							if (SUCCEEDED(p->Save(spStream, FALSE)))
							{
								LARGE_INTEGER l;
								l.QuadPart = 0;
								spStream->Seek(l, STREAM_SEEK_SET, NULL);
								OleLoadPicture(spStream, l.LowPart, FALSE, __uuidof(IPictureDisp), (void**)&pT->m_pPicture);
							}
							spStream.Release();
						}
						GlobalFree(hGlob);
					}
				}
			}
			pT->m_bRequiresSave = TRUE;
			if (pT->m_nFreezeEvents == 0)
				pT->FireOnChanged(DISPID_PICTURE);
			__if_exists(T::OnPictureChanged)
			{
				pT->OnPictureChanged();
			}
			pT->FireViewChange();
			pT->SendOnDataChange(NULL);
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE putref_Picture(_In_ IPictureDisp* pPicture)
	{
		__if_exists(T::m_pPicture)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::putref_Picture\n"));
			T* pT = (T*) this;
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(DISPID_PICTURE) == S_FALSE)
				return S_FALSE;
			pT->m_pPicture = pPicture;
			pT->m_bRequiresSave = TRUE;
			if (pT->m_nFreezeEvents == 0)
				pT->FireOnChanged(DISPID_PICTURE);
			__if_exists(T::OnPictureChanged)
			{
				pT->OnPictureChanged();
			}
			pT->FireViewChange();
			pT->SendOnDataChange(NULL);
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE get_Picture(
		_Outptr_result_maybenull_ IPictureDisp** ppPicture)
	{
		__if_exists(T::m_pPicture)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::get_Picture\n"));
			ATLASSERT(ppPicture != NULL);
			if (ppPicture == NULL)
				return E_POINTER;

			T* pT = (T*) this;
			*ppPicture = pT->m_pPicture;
			if (*ppPicture != NULL)
				(*ppPicture)->AddRef();
		}
		return S_OK;
	}
	// MouseIcon
	HRESULT STDMETHODCALLTYPE put_MouseIcon(_In_ IPictureDisp* pPicture)
	{
		__if_exists(T::m_pMouseIcon)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::put_MouseIcon\n"));
			T* pT = (T*) this;
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(DISPID_MOUSEICON) == S_FALSE)
				return S_FALSE;
			pT->m_pMouseIcon = 0;
			if (pPicture)
			{
				CComQIPtr<IPersistStream, &__uuidof(IPersistStream)> p(pPicture);
				if (p)
				{
					ULARGE_INTEGER l;
					p->GetSizeMax(&l);
					HGLOBAL hGlob = GlobalAlloc(GHND, l.LowPart);
					if (hGlob)
					{
						CComPtr<IStream> spStream;
						CreateStreamOnHGlobal(hGlob, TRUE, &spStream);
						if (spStream)
						{
							if (SUCCEEDED(p->Save(spStream, FALSE)))
							{
								LARGE_INTEGER l;
								l.QuadPart = 0;
								spStream->Seek(l, STREAM_SEEK_SET, NULL);
								OleLoadPicture(spStream, l.LowPart, FALSE, __uuidof(IPictureDisp), (void**)&pT->m_pMouseIcon);
							}
							spStream.Release();
						}
						GlobalFree(hGlob);
					}
				}
			}
			pT->m_bRequiresSave = TRUE;
			if (pT->m_nFreezeEvents == 0)
				pT->FireOnChanged(DISPID_MOUSEICON);
			__if_exists(T::OnMouseIconChanged)
			{
				pT->OnMouseIconChanged();
			}
			pT->FireViewChange();
			pT->SendOnDataChange(NULL);
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE putref_MouseIcon(_In_ IPictureDisp* pPicture)
	{
		__if_exists(T::m_pMouseIcon)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::putref_MouseIcon\n"));
			T* pT = (T*) this;
			if (pT->m_nFreezeEvents == 0 && pT->FireOnRequestEdit(DISPID_MOUSEICON) == S_FALSE)
				return S_FALSE;
			pT->m_pMouseIcon = pPicture;
			pT->m_bRequiresSave = TRUE;
			if (pT->m_nFreezeEvents == 0)
				pT->FireOnChanged(DISPID_MOUSEICON);
			__if_exists(T::OnMouseIconChanged)
			{
				pT->OnMouseIconChanged();
			}
			pT->FireViewChange();
			pT->SendOnDataChange(NULL);
		}
		return S_OK;
	}
	HRESULT STDMETHODCALLTYPE get_MouseIcon(
		_Outptr_result_maybenull_ IPictureDisp** ppPicture)
	{
		__if_exists(T::m_pMouseIcon)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::get_MouseIcon\n"));
			ATLASSERT(ppPicture != NULL);
			if (ppPicture == NULL)
				return E_POINTER;

			T* pT = (T*) this;
			*ppPicture = pT->m_pMouseIcon;
			if (*ppPicture != NULL)
				(*ppPicture)->AddRef();
		}
		return S_OK;
	}
	IMPLEMENT_STOCKPROP(OLE_COLOR, BackColor, clrBackColor, DISPID_BACKCOLOR)
	IMPLEMENT_STOCKPROP(OLE_COLOR, BorderColor, clrBorderColor, DISPID_BORDERCOLOR)
	IMPLEMENT_STOCKPROP(OLE_COLOR, FillColor, clrFillColor, DISPID_FILLCOLOR)
	IMPLEMENT_STOCKPROP(OLE_COLOR, ForeColor, clrForeColor, DISPID_FORECOLOR)
	IMPLEMENT_BOOL_STOCKPROP(AutoSize, bAutoSize, DISPID_AUTOSIZE)
	IMPLEMENT_BOOL_STOCKPROP(Valid, bValid, DISPID_VALID)
	IMPLEMENT_BOOL_STOCKPROP(Enabled, bEnabled, DISPID_ENABLED)
	IMPLEMENT_BOOL_STOCKPROP(TabStop, bTabStop, DISPID_TABSTOP)
	IMPLEMENT_BOOL_STOCKPROP(BorderVisible, bBorderVisible, DISPID_BORDERVISIBLE)
	IMPLEMENT_BSTR_STOCKPROP(Text, bstrText, DISPID_TEXT)
	IMPLEMENT_BSTR_STOCKPROP(Caption, bstrCaption, DISPID_CAPTION)
	HRESULT STDMETHODCALLTYPE put_Window(_In_ LONG_PTR hWnd)
	{
		return put_HWND(hWnd);
	}
	HRESULT STDMETHODCALLTYPE get_Window(_Out_ LONG_PTR* phWnd)
	{
		return get_HWND(phWnd);
	}
	HRESULT STDMETHODCALLTYPE put_HWND(_In_ LONG_PTR /*hWnd*/)
	{
		ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::put_HWND\n"));
		return E_FAIL;
	}
	HRESULT STDMETHODCALLTYPE get_HWND(_Out_ LONG_PTR* phWnd)
	{
		__if_exists(T::m_hWnd)
		{
			ATLTRACE(atlTraceControls,2,_T("CStockPropImpl::get_HWND\n"));
			ATLASSERT(phWnd != NULL);
			if (phWnd == NULL)
				return E_POINTER;
			T* pT = (T*) this;
			*phWnd = reinterpret_cast<LONG_PTR>(pT->m_hWnd);
		}
		return S_OK;
	}
	IMPLEMENT_STOCKPROP(LONG, BackStyle, nBackStyle, DISPID_BACKSTYLE)
	IMPLEMENT_STOCKPROP(LONG, BorderStyle, nBorderStyle, DISPID_BORDERSTYLE)
	IMPLEMENT_STOCKPROP(LONG, BorderWidth, nBorderWidth, DISPID_BORDERWIDTH)
	IMPLEMENT_STOCKPROP(LONG, DrawMode, nDrawMode, DISPID_DRAWMODE)
	IMPLEMENT_STOCKPROP(LONG, DrawStyle, nDrawStyle, DISPID_DRAWSTYLE)
	IMPLEMENT_STOCKPROP(LONG, DrawWidth, nDrawWidth, DISPID_DRAWWIDTH)
	IMPLEMENT_STOCKPROP(LONG, FillStyle, nFillStyle, DISPID_FILLSTYLE)
	IMPLEMENT_STOCKPROP(SHORT, Appearance, nAppearance, DISPID_APPEARANCE)
	IMPLEMENT_STOCKPROP(LONG, MousePointer, nMousePointer, DISPID_MOUSEPOINTER)
	IMPLEMENT_STOCKPROP(LONG, ReadyState, nReadyState, DISPID_READYSTATE)
};

#pragma pack(pop)

}; //namespace ATL

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (pop)
#endif //!_ATL_NO_PRAGMA_WARNINGS

#endif // _ATLCTL_IMPL
