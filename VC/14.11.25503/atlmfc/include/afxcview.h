// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXCVIEW_H__
#define __AFXCVIEW_H__

#pragma once

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

#ifndef __AFXCMN_H__
	#include <afxcmn.h>
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif 

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////
// AFXRICH - MFC RichEdit classes

// Classes declared in this file

//CObject
	//CCmdTarget;
		//CWnd
			//CView
				//CCtrlView
					class CListView;// list control view
					class CTreeView;// tree control view

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/*============================================================================*/
// CListView

class CListView : public CCtrlView
{
	DECLARE_DYNCREATE(CListView)

// Construction
public:
	CListView();

// Attributes
public:
	CListCtrl& GetListCtrl() const;

// Overridables
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	void RemoveImageList(int nImageList);
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnNcDestroy();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CTreeView

class CTreeView : public CCtrlView
{
	DECLARE_DYNCREATE(CTreeView)

// Construction
public:
	CTreeView();

// Attributes
public:
	CTreeCtrl& GetTreeCtrl() const;

protected:
	void RemoveImageList(int nImageList);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// Inline function declarations

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXCVIEW_INLINE AFX_INLINE
#include <afxcview.inl>
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif //__AFXCVIEW_H__

/////////////////////////////////////////////////////////////////////////////
