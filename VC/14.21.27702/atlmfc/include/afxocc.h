// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXOCC_H__
#define __AFXOCC_H__

#pragma once

#include <afxtempl.h>
#include <oledb.h>
#include "olebind.h"
#include "ocdbid.h"
#include "ocdb.h"

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CDataSourceControl;
class CDataBoundProperty;

// CCmdTarget
	class COleControlContainer;
	class COleControlSite;

class COccManager;
struct _AFX_OCC_DIALOG_INFO;

#define DISPID_DATASOURCE   0x80010001
#define DISPID_DATAFIELD    0x80010002


class COleControlSiteFactory;
__declspec(selectany) extern const CLSID CLSID_WinFormsControl = 
{0xb7e7a666,0xd623,0x457f,{0xa3,0x0a,0x6a,0x49,0xa3,0xe5,0xb4,0x70}};
/*============================================================================*/
// Control site factory interface - allow instantiation of different control sites

class IControlSiteFactory 
{
public:
		virtual ~IControlSiteFactory() {}
		virtual COleControlSite* CreateSite(COleControlContainer* pCtrlCont,const CControlCreationInfo& creationInfo) = 0;
};

/*============================================================================*/
// Control site factory collection manager class

class CControlSiteFactoryMgr : public CNoTrackObject {
public:
	CControlSiteFactoryMgr();
	virtual ~CControlSiteFactoryMgr();
	BOOL RegisterSiteFactory(IControlSiteFactory* pFactory);
	BOOL UnregisterSiteFactory(IControlSiteFactory* pFactory);
	COleControlSite* CreateSite(COleControlContainer* pCtrlCont,const CControlCreationInfo& creationInfo);
protected:
	CList <IControlSiteFactory*,IControlSiteFactory*> m_lstFactory;
	COleControlSiteFactory* m_pOleControlSiteDefaultFactory;
};

BOOL AFXAPI AfxRegisterSiteFactory(IControlSiteFactory* pFactory);
BOOL AFXAPI AfxUnregisterSiteFactory(IControlSiteFactory* pFactory);



class CControlCreationInfo {
public:
	CControlCreationInfo();
	enum HandleKind { ReflectionType,ControlInstance,NullHandle };
	BOOL IsManaged() const;
	HandleKind m_hk;
	intptr_t   m_nHandle;
	
	CLSID m_clsid;
};


/*============================================================================*/
// Control containment helper functions

DLGTEMPLATE* _AfxSplitDialogTemplate(const DLGTEMPLATE* pTemplate,
	CMapWordToPtr* pOleItemMap);

void _AfxZOrderOleControls(CWnd* pWnd, CMapWordToPtr* pOleItemMap);

/*============================================================================*/
// COleControlContainer

class COleControlContainer : public CCmdTarget
{
public:
// Constructor
	explicit COleControlContainer(CWnd*  pWnd);

// Operations
	virtual void BroadcastAmbientPropertyChange(DISPID dispid);
	BOOL CreateControl(CWnd* pWndCtrl, REFCLSID clsid,
		LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, UINT nID,
		CFile* pPersist=NULL, BOOL bStorage=FALSE, BSTR bstrLicKey=NULL,
		COleControlSite** ppNewSite=NULL);
   // Overload to allow creation of default-sized controls
	BOOL CreateControl(CWnd* pWndCtrl, REFCLSID clsid,
		LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt,
	  const SIZE* psize, UINT nID, CFile* pPersist=NULL, BOOL bStorage=FALSE,
	  BSTR bstrLicKey=NULL, COleControlSite** ppNewSite=NULL);
	//Overload to create managed controls
	BOOL CreateControl(CWnd* pWndCtrl,const CControlCreationInfo& creationInfo,
			DWORD dwStyle, const POINT* ppt, const SIZE* psize,UINT nID);

	BOOL CreateControlCommon(CWnd* pWndCtrl, REFCLSID clsid,const CControlCreationInfo& creationInfo,
			LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize,
		   UINT nID, CFile* pPersist=NULL, BOOL bStorage=FALSE, BSTR bstrLicKey=NULL, COleControlSite** ppNewSite=NULL);


