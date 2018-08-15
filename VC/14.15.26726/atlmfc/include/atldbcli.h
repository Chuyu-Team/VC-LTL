// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

// ATLDBCLI.H : ATL consumer code for OLEDB

#ifndef __ATLDBCLI_H__
#define __ATLDBCLI_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif


#ifndef __ATLBASE_H__
#include <atlbase.h>
#endif

#include <atlsimpstr.h>

#ifndef __oledb_h__
#include <oledb.h>
#endif // __oledb_h__

#include <oledberr.h>
#include <msdaguid.h>
#include <msdasc.h>

#pragma warning(push)

#pragma warning(disable: 4244)
#pragma warning(disable: 4127)  // conditional expression constant
#pragma warning(disable: 4984)  // if constexpr

#ifndef _CPPUNWIND
#pragma warning(disable: 4702) // unreachable code
#endif

#ifdef _ARM_WORKAROUND_
// OLEDB headers set packing to 2
// There's gap where there should be padding on ARM because it requires
// 8 byte align for double yet pointers are only on 4 byte boundaries
// So it's different from x86 and x64
#pragma warning(push)
#pragma warning(disable:4366)
#endif

#pragma pack(push,_ATL_PACKING)
namespace ATL
{

#define DEFINE_OLEDB_TYPE_FUNCTION(ctype, oledbtype) \
	inline DBTYPE _GetOleDBType(_In_ ctype&) throw ()\
	{ \
		return oledbtype; \
	}
	inline DBTYPE _GetOleDBType(_In_ BYTE[]) throw ()
	{
		return DBTYPE_BYTES;
	}
	inline DBTYPE _GetOleDBType(_In_ CHAR[]) throw ()
	{
		return DBTYPE_STR;
	}
	inline DBTYPE _GetOleDBType(_In_ WCHAR[]) throw()
	{
		return DBTYPE_WSTR;
	}

	DEFINE_OLEDB_TYPE_FUNCTION(const WCHAR*, DBTYPE_WSTR | DBTYPE_BYREF)
	DEFINE_OLEDB_TYPE_FUNCTION(const CHAR*, DBTYPE_STR | DBTYPE_BYREF)

	DEFINE_OLEDB_TYPE_FUNCTION(CComBSTR, DBTYPE_BSTR)

	//DEFINE_OLEDB_TYPE_FUNCTION(__int8, DBTYPE_I1)
	//DEFINE_OLEDB_TYPE_FUNCTION(__int16, DBTYPE_I2)
	//DEFINE_OLEDB_TYPE_FUNCTION(__int32, DBTYPE_I4)
	DEFINE_OLEDB_TYPE_FUNCTION(__int64, DBTYPE_I8)
	//DEFINE_OLEDB_TYPE_FUNCTION(unsigned __int8, DBTYPE_UI1)
	//DEFINE_OLEDB_TYPE_FUNCTION(unsigned __int16, DBTYPE_UI2)
	//DEFINE_OLEDB_TYPE_FUNCTION(unsigned __int32, DBTYPE_UI4)
	DEFINE_OLEDB_TYPE_FUNCTION(unsigned __int64, DBTYPE_UI8)

	DEFINE_OLEDB_TYPE_FUNCTION(signed char      ,DBTYPE_I1)
	DEFINE_OLEDB_TYPE_FUNCTION(SHORT            ,DBTYPE_I2)     // DBTYPE_BOOL
	DEFINE_OLEDB_TYPE_FUNCTION(int              ,DBTYPE_I4)
	DEFINE_OLEDB_TYPE_FUNCTION(LONG             ,DBTYPE_I4)     // DBTYPE_ERROR (SCODE)
	DEFINE_OLEDB_TYPE_FUNCTION(LARGE_INTEGER    ,DBTYPE_I8)     // DBTYPE_CY
	DEFINE_OLEDB_TYPE_FUNCTION(CURRENCY		    ,DBTYPE_CY)     // DBTYPE_CY
	DEFINE_OLEDB_TYPE_FUNCTION(BYTE             ,DBTYPE_UI1)
	DEFINE_OLEDB_TYPE_FUNCTION(unsigned short   ,DBTYPE_UI2)
	DEFINE_OLEDB_TYPE_FUNCTION(unsigned int     ,DBTYPE_UI4)
	DEFINE_OLEDB_TYPE_FUNCTION(unsigned long    ,DBTYPE_UI4)
	DEFINE_OLEDB_TYPE_FUNCTION(ULARGE_INTEGER   ,DBTYPE_UI8)
	DEFINE_OLEDB_TYPE_FUNCTION(float            ,DBTYPE_R4)
	DEFINE_OLEDB_TYPE_FUNCTION(double           ,DBTYPE_R8)     // DBTYPE_DATE
	DEFINE_OLEDB_TYPE_FUNCTION(DECIMAL          ,DBTYPE_DECIMAL)
	DEFINE_OLEDB_TYPE_FUNCTION(DB_NUMERIC       ,DBTYPE_NUMERIC)
	DEFINE_OLEDB_TYPE_FUNCTION(VARIANT          ,DBTYPE_VARIANT)
	DEFINE_OLEDB_TYPE_FUNCTION(IDispatch*       ,DBTYPE_IDISPATCH)
	DEFINE_OLEDB_TYPE_FUNCTION(IUnknown*        ,DBTYPE_IUNKNOWN)
	DEFINE_OLEDB_TYPE_FUNCTION(GUID             ,DBTYPE_GUID)
	DEFINE_OLEDB_TYPE_FUNCTION(SAFEARRAY*       ,DBTYPE_ARRAY)
	DEFINE_OLEDB_TYPE_FUNCTION(DBVECTOR         ,DBTYPE_VECTOR)
	DEFINE_OLEDB_TYPE_FUNCTION(DBDATE           ,DBTYPE_DBDATE)
	DEFINE_OLEDB_TYPE_FUNCTION(DBTIME           ,DBTYPE_DBTIME)
	DEFINE_OLEDB_TYPE_FUNCTION(DBTIMESTAMP      ,DBTYPE_DBTIMESTAMP)
	DEFINE_OLEDB_TYPE_FUNCTION(FILETIME			,DBTYPE_FILETIME)
	DEFINE_OLEDB_TYPE_FUNCTION(PROPVARIANT		,DBTYPE_PROPVARIANT)
	DEFINE_OLEDB_TYPE_FUNCTION(DB_VARNUMERIC	,DBTYPE_VARNUMERIC)

// Internal structure containing the accessor handle and a flag
// indicating whether the data for the accessor is automatically
// retrieved
struct _ATL_ACCESSOR_INFO
{
	HACCESSOR   hAccessor;
	bool        bAutoAccessor;
};

class _CNoOutputColumns
{
public:
	static bool HasOutputColumns() throw ()
	{
		return false;
	}
	static ULONG _GetNumAccessors() throw ()
	{
		return 0;
	}
	static HRESULT _GetBindEntries(
		_In_opt_z_ LPOLESTR*,
		_In_opt_ DBORDINAL*,
		_In_opt_ DBBINDING*,
		_In_ ULONG,
		_In_opt_ bool*,
		_In_opt_ BYTE* pBuffer = NULL,
		_In_ bool bClearOnly = false) throw ()
	{
		(bClearOnly);
		(pBuffer);
		return E_FAIL;
	}
};

class _CNoParameters
{
public:
	static bool HasParameters() throw ()
	{
		return false;
	}
	static HRESULT _GetParamEntries(
		_In_opt_z_ LPOLESTR*,
		_In_opt_ DBORDINAL*,
		_In_opt_ DBBINDING*,
		_In_opt_ BYTE* pBuffer = NULL,
		_In_ bool bClearOnly = false) throw ()
	{
		(bClearOnly);
		(pBuffer);
		return E_FAIL;
	}
};

class _CNoCommand
{
public:
	static HRESULT GetDefaultCommand(
		_In_opt_z_ LPCWSTR* /*ppszCommand*/) throw ()
	{
		return S_OK;
	}
};

typedef _CNoOutputColumns   _OutputColumnsClass;
typedef _CNoParameters      _ParamClass;
typedef _CNoCommand         _CommandClass;

#define BEGIN_ACCESSOR_MAP(x, num) \
	public: \
	typedef x _classtype; \
	typedef x _OutputColumnsClass; \
	static ULONG _GetNumAccessors() throw()\
	{ \
		return num;  \
	} \
	static bool HasOutputColumns() throw () { return true; } \
	/* If pBindings == NULL means we only return the column number */ \
	/* If pBuffer != NULL then it points to the accessor buffer and */ \
	/* we release any appropriate memory e.g. BSTR's or interface pointers */ \
ATLPREFAST_SUPPRESS(6101 6054) \
	inline static HRESULT _GetBindEntries( \
		_Out_writes_opt_(*pColumns) _Outptr_opt_result_maybenull_z_ LPOLESTR* pColumnNames, \
		_Out_ DBORDINAL* pColumns, \
		_In_opt_ DBBINDING *pBinding, \
		_In_ ULONG nAccessor, \
		_Inout_opt_ bool* pAuto, \
		_In_opt_ BYTE* pBuffer = NULL, \
		_In_ bool bClearOnly = false) \
	{ \
		ATLENSURE(pColumns != NULL); \
		DBPARAMIO eParamIO = DBPARAMIO_NOTPARAM; \
		DBORDINAL nColumns = 0; \
		(pBuffer);\

#define BEGIN_ACCESSOR(num, bAuto) \
	if (nAccessor == num) \
	{ \
		if (pBinding != NULL) \
		{ \
			ATLASSUME(pAuto != NULL); \
			*pAuto = bAuto; \
		}

#define END_ACCESSOR() \
	} \
	else

#define END_ACCESSOR_MAP() \
		; \
		*pColumns = nColumns; \
		return S_OK; \
	} \
ATLPREFAST_UNSUPPRESS()

#define BEGIN_COLUMN_MAP(x) \
	BEGIN_ACCESSOR_MAP(x, 1) \
		BEGIN_ACCESSOR(0, true)

#define END_COLUMN_MAP() \
		END_ACCESSOR() \
	END_ACCESSOR_MAP()

#define offsetbuf(m) offsetof(_classtype, m)
#define _OLEDB_TYPE(data) ATL::_GetOleDBType(((_classtype*)0)->data)
#define _SIZE_TYPE(data) sizeof(((_classtype*)0)->data)

#define _COLUMN_ENTRY_CODE(nOrdinal, wType, nLength, nPrecision, nScale, dataOffset, lengthOffset, statusOffset) \
	if (pBuffer != NULL) \
	{ \
		if (!bClearOnly) \
			ATL::CAccessorBase::FreeType(wType, pBuffer + dataOffset); \
		memset(pBuffer + dataOffset, 0, nLength); \
	} \
	else if (pBinding != NULL) \
	{ \
		ATLENSURE( pColumnNames != NULL ); \
		*pColumnNames = NULL; \
		ATL::CAccessorBase::Bind(pBinding, nOrdinal, wType, nLength, nPrecision, nScale, eParamIO, \
			dataOffset, lengthOffset, statusOffset); \
		pColumnNames++; \
		pBinding++; \
	} \
	nColumns++;

#define _COLUMN_NAME_CODE(pszName, wType, nLength, nPrecision, nScale, dataOffset, lengthOffset, statusOffset) \
	if (pBuffer != NULL) \
	{ \
		if (!bClearOnly) \
			ATL::CAccessorBase::FreeType(wType, pBuffer + dataOffset); \
		memset(pBuffer + dataOffset, 0, nLength); \
	} \
	else if (pBinding != NULL) \
	{ \
		ATLENSURE( pColumnNames != NULL ); \
		*pColumnNames = pszName; \
		ATL::CAccessorBase::Bind(pBinding, 0, wType, nLength, nPrecision, nScale, eParamIO, \
			dataOffset, lengthOffset, statusOffset); \
		pColumnNames++; \
		pBinding++; \
	} \
	nColumns++;

///////////////////////////////////////////////////////////////////////////
// the following macros are used for binding column by the column ordinal number

#define COLUMN_ENTRY_EX(nOrdinal, wType, nLength, nPrecision, nScale, data, length, status) \
	_COLUMN_ENTRY_CODE(nOrdinal, wType, nLength, nPrecision, nScale, offsetbuf(data), offsetbuf(length), offsetbuf(status))

#define COLUMN_ENTRY_TYPE(nOrdinal, wType, data) \
	COLUMN_ENTRY_TYPE_SIZE(nOrdinal, wType, _SIZE_TYPE(data), data)

#define COLUMN_ENTRY_TYPE_SIZE(nOrdinal, wType, nLength, data) \
	_COLUMN_ENTRY_CODE(nOrdinal, wType, nLength, 0, 0, offsetbuf(data), 0, 0)

#define COLUMN_ENTRY_TYPE_STATUS(nOrdinal, wType, status, data) \
	_COLUMN_ENTRY_CODE(nOrdinal, wType, _SIZE_TYPE(data), 0, 0, offsetbuf(data), 0, offsetbuf(status))

#define COLUMN_ENTRY_TYPE_PS(nOrdinal, wType, nPrecision, nScale, data) \
	_COLUMN_ENTRY_CODE(nOrdinal, wType, _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), 0, 0)

// Standard macros where type and size is worked out
#define COLUMN_ENTRY(nOrdinal, data) \
	COLUMN_ENTRY_TYPE(nOrdinal, _OLEDB_TYPE(data), data)

#define COLUMN_ENTRY_LENGTH(nOrdinal, data, length) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), 0, 0, offsetbuf(data), offsetbuf(length), 0)

#define COLUMN_ENTRY_STATUS(nOrdinal, data, status) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), 0, 0, offsetbuf(data), 0, offsetbuf(status))

#define COLUMN_ENTRY_LENGTH_STATUS(nOrdinal, data, length, status) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), 0, 0, offsetbuf(data), offsetbuf(length), offsetbuf(status))

// Follow macros are used if precision and scale need to be specified
#define COLUMN_ENTRY_PS(nOrdinal, nPrecision, nScale, data) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), 0, 0)

#define COLUMN_ENTRY_PS_LENGTH(nOrdinal, nPrecision, nScale, data, length) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), offsetbuf(length), 0)

#define COLUMN_ENTRY_PS_STATUS(nOrdinal, nPrecision, nScale, data, status) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), 0, offsetbuf(status))

#define COLUMN_ENTRY_PS_LENGTH_STATUS(nOrdinal, nPrecision, nScale, data, length, status) \
	_COLUMN_ENTRY_CODE(nOrdinal, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), offsetbuf(length), offsetbuf(status))

///////////////////////////////////////////////////////////////////////////
// the following macros are used for binding column by the column name

#define COLUMN_NAME_EX(pszName, wType, nLength, nPrecision, nScale, data, length, status) \
	_COLUMN_NAME_CODE(pszName, wType, nLength, nPrecision, nScale, offsetbuf(data), offsetbuf(length), offsetbuf(status))

#define COLUMN_NAME_TYPE(pszName, wType, data) \
	COLUMN_NAME_TYPE_SIZE(pszName, wType, _SIZE_TYPE(data), data)

#define COLUMN_NAME_TYPE_SIZE(pszName, wType, nLength, data) \
	_COLUMN_NAME_CODE(pszName, wType, nLength, 0, 0, offsetbuf(data), 0, 0)

#define COLUMN_NAME_TYPE_STATUS(pszName, wType, status, data) \
	_COLUMN_NAME_CODE(pszName, wType, _SIZE_TYPE(data), 0, 0, offsetbuf(data), 0, offsetbuf(status))

#define COLUMN_NAME_TYPE_PS(pszName, wType, nPrecision, nScale, data) \
	_COLUMN_NAME_CODE(pszName, wType, _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), 0, 0)

// Standard macros where type and size is worked out
#define COLUMN_NAME(pszName, data) \
	COLUMN_NAME_TYPE(pszName, _OLEDB_TYPE(data), data)

#define COLUMN_NAME_LENGTH(pszName, data, length) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), 0, 0, offsetbuf(data), offsetbuf(length), 0)

#define COLUMN_NAME_STATUS(pszName, data, status) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), 0, 0, offsetbuf(data), 0, offsetbuf(status))

#define COLUMN_NAME_LENGTH_STATUS(pszName, data, length, status) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), 0, 0, offsetbuf(data), offsetbuf(length), offsetbuf(status))

// Follow macros are used if precision and scale need to be specified
#define COLUMN_NAME_PS(pszName, nPrecision, nScale, data) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), 0, 0)

#define COLUMN_NAME_PS_LENGTH(pszName, nPrecision, nScale, data, length) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), offsetbuf(length), 0)

#define COLUMN_NAME_PS_STATUS(pszName, nPrecision, nScale, data, status) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), 0, offsetbuf(status))

#define COLUMN_NAME_PS_LENGTH_STATUS(pszName, nPrecision, nScale, data, length, status) \
	_COLUMN_NAME_CODE(pszName, _OLEDB_TYPE(data), _SIZE_TYPE(data), nPrecision, nScale, offsetbuf(data), offsetbuf(length), offsetbuf(status))



#define BOOKMARK_ENTRY(variable) \
	COLUMN_ENTRY_TYPE_SIZE(0, DBTYPE_BYTES, _SIZE_TYPE(variable##.m_rgBuffer), variable##.m_rgBuffer)

#define _BLOB_ENTRY_CODE(nOrdinal, IID, flags, dataOffset, lengthOffset, statusOffset) \
	if (pBuffer != NULL) \
	{ \
		if (!bClearOnly) \
			ATL::CAccessorBase::FreeType(DBTYPE_IUNKNOWN, pBuffer + dataOffset); \
		memset(pBuffer + dataOffset, 0, sizeof(IUnknown*)); \
	} \
	else if (pBinding != NULL) \
	{ \
		ATLASSERT( pColumnNames != NULL ); \
		DBOBJECT* pObject = _ATL_NEW DBOBJECT; \
		if (pObject == NULL) \
			return E_OUTOFMEMORY; \
		pObject->dwFlags = flags; \
		pObject->iid     = IID; \
		*pColumnNames = NULL; \
		ATL::CAccessorBase::Bind(pBinding, nOrdinal, DBTYPE_IUNKNOWN, sizeof(IUnknown*), 0, 0, eParamIO, \
			dataOffset, lengthOffset, statusOffset, pObject); \
		pColumnNames++; \
		pBinding++; \
	} \
	nColumns++;

#define _BLOB_NAME_CODE(pszName, IID, flags, dataOffset, lengthOffset, statusOffset) \
	if (pBuffer != NULL) \
	{ \
		if (!bClearOnly) \
			ATL::CAccessorBase::FreeType(DBTYPE_IUNKNOWN, pBuffer + dataOffset); \
		memset(pBuffer + dataOffset, 0, sizeof(IUnknown*)); \
	} \
	else if (pBinding != NULL) \
	{ \
		ATLASSERT( pColumnNames != NULL ); \
		DBOBJECT* pObject = _ATL_NEW DBOBJECT; \
		if (pObject == NULL) \
			return E_OUTOFMEMORY; \
		pObject->dwFlags = flags; \
		pObject->iid     = IID; \
		*pColumnNames = pszName; \
		ATL::CAccessorBase::Bind(pBinding, 0, DBTYPE_IUNKNOWN, sizeof(IUnknown*), 0, 0, eParamIO, \
			dataOffset, lengthOffset, statusOffset, pObject); \
		pColumnNames++; \
		pBinding++; \
	} \
	nColumns++;

#define BLOB_ENTRY(nOrdinal, IID, flags, data) \
	_BLOB_ENTRY_CODE(nOrdinal, IID, flags, offsetbuf(data), 0, 0);

#define BLOB_ENTRY_STATUS(nOrdinal, IID, flags, data, status) \
	_BLOB_ENTRY_CODE(nOrdinal, IID, flags, offsetbuf(data), 0, offsetbuf(status));

#define BLOB_ENTRY_LENGTH(nOrdinal, IID, flags, data, length) \
	_BLOB_ENTRY_CODE(nOrdinal, IID, flags, offsetbuf(data), offsetbuf(length), 0);

#define BLOB_ENTRY_LENGTH_STATUS(nOrdinal, IID, flags, data, length, status) \
	_BLOB_ENTRY_CODE(nOrdinal, IID, flags, offsetbuf(data), offsetbuf(length), offsetbuf(status));

#define BLOB_NAME(pszName, IID, flags, data) \
	_BLOB_NAME_CODE(pszName, IID, flags, offsetbuf(data), 0, 0);

#define BLOB_NAME_STATUS(pszName, IID, flags, data, status) \
	_BLOB_NAME_CODE(pszName, IID, flags, offsetbuf(data), 0, offsetbuf(status));

#define BLOB_NAME_LENGTH(pszName, IID, flags, data, length) \
	_BLOB_NAME_CODE(pszName, IID, flags, offsetbuf(data), offsetbuf(length), 0);

#define BLOB_NAME_LENGTH_STATUS(pszName, IID, flags, data, length, status) \
	_BLOB_NAME_CODE(pszName, IID, flags, offsetbuf(data), offsetbuf(length), offsetbuf(status));

#define BEGIN_PARAM_MAP(x) \
	public: \
	typedef x _classtype; \
	typedef x _ParamClass; \
	static bool HasParameters() throw () { return true; } \
	/* If pBinding == NULL means we only return the column number */ \
	/* If pBuffer != NULL then it points to the accessor buffer and */ \
	/* we release any appropriate memory e.g. BSTR's or interface pointers */ \
ATLPREFAST_SUPPRESS(6101 6054) \
	static HRESULT _GetParamEntries( \
		_Out_writes_opt_(*pColumns) _Outptr_opt_result_maybenull_z_ LPOLESTR* pColumnNames, \
		_Out_ DBORDINAL* pColumns, \
		_In_opt_ DBBINDING *pBinding, \
		_In_opt_ BYTE* pBuffer = NULL, \
		_In_ bool bClearOnly = false) \
	{ \
		ATLENSURE(pColumns != NULL); \
		DBPARAMIO eParamIO = DBPARAMIO_INPUT; \
		int nColumns = 0; \
		(pBuffer);

#define END_PARAM_MAP() \
		*pColumns = nColumns; \
		return S_OK; \
	} \
ATLPREFAST_UNSUPPRESS()

#define SET_PARAM_TYPE(type) \
	eParamIO = type;

#ifdef _UNICODE
#define DEFINE_COMMAND(x, szCommand) \
	typedef x _CommandClass; \
	static HRESULT GetDefaultCommand(LPCWSTR* ppwszCommand) throw () \
	{ \
		*ppwszCommand = szCommand; \
		return S_OK; \
	}
#else // !_UNICODE
#define DEFINE_COMMAND(x, szCommand) \
	typedef x _CommandClass; \
	static HRESULT GetDefaultCommand(LPCWSTR* ppwszCommand) throw () \
	{ \
		static CA2WEX<sizeof(szCommand)> szCmd(szCommand); \
		*ppwszCommand = szCmd; \
		return S_OK; \
	}
#endif // !_UNICODE

#pragma deprecated("DEFINE_COMMAND") // Use DEFINE_COMMAND_EX instead!

#define DEFINE_COMMAND_EX(x, wszCommand) \
	typedef x _CommandClass; \
	static HRESULT GetDefaultCommand(LPCWSTR* ppwszCommand) throw () \
	{ \
		*ppwszCommand = wszCommand; \
		return S_OK; \
	}


///////////////////////////////////////////////////////////////////////////
// class CDBErrorInfo

class CDBErrorInfo
{
public:
	// Use to get the number of error record when you want to explicitly check that
	// the passed interface set the error information
	HRESULT GetErrorRecords(
		_Inout_ IUnknown* pUnk,
		_In_ const IID& iid,
		_Out_ ULONG* pcRecords) throw()
	{
		ATLENSURE_RETURN(pUnk);
		ATLENSURE_RETURN(pcRecords);
		CComPtr<ISupportErrorInfo> spSupportErrorInfo;
		HRESULT hr = pUnk->QueryInterface(&spSupportErrorInfo);
		if (FAILED(hr))
			return hr;

		hr = spSupportErrorInfo->InterfaceSupportsErrorInfo(iid);
		if (FAILED(hr))
			return hr;

		return GetErrorRecords(pcRecords);
	}
	// Use to get the number of error records
	HRESULT GetErrorRecords(_Out_ ULONG* pcRecords) throw ()
	{
		ATLENSURE_RETURN(pcRecords != NULL);
		HRESULT hr;
		m_spErrorInfo.Release();
		m_spErrorRecords.Release();
		hr = ::GetErrorInfo(0, &m_spErrorInfo);
		if (hr == S_FALSE)
			return E_FAIL;

		hr = m_spErrorInfo->QueryInterface(__uuidof(IErrorRecords), (void**)&m_spErrorRecords);
		if (FAILED(hr))
		{
			// Well we got the IErrorInfo so we'll just treat that as
			// the one record
			*pcRecords = 1;
			return S_OK;
		}

		return m_spErrorRecords->GetRecordCount(pcRecords);
	}
	// Get the error information for the passed record number. GetErrorRecords must
	// be called before this function is called.
	HRESULT GetAllErrorInfo(
		_In_ ULONG ulRecordNum,
		_In_ LCID lcid,
		_Outptr_opt_result_maybenull_z_ BSTR* pbstrDescription,
		_Outptr_opt_result_maybenull_z_ BSTR* pbstrSource = NULL,
		_Out_opt_ GUID* pguid = NULL,
		_Out_opt_ DWORD* pdwHelpContext = NULL,
		_Outptr_opt_result_maybenull_z_ BSTR* pbstrHelpFile = NULL) const throw()
	{
		CComPtr<IErrorInfo> spErrorInfo;

		// If we have the IErrorRecords interface pointer then use it, otherwise
		// we'll just default to the IErrorInfo we have already retrieved in the call
		// to GetErrorRecords
		if (m_spErrorRecords != NULL)
		{
			HRESULT hr = m_spErrorRecords->GetErrorInfo(ulRecordNum, lcid, &spErrorInfo);
			if (FAILED(hr))
				return hr;
		}
		else
		{
			ATLASSUME(m_spErrorInfo != NULL);
			spErrorInfo = m_spErrorInfo;
		}

		if (pbstrDescription != NULL)
			if (FAILED(spErrorInfo->GetDescription(pbstrDescription)))
					*pbstrDescription = NULL;

		if (pguid != NULL)
			if (FAILED(spErrorInfo->GetGUID(pguid)))
				*pguid = GUID{};

		if (pdwHelpContext != NULL)
			if (FAILED(spErrorInfo->GetHelpContext(pdwHelpContext)))
				*pdwHelpContext = 0;

		if (pbstrHelpFile != NULL)
			if (FAILED(spErrorInfo->GetHelpFile(pbstrHelpFile)))
				*pbstrHelpFile = NULL;

		if (pbstrSource != NULL)
			if (FAILED(spErrorInfo->GetSource(pbstrSource)))
				*pbstrSource = NULL;

		return S_OK;
	}
	// Get the error information for the passed record number
	HRESULT GetBasicErrorInfo(
		_In_ ULONG ulRecordNum,
		_Out_ ERRORINFO* pErrorInfo) const throw ()
	{
		return m_spErrorRecords->GetBasicErrorInfo(ulRecordNum, pErrorInfo);
	}

ATLPREFAST_SUPPRESS(6387)
	// Get the custom error object for the passed record number
	HRESULT GetCustomErrorObject(
		_In_ ULONG ulRecordNum,
		_In_ REFIID riid,
		_Outptr_result_maybenull_ IUnknown** ppObject) const throw ()
	{
		return m_spErrorRecords->GetCustomErrorObject(ulRecordNum, riid, ppObject);
	}
ATLPREFAST_UNSUPPRESS();

	// Get the IErrorInfo interface for the passed record number
	HRESULT GetErrorInfo(
		_In_ ULONG ulRecordNum,
		_In_ LCID lcid,
		_Outptr_ IErrorInfo** ppErrorInfo) const throw ()
	{
		return m_spErrorRecords->GetErrorInfo(ulRecordNum, lcid, ppErrorInfo);
	}
	// Get the error parameters for the passed record number
	HRESULT GetErrorParameters(
		_In_ ULONG ulRecordNum,
		_Out_ DISPPARAMS* pdispparams) const throw ()
	{
		return m_spErrorRecords->GetErrorParameters(ulRecordNum, pdispparams);
	}

// Implementation
	CComPtr<IErrorInfo>     m_spErrorInfo;
	CComPtr<IErrorRecords>  m_spErrorRecords;
};

#ifdef _DEBUG
inline void AtlTraceErrorRecords(_In_ HRESULT hrErr = S_OK)
{
	CDBErrorInfo ErrorInfo;
	ULONG        cRecords;
	HRESULT      hr;
	ULONG        i;
	CComBSTR     bstrDesc, bstrHelpFile, bstrSource;
	GUID         guid;
	DWORD        dwHelpContext;
	WCHAR        wszGuid[40];

	// If the user passed in an HRESULT then trace it
	if (hrErr != S_OK)
	{
		ATLTRACE(atlTraceDBClient, 0, _T("OLE DB Error Record dump for hr = 0x%x\n"), hrErr);
	}

	LCID lcLocale = GetSystemDefaultLCID();

	hr = ErrorInfo.GetErrorRecords(&cRecords);
	if (FAILED(hr) || ErrorInfo.m_spErrorInfo == NULL)
	{
		ATLTRACE(atlTraceDBClient, 0, _T("No OLE DB Error Information found: hr = 0x%x\n"), hr);
	}
	else
	{
		for (i = 0; i < cRecords; i++)
		{
			hr = ErrorInfo.GetAllErrorInfo(i, lcLocale, &bstrDesc, &bstrSource, &guid,
										&dwHelpContext, &bstrHelpFile);
			if (FAILED(hr))
			{
				ATLTRACE(atlTraceDBClient, 0,
					_T("OLE DB Error Record dump retrieval failed: hr = 0x%x\n"), hr);
				return;
			}
			int result = ::StringFromGUID2(guid, wszGuid, sizeof(wszGuid) / sizeof(WCHAR));
			ATLASSERT(result != 0);
			UNREFERENCED_PARAMETER(result);
			ATLTRACE(atlTraceDBClient, 0,
				_T("Row #: %4d Source: \"%Ts\" Description: \"%Ts\" Help File: \"%Ts\" Help Context: %4d GUID: %Ts\n"),
				i, static_cast<TCHAR*>(COLE2T(bstrSource)), static_cast<TCHAR*>(COLE2T(bstrDesc)), static_cast<TCHAR*>(COLE2T(bstrHelpFile)), dwHelpContext, static_cast<TCHAR*>(COLE2T(wszGuid)));
			bstrSource.Empty();
			bstrDesc.Empty();
			bstrHelpFile.Empty();
		}
		ATLTRACE(atlTraceDBClient, 0, _T("OLE DB Error Record dump end\n"));
	}
}
#else
inline void AtlTraceErrorRecords(_In_ HRESULT hrErr = S_OK) throw()
{
	(hrErr);
}
#endif


///////////////////////////////////////////////////////////////////////////
// class CDBPropSet

class CDBPropSet :
	public tagDBPROPSET
{
public:
	CDBPropSet()
	{
		rgProperties    = NULL;
		cProperties     = 0;
	}
	CDBPropSet(_In_ const GUID& guid)
	{
		rgProperties    = NULL;
		cProperties     = 0;
		guidPropertySet = guid;
	}
	CDBPropSet(_In_ const CDBPropSet& propset)
	{
		InternalCopy(propset);
	}
	~CDBPropSet()
	{
		for (ULONG i = 0; i < cProperties; i++)
			VariantClear(&rgProperties[i].vValue);

		CoTaskMemFree(rgProperties);
	}
	CDBPropSet& operator=(_In_ CDBPropSet& propset) throw()
	{
		if(this!=&propset)
		{
			this->~CDBPropSet();
			InternalCopy(propset);
		}
		return *this;
	}
	// Set the GUID of the property set this class represents.
	// Use if you didn't pass the GUID to the constructor.
	void SetGUID(_In_ const GUID& guid) throw()
	{
		guidPropertySet = guid;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ const VARIANT& var,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		HRESULT hr;
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID   = dwPropertyID;
		hr = ::VariantCopy(&(rgProperties[cProperties].vValue), const_cast<VARIANT*>(&var));
		if (FAILED(hr))
			return false;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_z_ LPCSTR szValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		if (!Add(propoptions))
		{
			return false;
		}
		rgProperties[cProperties].dwPropertyID   = dwPropertyID;
		rgProperties[cProperties].vValue.vt      = VT_BSTR;
		rgProperties[cProperties].vValue.bstrVal = CComBSTR(szValue).Detach();
#pragma warning(push)
#pragma warning(disable:4068)
#pragma prefast(push)
#pragma prefast(disable:325, "We are deliberately checking if this has already been allocated")
		if (rgProperties[cProperties].vValue.bstrVal == NULL)
		{
			return false;
		}
#pragma prefast(pop)
#pragma warning(pop)
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_z_ LPCWSTR szValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		if (!Add(propoptions))
		{
			return false;
		}
		rgProperties[cProperties].dwPropertyID   = dwPropertyID;
		rgProperties[cProperties].vValue.vt      = VT_BSTR;
		rgProperties[cProperties].vValue.bstrVal = CComBSTR(szValue).Detach();
#pragma warning(push)
#pragma warning(disable:4068)
#pragma prefast(push)
#pragma prefast(disable:325, "We are deliberately checking if this has already been allocated")
		if (rgProperties[cProperties].vValue.bstrVal == NULL)
		{
			return false;
		}
#pragma prefast(pop)
#pragma warning(pop)
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ bool bValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID   = dwPropertyID;
		rgProperties[cProperties].vValue.vt      = VT_BOOL;
		rgProperties[cProperties].vValue.boolVal = (bValue) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ BYTE bValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED)
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID  = dwPropertyID;
		rgProperties[cProperties].vValue.vt     = VT_UI1;
		rgProperties[cProperties].vValue.bVal   = bValue;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ short nValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED)
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID  = dwPropertyID;
		rgProperties[cProperties].vValue.vt     = VT_I2;
		rgProperties[cProperties].vValue.iVal   = nValue;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ long nValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED)
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID  = dwPropertyID;
		rgProperties[cProperties].vValue.vt     = VT_I4;
		rgProperties[cProperties].vValue.lVal   = nValue;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ float fltValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED)
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID  = dwPropertyID;
		rgProperties[cProperties].vValue.vt     = VT_R4;
		rgProperties[cProperties].vValue.fltVal = fltValue;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ double dblValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID  = dwPropertyID;
		rgProperties[cProperties].vValue.vt     = VT_R8;
		rgProperties[cProperties].vValue.dblVal = dblValue;
		cProperties++;
		return true;
	}
	// Add the passed property to the property set
	_Check_return_ bool AddProperty(
		_In_ DWORD dwPropertyID,
		_In_ CY cyValue,
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		if (!Add(propoptions))
			return false;
		rgProperties[cProperties].dwPropertyID  = dwPropertyID;
		rgProperties[cProperties].vValue.vt     = VT_CY;
		rgProperties[cProperties].vValue.cyVal  = cyValue;
		cProperties++;
		return true;
	}
