// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

///////////////////////////////////////////////////////////////////////////
// ATLDBSCH.H : Declarations for OLE DB Schema Rowset Consumer Support
//

#ifndef __ATLDBSCH_H__
#define __ATLDBSCH_H__

#pragma once

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#pragma pack(push,_ATL_PACKING)
namespace ATL
{

template <short nRestrictions>
class _CStoreRestrictions
{
public:
	_CStoreRestrictions()
	{
		m_pvarRestrictions = NULL;
		ATLENSURE(m_pvarRestrictions = _ATL_NEW CComVariant[nRestrictions]);
	}
	~_CStoreRestrictions()
	{
		delete [] m_pvarRestrictions;
	}

ATLPREFAST_SUPPRESS(6387)
	HRESULT GetRowset(
		_Inout_ const CSession& session,
		_In_ const GUID& guidSchema,
		_Outptr_result_maybenull_ IRowset** ppRowset)
	{
		ATLENSURE_RETURN(session.m_spOpenRowset != NULL);
		CComPtr<IDBSchemaRowset> spSchemaRowset;

		HRESULT hr = session.m_spOpenRowset->QueryInterface(__uuidof(IDBSchemaRowset), (void**)&spSchemaRowset);
		if (FAILED(hr))
			return hr;

		return spSchemaRowset->GetRowset(NULL, guidSchema, nRestrictions,
			m_pvarRestrictions, __uuidof(IRowset), 0, NULL, (IUnknown**)ppRowset);
	}
ATLPREFAST_UNSUPPRESS()

	CComVariant* m_pvarRestrictions;
};

template <>
class _CStoreRestrictions<0>
{
public:
ATLPREFAST_SUPPRESS(6387)
	HRESULT GetRowset(
		_Inout_ const CSession& session,
		_In_ const GUID& guidSchema,
		_Outptr_result_maybenull_ IRowset** ppRowset)
	{
		ATLENSURE_RETURN(session.m_spOpenRowset != NULL);
		CComPtr<IDBSchemaRowset> spSchemaRowset;

		HRESULT hr = session.m_spOpenRowset->QueryInterface(__uuidof(IDBSchemaRowset), (void**)&spSchemaRowset);
		if (FAILED(hr))
			return hr;

		return spSchemaRowset->GetRowset(NULL, guidSchema, 0,
			NULL, __uuidof(IRowset), 0, NULL, (IUnknown**)ppRowset);
	}
ATLPREFAST_UNSUPPRESS()
};

///////////////////////////////////////////////////////////////////////////
// class CSchemaRowset
template <class T, short nRestrictions, template <typename T> class TRowset = CRowset>
class CSchemaRowset :
	public CAccessorRowset<T, TRowset>,
	public _CStoreRestrictions<nRestrictions>
{
public:
// Operations
	HRESULT Open(
		_Inout_ const CSession& session,
		_In_ const GUID& guidSchema,
		_In_ bool bBind = true)
	{
		HRESULT hr = GetRowset(session, guidSchema, &this->m_spRowset);
		if ( SUCCEEDED(hr) && bBind )
			hr = this->Bind();

		return hr;
	}
};

 ///////////////////////////////////////////////////////////////////////////
// class CRestrictions

template <class T, short nRestrictions, const GUID* pguid>
class CRestrictions :
	public CSchemaRowset<T, nRestrictions>
{
public:
	HRESULT Open(
		_Inout_ const CSession& session,
		_In_opt_z_ LPCTSTR lpszParam1 = NULL,
		_In_opt_z_ LPCTSTR lpszParam2 = NULL,
		_In_opt_z_ LPCTSTR lpszParam3 = NULL,
		_In_opt_z_ LPCTSTR lpszParam4 = NULL,
		_In_opt_z_ LPCTSTR lpszParam5 = NULL,
		_In_opt_z_ LPCTSTR lpszParam6 = NULL,
		_In_opt_z_ LPCTSTR lpszParam7 = NULL,
		_In_ bool bBind = true)
	{
		USES_CONVERSION_EX;
		CComVariant* pVariant;

		if (this->m_pvarRestrictions == NULL)
			return E_OUTOFMEMORY;

		if (lpszParam1 != NULL)
		{
			this->m_pvarRestrictions->vt = VT_BSTR;
			this->m_pvarRestrictions->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam1));
			if (this->m_pvarRestrictions->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		if (lpszParam2 != NULL)
		{
			pVariant = this->m_pvarRestrictions + 1;
			pVariant->vt = VT_BSTR;
			pVariant->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam2));
			if (pVariant->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		if (lpszParam3 != NULL)
		{
			pVariant = this->m_pvarRestrictions + 2;
			pVariant->vt = VT_BSTR;
			pVariant->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam3));
			if (pVariant->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		if (lpszParam4 != NULL)
		{
			pVariant = this->m_pvarRestrictions + 3;
			pVariant->vt = VT_BSTR;
			pVariant->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam4));
			if (pVariant->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		if (lpszParam5 != NULL)
		{
			pVariant = this->m_pvarRestrictions + 4;
			pVariant->vt = VT_BSTR;
			pVariant->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam5));
			if (pVariant->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		if (lpszParam6 != NULL)
		{
			pVariant = this->m_pvarRestrictions + 5;
			pVariant->vt = VT_BSTR;
			pVariant->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam6));
			if (pVariant->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		if (lpszParam7 != NULL)
		{
			pVariant = this->m_pvarRestrictions + 6;
			pVariant->vt = VT_BSTR;
			pVariant->bstrVal = ::SysAllocString(T2COLE_EX_DEF(lpszParam7));
			if (pVariant->bstrVal == NULL)
				return E_OUTOFMEMORY;
		}

		return CSchemaRowset<T, nRestrictions>::Open(session, *pguid, bBind);
	}
};


///////////////////////////////////////////////////////////////////////////
// CSchemas

class CSchemas
{
public:
	CSchemas()
	{
		m_nSchemas          = 0;
		m_pSchemaGuids      = NULL;
		m_pulRestrictions   = NULL;
	};