	virtual COleControlSite* FindItem(UINT nID) const;
	virtual BOOL GetAmbientProp(COleControlSite* pSite, DISPID dispid,
		VARIANT* pvarResult);
	void CreateOleFont(CFont* pFont);
	void FreezeAllEvents(BOOL bFreeze);
	virtual void ScrollChildren(int dx, int dy);
	virtual void OnUIActivate(COleControlSite* pSite);
	virtual void OnUIDeactivate(COleControlSite* pSite);

	virtual void CheckDlgButton(int nIDButton, UINT nCheck);
	virtual void CheckRadioButton(int nIDFirstButton, int nIDLastButton,
		int nIDCheckButton);
	virtual CWnd* GetDlgItem(int nID) const;
	virtual void GetDlgItem(int nID, HWND* phWnd) const;
	virtual UINT GetDlgItemInt(int nID, BOOL* lpTrans, BOOL bSigned) const;
	virtual int GetDlgItemText(_In_ int nID, _Out_writes_to_(nMaxCount, return + 1) _Pre_notnull_ _Post_z_ LPTSTR lpStr, _In_ int nMaxCount) const;
	virtual LRESULT SendDlgItemMessage(int nID, UINT message, WPARAM wParam,
		LPARAM lParam);
	virtual void SetDlgItemInt(int nID, UINT nValue, BOOL bSigned);
	virtual void SetDlgItemText(int nID, LPCTSTR lpszString);
	virtual UINT IsDlgButtonChecked(int nIDButton) const;

   virtual BOOL HandleSetFocus();
   virtual BOOL HandleWindowlessMessage(UINT message, WPARAM wParam, 
	  LPARAM lParam, LRESULT* plResult );
   virtual BOOL OnPaint(CDC* pDC);
#ifndef _AFXDLL
	virtual void AttachControlSite(CWnd* pWnd, UINT nIDC = 0);
#else
	void AttachControlSite(CWnd* pWnd, UINT nIDC = 0);
#endif
	virtual BOOL FillListSitesOrWnds(_AFX_OCC_DIALOG_INFO* pOccDlgInfo);
// Attributes
	CWnd* m_pWnd;
	CMapPtrToPtr m_siteMap;
	CTypedPtrList< CPtrList, COleControlSiteOrWnd* > m_listSitesOrWnds;
	COLORREF m_crBack;
	COLORREF m_crFore;
	LPFONTDISP m_pOleFont;
	COleControlSite* m_pSiteUIActive;
	COleControlSite* m_pSiteCapture;
	COleControlSite* m_pSiteFocus;
	int m_nWindowlessControls;

// Implementation
public:
	virtual ~COleControlContainer();

public:
	// Interface maps
	BEGIN_INTERFACE_PART(OleIPFrame, IOleInPlaceFrame)
		INIT_INTERFACE_PART(COleControlContainer, OleIPFrame)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
		STDMETHOD(GetBorder)(LPRECT);
		STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS);
		STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS);
		STDMETHOD(SetActiveObject)(LPOLEINPLACEACTIVEOBJECT, LPCOLESTR);
		STDMETHOD(InsertMenus)(HMENU, LPOLEMENUGROUPWIDTHS);
		STDMETHOD(SetMenu)(HMENU, HOLEMENU, HWND);
		STDMETHOD(RemoveMenus)(HMENU);
		STDMETHOD(SetStatusText)(LPCOLESTR);
		STDMETHOD(EnableModeless)(BOOL);
		STDMETHOD(TranslateAccelerator)(LPMSG, WORD);
	END_INTERFACE_PART(OleIPFrame)

	BEGIN_INTERFACE_PART(OleContainer, IOleContainer)
		INIT_INTERFACE_PART(COleControlContainer, OleContainer)
		STDMETHOD(ParseDisplayName)(LPBINDCTX, _In_z_ LPOLESTR, ULONG*, LPMONIKER*);
		STDMETHOD(EnumObjects)(DWORD, LPENUMUNKNOWN*);
		STDMETHOD(LockContainer)(BOOL);
	END_INTERFACE_PART(OleContainer)

	DECLARE_INTERFACE_MAP()
	DECLARE_DISPATCH_MAP()
};


/*============================================================================*/
// COleControlSite

