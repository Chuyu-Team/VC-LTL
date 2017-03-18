// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLIMAGE_H__
#define __ATLIMAGE_H__

#pragma once

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#include <atlbase.h>
#include <atlstr.h>
#include <atlsimpcoll.h>
#include <atltypes.h>

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (push)
#pragma warning(disable : 4820)	// padding added after member
#endif //!_ATL_NO_PRAGMA_WARNINGS

#pragma warning( push, 3 )
#pragma push_macro("new")
#undef new

#pragma warning(push)
#pragma warning(disable:4458)

ATLPREFAST_SUPPRESS(6385)
#include <gdiplus.h>
ATLPREFAST_UNSUPPRESS()

#pragma warning(pop)

#pragma pop_macro("new")
#pragma warning( pop )

#include <shlwapi.h>

#ifndef _ATL_NO_DEFAULT_LIBS
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "msimg32.lib")
#endif  // !_ATL_NO_DEFAULT_LIBS

#pragma pack(push, _ATL_PACKING)

namespace ATL
{

const int CIMAGE_DC_CACHE_SIZE = 4;

class CImage;

#pragma warning(push)
#pragma warning(disable:4512)

class CImageDC
{
public:
	CImageDC(_In_ const CImage& image) throw( ... );
	~CImageDC() throw();

	operator HDC() const throw();

private:
	const CImage& m_image;
	HDC m_hDC;
};

#pragma warning(pop)

class CImage
{
private:
	class CDCCache
	{
	public:
		CDCCache() throw();
		~CDCCache() throw();

		HDC GetDC() throw();
		void ReleaseDC(_In_ HDC) throw();

	private:
		HDC m_ahDCs[CIMAGE_DC_CACHE_SIZE];
	};

	class CInitGDIPlus
	{
	public:
		CInitGDIPlus() throw();
		~CInitGDIPlus() throw();

		bool Init() throw();
		void ReleaseGDIPlus() throw();
		void IncreaseCImageCount() throw();
		void DecreaseCImageCount() throw();

	private:
		ULONG_PTR m_dwToken;
		CRITICAL_SECTION m_sect;
		LONG m_nCImageObjects;
		DWORD m_dwLastError;
	};

public:
	static const DWORD createAlphaChannel = 0x01;

	static const DWORD excludeGIF = 0x01;
	static const DWORD excludeBMP = 0x02;
	static const DWORD excludeEMF = 0x04;
	static const DWORD excludeWMF = 0x08;
	static const DWORD excludeJPEG = 0x10;
	static const DWORD excludePNG = 0x20;
	static const DWORD excludeTIFF = 0x40;
	static const DWORD excludeIcon = 0x80;
	static const DWORD excludeOther = 0x80000000;
	static const DWORD excludeDefaultLoad = 0;
	static const DWORD excludeDefaultSave = excludeIcon|excludeEMF|excludeWMF;
	static const DWORD excludeValid = 0x800000ff;

	enum DIBOrientation
	{
		DIBOR_DEFAULT,
		DIBOR_TOPDOWN,
		DIBOR_BOTTOMUP
	};

public:
	CImage() throw();
	virtual ~CImage() throw();

