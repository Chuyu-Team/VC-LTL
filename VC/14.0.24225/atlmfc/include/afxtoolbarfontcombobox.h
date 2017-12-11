// This MFC Library source code supports the Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

#include "afxcontrolbarutil.h"
#include "afxtoolbarcomboboxbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCFontInfo : public CObject
{
public:
	CMFCFontInfo(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet, BYTE nPitchAndFamily, int nType) :
		m_strName(lpszName), m_strScript(lpszScript), m_nCharSet(nCharSet), m_nPitchAndFamily(nPitchAndFamily), m_nType(nType)
	{
	}

	CMFCFontInfo(const CMFCFontInfo& src) :
		m_strName(src.m_strName), m_strScript(src.m_strScript), m_nCharSet(src.m_nCharSet), m_nPitchAndFamily(src.m_nPitchAndFamily), m_nType(src.m_nType)
	{
	}

	const CString m_strName;
	const CString m_strScript;
	const BYTE    m_nCharSet;
	const BYTE    m_nPitchAndFamily;
	const int     m_nType;  // DEVICE_FONTTYPE, RASTER_FONTTYPE or TRUETYPE_FONTTYPE

	CString GetFullName() const
	{
		CString str = m_strName;
		if (!m_strScript.IsEmpty())
		{
			str += _T(" (") + m_strScript + _T(")");
		}

		return str;
	}

	bool operator == (const CMFCFontInfo& other)
	{
		return (m_strName == other.m_strName && m_strScript == other.m_strScript && m_nCharSet == other.m_nCharSet &&
			m_nPitchAndFamily == other.m_nPitchAndFamily && m_nType == other.m_nType);
	}

};

#pragma warning(push)
#pragma warning(disable : 4312)

/*============================================================================*/
//
class CMFCToolBarFontComboBox : public CMFCToolBarComboBoxButton
{
	friend class CMFCRibbonFontComboBox;

	DECLARE_SERIAL(CMFCToolBarFontComboBox)

// Construction/Destruction
public:
	CMFCToolBarFontComboBox(UINT uiID, int iImage, int nFontType = DEVICE_FONTTYPE | RASTER_FONTTYPE | TRUETYPE_FONTTYPE,
		BYTE nCharSet = DEFAULT_CHARSET, DWORD dwStyle = CBS_DROPDOWN, int iWidth = 0, BYTE nPitchAndFamily = DEFAULT_PITCH);
	virtual ~CMFCToolBarFontComboBox();

protected:
	CMFCToolBarFontComboBox(CObList* pLstFontsExternal, int nFontType, BYTE nCharSet, BYTE nPitchAndFamily);
	CMFCToolBarFontComboBox();

// Oprerations:
public:
	BOOL SetFont(LPCTSTR lpszName, BYTE nCharSet = DEFAULT_CHARSET, BOOL bExact = FALSE);
	const CMFCFontInfo* GetFontDesc(int iIndex = -1) const { return(CMFCFontInfo*) GetItemData(iIndex); }

protected:
	void SetContext();
	void RebuildFonts();
	static void __stdcall ClearFonts();
	static int __stdcall GetFontsCount(LPCTSTR lpszName);

// Overrides:
protected:
	virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
	virtual BOOL AddFont(ENUMLOGFONT* pelf, int nType, LPCTSTR lpszScript);
	virtual void Serialize(CArchive& ar);

	static BOOL CALLBACK AFX_EXPORT EnumFamScreenCallBackEx(ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis);
	static BOOL CALLBACK AFX_EXPORT EnumFamPrinterCallBackEx(ENUMLOGFONTEX* pelf, NEWTEXTMETRICEX* /*lpntm*/, int FontType, LPVOID pThis);

	virtual void CopyFrom(const CMFCToolBarButton& s);

// Attributes:
public:
	AFX_IMPORT_DATA static int m_nFontHeight;

protected:
	int  m_nFontType;// Combination of DEVICE_FONTTYPE, RASTER_FONTTYPE and TRUETYPE_FONTTYPE
	BYTE m_nCharSet;
	BYTE m_nPitchAndFamily;

	CObList* m_pLstFontsExternal;

	AFX_IMPORT_DATA static CObList  m_lstFonts;
	AFX_IMPORT_DATA static int      m_nCount;   // CMFCToolBarFontComboBox count
};

#pragma warning(pop)

/*============================================================================*/
// 
class CMFCToolBarFontSizeComboBox : public CMFCToolBarComboBoxButton  
{
	DECLARE_SERIAL(CMFCToolBarFontSizeComboBox)

// Construction/Destruction
public:
	CMFCToolBarFontSizeComboBox(UINT uiID, int iImage, DWORD dwStyle = CBS_DROPDOWN, int iWidth = 0);
	virtual ~CMFCToolBarFontSizeComboBox();

protected:
	CMFCToolBarFontSizeComboBox();

protected:
	int m_nLogVert;

// Operations:
public:
	void RebuildFontSizes(const CString& strFontName);

	void SetTwipSize(int nSize);
	int GetTwipSize() const;

protected:
	CString TwipsToPointString(int nTwips);
	void InsertSize(int nSize);
	static BOOL FAR PASCAL EnumSizeCallBack(LOGFONT FAR* lplf, LPNEWTEXTMETRIC lpntm,int FontType, LPVOID lpv);

// Overrides:
protected:
	virtual CComboBox* CreateCombo(CWnd* pWndParent, const CRect& rect);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
