// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXDHTML_H__
#define __AFXDHTML_H__

#ifndef __mshtml_h__
#include <MsHTML.h>
#endif

#ifndef __MSHTMDID_H__
#include <MsHtmdid.h>
#endif

#ifndef __mshtmhst_h__
#include <MsHtmHst.h>
#endif

#ifndef __ATLBASE_H__
#include <atlbase.h>
#endif

#ifndef __exdisp_h__
#include <ExDisp.h>
#endif

#include <atlbase.h>
#include <atlcom.h>

#include <afxpriv2.h>
#include <afxocc.h>
#include <afxtempl.h>
#include <float.h>

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

// Classes declared in this file
class CDHtmlSinkHandler;
	class CDHtmlEventSink;

// IDispatch
	class CDHtmlControlSink;

// IDispatch
	class CDHtmlElementEventSink;

// CObject
	// CCmdTarget
		// COleControlSite
			class CBrowserControlSite;

// CObject
	// CCmdTarget
		// CWnd
			// CDialog
				class CDHtmlDialog;
					class CMultiPageDHtmlDialog;

enum DHtmlEventMapEntryType { DHTMLEVENTMAPENTRY_NAME, DHTMLEVENTMAPENTRY_CLASS, DHTMLEVENTMAPENTRY_TAG,
	DHTMLEVENTMAPENTRY_ELEMENT, DHTMLEVENTMAPENTRY_CONTROL, DHTMLEVENTMAPENTRY_END };

struct DHtmlEventMapEntry;

// Dummy base classes just to force pointers-to-member that work with multiple inheritance
class CDHtmlSinkHandlerBase1
{
};
class CDHtmlSinkHandlerBase2
{
};

class CDHtmlSinkHandler :
	public CDHtmlSinkHandlerBase1,
	public CDHtmlSinkHandlerBase2
{
public:
	STDMETHOD(CDHtmlSinkHandlerQueryInterface)(REFIID riid, void ** ppvObject) = 0;
	virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
	virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) = 0;
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) = 0;
	STDMETHOD(GetIDsOfNames)(REFIID riid, _In_reads_(cNames) OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) = 0;
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) = 0;
	virtual const DHtmlEventMapEntry* GetDHtmlEventMap() = 0;
};

struct DHtmlEventMapEntry
{
	DHtmlEventMapEntryType nType;
	DISPID dispId;
	LPCTSTR szName;	// name or class based on nType
	void (__stdcall CDHtmlSinkHandler::*pfnEventFunc)();
};

typedef HRESULT (CDHtmlSinkHandler::*DHEVTFUNC)(IHTMLElement *pElement);
typedef void (__stdcall CDHtmlSinkHandler::*DHEVTFUNCCONTROL)();

#define DECLARE_DHTML_EVENT_MAP()\
private:\
protected:\
	virtual const DHtmlEventMapEntry* GetDHtmlEventMap();\

#define BEGIN_DHTML_EVENT_MAP(className)\
const DHtmlEventMapEntry* className::GetDHtmlEventMap()\
{\
	typedef className theClass;\
	PTM_WARNING_DISABLE \
	static const DHtmlEventMapEntry _dhtmlEventEntries[] = {

#define END_DHTML_EVENT_MAP()\
{ DHTMLEVENTMAPENTRY_END, 0, NULL, (DHEVTFUNCCONTROL) NULL },\
};\
	PTM_WARNING_RESTORE \
	return _dhtmlEventEntries;\
}

#define BEGIN_DHTML_EVENT_MAP_INLINE(className)\
const DHtmlEventMapEntry* GetDHtmlEventMap()\
{\
	typedef className theClass;\
	PTM_WARNING_DISABLE \
	__pragma(warning(push))							   \
	__pragma(warning(disable: 4640)) /* message maps can only be called by single threaded message pump */ \
	static const DHtmlEventMapEntry _dhtmlEventEntries[] = \
	{\

#define END_DHTML_EVENT_MAP_INLINE()\
	{ DHTMLEVENTMAPENTRY_END, 0, NULL, (DHEVTFUNCCONTROL) NULL },\
	};\
	__pragma(warning(pop))	\
	PTM_WARNING_RESTORE \
	return _dhtmlEventEntries;\
}\


