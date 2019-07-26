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

#pragma warning(push)
#pragma warning(disable:4458)

// This header resets packing to the default packing; we must defend ourselves
// against this.
#pragma pack(push)
#include <d3d9types.h>
#pragma pack(pop)

#include <dxgitype.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma warning(pop)

class CRenderTarget;
class CBitmapRenderTarget;
class CHwndRenderTarget;

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/// <summary>
/// D2D1_POINT_2F wrapper
/// </summary>
class CD2DPointF : public D2D1_POINT_2F
{
public:
	/// <summary>
	/// Constructs a CD2DPointF object from CPoint object.</summary>
	/// <param name="pt">source point</param>
	CD2DPointF(const CPoint& pt);

	/// <summary>
	/// Constructs a CD2DPointF object from D2D1_POINT_2F object.</summary>
	/// <param name="pt">source point</param>
	CD2DPointF(const D2D1_POINT_2F& pt);

	/// <summary>
	/// Constructs a CD2DPointF object from D2D1_POINT_2F object.</summary>
	/// <param name="pt">source point</param>
	CD2DPointF(const D2D1_POINT_2F* pt);

	/// <summary>
	/// Constructs a CD2DPointF object from two FLOAT values.</summary>
	/// <param name="fX">source X</param>
	/// <param name="fY">source Y</param>
	CD2DPointF(FLOAT fX = 0., FLOAT fY = 0.);

	/// <summary>
	/// Converts CD2DPointF to CPoint object.</summary>
	/// <returns>
	/// Current value of D2D point.
	/// </returns>
	operator CPoint() { return CPoint((LONG)x, (LONG)y); }
};

/// <summary>
/// D2D1_POINT_2U wrapper
/// </summary>
class CD2DPointU : public D2D1_POINT_2U
{
public:
	/// <summary>
	/// Constructs a CD2DPointU object from CPoint object.</summary>
	/// <param name="pt">source point</param>
	CD2DPointU(const CPoint& pt);

	/// <summary>
	/// Constructs a CD2DPointU from object D2D1_POINT_2U object.</summary>
	/// <param name="pt">source point</param>
	CD2DPointU(const D2D1_POINT_2U& pt);

	/// <summary>
	/// Constructs a CD2DPointU from object D2D1_POINT_2U object.</summary>
	/// <param name="pt">source point</param>
	CD2DPointU(const D2D1_POINT_2U* pt);

	/// <summary>
	/// Constructs a CD2DPointU object from two UINT32 values.</summary>
	/// <param name="uX">source X</param>
	/// <param name="uY">source Y</param>
	CD2DPointU(UINT32 uX = 0, UINT32 uY = 0);

	/// <summary>
	/// Converts CD2DPointU to CPoint object.</summary>
	/// <returns>
	/// Current value of D2D point.
	/// </returns>
	operator CPoint() { return CPoint((LONG)x, (LONG)y); }
};

/// <summary>
/// D2D1_SIZE_F wrapper
/// </summary>
class CD2DSizeF : public D2D1_SIZE_F
{
public:
	/// <summary>
	/// Constructs a CD2DSizeF object from CSize object.</summary>
	/// <param name="size">source size</param>
	CD2DSizeF(const CSize& size);

	/// <summary>
	/// Constructs a CD2DSizeF object from D2D1_SIZE_F object.</summary>
	/// <param name="size">source size</param>
	CD2DSizeF(const D2D1_SIZE_F& size);

	/// <summary>
	/// Constructs a CD2DSizeF object from D2D1_SIZE_F object.</summary>
	/// <param name="size">source size</param>
	CD2DSizeF(const D2D1_SIZE_F* size);

	/// <summary>
	/// Constructs a CD2DSizeF object from two FLOAT values.</summary>
	/// <param name="cx">source width</param>
	/// <param name="cy">source height</param>
	CD2DSizeF(FLOAT cx = 0., FLOAT cy = 0.);

	/// <summary>
	/// Returns a Boolean value that indicates whether an expression contains no valid data (Null).</summary>
	/// <returns>
	/// TRUE if width and height are empty; otherwise FALSE.
	/// </returns>
	BOOL IsNull() const { return width == 0. && height == 0.; }

	/// <summary>
	/// Converts CD2DSizeF to CSize object.</summary>
	/// <returns>
	/// Current value of D2D size.
	/// </returns>
	operator CSize() { return CSize((LONG)width, (LONG)height); }
};

/// <summary>
/// D2D1_SIZE_U wrapper
/// </summary>
class CD2DSizeU : public D2D1_SIZE_U
{
public:
	/// <summary>
	/// Constructs a CD2DSizeU object from CSize object.</summary>
	/// <param name="size">source size</param>
	CD2DSizeU(const CSize& size);

	/// <summary>
	/// Constructs a CD2DSizeU object from D2D1_SIZE_U object.</summary>
	/// <param name="size">source size</param>
	CD2DSizeU(const D2D1_SIZE_U& size);

	/// <summary>
	/// Constructs a CD2DSizeU object from D2D1_SIZE_U object.</summary>
	/// <param name="size">source size</param>
	CD2DSizeU(const D2D1_SIZE_U* size);

	/// <summary>
	/// Constructs a CD2DSizeU object from two UINT32 values.</summary>
	/// <param name="cx">source width</param>
	/// <param name="cy">source height</param>
	CD2DSizeU(UINT32 cx = 0, UINT32 cy = 0);

	/// <summary>
	/// Returns a Boolean value that indicates whether an expression contains no valid data (Null).</summary>
	/// <returns>
	/// TRUE if width and height are empty; otherwise FALSE.
	/// </returns>
	BOOL IsNull() const { return width == 0 && height == 0; }

	/// <summary>
	/// Converts CD2DSizeU to CSize object.</summary>
	/// <returns>
	/// Current value of D2D size.
	/// </returns>
	operator CSize() { return CSize((LONG)width, (LONG)height); }
};

/// <summary>
/// D2D1_RECT_F wrapper
/// </summary>
class CD2DRectF : public D2D1_RECT_F
{
public:
	/// <summary>
	/// Constructs a CD2DRectF object from CRect object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DRectF(const CRect& rect);

	/// <summary>
	/// Constructs a CD2DRectF object from D2D1_RECT_F object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DRectF(const D2D1_RECT_F& rect);

	/// <summary>
	/// Constructs a CD2DRectF object from D2D1_RECT_F object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DRectF(const D2D1_RECT_F* rect);

	/// <summary>
	/// Constructs a CD2DRectF object from four FLOAT values.</summary>
	/// <param name="fLeft">source left coordinate</param>
	/// <param name="fTop">source top coordinate</param>
	/// <param name="fRight">source right coordinate</param>
	/// <param name="fBottom">source bottom coordinate</param>
	CD2DRectF(FLOAT fLeft = 0., FLOAT fTop = 0., FLOAT fRight = 0., FLOAT fBottom = 0.);

	/// <summary>
	/// Returns a Boolean value that indicates whether an expression contains no valid data (Null).</summary>
	/// <returns>
	/// TRUE if rectangle's top, left, bottom, and right values are all equal to 0; otherwise FALSE.
	/// </returns>
	BOOL IsNull() const { return left == 0. && right == 0. && top == 0. && bottom == 0.; }

	/// <summary>
	/// Converts CD2DRectF to CRect object.</summary>
	/// <returns>
	/// Current value of D2D rectangle.
	/// </returns>
	operator CRect() { return CRect((LONG)left, (LONG)top, (LONG)right, (LONG)bottom); }
};

/// <summary>
/// D2D1_RECT_U wrapper
/// </summary>
class CD2DRectU : public D2D1_RECT_U
{
public:
	/// <summary>
	/// Constructs a CD2DRectU object from CRect object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DRectU(const CRect& rect);

	/// <summary>
	/// Constructs a CD2DRectU object from D2D1_RECT_U object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DRectU(const D2D1_RECT_U& rect);

	/// <summary>
	/// Constructs a CD2DRectU object from D2D1_RECT_U object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DRectU(const D2D1_RECT_U* rect);

	/// <summary>
	/// Constructs a CD2DRectU object from four UINT32 values.</summary>
	/// <param name="uLeft">source left coordinate</param>
	/// <param name="uTop">source top coordinate</param>
	/// <param name="uRight">source right coordinate</param>
	/// <param name="uBottom">source bottom coordinate</param>
	CD2DRectU(UINT32 uLeft = 0, UINT32 uTop = 0, UINT32 uRight = 0, UINT32 uBottom = 0);

	/// <summary>
	/// Returns a Boolean value that indicates whether an expression contains no valid data (Null).</summary>
	/// <returns>
	/// TRUE if rectangle's top, left, bottom, and right values are all equal to 0; otherwise FALSE.
	/// </returns>
	BOOL IsNull() const { return left == 0 && right == 0 && top == 0 && bottom == 0; }

	/// <summary>
	/// Converts CD2DRectU to CRect object.</summary>
	/// <returns>
	/// Current value of D2D rectangle.
	/// </returns>
	operator CRect() { return CRect((LONG)left, (LONG)top, (LONG)right, (LONG)bottom); }
};

/// <summary>
/// D2D1_ROUNDED_RECT wrapper
/// </summary>
class CD2DRoundedRect : public D2D1_ROUNDED_RECT
{
public:
	/// <summary>
	/// Constructs a CD2DRoundedRect object from CD2DRectF object.</summary>
	/// <param name="rectIn">source rectangle</param>
	/// <param name="sizeRadius">radius size</param>
	CD2DRoundedRect(const CD2DRectF& rectIn, const CD2DSizeF& sizeRadius);

	/// <summary>
	/// Constructs a CD2DRoundedRect object from D2D1_ROUNDED_RECT object.</summary>
	/// <param name="rectIn">source rectangle</param>
	CD2DRoundedRect(const D2D1_ROUNDED_RECT& rectIn);

	/// <summary>
	/// Constructs a CD2DRoundedRect object from D2D1_ROUNDED_RECT object.</summary>
	/// <param name="rectIn">source rectangle</param>
	CD2DRoundedRect(const D2D1_ROUNDED_RECT* rectIn);
};

/// <summary>
/// D2D1_ELLIPSE wrapper
/// </summary>
class CD2DEllipse : public D2D1_ELLIPSE
{
public:
	/// <summary>
	/// Constructs a CD2DEllipse object from CD2DRectF object.</summary>
	/// <param name="rect">source rectangle</param>
	CD2DEllipse(const CD2DRectF& rect);

	/// <summary>
	/// Constructs a CD2DEllipse object from D2D1_ELLIPSE object.</summary>
	/// <param name="ellipse">source ellipse</param>
	CD2DEllipse(const D2D1_ELLIPSE& ellipse);

	/// <summary>
	/// Constructs a CD2DEllipse object from D2D1_ELLIPSE object.</summary>
	/// <param name="ellipse">source ellipse</param>
	CD2DEllipse(const D2D1_ELLIPSE* ellipse);

	/// <summary>
	/// Constructs a CD2DEllipse from CD2DPointF and CD2DSizeF objects.</summary>
	/// <param name="ptCenter">The center point of the ellipse.</param>
	/// <param name="sizeRadius">The X-radius and Y-radius of the ellipse.</param>
	CD2DEllipse(const CD2DPointF& ptCenter, const CD2DSizeF& sizeRadius);
};

/// <summary>
/// An abstract class, which provides a interface for creating and managing D2D resources such as brushes, layers and texts.
/// </summary>
class CD2DResource : public CObject
{
	DECLARE_DYNAMIC(CD2DResource)

	friend class CRenderTarget;
	friend class CHwndRenderTarget;

protected:
	/// <summary>
	/// Constructs a CD2DResource object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DResource(CRenderTarget* pParentTarget, BOOL bAutoDestroy);

	/// <summary>
	/// The destructor. Called when a D2D resource object is being destroyed.
	/// </summary>
	virtual ~CD2DResource();

	/// <summary>
	/// Check auto destroy flag.</summary>
	/// <returns>
	/// TRUE if the object will be destroyed by its owner; otherwise FALSE.</returns>
	BOOL IsAutoDestroy() const { return m_bIsAutoDestroy; }

	/// <summary>
	/// Re-creates a CD2DResource.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT ReCreate(CRenderTarget* pRenderTarget);

public:
	/// <summary>
	/// Creates a CD2DResource.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget) = 0;

