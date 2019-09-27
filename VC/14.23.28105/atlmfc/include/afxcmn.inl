// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXCMN.H

#pragma once

#ifdef _AFXCMN_INLINE

#pragma warning(push)
#pragma warning(disable: 6001)

_AFXCMN_INLINE CDragListBox::CDragListBox()
	{ m_nLast = -1;}
_AFXCMN_INLINE int CDragListBox::ItemFromPt(_In_ CPoint pt, _In_ BOOL bAutoScroll) const
	{ASSERT(::IsWindow(m_hWnd)); return LBItemFromPt(m_hWnd, pt, bAutoScroll); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CToolBarCtrl::CToolBarCtrl()
	{ }
_AFXCMN_INLINE BOOL CToolBarCtrl::EnableButton(_In_ int nID, _In_ BOOL bEnable)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ENABLEBUTTON, nID, MAKELPARAM(bEnable, 0)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::CheckButton(_In_ int nID, _In_ BOOL bCheck)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_CHECKBUTTON, nID, MAKELPARAM(bCheck, 0)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::PressButton(_In_ int nID, _In_ BOOL bPress)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_PRESSBUTTON, nID, MAKELPARAM(bPress, 0)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::HideButton(_In_ int nID, _In_ BOOL bHide)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_HIDEBUTTON, nID, MAKELPARAM(bHide, 0)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::Indeterminate(_In_ int nID, _In_ BOOL bIndeterminate)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_INDETERMINATE, nID, MAKELPARAM(bIndeterminate, 0)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::IsButtonEnabled(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONENABLED, nID, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::IsButtonChecked(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONCHECKED, nID, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::IsButtonPressed(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONPRESSED, nID, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::IsButtonHidden(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONHIDDEN, nID, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::IsButtonIndeterminate(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONINDETERMINATE, nID, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetState(_In_ int nID, _In_ UINT nState)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETSTATE, nID, MAKELPARAM(nState, 0)); }
_AFXCMN_INLINE int CToolBarCtrl::GetState(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETSTATE, nID, 0L); }
_AFXCMN_INLINE BOOL CToolBarCtrl::AddButtons(_In_ int nNumButtons, _In_ LPTBBUTTON lpButtons)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ADDBUTTONS, nNumButtons, (LPARAM)lpButtons); }
_AFXCMN_INLINE BOOL CToolBarCtrl::InsertButton(_In_ int nIndex, _In_ LPTBBUTTON lpButton)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_INSERTBUTTON, nIndex, (LPARAM)lpButton); }
_AFXCMN_INLINE BOOL CToolBarCtrl::DeleteButton(_In_ int nIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_DELETEBUTTON, nIndex, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::GetButton(_In_ int nIndex, _Out_ LPTBBUTTON lpButton) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETBUTTON, nIndex, (LPARAM)lpButton); }
_AFXCMN_INLINE int CToolBarCtrl::GetButtonCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_BUTTONCOUNT, 0, 0L); }
_AFXCMN_INLINE UINT CToolBarCtrl::CommandToIndex(_In_ UINT nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TB_COMMANDTOINDEX, nID, 0L); }
_AFXCMN_INLINE void CToolBarCtrl::Customize()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_CUSTOMIZE, 0, 0L); }
// lpszStrings are separated by zeroes, last one is marked by two zeroes
_AFXCMN_INLINE int CToolBarCtrl::AddStrings(_In_z_ LPCTSTR lpszStrings)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_ADDSTRING, 0, (LPARAM)lpszStrings); }
_AFXCMN_INLINE BOOL CToolBarCtrl::GetItemRect(_In_ int nIndex, _Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETITEMRECT, nIndex, (LPARAM)lpRect); }
_AFXCMN_INLINE void CToolBarCtrl::SetButtonStructSize(_In_ int nSize)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_BUTTONSTRUCTSIZE, nSize, 0L); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetButtonSize(_In_ CSize size)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(size.cx, size.cy)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetBitmapSize(_In_ CSize size)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETBITMAPSIZE, 0, MAKELPARAM(size.cx, size.cy)); }
_AFXCMN_INLINE void CToolBarCtrl::AutoSize()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_AUTOSIZE, 0, 0L); }
_AFXCMN_INLINE CToolTipCtrl* CToolBarCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TB_GETTOOLTIPS, 0, 0L)); }
_AFXCMN_INLINE void CToolBarCtrl::SetToolTips(_In_ CToolTipCtrl* pTip)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETTOOLTIPS, (WPARAM)pTip->GetSafeHwnd(), 0L); }
_AFXCMN_INLINE void CToolBarCtrl::SetOwner(_In_ CWnd* pWnd)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETPARENT, (WPARAM)pWnd->GetSafeHwnd(), 0L); }
_AFXCMN_INLINE void CToolBarCtrl::SetRows(_In_ int nRows, _In_ BOOL bLarger, _In_ LPRECT lpRect)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETROWS, MAKEWPARAM(nRows, bLarger), (LPARAM)lpRect); }
_AFXCMN_INLINE int CToolBarCtrl::GetRows() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETROWS, 0, 0L); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetCmdID(_In_ int nIndex, _In_ UINT nID)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETCMDID, nIndex, nID); }
_AFXCMN_INLINE UINT CToolBarCtrl::GetBitmapFlags() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TB_GETBITMAPFLAGS, 0, 0L); }
_AFXCMN_INLINE int CToolBarCtrl::GetBitmap(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETBITMAP, nID, 0L); }
_AFXCMN_INLINE BOOL CToolBarCtrl::ReplaceBitmap(_In_ LPTBREPLACEBITMAP pReplaceBitmap)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_REPLACEBITMAP, 0, (LPARAM)pReplaceBitmap); }
_AFXCMN_INLINE BOOL CToolBarCtrl::ChangeBitmap(_In_ int idButton, _In_ int iBitmap)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_CHANGEBITMAP, idButton, (LPARAM)MAKELPARAM(iBitmap, 0)); }
_AFXCMN_INLINE int CToolBarCtrl::HitTest(_In_ LPPOINT ppt) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)ppt); }
_AFXCMN_INLINE DWORD CToolBarCtrl::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_GETEXTENDEDSTYLE, 0, 0L); }
_AFXCMN_INLINE DWORD CToolBarCtrl::SetExtendedStyle(_In_ DWORD dwExStyle)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_SETEXTENDEDSTYLE, 0, dwExStyle); }
_AFXCMN_INLINE void CToolBarCtrl::GetPadding(_Out_ int& nHorzPadding, _Out_ int& nVertPadding) const
	{ ASSERT(::IsWindow(m_hWnd)); DWORD dwPadding = (DWORD)::SendMessage(m_hWnd, TB_GETPADDING, 0, 0L); nHorzPadding = (int)LOWORD(dwPadding); nVertPadding = (int)HIWORD(dwPadding); }
