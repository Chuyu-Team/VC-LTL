// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXCVIEW.H

#ifdef _AFXCVIEW_INLINE

#pragma once

// CListView
_AFXCVIEW_INLINE CListView::CListView() : CCtrlView(WC_LISTVIEW,
	AFX_WS_DEFAULT_VIEW)
	{ }
// NOTE: The cast in GetListCtrl is ugly, but must be preserved for compatibility.
// CListCtrl is not related to CListView by inheritance so we must be careful to ensure 
// that CListCtrl remains a binary compatible subset of CListView.
_AFXCVIEW_INLINE CListCtrl& CListView::GetListCtrl() const
	{ return *(CListCtrl*)this; }
_AFXCVIEW_INLINE CTreeView::CTreeView() : CCtrlView(WC_TREEVIEW,
	AFX_WS_DEFAULT_VIEW)
	{ }
// NOTE: The cast in GetTreeCtrl is ugly, but must be preserved for compatibility.
// CTreeCtrl is not related to CTreeView by inheritance so we must be careful to ensure 
// that CTreeCtrl remains a binary compatible subset of CTreeView.
_AFXCVIEW_INLINE CTreeCtrl& CTreeView::GetTreeCtrl() const
	{ return *(CTreeCtrl*)this; }

#endif //_AFXCVIEW_INLINE

/////////////////////////////////////////////////////////////////////////////