	/// <summary>
	/// Destroys a CD2DResource object.</summary>
	virtual void Destroy() = 0;

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const = 0;

protected:
	/// <summary>
	/// Resource will be destoyed by owner (CRenderTarget)</summary>
	BOOL m_bIsAutoDestroy;

	/// <summary>
	/// Pointer to the parent CRenderTarget)</summary>
	CRenderTarget* m_pParentTarget;
};

/// <summary>
/// ID2D1Layer wrapper.
/// </summary>
class CD2DLayer : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DLayer)

public:
	/// <summary>
	/// Constructs a CD2DLayer object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DLayer(CRenderTarget* pParentTarget, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D layer object is being destroyed.
	/// </summary>
	virtual ~CD2DLayer();

	/// <summary>
	/// Returns the size of the render target in device-independent pixels</summary>
	/// <returns> 
	/// The current size of the render target in device-independent pixels</returns>
	CD2DSizeF GetSize() const;

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pLayer != NULL; }

	/// <summary>
	/// Returns ID2D1Layer interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Layer interface or NULL if object is not initialized yet.</returns>
	ID2D1Layer* Get() { return m_pLayer; }

	/// <summary>
	/// Returns ID2D1Layer interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Layer interface or NULL if object is not initialized yet.</returns>
	operator ID2D1Layer* () { return m_pLayer; }

	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1Layer* pResource);

	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1Layer* Detach();

public:
	/// <summary>
	/// Creates a CD2DLayer.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DLayer object.</summary>
	virtual void Destroy();

protected:
	/// <summary>
	/// Stores a pointer to an ID2D1Layer object.
	/// </summary>
	ID2D1Layer* m_pLayer;
};

/// <summary>
/// ID2D1Bitmap wrapper.
/// </summary>
class CD2DBitmap : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DBitmap)

	friend class CRenderTarget;
	friend class CD2DBitmapBrush;
	friend class CBitmapRenderTarget;

public:
	/// <summary>
	/// Constructs a CD2DBitmap object from resource.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="uiResID">The resource ID number of the resource.</param>
	/// <param name="lpszType">Pointer to a null-terminated string that contains the resource type.</param>
	/// <param name="sizeDest">Destination size of the bitmap.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmap(CRenderTarget* pParentTarget, UINT uiResID, LPCTSTR lpszType = NULL, CD2DSizeU sizeDest = CD2DSizeU(0, 0), BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// Constructs a CD2DBitmap object from file.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="lpszPath">Pointer to a null-terminated string that contains the name of file.</param>
	/// <param name="sizeDest">Destination size of the bitmap.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmap(CRenderTarget* pParentTarget, LPCTSTR lpszPath, CD2DSizeU sizeDest = CD2DSizeU(0, 0), BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// Constructs a CD2DBitmap object from HBITMAP.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="hbmpSrc">Handle to the bitmap.</param>
	/// <param name="sizeDest">Destination size of the bitmap.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmap(CRenderTarget* pParentTarget, HBITMAP hbmpSrc, CD2DSizeU sizeDest = CD2DSizeU(0, 0), BOOL bAutoDestroy = TRUE);

protected:
	/// <summary>
	/// Constructs a CD2DBitmap object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmap(CRenderTarget* pParentTarget, BOOL bAutoDestroy = TRUE);

public:
	/// <summary>
	/// The destructor. Called when a D2D bitmap object is being destroyed.
	/// </summary>
	virtual ~CD2DBitmap();
	
	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1Bitmap* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1Bitmap* Detach();

	/// <summary>
	/// Returns the size, in device-independent pixels (DIPs), of the bitmap</summary>
	/// <returns> 
	/// The size, in DIPs, of the bitmap.</returns>
	CD2DSizeF GetSize() const;

	/// <summary>
	/// Returns the size, in device-dependent units (pixels), of the bitmap</summary>
	/// <returns> 
	/// The size, in pixels, of the bitmap..</returns>
	CD2DSizeU GetPixelSize() const;

	/// <summary>
	/// Retrieves the pixel format and alpha mode of the bitmap</summary>
	/// <returns> 
	/// The pixel format and alpha mode of the bitmap.</returns>
	D2D1_PIXEL_FORMAT GetPixelFormat() const;
	
	/// <summary>
	/// Return the dots per inch (DPI) of the bitmap</summary>
	/// <returns> 
	/// The horizontal and vertical DPI of the bitmap.</returns>
	CD2DSizeF GetDPI() const;
	
	/// <summary>
	/// Copies the specified region from the specified bitmap into the current bitmap</summary>
	/// <returns> 
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pBitmap">The bitmap to copy from</param>
	/// <param name="destPoint">In the current bitmap, the upper-left corner of the area to which the region specified by srcRect is copied</param>
	/// <param name="srcRect">The area of bitmap to copy</param>
	HRESULT CopyFromBitmap(const CD2DBitmap* pBitmap, const CD2DPointU* destPoint = NULL, const CD2DRectU* srcRect = NULL);
	
	/// <summary>
	/// Copies the specified region from the specified render target into the current bitmap</summary>
	/// <returns> 
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">The render target that contains the region to copy</param>
	/// <param name="destPoint">In the current bitmap, the upper-left corner of the area to which the region specified by srcRect is copied</param>
	/// <param name="srcRect">The area of renderTarget to copy</param>
	HRESULT CopyFromRenderTarget(const CRenderTarget* pRenderTarget, const CD2DPointU* destPoint = NULL, const CD2DRectU* srcRect = NULL);
	
	/// <summary>
	/// Copies the specified region from memory into the current bitmap</summary>
	/// <returns> 
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="srcData">The data to copy</param>
	/// <param name="pitch">The stride, or pitch, of the source bitmap stored in srcData. The stride is the byte count of a scanline (one row of pixels in memory). The stride can be computed from the following formula: pixel width * bytes per pixel + memory padding</param>
	/// <param name="destRect">In the current bitmap, the upper-left corner of the area to which the region specified by srcRect is copied</param>
	HRESULT CopyFromMemory(const void *srcData, UINT32 pitch, const CD2DRectU* destRect = NULL);

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pBitmap != NULL; }

	/// <summary>
	/// Returns ID2D1Bitmap interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Bitmap interface or NULL if object is not initialized yet.</returns>
	ID2D1Bitmap* Get() { return m_pBitmap; }

	/// <summary>
	/// Returns ID2D1Bitmap interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Bitmap interface or NULL if object is not initialized yet.</returns>
	operator ID2D1Bitmap*() { return m_pBitmap; }

protected:
	/// <summary>
	/// Initializes the object</summary>
	void CommonInit();

public:
	/// <summary>
	/// Creates a CD2DBitmap.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DBitmap object.</summary>
	virtual void Destroy();

protected:
	/// <summary>
	/// Stores a pointer to an ID2D1Bitmap object.
	/// </summary>
	ID2D1Bitmap*		m_pBitmap;

	/// <summary>
	/// Bitmap resource ID.
	/// </summary>
	UINT				m_uiResID;

	/// <summary>
	/// Resource type.
	/// </summary>
	LPCTSTR				m_lpszType;

	/// <summary>
	/// Botmap file path.
	/// </summary>
	CString				m_strPath;

	/// <summary>
	/// Bitmap destination size.
	/// </summary>
	CD2DSizeU			m_sizeDest;
	
	/// <summary>
	/// Source bitmap handle.
	/// </summary>
	HBITMAP				m_hBmpSrc;

	/// <summary>
	/// TRUE if m_hBmpSrc should be destroyed; otherwise FALSE.
	/// </summary>
	BOOL				m_bAutoDestroyHBMP;
};

/// <summary>
/// D2D1_BRUSH_PROPERTIES wrapper.
/// </summary>
class CD2DBrushProperties : public D2D1_BRUSH_PROPERTIES
{
public:
	/// <summary>
	/// Creates a CD2D_BRUSH_PROPERTIES structure</summary>
	CD2DBrushProperties();

	/// <summary>
	/// Creates a CD2D_BRUSH_PROPERTIES structure</summary>
	/// <param name="_opacity">The base opacity of the brush. The default value is 1.0.</param>
	CD2DBrushProperties(FLOAT _opacity);

	/// <summary>
	/// Creates a CD2D_BRUSH_PROPERTIES structure</summary>
	/// <param name="_transform">The transformation to apply to the brush</param>
	/// <param name="_opacity">The base opacity of the brush. The default value is 1.0.</param>
	CD2DBrushProperties(D2D1_MATRIX_3X2_F _transform, FLOAT _opacity = 1.);

protected:
	/// <summary>
	/// Initializes the object</summary>
	void CommonInit();
};

/// <summary>
/// ID2D1Brush wrapper.
/// </summary>
class CD2DBrush : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DBrush)

	friend class CRenderTarget;

protected:
	/// <summary>
	/// Constructs a CD2DBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBrush(CRenderTarget* pParentTarget, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D brush object is being destroyed.
	/// </summary>
	virtual ~CD2DBrush();

public:
	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1Brush* pResource);

	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1Brush* Detach();

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pBrush != NULL; }

	/// <summary>
	/// Returns ID2D1Brush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Brush interface or NULL if object is not initialized yet.</returns>
	ID2D1Brush* Get() { return m_pBrush; }

	/// <summary>
	/// Returns ID2D1Brush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Brush interface or NULL if object is not initialized yet.</returns>
	operator ID2D1Brush*() { return m_pBrush; }

public:
	/// <summary>
	/// Destroys a CD2DBrush object.</summary>
	virtual void Destroy();

	/// <summary>
	/// Sets the degree of opacity of this brush</summary>
	/// <param name="opacity">A value between zero and 1 that indicates the opacity of the brush. This value is a constant multiplier that linearly scales the alpha value of all pixels filled by the brush. The opacity values are clamped in the range 0 to 1 before they are multiplied together</param>
	void SetOpacity(FLOAT opacity);
	
	/// <summary>
	/// Gets the degree of opacity of this brush</summary>
	/// <returns> 
	/// A value between zero and 1 that indicates the opacity of the brush. This value is a constant multiplier that linearly scales the alpha value of all pixels filled by the brush. The opacity values are clamped in the range 0 to 1 before they are multiplied together</returns>
	FLOAT GetOpacity() const;

	/// <summary>
	/// Applies the specified transform to the render target, replacing the existing transformation. All subsequent drawing operations occur in the transformed space</summary>
	/// <param name="transform">The transform to apply to the render target</param>
	void SetTransform(const D2D1_MATRIX_3X2_F *transform);
	
	/// <summary>
	/// Gets the current transform of the render target</summary>
	/// <param name="transform">When this returns, contains the current transform of the render target. This parameter is passed uninitialized</param>
	void GetTransform(D2D1_MATRIX_3X2_F *transform) const;

protected:
	/// <summary>
	/// Stores a pointer to an ID2D1Brush object.
	/// </summary>
	ID2D1Brush*			m_pBrush;

	/// <summary>
	/// Brush properties.
	/// </summary>
	CD2DBrushProperties* m_pBrushProperties;
};

/// <summary>
/// ID2D1SolidColorBrush wrapper.
/// </summary>
class CD2DSolidColorBrush : public CD2DBrush
{
	DECLARE_DYNAMIC(CD2DSolidColorBrush)

public:
	/// <summary>
	/// Constructs a CD2DSolidColorBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="color">The red, green, blue, and alpha values of the brush's color.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DSolidColorBrush(CRenderTarget* pParentTarget, D2D1_COLOR_F color, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);
	
	/// <summary>
	/// Constructs a CD2DSolidColorBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="color">The red, green, and blue values of the brush's color.</param>
	/// <param name="nAlpha">The opacity of the brush's color.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DSolidColorBrush(CRenderTarget* pParentTarget, COLORREF color, int nAlpha = 255, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D solid brush object is being destroyed.
	/// </summary>
	virtual ~CD2DSolidColorBrush();

public:
	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1SolidColorBrush* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1SolidColorBrush* Detach();

	/// <summary>
	/// Returns ID2D1SolidColorBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1SolidColorBrush interface or NULL if object is not initialized yet.</returns>
	ID2D1SolidColorBrush* Get() { return m_pSolidColorBrush; }

	/// <summary>
	/// Returns ID2D1SolidColorBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1SolidColorBrush interface or NULL if object is not initialized yet.</returns>
	operator ID2D1SolidColorBrush*() { return m_pSolidColorBrush; }

