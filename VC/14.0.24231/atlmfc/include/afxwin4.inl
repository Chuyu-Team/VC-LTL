// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Always Inline. Functions only in Win2K or later

#pragma once

AFX_INLINE COLORREF CDC::GetDCBrushColor() const
	{ ENSURE(m_hDC != NULL); return ::GetDCBrushColor(m_hDC); }
AFX_INLINE COLORREF CDC::SetDCBrushColor(COLORREF crColor)
	{ ENSURE(m_hDC != NULL); return ::SetDCBrushColor(m_hDC, crColor); }

AFX_INLINE COLORREF CDC::GetDCPenColor() const
	{ ENSURE(m_hDC != NULL); return ::GetDCPenColor(m_hDC); }
AFX_INLINE COLORREF CDC::SetDCPenColor(COLORREF crColor)
	{ ENSURE(m_hDC != NULL); return ::SetDCPenColor(m_hDC, crColor); }

AFX_INLINE BOOL CDC::GetCharABCWidthsI(UINT giFirst, UINT cgi, LPWORD pgi, LPABC lpabc) const
	{ ENSURE(m_hDC != NULL); return ::GetCharABCWidthsI(m_hDC, giFirst, cgi, pgi, lpabc); }
AFX_INLINE BOOL CDC::GetCharWidthI(UINT giFirst, UINT cgi, LPWORD pgi, LPINT lpBuffer) const
	{ ENSURE(m_hDC != NULL); return ::GetCharWidthI(m_hDC, giFirst, cgi, pgi, lpBuffer); }

AFX_INLINE BOOL CDC::GetTextExtentExPointI(LPWORD pgiIn, int cgi, int nMaxExtent, LPINT lpnFit, LPINT alpDx, _Out_opt_ LPSIZE lpSize) const
{
	ENSURE(lpSize != NULL);
	ENSURE(m_hDC != NULL);
	return ::GetTextExtentExPointI(m_hDC, pgiIn, cgi, nMaxExtent, lpnFit, alpDx, lpSize);
}
AFX_INLINE BOOL CDC::GetTextExtentPointI(LPWORD pgiIn, int cgi, _Out_opt_ LPSIZE lpSize) const
{
	ENSURE(lpSize != NULL);
	ENSURE(m_hDC != NULL);
	return ::GetTextExtentPointI(m_hDC, pgiIn, cgi, lpSize);
}

AFX_INLINE BOOL CWnd::GetWindowInfo(PWINDOWINFO pwi) const
	{ ASSERT(::IsWindow(m_hWnd)); return ::GetWindowInfo(m_hWnd, pwi); }

AFX_INLINE CWnd* CWnd::GetAncestor(UINT gaFlags) const
	{ ASSERT(::IsWindow(m_hWnd)); return  CWnd::FromHandle(::GetAncestor(m_hWnd, gaFlags)); }

AFX_INLINE BOOL CWnd::GetScrollBarInfo(LONG idObject, PSCROLLBARINFO psbi) const
{
	ASSERT(::IsWindow(m_hWnd));
	ENSURE(psbi != NULL);
	return ::GetScrollBarInfo(m_hWnd, idObject, psbi);
}

AFX_INLINE BOOL CWnd::GetTitleBarInfo(PTITLEBARINFO pti) const
{
	ASSERT(::IsWindow(m_hWnd));
	ENSURE(pti != NULL);
	return ::GetTitleBarInfo(m_hWnd, pti);
}

AFX_INLINE BOOL CWnd::AnimateWindow(DWORD dwTime, DWORD dwFlags)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::AnimateWindow(m_hWnd, dwTime, dwFlags);
}

AFX_INLINE BOOL CWnd::FlashWindowEx(DWORD dwFlags, UINT  uCount, DWORD dwTimeout)
{
	ASSERT(::IsWindow(m_hWnd));
	FLASHWINFO fwi;
	fwi.cbSize = sizeof(fwi);
	fwi.hwnd = m_hWnd;
	fwi.dwFlags = dwFlags;
	fwi.uCount = uCount;
	fwi.dwTimeout = dwTimeout;

	return ::FlashWindowEx(&fwi);
}

AFX_INLINE BOOL CComboBox::GetComboBoxInfo(PCOMBOBOXINFO pcbi) const
{
	ASSERT(::IsWindow(m_hWnd));
	ENSURE(pcbi != NULL);
	return ::GetComboBoxInfo(m_hWnd, pcbi);
}

AFX_INLINE DWORD CListBox::GetListBoxInfo() const
	{ ASSERT(::IsWindow(m_hWnd)); return ::GetListBoxInfo(m_hWnd); }

