// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLEVENT_H__
#define __ATLEVENT_H__

#define __ATLEVENT_VER 3

#pragma once

#ifndef __cplusplus
		#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <new.h>
#include <atlwinverapi.h>

struct __EventingCriticalSectionStub 
{
   void Lock()
   {
   }
   void Unlock()
   {
   }
};

struct __EventingCriticalSectionAuto 
{
private:
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	void EmitError(_In_ DWORD dwLastError)
	{
		static const int nMax = 256;
		TCHAR pszMessage[nMax];

		if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwLastError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR)&pszMessage, nMax, NULL)) {

			static const TCHAR szDefaultMessage[] = _T("cannot initialize critical section");

			::ATL::Checked::tcscpy_s(pszMessage, _countof(pszMessage), szDefaultMessage);
		}

		MessageBox(0, pszMessage, 0, MB_OK | MB_ICONHAND);
	}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
public:
	_Acquires_lock_(this->m_sec) void Lock()
	{
		EnterCriticalSection(&m_sec);
	}
	_Releases_lock_(this->m_sec) void Unlock()
	{
		LeaveCriticalSection(&m_sec);
	}
	__EventingCriticalSectionAuto()
	{
		if (!::ATL::_AtlInitializeCriticalSectionEx(&m_sec, 0, 0))
		{
#if defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)

			EmitError(GetLastError());
#endif
		}
	}
	~__EventingCriticalSectionAuto()
	{
		DeleteCriticalSection(&m_sec);
	}
	CRITICAL_SECTION m_sec;
};

template <class T>
struct __eventingGetAddr 
{
	typedef void (T::*pmfn_type) ();
	typedef void (*pgfn_type) ();
	union U {
		void *addr;
		void (T::*pmfn)();
		void (*pgfn)();
	};
	static pmfn_type __getMAddr(_In_ void *addr)
	{
		U u;
		u.addr = addr;
		return u.pmfn;
	}
	static void* __getVAddr(_In_ pmfn_type pmfn)
	{
		U u;
		u.pmfn = pmfn;
		return u.addr;
	}
	static pgfn_type __getSMAddr(_In_ void *addr)
	{
		U u;
		u.addr = addr;
		return u.pgfn;
	}
	static void* __getSVAddr(_In_ pgfn_type pgfn)
	{
		U u;
		u.pgfn = pgfn;
		return u.addr;
	}
};

struct __eventNode 
{
	virtual int __isEqual(
		_In_ void*,
		_In_ void*) = 0;
	virtual int __isEqual(_In_ void*) = 0;
	__eventNode* next;
};

struct __eventMainNode 
{
	int key;
	__eventNode* root_node;
	__eventMainNode* next_event;
};


//
// pvargSrc should only contain the memory for a VARIANT
//
inline HRESULT WINAPI __VariantChangeType(
	_Inout_ VARIANTARG*& pvargDest,
	_In_ VARIANTARG* pvargSrc,
	_In_ VARTYPE vt)
{
	ATLASSERT(pvargDest != 0 && pvargSrc != 0);
	if( pvargDest == 0 || pvargSrc == 0 )
		return E_INVALIDARG;
	ATLTRACE(_T("ATLEVENT: __VariantChangeType(pvargDest->vt=%d,vt=%d)\n"), pvargDest->vt, vt);

	::VariantInit(pvargSrc);
	HRESULT hr = ::VariantChangeType(pvargSrc, pvargDest, 0, vt);
	if (FAILED(hr)) {
		if (hr == DISP_E_TYPEMISMATCH && (vt & VT_ARRAY) != 0 && (pvargDest->vt & ~VT_BYREF) == (VT_ARRAY | VT_VARIANT)) {
			return S_OK; // you're on your own
		}
	} else {
		::VariantClear(pvargDest);
		*pvargDest = *pvargSrc;
	}
	return hr;
}

HRESULT WINAPI _com_handle_excepinfo(
	_In_ EXCEPINFO& excepInfo,
	_Outptr_ IErrorInfo** pperrinfo);