	/// <summary>
	/// Specifies the color of this solid color brush</summary>
	/// <param name="color">The color of this solid color brush</param>
	void SetColor(D2D1_COLOR_F color);
	
	/// <summary>
	/// Retrieves the color of the solid color brush</summary>
	/// <returns> 
	/// The color of this solid color brush</returns>
	D2D1_COLOR_F GetColor() const;

public:
	/// <summary>
	/// Creates a CD2DSolidColorBrush.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DSolidColorBrush object.</summary>
	virtual void Destroy();

protected:
	/// <summary>
	/// Stores a pointer to an ID2D1SolidColorBrush object.
	/// </summary>
	ID2D1SolidColorBrush*	m_pSolidColorBrush;

	/// <summary>
	/// Brush solid color.
	/// </summary>
	D2D1_COLOR_F			m_colorSolid;
};

/// <summary>
/// ID2D1BitmapBrush wrapper.
/// </summary>
class CD2DBitmapBrush : public CD2DBrush
{
	DECLARE_DYNAMIC(CD2DBitmapBrush)

public:
	/// <summary>
	/// Constructs a CD2DBitmapBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="pBitmapBrushProperties">A pointer to the extend modes and the interpolation mode of a bitmap brush.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmapBrush(CRenderTarget* pParentTarget, D2D1_BITMAP_BRUSH_PROPERTIES* pBitmapBrushProperties = NULL, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// Constructs a CD2DBitmapBrush object from resource.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="uiResID">The resource ID number of the resource.</param>
	/// <param name="lpszType">Pointer to a null-terminated string that contains the resource type.</param>
	/// <param name="sizeDest">Destination size of the bitmap.</param>
	/// <param name="pBitmapBrushProperties">A pointer to the extend modes and the interpolation mode of a bitmap brush.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmapBrush(CRenderTarget* pParentTarget, UINT uiResID, LPCTSTR lpszType = NULL, CD2DSizeU sizeDest = CD2DSizeU(0, 0), D2D1_BITMAP_BRUSH_PROPERTIES* pBitmapBrushProperties = NULL, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);
	
	/// <summary>
	/// Constructs a CD2DBitmapBrush object from file.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="lpszImagePath">Pointer to a null-terminated string that contains the name of file.</param>
	/// <param name="sizeDest">Destination size of the bitmap.</param>
	/// <param name="pBitmapBrushProperties">A pointer to the extend modes and the interpolation mode of a bitmap brush.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DBitmapBrush(CRenderTarget* pParentTarget, LPCTSTR lpszImagePath, CD2DSizeU sizeDest = CD2DSizeU(0, 0), D2D1_BITMAP_BRUSH_PROPERTIES* pBitmapBrushProperties = NULL, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D bitmap brush object is being destroyed.
	/// </summary>
	virtual ~CD2DBitmapBrush();
	
	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1BitmapBrush* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1BitmapBrush* Detach();

	/// <summary>
	/// Returns ID2D1BitmapBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1BitmapBrush interface or NULL if object is not initialized yet.</returns>
	ID2D1BitmapBrush* Get() { return m_pBitmapBrush; }

	/// <summary>
	/// Returns ID2D1BitmapBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1BitmapBrush interface or NULL if object is not initialized yet.</returns>
	operator ID2D1BitmapBrush*() { return m_pBitmapBrush; }

	/// <summary>
	/// Gets the bitmap source that this brush uses to paint</summary>
	/// <returns> 
	/// Pointer to an CD2DBitmap object or NULL if object is not initialized yet.</returns>
	CD2DBitmap* GetBitmap()
	{
		ASSERT_VALID(m_pBitmap);
		return m_pBitmap;
	}
	
	/// <summary>
	/// Specifies how the brush horizontally tiles those areas that extend past its bitmap</summary>
	/// <param name="extendModeX">A value that specifies how the brush horizontally tiles those areas that extend past its bitmap</param>
	void SetExtendModeX(D2D1_EXTEND_MODE extendModeX);

	/// <summary>
	/// Specifies how the brush vertically tiles those areas that extend past its bitmap</summary>
	/// <param name="extendModeY">A value that specifies how the brush vertically tiles those areas that extend past its bitmap</param>
	void SetExtendModeY(D2D1_EXTEND_MODE extendModeY);
	
	/// <summary>
	/// Specifies the interpolation mode used when the brush bitmap is scaled or rotated</summary>
	/// <param name="interpolationMode">The interpolation mode used when the brush bitmap is scaled or rotated</param>
	void SetInterpolationMode(D2D1_BITMAP_INTERPOLATION_MODE interpolationMode);

	/// <summary>
	/// Specifies the bitmap source that this brush uses to paint</summary>
	/// <param name="pBitmap">The bitmap source used by the brush</param>
	void SetBitmap(CD2DBitmap* pBitmap);

	/// <summary>
	/// Gets the method by which the brush horizontally tiles those areas that extend past its bitmap</summary>
	/// <returns> 
	/// A value that specifies how the brush horizontally tiles those areas that extend past its bitmap</returns>
	D2D1_EXTEND_MODE GetExtendModeX() const;

	/// <summary>
	/// Gets the method by which the brush vertically tiles those areas that extend past its bitmap</summary>
	/// <returns> 
	/// A value that specifies how the brush vertically tiles those areas that extend past its bitmap</returns>
	D2D1_EXTEND_MODE GetExtendModeY() const;

	/// <summary>
	/// Gets the interpolation method used when the brush bitmap is scaled or rotated</summary>
	/// <returns> 
	/// The interpolation method used when the brush bitmap is scaled or rotated</returns>
	D2D1_BITMAP_INTERPOLATION_MODE GetInterpolationMode() const;

public:
	/// <summary>
	/// Creates a CD2DBitmapBrush.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DBitmapBrush object.</summary>
	virtual void Destroy();

protected:
	/// <summary>
	/// Initializes the object</summary>
	/// <param name="pBitmapBrushProperties">A pointer to the bitmap brush properties.</param>
	void CommonInit(D2D1_BITMAP_BRUSH_PROPERTIES* pBitmapBrushProperties);

protected:
	/// <summary>
	/// Stores a pointer to a CD2DBitmap object.
	/// </summary>
	CD2DBitmap* 					m_pBitmap;

	/// <summary>
	/// Stores a pointer to an ID2D1BitmapBrush object.
	/// </summary>
	ID2D1BitmapBrush*				m_pBitmapBrush;

	/// <summary>
	/// Bitmap brush properties.
	/// </summary>
	D2D1_BITMAP_BRUSH_PROPERTIES*	m_pBitmapBrushProperties;
};

/// <summary>
/// The base class of CD2DLinearGradientBrush and CD2DRadialGradientBrush classes.
/// </summary>
class CD2DGradientBrush : public CD2DBrush
{
	DECLARE_DYNAMIC(CD2DGradientBrush)

public:
	/// <summary>
	/// Constructs a CD2DGradientBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="gradientStops">A pointer to an array of D2D1_GRADIENT_STOP structures.</param>
	/// <param name="gradientStopsCount">A value greater than or equal to 1 that specifies the number of gradient stops in the gradientStops array.</param>
	/// <param name="colorInterpolationGamma">The space in which color interpolation between the gradient stops is performed.</param>
	/// <param name="extendMode">The behavior of the gradient outside the [0,1] normalized range.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DGradientBrush(CRenderTarget* pParentTarget, const D2D1_GRADIENT_STOP* gradientStops, UINT gradientStopsCount, D2D1_GAMMA colorInterpolationGamma = D2D1_GAMMA_2_2, D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE_CLAMP, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D gradient brush object is being destroyed.
	/// </summary>
	virtual ~CD2DGradientBrush();

protected:
	/// <summary>
	/// Destroys a CD2DGradientBrush object.</summary>
	virtual void Destroy();

protected:
	/// <summary>
	/// Array of the D2D1_GRADIENT_STOP structures.
	/// </summary>
	CArray<D2D1_GRADIENT_STOP, D2D1_GRADIENT_STOP>		m_arGradientStops;

	/// <summary>
	/// The space in which color interpolation between the gradient stops is performed.
	/// </summary>
	D2D1_GAMMA 								m_colorInterpolationGamma;

	/// <summary>
	/// The behavior of the gradient outside the [0,1] normalized range.
	/// </summary>
	D2D1_EXTEND_MODE						m_extendMode;

	/// <summary>
	/// A pointer to an array of D2D1_GRADIENT_STOP structures.
	/// </summary>
	ID2D1GradientStopCollection*			m_pGradientStops;
};

/// <summary>
/// ID2D1LinearGradientBrush wrapper.
/// </summary>
class CD2DLinearGradientBrush : public CD2DGradientBrush
{
	DECLARE_DYNAMIC(CD2DLinearGradientBrush)

public:
	/// <summary>
	/// Constructs a CD2DLinearGradientBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="gradientStops">A pointer to an array of D2D1_GRADIENT_STOP structures.</param>
	/// <param name="gradientStopsCount">A value greater than or equal to 1 that specifies the number of gradient stops in the gradientStops array.</param>
	/// <param name="LinearGradientBrushProperties">The start and end points of the gradient.</param>
	/// <param name="colorInterpolationGamma">The space in which color interpolation between the gradient stops is performed.</param>
	/// <param name="extendMode">The behavior of the gradient outside the [0,1] normalized range.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DLinearGradientBrush(CRenderTarget* pParentTarget, const D2D1_GRADIENT_STOP* gradientStops, UINT gradientStopsCount, D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES LinearGradientBrushProperties, D2D1_GAMMA colorInterpolationGamma = D2D1_GAMMA_2_2, D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE_CLAMP, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D linear gradient brush object is being destroyed.
	/// </summary>
	virtual ~CD2DLinearGradientBrush();

public:
	/// <summary>
	/// Creates a CD2DLinearGradientBrush.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DLinearGradientBrush object.</summary>
	virtual void Destroy();
	
	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1LinearGradientBrush* pResource);

	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1LinearGradientBrush* Detach();

	/// <summary>
	/// Returns ID2D1LinearGradientBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1LinearGradientBrush interface or NULL if object is not initialized yet.</returns>
	ID2D1LinearGradientBrush* Get() { return m_pLinearGradientBrush; }

	/// <summary>
	/// Returns ID2D1LinearGradientBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1LinearGradientBrush interface or NULL if object is not initialized yet.</returns>
	operator ID2D1LinearGradientBrush*() { return m_pLinearGradientBrush; }

	/// <summary>
	/// Sets the starting coordinates of the linear gradient in the brush's coordinate space</summary>
	/// <param name="point">The starting two-dimensional coordinates of the linear gradient, in the brush's coordinate space</param>
	void SetStartPoint(CD2DPointF point);
	
	/// <summary>
	/// Sets the ending coordinates of the linear gradient in the brush's coordinate space</summary>
	/// <param name="point">The ending two-dimensional coordinates of the linear gradient, in the brush's coordinate space</param>
	void SetEndPoint(CD2DPointF point);

	/// <summary>
	/// Retrieves the starting coordinates of the linear gradient</summary>
	/// <returns> 
	/// The starting two-dimensional coordinates of the linear gradient, in the brush's coordinate space</returns>
	CD2DPointF GetStartPoint() const;
	
	/// <summary>
	/// Retrieves the ending coordinates of the linear gradient</summary>
	/// <returns> 
	/// The ending two-dimensional coordinates of the linear gradient, in the brush's coordinate space</returns>
	CD2DPointF GetEndPoint() const;

protected:
	/// <summary>
	/// The start and end points of the gradient.
	/// </summary>
	D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES 	m_LinearGradientBrushProperties;

	/// <summary>
	/// A pointer to an ID2D1LinearGradientBrush.
	/// </summary>
	ID2D1LinearGradientBrush*				m_pLinearGradientBrush;
};

