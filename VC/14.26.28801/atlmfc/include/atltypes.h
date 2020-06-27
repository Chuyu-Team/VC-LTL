// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __ATLTYPES_H__
#define __ATLTYPES_H__

#pragma once

#include <atldef.h>
#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file

class CSize;
class CPoint;
class CRect;

/////////////////////////////////////////////////////////////////////////////
// CSize - An extent, similar to Windows SIZE structure.

class CSize :
	public tagSIZE
{
public:

// Constructors
	// construct an uninitialized size
	CSize() throw();
	// create from two integers
	CSize(
		_In_ int initCX,
		_In_ int initCY) throw();
	// create from another size
	CSize(_In_ SIZE initSize) throw();
	// create from a point
	CSize(_In_ POINT initPt) throw();
	// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
	CSize(_In_ DWORD dwSize) throw();

// Operations
	BOOL operator==(_In_ SIZE size) const throw();
	BOOL operator!=(_In_ SIZE size) const throw();
	void operator+=(_In_ SIZE size) throw();
	void operator-=(_In_ SIZE size) throw();
	void SetSize(_In_ int CX, _In_ int CY) throw();

// Operators returning CSize values
	CSize operator+(_In_ SIZE size) const throw();
	CSize operator-(_In_ SIZE size) const throw();
	CSize operator-() const throw();

// Operators returning CPoint values
	CPoint operator+(_In_ POINT point) const throw();
	CPoint operator-(_In_ POINT point) const throw();

// Operators returning CRect values
	CRect operator+(_In_ const RECT* lpRect) const throw();
	CRect operator-(_In_ const RECT* lpRect) const throw();
};

/////////////////////////////////////////////////////////////////////////////
// CPoint - A 2-D point, similar to Windows POINT structure.

class CPoint :
	public tagPOINT
{
public:
// Constructors

	// create an uninitialized point
	CPoint() throw();
	// create from two integers
	CPoint(
		_In_ int initX,
		_In_ int initY) throw();
	// create from another point
	CPoint(_In_ POINT initPt) throw();
	// create from a size
	CPoint(_In_ SIZE initSize) throw();
	// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
	CPoint(_In_ LPARAM dwPoint) throw();


// Operations

// translate the point
	void Offset(
		_In_ int xOffset,
		_In_ int yOffset) throw();
	void Offset(_In_ POINT point) throw();
	void Offset(_In_ SIZE size) throw();
	void SetPoint(
		_In_ int X,
		_In_ int Y) throw();

	BOOL operator==(_In_ POINT point) const throw();
	BOOL operator!=(_In_ POINT point) const throw();
	void operator+=(_In_ SIZE size) throw();
	void operator-=(_In_ SIZE size) throw();
	void operator+=(_In_ POINT point) throw();
	void operator-=(_In_ POINT point) throw();

// Operators returning CPoint values
	CPoint operator+(_In_ SIZE size) const throw();
	CPoint operator-(_In_ SIZE size) const throw();
	CPoint operator-() const throw();
	CPoint operator+(_In_ POINT point) const throw();

// Operators returning CSize values
	CSize operator-(_In_ POINT point) const throw();

// Operators returning CRect values
	CRect operator+(_In_ const RECT* lpRect) const throw();
	CRect operator-(_In_ const RECT* lpRect) const throw();
};

/////////////////////////////////////////////////////////////////////////////
// CRect - A 2-D rectangle, similar to Windows RECT structure.