#pragma pack(push,_ATL_PACKING)
namespace ATL {
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
    inline HRESULT AtlExcepInfoFromErrorInfo(
		_In_ HRESULT hrInvoke,
		_Inout_ EXCEPINFO *pExcepInfo)
    {
        if (pExcepInfo == NULL)
        {
            return E_POINTER;
        }
        pExcepInfo->pfnDeferredFillIn = NULL;
        pExcepInfo->scode = hrInvoke;
        CComPtr<IErrorInfo> spErrInfo;
        HRESULT hr = GetErrorInfo(0, &spErrInfo);
        if (hr == S_OK)
        {
            // Set up ErrInfo object
            // Ignore any errors. If additional error information is not
            // available then corresponding pointer will be NULL or 0
            spErrInfo->GetSource(&pExcepInfo->bstrSource);
            spErrInfo->GetDescription(&pExcepInfo->bstrDescription);
            spErrInfo->GetHelpFile(&pExcepInfo->bstrHelpFile);
            spErrInfo->GetHelpContext(&pExcepInfo->dwHelpContext);
        }
        return hr;
    }

ATLPREFAST_SUPPRESS(6102)
	inline HRESULT __ComInvokeEventHandler(
		_Inout_ IDispatch* pDispatch,
		_In_ DISPID id,
		_In_ WORD wFlags,
		_In_ DISPPARAMS *pDispParams,
		_Out_opt_ VARIANT *pVarResult)
	{
		if (pVarResult != 0) {
			::VariantInit(pVarResult);
		}
		EXCEPINFO excepInfo;
		memset(&excepInfo, 0, sizeof excepInfo);
		UINT nArgErr = (UINT)-1;
		HRESULT hr = pDispatch->Invoke(id, IID_NULL, LOCALE_USER_DEFAULT, wFlags, pDispParams, pVarResult, &excepInfo, &nArgErr);
		if (FAILED(hr)) {
			IErrorInfo* perrinfo = 0;
			if (SUCCEEDED(_com_handle_excepinfo(excepInfo, &perrinfo)))
			{
				SetErrorInfo(0, perrinfo);
				perrinfo->Release();
			}
		}
		return hr;
	}
ATLPREFAST_UNSUPPRESS()
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