	operator HBITMAP() const throw();
	BOOL AlphaBlend(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ BYTE bSrcAlpha = 0xff,
		_In_ BYTE bBlendOp = AC_SRC_OVER) const throw();
	BOOL AlphaBlend(
		_In_ HDC hDestDC,
		_In_ const POINT& pointDest,
		_In_ BYTE bSrcAlpha = 0xff,
		_In_ BYTE bBlendOp = AC_SRC_OVER) const throw();
	BOOL AlphaBlend(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ int nSrcWidth,
		_In_ int nSrcHeight,
		_In_ BYTE bSrcAlpha = 0xff,
		_In_ BYTE bBlendOp = AC_SRC_OVER) const throw();
	BOOL AlphaBlend(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ const RECT& rectSrc,
		_In_ BYTE bSrcAlpha = 0xff,
		_In_ BYTE bBlendOp = AC_SRC_OVER) const throw();
	void Attach(
		_In_ HBITMAP hBitmap,
		_In_ DIBOrientation eOrientation = DIBOR_DEFAULT) throw();
	BOOL BitBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL BitBlt(
		_In_ HDC hDestDC,
		_In_ const POINT& pointDest,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL BitBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL BitBlt(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ const POINT& pointSrc,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL Create(
		_In_ int nWidth,
		_In_ int nHeight,
		_In_ int nBPP,
		_In_ DWORD dwFlags = 0) throw();
	BOOL CreateEx(
		_In_ int nWidth,
		_In_ int nHeight,
		_In_ int nBPP,
		_In_ DWORD eCompression,
		_In_reads_opt_(3) const DWORD* pdwBitmasks = NULL,
		_In_ DWORD dwFlags = 0) throw();
	void Destroy() throw();
	HBITMAP Detach() throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ int nSrcWidth,
		_In_ int nSrcHeight) const throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ const RECT& rectSrc) const throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest) const throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ const POINT& pointDest) const throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight ) const throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest) const throw();
	BOOL Draw(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ Gdiplus::InterpolationMode interpolationMode) const throw();
	const void* GetBits() const throw();
	void* GetBits() throw();
	int GetBPP() const throw();
	void GetColorTable(
		_In_ UINT iFirstColor,
		_In_ UINT nColors,
		_In_ RGBQUAD* prgbColors) const throw();
	HDC GetDC() const throw();
	static HRESULT GetExporterFilterString(
		_Inout_ CSimpleString& strExporters,
		_Inout_ CSimpleArray< GUID >& aguidFileTypes,
		_In_opt_z_ LPCTSTR pszAllFilesDescription = NULL,
		_In_ DWORD dwExclude = excludeDefaultSave,
		_In_ TCHAR chSeparator = _T( '|' ) );
	static HRESULT GetImporterFilterString(
		_Inout_ CSimpleString& strImporters,
		_Inout_ CSimpleArray< GUID >& aguidFileTypes,
		_In_opt_z_ LPCTSTR pszAllFilesDescription = NULL,
		_In_ DWORD dwExclude = excludeDefaultLoad,
		_In_ TCHAR chSeparator = _T( '|' ) );
	int GetHeight() const throw();
	int GetMaxColorTableEntries() const throw();
	int GetPitch() const throw();
	const void* GetPixelAddress(
		_In_ int x,
		_In_ int y) const throw();
	void* GetPixelAddress(
		_In_ int x,
		_In_ int y) throw();
	COLORREF GetPixel(
		_In_ int x,
		_In_ int y) const throw();
	LONG GetTransparentColor() const throw();
	int GetWidth() const throw();
	bool IsDIBSection() const throw();
	bool IsIndexed() const throw();
	bool IsNull() const throw();
	HRESULT Load(_In_z_ LPCTSTR pszFileName) throw();
	HRESULT Load(_Inout_ IStream* pStream) throw();
	void LoadFromResource(
		_In_opt_ HINSTANCE hInstance,
		_In_z_ LPCTSTR pszResourceName) throw();
	void LoadFromResource(
		_In_opt_ HINSTANCE hInstance,
		_In_ UINT nIDResource) throw();
	BOOL MaskBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ HBITMAP hbmMask,
		_In_ int xMask,
		_In_ int yMask,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL MaskBlt(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ const POINT& pointSrc,
		_In_ HBITMAP hbmMask,
		_In_ const POINT& pointMask,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL MaskBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ HBITMAP hbmMask,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL MaskBlt(
		_In_ HDC hDestDC,
		_In_ const POINT& pointDest,
		_In_ HBITMAP hbmMask,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL PlgBlt(
		_In_ HDC hDestDC,
		_In_reads_(3) const POINT* pPoints,
		_In_opt_ HBITMAP hbmMask = NULL) const throw();
	BOOL PlgBlt(
		_In_ HDC hDestDC,
		_In_reads_(3) const POINT* pPoints,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ int nSrcWidth,
		_In_ int nSrcHeight,
		_In_opt_ HBITMAP hbmMask = NULL,
		_In_ int xMask = 0,
		_In_ int yMask = 0) const throw();
	BOOL PlgBlt(
		_In_ HDC hDestDC,
		_In_reads_(3) const POINT* pPoints,
		_In_ const RECT& rectSrc,
		_In_opt_ HBITMAP hbmMask = NULL,
		_In_ const POINT& pointMask = CPoint( 0, 0 )) const throw();
	void ReleaseDC() const throw();
	HRESULT Save(
		_Inout_ IStream* pStream,
		_In_ REFGUID guidFileType) const throw();
	HRESULT Save(
		_In_z_ LPCTSTR pszFileName,
		_In_ REFGUID guidFileType = GUID_NULL) const throw();
	void SetColorTable(
		_In_ UINT iFirstColor,
		_In_ UINT nColors,
		_In_ const RGBQUAD* prgbColors) throw();
	void SetPixel(
		_In_ int x,
		_In_ int y,
		_In_ COLORREF color) throw();
	void SetPixelIndexed(
		_In_ int x,
		_In_ int y,
		_In_ int iIndex) throw();
	void SetPixelRGB(
		_In_ int x,
		_In_ int y,
		_In_ BYTE r,
		_In_ BYTE g,
		_In_ BYTE b) throw();
	void SetHasAlphaChannel(_In_ bool bHasAlphaChannel) throw();
	LONG SetTransparentColor(_In_ LONG iTransparentColor) throw();
	COLORREF SetTransparentColor(_In_ COLORREF clrTransparentColor) throw();
	BOOL StretchBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL StretchBlt(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL StretchBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ int nSrcWidth,
		_In_ int nSrcHeight,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL StretchBlt(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ const RECT& rectSrc,
		_In_ DWORD dwROP = SRCCOPY) const throw();
	BOOL TransparentBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ UINT crTransparent = CLR_INVALID) const throw();
	BOOL TransparentBlt(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ UINT crTransparent = CLR_INVALID) const throw();
	BOOL TransparentBlt(
		_In_ HDC hDestDC,
		_In_ int xDest,
		_In_ int yDest,
		_In_ int nDestWidth,
		_In_ int nDestHeight,
		_In_ int xSrc,
		_In_ int ySrc,
		_In_ int nSrcWidth,
		_In_ int nSrcHeight,
		_In_ UINT crTransparent = CLR_INVALID) const throw();
	BOOL TransparentBlt(
		_In_ HDC hDestDC,
		_In_ const RECT& rectDest,
		_In_ const RECT& rectSrc,
		_In_ UINT crTransparent = CLR_INVALID) const throw();

	static BOOL IsTransparencySupported() throw();

private:
	HBITMAP m_hBitmap;
	void* m_pBits;
	int m_nWidth;
	int m_nHeight;
	int m_nPitch;
	int m_nBPP;
	bool m_bIsDIBSection;
	bool m_bHasAlphaChannel;
	LONG m_iTransparentColor;
	COLORREF m_clrTransparentColor;

	static CInitGDIPlus* GetInitGDIPlusInstance()
	{
		static CInitGDIPlus gdiPlus;
		return &gdiPlus;
	}

public:
	inline static void ReleaseGDIPlus()
	{
		GetInitGDIPlusInstance()->ReleaseGDIPlus();
	}


// Implementation
private:
	static CLSID FindCodecForExtension(
		_In_z_ LPCTSTR pszExtension,
		_In_reads_(nCodecs) const Gdiplus::ImageCodecInfo* pCodecs,
		_In_ UINT nCodecs);
	static CLSID FindCodecForFileType(
		_In_ REFGUID guidFileType,
		_In_reads_(nCodecs) const Gdiplus::ImageCodecInfo* pCodecs,
		_In_ UINT nCodecs);
	static void BuildCodecFilterString(
		_In_reads_(nCodecs) const Gdiplus::ImageCodecInfo* pCodecs,
		_In_ UINT nCodecs,
		_Inout_ CSimpleString& strFilter,
		_Inout_ CSimpleArray< GUID >& aguidFileTypes,
		_In_opt_z_ LPCTSTR pszAllFilesDescription,
		_In_ DWORD dwExclude,
		_In_ TCHAR chSeparator);

	static bool ShouldExcludeFormat(
		_In_ REFGUID guidFileType,
		_In_ DWORD dwExclude) throw();
	void UpdateBitmapInfo(_In_ DIBOrientation eOrientation);
	HRESULT CreateFromGdiplusBitmap(_Inout_ Gdiplus::Bitmap& bmSrc) throw();

	static bool InitGDIPlus() throw();

	static int ComputePitch(
		_In_ int nWidth,
		_In_ int nBPP)
	{
		return( (((nWidth*nBPP)+31)/32)*4 );
	}
	COLORREF GetTransparentRGB() const;

private:
	mutable HDC m_hDC;
	mutable int m_nDCRefCount;
	mutable HBITMAP m_hOldBitmap;

	static CDCCache* GetCDCCacheInstance()
	{
		static CDCCache cache;
		return &cache;
	}

	static bool CImageStaticInitialize()
	{
		GetInitGDIPlusInstance();
		GetCDCCacheInstance();
		return true;
	}

	static bool isCImageStaticInitialized;
};

__declspec(selectany) bool CImage::isCImageStaticInitialized = CImage::CImageStaticInitialize();

inline CImageDC::CImageDC(_In_ const CImage& image) throw( ... ) :
	m_image( image ),
	m_hDC( image.GetDC() )
{
	if( m_hDC == NULL )
	{
		AtlThrow( E_OUTOFMEMORY );
	}
}

inline CImageDC::~CImageDC() throw()
{
	m_image.ReleaseDC();
}

inline CImageDC::operator HDC() const throw()
{
	return( m_hDC );
}

inline CImage::CInitGDIPlus::CInitGDIPlus() throw() :
	m_dwToken( 0 ), m_nCImageObjects( 0 ), m_dwLastError(S_OK)
{
	if (!_AtlInitializeCriticalSectionEx(&m_sect, 0, 0))
	{
		m_dwLastError = HRESULT_FROM_WIN32(GetLastError());
	}
}

inline CImage::CInitGDIPlus::~CInitGDIPlus() throw()
{
	ReleaseGDIPlus();
	DeleteCriticalSection(&m_sect);
}

inline bool CImage::CInitGDIPlus::Init() throw()
{
	if (m_dwLastError != S_OK)
	{
		return false;
	}

	EnterCriticalSection(&m_sect);
	bool fRet = true;

	if( m_dwToken == 0 )
	{
		Gdiplus::GdiplusStartupInput input;
		Gdiplus::GdiplusStartupOutput output;
		Gdiplus::Status status = Gdiplus::GdiplusStartup( &m_dwToken, &input, &output );
		if( status != Gdiplus::Ok )
			fRet = false;
	}
	LeaveCriticalSection(&m_sect);
	return fRet;
}

inline void CImage::CInitGDIPlus::ReleaseGDIPlus() throw()
{
	EnterCriticalSection(&m_sect);
	if( m_dwToken != 0 )
	{
		Gdiplus::GdiplusShutdown( m_dwToken );
	}
	m_dwToken = 0;
	LeaveCriticalSection(&m_sect);
}

inline void CImage::CInitGDIPlus::IncreaseCImageCount() throw()
{
	EnterCriticalSection(&m_sect);
	m_nCImageObjects++;
	LeaveCriticalSection(&m_sect);
}

inline void CImage::CInitGDIPlus::DecreaseCImageCount() throw()
{
	EnterCriticalSection(&m_sect);
	if( --m_nCImageObjects == 0 )
		ReleaseGDIPlus();
	LeaveCriticalSection(&m_sect);
}

inline CImage::CDCCache::CDCCache() throw()
{
	int iDC;

	for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		m_ahDCs[iDC] = NULL;
	}
}