/// <summary>
/// ID2D1RadialGradientBrush wrapper.
/// </summary>
class CD2DRadialGradientBrush : public CD2DGradientBrush
{
	DECLARE_DYNAMIC(CD2DRadialGradientBrush)

public:
	/// <summary>
	/// Constructs a CD2DLinearGradientBrush object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="gradientStops">A pointer to an array of D2D1_GRADIENT_STOP structures.</param>
	/// <param name="gradientStopsCount">A value greater than or equal to 1 that specifies the number of gradient stops in the gradientStops array.</param>
	/// <param name="RadialGradientBrushProperties">The center, gradient origin offset, and x-radius and y-radius of the brush's gradient.</param>
	/// <param name="colorInterpolationGamma">The space in which color interpolation between the gradient stops is performed.</param>
	/// <param name="extendMode">The behavior of the gradient outside the [0,1] normalized range.</param>
	/// <param name="pBrushProperties">A pointer to the opacity and transformation of a brush.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DRadialGradientBrush(CRenderTarget* pParentTarget, const D2D1_GRADIENT_STOP* gradientStops, UINT gradientStopsCount, D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES RadialGradientBrushProperties, D2D1_GAMMA colorInterpolationGamma = D2D1_GAMMA_2_2, D2D1_EXTEND_MODE extendMode = D2D1_EXTEND_MODE_CLAMP, CD2DBrushProperties* pBrushProperties = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D radial gradient brush object is being destroyed.
	/// </summary>
	virtual ~CD2DRadialGradientBrush();

public:
	/// <summary>
	/// Creates a CD2DRadialGradientBrush.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DRadialGradientBrush object.</summary>
	virtual void Destroy();

	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1RadialGradientBrush* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1RadialGradientBrush* Detach();

	/// <summary>
	/// Returns ID2D1RadialGradientBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1RadialGradientBrush interface or NULL if object is not initialized yet.</returns>
	ID2D1RadialGradientBrush* Get() { return m_pRadialGradientBrush; }

	/// <summary>
	/// Returns ID2D1RadialGradientBrush interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1RadialGradientBrush interface or NULL if object is not initialized yet.</returns>
	operator ID2D1RadialGradientBrush*() { return m_pRadialGradientBrush; }

	/// <summary>
	/// Specifies the center of the gradient ellipse in the brush's coordinate space</summary>
	/// <param name="point">The center of the gradient ellipse, in the brush's coordinate space</param>
	void SetCenter(CD2DPointF point);

	/// <summary>
	/// Specifies the offset of the gradient origin relative to the gradient ellipse's center</summary>
	/// <param name="gradientOriginOffset">The offset of the gradient origin from the center of the gradient ellipse</param>
	void SetGradientOriginOffset(CD2DPointF gradientOriginOffset);
	
	/// <summary>
	/// Specifies the x-radius of the gradient ellipse, in the brush's coordinate space</summary>
	/// <param name="radiusX">The x-radius of the gradient ellipse. This value is in the brush's coordinate space</param>
	void SetRadiusX(FLOAT radiusX);
	
	/// <summary>
	/// Specifies the y-radius of the gradient ellipse, in the brush's coordinate space</summary>
	/// <param name="radiusY">The y-radius of the gradient ellipse. This value is in the brush's coordinate space</param>
	void SetRadiusY(FLOAT radiusY);

	/// <summary>
	/// Retrieves the center of the gradient ellipse</summary>
	/// <returns> 
	/// The center of the gradient ellipse. This value is expressed in the brush's coordinate space</returns>
	CD2DPointF GetCenter() const;
	
	/// <summary>
	/// Retrieves the offset of the gradient origin relative to the gradient ellipse's center</summary>
	/// <returns> 
	/// The offset of the gradient origin from the center of the gradient ellipse. This value is expressed in the brush's coordinate space</returns>
	CD2DPointF GetGradientOriginOffset() const;
	
	/// <summary>
	/// Retrieves the x-radius of the gradient ellipse</summary>
	/// <returns> 
	/// The x-radius of the gradient ellipse. This value is expressed in the brush's coordinate space</returns>
	FLOAT GetRadiusX() const;
	
	/// <summary>
	/// Retrieves the y-radius of the gradient ellipse</summary>
	/// <returns> 
	/// The y-radius of the gradient ellipse. This value is expressed in the brush's coordinate space</returns>
	FLOAT GetRadiusY() const;

protected:
	/// <summary>
	/// The center, gradient origin offset, and x-radius and y-radius of the brush's gradient.
	/// </summary>
	D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES 	m_RadialGradientBrushProperties;

	/// <summary>
	/// A pointer to an ID2D1RadialGradientBrush.
	/// </summary>
	ID2D1RadialGradientBrush*				m_pRadialGradientBrush;
};

/// <summary>
/// IDWriteTextFormat wrapper.
/// </summary>
class CD2DTextFormat : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DTextFormat)

	friend class CRenderTarget;

public:
	/// <summary>
	/// Constructs a CD2DTextFormat object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="strFontFamilyName">A CString object that contains the name of the font family.</param>
	/// <param name="fontSize">The logical size of the font in DIP ("device-independent pixel") units. A DIPequals 1/96 inch.</param>
	/// <param name="fontWeight">A value that indicates the font weight for the text object.</param>
	/// <param name="fontStyle">A value that indicates the font style for the text object.</param>
	/// <param name="fontStretch">A value that indicates the font stretch for the text object.</param>
	/// <param name="strFontLocale">A CString object that contains the locale name.</param>
	/// <param name="pFontCollection">A pointer to a font collection object. When this is NULL, indicates the system font collection.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DTextFormat(CRenderTarget* pParentTarget, const CString& strFontFamilyName, FLOAT fontSize, DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL, const CString& strFontLocale = _T(""), IDWriteFontCollection* pFontCollection = NULL, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D text format object is being destroyed.
	/// </summary>
	virtual ~CD2DTextFormat();

	/// <summary>
	/// Re-creates a CD2DTextFormat.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT ReCreate(CRenderTarget* pRenderTarget) { UNREFERENCED_PARAMETER(pRenderTarget); return S_OK; }

	/// <summary>
	/// Creates a CD2DTextFormat.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget) { UNREFERENCED_PARAMETER(pRenderTarget); return S_OK; }

	/// <summary>
	/// Destroys a CD2DTextFormat object.</summary>
	virtual void Destroy()	{}

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pTextFormat != NULL; }

	/// <summary>
	/// Returns IDWriteTextFormat interface</summary>
	/// <returns> 
	/// Pointer to an IDWriteTextFormat interface or NULL if object is not initialized yet.</returns>
	IDWriteTextFormat* Get() { return m_pTextFormat; }

	/// <summary>
	/// Returns IDWriteTextFormat interface</summary>
	/// <returns>
	/// Pointer to an IDWriteTextFormat interface or NULL if object is not initialized yet.</returns>
	operator IDWriteTextFormat*() { return m_pTextFormat; }

	/// <summary>
	/// Gets a copy of the font family name.</summary>
	/// <returns>
	/// CString object that contains the current font family name.</returns>
	CString GetFontFamilyName() const;

	/// <summary>
	/// Gets a copy of the locale name.</summary>
	/// <returns>
	/// CString object that contains the current locale name.</returns>
	CString GetLocaleName() const;

protected:
	/// <summary>
	/// A pointer to an IDWriteTextFormat.
	/// </summary>
	IDWriteTextFormat* m_pTextFormat;
};

/// <summary>
/// IDWriteTextLayout wrapper.
/// </summary>
class CD2DTextLayout : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DTextLayout)

public:
	/// <summary>
	/// Constructs a CD2DTextLayout object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="strText">A CString object that contains the string to create a new CD2DTextLayout object from.</param>
	/// <param name="textFormat">A CString object that contains the format to apply to the string.</param>
	/// <param name="sizeMax">The size of the layout box.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DTextLayout(CRenderTarget* pParentTarget, const CString& strText, CD2DTextFormat& textFormat, const CD2DSizeF& sizeMax, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D text layout object is being destroyed.
	/// </summary>
	virtual ~CD2DTextLayout();

	/// <summary>
	/// Re-creates a CD2DTextLayout.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT ReCreate(CRenderTarget* pRenderTarget) { UNREFERENCED_PARAMETER(pRenderTarget); return S_OK; }

	/// <summary>
	/// Creates a CD2DTextLayout.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget) { UNREFERENCED_PARAMETER(pRenderTarget); return S_OK; }

	/// <summary>
	/// Destroys a CD2DTextLayout object.</summary>
	virtual void Destroy()	{}

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pTextLayout != NULL; }

	/// <summary>
	/// Returns IDWriteTextLayout interface</summary>
	/// <returns>
	/// Pointer to an IDWriteTextLayout interface or NULL if object is not initialized yet.</returns>
	IDWriteTextLayout* Get() { return m_pTextLayout; }

	/// <summary>
	/// Returns IDWriteTextLayout interface</summary>
	/// <returns>
	/// Pointer to an IDWriteTextLayout interface or NULL if object is not initialized yet.</returns>
	operator IDWriteTextLayout*() { return m_pTextLayout; }

	/// <summary>
	/// Sets null-terminated font family name for text within a specified text range</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE</returns>
	/// <param name="pwzFontFamilyName">The font family name that applies to the entire text string within the range specified by textRange</param>
	/// <param name="textRange">Text range to which this change applies</param>
	BOOL SetFontFamilyName(LPCWSTR pwzFontFamilyName, DWRITE_TEXT_RANGE textRange);

	/// <summary>
	/// Sets the locale name for text within a specified text range</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE</returns>
	/// <param name="pwzLocaleName">A null-terminated locale name string</param>
	/// <param name="textRange">Text range to which this change applies</param>
	BOOL SetLocaleName(LPCWSTR pwzLocaleName, DWRITE_TEXT_RANGE textRange);

	/// <summary>
	/// Copies the font family name of the text at the specified position.</summary>
	/// <returns>
	/// CString object that contains the current font family name.</returns>
	/// <param name="currentPosition">The position of the text to examine.</param>
	/// <param name="textRange">The range of text that has the same formatting as the text at the position specified by currentPosition. This means the run has the exact formatting as the position specified, including but not limited to the font family name.</param>
	CString GetFontFamilyName(UINT32 currentPosition, DWRITE_TEXT_RANGE* textRange = NULL) const;

	/// <summary>
	/// Gets the locale name of the text at the specified position.</summary>
	/// <returns>
	/// CString object that contains the current locale name.</returns>
	/// <param name="currentPosition">The position of the text to inspect.</param>
	/// <param name="textRange">The range of text that has the same formatting as the text at the position specified by currentPosition. This means the run has the exact formatting as the position specified, including but not limited to the locale name.</param>
	CString GetLocaleName(UINT32 currentPosition, DWRITE_TEXT_RANGE* textRange = NULL) const;

protected:
	/// <summary>
	/// A pointer to an IDWriteTextLayout.
	/// </summary>
	IDWriteTextLayout* m_pTextLayout;
};

/// <summary>
/// ID2D1Geometry wrapper.
/// </summary>
class CD2DGeometry : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DGeometry)