_AFXCMN_INLINE DWORD CToolBarCtrl::SetPadding(_In_ int nHorzPadding, _In_ int nVertPadding)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_SETPADDING, 0, (LPARAM)MAKELPARAM(nHorzPadding, nVertPadding)); }
_AFXCMN_INLINE COLORREF CToolBarCtrl::GetInsertMarkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TB_GETINSERTMARKCOLOR, 0, 0); }
_AFXCMN_INLINE COLORREF CToolBarCtrl::SetInsertMarkColor(_In_ COLORREF clrNew)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TB_SETINSERTMARKCOLOR, 0, (LPARAM) clrNew); }
_AFXCMN_INLINE BOOL CToolBarCtrl::GetColorScheme(_Out_ COLORSCHEME* lpcs) const
	{ ASSERT(::IsWindow(m_hWnd)); lpcs->dwSize = ULONG(sizeof(COLORSCHEME)); return (BOOL)(::SendMessage(m_hWnd, TB_GETCOLORSCHEME, 0, (LPARAM)lpcs) != 0); }
_AFXCMN_INLINE void CToolBarCtrl::SetColorScheme(_In_ const COLORSCHEME* lpcs)
	{ ASSERT(::IsWindow(m_hWnd)); ((COLORSCHEME*)lpcs)->dwSize = sizeof(COLORSCHEME); ::SendMessage(m_hWnd, TB_SETCOLORSCHEME, 0, (LPARAM)lpcs); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CStatusBarCtrl::CStatusBarCtrl()
	{ }
_AFXCMN_INLINE BOOL CStatusBarCtrl::SetText(_In_z_ LPCTSTR lpszText, _In_ int nPane, _In_ int nType)
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT(nPane < 256); return (BOOL) ::SendMessage(m_hWnd, SB_SETTEXT, (nPane|nType), (LPARAM)lpszText); }
_AFXCMN_INLINE BOOL CStatusBarCtrl::SetParts(_In_ int nParts, _In_ int* pWidths)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, SB_SETPARTS, nParts, (LPARAM)pWidths); }
_AFXCMN_INLINE int CStatusBarCtrl::GetParts(_In_ int nParts, _Out_ int* pParts) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, SB_GETPARTS, nParts, (LPARAM)pParts); }
_AFXCMN_INLINE BOOL CStatusBarCtrl::GetBorders(_Out_ int* pBorders) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, SB_GETBORDERS, 0, (LPARAM)pBorders); }
_AFXCMN_INLINE void CStatusBarCtrl::SetMinHeight(_In_ int nMin)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, SB_SETMINHEIGHT, nMin, 0L); }
_AFXCMN_INLINE BOOL CStatusBarCtrl::SetSimple(_In_ BOOL bSimple)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, SB_SIMPLE, bSimple, 0L); }
_AFXCMN_INLINE BOOL CStatusBarCtrl::GetRect(_In_ int nPane, _Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, SB_GETRECT, nPane, (LPARAM)lpRect); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CListCtrl::CListCtrl()
	{ }
_AFXCMN_INLINE CToolTipCtrl* CListCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_GETTOOLTIPS, 0, 0L)); }
_AFXCMN_INLINE CToolTipCtrl* CListCtrl::SetToolTips(_In_ CToolTipCtrl* pTip)
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_SETTOOLTIPS, 0, (LPARAM) pTip->GetSafeHwnd())); }
_AFXCMN_INLINE COLORREF CListCtrl::GetBkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, LVM_GETBKCOLOR, 0, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::SetBkColor(_In_ COLORREF cr)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETBKCOLOR, 0, cr); }
_AFXCMN_INLINE CImageList* CListCtrl::GetImageList(_In_ int nImageList) const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, LVM_GETIMAGELIST, nImageList, 0L)); }
_AFXCMN_INLINE CImageList* CListCtrl::SetImageList(_In_ CImageList* pImageList, _In_ int nImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, LVM_SETIMAGELIST, nImageList, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE int CListCtrl::GetItemCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::GetItem(_Out_ LVITEM* pItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem); }
_AFXCMN_INLINE BOOL CListCtrl::SetItem(_In_ const LVITEM* pItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)pItem); }
_AFXCMN_INLINE BOOL CListCtrl::SetItemData(_In_ int nItem, _In_ DWORD_PTR dwData)
	{ ASSERT(::IsWindow(m_hWnd)); return SetItem(nItem, 0, LVIF_PARAM, NULL, 0, 0, 0, (LPARAM)dwData); }
_AFXCMN_INLINE int CListCtrl::InsertItem(_In_ const LVITEM* pItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)pItem); }
_AFXCMN_INLINE int CListCtrl::InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem)
	{ ASSERT(::IsWindow(m_hWnd)); return InsertItem(LVIF_TEXT, nItem, lpszItem, 0, 0, 0, 0); }
_AFXCMN_INLINE int CListCtrl::InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage)
	{ ASSERT(::IsWindow(m_hWnd)); return InsertItem(LVIF_TEXT|LVIF_IMAGE, nItem, lpszItem, 0, 0, nImage, 0); }
_AFXCMN_INLINE BOOL CListCtrl::DeleteItem(_In_ int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::DeleteAllItems()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_DELETEALLITEMS, 0, 0L); }
_AFXCMN_INLINE UINT CListCtrl::GetCallbackMask() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, LVM_GETCALLBACKMASK, 0, 0); }
_AFXCMN_INLINE BOOL CListCtrl::SetCallbackMask(_In_ UINT nMask)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETCALLBACKMASK, nMask, 0); }
_AFXCMN_INLINE int CListCtrl::GetNextItem(_In_ int nItem, _In_ int nFlags) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETNEXTITEM, nItem, MAKELPARAM(nFlags, 0)); }
_AFXCMN_INLINE POSITION CListCtrl::GetFirstSelectedItemPosition() const
	{ ASSERT(::IsWindow(m_hWnd)); return (POSITION)(DWORD_PTR)(1+GetNextItem(-1, LVIS_SELECTED)); }
_AFXCMN_INLINE int CListCtrl::GetNextSelectedItem(_Inout_ POSITION& pos) const
	{ ASSERT(::IsWindow(m_hWnd)); DWORD_PTR nOldPos = (DWORD_PTR)pos-1; pos = (POSITION)(DWORD_PTR)(1+GetNextItem((UINT)nOldPos, LVIS_SELECTED)); return (UINT)nOldPos; }
