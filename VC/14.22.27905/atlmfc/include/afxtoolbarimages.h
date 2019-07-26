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

#ifndef __AFXMT_H__
	#include <afxmt.h>
#endif

#ifndef __ATLIMAGE_H__
	#include "atlimage.h"
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

// for custom drawing
typedef struct tagAFXDrawState
{
	HBITMAP hbmMono;
	HBITMAP hbmMonoOld;
	HBITMAP hbmOldGlyphs;
}
CAfxDrawState;

class CMFCToolBarImages : public CObject
{
	friend class CMFCDropDownFrame;
	friend class CMFCImageEditorDialog;
	friend class CMenuImages;
	friend class CPngImage;
	friend class CMFCRibbonCollector;

public:
	CMFCToolBarImages();
	CMFCToolBarImages(BOOL fDelayInitialize);

	void CommonInit(BOOL fDelayInitialize);
	void Initialize();

	virtual ~CMFCToolBarImages();

	enum ImageAlignHorz
	{
		ImageAlignHorzLeft,
		ImageAlignHorzCenter,
		ImageAlignHorzRight,
		ImageAlignHorzStretch
	};

	enum ImageAlignVert
	{
		ImageAlignVertTop,
		ImageAlignVertCenter,
		ImageAlignVertBottom,
		ImageAlignVertStretch
	};

// Operations:
public:
	static BOOL __stdcall Is32BitTransparencySupported();
	static BOOL __stdcall PreMultiplyAlpha(HBITMAP hbmp, BOOL bAutoCheckPremlt);

	BOOL IsValid() const { return (m_hbmImageWell != NULL); }
	BOOL IsReadOnly() const { return m_bReadOnly; }

	HBITMAP GetImageWell() const { return m_hbmImageWell; }
	HBITMAP GetImageWellLight() const { return m_hbmImageLight; }

	BOOL IsUserImagesList() const { return m_bUserImagesList; }

	void SetImageSize(SIZE sizeImage, BOOL bUpdateCount = FALSE)
	{
		m_sizeImage = sizeImage;
		if (bUpdateCount)
		{
			UpdateCount();
		}
	}

	SIZE GetImageSize(BOOL bDest = FALSE) const { return bDest ? m_sizeImageDest : m_sizeImage; }

	int GetCount() const { return m_iCount; }

	int GetResourceOffset(UINT uiResId) const
	{
		int iOffset = -1;
		if (m_mapOrigResOffsets.Lookup(uiResId, iOffset))
		{
			return iOffset;
		}
		return -1;
	}

	COLORREF SetTransparentColor(COLORREF clrTransparent)
	{
		COLORREF clrOld = m_clrTransparent;
		if (clrTransparent != clrOld)
		{
			m_clrTransparent = clrTransparent;
			UpdateInternalImage(0);
			UpdateInternalImage(1);
		}
		return clrOld;
	}

	COLORREF GetTransparentColor() const { return m_clrTransparent; }

	BOOL Load(UINT uiResID, HINSTANCE hinstRes = NULL, BOOL bAdd = FALSE);
	BOOL LoadStr(LPCTSTR lpszResourceName, HINSTANCE hinstRes = NULL, BOOL bAdd = FALSE);

	BOOL Load(LPCTSTR lpszBmpFileName, DWORD nMaxFileSize = 819200);
	BOOL Save(LPCTSTR lpszBmpFileName = NULL);

	void Clear();

	BOOL PrepareDrawImage(CAfxDrawState& ds, CSize sizeImageDest = CSize(0, 0), BOOL bFadeInactive = FALSE);

	BOOL Draw(CDC* pDC, int x, int y, int iImageIndex, BOOL bHilite = FALSE, BOOL bDisabled = FALSE,
		BOOL bIndeterminate = FALSE, BOOL bShadow = FALSE, BOOL bInactive = FALSE, BYTE alphaSrc = 255);

	BOOL DrawEx(CDC* pDC, CRect rect, int iImageIndex, ImageAlignHorz horzAlign = ImageAlignHorzLeft,
		ImageAlignVert vertAlign = ImageAlignVertTop, CRect rectSrc = CRect(0, 0, 0, 0), BYTE alphaSrc = 255);

	void EndDrawImage(CAfxDrawState& ds);

	int AddImage(HBITMAP hbmp, BOOL bSetBitPerPixel = FALSE);
	int AddImage(const CMFCToolBarImages& imageList, int nIndex);
	int AddIcon(HICON hIcon, BOOL bAlphaBlend = FALSE);
	BOOL UpdateImage(int iImage, HBITMAP hbmp);
	BOOL DeleteImage(int iImage);