#define VT_MFCFORCEPUTREF   0x8000  // force DISPATCH_PROPERTYPUTREF

class COleControlSite : public CCmdTarget
{
public:
// Constructor
	explicit COleControlSite(COleControlContainer* pCtrlCont);

// Operations
	virtual HRESULT CreateControl(CWnd* pWndCtrl, REFCLSID clsid,
		LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, UINT nID,
		CFile* pPersist=NULL, BOOL bStorage=FALSE, BSTR bstrLicKey=NULL);
   // Overload to allow creation of default-sized controls
	virtual HRESULT CreateControl(CWnd* pWndCtrl, REFCLSID clsid,
		LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt,
	  const SIZE* psize, UINT nID, CFile* pPersist=NULL, BOOL bStorage=FALSE,
	  BSTR bstrLicKey=NULL);

	HRESULT CreateControl(CWnd* pWndCtrl, const CControlCreationInfo& creationInfo,
			 DWORD dwStyle, const POINT* ppt, const SIZE* psize,UINT nID);

	virtual BOOL DestroyControl();
	BOOL GetEventIID(IID* piid);
	virtual HRESULT DoVerb(LONG nVerb, LPMSG lpMsg = NULL);
	BOOL IsDefaultButton();
	DWORD GetDefBtnCode();
	void SetDefaultButton(BOOL bDefault);
	void FreezeEvents(BOOL bFreeze);

	virtual void InvokeHelperV(DISPID dwDispID, WORD wFlags, VARTYPE vtRet,
		void* pvRet, const BYTE* pbParamInfo, va_list argList);
	virtual void SetPropertyV(DISPID dwDispID, VARTYPE vtProp, va_list argList);
	virtual void AFX_CDECL InvokeHelper(DISPID dwDispID, WORD wFlags, VARTYPE vtRet,
		void* pvRet, const BYTE* pbParamInfo, ...);
	virtual void GetProperty(DISPID dwDispID, VARTYPE vtProp, void* pvProp) const;
	virtual void AFX_CDECL SetProperty(DISPID dwDispID, VARTYPE vtProp, ...);
	virtual BOOL AFX_CDECL SafeSetProperty(DISPID dwDispID, VARTYPE vtProp, ...);

	virtual DWORD GetStyle() const;
	virtual DWORD GetExStyle() const;
	virtual BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags);
	virtual BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags);
	virtual void SetWindowText(LPCTSTR lpszString);
	virtual void GetWindowText(CString& str) const;
	virtual int GetDlgCtrlID() const;
	virtual int SetDlgCtrlID(int nID);
	virtual void MoveWindow(int x, int y, int nWidth, int nHeight);
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
		int cx, int cy, UINT nFlags);
	virtual BOOL ShowWindow(int nCmdShow);
	virtual BOOL IsWindowEnabled() const;
	virtual BOOL EnableWindow(BOOL bEnable);
	virtual CWnd* SetFocus();
	virtual CWnd* SetFocus(LPMSG lpmsg);
	virtual void EnableDSC();
	virtual void BindDefaultProperty(DISPID dwDispID, VARTYPE vtProp, LPCTSTR szFieldName, CWnd* pDSCWnd);
	virtual void BindProperty(DISPID dwDispId, CWnd* pWndDSC);

// Overridables
	virtual BOOL QuickActivate();

// Attributes
	COleControlContainer* m_pCtrlCont;
	HWND m_hWnd;
	CWnd* m_pWndCtrl;
	UINT m_nID;
	CRect m_rect;
	IID m_iidEvents;
	LPOLEOBJECT m_pObject;
	LPOLEINPLACEOBJECT m_pInPlaceObject;
	LPOLEINPLACEACTIVEOBJECT m_pActiveObject;
	IOleInPlaceObjectWindowless* m_pWindowlessObject;
	DWORD m_dwEventSink;
	DWORD m_dwPropNotifySink;
	DWORD m_dwStyleMask;
	DWORD m_dwStyle;
	DWORD m_dwMiscStatus;
	BOOL m_bIsWindowless;
	CONTROLINFO m_ctlInfo;