_AFXCMN_INLINE int CListCtrl::FindItem(_In_ LVFINDINFO* pFindInfo, _In_ int nStart) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_FINDITEM, nStart, (LPARAM)pFindInfo); }
_AFXCMN_INLINE int CListCtrl::HitTest(_In_ LVHITTESTINFO* pHitTestInfo) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)pHitTestInfo); }
_AFXCMN_INLINE BOOL CListCtrl::SetItemPosition(_In_ int nItem, _In_ POINT pt)  // LVM_SETITEMPOSITION is not supported, only LVM_SETITEMPOSITION32 is.
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT((GetStyle() & LVS_OWNERDATA)==0); return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMPOSITION32, nItem, (LPARAM)&pt); }
_AFXCMN_INLINE BOOL CListCtrl::GetItemPosition(_In_ int nItem, _Out_ LPPOINT lpPoint) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETITEMPOSITION, nItem, (LPARAM)lpPoint); }
_AFXCMN_INLINE int CListCtrl::GetStringWidth(_In_z_ LPCTSTR lpsz) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETSTRINGWIDTH, 0, (LPARAM)lpsz); }
_AFXCMN_INLINE BOOL CListCtrl::EnsureVisible(_In_ int nItem, _In_ BOOL bPartialOK)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_ENSUREVISIBLE, nItem, MAKELPARAM(bPartialOK, 0)); }
_AFXCMN_INLINE BOOL CListCtrl::Scroll(_In_ CSize size)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SCROLL, size.cx, size.cy); }
_AFXCMN_INLINE BOOL CListCtrl::RedrawItems(_In_ int nFirst, _In_ int nLast)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_REDRAWITEMS, nFirst, nLast); }
_AFXCMN_INLINE BOOL CListCtrl::Arrange(_In_ UINT nCode)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_ARRANGE, nCode, 0L); }
_AFXCMN_INLINE CEdit* CListCtrl::EditLabel(_In_ int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle( (HWND)::SendMessage(m_hWnd, LVM_EDITLABEL, nItem, 0L)); }
_AFXCMN_INLINE CEdit* CListCtrl::GetEditControl() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle( (HWND)::SendMessage(m_hWnd, LVM_GETEDITCONTROL, 0, 0L)); }
_AFXCMN_INLINE BOOL CListCtrl::GetColumn(_In_ int nCol, _Out_ LVCOLUMN* pColumn) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETCOLUMN, nCol, (LPARAM)pColumn); }
_AFXCMN_INLINE BOOL CListCtrl::SetColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETCOLUMN, nCol, (LPARAM)pColumn); }
_AFXCMN_INLINE int CListCtrl::InsertColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)pColumn); }
_AFXCMN_INLINE BOOL CListCtrl::DeleteColumn(_In_ int nCol)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_DELETECOLUMN, nCol, 0); }
_AFXCMN_INLINE int CListCtrl::GetColumnWidth(_In_ int nCol) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, nCol, 0); }
_AFXCMN_INLINE BOOL CListCtrl::SetColumnWidth(_In_ int nCol, _In_ int cx)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, nCol, MAKELPARAM(cx, 0)); }
_AFXCMN_INLINE BOOL CListCtrl::GetViewRect(_Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETVIEWRECT, 0, (LPARAM)lpRect); }
_AFXCMN_INLINE COLORREF CListCtrl::GetTextColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, LVM_GETTEXTCOLOR, 0, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::SetTextColor(_In_ COLORREF cr)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETTEXTCOLOR, 0, cr); }
_AFXCMN_INLINE COLORREF CListCtrl::GetTextBkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, LVM_GETTEXTBKCOLOR, 0, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::SetTextBkColor(_In_ COLORREF cr)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETTEXTBKCOLOR, 0, cr); }
_AFXCMN_INLINE int CListCtrl::GetTopIndex() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETTOPINDEX, 0, 0); }
_AFXCMN_INLINE int CListCtrl::GetCountPerPage() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETCOUNTPERPAGE, 0, 0); }
_AFXCMN_INLINE BOOL CListCtrl::GetOrigin(_Out_ LPPOINT lpPoint) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETORIGIN, 0, (LPARAM)lpPoint); }
_AFXCMN_INLINE BOOL CListCtrl::Update(_In_ int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_UPDATE, nItem, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::SetItemState(_In_ int nItem, _In_ LVITEM* pItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)pItem); }
_AFXCMN_INLINE UINT CListCtrl::GetItemState(_In_ int nItem, _In_ UINT nMask) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, LVM_GETITEMSTATE, nItem, nMask); }
_AFXCMN_INLINE void CListCtrl::SetItemCount(_In_ int nItems)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, 0); }
_AFXCMN_INLINE BOOL CListCtrl::SortItems(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData)
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT((GetStyle() & LVS_OWNERDATA)==0); return (BOOL) ::SendMessage(m_hWnd, LVM_SORTITEMS, dwData, (LPARAM)pfnCompare); }
_AFXCMN_INLINE BOOL CListCtrl::SortItemsEx(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData)
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT((GetStyle() & LVS_OWNERDATA)==0); return (BOOL) ::SendMessage(m_hWnd, LVM_SORTITEMSEX, dwData, (LPARAM)pfnCompare); }
_AFXCMN_INLINE UINT CListCtrl::GetSelectedCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0L); }
_AFXCMN_INLINE BOOL CListCtrl::GetItemSpacing(_In_ BOOL fSmall, _Out_ int* pnHorzSpacing, _Out_ int* pnVertSpacing) const
{ 
	ASSERT(::IsWindow(m_hWnd)); 
	ASSERT(pnHorzSpacing != NULL);
	ASSERT(pnVertSpacing != NULL);
        if (pnHorzSpacing == NULL || pnVertSpacing == NULL)
        {
            return FALSE;
        }
	DWORD dwSpacing = (DWORD)::SendMessage(m_hWnd, LVM_GETITEMSPACING, (WPARAM)fSmall, 0L); 
	*pnHorzSpacing = (int)LOWORD(dwSpacing); 
	*pnVertSpacing = (int)HIWORD(dwSpacing); 
        return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CTreeCtrl::CTreeCtrl()
	{ }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::InsertItem(_In_ LPTVINSERTSTRUCT lpInsertStruct)
	{ ASSERT(::IsWindow(m_hWnd));  return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)lpInsertStruct); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::InsertItem(_In_z_ LPCTSTR lpszItem, _In_ int nImage,
	_In_ int nSelectedImage, _In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter)
	{ ASSERT(::IsWindow(m_hWnd)); return InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImage, nSelectedImage, 0, 0, 0, hParent, hInsertAfter); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::InsertItem(_In_z_ LPCTSTR lpszItem, _In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter)
	{ ASSERT(::IsWindow(m_hWnd)); return InsertItem(TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0, hParent, hInsertAfter); }