public:
	/// <summary>
	/// Constructs a CD2DGeometry object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DGeometry(CRenderTarget* pParentTarget, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D geometry object is being destroyed.
	/// </summary>
	virtual ~CD2DGeometry();
	
	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1Geometry* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1Geometry* Detach();

	/// <summary>
	/// Returns ID2D1Geometry interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Geometry interface or NULL if object is not initialized yet.</returns>
	ID2D1Geometry* Get() { return m_pGeometry; }

	/// <summary>
	/// Returns ID2D1Geometry interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Geometry interface or NULL if object is not initialized yet.</returns>
	operator ID2D1Geometry*() { return m_pGeometry; }

	/// <summary>
	/// Destroys a CD2DGeometry object.</summary>
	virtual void Destroy();

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pGeometry != NULL; }

	/// <summary>
	/// Combines this geometry with the specified geometry and stores the result in an ID2D1SimplifiedGeometrySink.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="inputGeometry">The geometry to combine with this instance.</param>
	/// <param name="combineMode">The type of combine operation to perform.</param>
	/// <param name="inputGeometryTransform">The transform to apply to inputGeometry before combining.</param>
	/// <param name="geometrySink">The result of the combine operation.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometries. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL CombineWithGeometry(CD2DGeometry& inputGeometry, D2D1_COMBINE_MODE combineMode, const D2D1_MATRIX_3X2_F& inputGeometryTransform, ID2D1SimplifiedGeometrySink* geometrySink, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Describes the intersection between this geometry and the specified geometry. The comparison is performed using the specified flattening tolerance.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="inputGeometry">The geometry to test.</param>
	/// <param name="inputGeometryTransform">The transform to apply to inputGeometry.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometries. Smaller values produce more accurate results but cause slower execution. </param>
	D2D1_GEOMETRY_RELATION CompareWithGeometry(CD2DGeometry& inputGeometry, const D2D1_MATRIX_3X2_F& inputGeometryTransform, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;

	/// <summary>
	/// Computes the area of the geometry after it has been transformed by the specified matrix and flattened using the specified tolerance.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="worldTransform">The transform to apply to this geometry before computing its area.</param>
	/// <param name="area">When this method returns, contains a pointer to the area of the transformed, flattened version of this geometry. You must allocate storage for this parameter.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL ComputeArea(const D2D1_MATRIX_3X2_F& worldTransform, FLOAT& area, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Calculates the length of the geometry as though each segment were unrolled into a line.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="worldTransform">The transform to apply to the geometry before calculating its length.</param>
	/// <param name="length">When this method returns, contains a pointer to the length of the geometry. For closed geometries, the length includes an implicit closing segment. You must allocate storage for this parameter.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL ComputeLength(const D2D1_MATRIX_3X2_F& worldTransform, FLOAT& length, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Calculates the point and tangent vector at the specified distance along the geometry after it has been transformed by the specified matrix and flattened using the specified tolerance.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="length">The distance along the geometry of the point and tangent to find. If this distance is less then 0, this method calculates the first point in the geometry. If this distance is greater than the length of the geometry, this method calculates the last point in the geometry.</param>
	/// <param name="worldTransform">The transform to apply to the geometry before calculating the specified point and tangent.</param>
	/// <param name="point">The location at the specified distance along the geometry. If the geometry is empty, this point contains NaN as its x and y values.</param>
	/// <param name="unitTangentVector">When this method returns, contains a pointer to the tangent vector at the specified distance along the geometry. If the geometry is empty, this vector contains NaN as its x and y values. You must allocate storage for this parameter.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL ComputePointAtLength(FLOAT length, const D2D1_MATRIX_3X2_F &worldTransform, CD2DPointF& point, CD2DPointF& unitTangentVector, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;

	/// <summary>
	/// Retrieves the bounds of the geometry.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="worldTransform">The transform to apply to this geometry before calculating its bounds.</param>
	/// <param name="bounds">When this method returns, contains the bounds of this geometry. If the bounds are empty, this will be a rect where bounds.left is greater than bounds.right. You must allocate storage for this parameter.</param>
	BOOL GetBounds(const D2D1_MATRIX_3X2_F& worldTransform, CD2DRectF& bounds) const;
	
	/// <summary>
	/// Gets the bounds of the geometry after it has been widened by the specified stroke width and style and transformed by the specified matrix.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="strokeWidth">The amount by which to widen the geometry by stroking its outline.</param>
	/// <param name="strokeStyle">The style of the stroke that widens the geometry.</param>
	/// <param name="worldTransform">A transform to apply to the geometry after the geometry is transformed and after the geometry has been stroked.</param>
	/// <param name="bounds">When this method returns, contains the bounds of the widened geometry. You must allocate storage for this parameter.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometries. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL GetWidenedBounds(FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, const D2D1_MATRIX_3X2_F& worldTransform, CD2DRectF& bounds, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;

	/// <summary>
	/// Indicates whether the area filled by the geometry would contain the specified point given the specified flattening tolerance.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="point">The point to test.</param>
	/// <param name="worldTransform">The transform to apply to the geometry prior to testing for containment.</param>
	/// <param name="contains">When this method returns, contains a bool value that is TRUE if the area filled by the geometry contains point; otherwise, FALSE. You must allocate storage for this parameter.</param>
	/// <param name="flatteningTolerance">The numeric accuracy with which the precise geometric path and path intersection is calculated. Points missing the fill by less than the tolerance are still considered inside. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL FillContainsPoint(CD2DPointF point, const D2D1_MATRIX_3X2_F& worldTransform, BOOL* contains, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Determines whether the geometry's stroke contains the specified point given the specified stroke thickness, style, and transform.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="point">The point to test for containment.</param>
	/// <param name="strokeWidth">The thickness of the stroke to apply.</param>
	/// <param name="strokeStyle">The style of the stroke to apply.</param>
	/// <param name="worldTransform">The transform to apply to the stroked geometry.</param>
	/// <param name="contains">When this method returns, contains a boolean value set to TRUE if the geometry's stroke contains the specified point; otherwise, FALSE. You must allocate storage for this parameter.</param>
	/// <param name="flatteningTolerance">The numeric accuracy with which the precise geometric path and path intersection is calculated. Points missing the stroke by less than the tolerance are still considered inside. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL StrokeContainsPoint(CD2DPointF point, FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, const D2D1_MATRIX_3X2_F& worldTransform, BOOL* contains, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Computes the outline of the geometry and writes the result to an ID2D1SimplifiedGeometrySink.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="worldTransform">The transform to apply to the geometry outline.</param>
	/// <param name="geometrySink">The ID2D1SimplifiedGeometrySink to which the geometry transformed outline is appended.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL Outline(const D2D1_MATRIX_3X2_F& worldTransform, ID2D1SimplifiedGeometrySink* geometrySink, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Creates a simplified version of the geometry that contains only lines and (optionally) cubic Bezier curves and writes the result to an ID2D1SimplifiedGeometrySink.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="simplificationOption">A value that specifies whether the simplified geometry should contain curves.</param>
	/// <param name="worldTransform">The transform to apply to the simplified geometry.</param>
	/// <param name="geometrySink">The ID2D1SimplifiedGeometrySink to which the simplified geometry is appended.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL Simplify(D2D1_GEOMETRY_SIMPLIFICATION_OPTION simplificationOption, const D2D1_MATRIX_3X2_F& worldTransform, ID2D1SimplifiedGeometrySink* geometrySink, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Creates a set of clockwise-wound triangles that cover the geometry after it has been transformed using the specified matrix and flattened using the specified tolerance.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="worldTransform">The transform to apply to this geometry, or NULL.</param>
	/// <param name="tessellationSink">The ID2D1TessellationSink to which the tessellated is appended.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL Tessellate(const D2D1_MATRIX_3X2_F& worldTransform, ID2D1TessellationSink* tessellationSink, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;
	
	/// <summary>
	/// Widens the geometry by the specified stroke and writes the result to an ID2D1SimplifiedGeometrySink after it has been transformed by the specified matrix and flattened using the specified tolerance.</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="strokeWidth">The amount by which to widen the geometry.</param>
	/// <param name="strokeStyle">The style of stroke to apply to the geometry, or NULL.</param>
	/// <param name="worldTransform">The transform to apply to the geometry after widening it.</param>
	/// <param name="geometrySink">The ID2D1SimplifiedGeometrySink to which the widened geometry is appended.</param>
	/// <param name="flatteningTolerance">The maximum bounds on the distance between points in the polygonal approximation of the geometry. Smaller values produce more accurate results but cause slower execution.</param>
	BOOL Widen(FLOAT strokeWidth, ID2D1StrokeStyle* strokeStyle, const D2D1_MATRIX_3X2_F& worldTransform, ID2D1SimplifiedGeometrySink* geometrySink, FLOAT flatteningTolerance = D2D1_DEFAULT_FLATTENING_TOLERANCE) const;

protected:
	/// <summary>
	/// A pointer to an ID2D1Geometry.
	/// </summary>
	ID2D1Geometry* m_pGeometry;
};

/// <summary>
/// ID2D1PathGeometry wrapper.
/// </summary>
class CD2DPathGeometry : public CD2DGeometry
{
	DECLARE_DYNAMIC(CD2DPathGeometry)

public:
	/// <summary>
	/// Constructs a CD2DPathGeometry object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DPathGeometry(CRenderTarget* pParentTarget, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// Creates a CD2DPathGeometry.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DPathGeometry object.</summary>
	virtual void Destroy();

	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1PathGeometry* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1PathGeometry* Detach();

	/// <summary>
	/// Retrieves the geometry sink that is used to populate the path geometry with figures and segments.</summary>
	/// <returns>
	/// A pointer to the ID2D1GeometrySink that is used to populate the path geometry with figures and segments.</returns>
	ID2D1GeometrySink* Open();

	/// <summary>
	/// Copies the contents of the path geometry to the specified ID2D1GeometrySink.</summary>
	/// <returns>
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="geometrySink">The sink to which the path geometry's contents are copied. Modifying this sink does not change the contents of this path geometry.</param>
	BOOL Stream(ID2D1GeometrySink* geometrySink);

	/// <summary>
	/// Retrieves the number of segments in the path geometry.</summary>
	/// <returns> 
	/// Returns the number of segments in the path geometry.</returns>
	int GetSegmentCount() const;

	/// <summary>
	/// Retrieves tthe number of figures in the path geometry.</summary>
	/// <returns> 
	/// Returns the number of figures in the path geometry.</returns>
	int GetFigureCount() const;

protected:
	/// <summary>
	/// A pointer to an ID2D1PathGeometry.
	/// </summary>
	ID2D1PathGeometry* m_pPathGeometry;
};

/// <summary>
/// ID2D1GeometrySink wrapper.
/// </summary>
class CD2DGeometrySink
{
public:
	/// <summary>
	/// Constructs a CD2DGeometrySink object from CD2DPathGeometry object.</summary>
	/// <param name="pathGeometry">An existing CD2DPathGeometry object.</param>
	CD2DGeometrySink(CD2DPathGeometry& pathGeometry);

	/// <summary>
	/// The destructor. Called when a D2D geometry sink object is being destroyed.
	/// </summary>
	virtual ~CD2DGeometrySink();

	/// <summary>
	/// Checks geometry sink validity</summary>
	/// <returns> 
	/// TRUE if geometry sink is valid; otherwise FALSE.</returns>
	BOOL IsValid() const { return m_pSink != NULL; }

	/// <summary>
	/// Returns ID2D1GeometrySink interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1GeometrySink interface or NULL if object is not initialized yet.</returns>
	ID2D1GeometrySink* Get() { return m_pSink; }

	/// <summary>
	/// Returns ID2D1GeometrySink interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1GeometrySink interface or NULL if object is not initialized yet.</returns>
	operator ID2D1GeometrySink*() { return m_pSink; }

	/// <summary>
	/// Starts a new figure at the specified point.</summary>
	/// <param name="startPoint">The point at which to begin the new figure.</param>
	/// <param name="figureBegin">Whether the new figure should be hollow or filled.</param>
	void BeginFigure(CD2DPointF startPoint, D2D1_FIGURE_BEGIN figureBegin);
	
	/// <summary>
	/// Ends the current figure; optionally, closes it.</summary>
	/// <param name="figureEnd">A value that indicates whether the current figure is closed. If the figure is closed, a line is drawn between the current point and the start point specified by BeginFigure.</param>
	void EndFigure(D2D1_FIGURE_END figureEnd);

	/// <summary>
	/// Adds a single arc to the path geometry</summary>
	/// <param name="arc">The arc segment to add to the figure</param>
	void AddArc(const D2D1_ARC_SEGMENT& arc);

	/// <summary>
	/// Creates a cubic Bezier curve between the current point and the specified end point.</summary>
	/// <param name="bezier">A structure that describes the control points and end point of the Bezier curve to add.</param>
	void AddBezier(const D2D1_BEZIER_SEGMENT& bezier);
	
	/// <summary>
	/// Creates a sequence of cubic Bezier curves and adds them to the geometry sink.</summary>
	/// <param name="beziers">An array of Bezier segments that describes the Bezier curves to create. A curve is drawn from the geometry sink's current point (the end point of the last segment drawn or the location specified by BeginFigure) to the end point of the first Bezier segment in the array. if the array contains additional Bezier segments, each subsequent Bezier segment uses the end point of the preceding Bezier segment as its start point.</param>
	void AddBeziers(const CArray<D2D1_BEZIER_SEGMENT, D2D1_BEZIER_SEGMENT>& beziers);

	/// <summary>
	/// Creates a quadratic Bezier curve between the current point and the specified end point.</summary>
	/// <param name="bezier">A structure that describes the control point and the end point of the quadratic Bezier curve to add.</param>
	void AddQuadraticBezier(const D2D1_QUADRATIC_BEZIER_SEGMENT& bezier);
	