inline CImage::CDCCache::~CDCCache() throw()
{
	int iDC;

	for( iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		if( m_ahDCs[iDC] != NULL )
		{
			::DeleteDC( m_ahDCs[iDC] );
		}
	}
}

inline HDC CImage::CDCCache::GetDC() throw()
{
	HDC hDC;

	for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		hDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), NULL ) );
		if( hDC != NULL )
		{
			return( hDC );
		}
	}

	hDC = ::CreateCompatibleDC( NULL );

	return( hDC );
}

inline void CImage::CDCCache::ReleaseDC(_In_ HDC hDC) throw()
{
	for( int iDC = 0; iDC < CIMAGE_DC_CACHE_SIZE; iDC++ )
	{
		HDC hOldDC;

		hOldDC = static_cast< HDC >( InterlockedExchangePointer( reinterpret_cast< void** >(&m_ahDCs[iDC]), hDC ) );
		if( hOldDC == NULL )
		{
			return;
		}
		else
		{
			hDC = hOldDC;
		}
	}
	if( hDC != NULL )
	{
		::DeleteDC( hDC );
	}
}

inline CImage::CImage() throw() :
	m_hBitmap( NULL ),
	m_pBits( NULL ),
	m_hDC( NULL ),
	m_nDCRefCount( 0 ),
	m_hOldBitmap( NULL ),
	m_nWidth( 0 ),
	m_nHeight( 0 ),
	m_nPitch( 0 ),
	m_nBPP( 0 ),
	m_iTransparentColor( -1 ),
	m_clrTransparentColor( (COLORREF)-1 ),
	m_bHasAlphaChannel( false ),
	m_bIsDIBSection( false )
{
	GetInitGDIPlusInstance()->IncreaseCImageCount();
}

inline CImage::~CImage() throw()
{
	Destroy();
	GetInitGDIPlusInstance()->DecreaseCImageCount();
}

inline CImage::operator HBITMAP() const throw()
{
	return( m_hBitmap );
}

inline BOOL CImage::AlphaBlend(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ BYTE bSrcAlpha,
	_In_ BYTE bBlendOp) const throw()
{
	return AlphaBlend( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0,
		m_nWidth, m_nHeight, bSrcAlpha, bBlendOp );
}

inline BOOL CImage::AlphaBlend(
	_In_ HDC hDestDC,
	_In_ const POINT& pointDest,
    _In_ BYTE bSrcAlpha,
	_In_ BYTE bBlendOp) const throw()
{
	return AlphaBlend( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight,
		0, 0, m_nWidth, m_nHeight, bSrcAlpha, bBlendOp );
}

inline BOOL CImage::AlphaBlend(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ int nSrcWidth,
	_In_ int nSrcHeight,
	_In_ BYTE bSrcAlpha,
	_In_ BYTE bBlendOp) const throw()
{
	BLENDFUNCTION blend;
	BOOL bResult;

	blend.SourceConstantAlpha = bSrcAlpha;
	blend.BlendOp = bBlendOp;
	blend.BlendFlags = 0;
	if( m_bHasAlphaChannel )
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
	}
	else
	{
		blend.AlphaFormat = 0;
	}

	GetDC();

	bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC,
		xSrc, ySrc, nSrcWidth, nSrcHeight, blend );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::AlphaBlend(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ const RECT& rectSrc,
	_In_ BYTE bSrcAlpha,
	_In_ BYTE bBlendOp) const throw()
{
	return AlphaBlend(hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top,
		rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, bSrcAlpha,
		bBlendOp);
}
inline void CImage::Attach(
	_In_ HBITMAP hBitmap,
	_In_ DIBOrientation eOrientation) throw()
{
	ATLASSUME( m_hBitmap == NULL );
	ATLASSERT( hBitmap != NULL );

	m_hBitmap = hBitmap;

	UpdateBitmapInfo(eOrientation);
}

inline BOOL CImage::BitBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ DWORD dwROP) const throw()
{
	return BitBlt(hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, dwROP);
}

inline BOOL CImage::BitBlt(
	_In_ HDC hDestDC,
	_In_ const POINT& pointDest,
	_In_ DWORD dwROP) const throw()
{
	return BitBlt(hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 0, dwROP);
}

inline BOOL CImage::BitBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ DWORD dwROP) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLENSURE_RETURN_VAL( hDestDC != NULL, FALSE );

	GetDC();

	BOOL bResult = ::BitBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC,
		xSrc, ySrc, dwROP );

	ReleaseDC();

	return bResult;
}

inline BOOL CImage::BitBlt(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ const POINT& pointSrc,
	_In_ DWORD dwROP) const throw()
{
	return BitBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y,
		dwROP );
}

inline BOOL CImage::Create(
	_In_ int nWidth,
	_In_ int nHeight,
	_In_ int nBPP,
	_In_ DWORD dwFlags) throw()
{
	return( CreateEx( nWidth, nHeight, nBPP, BI_RGB, NULL, dwFlags ) );
}

