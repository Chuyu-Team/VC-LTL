// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXCMN.H (via WINCTRL6.CPP)

#pragma once

#ifdef _AFXCMN_INLINE

#pragma warning(push)
#pragma warning(disable: 6001)

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE BOOL CStatusBarCtrl::IsSimple() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, SB_ISSIMPLE, 0, 0); }
_AFXCMN_INLINE void CStatusBarCtrl::SetTipText(_In_ int nPane, _In_z_ LPCTSTR pszTipText)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, SB_SETTIPTEXT, nPane, (LPARAM)pszTipText); }
_AFXCMN_INLINE COLORREF CStatusBarCtrl::SetBkColor(_In_ COLORREF cr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, SB_SETBKCOLOR, 0, (LPARAM)cr); }
_AFXCMN_INLINE BOOL CStatusBarCtrl::SetIcon(_In_ int iPart, _In_ HICON hIcon)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, SB_SETICON, iPart, (LPARAM)hIcon); }
_AFXCMN_INLINE HICON CStatusBarCtrl::GetIcon(_In_ int iPart) const
	{ ASSERT(::IsWindow(m_hWnd)); return (HICON) ::SendMessage(m_hWnd, SB_GETICON, iPart, 0L); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE void CSpinButtonCtrl::SetRange32(_In_ int nLower, _In_ int nUpper)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, UDM_SETRANGE32, (WPARAM) nLower, (LPARAM) nUpper); }
_AFXCMN_INLINE void CSpinButtonCtrl::GetRange32(_Out_ int& nLower, _Out_ int& nUpper) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, UDM_GETRANGE32, (WPARAM) &nLower, (LPARAM) &nUpper); }

_AFXCMN_INLINE int CSpinButtonCtrl::GetPos32(_Out_opt_ LPBOOL lpbError /*= NULL*/) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, UDM_GETPOS32, 0, (LPARAM)lpbError); }
_AFXCMN_INLINE int CSpinButtonCtrl::SetPos32(_In_ int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, UDM_SETPOS32, 0, (LPARAM)nPos); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE DWORD CToolBarCtrl::GetButtonSize() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_GETBUTTONSIZE, 0, 0L); }
_AFXCMN_INLINE CImageList* CToolBarCtrl::GetDisabledImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0)); }
_AFXCMN_INLINE CImageList* CToolBarCtrl::GetHotImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_GETHOTIMAGELIST, 0, 0)); }
_AFXCMN_INLINE CImageList* CToolBarCtrl::GetImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_GETIMAGELIST, 0, 0)); }
_AFXCMN_INLINE DWORD CToolBarCtrl::GetStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_GETSTYLE, 0, 0L); }
_AFXCMN_INLINE INT CToolBarCtrl::GetMaxTextRows() const
	{ ASSERT(::IsWindow(m_hWnd)); return (INT) ::SendMessage(m_hWnd, TB_GETTEXTROWS, 0, 0L); }
