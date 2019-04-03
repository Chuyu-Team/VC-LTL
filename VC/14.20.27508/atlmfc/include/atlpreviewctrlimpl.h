// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#pragma once

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif


#include <atlwin.h>
#include <atlhandler.h>

#pragma once

#ifndef __cplusplus
#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#pragma pack(push,_ATL_PACKING)
namespace ATL
{
/// <summary>
/// This class is an ATL implementation of a window that is placed on a host window provided by the Shell
/// for Rich Preview. </summary>
class CAtlPreviewCtrlImpl :
	public CWindowImpl<CAtlPreviewCtrlImpl>,
	public IPreviewCtrl
{
public:
	BEGIN_MSG_MAP(CAtlPreviewCtrlImpl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	/// <summary>
	/// Constructs a preview control object.</summary>
	CAtlPreviewCtrlImpl(void) : m_clrText(0), m_clrBack(RGB(255, 255, 255)), m_plf(NULL)
	{
	}

	/// <summary>
	/// Destructs a preview control object.</summary>
	virtual ~CAtlPreviewCtrlImpl(void)
	{
	}
	/// <summary>Handles WM_PAINT message.</summary>
	/// <param name="nMsg">Set to WM_PAINT.</param>
	/// <param name="wParam">This parameter is not used. </param>
	/// <param name="lParam">This parameter is not used. </param>
	/// <param name="bHandled">When this function returns it contains TRUE.</param>
	/// <returns> Always returns 0.</returns>
	LRESULT OnPaint(
		_In_ UINT nMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Out_ BOOL& bHandled)
	{
		UNREFERENCED_PARAMETER(nMsg);
		UNREFERENCED_PARAMETER(wParam);
		UNREFERENCED_PARAMETER(lParam);

		PAINTSTRUCT ps;
		this->CWindowImpl<CAtlPreviewCtrlImpl>::BeginPaint(&ps);
		DoPaint(ps.hdc);
		this->CWindowImpl<CAtlPreviewCtrlImpl>::EndPaint(&ps);

		bHandled = TRUE;

		return 0;
	}

	/// <summary>
	/// Called by a Rich Preview handler to create the Windows window. </summary>
	/// <param name="hWndParent"> A handle to the host window supplied by the Shell for Rich Preview. </param>
	/// <param name="prc"> Specifies initial size and position of the window. </param>
	/// <returns>TRUE if creation succeeded; otherwise FALSE.</returns>
	virtual BOOL Create(
		_In_ HWND hWndParent,
		_In_ const RECT* prc)
	{
		_U_RECT rect((LPRECT)prc);
		this->CWindowImpl<CAtlPreviewCtrlImpl>::Create(hWndParent, rect);
		return TRUE;
	}
	/// <summary>
	/// Called by a Rich Preview handler when it needs to destroy this control.</summary>
	virtual void Destroy()
	{
		this->CWindowImpl<CAtlPreviewCtrlImpl>::DestroyWindow();
	}
	/// <summary>
	/// Sets a new parent for this control. </summary>
	/// <param name="hWndParent">A handle to the new parent window.</param>
	virtual void SetHost(_In_ HWND hWndParent)
	{
		this->CWindowImpl<CAtlPreviewCtrlImpl>::SetParent(hWndParent);
	}
	/// <summary>
	/// Sets input focus to this control. </summary>
	virtual void Focus()
	{
		this->CWindowImpl<CAtlPreviewCtrlImpl>::SetFocus();
	}
	/// <summary>
	/// Tells this control to redraw. </summary>
	virtual void Redraw()
	{
		this->CWindowImpl<CAtlPreviewCtrlImpl>::RedrawWindow();
	}
	/// <summary>
	/// Sets a new bounding rectangle for this control. </summary>
	/// <remarks> Usually new bounding rectangle is set when the host control is resized.</remarks>
	/// <param name="prc">Specifies the new size and position of preview control.</param>
	/// <param name="bRedraw">Specifies whether the control should be redrawn.</param>
	virtual void SetRect(
		_In_ const RECT* prc,
		_In_ BOOL bRedraw)
	{
		DWORD dwFlags = SWP_NOZORDER;
		if (!bRedraw)
		{
			dwFlags |= SWP_NOREDRAW;
		}
		this->CWindowImpl<CAtlPreviewCtrlImpl>::SetWindowPos(NULL, prc, dwFlags);
	}
	/// <summary>
	/// Called by a Rich Preview handler when it needs to set visuals of rich preview content.</summary>
	/// <param name="clrBack"> Background color of preview window. </param>
	/// <param name="clrText"> Text color of preview window. </param>
	/// <param name="plf"> Font used to display texts in preview window. </param>
	virtual void SetPreviewVisuals(
		_In_ COLORREF clrBack,
		_In_ COLORREF clrText,
		_In_ const LOGFONTW *plf)
	{
		m_clrText = clrText;
		m_clrBack = clrBack;
		m_plf = plf;
	}

protected:
	/// <summary> Called by the framework to render the preview.</summary>
	/// <param name="hdc">A handle to a device context for painting.</par
	virtual void DoPaint(_In_ HDC hdc)
	{
		CString str = _T("Override CAtlPreviewCtrlImpl::DoPaint in a derived class for painting.");
		TextOut(hdc, 0, 0, str, str.GetLength());
	}

protected:
	/// <summary> Text color of preview window. </summary>
	COLORREF m_clrText;
	/// <summary> Background color of preview window. </summary>
	COLORREF m_clrBack;
	/// <summary> Font used to display texts in preview window. </summary>
	const LOGFONTW* m_plf;
};
} //namespace ATL
#pragma pack(pop)