	/// <summary> Returns current resolution of underlined images.</summary>
	/// <returns> An integer value representing the current resolution of underlined images, in bits per pixel (bpp).</returns>
	int GetBitsPerPixel() const
	{
		return m_nBitsPerPixel;
	}

	HICON ExtractIcon(int nIndex);

	BOOL CreateFromImageList(const CImageList& imageList);

	BOOL CopyTo(CMFCToolBarImages& imageList);
	BOOL CopyImageToClipboard(int iImage);

	BOOL GrayImages(int nGrayImageLuminancePercentage);

	HBITMAP GetMask(int iImage);

	void OnSysColorChange();

	BOOL MapTo3dColors(BOOL bUseRGBQUAD = TRUE, COLORREF clrSrc = (COLORREF)-1, COLORREF clrDest = (COLORREF)-1);
	static COLORREF __stdcall MapToSysColorAlpha(COLORREF color);
	static COLORREF __stdcall MapToSysColor(COLORREF color, BOOL bUseRGBQUAD = TRUE);
	static COLORREF __stdcall MapFromSysColor(COLORREF color, BOOL bUseRGBQUAD = TRUE);
	static void __stdcall FillDitheredRect(CDC* pDC, const CRect& rect);

	static void __stdcall CleanUp();

	static void __stdcall SetDisabledImageAlpha(BYTE nValue)
	{
		ASSERT(nValue > 0);
		ASSERT(nValue <= 255);
		m_nDisabledImageAlpha = nValue;
	}
	static BYTE __stdcall GetDisabledImageAlpha() { return m_nDisabledImageAlpha; }

	static void __stdcall SetFadedImageAlpha(BYTE nValue)
	{
		ASSERT(nValue > 0);
		ASSERT(nValue <= 255);
		m_nFadedImageAlpha = nValue;
	}
	static BYTE __stdcall GetFadedImageAlpha() { return m_nFadedImageAlpha; }

	void SetLightPercentage(int nValue) { m_nLightPercentage = nValue; }
	int GetLightPercentage() const { return m_nLightPercentage; }

	void SetPreMultiplyAutoCheck(BOOL bAuto = TRUE) { m_bAutoCheckPremlt = bAuto; }
	BOOL IsPreMultiplyAutoCheck() const { return m_bAutoCheckPremlt; }

	// RTL support:
	static void __stdcall EnableRTL(BOOL bIsRTL = TRUE);
	static BOOL __stdcall IsRTL() { return m_bIsRTL; }

	BOOL Mirror();
	static BOOL __stdcall MirrorBitmap(HBITMAP& hbmp, int cxImage);

	BOOL MirrorVert();
	static BOOL __stdcall MirrorBitmapVert(HBITMAP& hbmp, int cyImage);

	static HRGN __stdcall CreateRegionFromImage(HBITMAP bmp, COLORREF clrTransparent);

	void AdaptColors(COLORREF clrBase, COLORREF clrTone);
	void SetSingleImage();

	BOOL GetMapTo3DColors() const { return m_bMapTo3DColors; }
	void SetMapTo3DColors(BOOL bMapTo3DColors) { m_bMapTo3DColors = bMapTo3DColors; }

	BOOL GetAlwaysLight() const { return m_bAlwaysLight; }
	void SetAlwaysLight(BOOL bAlwaysLight = TRUE) { m_bAlwaysLight = bAlwaysLight; }

	CRect GetLastImageRect() const { return m_rectLastDraw; }

	static HBITMAP __stdcall Copy(HBITMAP hbmpSrc);

	/// <summary> 
	/// Smoothly resizes underlined images.</summary>
	/// <param name="dblImageScale"> Scale ratio.</param>
	/// <returns> TRUE if resize succeeds; otherwise FALSE.</returns>
	BOOL SmoothResize(double dblImageScale);

	/// <summary>
	/// Returns current scale ratio of underlined images.</summary>
	/// <returns> A value representing current scale ratio.</returns>
	double GetScale() const
	{
		return m_dblScale;
	}

	/// <summary>
	/// Tells whether the underlined images are scaled or not.</summary>
	/// <returns> TRUE if underlined images are scaled; otherwise FALSE.</returns>
	BOOL IsScaled () const
	{
		return GetScale() != 1.0;
	}