_AFXCMN_INLINE BOOL CToolBarCtrl::GetRect(_In_ int nID, _Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETRECT, nID, (LPARAM)lpRect); }
_AFXCMN_INLINE BOOL CToolBarCtrl::IsButtonHighlighted(_In_ int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONHIGHLIGHTED, nID, 0); }
_AFXCMN_INLINE void CToolBarCtrl::LoadImages(_In_ int iBitmapID, _In_ HINSTANCE hinst)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_LOADIMAGES, iBitmapID, (LPARAM)hinst); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetButtonWidth(_In_ int cxMin, _In_ int cxMax)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETBUTTONWIDTH, 0, MAKELPARAM(cxMin, cxMax)); }
_AFXCMN_INLINE CImageList* CToolBarCtrl::SetDisabledImageList(_In_ CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE CImageList* CToolBarCtrl::SetHotImageList(_In_ CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_SETHOTIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE CImageList* CToolBarCtrl::SetImageList(_In_ CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetIndent(_In_ int iIndent)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETINDENT, iIndent, 0L); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetMaxTextRows(_In_ int iMaxRows)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETMAXTEXTROWS, iMaxRows, 0L); }
_AFXCMN_INLINE void CToolBarCtrl::SetStyle(_In_ DWORD dwStyle)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETSTYLE, 0, dwStyle); }
_AFXCMN_INLINE int CToolBarCtrl::GetButtonInfo(_In_ int nID, _Out_ TBBUTTONINFO* ptbbi) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETBUTTONINFO, nID, (LPARAM)ptbbi); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetButtonInfo(_In_ int nID, _In_ TBBUTTONINFO* ptbbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETBUTTONINFO, nID, (LPARAM)ptbbi); }
_AFXCMN_INLINE DWORD CToolBarCtrl::SetDrawTextFlags(_In_ DWORD dwMask, _In_ DWORD dwDTFlags)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_SETDRAWTEXTFLAGS, dwMask, dwDTFlags); }
_AFXCMN_INLINE BOOL CToolBarCtrl::GetAnchorHighlight() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETANCHORHIGHLIGHT, 0, 0); }
_AFXCMN_INLINE BOOL CToolBarCtrl::SetAnchorHighlight(_In_ BOOL fAnchor)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETANCHORHIGHLIGHT, fAnchor, 0); }
_AFXCMN_INLINE int CToolBarCtrl::GetHotItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETHOTITEM, 0, 0); }
_AFXCMN_INLINE int CToolBarCtrl::SetHotItem(_In_ int nHot)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_SETHOTITEM, nHot, 0); }
_AFXCMN_INLINE void CToolBarCtrl::GetInsertMark(_Out_ TBINSERTMARK* ptbim) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_GETINSERTMARK, 0, (LPARAM)ptbim); }
_AFXCMN_INLINE void CToolBarCtrl::SetInsertMark(_In_ TBINSERTMARK* ptbim)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETINSERTMARK, 0, (LPARAM)ptbim); }
_AFXCMN_INLINE BOOL CToolBarCtrl::GetMaxSize(_Out_ LPSIZE pSize) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)pSize); }
_AFXCMN_INLINE BOOL CToolBarCtrl::InsertMarkHitTest(_In_ LPPOINT ppt, _In_ LPTBINSERTMARK ptbim) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_INSERTMARKHITTEST, (WPARAM)ppt, (LPARAM)ptbim); }
_AFXCMN_INLINE BOOL CToolBarCtrl::MapAccelerator(_In_ TCHAR chAccel, _In_ UINT* pIDBtn)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_MAPACCELERATOR, (WPARAM)chAccel, (LPARAM)pIDBtn); }
_AFXCMN_INLINE BOOL CToolBarCtrl::MarkButton(_In_ int nID, _In_ BOOL bHighlight)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_MARKBUTTON, nID, MAKELPARAM(bHighlight, 0)); }
_AFXCMN_INLINE BOOL CToolBarCtrl::MoveButton(_In_ UINT nOldPos, _In_ UINT nNewPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_MOVEBUTTON, nOldPos, nNewPos); }

#pragma warning(push)
#pragma warning(disable: 6054)
_AFXCMN_INLINE int CToolBarCtrl::GetString(_In_ int nString, _Out_writes_to_(cchMaxLen, return + 1) LPTSTR lpstrString, _In_ size_t cchMaxLen) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETSTRING, MAKEWPARAM(cchMaxLen, nString), (LPARAM)lpstrString); lpstrString[cchMaxLen]=_T('\0'); }
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE DWORD CListCtrl::SetExtendedStyle(_In_ DWORD dwNewStyle)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM) dwNewStyle); }
_AFXCMN_INLINE HCURSOR CListCtrl::SetHotCursor(_In_ HCURSOR hc)
	{ ASSERT(::IsWindow(m_hWnd) && hc != NULL); return (HCURSOR) ::SendMessage(m_hWnd, LVM_SETHOTCURSOR, 0, (LPARAM) hc); }