#define DHTML_EVENT(dispid, elemName, memberFxn)\
{ DHTMLEVENTMAPENTRY_NAME, dispid, elemName, (DHEVTFUNCCONTROL) (DHEVTFUNC) &theClass::memberFxn },\

#define DHTML_EVENT_CLASS(dispid, elemName, memberFxn)\
{ DHTMLEVENTMAPENTRY_CLASS, dispid, elemName, (DHEVTFUNCCONTROL) (DHEVTFUNC) &theClass::memberFxn },\

#define DHTML_EVENT_TAG(dispid, elemName, memberFxn)\
{ DHTMLEVENTMAPENTRY_TAG, dispid, elemName, (DHEVTFUNCCONTROL) (DHEVTFUNC) &theClass::memberFxn },\

#define DHTML_EVENT_ELEMENT(dispid, elemName, memberFxn)\
{ DHTMLEVENTMAPENTRY_ELEMENT, dispid, elemName, (DHEVTFUNCCONTROL) (DHEVTFUNC) &theClass::memberFxn },\

#define DHTML_EVENT_AXCONTROL(dispid, controlName, memberFxn)\
{ DHTMLEVENTMAPENTRY_CONTROL, dispid, controlName, (DHEVTFUNCCONTROL) (static_cast<void (__stdcall theClass::*)()>(&theClass::memberFxn)) },\

// specific commonly used events
#define DHTML_EVENT_ONHELP(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONHELP, elemName, memberFxn)\

#define DHTML_EVENT_ONCLICK(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONCLICK, elemName, memberFxn)\

#define DHTML_EVENT_ONDBLCLICK(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDBLCLICK, elemName, memberFxn)\

#define DHTML_EVENT_ONKEYPRESS(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONKEYPRESS, elemName, memberFxn)\

#define DHTML_EVENT_ONKEYDOWN(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONKEYDOWN, elemName, memberFxn)\

#define DHTML_EVENT_ONKEYUP(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONKEYUP, elemName, memberFxn)\

#define DHTML_EVENT_ONMOUSEOUT(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEOUT, elemName, memberFxn)\

#define DHTML_EVENT_ONMOUSEOVER(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEOVER, elemName, memberFxn)\

#define DHTML_EVENT_ONMOUSEMOVE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEMOVE, elemName, memberFxn)\

#define DHTML_EVENT_ONMOUSEDOWN(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEDOWN, elemName, memberFxn)\

#define DHTML_EVENT_ONMOUSEUP(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONMOUSEUP, elemName, memberFxn)\

#define DHTML_EVENT_ONSELECTSTART(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONSELECTSTART, elemName, memberFxn)\

#define DHTML_EVENT_ONFILTERCHANGE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONFILTERCHANGE, elemName, memberFxn)\

#define DHTML_EVENT_ONDRAGSTART(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDRAGSTART, elemName, memberFxn)\

#define DHTML_EVENT_ONBEFOREUPDATE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONBEFOREUPDATE, elemName, memberFxn)\

#define DHTML_EVENT_ONAFTERUPDATE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONAFTERUPDATE, elemName, memberFxn)\

#define DHTML_EVENT_ONERRORUPDATE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONERRORUPDATE, elemName, memberFxn)\

#define DHTML_EVENT_ONROWEXIT(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONROWEXIT, elemName, memberFxn)\

#define DHTML_EVENT_ONROWENTER(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONROWENTER, elemName, memberFxn)\

#define DHTML_EVENT_ONDATASETCHANGED(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDATASETCHANGED, elemName, memberFxn)\

#define DHTML_EVENT_ONDATAAVAILABLE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDATAAVAILABLE, elemName, memberFxn)\

#define DHTML_EVENT_ONDATASETCOMPLETE(elemName, memberFxn)\
DHTML_EVENT(DISPID_HTMLELEMENTEVENTS_ONDATASETCOMPLETE, elemName, memberFxn)\

