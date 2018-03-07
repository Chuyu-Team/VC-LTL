// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFX.H

#ifndef AFX_H_TEMPLATE_INCLUDE_GUARD
#define AFX_H_TEMPLATE_INCLUDE_GUARD
//Used CStringT to serialize CSimpleStringT because it supports conversions (char <--> wchar_t).
template < typename BaseType , bool t_bMFCDLL>
CArchive& CArchive::operator<<(const ATL::CSimpleStringT<BaseType, t_bMFCDLL>& str)
{
	ATL::CStringT< BaseType, StrTraitMFC_DLL< BaseType > > cstrT(str);
	*this << cstrT;
	return *this;
}	

template< typename BaseType, class StringTraits >
CArchive& CArchive::operator<<(const ATL::CStringT<BaseType, StringTraits>& str)
{
	AfxWriteStringLength(*this, str.GetLength(), sizeof(BaseType) == sizeof(wchar_t));
	Write(str, str.GetLength()*sizeof(BaseType));
	return *this;
}   

template < typename BaseType , bool t_bMFCDLL>
CArchive& CArchive::operator>>(ATL::CSimpleStringT<BaseType, t_bMFCDLL>& str)
{
	ATL::CStringT< BaseType, StrTraitMFC_DLL< BaseType > > cstrT;
	*this >> cstrT;
	str = cstrT;
	return *this;
}

template< typename BaseType, class StringTraits >
CArchive& CArchive::operator>>(ATL::CStringT<BaseType, StringTraits>& str)
{
	int nCharSize;  // 1 = char, 2 = wchar_t
	UINT nLength = UINT( AfxReadStringLength(*this, nCharSize) );
	if (nCharSize == sizeof(char))
	{
		ATL::CTempBuffer< char > pszBufferA(nLength);

		UINT nBytesRead = Read(pszBufferA, nLength*sizeof(char));
		if (nBytesRead != (nLength*sizeof(char)))
			AfxThrowArchiveException(CArchiveException::endOfFile);
		str = ATL::CStringT<BaseType, StringTraits>(pszBufferA, nLength);
	}
	else
	{
		ASSERT(nCharSize == sizeof(wchar_t));

		ATL::CTempBuffer< wchar_t > pszBufferW(nLength);

		UINT nBytesRead = Read(pszBufferW, nLength*sizeof(wchar_t));
		if (nBytesRead != (nLength*sizeof(wchar_t)))
			AfxThrowArchiveException(CArchiveException::endOfFile);
		str = ATL::CStringT<BaseType, StringTraits>(pszBufferW, nLength);
	}

	return *this;
}

#endif //AFX_H_TEMPLATE_INCLUDE_GUARD

#ifndef AFX_INL_MANAGED_INIT_INCLUDE_GUARD
#define AFX_INL_MANAGED_INIT_INCLUDE_GUARD
#if !defined(_AFX_MFCS) && !defined(_AFX_ISAPI)
__declspec( noinline ) inline int __cdecl  _AfxInitManaged()
{
	return 0;
}
#endif // !_AFX_MFCS && !_AFX_ISAPI
extern "C"
{
__declspec(selectany) void* _pIncludeInitManaged = (void*)_AfxInitManaged;
}
#endif  // AFX_INL_MANAGED_INIT_INCLUDE_GUARD

#ifdef _AFX_INLINE
#ifndef AFX_INL_INCLUDE_GUARD
#define AFX_INL_INCLUDE_GUARD

// CObject
_AFX_INLINE CObject::CObject()
	{ }
_AFX_INLINE CObject::~CObject()
	{ }
_AFX_INLINE void CObject::Serialize(CArchive&)
	{ /* CObject does not serialize anything by default */ }
_AFX_INLINE void* PASCAL CObject::operator new(size_t, void* p)
	{ return p; }
#ifndef _DEBUG
// _DEBUG versions in afxmem.cpp
_AFX_INLINE void PASCAL CObject::operator delete(void* p)
	{ ::operator delete(p); }
_AFX_INLINE void PASCAL CObject::operator delete(void* p, void*)
	{ ::operator delete(p); }
_AFX_INLINE void* PASCAL CObject::operator new(size_t nSize)
	{ return ::operator new(nSize); }
// _DEBUG versions in objcore.cpp
#ifdef _AFXDLL
_AFX_INLINE void CObject::AssertValid() const
	{ /* no asserts in release builds */ }
_AFX_INLINE void CObject::Dump(CDumpContext&) const
	{ /* no dumping in release builds */ }