_AFXCMN_INLINE int CListCtrl::SetHotItem(_In_ int iIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_SETHOTITEM, (WPARAM) iIndex, 0); }
_AFXCMN_INLINE void CListCtrl::SetWorkAreas(_In_ int nWorkAreas, _In_ LPRECT lpRect)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LVM_SETWORKAREAS, nWorkAreas, (LPARAM) lpRect); }
_AFXCMN_INLINE int CListCtrl::SubItemHitTest(_In_ LPLVHITTESTINFO pInfo)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_SUBITEMHITTEST, 0, (LPARAM) pInfo); }
_AFXCMN_INLINE HCURSOR CListCtrl::GetHotCursor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HCURSOR) ::SendMessage(m_hWnd, LVM_GETHOTCURSOR, 0, 0); }
_AFXCMN_INLINE int CListCtrl::GetHotItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETHOTITEM, 0, 0); }
_AFXCMN_INLINE DWORD CListCtrl::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0); }
_AFXCMN_INLINE CSize CListCtrl::ApproximateViewRect(_In_ CSize sz, _In_ int iCount) const
	{ ASSERT(::IsWindow(m_hWnd)); return CSize((DWORD) ::SendMessage(m_hWnd, LVM_APPROXIMATEVIEWRECT, iCount, MAKELPARAM(sz.cx, sz.cy))); }
_AFXCMN_INLINE BOOL CListCtrl::GetBkImage(_Out_ LVBKIMAGE* plvbkImage) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETBKIMAGE, 0, (LPARAM)plvbkImage); }
_AFXCMN_INLINE DWORD CListCtrl::GetHoverTime() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, LVM_GETHOVERTIME, 0, 0); }
_AFXCMN_INLINE void CListCtrl::GetWorkAreas(_In_ int nWorkAreas, _Out_ LPRECT prc) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LVM_GETWORKAREAS, nWorkAreas, (LPARAM)prc); }
_AFXCMN_INLINE BOOL CListCtrl::SetBkImage(_In_ LVBKIMAGE* plvbkImage)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, LVM_SETBKIMAGE, 0, (LPARAM)plvbkImage); }
_AFXCMN_INLINE DWORD CListCtrl::SetHoverTime(_In_ DWORD dwHoverTime)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)::SendMessage(m_hWnd, LVM_SETHOVERTIME, 0, dwHoverTime); }
_AFXCMN_INLINE UINT CListCtrl::GetNumberOfWorkAreas() const
	{ ASSERT(::IsWindow(m_hWnd)); UINT nWorkAreas; ::SendMessage(m_hWnd, LVM_GETNUMBEROFWORKAREAS, 0, (LPARAM)&nWorkAreas); return nWorkAreas; }
_AFXCMN_INLINE int CListCtrl::SetSelectionMark(_In_ int iIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_SETSELECTIONMARK, 0, (LPARAM) iIndex); }
_AFXCMN_INLINE int CListCtrl::GetSelectionMark() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETSELECTIONMARK, 0, 0); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE BOOL CHeaderCtrl::GetItemRect(_In_ int nIndex, _Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT(lpRect != NULL); return (BOOL)::SendMessage(m_hWnd, HDM_GETITEMRECT, nIndex, (LPARAM)lpRect); }
_AFXCMN_INLINE int CHeaderCtrl::OrderToIndex(_In_ int nOrder) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_ORDERTOINDEX, nOrder, 0); }
_AFXCMN_INLINE int CHeaderCtrl::SetHotDivider(_In_ CPoint pt)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_SETHOTDIVIDER, TRUE, MAKELPARAM(pt.x, pt.y)); }
_AFXCMN_INLINE int CHeaderCtrl::SetHotDivider(_In_ int nIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_SETHOTDIVIDER, FALSE, nIndex); }
_AFXCMN_INLINE CImageList* CHeaderCtrl::GetImageList(_In_ int nImageList /* = HDSIL_NORMAL */) const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, HDM_GETIMAGELIST, nImageList, 0L)); }
_AFXCMN_INLINE CImageList* CHeaderCtrl::SetImageList(_In_ CImageList* pImageList, _In_ int nImageList /* = HDSIL_NORMAL */)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, HDM_SETIMAGELIST, nImageList, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE CImageList* CHeaderCtrl::CreateDragImage(_In_ int nIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, HDM_CREATEDRAGIMAGE, nIndex, 0L)); }