// non-bubbled element events
#define DHTML_EVENT_ONBLUR(elemName, memberFxn)\
DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONBLUR, elemName, memberFxn)\

#define DHTML_EVENT_ONCHANGE(elemName, memberFxn)\
DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONCHANGE, elemName, memberFxn)\

#define DHTML_EVENT_ONFOCUS(elemName, memberFxn)\
DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONFOCUS, elemName, memberFxn)\

#define DHTML_EVENT_ONRESIZE(elemName, memberFxn)\
DHTML_EVENT_ELEMENT(DISPID_EVMETH_ONRESIZE, elemName, memberFxn)\

class CDHtmlEventSink : public CDHtmlSinkHandler
{
public:
	HRESULT ConnectToConnectionPoint(IUnknown *punkObj, REFIID riid, DWORD *pdwCookie);
	void DisconnectFromConnectionPoint(IUnknown *punkObj, REFIID riid, DWORD& dwCookie);
	STDMETHOD(CDHtmlSinkHandlerQueryInterface)(REFIID iid, LPVOID* ppvObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, _In_reads_(cNames) OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	virtual BOOL DHtmlEventHook(HRESULT *phr, DISPID dispIdMember, DISPPARAMS *pDispParams,
		VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	virtual const DHtmlEventMapEntry* GetDHtmlEventMap() = 0;
	virtual HRESULT GetDHtmlDocument(IHTMLDocument2 **pphtmlDoc) = 0;
	int FindDHtmlEventEntry(const DHtmlEventMapEntry *pEventMap, DISPID dispIdMember,
							   IHTMLElement **ppsrcElement);
};

class CDHtmlControlSink : public IDispatch
{
public:
	LPCTSTR m_szControlId;
	DWORD m_dwCookie;
	::ATL::CComPtr<IUnknown> m_spunkObj;
	IID m_iid;
	GUID m_libid;
	WORD m_wMajor;
	WORD m_wMinor;
	::ATL::CComPtr<ITypeInfo> m_spTypeInfo;
	CDHtmlSinkHandler *m_pHandler;
	DWORD_PTR m_dwThunkOffset;

	CDHtmlControlSink();
	CDHtmlControlSink(IUnknown *punkObj, CDHtmlSinkHandler *pHandler, LPCTSTR szControlId,
			DWORD_PTR dwThunkOffset = 0);
	virtual ~CDHtmlControlSink();
	HRESULT ConnectToControl(IUnknown *punkObj);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, _In_reads_(cNames) OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pdispparams, VARIANT *pvarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	HRESULT InvokeFromFuncInfo(DHEVTFUNCCONTROL pEvent, ATL::_ATL_FUNC_INFO& info, DISPPARAMS* pdispparams, VARIANT* pvarResult);
	HRESULT GetFuncInfoFromId(const IID& iid, DISPID dispidMember, LCID lcid, ATL::_ATL_FUNC_INFO& info);
	VARTYPE GetUserDefinedType(ITypeInfo *pTI, HREFTYPE hrt);
};

class CDHtmlElementEventSink : public IDispatch
{
public:
	CDHtmlEventSink *m_pHandler;
	::ATL::CComPtr<IUnknown> m_spunkElem;
	DWORD m_dwCookie;

	CDHtmlElementEventSink(CDHtmlEventSink *pHandler, IDispatch *pdisp);
	virtual ~CDHtmlElementEventSink() {}
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj);
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo);
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
	STDMETHOD(GetIDsOfNames)(REFIID riid, _In_reads_(cNames) OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS *pdispparams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	HRESULT Advise(LPUNKNOWN pUnkObj, REFIID iid);
	HRESULT UnAdvise(LPUNKNOWN pUnkObj, REFIID iid);
};

/*============================================================================*/
// CBrowserControlSite class - Implements the control site for hosting
// the browser.  Used by CDHtmlDialog
class CBrowserControlSite : public COleControlSite,
	public IDocHostUIHandler
{
public:
	CBrowserControlSite(COleControlContainer* pCtrlCont, CDHtmlDialog *pHandler);

protected:
// Implementation
	LPUNKNOWN GetInterfaceHook(const void *iid);
// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
// IDocHostUIHandler methods
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch **ppDispatch);
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);