AFX_INLINE BOOL CMenu::SetMenuInfo(LPCMENUINFO lpcmi)
	{ ASSERT(::IsMenu(m_hMenu)); return ::SetMenuInfo(m_hMenu, lpcmi); }

AFX_INLINE BOOL CMenu::GetMenuInfo(LPMENUINFO lpcmi) const
	{ ASSERT(::IsMenu(m_hMenu)); return ::GetMenuInfo(m_hMenu, lpcmi); }

#if defined(UNICODE)
AFX_INLINE BOOL CComboBox::SetMinVisibleItems(_In_ int iMinVisible)
{
	ASSERT(::IsWindow(m_hWnd));
	return ComboBox_SetMinVisible(m_hWnd, iMinVisible); // CB_SETMINVISIBLE
}

AFX_INLINE int CComboBox::GetMinVisible() const
{
	ASSERT(::IsWindow(m_hWnd));
	return ComboBox_GetMinVisible(m_hWnd); // CB_GETMINVISIBLE
}

#ifdef CB_SETCUEBANNER
AFX_INLINE BOOL CComboBox::SetCueBanner(_In_z_ LPCTSTR lpszText)
{
	ASSERT(::IsWindow(m_hWnd));
	return ComboBox_SetCueBannerText(m_hWnd, lpszText); // CB_SETCUEBANNER
}

AFX_INLINE CString CComboBox::GetCueBanner() const
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpszText=NULL;
	int cchText = 0;
	CString strOut;
	BOOL b = CWnd::EnlargeBufferGetText<BOOL>(FALSE, lpszText, cchText, CB_GETCUEBANNER, (WPARAM&)lpszText, (LPARAM&)cchText, strOut);
	if (b)
		return strOut;
	else
		return CString();
}

AFX_INLINE BOOL CComboBox::GetCueBanner(_Out_writes_z_(cchText) LPWSTR lpszText, _In_ int cchText) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(lpszText != NULL);
	if (cchText == 0 || lpszText == NULL)
		return FALSE;

	lpszText[0] = L'\0';
	return ComboBox_GetCueBannerText(m_hWnd, lpszText, cchText); // CB_GETCUEBANNER
}
#endif  // CB_SETCUEBANNER
#endif  // defined(UNICODE)

AFX_INLINE BOOL CWnd::SetLayeredWindowAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SetLayeredWindowAttributes(m_hWnd, crKey, bAlpha, dwFlags);
}

AFX_INLINE BOOL CWnd::UpdateLayeredWindow(CDC* pDCDst, POINT *pptDst, SIZE *psize,
		CDC* pDCSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::UpdateLayeredWindow(m_hWnd, pDCDst->GetSafeHdc(), pptDst, psize,
		pDCSrc->GetSafeHdc(), pptSrc, crKey, pblend, dwFlags);
}

AFX_INLINE BOOL CWnd::GetLayeredWindowAttributes(COLORREF *pcrKey, BYTE *pbAlpha, DWORD *pdwFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	return ::GetLayeredWindowAttributes(m_hWnd, pcrKey, pbAlpha, pdwFlags);
}

AFX_INLINE BOOL CWnd::PrintWindow(CDC* pDC, UINT nFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	return ::PrintWindow(m_hWnd, pDC->GetSafeHdc(), nFlags);
}

AFX_INLINE BOOL CScrollBar::GetScrollBarInfo(PSCROLLBARINFO pScrollInfo) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, SBM_GETSCROLLBARINFO, NULL, (LPARAM)pScrollInfo); }

AFX_INLINE BOOL CEdit::SetCueBanner(_In_z_ LPCWSTR lpszText, _In_ BOOL fDrawWhenFocused /* = FALSE */)
{
	ASSERT(::IsWindow(m_hWnd));
#ifdef Edit_SetCueBannerTextFocused
	return Edit_SetCueBannerTextFocused(m_hWnd, lpszText, fDrawWhenFocused); // EM_SETCUEBANNER
#else
	(fDrawWhenFocused);
	return Edit_SetCueBannerText(m_hWnd, lpszText); // EM_SETCUEBANNER
#endif
}

AFX_INLINE BOOL CEdit::GetCueBanner(_Out_writes_z_(cchText) LPWSTR lpszText, _In_ int cchText) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(lpszText != NULL);
	if (cchText == 0 || lpszText == NULL)
		return FALSE;

	lpszText[0] = L'\0';

	return Edit_GetCueBannerText(m_hWnd, lpszText, cchText); // EM_GETCUEBANNER
}