_AFXCMN_INLINE int CHeaderCtrl::GetBitmapMargin() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_GETBITMAPMARGIN, 0, 0L); }
_AFXCMN_INLINE int CHeaderCtrl::SetBitmapMargin(_In_ int nWidth)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_SETBITMAPMARGIN, nWidth, 0L); }
_AFXCMN_INLINE int CHeaderCtrl::SetFilterChangeTimeout(_In_ DWORD dwTimeOut)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_SETFILTERCHANGETIMEOUT, 0, dwTimeOut); }
_AFXCMN_INLINE int CHeaderCtrl::EditFilter(_In_ int nColumn, _In_ BOOL bDiscardChanges)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, HDM_EDITFILTER, nColumn, MAKELPARAM(bDiscardChanges, 0)); }
_AFXCMN_INLINE BOOL CHeaderCtrl::ClearFilter(_In_ int nColumn)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, HDM_CLEARFILTER, nColumn, 0L); }
_AFXCMN_INLINE BOOL CHeaderCtrl::ClearAllFilters()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, HDM_CLEARFILTER, (WPARAM)-1, 0L); }

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXCMN_INLINE BOOL CHeaderCtrl::GetItemDropDownRect(_In_ int iItem, _Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return Header_GetItemDropDownRect(m_hWnd, iItem, lpRect); } // HDM_GETITEMDROPDOWNRECT
_AFXCMN_INLINE BOOL CHeaderCtrl::GetOverflowRect(_Out_ LPRECT lpRect) const
	{ ASSERT(::IsWindow(m_hWnd)); return Header_GetOverflowRect(m_hWnd, lpRect); } // HDM_GETOVERFLOWRECT
_AFXCMN_INLINE int CHeaderCtrl::GetFocusedItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return Header_GetFocusedItem(m_hWnd); } // HDM_GETFOCUSEDITEM
_AFXCMN_INLINE BOOL CHeaderCtrl::SetFocusedItem(_In_ int iItem)
	{ ASSERT(::IsWindow(m_hWnd)); return Header_SetFocusedItem(m_hWnd, iItem); } // HDM_SETFOCUSEDITEM
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CReBarCtrl::CReBarCtrl()
	{ }
_AFXCMN_INLINE UINT CReBarCtrl::GetBandCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETBANDCOUNT, 0, 0L); }
_AFXCMN_INLINE BOOL CReBarCtrl::GetBandInfo(_In_ UINT uBand, _Out_ REBARBANDINFO* prbbi) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_GETBANDINFO, uBand, (LPARAM)prbbi); }
_AFXCMN_INLINE UINT CReBarCtrl::GetBarHeight() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETBARHEIGHT, 0, 0L); }
_AFXCMN_INLINE BOOL CReBarCtrl::GetBarInfo(_Out_ REBARINFO* prbi) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_GETBARINFO, 0, (LPARAM)prbi); }
_AFXCMN_INLINE COLORREF CReBarCtrl::GetBkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_GETBKCOLOR, 0, 0L); }
_AFXCMN_INLINE IDropTarget* CReBarCtrl::GetDropTarget() const
	{ ASSERT(::IsWindow(m_hWnd)); IDropTarget* pdt; ::SendMessage(m_hWnd, RB_GETDROPTARGET, 0, (LPARAM)&pdt); return pdt; }