class CRect :
	public tagRECT
{
// Constructors
public:
	// uninitialized rectangle
	CRect() throw();
	// from left, top, right, and bottom
	CRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();
	// copy constructor
	CRect(_In_ const RECT& srcRect) throw();

	// from a pointer to another rect
	CRect(_In_ LPCRECT lpSrcRect) throw();
	// from a point and size
	CRect(
		_In_ POINT point,
		_In_ SIZE size) throw();
	// from two points
	CRect(
		_In_ POINT topLeft,
		_In_ POINT bottomRight) throw();

// Attributes (in addition to RECT members)

	// retrieves the width
	int Width() const throw();
	// returns the height
	int Height() const throw();
	// returns the size
	CSize Size() const throw();
	// reference to the top-left point
	CPoint& TopLeft() throw();
	// reference to the bottom-right point
	CPoint& BottomRight() throw();
	// const reference to the top-left point
	const CPoint& TopLeft() const throw();
	// const reference to the bottom-right point
	const CPoint& BottomRight() const throw();
	// the geometric center point of the rectangle
	CPoint CenterPoint() const throw();
	// swap the left and right
	void SwapLeftRight() throw();
	static void WINAPI SwapLeftRight(_Inout_ LPRECT lpRect) throw();

	// convert between CRect and LPRECT/LPCRECT (no need for &)
	operator LPRECT() throw();
	operator LPCRECT() const throw();

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const throw();
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const throw();
	// returns TRUE if point is within rectangle
	BOOL PtInRect(_In_ POINT point) const throw();

// Operations

	// set rectangle from left, top, right, and bottom
	void SetRect(
		_In_ int x1,
		_In_ int y1,
		_In_ int x2,
		_In_ int y2) throw();
	void SetRect(
		_In_ POINT topLeft,
		_In_ POINT bottomRight) throw();
	// empty the rectangle
	void SetRectEmpty() throw();
	// copy from another rectangle
	void CopyRect(_In_ LPCRECT lpSrcRect) throw();
	// TRUE if exactly the same as another rectangle
	BOOL EqualRect(_In_ LPCRECT lpRect) const throw();

	// Inflate rectangle's width and height by
	// x units to the left and right ends of the rectangle
	// and y units to the top and bottom.
	void InflateRect(
		_In_ int x,
		_In_ int y) throw();
	// Inflate rectangle's width and height by
	// size.cx units to the left and right ends of the rectangle
	// and size.cy units to the top and bottom.
	void InflateRect(_In_ SIZE size) throw();
	// Inflate rectangle's width and height by moving individual sides.
	// Left side is moved to the left, right side is moved to the right,
	// top is moved up and bottom is moved down.
	void InflateRect(_In_ LPCRECT lpRect) throw();
	void InflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();

	// deflate the rectangle's width and height without
	// moving its top or left
	void DeflateRect(
		_In_ int x,
		_In_ int y) throw();
	void DeflateRect(_In_ SIZE size) throw();
	void DeflateRect(_In_ LPCRECT lpRect) throw();
	void DeflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw();

	// translate the rectangle by moving its top and left
	void OffsetRect(
		_In_ int x,
		_In_ int y) throw();
	void OffsetRect(_In_ SIZE size) throw();
	void OffsetRect(_In_ POINT point) throw();
	void NormalizeRect() throw();

	// absolute position of rectangle
	void MoveToY(_In_ int y) throw();
	void MoveToX(_In_ int x) throw();
	void MoveToXY(
		_In_ int x,
		_In_ int y) throw();
	void MoveToXY(_In_ POINT point) throw();

	// set this rectangle to intersection of two others
	BOOL IntersectRect(
		_In_ LPCRECT lpRect1,
		_In_ LPCRECT lpRect2) throw();

	// set this rectangle to bounding union of two others
	BOOL UnionRect(
		_In_ LPCRECT lpRect1,
		_In_ LPCRECT lpRect2) throw();

	// set this rectangle to minimum of two others
	BOOL SubtractRect(
		_In_ LPCRECT lpRectSrc1,
		_In_ LPCRECT lpRectSrc2) throw();

// Additional Operations
	void operator=(_In_ const RECT& srcRect) throw();
	BOOL operator==(_In_ const RECT& rect) const throw();
	BOOL operator!=(_In_ const RECT& rect) const throw();
	void operator+=(_In_ POINT point) throw();
	void operator+=(_In_ SIZE size) throw();
	void operator+=(_In_ LPCRECT lpRect) throw();
	void operator-=(_In_ POINT point) throw();
	void operator-=(_In_ SIZE size) throw();
	void operator-=(_In_ LPCRECT lpRect) throw();
	void operator&=(_In_ const RECT& rect) throw();
	void operator|=(_In_ const RECT& rect) throw();

// Operators returning CRect values
	CRect operator+(_In_ POINT point) const throw();
	CRect operator-(_In_ POINT point) const throw();
	CRect operator+(_In_ LPCRECT lpRect) const throw();
	CRect operator+(_In_ SIZE size) const throw();
	CRect operator-(_In_ SIZE size) const throw();
	CRect operator-(_In_ LPCRECT lpRect) const throw();
	CRect operator&(_In_ const RECT& rect2) const throw();
	CRect operator|(_In_ const RECT& rect2) const throw();
	CRect MulDiv(
		_In_ int nMultiplier,
		_In_ int nDivisor) const throw();
};