	/// <summary>
	/// Adds a sequence of quadratic Bezier segments as an array in a single call.</summary>
	/// <param name="beziers">An array of a sequence of quadratic Bezier segments.</param>
	void AddQuadraticBeziers(const CArray<D2D1_QUADRATIC_BEZIER_SEGMENT, D2D1_QUADRATIC_BEZIER_SEGMENT>& beziers);

	/// <summary>
	/// Creates a line segment between the current point and the specified end point and adds it to the geometry sink.</summary>
	/// <param name="point">The end point of the line to draw.</param>
	void AddLine(CD2DPointF point);
	
	/// <summary>
	/// Creates a sequence of lines using the specified points and adds them to the geometry sink.</summary>
	/// <param name="points">An array of one or more points that describe the lines to draw. A line is drawn from the geometry sink's current point (the end point of the last segment drawn or the location specified by BeginFigure) to the first point in the array. if the array contains additional points, a line is drawn from the first point to the second point in the array, from the second point to the third point, and so on. An array of a sequence of the end points of the lines to draw.</param>
	void AddLines(const CArray<CD2DPointF, CD2DPointF>& points);

	/// <summary>
	/// Closes the geometry sink</summary>
	/// <returns> 
	/// Nonzero if successful; otherwise FALSE.</returns>
	BOOL Close();

	/// <summary>
	/// Specifies the method used to determine which points are inside the geometry described by this geometry sink and which points are outside.</summary>
	/// <param name="fillMode">The method used to determine whether a given point is part of the geometry.</param>
	void SetFillMode(D2D1_FILL_MODE fillMode);
	
	/// <summary>
	/// Specifies stroke and join options to be applied to new segments added to the geometry sink.</summary>
	/// <param name="vertexFlags">Stroke and join options to be applied to new segments added to the geometry sink.</param>
	void SetSegmentFlags(D2D1_PATH_SEGMENT vertexFlags);

protected:
	/// <summary>
	/// A pointer to an ID2D1GeometrySink.
	/// </summary>
	ID2D1GeometrySink* m_pSink;
};

/// <summary>
/// ID2D1Mesh wrapper.
/// </summary>
class CD2DMesh : public CD2DResource
{
	DECLARE_DYNAMIC(CD2DMesh)

public:
	/// <summary>
	/// Constructs a CD2DMesh object.</summary>
	/// <param name="pParentTarget">A pointer to the render target.</param>
	/// <param name="bAutoDestroy">Indicates that the object will be destroyed by owner (pParentTarget).</param>
	CD2DMesh(CRenderTarget* pParentTarget, BOOL bAutoDestroy = TRUE);

	/// <summary>
	/// The destructor. Called when a D2D mesh object is being destroyed.
	/// </summary>
	virtual ~CD2DMesh();

	/// <summary>
	/// Attaches existing resource interface to the object</summary>
	/// <param name="pResource">Existing resource interface. Cannot be NULL</param>
	void Attach(ID2D1Mesh* pResource);
	
	/// <summary>
	/// Detaches resource interface from the object</summary>
	/// <returns> 
	/// Pointer to detached resource interface.</returns>
	ID2D1Mesh* Detach();

	/// <summary>
	/// Returns ID2D1Mesh interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Mesh interface or NULL if object is not initialized yet.</returns>
	ID2D1Mesh* Get() { return m_pMesh; }

	/// <summary>
	/// Returns ID2D1Mesh interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1Mesh interface or NULL if object is not initialized yet.</returns>
	operator ID2D1Mesh*() { return m_pMesh; }

	/// <summary>
	/// Opens the mesh for population.</summary>
	/// <returns>
	/// A pointer to an ID2D1TessellationSink that is used to populate the mesh.</returns>
	ID2D1TessellationSink* Open();

	/// <summary>
	/// Creates a CD2DMesh.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	/// <param name="pRenderTarget">A pointer to the render target.</param>
	virtual HRESULT Create(CRenderTarget* pRenderTarget);

	/// <summary>
	/// Destroys a CD2DMesh object.</summary>
	virtual void Destroy();

	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	virtual BOOL IsValid() const { return m_pMesh != NULL; }

protected:
	/// <summary>
	/// A pointer to an ID2D1Mesh.
	/// </summary>
	ID2D1Mesh* m_pMesh;
};

/// <summary>
/// ID2D1RenderTarget wrapper.
/// </summary>
class CRenderTarget : public CObject
{
	DECLARE_DYNAMIC(CRenderTarget)

	friend class CD2DResource;
	friend class CD2DBitmapBrush;
	friend class CD2DBitmap;

public:
	/// <summary>
	/// Constructs a CRenderTarget object.</summary>
	CRenderTarget();

	/// <summary>
	/// The destructor. Called when a render target object is being destroyed.
	/// </summary>
	virtual ~CRenderTarget();
	
	/// <summary>
	/// Deletes one or more resources</summary>
	/// <returns> 
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE</returns>
	/// <param name="bDeleteResources">If bDeleteResources is TRUE, all resources located in m_lstResources will be automatically destroyed.</param>
	BOOL Destroy(BOOL bDeleteResources = TRUE);

// Attributes:
public:
	/// <summary>
	/// Checks resource validity</summary>
	/// <returns> 
	/// TRUE if resource is valid; otherwise FALSE.</returns>
	BOOL IsValid() const { return m_pRenderTarget != NULL; }

	/// <summary>
	/// Returns ID2D1RenderTarget interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1RenderTarget interface or NULL if object is not initialized yet.</returns>
	ID2D1RenderTarget* GetRenderTarget() { return m_pRenderTarget; }

	/// <summary>
	/// Returns ID2D1RenderTarget interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1RenderTarget interface or NULL if object is not initialized yet.</returns>
	operator ID2D1RenderTarget*() { return m_pRenderTarget; }

	/// <summary>
	/// Returns the size of the render target in device-independent pixels</summary>
	/// <returns> 
	/// The current size of the render target in device-independent pixels</returns>
	CD2DSizeF GetSize() const;

	/// <summary>
	/// Sets the dots per inch (DPI) of the render target.</summary>
	/// <param name="sizeDPI">A value greater than or equal to zero that specifies the horizontal/verticalDPI of the render target.</param>
	void SetDpi(const CD2DSizeF& sizeDPI);

	/// <summary>
	/// Returns the render target's dots per inch (DPI)</summary>
	/// <returns> 
	/// The render target's dots per inch (DPI).</returns>
	CD2DSizeF GetDpi() const;

	/// <summary>
	/// Retrieves the pixel format and alpha mode of the render target</summary>
	/// <returns> 
	/// The pixel format and alpha mode of the render target</returns>
	D2D1_PIXEL_FORMAT GetPixelFormat() const;

	/// <summary>
	/// Returns the size of the render target in device pixels</summary>
	/// <returns> 
	/// The size of the render target in device pixels</returns>
	CD2DSizeU GetPixelSize() const;

	/// <summary>
	/// Gets the label for subsequent drawing operations.</summary>
	/// <param name="tag1">Contains the first label for subsequent drawing operations. This parameter is passed uninitialized. If NULL is specified, no value is retrieved for this parameter.</param>
	/// <param name="tag2">Contains the second label for subsequent drawing operations. This parameter is passed uninitialized. If NULL is specified, no value is retrieved for this parameter.</param>
	void GetTags(D2D1_TAG *tag1 = NULL, D2D1_TAG *tag2 = NULL) const;

	/// <summary>
	/// Specifies a label for subsequent drawing operations.</summary>
	/// <param name="tag1">A label to apply to subsequent drawing operations.</param>
	/// <param name="tag2">A label to apply to subsequent drawing operations.</param>
	void SetTags(D2D1_TAG tag1, D2D1_TAG tag2);

	/// <summary>
	/// Indicates whether the render target supports the specified properties</summary>
	/// <returns> 
	/// TRUE if the specified render target properties are supported by this render target; otherwise, FALSE</returns>
	/// <param name="renderTargetProperties">The render target properties to test</param>
	BOOL IsSupported(const D2D1_RENDER_TARGET_PROPERTIES& renderTargetProperties) const;

	/// <summary>
	/// Gets the maximum size, in device-dependent units (pixels), of any one bitmap dimension supported by the render target</summary>
	/// <returns> 
	/// The maximum size, in pixels, of any one bitmap dimension supported by the render target</returns>
	UINT32 GetMaximumBitmapSize() const;

// Operations:
public:
	/// <summary>
	/// Attaches existing render target interface to the object</summary>
	/// <param name="pRenderTarget">Existing render target interface. Cannot be NULL</param>
	void Attach(ID2D1RenderTarget* pRenderTarget);
	
	/// <summary>
	/// Detaches render target interface from the object</summary>
	/// <returns> 
	/// Pointer to detached render target interface.</returns>
	ID2D1RenderTarget* Detach ();

	/// <summary>
	/// Initiates drawing on this render target.</summary>
	void BeginDraw();

	/// <summary>
	/// Ends drawing operations on the render target and indicates the current error state and associated tags.</summary>
	/// <returns>
	/// If the method succeeds, it returns S_OK. Otherwise, it returns an HRESULT error code.</returns>
	HRESULT EndDraw();

	/// <summary>
	/// Converts GDI color and alpha values to the D2D1_COLOR_F object.</summary>
	/// <param name="color">RGB value.</param>
	/// <param name="nAlpha">Alpha value.</param>
	/// <returns>
	/// D2D1_COLOR_F value.</returns>
	static D2D1_COLOR_F COLORREF_TO_D2DCOLOR(COLORREF color, int nAlpha = 255);

	/// <summary>
	/// Clears the drawing area to the specified color.</summary>
	/// <param name="color">The color to which the drawing area is cleared.</param>
	void Clear(D2D1_COLOR_F color);

	/// <summary>
	/// Draws the outline of a rectangle that has the specified dimensions and stroke style.</summary>
	/// <param name="rect">The dimensions of the rectangle to draw, in device-independent pixels</param>
	/// <param name="pBrush">The brush used to paint the rectangle's stroke</param>
	/// <param name="fStrokeWidth">A value greater than or equal to 0.0f that specifies the width of the rectangle's stroke. The stroke is centered on the rectangle's outline.</param>
	/// <param name="strokeStyle">The style of stroke to paint, or NULL to paint a solid stroke.</param>
	void DrawRectangle(const CD2DRectF& rect, CD2DBrush* pBrush, FLOAT fStrokeWidth = 1.0, ID2D1StrokeStyle* strokeStyle = NULL);

	/// <summary>
	/// Paints the interior of the specified rectangle.</summary>
	/// <param name="rect">The dimension of the rectangle to paint, in device-independent pixels.</param>
	/// <param name="pBrush">The brush used to paint the rectangle's interior.</param>
	void FillRectangle(const CD2DRectF& rect, CD2DBrush* pBrush);

	/// <summary>
	/// Draws the outline of the specified rounded rectangle using the specified stroke style.</summary>
	/// <param name="rectRounded">The dimensions of the rounded rectangle to draw, in device-independent pixels.</param>
	/// <param name="pBrush">The brush used to paint the rounded rectangle's outline.</param>
	/// <param name="fStrokeWidth">The width of the rounded rectangle's stroke. The stroke is centered on the rounded rectangle's outline. The default value is 1.0f.</param>
	/// <param name="strokeStyle">The style of the rounded rectangle's stroke, or NULL to paint a solid stroke. The default value is NULL.</param>
	void DrawRoundedRectangle(const CD2DRoundedRect& rectRounded, CD2DBrush* pBrush, FLOAT fStrokeWidth = 1.0, ID2D1StrokeStyle* strokeStyle = NULL);

	/// <summary>
	/// Paints the interior of the specified rounded rectangle.</summary>
	/// <param name="rectRounded">The dimensions of the rounded rectangle to paint, in device independent pixels.</param>
	/// <param name="pBrush">The brush used to paint the interior of the rounded rectangle.</param>
	void FillRoundedRectangle(const CD2DRoundedRect& rectRounded, CD2DBrush* pBrush);

	/// <summary>
	/// Draws the outline of the specified ellipse using the specified stroke style.</summary>
	/// <param name="ellipse">The position and radius of the ellipse to draw, in device-independent pixels.</param>
	/// <param name="pBrush">The brush used to paint the ellipse's outline.</param>
	/// <param name="fStrokeWidth">The thickness of the ellipse's stroke. The stroke is centered on the ellipse's outline.</param>
	/// <param name="strokeStyle">The style of stroke to apply to the ellipse's outline, or NULL to paint a solid stroke.</param>
	void DrawEllipse(const CD2DEllipse& ellipse, CD2DBrush* pBrush, FLOAT fStrokeWidth = 1.0, ID2D1StrokeStyle* strokeStyle = NULL);