_AFXCMN_INLINE BOOL CTreeCtrl::DeleteItem(_In_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)hItem); }
#pragma warning(push)
#pragma warning(disable: 4310)
_AFXCMN_INLINE BOOL CTreeCtrl::DeleteAllItems()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT); }
#pragma warning(pop)
_AFXCMN_INLINE BOOL CTreeCtrl::Expand(_In_ HTREEITEM hItem, _In_ UINT nCode)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_EXPAND, nCode, (LPARAM)hItem); }
_AFXCMN_INLINE UINT CTreeCtrl::GetCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, TVM_GETCOUNT, 0, 0); }
_AFXCMN_INLINE UINT CTreeCtrl::GetIndent() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, TVM_GETINDENT, 0, 0); }
_AFXCMN_INLINE void CTreeCtrl::SetIndent(_In_ UINT nIndent)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TVM_SETINDENT, nIndent, 0); }
_AFXCMN_INLINE CImageList* CTreeCtrl::GetImageList(_In_ int nImageList) const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TVM_GETIMAGELIST, (WPARAM)nImageList, 0)); }
_AFXCMN_INLINE CImageList* CTreeCtrl::SetImageList(_In_opt_ CImageList* pImageList, _In_ int nImageList)
{
	ASSERT(::IsWindow(m_hWnd));
#pragma warning(suppress : 6011) // GetSafeHandle does the "right" thing even when called on a null pointer, so the warning is spurious.
	return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM)nImageList, (LPARAM)pImageList->GetSafeHandle()));
}
_AFXCMN_INLINE UINT CTreeCtrl::SetScrollTime(_In_ UINT uScrollTime)
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_SETSCROLLTIME, (WPARAM) uScrollTime, 0); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetNextItem(_In_ HTREEITEM hItem, _In_ UINT nCode) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, nCode, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetChildItem(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetNextSiblingItem(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetPrevSiblingItem(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetParentItem(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetFirstVisibleItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetNextVisibleItem(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetPrevVisibleItem(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE, (LPARAM)hItem); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetLastVisibleItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_LASTVISIBLE, 0); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetSelectedItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0); }
_AFXCMN_INLINE UINT CTreeCtrl::GetScrollTime() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_GETSCROLLTIME, 0, 0); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetDropHilightItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::GetRootItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0); }
_AFXCMN_INLINE BOOL CTreeCtrl::Select(_In_ HTREEITEM hItem, _In_ UINT nCode)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, nCode, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SelectItem(_In_opt_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SelectDropTarget(_In_opt_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SelectSetFirstVisible(_In_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::GetItem(_Out_ TVITEM* pItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)pItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItem(_In_ TVITEM* pItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)pItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItemText(_In_ HTREEITEM hItem, _In_z_ LPCTSTR lpszItem)
	{ ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_TEXT, lpszItem, 0, 0, 0, 0, NULL); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItemImage(_In_ HTREEITEM hItem, _In_ int nImage, _In_ int nSelectedImage)
	{ ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_IMAGE|TVIF_SELECTEDIMAGE, NULL, nImage, nSelectedImage, 0, 0, NULL); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItemState(_In_ HTREEITEM hItem, _In_ UINT nState, _In_ UINT nStateMask)
	{ ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_STATE, NULL, 0, 0, nState, nStateMask, NULL); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItemData(_In_ HTREEITEM hItem, _In_ DWORD_PTR dwData)
	{ ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)dwData); }
_AFXCMN_INLINE CEdit* CTreeCtrl::EditLabel(_In_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem)); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::HitTest(_In_ TVHITTESTINFO* pHitTestInfo) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)pHitTestInfo); }
_AFXCMN_INLINE CEdit* CTreeCtrl::GetEditControl() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_GETEDITCONTROL, 0, 0)); }
_AFXCMN_INLINE UINT CTreeCtrl::GetVisibleCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, TVM_GETVISIBLECOUNT, 0, 0); }
_AFXCMN_INLINE BOOL CTreeCtrl::SortChildren(_In_opt_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDREN, 0, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::EnsureVisible(_In_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SortChildrenCB(_In_ LPTVSORTCB pSort)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDRENCB, 0, (LPARAM)pSort); }
_AFXCMN_INLINE CToolTipCtrl* CTreeCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_GETTOOLTIPS, 0, 0L)); }
_AFXCMN_INLINE CToolTipCtrl* CTreeCtrl::SetToolTips(_In_ CToolTipCtrl* pWndTip)
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_SETTOOLTIPS, (WPARAM)pWndTip->GetSafeHwnd(), 0L)); }
_AFXCMN_INLINE COLORREF CTreeCtrl::GetBkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_GETBKCOLOR, 0, 0L); }
_AFXCMN_INLINE COLORREF CTreeCtrl::SetBkColor(_In_ COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_SETBKCOLOR, 0, (LPARAM)clr); }
_AFXCMN_INLINE SHORT CTreeCtrl::GetItemHeight() const
	{ ASSERT(::IsWindow(m_hWnd)); return (SHORT) ::SendMessage(m_hWnd, TVM_GETITEMHEIGHT, 0, 0L); }
_AFXCMN_INLINE SHORT CTreeCtrl::SetItemHeight(_In_ SHORT cyHeight)
	{ ASSERT(::IsWindow(m_hWnd)); return (SHORT) ::SendMessage(m_hWnd, TVM_SETITEMHEIGHT, cyHeight, 0L); }
_AFXCMN_INLINE COLORREF CTreeCtrl::GetTextColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_GETTEXTCOLOR, 0, 0L); }
_AFXCMN_INLINE COLORREF CTreeCtrl::SetTextColor(_In_ COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_SETTEXTCOLOR, 0, (LPARAM)clr); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetInsertMark(_In_ HTREEITEM hItem, _In_ BOOL fAfter)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SETINSERTMARK, fAfter, (LPARAM)hItem); }
_AFXCMN_INLINE COLORREF CTreeCtrl::GetInsertMarkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_GETINSERTMARKCOLOR, 0, 0L); }
_AFXCMN_INLINE COLORREF CTreeCtrl::SetInsertMarkColor(_In_ COLORREF clrNew)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_SETINSERTMARKCOLOR, 0, (LPARAM)clrNew); }
_AFXCMN_INLINE BOOL CTreeCtrl::EndEditLabelNow(_In_ BOOL fCancelWithoutSave)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_ENDEDITLABELNOW, (WPARAM)fCancelWithoutSave, 0); }

_AFXCMN_INLINE COLORREF CTreeCtrl::GetLineColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF)::SendMessage(m_hWnd, TVM_GETLINECOLOR, 0, 0L); }
_AFXCMN_INLINE COLORREF CTreeCtrl::SetLineColor(_In_ COLORREF clrNew /*= CLR_DEFAULT*/)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF)::SendMessage(m_hWnd, TVM_SETLINECOLOR, 0, (LPARAM)clrNew); }

#if defined(UNICODE)
_AFXCMN_INLINE UINT CTreeCtrl::MapItemToAccId(_In_ HTREEITEM hItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_MAPHTREEITEMTOACCID, (WPARAM)hItem, 0); }
_AFXCMN_INLINE HTREEITEM CTreeCtrl::MapAccIdToItem(_In_ UINT uAccId) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM) ::SendMessage(m_hWnd, TVM_MAPACCIDTOHTREEITEM, (WPARAM)uAccId, 0); }
#endif // defined(UNICODE)

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXCMN_INLINE UINT CTreeCtrl::GetSelectedCount()
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_GETSELECTEDCOUNT, 0, 0); }
_AFXCMN_INLINE void CTreeCtrl::ShowInfoTip(_In_ HTREEITEM hItem)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TVM_SHOWINFOTIP, 0, (LPARAM)hItem); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItemStateEx(_In_ HTREEITEM hItem, _In_ UINT uStateEx)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SetItemEx(hItem, TVIF_STATEEX, 0, 0, 0, 0, 0, 0, uStateEx, 0, 0); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetItemExpandedImageIndex(_In_ HTREEITEM hItem, _In_ int nIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SetItemEx(hItem, TVIF_EXPANDEDIMAGE, 0, 0, 0, 0, 0, 0, TVIF_EXPANDEDIMAGE, 0, nIndex); }
_AFXCMN_INLINE DWORD CTreeCtrl::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TVM_GETEXTENDEDSTYLE, (WPARAM)0, 0); }
_AFXCMN_INLINE DWORD CTreeCtrl::SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TVM_SETEXTENDEDSTYLE, (WPARAM)dwExMask, (LPARAM)dwExStyles); }
_AFXCMN_INLINE BOOL CTreeCtrl::SetAutoscrollInfo(_In_ UINT uPixelsPerSec, _In_ UINT uUpdateTime)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SETAUTOSCROLLINFO, (WPARAM)uPixelsPerSec, (LPARAM)uUpdateTime); }
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CHotKeyCtrl::CHotKeyCtrl()
	{ }