	struct __EventHandlerProxy 
	{
		virtual HRESULT CDECL __eventHandlerProxy(_In_ int idx, ...) = 0;
	};
	struct __EventHandlerNodeProxy 
	{
		virtual int __Index(_In_ int i) = 0;
	};
	template <typename T /*super*/>
	class __ComEventingImpl : 
		public __EventHandlerProxy 
	{
		enum { __InvalidIndex = -1 };
		struct __ComEventingNode : __EventHandlerNodeProxy {
			__ComEventingNode(
				_In_opt_ T* pThis = 0,
				_In_opt_ IUnknown* pSource = 0,
				_In_opt_ IUnknown* pSink = 0,
				_In_opt_ const _GUID* pGuid = 0,
				_In_ int nSize = 0)
			{
				__pThis = pThis;
				__nHooks = 0;
				__dwAdvise = 0;
				__pSource = pSource;
				__pSink = pSink;
				__pNext = 0;
				__pGuid = const_cast<_GUID*> (pGuid);
				__nArraySize = nSize;
				__proxyIndex = _ATL_NEW int[__nArraySize];
				memset(__proxyIndex, 0xff, __nArraySize*sizeof(int));
			}
			int __Index(_In_ int i) {
				return __proxyIndex[i];
			}
			T* __pThis;
			int *__proxyIndex;
			int __nArraySize;
			int __nHooks;
			unsigned long __dwAdvise;
			CComPtr<IUnknown> __pSource;
			IUnknown* __pSink;
			_GUID* __pGuid;
			__ComEventingNode* __pNext;
		};
		__ComEventingNode* __pFirst;
		__ComEventingNode* __pLast;
		__ComEventingNode* __pCurrent;
	public:
		__ComEventingImpl() {
			__pCurrent = __pFirst = __pLast = 0;
		}
		~__ComEventingImpl() {
			if (__pFirst != 0) {
				__pCurrent = __pFirst;
				while (__pCurrent != 0) {
					__pCurrent->__pSink->Release();
					delete [] __pCurrent->__proxyIndex;
					__pFirst = __pCurrent;
					__pCurrent = __pCurrent->__pNext;
					delete __pFirst;
				}
			}
		}
		template <typename U /*interface*/>
		HRESULT __WhichThis(
			_In_ IUnknown* pS,
			_In_ T* pThis,
			_In_ int nSize,
			_In_ bool bNext,
			_In_ int idx)
		{
			if (bNext) {
				 if (__pCurrent != 0) {
					 __pCurrent = __pCurrent->__pNext;
				 }
			} else {
				__pCurrent = __pFirst;
			}
			if (__pCurrent == 0
				|| !__pCurrent->__pSource.IsEqualObject(pS)
				|| __pCurrent->__pThis != pThis
				|| !IsEqualGUID(*U::Guid(), *__pCurrent->__pGuid)) {
				if (__pCurrent != 0) {
					__ComEventingNode* pTheOne = 0;
					while ((__pCurrent = __pCurrent->__pNext) != 0) {
						if (__pCurrent->__pSource.IsEqualObject(pS)
							&& (__pCurrent->__pThis == pThis)
							&& IsEqualGUID(*U::Guid(), *__pCurrent->__pGuid)) {
							if (nSize > 0 || __pCurrent->__proxyIndex[idx] != __InvalidIndex) {
								pTheOne = __pCurrent;
								if (nSize > 0) {
									break;
								}
							}
						 }
					 }
					 if (pTheOne != 0) {
						 __pCurrent = pTheOne;
						 return S_OK;
					 }
				}
				if (nSize > 0) {
					CComObject<U>* pSink;
					HRESULT hr = CComObject<U>::CreateInstance(&pSink);
					if (FAILED(hr)) {
						return hr;
					}
					pSink->AddRef();
					pSink->__pThis = pThis;
					pSink->__pThat = __pCurrent = _ATL_NEW __ComEventingNode(pThis, pS, pSink, U::Guid(), nSize);
					if( pSink->__pThat == NULL )
						return E_OUTOFMEMORY;
					if (__pLast != 0) {
						__pLast->__pNext = __pCurrent;
					} else {
						__pFirst = __pCurrent;
					}
					__pLast = __pCurrent;
				}
			}
			return S_OK;
		}
		template <typename U /*interface*/>
		HRESULT __AddHandler(
			_In_ T* pThis,
			_In_ int idxSink,
			_Inout_ IUnknown* pS,
			_In_ int idx,
			_In_ int nSize)
		{
			if (pS == 0 || pThis == 0) {
				return E_FAIL;
			}
			bool bNext = false;
			bool bDone = false;
			while (!bDone) {
				HRESULT hr = __WhichThis<U>(pS, pThis, nSize, bNext, idx);
				if (SUCCEEDED(hr)) {
					if (__pCurrent->__proxyIndex[idx] == __InvalidIndex) {
						__pCurrent->__proxyIndex[idx] = idxSink;
						++__pCurrent->__nHooks;
						__pCurrent->__pThis = pThis;
						bDone = true;
					}
					if (__pCurrent->__dwAdvise == 0) {
						return AtlAdvise(pS, __pCurrent->__pSink, *U::Guid(), &__pCurrent->__dwAdvise);
					}
					bNext = true;
				} else {
					return hr;
				}
			}
			return S_OK;
		}
		template <typename U /*interface*/>
		HRESULT __RemoveHandler(
			_In_ T* pThis,
			_In_ IUnknown* pS,
			_In_ int idx)
		{
			bool bNext = false;
			bool bDone = false;
			while (!bDone) {
				__WhichThis<U>(pS, pThis, 0, bNext, idx);
				if (__pCurrent == 0) {
					return E_FAIL;
				}
				if (__pCurrent->__proxyIndex[idx] != __InvalidIndex) {
					__pCurrent->__proxyIndex[idx] = __InvalidIndex;
					--__pCurrent->__nHooks;
					bDone = true;
				}
				if (__pCurrent->__dwAdvise != 0 && __pCurrent->__nHooks == 0) {
					HRESULT hr = AtlUnadvise(pS, *U::Guid(), __pCurrent->__dwAdvise);
					if (SUCCEEDED(hr)) {
						__pCurrent->__pSource.Release();
						__pCurrent->__dwAdvise = 0;
					}
					return hr;
				}
				bNext = true;
			}
			return S_OK;
		}
		HRESULT __RemoveAllHandlers(
			_In_opt_ IUnknown* pS,
			_In_opt_ const _GUID* pIID)
		{
			HRESULT hr = E_FAIL;
			__pCurrent = __pFirst;
			while (1) {
				if (__pCurrent == 0) {
					return hr;
				} else if ((pS == 0 || __pCurrent->__pSource.IsEqualObject(pS)
					&& (pIID == 0 || IsEqualGUID(*pIID, *__pCurrent->__pGuid)))
					&& __pCurrent->__dwAdvise != 0) {
					hr = AtlUnadvise(__pCurrent->__pSource, *__pCurrent->__pGuid, __pCurrent->__dwAdvise);
					if (SUCCEEDED(hr)) {
						__pCurrent->__pSource.Release();
						__pCurrent->__dwAdvise = 0;
						memset(__pCurrent->__proxyIndex, 0xff, __pCurrent->__nArraySize*sizeof(int));
						__pCurrent->__nHooks = 0;
					}
				}
				__pCurrent = __pCurrent->__pNext;
			}
		}
	};