#ifndef _ATL_STATIC_LIB_IMPL

// CSize
inline CSize::CSize() throw()
{
	cx = 0;
	cy = 0;
}

inline CSize::CSize(
	_In_ int initCX,
	_In_ int initCY) throw()
{
	cx = initCX;
	cy = initCY;
}

inline CSize::CSize(_In_ SIZE initSize) throw()
{
	*(SIZE*)this = initSize;
}

inline CSize::CSize(_In_ POINT initPt) throw()
{
	*(POINT*)this = initPt;
}

inline CSize::CSize(_In_ DWORD dwSize) throw()
{
		cx = (short)LOWORD(dwSize);
		cy = (short)HIWORD(dwSize);
}

inline BOOL CSize::operator==(_In_ SIZE size) const throw()
{
	return (cx == size.cx && cy == size.cy);
}

inline BOOL CSize::operator!=(_In_ SIZE size) const throw()
{
	return (cx != size.cx || cy != size.cy);
}

inline void CSize::operator+=(_In_ SIZE size) throw()
{
	cx += size.cx;
	cy += size.cy;
}

inline void CSize::operator-=(_In_ SIZE size) throw()
{
	cx -= size.cx;
	cy -= size.cy;
}

inline void CSize::SetSize(
	_In_ int CX,
	_In_ int CY) throw()
{
	cx = CX;
	cy = CY;
}

inline CSize CSize::operator+(_In_ SIZE size) const throw()
{
	return CSize(cx + size.cx, cy + size.cy);
}

inline CSize CSize::operator-(_In_ SIZE size) const throw()
{
	return CSize(cx - size.cx, cy - size.cy);
}

inline CSize CSize::operator-() const throw()
{
	return CSize(-cx, -cy);
}

inline CPoint CSize::operator+(_In_ POINT point) const throw()
{
	return CPoint(cx + point.x, cy + point.y);
}

inline CPoint CSize::operator-(_In_ POINT point) const throw()
{
	return CPoint(cx - point.x, cy - point.y);
}

inline CRect CSize::operator+(_In_ const RECT* lpRect) const throw()
{
	return CRect(lpRect) + *this;
}

inline CRect CSize::operator-(_In_ const RECT* lpRect) const throw()
{
	return CRect(lpRect) - *this;
}

// CPoint
inline CPoint::CPoint() throw()
{
	x = 0;
	y = 0;
}

inline CPoint::CPoint(
	_In_ int initX,
	_In_ int initY) throw()
{
	x = initX;
	y = initY;
}

inline CPoint::CPoint(_In_ POINT initPt) throw()
{
	*(POINT*)this = initPt;
}

inline CPoint::CPoint(_In_ SIZE initSize) throw()
{
	*(SIZE*)this = initSize;
}

inline CPoint::CPoint(_In_ LPARAM dwPoint) throw()
{
	x = (short)LOWORD(dwPoint);
	y = (short)HIWORD(dwPoint);
}