protected:
	CDHtmlDialog *m_pHandler;
};

// DDX_DHtml helper macros
#define DDX_DHtml_ElementValue(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_A_VALUE, var)
#define DDX_DHtml_ElementInnerText(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLELEMENT_INNERTEXT, var)
#define DDX_DHtml_ElementInnerHtml(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLELEMENT_INNERHTML, var)
#define DDX_DHtml_Anchor_Href(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLANCHORELEMENT_HREF, var)
#define DDX_DHtml_Anchor_Target(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLANCHORELEMENT_TARGET, var)
#define DDX_DHtml_Img_Src(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLIMGELEMENT_SRC, var)
#define DDX_DHtml_Frame_Src(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLFRAMEBASE_SRC, var)
#define DDX_DHtml_IFrame_Src(dx, name, var) DDX_DHtml_ElementText(dx, name, DISPID_IHTMLFRAMEBASE_SRC, var)

/*============================================================================*/
// CDHtmlDialog class - Implements a CDialog that hosts the WebBrowser
// control and allows for handling events generated by HTML elements
class CDHtmlDialog : public CDialog,
	public CDHtmlEventSink
{
public:
	CString m_strCurrentUrl;
	LPTSTR m_szHtmlResID;
	UINT m_nHtmlResID;
	::ATL::CComPtr<IWebBrowser2> m_pBrowserApp;
	::ATL::CComPtr<IHTMLDocument2> m_spHtmlDoc;
	BOOL m_bUseHtmlTitle;

	CDHtmlDialog();
	CDHtmlDialog(UINT nIDTemplate, UINT nHtmlResID = 0, CWnd *pParentWnd = NULL);
	CDHtmlDialog(LPCTSTR lpszTemplateName, LPCTSTR szHtmlResID, CWnd *pParentWnd = NULL);
	virtual ~CDHtmlDialog();
	virtual BOOL CreateControlSite(COleControlContainer* pContainer,
			COleControlSite** ppSite, UINT /* nID */, REFCLSID /* clsid */);
	HRESULT GetDHtmlDocument(IHTMLDocument2 **pphtmlDoc);
	void GetCurrentUrl(CString& szUrl);
	virtual BOOL CanAccessExternal();
	virtual void OnBeforeNavigate(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual BOOL OnInitDialog();
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);
	void Navigate(LPCTSTR lpszURL, DWORD dwFlags= 0, LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
	void DestroyModeless();

// DDX functions
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, CString& value);
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, short& value);
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, int& value);
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, long& value);
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, DWORD& value);
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, float& value);
	void DDX_DHtml_ElementText(CDataExchange* pDX, LPCTSTR szId, DISPID dispid, double& value);
	void DDX_DHtml_CheckBox(CDataExchange* pDX, LPCTSTR szId, int& value);
	void DDX_DHtml_Radio(CDataExchange* pDX, LPCTSTR szId, long& value);
	void DDX_DHtml_SelectValue(CDataExchange* pDX, LPCTSTR szId, CString& value);
	void DDX_DHtml_SelectString(CDataExchange* pDX, LPCTSTR szId, CString& value);
	void DDX_DHtml_SelectIndex(CDataExchange* pDX, LPCTSTR szId, long& value);
	void DDX_DHtml_AxControl(CDataExchange *pDX, LPCTSTR szId, DISPID dispid, VARIANT& var);
	void DDX_DHtml_AxControl(CDataExchange *pDX, LPCTSTR szId, LPCTSTR szPropName, VARIANT& var);