inline BOOL CImage::CreateEx(
	_In_ int nWidth,
	_In_ int nHeight,
	_In_ int nBPP,
	_In_ DWORD eCompression,
	_In_reads_opt_(3) const DWORD* pdwBitfields,
	_In_ DWORD dwFlags) throw()
{
	USES_ATL_SAFE_ALLOCA;
	LPBITMAPINFO pbmi;
	HBITMAP hBitmap;

	ATLASSERT( (eCompression == BI_RGB) || (eCompression == BI_BITFIELDS) );
	if( dwFlags&createAlphaChannel )
	{
		ATLASSERT( (nBPP == 32) && (eCompression == BI_RGB) );
	}

	pbmi = (LPBITMAPINFO)_ATL_SAFE_ALLOCA(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
	if( pbmi == NULL )
		return FALSE;

	memset( &pbmi->bmiHeader, 0, sizeof( pbmi->bmiHeader ) );
	pbmi->bmiHeader.biSize = sizeof( pbmi->bmiHeader );
	pbmi->bmiHeader.biWidth = nWidth;
	pbmi->bmiHeader.biHeight = nHeight;
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = USHORT( nBPP );
	pbmi->bmiHeader.biCompression = eCompression;
	if( nBPP <= 8 )
	{
		ATLASSERT( eCompression == BI_RGB );
#pragma warning(push)
#pragma warning(disable:4068) //Disable unknown pragma warning that prefast pragma causes.
#pragma prefast(push)
#pragma prefast(disable:203, "no buffer overrun here, buffer was alocated properly")
		memset( pbmi->bmiColors, 0, 256*sizeof( RGBQUAD ) );
#pragma prefast(pop)
#pragma warning(pop)
	}

	else
	{
		if( eCompression == BI_BITFIELDS )
		{
			ATLASSUME( pdwBitfields != NULL );
			Checked::memcpy_s(pbmi->bmiColors, 3*sizeof( DWORD ), pdwBitfields, 3*sizeof( DWORD ));
		}
	}

	hBitmap = ::CreateDIBSection( NULL, pbmi, DIB_RGB_COLORS, &m_pBits, NULL,
		0 );
	if( hBitmap == NULL )
	{
		return( FALSE );
	}

	Attach( hBitmap, (nHeight < 0) ? DIBOR_TOPDOWN : DIBOR_BOTTOMUP );

	if( dwFlags&createAlphaChannel )
	{
		m_bHasAlphaChannel = true;
	}

	return( TRUE );
}

inline void CImage::Destroy() throw()
{
	HBITMAP hBitmap;

	if( m_hBitmap != NULL )
	{
		hBitmap = Detach();
		::DeleteObject( hBitmap );
	}
}

inline HBITMAP CImage::Detach() throw()
{
	HBITMAP hBitmap;

	ATLASSUME( m_hBitmap != NULL );
	ATLASSUME( m_hDC == NULL );

	hBitmap = m_hBitmap;
	m_hBitmap = NULL;
	m_pBits = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBPP = 0;
	m_nPitch = 0;
	m_iTransparentColor = -1;
	m_clrTransparentColor = (COLORREF)-1;
	m_bHasAlphaChannel = false;
	m_bIsDIBSection = false;

	return( hBitmap );
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest) const throw()
{
	return Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth,
		m_nHeight );
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight) const throw()
{
	return Draw(hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0, m_nWidth, m_nHeight);
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ const POINT& pointDest) const throw()
{
	return Draw( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight );
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest) const throw()
{
	return Draw( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight );
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ const RECT& rectSrc) const throw()
{
	return Draw( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top,
		rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top );
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ int nSrcWidth,
	_In_ int nSrcHeight) const throw()
{
	BOOL bResult;

	ATLASSUME( m_hBitmap != NULL );
	ATLENSURE_RETURN_VAL( hDestDC != NULL, FALSE );
	ATLASSERT( nDestWidth > 0 );
	ATLASSERT( nDestHeight > 0 );
	ATLASSERT( nSrcWidth > 0 );
	ATLASSERT( nSrcHeight > 0 );

	GetDC();

	if( ((m_iTransparentColor != -1) || (m_clrTransparentColor != (COLORREF)-1)) && IsTransparencySupported() )
	{
		bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, GetTransparentRGB() );
	}
	else if( m_bHasAlphaChannel && IsTransparencySupported() )
	{
		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		bResult = ::AlphaBlend( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, bf );
	}
	else
	{
		bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
			m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, SRCCOPY );
	}

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::Draw(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ Gdiplus::InterpolationMode interpolationMode) const throw()
{
	if (!InitGDIPlus())
	{
		return FALSE;
	}

	Gdiplus::Bitmap bm(m_hBitmap, NULL);
	if (bm.GetLastStatus() != Gdiplus::Ok)
	{
		return FALSE;
	}

	Gdiplus::Graphics dcDst(hDestDC);
	dcDst.SetInterpolationMode(interpolationMode);

	Gdiplus::Status status = dcDst.DrawImage(&bm, rectDest.left, rectDest.top, rectDest.right - rectDest.left, rectDest.bottom - rectDest.top);

	return status == Gdiplus::Ok;
}

inline const void* CImage::GetBits() const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_pBits );
}

inline void* CImage::GetBits() throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_pBits );
}

inline int CImage::GetBPP() const throw()
{
	ATLASSUME( m_hBitmap != NULL );

	return( m_nBPP );
}

inline void CImage::GetColorTable(
	_In_ UINT iFirstColor,
	_In_ UINT nColors,
	_In_ RGBQUAD* prgbColors) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSUME( m_pBits != NULL );
	ATLASSERT( IsIndexed() );

	GetDC();

	::GetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

	ReleaseDC();
}

inline HDC CImage::GetDC() const throw()
{
	ATLASSUME( m_hBitmap != NULL );

	m_nDCRefCount++;
	if( m_hDC == NULL )
	{
		m_hDC = GetCDCCacheInstance()->GetDC();
		m_hOldBitmap = HBITMAP( ::SelectObject( m_hDC, m_hBitmap ) );
	}

	return( m_hDC );
}

inline bool CImage::ShouldExcludeFormat(
	_In_ REFGUID guidFileType,
	_In_ DWORD dwExclude) throw()
{
	static const GUID* apguidFormats[] =
	{
		&Gdiplus::ImageFormatGIF,
		&Gdiplus::ImageFormatBMP,
		&Gdiplus::ImageFormatEMF,
		&Gdiplus::ImageFormatWMF,
		&Gdiplus::ImageFormatJPEG,
		&Gdiplus::ImageFormatPNG,
		&Gdiplus::ImageFormatTIFF,
		&Gdiplus::ImageFormatIcon,
		NULL
	};

	ATLASSERT( (dwExclude|excludeValid) == excludeValid );
	for( int iFormat = 0; apguidFormats[iFormat] != NULL; iFormat++ )
	{
		if( guidFileType == *apguidFormats[iFormat] )
		{
			return( (dwExclude&(1<<iFormat)) != 0 );
		}
	}

	return( (dwExclude&excludeOther) != 0 );
}

