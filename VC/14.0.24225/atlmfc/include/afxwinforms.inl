// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXWINFORMS.H

#pragma warning(disable:4439)	// C4439: function with a managed parameter must have a __clrcall calling convention

extern "C" void __cdecl AfxmEnsureManagedInitialization();
extern "C" void __cdecl AfxmReleaseManagedReferences(IUnknown*);

namespace Microsoft {
	namespace VisualC {
		namespace MFC {

#ifndef AFXWFRM_TEMPLATES_INL_INCLUDE_GUARD
#define AFXWFRM_TEMPLATES_INL_INCLUDE_GUARD

//CWinFormsEventsHelper
inline CWinFormsEventsHelper::CWinFormsEventsHelper()
    {
		m_pSink = NULL;
	}
inline void CWinFormsEventsHelper::Control::set(System::Windows::Forms::Control^ pControl)
	{
		ENSURE_ARG(pControl!=nullptr);
		ENSURE(m_pControl == nullptr);
		m_pControl=pControl;
		  m_pControl->HandleCreated +=
			  gcnew System::EventHandler(this, &CWinFormsEventsHelper::OnHandleCreated);
		  m_pControl->HandleDestroyed +=
			  gcnew System::EventHandler(this, &CWinFormsEventsHelper::OnHandleDestroyed);

	}
inline System::Windows::Forms::Control^ CWinFormsEventsHelper::Control::get()
	{
		return m_pControl;
	}
inline void CWinFormsEventsHelper::Advise(IHandleEvents* pSink)
	{
		ENSURE_ARG(pSink!=NULL);
		ENSURE_ARG(m_pSink == NULL || m_pSink == pSink);
		m_pSink = pSink;
	}

inline void CWinFormsEventsHelper::Unadvise(IHandleEvents* pSink)
	{
		ENSURE_ARG(m_pSink == NULL || m_pSink == pSink);
		m_pSink = NULL;
	}

inline void CWinFormsEventsHelper::OnHandleCreated( System::Object^ o, System::EventArgs^ args)
	{
		if (m_pSink)
		{
			m_pSink->OnHandleCreated(o, args);
		}
	}
inline void CWinFormsEventsHelper::OnHandleDestroyed( System::Object^ o, System::EventArgs^ args)
	{
		if (m_pSink)
		{
			m_pSink->OnHandleDestroyed(o, args);
		}

	}

//CWinFormsControl<class TManagedControl>

template<class TManagedControl>
CWinFormsControl<TManagedControl>::CWinFormsControl()
	{
	}

template<class TManagedControl>
inline TManagedControl^ CWinFormsControl<TManagedControl>::GetControl() const
	{
		CWinFormsControlSite* pSite=static_cast<CWinFormsControlSite*>(m_pCtrlSite);
		TManagedControl^ pCtrl=nullptr;
		if (pSite)
		{
			pCtrl=static_cast<TManagedControl^>(pSite->get_Control());
		}
		return pCtrl;
	}

template<class TManagedControl>
inline CWinFormsControl<TManagedControl>::operator TManagedControl^() const
	{
		return GetControl();
	}

template<class TManagedControl>
inline TManagedControl^ CWinFormsControl<TManagedControl>::operator->() const
	{
		return GetControl();
	}

template<class TManagedControl>
inline HWND CWinFormsControl<TManagedControl>::GetControlHandle() const
	{
		return reinterpret_cast<HWND>(static_cast<INT_PTR>(GetControl()->Handle));
	}
template<class TManagedControl>
inline BOOL CWinFormsControl<TManagedControl>::InternalCreateManagedControl(const CControlCreationInfo& info,DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID)
{
	CRect rect2( rect );
	CPoint pt;
	CSize size;

	pt = rect2.TopLeft();
	size = rect2.Size();
	AfxmEnsureManagedInitialization();
	pParentWnd->m_nFlags |= WF_NOWIN32ISDIALOGMSG;
	return CreateControl(info,dwStyle,&pt,&size,pParentWnd,nID);
}

template<class TManagedControl>
inline BOOL CWinFormsControl<TManagedControl>::CreateManagedControl(System::Type^ pType,DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID)
	{
		//Marshal the .Net Type a GCHandle to pass through native MFC.
		CControlCreationInfoEx info;
		info.Init(pType,CControlCreationInfo::ReflectionType);
		info.m_clsid=CLSID_WinFormsControl;
		return InternalCreateManagedControl(info,dwStyle,rect,pParentWnd,nID);
	}

template<class TManagedControl>
inline BOOL CWinFormsControl<TManagedControl>::CreateManagedControl(TManagedControl^ pControl,DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID)
	{
		ENSURE_ARG(pControl!=nullptr);
		CControlCreationInfoEx info;
		info.Init(pControl,CControlCreationInfo::ControlInstance);
		info.m_clsid=CLSID_WinFormsControl;
		return InternalCreateManagedControl(info,dwStyle,rect,pParentWnd,nID);
	}
template<class TManagedControl>
inline BOOL CWinFormsControl<TManagedControl>::CreateManagedControl(DWORD dwStyle,const RECT& rect, CWnd* pParentWnd, int nID)
	{
		return CreateManagedControl(TManagedControl::typeid,dwStyle,rect, pParentWnd, nID);
	}
template<class TManagedControl>
inline BOOL CWinFormsControl<TManagedControl>::CreateManagedControl(DWORD dwStyle,
		int nPlaceHolderID, CWnd* pParentWnd)
	{
		ENSURE_ARG(pParentWnd!=NULL);
		CWnd* pwnd = pParentWnd->GetDlgItem(nPlaceHolderID);
		CRect rectPlaceHolder;
		ENSURE(pwnd != NULL);
		pwnd->GetWindowRect(&rectPlaceHolder);
		pParentWnd->ScreenToClient(rectPlaceHolder);
		//Combine caller supplied with place holder styles.
		DWORD controlStyle = dwStyle | pwnd->GetStyle();
		BOOL ret=CreateManagedControl(controlStyle,rectPlaceHolder,pParentWnd,nPlaceHolderID);
		// Set ZOrder only, so managed control replaces the STATIC place holder in the child windows list.
		// This list affects Z-Order and Tab order.

		ASSERT(GetControlHandle()!=NULL);
		BOOL ok=::SetWindowPos(GetControlHandle(), pwnd->m_hWnd, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		ASSERT(ok);


		//Also need to change the linked list of tab order, maintained to support MFC implementation of
		//IsDialogMessage, to delete the last entry (added by CreateControl), and change the place holder
		//entry, to point at our new control.
		COleControlSiteOrWnd *pThisSitePlaceHolderWnd=_AfxFindSiteOrWnd(pParentWnd,this);
		COleControlSiteOrWnd *pSitePlaceHolderWnd=_AfxFindSiteOrWnd(pParentWnd,pwnd);
		POSITION posRemove=pParentWnd->GetControlContainer()->m_listSitesOrWnds.Find(pThisSitePlaceHolderWnd);
		ASSERT(posRemove != NULL);
		if (posRemove)
		{
			pParentWnd->GetControlContainer()->m_listSitesOrWnds.RemoveAt(posRemove);
			ENSURE(pThisSitePlaceHolderWnd!=NULL);
			pThisSitePlaceHolderWnd->m_pSite = NULL; //So delete m_pSite in COleControlSiteOrWnd will not destory the site.
			delete pThisSitePlaceHolderWnd;
		}

		ASSERT(pSitePlaceHolderWnd);
		if (pSitePlaceHolderWnd)
		{
			pSitePlaceHolderWnd->m_hWnd = NULL;
			pSitePlaceHolderWnd->m_pSite = GetControlSite();
		}

		pwnd->DestroyWindow();
		return ret;
	}

///////////////////////////////////////////////////////////////////////////////
//CWinFormsDialog

BEGIN_TEMPLATE_MESSAGE_MAP(CWinFormsDialog, TManagedControl, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

template <typename TManagedControl>
CWinFormsDialog<TManagedControl>::CWinFormsDialog(UINT nIDTemplate, CWnd* pParentWnd)
: CDialog(nIDTemplate, pParentWnd)
{
	m_pWFControl = gcnew TManagedControl();
}

template <typename TManagedControl>
BOOL CWinFormsDialog<TManagedControl>::OnInitDialog()
{
	BOOL bRet=CDialog::OnInitDialog();
	ENSURE(bRet);
	//Create an instance of the control and retrieve its Size
	TManagedControl^ pControl = m_pWFControl;
	//Adjust the size of the dialog to accommodate for the control size.
	CRect rcDlg;
	GetWindowRect(&rcDlg);
	CRect rcDlgClient;
	GetClientRect(&rcDlgClient);
	ClientToScreen(&rcDlgClient);
	rcDlg.bottom -= rcDlgClient.Height();
	rcDlg.bottom +=  pControl->Size.Height;
	rcDlg.right  = rcDlg.left + pControl->Size.Width;
	MoveWindow(&rcDlg,FALSE);

	//Set title text to match control Text property
	CString title(pControl->Text);
	SetWindowText(title.GetString());
	//Host the new control in the mfc dialog
	CControlCreationInfoEx info;
	info.Init(pControl,CControlCreationInfo::ControlInstance);
	info.m_clsid=CLSID_WinFormsControl;

	CRect rcCtrl;
	rcCtrl.top  = 0;
	rcCtrl.left = 0;
	rcCtrl.bottom = pControl->Size.Height;
	rcCtrl.right  = pControl->Size.Width;
	bRet=m_control.InternalCreateManagedControl(info,WS_VISIBLE | WS_TABSTOP,rcCtrl,this,IDC_WINFRMCTRL_DIALOG);

	ENSURE(GetControlHandle()!=NULL && bRet);
	return TRUE;
}
template <typename TManagedControl>
inline TManagedControl^ CWinFormsDialog<TManagedControl>::GetControl() const
	{
		return m_pWFControl;
	}

template <typename TManagedControl>
inline CWinFormsDialog<TManagedControl>::operator TManagedControl^() const throw()
	{
		return GetControl();
	}

template <typename TManagedControl>
inline TManagedControl^ CWinFormsDialog<TManagedControl>::operator->() const throw()
	{
		return GetControl();
	}

template <typename TManagedControl>
inline HWND CWinFormsDialog<TManagedControl>::GetControlHandle() const throw()
	{
		return reinterpret_cast<HWND>(static_cast<INT_PTR>(GetControl()->Handle));
	}

template <typename TManagedControl>
void CWinFormsDialog<TManagedControl>::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//When view size changes, adjust the WinForms control (which is child of the view)
	//size to occupy the entire client area of the view.
	CRect rcView;
	GetClientRect(&rcView);
	System::Drawing::Size size(rcView.Width(), rcView.Height());

	if (GetControl()!=nullptr)
	{
		GetControl()->Size = size;
	}
}

template <typename T>
void DDX_ManagedControl(CDataExchange* pDX, int nIDC,CWinFormsControl<T>& control)
{
	if ((control.m_hWnd == NULL) && (control.GetControlUnknown() == NULL))    // not attached yet
	{
		ASSERT(!pDX->m_bSaveAndValidate);

		if (!control.CreateManagedControl(0,nIDC, pDX->m_pDlgWnd))
		{
			ASSERT(FALSE);
			AfxThrowNotSupportedException();
		}
	}
}

#endif //AFXWFRM_TEMPLATES_INL_INCLUDE_GUARD

#ifndef METHODS_INLINED_IN_BOTH_DEBUG_AND_RELEASE
#define METHODS_INLINED_IN_BOTH_DEBUG_AND_RELEASE
//Inline in both Debug and Release - workaround for non-exportable __clrcall methods.

inline void CWinFormsControlSite::OnHandleCreated( System::Object^ , System::EventArgs^ )
	{
		OnHandleCreatedHandler();
	}

inline void CWinFormsControlSite::OnHandleDestroyed( System::Object^ , System::EventArgs^ )
	{
		DetachWindow();
	}

inline void CControlCreationInfoEx::Init(System::Object^ p,HandleKind hk)
	{
		// no need to check for valid handle; was allocated in ctor
		(GCHandle::operator GCHandle(static_cast<System::IntPtr>(m_nHandle) )).Target = p;
		m_hk=hk;
	}

inline System::Windows::Forms::Control^  CWinFormsControlSite::get_Control() const
	{
		System::Windows::Forms::Control^ pControl=m_gcEventHelper->Control::get();
		ENSURE((CWinFormsEventsHelper^)m_gcEventHelper!=nullptr && pControl!=nullptr);
		return pControl;
	}

//CWinFormsView
inline CWinFormsView::CWinFormsView(System::Type^ pManagedViewType)
	: m_pManagedViewType(pManagedViewType)
	{
		ASSERT((System::Type^)m_pManagedViewType!=nullptr);
	}

inline System::Windows::Forms::Control^ CWinFormsView::GetControl() const
	{
		return m_control.GetControl();
	}

inline CWinFormsView::operator System::Windows::Forms::Control^() const
	{
		return GetControl();
	}

#endif //METHODS_INLINED_IN_BOTH_DEBUG_AND_RELEASE

#ifdef _AFXWIN_INLINE
#ifndef AFXWFRM_INL_INCLUDE_GUARD
#define AFXWFRM_INL_INCLUDE_GUARD

//CControlCreationInfoEx
_AFXWIN_INLINE CControlCreationInfoEx::CControlCreationInfoEx()
	{
		m_nHandle = static_cast<intptr_t>( GCHandle::operator System::IntPtr(GCHandle::Alloc(nullptr)) );
	}

_AFXWIN_INLINE CControlCreationInfoEx::~CControlCreationInfoEx()
	{
		GCHandle g = GCHandle::operator GCHandle(static_cast<System::IntPtr>(m_nHandle));
		g.Free();
		m_nHandle = 0;
	}

//CWinFormsControlSite
_AFXWIN_INLINE CWinFormsControlSite::CWinFormsControlSite(COleControlContainer* pCtrlCont)
	: COleControlSite(pCtrlCont),m_gcEventHelper(gcnew CWinFormsEventsHelper())
	{
		m_gcEventHelper->Advise(this);
	}

_AFXWIN_INLINE CWinFormsControlSite::~CWinFormsControlSite()
	{
		m_gcEventHelper->Unadvise(this);
	}
_AFXWIN_INLINE void CWinFormsControlSite::SetControlEnabled(bool bEnable)
	{
		get_Control()->Enabled=bEnable;
	}
_AFXWIN_INLINE void CWinFormsControlSite::OnHandleDestroyed( gcroot<System::Object^> , gcroot<System::EventArgs^> )
	{
		DetachWindow();
	}


#endif //AFXWFRM_INL_INCLUDE_GUARD
#endif // _AFXWIN_INLINE
		} //MFC
	} //VisualC
} //Microsoft


