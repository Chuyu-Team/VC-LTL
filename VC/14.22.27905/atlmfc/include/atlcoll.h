// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLCOLL_H__
#define __ATLCOLL_H__

#pragma once

#include <atldef.h>
#include <atlbase.h>
#include <new.h>

#pragma warning(push)
#pragma warning(disable: 4702)  // Unreachable code.  This file will have lots of it, especially without EH enabled.
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4290)  // C++ Exception Specification ignored
#pragma warning(disable: 4127)  // conditional expression constant
#pragma warning(disable: 4571)  //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions

// abstract iteration position
#ifndef _AFX
struct __POSITION
{
};
#endif
typedef __POSITION* POSITION;

#pragma pack(push,_ATL_PACKING)
namespace ATL {

#pragma warning(push)
#pragma warning(disable:4324)
__declspec(align(8)) struct CAtlPlex
{
	CAtlPlex* pNext;

	void* data()
	{
		return this+1;
	}

	static CAtlPlex* Create(
		_Inout_ CAtlPlex*& head,
		_In_ size_t nMax,
		_In_ size_t cbElement);
			// like 'calloc' but no zero fill
			// may throw memory exceptions

	void FreeDataChain();       // free this one and links
};
#pragma warning(pop)

inline CAtlPlex* CAtlPlex::Create(
	_Inout_ CAtlPlex*& pHead,
	_In_ size_t nMax,
	_In_ size_t nElementSize)
{
	CAtlPlex* pPlex;

	ATLASSERT( nMax > 0 );
	ATLASSERT( nElementSize > 0 );

	size_t nBytes=0;
	if( 	FAILED(::ATL::AtlMultiply(&nBytes, nMax, nElementSize)) ||
		FAILED(::ATL::AtlAdd(&nBytes, nBytes, sizeof(CAtlPlex))) )
	{
		return NULL;
	}
	pPlex = static_cast< CAtlPlex* >( malloc( nBytes ) );
	if( pPlex == NULL )
	{
		return( NULL );
	}

	pPlex->pNext = pHead;
	pHead = pPlex;

	return( pPlex );
}

inline void CAtlPlex::FreeDataChain()
{
	CAtlPlex* pPlex;

	pPlex = this;
	while( pPlex != NULL )
	{
		CAtlPlex* pNextPlex;

		pNextPlex = pPlex->pNext;
		free( pPlex );
		pPlex = pNextPlex;
	}
}

template< typename T >
class CElementTraitsBase
{
public:
	typedef const T& INARGTYPE;
	typedef T& OUTARGTYPE;

	static void CopyElements(
		_Out_writes_all_(nElements) T* pDest,
		_In_reads_(nElements) const T* pSrc,
		_In_ size_t nElements)
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}

	static void RelocateElements(
		_Out_writes_all_(nElements) T* pDest,
		_In_reads_(nElements) T* pSrc,
		_In_ size_t nElements)
	{
		// A simple memmove works for nearly all types.
		// You'll have to override this for types that have pointers to their
		// own members.
		Checked::memmove_s( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ));
	}
};

template< typename T >
class CDefaultHashTraits
{
public:
	static ULONG Hash(_In_ const T& element) throw()
	{
		return( ULONG( ULONG_PTR( element ) ) );
	}
};

template< typename T >
class CDefaultCompareTraits
{
public:
	static bool CompareElements(
		_In_ const T& element1,
		_In_ const T& element2)
	{
		return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
	}

	static int CompareElementsOrdered(
		_In_ const T& element1,
		_In_ const T& element2)
	{
		if( element1 < element2 )
		{
			return( -1 );
		}
		else if( element1 == element2 )
		{
			return( 0 );
		}
		else
		{
			ATLASSERT( element1 > element2 );
			return( 1 );
		}
	}
};

template< typename T >
class CDefaultElementTraits :
	public CElementTraitsBase< T >,
	public CDefaultHashTraits< T >,
	public CDefaultCompareTraits< T >
{
};

template< typename T >
class CElementTraits :
	public CDefaultElementTraits< T >
{
};

template<>
class CElementTraits< GUID > :
	public CElementTraitsBase< GUID >
{
public:
	static ULONG Hash(_In_ INARGTYPE guid)
	{
		const DWORD* pdwData = reinterpret_cast< const DWORD* >( &guid );

		return( pdwData[0]^pdwData[1]^pdwData[2]^pdwData[3] );
	}

	static bool CompareElements(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2)
	{
		return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
	}

	static int CompareElementsOrdered(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2)
	{
		const DWORD* pdwData1 = reinterpret_cast< const DWORD* >( &element1 );
		const DWORD* pdwData2 = reinterpret_cast< const DWORD* >( &element2 );

		for( int iDWORD = 3; iDWORD >= 0; iDWORD-- )
		{
			if( pdwData1[iDWORD] > pdwData2[iDWORD] )
			{
				return( 1 );
			}
			else if( pdwData1[iDWORD] < pdwData2[iDWORD] )
			{
				return( -1 );
			}
		}

		return( 0 );
	}
};

template<>
class CElementTraits< CComVariant > :
	public CElementTraitsBase< CComVariant >
{
public:
	typedef const VARIANT& INARGTYPE;

//	static ULONG Hash( INARGTYPE t );  // variant hashing is problematic

	static bool CompareElements(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2)
	{
		return VarCmp(const_cast<VARIANT*>(&element1), const_cast<VARIANT*>(&element2), LOCALE_USER_DEFAULT, 0)==static_cast<HRESULT>(VARCMP_EQ);
	}

	static int CompareElementsOrdered(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2)
	{
		HRESULT hr = VarCmp(const_cast<VARIANT*>(&element1), const_cast<VARIANT*>(&element2), LOCALE_USER_DEFAULT, 0);
		if( hr == static_cast<HRESULT>(VARCMP_LT) )
		{
			return( -1 );
		}
		else if( hr == static_cast<HRESULT>(VARCMP_GT) )
		{
			return( 1 );
		}
		else
		{
			ATLASSERT( hr == static_cast<HRESULT>(VARCMP_EQ) || hr == static_cast<HRESULT>(VARCMP_NULL) );
			return( 0 );
		}
	}
};

template<>
class CElementTraits< CComBSTR > :
	public CElementTraitsBase< CComBSTR >
{
public:
	static ULONG Hash(_In_ INARGTYPE bstr) throw()
	{
		ULONG nHash = 0;
		const OLECHAR* pch = bstr;
		ULONG nLength = bstr.Length();
		for( ULONG iChar = 0; iChar < nLength; iChar++ )
		{
			nHash = (nHash<<5)+nHash+pch[iChar];
		}

		return( nHash );
	}

	static bool CompareElements(
		_In_ INARGTYPE bstr1,
		_In_ INARGTYPE bstr2) throw()
	{
		return( bstr1 == bstr2 );
	}

	static int CompareElementsOrdered(
		_In_ INARGTYPE bstr1,
		_In_ INARGTYPE bstr2) throw()
	{
		HRESULT hr = VarBstrCmp( bstr1, bstr2, LOCALE_SYSTEM_DEFAULT, 0 );
		switch( hr )
		{
		case static_cast<HRESULT>(VARCMP_LT):
			return( -1 );
			break;

		case static_cast<HRESULT>(VARCMP_GT):
			return( 1 );
			break;

		case static_cast<HRESULT>(VARCMP_EQ):
			return( 0 );
			break;

		default:
			ATLASSERT( false );
			return( 0 );
			break;
		}
	}
};

template< typename I, const IID* piid = &__uuidof( I ) >
class CComQIPtrElementTraits :
	public CDefaultElementTraits< ATL::CComQIPtr< I, piid > >
{
public:
	typedef I* INARGTYPE;
};

template< typename T >
class CAutoPtrElementTraits :
	public CDefaultElementTraits< ATL::CAutoPtr< T > >
{
public:
	typedef ATL::CAutoPtr< T >& INARGTYPE;
	typedef T*& OUTARGTYPE;

	// Specialize copy elements to allow non-const since we transfer ownership on assignment
	static void CopyElements(
		_Out_writes_(nElements) ::ATL::CAutoPtr< T >* pDest,
		_In_reads_(nElements) ::ATL::CAutoPtr< T >* pSrc,
		_In_ size_t nElements)
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}
};

template< typename T >
class CAutoVectorPtrElementTraits :
	public CDefaultElementTraits< ATL::CAutoVectorPtr< T > >
{
public:
	typedef ATL::CAutoVectorPtr< T >& INARGTYPE;
	typedef T*& OUTARGTYPE;

	// Specialize copy elements to allow non-const since we transfer ownership on assignment
	static void CopyElements(
		_Out_writes_(nElements) ::ATL::CAutoVectorPtr< T >* pDest,
		_In_reads_(nElements) ::ATL::CAutoVectorPtr< T >* pSrc,
		_In_ size_t nElements)
	{
		for( size_t iElement = 0; iElement < nElements; iElement++ )
		{
			pDest[iElement] = pSrc[iElement];
		}
	}
};

template< typename T, class Allocator = ATL::CCRTAllocator >
class CHeapPtrElementTraits :
	public CDefaultElementTraits< ATL::CHeapPtr< T, Allocator > >
{
public:
	typedef ATL::CHeapPtr< T, Allocator >& INARGTYPE;
	typedef T*& OUTARGTYPE;
};

template < typename T >
class CDefaultCharTraits
{
};

template <>
class CDefaultCharTraits<char>
{
public:
	static char CharToUpper(_In_ char x)
	{
		return (char)toupper(x);
	}

	static char CharToLower(_In_ char x)
	{
		return (char)tolower(x);
	}
};

template <>
class CDefaultCharTraits<wchar_t>
{
public:
	static wchar_t CharToUpper(_In_ wchar_t x)
	{
		return (wchar_t)towupper(x);
	}

	static wchar_t CharToLower(_In_ wchar_t x)
	{
		return (wchar_t)towlower(x);
	}
};

template< typename T, class CharTraits = CDefaultCharTraits<typename T::XCHAR> >
class CStringElementTraitsI :
	public CElementTraitsBase< T >
{
public:
	typedef typename T::PCXSTR INARGTYPE;
	typedef T& OUTARGTYPE;

	static ULONG Hash(_In_ INARGTYPE str)
	{
		ULONG nHash = 0;

		const typename T::XCHAR* pch = str;

		ATLENSURE( pch != NULL );

		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+CharTraits::CharToUpper(*pch);
			pch++;
		}

		return( nHash );
	}

	static bool CompareElements(
		_In_ INARGTYPE str1,
		_In_ INARGTYPE str2) throw()
	{
		return( T::StrTraits::StringCompareIgnore( str1, str2 ) == 0 );
	}

	static int CompareElementsOrdered(
		_In_ INARGTYPE str1,
		_In_ INARGTYPE str2) throw()
	{
		return( T::StrTraits::StringCompareIgnore( str1, str2 ) );
	}
};

template< typename T >
class CStringRefElementTraits :
	public CElementTraitsBase< T >
{
public:
	using typename CElementTraitsBase<T>::INARGTYPE;

	static ULONG Hash(_In_ INARGTYPE str)
	{
		ULONG nHash = 0;

		const typename T::XCHAR* pch = str;

		ATLENSURE( pch != NULL );

		while( *pch != 0 )
		{
			nHash = (nHash<<5)+nHash+(*pch);
			pch++;
		}

		return( nHash );
	}

	static bool CompareElements(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2) throw()
	{
		return( element1 == element2 );
	}

	static int CompareElementsOrdered(
		_In_ INARGTYPE str1,
		_In_ INARGTYPE str2) throw()
	{
		return( str1.Compare( str2 ) );
	}
};

template< typename T >
class CPrimitiveElementTraits :
	public CDefaultElementTraits< T >
{
public:
	typedef T INARGTYPE;
	typedef T& OUTARGTYPE;
};

#define _DECLARE_PRIMITIVE_TRAITS( T ) \
	template<> \
	class CElementTraits< T > : \
		public CPrimitiveElementTraits< T > \
	{ \
	};

_DECLARE_PRIMITIVE_TRAITS( unsigned char )
_DECLARE_PRIMITIVE_TRAITS( unsigned short )
_DECLARE_PRIMITIVE_TRAITS( unsigned int )
_DECLARE_PRIMITIVE_TRAITS( unsigned long )
_DECLARE_PRIMITIVE_TRAITS( unsigned __int64 )
_DECLARE_PRIMITIVE_TRAITS( signed char )
_DECLARE_PRIMITIVE_TRAITS( char )
_DECLARE_PRIMITIVE_TRAITS( short )
_DECLARE_PRIMITIVE_TRAITS( int )
_DECLARE_PRIMITIVE_TRAITS( long )
_DECLARE_PRIMITIVE_TRAITS( __int64 )
_DECLARE_PRIMITIVE_TRAITS( float )
_DECLARE_PRIMITIVE_TRAITS( double )
_DECLARE_PRIMITIVE_TRAITS( bool )
#ifdef _NATIVE_WCHAR_T_DEFINED
_DECLARE_PRIMITIVE_TRAITS( wchar_t )
#endif
_DECLARE_PRIMITIVE_TRAITS( void* )