inline void CImage::BuildCodecFilterString(
	_In_reads_(nCodecs) const Gdiplus::ImageCodecInfo* pCodecs,
	_In_ UINT nCodecs,
	_Inout_ CSimpleString& strFilter,
	_Inout_ CSimpleArray< GUID >& aguidFileTypes,
	_In_opt_z_ LPCTSTR pszAllFilesDescription,
	_In_ DWORD dwExclude,
	_In_ TCHAR chSeparator)
{
	if( pszAllFilesDescription != NULL )
	{
		aguidFileTypes.Add( GUID_NULL );
	}

	CString strAllExtensions;
	CString strTempFilter;
	for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
	{
		const Gdiplus::ImageCodecInfo* pCodec = &pCodecs[iCodec];

		if( !ShouldExcludeFormat( pCodec->FormatID, dwExclude ) )
		{
			CW2CT pwszFilenameExtension( pCodec->FilenameExtension );
			strTempFilter += CW2CT( pCodec->FormatDescription );
			strTempFilter += _T( " (" );
			strTempFilter += pwszFilenameExtension;
			strTempFilter += _T( ")" );
			strTempFilter += chSeparator;
			strTempFilter += pwszFilenameExtension;
			strTempFilter += chSeparator;

			aguidFileTypes.Add( pCodec->FormatID );

			if( !strAllExtensions.IsEmpty() )
			{
				strAllExtensions += _T( ";" );
			}
			strAllExtensions += pwszFilenameExtension;
		}
	}

	if( pszAllFilesDescription != NULL )
	{
		strFilter += pszAllFilesDescription;
		strFilter += chSeparator;
		strFilter += strAllExtensions;
		strFilter += chSeparator;
	}
	strFilter += strTempFilter;

	strFilter += chSeparator;
	if( aguidFileTypes.GetSize() == 0 )
	{
		strFilter += chSeparator;
	}
}

inline HRESULT CImage::GetImporterFilterString(
	_Inout_ CSimpleString& strImporters,
	_Inout_ CSimpleArray< GUID >& aguidFileTypes,
	_In_opt_z_ LPCTSTR pszAllFilesDescription /* = NULL */,
	_In_ DWORD dwExclude /* = excludeDefaultLoad */,
	_In_ TCHAR chSeparator /* = '|' */)
{
	if( !InitGDIPlus() )
	{
		return( E_FAIL );
	}

	UINT nCodecs;
	UINT nSize;
	Gdiplus::Status status;
	Gdiplus::ImageCodecInfo* pCodecs;

	status = Gdiplus::GetImageDecodersSize( &nCodecs, &nSize );
    if (status != Gdiplus::Ok)
        return( E_FAIL );
    
	USES_ATL_SAFE_ALLOCA;
	pCodecs = static_cast< Gdiplus::ImageCodecInfo* >( _ATL_SAFE_ALLOCA(nSize, _ATL_SAFE_ALLOCA_DEF_THRESHOLD) );

	if( pCodecs == NULL )
		return E_OUTOFMEMORY;

	status = Gdiplus::GetImageDecoders( nCodecs, nSize, pCodecs );
    if (status != Gdiplus::Ok)
        return( E_FAIL );

	BuildCodecFilterString( pCodecs, nCodecs, strImporters, aguidFileTypes, pszAllFilesDescription, dwExclude, chSeparator );

	return( S_OK );
}

inline HRESULT CImage::GetExporterFilterString(
	_Inout_ CSimpleString& strExporters,
	_Inout_ CSimpleArray< GUID >& aguidFileTypes,
	_In_opt_z_ LPCTSTR pszAllFilesDescription /* = NULL */,
	_In_ DWORD dwExclude /* = excludeDefaultSave */,
	_In_ TCHAR chSeparator /* = '|' */)
{
	if( !InitGDIPlus() )
	{
		return( E_FAIL );
	}

	UINT nCodecs;
	UINT nSize;
	Gdiplus::Status status;
	Gdiplus::ImageCodecInfo* pCodecs;

	status = Gdiplus::GetImageDecodersSize( &nCodecs, &nSize );
    if (status != Gdiplus::Ok)
        return( E_FAIL );

    USES_ATL_SAFE_ALLOCA;
	pCodecs = static_cast< Gdiplus::ImageCodecInfo* >( _ATL_SAFE_ALLOCA(nSize, _ATL_SAFE_ALLOCA_DEF_THRESHOLD) );

	if( pCodecs == NULL )
		return E_OUTOFMEMORY;

	status = Gdiplus::GetImageDecoders( nCodecs, nSize, pCodecs );
    if (status != Gdiplus::Ok)
        return( E_FAIL );

    BuildCodecFilterString( pCodecs, nCodecs, strExporters, aguidFileTypes, pszAllFilesDescription, dwExclude, chSeparator );

	return( S_OK );
}

inline int CImage::GetHeight() const throw()
{
	ATLASSUME( m_hBitmap != NULL );

	return( m_nHeight );
}

inline int CImage::GetMaxColorTableEntries() const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	if( IsIndexed() )
	{
		return( 1<<m_nBPP );
	}
	else
	{
		return( 0 );
	}
}

inline int CImage::GetPitch() const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_nPitch );
}

inline COLORREF CImage::GetPixel(
	_In_ int x,
	_In_ int y) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	GetDC();

	COLORREF clr = ::GetPixel( m_hDC, x, y );

	ReleaseDC();

	return( clr );
}

inline const void* CImage::GetPixelAddress(
	_In_ int x,
	_In_ int y) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
}

inline void* CImage::GetPixelAddress(
	_In_ int x,
	_In_ int y) throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	return( LPBYTE( m_pBits )+(y*m_nPitch)+((x*m_nBPP)/8) );
}

inline LONG CImage::GetTransparentColor() const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( (m_nBPP == 4) || (m_nBPP == 8) );

	return( m_iTransparentColor );
}

inline int CImage::GetWidth() const throw()
{
	ATLASSUME( m_hBitmap != NULL );

	return( m_nWidth );
}

inline bool CImage::IsDIBSection() const throw()
{
	return( m_bIsDIBSection );
}

inline bool CImage::IsIndexed() const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );

	return( m_nBPP <= 8 );
}

inline bool CImage::IsNull() const throw()
{
	return( m_hBitmap == NULL );
}

inline HRESULT CImage::Load(_Inout_ IStream* pStream) throw()
{
	if( !InitGDIPlus() )
	{
		return( E_FAIL );
	}

	Gdiplus::Bitmap bmSrc( pStream );
	if( bmSrc.GetLastStatus() != Gdiplus::Ok )
	{
		return( E_FAIL );
	}

	return( CreateFromGdiplusBitmap( bmSrc ) );
}