_AFXCMN_INLINE void CHotKeyCtrl::SetHotKey(_In_ WORD wVirtualKeyCode, _In_ WORD wModifiers)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, HKM_SETHOTKEY, MAKEWORD(wVirtualKeyCode, wModifiers), 0L); }
_AFXCMN_INLINE DWORD CHotKeyCtrl::GetHotKey() const
	{ ASSERT(::IsWindow(m_hWnd)); return DWORD(::SendMessage(m_hWnd, HKM_GETHOTKEY, 0, 0L)); }
_AFXCMN_INLINE void CHotKeyCtrl::SetRules(_In_ WORD wInvalidComb, _In_ WORD wModifiers)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, HKM_SETRULES, wInvalidComb, MAKELPARAM(wModifiers, 0)); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE void CToolTipCtrl::Activate(_In_ BOOL bActivate)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_ACTIVATE, bActivate, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::SetToolInfo(_In_ LPTOOLINFO lpToolInfo)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_SETTOOLINFO, 0, (LPARAM)lpToolInfo); }
_AFXCMN_INLINE void CToolTipCtrl::RelayEvent(_In_ LPMSG lpMsg)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_RELAYEVENT, 0, (LPARAM)lpMsg); }
_AFXCMN_INLINE int CToolTipCtrl::GetToolCount() const
	{ ASSERT(::IsWindow(m_hWnd));  return (int) ::SendMessage(m_hWnd, TTM_GETTOOLCOUNT, 0, 0L); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CSpinButtonCtrl::CSpinButtonCtrl()
 	{ }
_AFXCMN_INLINE UINT CSpinButtonCtrl::GetAccel(_In_ int nAccel, _Out_ UDACCEL* pAccel) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) LOWORD(::SendMessage(m_hWnd, UDM_GETACCEL, nAccel, (LPARAM)pAccel)); }
_AFXCMN_INLINE UINT CSpinButtonCtrl::GetBase() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) LOWORD(::SendMessage(m_hWnd, UDM_GETBASE, 0, 0l)); }
_AFXCMN_INLINE CWnd* CSpinButtonCtrl::GetBuddy() const
	{ ASSERT(::IsWindow(m_hWnd)); return CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, UDM_GETBUDDY, 0, 0l)); }
_AFXCMN_INLINE int CSpinButtonCtrl::GetPos() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) (::SendMessage(m_hWnd, UDM_GETPOS, 0, 0l)); }
_AFXCMN_INLINE DWORD CSpinButtonCtrl::GetRange() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, UDM_GETRANGE, 0, 0l); }
_AFXCMN_INLINE BOOL CSpinButtonCtrl::SetAccel(_In_ int nAccel, _In_ UDACCEL* pAccel)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) LOWORD(::SendMessage(m_hWnd, UDM_SETACCEL, nAccel, (LPARAM)pAccel)); }
_AFXCMN_INLINE int CSpinButtonCtrl::SetBase(_In_ int nBase)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, UDM_SETBASE, nBase, 0L); }
_AFXCMN_INLINE CWnd* CSpinButtonCtrl::SetBuddy(_In_ CWnd* pWndBuddy)
	{ ASSERT(::IsWindow(m_hWnd)); return CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, UDM_SETBUDDY, (WPARAM)pWndBuddy->GetSafeHwnd(), 0L)); }
_AFXCMN_INLINE int CSpinButtonCtrl::SetPos(_In_ int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) (short) LOWORD(::SendMessage(m_hWnd, UDM_SETPOS, 0, MAKELPARAM(nPos, 0))); }
_AFXCMN_INLINE void CSpinButtonCtrl::SetRange(_In_ short nLower, _In_ short nUpper)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, UDM_SETRANGE, 0, MAKELPARAM(nUpper, nLower)); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CSliderCtrl::CSliderCtrl()
	{ }
_AFXCMN_INLINE int CSliderCtrl::GetLineSize() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETLINESIZE, 0, 0l); }
_AFXCMN_INLINE int CSliderCtrl::SetLineSize(_In_ int nSize)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_SETLINESIZE, 0, nSize); }
_AFXCMN_INLINE int CSliderCtrl::GetPageSize() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETPAGESIZE, 0, 0l); }
_AFXCMN_INLINE int CSliderCtrl::SetPageSize(_In_ int nSize)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_SETPAGESIZE, 0, nSize); }
_AFXCMN_INLINE int CSliderCtrl::GetRangeMax() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETRANGEMAX, 0, 0l); }
_AFXCMN_INLINE int CSliderCtrl::GetRangeMin() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETRANGEMIN, 0, 0l); }
_AFXCMN_INLINE void CSliderCtrl::SetRangeMin(_In_ int nMin, _In_ BOOL bRedraw)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_SETRANGEMIN, bRedraw, nMin); }
_AFXCMN_INLINE void CSliderCtrl::SetRangeMax(_In_ int nMax, _In_ BOOL bRedraw)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_SETRANGEMAX, bRedraw, nMax); }
_AFXCMN_INLINE void CSliderCtrl::ClearSel(_In_ BOOL bRedraw)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_CLEARSEL, bRedraw, 0l); }
_AFXCMN_INLINE void CSliderCtrl::GetChannelRect(_Out_ LPRECT lprc) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_GETCHANNELRECT, 0, (LPARAM)lprc); }
_AFXCMN_INLINE void CSliderCtrl::GetThumbRect(_Out_ LPRECT lprc) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_GETTHUMBRECT, 0, (LPARAM)lprc); }
_AFXCMN_INLINE int CSliderCtrl::GetPos() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETPOS, 0, 0l); }
_AFXCMN_INLINE void CSliderCtrl::SetPos(_In_ int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_SETPOS, TRUE, nPos); }
_AFXCMN_INLINE void CSliderCtrl::ClearTics(_In_ BOOL bRedraw)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_CLEARTICS, bRedraw, 0l); }
_AFXCMN_INLINE UINT CSliderCtrl::GetNumTics() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TBM_GETNUMTICS, 0, 0l); }
_AFXCMN_INLINE DWORD* CSliderCtrl::GetTicArray() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD*) ::SendMessage(m_hWnd, TBM_GETPTICS, 0, 0l); }
_AFXCMN_INLINE int CSliderCtrl::GetTic(_In_ int nTic) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETTIC, nTic, 0L); }
_AFXCMN_INLINE int CSliderCtrl::GetTicPos(_In_ int nTic) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETTICPOS, nTic, 0L); }
_AFXCMN_INLINE BOOL CSliderCtrl::SetTic(_In_ int nTic)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TBM_SETTIC, 0, nTic); }
_AFXCMN_INLINE void CSliderCtrl::SetTicFreq(_In_ int nFreq)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_SETTICFREQ, nFreq, 0L); }
_AFXCMN_INLINE CWnd* CSliderCtrl::GetBuddy(_In_ BOOL fLocation) const
	{ ASSERT(::IsWindow(m_hWnd)); return CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, TBM_GETBUDDY, fLocation, 0l)); }
