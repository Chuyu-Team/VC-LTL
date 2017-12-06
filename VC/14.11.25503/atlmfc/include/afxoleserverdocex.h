// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// COleServerDocEx document

class COleServerDocEx : public COleServerDoc
{
protected:
	COleServerDocEx();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COleServerDocEx)

// Overrides
public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

protected:
	virtual BOOL OnNewDocument();
	virtual COleServerItem* OnGetEmbeddedItem();
	virtual void OnResizeBorder( LPCRECT lpRectBorder, LPOLEINPLACEUIWINDOW lpUIWindow, BOOL bFrame );
	virtual void OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);
	virtual void OnDocWindowActivate( BOOL bActivate );

// Implementation
public:
	virtual ~COleServerDocEx();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