_AFXCMN_INLINE BOOL CReBarCtrl::GetRect(_In_ UINT uBand, _Out_ LPRECT prc) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_GETRECT, uBand, (LPARAM)prc); }
_AFXCMN_INLINE UINT CReBarCtrl::GetRowCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETROWCOUNT, 0, 0L); }
_AFXCMN_INLINE UINT CReBarCtrl::GetRowHeight(_In_ UINT uRow) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETROWHEIGHT, uRow, 0L); }
_AFXCMN_INLINE COLORREF CReBarCtrl::GetTextColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_GETTEXTCOLOR, 0, 0L); }
_AFXCMN_INLINE CToolTipCtrl* CReBarCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, RB_GETTOOLTIPS, 0, 0L)); }
_AFXCMN_INLINE int CReBarCtrl::IDToIndex(_In_ UINT uBandID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, RB_IDTOINDEX, uBandID, 0L); }
_AFXCMN_INLINE BOOL CReBarCtrl::SetBandInfo(_In_ UINT uBand, _In_ REBARBANDINFO* prbbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SETBANDINFO, uBand, (LPARAM)prbbi); }
_AFXCMN_INLINE BOOL CReBarCtrl::SetBarInfo(_In_ REBARINFO* prbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SETBARINFO, 0, (LPARAM)prbi); }
_AFXCMN_INLINE COLORREF CReBarCtrl::SetBkColor(_In_ COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_SETBKCOLOR, 0, (LPARAM)clr); }
_AFXCMN_INLINE CWnd* CReBarCtrl::SetOwner(_In_ CWnd* pWnd)
	{ ASSERT(::IsWindow(m_hWnd)); CWnd::SetOwner(pWnd);  return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, RB_SETPARENT, (WPARAM)pWnd->m_hWnd, 0L)); }
_AFXCMN_INLINE COLORREF CReBarCtrl::SetTextColor(_In_ COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_SETTEXTCOLOR, 0, (LPARAM)clr); }
_AFXCMN_INLINE void CReBarCtrl::SetToolTips(_In_ CToolTipCtrl* pTip)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_SETTOOLTIPS, (WPARAM)pTip->m_hWnd, 0L); }
_AFXCMN_INLINE void CReBarCtrl::BeginDrag(_In_ UINT uBand, _In_ DWORD dwPos)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_BEGINDRAG, uBand, dwPos); }
_AFXCMN_INLINE BOOL CReBarCtrl::DeleteBand(_In_ UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_DELETEBAND, uBand, 0L); }
_AFXCMN_INLINE void CReBarCtrl::DragMove(_In_ DWORD dwPos)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_DRAGMOVE, 0, dwPos); }
_AFXCMN_INLINE void CReBarCtrl::EndDrag()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_ENDDRAG, 0, 0L); }
_AFXCMN_INLINE int CReBarCtrl::HitTest(_In_ RBHITTESTINFO* prbht)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, RB_HITTEST, 0, (LPARAM)prbht); }
_AFXCMN_INLINE BOOL CReBarCtrl::InsertBand(_In_ UINT uIndex, _In_ REBARBANDINFO* prbbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_INSERTBAND, uIndex, (LPARAM)prbbi); }
_AFXCMN_INLINE void CReBarCtrl::MaximizeBand(_In_ UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_MAXIMIZEBAND, uBand, 0L); }
_AFXCMN_INLINE void CReBarCtrl::MinimizeBand(_In_ UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_MINIMIZEBAND, uBand, 0L); }
_AFXCMN_INLINE void CReBarCtrl::RestoreBand(_In_ UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_MAXIMIZEBAND, uBand, 1L); }
_AFXCMN_INLINE BOOL CReBarCtrl::SizeToRect(_Out_ CRect& rect)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SIZETORECT, 0, (LPARAM)&rect); }
_AFXCMN_INLINE BOOL CReBarCtrl::ShowBand(_In_ UINT uBand, _In_ BOOL fShow /*= TRUE*/)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SHOWBAND, uBand, fShow); }
_AFXCMN_INLINE void CReBarCtrl::GetBandBorders(_In_ UINT uBand, _Out_ LPRECT prc) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_GETBANDBORDERS, uBand, (LPARAM)prc); }
_AFXCMN_INLINE CPalette* CReBarCtrl::GetPalette() const
	{ ASSERT(::IsWindow(m_hWnd)); return CPalette::FromHandle((HPALETTE)::SendMessage(m_hWnd, RB_GETPALETTE, 0, 0L)); }
