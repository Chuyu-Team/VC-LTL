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

#include "afxcontrolbarutil.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarImages;
class CMFCControlRendererInfo;
class CMFCControlRenderer;
class CMFCToolTipInfo;

class CTagManager : public CObject
{
// Construction
public:
	CTagManager(LPCTSTR lpszBuffer = NULL);
	virtual ~CTagManager();

// Attributes:
public:
	LPCTSTR GetBuffer() const
	{
		return m_strBuffer;
	}

	void SetBuffer(LPCTSTR lpszBuffer);

	BOOL IsEmpty() const
	{
		return m_strBuffer.IsEmpty();
	}

// Operations:
public:
	BOOL LoadFromResource(UINT uiResID, LPCTSTR lpszResType);
	BOOL LoadFromResource(LPCTSTR lpszResID, LPCTSTR lpszResType);
	BOOL LoadFromFile(LPCTSTR lpszFileName);

	BOOL ExcludeTag(LPCTSTR lpszTag, CString& strTag, BOOL bIsCharsList = FALSE);

// Global helpers:
	static BOOL __stdcall ParseString(const CString& str, const CString& sep, CStringArray& sa, BOOL bTrim, BOOL bIncludeEmpty);
	static BOOL __stdcall ParseColor(const CString& strItem, COLORREF& value);
	static BOOL __stdcall ParseColorHEX(const CString& strItem, COLORREF& value);
	static BOOL __stdcall ParsePoint(const CString& strItem, CPoint& value);
	static BOOL __stdcall ParseSize(const CString& strItem, CSize& value);
	static BOOL __stdcall ParseRect(const CString& strItem, CRect& value);
	static BOOL __stdcall ParseFont(const CString& strItem, LOGFONT& value);
	static BOOL __stdcall ParseToolBarImages(const CString& strItem, CMFCToolBarImages& value, UINT ID);
	static BOOL __stdcall ParseToolBarImages(const CString& strItem, CMFCToolBarImages& value, LPCTSTR lpszID);
	static BOOL __stdcall ParseControlRendererInfo(const CString& strItem, CMFCControlRendererInfo& value);
	static BOOL __stdcall ParseControlRenderer(const CString& strItem, CMFCControlRenderer& value, UINT ID);
	static BOOL __stdcall ParseControlRenderer(const CString& strItem, CMFCControlRenderer& value, LPCTSTR lpszID);
	static BOOL __stdcall ParseToolTipInfo(const CString& strItem, CMFCToolTipInfo& value);

	// Read tag methods:
	BOOL ReadBool(const CString& strValue, BOOL& value);
	BOOL ReadInt(const CString& strValue, int& value);
	BOOL ReadPoint(const CString& strValue, CPoint& value);
	BOOL ReadSize(const CString& strValue, CSize& value);
	BOOL ReadRect(const CString& strValue, CRect& value);
	BOOL ReadColor(const CString& strValue, COLORREF& value);
	BOOL ReadFont(const CString& strValue, LOGFONT& value);
	BOOL ReadToolBarImages(const CString& strValue, CMFCToolBarImages& value, UINT ID);
	BOOL ReadToolBarImages(const CString& strValue, CMFCToolBarImages& value, LPCTSTR lpszID);
	BOOL ReadControlRendererInfo(const CString& strValue, CMFCControlRendererInfo& value);
	BOOL ReadControlRenderer(const CString& strValue, CMFCControlRenderer& value, UINT ID);
	BOOL ReadControlRenderer(const CString& strValue, CMFCControlRenderer& value, LPCTSTR lpszID);
	BOOL ReadToolTipInfo(const CString& strValue, CMFCToolTipInfo& value);

protected:
	CString m_strBuffer;
};

BOOL __stdcall ReadBoolProp(CTagManager& tagManager, LPCTSTR lpszTag, BOOL& bMember);

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
