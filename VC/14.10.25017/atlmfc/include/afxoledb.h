// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXOLEDB_H__
#define __AFXOLEDB_H__

//#ifdef _AFX_NO_DAO_SUPPORT
//	#error OLE DB classes not supported in this library variant.
//#endif

#ifndef __AFXEXT_H__
	#include <afxext.h>
#endif

#include <atldbcli.h>

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif 

/////////////////////////////////////////////////////////////////////////////
// Win32 libraries

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/////////////////////////////////////////////////////////////////////////////

//CObject
	//CCmdTarget;
		//CWnd
			//CView
				//CScrollView
					//CFormView
						class COleDBRecordView; // view records with a form

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA AFX_DB_DATA

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, DB_NUMERIC& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, DBDATE& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, DBTIME& value);
void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, DBTIMESTAMP& value);

void AFXAPI DDX_DateTimeCtrl(CDataExchange* pDX, int nIDC, DBDATE& value);
void AFXAPI DDX_DateTimeCtrl(CDataExchange* pDX, int nIDC, DBTIME& value);
void AFXAPI DDX_DateTimeCtrl(CDataExchange* pDX, int nIDC, DBTIMESTAMP& value);
void AFXAPI DDX_MonthCalCtrl(CDataExchange* pDX, int nIDC, DBDATE& value);

/*============================================================================*/
// COleDBRecordView - form for viewing data records

class COleDBRecordView : public CFormView
{
	DECLARE_DYNAMIC(COleDBRecordView)

// Construction
protected:  // must derive your own class
	COleDBRecordView(LPCTSTR lpszTemplateName)
		: CFormView(lpszTemplateName)
	{
		m_bOnFirstRecord = TRUE;
		m_bOnLastRecord = FALSE;
	}
	COleDBRecordView(UINT nIDTemplate)
		: CFormView(nIDTemplate)
	{
		m_bOnFirstRecord = TRUE;
		m_bOnLastRecord = FALSE;
	}

// Attributes
public:
	virtual ATL::CRowset<>* OnGetRowset() = 0;

// Operations
public:
	virtual BOOL OnMove(UINT nIDMoveCommand);

// Implementation
public:
	virtual void OnInitialUpdate();

protected:
	BOOL m_bOnFirstRecord;
	BOOL m_bOnLastRecord;

	afx_msg void OnUpdateRecordFirst(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecordPrev(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecordNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecordLast(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif __AFXOLEDB_H__