protected:
	void GetControlInfo();

	virtual HRESULT CreateControlCommon(CWnd* pWndCtrl, REFCLSID clsid,const CControlCreationInfo& creationInfo,
		LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize, UINT nID,
		CFile* pPersist, BOOL bStorage, BSTR bstrLicKey);

// Implementation
public:
	~COleControlSite();

	BOOL IsMatchingMnemonic(LPMSG lpMsg);
	void SendMnemonic(LPMSG lpMsg);
	
	void AttachWindow();
	void DetachWindow();
protected:
	BOOL SetExtent();

	virtual HRESULT CreateOrLoad(const CControlCreationInfo& creationInfo);
	virtual	HRESULT CreateOrLoad(REFCLSID clsid, CFile* pPersist, BOOL bStorage,
		BSTR bstrLicKey);

	DWORD ConnectSink(REFIID iid, LPUNKNOWN punkSink);
	void DisconnectSink(REFIID iid, DWORD dwCookie);
	
	BOOL OnEvent(AFX_EVENT* pEvent);
	HRESULT GetCursor(DISPID dispid, LPUNKNOWN* ppcursorOut, LPVOID *ppcidOut);

public:
	// Databound control stuff
	BOOL m_bIgnoreNotify;
	DWORD m_dwNotifyDBEvents; // INotifyDBEvents sink cookie
	CDataSourceControl* m_pDataSourceControl;
	CDataBoundProperty* m_pBindings;
	union {
		COleControlSite *m_pDSCSite;
		WORD m_ctlidRowSource;
	};
	DISPID m_defdispid;
	UINT m_dwType;
	CString m_strDataField;
	BOOL m_bIsDirty;
	VARIANT m_varResult;

protected:
	COleDispatchDriver m_dispDriver;
   CDC* m_pDC;