// Implementation
	// Create memory to add a new property
	_Check_return_ bool Add(
		_In_ DBPROPOPTIONS propoptions = DBPROPOPTIONS_REQUIRED) throw()
	{
		if (((cProperties + 1) < cProperties))
		{
			return false;
		}
		DBPROP* rgTemp = (DBPROP*)::ATL::AtlCoTaskMemRecalloc(rgProperties, (cProperties + 1), sizeof(DBPROP));
		if (rgTemp == NULL)
			return false;
		rgProperties = rgTemp;
		rgProperties[cProperties].dwOptions = propoptions;
		rgProperties[cProperties].colid     = DB_NULLID;
		rgProperties[cProperties].vValue.vt = VT_EMPTY;
		return true;
	}
	// Copies in the passed value now it this value been cleared
	void InternalCopy(_In_ const CDBPropSet& propset) throw()
	{
		cProperties     = propset.cProperties;
		rgProperties    = (DBPROP*)::ATL::AtlCoTaskMemCAlloc(cProperties, static_cast<ULONG>(sizeof(DBPROP)));
		if (rgProperties != NULL)
		{
			guidPropertySet = propset.guidPropertySet;
			for (ULONG i = 0; i < cProperties; i++)
			{
				rgProperties[i].dwPropertyID = propset.rgProperties[i].dwPropertyID;
				if( propset.rgProperties[i].dwOptions == DBPROPOPTIONS_OPTIONAL )
					rgProperties[i].dwOptions = DBPROPOPTIONS_OPTIONAL;
				else
					rgProperties[i].dwOptions = DBPROPOPTIONS_REQUIRED;
				rgProperties[i].colid        = DB_NULLID;
				rgProperties[i].vValue.vt    = VT_EMPTY;
				HRESULT hr = VariantCopy(&rgProperties[i].vValue, &propset.rgProperties[i].vValue);
				ATLASSERT( SUCCEEDED(hr) );
				if( FAILED(hr) )
					VariantInit( &rgProperties[i].vValue );
			}
		}
		else
		{
			// The memory allocation failed so set the count
			// of properties to zero
			cProperties = 0;
		}
	}
};


///////////////////////////////////////////////////////////////////////////
// class CDBPropIDSet

class CDBPropIDSet :
	public tagDBPROPIDSET
{
// Constructors and Destructors
public:
	CDBPropIDSet()
	{
		rgPropertyIDs   = NULL;
		cPropertyIDs    = 0;
	}
	CDBPropIDSet(_In_ const GUID& guid)
	{
		rgPropertyIDs   = NULL;
		cPropertyIDs    = 0;
		guidPropertySet = guid;
	}
	CDBPropIDSet(_In_ const CDBPropIDSet& propidset)
	{
		InternalCopy(propidset);
	}
	~CDBPropIDSet()
	{
		free(rgPropertyIDs);
	}
	CDBPropIDSet& operator=(_In_ CDBPropIDSet& propset) throw()
	{
		this->~CDBPropIDSet();
		InternalCopy(propset);
		return *this;
	}
	// Set the GUID of the property ID set
	void SetGUID(_In_ const GUID& guid) throw()
	{
		guidPropertySet = guid;
	}
	// Add a property ID to the set
	bool AddPropertyID(_In_ DBPROPID propid) throw()
	{
		if (!Add())
			return false;
		rgPropertyIDs[cPropertyIDs] = propid;
		cPropertyIDs++;
		return true;
	}
// Implementation
	bool Add() throw()
	{
		DBPROPID* pTempID  = (DBPROPID*)_recalloc(rgPropertyIDs, (cPropertyIDs + 1), sizeof(DBPROPID));
		if (pTempID == NULL)
			return false;

		rgPropertyIDs = pTempID;
		return true;
	}
	void InternalCopy(_In_ const CDBPropIDSet& propidset) throw()
	{
		cPropertyIDs    = propidset.cPropertyIDs;
		rgPropertyIDs   = NULL;
		ATLTRY(rgPropertyIDs   = (DBPROPID*)calloc(cPropertyIDs, sizeof(DBPROPID)));
		if (rgPropertyIDs != NULL)
		{
			guidPropertySet = propidset.guidPropertySet;
			for (ULONG i = 0; i < cPropertyIDs; i++)
				rgPropertyIDs[i] = propidset.rgPropertyIDs[i];
		}
		else
		{
			// The memory allocation failed so set the count
			// of properties to zero
			cPropertyIDs = 0;
		}
	}
};


///////////////////////////////////////////////////////////////////////////
// class CBookmarkBase

class ATL_NO_VTABLE CBookmarkBase
{
public:
	virtual DBLENGTH GetSize() const = 0;
	virtual BYTE* GetBuffer() const = 0;
};


///////////////////////////////////////////////////////////////////////////
// class CBookmark

template <DBLENGTH nSize = 0>
class CBookmark :
	public CBookmarkBase
{
public:
	virtual DBLENGTH GetSize() const throw()
	{
		return nSize;
	}
	virtual BYTE* GetBuffer() const throw()
	{
		return (BYTE*)m_rgBuffer;
	}

// Implementation
	BYTE m_rgBuffer[nSize];
};


// Size of 0 means that the memory for the bookmark will be allocated
// at run time.
template <>
class CBookmark<0> :
	public CBookmarkBase
{
public:
	CBookmark()
	{
		m_nSize = 0;
		m_pBuffer = NULL;
	}
	CBookmark(_In_ DBLENGTH nSize)
	{
		m_pBuffer = _ATL_NEW BYTE[nSize];
		m_nSize = (m_pBuffer == NULL) ? 0 : nSize;
	}
	virtual ~CBookmark()
	{
		delete [] m_pBuffer;
	}
	CBookmark& operator=(_In_ const CBookmark& bookmark) throw()
	{
		if(this!=&bookmark)
		{
			SetBookmark(bookmark.GetSize(), bookmark.GetBuffer());
		}
		return *this;
	}
	virtual DBLENGTH GetSize() const throw()
	{
		return m_nSize;
	}
	virtual BYTE* GetBuffer() const throw()
	{
		return m_pBuffer;
	}
	// Sets the bookmark to the passed value
	HRESULT SetBookmark(
		_In_ DBLENGTH nSize,
		_In_reads_(nSize) BYTE* pBuffer) throw()
	{
		ATLENSURE_RETURN(pBuffer != NULL);
		delete [] m_pBuffer;
		m_pBuffer = _ATL_NEW BYTE[nSize];
		if (m_pBuffer != NULL)
		{
			Checked::memcpy_s(m_pBuffer, nSize, pBuffer, nSize);
			m_nSize = nSize;
			return S_OK;
		}
		else
		{
			m_nSize = 0;
			return E_OUTOFMEMORY;
		}
	}
	DBLENGTH  m_nSize;
	BYTE*   m_pBuffer;
};


///////////////////////////////////////////////////////////////////////////
// class CAccessorBase

class CAccessorBase
{
public:
	CAccessorBase()
	{
		m_pAccessorInfo  = NULL;
		m_nAccessors     = 0;
		m_pBuffer        = NULL;
	}
	void Close() throw()
	{
		// If Close is called then ReleaseAccessors must have been
		// called first
		ATLASSUME(m_nAccessors == 0);
		ATLASSUME(m_pAccessorInfo == NULL);
	}
	// Get the number of accessors that have been created
	ULONG GetNumAccessors() const throw()
	{
		return m_nAccessors;
	}
	// Get the handle of the passed accessor (offset from 0)
	HACCESSOR GetHAccessor(_In_ ULONG nAccessor) const throw()
	{
		ATLENSURE_RETURN_VAL(nAccessor<m_nAccessors, 0);
		return m_pAccessorInfo[nAccessor].hAccessor;
	};
	// Called during Close to release the accessor information
	HRESULT ReleaseAccessors(_Inout_ IUnknown* pUnk) throw()
	{
		ATLENSURE_RETURN(pUnk != NULL);
		HRESULT hr = S_OK;
		if (m_nAccessors > 0)
		{
			CComPtr<IAccessor> spAccessor;
			hr = pUnk->QueryInterface(__uuidof(IAccessor), (void**)&spAccessor);
			if (SUCCEEDED(hr))
			{
				ATLENSURE_RETURN(m_pAccessorInfo != NULL);
				for (ULONG i = 0; i < m_nAccessors; i++)
					spAccessor->ReleaseAccessor(m_pAccessorInfo[i].hAccessor, NULL);
			}
			m_nAccessors = 0;
			delete [] m_pAccessorInfo;
			m_pAccessorInfo = NULL;
		}
		return hr;
	}
	// Returns true or false depending upon whether data should be
	// automatically retrieved for the passed accessor.
	bool IsAutoAccessor(_In_ ULONG nAccessor) const throw()
	{
		ATLENSURE_RETURN_VAL(nAccessor < m_nAccessors, false);
		ATLENSURE_RETURN_VAL(m_pAccessorInfo != NULL, false);
		return m_pAccessorInfo[nAccessor].bAutoAccessor;
	}

// Implementation
	// Used by the rowset class to find out where to place the data
	BYTE* GetBuffer() const throw()
	{
		return m_pBuffer;
	}
	// Set the buffer that is used to retrieve the data
	void SetBuffer(_In_opt_ BYTE* pBuffer) throw()
	{
		m_pBuffer = pBuffer;
	}

	bool NoBindOnNullRowset() const throw()
	{
		return false;
	}

	// Allocate internal memory for the passed number of accessors
	_At_(this->m_pAccessorInfo, _Post_writable_size_(nAccessors))
	_Post_satisfies_(this->m_nAccessors == nAccessors)
	HRESULT AllocateAccessorMemory(_In_ int nAccessors) throw()
	{
		// Can't be called twice without calling ReleaseAccessors first
		ATLASSUME(m_pAccessorInfo == NULL);
		m_pAccessorInfo = _ATL_NEW _ATL_ACCESSOR_INFO[nAccessors];
		if (m_pAccessorInfo == NULL)
			return E_OUTOFMEMORY;
		m_nAccessors    = nAccessors;
		return S_OK;
	}
	// BindParameters will be overridden if parameters are used
	HRESULT BindParameters(
		_In_opt_ HACCESSOR*,
		_In_opt_ ICommand*,
		_In_opt_ void**) throw()
	{
		return S_OK;
	}

	// Create an accessor for the passed binding information. The created accessor is
	// returned through the pHAccessor parameter.
	static HRESULT BindEntries(
		_In_reads_(nColumns) DBBINDING* pBindings,
		_In_ DBORDINAL nColumns,
		_Out_ HACCESSOR* pHAccessor,
		_In_ DBLENGTH nSize,
		_Inout_ IAccessor* pAccessor) throw()
	{
		ATLENSURE_RETURN(pBindings  != NULL);
		ATLENSURE_RETURN(pHAccessor != NULL);
		ATLENSURE_RETURN(pAccessor  != NULL);
		HRESULT hr;
		DBORDINAL i;
		DWORD dwAccessorFlags = (pBindings->eParamIO == DBPARAMIO_NOTPARAM) ?
			DBACCESSOR_ROWDATA : DBACCESSOR_PARAMETERDATA;

#ifdef _DEBUG
		// In debug builds we will retrieve the status flags and trace out
		// any errors that may occur.
		CAutoVectorPtr<DBBINDSTATUS> spStatus;
		spStatus.Allocate( nColumns );
		hr = pAccessor->CreateAccessor(dwAccessorFlags, nColumns,
			pBindings, nSize, pHAccessor, spStatus);
		if (FAILED(hr) && (DBBINDSTATUS*)spStatus)
		{
			for (i=0; i<nColumns; i++)
			{
				if (spStatus[i] != DBBINDSTATUS_OK)
					ATLTRACE(atlTraceDBClient, 0, _T("Binding entry %d failed. Status: %d\n"), i, spStatus[i]);
			}
		}
#else
		hr = pAccessor->CreateAccessor(dwAccessorFlags, nColumns,
			pBindings, nSize, pHAccessor, NULL);
#endif
		for (i=0; i<nColumns; i++)
			delete pBindings[i].pObject;

		return hr;
	}

	// Set up the binding structure pointed to by pBindings based upon
	// the other passed parameters.
	static void Bind(
		_Out_opt_ DBBINDING* pBinding,
		_In_ DBORDINAL nOrdinal,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nLength,
		_In_ BYTE nPrecision,
		_In_ BYTE nScale,
		_In_ DBPARAMIO eParamIO,
		_In_ DBBYTEOFFSET nDataOffset,
		_In_ DBBYTEOFFSET nLengthOffset = 0,
		_In_ DBBYTEOFFSET nStatusOffset = 0,
		_In_opt_ DBOBJECT* pdbobject = NULL)
	{
		ATLENSURE(pBinding != NULL);

		// If we are getting a pointer to the data then let the provider
		// own the memory
		if (wType & DBTYPE_BYREF)
			pBinding->dwMemOwner = DBMEMOWNER_PROVIDEROWNED;
		else
			pBinding->dwMemOwner = DBMEMOWNER_CLIENTOWNED;

		pBinding->pObject   = pdbobject;

		pBinding->eParamIO      = eParamIO;
		pBinding->iOrdinal      = nOrdinal;
		pBinding->wType         = wType;
		pBinding->bPrecision    = nPrecision;
		pBinding->bScale        = nScale;
		pBinding->dwFlags       = 0;

		pBinding->obValue       = nDataOffset;
		pBinding->obLength      = 0;
		pBinding->obStatus      = 0;
		pBinding->pTypeInfo     = NULL;
		pBinding->pBindExt      = NULL;
		pBinding->cbMaxLen      = nLength;

		pBinding->dwPart = DBPART_VALUE;
		if (nLengthOffset != 0)
		{
			pBinding->dwPart |= DBPART_LENGTH;
			pBinding->obLength = nLengthOffset;
		}
		if (nStatusOffset != 0)
		{
			pBinding->dwPart |= DBPART_STATUS;
			pBinding->obStatus = nStatusOffset;
		}
	}

	// Free memory if appropriate
	static inline void FreeType(
		_In_ DBTYPE wType,
		_In_opt_ BYTE* pValue,
		_Inout_opt_ IRowset* pRowset = NULL) throw()
	{
		if (pValue == NULL)
			return;

		if( wType & DBTYPE_ARRAY )
		{
			SAFEARRAY** ppSafeArray = (SAFEARRAY**)pValue;
			if( (*ppSafeArray) != NULL )
			{
				SafeArrayDestroy(*ppSafeArray);
				*ppSafeArray = NULL;
			}
		}
		else
		{
			switch (wType)
			{
				case DBTYPE_BSTR:
					SysFreeString(*((BSTR*)pValue));
					*((BSTR*)pValue) = NULL;
				break;
				case DBTYPE_VARIANT:
					VariantClear((VARIANT*)pValue);
				break;
				case DBTYPE_IUNKNOWN:
				case DBTYPE_IDISPATCH:
					if( (*(IUnknown**)pValue) != NULL )
					{
						(*(IUnknown**)pValue)->Release();
						*(IUnknown**)pValue = NULL;
					}
				break;

				case DBTYPE_HCHAPTER:
					CComQIPtr<IChapteredRowset> spChapteredRowset = pRowset;
					if (spChapteredRowset != NULL)
						spChapteredRowset->ReleaseChapter(*(HCHAPTER*)pValue, NULL);
				break;
			}
		}
		if ((wType & DBTYPE_VECTOR) && ~(wType & DBTYPE_BYREF))
			CoTaskMemFree(((DBVECTOR*)pValue)->ptr);
	}

	void FreeRecordMemory(_Inout_opt_ IRowset* /*pRowset*/) throw()
	{
	}

	_ATL_ACCESSOR_INFO* m_pAccessorInfo;
	ULONG               m_nAccessors;
	BYTE*               m_pBuffer;
};

// Used when you don't need any parameters or output columns
class CNoAccessor
{
public:
	// We don't need any typedef's here as the default
	// global typedef is not to have any parameters and
	// output columns.
	HRESULT BindColumns(_In_opt_ IUnknown*) throw()
	{
		return S_OK;
	}
	ATLPREFAST_SUPPRESS(6101)
	HRESULT BindParameters(
		_In_opt_ HACCESSOR*,
		_In_opt_ ICommand*,
		_Outptr_opt_ void**) throw()
	{
		return S_OK;
	}
	ATLPREFAST_UNSUPPRESS()
	void Close() throw()
	{
	}
	HRESULT ReleaseAccessors(_In_opt_ IUnknown*) throw()
	{
		return S_OK;
	}
	void FreeRecordMemory(_Inout_opt_ IRowset* /*pRowset*/) throw()
	{
	}
	void FreeRecordMemory(_In_ int /*nAccessor*/, _Inout_opt_ IRowset* /*pRowset*/) throw()
	{
	}
	HRESULT GetColumnInfo(
		_In_opt_ IRowset*,
		_In_opt_ DBORDINAL*,
		_In_opt_ DBCOLUMNINFO**) throw()
	{
		return E_FAIL;
	}
	ULONG GetNumAccessors() const throw()
	{
		return 0;
	}
	bool IsAutoAccessor(_In_ ULONG /*nAccessor*/) const throw()
	{
		return false;
	}
	HACCESSOR GetHAccessor(_In_ ULONG /*nAccessor*/) const throw()
	{
		return 0;
	}
	BYTE* GetBuffer() const throw()
	{
		ATLASSERT(FALSE);
		return NULL;
	}
	static void Bind(
		_In_opt_ DBBINDING*,
		_In_ DBORDINAL,
		_In_ DBTYPE,
		_In_ DBLENGTH,
		_In_ BYTE,
		_In_ BYTE,
		_In_ DBPARAMIO,
		_In_ DBBYTEOFFSET,
		_In_ DBBYTEOFFSET = 0,
		_In_ DBBYTEOFFSET = 0,
		_In_opt_ DBOBJECT* = NULL) throw()
	{
		ATLASSERT(FALSE);
	}
	bool NoBindOnNullRowset() const throw()
	{
		return false;
	}
};

// Used when a rowset will not be returned from the command
template <class TAccessor = CAccessorBase>
class CNoRowset
{
public:
	HRESULT BindFinished() throw()
	{
		return S_OK;
	}
	void Close() throw()
	{
	}
	static const IID& GetIID() throw()
	{
		return IID_NULL;
	}
	IRowset* GetInterface() const throw()
	{
		return NULL;
	}
	IRowset** GetInterfacePtr() throw()
	{
		return NULL;
	}
	void SetAccessor(_In_opt_ void*) throw()
	{
	}
	void SetupOptionalRowsetInterfaces() throw()
	{
	}
};

// Used with SQL Server 2000, a rowset will not be returned from the command, but instead
// we get an ISequentialStream object and use it to read the data.
template <class TAccessor = CAccessorBase>
class CStreamRowset
{
public:
	// Constructors & destructors
	CStreamRowset()
	{
		m_spStream = NULL;
	}

	~CStreamRowset()
	{
		Close();
	}

	// Methods
	void Close()
	{
		if (m_spStream != NULL)
			m_spStream.Release();
	}

	// Implementation
	static const IID& GetIID()
	{
		return __uuidof(ISequentialStream);
	}
	ISequentialStream* GetInterface() const
	{
		return m_spStream;
	}
	ISequentialStream** GetInterfacePtr()
	{
		return &m_spStream;
	}

	HRESULT BindFinished() throw()
	{
		return S_OK;
	}
	void SetAccessor(_In_opt_ void*) throw()
	{
	}
	void SetupOptionalRowsetInterfaces() throw()
	{
	}

	CComPtr<ISequentialStream> m_spStream;
};


