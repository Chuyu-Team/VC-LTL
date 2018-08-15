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
#include "afxtoolbarimages.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCControlRendererInfo
{
public:
	CMFCControlRendererInfo();
	~CMFCControlRendererInfo();

	CMFCControlRendererInfo(UINT uiBmpResID, const CRect& rtImage, const CRect& rtCorners, const CRect& rtSides = CRect(0, 0, 0, 0), const CRect& rtInner = CRect(0, 0, 0, 0));
	CMFCControlRendererInfo(LPCTSTR lpszBmpResID, const CRect& rtImage, const CRect& rtCorners, const CRect& rtSides = CRect(0, 0, 0, 0), const CRect& rtInner = CRect(0, 0, 0, 0));
	CMFCControlRendererInfo(UINT uiBmpResID, COLORREF clrTransparent, const CRect& rtImage, const CRect& rtCorners, const CRect& rtSides = CRect(0, 0, 0, 0), const CRect& rtInner = CRect(0, 0, 0, 0), BOOL bPreMultiplyCheck = TRUE);
	CMFCControlRendererInfo(LPCTSTR lpszBmpResID, COLORREF clrTransparent, const CRect& rtImage, const CRect& rtCorners, const CRect& rtSides = CRect(0, 0, 0, 0), const CRect& rtInner = CRect(0, 0, 0, 0), BOOL bPreMultiplyCheck = TRUE);
	CMFCControlRendererInfo(const CMFCControlRendererInfo& rSrc);

public:
	void CommonInit();

	LPCTSTR GetResourceID() const;
	void SetResourceID(LPCTSTR lpszBmpResID);

	CMFCControlRendererInfo& operator = (const CMFCControlRendererInfo& rSrc);

public:
	UINT     m_uiBmpResID;
	CString  m_strBmpResID;
	CRect    m_rectImage;
	CRect    m_rectCorners;
	CRect    m_rectSides;
	CRect    m_rectInter;
	COLORREF m_clrTransparent;
	BOOL     m_bPreMultiplyCheck;
};

class CMFCControlRenderer : public CObject
{
	DECLARE_DYNCREATE(CMFCControlRenderer)

public:
	CMFCControlRenderer();
	virtual ~CMFCControlRenderer();

// Operations:
public:
	virtual BOOL Create(const CMFCControlRendererInfo& params, BOOL bFlipvert = FALSE);

	/// <summary> 
	/// Smoothly resizes images.</summary>
	/// <param name="dblScale"> Scale ratio.</param>
	/// <returns> TRUE if resize succeeds; otherwise FALSE.</returns>
	BOOL SmoothResize(double dblScale);

	virtual void Draw(CDC* pDC, CRect rect, UINT index = 0, BYTE alphaSrc = 255);
	virtual void DrawFrame(CDC* pDC, CRect rect, UINT index = 0, BYTE alphaSrc = 255);
	virtual void FillInterior(CDC* pDC, CRect rect, UINT index = 0, BYTE alphaSrc = 255);
	virtual void FillInterior(CDC* pDC, CRect rect, CMFCToolBarImages::ImageAlignHorz horz, CMFCToolBarImages::ImageAlignVert vert, UINT index = 0, BYTE alphaSrc = 255);

	virtual void OnSysColorChange();
	const CMFCControlRendererInfo& GetParams() const { return m_Params; }

	BOOL IsValid() const { return m_Bitmap.IsValid(); }
	BOOL IsMirror() const { return m_bMirror; }

	virtual void Mirror();
	virtual void CleanUp();
	
	int GetImageCount() const
	{
		if (m_Bitmap.GetCount() == 1)
		{
			HBITMAP hBmp = m_Bitmap.GetImageWell();
			BITMAP bmp;

			if (::GetObject(hBmp, sizeof(BITMAP), &bmp) == sizeof(BITMAP))
			{
				return bmp.bmHeight / m_Params.m_rectImage.Height();
			}

			return 0;
		}

		return m_Bitmap.GetCount();
	}

	/// <summary>
	/// Tells whether this control renderer works with scaled (resized) images.</summary>
	/// <returns>Returns TRUE if this control renderer works with resized (scaled) images.</returns>
	BOOL IsScaled() const
	{
		return m_bIsScaled;
	}

// Attributes:
protected:
	CMFCToolBarImages       m_Bitmap;
	CMFCControlRendererInfo m_Params;
	BOOL                    m_bMirror;
	BOOL                    m_bIsScaled;
};

class CMFCShadowRenderer : public CMFCControlRenderer  
{
	DECLARE_DYNCREATE(CMFCShadowRenderer)

public:
	CMFCShadowRenderer();
	virtual ~CMFCShadowRenderer();

// Operations:
public:
	virtual BOOL Create (int nDepth, COLORREF clrBase, int iMinBrightness = 0, int iMaxBrightness = 100);

	virtual void OnSysColorChange ();
	virtual void Draw (CDC* pDC, CRect rect, UINT index = 0, BYTE alphaSrc = 255);
	virtual void DrawFrame (CDC* pDC, CRect rect, UINT index = 0, BYTE alphaSrc = 255);

	
protected:
	virtual BOOL Create (const CMFCControlRendererInfo& params, BOOL bFlipvert = FALSE);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