public:
// Interface maps
	BEGIN_INTERFACE_PART(OleClientSite, IOleClientSite)
		INIT_INTERFACE_PART(COleControlSite, OleClientSite)
		STDMETHOD(SaveObject)();
		STDMETHOD(GetMoniker)(DWORD, DWORD, LPMONIKER*);
		STDMETHOD(GetContainer)(LPOLECONTAINER*);
		STDMETHOD(ShowObject)();
		STDMETHOD(OnShowWindow)(BOOL);
		STDMETHOD(RequestNewObjectLayout)();
	END_INTERFACE_PART(OleClientSite)

	BEGIN_INTERFACE_PART(OleIPSite, IOleInPlaceSiteWindowless)
		INIT_INTERFACE_PART(COleControlSite, OleIPSite)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);

		STDMETHOD(CanInPlaceActivate)();
		STDMETHOD(OnInPlaceActivate)();
		STDMETHOD(OnUIActivate)();
		STDMETHOD(GetWindowContext)(LPOLEINPLACEFRAME*,
			LPOLEINPLACEUIWINDOW*, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO);
		STDMETHOD(Scroll)(SIZE);
		STDMETHOD(OnUIDeactivate)(BOOL);
		STDMETHOD(OnInPlaceDeactivate)();
		STDMETHOD(DiscardUndoState)();
		STDMETHOD(DeactivateAndUndo)();
		STDMETHOD(OnPosRectChange)(LPCRECT);

	  STDMETHOD(OnInPlaceActivateEx)( BOOL* ptNoRedraw, DWORD dwFlags );
	  STDMETHOD(OnInPlaceDeactivateEx)( BOOL tNoRedraw );
	  STDMETHOD(RequestUIActivate)();

	  STDMETHOD(AdjustRect)( LPRECT prect );
	  STDMETHOD(CanWindowlessActivate)();
	  STDMETHOD(GetCapture)();
	  STDMETHOD(GetDC)( LPCRECT prect, DWORD dwFlags, HDC* phDC );
	  STDMETHOD(GetFocus)();
	  STDMETHOD(InvalidateRect)( LPCRECT prect, BOOL tErase );
	  STDMETHOD(InvalidateRgn)( HRGN hRgn, BOOL tErase );
	  STDMETHOD(OnDefWindowMessage)( UINT nMessage, WPARAM wParam,
		 LPARAM lParam, LRESULT* plResult );
	  STDMETHOD(ReleaseDC)( HDC hDC );
	  STDMETHOD(ScrollRect)( int dx, int dy, LPCRECT prectScroll,
		 LPCRECT prectClip );
	  STDMETHOD(SetCapture)( BOOL tCapture );
	  STDMETHOD(SetFocus)( BOOL tFocus );
   END_INTERFACE_PART(OleIPSite)

	BEGIN_INTERFACE_PART(OleControlSite, IOleControlSite)
		INIT_INTERFACE_PART(COleControlSite, OleControlSite)
		STDMETHOD(OnControlInfoChanged)();
		STDMETHOD(LockInPlaceActive)(BOOL fLock);
		STDMETHOD(GetExtendedControl)(LPDISPATCH* ppDisp);
		STDMETHOD(TransformCoords)(POINTL* lpptlHimetric,
			POINTF* lpptfContainer, DWORD flags);
		STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, DWORD grfModifiers);
		STDMETHOD(OnFocus)(BOOL fGotFocus);
		STDMETHOD(ShowPropertyFrame)();
	END_INTERFACE_PART(OleControlSite)

	BEGIN_INTERFACE_PART(AmbientProps, IDispatch)
		INIT_INTERFACE_PART(COleControlSite, AmbientProps)
		STDMETHOD(GetTypeInfoCount)(unsigned int*);
		STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo**);
		STDMETHOD(GetIDsOfNames)(REFIID, _In_reads_(cNames) LPOLESTR*, unsigned int cNames, LCID, DISPID*);
		STDMETHOD(Invoke)(DISPID, REFIID, LCID, unsigned short, DISPPARAMS*,
						  VARIANT*, EXCEPINFO*, unsigned int*);
	END_INTERFACE_PART(AmbientProps)

	BEGIN_INTERFACE_PART(PropertyNotifySink, IPropertyNotifySink)
		INIT_INTERFACE_PART(COleControlSite, PropertyNotifySink)
		STDMETHOD(OnChanged)(DISPID dispid);
		STDMETHOD(OnRequestEdit)(DISPID dispid);
	END_INTERFACE_PART(PropertyNotifySink)

	BEGIN_INTERFACE_PART(EventSink, IDispatch)
		INIT_INTERFACE_PART(COleControlSite, EventSink)
		STDMETHOD(GetTypeInfoCount)(unsigned int*);
		STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo**);
		STDMETHOD(GetIDsOfNames)(REFIID, _In_reads_(cNames) LPOLESTR*, unsigned int cNames, LCID, DISPID*);
		STDMETHOD(Invoke)(DISPID, REFIID, LCID, unsigned short, DISPPARAMS*,
						  VARIANT*, EXCEPINFO*, unsigned int*);
	END_INTERFACE_PART(EventSink)

	BEGIN_INTERFACE_PART(BoundObjectSite, IBoundObjectSite)
		STDMETHOD(GetCursor)(DISPID dispid, LPLPCURSOR ppcursorOut, LPVOID *ppcidOut);
	END_INTERFACE_PART(BoundObjectSite)

	BEGIN_INTERFACE_PART(NotifyDBEvents, INotifyDBEvents)
		STDMETHOD(OKToDo)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);
		STDMETHOD(Cancelled)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);
		STDMETHOD(SyncBefore)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);
		STDMETHOD(AboutToDo)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);
		STDMETHOD(FailedToDo)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);
		STDMETHOD(SyncAfter)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);
		STDMETHOD(DidEvent)(DWORD dwEventWhat, ULONG cReasons, DBNOTIFYREASON rgReasons[]);

		// Not part of i/f - just a helper
		HRESULT FireEvent(DWORD dwEventWhat, ULONG cReasons,
			DBNOTIFYREASON rgReasons[], DSCSTATE nState);
	END_INTERFACE_PART(NotifyDBEvents)

	BEGIN_INTERFACE_PART(RowsetNotify, IRowsetNotify)
		STDMETHOD(OnFieldChange)(IRowset* pRowset, HROW hRow, DBORDINAL cColumns, DBORDINAL rgColumns[], DBREASON eReason, DBEVENTPHASE ePhase, BOOL fCantDeny);
		STDMETHOD(OnRowChange)(IRowset* pRowset, DBCOUNTITEM cRows, const HROW rghRows[], DBREASON eReason, DBEVENTPHASE ePhase, BOOL fCantDeny);
		STDMETHOD(OnRowsetChange)(IRowset* pRowset, DBREASON eReason, DBEVENTPHASE ePhase, BOOL fCantDeny);
	END_INTERFACE_PART(RowsetNotify)

	DECLARE_INTERFACE_MAP()
};