///////////////////////////////////////////////////////////////////////////
// class CAccessor
// T is the class that contains the data that will be accessed.
ATLPREFAST_SUPPRESS(6387)
template <class T>
class CAccessor :
	public CAccessorBase,
	public T
{
public:
	typedef T DataClass;

// Implementation
	HRESULT ReleaseAccessors(_Inout_ IUnknown* pUnk) throw()
	{
		FreeRecordMemory( (IRowset*) pUnk );
		return CAccessorBase::ReleaseAccessors(pUnk);
	}

	// Free's any columns in the current record that need to be freed.
	// E.g. Calls SysFreeString on any BSTR's and Release on any interfaces.
	void FreeRecordMemory(
		_In_ int nAccessor,
		_In_opt_ IRowset* /* pRowset */) throw()
	{
		(nAccessor);
		DBORDINAL nColumns;

		// Passing in m_pBuffer tells the column entry maps to free the
		// memory for the types if appropriate

		/* Signature of called function:
		HRESULT _GetBindEntries(
			_In_opt_z_ LPOLESTR*,
			_In_opt_ DBORDINAL*,
			_In_opt_ DBBINDING*,
			_In_ ULONG,
			_In_opt_ bool*,
			_In_opt_ BYTE* pBuffer = NULL,
			_In_ bool bClearOnly = false) throw ()
		*/
		this->call_OutputColumnsClass_GetBindEntries(ATLImplementationDetails::high_priority_tag{}, nullptr, &nColumns, nullptr, nAccessor, nullptr, m_pBuffer);
	}
	void FreeRecordMemory(_Inout_opt_ IRowset* pRowset) throw()
	{
		for (ULONG i = 0; i < this->call_OutputColumnsClass_GetNumAccessors(ATLImplementationDetails::high_priority_tag{}); i++)
			FreeRecordMemory(i, pRowset);
	}

	HRESULT GetColumnInfo(
		_In_opt_ IRowset*,
		_In_opt_ DBORDINAL*,
		_Outptr_result_maybenull_ DBCOLUMNINFO** ppColumnInfo) throw()
	{
		ATLASSERT(ppColumnInfo != NULL);
		*ppColumnInfo = NULL;

		return E_FAIL;
	}

	void ClearRecordMemory() throw()
	{
		for (ULONG i = 0; i < this->call_OutputColumnsClass_GetNumAccessors(ATLImplementationDetails::high_priority_tag{}); i++)
		{
			DBORDINAL nColumns;
		/* Signature of called function:
		HRESULT _GetBindEntries(
			_In_opt_z_ LPOLESTR*,
			_In_opt_ DBORDINAL*,
			_In_opt_ DBBINDING*,
			_In_ ULONG,
			_In_opt_ bool*,
			_In_opt_ BYTE* pBuffer = NULL,
			_In_ bool bClearOnly = false) throw ()
		*/
			this->call_OutputColumnsClass_GetBindEntries(ATLImplementationDetails::high_priority_tag{}, nullptr, &nColumns, nullptr, i, nullptr, m_pBuffer, true);
		}
	}

	HRESULT BindColumns(_Inout_ IUnknown* pUnk) throw()
	{
		HRESULT hr;
		ULONG   nAccessors;
		DBLENGTH  nSize;
		nAccessors = this->call_OutputColumnsClass_GetNumAccessors(ATLImplementationDetails::high_priority_tag{});

		SetBuffer((BYTE*)(T*)this);
		ClearRecordMemory();

		nSize = sizeof(T);
		hr = BindAccessors(nAccessors, nSize, pUnk);
		return hr;
	}
	HRESULT BindAccessors(
		_In_ ULONG nAccessors,
		_In_ DBLENGTH nSize,
		_Inout_ IUnknown* pUnk) throw()
	{
		ATLENSURE_RETURN(pUnk != NULL);
		HRESULT hr;

		CComPtr<IAccessor> spAccessor;
		hr = pUnk->QueryInterface(&spAccessor);
		if (SUCCEEDED(hr))
		{
			// Allocate the accessor memory if we haven't done so yet
			if (m_pAccessorInfo == NULL)
			{
				hr = AllocateAccessorMemory(nAccessors);
				if (FAILED(hr))
					return hr;
			}

			for (ULONG i=0; i<nAccessors && SUCCEEDED(hr); i++)
				hr = BindAccessor(spAccessor, i, nSize);
		}

		return hr;
	}

	HRESULT BindAccessor(
		_Inout_ IAccessor* pAccessor,
		_In_ ULONG nAccessor,
		_In_ DBLENGTH nSize) throw()
	{
		CAutoVectorPtr<DBBINDING>	spBindings;
		CAutoVectorPtr<LPOLESTR>	spColumnNames;
		DBORDINAL					nColumns = 0;
		bool						bAuto = false;
		HRESULT						hr;
		CComHeapPtr<DBCOLUMNINFO>	spColumnInfo;
		DBORDINAL					nColumnInfoCount = 0;
		CComHeapPtr<OLECHAR>		spStringsBuffer;

		// First time just get the number of entries by passing in &nColumns
		/* Signature of called function:
		HRESULT _GetBindEntries(
			_In_opt_z_ LPOLESTR*,
			_In_opt_ DBORDINAL*,
			_In_opt_ DBBINDING*,
			_In_ ULONG,
			_In_opt_ bool*,
			_In_opt_ BYTE* pBuffer = NULL,
			_In_ bool bClearOnly = false) throw ()
		*/
		hr = this->call_OutputColumnsClass_GetBindEntries(ATLImplementationDetails::high_priority_tag{}, nullptr, &nColumns, nullptr, nAccessor, nullptr);
		if (FAILED(hr))
			return hr;

		ATLASSERT(nColumns > 0);

		// Allocate the binding structures
		if( !spBindings.Allocate(nColumns) )
			return E_OUTOFMEMORY;

		for( ULONG i = 0; i < nColumns; i++ )
			spBindings[i].pObject = NULL;

		// Allocate the column names strings array
		if( !spColumnNames.Allocate(nColumns) )
			return E_OUTOFMEMORY;

		// Now get the bind entries
		hr = this->call_OutputColumnsClass_GetBindEntries(ATLImplementationDetails::high_priority_tag{}, spColumnNames, &nColumns, spBindings, nAccessor, &bAuto);
		if (SUCCEEDED(hr))
		{
			// translate the columns names to oridinals if necessary
			for( ULONG i = 0; i < nColumns; i++ )
				if( spColumnNames[i] != NULL ) // if a column name was given, translate it to oridinal
				{
					// if necessary, get the column information
					if( spColumnInfo == NULL )
					{
						if( FAILED( GetColumnNames( pAccessor, &spColumnInfo, &spStringsBuffer, &nColumnInfoCount ) ) )
						{
							ATLASSERT( FALSE ); // unable to get columns information
							return E_FAIL;
						}
					}
					if( ! GetOridinalColumnNo(spColumnNames[i], spBindings[i].iOrdinal, spColumnInfo, nColumnInfoCount ) )
					{
						ATLASSERT(FALSE); // unable to match column name to an oridinal !!!
						return E_FAIL;
					}
				}

			m_pAccessorInfo[nAccessor].bAutoAccessor = bAuto;
			hr = BindEntries(spBindings, nColumns, &m_pAccessorInfo[nAccessor].hAccessor, nSize, pAccessor);
		}
		else
		{
ATLPREFAST_SUPPRESS(6102)
			// free any DBBINDING::pObject's
			for( ULONG i = 0; i < nColumns; i++ )
				delete spBindings[i].pObject;
ATLPREFAST_UNSUPPRESS()

		}
		return hr;
	}

	HRESULT BindParameters(
		_In_ HACCESSOR* pHAccessor,
		_Inout_ ICommand* pCommand,
		_Outptr_ void** ppParameterBuffer) throw()
	{
		HRESULT				hr = S_OK;
		ATLENSURE_RETURN(ppParameterBuffer);

		// In the static accessor case, the parameter buffer will be T plus the size of CAccessorBase
		*ppParameterBuffer = static_cast<T*>(this);

		// Only bind the parameters if we haven't already done it
		if (*pHAccessor == NULL)
		{
			DBORDINAL					nColumns = 0;
			CAutoVectorPtr<DBBINDING>	spBinding;
			CAutoVectorPtr<LPOLESTR>	spColumnNames;

			CComHeapPtr<DBPARAMINFO>	spParameterInfo;
			DB_UPARAMS					nParameterInfoCount = 0;
			CComHeapPtr<OLECHAR>		spStringsBuffer;

			/* Signature of called function:
			static HRESULT _GetParamEntries(
				_In_opt_z_ LPOLESTR*,
				_In_opt_ DBORDINAL*,
				_In_opt_ DBBINDING*,
				_In_opt_ BYTE* pBuffer = NULL,
				_In_ bool bClearOnly = false) throw ()
			*/
			this->call_ParamClass_GetParamEntries(ATLImplementationDetails::high_priority_tag{}, nullptr, &nColumns, nullptr);

			// Allocate the DBPARAMINFO array
			if( !spBinding.Allocate(nColumns) )
				return E_OUTOFMEMORY;

			// Allocate the column names strings array
			if( !spColumnNames.Allocate(nColumns) )
				return E_OUTOFMEMORY;

			this->call_ParamClass_GetParamEntries(ATLImplementationDetails::high_priority_tag{}, spColumnNames, &nColumns, spBinding);
			if (SUCCEEDED(hr))
			{
				for( ULONG i = 0; i < nColumns; i++ )
				{
					// For bindings in parameter accessors, consumer-owned memory must always be used.
					spBinding[i].dwMemOwner = DBMEMOWNER_CLIENTOWNED;

					// translate the columns names to oridinals if necessary
					if( spColumnNames[i] != NULL ) // if a column name was given, translate it to oridinal
					{
						// if necessary, get the column information
						if( spParameterInfo == NULL )
						{
							if( FAILED( GetParameterNames( pCommand, &spParameterInfo, &spStringsBuffer, &nParameterInfoCount ) ) )
							{
								ATLASSERT( FALSE ); // unable to get columns information
								return E_FAIL;
							}
						}
						if( ! GetOridinalParameterNo(spColumnNames[i], spBinding[i].iOrdinal, spParameterInfo, nParameterInfoCount ) )
						{
							ATLASSERT(FALSE); // unable to match parameter name to an oridinal !!!
							return E_FAIL;
						}
					}
				}

				// Get the IAccessor from the passed ICommand
				ATLENSURE_RETURN(pCommand != NULL);
				CComPtr<IAccessor> spAccessor;
				hr = pCommand->QueryInterface(&spAccessor);
				if (SUCCEEDED(hr))
				{
					hr = BindEntries(spBinding, nColumns, pHAccessor, sizeof(T), spAccessor);
				}
			}
		}
		return hr;
	}

protected:
	_Success_(return != false)
	bool GetOridinalColumnNo(
		_In_z_ LPCOLESTR pOleColumnName,
		_Out_ DBORDINAL& nColumn,
		_In_reads_(nColumns) DBCOLUMNINFO* pColumnInfo,
		_In_ DBORDINAL nColumns) throw()
	{
		ATLASSERT(pOleColumnName != NULL);
		ATLASSERT(pColumnInfo != NULL);

		if( pOleColumnName == NULL || pColumnInfo == NULL )
			return false;

		// Search through the columns trying to find a match
		for (ULONG i = 0; i < nColumns; i++)
		{
			if (pColumnInfo[i].pwszName != NULL && wcscmp(pColumnInfo[i].pwszName, pOleColumnName) == 0)
			{
				nColumn = pColumnInfo[i].iOrdinal;
				return true;
			}
		}
		return false;   // Not Found
	}

	HRESULT GetColumnNames(
		_Inout_ IAccessor* pAccessor,
		_Outptr_result_buffer_maybenull_(*pnColumns) DBCOLUMNINFO** ppColumnInfo,
		_Outptr_result_maybenull_z_ OLECHAR** ppStringsBuffer,
		_Out_ DBORDINAL* pnColumns) throw()
	{
		ATLASSERT( ppColumnInfo != NULL );
		ATLASSERT( ppStringsBuffer != NULL );
		ATLASSERT( pnColumns != NULL );
		ATLASSERT( pAccessor != NULL );

		if( ppColumnInfo == NULL || ppStringsBuffer == NULL || pnColumns == NULL || pAccessor == NULL )
			return E_FAIL;

		CComPtr<IColumnsInfo> spColumnsInfo;
		HRESULT hr = pAccessor->QueryInterface(&spColumnsInfo);
		if (FAILED(hr))
			return hr;

		hr = spColumnsInfo->GetColumnInfo(pnColumns, ppColumnInfo, ppStringsBuffer);
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

	_Success_(return != false)
	bool GetOridinalParameterNo(
		_In_z_ LPCOLESTR pOleParameterName,
		_Out_ DB_UPARAMS& nParameter,
		_In_reads_(nParameters) DBPARAMINFO* pParameterInfo,
		_In_ DB_UPARAMS nParameters) throw()
	{
		ATLASSERT(pOleParameterName != NULL);
		ATLASSERT(pParameterInfo != NULL);

		if( pOleParameterName == NULL || pParameterInfo == NULL )
			return false;

		// Search through the columns trying to find a match
		for (ULONG i = 0; i < nParameters; i++)
		{
			if (pParameterInfo[i].pwszName != NULL && wcscmp(pParameterInfo[i].pwszName, pOleParameterName) == 0)
			{
				nParameter = pParameterInfo[i].iOrdinal;
				return true;
			}
		}
		return false;   // Not Found
	}

	HRESULT GetParameterNames(
		_Inout_ ICommand* pCmd,
		_Outptr_result_maybenull_ DBPARAMINFO** ppParameterInfo,
		_Outptr_result_z_ OLECHAR** ppStringsBuffer,
		_Out_ DB_UPARAMS* pnParameters) throw()
	{
		ATLASSERT( ppParameterInfo != NULL );
		ATLASSERT( ppStringsBuffer != NULL );
		ATLASSERT( pnParameters != NULL );
		ATLASSERT( pCmd != NULL );

		if( ppParameterInfo == NULL || ppStringsBuffer == NULL || pnParameters == NULL || pCmd == NULL )
			return E_FAIL;

		CComPtr<ICommandWithParameters> spCommandWithParameters;
		HRESULT hr = pCmd->QueryInterface(&spCommandWithParameters);
		if (FAILED(hr))
			return hr;

		hr = spCommandWithParameters->GetParameterInfo(pnParameters, ppParameterInfo, ppStringsBuffer);
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

private:
	template<class Ty = CAccessor, class U = typename Ty::_OutputColumnsClass>
	decltype(auto) call_OutputColumnsClass_GetNumAccessors(ATLImplementationDetails::high_priority_tag)
	{
		return U::_GetNumAccessors();
	}

	decltype(auto) call_OutputColumnsClass_GetNumAccessors(ATLImplementationDetails::low_priority_tag) const
	{
		return ::ATL::_OutputColumnsClass::_GetNumAccessors();
	}

	template<class Ty = CAccessor, class U = typename Ty::_OutputColumnsClass, class... Args>
	decltype(auto) call_OutputColumnsClass_GetBindEntries(ATLImplementationDetails::high_priority_tag, Args&&... args)
	{
		return U::_GetBindEntries(::std::forward<Args>(args)...);
	}

	template<class... Args>
	decltype(auto) call_OutputColumnsClass_GetBindEntries(ATLImplementationDetails::low_priority_tag, Args&&... args) const
	{
		return ::ATL::_OutputColumnsClass::_GetBindEntries(::std::forward<Args>(args)...);
	}

	template<class Ty = CAccessor, class U = typename Ty::_ParamClass, class... Args>
	decltype(auto) call_ParamClass_GetParamEntries(ATLImplementationDetails::high_priority_tag, Args&&... args)
	{
		return U::_GetParamEntries(::std::forward<Args>(args)...);
	}

	template<class... Args>
	decltype(auto) call_ParamClass_GetParamEntries(ATLImplementationDetails::low_priority_tag, Args&&... args) const
	{
		return ::ATL::_ParamClass::_GetParamEntries(::std::forward<Args>(args)...);
	}
};
ATLPREFAST_UNSUPPRESS()

enum DBBLOBHANDLINGENUM
{
	DBBLOBHANDLING_DEFAULT,
	DBBLOBHANDLING_NOSTREAMS,
	DBBLOBHANDLING_SKIP
};

///////////////////////////////////////////////////////////////////////////
// CDynamicAccessor
class CDynamicAccessor :
	public CAccessorBase
{
public:
	CDynamicAccessor(
		_In_ DBBLOBHANDLINGENUM eBlobHandling = DBBLOBHANDLING_DEFAULT,
		_In_ DBLENGTH nBlobSize = 8000)
	{
		ATLASSERT( eBlobHandling == DBBLOBHANDLING_DEFAULT ||
				   eBlobHandling == DBBLOBHANDLING_NOSTREAMS ||
				   eBlobHandling == DBBLOBHANDLING_SKIP );

		m_nColumns        = 0;
		m_pColumnInfo     = NULL;
		m_pStringsBuffer  = NULL;
		m_eBlobHandling   = eBlobHandling;
		m_nBlobSize       = nBlobSize;
		m_pfClientOwnedMemRef = NULL;
	};

	~CDynamicAccessor()
	{
		Close();
	}

	HRESULT ReleaseAccessors(_Inout_ IUnknown* pUnk) throw()
	{
		FreeRecordMemory( (IRowset*) pUnk );
		return CAccessorBase::ReleaseAccessors(pUnk);
	}

	bool SetBlobHandling(_In_ DBBLOBHANDLINGENUM eBlobHandling)
	{
		switch( eBlobHandling )
		{
		case DBBLOBHANDLING_DEFAULT:
		case DBBLOBHANDLING_NOSTREAMS:
		case DBBLOBHANDLING_SKIP:
			m_eBlobHandling = eBlobHandling;
			return true;
		default:
			ATLASSERT( FALSE ); // invalid blob handling mode!!!
			return false;
		}
	}

	const DBBLOBHANDLINGENUM GetBlobHandling() const
	{
		return m_eBlobHandling;
	}

	void SetBlobSizeLimit(_In_ DBLENGTH nBlobSize)
	{
		m_nBlobSize = nBlobSize;
	}

	const DBLENGTH GetBlobSizeLimit() const
	{
		return m_nBlobSize;
	}

//	void FreeRecordMemory(int , IRowset* ) throw() {}

	void Close() throw()
	{
		CoTaskMemFree(m_pColumnInfo);
		m_pColumnInfo = NULL;

		// Free the memory for the string buffer returned by IColumnsInfo::GetColumnInfo,
		// if necessary
		CoTaskMemFree(m_pStringsBuffer);
		m_pStringsBuffer = NULL;

		delete [] m_pBuffer;
		m_pBuffer = NULL;

		delete [] m_pfClientOwnedMemRef;
		m_pfClientOwnedMemRef = NULL;

		m_nColumns = 0;

		CAccessorBase::Close();
	}

	_Success_(return != false) bool GetColumnType(
		_In_ DBORDINAL nColumn,
		_Out_ DBTYPE* pType) const throw()
	{
		if (TranslateColumnNo(nColumn))
		{
			ATLENSURE_RETURN_VAL(pType, false);
			*pType = m_pColumnInfo[nColumn].wType;
			return true;
		}
		else
			return false;
	}

	_Success_(return != false) bool GetColumnFlags(
		_In_ DBORDINAL nColumn,
		_Out_ DBCOLUMNFLAGS* pFlags) const throw()
	{
		if (TranslateColumnNo(nColumn))
		{
			ATLENSURE_RETURN_VAL(pFlags, false);
			*pFlags = m_pColumnInfo[nColumn].dwFlags;
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetOrdinal(
		_In_z_ const CHAR* pColumnName,
		_Out_ DBORDINAL* pOrdinal) const throw()
	{
		ATLENSURE_RETURN_VAL(pColumnName != NULL, false);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			ATLENSURE_RETURN_VAL(pOrdinal, false);
			*pOrdinal = m_pColumnInfo[nColumn].iOrdinal;
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetOrdinal(
		_In_z_ const WCHAR* pColumnName,
		_Out_ DBORDINAL* pOrdinal) const throw()
	{
		ATLENSURE_RETURN_VAL(pColumnName != NULL, false);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			ATLENSURE_RETURN_VAL(pOrdinal, false);
			*pOrdinal = m_pColumnInfo[nColumn].iOrdinal;
			return true;
		}
		else
			return false;
	}

	void* GetValue(_In_ DBORDINAL nColumn) const throw()
	{
		if (TranslateColumnNo(nColumn))
			return _GetDataPtr(nColumn);
		else
			return NULL;
	}

	void* GetValue(_In_z_ const CHAR* pColumnName) const throw()
	{
		ATLENSURE_RETURN_VAL(pColumnName != NULL, NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
			return _GetDataPtr(nColumn);
		else
			return NULL;    // Not Found
	}

	void* GetValue(_In_z_ const WCHAR* pColumnName) const throw()
	{
		ATLENSURE_RETURN_VAL(pColumnName != NULL, NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
			return _GetDataPtr(nColumn);
		else
			return NULL;    // Not Found
	}

	template <class ctype>
	void _GetValue(
		_In_ DBORDINAL nColumn,
		_Out_ ctype* pData) const throw()
	{
		ATLASSUME(pData != NULL);
		ATLASSUME(m_pColumnInfo[nColumn].ulColumnSize == sizeof(ctype));
		ctype* pBuffer = (ctype*)_GetDataPtr(nColumn);
		*pData = *pBuffer;
	}
	template <class ctype>
	void _SetValue(
		_In_ DBORDINAL nColumn,
		_In_ const ctype& data) throw()
	{
		ATLASSUME(m_pColumnInfo[nColumn].ulColumnSize == sizeof(ctype));
		ctype* pBuffer = (ctype*)_GetDataPtr(nColumn);
		*pBuffer = (ctype)data;
	}
	ATLPREFAST_SUPPRESS(6101)
	template <class ctype>
	_Success_(return != false) bool GetValue(
		_In_ DBORDINAL nColumn,
		_Out_ ctype* pData) const throw()
	{
		ATLENSURE_RETURN_VAL(pData, false);
		if (TranslateColumnNo(nColumn))
		{
			_GetValue(nColumn, pData);
			return true;
		}
		return false;
	}
	template <class ctype>
	bool SetValue(
		_In_ DBORDINAL nColumn,
		_In_ const ctype& data) throw()
	{
		if (TranslateColumnNo(nColumn))
		{
			_SetValue(nColumn, data);
			return true;
		}
		return false;
	}
	template <class ctype>
	_Success_(return != false) bool GetValue(
		_In_z_ const CHAR *pColumnName,
		_Out_ ctype* pData) const throw()
	{
		ATLENSURE_RETURN_VAL(pColumnName != NULL, false);
		ATLENSURE_RETURN_VAL(pData, false);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			_GetValue(nColumn, pData);
			return true;
		}
		return false;
	}
	template <class ctype>
	_Success_(return != false) bool GetValue(
		_In_z_ const WCHAR *pColumnName,
		_Out_ ctype* pData) const throw()
	{
		ATLENSURE_RETURN_VAL(pColumnName != NULL, false);
		ATLENSURE_RETURN_VAL(pData, false);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			_GetValue(nColumn, pData);
			return true;
		}
		return false;
	}
	ATLPREFAST_UNSUPPRESS()
	template <class ctype>
	bool SetValue(
		_In_z_ const CHAR *pColumnName,
		_In_ const ctype& data) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			_SetValue(nColumn, data);
			return true;
		}
		return false;
	}
	template <class ctype>
	bool SetValue(
		_In_z_ const WCHAR *pColumnName,
		_In_ const ctype& data) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			_SetValue(nColumn, data);
			return true;
		}
		return false;
	}
	_Success_(return != false) bool GetLength(
		_In_ DBORDINAL nColumn,
		_Out_ DBLENGTH* pLength) const
	{
		ATLENSURE(pLength != NULL);
		if (TranslateColumnNo(nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			*pLength = *(DBLENGTH*)( m_pBuffer + nOffset );
			return true;
		}
		else
			return false;
	}
	bool SetLength(
		_In_ DBORDINAL nColumn,
		_In_ DBLENGTH nLength) throw()
	{
		if (TranslateColumnNo(nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			*(DBLENGTH*)( m_pBuffer + nOffset ) = nLength;
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetLength(
		_In_z_ const CHAR* pColumnName,
		_Out_ DBLENGTH* pLength) const
	{
		ATLENSURE(pColumnName != NULL);
		ATLENSURE(pLength != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			*pLength = *(DBLENGTH*)( m_pBuffer + nOffset );
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetLength(
		_In_z_ const WCHAR* pColumnName,
		_Out_ DBLENGTH* pLength) const
	{
		ATLENSURE(pColumnName != NULL);
		ATLENSURE(pLength != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			*pLength = *(DBLENGTH*)( m_pBuffer + nOffset );
			return true;
		}
		else
			return false;
	}
	bool SetLength(
		_In_z_ const CHAR* pColumnName,
		_In_ DBLENGTH nLength) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			*(DBLENGTH*)( m_pBuffer + nOffset ) = nLength;
			return true;
		}
		else
			return false;
	}
	bool SetLength(
		_In_z_ const WCHAR* pColumnName,
		_In_ DBLENGTH nLength) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			*(DBLENGTH*)( m_pBuffer + nOffset ) = nLength;
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetStatus(
		_In_ DBORDINAL nColumn,
		_Out_ DBSTATUS* pStatus) const
	{
		ATLENSURE(pStatus != NULL);
		if (TranslateColumnNo(nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
			*pStatus = *(DBSTATUS*)( m_pBuffer + nOffset );
			return true;
		}
		else
			return false;
	}
	bool SetStatus(
		_In_ DBORDINAL nColumn,
		_In_ DBSTATUS status) throw()
	{
		if (TranslateColumnNo(nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
			*(DBSTATUS*)( m_pBuffer + nOffset ) = status;
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetStatus(
		_In_z_ const CHAR* pColumnName,
		_Out_ DBSTATUS* pStatus) const
	{
		ATLENSURE(pColumnName != NULL);
		ATLENSURE(pStatus != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
			*pStatus = *(DBSTATUS*)( m_pBuffer + nOffset );
			return true;
		}
		else
			return false;
	}
	_Success_(return != false) bool GetStatus(
		_In_z_ const WCHAR* pColumnName,
		_Out_ DBSTATUS* pStatus) const
	{
		ATLENSURE(pColumnName != NULL);
		ATLENSURE(pStatus != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
			*pStatus = *(DBSTATUS*)( m_pBuffer + nOffset );
			return true;
		}
		else
			return false;
	}
	bool SetStatus(
		_In_z_ const CHAR* pColumnName,
		_In_ DBSTATUS status) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
			*(DBSTATUS*)( m_pBuffer + nOffset ) = status;
			return true;
		}
		else
			return false;
	}
	bool SetStatus(
		_In_z_ const WCHAR* pColumnName,
		_In_ DBSTATUS status) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
			IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
			IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
			*(DBSTATUS*)( m_pBuffer + nOffset ) = status;
			return true;
		}
		else
			return false;
	}

	// Returns true if a bookmark is available
	HRESULT GetBookmark(_Inout_ CBookmark<>* pBookmark) const throw()
	{
		ATLENSURE_RETURN(pBookmark);
		HRESULT hr;
		if (m_pColumnInfo->iOrdinal == 0)
			hr = pBookmark->SetBookmark(m_pColumnInfo->ulColumnSize, (BYTE*)_GetDataPtr(0));
		else
			hr = E_FAIL;
		return hr;
	}

	DBORDINAL GetColumnCount() const throw()
	{
		return m_nColumns;
	}

	LPOLESTR GetColumnName(_In_ DBORDINAL nColumn) const throw()
	{
		if (TranslateColumnNo(nColumn))
			return m_pColumnInfo[nColumn].pwszName;
		else
			return NULL;
	}

	ATL_DEPRECATED("CDynamicAccessor::GetColumnInfo must be passed an array of string buffers too.")
	HRESULT GetColumnInfo(
		_Inout_ IRowset* pRowset,
		_In_opt_ DBORDINAL* pColumns,
		_Outptr_opt_ DBCOLUMNINFO** ppColumnInfo);

ATLPREFAST_SUPPRESS(6387)
	HRESULT GetColumnInfo(
		_Inout_ IRowset* pRowset,
		_Out_ DBORDINAL* pColumns,
		_Outptr_result_buffer_maybenull_(*pColumns) DBCOLUMNINFO** ppColumnInfo,
		_Outptr_result_maybenull_z_ OLECHAR** ppStringsBuffer) throw()
	{
		CComPtr<IColumnsInfo> spColumnsInfo;
		HRESULT hr = pRowset->QueryInterface(&spColumnsInfo);
		if (SUCCEEDED(hr))
			hr = spColumnsInfo->GetColumnInfo(pColumns, ppColumnInfo, ppStringsBuffer);

		return hr;
	}
ATLPREFAST_UNSUPPRESS()

	HRESULT AddBindEntry(_In_ const DBCOLUMNINFO& info) throw()
	{
		if ((m_nColumns + 1 < m_nColumns))
		{
			return E_OUTOFMEMORY;
		}
		DBCOLUMNINFO* pTempInfo = (DBCOLUMNINFO*)::ATL::AtlCoTaskMemRecalloc(m_pColumnInfo, (m_nColumns + 1), sizeof(DBCOLUMNINFO));
		if (pTempInfo == NULL)
			return E_OUTOFMEMORY;
		m_pColumnInfo = pTempInfo;
		m_pColumnInfo[m_nColumns] = info;
		m_nColumns++;

		return S_OK;
	}

	bool NoBindOnNullRowset() const throw()
	{
		return true;
	}

// Implementation
	// Free's any columns in the current record that need to be freed.
	// E.g. Calls SysFreeString on any BSTR's and Release on any interfaces.
	void FreeRecordMemory(_Inout_ IRowset* pRowset) throw()
	{
		ULONG i;

		for (i = 0; i < m_nColumns; i++)
		{
			if( m_pfClientOwnedMemRef != NULL && m_pfClientOwnedMemRef[i] == true )
			{
				DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[i].pTypeInfo;
				IncrementAndAlignOffset( nOffset, m_pColumnInfo[i].ulColumnSize, __alignof(DBLENGTH) );
				IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
				if( *(DBSTATUS*)( m_pBuffer + nOffset ) != DBSTATUS_S_ISNULL )
				{
					void* pRef = _GetDataPtr(i);

					if( pRef != NULL && *(void**)pRef != NULL )
					{
						CoTaskMemFree( *(void**)_GetDataPtr(i) );
						*(void**)pRef = NULL;
					}
				}
			}
			else if( m_pColumnInfo[i].wType == DBTYPE_IUNKNOWN || m_pColumnInfo[i].wType == DBTYPE_IDISPATCH )
			{
				DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[i].pTypeInfo;
				IncrementAndAlignOffset( nOffset, m_pColumnInfo[i].ulColumnSize, __alignof(DBLENGTH) );
				IncrementAndAlignOffset( nOffset, sizeof(DBLENGTH), __alignof(DBSTATUS) );
				if( *(DBSTATUS*)( m_pBuffer + nOffset ) == DBSTATUS_S_OK )
					CAccessorBase::FreeType(m_pColumnInfo[i].wType, (BYTE*)_GetDataPtr(i), pRowset);
			}
			else
			{
				CAccessorBase::FreeType(m_pColumnInfo[i].wType, (BYTE*)_GetDataPtr(i), pRowset);
			}
		}
	}
	void ClearRecordMemory() throw()
	{
		for (ULONG i = 0; i < m_nColumns; i++)
		{
#pragma warning(suppress : 28313) // The C28313 warning associated with the following line is spurious.
			memset(_GetDataPtr(i), 0, m_pColumnInfo[i].ulColumnSize);
		}
	}

	void* _GetDataPtr(_In_ DBORDINAL nColumn) const throw()
	{
		return m_pBuffer + (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
	}
	_Success_(return != false) bool GetInternalColumnNo(
		_In_z_ const CHAR* pColumnName,
		_Out_ DBORDINAL* pColumn) const
	{
		ATLENSURE(pColumnName != NULL);
		ATLENSURE(pColumn != NULL);
		ULONG       i;
		size_t      nSize = (strlen(pColumnName) + 1) * sizeof(OLECHAR);
		CComBSTR	bstrColumnName(pColumnName);

		// Search through the columns trying to find a match
		for (i = 0; i < m_nColumns; i++)
		{
			ATLASSUME(m_pColumnInfo != NULL);
			if (m_pColumnInfo[i].pwszName != NULL &&
				memcmp(m_pColumnInfo[i].pwszName, bstrColumnName.m_str, nSize) == 0)
			{
				break;
			}
		}
		if (i < m_nColumns && pColumn)
		{
			*pColumn = i;
			return true;
		}
		else
		{
			return false;   // Not Found
		}
	}
	_Success_(return != false) bool GetInternalColumnNo(
		_In_z_ const WCHAR* pColumnName,
		_Out_ DBORDINAL* pColumn) const
	{
		ATLENSURE(pColumnName != NULL);
		ATLASSERT(pColumn != NULL);
		ULONG       i;
		size_t      nSize = (wcslen(pColumnName) + 1) * sizeof(OLECHAR);
		CComBSTR	bstrColumnName(pColumnName);

		// Search through the columns trying to find a match
		for (i = 0; i < m_nColumns; i++)
		{
			ATLASSUME(m_pColumnInfo != NULL);
			if (m_pColumnInfo[i].pwszName != NULL &&
				memcmp(m_pColumnInfo[i].pwszName, bstrColumnName.m_str, nSize) == 0)
				break;
		}

		if (i < m_nColumns && pColumn)
		{
			*pColumn = i;
			return true;
		}

		return false;   // Not Found
	}

	// Set up the binding structure pointed to by pBindings based upon
	// the other passed parameters.
	static void BindEx(
		_Out_ DBBINDING* pBinding,
		_In_ DBORDINAL nOrdinal,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nLength,
		_In_ BYTE nPrecision,
		_In_ BYTE nScale,
		_In_ DBPARAMIO eParamIO,
		_In_ DBBYTEOFFSET nDataOffset,
		_In_ DBBYTEOFFSET nLengthOffset,
		_In_ DBBYTEOFFSET nStatusOffset,
		_In_opt_ DBOBJECT* pdbobject,
		_In_ DBMEMOWNER dwMemOwner,
		_In_ bool fSkipData = false )
	{
		ATLENSURE(pBinding != NULL);

		pBinding->dwMemOwner    = dwMemOwner;

		pBinding->pObject       = pdbobject;

		pBinding->eParamIO      = eParamIO;
		pBinding->iOrdinal      = nOrdinal;
		pBinding->wType         = wType;
		pBinding->bPrecision    = nPrecision;
		pBinding->bScale        = nScale;
		pBinding->dwFlags       = 0;

		pBinding->obValue       = 0;
		pBinding->obLength      = 0;
		pBinding->obStatus      = 0;
		pBinding->pTypeInfo     = NULL;
		pBinding->pBindExt      = NULL;
		pBinding->cbMaxLen      = nLength;

		pBinding->dwPart = 0;

		if ( ! fSkipData ) // skip column data
		{
			pBinding->dwPart |= DBPART_VALUE;
			pBinding->obValue = nDataOffset;
		}
		if (nLengthOffset != 0) // skip length
		{
			pBinding->dwPart |= DBPART_LENGTH;
			pBinding->obLength = nLengthOffset;
		}
		if (nStatusOffset != 0) // skip status
		{
			pBinding->dwPart |= DBPART_STATUS;
			pBinding->obStatus = nStatusOffset;
		}
	}

	HRESULT GetRowsetProperties(
		_Inout_ IUnknown* pUnk,
		_In_ DBPROPID* prgPropertyIDs,
		_Out_writes_(nPropCount) BOOL* pbValues,
		_In_ ULONG nPropCount) throw()
	{
		ULONG t;

		ATLENSURE_RETURN(pUnk != NULL);
		ATLENSURE_RETURN(pbValues != NULL);
		ATLASSERT(prgPropertyIDs != NULL);
		ATLASSERT(nPropCount > 0);

		CComPtr<IRowsetInfo> spRowsetInfo;
		HRESULT hr = pUnk->QueryInterface(&spRowsetInfo);

		for( t = 0; t < nPropCount; t++ )
			pbValues[t] = FALSE;

		if (FAILED(hr))
			ATLASSERT(0); // unable to retrieve IRowsetInfo interface
		else
		{
			DBPROPIDSET            rgPropertyIDSets[1];
			ULONG                  cPropSets        = 0;
			CComHeapPtr<DBPROPSET> rgPropSets;

			// Set up the Property ID Set.
			rgPropertyIDSets[0].rgPropertyIDs       = prgPropertyIDs;
			rgPropertyIDSets[0].cPropertyIDs        = nPropCount;
			rgPropertyIDSets[0].guidPropertySet     = DBPROPSET_ROWSET;

			hr = spRowsetInfo->GetProperties(	1,					// cPropertyIDSets
												rgPropertyIDSets,	// rgPropertyIDSets
												&cPropSets,         // pcPropSets
												&rgPropSets );      // prgPropSets

			if( SUCCEEDED(hr) )
			{
				for( t = 0; t < __min(rgPropSets[0].cProperties, nPropCount); t++ )
				{
					_Analysis_assume_(t < nPropCount);
					pbValues[t] = V_BOOL(&rgPropSets[0].rgProperties[t].vValue);
				}

				if( rgPropSets[0].rgProperties )
					CoTaskMemFree(rgPropSets[0].rgProperties);
			}
		}

		return hr;
	}

	ATLPREFAST_SUPPRESS(6014 6211)
	HRESULT BindColumns(_Inout_ IUnknown* pUnk) throw()
	{
		IID iidStreamToUse = __uuidof(ISequentialStream);
		bool fIStreamSupportTested = false;

		ATLENSURE_RETURN(pUnk != NULL);
		CComPtr<IAccessor> spAccessor;
		HRESULT hr = pUnk->QueryInterface(&spAccessor);
		if (FAILED(hr))
			return hr;

		ULONG   i;
		DBBYTEOFFSET   nOffset = 0, nDataOffset, nLengthOffset, nStatusOffset;

		// If the user hasn't specified the column information to bind by calling AddBindEntry then
		// we get it ourselves
		if (m_pColumnInfo == NULL)
		{
			CComPtr<IColumnsInfo> spColumnsInfo;
			hr = pUnk->QueryInterface(&spColumnsInfo);
			if (FAILED(hr))
				return hr;

			hr = spColumnsInfo->GetColumnInfo(&m_nColumns, &m_pColumnInfo, &m_pStringsBuffer);
			if (FAILED(hr))
				return hr;

			m_bOverride = false;
		}
		else
			m_bOverride = true;

		DBBINDING* pBinding = _ATL_NEW DBBINDING[m_nColumns];
		if( pBinding == NULL )
			return E_OUTOFMEMORY;

		CAutoVectorPtr<DBBINDING> spBinding(pBinding);

		ATLASSUME(m_pfClientOwnedMemRef == NULL);
		m_pfClientOwnedMemRef = _ATL_NEW bool[m_nColumns];
		if( m_pfClientOwnedMemRef == NULL )
		{
			return E_OUTOFMEMORY;
		}

#ifdef _DEBUG
		ATLTRACE( "CDynamicAccessor::BindColumns\n" );
		ATLTRACE( "Number of columns: %d\n", m_nColumns );
		ATLTRACE( "Blob Handling Mode: " );
		switch( m_eBlobHandling )
		{
		case DBBLOBHANDLING_DEFAULT:
			ATLTRACE( "DBBLOBHANDLING_DEFAULT\n" );
			break;
		case DBBLOBHANDLING_NOSTREAMS:
			ATLTRACE( "DBBLOBHANDLING_NOSTREAMS\n" );
			break;
		case DBBLOBHANDLING_SKIP:
			ATLTRACE( "DBBLOBHANDLING_SKIP\n" );
			break;
		default:
			ATLTRACE( "IVALID HANDLING MODE!!!\n" );
		}
#endif
		DBBINDING* pCurrent = pBinding;
		DBOBJECT*  pObject;

		ATLASSUME(m_pColumnInfo != NULL || m_nColumns == 0);

		for (i = 0; i < m_nColumns; i++)
		{
			// If it's a BLOB or the column size is large enough for us to treat it as
			// a BLOB then we also need to set up the DBOBJECT structure.
			m_pfClientOwnedMemRef[i] = false;
			if (m_pColumnInfo[i].ulColumnSize > m_nBlobSize && m_pColumnInfo[i].wType != DBTYPE_IUNKNOWN )
			{
				if( m_eBlobHandling == DBBLOBHANDLING_SKIP )
				{
					// Calculate the column data offset
					nDataOffset = 0;
					// Calculate the column length offset
					nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );
					// Calculate the column status offset
					nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );
					//If ulColumnSize is not zeroed, GetLength and GetStatus will not access the correct offset in buffer.
					//As they add ulColumnSize to skip the data.
					m_pColumnInfo[i].ulColumnSize = 0;
					ATLTRACE( "%d. Column ordinal %d: Binding length and status ONLY\n", i, m_pColumnInfo[i].iOrdinal );

					BindEx(pCurrent, m_pColumnInfo[i].iOrdinal, m_pColumnInfo[i].wType,
						m_pColumnInfo[i].ulColumnSize, m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
						DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, NULL, DBMEMOWNER_CLIENTOWNED, true );
					pCurrent++;
				}
				else if( m_eBlobHandling == DBBLOBHANDLING_NOSTREAMS )
				{
					// get the value by reference
					m_pColumnInfo[i].wType |= DBTYPE_BYREF;
					m_pColumnInfo[i].ulColumnSize   = sizeof(WCHAR*);
					m_pfClientOwnedMemRef[i] = true;

					// Calculate the column data offset
					nDataOffset = AlignAndIncrementOffset( nOffset, m_pColumnInfo[i].ulColumnSize, GetAlignment( m_pColumnInfo[i].wType ) );
					// Calculate the column length offset
					nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );
					// Calculate the column status offset
					nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );

					ATLTRACE( "%d. Column ordinal %d: Binding by reference in provider allocated, consumer owned memory\n", i, m_pColumnInfo[i].iOrdinal );

					BindEx(pCurrent, m_pColumnInfo[i].iOrdinal, m_pColumnInfo[i].wType,
						m_pColumnInfo[i].ulColumnSize, m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
						DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, NULL, DBMEMOWNER_CLIENTOWNED);
					pCurrent++;

				}
				else // if( m_eBlobHandling == DBBLOBHANDLING_DEFAULT )
				{
					// we will try to bind blobs as streams
					// if we have not tested if the rowset supports streams, do it now
					if( ! fIStreamSupportTested )
					{
						ATLTRACE( "Testing streams support... " );

						DBPROPID rgPropertyIDs[2] = { DBPROP_ISequentialStream, DBPROP_IStream };
						BOOL     rgStreamsSupported[2] = {FALSE, FALSE};

						// check if the rowset supports IStream* interfaces
						hr = GetRowsetProperties( pUnk, rgPropertyIDs, rgStreamsSupported, 2 );
						ATLASSERT( SUCCEEDED( hr ) );

						if( rgStreamsSupported[0] )
						{
							iidStreamToUse = __uuidof(ISequentialStream);

							ATLTRACE( "ISequentialStream is supported\n" );
						}
						else if( rgStreamsSupported[1] )
						{
							iidStreamToUse = __uuidof(IStream);

							ATLTRACE( "IStream is supported\n" );
						}
						else
						{

							ATLTRACE( "neither ISequentialStream nor IStream are supported!\n" );

							ATLASSERT(FALSE); // the stream interfaces are not supported!!!
						}

						fIStreamSupportTested = true;
					}

					pObject = _ATL_NEW DBOBJECT;
					if (pObject == NULL)
					{
						for( ULONG t = 0; t < i; t++ )
							delete pBinding[t].pObject;

						delete [] m_pfClientOwnedMemRef;
						m_pfClientOwnedMemRef = NULL;

						return E_OUTOFMEMORY;
					}
					pObject->dwFlags = STGM_READ;
					pObject->iid     = iidStreamToUse;
					m_pColumnInfo[i].wType      = DBTYPE_IUNKNOWN;
					m_pColumnInfo[i].ulColumnSize   = sizeof(IUnknown*);

					// Calculate the column data offset
					nDataOffset = AlignAndIncrementOffset( nOffset, m_pColumnInfo[i].ulColumnSize, GetAlignment( m_pColumnInfo[i].wType ) );
					// Calculate the column length offset
					nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );
					// Calculate the column status offset
					nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );

#ifdef _DEBUG
					if( iidStreamToUse == __uuidof(ISequentialStream) )
						ATLTRACE( "%d. Column ordinal %d: Binding as an ISequentialStream object\n", i, m_pColumnInfo[i].iOrdinal );
					else
						ATLTRACE( "%d. Column ordinal %d: Binding as an IStream object\n", i, m_pColumnInfo[i].iOrdinal );
#endif
					Bind(pCurrent, m_pColumnInfo[i].iOrdinal, m_pColumnInfo[i].wType,
						m_pColumnInfo[i].ulColumnSize, m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
						DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, pObject);
					pCurrent++;

				}
			}
			// else if it's a COM object
			else if( m_pColumnInfo[i].wType == DBTYPE_IUNKNOWN)
			{
				pObject = _ATL_NEW DBOBJECT;
				if (pObject == NULL)
				{
					for( ULONG t = 0; t < i; t++ )
						delete pBinding[t].pObject;

					delete [] m_pfClientOwnedMemRef;
					m_pfClientOwnedMemRef = NULL;

					return E_OUTOFMEMORY;
				}
				pObject->dwFlags = STGM_READ;
				pObject->iid     = __uuidof(IUnknown); // iidPersistToUse;
				m_pColumnInfo[i].wType      = DBTYPE_IUNKNOWN;
				m_pColumnInfo[i].ulColumnSize   = sizeof(IUnknown*);

				// Calculate the column data offset
				nDataOffset = AlignAndIncrementOffset( nOffset, m_pColumnInfo[i].ulColumnSize, GetAlignment( m_pColumnInfo[i].wType ) );

				// Calculate the column length offset
				nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );

				// Calculate the column status offset
				nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );

				ATLTRACE( "%d. Column ordinal %d: Binding a COM object\n", i, m_pColumnInfo[i].iOrdinal );

				Bind(pCurrent, m_pColumnInfo[i].iOrdinal, m_pColumnInfo[i].wType,
					m_pColumnInfo[i].ulColumnSize, m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
					DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, pObject);
				pCurrent++;

			}
			// else if it's not a BLOB or COM object
			else
			{
				pObject = NULL;

				// Calculate the size needed if it's a string
				// including the NULL terminator.
				if (m_pColumnInfo[i].wType == DBTYPE_STR)
					m_pColumnInfo[i].ulColumnSize += 1;

				if (m_pColumnInfo[i].wType == DBTYPE_WSTR)
					m_pColumnInfo[i].ulColumnSize = m_pColumnInfo[i].ulColumnSize*2 + 2;

				// Calculate the column data offset
				nDataOffset = AlignAndIncrementOffset( nOffset, m_pColumnInfo[i].ulColumnSize, GetAlignment( m_pColumnInfo[i].wType ) );

				// Calculate the column length offset
				nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );

				// Calculate the column status offset
				nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );

				ATLTRACE( "%d. Column ordinal %d: Binding as native data type\n", i, m_pColumnInfo[i].iOrdinal );

				Bind(pCurrent, m_pColumnInfo[i].iOrdinal, m_pColumnInfo[i].wType,
					m_pColumnInfo[i].ulColumnSize, m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
					DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, pObject);
				pCurrent++;
			}

			// Note that, as we're not using this for anything else, we're using the
			// pTypeInfo element to store the offset to our data.
			m_pColumnInfo[i].pTypeInfo = (ITypeInfo*)(ULONG_PTR)nDataOffset;
		}
		// Allocate the accessor memory if we haven't done so yet
		if (m_pAccessorInfo == NULL)
		{
			hr = AllocateAccessorMemory(1); // We only have one accessor
			if (FAILED(hr))
			{
				for( ULONG t = 0; t < m_nColumns; t++ )
					delete pBinding[t].pObject;

				delete [] m_pfClientOwnedMemRef;
				m_pfClientOwnedMemRef = NULL;

				return hr;
			}
			_Analysis_assume_(m_pAccessorInfo != NULL);
			m_pAccessorInfo->bAutoAccessor = true;
		}

		// Allocate enough memory for the data buffer and tell the rowset
		// Note that the rowset will free the memory in its destructor.
		m_pBuffer = _ATL_NEW BYTE[nOffset];
		if (m_pBuffer == NULL)
		{
			for( ULONG t = 0; t < m_nColumns; t++ )
				delete pBinding[t].pObject;

			delete [] m_pfClientOwnedMemRef;
			m_pfClientOwnedMemRef = NULL;

			return E_OUTOFMEMORY;
		}
		memset(m_pBuffer, 0, nOffset);
		hr = BindEntries(pBinding, m_nColumns, &m_pAccessorInfo->hAccessor,
				nOffset, spAccessor);

		if( FAILED(hr))
		{
			delete [] m_pfClientOwnedMemRef;
			m_pfClientOwnedMemRef = NULL;
		}

		return hr;
	}
	ATLPREFAST_UNSUPPRESS()

	// Translate the column number to the index into the column info array
	bool TranslateColumnNo(_Inout_ DBORDINAL& nColumn) const throw()
	{
		ATLASSUME(m_pColumnInfo != NULL);
		// If the user has overridden the binding then we need to search
		// through the column info for the ordinal number
		if (m_bOverride)
		{
			for (ULONG i = 0; i < m_nColumns; i++)
			{
				if (m_pColumnInfo[i].iOrdinal == nColumn)
				{
					nColumn = i;
					return true;
				}
			}
			return false;
		}
		else
		{
			// Note that m_pColumnInfo->iOrdinal will be zero if have bound
			// a bookmark as the first entry, otherwise it will be 1.
			// If the column is out of range then return false
			if (nColumn > (m_nColumns - 1 + m_pColumnInfo->iOrdinal))
				return false;

			// otherwise translate the column to an index into our internal
			// binding entries array
			nColumn -= m_pColumnInfo->iOrdinal;
			return true;
		}
	}

	static size_t GetAlignment(_In_ DBTYPE bType) throw()
	{
		if( bType & DBTYPE_BYREF )
			return __alignof(void*);

		if( bType & DBTYPE_ARRAY )
			return __alignof(SAFEARRAY*);

		if( bType & DBTYPE_VECTOR )
			return __alignof(DBVECTOR);

		switch( bType )
		{
		case DBTYPE_I2:
			return __alignof(signed short);
			break;

		case DBTYPE_I4:
			return __alignof(signed int);
			break;

		case DBTYPE_R4:
			return __alignof(float);
			break;

		case DBTYPE_R8:
			return __alignof(double);
			break;

		case DBTYPE_CY:
			return __alignof(__int64);
			break;

		case DBTYPE_DATE:
			return __alignof(DATE);
			break;

		case DBTYPE_BSTR:
			return __alignof(BSTR*);
			break;

		case DBTYPE_IDISPATCH:
			return __alignof(IDispatch*);
			break;

		case DBTYPE_ERROR:
			return __alignof(SCODE);
			break;

		case DBTYPE_BOOL:
			return __alignof(VARIANT_BOOL);
			break;

		case DBTYPE_VARIANT:
			return __alignof(VARIANT);
			break;

		case DBTYPE_IUNKNOWN:
			return __alignof(IUnknown*);
			break;

		case DBTYPE_DECIMAL:
			return __alignof(DECIMAL);
			break;

		case DBTYPE_UI1:
			return __alignof(unsigned char);
			break;

		case DBTYPE_I1:
			return __alignof(signed char);
			break;

		case DBTYPE_UI2:
			return __alignof(unsigned short);
			break;

		case DBTYPE_UI4:
			return __alignof(unsigned int);
			break;

		case DBTYPE_I8:
			return __alignof(signed char);
			break;

		case DBTYPE_UI8:
			return __alignof(unsigned char);
			break;

		case DBTYPE_GUID:
			return __alignof(GUID);
			break;

		case DBTYPE_BYTES:
			return __alignof(BYTE);
			break;

		case DBTYPE_STR:
			return __alignof(char);
			break;

		case DBTYPE_WSTR:
			return __alignof(short);
			break;

		case DBTYPE_NUMERIC:
			return __alignof(DB_NUMERIC);
			break;

		case DBTYPE_DBDATE:
			return __alignof(DBDATE);
			break;

		case DBTYPE_DBTIME:
			return __alignof(DBTIME);
			break;

		case DBTYPE_DBTIMESTAMP:
			return __alignof(DBTIMESTAMP);
			break;

		default:
			return __alignof(__int64);
		}
	}

	inline static DBBYTEOFFSET AlignAndIncrementOffset(
		_Inout_ DBBYTEOFFSET& nOffset,
		_In_ DBLENGTH nSize,
		_In_ size_t nAlign) throw()
	{
		DBBYTEOFFSET nResult;

		nOffset = AtlAlignUp( nOffset, (ULONG)nAlign );
		nResult = nOffset;
		nOffset += nSize;

		return nResult;
	}

	inline static void IncrementAndAlignOffset(
		_Inout_ DBBYTEOFFSET& nOffset,
		_In_ DBLENGTH nSize,
		_In_ size_t nAlign) throw()
	{
		nOffset += nSize;
		nOffset = AtlAlignUp( nOffset, (ULONG)nAlign );
	}

	typedef CDynamicAccessor _OutputColumnsClass;
	static bool HasOutputColumns() throw()
	{
		return true;
	}

	DBORDINAL           m_nColumns;
	bool*				m_pfClientOwnedMemRef;
	DBCOLUMNINFO*       m_pColumnInfo;
	OLECHAR*            m_pStringsBuffer;
	bool                m_bOverride;

	protected:
	DBBLOBHANDLINGENUM  m_eBlobHandling;
	DBLENGTH            m_nBlobSize;
};

