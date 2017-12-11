// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFX_WINFORMS_H__
#define __AFX_WINFORMS_H__

#ifndef __cplusplus
	#error MFC requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __cplusplus_cli
	#error MFC WinForms support requires /clr (doesn't support oldSyntax)
#endif

#ifndef _AFXDLL
	#error MFC WinForms support requires MFC dll version.
#endif

#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>


#using <mfcmifc80.dll>

#ifdef BUILD_MFCM
#define AFX_MFCM_EXPORT __declspec(dllexport)
#else
#define AFX_MFCM_EXPORT __declspec(dllimport)

#ifndef _MFC_FILENAME_VER
#define _MFC_FILENAME_VER "140"
#endif

#ifndef _MFC_FILENAME_VER_DOT
#define _MFC_FILENAME_VER_DOT "14.0"
#endif

#ifndef _UNICODE
	#ifdef _DEBUG
		#pragma comment(lib, "mfcm" _MFC_FILENAME_VER "d.lib")		
	#else		
		#pragma comment(lib, "mfcm" _MFC_FILENAME_VER ".lib")		
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "mfcm" _MFC_FILENAME_VER "ud.lib")		
	#else
		#pragma comment(lib, "mfcm" _MFC_FILENAME_VER "u.lib")		
	#endif
#endif

#endif //BUILD_MFCM

#include <vcclr.h>
#include <msclr\event.h>
#include <afxdisp.h>
#include <afxpriv.h>



namespace Microsoft {
	namespace VisualC {
		namespace MFC {



/*============================================================================*/
// CWin32Window - Win32 HWND wrapper implementing IWin32Window. Used to pass 
// Parent HWNDs from MFC to WinForms.
//

public ref class CWin32Window : public System::Windows::Forms::IWin32Window 
{
	const HWND m_hWnd;
public:
	CWin32Window(HWND hWnd) : m_hWnd(hWnd){}
	property System::IntPtr Handle
	{
		virtual System::IntPtr get()
		{
			return static_cast<System::IntPtr>(m_hWnd);
		}		
	}	
};

/*============================================================================*/
// CControlCreationInfoEx - Inits and Cleanup CControlCreationInfo struct 
//  GCHandle information.
//

class CControlCreationInfoEx : public CControlCreationInfo {
public:
	typedef System::Runtime::InteropServices::GCHandle GCHandle;
	CControlCreationInfoEx();
	
	void Init(System::Object^ p,HandleKind hk);
	
	~CControlCreationInfoEx();
	
};

/*============================================================================*/
// Notification from __gc Helper class.
__interface IHandleEvents {
	void OnHandleCreated(System::Object^ o, System::EventArgs^ e);
	void OnHandleDestroyed(System::Object^ o, System::EventArgs^ e);
};


public ref class CWinFormsEventsHelper
{
public:
	CWinFormsEventsHelper();
    

	property System::Windows::Forms::Control^ Control
	{
		System::Windows::Forms::Control^ get();
		void set(System::Windows::Forms::Control^ pControl);
	}
	
	void Advise(IHandleEvents* pSink);
	void Unadvise(IHandleEvents* pSink);
	

private:
	void OnHandleCreated( System::Object^ o, System::EventArgs^ args);
	
	void OnHandleDestroyed( System::Object^ o, System::EventArgs^ args);
	
public:
	System::Windows::Forms::Control^ m_pControl;
	IHandleEvents* m_pSink;

};

/*============================================================================*/
// CWinFormsControlSite - overrides COleControlSite when the ActiveX is a 
// WinForm control.

 
class CWinFormsControlSite : public COleControlSite,public IHandleEvents {
public:
	CWinFormsControlSite(COleControlContainer* pCtrlCont);
	~CWinFormsControlSite();
	
	virtual HRESULT DoVerb(LONG nVerb, LPMSG lpMsg);
	
	virtual void GetProperty(DISPID dwDispID, VARTYPE vtProp, void* pvProp) const;
	
	void SetControlEnabled(bool bEnable);
	
	virtual void SetPropertyV(DISPID dwDispID, VARTYPE vtProp, va_list argList);
	
	virtual DWORD GetStyle() const;	

	virtual HRESULT CreateOrLoad(const CControlCreationInfo& creationInfo);	
	virtual HRESULT CreateControlCommon(CWnd* pWndCtrl, REFCLSID clsid,const CControlCreationInfo& creationInfo,
		LPCTSTR lpszWindowName, DWORD dwStyle, const POINT* ppt, const SIZE* psize, UINT nID,
		CFile* pPersist, BOOL bStorage, BSTR bstrLicKey);