	/// <summary>
	/// Paints the interior of the specified ellipse.</summary>
	/// <param name="ellipse">The position and radius, in device-independent pixels, of the ellipse to paint.</param>
	/// <param name="pBrush">The brush used to paint the interior of the ellipse.</param>
	void FillEllipse(const CD2DEllipse& ellipse, CD2DBrush* pBrush);

	/// <summary>
	/// Draws a line between the specified points using the specified stroke style.</summary>
	/// <param name="ptFrom">The start point of the line, in device-independent pixels.</param>
	/// <param name="ptTo">The end point of the line, in device-independent pixels.</param>
	/// <param name="pBrush">The brush used to paint the line's stroke.</param>
	/// <param name="fStrokeWidth">A value greater than or equal to 0.0f that specifies the width of the stroke. If this parameter isn't specified, it defaults to 1.0f. The stroke is centered on the line.</param>
	/// <param name="strokeStyle">The style of stroke to paint, or NULL to paint a solid line.</param>
	void DrawLine(const CD2DPointF& ptFrom, const CD2DPointF& ptTo, CD2DBrush* pBrush, FLOAT fStrokeWidth = 1.0, ID2D1StrokeStyle* strokeStyle = NULL);

	 /// <summary>
	 /// Applies the opacity mask described by the specified bitmap to a brush and uses that brush to paint a region of the render target.</summary> 
	 /// <param name="pOpacityMask">The position and radius, in device-independent pixels, of the ellipse to paint.</param>
	 /// <param name="pBrush">The brush used to paint the region of the render target specified by destinationRectangle.</param>
	 /// <param name="content">The type of content the opacity mask contains. The value is used to determine the color space in which the opacity mask is blended.</param>
	 /// <param name="rectDest">The region of the render target to paint, in device-independent pixels.</param>
	 /// <param name="rectSrc">The region of the bitmap to use as the opacity mask, in device-independent pixels.</param>
	void FillOpacityMask(CD2DBitmap* pOpacityMask, CD2DBrush* pBrush, D2D1_OPACITY_MASK_CONTENT content, const CD2DRectF& rectDest, const CD2DRectF& rectSrc);

	/// <summary>
	/// Draws the specified text using the format information provided by an IDWriteTextFormat object.</summary>
	/// <param name="strText">A pointer to an array of Unicode characters to draw.</param>
	/// <param name="rect">The size and position of the area in which the text is drawn.</param>
	/// <param name="pForegroundBrush">The brush used to paint the text.</param>
	/// <param name="textFormat">An object that describes formatting details of the text to draw, such as the font, the font size, and flow direction.</param>
	/// <param name="options">A value that indicates whether the text should be snapped to pixel boundaries and whether the text should be clipped to the layout rectangle. The default value is D2D1_DRAW_TEXT_OPTIONS_NONE, which indicates that text should be snapped to pixel boundaries and it should not be clipped to the layout rectangle.</param>
	/// <param name="measuringMode">A value that indicates how glyph metrics are used to measure text when it is formatted. The default value is DWRITE_MEASURING_MODE_NATURAL.</param>
	void DrawText(const CString& strText, const CD2DRectF& rect, CD2DBrush* pForegroundBrush, CD2DTextFormat* textFormat = NULL, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL);

	/// <summary>
	/// Draws the formatted text described by the specified IDWriteTextLayout object.</summary>
	/// <param name="ptOrigin">The point, described in device-independent pixels, at which the upper-left corner of the text described by textLayout is drawn.</param>
	/// <param name="textLayout">The formatted text to draw. Any drawing effects that do not inherit from ID2D1Resource are ignored. If there are drawing effects that inherit from ID2D1Resource that are not brushes, this method fails and the render target is put in an error state.</param>
	/// <param name="pBrushForeground">The brush used to paint any text in textLayout that does not already have a brush associated with it as a drawing effect (specified by the IDWriteTextLayout::SetDrawingEffect method).</param>
	/// <param name="options">A value that indicates whether the text should be snapped to pixel boundaries and whether the text should be clipped to the layout rectangle. The default value is D2D1_DRAW_TEXT_OPTIONS_NONE, which indicates that text should be snapped to pixel boundaries and it should not be clipped to the layout rectangle.</param>
	void DrawTextLayout(const CD2DPointF& ptOrigin, CD2DTextLayout* textLayout, CD2DBrush* pBrushForeground, D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE);
	
	/// <summary>
	/// Draws the formatted text described by the specified IDWriteTextLayout object.</summary>
	/// <param name="pBitmap">The bitmap to render.</param>
	/// <param name="rectDest">The size and position, in device-independent pixels in the render target's coordinate space, of the area to which the bitmap is drawn. If the rectangle is not well-ordered, nothing is drawn, but the render target does not enter an error state.</param>
	/// <param name="fOpacity">A value between 0.0f and 1.0f, inclusive, that specifies an opacity value to apply to the bitmap; this value is multiplied against the alpha values of the bitmap's contents.</param>
	/// <param name="interpolationMode">The interpolation mode to use if the bitmap is scaled or rotated by the drawing operation.</param>
	/// <param name="pRectSrc">The size and position, in device-independent pixels in the bitmap's coordinate space, of the area within the bitmap to draw.</param>
	void DrawBitmap(CD2DBitmap* pBitmap, const CD2DRectF& rectDest, float fOpacity = 1.0, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, const CD2DRectF* pRectSrc = NULL);

	/// <summary>
	/// Draws the outline of the specified geometry using the specified stroke style.</summary>
	/// <param name="pGeometry">The geometry to draw.</param>
	/// <param name="pBrush">The brush used to paint the geometry's stroke.</param>
	/// <param name="fStrokeWidth">The thickness of the geometry's stroke. The stroke is centered on the geometry's outline.</param>
	/// <param name="strokeStyle">The style of stroke to apply to the geometry's outline, or NULL to paint a solid stroke.</param>
	void DrawGeometry(CD2DGeometry* pGeometry, CD2DBrush* pBrush, FLOAT fStrokeWidth = 1.0, ID2D1StrokeStyle* strokeStyle = NULL);
	
	/// <summary>
	/// Paints the interior of the specified geometry.</summary>
	/// <param name="pGeometry">The geometry to paint.</param>
	/// <param name="pBrush">The brush used to paint the geometry's interior.</param>
	/// <param name="pOpacityBrush">The opacity mask to apply to the geometry;NULL for no opacity mask. If an opacity mask (the opacityBrush parameter) is specified, brush must be an ID2D1BitmapBrush that has its x- and y-extend modes set to D2D1_EXTEND_MODE_CLAMP. For more information, see the Remarks section.</param>
	void FillGeometry(CD2DGeometry* pGeometry, CD2DBrush* pBrush, CD2DBrush* pOpacityBrush = NULL);

	/// <summary>
	/// Draws the specified glyphs.</summary>
	/// <param name="ptBaseLineOrigin">The origin, in device-independent pixels, of the glyphs' baseline.</param>
	/// <param name="glyphRun">The glyphs to render.</param>
	/// <param name="pForegroundBrush">The brush used to paint the specified glyphs.</param>
	/// <param name="measuringMode">A value that indicates how glyph metrics are used to measure text when it is formatted. The default value is DWRITE_MEASURING_MODE_NATURAL.</param>
	void DrawGlyphRun(const CD2DPointF& ptBaseLineOrigin, const DWRITE_GLYPH_RUN& glyphRun, CD2DBrush* pForegroundBrush, DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL);

	/// <summary>
	/// Paints the interior of the specified mesh.</summary>
	/// <param name="pMesh">The mesh to paint.</param>
	/// <param name="pBrush">The brush used to paint the mesh.</param>
	void FillMesh(CD2DMesh* pMesh, CD2DBrush* pBrush);

	/// <summary>
	/// Sets the antialiasing mode of the render target. The antialiasing mode applies to all subsequent drawing operations, excluding text and glyph drawing operations.</summary>
	/// <param name="antialiasMode">The antialiasing mode for future drawing operations.</param>
	void SetAntialiasMode(D2D1_ANTIALIAS_MODE antialiasMode);
	
	/// <summary>
	/// Retrieves the current antialiasing mode for nontext drawing operations.</summary>
	/// <returns>
	/// Current antialiasing mode for nontext drawing operations.</returns>
	D2D1_ANTIALIAS_MODE GetAntialiasMode() const;

	/// <summary>
	/// Specifies the antialiasing mode to use for subsequent text and glyph drawing operations.</summary>
	/// <param name="textAntialiasMode">The antialiasing mode to use for subsequent text and glyph drawing operations.</param>
	void SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE textAntialiasMode);
	
	/// <summary>
	/// Gets the current antialiasing mode for text and glyph drawing operations.</summary>
	/// <returns>
	/// Current antialiasing mode for text and glyph drawing operations.</returns>
	D2D1_TEXT_ANTIALIAS_MODE GetTextAntialiasMode() const;

	/// <summary>
	/// Specifies text rendering options to be applied to all subsequent text and glyph drawing operations.</summary>
	/// <param name="textRenderingParams">The text rendering options to be applied to all subsequent text and glyph drawing operations; NULL to clear current text rendering options.</param>
	void SetTextRenderingParams(IDWriteRenderingParams* textRenderingParams = NULL);
	
	/// <summary>
	/// Retrieves the render target's current text rendering options.</summary>
	/// <param name="textRenderingParams">When this method returns, textRenderingParamscontains the address of a pointer to the render target's current text rendering options.</param>
	void GetTextRenderingParams(IDWriteRenderingParams** textRenderingParams);

	/// <summary>
	/// Applies the specified transform to the render target, replacing the existing transformation. All subsequent drawing operations occur in the transformed space.</summary> 
	/// <param name="transform">The transform to apply to the render target.</param>
	void SetTransform(const D2D1_MATRIX_3X2_F* transform);
	
	/// <summary>
	/// Applies the specified transform to the render target, replacing the existing transformation. All subsequent drawing operations occur in the transformed space.</summary> 
	/// <param name="transform">The transform to apply to the render target.</param>
	void SetTransform(const D2D1_MATRIX_3X2_F& transform);
	
	/// <summary>
	/// Applies the specified transform to the render target, replacing the existing transformation. All subsequent drawing operations occur in the transformed space.</summary> 
	/// <param name="transform">The transform to apply to the render target.</param>
	void GetTransform(D2D1_MATRIX_3X2_F* transform);

	/// <summary>
	/// Creates a new bitmap render target for use during intermediate offscreen drawing that is compatible with the current render target .</summary> 
	/// <returns>If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="bitmapTarget">When this method returns, contains the address of a pointer to a new bitmap render target. This parameter is passed uninitialized.</param>
	/// <param name="sizeDesired">The desired size of the new render target in device-independent pixels if it should be different from the original render target, or NULL. For more information, see the Remarks section.</param>
	/// <param name="sizePixelDesired">The desired size of the new render target in pixels if it should be different from the original render target, or NULL. For more information, see the Remarks section.</param>
	/// <param name="desiredFormat">The desired pixel format and alpha mode of the new render target, or NULL. If the pixel format is set to DXGI_FORMAT_UNKNOWN or if this parameter is null, the new render target uses the same pixel format as the original render target. If the alpha mode is D2D1_ALPHA_MODE_UNKNOWN or this parameter is NULL, the alpha mode of the new render target defaults to D2D1_ALPHA_MODE_PREMULTIPLIED. For information about supported pixel formats, see Supported Pixel Formats and Alpha Modes.</param>
	/// <param name="options">A value that specifies whether the new render target must be compatible with GDI.</param>
	BOOL CreateCompatibleRenderTarget(CBitmapRenderTarget& bitmapTarget, CD2DSizeF sizeDesired = CD2DSizeF(0., 0.), CD2DSizeU sizePixelDesired = CD2DSizeU(0, 0), D2D1_PIXEL_FORMAT* desiredFormat = NULL, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS options = D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE);

	/// <summary>
	/// Adds the specified layer to the render target so that it receives all subsequent drawing operations until PopLayer is called.</summary> 
	/// <param name="layerParameters">The content bounds, geometric mask, opacity, opacity mask, and antialiasing options for the layer.</param>
	/// <param name="layer">The layer that receives subsequent drawing operations.</param>
	void PushLayer(const D2D1_LAYER_PARAMETERS& layerParameters, CD2DLayer& layer);
	
	/// <summary>
	/// Stops redirecting drawing operations to the layer that is specified by the last PushLayer call.</summary> 
	void PopLayer();

	/// <summary>
	/// Executes all pending drawing commands.</summary> 
	/// <param name="tag1">Contains the tag for drawing operations that caused errors or 0 if there were no errors. This parameter is passed uninitialized.</param>
	/// <param name="tag2">Contains the tag for drawing operations that caused errors or 0 if there were no errors. This parameter is passed uninitialized.</param>
	void Flush(D2D1_TAG* tag1 = NULL, D2D1_TAG* tag2 = NULL);
	
	/// <summary>
	/// Saves the current drawing state to the specified ID2D1DrawingStateBlock.</summary> 
	/// <param name="drawingStateBlock">When this method returns, contains the current drawing state of the render target. This parameter must be initialized before passing it to the method.</param>
	void SaveDrawingState(ID2D1DrawingStateBlock& drawingStateBlock) const;
	
	/// <summary>
	/// Sets the render target's drawing state to that of the specified ID2D1DrawingStateBlock.</summary> 
	/// <param name="drawingStateBlock">The new drawing state of the render target.</param>
	void RestoreDrawingState(ID2D1DrawingStateBlock& drawingStateBlock);

	/// <summary>
	/// Removes the last axis-aligned clip from the render target. After this method is called, the clip is no longer applied to subsequent drawing operations.</summary> 
	/// <param name="rectClip">The size and position of the clipping area, in device-independent pixels.</param>
	/// <param name="mode">The antialiasing mode that is used to draw the edges of clip rects that have subpixel boundaries, and to blend the clip with the scene contents. The blending is performed once when the PopAxisAlignedClip method is called, and does not apply to each primitive within the layer.</param>
	void PushAxisAlignedClip(const CD2DRectF& rectClip, D2D1_ANTIALIAS_MODE mode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	
	/// <summary>
	/// Removes the last axis-aligned clip from the render target. After this method is called, the clip is no longer applied to subsequent drawing operations.</summary> 
	void PopAxisAlignedClip();

protected:
	/// <summary>
	/// Verifies CD2DResource object validity; creates the object if it didn't already exist.</summary>
	/// <param name="pResource">Pointer to CD2DResource object.</param>
	/// <returns> 
	/// TRUE is object if valid; otherwise FALSE.</returns>
	BOOL VerifyResource(CD2DResource* pResource);

	/// <summary>
	/// A pointer to an ID2D1RenderTarget object.
	/// </summary>
	ID2D1RenderTarget* m_pRenderTarget;

	/// <summary>
	/// A list of pointers to CD2DResource objects.
	/// </summary>
	CObList	m_lstResources;

	/// <summary>
	/// A pointer to CD2DTextFormat object that contains a default text format.
	/// </summary>
	CD2DTextFormat* m_pTextFormatDefault;
};