template< typename BaseType >
inline void strcpyT(
	_Out_writes_z_(maxCount) BaseType *strDest,
	_In_ size_t maxCount,
	_In_z_ const BaseType *strSource)
{
}

template< typename BaseType >
inline size_t strlenT(_In_z_ const BaseType *string)
{
	return NULL;
}

template<>
inline void strcpyT<CHAR>(
	_Out_writes_z_(maxCount) CHAR *strDest,
	_In_ size_t maxCount,
	_In_z_ const CHAR *strSource)
{
	Checked::strcpy_s( strDest, maxCount, strSource);
}

template<>
inline size_t strlenT<CHAR>(_In_z_ const CHAR *string)
{
	return AtlStrLen( string );
}

template<>
inline void strcpyT<WCHAR>(
	_Out_writes_z_(maxCount) WCHAR *strDest,
	_In_ size_t maxCount,
	_In_z_ const WCHAR *strSource)
{
	Checked::wcscpy_s( strDest, maxCount, strSource);
}

template<>
inline size_t strlenT<WCHAR>(_In_z_ const WCHAR *string)
{
	return AtlStrLen( string );
}

ATLPREFAST_SUPPRESS(6211)
template< typename BaseType, DBTYPEENUM OleDbType >
class CDynamicStringAccessorT :
	public CDynamicAccessor
{
public:
	explicit CDynamicStringAccessorT(_In_ DBLENGTH nBlobSize = 8000)
		: CDynamicAccessor( DBBLOBHANDLING_DEFAULT, nBlobSize )
	{
	}

	HRESULT BindColumns(_Inout_ IUnknown* pUnk) throw()
	{
		ATLENSURE_RETURN(pUnk != NULL);
		CComPtr<IAccessor> spAccessor;
		HRESULT hr = pUnk->QueryInterface(&spAccessor);
		if (FAILED(hr))
			return hr;

		ULONG   i;
		DBBYTEOFFSET nOffset = 0, nDataOffset, nLengthOffset, nStatusOffset;
		DBLENGTH nLength;

		// If the user hasn't specified the column information to bind by calling AddBindEntry then
		// we get it ourselves
		if (m_pColumnInfo == NULL)
		{
			CComPtr<IColumnsInfo> spColumnsInfo;
			hr = pUnk->QueryInterface(&spColumnsInfo);
			if (FAILED(hr))
				return hr;

			hr = spColumnsInfo->GetColumnInfo(&m_nColumns, &m_pColumnInfo, &m_pStringsBuffer);
			if (FAILED(hr))
				return hr;

			m_bOverride = false;
		}
		else
			m_bOverride = true;

		DBBINDING* pBinding = _ATL_NEW DBBINDING[m_nColumns];
		if (pBinding == NULL)
			return E_OUTOFMEMORY;

		::ATL::CAutoVectorPtr<DBBINDING> spBinding(pBinding);

		ATLASSUME(m_pfClientOwnedMemRef == NULL);
		m_pfClientOwnedMemRef = _ATL_NEW bool[m_nColumns];
		if( m_pfClientOwnedMemRef == NULL )
		{
			return E_OUTOFMEMORY;
		}

		DBBINDING* pCurrent = pBinding;

		ATLASSUME(m_pColumnInfo != NULL || m_nColumns == 0);

		for (i = 0; i < m_nColumns; i++)
		{
			m_pfClientOwnedMemRef[i] = false;

			// If it's a IPersist* object or the column size is large enough for us to treat it as
			// a BLOB then we will request references (in client owned memory) to a string
			if (m_pColumnInfo[i].ulColumnSize > m_nBlobSize || m_pColumnInfo[i].wType == DBTYPE_IUNKNOWN)
			{
				m_pColumnInfo[i].wType      = OleDbType | DBTYPE_BYREF;
				m_pColumnInfo[i].ulColumnSize   = sizeof(BaseType*);
				m_pfClientOwnedMemRef[i] = true;
			}
			else // We're treating everything as a string so add 1 for the NULL byte.
			{
				switch (m_pColumnInfo[i].wType)
				{
				case DBTYPE_BOOL:
					nLength = 2;
					break;
				case DBTYPE_BYTES:
					nLength = m_pColumnInfo[i].ulColumnSize * 2;
					break;
				case DBTYPE_BSTR:
					nLength = m_pColumnInfo[i].ulColumnSize;
					break;
				case DBTYPE_STR:
					nLength = m_pColumnInfo[i].ulColumnSize;
					break;
				case DBTYPE_WSTR:
					nLength = m_pColumnInfo[i].ulColumnSize;
					break;
				case DBTYPE_I1:
					nLength = 5;
					break;
				case DBTYPE_I2:
					nLength = 7;
					break;
				case DBTYPE_I4:
					nLength = 12;
					break;
				case DBTYPE_I8:
					nLength = 22;
					break;
				case DBTYPE_UI1:
					nLength = 4;
					break;
				case DBTYPE_UI2:
					nLength = 6;
					break;
				case DBTYPE_UI4:
					nLength = 11;
					break;
				case DBTYPE_UI8:
					nLength = 21;
					break;
				case DBTYPE_R4:
					nLength = 13;
					break;
				case DBTYPE_R8:
					nLength = 23;		// maybe 9
					break;
				case DBTYPE_DECIMAL:
					nLength = 23;
					break;
				case DBTYPE_NUMERIC:
					nLength = 23;
					break;
				case DBTYPE_VARIANT:
					nLength = 20;
					break;
				case DBTYPE_IDISPATCH:
					nLength = 32;
					break;
				case DBTYPE_IUNKNOWN:
					nLength = 32;
					break;
				case DBTYPE_GUID:
					nLength = 38;
					break;
				case DBTYPE_ARRAY:
					nLength = 32;
					break;
				case DBTYPE_VECTOR:
					nLength = 32;
					break;
				case DBTYPE_DATE:
					nLength = 32;
					break;
				case DBTYPE_DBDATE:
					nLength = 32;
					break;
				case DBTYPE_DBTIME:
					nLength = 32;
					break;
				case DBTYPE_DBTIMESTAMP:
					nLength = 32;
					break;
				case DBTYPE_FILETIME:
					nLength = 32;
					break;
				case DBTYPE_PROPVARIANT:
					nLength = 32;
					break;
				case DBTYPE_VARNUMERIC:
					nLength = 32;
					break;
				case DBTYPE_CY:
					nLength = 32;
					break;
				default:
					ATLASSERT(FALSE); // unhandled column type
					nLength = 32;
				}
				m_pColumnInfo[i].ulColumnSize = (nLength + 1) * sizeof(BaseType);
				m_pColumnInfo[i].wType = OleDbType;
			}

			// Calculate the column data offset
			nDataOffset = AlignAndIncrementOffset( nOffset, m_pColumnInfo[i].ulColumnSize, GetAlignment( OleDbType ) );

			// Calculate the column length offset
			nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );

			// Calculate the column status offset
			nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );

			BindEx(pCurrent, m_pColumnInfo[i].iOrdinal, m_pColumnInfo[i].wType,
				m_pColumnInfo[i].ulColumnSize, m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
				DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, NULL, DBMEMOWNER_CLIENTOWNED);

			pCurrent++;

			// Note that, as we're not using this for anything else, we're using the
			// pTypeInfo element to store the offset to our data.
			m_pColumnInfo[i].pTypeInfo = (ITypeInfo*)(ULONG_PTR)nDataOffset;
		}

		// Allocate the accessor memory if we haven't done so yet
		if (m_pAccessorInfo == NULL)
		{
			hr = AllocateAccessorMemory(1); // We only have one accessor
			if (FAILED(hr))
			{
				delete [] m_pfClientOwnedMemRef;
				m_pfClientOwnedMemRef = NULL;
				return hr;
			}
			_Analysis_assume_(m_pAccessorInfo != NULL);
			m_pAccessorInfo->bAutoAccessor = true;
		}

		// Allocate enough memory for the data buffer and tell the rowset
		// Note that the rowset will free the memory in its destructor.
		m_pBuffer = _ATL_NEW BYTE[nOffset];
		if (m_pBuffer == NULL)
		{
			delete [] m_pfClientOwnedMemRef;
			m_pfClientOwnedMemRef = NULL;
			return E_OUTOFMEMORY;
		}

		memset(m_pBuffer, 0, nOffset);
		hr = BindEntries(pBinding, m_nColumns, &m_pAccessorInfo->hAccessor,
				nOffset, spAccessor);

		if( FAILED(hr) )
		{
			delete [] m_pfClientOwnedMemRef;
			m_pfClientOwnedMemRef = NULL;
		}

		return hr;
	}

	BaseType* GetString(_In_ DBORDINAL nColumn) const throw()
	{
		if (TranslateColumnNo(nColumn))
		{
			if( m_pColumnInfo[nColumn].wType & DBTYPE_BYREF )
				return *(BaseType**)_GetDataPtr(nColumn);
			else
				return (BaseType*)_GetDataPtr(nColumn);
		}
		else
			return NULL;
	}

	BaseType* GetString(_In_z_ const CHAR* pColumnName) const throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			if( m_pColumnInfo[nColumn].wType & DBTYPE_BYREF )
				return *(BaseType**)_GetDataPtr(nColumn);
			else
				return (BaseType*)_GetDataPtr(nColumn);
		}
		else
			return NULL;    // Not Found
	}

	BaseType* GetString(_In_z_ const WCHAR* pColumnName) const throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
		{
			if( m_pColumnInfo[nColumn].wType & DBTYPE_BYREF )
				return *(BaseType**)_GetDataPtr(nColumn);
			else
				return (BaseType*)_GetDataPtr(nColumn);
		}
		else
			return NULL;    // Not Found
	}

	void _SetLength(
		_In_ DBORDINAL nColumn,
		_In_ DBLENGTH nLength) throw()
	{
		DBBYTEOFFSET nOffset = (DBBYTEOFFSET)(ULONG_PTR)m_pColumnInfo[nColumn].pTypeInfo;
		IncrementAndAlignOffset( nOffset, m_pColumnInfo[nColumn].ulColumnSize, __alignof(DBLENGTH) );
		*(DBLENGTH*)( m_pBuffer + nOffset ) = nLength;
	}

	HRESULT _SetString(
		_In_ DBORDINAL nColumn,
		_In_z_ BaseType* data) throw()
	{
		DBLENGTH stringLen = (DBLENGTH)strlenT<BaseType>( data );

		if( m_pColumnInfo[nColumn].wType & DBTYPE_BYREF )
		{
			BaseType** pBuffer = (BaseType**)_GetDataPtr(nColumn);



			//in case of overflow throw exception
			if (stringLen + 1>(size_t(-1)/sizeof(BaseType)))
			{
				return(E_FAIL);//arithmetic overflow
			}

			BaseType* pNewBuffer = (BaseType*)::ATL::AtlCoTaskMemRecalloc( *pBuffer, (stringLen + 1), sizeof(BaseType));
			if( pNewBuffer == NULL )
				return E_OUTOFMEMORY;

			*pBuffer = pNewBuffer;
			strcpyT<BaseType>( pNewBuffer, stringLen+1, data);
			_SetLength( nColumn, stringLen * sizeof(BaseType) );
		}
		else
		{
			BaseType* pBuffer = (BaseType*)_GetDataPtr(nColumn);
			if( stringLen >= m_pColumnInfo[nColumn].ulColumnSize / sizeof(BaseType) )
			{
				strcpyT<BaseType>( pBuffer, m_pColumnInfo[nColumn].ulColumnSize / sizeof(BaseType) - 1, data);
				pBuffer[m_pColumnInfo[nColumn].ulColumnSize / sizeof(BaseType) - 1] = 0;
				_SetLength( nColumn, m_pColumnInfo[nColumn].ulColumnSize - sizeof(BaseType));
				return DBSTATUS_S_TRUNCATED;
			}
			else
			{
				strcpyT<BaseType>( pBuffer, m_pColumnInfo[nColumn].ulColumnSize / sizeof(BaseType), data);
				_SetLength( nColumn, stringLen * sizeof(BaseType) );
			}
		}
		return S_OK;
	}

	HRESULT SetString(
		_In_ DBORDINAL nColumn,
		_In_z_ BaseType* data) throw()
	{
		if (TranslateColumnNo(nColumn))
			return _SetString(nColumn, data);
		else
			return DB_S_ERRORSOCCURRED;
	}
	HRESULT SetString(
		_In_z_ const CHAR* pColumnName,
		_In_z_ BaseType* data) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
			return _SetString(nColumn, data);
		else
			return DB_S_ERRORSOCCURRED;
	}
	HRESULT SetString(
		_In_z_ const WCHAR* pColumnName,
		_In_z_ BaseType* data) throw()
	{
		ATLASSERT(pColumnName != NULL);
		DBORDINAL nColumn;
		if (GetInternalColumnNo(pColumnName, &nColumn))
			return _SetString(nColumn, data);
		else
			return DB_S_ERRORSOCCURRED;
	}
};
ATLPREFAST_UNSUPPRESS()

typedef CDynamicStringAccessorT<CHAR, DBTYPE_STR> CDynamicStringAccessorA;
typedef CDynamicStringAccessorT<WCHAR, DBTYPE_WSTR> CDynamicStringAccessorW;

#ifdef _UNICODE
typedef CDynamicStringAccessorW CDynamicStringAccessor;
#else
typedef CDynamicStringAccessorA CDynamicStringAccessor;
#endif

class CXMLAccessor :
	public CDynamicStringAccessorW
{
public:

	HRESULT GetXMLColumnData(_Inout_ CSimpleStringW& strOutput) throw()
	{
		_ATLTRY
		{
			strOutput = L"<columninfo>\n";
			DBTYPE wType;
			DBORDINAL nColumns = m_nColumns;
			//If Bookmark column -> index is 0 based starting at bookmark - index 0.
			if (m_pColumnInfo->iOrdinal == 0 && m_nColumns > 0)
			{
				--nColumns;
			}
			for (ULONG i=1; i <= nColumns; i++)
			{
				strOutput += L"<column type=\"";
				if( !GetColumnType(i, &wType) )
					return E_FAIL;

				wType &= ~DBTYPE_BYREF;
				wType &= ~DBTYPE_ARRAY;
				wType &= ~DBTYPE_VECTOR;

				switch (wType)
				{
				case DBTYPE_BOOL:
					strOutput += L"BOOL";
					break;
				case DBTYPE_STR:
				case DBTYPE_WSTR:
					strOutput += L"STRING";
					break;
				case DBTYPE_I1:
					strOutput += L"I1";
					break;
				case DBTYPE_I2:
					strOutput += L"I2";
					break;
				case DBTYPE_I4:
					strOutput += L"I4";
					break;
				case DBTYPE_I8:
					strOutput += L"I8";
					break;
				case DBTYPE_UI1:
					strOutput += L"UI1";
					break;
				case DBTYPE_UI2:
					strOutput += L"UI2";
					break;
				case DBTYPE_UI4:
					strOutput += L"UI4";
					break;
				case DBTYPE_UI8:
					strOutput += L"UI8";
					break;
				case DBTYPE_R4:
					strOutput += L"R4";
					break;
				case DBTYPE_R8:
					strOutput += L"R8";
					break;
				case DBTYPE_DECIMAL:
					strOutput += L"DECIMAL";
					break;
				case DBTYPE_NUMERIC:
					strOutput += L"NUMERIC";
					break;
				case DBTYPE_VARIANT:
					strOutput += L"VARIANT";
					break;
				case DBTYPE_IDISPATCH:
					strOutput += L"DISPATCH";
					break;
				case DBTYPE_IUNKNOWN:
					strOutput += L"IUNKNOWN";
					break;
				case DBTYPE_GUID:
					strOutput += L"GUID";
					break;
				case DBTYPE_DBDATE:
					strOutput += L"DBDATE";
					break;
				case DBTYPE_DBTIME:
					strOutput += L"DBTIME";
					break;
				case DBTYPE_DBTIMESTAMP:
					strOutput += L"DBTIMESTAMP";
					break;
				case DBTYPE_FILETIME:
					strOutput += L"FILETIME";
					break;
				case DBTYPE_PROPVARIANT:
					strOutput += L"PROPVARIANT";
					break;
				case DBTYPE_VARNUMERIC:
					strOutput += L"VARNUMERIC";
					break;
				}
				strOutput += L"\">";
				strOutput += GetColumnName(i);
				strOutput += L"</column>\n";
			}
			strOutput += L"</columninfo>\n";
			return S_OK;
		}
		_ATLCATCH(e)
		{
			_ATLDELETEEXCEPTION(e)
			return E_FAIL;
		}
	}

	HRESULT GetXMLRowData(
		_Inout_ CSimpleStringW& strOutput,
		_In_ bool bAppend = false) throw()
	{
		_ATLTRY
		{
			LPOLESTR pszName;
			if (!bAppend)
				strOutput.Empty();
			strOutput += L"<row>";
			DBORDINAL nColumns = m_nColumns;
			//If Bookmark column -> index is 0 based starting at bookmark - index 0.
			if (m_pColumnInfo->iOrdinal == 0 && m_nColumns > 0)
			{
				--nColumns;
			}
			for (ULONG i=1; i<=nColumns; i++)
			{
				pszName = GetColumnName(i);
				strOutput += L"<";
				strOutput += pszName;
				strOutput += L">";
				DBSTATUS dbStatus=DBSTATUS_S_ISNULL;
				if( !GetStatus(i, &dbStatus) )
					return E_FAIL;

				if (dbStatus!=DBSTATUS_S_ISNULL)
					strOutput += GetString(i);

				strOutput += L"</";
				strOutput += pszName;
				strOutput += L">";
			}
			strOutput += L"</row>";
			return S_OK;
		}
		_ATLCATCH(e)
		{
			_ATLDELETEEXCEPTION(e)
			return E_FAIL;
		}
	}
};