#endif //_AFXDLL
#endif //!_DEBUG
_AFX_INLINE const CObject* AFX_CDECL AfxDynamicDownCast(CRuntimeClass* pClass, const CObject* pObject)
	{ return (const CObject*)AfxDynamicDownCast(pClass, (CObject*)pObject); }
#ifdef _DEBUG
_AFX_INLINE const CObject* AFX_CDECL AfxStaticDownCast(CRuntimeClass* pClass, const CObject* pObject)
	{ return (const CObject*)AfxStaticDownCast(pClass, (CObject*)pObject); }
#endif

// exceptions
_AFX_INLINE CException::~CException()
	{ }
_AFX_INLINE CSimpleException::CSimpleException()
	{ m_bInitialized = FALSE; m_bLoaded = FALSE; }
_AFX_INLINE CSimpleException::CSimpleException(BOOL bAutoDelete)
	: CException(bAutoDelete) { m_bInitialized = FALSE; m_bLoaded = FALSE; }
_AFX_INLINE CSimpleException::~CSimpleException()
	{ }

_AFX_INLINE CMemoryException::CMemoryException()
	: CSimpleException() { }
_AFX_INLINE CMemoryException::CMemoryException(BOOL bAutoDelete, UINT nResourceID)
	: CSimpleException(bAutoDelete) { m_nResourceID = nResourceID; }
_AFX_INLINE CMemoryException::~CMemoryException()
	{ }
_AFX_INLINE CNotSupportedException::CNotSupportedException()
	: CSimpleException() { }
_AFX_INLINE CNotSupportedException::CNotSupportedException(BOOL bAutoDelete, UINT nResourceID)
	: CSimpleException(bAutoDelete) { m_nResourceID = nResourceID; }
_AFX_INLINE CNotSupportedException::~CNotSupportedException()
	{ }
_AFX_INLINE CInvalidArgException::CInvalidArgException()
	: CSimpleException() { }
_AFX_INLINE CInvalidArgException::CInvalidArgException(BOOL bAutoDelete, UINT nResourceID)
	: CSimpleException(bAutoDelete) { m_nResourceID = nResourceID; }
_AFX_INLINE CInvalidArgException::~CInvalidArgException()
	{ }
_AFX_INLINE CArchiveException::CArchiveException(int cause,
	LPCTSTR lpszFileName /* = NULL */)
	{ m_cause = cause; m_strFileName = lpszFileName; }
	
_AFX_INLINE CArchiveException::~CArchiveException()
	{ }
_AFX_INLINE CFileException::CFileException(int cause, LONG lOsError,
	LPCTSTR pstrFileName /* = NULL */)
	{ m_cause = cause; m_lOsError = lOsError; m_strFileName = pstrFileName; }
_AFX_INLINE CFileException::~CFileException()
	{ }

// CFile
_AFX_INLINE CFile::operator HANDLE() const
	{ return m_hFile; }
_AFX_INLINE ULONGLONG CFile::SeekToEnd()
	{ return Seek(0, CFile::end); }
_AFX_INLINE void CFile::SeekToBegin()
	{ Seek(0, CFile::begin); }
_AFX_INLINE void CFile::SetFilePath(LPCTSTR lpszNewName)
{
	ASSERT_VALID(this);
	ASSERT(AfxIsValidString(lpszNewName));
	if(lpszNewName != NULL)
		m_strFileName = lpszNewName;
	else
		AfxThrowInvalidArgException();  

}

// CFileFind
_AFX_INLINE BOOL CFileFind::IsReadOnly() const
	{ return MatchesMask(FILE_ATTRIBUTE_READONLY); }
_AFX_INLINE BOOL CFileFind::IsDirectory() const
	{ return MatchesMask(FILE_ATTRIBUTE_DIRECTORY); }
_AFX_INLINE BOOL CFileFind::IsCompressed() const
	{ return MatchesMask(FILE_ATTRIBUTE_COMPRESSED); }
_AFX_INLINE BOOL CFileFind::IsSystem() const
	{ return MatchesMask(FILE_ATTRIBUTE_SYSTEM); }
_AFX_INLINE BOOL CFileFind::IsHidden() const
	{ return MatchesMask(FILE_ATTRIBUTE_HIDDEN); }
_AFX_INLINE BOOL CFileFind::IsTemporary() const
	{ return MatchesMask(FILE_ATTRIBUTE_TEMPORARY); }
_AFX_INLINE BOOL CFileFind::IsNormal() const
	{ return MatchesMask(FILE_ATTRIBUTE_NORMAL); }
_AFX_INLINE BOOL CFileFind::IsArchived() const
	{ return MatchesMask(FILE_ATTRIBUTE_ARCHIVE); }

// CArchive
_AFX_INLINE BOOL CArchive::IsLoading() const
	{ return (m_nMode & CArchive::load) != 0; }