inline HRESULT CImage::Load(_In_z_ LPCTSTR pszFileName) throw()
{
	if( !InitGDIPlus() )
	{
		return( E_FAIL );
	}

	Gdiplus::Bitmap bmSrc( (CT2W)pszFileName );
	if( bmSrc.GetLastStatus() != Gdiplus::Ok )
	{
		return( E_FAIL );
	}

	return( CreateFromGdiplusBitmap( bmSrc ) );
}

inline HRESULT CImage::CreateFromGdiplusBitmap(_Inout_ Gdiplus::Bitmap& bmSrc) throw()
{
	Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
	UINT nBPP = 32;
	DWORD dwFlags = 0;
	Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppRGB;
	if( eSrcPixelFormat&PixelFormatGDI )
	{
		nBPP = Gdiplus::GetPixelFormatSize( eSrcPixelFormat );
		eDestPixelFormat = eSrcPixelFormat;
	}
	if( Gdiplus::IsAlphaPixelFormat( eSrcPixelFormat ) )
	{
		nBPP = 32;
		dwFlags |= createAlphaChannel;
		eDestPixelFormat = PixelFormat32bppARGB;
	}

	BOOL bSuccess = Create( bmSrc.GetWidth(), bmSrc.GetHeight(), nBPP, dwFlags );
	if( !bSuccess )
	{
		return( E_FAIL );
	}
	USES_ATL_SAFE_ALLOCA;
	Gdiplus::ColorPalette* pPalette = NULL;
	if( Gdiplus::IsIndexedPixelFormat( eSrcPixelFormat ) )
	{
		UINT nPaletteSize = bmSrc.GetPaletteSize();
		pPalette = static_cast< Gdiplus::ColorPalette* >( _ATL_SAFE_ALLOCA(nPaletteSize, _ATL_SAFE_ALLOCA_DEF_THRESHOLD) );

		if( pPalette == NULL )
			return E_OUTOFMEMORY;

		bmSrc.GetPalette( pPalette, nPaletteSize );

		RGBQUAD argbPalette[256];
		ATLENSURE_RETURN( (pPalette->Count > 0) && (pPalette->Count <= 256) );
		for( UINT iColor = 0; iColor < pPalette->Count; iColor++ )
		{
			Gdiplus::ARGB color = pPalette->Entries[iColor];
			argbPalette[iColor].rgbRed = (BYTE)( (color>>RED_SHIFT) & 0xff );
			argbPalette[iColor].rgbGreen = (BYTE)( (color>>GREEN_SHIFT) & 0xff );
			argbPalette[iColor].rgbBlue = (BYTE)( (color>>BLUE_SHIFT) & 0xff );
			argbPalette[iColor].rgbReserved = 0;
		}

		SetColorTable( 0, pPalette->Count, argbPalette );
	}

	if( eDestPixelFormat == eSrcPixelFormat )
	{
		// The pixel formats are identical, so just memcpy the rows.
		Gdiplus::BitmapData data;
		Gdiplus::Rect rect( 0, 0, GetWidth(), GetHeight() );
		if(bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data )!=Gdiplus::Ok)
		{
			return E_OUTOFMEMORY;
		}

		size_t nBytesPerRow = AtlAlignUp( nBPP*GetWidth(), 8 )/8;
		BYTE* pbDestRow = static_cast< BYTE* >( GetBits() );
		BYTE* pbSrcRow = static_cast< BYTE* >( data.Scan0 );
		for( int y = 0; y < GetHeight(); y++ )
		{
			Checked::memcpy_s(pbDestRow, nBytesPerRow, pbSrcRow, nBytesPerRow);
			pbDestRow += GetPitch();
			pbSrcRow += data.Stride;
		}

		bmSrc.UnlockBits( &data );
	}
	else
	{
		// Let GDI+ work its magic
		Gdiplus::Bitmap bmDest( GetWidth(), GetHeight(), GetPitch(), eDestPixelFormat, static_cast< BYTE* >( GetBits() ) );
		Gdiplus::Graphics gDest( &bmDest );

		gDest.DrawImage( &bmSrc, 0, 0 );
	}

	return( S_OK );
}

inline void CImage::LoadFromResource(
	_In_opt_ HINSTANCE hInstance,
	_In_z_ LPCTSTR pszResourceName) throw()
{
	HBITMAP hBitmap;

	hBitmap = HBITMAP( ::LoadImage( hInstance, pszResourceName, IMAGE_BITMAP, 0,
		0, LR_CREATEDIBSECTION ) );

	Attach( hBitmap );
}

inline void CImage::LoadFromResource(
	_In_opt_ HINSTANCE hInstance,
	_In_ UINT nIDResource) throw()
{
	LoadFromResource( hInstance, MAKEINTRESOURCE( nIDResource ) );
}

inline BOOL CImage::MaskBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nWidth,
	_In_ int nHeight,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ HBITMAP hbmMask,
	_In_ int xMask,
	_In_ int yMask,
	_In_ DWORD dwROP) const throw()
{
	BOOL bResult;

	ATLASSUME( m_hBitmap != NULL );
	ATLENSURE_RETURN_VAL( hDestDC != NULL, FALSE );

	GetDC();

	bResult = ::MaskBlt( hDestDC, xDest, yDest, nWidth, nHeight, m_hDC, xSrc,
		ySrc, hbmMask, xMask, yMask, dwROP );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::MaskBlt(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ const POINT& pointSrc,
	_In_ HBITMAP hbmMask,
	_In_ const POINT& pointMask,
	_In_ DWORD dwROP) const throw()
{
	return MaskBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, pointSrc.x, pointSrc.y,
		hbmMask, pointMask.x, pointMask.y, dwROP );
}

inline BOOL CImage::MaskBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ HBITMAP hbmMask,
	_In_ DWORD dwROP) const throw()
{
	return MaskBlt( hDestDC, xDest, yDest, m_nWidth, m_nHeight, 0, 0, hbmMask,
		0, 0, dwROP );
}

inline BOOL CImage::MaskBlt(
	_In_ HDC hDestDC,
	_In_ const POINT& pointDest,
	_In_ HBITMAP hbmMask,
	_In_ DWORD dwROP) const throw()
{
	return MaskBlt( hDestDC, pointDest.x, pointDest.y, m_nWidth, m_nHeight, 0,
		0, hbmMask, 0, 0, dwROP );
}

inline BOOL CImage::PlgBlt(
	_In_ HDC hDestDC,
	_In_reads_(3) const POINT* pPoints,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ int nSrcWidth,
	_In_ int nSrcHeight,
	_In_opt_ HBITMAP hbmMask,
	_In_ int xMask,
	_In_ int yMask) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLENSURE_RETURN_VAL( hDestDC != NULL, FALSE );

	GetDC();

	BOOL bResult = ::PlgBlt( hDestDC, pPoints, m_hDC, xSrc, ySrc, nSrcWidth,
		nSrcHeight, hbmMask, xMask, yMask );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::PlgBlt(
	_In_ HDC hDestDC,
	_In_reads_(3) const POINT* pPoints,
	_In_ const RECT& rectSrc,
	_In_opt_ HBITMAP hbmMask,
	_In_ const POINT& pointMask) const throw()
{
	return PlgBlt( hDestDC, pPoints, rectSrc.left, rectSrc.top, rectSrc.right-
		rectSrc.left, rectSrc.bottom-rectSrc.top, hbmMask, pointMask.x,
		pointMask.y );
}