/// <summary>
/// ID2D1HwndRenderTarget wrapper.
/// </summary>
class CHwndRenderTarget : public CRenderTarget
{
	DECLARE_DYNAMIC(CHwndRenderTarget)

public:
	/// <summary>
	/// Constructs a CHwndRenderTarget object from HWND.</summary>
	/// <param name="hwnd">The HWND associated with this render target</param>
	CHwndRenderTarget(HWND hwnd = NULL);

	/// <summary>
	/// Attaches existing render target interface to the object</summary>
	/// <param name="pTarget">Existing render target interface. Cannot be NULL</param>
	void Attach(ID2D1HwndRenderTarget* pTarget);
	
	/// <summary>
	/// Detaches render target interface from the object</summary>
	/// <returns> 
	/// Pointer to detached render target interface.</returns>
	ID2D1HwndRenderTarget* Detach();
	
	/// <summary>
	/// Creates a render target associated with the window </summary> 
	/// <returns>If the method succeeds, it returns TRUE. Otherwise, it returns FALSE</returns>
	/// <param name="hWnd">The HWND associated with this render target</param>
	BOOL Create(HWND hWnd);
	
	/// <summary>
	/// Re-creates a render target associated with the window</summary> 
	/// <returns>If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="hWnd">The HWND associated with this render target</param>
	BOOL ReCreate(HWND hWnd);

	/// <summary>
	/// Returns ID2D1HwndRenderTarget interface.</summary>
	/// <returns>
	/// Pointer to an ID2D1HwndRenderTarget interface or NULL if object is not initialized yet.</returns>
	ID2D1HwndRenderTarget* GetHwndRenderTarget() { return m_pHwndRenderTarget; }

	/// <summary>
	/// Returns ID2D1HwndRenderTarget interface.</summary>
	/// <returns>
	/// Pointer to an ID2D1HwndRenderTarget interface or NULL if object is not initialized yet.</returns>
	operator ID2D1HwndRenderTarget*() { return m_pHwndRenderTarget; }

	/// <summary>
	/// Returns the HWND associated with this render target.</summary>
	/// <returns>
	/// The HWND associated with this render target.</returns>
	HWND GetHwnd() const { return m_pHwndRenderTarget == NULL ? NULL : m_pHwndRenderTarget->GetHwnd(); }

	/// <summary>
	/// Indicates whether the HWND associated with this render target is occluded.</summary>
	/// <returns>
	/// A value that indicates whether the HWND associated with this render target is occluded.</returns>
	D2D1_WINDOW_STATE CheckWindowState() const { return m_pHwndRenderTarget == NULL ? D2D1_WINDOW_STATE_NONE : m_pHwndRenderTarget->CheckWindowState(); }

	/// <summary>
	/// Changes the size of the render target to the specified pixel size</summary> 
	/// <returns>If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="size">The new size of the render target in device pixels</param>
	BOOL Resize(const CD2DSizeU& size);

protected:
	/// <summary>
	/// A pointer to an ID2D1HwndRenderTarget object.
	/// </summary>
	ID2D1HwndRenderTarget*	m_pHwndRenderTarget;
};

/// <summary>
/// ID2D1DCRenderTarget wrapper.
/// </summary>
class CDCRenderTarget : public CRenderTarget
{
	DECLARE_DYNAMIC(CDCRenderTarget)

public:
	/// <summary>
	/// Constructs a CDCRenderTarget object.</summary>
	CDCRenderTarget();

	/// <summary>
	/// Attaches existing render target interface to the object</summary>
	/// <param name="pTarget">Existing render target interface. Cannot be NULL</param>
	void Attach(ID2D1DCRenderTarget* pTarget);
	
	/// <summary>
	/// Detaches render target interface from the object</summary>
	/// <returns> 
	/// Pointer to detached render target interface.</returns>
	ID2D1DCRenderTarget* Detach();

	/// <summary>
	/// Creates a CDCRenderTarget.</summary>
	/// <returns>
	/// If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="props">The rendering mode, pixel format, remoting options, DPI information, and the minimum DirectX support required for hardware rendering.</param>
	BOOL Create(const D2D1_RENDER_TARGET_PROPERTIES& props);
	
	/// <summary>
	/// Binds the render target to the device context to which it issues drawing commands</summary> 
	/// <returns>If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="dc">The device context to which the render target issues drawing commands</param>
	/// <param name="rect">The dimensions of the handle to a device context (HDC) to which the render target is bound</param>
	BOOL BindDC(const CDC& dc, const CRect& rect);

	/// <summary>
	/// Returns ID2D1DCRenderTarget interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1DCRenderTarget interface or NULL if object is not initialized yet.</returns>
	ID2D1DCRenderTarget* GetDCRenderTarget() { return m_pDCRenderTarget; }

	/// <summary>
	/// Returns ID2D1DCRenderTarget interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1DCRenderTarget interface or NULL if object is not initialized yet.</returns>
	operator ID2D1DCRenderTarget*() { return m_pDCRenderTarget; }

protected:
	/// <summary>
	/// A pointer to an ID2D1DCRenderTarget object.
	/// </summary>
	ID2D1DCRenderTarget* m_pDCRenderTarget;
};

class CBitmapRenderTarget : public CRenderTarget
{
	DECLARE_DYNAMIC(CBitmapRenderTarget)

	friend class CRenderTarget;

public:
	/// <summary>
	/// Constructs a CBitmapRenderTarget object.</summary>
	CBitmapRenderTarget() { m_pBitmapRenderTarget = NULL; }

public:
	/// <summary>
	/// Attaches existing render target interface to the object</summary>
	/// <param name="pTarget">Existing render target interface. Cannot be NULL</param>
	void Attach(ID2D1BitmapRenderTarget* pTarget);
	
	/// <summary>
	/// Detaches render target interface from the object</summary>
	/// <returns> 
	/// Pointer to detached render target interface.</returns>
	ID2D1BitmapRenderTarget* Detach();

	/// <summary>
	/// Returns ID2D1BitmapRenderTarget interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1BitmapRenderTarget interface or NULL if object is not initialized yet.</returns>
	ID2D1BitmapRenderTarget* GetBitmapRenderTarget() { return m_pBitmapRenderTarget; }

	/// <summary>
	/// Returns ID2D1BitmapRenderTarget interface</summary>
	/// <returns> 
	/// Pointer to an ID2D1BitmapRenderTarget interface or NULL if object is not initialized yet.</returns>
	operator ID2D1BitmapRenderTarget*() { return m_pBitmapRenderTarget; }

	/// <summary>
	/// Retrieves the bitmap for this render target. The returned bitmap can be used for drawing operations. </summary>
	/// <returns>If the method succeeds, it returns TRUE. Otherwise, it returns FALSE.</returns>
	/// <param name="bitmap">When this method returns, contains the valid bitmap for this render target. This bitmap can be used for drawing operations.</param>
	BOOL GetBitmap(CD2DBitmap& bitmap);

protected:
	/// <summary>
	/// A pointer to an ID2D1BitmapRenderTarget object.
	/// </summary>
	ID2D1BitmapRenderTarget* m_pBitmapRenderTarget;
};

/*============================================================================*/
// _AFX_D2D_STATE

typedef HRESULT (WINAPI * D2D1MAKEROTATEMATRIX)(FLOAT angle, D2D1_POINT_2F center, D2D1_MATRIX_3X2_F *matrix);

class _AFX_D2D_STATE : public CNoTrackObject
{
public:
	_AFX_D2D_STATE();
	virtual ~_AFX_D2D_STATE();

	BOOL IsD2DInitialized() const { return m_bD2DInitialized; }

	BOOL InitD2D(D2D1_FACTORY_TYPE d2dFactoryType = D2D1_FACTORY_TYPE_SINGLE_THREADED, DWRITE_FACTORY_TYPE writeFactoryType = DWRITE_FACTORY_TYPE_SHARED);
	void ReleaseD2DRefs();

	ID2D1Factory* GetDirect2dFactory()	{ InitD2D(); return m_pDirect2dFactory; }
	IDWriteFactory* GetWriteFactory()	{ InitD2D(); return m_pWriteFactory; }
	IWICImagingFactory* GetWICFactory()	{ InitD2D(); return m_pWicFactory; }

	HRESULT D2D1MakeRotateMatrix(FLOAT angle, D2D1_POINT_2F center, D2D1_MATRIX_3X2_F *matrix)
	{
		return m_pfD2D1MakeRotateMatrix == NULL ? E_FAIL : (*m_pfD2D1MakeRotateMatrix)(angle, center, matrix);
	}

protected:
	HINSTANCE m_hinstD2DDLL;
	HINSTANCE m_hinstDWriteDLL;

	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* m_pWriteFactory;
	IWICImagingFactory* m_pWicFactory;

	D2D1MAKEROTATEMATRIX m_pfD2D1MakeRotateMatrix;

	BOOL m_bD2DInitialized;
	BOOL m_bComInitialized;
};

AFX_DATA EXTERN_PROCESS_LOCAL(_AFX_D2D_STATE, _afxD2DState)

_AFX_D2D_STATE* AFX_CDECL AfxGetD2DState();
void AFX_CDECL AfxReleaseD2DRefs();

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