_AFXCMN_INLINE CWnd* CSliderCtrl::SetBuddy(_In_ CWnd* pWndBuddy, _In_ BOOL fLocation)
	{ ASSERT(::IsWindow(m_hWnd)); return CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, TBM_SETBUDDY, fLocation, (LPARAM)pWndBuddy->GetSafeHwnd())); }
_AFXCMN_INLINE CToolTipCtrl* CSliderCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TBM_GETTOOLTIPS, 0, 0L)); }
_AFXCMN_INLINE void CSliderCtrl::SetToolTips(_In_ CToolTipCtrl* pTip)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_SETTOOLTIPS, (WPARAM)pTip->GetSafeHwnd(), 0L); }
_AFXCMN_INLINE int CSliderCtrl::SetTipSide(_In_ int nLocation)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TBM_SETTIPSIDE, nLocation, 0L); }
_AFXCMN_INLINE void CSliderCtrl::SetThumbLength(_In_ int nLength)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TBM_SETTHUMBLENGTH, (WPARAM)nLength, 0L); }
_AFXCMN_INLINE int CSliderCtrl::GetThumbLength() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TBM_GETTHUMBLENGTH, 0, 0L); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CProgressCtrl::CProgressCtrl()
	{ }
_AFXCMN_INLINE COLORREF CProgressCtrl::SetBkColor(_In_ COLORREF clrNew)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_SETBKCOLOR, 0, (LPARAM) clrNew); }
_AFXCMN_INLINE void CProgressCtrl::SetRange(_In_ short nLower, _In_ short nUpper)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, PBM_SETRANGE, 0, MAKELPARAM(nLower, nUpper)); }
_AFXCMN_INLINE void CProgressCtrl::SetRange32(_In_ int nLower, _In_ int nUpper)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, PBM_SETRANGE32, (WPARAM) nLower, (LPARAM) nUpper); }
_AFXCMN_INLINE int CProgressCtrl::GetPos() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_GETPOS, 0, 0); }
_AFXCMN_INLINE int CProgressCtrl::OffsetPos(_In_ int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_DELTAPOS, nPos, 0L); }
_AFXCMN_INLINE int CProgressCtrl::SetStep(_In_ int nStep)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_SETSTEP, nStep, 0L); }
_AFXCMN_INLINE int CProgressCtrl::StepIt()
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_STEPIT, 0, 0L); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CHeaderCtrl::CHeaderCtrl()
	{ }
_AFXCMN_INLINE int CHeaderCtrl::GetItemCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, HDM_GETITEMCOUNT, 0, 0L); }
_AFXCMN_INLINE int CHeaderCtrl::InsertItem(_In_ int nPos, _In_ HDITEM* phdi)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_INSERTITEM, nPos, (LPARAM)phdi); }
_AFXCMN_INLINE BOOL CHeaderCtrl::DeleteItem(_In_ int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, HDM_DELETEITEM, nPos, 0L); }
_AFXCMN_INLINE BOOL CHeaderCtrl::GetItem(_In_ int nPos, _Inout_ HDITEM* pHeaderItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, HDM_GETITEM, nPos, (LPARAM)pHeaderItem); }
_AFXCMN_INLINE BOOL CHeaderCtrl::SetItem(_In_ int nPos, _In_ HDITEM* pHeaderItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, HDM_SETITEM, nPos, (LPARAM)pHeaderItem); }
_AFXCMN_INLINE BOOL CHeaderCtrl::Layout(_In_ HDLAYOUT* pHeaderLayout)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, HDM_LAYOUT, 0, (LPARAM)pHeaderLayout); }
_AFXCMN_INLINE int CHeaderCtrl::HitTest(_Inout_ LPHDHITTESTINFO pHeaderHitTestInfo)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, HDM_HITTEST, 0, (LPARAM)pHeaderHitTestInfo); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CTabCtrl::CTabCtrl()
	{ }
_AFXCMN_INLINE CImageList* CTabCtrl::GetImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TCM_GETIMAGELIST, 0, 0L)); }
_AFXCMN_INLINE CImageList* CTabCtrl::SetImageList(_In_ CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TCM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE int CTabCtrl::GetItemCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_GETITEMCOUNT, 0, 0L); }
_AFXCMN_INLINE BOOL CTabCtrl::GetItem(_In_ int nItem, _Out_ TCITEM* pTabCtrlItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_GETITEM, nItem, (LPARAM)pTabCtrlItem); }
_AFXCMN_INLINE BOOL CTabCtrl::SetItem(_In_ int nItem, _In_ TCITEM* pTabCtrlItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_SETITEM, nItem, (LPARAM)pTabCtrlItem); }
_AFXCMN_INLINE BOOL CTabCtrl::SetItemExtra(_In_ int nBytes)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_SETITEMEXTRA, (WPARAM) nBytes, 0); }
_AFXCMN_INLINE void CTabCtrl::SetCurFocus(_In_ int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TCM_SETCURFOCUS, (WPARAM) nItem, 0); }
_AFXCMN_INLINE LONG CTabCtrl::InsertItem(_In_ int nItem, _In_ TCITEM* pTabCtrlItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (LONG)::SendMessage(m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)pTabCtrlItem); }
_AFXCMN_INLINE LONG CTabCtrl::InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem)
	{ ASSERT(::IsWindow(m_hWnd)); return CTabCtrl::InsertItem(TCIF_TEXT, nItem, lpszItem, 0, 0); }
_AFXCMN_INLINE LONG CTabCtrl::InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage)
	{ ASSERT(::IsWindow(m_hWnd)); return CTabCtrl::InsertItem(TCIF_TEXT|TCIF_IMAGE, nItem, lpszItem, nImage, 0); }