inline BOOL CImage::PlgBlt(
	_In_ HDC hDestDC,
	_In_reads_(3) const POINT* pPoints,
	_In_opt_ HBITMAP hbmMask) const throw()
{
	return PlgBlt( hDestDC, pPoints, 0, 0, m_nWidth, m_nHeight, hbmMask, 0, 0 );
}

inline void CImage::ReleaseDC() const throw()
{
	HBITMAP hBitmap;

	ATLASSUME( m_hDC != NULL );

	m_nDCRefCount--;
	if( m_nDCRefCount == 0 )
	{
		hBitmap = HBITMAP( ::SelectObject( m_hDC, m_hOldBitmap ) );
		ATLASSERT( hBitmap == m_hBitmap );
		GetCDCCacheInstance()->ReleaseDC( m_hDC );
		m_hDC = NULL;
	}
}

inline CLSID CImage::FindCodecForExtension(
	_In_z_ LPCTSTR pszExtension,
	_In_reads_(nCodecs) const Gdiplus::ImageCodecInfo* pCodecs,
	_In_ UINT nCodecs)
{
	CT2CW pszExtensionW( pszExtension );

	for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
	{
		CStringW strExtensions( pCodecs[iCodec].FilenameExtension );

		int iStart = 0;
		do
		{
			CStringW strExtension = ::PathFindExtensionW( strExtensions.Tokenize( L";", iStart ) );
			if( iStart != -1 )
			{
				if( strExtension.CompareNoCase( pszExtensionW ) == 0 )
				{
					return( pCodecs[iCodec].Clsid );
				}
			}
		} while( iStart != -1 );
	}

	return( CLSID_NULL );
}

inline CLSID CImage::FindCodecForFileType(
	_In_ REFGUID guidFileType,
	_In_reads_(nCodecs) const Gdiplus::ImageCodecInfo* pCodecs,
	_In_ UINT nCodecs)
{
	for( UINT iCodec = 0; iCodec < nCodecs; iCodec++ )
	{
		if( pCodecs[iCodec].FormatID == guidFileType )
		{
			return( pCodecs[iCodec].Clsid );
		}
	}

	return( CLSID_NULL );
}

inline HRESULT CImage::Save(
	_Inout_ IStream* pStream,
	_In_ REFGUID guidFileType) const throw()
{
	if( !InitGDIPlus() )
	{
		return( E_FAIL );
	}

	UINT nEncoders;
	UINT nBytes;
	Gdiplus::Status status;

	status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
	if( status != Gdiplus::Ok )
	{
		return( E_FAIL );
	}

	USES_ATL_SAFE_ALLOCA;
	Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( _ATL_SAFE_ALLOCA(nBytes, _ATL_SAFE_ALLOCA_DEF_THRESHOLD) );

	if( pEncoders == NULL )
		return E_OUTOFMEMORY;

	status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
	if( status != Gdiplus::Ok )
	{
		return( E_FAIL );
	}

	CLSID clsidEncoder = FindCodecForFileType( guidFileType, pEncoders, nEncoders );
	if( clsidEncoder == CLSID_NULL )
	{
		return( E_FAIL );
	}

	if( m_bHasAlphaChannel )
	{
		ATLASSUME( m_nBPP == 32 );
		Gdiplus::Bitmap bm( m_nWidth, m_nHeight, m_nPitch, PixelFormat32bppARGB, static_cast< BYTE* >( m_pBits ) );
		status = bm.Save( pStream, &clsidEncoder, NULL );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}
	}
	else
	{
		Gdiplus::Bitmap bm( m_hBitmap, NULL );
		status = bm.Save( pStream, &clsidEncoder, NULL );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}
	}

	return( S_OK );
}

inline HRESULT CImage::Save(
	_In_z_ LPCTSTR pszFileName,
	_In_ REFGUID guidFileType) const throw()
{
	if( !InitGDIPlus() )
	{
		return( E_FAIL );
	}

	UINT nEncoders;
	UINT nBytes;
	Gdiplus::Status status;

	status = Gdiplus::GetImageEncodersSize( &nEncoders, &nBytes );
	if( status != Gdiplus::Ok )
	{
		return( E_FAIL );
	}

	USES_CONVERSION_EX;
	Gdiplus::ImageCodecInfo* pEncoders = static_cast< Gdiplus::ImageCodecInfo* >( _ATL_SAFE_ALLOCA(nBytes, _ATL_SAFE_ALLOCA_DEF_THRESHOLD) );

	if( pEncoders == NULL )
		return E_OUTOFMEMORY;

	status = Gdiplus::GetImageEncoders( nEncoders, nBytes, pEncoders );
	if( status != Gdiplus::Ok )
	{
		return( E_FAIL );
	}

	CLSID clsidEncoder = CLSID_NULL;
	if( guidFileType == GUID_NULL )
	{
		// Determine clsid from extension
		clsidEncoder = FindCodecForExtension( ::PathFindExtension( pszFileName ), pEncoders, nEncoders );
	}
	else
	{
		// Determine clsid from file type
		clsidEncoder = FindCodecForFileType( guidFileType, pEncoders, nEncoders );
	}
	if( clsidEncoder == CLSID_NULL )
	{
		return( E_FAIL );
	}

	LPCWSTR pwszFileName = T2CW_EX( pszFileName, _ATL_SAFE_ALLOCA_DEF_THRESHOLD );
#ifndef _UNICODE
	if( pwszFileName == NULL )
		return E_OUTOFMEMORY;
#endif // _UNICODE
	if( m_bHasAlphaChannel )
	{
		ATLASSUME( m_nBPP == 32 );
		Gdiplus::Bitmap bm( m_nWidth, m_nHeight, m_nPitch, PixelFormat32bppARGB, static_cast< BYTE* >( m_pBits ) );
		status = bm.Save( pwszFileName, &clsidEncoder, NULL );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}
	}
	else
	{
		Gdiplus::Bitmap bm( m_hBitmap, NULL );
		status = bm.Save( pwszFileName, &clsidEncoder, NULL );
		if( status != Gdiplus::Ok )
		{
			return( E_FAIL );
		}
	}

	return( S_OK );
}

inline void CImage::SetColorTable(
	_In_ UINT iFirstColor,
	_In_ UINT nColors,
	_In_ const RGBQUAD* prgbColors) throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( IsDIBSection() );
	ATLASSERT( IsIndexed() );

	GetDC();

	::SetDIBColorTable( m_hDC, iFirstColor, nColors, prgbColors );

	ReleaseDC();
}

inline void CImage::SetPixel(
	_In_ int x,
	_In_ int y,
	_In_ COLORREF color) throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( (x >= 0) && (x < m_nWidth) );
	ATLASSERT( (y >= 0) && (y < m_nHeight) );

	GetDC();

	::SetPixel( m_hDC, x, y, color );

	ReleaseDC();
}