_AFXCMN_INLINE CPalette* CReBarCtrl::SetPalette(_In_ HPALETTE hPal)
	{ ASSERT(::IsWindow(m_hWnd)); return CPalette::FromHandle((HPALETTE)::SendMessage(m_hWnd, RB_SETPALETTE, 0, (LPARAM)hPal)); }
_AFXCMN_INLINE BOOL CReBarCtrl::MoveBand(_In_ UINT uFrom, _In_ UINT uTo)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_MOVEBAND, uFrom, uTo); }

_AFXCMN_INLINE void CReBarCtrl::PushChevron(_In_ UINT uBand, _In_ LPARAM lAppValue)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_PUSHCHEVRON, uBand, lAppValue); }

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXCMN_INLINE BOOL CReBarCtrl::SetBandWidth(_In_ UINT uBand, _In_ int cxWidth)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SETBANDWIDTH, (WPARAM)uBand, (LPARAM)cxWidth); }
_AFXCMN_INLINE DWORD CReBarCtrl::SetExtendedStyle(_In_ DWORD dwMask, _In_ DWORD dwStyleEx)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, RB_SETEXTENDEDSTYLE, dwMask, dwStyleEx); }
_AFXCMN_INLINE DWORD CReBarCtrl::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, RB_GETEXTENDEDSTYLE, 0, 0L); }
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE void CToolTipCtrl::SetDelayTime(_In_ UINT nDelay)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_SETDELAYTIME, 0, nDelay); }
_AFXCMN_INLINE int CToolTipCtrl::GetDelayTime(_In_ DWORD dwDuration) const
	{ ASSERT(::IsWindow(m_hWnd));  return (int) ::SendMessage(m_hWnd, TTM_GETDELAYTIME, dwDuration, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::GetMargin(_Out_ LPRECT lprc) const
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_GETMARGIN, 0, (LPARAM)lprc); }
_AFXCMN_INLINE int CToolTipCtrl::GetMaxTipWidth() const
	{ ASSERT(::IsWindow(m_hWnd));  return (int) ::SendMessage(m_hWnd, TTM_GETMAXTIPWIDTH, 0, 0L); }
_AFXCMN_INLINE COLORREF CToolTipCtrl::GetTipBkColor() const
	{ ASSERT(::IsWindow(m_hWnd));  return (COLORREF) ::SendMessage(m_hWnd, TTM_GETTIPBKCOLOR, 0, 0L); }
_AFXCMN_INLINE COLORREF CToolTipCtrl::GetTipTextColor() const
	{ ASSERT(::IsWindow(m_hWnd));  return (COLORREF) ::SendMessage(m_hWnd, TTM_GETTIPTEXTCOLOR, 0, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::Pop()
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_POP, 0, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::SetDelayTime(_In_ DWORD dwDuration, _In_ int iTime)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_SETDELAYTIME, dwDuration, MAKELPARAM(iTime, 0)); }
_AFXCMN_INLINE void CToolTipCtrl::SetMargin(_In_ LPRECT lprc)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_SETMARGIN, 0, (LPARAM)lprc); }
_AFXCMN_INLINE int CToolTipCtrl::SetMaxTipWidth(_In_ int iWidth)
	{ ASSERT(::IsWindow(m_hWnd));  return (int) ::SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, iWidth); }
_AFXCMN_INLINE void CToolTipCtrl::SetTipBkColor(_In_ COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_SETTIPBKCOLOR, clr, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::SetTipTextColor(_In_ COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_SETTIPTEXTCOLOR, clr, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::Update()
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_UPDATE, 0, 0L); }
_AFXCMN_INLINE BOOL CToolTipCtrl::GetCurrentTool(_Out_ LPTOOLINFO lpToolInfo) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TTM_GETCURRENTTOOL, 0, (LPARAM)lpToolInfo); }