	class __ComEventingImpl_LD {
		struct __EventCookieNode {
			__EventCookieNode(
				_In_opt_ IUnknown* pS = 0,
				_In_opt_ const _GUID* pG = 0)
			{
				__nextCookie = 0;
				__dwAdvise = 0;
				__pSource = pS;
				__pGuid = const_cast<_GUID*> (pG);
			}
			__EventCookieNode* __nextCookie;
			unsigned long __dwAdvise;
			CComPtr<IUnknown> __pSource;
			_GUID* __pGuid;
		} *__EventCookies;
	public:
		HRESULT __Advise(
			_Inout_ IUnknown* pSrc,
			_Inout_opt_ IUnknown* pSink,
			_In_ const IID& iid)
		{
			__EventCookieNode* pRoot = __EventCookies;
			while (pRoot != 0) {
				if (pRoot->__pSource.IsEqualObject(pSrc)
					&& IsEqualGUID(iid, *pRoot->__pGuid)
					&& pRoot->__dwAdvise == 0) {
					break;
				}
				pRoot = pRoot->__nextCookie;
			}
			if (pRoot == 0) {
				pRoot = _ATL_NEW __EventCookieNode(pSrc, &iid);
				if( pRoot == NULL )
					return E_OUTOFMEMORY;
				pRoot->__nextCookie = __EventCookies;
				__EventCookies = pRoot;
			}
			return AtlAdvise(pSrc, pSink, iid, &pRoot->__dwAdvise);
		}
		HRESULT __Unadvise(
			_Inout_ IUnknown* pSrc,
			_In_ const IID& iid)
		{
			__EventCookieNode* pRoot = __EventCookies;
			__EventCookieNode* pTheOne = 0;
			while (pRoot != 0) {
				if (pRoot->__pSource.IsEqualObject(pSrc)
					&& IsEqualGUID(iid, *pRoot->__pGuid)
					&& pRoot->__dwAdvise != 0) {
					pTheOne = pRoot;
				}
				pRoot = pRoot->__nextCookie;
			}
			HRESULT hr = E_FAIL;
			if (pTheOne != 0) {
				hr = AtlUnadvise(pSrc, iid, pTheOne->__dwAdvise);
				if (SUCCEEDED(hr)) {
					pTheOne->__pSource.Release();
					pTheOne->__dwAdvise = 0;
				}
			}
			return hr;
		}
		__ComEventingImpl_LD() {
			__EventCookies = 0;
		}
		~__ComEventingImpl_LD() {
			while (__EventCookies != 0) {
				__EventCookieNode* pDead = __EventCookies;
				__EventCookies = __EventCookies->__nextCookie;
				delete pDead;
			}
		}
	};
}

#pragma pack(pop)
#endif // __ATLEVENT_H__