inline void CImage::SetPixelIndexed(
	_In_ int x,
	_In_ int y,
	_In_ int iIndex) throw()
{
	SetPixel( x, y, PALETTEINDEX( iIndex ) );
}

inline void CImage::SetPixelRGB(
	_In_ int x,
	_In_ int y,
	_In_ BYTE r,
	_In_ BYTE g,
	_In_ BYTE b) throw()
{
	SetPixel( x, y, RGB( r, g, b ) );
}

inline void CImage::SetHasAlphaChannel(_In_ bool bHasAlphaChannel) throw()
{
	m_bHasAlphaChannel = bHasAlphaChannel;
}

inline LONG CImage::SetTransparentColor(_In_ LONG iTransparentColor) throw()
{
	LONG iOldTransparentColor;

	ATLASSUME( m_hBitmap != NULL );
	ATLASSERT( (m_nBPP == 4) || (m_nBPP == 8) );
	ATLASSERT( iTransparentColor < GetMaxColorTableEntries() );
	ATLASSERT( iTransparentColor >= -1 );

	iOldTransparentColor = m_iTransparentColor;
	m_iTransparentColor = iTransparentColor;

	return( iOldTransparentColor );
}

inline COLORREF CImage::SetTransparentColor(_In_ COLORREF clrTransparentColor) throw()
{
	COLORREF clrOldTransparentColor;

	clrOldTransparentColor = m_clrTransparentColor;
	m_clrTransparentColor = clrTransparentColor;

	return( clrOldTransparentColor );
}

inline BOOL CImage::StretchBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ DWORD dwROP) const throw()
{
	return StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0, 0,
		m_nWidth, m_nHeight, dwROP );
}

inline BOOL CImage::StretchBlt(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ DWORD dwROP) const throw()
{
	return StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, 0, 0, m_nWidth, m_nHeight,
		dwROP );
}

inline BOOL CImage::StretchBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ int nSrcWidth,
	_In_ int nSrcHeight,
	_In_ DWORD dwROP) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLENSURE_RETURN_VAL( hDestDC != NULL, FALSE );

	GetDC();

	BOOL bResult = ::StretchBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, m_hDC,
		xSrc, ySrc, nSrcWidth, nSrcHeight, dwROP );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::StretchBlt(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ const RECT& rectSrc,
	_In_ DWORD dwROP) const throw()
{
	return StretchBlt( hDestDC, rectDest.left, rectDest.top, rectDest.right-
		rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left, rectSrc.top,
		rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top, dwROP );
}

inline BOOL CImage::TransparentBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ UINT crTransparent) const throw()
{
	return TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight, 0,
		0, m_nWidth, m_nHeight, crTransparent );
}

inline BOOL CImage::TransparentBlt(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ UINT crTransparent) const throw()
{
	return TransparentBlt( hDestDC, rectDest.left, rectDest.top,
		rectDest.right-rectDest.left, rectDest.bottom-rectDest.top,
		crTransparent );
}

inline BOOL CImage::TransparentBlt(
	_In_ HDC hDestDC,
	_In_ int xDest,
	_In_ int yDest,
	_In_ int nDestWidth,
	_In_ int nDestHeight,
	_In_ int xSrc,
	_In_ int ySrc,
	_In_ int nSrcWidth,
	_In_ int nSrcHeight,
	_In_ UINT crTransparent) const throw()
{
	ATLASSUME( m_hBitmap != NULL );
	ATLENSURE_RETURN_VAL( hDestDC != NULL, FALSE );

	GetDC();

	if( crTransparent == CLR_INVALID )
	{
		crTransparent = GetTransparentRGB();
	}

	BOOL bResult = ::TransparentBlt( hDestDC, xDest, yDest, nDestWidth, nDestHeight,
		m_hDC, xSrc, ySrc, nSrcWidth, nSrcHeight, crTransparent );

	ReleaseDC();

	return( bResult );
}

inline BOOL CImage::TransparentBlt(
	_In_ HDC hDestDC,
	_In_ const RECT& rectDest,
	_In_ const RECT& rectSrc,
	_In_ UINT crTransparent) const throw()
{
	return TransparentBlt( hDestDC, rectDest.left, rectDest.top,
		rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, rectSrc.left,
		rectSrc.top, rectSrc.right-rectSrc.left, rectSrc.bottom-rectSrc.top,
		crTransparent );
}

inline BOOL CImage::IsTransparencySupported() throw()
{
	return( TRUE );
}

inline void CImage::UpdateBitmapInfo(_In_ DIBOrientation eOrientation)
{
	DIBSECTION dibsection;
	int nBytes;

	nBytes = ::GetObject( m_hBitmap, sizeof( DIBSECTION ), &dibsection );
	if( nBytes == sizeof( DIBSECTION ) )
	{
		m_bIsDIBSection = true;
		m_nWidth = dibsection.dsBmih.biWidth;
		m_nHeight = abs( dibsection.dsBmih.biHeight );
		m_nBPP = dibsection.dsBmih.biBitCount;
		m_nPitch = ComputePitch( m_nWidth, m_nBPP );
		m_pBits = dibsection.dsBm.bmBits;
		if( eOrientation == DIBOR_DEFAULT )
		{
			eOrientation = (dibsection.dsBmih.biHeight > 0) ? DIBOR_BOTTOMUP : DIBOR_TOPDOWN;
		}
		if( eOrientation == DIBOR_BOTTOMUP )
		{
			m_pBits = LPBYTE( m_pBits )+((m_nHeight-1)*m_nPitch);
			m_nPitch = -m_nPitch;
		}
	}
	else
	{
		// Non-DIBSection
		ATLASSERT( nBytes == sizeof( BITMAP ) );
		m_bIsDIBSection = false;
		m_nWidth = dibsection.dsBm.bmWidth;
		m_nHeight = dibsection.dsBm.bmHeight;
		m_nBPP = dibsection.dsBm.bmBitsPixel;
		m_nPitch = 0;
		m_pBits = 0;
	}
	m_iTransparentColor = -1;
	m_bHasAlphaChannel = false;
}

inline COLORREF CImage::GetTransparentRGB() const
{
	RGBQUAD rgb;

	ATLASSUME( m_hDC != NULL );  // Must have a DC
	ATLASSUME( m_iTransparentColor != -1 || m_clrTransparentColor != (COLORREF)-1 );

	if (m_iTransparentColor != -1)
	{
		::GetDIBColorTable( m_hDC, m_iTransparentColor, 1, &rgb );
		return( RGB( rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue ) );
	}
	else
	{
		return m_clrTransparentColor;
	}
}

inline bool CImage::InitGDIPlus() throw()
{
	bool bSuccess = GetInitGDIPlusInstance()->Init();
	return( bSuccess );
}

};  // namespace ATL

#pragma pack(pop)

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning (pop)
#endif

#endif  // __ATLIMAGE_H__