_AFXCMN_INLINE CSize CToolTipCtrl::GetBubbleSize(_In_ LPTOOLINFO lpToolInfo) const
	{ ASSERT(::IsWindow(m_hWnd));  return CSize((DWORD)::SendMessage(m_hWnd, TTM_GETBUBBLESIZE, 0, (LPARAM)lpToolInfo)); }
_AFXCMN_INLINE BOOL CToolTipCtrl::AdjustRect(_Out_ LPRECT lprc, _In_ BOOL bLarger /*= TRUE*/)
	{ ASSERT(::IsWindow(m_hWnd));  return (BOOL)::SendMessage(m_hWnd, TTM_ADJUSTRECT, bLarger, (LPARAM)lprc); }
_AFXCMN_INLINE BOOL CToolTipCtrl::SetTitle(_In_ UINT uIcon, _In_z_ LPCTSTR lpstrTitle)
	{ ASSERT(::IsWindow(m_hWnd));  return (BOOL)::SendMessage(m_hWnd, TTM_SETTITLE, uIcon, (LPARAM)lpstrTitle); }

#if defined(UNICODE)
_AFXCMN_INLINE void CToolTipCtrl::Popup()
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_POPUP, 0, 0L); }
_AFXCMN_INLINE void CToolTipCtrl::GetTitle(_Out_ PTTGETTITLE pTTGetTitle) const
	{ ASSERT(::IsWindow(m_hWnd));  ::SendMessage(m_hWnd, TTM_GETTITLE, 0, (LPARAM)pTTGetTitle); }
#endif
/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CComboBoxEx::CComboBoxEx()
	{ }
_AFXCMN_INLINE DWORD CComboBoxEx::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, CBEM_GETEXTENDEDSTYLE, 0, 0); }
_AFXCMN_INLINE DWORD CComboBoxEx::SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, CBEM_SETEXTENDEDSTYLE, (DWORD) dwExMask, (LPARAM) dwExStyles); }
_AFXCMN_INLINE BOOL CComboBoxEx::HasEditChanged()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, CBEM_HASEDITCHANGED, 0, 0); }
_AFXCMN_INLINE CEdit* CComboBoxEx::GetEditCtrl() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CEdit*) CEdit::FromHandle((HWND) ::SendMessage(m_hWnd, CBEM_GETEDITCONTROL, 0, 0)); }
_AFXCMN_INLINE CComboBox* CComboBoxEx::GetComboBoxCtrl() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CComboBox*) CComboBox::FromHandle((HWND) ::SendMessage(m_hWnd, CBEM_GETCOMBOCONTROL, 0, 0)); }
_AFXCMN_INLINE CImageList* CComboBoxEx::SetImageList(_In_ CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, CBEM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
_AFXCMN_INLINE CImageList* CComboBoxEx::GetImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, CBEM_GETIMAGELIST, 0, 0)); }

// While CComboBoxEx derives from CComboBox, there are some
// CB_messages the underlying ComboBoxEx control doesn't support.

_AFXCMN_INLINE int CComboBoxEx::Dir(_In_ UINT attr, _In_z_ LPCTSTR lpszWildCard)
	{ UNUSED_ALWAYS(attr); UNUSED_ALWAYS(lpszWildCard);
		ASSERT(FALSE); return CB_ERR; }
_AFXCMN_INLINE int CComboBoxEx::FindString(_In_ int nIndexStart, _In_z_ LPCTSTR lpszFind) const
	{ UNUSED_ALWAYS(nIndexStart); UNUSED_ALWAYS(lpszFind);
		ASSERT(FALSE); return CB_ERR; }
_AFXCMN_INLINE int CComboBoxEx::AddString(_In_z_ LPCTSTR lpszString)
	{ UNUSED_ALWAYS(lpszString); ASSERT(FALSE); return CB_ERR;}
_AFXCMN_INLINE BOOL CComboBoxEx::SetEditSel(_In_ int nStartChar, _In_ int nEndChar)
	{ UNUSED_ALWAYS(nStartChar); UNUSED_ALWAYS(nEndChar);
		ASSERT(FALSE); return FALSE; }