	void OnHandleCreatedHandler();

	virtual void OnHandleCreated( gcroot<System::Object^> , gcroot<System::EventArgs^> );
	virtual void OnHandleDestroyed( gcroot<System::Object^> , gcroot<System::EventArgs^> );

	virtual void OnHandleCreated(System::Object^, System::EventArgs^);	
	virtual void OnHandleDestroyed(System::Object^, System::EventArgs^);
	
	System::Windows::Forms::Control^ get_Control() const;
	
	gcroot<CWinFormsEventsHelper^> m_gcEventHelper;
};

/*============================================================================*/
// CWinFormsControl
// Provides the basic functionality for AX hosting of WinForms Control
//
template<class TManagedControl>
class CWinFormsControl : public CWnd	
{
public:
	CWinFormsControl();

	inline TManagedControl^ GetControl() const;
	

	inline operator TManagedControl^() const;
	inline TManagedControl^ operator->() const;
	inline HWND GetControlHandle() const;

	inline BOOL CreateManagedControl(System::Type^ pType,DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID);

	inline BOOL CreateManagedControl(typename TManagedControl^ pControl,DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID);
	

	inline BOOL CreateManagedControl(DWORD dwStyle,const RECT& rect, CWnd* pParentWnd, int nID);
	
	/*============================================================================*/
	//Purpose: Create a WinForms control and placing it on top
	// of a STATIC control, placed in Resource Editor as a place holder
	// for the .Net control.
	//Input: window styles, IDC_ of STATIC place holder, and Parent CWnd*
	//Return: TRUE if creation succeeded.
	/*============================================================================*/
	inline BOOL CreateManagedControl(DWORD dwStyle,int nPlaceHolderID, CWnd* pParentWnd);	

	//Creates a control of managed type.
	inline BOOL InternalCreateManagedControl(const CControlCreationInfo& info,DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID);
};

/*============================================================================*/
// CWinFormsView 
// Provides generic functionality for AX hosting of WinForms Control as an MFC View
// 

class CWinFormsView : public CView 
{
	DECLARE_DYNAMIC(CWinFormsView)

public:
	
	CWinFormsView(System::Type^ pManagedViewType);

	System::Windows::Forms::Control^ GetControl() const;
	
	operator System::Windows::Forms::Control^() const;
	

protected:
	virtual void OnInitialUpdate();
	
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
								CView* pDeactiveView);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);
	
protected:
	virtual void OnDraw(CDC*) { };
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	gcroot<System::Type^> m_pManagedViewType;
private:
	CWinFormsControl<System::Windows::Forms::Control> m_control;
	gcroot<System::Object^> m_CmdSource;
		
	DECLARE_MESSAGE_MAP()

};

#define IDC_WINFRMCTRL_DIALOG 45002

//
// The WinForms Form wrapper
//
template <typename TManagedControl>
class CWinFormsDialog : public CDialog
{	
public:
	enum { IDD = AFX_IDD_EMPTYDIALOG };
public:		
	CWinFormsDialog(UINT nIDTemplate = IDD, CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();		
	//
	// The .Net Control  interface accessors
	//
	inline TManagedControl^ GetControl() const;
	inline operator TManagedControl^() const throw();
	inline TManagedControl^ operator->() const throw();
	inline HWND GetControlHandle() const throw();	
protected:
	CWinFormsControl<TManagedControl> m_control;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
private:
	DECLARE_MESSAGE_MAP()
	gcroot<TManagedControl^> m_pWFControl;
};

template <typename T>
void DDX_ManagedControl(CDataExchange* pDX, int nIDC,CWinFormsControl<T>& control);

		namespace Internal {
			using Microsoft::VisualC::MFC::CControlCreationInfoEx;
			using Microsoft::VisualC::MFC::CWin32Window;
			using Microsoft::VisualC::MFC::IHandleEvents;
			using Microsoft::VisualC::MFC::CWinFormsEventsHelper;
		}
		
	} //namespace MFC


	} //VisualC
} //Microsoft

using Microsoft::VisualC::MFC::CWinFormsView;
using Microsoft::VisualC::MFC::CWinFormsDialog;
using Microsoft::VisualC::MFC::CWinFormsControl;
using Microsoft::VisualC::MFC::CWinFormsControlSite;
using Microsoft::VisualC::MFC::DDX_ManagedControl;

#ifdef _AFX_ENABLE_INLINES
#define _AFXWIN_INLINE AFX_INLINE
#endif
#include <afxwinforms.inl>



#endif // __AFX_WINFORMS_H__