_AFXCMN_INLINE BOOL CTabCtrl::DeleteItem(_In_ int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_DELETEITEM, nItem, 0L); }
_AFXCMN_INLINE BOOL CTabCtrl::DeleteAllItems()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_DELETEALLITEMS, 0, 0L); }
_AFXCMN_INLINE BOOL CTabCtrl::GetItemRect(_In_ int nItem, _Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_GETITEMRECT, nItem, (LPARAM)lpRect); }
_AFXCMN_INLINE int CTabCtrl::GetCurSel() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_GETCURSEL, 0, 0L); }
_AFXCMN_INLINE int CTabCtrl::SetCurSel(_In_ int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_SETCURSEL, nItem, 0L); }
_AFXCMN_INLINE int CTabCtrl::HitTest(_In_ TCHITTESTINFO* pHitTestInfo) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_HITTEST, 0, (LPARAM) pHitTestInfo); }
_AFXCMN_INLINE void CTabCtrl::AdjustRect(_In_ BOOL bLarger, _Inout_ LPRECT lpRect)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TCM_ADJUSTRECT, bLarger, (LPARAM)lpRect); }
_AFXCMN_INLINE CSize CTabCtrl::SetItemSize(_In_ CSize size)
	{ ASSERT(::IsWindow(m_hWnd)); return (CSize)DWORD(::SendMessage(m_hWnd, TCM_SETITEMSIZE, 0, MAKELPARAM(size.cx,size.cy))); }
_AFXCMN_INLINE void CTabCtrl::RemoveImage(_In_ int nImage)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TCM_REMOVEIMAGE, nImage, 0L); }
_AFXCMN_INLINE void CTabCtrl::SetPadding(_In_ CSize size)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TCM_SETPADDING, 0, MAKELPARAM(size.cx, size.cy)); }
_AFXCMN_INLINE int CTabCtrl::GetRowCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_GETROWCOUNT, 0, 0L); }
_AFXCMN_INLINE CToolTipCtrl* CTabCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TCM_GETTOOLTIPS, 0, 0L)); }
_AFXCMN_INLINE void CTabCtrl::SetToolTips(_In_ CToolTipCtrl* pWndTip)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TCM_SETTOOLTIPS, (WPARAM)pWndTip->GetSafeHwnd(), 0L); }
_AFXCMN_INLINE int CTabCtrl::GetCurFocus() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_GETCURFOCUS, 0, 0L); }
_AFXCMN_INLINE int CTabCtrl::SetMinTabWidth(_In_ int cx)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_SETMINTABWIDTH, 0, (LPARAM)cx); }
_AFXCMN_INLINE void CTabCtrl::DeselectAll(_In_ BOOL fExcludeFocus)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TCM_DESELECTALL, fExcludeFocus, 0); }
_AFXCMN_INLINE BOOL CTabCtrl::HighlightItem(_In_ int idItem, _In_ BOOL fHighlight /*= TRUE*/)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_HIGHLIGHTITEM, idItem, MAKELPARAM(fHighlight, 0)); }
_AFXCMN_INLINE DWORD CTabCtrl::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TCM_GETEXTENDEDSTYLE, 0, 0); }
_AFXCMN_INLINE DWORD CTabCtrl::SetExtendedStyle(_In_ DWORD dwNewStyle, _In_ DWORD dwExMask /*= 0*/)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TCM_SETEXTENDEDSTYLE, dwExMask, dwNewStyle); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CAnimateCtrl::CAnimateCtrl()
	{ }
_AFXCMN_INLINE BOOL CAnimateCtrl::Open(_In_z_ LPCTSTR lpszName)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_OPEN,
		(WPARAM) ::AfxFindResourceHandle(lpszName, _T("AVI")), (LPARAM)lpszName); }
_AFXCMN_INLINE BOOL CAnimateCtrl::Open(_In_ UINT nID)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_OPEN,
		(WPARAM) ::AfxFindResourceHandle(MAKEINTRESOURCE(nID), _T("AVI")), (LPARAM) nID); }
_AFXCMN_INLINE BOOL CAnimateCtrl::Play(_In_ UINT nFrom, _In_ UINT nTo, _In_ UINT nRep)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_PLAY, nRep, MAKELPARAM(nFrom, nTo)); }
_AFXCMN_INLINE BOOL CAnimateCtrl::Stop()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_STOP, 0, 0L); }
_AFXCMN_INLINE BOOL CAnimateCtrl::Close()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_OPEN, 0, 0L); }
_AFXCMN_INLINE BOOL CAnimateCtrl::Seek(_In_ UINT nTo)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_PLAY, 1, MAKELPARAM(nTo, nTo)); }
#if defined(UNICODE)
_AFXCMN_INLINE BOOL CAnimateCtrl::IsPlaying() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, ACM_ISPLAYING, 0, 0L); }
#endif

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CRichEditCtrl::CRichEditCtrl()
	{ }
_AFXCMN_INLINE BOOL CRichEditCtrl::CanUndo() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0); }
_AFXCMN_INLINE BOOL CRichEditCtrl::CanRedo() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_CANREDO, 0, 0); }
_AFXCMN_INLINE UNDONAMEID CRichEditCtrl::GetUndoName() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UNDONAMEID) ::SendMessage(m_hWnd, EM_GETUNDONAME, 0, 0); }
_AFXCMN_INLINE UNDONAMEID CRichEditCtrl::GetRedoName() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UNDONAMEID) ::SendMessage(m_hWnd, EM_GETREDONAME, 0, 0); }
_AFXCMN_INLINE int CRichEditCtrl::GetLineCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0); }
_AFXCMN_INLINE BOOL CRichEditCtrl::GetModify() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::SetModify(_In_ BOOL bModified /* = TRUE */)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0);}
_AFXCMN_INLINE BOOL CRichEditCtrl::SetTextMode(_In_ UINT fMode)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, EM_SETTEXTMODE, (WPARAM) fMode, 0); }
_AFXCMN_INLINE UINT CRichEditCtrl::GetTextMode() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, EM_GETTEXTMODE, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::GetRect(_Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect); }
_AFXCMN_INLINE CPoint CRichEditCtrl::GetCharPos(_In_ long lChar) const
	{ ASSERT(::IsWindow(m_hWnd)); CPoint pt; ::SendMessage(m_hWnd, EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar); return pt;}
_AFXCMN_INLINE UINT CRichEditCtrl::GetOptions() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, EM_GETOPTIONS, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::SetOptions(_In_ WORD wOp, _In_ DWORD dwFlags)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETOPTIONS, (WPARAM)wOp, (LPARAM)dwFlags); }
_AFXCMN_INLINE BOOL CRichEditCtrl::SetAutoURLDetect(_In_ BOOL bEnable /* = TRUE */)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, EM_AUTOURLDETECT, (WPARAM) bEnable, 0); }
_AFXCMN_INLINE void CRichEditCtrl::EmptyUndoBuffer()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0); }
_AFXCMN_INLINE UINT CRichEditCtrl::SetUndoLimit(_In_ UINT nLimit)
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, EM_SETUNDOLIMIT, (WPARAM) nLimit, 0); }
_AFXCMN_INLINE void CRichEditCtrl::ReplaceSel(_In_z_ LPCTSTR lpszNewText, _In_ BOOL bCanUndo)
	{ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText); }