_AFXCMN_INLINE int CComboBoxEx::InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString)
	{ UNUSED_ALWAYS(nIndex); UNUSED_ALWAYS(lpszString);
		ASSERT(FALSE); return CB_ERR; }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE int CProgressCtrl::SetPos(_In_ int nPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_SETPOS, nPos, 0L); }

_AFXCMN_INLINE COLORREF CProgressCtrl::SetBarColor(_In_ COLORREF clrBar)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_SETBARCOLOR, 0, clrBar); }

#if defined(UNICODE)
_AFXCMN_INLINE BOOL CProgressCtrl::SetMarquee(_In_ BOOL fMarqueeMode, _In_ int nInterval)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, PBM_SETMARQUEE, (WPARAM)fMarqueeMode, (LPARAM)nInterval); }
#endif	// defined(UNICODE)

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXCMN_INLINE int CProgressCtrl::GetStep() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_GETSTEP, 0, 0L); }
_AFXCMN_INLINE COLORREF CProgressCtrl::GetBkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_GETBKCOLOR, 0, 0L); }
_AFXCMN_INLINE COLORREF CProgressCtrl::GetBarColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_GETBARCOLOR, 0, 0L); }
_AFXCMN_INLINE int CProgressCtrl::SetState(_In_ int iState)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_SETSTATE, (WPARAM)iState, 0L); }
_AFXCMN_INLINE int CProgressCtrl::GetState() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_GETSTATE, 0, 0L); }
#endif //(NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE CIPAddressCtrl::CIPAddressCtrl()
	{ }
_AFXCMN_INLINE void CIPAddressCtrl::ClearAddress()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, IPM_CLEARADDRESS, 0, 0L); }
_AFXCMN_INLINE BOOL CIPAddressCtrl::IsBlank() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, IPM_ISBLANK, 0, 0L); }
_AFXCMN_INLINE int CIPAddressCtrl::GetAddress(_Out_ DWORD& dwAddress) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM) &dwAddress); }
_AFXCMN_INLINE void CIPAddressCtrl::SetAddress(_In_ DWORD dwAddress)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, IPM_SETADDRESS, 0, (LPARAM) dwAddress); }
_AFXCMN_INLINE void CIPAddressCtrl::SetAddress(_In_ BYTE nField0, _In_ BYTE nField1, _In_ BYTE nField2, _In_ BYTE nField3)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, IPM_SETADDRESS, 0, (LPARAM) MAKEIPADDRESS(nField0, nField1, nField2, nField3)); }
_AFXCMN_INLINE void CIPAddressCtrl::SetFieldFocus(_In_ WORD nField)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, IPM_SETFOCUS, (WPARAM) nField, 0); }
_AFXCMN_INLINE void CIPAddressCtrl::SetFieldRange(_In_ int nField, _In_ BYTE nLower, _In_ BYTE nUpper)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, IPM_SETRANGE, (WPARAM) nField, MAKEIPRANGE(nLower, nUpper)); }

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE BOOL CImageList::SetImageCount(UINT uNewCount)
	{ ASSERT(m_hImageList != NULL); return ImageList_SetImageCount(m_hImageList, uNewCount); }
_AFXCMN_INLINE BOOL CImageList::Copy(int iDst, int iSrc, UINT uFlags /* = ILCF_MOVE */)
	{ ASSERT(m_hImageList != NULL); return ImageList_Copy(m_hImageList, iDst, m_hImageList, iSrc, uFlags); }
_AFXCMN_INLINE BOOL CImageList::Copy(int iDst, CImageList* pSrc, int iSrc, UINT uFlags /* = ILCF_MOVE */)
	{ ASSERT(m_hImageList != NULL); ASSERT(pSrc != NULL && (HIMAGELIST)*pSrc != NULL); return ImageList_Copy(m_hImageList, iDst, *pSrc, iSrc, uFlags); }

/////////////////////////////////////////////////////////////////////////////

#pragma warning(pop)

#endif //_AFXCMN_INLINE

/////////////////////////////////////////////////////////////////////////////