#ifdef UNICODE
AFX_INLINE CString CEdit::GetCueBanner() const
{
	ASSERT(::IsWindow(m_hWnd));
	LPTSTR lpszText=NULL;
	int cchText = 0;
	CString strOut;
	BOOL b = CWnd::EnlargeBufferGetText<BOOL>(FALSE, lpszText, cchText, EM_GETCUEBANNER, (WPARAM&)lpszText, (LPARAM&)cchText, strOut);
	if (b)
		return strOut;
	else
		return CString();
}

AFX_INLINE BOOL CEdit::ShowBalloonTip(_In_z_ LPCWSTR lpszTitle, _In_z_ LPCWSTR lpszText, _In_ INT ttiIcon)
{
	ASSERT(::IsWindow(m_hWnd));
	EDITBALLOONTIP bt;
	::ZeroMemory(&bt, sizeof(EDITBALLOONTIP));

	bt.cbStruct = sizeof(EDITBALLOONTIP);
	bt.pszTitle = lpszTitle;
	bt.pszText = lpszText;
	bt.ttiIcon = ttiIcon;
	
	return Edit_ShowBalloonTip(m_hWnd, &bt); // EM_SHOWBALLOONTIP
}

AFX_INLINE BOOL CEdit::ShowBalloonTip(_In_ PEDITBALLOONTIP pEditBalloonTip)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pEditBalloonTip != NULL);
	if (pEditBalloonTip == NULL)
	{
		return FALSE;
	}
	return Edit_ShowBalloonTip(m_hWnd, pEditBalloonTip); // EM_SHOWBALLOONTIP
}

AFX_INLINE BOOL CEdit::HideBalloonTip()
{
	ASSERT(::IsWindow(m_hWnd));
	return Edit_HideBalloonTip(m_hWnd); // EM_HIDEBALLOONTIP
}
#endif // (UNICODE)

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

AFX_INLINE void CEdit::SetHighlight(_In_ int ichStart, _In_ int ichEnd)
{
	ASSERT(::IsWindow(m_hWnd));
	return Edit_SetHilite(m_hWnd, ichStart, ichEnd); // EM_SETHILITE
}

AFX_INLINE BOOL CEdit::GetHighlight(_Out_ int* pichStart, _Out_ int* pichEnd) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pichStart != NULL);
	ASSERT(pichEnd != NULL);
	if (pichStart == NULL || pichEnd == NULL)
		return FALSE;

	DWORD dwHighlight = Edit_GetHilite(m_hWnd); // EM_GETHILITE
	*pichStart = (int)LOWORD(dwHighlight);
	*pichEnd = (int)HIWORD(dwHighlight);
	return TRUE;
}

#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

#pragma warning(push)
#pragma warning(disable: 6001)
AFX_INLINE BOOL CButton::GetIdealSize(_Out_ SIZE* psize) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(psize != NULL);
	return Button_GetIdealSize(m_hWnd, psize); // BCM_GETIDEALSIZE
}
#pragma warning(pop)

AFX_INLINE BOOL CButton::SetImageList(_In_ PBUTTON_IMAGELIST pbuttonImagelist)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pbuttonImagelist != NULL);
	return Button_SetImageList(m_hWnd, pbuttonImagelist); // BCM_SETIMAGELIST
}

AFX_INLINE BOOL CButton::GetImageList(_In_ PBUTTON_IMAGELIST pbuttonImagelist) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pbuttonImagelist != NULL);
	return Button_GetImageList(m_hWnd, pbuttonImagelist); // BCM_GETIMAGELIST
}

AFX_INLINE BOOL CButton::SetTextMargin(_In_ RECT* pmargin)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pmargin != NULL);
	return Button_SetTextMargin(m_hWnd, pmargin); // BCM_SETTEXTMARGIN
}

#pragma warning(push)
#pragma warning(disable: 6001)
AFX_INLINE BOOL CButton::GetTextMargin(_Out_ RECT* pmargin) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pmargin != NULL);
	return Button_GetTextMargin(m_hWnd, pmargin); // BCM_GETTEXTMARGIN
}
#pragma warning(pop)

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

AFX_INLINE CString CButton::GetNote() const
{
	ASSERT(::IsWindow(m_hWnd));
	UINT nCount = 256;
	CString str;
	LPTSTR lpstr = str.GetBufferSetLength(nCount);
	if (lpstr == NULL)
	{
		AfxThrowMemoryException();
	}
	while((BOOL)Button_GetNote(m_hWnd, lpstr, &nCount) == FALSE)
	{
		if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			AfxThrowNotSupportedException();
		}
		lpstr = str.GetBufferSetLength(nCount);
		if (lpstr == NULL)
		{
			AfxThrowMemoryException();
		}
	}
	str.ReleaseBuffer();
	return str;
}