	/// <summary>
	/// Converts underlined bitmaps to 32 bpp images.</summary> 
	/// <returns> TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="clrTransparent">Specifies transparent color of underlined bitmaps.</param>
	BOOL ConvertTo32Bits(COLORREF clrTransparent = (COLORREF)-1);

protected:
	static void __stdcall TransparentBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, CDC* pDcSrc,
		int nXSrc, int nYSrc, COLORREF colorTransparent, int nWidthDest = -1, int nHeightDest = -1);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void CreateMask(int iImage, BOOL bHilite, BOOL bHiliteShadow);
	static HBITMAP __stdcall CreateDitherBitmap();
	void UpdateCount();
	BOOL UpdateInternalImage(int nIndex);

	BOOL PreMultiplyAlpha(HBITMAP hbmp);
	void CopyTemp(CMFCToolBarImages& imagesDest);

	static BOOL __stdcall MapBmpTo3dColors(HBITMAP& hBmp, BOOL bUseRGBQUAD = TRUE, COLORREF clrSrc = (COLORREF)-1, COLORREF clrDest = (COLORREF)-1);

// Attributes:
public:
	AFX_IMPORT_DATA static BOOL    m_bDisableTrueColorAlpha;
	AFX_IMPORT_DATA static BOOL    m_bMultiThreaded; // Set to TRUE if images are used in different threads
	AFX_IMPORT_DATA static BOOL    m_bIsDrawOnGlass; // Draw image on Vista Glass area

protected:
	int m_iCount;           // image counter
	int m_nBitsPerPixel;    // Bitmap color depth
	int m_nGrayImageLuminancePercentage;  // Grayed image brightness
	int m_nLightPercentage; // Light image percentage value

	BOOL m_bInitialized;
	BOOL m_bUserImagesList;  // is user-defined images list?
	BOOL m_bModified;        // is image modified?
	BOOL m_bStretch;         // stretch images
	BOOL m_bReadOnly;        // Loaded from read-only file
	BOOL m_bIsTemporary;     // Temporary copy of another image
	BOOL m_bFadeInactive;    // Inactive image will be drawn with the "fading" effect
	BOOL m_bIsGray;          // Image is grayed
	BOOL m_bMapTo3DColors;   // Map image colors to system
	BOOL m_bAlwaysLight;     // Always use light image in all color resolutions
	BOOL m_bAutoCheckPremlt; // Auto-check for 32 bpp images
	BOOL m_bCreateMonoDC;    // Create mono DC in CAfxDrawState

	CDC      m_dcMem;          // DC for the drawing in memory
	CSize    m_sizeImage;      // size of glyph
	CSize    m_sizeImageOriginal;
	CSize    m_sizeImageDest;  // destination size glyph
	CRect    m_rectLastDraw;   // Last drawn image location
	CRect    m_rectSubImage;   // The part of drawn image
	HBITMAP  m_hbmImageWell;   // glyphs only
	HBITMAP  m_hbmImageLight;  // "faded" version (hi-color only)
	HBITMAP  m_hbmImageShadow; // "shadow" version
	CString  m_strUDLPath;     // user-defined images path
	CBitmap  m_bmpMem;         // bitmap for the drawing in memory
	CBitmap* m_pBmpOriginal;
	COLORREF m_clrTransparent; // Transparent color
	COLORREF m_clrTransparentOriginal;
	COLORREF m_clrImageShadow; // Color of the shadow
	double   m_dblScale;

	CList<UINT, UINT>           m_lstOrigResIds;       // original resource ids
	CList<HINSTANCE, HINSTANCE> m_lstOrigResInstances; // original resource instances
	CMap<UINT, UINT, int, int>  m_mapOrigResOffsets;   // original resource offsets

	AFX_IMPORT_DATA static BYTE      m_nDisabledImageAlpha; // Used in the alpha-blending only
	AFX_IMPORT_DATA static BYTE      m_nFadedImageAlpha;    // Used in the alpha-blending only
	AFX_IMPORT_DATA static BOOL      m_bIsRTL;              // RTL languages support

	static CCriticalSection m_CriticalSection; // For multi-thread applications
};

class CPngImage : public CBitmap  
{
// Construction/Destruction
public:
	CPngImage();
	virtual ~CPngImage();

// Attributes:
protected:
	static ATL::CImage* m_pImage;

// Operations:
public:
	BOOL Load (UINT uiResID, HINSTANCE hinstRes = NULL);
	BOOL Load (LPCTSTR lpszResourceName, HINSTANCE hinstRes = NULL);

	BOOL LoadFromFile (LPCTSTR lpszPath);
	BOOL LoadFromBuffer (LPBYTE lpBuffer, UINT uiSize);

	static void __stdcall CleanUp ()
	{
		if (m_pImage != NULL)
		{
			delete m_pImage;
			m_pImage = NULL;
		}
	}
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