// Like CDynamicAccessor but everything is bound as a DBTYPE_VARIANT
class CDynamicVariantAccessor :
	public CDynamicAccessor
{
public:
	ATLPREFAST_SUPPRESS(6014 6211)
	HRESULT BindColumns(_Inout_ IUnknown* pUnk) throw()
	{
		ATLENSURE_RETURN(pUnk != NULL);
		CComPtr<IAccessor> spAccessor;
		HRESULT hr = pUnk->QueryInterface(&spAccessor);
		if (FAILED(hr))
			return hr;

		ULONG   i;
		DBBYTEOFFSET   nOffset = 0, nDataOffset, nLengthOffset, nStatusOffset;

		// If the user hasn't specified the column information to bind by calling AddBindEntry then
		// we get it ourselves
		if (m_pColumnInfo == NULL)
		{
			CComPtr<IColumnsInfo> spColumnsInfo;
			hr = pUnk->QueryInterface(&spColumnsInfo);
			if (FAILED(hr))
				return hr;

			hr = spColumnsInfo->GetColumnInfo(&m_nColumns, &m_pColumnInfo, &m_pStringsBuffer);
			if (FAILED(hr))
				return hr;

			m_bOverride = false;
		}
		else
			m_bOverride = true;

		DBBINDING* pBinding = _ATL_NEW DBBINDING[m_nColumns];

		if (pBinding == NULL)
			return E_OUTOFMEMORY;

		CAutoVectorPtr<DBBINDING> spBinding(pBinding);

		DBBINDING* pCurrent = pBinding;
		DBOBJECT*  pObject;

		ATLASSUME(m_pColumnInfo != NULL || m_nColumns == 0);

		for (i = 0; i < m_nColumns; i++)
		{
			// If it's a BLOB or the column size is large enough for us to treat it as
			// a BLOB then we also need to set up the DBOBJECT structure.
			if (m_pColumnInfo[i].ulColumnSize > m_nBlobSize || m_pColumnInfo[i].wType == DBTYPE_IUNKNOWN)
			{
				pObject = _ATL_NEW DBOBJECT;
				if (pObject == NULL)
				{
					for( UINT t = 0; t < i; t++ )
						delete pBinding[t].pObject;
					return E_OUTOFMEMORY;
				}
				pObject->dwFlags = STGM_READ;
				pObject->iid     = __uuidof(ISequentialStream);
				m_pColumnInfo[i].wType      = DBTYPE_IUNKNOWN;
				m_pColumnInfo[i].ulColumnSize   = sizeof(IUnknown*);
			}
			else
				pObject = NULL;

			m_pColumnInfo[i].ulColumnSize = sizeof(VARIANT);

			// Calculate the column data offset
			nDataOffset = AlignAndIncrementOffset(nOffset, sizeof(VARIANT), __alignof(VARIANT));

			// Calculate the column length offset
			nLengthOffset = AlignAndIncrementOffset(nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH));

			// Calculate the column status offset
			nStatusOffset = AlignAndIncrementOffset(nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS));

			Bind(pCurrent, m_pColumnInfo[i].iOrdinal, DBTYPE_VARIANT,
				sizeof(VARIANT), m_pColumnInfo[i].bPrecision, m_pColumnInfo[i].bScale,
				DBPARAMIO_NOTPARAM, nDataOffset, nLengthOffset, nStatusOffset, pObject);
			pCurrent++;

			// Note that, as we're not using this for anything else, we're using the
			// pTypeInfo element to store the offset to our data.
			m_pColumnInfo[i].pTypeInfo = (ITypeInfo*)(ULONG_PTR)nDataOffset;
		}
		// Allocate the accessor memory if we haven't done so yet
		if (m_pAccessorInfo == NULL)
		{
			hr = AllocateAccessorMemory(1); // We only have one accessor
			if (FAILED(hr))
			{
				for( UINT t = 0; t < m_nColumns; t++ )
					delete pBinding[t].pObject;
				return hr;
			}
			_Analysis_assume_(m_pAccessorInfo != NULL);
			m_pAccessorInfo->bAutoAccessor = true;
		}

		// Allocate enough memory for the data buffer and tell the rowset
		// Note that the rowset will free the memory in its destructor.
		m_pBuffer = _ATL_NEW BYTE[nOffset];
		if (m_pBuffer == NULL)
		{
			for( UINT t = 0; t < m_nColumns; t++ )
				delete pBinding[t].pObject;
			return E_OUTOFMEMORY;
		}
		memset(m_pBuffer, 0, nOffset);
		hr = BindEntries(pBinding, m_nColumns, &m_pAccessorInfo->hAccessor,
				nOffset, spAccessor);

		return hr;
	}
	ATLPREFAST_UNSUPPRESS()
};

///////////////////////////////////////////////////////////////////////////
// class CDynamicParameterAccessor

class CDynamicParameterAccessor :
	public CDynamicAccessor
{
// Constructors and Destructors
public:
	typedef CDynamicParameterAccessor _ParamClass;
	CDynamicParameterAccessor(
			_In_ DBBLOBHANDLINGENUM eBlobHandling = DBBLOBHANDLING_DEFAULT,
			_In_ DBLENGTH nBlobSize = 8000 )
		: CDynamicAccessor( eBlobHandling, nBlobSize )
	{
		m_pParameterEntry       = NULL;
		m_pParameterBuffer      = NULL;
		m_ppParamName           = NULL;
		m_nParameterBufferSize  = 0;
		m_nParams               = 0;
	};

	~CDynamicParameterAccessor()
	{
		delete [] m_pParameterEntry;
		if (m_ppParamName != NULL)
		{
			CoTaskMemFree(*m_ppParamName);
			delete [] m_ppParamName;
		}
		delete m_pParameterBuffer;
	};

	_Success_(return != false)
	bool GetParamSize(
		_In_ DBORDINAL nParam,
		_Out_ DBLENGTH *pLength) const
	{
		ATLENSURE( pLength != NULL );

		if( !TranslateParameterNo( nParam ) )
			return false;

		*pLength = m_pParameterEntry[nParam].cbMaxLen;
		return true;
	}

	_Success_(return != false)
	bool GetParamIO(
		_In_ DBORDINAL nParam,
		_Out_ DBPARAMIO *pParamIO) const
	{
		ATLENSURE( pParamIO != NULL );

		if( !TranslateParameterNo( nParam ) )
			return false;

		*pParamIO = m_pParameterEntry[nParam].eParamIO;
		return true;
	}

	_Success_(return != false)
	bool GetParamType(
		_In_ DBORDINAL nParam,
		_Out_ DBTYPE *pType) const
	{
		ATLENSURE( pType != NULL );

		if( !TranslateParameterNo( nParam ) )
			return false;

		*pType = m_pParameterEntry[nParam].wType;
		return true;
	}

	_Success_(return != false)
	bool GetParamLength(
		_In_ DBORDINAL nParam,
		_Out_ DBLENGTH *pLength)
	{
		ATLENSURE( pLength != NULL );

		DBLENGTH* pBuffer = GetParamLength(nParam);
		if (pBuffer == NULL)
			return false;
		*pLength = *pBuffer;
		return true;
	}

	_Success_(return != false)
	bool SetParamLength(
		_In_ DBORDINAL nParam,
		_In_ DBLENGTH length)
	{
		DBLENGTH* pBuffer = GetParamLength(nParam);
		if (pBuffer == NULL)
			return false;
		*pBuffer = length;
		return true;
	}

	_Success_(return != false)
	bool GetParamStatus(
		_In_ DBORDINAL nParam,
		_Out_ DBSTATUS *pStatus)
	{
		ATLENSURE( pStatus != NULL );

		DBSTATUS* pBuffer = GetParamStatus(nParam);
		if (pBuffer == NULL)
			return false;
		*pStatus = *pBuffer;
		return true;
	}

	_Success_(return != false)
	bool SetParamStatus(
		_In_ DBORDINAL nParam,
		_In_ DBSTATUS status)
	{
		DBSTATUS* pBuffer = GetParamStatus(nParam);
		if (pBuffer == NULL)
			return false;
		*pBuffer = status;
		return true;
	}

	ATLPREFAST_SUPPRESS(6101)
	template <class ctype>
	_Success_(return != false)
	bool GetParam(
		_In_ DBORDINAL nParam,
		_Out_ ctype* pData) const throw()
	{
		ATLENSURE_RETURN_VAL( pData != NULL, false);
		ctype* pBuffer = (ctype*)GetParam(nParam);
		if (pBuffer == NULL)
			return false;
		*pData = *pBuffer;
		return true;
	}
	ATLPREFAST_UNSUPPRESS()

	_Success_(return != false)
	bool GetParamString(
		_In_ DBORDINAL nParam,
		_Inout_ CSimpleStringA& strOutput) throw()
	{
		CHAR* pData = (CHAR*)GetParam(nParam);
		if (pData == NULL)
			return false;

		strOutput = pData;
		return true;
	}

	_Success_(return != false)
	bool GetParamString(
		_In_ DBORDINAL nParam,
		_Inout_ CSimpleStringW& strOutput) throw()
	{
		WCHAR* pData = (WCHAR*)GetParam(nParam);
		if (pData == NULL)
			return false;

		strOutput = pData;
		return true;
	}

	_Success_(return != false)
	bool GetParamString(
		_In_ DBORDINAL nParam,
		_When_(pBuffer != NULL, _Out_writes_to_(*pMaxLen, *pMaxLen) _Post_z_) CHAR* pBuffer,
		_Inout_ size_t* pMaxLen) throw()
	{
		ATLASSERT( pMaxLen != NULL );

		CHAR* pData = (CHAR*)GetParam(nParam);
		if (pData == NULL)
			return false;

		size_t nStrLen = strlen( pData );

		if( pBuffer == NULL )
		{
			*pMaxLen = nStrLen + 1;
			return true;
		}

		if( ( *pMaxLen ) < ( nStrLen + 1 ) )
		{
			return false;
		}

		Checked::strcpy_s( pBuffer, *pMaxLen, pData);
		return true;
	}

	_Success_(return != false)
	bool GetParamString(
		_In_ DBORDINAL nParam,
		_When_(pBuffer != NULL, _Out_writes_to_(*pMaxLen, *pMaxLen) _Post_z_) WCHAR* pBuffer,
		_Inout_ size_t* pMaxLen) throw()
	{
		ATLASSERT( pMaxLen != NULL );

		WCHAR* pData = (WCHAR*)GetParam(nParam);
		if (pData == NULL)
			return false;

		size_t nStrLen = wcslen( pData );

		if( pBuffer == NULL )
		{
			*pMaxLen = nStrLen + 1;
			return true;
		}

		if( ( * pMaxLen ) < ( nStrLen + 1 ) )
		{
			return false;
		}

		Checked::wcscpy_s( pBuffer, *pMaxLen, pData);
		return true;
	}

	template <class ctype>
	bool SetParam(
		_In_ DBORDINAL nParam,
		_In_ const ctype* pData,
		_In_ DBSTATUS status = DBSTATUS_S_OK)
	{
		ATLENSURE( pData != NULL );

		if( !TranslateParameterNo( nParam ) )
			return false;

		ctype* pBuffer = (ctype*)_GetParam(nParam);
		if (pBuffer == NULL)
			return false;

		*pBuffer = *pData;

		DBSTATUS *pStatus = _GetParamStatus( nParam );
		if( pStatus != NULL )
			*pStatus = status;

		return true;
	}

	bool SetParamString(
		_In_ DBORDINAL nParam,
		_In_z_ const CHAR* pString,
		_In_ DBSTATUS status = DBSTATUS_S_OK )
	{
		ATLENSURE( pString != NULL );

		if( !TranslateParameterNo( nParam ) )
			return false;

		CHAR* pBuffer = (CHAR*)_GetParam(nParam);
		if (pBuffer == NULL)
			return false;

		int nMaxLen = m_pParameterEntry[nParam].cbMaxLen;
		if( AtlStrLen( pString ) >= nMaxLen )
			return false;

		Checked::strcpy_s(pBuffer, nMaxLen, pString);

		DBSTATUS *pStatus = _GetParamStatus( nParam );
		if( pStatus != NULL )
			*pStatus = status;
		DBLENGTH *pLength = _GetParamLength( nParam );
		if( pLength != NULL )
			*pLength = (DBLENGTH)(strlen(pBuffer));

		return true;
	}

	bool SetParamString(
		_In_ DBORDINAL nParam,
		_In_z_ const WCHAR* pString,
		_In_ DBSTATUS status = DBSTATUS_S_OK ) throw()
	{
		ATLASSUME( pString != NULL );

		if( !TranslateParameterNo( nParam ) )
			return false;

		WCHAR* pBuffer = (WCHAR*)_GetParam(nParam);
		if (pBuffer == NULL)
			return false;

		size_t nMaxLen = m_pParameterEntry[nParam].cbMaxLen / 2;
		if( Checked::wcsnlen( pString, nMaxLen ) >= nMaxLen )
			return false;

		Checked::wcscpy_s( pBuffer, nMaxLen, pString);

		DBSTATUS *pStatus = _GetParamStatus( nParam );
		if( pStatus != NULL )
			*pStatus = status;
		DBLENGTH *pLength = _GetParamLength( nParam );
		if( pLength != NULL )
			*pLength = (DBLENGTH)( sizeof(WCHAR) * wcslen(pBuffer));

		return true;
	}

	ATLPREFAST_SUPPRESS(6101)
	template <class ctype>
	_Success_(return != false) bool GetParam(
		_In_z_ TCHAR* pParamName,
		_Out_ ctype* pData) const throw()
	{
		ATLENSURE_RETURN_VAL( pData != NULL, false);

		DBORDINAL nParam = 0;
		if( !_GetParameterNo( pParamName, nParam ) )
			return NULL;

		ctype* pBuffer = (ctype*)_GetParam(nParam);
		if (pBuffer == NULL)
			return false;

		*pData = *pBuffer;
		return true;
	}
	ATLPREFAST_UNSUPPRESS()

	template <class ctype>
	bool SetParam(
		_In_z_ TCHAR* pParamName,
		_In_ const ctype* pData,
		_In_ DBSTATUS status = DBSTATUS_S_OK) throw()
	{
		ATLASSERT( pData != NULL );

		DBORDINAL nParam = 0;
		if( !_GetParameterNo( pParamName, nParam ) )
			return NULL;

		ctype* pBuffer = (ctype*)_GetParam(nParam);
		if (pBuffer == NULL)
			return false;

		*pBuffer = *pData;

		DBSTATUS *pStatus = _GetParamStatus( nParam );
		if( pStatus != NULL )
			*pStatus = status;
		//DBLENGTH *pLength = _GetParamLength( nParam );
		//if( pLength != NULL )
		//	*pLength = sizeof(ctype);

		return true;
	}

	void* GetParam(_In_ DBORDINAL nParam) const throw()
	{
		if( !TranslateParameterNo( nParam ) )
			return NULL;

		return _GetParam( nParam );
	}

	DBLENGTH* GetParamLength(_In_ DBORDINAL nParam) const throw()
	{
		if( !TranslateParameterNo( nParam ) )
			return NULL;

		return _GetParamLength( nParam );
	}

	DBSTATUS* GetParamStatus(_In_ DBORDINAL nParam) const throw()
	{
		if( !TranslateParameterNo( nParam ) )
			return NULL;

		return _GetParamStatus( nParam );
	}

	void* GetParam(_In_z_ TCHAR* pParamName) const throw()
	{
		ATLENSURE_RETURN_VAL( pParamName, NULL);
		DBORDINAL nParam = 0;
		if( !_GetParameterNo( pParamName, nParam ) )
			return NULL;

		return _GetParam( nParam );
	}

	// Get the number of parameters
	DB_UPARAMS GetParamCount() const throw()
	{
		return m_nParams;
	}

	// Get the parameter name for the passed parameter number
	LPOLESTR GetParamName(_In_ DBORDINAL nParam) const throw()
	{
		if( !TranslateParameterNo( nParam ) )
			return NULL;

		return m_ppParamName[nParam];
	}

	bool TranslateParameterNo(_Inout_ DBORDINAL& nParam) const throw()
	{
		for( DBORDINAL i = 0; i < m_nParams; i++ )
		{
			if( m_pParameterEntry[i].iOrdinal == nParam )
			{
				nParam = i;
				return true;
			}
		}

		return false;
	}

	_Success_(return != false)
	bool _GetParameterNo(
		_In_z_ TCHAR* pParamName,
		_Out_ DBORDINAL& nParam) const throw()
	{
		if( pParamName == NULL )
			return false;

		DBORDINAL i;
		size_t nSize = (_tcslen(pParamName) + 1) * sizeof(OLECHAR);
		CComBSTR  bstrParamName(pParamName);

		for (i=0; i<m_nParams; i++)
		{
			if (memcmp(m_ppParamName[i], bstrParamName.m_str, nSize) == 0)
			{
				nParam = i;
				return true;
			}
		}

		return false;    // Not Found
	}

	void* _GetParam(_In_ DBORDINAL nParam) const throw()
	{
		return m_pParameterBuffer + m_pParameterEntry[nParam].obValue;
	}

	DBLENGTH* _GetParamLength(_In_ DBORDINAL nParam) const throw()
	{
		if( m_pParameterEntry[nParam].obLength == 0 )
			return NULL;
		else
			return (DBLENGTH*)(m_pParameterBuffer + m_pParameterEntry[nParam].obLength);
	}

	DBSTATUS* _GetParamStatus(_In_ DBORDINAL nParam) const throw()
	{
		if( m_pParameterEntry[nParam].obStatus == 0 )
			return NULL;
		else
			return (DBSTATUS*)(m_pParameterBuffer + m_pParameterEntry[nParam].obStatus);
	}

// Implementation
ATLPREFAST_SUPPRESS(6101)
	HRESULT BindParameters(
		_In_ HACCESSOR* pHAccessor,
		_Inout_ ICommand* pCommand,
		_Outptr_ void** ppParameterBuffer,
		_In_ bool fBindLength = false,
		_In_ bool fBindStatus = false) throw()
	{
		// If we have already bound the parameters then just return
		// the pointer to the parameter buffer
		if (*pHAccessor != 0)
		{
			*ppParameterBuffer = m_pParameterBuffer;
			return S_OK;
		}

		CComPtr<IAccessor> spAccessor;
		ATLENSURE_RETURN(pCommand != NULL);
		HRESULT hr = pCommand->QueryInterface(&spAccessor);
		if (FAILED(hr))
			return hr;

		// Try to bind parameters if available
		CComPtr<ICommandWithParameters> spCommandParameters;
		hr = pCommand->QueryInterface(&spCommandParameters);
		if (FAILED(hr))
			return hr;

		DB_UPARAMS ulParams     = 0;
		CComHeapPtr<DBPARAMINFO>    spParamInfo;
		LPOLESTR pNamesBuffer;

		// Get Parameter Information
		hr = spCommandParameters->GetParameterInfo(&ulParams, &spParamInfo,
				&pNamesBuffer);
		if (FAILED(hr))
			return hr;

		// Create the parameter information for binding
		hr = AllocateParameterInfo(ulParams);
		if (FAILED(hr))
		{
			CoTaskMemFree(pNamesBuffer);
			return hr;
		}

		DBBYTEOFFSET nOffset = 0;
		DBBYTEOFFSET nDataOffset = 0;
		DBBYTEOFFSET nLengthOffset = 0;
		DBBYTEOFFSET nStatusOffset = 0;

		DBBINDING* pCurrent = m_pParameterEntry;
		for (ULONG l=0; l<ulParams; l++)
		{
			m_pParameterEntry[l].eParamIO = 0;

			if (spParamInfo[l].dwFlags & DBPARAMFLAGS_ISINPUT)
				m_pParameterEntry[l].eParamIO |= DBPARAMIO_INPUT;

			if (spParamInfo[l].dwFlags & DBPARAMFLAGS_ISOUTPUT)
				m_pParameterEntry[l].eParamIO |= DBPARAMIO_OUTPUT;

			// if this is a BLOB, truncate column length to m_nBlobSize (like 8000 bytes)
			if( spParamInfo[l].ulParamSize > m_nBlobSize )
				spParamInfo[l].ulParamSize = m_nBlobSize;

			// if this is a string, recalculate column size in bytes
			DBLENGTH colLength = spParamInfo[l].ulParamSize;
			if (spParamInfo[l].wType == DBTYPE_STR)
				colLength += 1;
			if (spParamInfo[l].wType == DBTYPE_WSTR)
				colLength = colLength*2 + 2;

			// Calculate the column data offset
			nDataOffset = AlignAndIncrementOffset( nOffset, colLength, GetAlignment( spParamInfo[l].wType ) );

			if( fBindLength )
			{
				// Calculate the column length offset
				nLengthOffset = AlignAndIncrementOffset( nOffset, sizeof(DBLENGTH), __alignof(DBLENGTH) );
			}

			if( fBindStatus )
			{
				// Calculate the column status offset
				nStatusOffset = AlignAndIncrementOffset( nOffset, sizeof(DBSTATUS), __alignof(DBSTATUS) );
			}

			Bind(pCurrent, spParamInfo[l].iOrdinal, spParamInfo[l].wType,
				colLength, spParamInfo[l].bPrecision, spParamInfo[l].bScale,
				m_pParameterEntry[l].eParamIO, nDataOffset, nLengthOffset, nStatusOffset );

			pCurrent++;

			m_ppParamName[l] = pNamesBuffer;
			if (pNamesBuffer && *pNamesBuffer)
			{
				// Search for the NULL termination character
				while (*pNamesBuffer++)
					;
			}
		}

		// Allocate memory for the new buffer
		m_pParameterBuffer = _ATL_NEW BYTE[nOffset];
		if (m_pParameterBuffer == NULL)
		{
			// Note that pNamesBuffer will be freed in the destructor
			// by freeing *m_ppParamName
			return E_OUTOFMEMORY;
		}
		*ppParameterBuffer = m_pParameterBuffer;
		m_nParameterBufferSize = nOffset;
		m_nParams = ulParams;
		BindEntries(m_pParameterEntry, ulParams, pHAccessor, nOffset, spAccessor);

		return S_OK;
	}
ATLPREFAST_UNSUPPRESS()
	bool HasParameters() const throw()
	{
		return true;
	}
	HRESULT AllocateParameterInfo(_In_ DB_UPARAMS nParamEntries) throw()
	{
		// Allocate memory for the bind structures
		m_pParameterEntry = _ATL_NEW DBBINDING[nParamEntries];
		if (m_pParameterEntry == NULL)
			return E_OUTOFMEMORY;

		// Allocate memory to store the field names
		m_ppParamName = _ATL_NEW OLECHAR*[nParamEntries];
		if (m_ppParamName == NULL)
		{
			delete [] m_pParameterEntry;
			m_pParameterEntry = NULL;
			return E_OUTOFMEMORY;
		}
		return S_OK;
	}

// Data Members
	// Number of parameters
	DB_UPARAMS          m_nParams;
	// A pointer to the entry structures for each parameter
	DBBINDING*          m_pParameterEntry;
	// String names for the parameters
	OLECHAR**           m_ppParamName;
	// The size of the buffer where the parameters are stored
	DBLENGTH            m_nParameterBufferSize;
	// A pointer to the buffer where the parameters are stored
	BYTE*               m_pParameterBuffer;
};


///////////////////////////////////////////////////////////////////////////
// class CManualAccessor

class CManualAccessor :
	public CAccessorBase
{
public:
	CManualAccessor()
	{
		// By default we don't have any parameters unless CreateParameterAccessor
		// is called
		m_pEntry          = NULL;
		m_nParameters     = 0;
		m_pParameterEntry = NULL;
		m_nColumns        = 0;
		m_pParameterBuffer = NULL;
	}
	~CManualAccessor()
	{
		delete [] m_pEntry;
		delete [] m_pParameterEntry;
	}

	HRESULT ReleaseAccessors(_Inout_ IUnknown* pUnk) throw()
	{
		FreeRecordMemory( (IRowset*) pUnk );
		return CAccessorBase::ReleaseAccessors(pUnk);
	}

	HRESULT CreateAccessor(
		_In_ int nBindEntries,
		_Out_writes_bytes_(nBufferSize) void* pBuffer,
		_In_ DBLENGTH nBufferSize) throw()
	{
		m_pBuffer     = (BYTE*)pBuffer;
		m_nBufferSize = nBufferSize;
		m_nColumns    = 0;
		m_nEntry      = 0;
		memset(pBuffer, 0, nBufferSize);

		// If they've previously created some entries then free them
		delete [] m_pEntry;

		// Allocate memory for the bind structures
		m_pEntry = _ATL_NEW DBBINDING[nBindEntries];
		if (m_pEntry == NULL)
			return E_OUTOFMEMORY;

		m_nColumns    = nBindEntries;

		return S_OK;
	}
	HRESULT CreateParameterAccessor(
		_In_ int nBindEntries,
		_In_reads_bytes_(nBufferSize) void* pBuffer,
		_In_ DBLENGTH nBufferSize) throw()
	{
		// Should be called only once.  But, if you really insist on doing this...
		if (m_pParameterEntry != NULL)
		{
			delete [] m_pParameterEntry;
			m_pParameterEntry = NULL;
		}

		m_pParameterBuffer     = (BYTE*)pBuffer;
		m_nParameterBufferSize = nBufferSize;
		m_nParameters          = 0;
		m_nCurrentParameter    = 0;

		// Allocate memory for the bind structures
		m_pParameterEntry = _ATL_NEW DBBINDING[nBindEntries];
		if (m_pParameterEntry == NULL)
			return E_OUTOFMEMORY;

		m_nParameters          = nBindEntries;

		return S_OK;
	}
	void AddBindEntry(
		_In_ DBORDINAL nOrdinal,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nColumnSize,
		_In_ void* pData,
		_In_opt_ void* pLength = NULL,
		_In_opt_ void* pStatus = NULL) throw()
	{
		ATLASSUME(m_nEntry < m_nColumns);
		DBBYTEOFFSET   nLengthOffset, nStatusOffset;

		if (pStatus != NULL)
			nStatusOffset = (BYTE*)pStatus - m_pBuffer;
		else
			nStatusOffset = 0;

		if (pLength != NULL)
			nLengthOffset = (BYTE*)pLength - m_pBuffer;
		else
			nLengthOffset = 0;

		Bind(m_pEntry+m_nEntry, nOrdinal, wType, nColumnSize, 0, 0, DBPARAMIO_NOTPARAM,
			(BYTE*)pData - m_pBuffer, nLengthOffset, nStatusOffset);

		m_nEntry++;
	}
	void AddParameterEntry(
		_In_ DBORDINAL nOrdinal,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nColumnSize,
		_In_ void* pData,
		_In_opt_ void* pLength = NULL,
		_In_opt_ void* pStatus = NULL,
		_In_ DBPARAMIO eParamIO = DBPARAMIO_INPUT) throw()
	{
		ATLASSUME(m_nCurrentParameter < m_nParameters);
		DBBYTEOFFSET nLengthOffset, nStatusOffset;

		if (pStatus != NULL)
			nStatusOffset = (BYTE*)pStatus - m_pParameterBuffer;
		else
			nStatusOffset = 0;

		if (pLength != NULL)
			nLengthOffset = (BYTE*)pLength - m_pParameterBuffer;
		else
			nLengthOffset = 0;

		Bind(m_pParameterEntry + m_nCurrentParameter, nOrdinal, wType, nColumnSize, 0, 0,
			eParamIO, (BYTE*)pData - m_pParameterBuffer, nLengthOffset, nStatusOffset);

		m_nCurrentParameter++;
	}

// Implementation
	// Free's any columns in the current record that need to be freed.
	// E.g. Calls SysFreeString on any BSTR's and Release on any interfaces.
	void FreeRecordMemory(_Inout_ IRowset* pRowset) throw ()
	{
		ULONG i;

		for (i = 0; i < m_nColumns; i++)
			CAccessorBase::FreeType(m_pEntry[i].wType, m_pBuffer + m_pEntry[i].obValue, pRowset);
	}

	void ClearRecordMemory() throw()
	{
		memset(m_pBuffer, 0, m_nBufferSize);
	}

	HRESULT BindColumns(_Inout_ IUnknown* pUnk) throw()
	{
		ATLENSURE_RETURN(pUnk != NULL);
		CComPtr<IAccessor> spAccessor;
		HRESULT hr = pUnk->QueryInterface(&spAccessor);
		if (FAILED(hr))
			return hr;

		// Allocate the accessor memory if we haven't done so yet
		if (m_pAccessorInfo == NULL)
		{
			hr = AllocateAccessorMemory(1); // We only have one accessor
			if (FAILED(hr))
				return hr;
			_Analysis_assume_(m_pAccessorInfo != NULL);
			m_pAccessorInfo->bAutoAccessor = true;
		}

		return BindEntries(m_pEntry, m_nColumns, &m_pAccessorInfo->hAccessor, m_nBufferSize, spAccessor);
	}

	HRESULT BindParameters(
		_In_ HACCESSOR* pHAccessor,
		_Inout_ ICommand* pCommand,
		_Outptr_ void** ppParameterBuffer) throw()
	{
		HRESULT hr;
		ATLENSURE_RETURN(ppParameterBuffer);
		*ppParameterBuffer = m_pParameterBuffer;

		// Only bind the parameter if we haven't done so yet
		if (*pHAccessor == 0)
		{
			// Get the IAccessor from the passed IUnknown
			CComPtr<IAccessor> spAccessor;
			ATLENSURE_RETURN(pCommand != NULL);
			hr = pCommand->QueryInterface(&spAccessor);
			if (SUCCEEDED(hr))
			{
				hr = BindEntries(m_pParameterEntry, m_nParameters, pHAccessor,
						m_nParameterBufferSize, spAccessor);
			}
		}
		else
			hr = S_OK;

		return hr;
	}

	typedef CManualAccessor _ParamClass;
	typedef CManualAccessor _OutputColumnsClass;

	bool HasParameters() throw()
	{
		return (m_nParameters > 0);
	}
	bool HasOutputColumns() throw()
	{
		return (m_nColumns > 0);
	}
	DBORDINAL GetColumnCount() const throw()
	{
		return m_nColumns;
	}

	// The binding structure for the output columns
	DBBINDING*          m_pEntry;
	// The number of output columns
	DBORDINAL           m_nColumns;
	// The number of the current entry for the output columns
	DBORDINAL           m_nEntry;
	// The size of the data buffer for the output columns
	DBLENGTH            m_nBufferSize;
	// The number of parameters columns
	DBORDINAL           m_nParameters;
	// The number of the parameter column to bind next
	DBORDINAL           m_nCurrentParameter;
	// A pointer to the entry structures for each parameter
	DBBINDING*          m_pParameterEntry;
	// The size of the buffer where the parameters are stored
	DBLENGTH            m_nParameterBufferSize;
	// A pointer to the buffer where the parameters are stored
	BYTE*               m_pParameterBuffer;
};

class _ATL_COLUMN_PARAM_INFO
{
public:
	_ATL_COLUMN_PARAM_INFO()
	{
		m_pParams = NULL;
		m_pBuffer = NULL;
	}
	~_ATL_COLUMN_PARAM_INFO()
	{
		delete m_pParams;
	}
	BOOL AddBinding(_In_ DBBINDING& binding)
	{
		return m_rgBinding.Add(binding);
	}

	CSimpleArray<DBBINDING, CSimpleArrayEqualHelperFalse<DBBINDING> >	m_rgBinding;	// The binding for each parameter
	DBPARAMS*				m_pParams;
	BYTE*					m_pBuffer;
};