template< typename E, class ETraits = CElementTraits< E > >
class CAtlArray
{
public:
	typedef typename ETraits::INARGTYPE INARGTYPE;
	typedef typename ETraits::OUTARGTYPE OUTARGTYPE;

public:
	CAtlArray() throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();
	bool SetCount(_In_ size_t nNewSize, _In_ int nGrowBy = -1);

	void FreeExtra() throw();
	void RemoveAll() throw();

	const E& GetAt(_In_ size_t iElement) const;
	void SetAt(
		_In_ size_t iElement,
		/* _In_ */ INARGTYPE element);
	E& GetAt(_In_ size_t iElement);

	const E* GetData() const throw();
	E* GetData() throw();

	void SetAtGrow(
		_In_ size_t iElement,
		/* _In_ */ INARGTYPE element);
	// Add an empty element to the end of the array
	size_t Add();
	// Add an element to the end of the array
	size_t Add(/* _In_ */ INARGTYPE element);
	size_t Append(_In_ const CAtlArray< E, ETraits >& aSrc);
	void Copy(_In_ const CAtlArray< E, ETraits >& aSrc);

	const E& operator[](_In_ size_t iElement) const;
	E& operator[](_In_ size_t iElement);

	void InsertAt(
		_In_ size_t iElement,
		/* _In_ */ INARGTYPE element,
		_In_ size_t nCount = 1);
	void InsertArrayAt(
		_In_ size_t iStart,
		_In_ const CAtlArray< E, ETraits >* paNew);
	void RemoveAt(
		_In_ size_t iElement,
		_In_ size_t nCount = 1);

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

private:
	bool GrowBuffer(_In_ size_t nNewSize);

// Implementation
private:
	E* m_pData;
	size_t m_nSize;
	size_t m_nMaxSize;
	int m_nGrowBy;

private:
	static void CallConstructors(
		_Inout_updates_(nElements) E* pElements,
		_In_ size_t nElements);
	static void CallDestructors(
		_Inout_updates_to_(nElements, 0) E* pElements,
		_In_ size_t nElements) throw();

public:
	~CAtlArray() throw();

private:
	// Private to prevent use
	CAtlArray(_In_ const CAtlArray&) throw();
	CAtlArray& operator=(_In_ const CAtlArray&) throw();
};

template< class I, const IID* piid = &__uuidof( I ) >
class CInterfaceArray :
	public CAtlArray< ATL::CComQIPtr< I, piid >, CComQIPtrElementTraits< I, piid > >
{
public:
	CInterfaceArray() throw()
	{
	}

private:
	// Private to prevent use
	CInterfaceArray(_In_ const CInterfaceArray&) throw();
	CInterfaceArray& operator=(_In_ const CInterfaceArray&) throw();
};

template< typename E >
class CAutoPtrArray :
	public CAtlArray< ATL::CAutoPtr< E >, CAutoPtrElementTraits< E > >
{
public:
	CAutoPtrArray() throw()
	{
	}

private:
	// Private to prevent use
	CAutoPtrArray(_In_ const CAutoPtrArray&) throw();
	CAutoPtrArray& operator=(_In_ const CAutoPtrArray&) throw();
};

template< typename E, class Allocator = ATL::CCRTAllocator >
class CHeapPtrArray :
	public CAtlArray< ATL::CHeapPtr< E, Allocator >, CHeapPtrElementTraits< E, Allocator > >
{
public:
	CHeapPtrArray() throw()
	{
	}

private:
	// Private to prevent use
	CHeapPtrArray(_In_ const CHeapPtrArray&) throw();
	CHeapPtrArray& operator=(_In_ const CHeapPtrArray&) throw();
};

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::GetCount() const throw()
{
	return( m_nSize );
}

template< typename E, class ETraits >
inline bool CAtlArray< E, ETraits >::IsEmpty() const throw()
{
	return( m_nSize == 0 );
}

template< typename E, class ETraits >
inline void CAtlArray< E, ETraits >::RemoveAll() throw()
{
	SetCount( 0, -1 );
}

template< typename E, class ETraits >
inline const E& CAtlArray< E, ETraits >::GetAt(_In_ size_t iElement) const
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline void CAtlArray< E, ETraits >::SetAt(
	_In_ size_t iElement,
	/* _In_ */ INARGTYPE element)
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	m_pData[iElement] = element;
}

template< typename E, class ETraits >
inline E& CAtlArray< E, ETraits >::GetAt(_In_ size_t iElement)
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline const E* CAtlArray< E, ETraits >::GetData() const throw()
{
	return( m_pData );
}

