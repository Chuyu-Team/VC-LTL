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

class CDrawingManager : public CObject
{
public:
	CDrawingManager(CDC& dc);
	virtual ~CDrawingManager();

// Operations:
	BOOL DrawShadow(CRect rect,					// Shadow will be draw around this rect
					int nDepth,						// Shadow depth (pixels)
					int iMinBrightness = 100,		// Min. brighttness
					int iMaxBrightness = 50,		// Max. beightness
					CBitmap* pBmpSaveBottom = NULL,	// Bitmap may be saved in 
					CBitmap* pBmpSaveRight = NULL,	// two bitmaps
					COLORREF clrBase =(COLORREF)-1,	// Base color
					BOOL bRightShadow = TRUE);		// Shadow on right side

	BOOL HighlightRect(CRect rect,					// Rectangle to be highlighted
						int nPercentage = -1,		// < 100 - darker, > 100 - lighter, -1 - default light
						COLORREF clrTransparent = (COLORREF)-1,	// Don't change this color
						int nTolerance = 0, COLORREF clrBlend = (COLORREF)-1);

	void FillGradient(CRect rect, COLORREF colorStart, COLORREF colorFinish,
		BOOL bHorz = TRUE, int nStartFlatPercentage = 0, int nEndFlatPercentage = 0);

	void FillGradient2(CRect rect, COLORREF colorStart, COLORREF colorFinish, 
						int nAngle = 0 /* 0 - 360 */);

	void Fill4ColorsGradient(CRect rect, 
						COLORREF colorStart1, COLORREF colorFinish1, 
						COLORREF colorStart2, COLORREF colorFinish2,
						BOOL bHorz = TRUE,
						int nPercentage = 50);	/* 0 - 100 */

	void FillAlpha(const CRect& rect, BYTE bValue = 255);

	BOOL GrayRect(CRect rect,					// Rectangle to be grayed
				int nPercentage = -1,		// < 100 - darker, > 100 - lighter, -1 - default light
				COLORREF clrTransparent = (COLORREF)-1,	// Don't change this color
				COLORREF clrDisabled = (COLORREF)-1);		// Use this color for desaturation

	void MirrorRect(CRect rect, BOOL bHorz = TRUE);

	BOOL DrawGradientRing(CRect rect, COLORREF colorStart, COLORREF colorFinish, COLORREF colorBorder, int nAngle /* 0 - 360 */,
				int nWidth, COLORREF clrFace = (COLORREF) -1);

	void DrawLine(int x1, int y1, int x2, int y2, COLORREF clrLine);
	void DrawLineA(double x1, double y1, double x2, double y2, COLORREF clrLine);
	void DrawEllipse(const CRect& rect, COLORREF clrFill, COLORREF clrLine);
	void DrawRect(const CRect& rect, COLORREF clrFill, COLORREF clrLine);
	void DrawAlpha(CDC* pDstDC, const CRect& rectDst, CDC* pSrcDC, const CRect& rectSrc);

	/// <summary>
	/// Rotates a source DC content inside the given rectangle by +/- 90 degrees</summary>
	/// <param name="rectDest">Destination rectangle</param>
	/// <param name="dcSrc">The source device content.</param>
	/// <param name="bClockWise">TRUE - rotate +90 degrees, FALSE - 90.</param>
	void DrawRotated(CRect rectDest, CDC& dcSrc, BOOL bClockWise);

// Helpers:
	static COLORREF __stdcall PixelAlpha(COLORREF srcPixel, int percent);
	static COLORREF __stdcall PixelAlpha(COLORREF srcPixel, double percentR, double percentG, double percentB);
	static COLORREF __stdcall PixelAlpha(COLORREF srcPixel, COLORREF dstPixel, int percent);
	static void __stdcall SetAlphaPixel(COLORREF* pBits, CRect rect, int x, int y, int percent, int iShadowSize, COLORREF clrBase = (COLORREF)-1, BOOL bIsRight = TRUE);
	static void __stdcall SetPixel(COLORREF* pBits, int cx, int cy, int x, int y, COLORREF color);

// Conversion between the HSL (Hue, Saturation, and Luminosity) and RGB:
	static double __stdcall HuetoRGB(double m1, double m2, double h);
	static BYTE __stdcall HueToRGB(float rm1, float rm2, float rh);
	static COLORREF __stdcall HLStoRGB_ONE(double H, double L, double S);
	static COLORREF __stdcall HLStoRGB_TWO(double H, double L, double S);
	static void __stdcall RGBtoHSL(COLORREF rgb, double *H, double *S, double *L);

// Conversion between the HSV (Hue/Saturation/Value) and RGB:
	static void __stdcall RGBtoHSV(COLORREF rgb, double *H, double *S, double *V);
	static COLORREF __stdcall HSVtoRGB(double H, double S, double V);

	static COLORREF __stdcall SmartMixColors(COLORREF color1, COLORREF color2, double dblLumRatio = 1., int k1 = 1, int k2 = 1);

	/// <summary> Creates an empty 32 bit bitmap.</summary>
	/// <returns> A handle to created bitmap, or NULL, if creation fails.</returns>
	/// <param name="size">Specifies bitmap size.</param>
	/// <param name="pBits">When the function returns contains a pointer to bitmap bits.</param>
	static HBITMAP __stdcall CreateBitmap_32(const CSize& size, void** pBits);

	/// <summary> Creates a 32 bit bitmap from the specified bitmap.</summary>
	/// <returns> A handle to created bitmap, or NULL, if creation fails.</returns>
	/// <param name="bitmap"> A handle to the original bitmap.</param>
	/// <param name="clrTransparent"> An RGB value specifying transparent color of the original bitmap.</param>
	static HBITMAP __stdcall CreateBitmap_32(HBITMAP bitmap, COLORREF clrTransparent = -1);

	static HBITMAP __stdcall PrepareShadowMask(int nDepth, COLORREF clrBase, int iMinBrightness = 0, int iMaxBrightness = 100);


protected:
	void _FillGradient(CRect rect, COLORREF colorStart, COLORREF colorFinish, BOOL bHorz = TRUE, int nStartFlatPercentage = 0, int nEndFlatPercentage = 0);

// Attributes:
protected:
	CDC& m_dc;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