_AFX_INLINE BOOL CArchive::IsStoring() const
	{ return (m_nMode & CArchive::load) == 0; }
_AFX_INLINE BOOL CArchive::IsByteSwapping() const
	{ return FALSE; }
_AFX_INLINE BOOL CArchive::IsBufferEmpty() const
	{ return m_lpBufCur == m_lpBufMax; }
_AFX_INLINE CFile* CArchive::GetFile() const
	{ return m_pFile; }
_AFX_INLINE void CArchive::SetObjectSchema(UINT nSchema)
	{ m_nObjectSchema = nSchema; }
_AFX_INLINE void CArchive::SetStoreParams(UINT nHashSize, UINT nBlockSize)
{
	ASSERT(IsStoring());
	ASSERT(m_pStoreMap == NULL);    // must be before first object written
	m_nHashSize = nHashSize;
	m_nGrowSize = nBlockSize;
}
_AFX_INLINE void CArchive::SetLoadParams(UINT nGrowBy)
{
	ASSERT(IsLoading());
	ASSERT(m_pLoadArray == NULL);   // must be before first object read
	m_nGrowSize = nGrowBy;
}
_AFX_INLINE CArchive& CArchive::operator<<(int i)
	{ return CArchive::operator<<((LONG)i); }
_AFX_INLINE CArchive& CArchive::operator<<(unsigned u)
	{ return CArchive::operator<<((LONG)u); }
_AFX_INLINE CArchive& CArchive::operator<<(short w)
	{ return CArchive::operator<<((WORD)w); }
_AFX_INLINE CArchive& CArchive::operator<<(char ch)
	{ return CArchive::operator<<((BYTE)ch); }
#ifdef _NATIVE_WCHAR_T_DEFINED
_AFX_INLINE CArchive& CArchive::operator<<(wchar_t ch)
	{ return CArchive::operator<<((WORD)ch); }
#endif
_AFX_INLINE CArchive& CArchive::operator<<(bool b)
	{ return CArchive::operator <<((BYTE)(b ? 1 : 0)); }
_AFX_INLINE CArchive& CArchive::operator<<(BYTE by)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax) Flush();
		*(UNALIGNED BYTE*)m_lpBufCur = by; m_lpBufCur += sizeof(BYTE); return *this; 
}

_AFX_INLINE CArchive& CArchive::operator<<(LONGLONG dwdw)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(LONGLONG) > m_lpBufMax) Flush();
		*(UNALIGNED LONGLONG*)m_lpBufCur = dwdw; m_lpBufCur += sizeof(LONGLONG); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator<<(ULONGLONG dwdw)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(ULONGLONG) > m_lpBufMax) Flush();
		*(UNALIGNED ULONGLONG*)m_lpBufCur = dwdw; m_lpBufCur += sizeof(ULONGLONG); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator<<(WORD w)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(WORD) > m_lpBufMax) Flush();
			*(UNALIGNED WORD*)m_lpBufCur = w; m_lpBufCur += sizeof(WORD); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator<<(LONG l)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(LONG) > m_lpBufMax) Flush();
		*(UNALIGNED LONG*)m_lpBufCur = l; m_lpBufCur += sizeof(LONG); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator<<(DWORD dw)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax) Flush();
		*(UNALIGNED DWORD*)m_lpBufCur = dw; m_lpBufCur += sizeof(DWORD); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator<<(float f)
{ 
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(float) > m_lpBufMax) Flush();
		*(UNALIGNED float*)m_lpBufCur = f; m_lpBufCur += sizeof(float); return *this;
}
_AFX_INLINE CArchive& CArchive::operator<<(double d)
{
	if(!IsStoring())
		AfxThrowArchiveException(CArchiveException::readOnly,m_strFileName);
	if (m_lpBufCur + sizeof(double) > m_lpBufMax) Flush();
		*(UNALIGNED double*)m_lpBufCur = d; m_lpBufCur += sizeof(double); return *this; 
}

_AFX_INLINE CArchive& CArchive::operator>>(int& i)
	{ return CArchive::operator>>((LONG&)i); }
_AFX_INLINE CArchive& CArchive::operator>>(unsigned& u)
	{ return CArchive::operator>>((LONG&)u); }
_AFX_INLINE CArchive& CArchive::operator>>(short& w)
	{ return CArchive::operator>>((WORD&)w); }
_AFX_INLINE CArchive& CArchive::operator>>(char& ch)
	{ return CArchive::operator>>((BYTE&)ch); }