inline void CPoint::Offset(
	_In_ int xOffset,
	_In_ int yOffset) throw()
{
	x += xOffset;
	y += yOffset;
}

inline void CPoint::Offset(_In_ POINT point) throw()
{
	x += point.x;
	y += point.y;
}

inline void CPoint::Offset(_In_ SIZE size) throw()
{
	x += size.cx;
	y += size.cy;
}

inline void CPoint::SetPoint(
	_In_ int X,
	_In_ int Y) throw()
{
	x = X;
	y = Y;
}

inline BOOL CPoint::operator==(_In_ POINT point) const throw()
{
	return (x == point.x && y == point.y);
}

inline BOOL CPoint::operator!=(_In_ POINT point) const throw()
{
	return (x != point.x || y != point.y);
}

inline void CPoint::operator+=(_In_ SIZE size) throw()
{
	x += size.cx;
	y += size.cy;
}

inline void CPoint::operator-=(_In_ SIZE size) throw()
{
	x -= size.cx;
	y -= size.cy;
}

inline void CPoint::operator+=(_In_ POINT point) throw()
{
	x += point.x;
	y += point.y;
}

inline void CPoint::operator-=(_In_ POINT point) throw()
{
	x -= point.x;
	y -= point.y;
}

inline CPoint CPoint::operator+(_In_ SIZE size) const throw()
{
	return CPoint(x + size.cx, y + size.cy);
}

inline CPoint CPoint::operator-(_In_ SIZE size) const throw()
{
	return CPoint(x - size.cx, y - size.cy);
}

inline CPoint CPoint::operator-() const throw()
{
	return CPoint(-x, -y);
}

inline CPoint CPoint::operator+(_In_ POINT point) const throw()
{
	return CPoint(x + point.x, y + point.y);
}

inline CSize CPoint::operator-(_In_ POINT point) const throw()
{
	return CSize(x - point.x, y - point.y);
}

inline CRect CPoint::operator+(_In_ const RECT* lpRect) const throw()
{
	return CRect(lpRect) + *this;
}

inline CRect CPoint::operator-(_In_ const RECT* lpRect) const throw()
{
	return CRect(lpRect) - *this;
}

// CRect
inline CRect::CRect() throw()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}

inline CRect::CRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

inline CRect::CRect(_In_ const RECT& srcRect) throw()
{
	::CopyRect(this, &srcRect);
}

inline CRect::CRect(_In_ LPCRECT lpSrcRect) throw()
{
	::CopyRect(this, lpSrcRect);
}

inline CRect::CRect(
	_In_ POINT point,
	_In_ SIZE size) throw()
{
	right = (left = point.x) + size.cx;
	bottom = (top = point.y) + size.cy;
}

inline CRect::CRect(
	_In_ POINT topLeft,
	_In_ POINT bottomRight) throw()
{
	left = topLeft.x;
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y;
}

inline int CRect::Width() const throw()
{
	return right - left;
}

inline int CRect::Height() const throw()
{
	return bottom - top;
}

inline CSize CRect::Size() const throw()
{
	return CSize(right - left, bottom - top);
}

inline CPoint& CRect::TopLeft() throw()
{
	return *((CPoint*)this);
}

inline CPoint& CRect::BottomRight() throw()
{
	return *((CPoint*)this+1);
}

inline const CPoint& CRect::TopLeft() const throw()
{
	return *((CPoint*)this);
}

inline const CPoint& CRect::BottomRight() const throw()
{
	return *((CPoint*)this+1);
}

inline CPoint CRect::CenterPoint() const throw()
{
	return CPoint((left+right)/2, (top+bottom)/2);
}

inline void CRect::SwapLeftRight() throw()
{
	SwapLeftRight(LPRECT(this));
}

inline void WINAPI CRect::SwapLeftRight(_Inout_ LPRECT lpRect) throw()
{
	LONG temp = lpRect->left;
	lpRect->left = lpRect->right;
	lpRect->right = temp;
}