// host handler
	void SetHostFlags(DWORD dwFlags);
	void SetExternalDispatch(IDispatch *pdispExternal);
	virtual BOOL IsExternalDispatchSafe();
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	STDMETHOD(GetHostInfo)(DOCHOSTUIINFO *pInfo);
	STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	STDMETHOD(ResizeBorder)(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)(LPOLESTR *pchKey, DWORD dw);
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch **ppDispatch);
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR *pchURLIn, OLECHAR **ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject *pDO, IDataObject **ppDORet);

	HRESULT GetElement(LPCTSTR szElementId, IDispatch **ppdisp, BOOL *pbCollection = NULL);
	HRESULT GetElement(LPCTSTR szElementId, IHTMLElement **pphtmlElement);
	template <class Q>
	HRESULT GetElementInterface(LPCTSTR szElementId, Q** ppvObj)
	{
		return GetElementInterface(szElementId, __uuidof(Q), (void **) ppvObj);
	}
	HRESULT GetElementInterface(LPCTSTR szElementId, REFIID riid, void **ppvObj);
	BSTR GetElementText(LPCTSTR szElementId);
	void SetElementText(LPCTSTR szElementId, BSTR bstrText);
	void SetElementText(IUnknown *punkElem, BSTR bstrText);
	BSTR GetElementHtml(LPCTSTR szElementId);
	void SetElementHtml(LPCTSTR szElementId, BSTR bstrText);
	void SetElementHtml(IUnknown *punkElem, BSTR bstrText);
	VARIANT GetElementProperty(LPCTSTR szElementId, DISPID dispid);
	void SetElementProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar);
	HRESULT GetControlDispatch(LPCTSTR szId, IDispatch **ppdisp);
	VARIANT GetControlProperty(IDispatch *pdispControl, DISPID dispid);
	VARIANT GetControlProperty(LPCTSTR szId, DISPID dispid);
	VARIANT GetControlProperty(LPCTSTR szId, LPCTSTR szPropName);
	void SetControlProperty(IDispatch *pdispControl, DISPID dispid, VARIANT *pVar);
	void SetControlProperty(LPCTSTR szElementId, DISPID dispid, VARIANT *pVar);
	void SetControlProperty(LPCTSTR szElementId, LPCTSTR szPropName, VARIANT *pVar);
	HRESULT GetEvent(IHTMLEventObj **ppEventObj);

//Implementation
public:
	HRESULT OnDocumentReadyStateChange(IHTMLElement *phtmlElem);

protected:
	BOOL m_bAttachedControl;
	CString m_strDlgCaption;
	CWnd m_wndBrowser;

	static UINT WM_DESTROYMODELESS;

	DECLARE_DYNAMIC(CDHtmlDialog)
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Initialize();
	void _OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	void _OnNavigateComplete2(LPDISPATCH pDisp, VARIANT FAR* URL);
	void _OnDocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	LRESULT OnDestroyModeless(WPARAM, LPARAM);

	// DDX Functions
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, CString& value, BOOL bSave);
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, short& value, BOOL bSave);
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, int& value, BOOL bSave);
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, long& value, BOOL bSave);
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, DWORD& value, BOOL bSave);
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, float& value, BOOL bSave);
	void DDX_DHtml_ElementText(LPCTSTR szId, DISPID dispid, double& value, BOOL bSave);

	/*============================================================================*/
	// Data exchange for special controls
	void DDX_DHtml_CheckBox(LPCTSTR szId, int& value, BOOL bSave);
	void DDX_DHtml_Radio(LPCTSTR szId, long& value, BOOL bSave);
	void DDX_DHtml_SelectValue(LPCTSTR szId, CString& value, BOOL bSave);
	void DDX_DHtml_SelectString(LPCTSTR szId, CString& value, BOOL bSave);
	void DDX_DHtml_SelectIndex(LPCTSTR szId, long& value, BOOL bSave);
	void DDX_DHtml_AxControl(LPCTSTR szId, DISPID dispid, VARIANT& var, BOOL bSave);
	void DDX_DHtml_AxControl(LPCTSTR szId, LPCTSTR szPropName, VARIANT& var, BOOL bSave);
	void OnDDXError(LPCTSTR szId, UINT nIDPrompt, BOOL bSave);

	void __cdecl DDX_DHtml_ElementTextWithFormat(LPCTSTR szId,
		DISPID dispid, LPCTSTR lpszFormat, UINT nIDPrompt, BOOL bSave, ...);
	void DDX_DHtml_ElementTextFloatFormat(LPCTSTR szId,
		DISPID dispid, void* pData, double value, int nSizeGcvt, BOOL bSave);
	BOOL _AfxSimpleScanf(LPCTSTR lpszText,
		LPCTSTR lpszFormat, va_list pData);
	BOOL _AfxSimpleFloatParse(LPCTSTR lpszText, double& d);
	long Select_FindString(IHTMLSelectElement *pSelect, BSTR bstr, BOOL fExact);
	void SetFocusToElement(LPCTSTR szId);