///////////////////////////////////////////////////////////////////////////
// CColumnAccessor
class CColumnAccessor :
	public CAccessorBase
{
public:
	CColumnAccessor()
	{
		m_pParamInfo = NULL;
	}
	~CColumnAccessor()
	{
		delete m_pParamInfo;
	}

	HRESULT ReleaseAccessors(_Inout_ IUnknown* pUnk) throw()
	{
		FreeRecordMemory( (IRowset*) pUnk );
		return CAccessorBase::ReleaseAccessors(pUnk);
	}

	// pUnk is the interface the accessor will be created upon
	HRESULT CreateAccessor(
		_Inout_ IUnknown* pUnk,
		_In_ DBORDINAL nOrdinal,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nColumnSize,
		_In_ BYTE nPrecision,
		_In_ BYTE nScale,
		_In_ void* pData) throw()
	{
		(nPrecision);
		(nScale);
		ATLENSURE_RETURN(pUnk != NULL);
		DBBINDING binding;

		Bind(&binding, nOrdinal, wType, nColumnSize, 0, 0, DBPARAMIO_NOTPARAM,
			(BYTE*)pData - m_pBuffer);

		CComPtr<IAccessor> spAccessor;
		HRESULT hr = pUnk->QueryInterface(&spAccessor);
		if (FAILED(hr))
			return hr;

		// Add another accessor info structure
		AddAccessorInfo();
		_ATL_ACCESSOR_INFO* pAccessorInfo = m_pAccessorInfo + m_nAccessors - 1;

		pAccessorInfo->bAutoAccessor = true;	// Always auto

		// Bind it
		return BindEntries(&binding, 1, &pAccessorInfo->hAccessor, nColumnSize, spAccessor);
	}
	// Bind columns doesn't have to do anything here as we bind each accessor when
	// CreateAccessor is called
	HRESULT BindColumns(_In_opt_ IUnknown*) throw()
	{
		return S_OK;
	}
	HRESULT SetParameterBuffer(_In_opt_ BYTE* pBuffer) throw()
	{
		// This should only be called once.
		ATLASSUME(m_pParamInfo == NULL);

		m_pParamInfo = _ATL_NEW _ATL_COLUMN_PARAM_INFO;
		if (m_pParamInfo == NULL)
			return E_OUTOFMEMORY;

		m_pParamInfo->m_pBuffer = pBuffer;
		return S_OK;
	}
	HRESULT AddParameter(
		_In_ DBPARAMIO paramio,
		_In_ DBORDINAL nOrdinal,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nColumnSize,
		_In_ BYTE /* nPrecision*/,
		_In_ BYTE /* nScale */,
		_In_ void* pData) throw()
	{
		ATLASSUME(m_pParamInfo != NULL);

		DBBINDING binding;

		Bind(&binding, nOrdinal, wType, nColumnSize, 0, 0, paramio,
			(BYTE*)pData - m_pParamInfo->m_pBuffer);

		if (m_pParamInfo->AddBinding(binding) == FALSE)
			return E_OUTOFMEMORY;

		return S_OK;
	}
	bool HasOutputColumns() throw()
	{
		if( m_nAccessors )
			return true;
		else
			return false;
	}
	bool HasParameters() const throw()
	{
		return (m_pParamInfo != NULL) ? true : false;
	}
	// Called to bind the parameters created
	HRESULT BindParameters(
		_In_ HACCESSOR* pHAccessor,
		_Inout_ ICommand* pCommand,
		_When_(*pHAccessor == 0, _Outptr_) void** ppParameterBuffer)
	{
		ATLASSUME(m_pParamInfo != NULL);
		HRESULT hr = S_OK;

		// Only bind the parameters if we haven't already done it
		if (*pHAccessor == 0)
		{
			// Get the IAccessor from the passed ICommand
			CComPtr<IAccessor> spAccessor;
			ATLENSURE_RETURN(pCommand != NULL);
			hr = pCommand->QueryInterface(&spAccessor);
			if (SUCCEEDED(hr))
			{
				*ppParameterBuffer = m_pParamInfo->m_pBuffer;
				// size isn't being passed here
				hr = BindEntries(&m_pParamInfo->m_rgBinding[0], m_pParamInfo->m_rgBinding.GetSize(),
					pHAccessor, 4, spAccessor);
			}

		}
		return hr;
	}
// Implementation
	typedef CColumnAccessor _ParamClass;
	typedef CColumnAccessor _OutputColumnsClass;

	HRESULT AddAccessorInfo() throw()
	{
		_ATL_ACCESSOR_INFO* pAccessorInfo = NULL;
		pAccessorInfo = _ATL_NEW _ATL_ACCESSOR_INFO[m_nAccessors + 1];
		if (pAccessorInfo == NULL)
			return E_OUTOFMEMORY;

		// Now copy the current accessor information to the new buffer
		Checked::memcpy_s(pAccessorInfo, sizeof(_ATL_ACCESSOR_INFO) * m_nAccessors, m_pAccessorInfo, sizeof(_ATL_ACCESSOR_INFO) * m_nAccessors);
		m_nAccessors++;

		// Now delete the old memory and use the new one
		delete [] m_pAccessorInfo;
		m_pAccessorInfo = pAccessorInfo;
		return S_OK;
	}
	_ATL_COLUMN_PARAM_INFO* m_pParamInfo;
};

///////////////////////////////////////////////////////////////////////////
// class CRowset

template <class TAccessor = CAccessorBase>
class CRowset
{
// Constructors and Destructors
public:
	CRowset()
	{
		m_pXMLAccessor = NULL;
		m_pAccessor = NULL;
		m_hRow      = 0;
	}
	CRowset(_In_opt_ IRowset* pRowset)
	{
		m_pXMLAccessor = NULL;
		m_pAccessor = NULL;
		m_hRow      = 0;
		m_spRowset  = pRowset;
	}
	~CRowset()
	{
		Close();
	}

	HRESULT GetXMLColumnInfo(_Inout_ CSimpleStringW& strOutput) throw()
	{
		ATLASSUME(m_spRowset != NULL);
		HRESULT hr;
		if( m_pXMLAccessor == NULL )
		{
			m_pXMLAccessor = _ATL_NEW CXMLAccessor;
			if( m_pXMLAccessor == NULL )
				return E_OUTOFMEMORY;
			hr = m_pXMLAccessor->BindColumns( m_spRowset );
			if( FAILED(hr) )
			{
				delete m_pXMLAccessor;
				m_pXMLAccessor = NULL;
				return hr;
			}
		}

		ATLASSUME( m_pXMLAccessor != NULL );

		return m_pXMLAccessor->GetXMLColumnData( strOutput );
	}

	HRESULT GetXMLRow(
		_Inout_ CSimpleStringW& strOutput,
		_In_ bool bAppend = false) throw()
	{
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_hRow != 0);

		HRESULT hr;
		if( m_pXMLAccessor == NULL )
		{
			m_pXMLAccessor = _ATL_NEW CXMLAccessor;
			if( m_pXMLAccessor == NULL )
				return E_OUTOFMEMORY;
			hr = m_pXMLAccessor->BindColumns( m_spRowset );
			if( FAILED(hr) )
			{
				delete m_pXMLAccessor;
				m_pXMLAccessor = NULL;
				return hr;
			}
		}

		ATLASSUME( m_pXMLAccessor != NULL );

		hr = m_spRowset->GetData(m_hRow, m_pXMLAccessor->GetHAccessor(0), m_pXMLAccessor->GetBuffer());
		if( FAILED(hr) )
			return hr;

		hr = m_pXMLAccessor->GetXMLRowData( strOutput, bAppend );

		m_pXMLAccessor->FreeRecordMemory( GetInterface() );
		return hr;
	}

	// Release any retrieved row handles and then release the rowset
	void Close() throw()
	{
		if( m_pXMLAccessor != NULL )
		{
			if (m_spRowset != NULL)
				m_pXMLAccessor->ReleaseAccessors( m_spRowset );
			delete m_pXMLAccessor;
			m_pXMLAccessor = NULL;
		}
		if (m_spRowset != NULL)
		{
			m_pAccessor->FreeRecordMemory(m_spRowset);
			ReleaseRows();
			m_spRowset.Release();
			m_spRowsetChange.Release();
		}
	}
	// Addref the current row
	HRESULT AddRefRows() throw()
	{
		ATLASSUME(m_spRowset != NULL);
		return m_spRowset->AddRefRows(1, &m_hRow, NULL, NULL);
	}
	// Release the current row
	HRESULT ReleaseRows() throw()
	{
		ATLASSUME(m_spRowset != NULL);
		HRESULT hr = S_OK;

		if (m_hRow != 0)
		{
			hr = m_spRowset->ReleaseRows(1, &m_hRow, NULL, NULL, NULL);
			m_hRow = 0;
		}
		return hr;
	}

	CRowset<>* GetRowsetBase() throw()
	{
		return (CRowset<>*)this;
	}

	// Compare two bookmarks with each other
	HRESULT Compare(
		_In_ const CBookmarkBase& bookmark1,
		_In_ const CBookmarkBase& bookmark2,
		_Out_ DBCOMPARE* pComparison) const throw()
	{
		ATLASSUME(m_spRowset != NULL);
		CComPtr<IRowsetLocate> spLocate;
		HRESULT hr = m_spRowset.QueryInterface(&spLocate);
		if (FAILED(hr))
			return hr;

		return spLocate->Compare(NULL, bookmark1.GetSize(), bookmark1.GetBuffer(),
			bookmark2.GetSize(), bookmark2.GetBuffer(), pComparison);
	}

	// Compare the passed hRow with the current row
	HRESULT IsSameRow(_In_ HROW hRow) const throw()
	{
		ATLASSUME(m_spRowset != NULL);
		if (m_hRow == hRow)
			return S_OK;

		CComPtr<IRowsetIdentity> spRowsetIdentity;
		HRESULT hr = m_spRowset.QueryInterface(&spRowsetIdentity);
		if (FAILED(hr))
			return hr;

		return spRowsetIdentity->IsSameRow(m_hRow, hRow);
	}

	// Move to the previous record
	HRESULT MovePrev() throw()
	{
		// the following line of code may fail if the DBPROP_CANFETCHBACKWARDS and/or
		// DBPROP_CANSCROLLBACKWARDS properties have not been set.
		return MoveNext(-2);
	}

	// Move to the next record
	HRESULT MoveNext() throw()
	{
		return MoveNext(0);
	}

	// Move lSkip records forward or backward
	HRESULT MoveNext(
		_In_ LONG lSkip,
		_In_ bool bForward = true) throw()
	{
		HRESULT hr;
		DBCOUNTITEM ulRowsFetched = 0;

		// Check the data was opened successfully and the accessor
		// has been set.
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);

		m_pAccessor->FreeRecordMemory(m_spRowset);
		// Release a row if one is already around
		ReleaseRows();

		// Get the row handle
		HROW* phRow = &m_hRow;
		hr = m_spRowset->GetNextRows(0, lSkip, (bForward) ? 1 : -1, &ulRowsFetched, &phRow);
		if (hr != S_OK)
			return hr;

		// Get the data
		hr = GetData();
		if (FAILED(hr))
		{
			ATLTRACE(atlTraceDBClient, 0, _T("GetData failed - HRESULT = 0x%X\n"),hr);
			ReleaseRows();
		}
		return hr;
	}

	// Move to the first record
	HRESULT MoveFirst() throw()
	{
		HRESULT hr;

		// Check the data was opened successfully and the accessor
		// has been set.
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);

		m_pAccessor->FreeRecordMemory(m_spRowset);
		// Release a row if one is already around
		ReleaseRows();

		// the call to RestartPosition may fail if the DBPROP_CANFETCHBACKWARDS and/or
		// DBPROP_CANSCROLLBACKWARDS properties have not been set.
		hr = m_spRowset->RestartPosition(NULL);
		if (FAILED(hr))
			return hr;

		// Get the data
		return MoveNext();
	}

	// Move to the last record
	HRESULT MoveLast() throw()
	{
		// Check the data was opened successfully and the accessor
		// has been set.
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);

		// Release a row if one is already around
		m_pAccessor->FreeRecordMemory(m_spRowset);
		ReleaseRows();

		HRESULT hr;
		DBCOUNTITEM ulRowsFetched = 0;
		HROW* phRow = &m_hRow;

		// Restart the rowset position and then move backwards

		// the call to RestartPosition may fail if the DBPROP_CANFETCHBACKWARDS and/or
		// DBPROP_CANSCROLLBACKWARDS properties have not been set.
		m_spRowset->RestartPosition(NULL);
		hr = m_spRowset->GetNextRows(NULL, -1, 1, &ulRowsFetched, &phRow);
		if (hr != S_OK)
			return hr;

		// Get the data
		hr = GetData();
		if (FAILED(hr))
		{
			ATLTRACE(atlTraceDBClient, 0, _T("GetData from MoveLast failed - HRESULT = 0x%X\n"),hr);
			ReleaseRows();
		}

		return S_OK;
	}
	// Move to the passed bookmark
	HRESULT MoveToBookmark(
		_In_ const CBookmarkBase& bookmark,
		_In_ LONG lSkip = 0) throw()
	{
		// Check the data was opened successfully and the accessor
		// has been set.
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);

		CComPtr<IRowsetLocate> spLocate;
		HRESULT hr = m_spRowset.QueryInterface(&spLocate);
		if (FAILED(hr))
			return hr;

		m_pAccessor->FreeRecordMemory(m_spRowset);
		// Release a row if one is already around
		ReleaseRows();

		DBCOUNTITEM ulRowsFetched = 0;
		HROW* phRow = &m_hRow;
		hr = spLocate->GetRowsAt(NULL, NULL, bookmark.GetSize(), bookmark.GetBuffer(),
			lSkip, 1, &ulRowsFetched, &phRow);
		// Note we're not using SUCCEEDED here, because we could get DB_S_ENDOFROWSET
		if (hr != S_OK)
			return hr;

		// Get the data
		hr = GetData();
		if (FAILED(hr))
		{
			ATLTRACE(atlTraceDBClient, 0, _T("GetData from Bookmark failed - HRESULT = 0x%X\n"),hr);
			ReleaseRows();
		}

		return S_OK;
	}

	// Get the data for the current record
	HRESULT GetData() throw()
	{
		HRESULT hr = S_OK;
		ATLASSUME(m_pAccessor != NULL);

		ULONG nAccessors = m_pAccessor->GetNumAccessors();
		for (ULONG i=0; i<nAccessors; i++)
		{
			if (m_pAccessor->IsAutoAccessor(i))
			{
				hr = GetData(i);
				if (FAILED(hr))
					return hr;
			}
		}
		return hr;
	}

	// Get the data for the passed accessor. Use for a non-auto accessor
	HRESULT GetData(_In_ int nAccessor) throw()
	{
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);
		ATLASSUME(m_hRow != 0);

		// Note that we are using the specified buffer if it has been set,
		// otherwise we use the accessor for the data.
		return m_spRowset->GetData(m_hRow, m_pAccessor->GetHAccessor(nAccessor), m_pAccessor->GetBuffer());
	}

	// Get the data for the passed accessor. Use for a non-auto accessor
	HRESULT GetDataHere(
		_In_ int nAccessor,
		_Out_ void* pBuffer) throw()
	{
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);
		ATLASSUME(m_hRow != 0);

		// Note that we are using the specified buffer if it has been set,
		// otherwise we use the accessor for the data.
		return m_spRowset->GetData(m_hRow, m_pAccessor->GetHAccessor(nAccessor), pBuffer);
	}

	HRESULT GetDataHere(_Out_ void* pBuffer) throw()
	{
		HRESULT hr = S_OK;

		ULONG nAccessors = m_pAccessor->GetNumAccessors();
		_Analysis_assume_(nAccessors);
		for (ULONG i=0; i<nAccessors; i++)
		{
			hr = GetDataHere(i, pBuffer);
			if (FAILED(hr))
				return hr;
		}
		return hr;
	}

	// Insert the current record
	HRESULT Insert(
		_In_ int nAccessor = 0,
		_In_ bool bGetHRow = false) throw()
	{
		ATLASSUME(m_pAccessor != NULL);
		HRESULT hr;
		if (m_spRowsetChange != NULL)
		{
			HROW* pHRow;
			if (bGetHRow)
			{
				ReleaseRows();
				pHRow = &m_hRow;
			}
			else
				pHRow = NULL;

			hr = m_spRowsetChange->InsertRow(NULL, m_pAccessor->GetHAccessor(nAccessor),
					m_pAccessor->GetBuffer(), pHRow);

		}
		else
			hr = E_NOINTERFACE;

		return hr;
	}

	// Delete the current record
	HRESULT Delete() const throw()
	{
		ATLASSUME(m_pAccessor != NULL);
		HRESULT hr;
		if (m_spRowsetChange != NULL)
			hr = m_spRowsetChange->DeleteRows(NULL, 1, &m_hRow, NULL);
		else
			hr = E_NOINTERFACE;

		return hr;
	}

	// Update the current record
	HRESULT SetData() const throw()
	{
		ATLASSUME(m_pAccessor != NULL);
		HRESULT hr = S_OK;

		ULONG nAccessors = m_pAccessor->GetNumAccessors();
		for (ULONG i=0; i<nAccessors; i++)
		{
			hr = SetData(i);
			if (FAILED(hr))
				return hr;
		}
		return hr;
	}

	// Update the current record with the data in the passed accessor
	HRESULT SetData(_In_ int nAccessor) const throw()
	{
		ATLASSUME(m_pAccessor != NULL);
		HRESULT hr;
		if (m_spRowsetChange != NULL)
		{
			hr = m_spRowsetChange->SetData(m_hRow, m_pAccessor->GetHAccessor(nAccessor),
				m_pAccessor->GetBuffer());
		}
		else
			hr = E_NOINTERFACE;

		return hr;
	}

	// Get the data most recently fetched from or transmitted to the data source.
	// Does not get values based on pending changes.
	HRESULT GetOriginalData() throw()
	{
		ATLASSUME(m_spRowset != NULL);
		ATLASSUME(m_pAccessor != NULL);

		HRESULT hr = S_OK;
		CComPtr<IRowsetUpdate> spRowsetUpdate;
		hr = m_spRowset->QueryInterface(&spRowsetUpdate);
		if (FAILED(hr))
			return hr;

		ULONG nAccessors = m_pAccessor->GetNumAccessors();
		for (ULONG i = 0; i < nAccessors; i++)
		{
			hr = spRowsetUpdate->GetOriginalData(m_hRow, m_pAccessor->GetHAccessor(i), m_pAccessor->GetBuffer());
			if (FAILED(hr))
				return hr;
		}
		return hr;
	}

	// Get the status of the current row
	HRESULT GetRowStatus(_Out_ DBPENDINGSTATUS* pStatus) const throw()
	{
		ATLASSUME(m_spRowset != NULL);
		ATLASSERT(pStatus != NULL);

		CComPtr<IRowsetUpdate> spRowsetUpdate;
		HRESULT hr = m_spRowset->QueryInterface(&spRowsetUpdate);
		if (FAILED(hr))
			return hr;

		return spRowsetUpdate->GetRowStatus(NULL, 1, &m_hRow, pStatus);
	}

	// Undo any changes made to the current row since it was last fetched or Update
	// was called for it
	HRESULT Undo(
		_In_opt_ DBCOUNTITEM* pcRows = NULL,
		_Out_opt_ HROW* phRow = NULL,
		_Out_opt_ DBROWSTATUS* pStatus = NULL) throw()
	{
		ATLASSUME(m_spRowset != NULL);

		CComPtr<IRowsetUpdate> spRowsetUpdate;
		HRESULT hr = m_spRowset->QueryInterface(&spRowsetUpdate);
		if (FAILED(hr))
			return hr;

		CComHeapPtr<HROW> sprgRows;
		CComHeapPtr<DBROWSTATUS> spRowStatus;

		if (phRow != NULL)
			hr = spRowsetUpdate->Undo(NULL, 1, &m_hRow, pcRows, &sprgRows, &spRowStatus);
		else
			hr = spRowsetUpdate->Undo(NULL, 1, &m_hRow, pcRows, NULL, &spRowStatus);

		if (phRow != NULL && sprgRows != NULL)
			*phRow = *sprgRows;
		else
		{
			_Analysis_assume_(phRow == NULL);
		}

		if (pStatus != NULL && spRowStatus != NULL)
			*pStatus = *spRowStatus;
		else
		{
			_Analysis_assume_(pStatus == NULL);
		}

		return hr;
	}

	// Transmits any pending changes made to a row since it was last fetched or Update was
	// called for it. Also see SetData.
	HRESULT Update(
		_In_opt_ DBCOUNTITEM* pcRows = NULL,
		_Out_opt_ HROW* phRow = NULL,
		_Out_opt_ DBROWSTATUS* pStatus = NULL) throw()
	{
		ATLASSUME(m_spRowset != NULL);

		CComPtr<IRowsetUpdate> spRowsetUpdate;
		HRESULT hr = m_spRowset->QueryInterface(&spRowsetUpdate);
		if (FAILED(hr))
			return hr;

		CComHeapPtr<HROW>           sprgRows;
		CComHeapPtr<DBROWSTATUS>    spRowStatus;

		if (phRow != NULL)
			hr = spRowsetUpdate->Update(NULL, 1, &m_hRow, pcRows, &sprgRows, &spRowStatus);
		else
			hr = spRowsetUpdate->Update(NULL, 1, &m_hRow, pcRows, NULL, &spRowStatus);

		if (phRow != NULL && sprgRows != NULL)
			*phRow = *sprgRows;
		else
		{
			_Analysis_assume_(phRow == NULL);
		}

		if (pStatus != NULL && spRowStatus != NULL)
			*pStatus = *spRowStatus;
		else
		{
			_Analysis_assume_(pStatus == NULL);
		}

		return hr;
	}

	// Transmits any pending changes to all rows made since it was last fetched or Update was
	// called for it.  Differs from Update in that it will do every row (even if we don't hold
	// the handle for it).
	HRESULT UpdateAll(
		_In_opt_ DBCOUNTITEM* pcRows = NULL,
		_Outptr_opt_ HROW** pphRow = NULL,
		_Outptr_opt_ DBROWSTATUS** ppStatus = NULL) throw()
	{
		ATLASSUME(m_spRowset != NULL);

		CComPtr<IRowsetUpdate> spRowsetUpdate;
		HRESULT hr = m_spRowset->QueryInterface(&spRowsetUpdate);
		if (FAILED(hr))
			return hr;

		// Create some temporary variables to help with debugging.
		DBCOUNTITEM		cRowsReturned = 0;
		CComHeapPtr<HROW>			sprgRows;
		CComHeapPtr<DBROWSTATUS>	spRowStatus;

		// Passing zero for the 2nd parameter tells the provider to update ALL pending rows.
		// The 3rd parameter, prghRows is ignored.
		hr =  spRowsetUpdate->Update(NULL, 0, NULL, &cRowsReturned, &sprgRows, &spRowStatus);

		// NOTE, the user must CoTaskMemFree *pphRow and *ppStatus after return, if they
		// are non-NULL.  Otherwise, we'll CoTaskMemFree if they are NULL.
		if (pcRows != NULL)
			*pcRows = cRowsReturned;

		if (pphRow != NULL)
			*pphRow = sprgRows.Detach();

		if (ppStatus != NULL)
			*ppStatus = spRowStatus.Detach();

		return hr;
	}

	// Get the approximate position of the row corresponding to the passed bookmark
	HRESULT GetApproximatePosition(
		_In_ const CBookmarkBase* pBookmark,
		_Out_opt_ DBCOUNTITEM* pPosition,
		_Out_opt_ DBCOUNTITEM* pcRows) throw()
	{
		ATLASSUME(m_spRowset != NULL);

		CComPtr<IRowsetScroll> spRowsetScroll;
		HRESULT hr = m_spRowset->QueryInterface(&spRowsetScroll);
		if (SUCCEEDED(hr))
		{
			if (pBookmark != NULL)
				hr = spRowsetScroll->GetApproximatePosition(NULL, pBookmark->GetSize(), pBookmark->GetBuffer(),
						pPosition, pcRows);
			else
				hr = spRowsetScroll->GetApproximatePosition(NULL, 0, NULL, pPosition, pcRows);

		}
		return hr;

	}
	// Move to a fractional position in the rowset
	HRESULT MoveToRatio(
		_In_ DBCOUNTITEM nNumerator,
		_In_ DBCOUNTITEM nDenominator,
		_In_ bool bForward = true) throw()
	{
		ATLASSUME(m_spRowset != NULL);
		DBCOUNTITEM nRowsFetched;

		CComPtr<IRowsetScroll> spRowsetScroll;
		HRESULT hr = m_spRowset->QueryInterface(&spRowsetScroll);
		if (FAILED(hr))
			return hr;

		m_pAccessor->FreeRecordMemory(m_spRowset);
		ReleaseRows();
		HROW* phRow = &m_hRow;
		hr = spRowsetScroll->GetRowsAtRatio(NULL, NULL, nNumerator, nDenominator, (bForward) ? 1 : -1,
			&nRowsFetched, &phRow);
		// Note we're not using SUCCEEDED here, because we could get DB_S_ENDOFROWSET
		if (hr == S_OK)
			hr = GetData();

		return hr;
	}

	HRESULT FindNextRow(
		_In_ DBCOMPAREOP op,
		_In_ BYTE* pData,
		_In_ DBTYPE wType,
		_In_ DBLENGTH nLength,
		_In_ BYTE bPrecision,
		_In_ BYTE bScale,
		_In_ BOOL bSkipCurrent = TRUE,
		_In_opt_ CBookmarkBase* pBookmark = NULL) throw()
	{
		ATLASSUME(m_spRowset != NULL);
		DBBINDING   binding;
		HRESULT     hr;
		HACCESSOR   hAccessor;
		DBCOUNTITEM ulRowsFetched = 0;
		HROW*       phRow = &m_hRow;
		DBLENGTH    cbBookmark;
		BYTE*       pBookmarkBuffer;
		CComQIPtr<IAccessor, &__uuidof(IAccessor)>        spAccessor(m_spRowset);
		CComQIPtr<IRowsetFind, &__uuidof(IRowsetFind)>    spRowsetFind(m_spRowset);

		if (spAccessor == NULL || spRowsetFind == NULL)
			return E_NOINTERFACE;

		TAccessor::Bind(&binding, 1, wType, nLength, bPrecision, bScale, DBPARAMIO_NOTPARAM, 0);
		hr = CAccessorBase::BindEntries(&binding, 1, &hAccessor, nLength, spAccessor);
		if (FAILED(hr))
			return hr;

		if (pBookmark == NULL)
		{
			cbBookmark = 0;
			pBookmarkBuffer = NULL;
		}
		else
		{
			cbBookmark = pBookmark->GetSize();
			pBookmarkBuffer = pBookmark->GetBuffer();
		}

		hr = spRowsetFind->FindNextRow(DB_NULL_HCHAPTER, hAccessor, pData, op, cbBookmark, pBookmarkBuffer,
					bSkipCurrent, 1,  &ulRowsFetched, &phRow);
		// Note we're not using SUCCEEDED here, because we could get DB_S_ENDOFROWSET
		if (hr != S_OK)
			return hr;

		// Get the data
		hr = GetData();
		spAccessor->ReleaseAccessor(hAccessor, NULL);
		if (FAILED(hr))
		{
			ATLTRACE(_T("ATL: GetData from FindNextRows failed - HRESULT = 0x%X\n"),hr);
			ReleaseRows();
		}
		return S_OK;
	}

// Implementation
	static const IID& GetIID() throw()
	{
		return __uuidof(IRowset);
	}

	IRowset* GetInterface() const throw()
	{
		return m_spRowset;
	}

	IRowset** GetInterfacePtr() throw()
	{
		return &m_spRowset;
	}

	void SetupOptionalRowsetInterfaces() throw()
	{
		// Cache IRowsetChange if available
		if (m_spRowset != NULL)
			m_spRowset->QueryInterface(&m_spRowsetChange);
	}

	HRESULT BindFinished() const throw()
	{
		return S_OK;
	}

	void SetAccessor(_In_opt_ TAccessor* pAccessor) throw()
	{
		m_pAccessor = pAccessor;
	}

	CComPtr<IRowset>        m_spRowset;
	CComPtr<IRowsetChange>  m_spRowsetChange;
	TAccessor*				m_pAccessor;
	HROW                    m_hRow;
	CXMLAccessor* m_pXMLAccessor;
};

///////////////////////////////////////////////////////////////////////////
// class CBulkRowset