	~CSchemas()
	{
		// Clean up allocated memory
		CoTaskMemFree(m_pSchemaGuids);
		CoTaskMemFree(m_pulRestrictions);
	};

// Operations
	HRESULT GetSchemas(_Inout_ const CSession& session)
	{
		CComPtr<IDBSchemaRowset> spSchemaRowset;
		HRESULT hr;

		ATLENSURE_RETURN(session.m_spOpenRowset != NULL);

		hr = session.m_spOpenRowset->QueryInterface(__uuidof(IDBSchemaRowset),
			(void**)&spSchemaRowset);
		if (FAILED(hr))
			return hr;

		return spSchemaRowset->GetSchemas(&m_nSchemas, &m_pSchemaGuids,
				&m_pulRestrictions);
	};

// Attributes
	ULONG   m_nSchemas;
	LPGUID  m_pSchemaGuids;
	ULONG*  m_pulRestrictions;
};


///////////////////////////////////////////////////////////////////////////
// CAssertionInfo

class CAssertionInfo
{
public:
	CAssertionInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szCatalog[129];
	TCHAR           m_szSchema[129];
	TCHAR           m_szName[129];
	VARIANT_BOOL    m_bIsDeferrable;
	VARIANT_BOOL    m_bInitiallyDeferred;
	TCHAR           m_szDescription[129];

// Binding Map
BEGIN_COLUMN_MAP(CAssertionInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY_TYPE(4, DBTYPE_BOOL, m_bIsDeferrable)
	COLUMN_ENTRY_TYPE(5, DBTYPE_BOOL, m_bInitiallyDeferred)
	COLUMN_ENTRY(6, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CCatalogInfo

class CCatalogInfo
{
public:
	CCatalogInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szName[129];
	TCHAR   m_szDescription[129];

// Binding Info
BEGIN_COLUMN_MAP(CCatalogInfo)
	COLUMN_ENTRY(1, m_szName)
	COLUMN_ENTRY(2, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CCharacterSetInfo

class CCharacterSetInfo
{
public:
	CCharacterSetInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szCatalog[129];
	TCHAR           m_szSchema[129];
	TCHAR           m_szName[129];
	TCHAR           m_szFormOfUse[129];
	LARGE_INTEGER   m_nNumCharacters;
	TCHAR           m_szCollateCatalog[129];
	TCHAR           m_szCollateSchema[129];
	TCHAR           m_szCollateName[129];

// Binding Info
BEGIN_COLUMN_MAP(CCharacterSetInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szFormOfUse)
	COLUMN_ENTRY(5, m_nNumCharacters)
	COLUMN_ENTRY(6, m_szCollateCatalog)
	COLUMN_ENTRY(7, m_szCollateSchema)
	COLUMN_ENTRY(8, m_szCollateName)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CCheckConstraintInfo

class CCheckConstraintInfo
{
public:
	CCheckConstraintInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szCheckClause[129];
	TCHAR   m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CCheckConstraintInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szCheckClause)
	COLUMN_ENTRY(5, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CCollationInfo

class CCollationInfo
{
public:
// Constructors
	CCollationInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szCharSetCatalog[129];
	TCHAR   m_szCharSetSchema[129];
	TCHAR   m_szCharSetName[129];
	TCHAR   m_szPadAttribute[10];

// Binding Maps
BEGIN_COLUMN_MAP(CCollationInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szCharSetCatalog)
	COLUMN_ENTRY(5, m_szCharSetSchema)
	COLUMN_ENTRY(6, m_szCharSetName)
	COLUMN_ENTRY(7, m_szPadAttribute)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CColumnDomainUsageInfo

class CColumnDomainUsageInfo
{
public:
// Constructor
	CColumnDomainUsageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	TCHAR   m_szColumnName[129];
	GUID    m_guidColumn;
	ULONG   m_nColumnPropID;

// Binding Maps
BEGIN_COLUMN_MAP(CColumnDomainUsageInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szTableCatalog)
	COLUMN_ENTRY(5, m_szTableSchema)
	COLUMN_ENTRY(6, m_szTableName)
	COLUMN_ENTRY(7, m_szColumnName)
	COLUMN_ENTRY(8, m_guidColumn)
	COLUMN_ENTRY(9, m_nColumnPropID)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CColumnPrivilegeInfo

class CColumnPrivilegeInfo
{
public:
// Constructor
	CColumnPrivilegeInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szGrantor[129];
	TCHAR           m_szGrantee[129];
	TCHAR           m_szTableCatalog[129];
	TCHAR           m_szTableSchema[129];
	TCHAR           m_szTableName[129];
	TCHAR           m_szColumnName[129];
	GUID            m_guidColumn;
	ULONG           m_nColumnPropID;
	TCHAR           m_szPrivilegeType[20];
	VARIANT_BOOL    m_bIsGrantable;

// Binding Maps
BEGIN_COLUMN_MAP(CColumnPrivilegeInfo)
	COLUMN_ENTRY(1,  m_szGrantor)
	COLUMN_ENTRY(2,  m_szGrantee)
	COLUMN_ENTRY(3,  m_szTableCatalog)
	COLUMN_ENTRY(4,  m_szTableSchema)
	COLUMN_ENTRY(5,  m_szTableName)
	COLUMN_ENTRY(6,  m_szColumnName)
	COLUMN_ENTRY(7,  m_guidColumn)
	COLUMN_ENTRY(8,  m_nColumnPropID)
	COLUMN_ENTRY(9,  m_szPrivilegeType)
	COLUMN_ENTRY_TYPE(10, DBTYPE_BOOL, m_bIsGrantable)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CColumnsInfo

class CColumnsInfo
{
public:
// Constructors and Destructors
	CColumnsInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Operations
	TCHAR           m_szTableCatalog[129];
	TCHAR           m_szTableSchema[129];
	TCHAR           m_szTableName[129];
	TCHAR           m_szColumnName[129];
	GUID            m_guidColumn;
	ULONG           m_nColumnPropID;
	ULONG           m_nOrdinalPosition;
	VARIANT_BOOL    m_bColumnHasDefault;
	TCHAR           m_szColumnDefault[129];
	ULONG           m_nColumnFlags;
	VARIANT_BOOL    m_bIsNullable;
	USHORT          m_nDataType;
	GUID            m_guidType;
	ULONG           m_nMaxLength;
	ULONG           m_nOctetLength;
	USHORT          m_nNumericPrecision;
	SHORT           m_nNumericScale;
	ULONG           m_nDateTimePrecision;
	TCHAR           m_szCharSetCatalog[129];
	TCHAR           m_szCharSetSchema[129];
	TCHAR           m_szCharSetName[129];
	TCHAR           m_szCollationCatalog[129];
	TCHAR           m_szCollationSchema[129];
	TCHAR           m_szCollationName[129];
	TCHAR           m_szDomainCatalog[129];
	TCHAR           m_szDomainSchema[129];
	TCHAR           m_szDomainName[129];
	TCHAR           m_szDescription[129];

BEGIN_COLUMN_MAP(CColumnsInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_szColumnName)
	COLUMN_ENTRY(5, m_guidColumn)
	COLUMN_ENTRY(6, m_nColumnPropID)
	COLUMN_ENTRY(7, m_nOrdinalPosition)
	COLUMN_ENTRY_TYPE(8, DBTYPE_BOOL, m_bColumnHasDefault)
	COLUMN_ENTRY(9, m_szColumnDefault)
	COLUMN_ENTRY(10, m_nColumnFlags)
	COLUMN_ENTRY_TYPE(11, DBTYPE_BOOL, m_bIsNullable)
	COLUMN_ENTRY(12, m_nDataType)
	COLUMN_ENTRY(13, m_guidType)
	COLUMN_ENTRY(14, m_nMaxLength)
	COLUMN_ENTRY(15, m_nOctetLength)
	COLUMN_ENTRY(16, m_nNumericPrecision)
	COLUMN_ENTRY(17, m_nNumericScale)
	COLUMN_ENTRY(18, m_nDateTimePrecision)
	COLUMN_ENTRY(19, m_szCharSetCatalog)
	COLUMN_ENTRY(20, m_szCharSetSchema)
	COLUMN_ENTRY(21, m_szCharSetName)
	COLUMN_ENTRY(22, m_szCollationCatalog)
	COLUMN_ENTRY(23, m_szCollationSchema)
	COLUMN_ENTRY(24, m_szCollationName)
	COLUMN_ENTRY(25, m_szDomainCatalog)
	COLUMN_ENTRY(26, m_szDomainSchema)
	COLUMN_ENTRY(27, m_szDomainName)
	COLUMN_ENTRY(28, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CConstraintColumnUsageInfo

class CConstraintColumnUsageInfo
{
public:
// Constructor
	CConstraintColumnUsageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	TCHAR   m_szColumnName[129];
	GUID    m_guidColumn;
	ULONG   m_nColumnPropID;
	TCHAR   m_szConstraintCatalog[129];
	TCHAR   m_szConstraintSchema[129];
	TCHAR   m_szConstraintName[129];

// Binding Maps
BEGIN_COLUMN_MAP(CConstraintColumnUsageInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_szColumnName)
	COLUMN_ENTRY(5, m_guidColumn)
	COLUMN_ENTRY(6, m_nColumnPropID)
	COLUMN_ENTRY(7, m_szConstraintCatalog)
	COLUMN_ENTRY(8, m_szConstraintSchema)
	COLUMN_ENTRY(9, m_szConstraintName)
END_COLUMN_MAP()
};

///////////////////////////////////////////////////////////////////////////
// class CConstraintColumnUsageInfo

class CConstraintTableUsageInfo
{
public:
// Constructor
	CConstraintTableUsageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	TCHAR   m_szConstraintCatalog[129];
	TCHAR   m_szConstraintSchema[129];
	TCHAR   m_szConstraintName[129];

// Binding Maps
BEGIN_COLUMN_MAP(CConstraintTableUsageInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_szConstraintCatalog)
	COLUMN_ENTRY(5, m_szConstraintSchema)
	COLUMN_ENTRY(6, m_szConstraintName)
END_COLUMN_MAP()
};

///////////////////////////////////////////////////////////////////////////
// class CForeignKeysInfo

class CForeignKeysInfo
{
public:
// Constructor
	CForeignKeysInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szPKTableCatalog[129];
	TCHAR   m_szPKTableSchema[129];
	TCHAR   m_szPKTableName[129];
	TCHAR   m_szPKColumnName[129];
	GUID    m_guidPKColumn;
	ULONG   m_nPKColumnPropID;
	TCHAR   m_szFKTableCatalog[129];
	TCHAR   m_szFKTableSchema[129];
	TCHAR   m_szFKTableName[129];
	TCHAR   m_szFKColumnName[129];
	GUID    m_guidFKColumn;
	ULONG   m_nFKColumnPropID;
	ULONG   m_nOrdinal;
	TCHAR   m_szUpdateRule[12];
	TCHAR   m_szDeleteRule[12];

// Binding Info
BEGIN_COLUMN_MAP(CForeignKeysInfo)
	COLUMN_ENTRY(1, m_szPKTableCatalog)
	COLUMN_ENTRY(2, m_szPKTableSchema)
	COLUMN_ENTRY(3, m_szPKTableName)
	COLUMN_ENTRY(4, m_szPKColumnName)
	COLUMN_ENTRY(5, m_guidPKColumn)
	COLUMN_ENTRY(6, m_nPKColumnPropID)
	COLUMN_ENTRY(7, m_szFKTableCatalog)
	COLUMN_ENTRY(8, m_szFKTableSchema)
	COLUMN_ENTRY(9, m_szFKTableName)
	COLUMN_ENTRY(10, m_szFKColumnName)
	COLUMN_ENTRY(11, m_guidFKColumn)
	COLUMN_ENTRY(12, m_nFKColumnPropID)
	COLUMN_ENTRY(13, m_nOrdinal)
	COLUMN_ENTRY(14, m_szUpdateRule)
	COLUMN_ENTRY(15, m_szDeleteRule)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CIndexesInfo

class CIndexesInfo
{
public:
// Constructors
	CIndexesInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szTableCatalog[129];
	TCHAR           m_szTableSchema[129];
	TCHAR           m_szTableName[129];
	TCHAR           m_szIndexCatalog[129];
	TCHAR           m_szIndexSchema[129];
	TCHAR           m_szIndexName[129];
	VARIANT_BOOL    m_bPrimaryKey;
	VARIANT_BOOL    m_bUnique;
	VARIANT_BOOL    m_bClustered;
	USHORT          m_nType;
	LONG            m_nFillFactor;
	LONG            m_nInitialSize;
	LONG            m_nNulls;
	VARIANT_BOOL    m_bSortBookmarks;
	VARIANT_BOOL    m_bAutoUpdate;
	LONG            m_nNullCollation;
	ULONG           m_nOrdinalPosition;
	TCHAR           m_szColumnName[129];
	GUID            m_guidColumn;
	ULONG           m_nColumnPropID;
	SHORT           m_nCollation;
	LONG            m_nCardinality;
	LONG            m_nPages;
	TCHAR           m_szFilterCondition[129];

// Binding Maps
BEGIN_COLUMN_MAP(CIndexesInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_szIndexCatalog)
	COLUMN_ENTRY(5, m_szIndexSchema)
	COLUMN_ENTRY(6, m_szIndexName)
	COLUMN_ENTRY_TYPE(7, DBTYPE_BOOL, m_bPrimaryKey)
	COLUMN_ENTRY_TYPE(8, DBTYPE_BOOL, m_bUnique)
	COLUMN_ENTRY_TYPE(9, DBTYPE_BOOL, m_bClustered)
	COLUMN_ENTRY(10, m_nType)
	COLUMN_ENTRY(11, m_nFillFactor)
	COLUMN_ENTRY(12, m_nInitialSize)
	COLUMN_ENTRY(13, m_nNulls)
	COLUMN_ENTRY_TYPE(14, DBTYPE_BOOL, m_bSortBookmarks)
	COLUMN_ENTRY_TYPE(15, DBTYPE_BOOL, m_bAutoUpdate)
	COLUMN_ENTRY(16, m_nNullCollation)
	COLUMN_ENTRY(17, m_nOrdinalPosition)
	COLUMN_ENTRY(18, m_szColumnName)
	COLUMN_ENTRY(19, m_guidColumn)
	COLUMN_ENTRY(20, m_nColumnPropID)
	COLUMN_ENTRY(21, m_nCollation)
	COLUMN_ENTRY(22, m_nCardinality)
	COLUMN_ENTRY(23, m_nPages)
	COLUMN_ENTRY(25, m_szFilterCondition)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CKeyColumnUsageInfo

class CKeyColumnUsageInfo
{
public:
// Constructors
	CKeyColumnUsageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szConstraintCatalog[129];
	TCHAR   m_szConstraintSchema[129];
	TCHAR   m_szConstraintName[129];
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	TCHAR   m_szColumnName[129];
	GUID    m_guidColumn;
	ULONG   m_nColumnPropID;
	ULONG   m_nOrdinalPosition;

// Binding Maps
BEGIN_COLUMN_MAP(CKeyColumnUsageInfo)
	COLUMN_ENTRY(1,  m_szConstraintCatalog)
	COLUMN_ENTRY(2,  m_szConstraintSchema)
	COLUMN_ENTRY(3,  m_szConstraintName)
	COLUMN_ENTRY(4,  m_szTableCatalog)
	COLUMN_ENTRY(5,  m_szTableSchema)
	COLUMN_ENTRY(6,  m_szTableName)
	COLUMN_ENTRY(7,  m_szColumnName)
	COLUMN_ENTRY(8,  m_guidColumn)
	COLUMN_ENTRY(9,  m_nColumnPropID)
	COLUMN_ENTRY(10, m_nOrdinalPosition)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CPrimaryKeyInfo

class CPrimaryKeyInfo
{
public:
// Constructors
	CPrimaryKeyInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	TCHAR   m_szColumnName[129];
	GUID    m_guidColumn;
	ULONG   m_nColumnPropID;
	ULONG   m_nOrdinal;

// Binding Maps
BEGIN_COLUMN_MAP(CPrimaryKeyInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_szColumnName)
	COLUMN_ENTRY(5, m_guidColumn)
	COLUMN_ENTRY(6, m_nColumnPropID)
	COLUMN_ENTRY(7, m_nOrdinal)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CProcedureColumnInfo

class CProcedureColumnInfo
{
public:
// Constructors
	CProcedureColumnInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szCatalog[129];
	TCHAR           m_szSchema[129];
	TCHAR           m_szName[129];
	TCHAR           m_szColumnName[129];
	GUID            m_guidColumn;
	ULONG           m_nColumnPropID;
	ULONG           m_nRowsetNumber;
	ULONG           m_nOrdinalPosition;
	VARIANT_BOOL    m_bIsNullable;
	USHORT          m_nDataType;
	GUID            m_guidType;
	ULONG           m_nMaxLength;
	ULONG           m_nOctetLength;
	USHORT          m_nPrecision;
	SHORT           m_nScale;
	TCHAR           m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CProcedureColumnInfo)
	COLUMN_ENTRY(1,  m_szCatalog)
	COLUMN_ENTRY(2,  m_szSchema)
	COLUMN_ENTRY(3,  m_szName)
	COLUMN_ENTRY(4,  m_szColumnName)
	COLUMN_ENTRY(5,  m_guidColumn)
	COLUMN_ENTRY(6,  m_nColumnPropID)
	COLUMN_ENTRY(7,  m_nRowsetNumber)
	COLUMN_ENTRY(8,  m_nOrdinalPosition)
	COLUMN_ENTRY_TYPE(9, DBTYPE_BOOL, m_bIsNullable)
	COLUMN_ENTRY(10, m_nDataType)
	COLUMN_ENTRY(11, m_guidType)
	COLUMN_ENTRY(12, m_nMaxLength)
	COLUMN_ENTRY(13, m_nOctetLength)
	COLUMN_ENTRY(14, m_nPrecision)
	COLUMN_ENTRY(15, m_nScale)
	COLUMN_ENTRY(16, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CProcedureParameterInfo

class CProcedureParameterInfo
{
public:
// Constructors
	CProcedureParameterInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szCatalog[129];
	TCHAR           m_szSchema[129];
	TCHAR           m_szName[129];
	TCHAR           m_szParameterName[129];
	USHORT          m_nOrdinalPosition;
	USHORT          m_nType;
	VARIANT_BOOL    m_bHasDefault;
	TCHAR           m_szDefault[129];
	VARIANT_BOOL    m_bIsNullable;
	USHORT          m_nDataType;
	ULONG           m_nMaxLength;
	ULONG           m_nOctetLength;
	USHORT          m_nPrecision;
	SHORT           m_nScale;
	TCHAR           m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CProcedureParameterInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szParameterName)
	COLUMN_ENTRY(5, m_nOrdinalPosition)
	COLUMN_ENTRY(6, m_nType)
	COLUMN_ENTRY_TYPE(7, DBTYPE_BOOL, m_bHasDefault)
	COLUMN_ENTRY(8, m_szDefault)
	COLUMN_ENTRY_TYPE(9, DBTYPE_BOOL, m_bIsNullable)
	COLUMN_ENTRY(10, m_nDataType)
	COLUMN_ENTRY(11, m_nMaxLength)
	COLUMN_ENTRY(12, m_nOctetLength)
	COLUMN_ENTRY(13, m_nPrecision)
	COLUMN_ENTRY(14, m_nScale)
	COLUMN_ENTRY(15, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CProcedureInfo

class CProcedureInfo
{
public:
// Constructors
	CProcedureInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	SHORT   m_nType;
	TCHAR   m_szDefinition[129];
	TCHAR   m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CProcedureInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_nType)
	COLUMN_ENTRY(5, m_szDefinition)
	COLUMN_ENTRY(6, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CProviderTypeInfo

class CProviderTypeInfo
{
public:
// Constructors
	CProviderTypeInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szTypeName[129];
	USHORT          m_nDataType;
	ULONG           m_nColumnSize;
	TCHAR           m_szLiteralPrefix[129];
	TCHAR           m_szLiteralSuffix[129];
	TCHAR           m_szCreateParams[129];
	VARIANT_BOOL    m_bIsNullable;
	VARIANT_BOOL    m_bCaseSensitive;
	ULONG           m_nSearchable;
	VARIANT_BOOL    m_bUnsignedAttribute;
	VARIANT_BOOL    m_bFixedPrecScale;
	VARIANT_BOOL    m_bAutoUniqueValue;
	TCHAR           m_szLocalTypeName[129];
	SHORT           m_nMinScale;
	SHORT           m_nMaxScale;
	GUID            m_guidType;
	TCHAR           m_szTypeLib[129];
	TCHAR           m_szVersion[129];
	VARIANT_BOOL    m_bIsLong;
	VARIANT_BOOL    m_bBestMatch;
	VARIANT_BOOL    m_bIsFixedLength;

// Binding Maps
BEGIN_COLUMN_MAP(CProviderTypeInfo)
	COLUMN_ENTRY(1, m_szTypeName)
	COLUMN_ENTRY(2, m_nDataType)
	COLUMN_ENTRY(3, m_nColumnSize)
	COLUMN_ENTRY(4, m_szLiteralPrefix)
	COLUMN_ENTRY(5, m_szLiteralSuffix)
	COLUMN_ENTRY(6, m_szCreateParams)
	COLUMN_ENTRY_TYPE(7, DBTYPE_BOOL, m_bIsNullable)
	COLUMN_ENTRY_TYPE(8, DBTYPE_BOOL, m_bCaseSensitive)
	COLUMN_ENTRY(9, m_nSearchable)
	COLUMN_ENTRY_TYPE(10, DBTYPE_BOOL, m_bUnsignedAttribute)
	COLUMN_ENTRY_TYPE(11, DBTYPE_BOOL, m_bFixedPrecScale)
	COLUMN_ENTRY_TYPE(12, DBTYPE_BOOL, m_bAutoUniqueValue)
	COLUMN_ENTRY(13, m_szLocalTypeName)
	COLUMN_ENTRY(14, m_nMinScale)
	COLUMN_ENTRY(15, m_nMaxScale)
	COLUMN_ENTRY(16, m_guidType)
	COLUMN_ENTRY(17, m_szTypeLib)
	COLUMN_ENTRY(18, m_szVersion)
	COLUMN_ENTRY_TYPE(19, DBTYPE_BOOL, m_bIsLong)
	COLUMN_ENTRY_TYPE(20, DBTYPE_BOOL, m_bBestMatch)
	COLUMN_ENTRY_TYPE(21, DBTYPE_BOOL, m_bIsFixedLength)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CReferentialConstraintInfo

class CReferentialConstraintInfo
{
public:
// Constructors
	CReferentialConstraintInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szUniqueCatalog[129];
	TCHAR   m_szUniqueSchema[129];
	TCHAR   m_szUniqueName[129];
	TCHAR   m_szMatchOption[8];
	TCHAR   m_szUpdateRule[12];
	TCHAR   m_szDeleteRule[12];
	TCHAR   m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CReferentialConstraintInfo)
	COLUMN_ENTRY(1,  m_szCatalog)
	COLUMN_ENTRY(2,  m_szSchema)
	COLUMN_ENTRY(3,  m_szName)
	COLUMN_ENTRY(4,  m_szUniqueCatalog)
	COLUMN_ENTRY(5,  m_szUniqueSchema)
	COLUMN_ENTRY(6,  m_szUniqueName)
	COLUMN_ENTRY(7,  m_szMatchOption)
	COLUMN_ENTRY(8,  m_szUpdateRule)
	COLUMN_ENTRY(9,  m_szDeleteRule)
	COLUMN_ENTRY(10, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CSchemataInfo

class CSchemataInfo
{
public:
// Constructors
	CSchemataInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szName[129];
	TCHAR   m_szOwner[129];
	TCHAR   m_szCharCatalog[129];
	TCHAR   m_szCharSchema[129];
	TCHAR   m_szCharName[129];

// Binding Maps
BEGIN_COLUMN_MAP(CSchemataInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szName)
	COLUMN_ENTRY(3, m_szOwner)
	COLUMN_ENTRY(4, m_szCharCatalog)
	COLUMN_ENTRY(5, m_szCharSchema)
	COLUMN_ENTRY(6, m_szCharName)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CSQLLangugageInfo

class CSQLLanguageInfo
{
public:
// Constructors
	CSQLLanguageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szSource[10];
	TCHAR   m_szYear[5];
	TCHAR   m_szConformance[13];
	TCHAR   m_szIntegrity[4];
	TCHAR   m_szImplementation[24];
	TCHAR   m_szBindingStyle[10];
	TCHAR   m_szProgrammingLanguage[10];

// Binding Maps
BEGIN_COLUMN_MAP(CSQLLanguageInfo)
	COLUMN_ENTRY(1, m_szSource)
	COLUMN_ENTRY(2, m_szYear)
	COLUMN_ENTRY(3, m_szConformance)
	COLUMN_ENTRY(4, m_szIntegrity)
	COLUMN_ENTRY(5, m_szImplementation)
	COLUMN_ENTRY(6, m_szBindingStyle)
	COLUMN_ENTRY(7, m_szProgrammingLanguage)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CStatisticInfo

class CStatisticInfo
{
public:
// Constructors
	CStatisticInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	LONG    m_nCardinality;

// Binding Maps
BEGIN_COLUMN_MAP(CStatisticInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_nCardinality)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CTableConstraintInfo

class CTableConstraintInfo
{
public:
// Constructors
	CTableConstraintInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szCatalog[129];
	TCHAR           m_szSchema[129];
	TCHAR           m_szName[129];
	TCHAR           m_szTableCatalog[129];
	TCHAR           m_szTableSchema[129];
	TCHAR           m_szTableName[129];
	TCHAR           m_szType[12];
	VARIANT_BOOL    m_bIsDeferrable;
	VARIANT_BOOL    m_bInitiallyDeferred;
	TCHAR           m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CTableConstraintInfo)
	COLUMN_ENTRY(1,  m_szCatalog)
	COLUMN_ENTRY(2,  m_szSchema)
	COLUMN_ENTRY(3,  m_szName)
	COLUMN_ENTRY(4,  m_szTableCatalog)
	COLUMN_ENTRY(5,  m_szTableSchema)
	COLUMN_ENTRY(6,  m_szTableName)
	COLUMN_ENTRY(7,  m_szType)
	COLUMN_ENTRY_TYPE(8, DBTYPE_BOOL, m_bIsDeferrable)
	COLUMN_ENTRY_TYPE(9, DBTYPE_BOOL, m_bInitiallyDeferred)
	COLUMN_ENTRY(10, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CTablePrivilegeInfo

class CTablePrivilegeInfo
{
public:
// Constructors
	CTablePrivilegeInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szGrantor[129];
	TCHAR           m_szGrantee[129];
	TCHAR           m_szCatalog[129];
	TCHAR           m_szSchema[129];
	TCHAR           m_szName[129];
	TCHAR           m_szType[12];
	VARIANT_BOOL    m_bIsGrantable;

// Binding Maps
BEGIN_COLUMN_MAP(CTablePrivilegeInfo)
	COLUMN_ENTRY(1, m_szGrantor)
	COLUMN_ENTRY(2, m_szGrantee)
	COLUMN_ENTRY(3, m_szCatalog)
	COLUMN_ENTRY(4, m_szSchema)
	COLUMN_ENTRY(5, m_szName)
	COLUMN_ENTRY(6, m_szType)
	COLUMN_ENTRY_TYPE(7, DBTYPE_BOOL, m_bIsGrantable)
END_COLUMN_MAP()
};



///////////////////////////////////////////////////////////////////////////
// class CTableInfo

class CTableInfo
{
public:
// Constructors
	CTableInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szType[129];
	GUID    m_guidTable;
	TCHAR   m_szDescription[129];


// Binding Map
BEGIN_COLUMN_MAP(CTableInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szType)
	COLUMN_ENTRY(5, m_guidTable)
	COLUMN_ENTRY(6, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CTranslationInfo

class CTranslationInfo
{
public:
// Constructors
	CTranslationInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szSourceCatalog[129];
	TCHAR   m_szSourceSchema[129];
	TCHAR   m_szSourceName[129];
	TCHAR   m_szTargetCatalog[129];
	TCHAR   m_szTargetSchema[129];
	TCHAR   m_szTargetName[129];

// Binding Maps
BEGIN_COLUMN_MAP(CTranslationInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szSourceCatalog)
	COLUMN_ENTRY(5, m_szSourceSchema)
	COLUMN_ENTRY(6, m_szSourceName)
	COLUMN_ENTRY(7, m_szTargetCatalog)
	COLUMN_ENTRY(8, m_szTargetSchema)
	COLUMN_ENTRY(9, m_szTargetName)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CUsagePrivilegeInfo

class CUsagePrivilegeInfo
{
public:
// Constructors
	CUsagePrivilegeInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szGrantor[129];
	TCHAR   m_szGrantee[129];
	TCHAR   m_szObjectCatalog[129];
	TCHAR   m_szObjectSchema[129];
	TCHAR   m_szObjectName[129];
	TCHAR   m_szObjectType[12];
	TCHAR   m_szPrivilegeType[6];
	VARIANT_BOOL    m_bIsGrantable;

// Binding Maps
BEGIN_COLUMN_MAP(CUsagePrivilegeInfo)
	COLUMN_ENTRY(1, m_szGrantor)
	COLUMN_ENTRY(2, m_szGrantee)
	COLUMN_ENTRY(3, m_szObjectCatalog)
	COLUMN_ENTRY(4, m_szObjectSchema)
	COLUMN_ENTRY(5, m_szObjectName)
	COLUMN_ENTRY(6, m_szObjectType)
	COLUMN_ENTRY(7, m_szPrivilegeType)
	COLUMN_ENTRY_TYPE(8, DBTYPE_BOOL, m_bIsGrantable)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CViewColumnUsageInfo

class CViewColumnUsageInfo
{
public:
// Constructors
	CViewColumnUsageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];
	TCHAR   m_szColumnName[129];
	GUID    m_guidColumn;
	ULONG   m_nColumnPropID;

// Binding Maps
BEGIN_COLUMN_MAP(CViewColumnUsageInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szTableCatalog)
	COLUMN_ENTRY(5, m_szTableSchema)
	COLUMN_ENTRY(6, m_szTableName)
	COLUMN_ENTRY(7, m_szColumnName)
	COLUMN_ENTRY(8, m_guidColumn)
	COLUMN_ENTRY(9, m_nColumnPropID)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CViewTableUsageInfo

class CViewTableUsageInfo
{
public:
// Constructors
	CViewTableUsageInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR   m_szCatalog[129];
	TCHAR   m_szSchema[129];
	TCHAR   m_szName[129];
	TCHAR   m_szTableCatalog[129];
	TCHAR   m_szTableSchema[129];
	TCHAR   m_szTableName[129];

// Binding Maps
BEGIN_COLUMN_MAP(CViewTableUsageInfo)
	COLUMN_ENTRY(1, m_szCatalog)
	COLUMN_ENTRY(2, m_szSchema)
	COLUMN_ENTRY(3, m_szName)
	COLUMN_ENTRY(4, m_szTableCatalog)
	COLUMN_ENTRY(5, m_szTableSchema)
	COLUMN_ENTRY(6, m_szTableName)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// class CViewInfo

class CViewInfo
{
public:
// Constructors
	CViewInfo()
	{
		memset(this, 0, sizeof(*this));
	}

// Attributes
	TCHAR           m_szTableCatalog[129];
	TCHAR           m_szTableSchema[129];
	TCHAR           m_szTableName[129];
	TCHAR           m_szDefinition[129];
	VARIANT_BOOL    m_bCheckOption;
	VARIANT_BOOL    m_bIsUpdatable;
	TCHAR           m_szDescription[129];

// Binding Maps
BEGIN_COLUMN_MAP(CViewInfo)
	COLUMN_ENTRY(1, m_szTableCatalog)
	COLUMN_ENTRY(2, m_szTableSchema)
	COLUMN_ENTRY(3, m_szTableName)
	COLUMN_ENTRY(4, m_szDefinition)
	COLUMN_ENTRY_TYPE(5, DBTYPE_BOOL, m_bCheckOption)
	COLUMN_ENTRY_TYPE(6, DBTYPE_BOOL, m_bIsUpdatable)
	COLUMN_ENTRY(7, m_szDescription)
END_COLUMN_MAP()
};


///////////////////////////////////////////////////////////////////////////
// typedefs

typedef CRestrictions<CAccessor<CAssertionInfo>,            3, &DBSCHEMA_ASSERTIONS>                CAssertions;
typedef CRestrictions<CAccessor<CCatalogInfo>,              1, &DBSCHEMA_CATALOGS >                 CCatalogs;
typedef CRestrictions<CAccessor<CCharacterSetInfo>,         3, &DBSCHEMA_CHARACTER_SETS>            CCharacterSets;
typedef CRestrictions<CAccessor<CCheckConstraintInfo>,      3, &DBSCHEMA_CHECK_CONSTRAINTS>         CCheckConstraints;
typedef CRestrictions<CAccessor<CCollationInfo>,            3, &DBSCHEMA_COLLATIONS>                CCollations;
typedef CRestrictions<CAccessor<CColumnDomainUsageInfo>,    4, &DBSCHEMA_COLUMN_DOMAIN_USAGE>       CColumnDomainUsage;
typedef CRestrictions<CAccessor<CColumnPrivilegeInfo>,      6, &DBSCHEMA_COLUMN_PRIVILEGES>         CColumnPrivileges;
typedef CRestrictions<CAccessor<CColumnsInfo>,              4, &DBSCHEMA_COLUMNS>                   CColumns;
typedef CRestrictions<CAccessor<CConstraintColumnUsageInfo>,4, &DBSCHEMA_CONSTRAINT_COLUMN_USAGE>   CConstraintColumnUsage;
typedef CRestrictions<CAccessor<CConstraintTableUsageInfo>, 3, &DBSCHEMA_CONSTRAINT_TABLE_USAGE>    CConstraintTableUsage;
typedef CRestrictions<CAccessor<CForeignKeysInfo>,          6, &DBSCHEMA_FOREIGN_KEYS>              CForeignKeys;
typedef CRestrictions<CAccessor<CIndexesInfo>,              5, &DBSCHEMA_INDEXES>                   CIndexes;
typedef CRestrictions<CAccessor<CKeyColumnUsageInfo>,       7, &DBSCHEMA_KEY_COLUMN_USAGE>          CKeyColumnUsage;
typedef CRestrictions<CAccessor<CPrimaryKeyInfo>,           3, &DBSCHEMA_PRIMARY_KEYS>              CPrimaryKeys;
typedef CRestrictions<CAccessor<CProcedureColumnInfo>,      4, &DBSCHEMA_PROCEDURE_COLUMNS>         CProcedureColumns;
typedef CRestrictions<CAccessor<CProcedureParameterInfo>,   4, &DBSCHEMA_PROCEDURE_PARAMETERS>      CProcedureParameters;
typedef CRestrictions<CAccessor<CProcedureInfo>,            4, &DBSCHEMA_PROCEDURES>                CProcedures;
typedef CRestrictions<CAccessor<CProviderTypeInfo>,         2, &DBSCHEMA_PROVIDER_TYPES>            CProviderTypes;
typedef CRestrictions<CAccessor<CReferentialConstraintInfo>,3, &DBSCHEMA_REFERENTIAL_CONSTRAINTS>   CReferentialConstraints;
typedef CRestrictions<CAccessor<CSchemataInfo>,             3, &DBSCHEMA_SCHEMATA>                  CSchemata;
typedef CRestrictions<CAccessor<CSQLLanguageInfo>,          0, &DBSCHEMA_SQL_LANGUAGES>             CSQLLanguages;
typedef CRestrictions<CAccessor<CStatisticInfo>,            3, &DBSCHEMA_STATISTICS>                CStatistics ;
typedef CRestrictions<CAccessor<CTableConstraintInfo>,      7, &DBSCHEMA_TABLE_CONSTRAINTS>         CTableConstraints;
typedef CRestrictions<CAccessor<CTablePrivilegeInfo>,       5, &DBSCHEMA_TABLE_PRIVILEGES>          CTablePrivileges;
typedef CRestrictions<CAccessor<CTableInfo>,                4, &DBSCHEMA_TABLES>                    CTables;
typedef CRestrictions<CAccessor<CTranslationInfo>,          3, &DBSCHEMA_TRANSLATIONS>              CTranslations;
typedef CRestrictions<CAccessor<CUsagePrivilegeInfo>,       6, &DBSCHEMA_USAGE_PRIVILEGES>          CUsagePrivileges;
typedef CRestrictions<CAccessor<CViewColumnUsageInfo>,      3, &DBSCHEMA_VIEW_COLUMN_USAGE>         CViewColumnUsage;
typedef CRestrictions<CAccessor<CViewTableUsageInfo>,       3, &DBSCHEMA_VIEW_TABLE_USAGE>          CViewTableUsage;
typedef CRestrictions<CAccessor<CViewInfo>,                 3, &DBSCHEMA_VIEWS>                     CViews;

///////////////////////////////////////////////////////////////////////////

}; //namespace ATL

#pragma pack(pop)
#endif // __ATLDBSCH_H__

///////////////////////////////////////////////////////////////////////////