// event sink
	DWORD m_dwDHtmlEventSinkCookie;
	::ATL::CSimpleArray<CDHtmlControlSink *> m_ControlSinks;
	::ATL::CSimpleArray<CDHtmlElementEventSink *> m_SinkedElements;

	virtual const DHtmlEventMapEntry* GetDHtmlEventMap();

	HRESULT ConnectDHtmlEvents(IUnknown *punkDoc);
	void DisconnectDHtmlEvents();
	HRESULT ConnectDHtmlElementEvents(DWORD_PTR dwThunkOffset = 0);
	BOOL FindSinkForObject(LPCTSTR szName);
	BOOL IsSinkedElement(IDispatch *pdispElem);
	void DisconnectDHtmlElementEvents();

// host handler
	DWORD m_dwHostFlags;
	::ATL::CComPtr<IDispatch> m_spExternalDisp;
};

UINT __declspec(selectany) CDHtmlDialog::WM_DESTROYMODELESS = 0;

/*============================================================================*/
// Multi Url event map support
struct DHtmlUrlEventMapEntry
{
	LPCTSTR szUrl;	// the url
	const DHtmlEventMapEntry *pEventMap; // the map associated with the Url
};

#define DECLARE_DHTML_URL_EVENT_MAP()\
	virtual const DHtmlEventMapEntry *GetEventMapForUrl(LPCTSTR szUrl);

#define BEGIN_DHTML_URL_EVENT_MAP(className)\

#define BEGIN_EMBED_DHTML_EVENT_MAP(className, mapName)\
	typedef className theClass;\
	static const DHtmlEventMapEntry className##mapName##[] = \
	{\

#define END_EMBED_DHTML_EVENT_MAP()\
{ DHTMLEVENTMAPENTRY_END, 0, NULL, NULL },\
	};\

#define BEGIN_URL_ENTRIES(className)\
	static const DHtmlUrlEventMapEntry className##UrlEntries[] = \
	{\

#define URL_EVENT_ENTRY(className, url, mapName)\
{ url, className##mapName },\

#define END_URL_ENTRIES()\
	{ NULL, NULL },\
	};\

#define END_DHTML_URL_EVENT_MAP(className)\
	const DHtmlEventMapEntry *className::GetEventMapForUrl(LPCTSTR szUrl)\
	{\
		for (int i=0; className##UrlEntries[i].szUrl; i++)\
		{\
			if (!_tcsicmp(className##UrlEntries[i].szUrl, szUrl))\
				return className##UrlEntries[i].pEventMap;\
		}\
	return NULL;\
}\

class CMultiPageDHtmlDialog : public CDHtmlDialog
{
public:
	CMultiPageDHtmlDialog();
	CMultiPageDHtmlDialog(UINT nIDTemplate, UINT nHtmlResID = 0, CWnd *pParentWnd = NULL);
	CMultiPageDHtmlDialog(LPCTSTR lpszTemplateName, LPCTSTR szHtmlResID = NULL, CWnd *pParentWnd = NULL);
	virtual ~CMultiPageDHtmlDialog();

//implementation
protected:
	virtual const DHtmlEventMapEntry *GetEventMapForUrl(LPCTSTR szUrl);
	virtual const DHtmlEventMapEntry* GetDHtmlEventMap();
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	DECLARE_DYNAMIC(CMultiPageDHtmlDialog)

	const DHtmlEventMapEntry *m_pCurrentMap;
};

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFXDHTML_H__