template <class TAccessor>
class CBulkRowset :
	public CRowset<TAccessor>
{
public:
	CBulkRowset()
	{
		// Default the number of rows to bulk fetch to 10
		m_nRows = 10;
		m_hr    = S_OK;
		m_phRow = NULL;
	}

	~CBulkRowset()
	{
		Close();
	}

	void Close() throw()
	{
		if (this->m_spRowset != NULL)
		{
			this->m_pAccessor->FreeRecordMemory(this->m_spRowset);
			ReleaseRows();
		}
		CRowset<TAccessor>::Close();

		delete [] m_phRow;
		m_phRow = NULL;

		m_hr = S_OK;
	}
	// Set the number of row handles that will be retrieved in each
	// bulk row fetch. The default is 10 and this function must be called
	// before Open if you wish to change it.
	void SetRows(_In_ DBROWCOUNT nRows) throw()
	{
		if (nRows == 0)
			nRows = 10;
		if (nRows != m_nRows)
		{
			// This function must be called before the memory is allocated
			// during binding or between a Close() and a Open()
			delete m_phRow;
			m_phRow = NULL;
			m_nRows = nRows;
		}
	}
	// AddRef all the currently retrieved row handles
	HRESULT AddRefRows() throw()
	{
		ATLASSUME(this->m_spRowset != NULL);
		return this->m_spRowset->AddRefRows(m_nCurrentRows, m_phRow, NULL, NULL);
	}
	// Release all the currently retrieved row handles
	HRESULT ReleaseRows() throw()
	{
		ATLASSUME(this->m_spRowset != NULL);
		// We're going to Release the rows so reset the current row position
		m_nCurrentRow = 0;
		this->m_hRow        = 0;
		DBCOUNTITEM nCurrentRows = m_nCurrentRows;
		m_nCurrentRows = 0;
		return this->m_spRowset->ReleaseRows(nCurrentRows, m_phRow, NULL, NULL, NULL);
	}
	// Move to the first record
	HRESULT MoveFirst() throw()
	{
		ATLASSUME(this->m_spRowset != NULL);

		m_hr = S_OK;
		this->m_pAccessor->FreeRecordMemory(this->m_spRowset);
		ReleaseRows();
		// the following line of code may fail if the DBPROP_CANFETCHBACKWARDS and/or
		// DBPROP_CANSCROLLBACKWARDS properties have not been set.
		HRESULT hr = this->m_spRowset->RestartPosition(NULL);
		if (FAILED(hr))
			return hr;

		// Get the data
		return MoveNext(0);
	}
	// Move to the last record
	HRESULT MoveLast() throw()
	{
		m_hr = S_OK;
		this->m_pAccessor->FreeRecordMemory(this->m_spRowset);
		ReleaseRows();
		m_hr = S_OK;
		// the following line of code may fail if the DBPROP_CANFETCHBACKWARDS and/or
		// DBPROP_CANSCROLLBACKWARDS properties have not been set.
		return CRowset<TAccessor>::MoveLast();
	}
	// Move to the next record
	HRESULT MoveNext() throw()
	{
		return MoveNext(0);
	}
	// Move to the previous record
	HRESULT MovePrev() throw()
	{
		// the following line of code may fail if the DBPROP_CANFETCHBACKWARDS and/or
		// DBPROP_CANSCROLLBACKWARDS properties have not been set.
		return MoveNext(-2);
	}
	// Move lSkip records forward or backward
	HRESULT MoveNext(
		_In_ DBROWOFFSET lSkip,
		_In_ bool bForward = true) throw()
	{
		ATLASSUME(this->m_spRowset != NULL);
		ATLASSUME(m_phRow    != NULL);

		this->m_pAccessor->FreeRecordMemory(this->m_spRowset);

		// Calculate the record index in the buffer
		DBROWOFFSET nNewRow = m_nCurrentRow + lSkip + (bForward ? 1 : -1);

		bool bFetchNewRows = false;
		// Is the row in the buffer?
		// else adjust the skip value
		if (m_nCurrentRows == 0)
		{
			//lSkip = 0;
			bFetchNewRows = true;
		}
		else if (nNewRow >= (DBROWOFFSET)m_nCurrentRows)
		{
			bFetchNewRows = true;
			lSkip = nNewRow - m_nCurrentRows + (bForward ? 0 : (2 - m_nRows));
		}
		else if (nNewRow < 0)
		{
			lSkip = nNewRow - (m_nCurrentRows - m_nCurrentRow) + (bForward ? 0 : (2 - m_nRows));
			bFetchNewRows = true;
		}

		if (bFetchNewRows)
		{
			nNewRow = 0;
			// If we've reached the end of the buffer and we had a non S_OK HRESULT from
			// the last call to GetNextRows then return that HRESULT now.
			if (m_hr != S_OK && m_hr != DB_S_ROWLIMITEXCEEDED)
				return m_hr;

			// We've finished with these rows so we need some more
			// First release any HROWs that we have
			ReleaseRows();

			// the following line of code may fail if the DBPROP_CANFETCHBACKWARDS and/or
			// DBPROP_CANSCROLLBACKWARDS properties have not been set and the lSkip offset is negative.
			m_hr = this->m_spRowset->GetNextRows(NULL, lSkip, m_nRows, &m_nCurrentRows, &m_phRow);

			// If we have an error HRESULT or we haven't retrieved any rows then return
			// the HRESULT now.
			if (FAILED(m_hr) || m_nCurrentRows == 0)
				return m_hr;
			if (!bForward)
				nNewRow = m_nCurrentRows - 1;
		}

		// Get the data for the current row
		this->m_hRow = m_phRow[m_nCurrentRow = nNewRow];
		return this->GetData();
	}
	// Move to the passed bookmark
	HRESULT MoveToBookmark(
		_In_ const CBookmarkBase& bookmark,
		_In_ DBCOUNTITEM lSkip = 0) throw()
	{
		ATLASSUME(this->m_spRowset != NULL);
		CComPtr<IRowsetLocate> spLocate;
		HRESULT hr = this->m_spRowset->QueryInterface(&spLocate);
		if (FAILED(hr))
			return hr;

		this->m_pAccessor->FreeRecordMemory(this->m_spRowset);
		ReleaseRows();
		m_hr = spLocate->GetRowsAt(NULL, NULL, bookmark.GetSize(), bookmark.GetBuffer(),
			lSkip, m_nRows, &m_nCurrentRows, &m_phRow);
		if( (m_hr != S_OK || m_nCurrentRows == 0 ) && m_hr != DB_S_ENDOFROWSET)
			return m_hr;

		// Get the data
		this->m_hRow = m_phRow[m_nCurrentRow];
		return this->GetData();
	}
	// Move to a fractional position in the rowset
	HRESULT MoveToRatio(
		_In_ DBCOUNTITEM nNumerator,
		_In_ DBCOUNTITEM nDenominator) throw()
	{
		ATLASSUME(this->m_spRowset != NULL);

		CComPtr<IRowsetScroll> spRowsetScroll;
		HRESULT hr = this->m_spRowset->QueryInterface(&spRowsetScroll);
		if (FAILED(hr))
			return hr;

		this->m_pAccessor->FreeRecordMemory(this->m_spRowset);
		ReleaseRows();
		m_hr = spRowsetScroll->GetRowsAtRatio(NULL, NULL, nNumerator, nDenominator, m_nRows, &m_nCurrentRows, &m_phRow);
		if (m_hr != S_OK || m_nCurrentRows == 0)
			return m_hr;

		// Get the data
		this->m_hRow = m_phRow[m_nCurrentRow];
		return this->GetData();
	}
	// Insert the current record
	HRESULT Insert(
		_In_ int nAccessor = 0,
		_In_ bool bGetHRow = false) throw()
	{
		ReleaseRows();
		return CRowset< TAccessor >::Insert(nAccessor, bGetHRow);
	}

// Implementation
	HRESULT BindFinished() throw()
	{
		// No rows in the buffer yet
		m_nCurrentRows = 0;
		// Cause MoveNext to automatically perform a new bulk fetch the first time
		m_nCurrentRow  = 0;
		m_hr = S_OK;

		// Do not allocate if the buffer has been allocated by a previous call to BindFinished.
		if (m_phRow == NULL)
		{
			m_phRow = _ATL_NEW HROW[m_nRows];
			if (m_phRow == NULL)
				return E_OUTOFMEMORY;
		}

		return S_OK;
	}

	HRESULT m_hr;           // HRESULT to return from MoveNext at end of buffer
	HROW*   m_phRow;        // Pointer to array of HROWs for each row in buffer
	DBROWCOUNT   m_nRows;        // Number of rows that will fit in the buffer
	DBCOUNTITEM  m_nCurrentRows; // Number of rows currently in the buffer
	DBCOUNTITEM  m_nCurrentRow;
};

/////////////////////////////////////////////////////////////////////////////
// Large Block Allocation Helper - CVBufHelper & CVirtualBuffer
template <class T>
class CVBufHelper
{
public:
	virtual T* operator()(_In_opt_ T* pCurrent)
	{
		return pCurrent;
	}
};

template <class T>
class CVirtualBuffer
{
protected:
	CVirtualBuffer()
	{
	}
	T* m_pTop;
	int m_nMaxElements;
public:
	T* m_pBase;
	T* m_pCurrent;
	explicit CVirtualBuffer(_In_ int nMaxElements)
	{
		//in case of overflow throw exception
		ATLENSURE(nMaxElements>=0);

		ATLENSURE(nMaxElements <=size_t(-1)/sizeof(T)); //overflow check
		m_nMaxElements = nMaxElements;
		m_pBase = (T*) VirtualAlloc(NULL,sizeof(T)*nMaxElements,	MEM_RESERVE, PAGE_READWRITE);
		if(m_pBase == NULL)
		{
			_AtlRaiseException((DWORD)STATUS_NO_MEMORY);
		}
		m_pTop = m_pCurrent = m_pBase;
		// Commit first page - chances are this is all that will be used
		VirtualAlloc(m_pBase, sizeof(T), MEM_COMMIT, PAGE_READWRITE);
	}
	~CVirtualBuffer()
	{
		VirtualFree(m_pBase, 0, MEM_RELEASE);
	}
	int Except(_In_ LPEXCEPTION_POINTERS lpEP)
	{
		EXCEPTION_RECORD* pExcept = lpEP->ExceptionRecord;
		if (pExcept->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
			return EXCEPTION_CONTINUE_SEARCH;
		BYTE* pAddress = (LPBYTE) pExcept->ExceptionInformation[1];

		// The address is out of this buffer arrange means AV is not conflicted with this buffer.
		BYTE* pTmp = reinterpret_cast<BYTE*>(m_pBase);
		if ((pAddress < pTmp) || (pAddress >= pTmp + (sizeof(T) * m_nMaxElements)))
			return EXCEPTION_CONTINUE_SEARCH;

		VirtualAlloc(pAddress, sizeof(T), MEM_COMMIT, PAGE_READWRITE);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	bool Seek(_In_ int nElement)
	{
		ATLASSERT(nElement >= 0 && nElement < m_nMaxElements);
		if(nElement < 0 || nElement >= m_nMaxElements)
			return false;
		m_pCurrent = &m_pBase[nElement];
		return true;
	}

#define ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()							\
	__pragma(warning(push))												\
	/* we never return EXCEPTION_EXECUTE_HANDLER in Except */			\
	/* hence, we can safely ignore this warning about the handler */	\
	/* being empty */													\
	__pragma(warning(disable:6322))										\
	__try																\
	{																	\
		/**/
# define ATL_DBCLI_END_VBUF_GUARDED_REGION()							\
	} /* try { */														\
	__except(this->Except(GetExceptionInformation()))					\
	{																	\
		/*EMPTY*/														\
	}																	\
	__pragma(warning(pop))												\
		/**/

	void SetAt(
		_In_ int nElement,
		_In_ const T& Element)
	{
		ATLASSERT(nElement >= 0 && nElement < m_nMaxElements);

		ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
			T* p = &m_pBase[nElement];
			*p = Element;
			m_pTop = p++ > m_pTop ? p : m_pTop;
		ATL_DBCLI_END_VBUF_GUARDED_REGION()
	}
	template <class Q>
	void WriteBulk(_In_ Q& helper)
	{
		ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
			m_pCurrent = helper(m_pBase);
			m_pTop = m_pCurrent > m_pTop ? m_pCurrent : m_pTop;
		ATL_DBCLI_END_VBUF_GUARDED_REGION()
	}
	void Write(_In_ const T& Element)
	{
		ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
			*m_pCurrent = Element;
			m_pCurrent++;
			m_pTop = m_pCurrent > m_pTop ? m_pCurrent : m_pTop;
		ATL_DBCLI_END_VBUF_GUARDED_REGION()
	}
	T& Read()
	{
		return *m_pCurrent;
	}
	_Ret_maybenull_z_ operator BSTR()
	{
		BSTR bstrTemp = NULL ;
		ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
			bstrTemp = SysAllocStringByteLen((char*) m_pBase,
				(UINT) ((BYTE*)m_pTop - (BYTE*)m_pBase));
		ATL_DBCLI_END_VBUF_GUARDED_REGION()
		return bstrTemp;
	}
	const T& operator[](_In_ int nElement) const
	{
		if(nElement < 0 || nElement >= m_nMaxElements)
			_AtlRaiseException((DWORD)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);

		return m_pBase[nElement];
	}
	operator T*()
	{
		return m_pBase;
	}

};


typedef CVirtualBuffer<BYTE> CVirtualBytes;

///////////////////////////////////////////////////////////////////////////
// class CArrayRowset
//
// Allows you to access a rowset with an array syntax. TAccessor must be a
// CAccessor<> type class

template <class TAccessor>
class CArrayRowset :
	public CVirtualBuffer<typename TAccessor::DataClass>,
	protected CBulkRowset<TAccessor>
{
public:
	CArrayRowset(_In_ int nMax = 100000) :
		CVirtualBuffer<typename TAccessor::DataClass>(nMax)
	{
		m_nRowsRead = 0;
	}
	typename TAccessor::DataClass& operator[](_In_ int nRow)
	{
		ATLASSERT(nRow >= 0);
		if( nRow < 0 )
			AtlThrow(E_INVALIDARG);

		HRESULT hr = S_OK;
		typename TAccessor::DataClass* pCurrent = this->m_pBase + m_nRowsRead;

		// Retrieve the row if we haven't retrieved it already
		while ((ULONG)nRow >= m_nRowsRead)
		{
			this->m_pAccessor->SetBuffer(reinterpret_cast<BYTE*>(pCurrent));
			ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
				// Get the row
				hr = this->MoveNext();
				if (hr != S_OK)
					break;
			ATL_DBCLI_END_VBUF_GUARDED_REGION()
			m_nRowsRead++;
			pCurrent++;
		}

		if(hr != S_OK)
		{
			ATLASSERT(hr != DB_S_ENDOFROWSET);	// if you're getting this assertion, then
												// most likely you are trying to access an
												// out of bounds element of CArrayRowset
												// (ex. table[100].data where table has only
												// 50 records)
			AtlThrow(hr);
		}

		return *(this->m_pBase + nRow);
	}

	HRESULT Snapshot() throw()
	{
		HRESULT hr = S_FALSE;
		ATLASSUME(m_nRowsRead == 0);
		ATLASSUME(this->m_spRowset != NULL);
		typename TAccessor::DataClass* pCurrent = this->m_pBase;
		this->m_pAccessor->SetBuffer(reinterpret_cast<BYTE*>(pCurrent));
		ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
			hr = this->MoveFirst();
		ATL_DBCLI_END_VBUF_GUARDED_REGION()
		if (FAILED(hr))
			return hr;
		do
		{
			m_nRowsRead++;
			pCurrent++;
			this->m_pAccessor->SetBuffer(reinterpret_cast<BYTE*>(pCurrent));
			ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION()
				hr = this->MoveNext();
			ATL_DBCLI_END_VBUF_GUARDED_REGION()
		} while (SUCCEEDED(hr) &&  hr != DB_S_ENDOFROWSET);

		return (hr == DB_S_ENDOFROWSET) ? S_OK : hr;
	}
	ULONG   m_nRowsRead;
};

#undef ATL_DBCLI_BEGIN_VBUF_GUARDED_REGION
#undef ATL_DBCLI_END_VBUF_GUARDED_REGION

struct m_nAccessors_unique_type; // TRANSITION, C1XX

template<class T, class = void>
struct Has_m_nAccessors : ::std::false_type {};

template<class T>
struct Has_m_nAccessors<T, ::std::void_t<m_nAccessors_unique_type,
	decltype(::std::declval<T>().m_nAccessors)>> : ::std::true_type {};

///////////////////////////////////////////////////////////////////////////
// CAccessorRowset
template <class TAccessor = CNoAccessor, template <typename T> class TRowset = CRowset>
class CAccessorRowset :
	public TAccessor,
	public TRowset<TAccessor>
{
public:
	CAccessorRowset()
	{
		// Give the rowset a pointer to the accessor
#ifndef _ATL_IF_EXISTS_FALLBACK
		if constexpr(Has_m_nAccessors<CAccessorRowset>::value)
#else
		__if_exists(m_nAccessors)
#endif
		{
			this->SetAccessor(this);
		}
	}
	~CAccessorRowset()
	{
		Close();
	}
ATLPREFAST_SUPPRESS(6387)
	// Used to get the column information from the opened rowset. The user is responsible
	// for freeing the returned column information and string buffer.
	HRESULT GetColumnInfo(
		_Out_ DBORDINAL* pulColumns,
		_Outptr_result_buffer_maybenull_(*pulColumns) DBCOLUMNINFO** ppColumnInfo,
		_Outptr_result_maybenull_z_ LPOLESTR* ppStrings) const throw()
	{
		if (ppColumnInfo == NULL || pulColumns == NULL || ppStrings == NULL)
			return E_POINTER;

		ATLASSERT(this->GetInterface() != NULL);
		if (this->GetInterface() == NULL)
			return E_FAIL;
		CComPtr<IColumnsInfo> spColumns;
		HRESULT hr = this->GetInterface()->QueryInterface(&spColumns);
		if (SUCCEEDED(hr))
			hr = spColumns->GetColumnInfo(pulColumns, ppColumnInfo, ppStrings);

		return hr;
	}
ATLPREFAST_UNSUPPRESS()

	// Used to get the column information when overriding the bindings using CDynamicAccessor
	// The user should CoTaskMemFree the column information pointer that is returned.
	HRESULT GetColumnInfo(
		_In_ DBORDINAL* pColumns,
		_Outptr_ DBCOLUMNINFO** ppColumnInfo) throw()
	{
		// If you get a compilation here, then you are most likely calling this function
		// from a class that is not using CDynamicAccessor.
		ATLASSERT(this->GetInterface() != NULL);
		if (this->GetInterface() == NULL)
			return E_FAIL;

		ATLASSUME(ppColumnInfo != NULL);

		return TAccessor::GetColumnInfo(this->GetInterface(), pColumns, ppColumnInfo);
	}

	// Call to bind the output columns
	HRESULT Bind() throw()
	{
		// Bind should only be called when we've successfully opened the rowset
		ATLASSERT(this->GetInterface() != NULL);
		if (this->GetInterface() == NULL)
			return E_FAIL;
		HRESULT hr = TAccessor::BindColumns(this->GetInterface());
		if (SUCCEEDED(hr))
			hr = this->BindFinished();
		return hr;
	}
	// Close the opened rowset and release the created accessors for the output columns
	void Close() throw()
	{
		if (this->GetInterface() != NULL)
		{
			this->ReleaseAccessors(this->GetInterface());
			TAccessor::Close();
			TRowset<TAccessor>::Close();
		}
	}
	// Free's any columns in the current record that need to be freed.
	// E.g. Calls SysFreeString on any BSTR's and Release on any interfaces.
	void FreeRecordMemory() throw()
	{
		TAccessor::FreeRecordMemory(this->m_spRowset);
	}
	void FreeRecordMemory(int nAccessor) throw()
	{
		TAccessor::FreeRecordMemory(nAccessor, this->m_spRowset);
	}
};

///////////////////////////////////////////////////////////////////////////
// class CEnumeratorAccessor

class CEnumeratorAccessor
{
public:
	WCHAR           m_szName[129];
	WCHAR           m_szParseName[129];
	WCHAR           m_szDescription[129];
	USHORT          m_nType;
	VARIANT_BOOL    m_bIsParent;

// Binding Maps
BEGIN_COLUMN_MAP(CEnumeratorAccessor)
	COLUMN_ENTRY(1, m_szName)
	COLUMN_ENTRY(2, m_szParseName)
	COLUMN_ENTRY(3, m_szDescription)
	COLUMN_ENTRY(4, m_nType)
	COLUMN_ENTRY_TYPE(5, DBTYPE_BOOL, m_bIsParent)
END_COLUMN_MAP()
};

///////////////////////////////////////////////////////////////////////////
// class CEnumerator

class CEnumerator :
	public CAccessorRowset<CAccessor<CEnumeratorAccessor> >
{
public:
	HRESULT Open(_In_ LPMONIKER pMoniker) throw()
	{
		if (pMoniker == NULL)
			return E_FAIL;

		// Bind the moniker for the sources rowset
		if (FAILED(BindMoniker(pMoniker, 0, __uuidof(ISourcesRowset),
					(void**)&m_spSourcesRowset)))
			return E_FAIL;

		// Enumerate the data sources
		if (FAILED(m_spSourcesRowset->GetSourcesRowset(NULL, __uuidof(IRowset), 0,
			NULL, (IUnknown**)&m_spRowset)))
			return E_FAIL;

		return Bind();
	}
	HRESULT Open(_In_ const CEnumerator& enumerator) throw()
	{
		HRESULT hr;
		CComPtr<IMoniker> spMoniker;

		hr = enumerator.GetMoniker(&spMoniker);
		if (FAILED(hr))
			return hr;

		return Open(spMoniker);
	}
	HRESULT Open(_In_ const CLSID* pClsid = &CLSID_OLEDB_ENUMERATOR) throw()
	{
		if (pClsid == NULL)
			return E_FAIL;

		HRESULT hr;
		// Create the enumerator
		hr = CoCreateInstance(*pClsid, NULL, CLSCTX_INPROC_SERVER,
				__uuidof(ISourcesRowset), (LPVOID*)&m_spSourcesRowset);
		if (FAILED(hr))
			return hr;

		// Get the rowset so we can enumerate the data sources
		hr = m_spSourcesRowset->GetSourcesRowset(NULL, __uuidof(IRowset), 0,
			NULL, (IUnknown**)&m_spRowset);
		if (FAILED(hr))
			return hr;

		return Bind();
	}

	void Close() throw()
	{
		// Close the rowset pointer
		if (m_spSourcesRowset != NULL)
			m_spSourcesRowset.Release();

		// Close the base class pointers
		CAccessorRowset<CAccessor<CEnumeratorAccessor> >::Close();
	}

	HRESULT GetMoniker(_Outptr_ LPMONIKER* ppMoniker) const throw()
	{
		CComPtr<IParseDisplayName> spParse;
		HRESULT hr;
		ULONG   chEaten;

		if (ppMoniker == NULL)
			return E_POINTER;

		if (m_spSourcesRowset == NULL)
			return E_FAIL;

		hr = m_spSourcesRowset->QueryInterface(__uuidof(IParseDisplayName), (void**)&spParse);
		if (FAILED(hr))
			return hr;

		hr = spParse->ParseDisplayName(NULL, (LPOLESTR)m_szParseName,
				&chEaten, ppMoniker);
		return hr;
	}

	HRESULT GetMoniker(
		_Outptr_ LPMONIKER* ppMoniker,
		_In_z_ LPCTSTR lpszDisplayName) const throw()
	{
		CComPtr<IParseDisplayName> spParse;
		HRESULT hr;
		ULONG   chEaten;

		if (ppMoniker == NULL || lpszDisplayName == NULL)
			return E_POINTER;

		if (m_spSourcesRowset == NULL)
			return E_FAIL;

		hr = m_spSourcesRowset->QueryInterface(__uuidof(IParseDisplayName), (void**)&spParse);
		if (FAILED(hr))
			return hr;

		hr = spParse->ParseDisplayName(NULL, (LPOLESTR)CComBSTR(lpszDisplayName),
				&chEaten, ppMoniker);
		return hr;
	}

	bool Find(_In_z_ TCHAR* szSearchName) throw()
	{
		CT2W ctSearchName(szSearchName);
		WCHAR *wszSearchName = ctSearchName;

		// Loop through the providers looking for the passed name
		while (MoveNext()==S_OK && lstrcmpW(m_szName, wszSearchName))
		{
			ATLTRACE(atlTraceDBClient, 2, _T("%Ts, %Ts, %d\n"), m_szName, m_szParseName, m_nType);
		}
		if (lstrcmpW(m_szName, wszSearchName))
			return false;
		else
			return true;
	}

	CComPtr<ISourcesRowset> m_spSourcesRowset;
};


///////////////////////////////////////////////////////////////////////////
// CDataSource

class CDataSource
{
public:
	HRESULT Open(
		_In_ const CLSID& clsid,
		_Inout_updates_opt_(nPropertySets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG nPropertySets=1) throw()
	{
		HRESULT hr;

		m_spInit.Release();
		hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IDBInitialize),
				(void**)&m_spInit);
		if (FAILED(hr))
			return hr;

		// Initialize the provider
		return OpenWithProperties(pPropSet, nPropertySets);
	}
	HRESULT Open(
		_In_ const CLSID& clsid,
		_In_z_ LPCTSTR pName,
		_In_opt_z_ LPCTSTR pUserName = NULL,
		_In_opt_z_ LPCTSTR pPassword = NULL,
		_In_ long nInitMode = 0) throw()
	{
		m_spInit.Release();
		HRESULT hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, __uuidof(IDBInitialize),
				(void**)&m_spInit);
		if (FAILED(hr))
			return hr;

		return OpenWithNameUserPassword(pName, pUserName, pPassword, nInitMode);
	}
	HRESULT Open(
		_In_z_ LPCSTR szProgID,
		_Inout_updates_opt_(nPropertySets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG nPropertySets=1) throw()
	{
		HRESULT hr;
		CLSID   clsid;

		hr = CLSIDFromProgID(CComBSTR(szProgID), &clsid);
		if (FAILED(hr))
		{
			return hr;
		}

		return Open(clsid, pPropSet, nPropertySets);
	}
	HRESULT Open(
		_In_z_ LPCWSTR szProgID,
		_In_z_ LPCTSTR pName,
		_In_opt_z_ LPCTSTR pUserName = NULL,
		_In_opt_z_ LPCTSTR pPassword = NULL,
		_In_ long nInitMode = 0) throw()
	{
		CLSID clsid;

		HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
		if (FAILED(hr))
			return hr;

		return Open(clsid, pName, pUserName, pPassword, nInitMode);
	}
	HRESULT Open(
		_In_z_ LPCWSTR szProgID,
		_Inout_updates_opt_(nPropertySets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG nPropertySets=1) throw()
	{
		HRESULT hr;
		CLSID   clsid;

		hr = CLSIDFromProgID(szProgID, &clsid);
		if (FAILED(hr))
			return hr;

		return Open(clsid, pPropSet, nPropertySets);
	}
	HRESULT Open(
		_In_z_ LPCSTR szProgID,
		_In_z_ LPCTSTR pName,
		_In_opt_z_ LPCTSTR pUserName = NULL,
		_In_opt_z_ LPCTSTR pPassword = NULL,
		_In_ long nInitMode = 0) throw()
	{
		CLSID   clsid;

		HRESULT hr = CLSIDFromProgID(CComBSTR(szProgID), &clsid);
		if (FAILED(hr))
		{
			return hr;
		}

		return Open(clsid, pName, pUserName, pPassword, nInitMode);
	}
	HRESULT Open(
		_In_ const CEnumerator& enumerator,
		_Inout_updates_opt_(nPropertySets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG nPropertySets=1) throw()
	{
		CComPtr<IMoniker> spMoniker;

		HRESULT hr = enumerator.GetMoniker(&spMoniker);
		if (FAILED(hr))
			return hr;

		m_spInit.Release();
		//  Now bind the moniker
		hr = BindMoniker(spMoniker, 0, __uuidof(IDBInitialize), (void**)&m_spInit);
		if (FAILED(hr))
			return hr;

		return OpenWithProperties(pPropSet, nPropertySets);
	}
	HRESULT Open(
		_In_ const CEnumerator& enumerator,
		_In_z_ LPCTSTR pName,
		_In_opt_z_ LPCTSTR pUserName = NULL,
		_In_opt_z_ LPCTSTR pPassword = NULL,
		_In_ long nInitMode = 0) throw()
	{
		CComPtr<IMoniker> spMoniker;

		HRESULT hr = enumerator.GetMoniker(&spMoniker);
		if (FAILED(hr))
			return hr;

		m_spInit.Release();
		//  Now bind the moniker
		hr = BindMoniker(spMoniker, 0, __uuidof(IDBInitialize), (void**)&m_spInit);
		if (FAILED(hr))
			return hr;

		return OpenWithNameUserPassword(pName, pUserName, pPassword, nInitMode);
	}
	// Invoke the data links dialog and open the selected database
	HRESULT Open(
		_In_ HWND hWnd = GetActiveWindow(),
		_In_ DBPROMPTOPTIONS dwPromptOptions = DBPROMPTOPTIONS_WIZARDSHEET) throw()
	{
		CComPtr<IDBPromptInitialize> spDBInit;

		HRESULT hr = CoCreateInstance(__uuidof(DataLinks), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDBPromptInitialize), (void**) &spDBInit);
		if (FAILED(hr))
			return hr;

		CComPtr<IDBProperties> spIDBProperties;
		hr = spDBInit->PromptDataSource(NULL, hWnd, dwPromptOptions, 0, NULL, NULL,
			__uuidof(IDBProperties), (IUnknown**)&spIDBProperties);

		if (hr == S_OK)
		{
			hr = spIDBProperties->QueryInterface(&m_spInit);
			if (SUCCEEDED(hr))
				hr = m_spInit->Initialize();
		}
		return hr;
	}
	// Opens a data source using the service components
	HRESULT OpenWithServiceComponents(
		_In_ const CLSID& clsid,
		_In_reads_opt_(nPropertySets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG nPropertySets=1) throw()
	{
		CComPtr<IDataInitialize> spDataInit;

		HRESULT hr = CoCreateInstance(__uuidof(MSDAINITIALIZE), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDataInitialize), (void**)&spDataInit);
		if (FAILED(hr))
			return hr;

		m_spInit.Release();
		hr = spDataInit->CreateDBInstance(clsid, NULL, CLSCTX_INPROC_SERVER, NULL,
			__uuidof(IDBInitialize), (IUnknown**)&m_spInit);
		if (FAILED(hr))
			return hr;

		// Initialize the provider
		return OpenWithProperties(pPropSet, nPropertySets);
	}
	// Opens a data source using the service components
	HRESULT OpenWithServiceComponents(
		_In_z_ LPCTSTR szProgID,
		_In_reads_opt_(nPropertySets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG nPropertySets=1) throw()
	{
		CLSID clsid;

		HRESULT hr = CLSIDFromProgID(CComBSTR(szProgID), &clsid);
		if (FAILED(hr))
			return hr;

		return OpenWithServiceComponents(clsid, pPropSet, nPropertySets);
	}
	// Bring up the "Organize Dialog" which allows the user to select a previously created data link
	// file (.UDL file). The selected file will be used to open the database.
	HRESULT OpenWithPromptFileName(
		_In_ HWND hWnd = GetActiveWindow(),
		_In_ DBPROMPTOPTIONS dwPromptOptions = DBPROMPTOPTIONS_NONE,
		_In_opt_z_ LPCOLESTR szInitialDirectory = NULL) throw()
	{
		CComPtr<IDBPromptInitialize> spDBInit;

		HRESULT hr = CoCreateInstance(__uuidof(DataLinks), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDBPromptInitialize), (void**) &spDBInit);
		if (FAILED(hr))
		{
			return hr;
		}

		CComPtr<IDBProperties> spIDBProperties;
		LPOLESTR szSelected;

		hr = spDBInit->PromptFileName(hWnd, dwPromptOptions, szInitialDirectory, L"*.udl", &szSelected);

		if (hr == S_OK)
		{
			hr = OpenFromFileName(szSelected);
		}
		else if (hr == S_FALSE)
		{
			hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_CANCELLED);  // The user clicked cancel
		}

		return hr;
	}
	// Open the datasource specified by the passed filename, typically a .UDL file
	HRESULT OpenFromFileName(_In_z_ LPCOLESTR szFileName) throw()
	{
		CComPtr<IDataInitialize> spDataInit;
		CComHeapPtr<OLECHAR>     spszInitString;

		HRESULT hr = CoCreateInstance(__uuidof(MSDAINITIALIZE), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDataInitialize), (void**)&spDataInit);
		if (FAILED(hr))
			return hr;

		hr = spDataInit->LoadStringFromStorage(szFileName, &spszInitString);
		if (FAILED(hr))
			return hr;

		return OpenFromInitializationString(spszInitString);
	}
	// Open the datasource specified by the passed initialization string
	HRESULT OpenFromInitializationString(
		_In_z_ LPCOLESTR szInitializationString,
		_In_ bool fPromptForInfo = false) throw()
	{
		CComPtr<IDataInitialize> spDataInit;

		HRESULT hr = CoCreateInstance(__uuidof(MSDAINITIALIZE), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDataInitialize), (void**)&spDataInit);
		if (FAILED(hr))
			return hr;

		hr = spDataInit->GetDataSource(NULL, CLSCTX_INPROC_SERVER, szInitializationString,
			__uuidof(IDBInitialize), (IUnknown**)&m_spInit);
		if (FAILED(hr))
			return hr;

		if( fPromptForInfo )
		{
			CComPtr<IDBProperties> spIDBProperties;
			hr = m_spInit->QueryInterface( &spIDBProperties );

			DBPROP rgProperties[1];
			DBPROPSET rgPropertySets[1];

			VariantInit(&rgProperties[0].vValue);
			rgProperties[0].dwOptions = DBPROPOPTIONS_REQUIRED;
			rgProperties[0].colid = DB_NULLID;
			rgProperties[0].dwPropertyID = DBPROP_INIT_PROMPT;
			rgProperties[0].vValue.vt = VT_I2;
			rgProperties[0].vValue.lVal = DBPROMPT_COMPLETEREQUIRED;

			rgPropertySets[0].rgProperties = rgProperties;
			rgPropertySets[0].cProperties = 1;
			rgPropertySets[0].guidPropertySet = DBPROPSET_DBINIT;

			hr = spIDBProperties->SetProperties( 1, rgPropertySets );
			if (FAILED(hr))
				return hr;
		}

		return m_spInit->Initialize();
	}