inline CRect::operator LPRECT() throw()
{
	return this;
}

inline CRect::operator LPCRECT() const throw()
{
	return this;
}

inline BOOL CRect::IsRectEmpty() const throw()
{
	return ::IsRectEmpty(this);
}

inline BOOL CRect::IsRectNull() const throw()
{
	return (left == 0 && right == 0 && top == 0 && bottom == 0);
}

inline BOOL CRect::PtInRect(_In_ POINT point) const throw()
{
	return ::PtInRect(this, point);
}

inline void CRect::SetRect(
	_In_ int x1,
	_In_ int y1,
	_In_ int x2,
	_In_ int y2) throw()
{
	::SetRect(this, x1, y1, x2, y2);
}

inline void CRect::SetRect(
	_In_ POINT topLeft,
	_In_ POINT bottomRight) throw()
{
	::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
}

inline void CRect::SetRectEmpty() throw()
{
	::SetRectEmpty(this);
}

inline void CRect::CopyRect(_In_ LPCRECT lpSrcRect) throw()
{
	::CopyRect(this, lpSrcRect);
}

inline BOOL CRect::EqualRect(_In_ LPCRECT lpRect) const throw()
{
	return ::EqualRect(this, lpRect);
}

inline void CRect::InflateRect(
	_In_ int x,
	_In_ int y) throw()
{
	::InflateRect(this, x, y);
}

inline void CRect::InflateRect(_In_ SIZE size) throw()
{
	::InflateRect(this, size.cx, size.cy);
}

inline void CRect::DeflateRect(
	_In_ int x,
	_In_ int y) throw()
{
	::InflateRect(this, -x, -y);
}

inline void CRect::DeflateRect(_In_ SIZE size) throw()
{
	::InflateRect(this, -size.cx, -size.cy);
}

inline void CRect::OffsetRect(
	_In_ int x,
	_In_ int y) throw()
{
	::OffsetRect(this, x, y);
}

inline void CRect::OffsetRect(_In_ POINT point) throw()
{
	::OffsetRect(this, point.x, point.y);
}

inline void CRect::OffsetRect(_In_ SIZE size) throw()
{
	::OffsetRect(this, size.cx, size.cy);
}

inline void CRect::MoveToY(_In_ int y) throw()
{
	bottom = Height() + y;
	top = y;
}

inline void CRect::MoveToX(_In_ int x) throw()
{
	right = Width() + x;
	left = x;
}

inline void CRect::MoveToXY(
	_In_ int x,
	_In_ int y) throw()
{
	MoveToX(x);
	MoveToY(y);
}

inline void CRect::MoveToXY(_In_ POINT pt) throw()
{
	MoveToX(pt.x);
	MoveToY(pt.y);
}

inline BOOL CRect::IntersectRect(
	_In_ LPCRECT lpRect1,
	_In_ LPCRECT lpRect2) throw()
{
	return ::IntersectRect(this, lpRect1, lpRect2);
}

inline BOOL CRect::UnionRect(
	_In_ LPCRECT lpRect1,
	_In_ LPCRECT lpRect2) throw()
{
	return ::UnionRect(this, lpRect1, lpRect2);
}

inline void CRect::operator=(_In_ const RECT& srcRect) throw()
{
	::CopyRect(this, &srcRect);
}

inline BOOL CRect::operator==(_In_ const RECT& rect) const throw()
{
	return ::EqualRect(this, &rect);
}

inline BOOL CRect::operator!=(_In_ const RECT& rect) const throw()
{
	return !::EqualRect(this, &rect);
}

inline void CRect::operator+=(_In_ POINT point) throw()
{
	::OffsetRect(this, point.x, point.y);
}

inline void CRect::operator+=(_In_ SIZE size) throw()
{
	::OffsetRect(this, size.cx, size.cy);
}

inline void CRect::operator+=(_In_ LPCRECT lpRect) throw()
{
	InflateRect(lpRect);
}