#pragma warning(push)
#pragma warning(disable: 6054)
_Check_return_ AFX_INLINE BOOL CButton::GetNote(_Out_writes_z_(*pcchNote) LPTSTR lpszNote, _Inout_ UINT* pcchNote) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(lpszNote != NULL);
	ASSERT(pcchNote != NULL);
	return Button_GetNote(m_hWnd, lpszNote, pcchNote); // BCM_GETNOTE
}
#pragma warning(pop)

AFX_INLINE BOOL CButton::SetNote(_In_z_ LPCTSTR lpszNote)
{
	ASSERT(::IsWindow(m_hWnd));
	return Button_SetNote(m_hWnd, lpszNote); // BCM_SETNOTE
}

AFX_INLINE UINT CButton::GetNoteLength() const
{
	ASSERT(::IsWindow(m_hWnd));
	return (UINT)Button_GetNoteLength(m_hWnd); // BCM_GETNOTELENGTH
}

#pragma warning(push)
#pragma warning(disable: 6001)
AFX_INLINE BOOL CButton::GetSplitInfo(_Out_ PBUTTON_SPLITINFO pInfo) const
{
	ASSERT(::IsWindow(m_hWnd));
	return Button_GetSplitInfo(m_hWnd, pInfo); // BCM_GETSPLITINFO
}
#pragma warning(pop)

AFX_INLINE BOOL CButton::SetSplitInfo(_In_ PBUTTON_SPLITINFO pInfo)
{
	ASSERT(::IsWindow(m_hWnd));
	return Button_SetSplitInfo(m_hWnd, pInfo); // BCM_SETSPLITINFO
}

AFX_INLINE UINT CButton::GetSplitStyle() const
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_STYLE;
	Button_GetSplitInfo(m_hWnd, &buttonSplitInfo);
	return buttonSplitInfo.uSplitStyle;
}

AFX_INLINE BOOL CButton::SetSplitStyle(_In_ UINT nStyle)
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_STYLE;
	buttonSplitInfo.uSplitStyle = nStyle;
	return Button_SetSplitInfo(m_hWnd, &buttonSplitInfo);
}

AFX_INLINE BOOL CButton::GetSplitSize(_Out_ LPSIZE pSize) const
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_SIZE;
	BOOL fRet = Button_GetSplitInfo(m_hWnd, &buttonSplitInfo);
	pSize->cx = buttonSplitInfo.size.cx;
	pSize->cy = buttonSplitInfo.size.cy;
	return fRet;
}

AFX_INLINE BOOL CButton::SetSplitSize(_In_ LPSIZE pSize)
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_SIZE;
	buttonSplitInfo.size.cx = pSize->cx;
	buttonSplitInfo.size.cy = pSize->cy;
	return Button_SetSplitInfo(m_hWnd, &buttonSplitInfo);
}

AFX_INLINE CImageList* CButton::GetSplitImageList() const
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_IMAGE;
	Button_GetSplitInfo(m_hWnd, &buttonSplitInfo);
	return CImageList::FromHandle(buttonSplitInfo.himlGlyph);
}

AFX_INLINE BOOL CButton::SetSplitImageList(_In_ CImageList* pSplitImageList)
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_IMAGE;
	buttonSplitInfo.himlGlyph = pSplitImageList->GetSafeHandle();
	return Button_SetSplitInfo(m_hWnd, &buttonSplitInfo);
}

AFX_INLINE TCHAR CButton::GetSplitGlyph() const
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_GLYPH;
	Button_GetSplitInfo(m_hWnd, &buttonSplitInfo);
#pragma warning(push)
#pragma warning(disable: 4302) // 'type cast' : truncation from 'HIMAGELIST' to 'WCHAR'
	return (TCHAR)buttonSplitInfo.himlGlyph;
#pragma warning(pop)
}

AFX_INLINE BOOL CButton::SetSplitGlyph(_In_ TCHAR chGlyph)
{
	ASSERT(::IsWindow(m_hWnd));
	BUTTON_SPLITINFO buttonSplitInfo = {0};
	buttonSplitInfo.mask = BCSIF_GLYPH;
	buttonSplitInfo.himlGlyph = (HIMAGELIST)chGlyph;
	return Button_SetSplitInfo(m_hWnd, &buttonSplitInfo);
}

AFX_INLINE BOOL CButton::SetDropDownState(_In_ BOOL fDropDown)
{
	ASSERT(::IsWindow(m_hWnd));
	return Button_SetDropDownState(m_hWnd, fDropDown); // BCM_SETDROPDOWNSTATE
}

AFX_INLINE HICON CButton::SetShield(_In_ BOOL fElevationRequired)
{
	ASSERT(::IsWindow(m_hWnd));
	return (HICON)(Button_SetElevationRequiredState(m_hWnd, fElevationRequired)); // BCM_SETSHIELD
}

#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