ATLPREFAST_SUPPRESS(6387)
	// Get the initialization string from the currently open data source. The returned string
	// must be CoTaskMemFree'd when finished with.
	HRESULT GetInitializationString(
		_Outptr_result_z_ BSTR* pInitializationString,
		_In_ bool bIncludePassword=false) throw()
	{
		// If the datasource isn't open then we're not going to get an init string
		ATLASSUME(m_spInit != NULL);
		CComPtr<IDataInitialize> spDataInit;
		LPOLESTR    szInitString;

		HRESULT hr = CoCreateInstance(__uuidof(MSDAINITIALIZE), NULL, CLSCTX_INPROC_SERVER,
			__uuidof(IDataInitialize), (void**)&spDataInit);
		if (FAILED(hr))
			return hr;

		hr = spDataInit->GetInitializationString(m_spInit, bIncludePassword, &szInitString);

		if (SUCCEEDED(hr))
		{
			*pInitializationString = ::SysAllocString(szInitString);
			if (*pInitializationString == NULL && szInitString != NULL)
				hr = E_OUTOFMEMORY;
			CoTaskMemFree(szInitString);
		}

		return hr;
	}
ATLPREFAST_UNSUPPRESS()

	HRESULT GetProperties(
		_In_ ULONG ulPropIDSets,
		_In_reads_(ulPropIDSets) const DBPROPIDSET* pPropIDSet,
		_Out_ ULONG* pulPropertySets,
		_Outptr_result_buffer_maybenull_(*pulPropertySets) DBPROPSET** ppPropsets) const throw()
	{
		CComPtr<IDBProperties> spProperties;

		// Check that we are connected
		ATLASSUME(m_spInit != NULL);

		HRESULT hr = m_spInit->QueryInterface(__uuidof(IDBProperties), (void**)&spProperties);
		if (FAILED(hr))
			return hr;

		hr = spProperties->GetProperties(ulPropIDSets, pPropIDSet, pulPropertySets,
				ppPropsets);
		return hr;
	}

	HRESULT GetProperty(
		_In_ const GUID& guid,
		_In_ DBPROPID propid,
		_Inout_ VARIANT* pVariant) const throw()
	{
		ATLENSURE_RETURN(pVariant != NULL);
		CComPtr<IDBProperties> spProperties;

		// Check that we are connected
		ATLASSUME(m_spInit != NULL);

		HRESULT hr = m_spInit->QueryInterface(__uuidof(IDBProperties), (void**)&spProperties);
		if (FAILED(hr))
			return hr;

		CDBPropIDSet set(guid);
		if(!set.AddPropertyID(propid))
		{
			return E_FAIL;
		}
		CComHeapPtr<DBPROPSET> spPropSet;
		ULONG ulPropSet = 0;
		hr = spProperties->GetProperties(1, &set, &ulPropSet, &spPropSet);
		if (FAILED(hr))
			return hr;

		ATLASSERT(ulPropSet == 1);
		hr = VariantCopy(pVariant, &(spPropSet->rgProperties[0].vValue));
		VariantClear(&(spPropSet->rgProperties[0].vValue));
		CoTaskMemFree(spPropSet->rgProperties);

		return hr;
	}

	void Close() throw()
	{
		m_spInit.Release();
	}

// Implementation
	HRESULT OpenFromIDBProperties(_Inout_ IDBProperties* pIDBProperties) throw()
	{
		ATLENSURE_RETURN(pIDBProperties);

		CComPtr<IPersist> spPersist;
		CLSID   clsid;

		HRESULT hr = pIDBProperties->QueryInterface(__uuidof(IPersist), (void**)&spPersist);
		if (FAILED(hr))
			return hr;

		hr = spPersist->GetClassID(&clsid);
		if (FAILED(hr))
			return hr;

		ULONG       ulPropSets=0;
		CDBPropSet* pPropSets=NULL;
		hr = pIDBProperties->GetProperties(0, NULL, &ulPropSets, (DBPROPSET**)&pPropSets);
		if (FAILED(hr))
			return hr;

		hr = Open(clsid, &pPropSets[0], ulPropSets);

		ATLASSUME(pPropSets != NULL || ulPropSets == 0);

		for (ULONG i=0; i < ulPropSets; i++)
			(pPropSets+i)->~CDBPropSet();
		CoTaskMemFree(pPropSets);

		return hr;
	}

	HRESULT OpenWithNameUserPassword(
		_In_opt_z_ LPCTSTR pName,
		_In_opt_z_ LPCTSTR pUserName,
		_In_opt_z_ LPCTSTR pPassword,
		_In_ long nInitMode = 0) throw()
	{
		ATLASSUME(m_spInit != NULL);
		CComPtr<IDBProperties>  spProperties;

		HRESULT hr = m_spInit->QueryInterface(__uuidof(IDBProperties), (void**)&spProperties);
		if (FAILED(hr))
			return hr;

		// Set connection properties
		CDBPropSet propSet(DBPROPSET_DBINIT);

		// Add Database name, User name and Password
		if (pName != NULL)
		{
			if(!propSet.AddProperty(DBPROP_INIT_DATASOURCE, pName))
			{
				return E_FAIL;
			}
		}

		if (pUserName != NULL)
		{
			if(!propSet.AddProperty(DBPROP_AUTH_USERID, pUserName))
			{
				return E_FAIL;
			}
		}

		if (pPassword != NULL)
		{
			if(!propSet.AddProperty(DBPROP_AUTH_PASSWORD, pPassword))
			{
				return E_FAIL;
			}
		}

		if (nInitMode)
		{
			if(!propSet.AddProperty(DBPROP_INIT_MODE, nInitMode))
			{
				return E_FAIL;
			}
		}

		hr = spProperties->SetProperties(1, &propSet);
		if (FAILED(hr))
			return hr;

		// Initialize the provider
		return m_spInit->Initialize();
	}
	HRESULT OpenWithProperties(
		_Inout_updates_opt_(nPropertySets) DBPROPSET* pPropSet,
		_In_ ULONG nPropertySets=1) throw()
	{
		ATLASSUME(m_spInit != NULL);

		// Set the properties if there are some to set
		if (pPropSet != NULL)
		{
			CComPtr<IDBProperties>  spProperties;
			HRESULT                 hr;

			hr = m_spInit->QueryInterface(__uuidof(IDBProperties), (void**)&spProperties);
			if (FAILED(hr))
				return hr;

			hr = spProperties->SetProperties(nPropertySets, pPropSet);
			if (FAILED(hr))
				return hr;
		}

		// Initialize the provider
		return m_spInit->Initialize();
	}

	CComPtr<IDBInitialize>  m_spInit;
};


///////////////////////////////////////////////////////////////////////////
// class CSession

class CSession
{
public:
	~CSession()
	{
		Close();
	}

	// Create a session on the passed datasource
	HRESULT Open(
		_In_ const CDataSource& ds,
		_Inout_updates_opt_(ulPropSets) DBPROPSET *pPropSet = NULL,
		_In_ ULONG ulPropSets = 0) throw()
	{
		CComPtr<IDBCreateSession> spSession;

		// Check we have connected to the database
		ATLASSERT(ds.m_spInit != NULL);

		HRESULT hr = ds.m_spInit->QueryInterface(__uuidof(IDBCreateSession), (void**)&spSession);
		if (FAILED(hr))
			return hr;

		hr = spSession->CreateSession(NULL, __uuidof(IOpenRowset), (IUnknown**)&m_spOpenRowset);

		if( pPropSet != NULL && SUCCEEDED(hr) && m_spOpenRowset != NULL )
		{
			// If the user didn't specify the default parameter, use one
			if (pPropSet != NULL && ulPropSets == 0)
				ulPropSets = 1;

			CComPtr<ISessionProperties> spSessionProperties;
			hr = m_spOpenRowset->QueryInterface(__uuidof(ISessionProperties), (void**)&spSessionProperties);
			if(FAILED(hr))
				return hr;

			hr = spSessionProperties->SetProperties( ulPropSets, pPropSet );
		}
		return hr;
	}

	// Close the session
	void Close() throw()
	{
		m_spOpenRowset.Release();
	}
	// Start a transaction
	HRESULT StartTransaction(
		_In_ ISOLEVEL isoLevel = ISOLATIONLEVEL_READCOMMITTED,
		_In_ ULONG isoFlags = 0,
		_In_opt_ ITransactionOptions* pOtherOptions = NULL,
		_Out_opt_ ULONG* pulTransactionLevel = NULL) const throw()
	{
		ATLASSUME(m_spOpenRowset != NULL);
		CComPtr<ITransactionLocal> spTransactionLocal;
		HRESULT hr = m_spOpenRowset->QueryInterface(&spTransactionLocal);

		if (SUCCEEDED(hr))
			hr = spTransactionLocal->StartTransaction(isoLevel, isoFlags, pOtherOptions, pulTransactionLevel);

		return hr;
	}
	// Abort the current transaction
	HRESULT Abort(
		_In_opt_ BOID* pboidReason = NULL,
		_In_ BOOL bRetaining = FALSE,
		_In_ BOOL bAsync = FALSE) const throw()
	{
		ATLASSUME(m_spOpenRowset != NULL);
		CComPtr<ITransaction> spTransaction;
		HRESULT hr = m_spOpenRowset->QueryInterface(&spTransaction);

		if (SUCCEEDED(hr))
			hr = spTransaction->Abort(pboidReason, bRetaining, bAsync);

		return hr;
	}
	// Commit the current transaction
	HRESULT Commit(
		_In_ BOOL bRetaining = FALSE,
		_In_ DWORD grfTC = XACTTC_SYNC,
		_In_ DWORD grfRM = 0) const throw()
	{
		ATLASSUME(m_spOpenRowset != NULL);
		CComPtr<ITransaction> spTransaction;
		HRESULT hr = m_spOpenRowset->QueryInterface(&spTransaction);

		if (SUCCEEDED(hr))
			hr = spTransaction->Commit(bRetaining, grfTC, grfRM);

		return hr;
	}
	// Get information for the current transaction
	HRESULT GetTransactionInfo(_Out_ XACTTRANSINFO* pInfo) const throw()
	{
		ATLASSUME(m_spOpenRowset != NULL);
		CComPtr<ITransaction> spTransaction;
		HRESULT hr = m_spOpenRowset->QueryInterface(&spTransaction);

		if (SUCCEEDED(hr))
			hr = spTransaction->GetTransactionInfo(pInfo);

		return hr;
	}
// Implementation
	CComPtr<IOpenRowset> m_spOpenRowset;
};

///////////////////////////////////////////////////////////////////////////
// class CDataConnection

class CDataConnection
{
public:
	CDataConnection()
	{
	}

	CDataConnection(_In_ const CDataConnection &ds)
	{
		Copy(ds);
	}

	CDataConnection& Copy(_In_ const CDataConnection &ds) throw()
	{
		m_source.m_spInit = ds.m_source.m_spInit;
		m_session.m_spOpenRowset = ds.m_session.m_spOpenRowset;
		return *this;
	}

	HRESULT Open(_In_z_ LPCOLESTR szInitString) throw()
	{
		HRESULT hr = E_FAIL;
		hr = m_source.OpenFromInitializationString(szInitString);
		if (hr == S_OK)
		{
			hr = m_session.Open(m_source);
		}
		return hr;
	}

	HRESULT OpenNewSession(_In_ CSession &session) throw()
	{
		return session.Open(m_source);
	}

	operator const CSession&() throw()
	{
		return m_session;
	}

	operator const CSession*() throw()
	{
		return &m_session;
	}

	operator const CDataSource&() throw()
	{
		return m_source;
	}

	operator const CDataSource*() throw()
	{
		return &m_source;
	}

	CDataConnection& operator=(_In_ const CDataConnection &ds) throw()
	{
		return Copy(ds);
	}

	operator BOOL() throw()
	{
		return m_session.m_spOpenRowset != NULL ? TRUE : FALSE;
	}

	operator bool() throw()
	{
		return m_session.m_spOpenRowset != NULL ? true : false;
	}

	void CloseDataSource() throw()
	{
		m_session.Close();
		m_source.Close();
	}

	CSession m_session;
	CDataSource m_source;
};

///////////////////////////////////////////////////////////////////////////
// CTable

template <class TAccessor = CNoAccessor, template <typename T> class TRowset = CRowset>
class CTable :
	public CAccessorRowset<TAccessor, TRowset>
{
public:
	// Open a rowset on the passed name
	HRESULT Open(
		_In_ const CSession& session,
		_In_z_ LPCWSTR wszTableName,
		_Inout_updates_opt_(ulPropSets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG ulPropSets = 0) throw()
	{
		DBID    idTable;

		idTable.eKind           = DBKIND_NAME;
		idTable.uName.pwszName  = (LPOLESTR)wszTableName;

		return Open(session, idTable, pPropSet, ulPropSets);
	}

	HRESULT Open(
		_In_ const CSession& session,
		_In_z_ LPCSTR szTableName,
		_Inout_updates_opt_(ulPropSets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG ulPropSets = 0) throw()
	{
		return Open( session, CComBSTR(szTableName), pPropSet, ulPropSets );
	}

	// Open the a rowset on the passed DBID
	HRESULT Open(
		_In_ const CSession& session,
		_In_ DBID& dbid,
		_Inout_updates_opt_(ulPropSets) DBPROPSET* pPropSet = NULL,
		_In_ ULONG ulPropSets = 0) throw()
	{
		// Check the session is valid
		ATLENSURE_RETURN(session.m_spOpenRowset != NULL);

		// If the user didn't specify the default parameter, use one for
		// backward compatibility
		if (pPropSet != NULL && ulPropSets == 0)
			ulPropSets = 1;

		HRESULT hr = session.m_spOpenRowset->OpenRowset(NULL, &dbid, NULL, this->GetIID(),
			ulPropSets, pPropSet, (IUnknown**)this->GetInterfacePtr());
		if (SUCCEEDED(hr))
		{
			this->SetupOptionalRowsetInterfaces();

			// If we have output columns then bind
			if (this->call_OutputColumnsClass_HasOutputColumns(ATLImplementationDetails::high_priority_tag{}))
				hr = this->Bind();
		}

		return hr;
	}

private:
	template<class Ty = CTable, class U = typename Ty::_OutputColumnsClass>
	decltype(auto) call_OutputColumnsClass_HasOutputColumns(ATLImplementationDetails::high_priority_tag)
	{
		return U::HasOutputColumns();
	}

	decltype(auto) call_OutputColumnsClass_HasOutputColumns(ATLImplementationDetails::low_priority_tag) const
	{
		return ::ATL::_OutputColumnsClass::HasOutputColumns();
	}
};

#if (OLEDBVER < 0x0150)
#define DBGUID_DEFAULT DBGUID_DBSQL
#endif


///////////////////////////////////////////////////////////////////////////
// CCommandBase

class CCommandBase
{
public:
	CCommandBase()
	{
		m_hParameterAccessor = 0;
	}

	~CCommandBase()
	{
		ReleaseCommand();
	}
	// Create the command
	HRESULT CreateCommand(_In_ const CSession& session) throw()
	{
		// Before creating the command, release the old one if necessary.
		ReleaseCommand();

		// Check the session is valid
		ATLASSERT(session.m_spOpenRowset != NULL);

		CComPtr<IDBCreateCommand> spCreateCommand;

		HRESULT hr = session.m_spOpenRowset->QueryInterface(__uuidof(IDBCreateCommand), (void**)&spCreateCommand);
		if (FAILED(hr))
			return hr;

		return spCreateCommand->CreateCommand(NULL, __uuidof(ICommand), (IUnknown**)&m_spCommand);
	}
	// Prepare the command
	HRESULT Prepare(_In_ ULONG cExpectedRuns = 0) throw()
	{
		CComPtr<ICommandPrepare> spCommandPrepare;
		HRESULT hr = m_spCommand->QueryInterface(&spCommandPrepare);
		if (SUCCEEDED(hr))
			hr = spCommandPrepare->Prepare(cExpectedRuns);

		return hr;
	}
	// Unprepare the command
	HRESULT Unprepare() throw()
	{
		CComPtr<ICommandPrepare> spCommandPrepare;
		HRESULT hr = m_spCommand->QueryInterface(&spCommandPrepare);
		if (SUCCEEDED(hr))
			hr = spCommandPrepare->Unprepare();

		return hr;
	}
	// Create the command and set the command text
	HRESULT Create(
		_In_ const CSession& session,
		_In_z_ LPCWSTR wszCommand,
		_In_ REFGUID guidCommand = DBGUID_DEFAULT) throw()
	{
		HRESULT hr;

		hr = CreateCommand(session);
		if (SUCCEEDED(hr))
		{
			CComPtr<ICommandText> spCommandText;
			hr = m_spCommand->QueryInterface(&spCommandText);
			if (SUCCEEDED(hr))
				hr = spCommandText->SetCommandText(guidCommand, wszCommand);
		}
		return hr;
	}
	HRESULT Create(
		_In_ const CSession& session,
		_In_z_ LPCSTR szCommand,
		_In_ REFGUID guidCommand = DBGUID_DEFAULT) throw()
	{
		return Create( session, CComBSTR(szCommand), guidCommand );
	}

	// Release the command
	void ReleaseCommand() throw()
	{
		// Release the parameter accessor if necessary, before releasing the command
		if (m_hParameterAccessor != 0 && m_spCommand != NULL )
		{
			CComPtr<IAccessor> spAccessor;
			HRESULT hr = m_spCommand->QueryInterface(&spAccessor);
			if (SUCCEEDED(hr))
			{
				spAccessor->ReleaseAccessor(m_hParameterAccessor, NULL);
				m_hParameterAccessor = 0;
			}
		}
		m_spCommand.Release();
	}

	// Get the parameter information from the command
	HRESULT GetParameterInfo(
		_Out_ DB_UPARAMS* pParams,
		_Outptr_result_buffer_maybenull_(*pParams) DBPARAMINFO** ppParamInfo,
		_Outptr_result_buffer_maybenull_(*pParams) LPOLESTR* ppNamesBuffer) throw()
	{
		CComPtr<ICommandWithParameters> spCommandParameters;
		HRESULT hr = m_spCommand->QueryInterface(&spCommandParameters);
		if (SUCCEEDED(hr))
		{
			// Get the parameter information
			hr = spCommandParameters->GetParameterInfo(pParams, ppParamInfo,
					ppNamesBuffer);
		}
		return hr;
	}

	// Set the parameter information for the command
	HRESULT SetParameterInfo(
		_In_ DB_UPARAMS ulParams,
		_In_reads_(ulParams) const DBORDINAL* pOrdinals,
		_In_reads_opt_(ulParams) const DBPARAMBINDINFO* pParamInfo) throw()
	{
		CComPtr<ICommandWithParameters> spCommandParameters;
		HRESULT hr = m_spCommand->QueryInterface(&spCommandParameters);
		if (SUCCEEDED(hr))
		{
			// Set the parameter information
			hr = spCommandParameters->SetParameterInfo(ulParams, pOrdinals,
				pParamInfo);
		}
		return hr;
	}

	CComPtr<ICommand>   m_spCommand;
	HACCESSOR           m_hParameterAccessor;
};

// Used to turn on multiple result set support in CCommand
class CMultipleResults
{
public:
	bool UseMultipleResults() throw()
	{
		return true;
	}
	IMultipleResults** GetMultiplePtrAddress() throw()
	{
		return &m_spMultipleResults.p;
	}
	IMultipleResults* GetMultiplePtr() throw()
	{
		return m_spMultipleResults;
	}

	CComPtr<IMultipleResults> m_spMultipleResults;
};

// Used to turn off multiple result set support in CCommand
class CNoMultipleResults
{
public:
	bool UseMultipleResults() throw()
	{
		return false;
	}
	IMultipleResults** GetMultiplePtrAddress() throw()
	{
		return NULL;
	}
	IMultipleResults* GetMultiplePtr() throw()
	{
		return NULL;
	}
};


///////////////////////////////////////////////////////////////////////////
// CCommand

ATLPREFAST_SUPPRESS(6387)
template <class TAccessor = CNoAccessor, template <typename T> class TRowset = CRowset,
			class TMultiple = CNoMultipleResults>
class CCommand :
	public CAccessorRowset<TAccessor, TRowset>,
	public CCommandBase,
	public TMultiple
{
public:
	// Create a command on the session and execute it
	HRESULT Open(
		_In_ const CSession& session,
		_In_opt_z_ LPCWSTR wszCommand,
		_Inout_updates_opt_(ulPropSets) DBPROPSET *pPropSet = NULL,
		_Out_opt_ DBROWCOUNT* pRowsAffected = NULL,
		_In_ REFGUID guidCommand = DBGUID_DEFAULT,
		_In_ bool bBind = true,
		_In_ ULONG ulPropSets = 0) throw()
	{
		HRESULT hr;

		if (wszCommand == NULL)
		{
			hr = this->call_CommandClass_GetDefaultCommand(ATLImplementationDetails::high_priority_tag{}, &wszCommand);
			if (FAILED(hr))
				return hr;
		}
		hr = Create(session, wszCommand, guidCommand);
		if (FAILED(hr))
			return hr;

		return Open(pPropSet, pRowsAffected, bBind, ulPropSets);
	}

	HRESULT Open(
		_In_ const CSession& session,
		_In_opt_z_ LPCSTR szCommand,
		_Inout_updates_opt_(ulPropSets) DBPROPSET *pPropSet = NULL,
		_Out_opt_ DBROWCOUNT* pRowsAffected = NULL,
		_In_ REFGUID guidCommand = DBGUID_DEFAULT,
		_In_ bool bBind = true,
		_In_ ULONG ulPropSets = 0) throw()
	{
		if( szCommand == NULL )
		{
			return Open( session, (LPCWSTR)NULL, pPropSet, pRowsAffected, guidCommand, bBind, ulPropSets );
		}
		else
		{
			return Open( session, CComBSTR(szCommand), pPropSet, pRowsAffected, guidCommand, bBind, ulPropSets );
		}
	}

	// this version of Open, takes an INT instead of a string pointer.
	// this is to resolve an ambiguity when calling
	// Open( session, NULL, ... ) or Open( session )
	HRESULT Open(
		_In_ const CSession& session,
		_In_ INT szCommand = NULL,
		_Inout_updates_opt_(ulPropSets) DBPROPSET *pPropSet = NULL,
		_Out_opt_ DBROWCOUNT* pRowsAffected = NULL,
		_In_ REFGUID guidCommand = DBGUID_DEFAULT,
		_In_ bool bBind = true,
		_In_ ULONG ulPropSets = 0) throw()
	{
		UNREFERENCED_PARAMETER(szCommand);
		ATLASSERT( szCommand == NULL );

		return Open( session, (LPCWSTR)NULL, pPropSet, pRowsAffected, guidCommand, bBind, ulPropSets );
	}

	// Used if you have previously created the command
	HRESULT Open(
		_Inout_updates_opt_(ulPropSets) DBPROPSET *pPropSet = NULL,
		_Out_opt_ DBROWCOUNT* pRowsAffected = NULL,
		_In_ bool bBind = true,
		_In_ ULONG ulPropSets = 0) throw()
	{
		HRESULT     hr;
		DBPARAMS    params;
		DBPARAMS    *pParams;

		// Bind the parameters if we have some
		if (this->call_ParamClass_HasParameters(ATLImplementationDetails::high_priority_tag{}))
		{
			// Bind the parameters in the accessor if they haven't already been bound
			hr = this->BindParameters(&m_hParameterAccessor, m_spCommand, &params.pData);
			if (FAILED(hr))
				return hr;

			// Setup the DBPARAMS structure
			params.cParamSets = 1;
			params.hAccessor = m_hParameterAccessor;
			pParams = &params;
		}
		else
			pParams = NULL;

		return ExecuteAndBind(pParams, pPropSet, pRowsAffected, bBind, ulPropSets);
	}
	// Get the next rowset when using multiple result sets
	HRESULT GetNextResult(
		_Out_ DBROWCOUNT* pulRowsAffected,
		_In_ bool bBind = true) throw()
	{
		// This function should only be called if CMultipleResults is being
		// used as the third template parameter
		ATLASSERT(this->GetMultiplePtrAddress() != NULL);

		// If user calls GetNextResult but the interface is not available
		// return E_FAIL.
		if (this->GetMultiplePtr() == NULL)
			return E_FAIL;

		// Close the existing rowset in preparation for opening the next one
		this->Close();

		HRESULT hr = this->GetMultiplePtr()->GetResult(NULL, 0, this->GetIID(),
			pulRowsAffected, (IUnknown**)this->GetInterfacePtr());
		if (FAILED(hr))
			return hr;

		if (bBind && this->GetInterface() != NULL)
			return this->Bind();
		else
			return hr;
	}

// Implementation
	HRESULT ExecuteAndBind(
		_In_opt_ DBPARAMS* pParams,
		_Inout_updates_opt_(ulPropSets) DBPROPSET* pPropSet = NULL,
		_Out_opt_ DBROWCOUNT* pRowsAffected = NULL,
		_In_ bool bBind = true,
		_In_ ULONG ulPropSets = 0) throw()
	{
		HRESULT hr = Execute((IUnknown**)this->GetInterfacePtr(), pParams, pPropSet,
							pRowsAffected, ulPropSets);
		if (FAILED(hr))
			return hr;

		// Only bind if we have been asked to and we have output columns
		if (bBind && this->call_OutputColumnsClass_HasOutputColumns(ATLImplementationDetails::high_priority_tag{}))
		{
			// for dynamic accessors we don't want to automatically call Bind if we got no rowset in return
			if( this->NoBindOnNullRowset() && this->GetInterface() == NULL )
				return hr;
			else
				return this->Bind();
		}
		else
			return hr;
	}

	HRESULT Execute(
		_Outptr_opt_result_maybenull_ IRowset** ppRowset,
		_Inout_updates_opt_(ulPropSets) DBPARAMS* pParams,
		_In_ DBPROPSET *pPropSet,
		_Out_opt_ DBROWCOUNT* pRowsAffected,
		_In_ ULONG ulPropSets = 0) throw()
	{
		return Execute( (IUnknown**)ppRowset, pParams, pPropSet, pRowsAffected, ulPropSets );
	}


	HRESULT Execute(
		_Outptr_opt_result_maybenull_ IUnknown** ppInterface,
		_In_opt_ DBPARAMS* pParams,
		_Inout_updates_opt_(ulPropSets) DBPROPSET *pPropSet,
		_Out_opt_ DBROWCOUNT* pRowsAffected,
		_In_ ULONG ulPropSets = 0) throw()
	{
		HRESULT hr;
		if (ppInterface)
			*ppInterface = NULL;

		// Specify the properties if we have some
		if (pPropSet)
		{
			// For backward compatibility, if the default parameter is not
			// specified, then set it to one if a property set exists
			if (ulPropSets == 0)
				ulPropSets = 1;

			CComPtr<ICommandProperties> spCommandProperties;
			hr = m_spCommand->QueryInterface(&spCommandProperties);
			if (FAILED(hr))
				return hr;

			hr = spCommandProperties->SetProperties(ulPropSets, pPropSet);
			if (FAILED(hr))
				return hr;
		}

		// If the user want the rows affected then return it back, otherwise
		// just point to our local variable here.
		DBROWCOUNT nAffected, *pAffected;
		if (pRowsAffected)
			pAffected = pRowsAffected;
		else
			pAffected = &nAffected;

		if (this->UseMultipleResults())
		{
			hr = m_spCommand->Execute(NULL, __uuidof(IMultipleResults), pParams,
				pAffected, (IUnknown**)this->GetMultiplePtrAddress());

			if (SUCCEEDED(hr))
			{
				hr = GetNextResult(pAffected, false);
			}
			else
				// If we can't get IMultipleResults then just try to get IRowset
				hr = m_spCommand->Execute(NULL, this->GetIID(), pParams, pAffected,
					ppInterface);
		}
		else
		{
			hr = m_spCommand->Execute(NULL, this->GetIID(), pParams, pAffected,
				ppInterface);
		}
		if (SUCCEEDED(hr))
			this->SetupOptionalRowsetInterfaces();

		return hr;
	}

private:
	template<class Ty = CCommand, class U = typename Ty::_OutputColumnsClass>
	decltype(auto) call_OutputColumnsClass_HasOutputColumns(ATLImplementationDetails::high_priority_tag)
	{
		return U::HasOutputColumns();
	}

	decltype(auto) call_OutputColumnsClass_HasOutputColumns(ATLImplementationDetails::low_priority_tag) const
	{
		return ::ATL::_OutputColumnsClass::HasOutputColumns();
	}

	template<class Ty = CCommand, class U = typename Ty::_ParamClass>
	decltype(auto) call_ParamClass_HasParameters(ATLImplementationDetails::high_priority_tag)
	{
		return U::HasParameters();
	}

	decltype(auto) call_ParamClass_HasParameters(ATLImplementationDetails::low_priority_tag) const
	{
		return ::ATL::_ParamClass::HasParameters();
	}

	template<class Ty = CCommand, class U = typename Ty::_CommandClass, class... Args>
	decltype(auto) call_CommandClass_GetDefaultCommand(ATLImplementationDetails::high_priority_tag, Args&&... args)
	{
		return U::GetDefaultCommand(::std::forward<Args>(args)...);
	}

	template<class... Args>
	decltype(auto) call_CommandClass_GetDefaultCommand(ATLImplementationDetails::low_priority_tag, Args&&... args) const
	{
		return ::ATL::_CommandClass::GetDefaultCommand(::std::forward<Args>(args)...);
	}
};
ATLPREFAST_UNSUPPRESS()


// This class can be used to implement the IRowsetNotify interface.
// It is supplied so that if you only want to implement one of the
// notifications you don't have to supply empty functions for the
// other methods.
class ATL_NO_VTABLE IRowsetNotifyImpl :
	public IRowsetNotify
{
public:
	STDMETHOD(OnFieldChange)(
		_In_opt_ IRowset* /* pRowset */,
		HROW /* hRow */,
		DBORDINAL cColumns /* cColumns */,
		_In_reads_(cColumns) DBORDINAL /* rgColumns*/ [] ,
		DBREASON /* eReason */,
		DBEVENTPHASE /* ePhase */,
		BOOL /* fCantDeny */)
	{
		UNREFERENCED_PARAMETER(cColumns);
		ATLTRACENOTIMPL(_T("IRowsetNotifyImpl::OnFieldChange"));
	}
	STDMETHOD(OnRowChange)(
		_In_opt_ IRowset* /* pRowset */,
		DBCOUNTITEM cRows /* cRows */,
		_In_reads_(cRows) const HROW /* rghRows*/ [] ,
		DBREASON /* eReason */,
		DBEVENTPHASE /* ePhase */,
		BOOL /* fCantDeny */)
	{
		UNREFERENCED_PARAMETER(cRows);
		ATLTRACENOTIMPL(_T("IRowsetNotifyImpl::OnRowChange"));
	}
	STDMETHOD(OnRowsetChange)(
		_In_opt_ IRowset* /* pRowset */,
		DBREASON /* eReason */,
		DBEVENTPHASE /* ePhase */,
		BOOL /* fCantDeny*/)
	{
		ATLTRACENOTIMPL(_T("IRowsetNotifyImpl::OnRowsetChange"));
	}
};


}; //namespace ATL

#ifdef _ARM_WORKAROUND_
// OLEDB headers set packing to 2
// There's gap where there should be padding on ARM because it requires
// 8 byte align for double yet pointers are only on 4 byte boundaries
// So it's different from x86 and x64
// Restore warning state pushed above
#pragma warning(pop)
#endif

#pragma pack(pop)
#pragma warning(pop)

#endif // __ATLDBCLI_H__