/*============================================================================*/
// COleControlSiteOrWnd (helper)
struct COleControlSiteOrWnd
{
	COleControlSiteOrWnd();
	COleControlSiteOrWnd(COleControlSite *pSite);
	COleControlSiteOrWnd(HWND hWnd, BOOL bAutoRadioButton);
	~COleControlSiteOrWnd();

	DWORD GetStyle() const;

	HWND m_hWnd;
	COleControlSite *m_pSite;
	BOOL m_bAutoRadioButton;
};

COleControlSiteOrWnd* AFXAPI _AfxFindSiteOrWnd(CWnd *pWndDlg, CWnd *pWnd);

/*============================================================================*/
// OLE control container manager

class COccManager : public CNoTrackObject
{
// Operations
public:
	// Event handling
	virtual BOOL OnEvent(CCmdTarget* pCmdTarget, UINT idCtrl, AFX_EVENT* pEvent,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	// Internal object creation
	virtual COleControlContainer* CreateContainer(CWnd* pWnd);
	virtual COleControlSite* CreateSite(COleControlContainer* pCtrlCont);
	virtual COleControlSite* CreateSite(COleControlContainer* pCtrlCont,const CControlCreationInfo& creationInfo);

	// Dialog creation
	virtual const DLGTEMPLATE* PreCreateDialog(_AFX_OCC_DIALOG_INFO* pOccDialogInfo,
		const DLGTEMPLATE* pOrigTemplate);
	virtual void PostCreateDialog(_AFX_OCC_DIALOG_INFO* pOccDialogInfo);
	virtual DLGTEMPLATE* SplitDialogTemplate(const DLGTEMPLATE* pTemplate,
		DLGITEMTEMPLATE** ppOleDlgItems);
	virtual BOOL CreateDlgControls(CWnd* pWndParent, LPCTSTR lpszResourceName,
		_AFX_OCC_DIALOG_INFO* pOccDialogInfo);
	virtual BOOL CreateDlgControls(CWnd* pWndParent, void* lpResource,
		_AFX_OCC_DIALOG_INFO* pOccDialogInfo);

	// Dialog manager
	virtual BOOL IsDialogMessage(CWnd* pWndDlg, LPMSG lpMsg);
	static BOOL AFX_CDECL IsLabelControl(CWnd* pWnd);
	static BOOL AFX_CDECL IsLabelControl(COleControlSiteOrWnd* pWnd);
	static BOOL AFX_CDECL IsMatchingMnemonic(CWnd* pWnd, LPMSG lpMsg);
	static BOOL AFX_CDECL IsMatchingMnemonic(COleControlSiteOrWnd* pWnd, LPMSG lpMsg);
	static void AFX_CDECL SetDefaultButton(CWnd* pWnd, BOOL bDefault);
	static DWORD AFX_CDECL GetDefBtnCode(CWnd* pWnd);

// Implementation
protected:
	// Dialog creation
	BOOL CreateDlgControl(CWnd* pWndParent, HWND hwAfter, BOOL bDialogEx,
		LPDLGITEMTEMPLATE pDlgItem, WORD nMsg, BYTE* lpData, DWORD cb, HWND* phWnd);

	// Databinding
	void BindControls(CWnd* pWndParent);

	// Dialog manager
	static void AFX_CDECL UIActivateControl(CWnd* pWndNewFocus);
	static void AFX_CDECL UIDeactivateIfNecessary(CWnd* pWndOldFocus, CWnd* pWndNewFocus);
};

struct _AFX_OCC_DIALOG_INFO
{
	DLGTEMPLATE* m_pNewTemplate;
	DLGITEMTEMPLATE** m_ppOleDlgItems;

	unsigned m_cItems;
	struct ItemInfo
	{
		unsigned nId;
		BOOL bAutoRadioButton;
	};
	ItemInfo *m_pItemInfo;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif // __AFXOCC_H__