#ifdef _NATIVE_WCHAR_T_DEFINED
_AFX_INLINE CArchive& CArchive::operator>>(wchar_t& ch)
	{ return CArchive::operator>>((WORD&)ch); }
#endif
_AFX_INLINE CArchive& CArchive::operator>>(bool& b)
	{ BYTE by; CArchive& ar = CArchive::operator>>(by); b = (by ? true : false); return ar; }
_AFX_INLINE CArchive& CArchive::operator>>(BYTE& by)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax)
		FillBuffer(UINT(sizeof(BYTE) - (m_lpBufMax - m_lpBufCur)));
	by = *(UNALIGNED BYTE*)m_lpBufCur; m_lpBufCur += sizeof(BYTE); return *this; 
}

_AFX_INLINE CArchive& CArchive::operator>>(LONGLONG& dwdw)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(LONGLONG) > m_lpBufMax)
		FillBuffer(sizeof(LONGLONG) - (UINT)(m_lpBufMax - m_lpBufCur));
	dwdw = *(UNALIGNED LONGLONG*)m_lpBufCur; m_lpBufCur += sizeof(LONGLONG); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator>>(ULONGLONG& dwdw)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(ULONGLONG) > m_lpBufMax)
		FillBuffer(sizeof(ULONGLONG) - (UINT)(m_lpBufMax - m_lpBufCur));
	dwdw = *(UNALIGNED ULONGLONG*)m_lpBufCur; m_lpBufCur += sizeof(ULONGLONG); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator>>(WORD& w)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(WORD) > m_lpBufMax)
		FillBuffer(UINT(sizeof(WORD) - (m_lpBufMax - m_lpBufCur)));
	w = *(UNALIGNED WORD*)m_lpBufCur; m_lpBufCur += sizeof(WORD); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator>>(DWORD& dw)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax)
		FillBuffer(UINT(sizeof(DWORD) - (m_lpBufMax - m_lpBufCur)));
	dw = *(UNALIGNED DWORD*)m_lpBufCur; m_lpBufCur += sizeof(DWORD); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator>>(float& f)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(float) > m_lpBufMax)
		FillBuffer(UINT(sizeof(float) - (m_lpBufMax - m_lpBufCur)));
	f = *(UNALIGNED float*)m_lpBufCur; m_lpBufCur += sizeof(float); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator>>(double& d)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(double) > m_lpBufMax)
		FillBuffer(UINT(sizeof(double) - (m_lpBufMax - m_lpBufCur)));
	d = *(UNALIGNED double*)m_lpBufCur; m_lpBufCur += sizeof(double); return *this; 
}
_AFX_INLINE CArchive& CArchive::operator>>(LONG& l)
{ 
	if(!IsLoading())
		AfxThrowArchiveException(CArchiveException::writeOnly,m_strFileName);
	if (m_lpBufCur + sizeof(LONG) > m_lpBufMax)
		FillBuffer(UINT(sizeof(LONG) - (m_lpBufMax - m_lpBufCur)));
	l = *(UNALIGNED LONG*)m_lpBufCur; m_lpBufCur += sizeof(LONG); return *this; 
}

_AFX_INLINE CArchive::CArchive(const CArchive& /* arSrc */)
	{ }
_AFX_INLINE void CArchive::operator=(const CArchive& /* arSrc */)
	{ }
_AFX_INLINE CArchive& AFXAPI operator<<(CArchive& ar, const CObject* pOb)
	{ ar.WriteObject(pOb); return ar; }
_AFX_INLINE CArchive& AFXAPI operator>>(CArchive& ar, CObject*& pOb)
	{ pOb = ar.ReadObject(NULL); return ar; }
_AFX_INLINE CArchive& AFXAPI operator>>(CArchive& ar, const CObject*& pOb)
	{ pOb = ar.ReadObject(NULL); return ar; }

_AFX_INLINE void CArchive::EnsureRead(void *lpBuf, UINT nCount)
{
	UINT nRead=Read(lpBuf, nCount);
	if(nRead!=nCount)
	{
		AfxThrowArchiveException(CArchiveException::endOfFile);
	}
}

// CDumpContext
_AFX_INLINE int CDumpContext::GetDepth() const
	{ return m_nDepth; }
_AFX_INLINE void CDumpContext::SetDepth(int nNewDepth)
	{ m_nDepth = nNewDepth; }
_AFX_INLINE CDumpContext::CDumpContext(const CDumpContext& /* dcSrc */)
	{ }
_AFX_INLINE void CDumpContext::operator=(const CDumpContext& /* dcSrc */)
	{ }

/////////////////////////////////////////////////////////////////////////////
#endif //AFX_INL_INCLUDE_GUARD
#endif //_AFX_INLINE