inline void CRect::operator-=(_In_ POINT point) throw()
{
	::OffsetRect(this, -point.x, -point.y);
}

inline void CRect::operator-=(_In_ SIZE size) throw()
{
	::OffsetRect(this, -size.cx, -size.cy);
}

inline void CRect::operator-=(_In_ LPCRECT lpRect) throw()
{
	DeflateRect(lpRect);
}

inline void CRect::operator&=(_In_ const RECT& rect) throw()
{
	::IntersectRect(this, this, &rect);
}

inline void CRect::operator|=(_In_ const RECT& rect) throw()
{
	::UnionRect(this, this, &rect);
}

inline CRect CRect::operator+(_In_ POINT pt) const throw()
{
	CRect rect(*this);
	::OffsetRect(&rect, pt.x, pt.y);
	return rect;
}

inline CRect CRect::operator-(_In_ POINT pt) const throw()
{
	CRect rect(*this);
	::OffsetRect(&rect, -pt.x, -pt.y);
	return rect;
}

inline CRect CRect::operator+(_In_ SIZE size) const throw()
{
	CRect rect(*this);
	::OffsetRect(&rect, size.cx, size.cy);
	return rect;
}

inline CRect CRect::operator-(_In_ SIZE size) const throw()
{
	CRect rect(*this);
	::OffsetRect(&rect, -size.cx, -size.cy);
	return rect;
}

inline CRect CRect::operator+(_In_ LPCRECT lpRect) const throw()
{
	CRect rect(this);
	rect.InflateRect(lpRect);
	return rect;
}

inline CRect CRect::operator-(_In_ LPCRECT lpRect) const throw()
{
	CRect rect(this);
	rect.DeflateRect(lpRect);
	return rect;
}

inline CRect CRect::operator&(_In_ const RECT& rect2) const throw()
{
	CRect rect;
	::IntersectRect(&rect, this, &rect2);
	return rect;
}

inline CRect CRect::operator|(_In_ const RECT& rect2) const throw()
{
	CRect rect;
	::UnionRect(&rect, this, &rect2);
	return rect;
}

inline BOOL CRect::SubtractRect(
	_In_ LPCRECT lpRectSrc1,
	_In_ LPCRECT lpRectSrc2) throw()
{
	return ::SubtractRect(this, lpRectSrc1, lpRectSrc2);
}

inline void CRect::NormalizeRect() throw()
{
	int nTemp;
	if (left > right)
	{
		nTemp = left;
		left = right;
		right = nTemp;
	}
	if (top > bottom)
	{
		nTemp = top;
		top = bottom;
		bottom = nTemp;
	}
}

inline void CRect::InflateRect(_In_ LPCRECT lpRect) throw()
{
	left -= lpRect->left;
	top -= lpRect->top;
	right += lpRect->right;
	bottom += lpRect->bottom;
}

inline void CRect::InflateRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left -= l;
	top -= t;
	right += r;
	bottom += b;
}

inline void CRect::DeflateRect(_In_ LPCRECT lpRect) throw()
{
	left += lpRect->left;
	top += lpRect->top;
	right -= lpRect->right;
	bottom -= lpRect->bottom;
}

inline void CRect::DeflateRect(
	_In_ int l,
	_In_ int t,
	_In_ int r,
	_In_ int b) throw()
{
	left += l;
	top += t;
	right -= r;
	bottom -= b;
}

inline CRect CRect::MulDiv(
	_In_ int nMultiplier,
	_In_ int nDivisor) const throw()
{
	return CRect(
		::MulDiv(left, nMultiplier, nDivisor),
		::MulDiv(top, nMultiplier, nDivisor),
		::MulDiv(right, nMultiplier, nDivisor),
		::MulDiv(bottom, nMultiplier, nDivisor));
}

#endif // _ATL_STATIC_LIB_IMPL

#endif // __ATLTYPES_H__