template< typename E, class ETraits >
inline E* CAtlArray< E, ETraits >::GetData() throw()
{
	return( m_pData );
}

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::Add()
{
	size_t iElement;

	iElement = m_nSize;
	bool bSuccess = SetCount(AtlAddThrow<size_t>(m_nSize, 1));
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	return( iElement );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
inline size_t CAtlArray< E, ETraits >::Add(/* _In_ */ INARGTYPE element)
{
	size_t iElement;

	iElement = m_nSize;
	if( iElement >= m_nMaxSize )
	{
		bool bSuccess = GrowBuffer( iElement+1 );
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}
	::new( m_pData+iElement ) E( element );
	m_nSize++;

	return( iElement );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
inline const E& CAtlArray< E, ETraits >::operator[](_In_ size_t iElement) const
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline E& CAtlArray< E, ETraits >::operator[](_In_ size_t iElement)
{
	ATLASSERT( iElement < m_nSize );
	if(iElement >= m_nSize)
		AtlThrow(E_INVALIDARG);

	return( m_pData[iElement] );
}

template< typename E, class ETraits >
CAtlArray< E, ETraits >::CAtlArray()  throw():
	m_pData( NULL ),
	m_nSize( 0 ),
	m_nMaxSize( 0 ),
	m_nGrowBy( 0 )
{
}

template< typename E, class ETraits >
CAtlArray< E, ETraits >::~CAtlArray() throw()
{
	if( m_pData != NULL )
	{
		CallDestructors( m_pData, m_nSize );
		free( m_pData );
	}
}

template< typename E, class ETraits >
bool CAtlArray< E, ETraits >::GrowBuffer(_In_ size_t nNewSize)
{
	if( nNewSize > m_nMaxSize )
	{
		if( m_pData == NULL )
		{
			size_t nAllocSize =  size_t( m_nGrowBy ) > nNewSize ? size_t( m_nGrowBy ) : nNewSize ;
			m_pData = static_cast< E* >( calloc( nAllocSize,sizeof( E ) ) );
			if( m_pData == NULL )
			{
				return( false );
			}
			m_nMaxSize = nAllocSize;
		}
		else
		{
			// otherwise, grow array
			size_t nGrowBy = m_nGrowBy;
			if( nGrowBy == 0 )
			{
				// use 1.5 ratio for growing buffers
				nGrowBy = m_nMaxSize / 2;

				if ((nNewSize - m_nMaxSize) > nGrowBy)
				{
					nGrowBy = nNewSize - m_nMaxSize;
				}
			}
			size_t nNewMax;
			if( nNewSize < (m_nMaxSize+nGrowBy) )
				nNewMax = m_nMaxSize+nGrowBy;  // granularity
			else
				nNewMax = nNewSize;  // no slush

			ATLASSERT( nNewMax >= m_nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
			ATLASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
			E* pNewData = static_cast< E* >( calloc( nNewMax,sizeof( E ) ) );
			if( pNewData == NULL )
			{
				return false;
			}

			// copy new data from old
			ETraits::RelocateElements( pNewData, m_pData, m_nSize );

			// get rid of old stuff (note: no destructors called)
			free( m_pData );
			m_pData = pNewData;
			m_nMaxSize = nNewMax;
		}
	}

	return true;
}

template< typename E, class ETraits >
bool CAtlArray< E, ETraits >::SetCount(
	_In_ size_t nNewSize,
	_In_ int nGrowBy)
{
	ATLASSERT_VALID(this);

	if( nGrowBy != -1 )
	{
		m_nGrowBy = nGrowBy;  // set new size
	}

	if( nNewSize == 0 )
	{
		// shrink to nothing
		if( m_pData != NULL )
		{
			CallDestructors( m_pData, m_nSize );
			free( m_pData );
			m_pData = NULL;
		}
		m_nSize = 0;
		m_nMaxSize = 0;
	}
	else if( nNewSize <= m_nMaxSize )
	{
		// it fits
		if( nNewSize > m_nSize )
		{
			// initialize the new elements
			CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );
		}
		else if( m_nSize > nNewSize )
		{
			// destroy the old elements
			CallDestructors( m_pData+nNewSize, m_nSize-nNewSize );
		}
		m_nSize = nNewSize;
	}
	else
	{
		bool bSuccess;

		bSuccess = GrowBuffer( nNewSize );
		if( !bSuccess )
		{
			return( false );
		}

		// construct new elements
		ATLASSERT( nNewSize > m_nSize );
		CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );

		m_nSize = nNewSize;
	}

	return true;
}

template< typename E, class ETraits >
size_t CAtlArray< E, ETraits >::Append(_In_ const CAtlArray< E, ETraits >& aSrc)
{
	ATLASSERT_VALID(this);
	ATLASSERT( this != &aSrc );   // cannot append to itself

	size_t nOldSize = m_nSize;
	bool bSuccess = SetCount(AtlAddThrow<size_t>(m_nSize, aSrc.m_nSize));
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	ETraits::CopyElements( m_pData+nOldSize, aSrc.m_pData, aSrc.m_nSize );

	return( nOldSize );
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::Copy(_In_ const CAtlArray< E, ETraits >& aSrc)
{
	ATLASSERT_VALID(this);
	ATLASSERT( this != &aSrc );   // cannot append to itself

	bool bSuccess=SetCount( aSrc.m_nSize );
	if( !bSuccess )
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	ETraits::CopyElements( m_pData, aSrc.m_pData, aSrc.m_nSize );
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::FreeExtra() throw()
{
	ATLASSERT_VALID(this);

	if( m_nSize != m_nMaxSize )
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ATLASSUME( m_nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
		E* pNewData = NULL;
		if( m_nSize != 0 )
		{
			pNewData = (E*)calloc( m_nSize,sizeof( E ) );
			if( pNewData == NULL )
			{
				return;
			}

			// copy new data from old
			ETraits::RelocateElements( pNewData, m_pData, m_nSize );
		}

		// get rid of old stuff (note: no destructors called)
		free( m_pData );
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::SetAtGrow(
	_In_ size_t iElement,
	/* _In_ */ INARGTYPE element)
{
	ATLASSERT_VALID(this);
	size_t nOldSize;

	nOldSize = m_nSize;
	if( iElement >= m_nSize )
	{
		bool bSuccess = SetCount(AtlAddThrow<size_t>(iElement, 1), -1 );
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}

	_ATLTRY
	{
		m_pData[iElement] = element;
	}
	_ATLCATCHALL()
	{
		if( m_nSize != nOldSize )
		{
			SetCount( nOldSize, -1 );
		}
		_ATLRETHROW;
	}
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::InsertAt(
	_In_ size_t iElement,
	/* _In_ */ INARGTYPE element,
	_In_ size_t nElements /*=1*/)
{
	ATLASSERT_VALID(this);
	ATLASSERT( nElements > 0 );     // zero size not allowed

	if( iElement >= m_nSize )
	{
		// adding after the end of the array
		bool bSuccess = SetCount(AtlAddThrow<size_t>(iElement, nElements), -1 );   // grow so nIndex is valid
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}
	else
	{
		// inserting in the middle of the array
		size_t nOldSize = m_nSize;
		bool bSuccess = SetCount(AtlAddThrow<size_t>(m_nSize, nElements), -1 );  // grow it to new size
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
		// destroy initial data before copying over it
		CallDestructors( m_pData+nOldSize, nElements );
		// shift old data up to fill gap
		ETraits::RelocateElements( m_pData+(iElement+nElements), m_pData+iElement,
			nOldSize-iElement );

		_ATLTRY
		{
			// re-init slots we copied from
			CallConstructors( m_pData+iElement, nElements );
		}
		_ATLCATCHALL()
		{
			ETraits::RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
				nOldSize-iElement );
			SetCount( nOldSize, -1 );
			_ATLRETHROW;
		}
	}

	// insert new value in the gap
	ATLASSERT( (iElement+nElements) <= m_nSize );
	for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
	{
		m_pData[iNewElement] = element;
	}
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::RemoveAt(
	_In_ size_t iElement,
	_In_ size_t nElements)
{
	ATLASSERT_VALID(this);
	ATLASSERT( (iElement+nElements) <= m_nSize );

	size_t newCount = iElement+nElements;
	if ((newCount < iElement) || (newCount < nElements) || (newCount > m_nSize))
		AtlThrow(E_INVALIDARG);

	// just remove a range
	size_t nMoveCount = m_nSize-(newCount);
	CallDestructors( m_pData+iElement, nElements );
	if( nMoveCount > 0 )
	{
		ETraits::RelocateElements( m_pData+iElement, m_pData+(newCount),
			nMoveCount );
	}
	m_nSize -= nElements;
}

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::InsertArrayAt(
	_In_ size_t iStartElement,
	_In_ const CAtlArray< E, ETraits >* paNew)
{
	ATLASSERT_VALID( this );
	ATLENSURE( paNew != NULL );
	ATLASSERT_VALID( paNew );

	if( paNew->GetCount() > 0 )
	{
		InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
		for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
		{
			SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
		}
	}
}

#ifdef _DEBUG
template< typename E, class ETraits >
void CAtlArray< E, ETraits >::AssertValid() const
{
	if( m_pData == NULL )
	{
		ATLASSUME( m_nSize == 0 );
		ATLASSUME( m_nMaxSize == 0 );
	}
	else
	{
		ATLASSUME( m_nSize <= m_nMaxSize );
		ATLASSERT( AtlIsValidAddress( m_pData, m_nMaxSize * sizeof( E ) ) );
	}
}
#endif

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void CAtlArray< E, ETraits >::CallConstructors(
	_Inout_updates_(nElements) E* pElements,
	_In_ size_t nElements)
{
	size_t iElement = 0;

	_ATLTRY
	{
		for( iElement = 0; iElement < nElements; iElement++ )
		{
			::new( pElements+iElement ) E;
		}
	}
	_ATLCATCHALL()
	{
		while( iElement > 0 )
		{
			iElement--;
			pElements[iElement].~E();
		}

		_ATLRETHROW;
	}
}

#pragma pop_macro("new")

ATLPREFAST_SUPPRESS(6385)
template< typename E, class ETraits >
void CAtlArray< E, ETraits >::CallDestructors(
	_Inout_updates_to_(nElements, 0) E* pElements,
	_In_ size_t nElements) throw()
{
	(pElements);

	for( size_t iElement = 0; iElement < nElements; iElement++ )
	{
		pElements[iElement].~E();
	}
}
ATLPREFAST_UNSUPPRESS()

template< typename E, class ETraits = CElementTraits< E > >
class CAtlList
{
public:
	typedef typename ETraits::INARGTYPE INARGTYPE;

private:
	class CNode :
		public __POSITION
	{
	public:
		CNode()
		{
		}
		CNode(/* _In_ */ INARGTYPE element) :
			m_element( element )
		{
		}
		~CNode() throw()
		{
		}

	public:
		CNode* m_pNext;
		CNode* m_pPrev;
		E m_element;

	private:
		CNode(_In_ const CNode&) throw();
	};

public:
	CAtlList(_In_ UINT nBlockSize = 10) throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();

	E& GetHead();
	const E& GetHead() const;
	E& GetTail();
	const E& GetTail() const;

	E RemoveHead();
	E RemoveTail();
	void RemoveHeadNoReturn() throw();
	void RemoveTailNoReturn() throw();

	POSITION AddHead();
	POSITION AddHead(/* _In_ */ INARGTYPE element);
	void AddHeadList(_In_ const CAtlList< E, ETraits >* plNew);
	POSITION AddTail();
	POSITION AddTail(/* _In_ */ INARGTYPE element);
	void AddTailList(_In_ const CAtlList< E, ETraits >* plNew);

	void RemoveAll() throw();

	POSITION GetHeadPosition() const throw();
	POSITION GetTailPosition() const throw();
	E& GetNext(_Inout_ POSITION& pos);
	const E& GetNext(_Inout_ POSITION& pos) const;
	E& GetPrev(_Inout_ POSITION& pos);
	const E& GetPrev(_Inout_ POSITION& pos) const throw();

	E& GetAt(_In_ POSITION pos);
	const E& GetAt(_In_ POSITION pos) const;
	void SetAt(
		_In_ POSITION pos,
		/* _In_ */ INARGTYPE element);
	void RemoveAt(_In_ POSITION pos) throw();

	POSITION InsertBefore(
		_In_ POSITION pos,
		/* _In_ */ INARGTYPE element);
	POSITION InsertAfter(
		_In_ POSITION pos,
		/* _In_ */ INARGTYPE element);

	POSITION Find(
		/* _In_ */ INARGTYPE element,
		_In_opt_ POSITION posStartAfter = NULL) const throw();
	POSITION FindIndex(_In_ size_t iElement) const throw();

	void MoveToHead(_In_ POSITION pos);
	void MoveToTail(_In_ POSITION pos);
	void SwapElements(
		_In_ POSITION pos1,
		_In_ POSITION pos2) throw();

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

// Implementation
private:
	CNode* m_pHead;
	CNode* m_pTail;
	size_t m_nElements;
	CAtlPlex* m_pBlocks;
	CNode* m_pFree;
	UINT m_nBlockSize;

private:
	void GetFreeNode();
	CNode* NewNode(
		_In_opt_ CNode* pPrev,
		_In_opt_ CNode* pNext);
	CNode* NewNode(
		/* _In_ */ INARGTYPE element,
		_In_opt_ CNode* pPrev,
		_In_opt_ CNode* pNext);
	void FreeNode(_Inout_ CNode* pNode) throw();

public:
	~CAtlList() throw();

private:
	// Private to prevent use
	CAtlList(_In_ const CAtlList&) throw();
	CAtlList& operator=(_In_ const CAtlList&) throw();
};

template< class I, const IID* piid = &__uuidof( I ) >
class CInterfaceList :
	public CAtlList< ATL::CComQIPtr< I, piid >, CComQIPtrElementTraits< I, piid > >
{
public:
	CInterfaceList(_In_ UINT nBlockSize = 10) throw() :
		CAtlList< ATL::CComQIPtr< I, piid >, CComQIPtrElementTraits< I, piid > >( nBlockSize )
	{
	}

private:
	// Private to prevent use
	CInterfaceList(_In_ const CInterfaceList&) throw();
	CInterfaceList& operator=(_In_ const CInterfaceList&) throw();
};

template< typename E >
class CAutoPtrList :
	public CAtlList< ATL::CAutoPtr< E >, CAutoPtrElementTraits< E > >
{
public:
	CAutoPtrList(_In_ UINT nBlockSize = 10) throw() :
		CAtlList< ATL::CAutoPtr< E >, CAutoPtrElementTraits< E > >( nBlockSize )
	{
	}

private:
	// Private to prevent use
	CAutoPtrList(_In_ const CAutoPtrList&) throw();
	CAutoPtrList& operator=(_In_ const CAutoPtrList&) throw();
};

template< typename E, class Allocator = ATL::CCRTAllocator >
class CHeapPtrList :
	public CAtlList< ATL::CHeapPtr< E, Allocator >, CHeapPtrElementTraits< E, Allocator > >
{
public:
	CHeapPtrList(_In_ UINT nBlockSize = 10) throw() :
		CAtlList< ATL::CHeapPtr< E, Allocator >, CHeapPtrElementTraits< E, Allocator > >( nBlockSize )
	{
	}

private:
	// Private to prevent use
	CHeapPtrList(_In_ const CHeapPtrList&) throw();
	CHeapPtrList& operator=(_In_ const CHeapPtrList&) throw();
};

template< typename E, class ETraits >
inline size_t CAtlList< E, ETraits >::GetCount() const throw()
{
	return( m_nElements );
}

template< typename E, class ETraits >
inline bool CAtlList< E, ETraits >::IsEmpty() const throw()
{
	return( m_nElements == 0 );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetHead()
{
	ATLENSURE( m_pHead != NULL );
	return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetHead() const
{
	ATLENSURE( m_pHead != NULL );
	return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetTail()
{
	ATLENSURE( m_pTail != NULL );
	return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetTail() const
{
	ATLENSURE( m_pTail != NULL );
	return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline POSITION CAtlList< E, ETraits >::GetHeadPosition() const throw()
{
	return( POSITION( m_pHead ) );
}

template< typename E, class ETraits >
inline POSITION CAtlList< E, ETraits >::GetTailPosition() const throw()
{
	return( POSITION( m_pTail ) );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetNext(_Inout_ POSITION& pos)
{
	CNode* pNode;

	ATLENSURE( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pNext );

	return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetNext(_Inout_ POSITION& pos) const
{
	CNode* pNode;

	ATLENSURE( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pNext );

	return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetPrev(_Inout_ POSITION& pos)
{
	CNode* pNode;

	ATLENSURE( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pPrev );

	return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetPrev(_Inout_ POSITION& pos) const throw()
{
	CNode* pNode;

	ATLASSUME( pos != NULL );
	pNode = (CNode*)pos;
	pos = POSITION( pNode->m_pPrev );

	return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& CAtlList< E, ETraits >::GetAt(_In_ POSITION pos)
{
	ATLENSURE( pos != NULL );
	CNode* pNode = (CNode*)pos;
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& CAtlList< E, ETraits >::GetAt(_In_ POSITION pos) const
{
	ATLENSURE( pos != NULL );
	CNode* pNode = (CNode*)pos;
	return( pNode->m_element );
}

template< typename E, class ETraits >
inline void CAtlList< E, ETraits >::SetAt(
	_In_ POSITION pos,
	/* _In_ */ INARGTYPE element)
{
	ATLENSURE( pos != NULL );
	CNode* pNode = (CNode*)pos;
	pNode->m_element = element;
}

template< typename E, class ETraits >
CAtlList< E, ETraits >::CAtlList(_In_ UINT nBlockSize) throw() :
	m_nElements( 0 ),
	m_pHead( NULL ),
	m_pTail( NULL ),
	m_nBlockSize( nBlockSize ),
	m_pBlocks( NULL ),
	m_pFree( NULL )
{
	ATLASSERT( nBlockSize > 0 );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveAll() throw()
{
	while( m_nElements > 0 )
	{
		CNode* pKill = m_pHead;
		ATLENSURE( pKill != NULL );

		m_pHead = m_pHead->m_pNext;
		FreeNode( pKill );
	}

	ATLASSUME( m_nElements == 0 );
	m_pHead = NULL;
	m_pTail = NULL;
	m_pFree = NULL;

	if( m_pBlocks != NULL )
	{
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}
}

template< typename E, class ETraits >
CAtlList< E, ETraits >::~CAtlList() throw()
{
	RemoveAll();
	ATLASSUME( m_nElements == 0 );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void CAtlList< E, ETraits >::GetFreeNode()
{
	if( m_pFree == NULL )
	{
		CAtlPlex* pPlex;
		CNode* pNode;

		pPlex = CAtlPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
		if( pPlex == NULL )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
		pNode = (CNode*)pPlex->data();
		pNode += m_nBlockSize-1;
		for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->m_pNext = m_pFree;
			m_pFree = pNode;
			pNode--;
		}
	}
	ATLASSUME( m_pFree != NULL );
}

template< typename E, class ETraits >
typename CAtlList< E, ETraits >::CNode* CAtlList< E, ETraits >::NewNode(
	_In_opt_ CNode* pPrev,
	_In_opt_ CNode* pNext )
{
	GetFreeNode();

	CNode* pNewNode = m_pFree;
	CNode* pNextFree = m_pFree->m_pNext;

	::new( pNewNode ) CNode;

	m_pFree = pNextFree;
	pNewNode->m_pPrev = pPrev;
	pNewNode->m_pNext = pNext;
	m_nElements++;
	ATLASSUME( m_nElements > 0 );

	return( pNewNode );
}

template< typename E, class ETraits >
typename CAtlList< E, ETraits >::CNode* CAtlList< E, ETraits >::NewNode(
	/* _In_ */ INARGTYPE element,
	_In_opt_ CNode* pPrev,
	_In_opt_ CNode* pNext)
{
	GetFreeNode();

	CNode* pNewNode = m_pFree;
	CNode* pNextFree = m_pFree->m_pNext;

	::new( pNewNode ) CNode( element );

	m_pFree = pNextFree;
	pNewNode->m_pPrev = pPrev;
	pNewNode->m_pNext = pNext;
	m_nElements++;
	ATLASSUME( m_nElements > 0 );

	return( pNewNode );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void CAtlList< E, ETraits >::FreeNode(_Inout_ CNode* pNode) throw()
{
	pNode->~CNode();
	pNode->m_pNext = m_pFree;
	m_pFree = pNode;
	ATLASSUME( m_nElements > 0 );
	m_nElements--;
	if( m_nElements == 0 )
	{
		RemoveAll();
	}
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddHead()
{
	CNode* pNode = NewNode( NULL, m_pHead );
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = pNode;
	}
	else
	{
		m_pTail = pNode;
	}
	m_pHead = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddHead(/* _In_ */ INARGTYPE element)
{
	CNode* pNode;

	pNode = NewNode( element, NULL, m_pHead );

	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = pNode;
	}
	else
	{
		m_pTail = pNode;
	}
	m_pHead = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddTail()
{
	CNode* pNode = NewNode( m_pTail, NULL );
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = pNode;
	}
	else
	{
		m_pHead = pNode;
	}
	m_pTail = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::AddTail(/* _In_ */ INARGTYPE element)
{
	CNode* pNode;

	pNode = NewNode( element, m_pTail, NULL );

	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = pNode;
	}
	else
	{
		m_pHead = pNode;
	}
	m_pTail = pNode;

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::AddHeadList(_In_ const CAtlList< E, ETraits >* plNew)
{
	ATLENSURE( plNew != NULL );

	POSITION pos = plNew->GetTailPosition();
	while( pos != NULL )
	{
		INARGTYPE element = plNew->GetPrev( pos );
		AddHead( element );
	}
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::AddTailList(_In_ const CAtlList< E, ETraits >* plNew)
{
	ATLENSURE( plNew != NULL );

	POSITION pos = plNew->GetHeadPosition();
	while( pos != NULL )
	{
		INARGTYPE element = plNew->GetNext( pos );
		AddTail( element );
	}
}

template< typename E, class ETraits >
E CAtlList< E, ETraits >::RemoveHead()
{
	ATLENSURE( m_pHead != NULL );

	CNode* pNode = m_pHead;
	E element( pNode->m_element );

	m_pHead = pNode->m_pNext;
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = NULL;
	}
	else
	{
		m_pTail = NULL;
	}
	FreeNode( pNode );

	return( element );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveHeadNoReturn() throw()
{
	ATLENSURE( m_pHead != NULL );

	CNode* pNode = m_pHead;

	m_pHead = pNode->m_pNext;
	if( m_pHead != NULL )
	{
		m_pHead->m_pPrev = NULL;
	}
	else
	{
		m_pTail = NULL;
	}
	FreeNode( pNode );
}

template< typename E, class ETraits >
E CAtlList< E, ETraits >::RemoveTail()
{
	ATLENSURE( m_pTail != NULL );

	CNode* pNode = m_pTail;

	E element( pNode->m_element );

	m_pTail = pNode->m_pPrev;
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = NULL;
	}
	else
	{
		m_pHead = NULL;
	}
	FreeNode( pNode );

	return( element );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveTailNoReturn() throw()
{
	ATLENSURE( m_pTail != NULL );

	CNode* pNode = m_pTail;

	m_pTail = pNode->m_pPrev;
	if( m_pTail != NULL )
	{
		m_pTail->m_pNext = NULL;
	}
	else
	{
		m_pHead = NULL;
	}
	FreeNode( pNode );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::InsertBefore(
	_In_ POSITION pos,
	/* _In_ */ INARGTYPE element)
{
	ATLASSERT_VALID(this);

	if( pos == NULL )
		return AddHead( element ); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*)pos;
	CNode* pNewNode = NewNode( element, pOldNode->m_pPrev, pOldNode );

	if( pOldNode->m_pPrev != NULL )
	{
		ATLASSERT(AtlIsValidAddress(pOldNode->m_pPrev, sizeof(CNode)));
		pOldNode->m_pPrev->m_pNext = pNewNode;
	}
	else
	{
		ATLASSERT( pOldNode == m_pHead );
		m_pHead = pNewNode;
	}
	pOldNode->m_pPrev = pNewNode;

	return( POSITION( pNewNode ) );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::InsertAfter(
	_In_ POSITION pos,
	/* _In_ */ INARGTYPE element)
{
	ATLASSERT_VALID(this);

	if( pos == NULL )
		return AddTail( element ); // insert after nothing -> tail of the list

	// Insert it after position
	CNode* pOldNode = (CNode*)pos;
	CNode* pNewNode = NewNode( element, pOldNode, pOldNode->m_pNext );

	if( pOldNode->m_pNext != NULL )
	{
		ATLASSERT(AtlIsValidAddress(pOldNode->m_pNext, sizeof(CNode)));
		pOldNode->m_pNext->m_pPrev = pNewNode;
	}
	else
	{
		ATLASSERT( pOldNode == m_pTail );
		m_pTail = pNewNode;
	}
	pOldNode->m_pNext = pNewNode;

	return( POSITION( pNewNode ) );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::RemoveAt(_In_ POSITION pos) throw()
{
	ATLASSERT_VALID(this);
	ATLENSURE( pos != NULL );

	CNode* pOldNode = (CNode*)pos;

	// remove pOldNode from list
	if( pOldNode == m_pHead )
	{
		m_pHead = pOldNode->m_pNext;
	}
	else
	{
		ATLASSERT( AtlIsValidAddress( pOldNode->m_pPrev, sizeof(CNode) ));
		pOldNode->m_pPrev->m_pNext = pOldNode->m_pNext;
	}
	if( pOldNode == m_pTail )
	{
		m_pTail = pOldNode->m_pPrev;
	}
	else
	{
		ATLASSERT( AtlIsValidAddress( pOldNode->m_pNext, sizeof(CNode) ));
		pOldNode->m_pNext->m_pPrev = pOldNode->m_pPrev;
	}
	FreeNode( pOldNode );
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::FindIndex(_In_ size_t iElement) const throw()
{
	ATLASSERT_VALID(this);

	if( iElement >= m_nElements )
		return NULL;  // went too far

	if(m_pHead == NULL)
		return NULL;

	CNode* pNode = m_pHead;
	for( size_t iSearch = 0; iSearch < iElement; iSearch++ )
	{
		pNode = pNode->m_pNext;
	}

	return( POSITION( pNode ) );
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::MoveToHead(_In_ POSITION pos)
{
	ATLENSURE( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );

	if( pNode == m_pHead )
	{
		// Already at the head
		return;
	}

	if( pNode->m_pNext == NULL )
	{
		ATLASSERT( pNode == m_pTail );
		m_pTail = pNode->m_pPrev;
	}
	else
	{
		pNode->m_pNext->m_pPrev = pNode->m_pPrev;
	}

	ATLASSUME( pNode->m_pPrev != NULL );  // This node can't be the head, since we already checked that case
	pNode->m_pPrev->m_pNext = pNode->m_pNext;

	m_pHead->m_pPrev = pNode;
	pNode->m_pNext = m_pHead;
	pNode->m_pPrev = NULL;
	m_pHead = pNode;
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::MoveToTail(_In_ POSITION pos)
{
	ATLENSURE( pos != NULL );
	CNode* pNode = static_cast< CNode* >( pos );

	if( pNode == m_pTail )
	{
		// Already at the tail
		return;
	}

	if( pNode->m_pPrev == NULL )
	{
		ATLENSURE( pNode == m_pHead );
		m_pHead = pNode->m_pNext;
	}
	else
	{
		pNode->m_pPrev->m_pNext = pNode->m_pNext;
	}

	pNode->m_pNext->m_pPrev = pNode->m_pPrev;

	m_pTail->m_pNext = pNode;
	pNode->m_pPrev = m_pTail;
	pNode->m_pNext = NULL;
	m_pTail = pNode;
}

template< typename E, class ETraits >
void CAtlList< E, ETraits >::SwapElements(
	_In_ POSITION pos1,
	_In_ POSITION pos2) throw()
{
	ATLASSUME( pos1 != NULL );
	ATLASSUME( pos2 != NULL );

	if( pos1 == pos2 )
	{
		// Nothing to do
		return;
	}

	CNode* pNode1 = static_cast< CNode* >( pos1 );
	CNode* pNode2 = static_cast< CNode* >( pos2 );
	if( pNode2->m_pNext == pNode1 )
	{
		// Swap pNode2 and pNode1 so that the next case works
		CNode* pNodeTemp = pNode1;
		pNode1 = pNode2;
		pNode2 = pNodeTemp;
	}
	if( pNode1->m_pNext == pNode2 )
	{
		// Node1 and Node2 are adjacent
		pNode2->m_pPrev = pNode1->m_pPrev;
		if( pNode1->m_pPrev != NULL )
		{
			pNode1->m_pPrev->m_pNext = pNode2;
		}
		else
		{
			ATLASSUME( m_pHead == pNode1 );
			m_pHead = pNode2;
		}
		pNode1->m_pNext = pNode2->m_pNext;
		if( pNode2->m_pNext != NULL )
		{
			pNode2->m_pNext->m_pPrev = pNode1;
		}
		else
		{
			ATLASSUME( m_pTail == pNode2 );
			m_pTail = pNode1;
		}
		pNode2->m_pNext = pNode1;
		pNode1->m_pPrev = pNode2;
	}
	else
	{
		// The two nodes are not adjacent
		CNode* pNodeTemp;

		pNodeTemp = pNode1->m_pPrev;
		pNode1->m_pPrev = pNode2->m_pPrev;
		pNode2->m_pPrev = pNodeTemp;

		pNodeTemp = pNode1->m_pNext;
		pNode1->m_pNext = pNode2->m_pNext;
		pNode2->m_pNext = pNodeTemp;

		if( pNode1->m_pNext != NULL )
		{
			pNode1->m_pNext->m_pPrev = pNode1;
		}
		else
		{
			ATLASSUME( m_pTail == pNode2 );
			m_pTail = pNode1;
		}
		if( pNode1->m_pPrev != NULL )
		{
			pNode1->m_pPrev->m_pNext = pNode1;
		}
		else
		{
			ATLASSUME( m_pHead == pNode2 );
			m_pHead = pNode1;
		}
		if( pNode2->m_pNext != NULL )
		{
			pNode2->m_pNext->m_pPrev = pNode2;
		}
		else
		{
			ATLASSUME( m_pTail == pNode1 );
			m_pTail = pNode2;
		}
		if( pNode2->m_pPrev != NULL )
		{
			pNode2->m_pPrev->m_pNext = pNode2;
		}
		else
		{
			ATLASSUME( m_pHead == pNode1 );
			m_pHead = pNode2;
		}
	}
}

template< typename E, class ETraits >
POSITION CAtlList< E, ETraits >::Find(
	/* _In_ */ INARGTYPE element,
	_In_opt_ POSITION posStartAfter) const throw()
{
	ATLASSERT_VALID(this);

	CNode* pNode = (CNode*)posStartAfter;
	if( pNode == NULL )
	{
		pNode = m_pHead;  // start at head
	}
	else
	{
		pNode = SAL_Assume_bytecap_for_opt_(pNode, sizeof(CNode));
		ATLASSERT(AtlIsValidAddress(pNode, sizeof(CNode)));
		ATLASSUME(pNode != NULL);

		pNode = pNode->m_pNext;  // start after the one specified
	}

	for( ; pNode != NULL; pNode = pNode->m_pNext )
	{
		if( ETraits::CompareElements( pNode->m_element, element ) )
			return( POSITION( pNode ) );
	}

	return( NULL );
}

#ifdef _DEBUG
template< typename E, class ETraits >
void CAtlList< E, ETraits >::AssertValid() const
{
	if( IsEmpty() )
	{
		// empty list
		ATLASSUME(m_pHead == NULL);
		ATLASSUME(m_pTail == NULL);
	}
	else
	{
		// non-empty list
		ATLASSERT(AtlIsValidAddress(m_pHead, sizeof(CNode)));
		ATLASSERT(AtlIsValidAddress(m_pTail, sizeof(CNode)));
	}
}
#endif

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CAtlMap
{
public:
	typedef typename KTraits::INARGTYPE KINARGTYPE;
	typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
	typedef typename VTraits::INARGTYPE VINARGTYPE;
	typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

	class CPair :
		public __POSITION
	{
	protected:
		CPair(/* _In_ */ KINARGTYPE key) :
			m_key( key )
		{
		}

	public:
		const K m_key;
		V m_value;
	};

private:
	class CNode :
		public CPair
	{
	public:
		CNode(
				/* _In_ */ KINARGTYPE key,
				_In_ UINT nHash) :
			CPair( key ),
			m_nHash( nHash )
		{
		}

	public:
		UINT GetHash() const throw()
		{
			return( m_nHash );
		}

	public:
		CNode* m_pNext;
		UINT m_nHash;
	};

public:
	CAtlMap(
		_In_ UINT nBins = 17,
		_In_ float fOptimalLoad = 0.75f,
		_In_ float fLoThreshold = 0.25f,
		_In_ float fHiThreshold = 2.25f,
		_In_ UINT nBlockSize = 10) throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();

	_Success_(return != false) bool Lookup(
		/* _In_ */ KINARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const CPair* Lookup(/* _In_ */ KINARGTYPE key) const throw();
	CPair* Lookup(/* _In_ */ KINARGTYPE key) throw();
	V& operator[](/* _In_ */ KINARGTYPE key);

	POSITION SetAt(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	void SetValueAt(
		_In_ POSITION pos,
		/* _In_ */ VINARGTYPE value);

	bool RemoveKey(/* _In_ */ KINARGTYPE key) throw();
	void RemoveAll();
	void RemoveAtPos(_In_ POSITION pos) throw();

	POSITION GetStartPosition() const throw();
	void GetNextAssoc(
		_Inout_ POSITION& pos,
		_Out_ KOUTARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const CPair* GetNext(_Inout_ POSITION& pos) const throw();
	CPair* GetNext(_Inout_ POSITION& pos) throw();
	const K& GetNextKey(_Inout_ POSITION& pos) const;
	const V& GetNextValue(_Inout_ POSITION& pos) const;
	V& GetNextValue(_Inout_ POSITION& pos);
	void GetAt(
		_In_ POSITION pos,
		_Out_ KOUTARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	CPair* GetAt(_In_ POSITION pos) throw();
	const CPair* GetAt(_In_ POSITION pos) const throw();
	const K& GetKeyAt(_In_ POSITION pos) const;
	const V& GetValueAt(_In_ POSITION pos) const;
	V& GetValueAt(_In_ POSITION pos);

	UINT GetHashTableSize() const throw();
	bool InitHashTable(
		_In_ UINT nBins,
		_In_ bool bAllocNow = true);
	void EnableAutoRehash() throw();
	void DisableAutoRehash() throw();
	void Rehash(_In_ UINT nBins = 0);
	void SetOptimalLoad(
		_In_ float fOptimalLoad,
		_In_ float fLoThreshold,
		_In_ float fHiThreshold,
		_In_ bool bRehashNow = false);

#ifdef _DEBUG
	void AssertValid() const;
#endif  // _DEBUG

// Implementation
private:
	CNode** m_ppBins;
	size_t m_nElements;
	UINT m_nBins;
	float m_fOptimalLoad;
	float m_fLoThreshold;
	float m_fHiThreshold;
	size_t m_nHiRehashThreshold;
	size_t m_nLoRehashThreshold;
	ULONG m_nLockCount;
	UINT m_nBlockSize;
	CAtlPlex* m_pBlocks;
	CNode* m_pFree;

private:
	bool IsLocked() const throw();
	UINT PickSize(_In_ size_t nElements) const throw();
	CNode* NewNode(
		/* _In_ */ KINARGTYPE key,
		_In_ UINT iBin,
		_In_ UINT nHash);
	void FreeNode(_Inout_ CNode* pNode);
	void FreePlexes() throw();
	_Ret_maybenull_ CNode* GetNode(
		/* _In_ */ KINARGTYPE key,
		_Out_ UINT& iBin,
		_Out_ UINT& nHash,
		_Outref_result_maybenull_ CNode*& pPrev) const throw();
	CNode* CreateNode(
		/* _In_ */ KINARGTYPE key,
		_In_ UINT iBin,
		_In_ UINT nHash);
	void RemoveNode(
		_In_ CNode* pNode,
		_In_opt_ CNode* pPrev) throw();
	CNode* FindNextNode(_In_ CNode* pNode) const throw();
	void UpdateRehashThresholds() throw();

public:
	~CAtlMap() throw();

private:
	// Private to prevent use
	CAtlMap(_In_ const CAtlMap&) throw();
	CAtlMap& operator=(_In_ const CAtlMap&) throw();
};

template< typename K, typename I, class KTraits = CElementTraits< K > >
class CMapToInterface :
	public CAtlMap< K, ATL::CComQIPtr< I >, KTraits, CComQIPtrElementTraits< I > >
{
public:
	CMapToInterface(_In_ UINT nBins = 17) throw();

private:
	// Private to prevent use
	CMapToInterface(_In_ const CMapToInterface&) throw();
	CMapToInterface& operator=(_In_ const CMapToInterface&) throw();
};

template< typename K, typename I, class KTraits >
inline CMapToInterface< K, I, KTraits >::CMapToInterface(_In_ UINT nBins) throw() :
	CAtlMap< K, ATL::CComQIPtr< I >, KTraits, CComQIPtrElementTraits< I > >( nBins )
{
}

template< typename K, typename V, class KTraits = CElementTraits< K > >
class CMapToAutoPtr :
	public CAtlMap< K, ATL::CAutoPtr< V >, KTraits, CAutoPtrElementTraits< V > >
{
public:
	CMapToAutoPtr(_In_ UINT nBins = 17) throw();

private:
	// Private to prevent use
	CMapToAutoPtr(_In_ const CMapToAutoPtr&) throw();
	CMapToAutoPtr& operator=(_In_ const CMapToAutoPtr&) throw();
};

template< typename K, typename V, class KTraits >
inline CMapToAutoPtr< K, V, KTraits >::CMapToAutoPtr(_In_ UINT nBins) throw() :
	CAtlMap< K, ATL::CAutoPtr< V >, KTraits, CAutoPtrElementTraits< V > >( nBins )
{
}

template< typename K, typename V, class KTraits, class VTraits >
inline size_t CAtlMap< K, V, KTraits, VTraits >::GetCount() const throw()
{
	return( m_nElements );
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool CAtlMap< K, V, KTraits, VTraits >::IsEmpty() const throw()
{
	return( m_nElements == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& CAtlMap< K, V, KTraits, VTraits >::operator[](/* _In_ */ KINARGTYPE key)
{
	CNode* pNode;
	UINT iBin;
	UINT nHash;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		pNode = CreateNode( key, iBin, nHash );
	}

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline UINT CAtlMap< K, V, KTraits, VTraits >::GetHashTableSize() const throw()
{
	return( m_nBins );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos,
	_Out_ KOUTARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	ATLENSURE( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );

	key = pNode->m_key;
	value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
inline typename CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos) throw()
{
	ATLASSERT( pos != NULL );

	return( static_cast< CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const typename CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos) const throw()
{
	ATLASSERT( pos != NULL );

	return( static_cast< const CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const K& CAtlMap< K, V, KTraits, VTraits >::GetKeyAt(_In_ POSITION pos) const
{
	ATLENSURE( pos != NULL );

	CNode* pNode = (CNode*)pos;

	return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const V& CAtlMap< K, V, KTraits, VTraits >::GetValueAt(_In_ POSITION pos) const
{
	ATLENSURE( pos != NULL );

	CNode* pNode = (CNode*)pos;

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& CAtlMap< K, V, KTraits, VTraits >::GetValueAt(_In_ POSITION pos)
{
	ATLENSURE( pos != NULL );

	CNode* pNode = (CNode*)pos;

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::DisableAutoRehash() throw()
{
	m_nLockCount++;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CAtlMap< K, V, KTraits, VTraits >::EnableAutoRehash() throw()
{
	ATLASSUME( m_nLockCount > 0 );
	m_nLockCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool CAtlMap< K, V, KTraits, VTraits >::IsLocked() const throw()
{
	return( m_nLockCount != 0 );
}

// List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
extern __declspec(selectany) const UINT s_anPrimes[] =
{
	17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
	1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
	13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
	131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
	832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
	5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
};

template< typename K, typename V, class KTraits, class VTraits >
UINT CAtlMap< K, V, KTraits, VTraits >::PickSize(_In_ size_t nElements) const throw()
{
	size_t nBins = (size_t)(nElements/m_fOptimalLoad);
	UINT nBinsEstimate = UINT(  UINT_MAX < nBins ? UINT_MAX : nBins );

	// Find the smallest prime greater than our estimate
	int iPrime = 0;
	while( nBinsEstimate > s_anPrimes[iPrime] )
	{
		iPrime++;
	}

	if( s_anPrimes[iPrime] == UINT_MAX )
	{
		return( nBinsEstimate );
	}
	else
	{
		return( s_anPrimes[iPrime] );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
typename CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::CreateNode(
	/* _In_ */ KINARGTYPE key,
	_In_ UINT iBin,
	_In_ UINT nHash)
{
	CNode* pNode;

	if( m_ppBins == NULL )
	{
		bool bSuccess;

		bSuccess = InitHashTable( m_nBins );
		if( !bSuccess )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
	}

	pNode = NewNode( key, iBin, nHash );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CAtlMap< K, V, KTraits, VTraits >::GetStartPosition() const throw()
{
	if( IsEmpty() )
	{
		return( NULL );
	}

	for( UINT iBin = 0; iBin < m_nBins; iBin++ )
	{
		if( m_ppBins[iBin] != NULL )
		{
			return( POSITION( m_ppBins[iBin] ) );
		}
	}
	ATLASSERT( false );

	return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CAtlMap< K, V, KTraits, VTraits >::SetAt(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	CNode* pNode;
	UINT iBin;
	UINT nHash;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		pNode = CreateNode( key, iBin, nHash );
		_ATLTRY
		{
			pNode->m_value = value;
		}
		_ATLCATCHALL()
		{
			RemoveAtPos( POSITION( pNode ) );
			_ATLRETHROW;
		}
	}
	else
	{
		pNode->m_value = value;
	}

	return( POSITION( pNode ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::SetValueAt(
	_In_ POSITION pos,
	/* _In_ */ VINARGTYPE value)
{
	ATLASSUME( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );

	pNode->m_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::CAtlMap(
		_In_ UINT nBins,
		_In_ float fOptimalLoad,
		_In_ float fLoThreshold,
		_In_ float fHiThreshold,
		_In_ UINT nBlockSize) throw() :
	m_ppBins( NULL ),
	m_nElements( 0 ),
	m_nBins( nBins ),
	m_fOptimalLoad( fOptimalLoad ),
	m_fLoThreshold( fLoThreshold ),
	m_fHiThreshold( fHiThreshold ),
	m_nHiRehashThreshold( UINT_MAX ),
	m_nLoRehashThreshold( 0 ),
	m_nLockCount( 0 ),  // Start unlocked
	m_nBlockSize( nBlockSize ),
	m_pBlocks( NULL ),
	m_pFree( NULL )
{
	ATLASSERT( nBins > 0 );
	ATLASSERT( nBlockSize > 0 );

	SetOptimalLoad( fOptimalLoad, fLoThreshold, fHiThreshold, false );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::SetOptimalLoad(
	_In_ float fOptimalLoad,
	_In_ float fLoThreshold,
	_In_ float fHiThreshold,
	_In_ bool bRehashNow)
{
	ATLASSERT( fOptimalLoad > 0 );
	ATLASSERT( (fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad) );
	ATLASSERT( fHiThreshold > fOptimalLoad );

	m_fOptimalLoad = fOptimalLoad;
	m_fLoThreshold = fLoThreshold;
	m_fHiThreshold = fHiThreshold;

	UpdateRehashThresholds();

	if( bRehashNow && ((m_nElements > m_nHiRehashThreshold) ||
		(m_nElements < m_nLoRehashThreshold)) )
	{
		Rehash( PickSize( m_nElements ) );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::UpdateRehashThresholds() throw()
{
	m_nHiRehashThreshold = size_t( m_fHiThreshold*m_nBins );
	m_nLoRehashThreshold = size_t( m_fLoThreshold*m_nBins );
	if( m_nLoRehashThreshold < 17 )
	{
		m_nLoRehashThreshold = 0;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
bool CAtlMap< K, V, KTraits, VTraits >::InitHashTable(_In_ UINT nBins, _In_ bool bAllocNow)
{
	ATLASSUME( m_nElements == 0 );
	ATLASSERT( nBins > 0 );

	if( m_ppBins != NULL )
	{
		delete[] m_ppBins;
		m_ppBins = NULL;
	}

	if( bAllocNow )
	{
		m_ppBins = _ATL_NEW CNode*[nBins];
		if( m_ppBins == NULL )
		{
			return false;
		}

		ATLENSURE( UINT_MAX / sizeof( CNode* ) >= nBins );
		memset( m_ppBins, 0, sizeof( CNode* )*nBins );
	}
	m_nBins = nBins;

	UpdateRehashThresholds();

	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::RemoveAll()
{
	DisableAutoRehash();
	if( m_ppBins != NULL )
	{
		for( UINT iBin = 0; iBin < m_nBins; iBin++ )
		{
			CNode* pNext;

			pNext = m_ppBins[iBin];
			while( pNext != NULL )
			{
				CNode* pKill;

				pKill = pNext;
				pNext = pNext->m_pNext;
				FreeNode( pKill );
			}
		}
	}

	delete[] m_ppBins;
	m_ppBins = NULL;
	m_nElements = 0;

	if( !IsLocked() )
	{
		InitHashTable( PickSize( m_nElements ), false );
	}

	FreePlexes();
	EnableAutoRehash();
}

template< typename K, typename V, class KTraits, class VTraits >
CAtlMap< K, V, KTraits, VTraits >::~CAtlMap() throw()
{
	_ATLTRY
	{
		RemoveAll();
	}
	_ATLCATCHALL()
	{
		ATLASSERT(false);
	}
}

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits, class VTraits >
typename CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::NewNode(
	/* _In_ */ KINARGTYPE key,
	_In_ UINT iBin,
	_In_ UINT nHash)
{
	CNode* pNewNode;

	if( m_pFree == NULL )
	{
		CAtlPlex* pPlex;
		CNode* pNode;

		pPlex = CAtlPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
		if( pPlex == NULL )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
		pNode = (CNode*)pPlex->data();
		pNode += m_nBlockSize-1;
		for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->m_pNext = m_pFree;
			m_pFree = pNode;
			pNode--;
		}
	}
	ATLENSURE(m_pFree != NULL );
	pNewNode = m_pFree;
	m_pFree = pNewNode->m_pNext;

	_ATLTRY
	{
		::new( pNewNode ) CNode( key, nHash );
	}
	_ATLCATCHALL()
	{
		ATLPREFAST_SUPPRESS(6001)
		pNewNode->m_pNext = m_pFree;
		m_pFree = pNewNode;
		ATLPREFAST_UNSUPPRESS()

		_ATLRETHROW;
	}
	m_nElements++;

	pNewNode->m_pNext = m_ppBins[iBin];
	m_ppBins[iBin] = pNewNode;

	if( (m_nElements > m_nHiRehashThreshold) && !IsLocked() )
	{
		Rehash( PickSize( m_nElements ) );
	}

	return( pNewNode );
}

#pragma pop_macro("new")

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::FreeNode(_Inout_ CNode* pNode)
{
	ATLENSURE( pNode != NULL );

	pNode->~CNode();
	pNode->m_pNext = m_pFree;
	m_pFree = pNode;

	ATLASSUME( m_nElements > 0 );
	m_nElements--;

	if( (m_nElements < m_nLoRehashThreshold) && !IsLocked() )
	{
		Rehash( PickSize( m_nElements ) );
	}

	if( m_nElements == 0 )
	{
		FreePlexes();
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::FreePlexes() throw()
{
	m_pFree = NULL;
	if( m_pBlocks != NULL )
	{
		m_pBlocks->FreeDataChain();
		m_pBlocks = NULL;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
_Ret_maybenull_ typename CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::GetNode(
	/* _In_ */ KINARGTYPE key,
	_Out_ UINT& iBin,
	_Out_ UINT& nHash,
	_Outref_result_maybenull_ CNode*& pPrev) const throw()
{
	CNode* pFollow;

	nHash = KTraits::Hash( key );
	iBin = nHash%m_nBins;
	pPrev = NULL;

	if( m_ppBins == NULL )
	{
		return( NULL );
	}

	pFollow = NULL;
	for( CNode* pNode = m_ppBins[iBin]; pNode != NULL; pNode = pNode->m_pNext )
	{
		if( (pNode->GetHash() == nHash) && KTraits::CompareElements( pNode->m_key, key ) )
		{
			pPrev = pFollow;
			return( pNode );
		}
		pFollow = pNode;
	}

	return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
_Success_(return != false) bool CAtlMap< K, V, KTraits, VTraits >::Lookup(
	/* _In_ */ KINARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	UINT iBin;
	UINT nHash;
	CNode* pNode;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		return( false );
	}

	value = pNode->m_value;

	return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::Lookup(
	/* _In_ */ KINARGTYPE key) const throw()
{
	UINT iBin;
	UINT nHash;
	CNode* pNode;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::Lookup(
	/* _In_ */ KINARGTYPE key) throw()
{
	UINT iBin;
	UINT nHash;
	CNode* pNode;
	CNode* pPrev;

	pNode = GetNode( key, iBin, nHash, pPrev );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
bool CAtlMap< K, V, KTraits, VTraits >::RemoveKey(/* _In_ */ KINARGTYPE key) throw()
{
	CNode* pNode;
	UINT iBin;
	UINT nHash;
	CNode* pPrev;

	pPrev = NULL;
	pNode = GetNode( key, iBin, nHash, pPrev );
	if( pNode == NULL )
	{
		return( false );
	}

	RemoveNode( pNode, pPrev );

	return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::RemoveNode(
	_In_ CNode* pNode,
	_In_opt_ CNode* pPrev) throw()
{
	ATLENSURE( pNode != NULL );

	UINT iBin = pNode->GetHash() % m_nBins;

	if( pPrev == NULL )
	{
		ATLASSUME( m_ppBins[iBin] == pNode );
		m_ppBins[iBin] = pNode->m_pNext;
	}
	else
	{
		ATLASSERT( pPrev->m_pNext == pNode );
		pPrev->m_pNext = pNode->m_pNext;
	}
	FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::RemoveAtPos(_In_ POSITION pos) throw()
{
	ATLENSURE( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );
	CNode* pPrev = NULL;
	UINT iBin = pNode->GetHash() % m_nBins;

	ATLASSUME( m_ppBins[iBin] != NULL );
	if( pNode == m_ppBins[iBin] )
	{
		pPrev = NULL;
	}
	else
	{
		pPrev = m_ppBins[iBin];
		while( pPrev->m_pNext != pNode )
		{
			pPrev = pPrev->m_pNext;
			ATLASSERT( pPrev != NULL );
		}
	}
	RemoveNode( pNode, pPrev );
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::Rehash(_In_ UINT nBins)
{
	if( nBins == 0 )
	{
		nBins = PickSize( m_nElements );
	}

	if( nBins == m_nBins )
	{
		return;
	}

	ATLTRACE(atlTraceMap, 2, _T("Rehash: %u bins\n"), nBins );

	if( m_ppBins == NULL )
	{
		// Just set the new number of bins
		InitHashTable( nBins, false );
		return;
	}

	CNode** ppBins = _ATL_NEW CNode*[nBins];
	if (ppBins == NULL)
	{
		AtlThrow( E_OUTOFMEMORY );
	}

	ATLENSURE( UINT_MAX / sizeof( CNode* ) >= nBins );
	memset( ppBins, 0, nBins*sizeof( CNode* ) );

	// Nothing gets copied.  We just rewire the old nodes
	// into the new bins.
	for( UINT iSrcBin = 0; iSrcBin < m_nBins; iSrcBin++ )
	{
		CNode* pNode;

		pNode = m_ppBins[iSrcBin];
		while( pNode != NULL )
		{
			CNode* pNext;
			UINT iDestBin;

			pNext = pNode->m_pNext;  // Save so we don't trash it
			iDestBin = pNode->GetHash()%nBins;
			pNode->m_pNext = ppBins[iDestBin];
			ppBins[iDestBin] = pNode;

			pNode = pNext;
		}
	}

	delete[] m_ppBins;
	m_ppBins = ppBins;
	m_nBins = nBins;

	UpdateRehashThresholds();
}

template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::GetNextAssoc(
	_Inout_ POSITION& pos,
	_Out_ KOUTARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSUME( m_ppBins != NULL );
	ATLENSURE( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );
	key = pNode->m_key;
	value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::GetNext(
	_Inout_ POSITION& pos) const throw()
{
	CNode* pNode;
	CNode* pNext;

	ATLASSUME( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename CAtlMap< K, V, KTraits, VTraits >::CPair* CAtlMap< K, V, KTraits, VTraits >::GetNext(
	_Inout_ POSITION& pos) throw()
{
	ATLASSUME( m_ppBins != NULL );
	ATLASSERT( pos != NULL );

	CNode* pNode = static_cast< CNode* >( pos );
	CNode* pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const K& CAtlMap< K, V, KTraits, VTraits >::GetNextKey(
	_Inout_ POSITION& pos) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSUME( m_ppBins != NULL );
	ATLENSURE( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CAtlMap< K, V, KTraits, VTraits >::GetNextValue(
	_Inout_ POSITION& pos) const
{
	CNode* pNode;
	CNode* pNext;

	ATLASSUME( m_ppBins != NULL );
	ATLENSURE( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& CAtlMap< K, V, KTraits, VTraits >::GetNextValue(
	_Inout_ POSITION& pos)
{
	CNode* pNode;
	CNode* pNext;

	ATLASSUME( m_ppBins != NULL );
	ATLENSURE( pos != NULL );

	pNode = (CNode*)pos;
	pNext = FindNextNode( pNode );

	pos = POSITION( pNext );

	return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
typename CAtlMap< K, V, KTraits, VTraits >::CNode* CAtlMap< K, V, KTraits, VTraits >::FindNextNode(
	_In_ CNode* pNode) const throw()
{
	CNode* pNext;

	if(pNode == NULL)
	{
		ATLASSERT(FALSE);
		return NULL;
	}

	if( pNode->m_pNext != NULL )
	{
		pNext = pNode->m_pNext;
	}
	else
	{
		UINT iBin;

		pNext = NULL;
		iBin = (pNode->GetHash()%m_nBins)+1;
		while( (pNext == NULL) && (iBin < m_nBins) )
		{
			if( m_ppBins[iBin] != NULL )
			{
				pNext = m_ppBins[iBin];
			}

			iBin++;
		}
	}

	return( pNext );
}

#ifdef _DEBUG
template< typename K, typename V, class KTraits, class VTraits >
void CAtlMap< K, V, KTraits, VTraits >::AssertValid() const
{
	ATLASSUME( m_nBins > 0 );
	// non-empty map should have hash table
	ATLASSERT( IsEmpty() || (m_ppBins != NULL) );
}
#endif

#pragma push_macro("new")
#undef new

//
// The red-black tree code is based on the descriptions in
// "Introduction to Algorithms", by Cormen, Leiserson, and Rivest
//
template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CRBTree
{
public:
	typedef typename KTraits::INARGTYPE KINARGTYPE;
	typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
	typedef typename VTraits::INARGTYPE VINARGTYPE;
	typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

public:
	class CPair :
		public __POSITION
	{
	protected:

		CPair(
				/* _In_ */ KINARGTYPE key,
				/* _In_ */ VINARGTYPE value) :
			m_key( key ),
			m_value( value )
		{
		}
		~CPair() throw()
		{
		}

	public:
		const K m_key;
		V m_value;
	};

private:

	class CNode :
		public CPair
	{
	public:
		enum RB_COLOR
		{
			RB_RED,
			RB_BLACK
		};

	public:
		RB_COLOR m_eColor;
		CNode* m_pLeft;
		CNode* m_pRight;
		CNode* m_pParent;

		CNode(
				/* _In_ */ KINARGTYPE key,
				/* _In_ */ VINARGTYPE value) :
			CPair( key, value ),
			m_pParent( NULL ),
			m_eColor( RB_BLACK )
		{
		}
		~CNode() throw()
		{
		}
	};

private:
	CNode* m_pRoot;
	size_t m_nCount;
	CNode* m_pFree;
	CAtlPlex* m_pBlocks;
	size_t m_nBlockSize;

	// sentinel node
	CNode *m_pNil;

	// methods
	bool IsNil(_In_ CNode *p) const throw();
	void SetNil(_Outptr_ CNode **p) throw();

	CNode* NewNode(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	void FreeNode(_Inout_ CNode* pNode) throw();
	void RemovePostOrder(_In_ CNode* pNode) throw();
	CNode* LeftRotate(_In_ CNode* pNode) throw();
	CNode* RightRotate(_In_ CNode* pNode) throw();
	void SwapNode(
		_Out_ CNode* pDest,
		_Inout_ CNode* pSrc) throw();
	CNode* InsertImpl(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	void RBDeleteFixup(_In_ CNode* pNode) throw();
	bool RBDelete(_In_opt_ CNode* pZ) throw();

#ifdef _DEBUG

	// internal debugging code to verify red-black properties of tree:
	// 1) Every node is either red or black
	// 2) Every leaf (NIL) is black
	// 3) If a node is red, both its children are black
	// 4) Every simple path from a node to a descendant leaf node contains
	//    the same number of black nodes
private:
	void VerifyIntegrity(
		_In_ const CNode *pNode,
		_In_ int nCurrBlackDepth,
		_Out_ int &nBlackDepth) const throw();

public:
	void VerifyIntegrity() const throw();

#endif // _DEBUG

protected:
	CNode* Minimum(_In_opt_ CNode* pNode) const throw();
	CNode* Maximum(_In_opt_ CNode* pNode) const throw();
	CNode* Predecessor(_In_opt_ CNode* pNode) const throw();
	CNode* Successor(_In_opt_ CNode* pNode) const throw();
	CNode* RBInsert(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	CNode* Find(/* _In_ */ KINARGTYPE key) const throw();
	CNode* FindPrefix(/* _In_ */ KINARGTYPE key) const throw();

protected:
	explicit CRBTree(_In_ size_t nBlockSize = 10) throw();  // protected to prevent instantiation

public:
	~CRBTree() throw();

	void RemoveAll() throw();
	void RemoveAt(_In_ POSITION pos) throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();

	POSITION FindFirstKeyAfter(/* _In_ */ KINARGTYPE key) const throw();

	POSITION GetHeadPosition() const throw();
	POSITION GetTailPosition() const throw();
	void GetNextAssoc(
		_Inout_ POSITION& pos,
		_Out_ KOUTARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const CPair* GetNext(_Inout_ POSITION& pos) const throw();
	CPair* GetNext(_Inout_ POSITION& pos) throw();
	const CPair* GetPrev(_Inout_ POSITION& pos) const throw();
	CPair* GetPrev(_Inout_ POSITION& pos) throw();
	const K& GetNextKey(_Inout_ POSITION& pos) const throw();
	const V& GetNextValue(_Inout_ POSITION& pos) const throw();
	V& GetNextValue(_Inout_ POSITION& pos) throw();

	CPair* GetAt(_In_ POSITION pos) throw();
	const CPair* GetAt(_In_ POSITION pos) const throw();
	void GetAt(
		_In_ POSITION pos,
		_Out_ KOUTARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const K& GetKeyAt(_In_ POSITION pos) const;
	const V& GetValueAt(_In_ POSITION pos) const;
	V& GetValueAt(_In_ POSITION pos);
	void SetValueAt(
		_In_ POSITION pos,
		/* _In_ */ VINARGTYPE value);

private:
	// Private to prevent use
	CRBTree(_In_ const CRBTree&) throw();
	CRBTree& operator=(_In_ const CRBTree&) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
inline bool CRBTree< K, V, KTraits, VTraits >::IsNil(_In_ CNode *p) const throw()
{
	return ( p == m_pNil );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CRBTree< K, V, KTraits, VTraits >::SetNil(_Outptr_ CNode **p) throw()
{
	ATLENSURE( p != NULL );
	*p = m_pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
CRBTree< K, V, KTraits, VTraits >::CRBTree(_In_ size_t nBlockSize) throw() :
	m_pRoot( NULL ),
	m_nCount( 0 ),
	m_nBlockSize( nBlockSize ),
	m_pFree( NULL ),
	m_pBlocks( NULL ),
	m_pNil( NULL )
{
	ATLASSERT( nBlockSize > 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
CRBTree< K, V, KTraits, VTraits >::~CRBTree() throw()
{
	RemoveAll();
	if (m_pNil != NULL)
	{
		free(m_pNil);
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::RemoveAll() throw()
{
	if (!IsNil(m_pRoot))
		RemovePostOrder(m_pRoot);
	m_nCount = 0;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
	m_pFree = NULL;
	m_pRoot = m_pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
size_t CRBTree< K, V, KTraits, VTraits >::GetCount() const throw()
{
	return m_nCount;
}

template< typename K, typename V, class KTraits, class VTraits >
bool CRBTree< K, V, KTraits, VTraits >::IsEmpty() const throw()
{
	return( m_nCount == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBTree< K, V, KTraits, VTraits >::FindFirstKeyAfter(/* _In_ */ KINARGTYPE key) const throw()
{
	return( FindPrefix( key ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::RemoveAt(_In_ POSITION pos) throw()
{
	ATLASSERT(pos != NULL);
	RBDelete(static_cast<CNode*>(pos));
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBTree< K, V, KTraits, VTraits >::GetHeadPosition() const throw()
{
	return( Minimum( m_pRoot ) );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBTree< K, V, KTraits, VTraits >::GetTailPosition() const throw()
{
	return( Maximum( m_pRoot ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::GetNextAssoc(
	_Inout_ POSITION& pos,
	_Out_ KOUTARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast< CNode* >(pos);

	key = pNode->m_key;
	value = pNode->m_value;

	pos = Successor(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBTree< K, V, KTraits, VTraits >::CPair* CRBTree< K, V, KTraits, VTraits >::GetNext(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast< CNode* >(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CPair* CRBTree< K, V, KTraits, VTraits >::GetNext(
	_Inout_ POSITION& pos) throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast< CNode* >(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBTree< K, V, KTraits, VTraits >::CPair* CRBTree< K, V, KTraits, VTraits >::GetPrev(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast< CNode* >(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CPair* CRBTree< K, V, KTraits, VTraits >::GetPrev(
	_Inout_ POSITION& pos) throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast< CNode* >(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& CRBTree< K, V, KTraits, VTraits >::GetNextKey(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CRBTree< K, V, KTraits, VTraits >::GetNextValue(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& CRBTree< K, V, KTraits, VTraits >::GetNextValue(
	_Inout_ POSITION& pos) throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CPair* CRBTree< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos) throw()
{
	ATLASSERT( pos != NULL );

	return( static_cast< CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBTree< K, V, KTraits, VTraits >::CPair* CRBTree< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos) const throw()
{
	ATLASSERT( pos != NULL );

	return( static_cast< const CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos,
	_Out_ KOUTARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	ATLENSURE( pos != NULL );
	key = static_cast<CNode*>(pos)->m_key;
	value = static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& CRBTree< K, V, KTraits, VTraits >::GetKeyAt(_In_ POSITION pos) const
{
	ATLENSURE( pos != NULL );
	return static_cast<CNode*>(pos)->m_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CRBTree< K, V, KTraits, VTraits >::GetValueAt(_In_ POSITION pos) const
{
	ATLENSURE( pos != NULL );
	return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& CRBTree< K, V, KTraits, VTraits >::GetValueAt(_In_ POSITION pos)
{
	ATLENSURE( pos != NULL );
	return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::SetValueAt(
	_In_ POSITION pos,
	/* _In_ */ VINARGTYPE value)
{
	ATLENSURE( pos != NULL );
	static_cast<CNode*>(pos)->m_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::NewNode(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	if( m_pFree == NULL )
	{
		if (m_pNil == NULL)
		{
			m_pNil = reinterpret_cast<CNode *>(malloc(sizeof( CNode )));
			if (m_pNil == NULL)
			{
				AtlThrow( E_OUTOFMEMORY );
			}
			memset(m_pNil, 0x00, sizeof(CNode));
			m_pNil->m_eColor = CNode::RB_BLACK;
			m_pNil->m_pParent = m_pNil->m_pLeft = m_pNil->m_pRight = m_pNil;
			m_pRoot = m_pNil;
		}

		CAtlPlex* pPlex = CAtlPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
		if( pPlex == NULL )
		{
			AtlThrow( E_OUTOFMEMORY );
		}
		CNode* pNode = static_cast< CNode* >( pPlex->data() );
		pNode += m_nBlockSize-1;
		for( INT_PTR iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
		{
			pNode->m_pLeft = m_pFree;
			m_pFree = pNode;
			pNode--;
		}
	}
	ATLASSUME( m_pFree != NULL );

	CNode* pNewNode = m_pFree;
	::new( pNewNode ) CNode( key, value );

	m_pFree = m_pFree->m_pLeft;
	pNewNode->m_eColor = CNode::RB_RED;
	SetNil(&pNewNode->m_pLeft);
	SetNil(&pNewNode->m_pRight);
	SetNil(&pNewNode->m_pParent);

	m_nCount++;
	ATLASSUME( m_nCount > 0 );

	return( pNewNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::FreeNode(_Inout_ CNode* pNode) throw()
{
	ATLENSURE( pNode != NULL );
	pNode->~CNode();
	pNode->m_pLeft = m_pFree;
	m_pFree = pNode;
	ATLASSUME( m_nCount > 0 );
	m_nCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::RemovePostOrder(_In_ CNode* pNode) throw()
{
	if (IsNil(pNode))
		return;
	RemovePostOrder(pNode->m_pLeft);
	RemovePostOrder(pNode->m_pRight);
	FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::LeftRotate(
	_In_ CNode* pNode) throw()
{
	ATLASSERT(pNode != NULL);
	if(pNode == NULL)
		return NULL;

	CNode* pRight = pNode->m_pRight;
	pNode->m_pRight = pRight->m_pLeft;
	if (!IsNil(pRight->m_pLeft))
		pRight->m_pLeft->m_pParent = pNode;

	pRight->m_pParent = pNode->m_pParent;
	if (IsNil(pNode->m_pParent))
		m_pRoot = pRight;
	else if (pNode == pNode->m_pParent->m_pLeft)
		pNode->m_pParent->m_pLeft = pRight;
	else
		pNode->m_pParent->m_pRight = pRight;

	pRight->m_pLeft = pNode;
	pNode->m_pParent = pRight;
	return pNode;

}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::RightRotate(
	_In_ CNode* pNode) throw()
{
	ATLASSERT(pNode != NULL);
	if(pNode == NULL)
		return NULL;

	CNode* pLeft = pNode->m_pLeft;
	pNode->m_pLeft = pLeft->m_pRight;
	if (!IsNil(pLeft->m_pRight))
		pLeft->m_pRight->m_pParent = pNode;

	pLeft->m_pParent = pNode->m_pParent;
	if (IsNil(pNode->m_pParent))
		m_pRoot = pLeft;
	else if (pNode == pNode->m_pParent->m_pRight)
		pNode->m_pParent->m_pRight = pLeft;
	else
		pNode->m_pParent->m_pLeft = pLeft;

	pLeft->m_pRight = pNode;
	pNode->m_pParent = pLeft;
	return pNode;
}

#pragma warning(push)
#pragma warning(disable:28182 28183)

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::Find(
	/* _In_ */ KINARGTYPE key) const throw()
{
	CNode* pKey = NULL;
	CNode* pNode = m_pRoot;
	while( !IsNil(pNode) && (pKey == NULL) )
	{
		int nCompare = KTraits::CompareElementsOrdered( key, pNode->m_key );
		if( nCompare == 0 )
		{
			pKey = pNode;
		}
		else
		{
			if( nCompare < 0 )
			{
				pNode = pNode->m_pLeft;
			}
			else
			{
				pNode = pNode->m_pRight;
			}
		}
	}

	if( pKey == NULL )
	{
		return( NULL );
	}

#pragma warning(push)
#pragma warning(disable:4127)

	while( true )
	{
		CNode* pPrev = Predecessor( pKey );
		if( (pPrev != NULL) && KTraits::CompareElements( key, pPrev->m_key ) )
		{
			pKey = pPrev;
		}
		else
		{
			return( pKey );
		}
	}

#pragma warning(pop)
}

#pragma warning(pop)

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::FindPrefix(
	/* _In_ */ KINARGTYPE key) const throw()
{
	// First, attempt to find a node that matches the key exactly
	CNode* pParent = NULL;
	CNode* pKey = NULL;
	CNode* pNode = m_pRoot;
	while( !IsNil(pNode) && (pKey == NULL) )
	{
		pParent = pNode;
		int nCompare = KTraits::CompareElementsOrdered( key, pNode->m_key );
		if( nCompare == 0 )
		{
			pKey = pNode;
		}
		else if( nCompare < 0 )
		{
			pNode = pNode->m_pLeft;
		}
		else
		{
			pNode = pNode->m_pRight;
		}
	}

	if( pKey != NULL )
	{
		// We found a node with the exact key, so find the first node after
		// this one with a different key
		while( true )
		{
			CNode* pNext = Successor( pKey );
			if ((pNext != NULL) && KTraits::CompareElements( key, pNext->m_key ))
			{
				pKey = pNext;
			}
			else
			{
				return pNext;
			}
		}
	}
	else if (pParent != NULL)
	{
		// No node matched the key exactly, so pick the first node with
		// a key greater than the given key
		int nCompare = KTraits::CompareElementsOrdered( key, pParent->m_key );
		if( nCompare < 0 )
		{
			pKey = pParent;
		}
		else
		{
			ATLASSERT( nCompare > 0 );
			pKey = Successor( pParent );
		}
	}

	return( pKey );
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::SwapNode(_Out_ CNode* pDest, _Inout_ CNode* pSrc) throw()
{
	ATLENSURE( pDest != NULL );
	ATLENSURE( pSrc != NULL );

	pDest->m_pParent = pSrc->m_pParent;
	if (pSrc->m_pParent->m_pLeft == pSrc)
	{
		pSrc->m_pParent->m_pLeft = pDest;
	}
	else
	{
		pSrc->m_pParent->m_pRight = pDest;
	}

	pDest->m_pRight = pSrc->m_pRight;
	pDest->m_pLeft = pSrc->m_pLeft;
	pDest->m_eColor = pSrc->m_eColor;
	pDest->m_pRight->m_pParent = pDest;
	pDest->m_pLeft->m_pParent = pDest;

	if (m_pRoot == pSrc)
	{
		m_pRoot = pDest;
	}
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::InsertImpl(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	CNode* pNew = NewNode( key, value );

	CNode* pY = NULL;
	CNode* pX = m_pRoot;

	while (!IsNil(pX))
	{
		pY = pX;
		if( KTraits::CompareElementsOrdered( key, pX->m_key ) <= 0 )
			pX = pX->m_pLeft;
		else
			pX = pX->m_pRight;
	}

	pNew->m_pParent = pY;
	if (pY == NULL)
	{
		m_pRoot = pNew;
	}
	else if( KTraits::CompareElementsOrdered( key, pY->m_key ) <= 0 )
		pY->m_pLeft = pNew;
	else
		pY->m_pRight = pNew;

	return pNew;
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::RBDeleteFixup(_In_ CNode* pNode) throw()
{
	ATLENSURE( pNode != NULL );

	CNode* pX = pNode;
	CNode* pW = NULL;

	while (( pX != m_pRoot ) && ( pX->m_eColor == CNode::RB_BLACK ))
	{
		if (pX == pX->m_pParent->m_pLeft)
		{
			pW = pX->m_pParent->m_pRight;
			if (pW->m_eColor == CNode::RB_RED)
			{
				pW->m_eColor = CNode::RB_BLACK;
				pW->m_pParent->m_eColor = CNode::RB_RED;
				LeftRotate(pX->m_pParent);
				pW = pX->m_pParent->m_pRight;
			}
			if (pW->m_pLeft->m_eColor == CNode::RB_BLACK && pW->m_pRight->m_eColor == CNode::RB_BLACK)
			{
				pW->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent;
			}
			else
			{
				if (pW->m_pRight->m_eColor == CNode::RB_BLACK)
				{
					pW->m_pLeft->m_eColor = CNode::RB_BLACK;
					pW->m_eColor = CNode::RB_RED;
					RightRotate(pW);
					pW = pX->m_pParent->m_pRight;
				}
				pW->m_eColor = pX->m_pParent->m_eColor;
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pW->m_pRight->m_eColor = CNode::RB_BLACK;
				LeftRotate(pX->m_pParent);
				pX = m_pRoot;
			}
		}
		else
		{
			pW = pX->m_pParent->m_pLeft;
			if (pW->m_eColor == CNode::RB_RED)
			{
				pW->m_eColor = CNode::RB_BLACK;
				pW->m_pParent->m_eColor = CNode::RB_RED;
				RightRotate(pX->m_pParent);
				pW = pX->m_pParent->m_pLeft;
			}
			if (pW->m_pRight->m_eColor == CNode::RB_BLACK && pW->m_pLeft->m_eColor == CNode::RB_BLACK)
			{
				pW->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent;
			}
			else
			{
				if (pW->m_pLeft->m_eColor == CNode::RB_BLACK)
				{
					pW->m_pRight->m_eColor = CNode::RB_BLACK;
					pW->m_eColor = CNode::RB_RED;
					LeftRotate(pW);
					pW = pX->m_pParent->m_pLeft;
				}
				pW->m_eColor = pX->m_pParent->m_eColor;
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pW->m_pLeft->m_eColor = CNode::RB_BLACK;
				RightRotate(pX->m_pParent);
				pX = m_pRoot;
			}
		}
	}

	pX->m_eColor = CNode::RB_BLACK;
}


template< typename K, typename V, class KTraits, class VTraits >
bool CRBTree< K, V, KTraits, VTraits >::RBDelete(_In_opt_ CNode* pZ) throw()
{
	if (pZ == NULL)
		return false;

	CNode* pY = NULL;
	CNode* pX = NULL;
	if (IsNil(pZ->m_pLeft) || IsNil(pZ->m_pRight))
		pY = pZ;
	else
		pY = Successor(pZ);

	if (!IsNil(pY->m_pLeft))
		pX = pY->m_pLeft;
	else
		pX = pY->m_pRight;

	pX->m_pParent = pY->m_pParent;

	if (IsNil(pY->m_pParent))
		m_pRoot = pX;
	else if (pY == pY->m_pParent->m_pLeft)
		pY->m_pParent->m_pLeft = pX;
	else
		pY->m_pParent->m_pRight = pX;

	if (pY->m_eColor == CNode::RB_BLACK)
		RBDeleteFixup(pX);

	if (pY != pZ)
		SwapNode(pY, pZ);

	if (m_pRoot != NULL)
		SetNil(&m_pRoot->m_pParent);

	FreeNode( pZ );

	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::Minimum(
	_In_opt_ CNode* pNode) const throw()
{
	if (pNode == NULL || IsNil(pNode))
	{
		return NULL;
	}

	CNode* pMin = pNode;
	while (!IsNil(pMin->m_pLeft))
	{
		pMin = pMin->m_pLeft;
	}

	return pMin;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::Maximum(
	_In_opt_ CNode* pNode) const throw()
{
	if (pNode == NULL || IsNil(pNode))
	{
		return NULL;
	}

	CNode* pMax = pNode;
	while (!IsNil(pMax->m_pRight))
	{
		pMax = pMax->m_pRight;
	}

	return pMax;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::Predecessor(
	_In_opt_ CNode* pNode ) const throw()
{
	if( pNode == NULL )
	{
		return( NULL );
	}
	if( !IsNil(pNode->m_pLeft) )
	{
		return( Maximum( pNode->m_pLeft ) );
	}

	CNode* pParent = pNode->m_pParent;
	CNode* pLeft = pNode;
	while( !IsNil(pParent) && (pLeft == pParent->m_pLeft) )
	{
		pLeft = pParent;
		pParent = pParent->m_pParent;
	}

	if (IsNil(pParent))
	{
		pParent = NULL;
	}
	return( pParent );
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::Successor(
	_In_opt_ CNode* pNode) const throw()
{
	if ( pNode == NULL )
	{
		return NULL;
	}
	if ( !IsNil(pNode->m_pRight) )
	{
		return Minimum(pNode->m_pRight);
	}

	CNode* pParent = pNode->m_pParent;
	CNode* pRight = pNode;
	while ( !IsNil(pParent) && (pRight == pParent->m_pRight) )
	{
		pRight = pParent;
		pParent = pParent->m_pParent;
	}

	if (IsNil(pParent))
	{
		pParent = NULL;
	}
	return pParent;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree< K, V, KTraits, VTraits >::CNode* CRBTree< K, V, KTraits, VTraits >::RBInsert(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	CNode* pNewNode = InsertImpl( key, value );

	CNode* pX = pNewNode;
	pX->m_eColor = CNode::RB_RED;
	CNode* pY = NULL;
	while (pX != m_pRoot && pX->m_pParent->m_eColor == CNode::RB_RED)
	{
		if (pX->m_pParent == pX->m_pParent->m_pParent->m_pLeft)
		{
			pY = pX->m_pParent->m_pParent->m_pRight;
			if (pY != NULL && pY->m_eColor == CNode::RB_RED)
			{
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pY->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent->m_pParent;
			}
			else
			{
				if (pX == pX->m_pParent->m_pRight)
				{
					pX = pX->m_pParent;
					LeftRotate(pX);
				}
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				RightRotate(pX->m_pParent->m_pParent);
			}
		}
		else
		{
			pY = pX->m_pParent->m_pParent->m_pLeft;
			if (pY != NULL && pY->m_eColor == CNode::RB_RED)
			{
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pY->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent->m_pParent;
			}
			else
			{
				if (pX == pX->m_pParent->m_pLeft)
				{
					pX = pX->m_pParent;
					RightRotate(pX);
				}
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				LeftRotate(pX->m_pParent->m_pParent);
			}
		}
	}

	m_pRoot->m_eColor = CNode::RB_BLACK;
	SetNil(&m_pRoot->m_pParent);

	return( pNewNode );
}

#ifdef _DEBUG

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::VerifyIntegrity(
	_In_ const CNode *pNode,
	_In_ int nCurrBlackDepth,
	_Out_ int &nBlackDepth) const throw()
{
	bool bCheckForBlack = false;
	bool bLeaf = true;

	if (pNode->m_eColor == CNode::RB_RED)
		bCheckForBlack = true;
	else
		nCurrBlackDepth++;

	ATLASSERT(pNode->m_pLeft != NULL);
	if (!IsNil(pNode->m_pLeft))
	{
		bLeaf = false;
		if (bCheckForBlack)
		{
			ATLASSERT(pNode->m_pLeft->m_eColor == CNode::RB_BLACK);
		}

		VerifyIntegrity(pNode->m_pLeft, nCurrBlackDepth, nBlackDepth);
	}

	ATLASSERT(pNode->m_pRight != NULL);
	if (!IsNil(pNode->m_pRight))
	{
		bLeaf = false;
		if (bCheckForBlack)
		{
			ATLASSERT(pNode->m_pRight->m_eColor == CNode::RB_BLACK);
		}

		VerifyIntegrity(pNode->m_pRight, nCurrBlackDepth, nBlackDepth);
	}

	ATLASSERT( pNode->m_pParent != NULL );
	ATLASSERT( ( IsNil(pNode->m_pParent) ) ||
			( pNode->m_pParent->m_pLeft == pNode ) ||
			( pNode->m_pParent->m_pRight == pNode ) );

	if (bLeaf)
	{
		if (nBlackDepth == 0)
		{
			nBlackDepth = nCurrBlackDepth;
		}
		else
		{
			ATLASSERT(nBlackDepth == nCurrBlackDepth);
		}
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree< K, V, KTraits, VTraits >::VerifyIntegrity() const throw()
{
	if ((m_pRoot == NULL) || (IsNil(m_pRoot)))
		return;

	ATLASSUME(m_pRoot->m_eColor == CNode::RB_BLACK);
	int nBlackDepth = 0;
	VerifyIntegrity(m_pRoot, 0, nBlackDepth);
}

#endif // _DEBUG

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CRBMap :
	public CRBTree< K, V, KTraits, VTraits >
{
public:
	using typename CRBTree<K, V, KTraits, VTraits>::KINARGTYPE;
	using typename CRBTree<K, V, KTraits, VTraits>::VINARGTYPE;
	using typename CRBTree<K, V, KTraits, VTraits>::VOUTARGTYPE;
	typedef typename CRBTree<K, V, KTraits, VTraits>::CPair CPair;

	explicit CRBMap(_In_ size_t nBlockSize = 10) throw();
	~CRBMap() throw();

	_Success_(return == true) bool Lookup(
		/* _In_ */ KINARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const CPair* Lookup(/* _In_ */ KINARGTYPE key) const throw();
	CPair* Lookup(/* _In_ */ KINARGTYPE key) throw();
	POSITION SetAt(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	bool RemoveKey(/* _In_ */ KINARGTYPE key) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
CRBMap< K, V, KTraits, VTraits >::CRBMap(_In_ size_t nBlockSize) throw() :
	CRBTree< K, V, KTraits, VTraits >( nBlockSize )
{
}

template< typename K, typename V, class KTraits, class VTraits >
CRBMap< K, V, KTraits, VTraits >::~CRBMap() throw()
{
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBMap< K, V, KTraits, VTraits >::CPair* CRBMap< K, V, KTraits, VTraits >::Lookup(
	/* _In_ */ KINARGTYPE key) const throw()
{
	return this->Find(key);
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBMap< K, V, KTraits, VTraits >::CPair* CRBMap< K, V, KTraits, VTraits >::Lookup(
	/* _In_ */ KINARGTYPE key) throw()
{
	return this->Find(key);
}

template< typename K, typename V, class KTraits, class VTraits >
_Success_(return == true) bool CRBMap< K, V, KTraits, VTraits >::Lookup(
	/* _In_ */ KINARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	const CPair* pLookup = this->Find( key );
	if( pLookup == NULL )
		return false;

	value = pLookup->m_value;
	return true;
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBMap< K, V, KTraits, VTraits >::SetAt(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	CPair* pNode = this->Find( key );
	if( pNode == NULL )
	{
		return(this->RBInsert( key, value ) );
	}
	else
	{
		pNode->m_value = value;

		return( pNode );
	}
}

template< typename K, typename V, class KTraits, class VTraits >
bool CRBMap< K, V, KTraits, VTraits >::RemoveKey(/* _In_ */ KINARGTYPE key) throw()
{
	POSITION pos = Lookup( key );
	if( pos != NULL )
	{
		this->RemoveAt( pos );

		return( true );
	}
	else
	{
		return( false );
	}
}

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CRBMultiMap :
	public CRBTree< K, V, KTraits, VTraits >
{
public:
	using typename CRBTree<K, V, KTraits, VTraits>::KINARGTYPE;
	using typename CRBTree<K, V, KTraits, VTraits>::VINARGTYPE;
	typedef typename CRBTree<K, V, KTraits, VTraits>::CPair CPair;

	explicit CRBMultiMap(_In_ size_t nBlockSize = 10) throw();
	~CRBMultiMap() throw();

	POSITION Insert(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	size_t RemoveKey(/* _In_ */ KINARGTYPE key) throw();

	POSITION FindFirstWithKey(/* _In_ */ KINARGTYPE key) const throw();
	const CPair* GetNextWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) const throw();
	CPair* GetNextWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) throw();
	const V& GetNextValueWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) const throw();
	V& GetNextValueWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
CRBMultiMap< K, V, KTraits, VTraits >::CRBMultiMap(_In_ size_t nBlockSize) throw() :
	CRBTree< K, V, KTraits, VTraits >( nBlockSize )
{
}

template< typename K, typename V, class KTraits, class VTraits >
CRBMultiMap< K, V, KTraits, VTraits >::~CRBMultiMap() throw()
{
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBMultiMap< K, V, KTraits, VTraits >::Insert(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	return( this->RBInsert( key, value ) );
}

template< typename K, typename V, class KTraits, class VTraits >
size_t CRBMultiMap< K, V, KTraits, VTraits >::RemoveKey(
	/* _In_ */ KINARGTYPE key) throw()
{
	size_t nElementsDeleted = 0;

	POSITION pos = FindFirstWithKey( key );
	while( pos != NULL )
	{
		POSITION posDelete = pos;
		GetNextWithKey( pos, key );
		this->RemoveAt( posDelete );
		nElementsDeleted++;
	}

	return( nElementsDeleted );
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBMultiMap< K, V, KTraits, VTraits >::FindFirstWithKey(
	/* _In_ */ KINARGTYPE key) const throw()
{
	return( this->Find( key ) );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBMultiMap< K, V, KTraits, VTraits >::CPair* CRBMultiMap< K, V, KTraits, VTraits >::GetNextWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) const throw()
{
	ATLASSERT( pos != NULL );
	const CPair* pNode = this->GetNext( pos );
	if( (pos == NULL) || !KTraits::CompareElements( static_cast< CPair* >( pos )->m_key, key ) )
	{
		pos = NULL;
	}

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBMultiMap< K, V, KTraits, VTraits >::CPair* CRBMultiMap< K, V, KTraits, VTraits >::GetNextWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) throw()
{
	ATLASSERT( pos != NULL );
	CPair* pNode = this->GetNext( pos );
	if( (pos == NULL) || !KTraits::CompareElements( static_cast< CPair* >( pos )->m_key, key ) )
	{
		pos = NULL;
	}

	return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CRBMultiMap< K, V, KTraits, VTraits >::GetNextValueWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) const throw()
{
	const CPair* pPair = GetNextWithKey( pos, key );

	return( pPair->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& CRBMultiMap< K, V, KTraits, VTraits >::GetNextValueWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) throw()
{
	CPair* pPair = GetNextWithKey( pos, key );

	return( pPair->m_value );
}

#pragma pop_macro("new")

}; // namespace ATL
#pragma pack(pop)

#pragma warning(pop)

#endif  // __ATLCOLL_H__