_AFXCMN_INLINE void CRichEditCtrl::SetRect(_In_ LPCRECT lpRect)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect); }
_AFXCMN_INLINE void CRichEditCtrl::StopGroupTyping()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_STOPGROUPTYPING, 0, 0); }
_AFXCMN_INLINE BOOL CRichEditCtrl::Redo()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, EM_REDO, 0, 0); }
_AFXCMN_INLINE BOOL CRichEditCtrl::Undo()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::Clear()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_CLEAR, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::Copy()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_COPY, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::Cut()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_CUT, 0, 0); }
_AFXCMN_INLINE void CRichEditCtrl::Paste()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_PASTE, 0, 0); }
_AFXCMN_INLINE BOOL CRichEditCtrl::SetReadOnly(_In_ BOOL bReadOnly /* = TRUE */ )
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0L); }
_AFXCMN_INLINE int CRichEditCtrl::GetFirstVisibleLine() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L); }
_AFXCMN_INLINE BOOL CRichEditCtrl::DisplayBand(_In_ LPRECT pDisplayRect)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_DISPLAYBAND, 0, (LPARAM)pDisplayRect); }
_AFXCMN_INLINE void CRichEditCtrl::GetSel(_Out_ CHARRANGE &cr) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_EXGETSEL, 0, (LPARAM)&cr); }
_AFXCMN_INLINE BOOL CRichEditCtrl::GetPunctuation(_In_ UINT fType, _Out_ PUNCTUATION* lpPunc) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, EM_GETPUNCTUATION, (WPARAM) fType, (LPARAM) lpPunc); }
_AFXCMN_INLINE BOOL CRichEditCtrl::SetPunctuation(_In_ UINT fType, _In_ PUNCTUATION* lpPunc)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, EM_SETPUNCTUATION, (WPARAM) fType, (LPARAM) lpPunc); }
_AFXCMN_INLINE void CRichEditCtrl::LimitText(_In_ long nChars)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_EXLIMITTEXT, 0, nChars); }
_AFXCMN_INLINE long CRichEditCtrl::LineFromChar(_In_ long nIndex) const
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_EXLINEFROMCHAR, 0, nIndex); }
_AFXCMN_INLINE CPoint CRichEditCtrl::PosFromChar(_In_ UINT nChar) const
	{ ASSERT(::IsWindow(m_hWnd)); POINTL pt; ::SendMessage(m_hWnd, EM_POSFROMCHAR, (WPARAM)&pt, nChar); return CPoint(pt.x, pt.y); }
_AFXCMN_INLINE int CRichEditCtrl::CharFromPos(_In_ CPoint pt) const
	{ ASSERT(::IsWindow(m_hWnd)); POINTL ptl = {pt.x, pt.y}; return (int)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, (LPARAM)&ptl); }
_AFXCMN_INLINE void CRichEditCtrl::SetSel(_In_ CHARRANGE &cr)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_EXSETSEL, 0, (LPARAM)&cr); }
_AFXCMN_INLINE DWORD CRichEditCtrl::FindWordBreak(_In_ UINT nCode, _In_ DWORD nStart) const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)::SendMessage(m_hWnd, EM_FINDWORDBREAK, (WPARAM) nCode, (LPARAM) nStart); }

#pragma push_macro("FindTextA")
#pragma push_macro("FindTextW")
#undef FindTextA
#undef FindTextW
_AFXCMN_INLINE long CRichEditCtrl::FindText(_In_ DWORD dwFlags, _Out_ FINDTEXTEX* pFindText) const
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_FINDTEXTEX, dwFlags, (LPARAM)pFindText); }
#pragma pop_macro("FindTextA")
#pragma pop_macro("FindTextW")

_AFXCMN_INLINE long CRichEditCtrl::FormatRange(_In_ FORMATRANGE* pfr, _In_ BOOL bDisplay)
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_FORMATRANGE, (WPARAM)bDisplay, (LPARAM)pfr); }

_AFXCMN_INLINE long CRichEditCtrl::GetEventMask() const
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_GETEVENTMASK, 0, 0L); }

_AFXCMN_INLINE long CRichEditCtrl::GetLimitText() const
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0L); }

#pragma warning(push)
#pragma warning(disable: 6054)
_AFXCMN_INLINE long CRichEditCtrl::GetSelText(_Pre_notnull_ _Post_z_ LPSTR lpBuf) const
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_GETSELTEXT, 0, (LPARAM)lpBuf); }
#pragma warning(pop)

_AFXCMN_INLINE void CRichEditCtrl::HideSelection(_In_ BOOL bHide, _In_ BOOL bPerm)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_HIDESELECTION, bHide, bPerm); }

_AFXCMN_INLINE void CRichEditCtrl::RequestResize()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_REQUESTRESIZE, 0, 0L); }

_AFXCMN_INLINE WORD CRichEditCtrl::GetSelectionType() const
	{ ASSERT(::IsWindow(m_hWnd)); return (WORD)::SendMessage(m_hWnd, EM_SELECTIONTYPE, 0, 0L); }

_AFXCMN_INLINE UINT CRichEditCtrl::GetWordWrapMode() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, EM_GETWORDWRAPMODE, 0, 0); }

_AFXCMN_INLINE UINT CRichEditCtrl::SetWordWrapMode(_In_ UINT uFlags) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, EM_SETWORDWRAPMODE, (WPARAM) uFlags, 0); }

_AFXCMN_INLINE COLORREF CRichEditCtrl::SetBackgroundColor(_In_ BOOL bSysColor, _In_ COLORREF cr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF)::SendMessage(m_hWnd, EM_SETBKGNDCOLOR, bSysColor, cr); }

_AFXCMN_INLINE DWORD CRichEditCtrl::SetEventMask(_In_ DWORD dwEventMask)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)::SendMessage(m_hWnd, EM_SETEVENTMASK, 0, dwEventMask); }

_AFXCMN_INLINE BOOL CRichEditCtrl::SetOLECallback(_In_ IRichEditOleCallback* pCallback)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETOLECALLBACK, 0, (LPARAM)pCallback); }

_AFXCMN_INLINE BOOL CRichEditCtrl::SetTargetDevice(_In_ HDC hDC, _In_ long lLineWidth)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETTARGETDEVICE, (WPARAM)hDC, lLineWidth); }

_AFXCMN_INLINE BOOL CRichEditCtrl::SetTargetDevice(_In_ CDC &dc, _In_ long lLineWidth)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETTARGETDEVICE, (WPARAM)dc.m_hDC, lLineWidth); }

_AFXCMN_INLINE long CRichEditCtrl::StreamIn(_In_ int nFormat, EDITSTREAM &es)
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_STREAMIN, nFormat, (LPARAM)&es); }

_AFXCMN_INLINE long CRichEditCtrl::StreamOut(_In_ int nFormat, EDITSTREAM &es)
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, EM_STREAMOUT, nFormat, (LPARAM)&es); }

_AFXCMN_INLINE long CRichEditCtrl::GetTextLength() const
	{ ASSERT(::IsWindow(m_hWnd)); return (long)::SendMessage(m_hWnd, WM_GETTEXTLENGTH, NULL, NULL); }

/////////////////////////////////////////////////////////////////////////////

#pragma warning(pop)

#endif //_AFXCMN_INLINE

/////////////////////////////////////////////////////////////////////////////
