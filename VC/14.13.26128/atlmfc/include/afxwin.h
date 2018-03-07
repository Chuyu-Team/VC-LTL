// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXWIN_H__
#ifndef RC_INVOKED
#define __AFXWIN_H__

/*============================================================================*/
// Make sure 'afx.h' is included first

#ifndef __AFX_H__
	#include <afx.h>
#endif

// Note: WINDOWS.H already included from AFXV_W32.H
#ifndef NTDDI_LONGHORN
#define NTDDI_LONGHORN 0x06000000
#if (WINVER >= 0x0600) || (_WIN32_WINNT >= 0x0600)
#error Your version of the Windows SDK is earlier than 6.0. Try setting the 'WINVER' and '_WIN32_WINNT' definitions in your project to less than 0x0600.
#endif
#endif

#ifndef _INC_SHELLAPI
	#include <shellapi.h>
#endif

#ifndef __AFXRES_H__
	#include <afxres.h>     // standard resource IDs
#endif

#ifndef __AFXCOLL_H__
	#include <afxcoll.h>    // standard collections
#endif

#ifndef _OBJBASE_H_
	#include <objbase.h> //needed for commdlg.h (STDMETHOD)
#endif

#ifndef _INC_COMMDLG
	#include <commdlg.h>    // common dialog APIs
#endif

#include <afxctrlcontainer.h>

#ifndef _WIN32_IE
#define _WIN32_IE 0x0700
#else
#undef _WIN32_IE
#define _WIN32_IE 0x0700
#endif

#ifndef __shobjidl_h__
	#include <ShObjIdl.h>    // for IPreviewHandler, IPreviewHandlerVisuals
#endif

#ifndef __IThumbnailProvider_INTERFACE_DEFINED__
	#include <thumbcache.h>  // for IThumbnailProvider
#endif

#include <atlhandler.h> // for IFilterChunkValue and IDocument

#if (NTDDI_VERSION >= NTDDI_WIN7)
#ifndef __tpcshrd_h__
	#include <tpcshrd.h>	// for touch and gesture features
#endif
#endif

// Avoid mapping GetFileTitle to GetFileTitle[A/W]
#ifdef GetFileTitle
#undef GetFileTitle
AFX_INLINE short APIENTRY GetFileTitle(LPCTSTR lpszFile, LPTSTR lpszTitle, WORD cbBuf)
#ifdef UNICODE
	{ return ::GetFileTitleW(lpszFile, lpszTitle, cbBuf); }
#else
	{ return ::GetFileTitleA(lpszFile, lpszTitle, cbBuf); }
#endif
#endif

#include <vssym32.h>

#ifndef _WINSOCK2API_
#ifdef _WINSOCKAPI_
	#error MFC requires use of Winsock2.h
#endif
	#include <WinSock2.h>
#endif

#ifndef _WS2IPDEF_
	#include <ws2ipdef.h>
#endif

#ifndef _WINDNS_INCLUDED_
	#include <WinDNS.h>
#endif

#ifndef IPV6STRICT
#ifndef __IPHLPAPI_H__
	#include <iphlpapi.h>
#endif
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

#ifndef _AFX_NOFORCE_LIBS
#pragma comment(lib, "uuid.lib")
#endif

#include <dwmapi.h>

#ifdef _INC_WINDOWSX
// The following names from WINDOWSX.H collide with names in this header
#undef SubclassWindow
#undef CopyRgn
#endif

#include <HtmlHelp.h>

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif


#ifndef __UIAnimation_h__
	#include <UIAnimation.h> // Windows Animation API interfaces
	#include "afxanimationcontroller.h" // animation controller - MFC Windows Animation API implementation
#endif // __UIAnimation_h__

#pragma warning( push )
#pragma warning( disable: 4121 )

using ATL::CAtlTransactionManager;

/*============================================================================*/
// Classes declared in this file

//CObject
	//CException
		//CSimpleException
			class CResourceException;// Win resource failure exception
			class CUserException;    // Message Box alert and stop operation

	class CGdiObject;            // CDC drawing tool
		class CPen;              // a pen / HPEN wrapper
		class CBrush;            // a brush / HBRUSH wrapper
		class CFont;             // a font / HFONT wrapper
		class CBitmap;           // a bitmap / HBITMAP wrapper
		class CPalette;          // a palette / HPALLETE wrapper
		class CRgn;              // a region / HRGN wrapper

	class CDC;                   // a Display Context / HDC wrapper
		class CClientDC;         // CDC for client of window
		class CWindowDC;         // CDC for entire window
		class CPaintDC;          // embeddable BeginPaint struct helper

	class CImageList;            // an image list / HIMAGELIST wrapper

	class CMenu;                 // a menu / HMENU wrapper

	class CCmdTarget;            // a target for user commands
		class CWnd;                 // a window / HWND wrapper
			class CDialog;          // a dialog

			// standard windows controls
			class CStatic;          // Static control
			class CButton;          // Button control
			class CListBox;         // ListBox control
				class CCheckListBox;// special listbox with checks
			class CComboBox;        // ComboBox control
			class CEdit;            // Edit control
			class CScrollBar;       // ScrollBar control

			class CMFCPreviewCtrlImpl; // helper window for DLL implementation of Rich Preview

			// frame windows
			class CFrameWnd;        // standard SDI frame
				class CMDIFrameWnd; // standard MDI frame
				class CMDIChildWnd; // standard MDI child
				class CMiniFrameWnd;// half-height caption frame wnd

			// views on a document
			class CView;            // a view on a document
				class CScrollView;  // a scrolling view

		class CWinThread;           // thread base class
			class CWinApp;          // application base class

		class CDocTemplate;         // template for document creation
			class CSingleDocTemplate;// SDI support
			class CMultiDocTemplate; // MDI support

		class CDocument;            // main document abstraction
		class CMFCFilterChunkValueImpl; // search/organize/preview/thumbnail support - filter chunk value implementation


// Helper classes
class CCmdUI;           // Menu/button enabling
class CDataExchange;    // Data exchange and validation context
class CCommandLineInfo; // CommandLine parsing helper
class CDocManager;      // CDocTemplate manager object

struct COleControlSiteOrWnd; // ActiveX dialog control helper


class CControlCreationInfo; //Used in CWnd::CreateControl overloads.

class CVariantBoolConverter;
class CMFCDynamicLayout;

/*============================================================================*/

enum AFX_HELP_TYPE
{
	afxWinHelp = 0,
	afxHTMLHelp = 1
};

// Type modifier for message handlers
#ifndef afx_msg
#define afx_msg         // intentional placeholder
#endif

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA


#ifndef __ATLTYPES_H__
#include <atltypes.h>
#endif

#ifdef _DEBUG
// Diagnostic Output
CDumpContext& AFXAPI operator<<(CDumpContext& dc, SIZE size);
CDumpContext& AFXAPI operator<<(CDumpContext& dc, POINT point);
CDumpContext& AFXAPI operator<<(CDumpContext& dc, const RECT& rect);
#endif //_DEBUG

// Serialization
CArchive& AFXAPI operator<<(CArchive& ar, SIZE size);
CArchive& AFXAPI operator<<(CArchive& ar, POINT point);
CArchive& AFXAPI operator<<(CArchive& ar, const RECT& rect);
CArchive& AFXAPI operator>>(CArchive& ar, SIZE& size);
CArchive& AFXAPI operator>>(CArchive& ar, POINT& point);
CArchive& AFXAPI operator>>(CArchive& ar, RECT& rect);

// macro to be used in ATL search/organize/preview/thumbnail handlers with MFC document support
#define DECLARE_DOCUMENT(classDocument)\
protected:\
	virtual IDocument* CreateDocument()\
	{\
		CRuntimeClass* pDocRTC = RUNTIME_CLASS(classDocument);\
		if (pDocRTC == NULL)\
		{\
			TRACE("Document class does not support dynamic creation."); \
			return NULL;\
		}\
		classDocument* pDoc = DYNAMIC_DOWNCAST(classDocument, pDocRTC->CreateObject());\
		ASSERT_VALID(pDoc);\
		return pDoc->GetAdapter();\
	}



/*============================================================================*/
// Standard exceptions

class CResourceException : public CSimpleException    // resource failure
{
	DECLARE_DYNAMIC(CResourceException)
public:
	CResourceException();

// Implementation
public:
	explicit CResourceException(BOOL bAutoDelete);
	CResourceException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CResourceException();
};

class CUserException : public CSimpleException   // general user visible alert
{
	DECLARE_DYNAMIC(CUserException)
public:
	CUserException();

// Implementation
public:
	explicit CUserException(BOOL bAutoDelete);
	CUserException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CUserException();
};

void AFXAPI AfxThrowResourceException();
void AFXAPI AfxThrowUserException();

void AFXAPI AfxGetGrayBitmap(const CBitmap &rSrc, CBitmap *pDest, COLORREF crBackground);
void AFXAPI AfxDrawGrayBitmap(CDC *pDC, int x, int y, const CBitmap &rSrc, COLORREF crBackground);
void AFXAPI AfxGetDitheredBitmap(const CBitmap &rSrc, CBitmap *pDest, COLORREF cr1, COLORREF cr2);
void AFXAPI AfxDrawDitheredBitmap(CDC *pDC, int x, int y, const CBitmap &rSrc, COLORREF cr1, COLORREF cr2);

/*============================================================================*/
// CGdiObject abstract class for CDC SelectObject

class CGdiObject : public CObject
{
	DECLARE_DYNCREATE(CGdiObject)
public:

// Attributes
	HGDIOBJ m_hObject;                  // must be first data member
	operator HGDIOBJ() const;
	HGDIOBJ GetSafeHandle() const;

	static CGdiObject* PASCAL FromHandle(HGDIOBJ hObject);
	static void PASCAL DeleteTempMap();
	BOOL Attach(HGDIOBJ hObject);
	HGDIOBJ Detach();

// Constructors
	CGdiObject(); // must Create a derived class object
	BOOL DeleteObject();

// Operations
#pragma push_macro("GetObject")
#undef GetObject
	int _AFX_FUNCNAME(GetObject)(int nCount, LPVOID lpObject) const;
	int GetObject(int nCount, LPVOID lpObject) const;
#pragma pop_macro("GetObject")
	UINT GetObjectType() const;
	BOOL CreateStockObject(int nIndex);
	BOOL UnrealizeObject();
	BOOL operator==(const CGdiObject& obj) const;
	BOOL operator!=(const CGdiObject& obj) const;

// Implementation
public:
	virtual ~CGdiObject();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif
};

/*============================================================================*/
// CGdiObject subclasses (drawing tools)

class CPen : public CGdiObject
{
	DECLARE_DYNAMIC(CPen)

public:
	static CPen* PASCAL FromHandle(HPEN hPen);

// Constructors
	CPen();
	CPen(int nPenStyle, int nWidth, COLORREF crColor);
	CPen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
		int nStyleCount = 0, const DWORD* lpStyle = NULL);
	BOOL CreatePen(int nPenStyle, int nWidth, COLORREF crColor);
	BOOL CreatePen(int nPenStyle, int nWidth, const LOGBRUSH* pLogBrush,
		int nStyleCount = 0, const DWORD* lpStyle = NULL);
	BOOL CreatePenIndirect(LPLOGPEN lpLogPen);

// Attributes
	operator HPEN() const;
	int GetLogPen(LOGPEN* pLogPen);
	int GetExtLogPen(EXTLOGPEN* pLogPen);

// Implementation
public:
	virtual ~CPen();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CBrush : public CGdiObject
{
	DECLARE_DYNAMIC(CBrush)

public:
	static CBrush* PASCAL FromHandle(HBRUSH hBrush);

// Constructors
	CBrush();
	CBrush(COLORREF crColor);             // CreateSolidBrush
	CBrush(int nIndex, COLORREF crColor); // CreateHatchBrush
	explicit CBrush(CBitmap* pBitmap);          // CreatePatternBrush

	BOOL CreateSolidBrush(COLORREF crColor);
	BOOL CreateHatchBrush(int nIndex, COLORREF crColor);
	BOOL CreateBrushIndirect(const LOGBRUSH* lpLogBrush);
	BOOL CreatePatternBrush(CBitmap* pBitmap);
	BOOL CreateDIBPatternBrush(HGLOBAL hPackedDIB, UINT nUsage);
	BOOL CreateDIBPatternBrush(const void* lpPackedDIB, UINT nUsage);
	BOOL CreateSysColorBrush(int nIndex);

// Attributes
	operator HBRUSH() const;
	int GetLogBrush(LOGBRUSH* pLogBrush);

// Implementation
public:
	virtual ~CBrush();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CFont : public CGdiObject
{
	DECLARE_DYNAMIC(CFont)

public:
	static CFont* PASCAL FromHandle(HFONT hFont);

// Constructors
	CFont();
	BOOL CreateFontIndirect(const LOGFONT* lpLogFont);
	BOOL CreateFont(int nHeight, int nWidth, int nEscapement,
			int nOrientation, int nWeight, BYTE bItalic, BYTE bUnderline,
			BYTE cStrikeOut, BYTE nCharSet, BYTE nOutPrecision,
			BYTE nClipPrecision, BYTE nQuality, BYTE nPitchAndFamily,
			LPCTSTR lpszFacename);
	BOOL CreatePointFont(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC = NULL);
	BOOL CreatePointFontIndirect(const LOGFONT* lpLogFont, CDC* pDC = NULL);

// Attributes
	operator HFONT() const;
	int GetLogFont(LOGFONT* pLogFont);

// Implementation
public:
	virtual ~CFont();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CBitmap : public CGdiObject
{
	DECLARE_DYNAMIC(CBitmap)

public:
	static CBitmap* PASCAL FromHandle(HBITMAP hBitmap);

// Constructors
	CBitmap();

	BOOL LoadBitmap(LPCTSTR lpszResourceName);
	BOOL LoadBitmap(UINT nIDResource);
	BOOL LoadOEMBitmap(UINT nIDBitmap); // for OBM_/OCR_/OIC_
	BOOL LoadMappedBitmap(UINT nIDBitmap, UINT nFlags = 0,
		LPCOLORMAP lpColorMap = NULL, int nMapSize = 0);
	BOOL CreateBitmap(int nWidth, int nHeight, UINT nPlanes, UINT nBitcount,
			const void* lpBits);
	BOOL CreateBitmapIndirect(LPBITMAP lpBitmap);
	BOOL CreateCompatibleBitmap(CDC* pDC, int nWidth, int nHeight);
	BOOL CreateDiscardableBitmap(CDC* pDC, int nWidth, int nHeight);

// Attributes
	operator HBITMAP() const;
	int GetBitmap(BITMAP* pBitMap);

// Operations
	DWORD SetBitmapBits(DWORD dwCount, const void* lpBits);
	DWORD GetBitmapBits(DWORD dwCount, LPVOID lpBits) const;
	CSize SetBitmapDimension(int nWidth, int nHeight);
	CSize GetBitmapDimension() const;

// Implementation
public:
	virtual ~CBitmap();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CPalette : public CGdiObject
{
	DECLARE_DYNAMIC(CPalette)

public:
	static CPalette* PASCAL FromHandle(HPALETTE hPalette);

// Constructors
	CPalette();
	BOOL CreatePalette(LPLOGPALETTE lpLogPalette);
	BOOL CreateHalftonePalette(CDC* pDC);

// Attributes
	operator HPALETTE() const;
	int GetEntryCount();
	UINT GetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors) const;
	UINT SetPaletteEntries(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors);

// Operations
	void AnimatePalette(UINT nStartIndex, UINT nNumEntries,
			LPPALETTEENTRY lpPaletteColors);
	UINT GetNearestPaletteIndex(COLORREF crColor) const;
	BOOL ResizePalette(UINT nNumEntries);

// Implementation
	virtual ~CPalette();
};

class CRgn : public CGdiObject
{
	DECLARE_DYNAMIC(CRgn)

public:
	static CRgn* PASCAL FromHandle(HRGN hRgn);
	operator HRGN() const;

// Constructors
	CRgn();
	BOOL CreateRectRgn(int x1, int y1, int x2, int y2);
	BOOL CreateRectRgnIndirect(LPCRECT lpRect);
	BOOL CreateEllipticRgn(int x1, int y1, int x2, int y2);
	BOOL CreateEllipticRgnIndirect(LPCRECT lpRect);
	BOOL CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode);
	BOOL CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts,
			int nCount, int nPolyFillMode);
	BOOL CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3);
	BOOL CreateFromPath(CDC* pDC);
	BOOL CreateFromData(const XFORM* lpXForm, int nCount,
		const RGNDATA* pRgnData);

// Operations
	void SetRectRgn(int x1, int y1, int x2, int y2);
	void SetRectRgn(LPCRECT lpRect);
	int CombineRgn(const CRgn* pRgn1, const CRgn* pRgn2, int nCombineMode);
	int CopyRgn(const CRgn* pRgnSrc);
	BOOL EqualRgn(const CRgn* pRgn) const;
	int OffsetRgn(int x, int y);
	int OffsetRgn(POINT point);
	int GetRgnBox(LPRECT lpRect) const;
	BOOL PtInRegion(int x, int y) const;
	BOOL PtInRegion(POINT point) const;
	BOOL RectInRegion(LPCRECT lpRect) const;
	int GetRegionData(LPRGNDATA lpRgnData, int nCount) const;

// Implementation
	virtual ~CRgn();
};

/*============================================================================*/
// The device context

class CDC : public CObject
{
	DECLARE_DYNCREATE(CDC)
public:

// Attributes
	HDC m_hDC;          // The output DC (must be first data member)
	HDC m_hAttribDC;    // The Attribute DC
	operator HDC() const;
	HDC GetSafeHdc() const; // Always returns the Output DC
	CWnd* GetWindow() const;

	static CDC* PASCAL FromHandle(HDC hDC);
	static void PASCAL DeleteTempMap();
	BOOL Attach(HDC hDC);   // Attach/Detach affects only the Output DC
	HDC Detach();

	virtual void SetAttribDC(HDC hDC);  // Set the Attribute DC
	virtual void SetOutputDC(HDC hDC);  // Set the Output DC
	virtual void ReleaseAttribDC();     // Release the Attribute DC
	virtual void ReleaseOutputDC();     // Release the Output DC

	BOOL IsPrinting() const;            // TRUE if being used for printing

	CPen* GetCurrentPen() const;
	CBrush* GetCurrentBrush() const;
	CPalette* GetCurrentPalette() const;
	CFont* GetCurrentFont() const;
	CBitmap* GetCurrentBitmap() const;

	// for bidi and mirrored localization
	DWORD GetLayout() const;
	DWORD SetLayout(DWORD dwLayout);

// Constructors
	CDC();
	BOOL CreateDC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName,
		LPCTSTR lpszOutput, const void* lpInitData);
	BOOL CreateIC(LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName,
		LPCTSTR lpszOutput, const void* lpInitData);
	BOOL CreateCompatibleDC(CDC* pDC);

	BOOL DeleteDC();

// Device-Context Functions
	virtual int SaveDC();
	virtual BOOL RestoreDC(int nSavedDC);
	int GetDeviceCaps(int nIndex) const;
	UINT SetBoundsRect(LPCRECT lpRectBounds, UINT flags);
	UINT GetBoundsRect(LPRECT lpRectBounds, UINT flags);
	BOOL ResetDC(const DEVMODE* lpDevMode);

// Drawing-Tool Functions
	CPoint GetBrushOrg() const;
	CPoint SetBrushOrg(int x, int y);
	CPoint SetBrushOrg(POINT point);
	int EnumObjects(int nObjectType,
			int (CALLBACK* lpfn)(LPVOID, LPARAM), LPARAM lpData);

// Type-safe selection helpers
public:
	virtual CGdiObject* SelectStockObject(int nIndex);
	CPen* SelectObject(CPen* pPen);
	CBrush* SelectObject(CBrush* pBrush);
	virtual CFont* SelectObject(CFont* pFont);
	CBitmap* SelectObject(CBitmap* pBitmap);
	int SelectObject(CRgn* pRgn);       // special return for regions
	CGdiObject* SelectObject(CGdiObject* pObject);
		// CGdiObject* provided so compiler doesn't use SelectObject(HGDIOBJ)

// Color and Color Palette Functions
	COLORREF GetNearestColor(COLORREF crColor) const;
	CPalette* SelectPalette(CPalette* pPalette, BOOL bForceBackground);
	UINT RealizePalette();
	void UpdateColors();

// Drawing-Attribute Functions
	COLORREF GetBkColor() const;
	int GetBkMode() const;
	int GetPolyFillMode() const;
	int GetROP2() const;
	int GetStretchBltMode() const;
	COLORREF GetTextColor() const;

	virtual COLORREF SetBkColor(COLORREF crColor);
	int SetBkMode(int nBkMode);
	int SetPolyFillMode(int nPolyFillMode);
	int SetROP2(int nDrawMode);
	int SetStretchBltMode(int nStretchMode);
	virtual COLORREF SetTextColor(COLORREF crColor);

	BOOL GetColorAdjustment(LPCOLORADJUSTMENT lpColorAdjust) const;
	BOOL SetColorAdjustment(const COLORADJUSTMENT* lpColorAdjust);

	COLORREF GetDCBrushColor() const;
	COLORREF SetDCBrushColor(COLORREF crColor);

	COLORREF GetDCPenColor() const;
	COLORREF SetDCPenColor(COLORREF crColor);

	// Graphics mode
	int SetGraphicsMode(int iMode);
	int GetGraphicsMode() const;

	// World transform
	BOOL SetWorldTransform(const XFORM* pXform);
	BOOL ModifyWorldTransform(const XFORM* pXform,DWORD iMode);
	BOOL GetWorldTransform(XFORM* pXform) const;

	// Mapping Functions
	int GetMapMode() const;
	CPoint GetViewportOrg() const;
	virtual int SetMapMode(int nMapMode);
	// Viewport Origin
	virtual CPoint SetViewportOrg(int x, int y);
			CPoint SetViewportOrg(POINT point);
	virtual CPoint OffsetViewportOrg(int nWidth, int nHeight);

	// Viewport Extent
	CSize GetViewportExt() const;
	virtual CSize SetViewportExt(int cx, int cy);
			CSize SetViewportExt(SIZE size);
	virtual CSize ScaleViewportExt(int xNum, int xDenom, int yNum, int yDenom);

	// Window Origin
	CPoint GetWindowOrg() const;
	CPoint SetWindowOrg(int x, int y);
	CPoint SetWindowOrg(POINT point);
	CPoint OffsetWindowOrg(int nWidth, int nHeight);

	// Window extent
	CSize GetWindowExt() const;
	virtual CSize SetWindowExt(int cx, int cy);
			CSize SetWindowExt(SIZE size);
	virtual CSize ScaleWindowExt(int xNum, int xDenom, int yNum, int yDenom);

// Coordinate Functions
	void DPtoLP(LPPOINT lpPoints, int nCount = 1) const;
	void DPtoLP(LPRECT lpRect) const;
	void DPtoLP(LPSIZE lpSize) const;
	void LPtoDP(LPPOINT lpPoints, int nCount = 1) const;
	void LPtoDP(LPRECT lpRect) const;
	void LPtoDP(LPSIZE lpSize) const;

// Special Coordinate Functions (useful for dealing with metafiles and OLE)
	void DPtoHIMETRIC(LPSIZE lpSize) const;
	void LPtoHIMETRIC(LPSIZE lpSize) const;
	void HIMETRICtoDP(LPSIZE lpSize) const;
	void HIMETRICtoLP(LPSIZE lpSize) const;

// Region Functions
	BOOL FillRgn(CRgn* pRgn, CBrush* pBrush);
	BOOL FrameRgn(CRgn* pRgn, CBrush* pBrush, int nWidth, int nHeight);
	BOOL InvertRgn(CRgn* pRgn);
	BOOL PaintRgn(CRgn* pRgn);

// Clipping Functions
	virtual int GetClipBox(LPRECT lpRect) const;
	virtual BOOL PtVisible(int x, int y) const;
			BOOL PtVisible(POINT point) const;
	virtual BOOL RectVisible(LPCRECT lpRect) const;
			int SelectClipRgn(CRgn* pRgn);
			int ExcludeClipRect(int x1, int y1, int x2, int y2);
			int ExcludeClipRect(LPCRECT lpRect);
			int ExcludeUpdateRgn(CWnd* pWnd);
			int IntersectClipRect(int x1, int y1, int x2, int y2);
			int IntersectClipRect(LPCRECT lpRect);
			int OffsetClipRgn(int x, int y);
			int OffsetClipRgn(SIZE size);
	int SelectClipRgn(CRgn* pRgn, int nMode);

// Line-Output Functions
	CPoint GetCurrentPosition() const;
	CPoint MoveTo(int x, int y);
	CPoint MoveTo(POINT point);
	BOOL LineTo(int x, int y);
	BOOL LineTo(POINT point);
	BOOL Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	BOOL Arc(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
	BOOL Polyline(const POINT* lpPoints, int nCount);

	BOOL AngleArc(int x, int y, int nRadius, float fStartAngle, float fSweepAngle);
	BOOL ArcTo(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	BOOL ArcTo(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
	int GetArcDirection() const;
	int SetArcDirection(int nArcDirection);

	BOOL PolyDraw(const POINT* lpPoints, const BYTE* lpTypes, int nCount);
	BOOL PolylineTo(const POINT* lpPoints, int nCount);
	BOOL PolyPolyline(const POINT* lpPoints,
		const DWORD* lpPolyPoints, int nCount);

	BOOL PolyBezier(const POINT* lpPoints, int nCount);
	BOOL PolyBezierTo(const POINT* lpPoints, int nCount);

// Simple Drawing Functions
	void FillRect(LPCRECT lpRect, CBrush* pBrush);
	void FrameRect(LPCRECT lpRect, CBrush* pBrush);
	void InvertRect(LPCRECT lpRect);
	BOOL DrawIcon(int x, int y, HICON hIcon);
	BOOL DrawIcon(POINT point, HICON hIcon);
	BOOL DrawState(CPoint pt, CSize size, HBITMAP hBitmap, UINT nFlags,
		HBRUSH hBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, CBitmap* pBitmap, UINT nFlags,
		CBrush* pBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, HICON hIcon, UINT nFlags,
		HBRUSH hBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, HICON hIcon, UINT nFlags,
		CBrush* pBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, LPCTSTR lpszText, UINT nFlags,
		BOOL bPrefixText = TRUE, int nTextLen = 0, HBRUSH hBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, LPCTSTR lpszText, UINT nFlags,
		BOOL bPrefixText = TRUE, int nTextLen = 0, CBrush* pBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, DRAWSTATEPROC lpDrawProc,
		LPARAM lData, UINT nFlags, HBRUSH hBrush = NULL);
	BOOL DrawState(CPoint pt, CSize size, DRAWSTATEPROC lpDrawProc,
		LPARAM lData, UINT nFlags, CBrush* pBrush = NULL);

// Ellipse and Polygon Functions
	BOOL Chord(int x1, int y1, int x2, int y2, int x3, int y3,
		int x4, int y4);
	BOOL Chord(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
	void DrawFocusRect(LPCRECT lpRect);
	BOOL Ellipse(int x1, int y1, int x2, int y2);
	BOOL Ellipse(LPCRECT lpRect);
	BOOL Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
	BOOL Pie(LPCRECT lpRect, POINT ptStart, POINT ptEnd);
	BOOL Polygon(const POINT* lpPoints, int nCount);
	BOOL PolyPolygon(const POINT* lpPoints, const INT* lpPolyCounts, int nCount);
	BOOL Rectangle(int x1, int y1, int x2, int y2);
	BOOL Rectangle(LPCRECT lpRect);
	BOOL RoundRect(int x1, int y1, int x2, int y2, int x3, int y3);
	BOOL RoundRect(LPCRECT lpRect, POINT point);

// Bitmap Functions
	BOOL PatBlt(int x, int y, int nWidth, int nHeight, DWORD dwRop);
	BOOL BitBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
		int xSrc, int ySrc, DWORD dwRop);
	BOOL StretchBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
		int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop);
	COLORREF GetPixel(int x, int y) const;
	COLORREF GetPixel(POINT point) const;
	COLORREF SetPixel(int x, int y, COLORREF crColor);
	COLORREF SetPixel(POINT point, COLORREF crColor);
	BOOL FloodFill(int x, int y, COLORREF crColor);
	BOOL ExtFloodFill(int x, int y, COLORREF crColor, UINT nFillType);
	BOOL MaskBlt(int x, int y, int nWidth, int nHeight, CDC* pSrcDC,
		int xSrc, int ySrc, CBitmap& maskBitmap, int xMask, int yMask,
		DWORD dwRop);
	BOOL PlgBlt(LPPOINT lpPoint, CDC* pSrcDC, int xSrc, int ySrc,
		int nWidth, int nHeight, CBitmap& maskBitmap, int xMask, int yMask);
	BOOL SetPixelV(int x, int y, COLORREF crColor);
	BOOL SetPixelV(POINT point, COLORREF crColor);
   BOOL GradientFill(TRIVERTEX* pVertices, ULONG nVertices,
	  void* pMesh, ULONG nMeshElements, DWORD dwMode);
   BOOL TransparentBlt(int xDest, int yDest, int nDestWidth, int nDestHeight,
	  CDC* pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
	  UINT clrTransparent);
   BOOL AlphaBlend(int xDest, int yDest, int nDestWidth, int nDestHeight,
	  CDC* pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
	  BLENDFUNCTION blend);

// Text Functions
	virtual BOOL TextOut(int x, int y, LPCTSTR lpszString, int nCount);
			BOOL TextOut(int x, int y, const CString& str);
	virtual BOOL ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect,
				LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths);
			BOOL ExtTextOut(int x, int y, UINT nOptions, LPCRECT lpRect,
				const CString& str, LPINT lpDxWidths);
	virtual CSize TabbedTextOut(int x, int y, LPCTSTR lpszString, int nCount,
				int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin);
			CSize TabbedTextOut(int x, int y, const CString& str,
				int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin);

#pragma push_macro("DrawText")
#pragma push_macro("DrawTextEx")
#undef DrawText
#undef DrawTextEx
	virtual int _AFX_FUNCNAME(DrawText)(LPCTSTR lpszString, int nCount, LPRECT lpRect,
				UINT nFormat);
			int _AFX_FUNCNAME(DrawText)(const CString& str, LPRECT lpRect, UINT nFormat);

	virtual int _AFX_FUNCNAME(DrawTextEx)(LPTSTR lpszString, int nCount, LPRECT lpRect,
				UINT nFormat, LPDRAWTEXTPARAMS lpDTParams);
			int _AFX_FUNCNAME(DrawTextEx)(const CString& str, LPRECT lpRect, UINT nFormat, LPDRAWTEXTPARAMS lpDTParams);

			int DrawText(LPCTSTR lpszString, int nCount, LPRECT lpRect,
				UINT nFormat);
			int DrawText(const CString& str, LPRECT lpRect, UINT nFormat);

			int DrawTextEx(LPTSTR lpszString, int nCount, LPRECT lpRect,
				UINT nFormat, LPDRAWTEXTPARAMS lpDTParams);
			int DrawTextEx(const CString& str, LPRECT lpRect, UINT nFormat, LPDRAWTEXTPARAMS lpDTParams);
#pragma pop_macro("DrawText")
#pragma pop_macro("DrawTextEx")

	CSize GetTextExtent(LPCTSTR lpszString, int nCount) const;
	CSize GetTextExtent(const CString& str) const;
	CSize GetOutputTextExtent(LPCTSTR lpszString, int nCount) const;
	CSize GetOutputTextExtent(const CString& str) const;
	CSize GetTabbedTextExtent(LPCTSTR lpszString, int nCount,
		int nTabPositions, LPINT lpnTabStopPositions) const;
	CSize GetTabbedTextExtent(const CString& str,
		int nTabPositions, LPINT lpnTabStopPositions) const;
	CSize GetOutputTabbedTextExtent(LPCTSTR lpszString, int nCount,
		int nTabPositions, LPINT lpnTabStopPositions) const;
	CSize GetOutputTabbedTextExtent(const CString& str,
		int nTabPositions, LPINT lpnTabStopPositions) const;
	virtual BOOL GrayString(CBrush* pBrush,
		BOOL (CALLBACK* lpfnOutput)(HDC, LPARAM, int), LPARAM lpData,
			int nCount, int x, int y, int nWidth, int nHeight);
	UINT GetTextAlign() const;
	UINT SetTextAlign(UINT nFlags);
	int GetTextFace(_In_ int nCount, _Out_writes_to_(nCount, return) LPTSTR lpszFacename) const;
	int GetTextFace(CString& rString) const;
#pragma push_macro("GetTextMetrics")
#undef GetTextMetrics
	BOOL _AFX_FUNCNAME(GetTextMetrics)(LPTEXTMETRIC lpMetrics) const;
	BOOL GetTextMetrics(LPTEXTMETRIC lpMetrics) const;
#pragma pop_macro("GetTextMetrics")
	BOOL GetOutputTextMetrics(LPTEXTMETRIC lpMetrics) const;
	int SetTextJustification(int nBreakExtra, int nBreakCount);
	int GetTextCharacterExtra() const;
	int SetTextCharacterExtra(int nCharExtra);

	DWORD GetCharacterPlacement(LPCTSTR lpString, int nCount, int nMaxExtent, LPGCP_RESULTS lpResults, DWORD dwFlags) const;
	DWORD GetCharacterPlacement(CString& str, int nMaxExtent, LPGCP_RESULTS lpResults, DWORD dwFlags) const;

	BOOL GetTextExtentExPointI(LPWORD pgiIn, int cgi, int nMaxExtent, LPINT lpnFit, LPINT alpDx, _Out_opt_ LPSIZE lpSize) const;
	BOOL GetTextExtentPointI(LPWORD pgiIn, int cgi, _Out_opt_ LPSIZE lpSize) const;

// Advanced Drawing
	BOOL DrawEdge(LPRECT lpRect, UINT nEdge, UINT nFlags);
	BOOL DrawFrameControl(LPRECT lpRect, UINT nType, UINT nState);

// Scrolling Functions
	BOOL ScrollDC(int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip,
		CRgn* pRgnUpdate, LPRECT lpRectUpdate);

// Font Functions
	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const;
	BOOL GetOutputCharWidth(UINT nFirstChar, UINT nLastChar, LPINT lpBuffer) const;
	DWORD SetMapperFlags(DWORD dwFlag);
	CSize GetAspectRatioFilter() const;

	BOOL GetCharABCWidths(UINT nFirstChar, UINT nLastChar, LPABC lpabc) const;
	DWORD GetFontData(DWORD dwTable, DWORD dwOffset, LPVOID lpData, DWORD cbData) const;
	int GetKerningPairs(int nPairs, LPKERNINGPAIR lpkrnpair) const;
	UINT GetOutlineTextMetrics(UINT cbData, LPOUTLINETEXTMETRIC lpotm) const;
	DWORD GetGlyphOutline(UINT nChar, UINT nFormat, LPGLYPHMETRICS lpgm,
		DWORD cbBuffer, LPVOID lpBuffer, const MAT2* lpmat2) const;

	BOOL GetCharABCWidths(UINT nFirstChar, UINT nLastChar,
		LPABCFLOAT lpABCF) const;
	BOOL GetCharWidth(UINT nFirstChar, UINT nLastChar,
		float* lpFloatBuffer) const;

	DWORD GetFontLanguageInfo() const;

	BOOL GetCharABCWidthsI(UINT giFirst, UINT cgi, LPWORD pgi, LPABC lpabc) const;
	BOOL GetCharWidthI(UINT giFirst, UINT cgi, LPWORD pgi, LPINT lpBuffer) const;

// Printer/Device Escape Functions
	virtual int Escape(_In_ int nEscape, _In_ int nCount,
		_In_reads_bytes_(nCount) LPCSTR lpszInData, _In_ LPVOID lpOutData);
	int Escape(_In_ int nEscape, _In_ int nInputSize, _In_reads_bytes_(nInputSize) LPCSTR lpszInputData,
		_In_ int nOutputSize, _Out_writes_bytes_(nOutputSize) LPSTR lpszOutputData);
	int DrawEscape(int nEscape, int nInputSize, LPCSTR lpszInputData);

	// Escape helpers
	int StartDoc(LPCTSTR lpszDocName);  // old Win3.0 version
	int StartDoc(LPDOCINFO lpDocInfo);
	int StartPage();
	int EndPage();
	int SetAbortProc(BOOL (CALLBACK* lpfn)(HDC, int));
	int AbortDoc();
	int EndDoc();

// MetaFile Functions
	BOOL PlayMetaFile(HMETAFILE hMF);
	BOOL PlayMetaFile(HENHMETAFILE hEnhMetaFile, LPCRECT lpBounds);
	BOOL AddMetaFileComment(UINT nDataSize, const BYTE* pCommentData);
		// can be used for enhanced metafiles only

// Path Functions
	BOOL AbortPath();
	BOOL BeginPath();
	BOOL CloseFigure();
	BOOL EndPath();
	BOOL FillPath();
	BOOL FlattenPath();
	BOOL StrokeAndFillPath();
	BOOL StrokePath();
	BOOL WidenPath();
	float GetMiterLimit() const;
	BOOL SetMiterLimit(float fMiterLimit);
	int GetPath(LPPOINT lpPoints, LPBYTE lpTypes, int nCount) const;
	BOOL SelectClipPath(int nMode);

// Misc Helper Functions
	static CBrush* PASCAL GetHalftoneBrush();
	void DrawDragRect(LPCRECT lpRect, SIZE size,
		LPCRECT lpRectLast, SIZE sizeLast,
		CBrush* pBrush = NULL, CBrush* pBrushLast = NULL);
	void FillSolidRect(LPCRECT lpRect, COLORREF clr);
	void FillSolidRect(int x, int y, int cx, int cy, COLORREF clr);
	void Draw3dRect(LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
	void Draw3dRect(int x, int y, int cx, int cy,
		COLORREF clrTopLeft, COLORREF clrBottomRight);

// Implementation
public:
	virtual ~CDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// advanced use and implementation
	BOOL m_bPrinting;
	HGDIOBJ SelectObject(HGDIOBJ);      // do not use for regions

protected:
	// used for implementation of non-virtual SelectObject calls
	static CGdiObject* PASCAL SelectGdiObject(HDC hDC, HGDIOBJ h);
};

/*============================================================================*/
// CDC Helpers

class CPaintDC : public CDC
{
	DECLARE_DYNAMIC(CPaintDC)

// Constructors
public:
	explicit CPaintDC(CWnd* pWnd);   // BeginPaint

// Attributes
protected:
	HWND m_hWnd;
public:
	PAINTSTRUCT m_ps;       // actual paint struct!

// Implementation
public:
	virtual ~CPaintDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CClientDC : public CDC
{
	DECLARE_DYNAMIC(CClientDC)

// Constructors
public:
	explicit CClientDC(CWnd* pWnd);

// Attributes
protected:
	HWND m_hWnd;

// Implementation
public:
	virtual ~CClientDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

class CWindowDC : public CDC
{
	DECLARE_DYNAMIC(CWindowDC)

// Constructors
public:
	explicit CWindowDC(CWnd* pWnd);

// Attributes
protected:
	HWND m_hWnd;

// Implementation
public:
	virtual ~CWindowDC();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/*============================================================================*/
// CImageList

class CImageList : public CObject
{
	DECLARE_DYNCREATE(CImageList)

// Constructors
public:
	CImageList();
	BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow);
	BOOL Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask);
	BOOL Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask);
	BOOL Create(CImageList& imagelist1, int nImage1, CImageList& imagelist2,
		int nImage2, int dx, int dy);
	BOOL Create(CImageList* pImageList);

// Attributes
	HIMAGELIST m_hImageList;            // must be first data member
	operator HIMAGELIST() const;
	HIMAGELIST GetSafeHandle() const;

	static CImageList* PASCAL FromHandle(HIMAGELIST hImageList);
	static CImageList* PASCAL FromHandlePermanent(HIMAGELIST hImageList);
	static void PASCAL DeleteTempMap();
	BOOL Attach(HIMAGELIST hImageList);
	HIMAGELIST Detach();

	int GetImageCount() const;
	COLORREF SetBkColor(COLORREF cr);
	COLORREF GetBkColor() const;
	BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const;

// Operations
	BOOL DeleteImageList();
	BOOL SetImageCount(UINT uNewCount);

	int Add(CBitmap* pbmImage, CBitmap* pbmMask);
	int Add(CBitmap* pbmImage, COLORREF crMask);
	BOOL Remove(int nImage);
	BOOL Replace(int nImage, CBitmap* pbmImage, CBitmap* pbmMask);
	int Add(HICON hIcon);
	int Replace(int nImage, HICON hIcon);
	HICON ExtractIcon(int nImage);
	BOOL Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle);
	BOOL DrawEx(CDC* pDC, int nImage, POINT pt, SIZE sz, COLORREF clrBk, COLORREF clrFg, UINT nStyle);
	BOOL SetOverlayImage(int nImage, int nOverlay);
	BOOL Copy(int iDst, int iSrc, UINT uFlags = ILCF_MOVE);
	BOOL Copy(int iDst, CImageList* pSrc, int iSrc, UINT uFlags = ILCF_MOVE);
	BOOL DrawIndirect(IMAGELISTDRAWPARAMS* pimldp);
	BOOL DrawIndirect(CDC* pDC, int nImage, POINT pt, SIZE sz, POINT ptOrigin,
			UINT fStyle = ILD_NORMAL, DWORD dwRop = SRCCOPY,
			COLORREF rgbBack = CLR_DEFAULT, COLORREF rgbFore = CLR_DEFAULT,
			DWORD fState = ILS_NORMAL, DWORD Frame = 0, COLORREF crEffect = CLR_DEFAULT);

	BOOL Read(CArchive* pArchive);
	BOOL Write(CArchive* pArchive);

// Drag APIs
	BOOL BeginDrag(int nImage, CPoint ptHotSpot);
	static void PASCAL EndDrag();
	static BOOL PASCAL DragMove(CPoint pt);
	BOOL SetDragCursorImage(int nDrag, CPoint ptHotSpot);
	static BOOL PASCAL DragShowNolock(BOOL bShow);
	static CImageList* PASCAL GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot);
	static BOOL PASCAL DragEnter(CWnd* pWndLock, CPoint point);
	static BOOL PASCAL DragLeave(CWnd* pWndLock);

// Implementation
public:
	virtual ~CImageList();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif
};

/*============================================================================*/
// CMenu

class CMenu : public CObject
{
	DECLARE_DYNCREATE(CMenu)
public:

// Constructors
	CMenu();

	BOOL CreateMenu();
	BOOL CreatePopupMenu();
	BOOL LoadMenu(LPCTSTR lpszResourceName);
	BOOL LoadMenu(UINT nIDResource);
	BOOL LoadMenuIndirect(const void* lpMenuTemplate);
	BOOL DestroyMenu();

// Attributes
	HMENU m_hMenu;          // must be first data member
	HMENU GetSafeHmenu() const;
	operator HMENU() const;

	static CMenu* PASCAL FromHandle(HMENU hMenu);
	static void PASCAL DeleteTempMap();
	BOOL Attach(HMENU hMenu);
	HMENU Detach();

// CMenu Operations
	BOOL DeleteMenu(UINT nPosition, UINT nFlags);
	BOOL TrackPopupMenu(UINT nFlags, int x, int y,
						CWnd* pWnd, LPCRECT lpRect = 0);
	BOOL TrackPopupMenuEx(UINT fuFlags, int x, int y, CWnd* pWnd, LPTPMPARAMS lptpm);

	BOOL SetMenuInfo(LPCMENUINFO lpcmi);
	BOOL GetMenuInfo(LPMENUINFO lpcmi) const;

	BOOL operator==(const CMenu& menu) const;
	BOOL operator!=(const CMenu& menu) const;

// CMenuItem Operations
	BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem = 0,
					LPCTSTR lpszNewItem = NULL);
	BOOL AppendMenu(UINT nFlags, UINT_PTR nIDNewItem, const CBitmap* pBmp);
	UINT CheckMenuItem(UINT nIDCheckItem, UINT nCheck);
	UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable);
	int GetMenuItemCount() const;
	UINT GetMenuItemID(int nPos) const;
	UINT GetMenuState(UINT nID, UINT nFlags) const;
	int GetMenuString(_In_ UINT nIDItem, _Out_writes_z_(nMaxCount) LPTSTR lpString, _In_ int nMaxCount,
					_In_ UINT nFlags) const;
	int GetMenuString(UINT nIDItem, CString& rString, UINT nFlags) const;
	BOOL GetMenuItemInfo(UINT uItem, LPMENUITEMINFO lpMenuItemInfo,
					BOOL fByPos = FALSE);
	BOOL SetMenuItemInfo(UINT uItem, LPMENUITEMINFO lpMenuItemInfo,
					BOOL fByPos = FALSE);
	CMenu* GetSubMenu(int nPos) const;
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem = 0,
					LPCTSTR lpszNewItem = NULL);
	BOOL InsertMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem,
					const CBitmap* pBmp);
	BOOL InsertMenuItem(UINT uItem, LPMENUITEMINFO lpMenuItemInfo,
					BOOL fByPos = FALSE);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem = 0,
					LPCTSTR lpszNewItem = NULL);
	BOOL ModifyMenu(UINT nPosition, UINT nFlags, UINT_PTR nIDNewItem,
					const CBitmap* pBmp);
	BOOL RemoveMenu(UINT nPosition, UINT nFlags);
	BOOL SetMenuItemBitmaps(UINT nPosition, UINT nFlags,
					const CBitmap* pBmpUnchecked, const CBitmap* pBmpChecked);
	BOOL CheckMenuRadioItem(UINT nIDFirst, UINT nIDLast, UINT nIDItem, UINT nFlags);
	BOOL SetDefaultItem(UINT uItem, BOOL fByPos = FALSE);
	UINT GetDefaultItem(UINT gmdiFlags, BOOL fByPos = FALSE);

// Context Help Functions
	BOOL SetMenuContextHelpId(DWORD dwContextHelpId);
	DWORD GetMenuContextHelpId() const;

// Overridables (must override draw and measure for owner-draw menu items)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

// Implementation
public:
	virtual ~CMenu();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	static CMenu* PASCAL FromHandlePermanent(HMENU hMenu);
};

/*============================================================================*/
// Window message map handling

struct AFX_MSGMAP_ENTRY;       // declared below after CWnd

struct AFX_MSGMAP
{
	const AFX_MSGMAP* (PASCAL* pfnGetBaseMap)();
	const AFX_MSGMAP_ENTRY* lpEntries;
};


#define DECLARE_MESSAGE_MAP() \
protected: \
	static const AFX_MSGMAP* PASCAL GetThisMessageMap(); \
	virtual const AFX_MSGMAP* GetMessageMap() const; \

#define BEGIN_TEMPLATE_MESSAGE_MAP(theClass, type_name, baseClass)			\
	PTM_WARNING_DISABLE														\
	template < typename type_name >											\
	const AFX_MSGMAP* theClass< type_name >::GetMessageMap() const			\
		{ return GetThisMessageMap(); }										\
	template < typename type_name >											\
	const AFX_MSGMAP* PASCAL theClass< type_name >::GetThisMessageMap()		\
	{																		\
		typedef theClass< type_name > ThisClass;							\
		typedef baseClass TheBaseClass;										\
		__pragma(warning(push))												\
		__pragma(warning(disable: 4640)) /* message maps can only be called by single threaded message pump */ \
		static const AFX_MSGMAP_ENTRY _messageEntries[] =					\
		{

#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	PTM_WARNING_DISABLE \
	const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return GetThisMessageMap(); } \
	const AFX_MSGMAP* PASCAL theClass::GetThisMessageMap() \
	{ \
		typedef theClass ThisClass;						   \
		typedef baseClass TheBaseClass;					   \
		__pragma(warning(push))							   \
		__pragma(warning(disable: 4640)) /* message maps can only be called by single threaded message pump */ \
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
		{

#define END_MESSAGE_MAP() \
		{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \
	}; \
		__pragma(warning(pop))	\
		static const AFX_MSGMAP messageMap = \
		{ &TheBaseClass::GetThisMessageMap, &_messageEntries[0] }; \
		return &messageMap; \
	}								  \
	PTM_WARNING_RESTORE


// Message map signature values and macros in separate header
#include <afxmsg_.h>

/*============================================================================*/
// Dialog data exchange (DDX_) and validation (DDV_)

class COleControlSite;

// CDataExchange - for data exchange and validation
class CDataExchange
{
// Attributes
public:
	BOOL m_bSaveAndValidate;   // TRUE => save and validate data
	CWnd* m_pDlgWnd;           // container usually a dialog

// Operations (for implementors of DDX and DDV procs)
	HWND PrepareCtrl(int nIDC);
	HWND PrepareEditCtrl(int nIDC);
	void Fail();                    // will throw exception

	CDataExchange(CWnd* pDlgWnd, BOOL bSaveAndValidate);

	COleControlSite* PrepareOleCtrl(int nIDC); // for OLE controls in dialog

// Implementation
   UINT m_idLastControl;      // last control used (for validation)
	BOOL m_bEditLastControl;   // last control was an edit item
};

#include <afxdd_.h>     // standard DDX_ and DDV_ routines

/*============================================================================*/
// OLE types

typedef _Return_type_success_(return >= 0) LONG HRESULT;

struct IUnknown;
typedef IUnknown* LPUNKNOWN;

struct IDispatch;
typedef /* [unique] */ __RPC_unique_pointer IDispatch* LPDISPATCH;

struct IConnectionPoint;
typedef IConnectionPoint* LPCONNECTIONPOINT;

struct IEnumOLEVERB;
typedef /* [unique] */ __RPC_unique_pointer IEnumOLEVERB* LPENUMOLEVERB;

typedef struct _GUID GUID;
typedef GUID IID;
typedef GUID CLSID;
#ifndef _REFCLSID_DEFINED
#define REFCLSID const CLSID &
#endif

typedef long DISPID;
typedef unsigned short VARTYPE;
typedef long SCODE;

typedef WCHAR OLECHAR;
typedef _Null_terminated_ OLECHAR* BSTR;

struct tagDISPPARAMS;
typedef tagDISPPARAMS DISPPARAMS;

struct tagVARIANT;
typedef tagVARIANT VARIANT;

struct ITypeInfo;
typedef /* [unique] */ __RPC_unique_pointer ITypeInfo* LPTYPEINFO;

struct ITypeLib;
typedef /* [unique] */ __RPC_unique_pointer ITypeLib* LPTYPELIB;

struct IAccessible;
struct IAccessibleProxy;
struct IAccessibleServer;
struct IEnumVARIANT;

struct tagEXCEPINFO;
typedef tagEXCEPINFO EXCEPINFO;


/*============================================================================*/
// CCmdTarget
#ifdef _M_ARM64
typedef struct _ARM64_PARAMS ARM64_PARAMS, *PARM64_PARAMS;
#endif
// private structures
struct AFX_CMDHANDLERINFO;  // info about where the command is handled
struct AFX_EVENT;           // info about an event
class CTypeLibCache;        // cache for OLE type libraries

/*============================================================================*/
// OLE interface map handling (more in AFXDISP.H)

struct AFX_INTERFACEMAP_ENTRY
{
	const void* piid;       // the interface id (IID) (NULL for aggregate)
	size_t nOffset;         // offset of the interface vtable from m_unknown
};

struct AFX_INTERFACEMAP
{
#ifdef _AFXDLL
	const AFX_INTERFACEMAP* (PASCAL* pfnGetBaseMap)(); // NULL is root class
#else
	const AFX_INTERFACEMAP* pBaseMap;
#endif
	const AFX_INTERFACEMAP_ENTRY* pEntry; // map for this class
};


#ifdef _AFXDLL
#define DECLARE_INTERFACE_MAP() \
private: \
	static const AFX_INTERFACEMAP_ENTRY _interfaceEntries[]; \
protected: \
	static const AFX_INTERFACEMAP interfaceMap; \
	static const AFX_INTERFACEMAP* PASCAL GetThisInterfaceMap(); \
	virtual const AFX_INTERFACEMAP* GetInterfaceMap() const; \

#else
#define DECLARE_INTERFACE_MAP() \
private: \
	static const AFX_INTERFACEMAP_ENTRY _interfaceEntries[]; \
protected: \
	static const AFX_INTERFACEMAP interfaceMap; \
	virtual const AFX_INTERFACEMAP* GetInterfaceMap() const; \

#endif

/*============================================================================*/
// OLE COM (Component Object Model) implementation infrastructure
//      - data driven QueryInterface
//      - standard implementation of aggregate AddRef and Release
// (see CCmdTarget in AFXWIN.H for more information)

#define METHOD_PROLOGUE(theClass, localClass) \
	theClass* pThis = \
		((theClass*)((BYTE*)this - offsetof(theClass, m_x##localClass))); \
	AFX_MANAGE_STATE(pThis->m_pModuleState) \
	pThis; // avoid warning from compiler \

#define METHOD_PROLOGUE_(theClass, localClass) \
	theClass* pThis = \
		((theClass*)((BYTE*)this - offsetof(theClass, m_x##localClass))); \
	pThis; // avoid warning from compiler \

#define METHOD_PROLOGUE_EX(theClass, localClass) \
	METHOD_PROLOGUE(theClass, localClass) \

#define METHOD_PROLOGUE_EX_(theClass, localClass) \
	METHOD_PROLOGUE_(theClass, localClass) \


// Provided only for compatibility with CDK 1.x
#define METHOD_MANAGE_STATE(theClass, localClass) \
	METHOD_PROLOGUE_EX(theClass, localClass) \

#define BEGIN_INTERFACE_PART(localClass, baseClass) \
	class X##localClass : public baseClass \
	{ \
	public: \
		STDMETHOD_(ULONG, AddRef)(); \
		STDMETHOD_(ULONG, Release)(); \
		STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj); \

#define BEGIN_INTERFACE_PART_DERIVE(localClass, baseClass) \
	BEGIN_INTERFACE_PART(localClass, baseClass) \

#define INIT_INTERFACE_PART(theClass, localClass)
#define INIT_INTERFACE_PART_DERIVE(theClass, localClass)

// Note: Inserts the rest of OLE functionality between these two macros,
//  depending upon the interface that is being implemented.  It is not
//  necessary to include AddRef, Release, and QueryInterface since those
//  member functions are declared by the macro.

#define END_INTERFACE_PART(localClass) \
	} m_x##localClass; \
	friend class X##localClass; \

struct CInterfacePlaceHolder
{
	DWORD_PTR m_vtbl;   // filled in with USE_INTERFACE_PART
	CInterfacePlaceHolder() { m_vtbl = 0; }
};

#define END_INTERFACE_PART_OPTIONAL(localClass) \
	}; \
	CInterfacePlaceHolder m_x##localClass; \
	friend class X##localClass; \

#ifdef _AFXDLL
#define END_INTERFACE_PART_STATIC END_INTERFACE_PART
#else
#define END_INTERFACE_PART_STATIC END_INTERFACE_PART
#endif

#define USE_INTERFACE_PART(localClass) \
	m_x##localClass.m_vtbl = *(DWORD_PTR*)&X##localClass(); \

// To avoid C4238.
#define USE_INTERFACE_PART_STD(localClass) \
	X##localClass tmp##localClass; \
	m_x##localClass.m_vtbl = *(DWORD_PTR*)&tmp##localClass;

#ifdef _AFXDLL
#define BEGIN_INTERFACE_MAP(theClass, theBase) \
	const AFX_INTERFACEMAP* PASCAL theClass::GetThisInterfaceMap() \
		{ return &theClass::interfaceMap; } \
	const AFX_INTERFACEMAP* theClass::GetInterfaceMap() const \
		{ return &theClass::interfaceMap; } \
	AFX_COMDAT const AFX_INTERFACEMAP theClass::interfaceMap = \
		{ &theBase::GetThisInterfaceMap, &theClass::_interfaceEntries[0], }; \
	AFX_COMDAT const AFX_INTERFACEMAP_ENTRY theClass::_interfaceEntries[] = \
	{ \

#else
#define BEGIN_INTERFACE_MAP(theClass, theBase) \
	const AFX_INTERFACEMAP* theClass::GetInterfaceMap() const \
		{ return &theClass::interfaceMap; } \
	AFX_COMDAT const AFX_INTERFACEMAP theClass::interfaceMap = \
		{ &theBase::interfaceMap, &theClass::_interfaceEntries[0], }; \
	AFX_COMDAT const AFX_INTERFACEMAP_ENTRY theClass::_interfaceEntries[] = \
	{ \

#endif

#define INTERFACE_PART(theClass, iid, localClass) \
		{ &iid, offsetof(theClass, m_x##localClass) }, \

#define INTERFACE_AGGREGATE(theClass, theAggr) \
		{ NULL, offsetof(theClass, theAggr) }, \

#define END_INTERFACE_MAP() \
		{ NULL, (size_t)-1 } \
	}; \


/*============================================================================*/
// OLE dispatch map handling (more in AFXDISP.H)

struct AFX_DISPMAP_ENTRY;

struct AFX_DISPMAP
{
#ifdef _AFXDLL
	const AFX_DISPMAP* (PASCAL* pfnGetBaseMap)();
#else
	const AFX_DISPMAP* pBaseMap;
#endif
	const AFX_DISPMAP_ENTRY* lpEntries;
	UINT* lpEntryCount;
	DWORD* lpStockPropMask;
};

#ifdef _AFXDLL
#define DECLARE_DISPATCH_MAP() \
private: \
	static const AFX_DISPMAP_ENTRY _dispatchEntries[]; \
	static UINT _dispatchEntryCount; \
	static DWORD _dwStockPropMask; \
protected: \
	static const AFX_DISPMAP dispatchMap; \
	static const AFX_DISPMAP* PASCAL GetThisDispatchMap(); \
	virtual const AFX_DISPMAP* GetDispatchMap() const; \

#else
#define DECLARE_DISPATCH_MAP() \
private: \
	static const AFX_DISPMAP_ENTRY _dispatchEntries[]; \
	static UINT _dispatchEntryCount; \
	static DWORD _dwStockPropMask; \
protected: \
	static const AFX_DISPMAP dispatchMap; \
	virtual const AFX_DISPMAP* GetDispatchMap() const; \

#endif

/*============================================================================*/
// OLE Document Object command target handling

struct AFX_OLECMDMAP_ENTRY
{
   const GUID* pguid;   // id of the command group
   ULONG       cmdID;   // OLECMD ID
   UINT        nID;     // corresponding WM_COMMAND message ID
};

struct AFX_OLECMDMAP
{
#ifdef _AFXDLL
	const AFX_OLECMDMAP* (PASCAL* pfnGetBaseMap)();
#else
	const AFX_OLECMDMAP* pBaseMap;
#endif
	const AFX_OLECMDMAP_ENTRY* lpEntries;
};

#ifdef _AFXDLL
#define DECLARE_OLECMD_MAP() \
private: \
	static const AFX_OLECMDMAP_ENTRY _commandEntries[]; \
protected: \
	static const AFX_OLECMDMAP commandMap; \
	static const AFX_OLECMDMAP* PASCAL GetThisCommandMap(); \
	virtual const AFX_OLECMDMAP* GetCommandMap() const; \

#else
#define DECLARE_OLECMD_MAP() \
private: \
	static const AFX_OLECMDMAP_ENTRY _commandEntries[]; \
protected: \
	static const AFX_OLECMDMAP commandMap; \
	virtual const AFX_OLECMDMAP* GetCommandMap() const; \

#endif

#ifdef _AFXDLL
#define BEGIN_OLECMD_MAP(theClass, baseClass) \
	const AFX_OLECMDMAP* PASCAL theClass::GetThisCommandMap() \
		{ return &theClass::commandMap; } \
	const AFX_OLECMDMAP* theClass::GetCommandMap() const \
		{ return &theClass::commandMap; } \
	AFX_COMDAT const AFX_OLECMDMAP theClass::commandMap = \
	{ &baseClass::GetThisCommandMap, &theClass::_commandEntries[0] }; \
	AFX_COMDAT const AFX_OLECMDMAP_ENTRY theClass::_commandEntries[] = \
	{ \

#else
#define BEGIN_OLECMD_MAP(theClass, baseClass) \
	const AFX_OLECMDMAP* theClass::GetCommandMap() const \
		{ return &theClass::commandMap; } \
	AFX_COMDAT const AFX_OLECMDMAP theClass::commandMap = \
	{ &baseClass::commandMap, &theClass::_commandEntries[0] }; \
	AFX_COMDAT const AFX_OLECMDMAP_ENTRY theClass::_commandEntries[] = \
	{ \

#endif

#define END_OLECMD_MAP() \
		{NULL, 0, 0} \
	}; \

class COleCmdUI;

/*============================================================================*/
// OLE event sink map handling (more in AFXDISP.H)

struct AFX_EVENTSINKMAP_ENTRY;

struct AFX_EVENTSINKMAP
{
#ifdef _AFXDLL
	const AFX_EVENTSINKMAP* (PASCAL* pfnGetBaseMap)();
#else
	const AFX_EVENTSINKMAP* pBaseMap;
#endif
	const AFX_EVENTSINKMAP_ENTRY* lpEntries;
	UINT* lpEntryCount;
};

#ifdef _AFXDLL
#define DECLARE_EVENTSINK_MAP() \
private: \
	static const AFX_EVENTSINKMAP_ENTRY _eventsinkEntries[]; \
	static UINT _eventsinkEntryCount; \
protected: \
	static const AFX_EVENTSINKMAP eventsinkMap; \
	static const AFX_EVENTSINKMAP* PASCAL GetThisEventSinkMap(); \
	virtual const AFX_EVENTSINKMAP* GetEventSinkMap() const; \

#else
#define DECLARE_EVENTSINK_MAP() \
private: \
	static const AFX_EVENTSINKMAP_ENTRY _eventsinkEntries[]; \
	static UINT _eventsinkEntryCount; \
protected: \
	static const AFX_EVENTSINKMAP eventsinkMap; \
	virtual const AFX_EVENTSINKMAP* GetEventSinkMap() const; \

#endif

/*============================================================================*/
// OLE connection map handling (more in AFXDISP.H)

struct AFX_CONNECTIONMAP_ENTRY
{
	const void* piid;   // the interface id (IID)
	size_t nOffset;         // offset of the interface vtable from m_unknown
};

struct AFX_CONNECTIONMAP
{
#ifdef _AFXDLL
	const AFX_CONNECTIONMAP* (PASCAL* pfnGetBaseMap)(); // NULL is root class
#else
	const AFX_CONNECTIONMAP* pBaseMap;
#endif
	const AFX_CONNECTIONMAP_ENTRY* pEntry; // map for this class
};

#ifdef _AFXDLL
#define DECLARE_CONNECTION_MAP() \
private: \
	static const AFX_CONNECTIONMAP_ENTRY _connectionEntries[]; \
protected: \
	static const AFX_CONNECTIONMAP connectionMap; \
	static const AFX_CONNECTIONMAP* PASCAL GetThisConnectionMap(); \
	virtual const AFX_CONNECTIONMAP* GetConnectionMap() const; \

#else
#define DECLARE_CONNECTION_MAP() \
private: \
	static const AFX_CONNECTIONMAP_ENTRY _connectionEntries[]; \
protected: \
	static const AFX_CONNECTIONMAP connectionMap; \
	virtual const AFX_CONNECTIONMAP* GetConnectionMap() const; \

#endif

/*============================================================================*/
// CCmdTarget proper

#ifdef _ARM64_
typedef struct _ARM64_PARAMS ARM64_PARAMS, *PARM64_PARAMS;
#endif

class COccManager;      // forward reference (see ..\src\occimpl.h)

class AFX_NOVTABLE CCmdTarget : public CObject
{
	DECLARE_DYNAMIC(CCmdTarget)
protected:

public:
// Constructors
	CCmdTarget();

// Attributes
	LPDISPATCH GetIDispatch(BOOL bAddRef);
		// retrieve IDispatch part of CCmdTarget
	static CCmdTarget* PASCAL FromIDispatch(LPDISPATCH lpDispatch);
		// map LPDISPATCH back to CCmdTarget* (inverse of GetIDispatch)
	BOOL IsResultExpected();
		// returns TRUE if automation function should return a value

// Operations
	void EnableAutomation();
		// call in constructor to wire up IDispatch
	void EnableConnections();
		// call in constructor to wire up IConnectionPointContainer

	void BeginWaitCursor();
	void EndWaitCursor();
	void RestoreWaitCursor();       // call after messagebox

	// dispatch OLE verbs through the message map
	BOOL EnumOleVerbs(LPENUMOLEVERB* ppenumOleVerb);
	BOOL DoOleVerb(LONG iVerb, LPMSG lpMsg, HWND hWndParent, LPCRECT lpRect);

// Overridables
	// route and dispatch standard command message types
	//   (more sophisticated than OnCommand)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	// called when last OLE reference is released
	virtual void OnFinalRelease();

	// called before dispatching to an automation handler function
	virtual BOOL IsInvokeAllowed(DISPID dispid);

	// support for OLE type libraries
	void EnableTypeLib();
	HRESULT GetTypeInfoOfGuid(LCID lcid, const GUID& guid,
		LPTYPEINFO* ppTypeInfo);
	virtual BOOL GetDispatchIID(IID* pIID);
	virtual UINT GetTypeInfoCount();
	virtual CTypeLibCache* GetTypeLibCache();
	virtual HRESULT GetTypeLib(LCID lcid, LPTYPELIB* ppTypeLib);

// Implementation
public:
	virtual ~CCmdTarget() = 0;
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif

	void GetNotSupported();
	void SetNotSupported();

protected:
	friend class CView;

	CView* GetRoutingView();
	CFrameWnd* GetRoutingFrame();
	static CView* PASCAL GetRoutingView_();
	static CFrameWnd* PASCAL GetRoutingFrame_();
	DECLARE_MESSAGE_MAP()       // base class - no {{ }} macros

	DECLARE_OLECMD_MAP()
	friend class COleCmdUI;

	DECLARE_DISPATCH_MAP()
	DECLARE_CONNECTION_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_EVENTSINK_MAP()

	// OLE interface map implementation
public:
	// data used when CCmdTarget is made OLE aware
	long m_dwRef;
	LPUNKNOWN m_pOuterUnknown;  // external controlling unknown if != NULL
	DWORD_PTR m_xInnerUnknown;  // place-holder for inner controlling unknown

public:
	// advanced operations
	void EnableAggregation();       // call to enable aggregation
	void ExternalDisconnect();      // forcibly disconnect
	LPUNKNOWN GetControllingUnknown();
		// get controlling IUnknown for aggregate creation

	// these versions do not delegate to m_pOuterUnknown
	DWORD InternalQueryInterface(const void*, LPVOID* ppvObj);
	DWORD InternalAddRef();
	DWORD InternalRelease();
	// these versions delegate to m_pOuterUnknown
	DWORD ExternalQueryInterface(const void*, LPVOID* ppvObj);
	DWORD ExternalAddRef();
	DWORD ExternalRelease();

	// implementation helpers
	LPUNKNOWN GetInterface(const void*);
	LPUNKNOWN QueryAggregates(const void*);

	// advanced overrideables for implementation
	virtual BOOL OnCreateAggregates();
	virtual LPUNKNOWN GetInterfaceHook(const void*);

	// OLE automation implementation
protected:
	struct XDispatch
	{
		DWORD_PTR m_vtbl;   // place-holder for IDispatch vtable
	} m_xDispatch;
	BOOL m_bResultExpected;

	// member variable-based properties
	void GetStandardProp(const AFX_DISPMAP_ENTRY* pEntry,
		VARIANT* pvarResult, UINT* puArgErr);
	SCODE SetStandardProp(const AFX_DISPMAP_ENTRY* pEntry,
		DISPPARAMS* pDispParams, UINT* puArgErr);

	// DISPID to dispatch map lookup
	static UINT PASCAL GetEntryCount(const AFX_DISPMAP* pDispMap);
	const AFX_DISPMAP_ENTRY* PASCAL GetDispEntry(LONG memid);
	static LONG PASCAL MemberIDFromName(const AFX_DISPMAP* pDispMap, LPCTSTR lpszName);

	// helpers for member function calling implementation
	static UINT PASCAL GetStackSize(const BYTE* pbParams, VARTYPE vtResult);
#if defined(_SHADOW_DOUBLES)
	SCODE PushStackArgs(BYTE* pStack, const BYTE* pbParams,
		void* pResult, VARTYPE vtResult, DISPPARAMS* pDispParams,
		UINT* puArgErr, VARIANT* rgTempVars, UINT nSizeArgs,CVariantBoolConverter* pTempStackArgs = NULL);
#elif defined(_M_ARM64)
    SCODE PushStackArgs(BYTE* pStack, const BYTE* pbParams,
                                    void* pResult, VARTYPE vtResult, DISPPARAMS* pDispParams, UINT* puArgErr,
                                    VARIANT* rgTempVars, CVariantBoolConverter* pTempStackArgs = NULL, PARM64_PARAMS pArm64Params = NULL);
#else
	SCODE PushStackArgs(BYTE* pStack, const BYTE* pbParams,
		void* pResult, VARTYPE vtResult, DISPPARAMS* pDispParams,
		UINT* puArgErr, VARIANT* rgTempVars,CVariantBoolConverter* pTempStackArgs = NULL);
#endif
	SCODE CallMemberFunc(const AFX_DISPMAP_ENTRY* pEntry, WORD wFlags,
		VARIANT* pvarResult, DISPPARAMS* pDispParams, UINT* puArgErr);

	friend class COleDispatchImpl;

public:
	// OLE event sink implementation
	BOOL OnEvent(UINT idCtrl, AFX_EVENT* pEvent,
		AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	const AFX_EVENTSINKMAP_ENTRY* PASCAL GetEventSinkEntry(UINT idCtrl,
		AFX_EVENT* pEvent);

	// OLE connection implementation
	struct XConnPtContainer
	{
		DWORD_PTR m_vtbl;   // place-holder for IConnectionPointContainer vtable
	} m_xConnPtContainer;

	AFX_MODULE_STATE* m_pModuleState;
	friend class CInnerUnknown;
	friend UINT APIENTRY _AfxThreadEntry(void* pParam);

	virtual BOOL GetExtraConnectionPoints(CPtrArray* pConnPoints);
	virtual LPCONNECTIONPOINT GetConnectionHook(const IID& iid);

	friend class COleConnPtContainer;
};

class CCmdUI        // simple helper class
{
public:
// Attributes
	UINT m_nID;
	UINT m_nIndex;          // menu item or other index

	// if a menu item
	CMenu* m_pMenu;         // NULL if not a menu
	CMenu* m_pSubMenu;      // sub containing menu item
							// if a popup sub menu - ID is for first in popup

	// if from some other window
	CWnd* m_pOther;         // NULL if a menu or not a CWnd

// Operations to do in ON_UPDATE_COMMAND_UI
	virtual void Enable(BOOL bOn = TRUE);
	virtual void SetCheck(int nCheck = 1);   // 0, 1 or 2 (indeterminate)
	virtual void SetRadio(BOOL bOn = TRUE);
	virtual void SetText(LPCTSTR lpszText);

// Advanced operation
	void ContinueRouting();

// Implementation
	CCmdUI();
	BOOL m_bEnableChanged;
	BOOL m_bContinueRouting;
	UINT m_nIndexMax;       // last + 1 for iterating m_nIndex

	CMenu* m_pParentMenu;   // NULL if parent menu not easily determined
							//  (probably a secondary popup menu)

	BOOL DoUpdate(CCmdTarget* pTarget, BOOL bDisableIfNoHndler);
};

// special CCmdUI derived classes are used for other UI paradigms
//  like toolbar buttons and status indicators

// pointer to afx_msg member function
#ifndef AFX_MSG_CALL
#define AFX_MSG_CALL
#endif
typedef void (AFX_MSG_CALL CCmdTarget::*AFX_PMSG)(void);

enum AFX_DISPMAP_FLAGS
{
	afxDispCustom = 0,
	afxDispStock = 1
};

#pragma warning(push)
#pragma warning(disable: 4121)
struct AFX_DISPMAP_ENTRY
{
	LPCTSTR lpszName;       // member/property name
	long lDispID;           // DISPID (may be DISPID_UNKNOWN)
	LPCSTR lpszParams;      // member parameter description
	WORD vt;                // return value type / or type of property
	AFX_PMSG pfn;           // normal member On<membercall> or, OnGet<property>
	AFX_PMSG pfnSet;        // special member for OnSet<property>
	size_t nPropOffset;     // property offset
	AFX_DISPMAP_FLAGS flags;// flags (e.g. stock/custom)
};
#pragma warning(pop)

struct AFX_EVENTSINKMAP_ENTRY
{
	AFX_DISPMAP_ENTRY dispEntry;
	UINT nCtrlIDFirst;
	UINT nCtrlIDLast;
};

// DSC Sink state/reason codes passed to MFC user event handlers
enum DSCSTATE
{
	dscNoState = 0,
	dscOKToDo,
	dscCancelled,
	dscSyncBefore,
	dscAboutToDo,
	dscFailedToDo,
	dscSyncAfter,
	dscDidEvent
};

enum DSCREASON
{
	dscNoReason = 0,
	dscClose,
	dscCommit,
	dscDelete,
	dscEdit,
	dscInsert,
	dscModify,
	dscMove
};

/*============================================================================*/
// CWnd implementation

#if (NTDDI_VERSION >= NTDDI_WIN7)

/// <summary>
/// CGestureConfig class allows to customize Windows gesture features such as zoom, pan or rotate. This class is used in CWnd::SetGestureConfig and CWnd::GetGestureConfig methods.</summary>
class CGestureConfig : public CObject
{
	friend class CWnd;

public:
	/// <summary>
	/// CGestureConfig constructor</summary>
	CGestureConfig();

	/// <summary>
	/// CGestureConfig destructor</summary>
	virtual ~CGestureConfig();

	/// <summary>
	/// Enable/disable gesture zoom</summary>
	/// <param name="bEnable">TRUE - enable the feature. FALSE - disable it</param>
	void EnableZoom(BOOL bEnable = TRUE);

	/// <summary>
	/// Enable/disable gesture rotate</summary>
	/// <param name="bEnable">TRUE - enable the feature. FALSE - disable it</param>
	void EnableRotate(BOOL bEnable = TRUE);

	/// <summary>
	/// Enable/disable gesture 2 finger tap</summary>
	/// <param name="bEnable">TRUE - enable the feature. FALSE - disable it</param>
	void EnableTwoFingerTap(BOOL bEnable = TRUE);

	/// <summary>
	/// Enable/disable gesture press and tap</summary>
	/// <param name="bEnable">TRUE - enable the feature. FALSE - disable it</param>
	void EnablePressAndTap(BOOL bEnable = TRUE);

	/// <summary>
	/// Enable/disable gesture pan</summary>
	/// <param name="bEnable">TRUE - enable the feature. FALSE - disable it</param>
	/// <param name="dwFlags">Gesture pan flags. Can be either GC_PAN (all pan gestures) or combination of the following flags: GC_PAN_WITH_SINGLE_FINGER_VERTICALLY, GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY, GC_PAN_WITH_GUTTER and GC_PAN_WITH_INTERTIA</param>
	void EnablePan(BOOL bEnable = TRUE, DWORD dwFlags = GC_PAN_WITH_GUTTER | GC_PAN_WITH_INERTIA);

	/// <summary>
	/// Determines whether the gesture zoom feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsZoomEnabled() const { return (Get(GID_ZOOM) & GC_ZOOM) == GC_ZOOM; }

	/// <summary>
	/// Determines whether the gesture rotate feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsRotateEnabled() const { return (Get(GID_ROTATE) & GC_ROTATE) == GC_ROTATE; }

	/// <summary>
	/// Determines whether the gesture 2 finger tap feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsTwoFingerTapEnabled() const { return (Get(GID_TWOFINGERTAP) & GC_TWOFINGERTAP) == GC_TWOFINGERTAP; }

#if defined(GID_PRESSANDTAP) && defined(GC_PRESSANDTAP)
	/// <summary>
	/// Determines whether the gesture "press and tap" feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsPressAndTapEnabled() const { return (Get(GID_PRESSANDTAP) & GC_PRESSANDTAP) == GC_PRESSANDTAP; }
#endif

	/// <summary>
	/// Determines whether the gesture pan feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsPanAllEnabled() const { return (Get(GID_PAN) & GC_PAN) == GC_PAN; }

	/// <summary>
	/// Determines whether the gesture pan vertical feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsPanVerticalEnabled() const { return (Get(GID_PAN) & GC_PAN_WITH_SINGLE_FINGER_VERTICALLY) == GC_PAN_WITH_SINGLE_FINGER_VERTICALLY; }

	/// <summary>
	/// Determines whether the gesture pan horizontal feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsPanHorizontalEnabled() const { return (Get(GID_PAN) & GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY) == GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY; }

	/// <summary>
	/// Determines whether the gesture pan with gutter feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsPanWithGutterEnabled() const { return (Get(GID_PAN) & GC_PAN_WITH_GUTTER) == GC_PAN_WITH_GUTTER; }

	/// <summary>
	/// Determines whether the gesture pan with inertia feature is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsPanWithInertiaEnabled() const { return (Get(GID_PAN) & GC_PAN_WITH_INERTIA) == GC_PAN_WITH_INERTIA; }

	/// <summary>
	/// Modify specific gesture touch paramaters</summary>
	/// <returns>
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="dwID">Gesture feature ID. Can be one of the following: GID_ZOOM, GID_PAN, GID_ROTATE, GID_TWOFINGERTAP or GID_PRESSANDTAP</param>
	/// <param name="dwWant">Gesture features to enable. Can be 0 or GC_ALLGESTURES for all features except GID_PAN and GC_PAN or combination of the following flags: GC_PAN_WITH_SINGLE_FINGER_VERTICALLY, GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY, GC_PAN_WITH_GUTTER and GC_PAN_WITH_INTERTIA for gesture pan</param>
	BOOL Modify(DWORD dwID, DWORD dwWant = GC_ALLGESTURES, DWORD dwBlock = 0);

	/// <summary>
	/// Obtains a specific gesture touch paramaters</summary>
	/// <returns>
	/// Gesture features. Can be 0 or GC_ALLGESTURES for all features except GID_PAN and GC_PAN or combination of the following flags: GC_PAN_WITH_SINGLE_FINGER_VERTICALLY, GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY, GC_PAN_WITH_GUTTER and GC_PAN_WITH_INTERTIA for gesture pan</returns>
	/// <param name="dwID">Gesture feature ID. Can be one of the following: GID_ZOOM, GID_PAN, GID_ROTATE, GID_TWOFINGERTAP or GID_PRESSANDTAP</param>
	/// <param name="bWant">TRUE - the method returns the enabled features; FALSE - disabled</param>
	DWORD Get(DWORD dwID, BOOL bWant = TRUE) const;

#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	PGESTURECONFIG m_pConfigs;
	int	m_nConfigs;
};
#endif


/*============================================================================*/
// CWnd implementation

#include "afxrendertarget.h"

//---------------------------
// D2D notification messages:
//---------------------------
extern AFX_IMPORT_DATA UINT AFX_WM_DRAW2D;
extern AFX_IMPORT_DATA UINT AFX_WM_RECREATED2DRESOURCES;

// structures (see afxext.h)
struct CCreateContext;      // context for creating things
struct CPrintInfo;          // print preview customization info

struct AFX_MSGMAP_ENTRY
{
	UINT nMessage;   // windows message
	UINT nCode;      // control code or WM_NOTIFY code
	UINT nID;        // control ID (or 0 for windows messages)
	UINT nLastID;    // used for entries specifying a range of control id's
	UINT_PTR nSig;       // signature type (action) or pointer to message #
	AFX_PMSG pfn;    // routine to call (or special value)
};

/*============================================================================*/
// CWnd - a Microsoft Windows application window

class COleDropTarget;   // for more information see AFXOLE.H
class COleControlContainer;
class COleControlSite;

// CWnd::m_nFlags (generic to CWnd)
#define WF_TOOLTIPS         0x0001  // window is enabled for tooltips
#define WF_TEMPHIDE         0x0002  // window is temporarily hidden
#define WF_STAYDISABLED     0x0004  // window should stay disabled
#define WF_MODALLOOP        0x0008  // currently in modal loop
#define WF_CONTINUEMODAL    0x0010  // modal loop should continue running
#define WF_OLECTLCONTAINER  0x0100  // some descendant is an OLE control
#define WF_TRACKINGTOOLTIPS 0x0400  // window is enabled for tracking tooltips

// CWnd::m_nFlags (specific to CFrameWnd)
#define WF_STAYACTIVE       0x0020  // look active even though not active
#define WF_NOPOPMSG         0x0040  // ignore WM_POPMESSAGESTRING calls
#define WF_MODALDISABLE     0x0080  // window is disabled
#define WF_KEEPMINIACTIVE   0x0200  // stay activate even though you are deactivated


#define WF_NOWIN32ISDIALOGMSG   0x0800
#define WF_ISWINFORMSVIEWWND    0x1000

// flags for CWnd::RunModalLoop
#define MLF_NOIDLEMSG       0x0001  // don't send WM_ENTERIDLE messages
#define MLF_NOKICKIDLE      0x0002  // don't send WM_KICKIDLE messages
#define MLF_SHOWONIDLE      0x0004  // show window if not visible at idle time

// extra MFC defined TTF_ flags for TOOLINFO::uFlags
#define TTF_NOTBUTTON       0x80000000L // no status help on buttondown
#define TTF_ALWAYSTIP       0x40000000L // always show the tip even if not active

#if (NTDDI_VERSION < NTDDI_WIN7)
typedef struct tagTOUCHINPUT {
} TOUCHINPUT, *PTOUCHINPUT;
typedef struct tagGESTUREINFO {
} GESTUREINFO, *PGESTUREINFO;
#endif

class CWnd : public CCmdTarget
{
	DECLARE_DYNCREATE(CWnd)
protected:
	static const MSG* PASCAL GetCurrentMessage();

// Attributes
public:
	HWND m_hWnd;            // must be first data member
	operator HWND() const;
	BOOL operator==(const CWnd& wnd) const;
	BOOL operator!=(const CWnd& wnd) const;

	HWND GetSafeHwnd() const;
	DWORD GetStyle() const;
	DWORD GetExStyle() const;
	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);
	BOOL ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0);

	CWnd* GetOwner() const;
	void SetOwner(_In_ CWnd* pOwnerWnd);


	BOOL GetWindowInfo(PWINDOWINFO pwi) const;
	BOOL GetTitleBarInfo(PTITLEBARINFO pti) const;

// Constructors and other creation
	CWnd();

	static CWnd* PASCAL FromHandle(HWND hWnd);
	static CWnd* PASCAL FromHandlePermanent(HWND hWnd);
	static void PASCAL DeleteTempMap();
	BOOL Attach(HWND hWndNew);
	HWND Detach();

	// subclassing/unsubclassing functions
	virtual void PreSubclassWindow();
	BOOL SubclassWindow(HWND hWnd);
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent);
	HWND UnsubclassWindow();

	// handling of RT_DLGINIT resource (extension to RT_DIALOG)
	BOOL ExecuteDlgInit(LPCTSTR lpszResourceName);
	BOOL ExecuteDlgInit(LPVOID lpResource);

public:
	// for child windows, views, panes etc
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL);

	// advanced creation (allows access to extended styles)
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL);

	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		LPVOID lpParam = NULL);

	// for wrapping OLE controls
	BOOL CreateControl(REFCLSID clsid, LPCTSTR pszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID, CFile* pPersist=NULL,
		BOOL bStorage=FALSE, BSTR bstrLicKey=NULL);

	BOOL CreateControl(LPCTSTR pszClass, LPCTSTR pszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID, CFile* pPersist=NULL,
		BOOL bStorage=FALSE, BSTR bstrLicKey=NULL);

   // Another overload for creating controls that use default extents.
   BOOL CreateControl( REFCLSID clsid, LPCTSTR pszWindowName, DWORD dwStyle,
	  const POINT* ppt, const SIZE* psize, CWnd* pParentWnd, UINT nID,
	  CFile* pPersist = NULL, BOOL bStorage = FALSE, BSTR bstrLicKey = NULL );


   //Overload for special controls (WinForms), that require more than CLSID.
   BOOL CreateControl(const CControlCreationInfo& creationInfo, DWORD dwStyle,
	const POINT* ppt, const SIZE* psize, CWnd* pParentWnd, UINT nID);

	LPUNKNOWN GetControlUnknown();
	BOOL PaintWindowlessControls(CDC *pDC);

	virtual BOOL DestroyWindow();

	// special pre-creation and window rect adjustment hooks
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Advanced: virtual AdjustWindowRect
	enum AdjustType { adjustBorder = 0, adjustOutside = 1 };
	virtual void CalcWindowRect(LPRECT lpClientRect,
		UINT nAdjustType = adjustBorder);

// Window tree access
	int GetDlgCtrlID() const;
	int SetDlgCtrlID(int nID);
		// get and set window ID, for child windows only
	CWnd* GetDlgItem(int nID) const;
		// get immediate child with given ID
	void GetDlgItem(int nID, HWND* phWnd) const;
		// as above, but returns HWND
	CWnd* GetDescendantWindow(int nID, BOOL bOnlyPerm = FALSE) const;
		// like GetDlgItem but recursive
	void SendMessageToDescendants(UINT message, WPARAM wParam = 0,
		LPARAM lParam = 0, BOOL bDeep = TRUE, BOOL bOnlyPerm = FALSE);
	CFrameWnd* GetParentFrame() const;
	CFrameWnd* EnsureParentFrame() const;
	CWnd* GetTopLevelParent() const;
	CWnd* EnsureTopLevelParent() const;
	CWnd* GetTopLevelOwner() const;
	CWnd* GetParentOwner() const;
	CFrameWnd* GetTopLevelFrame() const;
	static CWnd* PASCAL GetSafeOwner(CWnd* pParent = NULL, HWND* pWndTop = NULL);

	CWnd* GetAncestor(UINT gaFlags) const;

// Message Functions
#pragma push_macro("SendMessage")
#undef SendMessage
	LRESULT _AFX_FUNCNAME(SendMessage)(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;
	LRESULT SendMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0) const;
#pragma pop_macro("SendMessage")
	BOOL PostMessage(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	BOOL SendNotifyMessage(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL SendChildNotifyLastMsg(LRESULT* pResult = NULL);

	BOOL DragDetect(POINT pt) const;


// Message processing for modeless dialog-like windows
	BOOL IsDialogMessage(LPMSG lpMsg);

// Window Text Functions
	void SetWindowText(LPCTSTR lpszString);
	int GetWindowText(_Out_writes_to_(nMaxCount, return + 1) LPTSTR lpszStringBuf, _In_ int nMaxCount) const;
	void GetWindowText(CString& rString) const;
	int GetWindowTextLength() const;
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);
	CFont* GetFont() const;

// CMenu Functions - non-Child windows only
	virtual CMenu* GetMenu() const;
	virtual BOOL SetMenu(CMenu* pMenu);
	void DrawMenuBar();
	CMenu* GetSystemMenu(BOOL bRevert) const;
	BOOL HiliteMenuItem(CMenu* pMenu, UINT nIDHiliteItem, UINT nHilite);

// Window Size and Position Functions
	BOOL IsIconic() const;
	BOOL IsZoomed() const;
	void MoveWindow(int x, int y, int nWidth, int nHeight,
				BOOL bRepaint = TRUE);
	void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);
	int SetWindowRgn(HRGN hRgn, BOOL bRedraw);
	int GetWindowRgn(HRGN hRgn) const;

	static AFX_DATA const CWnd wndTop; // SetWindowPos's pWndInsertAfter
	static AFX_DATA const CWnd wndBottom; // SetWindowPos's pWndInsertAfter
	static AFX_DATA const CWnd wndTopMost; // SetWindowPos pWndInsertAfter
	static AFX_DATA const CWnd wndNoTopMost; // SetWindowPos pWndInsertAfter

	BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
				int cx, int cy, UINT nFlags);
	UINT ArrangeIconicWindows();
	void BringWindowToTop();
	void GetWindowRect(LPRECT lpRect) const;
	void GetClientRect(LPRECT lpRect) const;

	BOOL GetWindowPlacement(WINDOWPLACEMENT* lpwndpl) const;
	BOOL SetWindowPlacement(const WINDOWPLACEMENT* lpwndpl);

// Coordinate Mapping Functions
	void ClientToScreen(LPPOINT lpPoint) const;
	void ClientToScreen(LPRECT lpRect) const;
	void ScreenToClient(LPPOINT lpPoint) const;
	void ScreenToClient(LPRECT lpRect) const;
	void MapWindowPoints(CWnd* pwndTo, LPPOINT lpPoint, UINT nCount) const;
	void MapWindowPoints(CWnd* pwndTo, LPRECT lpRect) const;

// Update/Painting Functions
	CDC* BeginPaint(LPPAINTSTRUCT lpPaint);
	void EndPaint(LPPAINTSTRUCT lpPaint);
	CDC* GetDC();
	CDC* GetWindowDC();
	int ReleaseDC(CDC* pDC);
	void Print(CDC* pDC, DWORD dwFlags) const;
	void PrintClient(CDC* pDC, DWORD dwFlags) const;

	void UpdateWindow();
	void SetRedraw(BOOL bRedraw = TRUE);
	BOOL GetUpdateRect(LPRECT lpRect, BOOL bErase = FALSE);
	int GetUpdateRgn(CRgn* pRgn, BOOL bErase = FALSE);
	void Invalidate(BOOL bErase = TRUE);
	void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
	void InvalidateRgn(CRgn* pRgn, BOOL bErase = TRUE);
	void ValidateRect(LPCRECT lpRect);
	void ValidateRgn(CRgn* pRgn);
	BOOL ShowWindow(int nCmdShow);
	BOOL IsWindowVisible() const;
	void ShowOwnedPopups(BOOL bShow = TRUE);

	CDC* GetDCEx(CRgn* prgnClip, DWORD flags);
	BOOL LockWindowUpdate();
	void UnlockWindowUpdate();
	BOOL RedrawWindow(LPCRECT lpRectUpdate = NULL,
		CRgn* prgnUpdate = NULL,
		UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
	BOOL EnableScrollBar(int nSBFlags, UINT nArrowFlags = ESB_ENABLE_BOTH);

	BOOL DrawAnimatedRects(int idAni, CONST RECT *lprcFrom, CONST RECT *lprcTo);
	BOOL DrawCaption(CDC* pDC, LPCRECT lprc, UINT uFlags);

	BOOL AnimateWindow(DWORD dwTime, DWORD dwFlags);
	BOOL PrintWindow(CDC* pDC, UINT nFlags) const;

public:
// Layered Window

	BOOL SetLayeredWindowAttributes(COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
	BOOL UpdateLayeredWindow(CDC* pDCDst, POINT *pptDst, SIZE *psize,
		CDC* pDCSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
	BOOL GetLayeredWindowAttributes(COLORREF *pcrKey, BYTE *pbAlpha, DWORD *pdwFlags) const;

// Timer Functions
	UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT nElapse,
		void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD));
	BOOL KillTimer(UINT_PTR nIDEvent);

// ToolTip Functions
	BOOL EnableToolTips(BOOL bEnable = TRUE);
	BOOL EnableTrackingToolTips(BOOL bEnable = TRUE);
	static void PASCAL CancelToolTips(BOOL bKeys = FALSE);
	void FilterToolTipMessage(MSG* pMsg);

	// for command hit testing (used for automatic tooltips)
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

// Window State Functions
	BOOL IsWindowEnabled() const;
	BOOL EnableWindow(BOOL bEnable = TRUE);

	// the active window applies only to top-level (frame windows)
	static CWnd* PASCAL GetActiveWindow();
	CWnd* SetActiveWindow();

	// the foreground window applies only to top-level windows (frame windows)
	BOOL SetForegroundWindow();
	static CWnd* PASCAL GetForegroundWindow();

	// capture and focus apply to all windows
	static CWnd* PASCAL GetCapture();
	CWnd* SetCapture();
	static CWnd* PASCAL GetFocus();
	CWnd* SetFocus();

	static CWnd* PASCAL GetDesktopWindow();

// Obsolete and non-portable APIs - not recommended for new code
	void CloseWindow();
	BOOL OpenIcon();

// Dialog-Box Item Functions
// (NOTE: Dialog-Box Items/Controls are not necessarily in dialog boxes!)
	void CheckDlgButton(int nIDButton, UINT nCheck);
	void CheckRadioButton(int nIDFirstButton, int nIDLastButton,
					int nIDCheckButton);
	int GetCheckedRadioButton(int nIDFirstButton, int nIDLastButton) const;
	int DlgDirList(_Inout_z_ LPTSTR lpPathSpec, _In_ int nIDListBox,
					_In_ int nIDStaticPath, _In_ UINT nFileType);
	int DlgDirListComboBox(_Inout_z_ LPTSTR lpPathSpec, _In_ int nIDComboBox,
					_In_ int nIDStaticPath, _In_ UINT nFileType);
	AFX_DEPRECATED("CWnd::DlgDirSelect(lpszOut, nControlId) is no longer supported. Instead, use CWnd::DlgDirSelect(lpszOut, nSize, nControlId)")
		BOOL DlgDirSelect(_Out_writes_z_(_MAX_PATH) LPTSTR lpString, _In_ int nIDListBox);
	BOOL DlgDirSelect(_Out_writes_z_(nSize) LPTSTR lpString, _In_ int nSize, _In_ int nIDListBox);
	AFX_DEPRECATED("CWnd::DlgDirSelectComboBox(lpszOut, nControlId) is no longer supported. Instead, use CWnd::DlgDirSelectComboBox(lpszOut, nSize, nControlId)")
		BOOL DlgDirSelectComboBox(_Out_writes_z_(_MAX_PATH) LPTSTR lpString, _In_ int nIDComboBox);
	BOOL DlgDirSelectComboBox(_Out_writes_z_(nSize) LPTSTR lpString, _In_ int nSize, _In_ int nIDComboBox);

	UINT GetDlgItemInt(int nID, BOOL* lpTrans = NULL,
					BOOL bSigned = TRUE) const;
	int GetDlgItemText(_In_ int nID, _Out_writes_to_(nMaxCount, return + 1) LPTSTR lpStr, _In_ int nMaxCount) const;
	int GetDlgItemText(int nID, CString& rString) const;
	CWnd* GetNextDlgGroupItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;
	COleControlSiteOrWnd* GetNextDlgGroupItem(COleControlSiteOrWnd *pCurSiteOrWnd = NULL) const;
	COleControlSiteOrWnd* GetPrevDlgGroupItem(COleControlSiteOrWnd *pCurSiteOrWnd = NULL) const;
	void RemoveRadioCheckFromGroup(const COleControlSiteOrWnd *pSiteOrWnd) const;
	CWnd* GetNextDlgTabItem(CWnd* pWndCtl, BOOL bPrevious = FALSE) const;
	COleControlSiteOrWnd* GetNextDlgTabItem(COleControlSiteOrWnd *pCurSiteOrWnd, BOOL bPrevious) const;
	UINT IsDlgButtonChecked(int nIDButton) const;
	LRESULT SendDlgItemMessage(int nID, UINT message,
					WPARAM wParam = 0, LPARAM lParam = 0);
	void SetDlgItemInt(int nID, UINT nValue, BOOL bSigned = TRUE);
	void SetDlgItemText(int nID, LPCTSTR lpszString);
	POSITION FindSiteOrWnd(const COleControlSiteOrWnd *pSiteOrWnd) const;
	POSITION FindSiteOrWndWithFocus() const;

// Scrolling Functions
	int GetScrollPos(int nBar) const;
	void GetScrollRange(int nBar, LPINT lpMinPos, LPINT lpMaxPos) const;
	void ScrollWindow(int xAmount, int yAmount,
					LPCRECT lpRect = NULL,
					LPCRECT lpClipRect = NULL);
	int SetScrollPos(int nBar, int nPos, BOOL bRedraw = TRUE);
	void SetScrollRange(int nBar, int nMinPos, int nMaxPos,
			BOOL bRedraw = TRUE);
	void ShowScrollBar(UINT nBar, BOOL bShow = TRUE);
	void EnableScrollBarCtrl(int nBar, BOOL bEnable = TRUE);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
			// return sibling scrollbar control (or NULL if none)

	int ScrollWindowEx(int dx, int dy,
				LPCRECT lpRectScroll, LPCRECT lpRectClip,
				CRgn* prgnUpdate, LPRECT lpRectUpdate, UINT flags);
	BOOL SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo,
		BOOL bRedraw = TRUE);
	BOOL GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL);
	int GetScrollLimit(int nBar);

	BOOL GetScrollBarInfo(LONG idObject, PSCROLLBARINFO psbi) const;

// Window Access Functions
	CWnd* ChildWindowFromPoint(POINT point) const;
	CWnd* ChildWindowFromPoint(POINT point, UINT nFlags) const;
	static CWnd* PASCAL FindWindow(LPCTSTR lpszClassName, LPCTSTR lpszWindowName);
	static CWnd* PASCAL FindWindowEx(HWND hwndParent, HWND hwndChildAfter, LPCTSTR lpszClass, LPCTSTR lpszWindow);

	CWnd* GetNextWindow(UINT nFlag = GW_HWNDNEXT) const;
	CWnd* GetTopWindow() const;

	CWnd* GetWindow(UINT nCmd) const;
	CWnd* GetLastActivePopup() const;

	BOOL IsChild(const CWnd* pWnd) const;
	CWnd* GetParent() const;
	CWnd* SetParent(CWnd* pWndNewParent);
	static CWnd* PASCAL WindowFromPoint(POINT point);

// Alert Functions
	BOOL FlashWindow(BOOL bInvert);
#pragma push_macro("MessageBox")
#undef MessageBox
	int _AFX_FUNCNAME(MessageBox)(LPCTSTR lpszText, LPCTSTR lpszCaption = NULL,
			UINT nType = MB_OK);
	int MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption = NULL,
			UINT nType = MB_OK);
#pragma pop_macro("MessageBox")

	BOOL FlashWindowEx(DWORD dwFlags, UINT  uCount, DWORD dwTimeout);

// Clipboard Functions
	BOOL ChangeClipboardChain(HWND hWndNext);
	HWND SetClipboardViewer();
	BOOL OpenClipboard();
	static CWnd* PASCAL GetClipboardOwner();
	static CWnd* PASCAL GetClipboardViewer();
	static CWnd* PASCAL GetOpenClipboardWindow();

// Caret Functions
	void CreateCaret(CBitmap* pBitmap);
	void CreateSolidCaret(int nWidth, int nHeight);
	void CreateGrayCaret(int nWidth, int nHeight);
	static CPoint PASCAL GetCaretPos();
	static void PASCAL SetCaretPos(POINT point);
	void HideCaret();
	void ShowCaret();

// Shell Interaction Functions
	void DragAcceptFiles(BOOL bAccept = TRUE);

// Icon Functions
	HICON SetIcon(HICON hIcon, BOOL bBigIcon);
	HICON GetIcon(_In_ BOOL bBigIcon) const;

// Context Help Functions
	BOOL SetWindowContextHelpId(DWORD dwContextHelpId);
	DWORD GetWindowContextHelpId() const;

// Dialog Data support
public:
	BOOL UpdateData(BOOL bSaveAndValidate = TRUE);
			// data wnd must be same type as this

// Help Command Handlers
	afx_msg void OnHelp();          // F1 (uses current context)
	afx_msg void OnHelpIndex();     // ID_HELP_INDEX
	afx_msg void OnHelpFinder();    // ID_HELP_FINDER, ID_DEFAULT_HELP
	afx_msg void OnHelpUsing();     // ID_HELP_USING
	virtual void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

// Layout and other functions
public:
	enum RepositionFlags
		{ reposDefault = 0, reposQuery = 1, reposExtra = 2, reposNoPosLeftOver=0x8000 };
	void RepositionBars(UINT nIDFirst, UINT nIDLast, UINT nIDLeftOver,
		UINT nFlag = reposDefault, LPRECT lpRectParam = NULL,
		LPCRECT lpRectClient = NULL, BOOL bStretch = TRUE);

	// dialog support
	void UpdateDialogControls(CCmdTarget* pTarget, BOOL bDisableIfNoHndler);
	void CenterWindow(CWnd* pAlternateOwner = NULL);
	int RunModalLoop(DWORD dwFlags = 0);
	virtual BOOL ContinueModal();
	virtual void EndModalLoop(int nResult);

// OLE control wrapper functions
   COleControlSite* GetOleControlSite(UINT idControl) const;
	void AFX_CDECL InvokeHelper(DISPID dwDispID, WORD wFlags,
		VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, ...);
	void AFX_CDECL SetProperty(DISPID dwDispID, VARTYPE vtProp, ...);
	void GetProperty(DISPID dwDispID, VARTYPE vtProp, void* pvProp) const;
	IUnknown* GetDSCCursor();
	void BindDefaultProperty(DISPID dwDispID, VARTYPE vtProp, LPCTSTR szFieldName, CWnd* pDSCWnd);
	void BindProperty(DISPID dwDispId, CWnd* pWndDSC);

// Accessibility Support
public :
	void EnableActiveAccessibility();
	void NotifyWinEvent(DWORD event, LONG idObjectType, LONG idObject);

public :
	// Windows 7 taskbar Tabs support

	/// <summary>
	/// Called by the framework when it needs to obtain a bitmap to be displayed on Windows 7 tab thumbnail,
	/// or on the client for application peek. </summary>
	/// <remarks>
	/// Override this method in a derived class and draw on the specified device context in order to customize thumbnail and peek.
	/// If bThumbnail is TRUE, szRequiredThumbnailSize can be ignored. In this case you should be aware
	/// that you draw full sized bitmap (e.g. a bitmap that cover the whole client area). The device context (dc) comes with selected 32 bits bitmap.
	/// The default implementation sends WM_PRINT to this window with PRF_CLIENT, PRF_CHILDREN and PRF_NONCLIENT flags.</remarks>
	/// <param name="dc"> Specifies the device context.</param>
	/// <param name="rect"> Specifies the bounding rectangle of area to render.</param>
	/// <param name="szRequiredThumbnailSize"> Specifies the size of target thumbnail. Should be ignored if bIsThumbnail is FALSE.</param>
	/// <param name="bIsThumbnail"> Specifies whether this method is called for iconic thumbnail or live preview (peek).</param>
	/// <param name="bAlphaChannelSet"> Output parameter. Set it to TRUE if your implementation initializes alpha channel of a bitmap
	/// selected in dc.</param>
	virtual void OnDrawIconicThumbnailOrLivePreview(CDC& dc, CRect rect, CSize szRequiredThumbnailSize, BOOL bIsThumbnail, BOOL& bAlphaChannelSet);

protected :
	bool m_bEnableActiveAccessibility;
	IAccessible* m_pStdObject;
	friend BOOL AFXAPI AfxWinInit(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
		_In_z_ LPTSTR lpCmdLine, _In_ int nCmdShow);

protected:
	IAccessibleProxy* m_pProxy;
	afx_msg LRESULT OnGetObject(WPARAM, LPARAM);

	DECLARE_INTERFACE_MAP()

	class XAccessible //: public IAccessible
	{
	public:
		virtual ULONG __stdcall AddRef();
		virtual ULONG __stdcall Release();
		virtual HRESULT __stdcall QueryInterface(REFIID iid, LPVOID* ppvObj);
		virtual HRESULT __stdcall Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);
		virtual HRESULT __stdcall GetIDsOfNames(REFIID, LPOLESTR *, UINT, LCID, DISPID *);
		virtual HRESULT __stdcall GetTypeInfoCount(unsigned int *);
		virtual HRESULT __stdcall GetTypeInfo(unsigned int, LCID, ITypeInfo**);
		virtual HRESULT __stdcall get_accParent(IDispatch **ppdispParent);
		virtual HRESULT __stdcall get_accChildCount(long *pcountChildren);
		virtual HRESULT __stdcall get_accChild(VARIANT varChild, IDispatch **ppdispChild);
		virtual HRESULT __stdcall get_accName(VARIANT varChild, BSTR *pszName);
		virtual HRESULT __stdcall get_accValue(VARIANT varChild, BSTR *pszValue);
		virtual HRESULT __stdcall get_accDescription(VARIANT varChild, BSTR *pszDescription);
		virtual HRESULT __stdcall get_accRole(VARIANT varChild, VARIANT *pvarRole);
		virtual HRESULT __stdcall get_accState(VARIANT varChild, VARIANT *pvarState);
		virtual HRESULT __stdcall get_accHelp(VARIANT varChild, BSTR *pszHelp);
		virtual HRESULT __stdcall get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic);
		virtual HRESULT __stdcall get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut);
		virtual HRESULT __stdcall get_accFocus(VARIANT *pvarChild);
		virtual HRESULT __stdcall get_accSelection(VARIANT *pvarChildren);
		virtual HRESULT __stdcall get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);
		virtual HRESULT __stdcall accSelect(long flagsSelect, VARIANT varChild);
		virtual HRESULT __stdcall accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
		virtual HRESULT __stdcall accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
		virtual HRESULT __stdcall accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
		virtual HRESULT __stdcall accDoDefaultAction(VARIANT varChild);
		virtual HRESULT __stdcall put_accName(VARIANT varChild, BSTR szName);
		virtual HRESULT __stdcall put_accValue(VARIANT varChild, BSTR szValue);
	} m_xAccessible;
	friend class XAccessible;

	class XAccessibleServer //: public IAccessibleServer
	{
	public:
		virtual ULONG __stdcall AddRef();
		virtual ULONG __stdcall Release();
		virtual HRESULT __stdcall QueryInterface(REFIID iid, LPVOID* ppvObj);
		virtual HRESULT __stdcall SetProxy(IAccessibleProxy *pProxy);
		virtual HRESULT __stdcall GetHWND(HWND *phWnd);
		virtual HRESULT __stdcall GetEnumVariant(IEnumVARIANT **ppEnumVariant);
	} m_xAccessibleServer;
	friend class XAccessibleServer;

public :
	virtual HRESULT EnsureStdObj();

	virtual HRESULT get_accParent(IDispatch **ppdispParent);
	virtual HRESULT get_accChildCount(long *pcountChildren);
	virtual HRESULT get_accChild(VARIANT varChild, IDispatch **ppdispChild);
	virtual HRESULT get_accName(VARIANT varChild, BSTR *pszName);
	virtual HRESULT get_accValue(VARIANT varChild, BSTR *pszValue);
	virtual HRESULT get_accDescription(VARIANT varChild, BSTR *pszDescription);
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);
	virtual HRESULT get_accHelp(VARIANT varChild, BSTR *pszHelp);
	virtual HRESULT get_accHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long *pidTopic);
	virtual HRESULT get_accKeyboardShortcut(VARIANT varChild, BSTR *pszKeyboardShortcut);
	virtual HRESULT get_accFocus(VARIANT *pvarChild);
	virtual HRESULT get_accSelection(VARIANT *pvarChildren);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);
	virtual HRESULT accSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT accLocation(long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight, VARIANT varChild);
	virtual HRESULT accNavigate(long navDir, VARIANT varStart, VARIANT *pvarEndUpAt);
	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT *pvarChild);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	//Obsolete
	virtual HRESULT put_accName(VARIANT varChild, BSTR szName);
	//Obsolete
	virtual HRESULT put_accValue(VARIANT varChild, BSTR szValue);
	virtual HRESULT SetProxy(IAccessibleProxy *pProxy);
	virtual HRESULT CreateAccessibleProxy(WPARAM wParam, LPARAM lParam, LRESULT *pResult);

	// Helpers for windows that contain windowless controls
	long GetWindowLessChildCount();
	long GetWindowedChildCount();
	long GetAccessibleChildCount();
	HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	HRESULT GetAccessibilityLocation(VARIANT varChild, long *pxLeft, long *pyTop, long *pcxWidth, long *pcyHeight);
	HRESULT GetAccessibilityHitTest(long xLeft, long yTop, VARIANT *pvarChild);


// Window-Management message handler member functions
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg LRESULT OnActivateTopLevel(WPARAM, LPARAM);
	afx_msg void OnCancelMode();
	afx_msg void OnChildActivate();
	afx_msg void OnClose();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnDestroy();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnHelpInfo(HELPINFO* lpHelpInfo);
	afx_msg void OnIconEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	afx_msg void OnSyncPaint();
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg UINT OnNotifyFormat(CWnd* pWnd, UINT nCommand);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnQueryEndSession();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg BOOL OnQueryOpen();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTCard(UINT idAction, DWORD dwActionData);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnSessionChange(UINT nSessionState, UINT nId);

	afx_msg void OnChangeUIState(UINT nAction, UINT nUIElement);
	afx_msg void OnUpdateUIState(UINT nAction, UINT nUIElement);
	afx_msg UINT OnQueryUIState();

// Nonclient-Area message handler member functions
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseHover(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseLeave();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnNcRButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcXButtonDown(short zHitTest, UINT nButton, CPoint point);
	afx_msg void OnNcXButtonUp(short zHitTest, UINT nButton, CPoint point);
	afx_msg void OnNcXButtonDblClk(short zHitTest, UINT nButton, CPoint point);

// System message handler member functions
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnPaletteIsChanging(CWnd* pRealizeWnd);
	afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSysDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey);
	afx_msg void OnRawInput(UINT nInputCode, HRAWINPUT hRawInput);
	afx_msg void OnCompacting(UINT nCpuTime);
	afx_msg void OnDevModeChange(_In_z_ LPTSTR lpDeviceName);
	afx_msg void OnFontChange();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnSpoolerStatus(UINT nStatus, UINT nJobs);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimeChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnWinIniChange(LPCTSTR lpszSection);
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM lEventData);
	afx_msg void OnUserChanged();
	afx_msg void OnInputLangChange(UINT nCharSet, UINT nLocaleId);
	afx_msg void OnInputLangChangeRequest(UINT nFlags, UINT nLocaleId);
	afx_msg void OnInputDeviceChange(unsigned short nFlags, HANDLE hDevice);

// Input message handler member functions
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDeadChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUniChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnXButtonDblClk(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnXButtonDown(UINT nFlags, UINT nButton, CPoint point);
	afx_msg void OnXButtonUp(UINT nFlags, UINT nButton, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

// Initialization message handler member functions
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUnInitMenuPopup(CMenu* pPopupMenu, UINT nFlags);

// Clipboard message handler member functions
	afx_msg void OnAskCbFormatName(_In_ UINT nMaxCount, _Out_writes_z_(nMaxCount) LPTSTR lpszString);
	afx_msg void OnChangeCbChain(HWND hWndRemove, HWND hWndAfter);
	afx_msg void OnDestroyClipboard();
	afx_msg void OnDrawClipboard();
	afx_msg void OnHScrollClipboard(CWnd* pClipAppWnd, UINT nSBCode, UINT nPos);
	afx_msg void OnPaintClipboard(CWnd* pClipAppWnd, HGLOBAL hPaintStruct);
	afx_msg void OnRenderAllFormats();
	afx_msg void OnRenderFormat(UINT nFormat);
	afx_msg void OnSizeClipboard(CWnd* pClipAppWnd, HGLOBAL hRect);
	afx_msg void OnVScrollClipboard(CWnd* pClipAppWnd, UINT nSBCode, UINT nPos);
	afx_msg void OnClipboardUpdate();

// Control message handler member functions
	afx_msg int OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);

// MDI message handler member functions
	afx_msg void OnMDIActivate(BOOL bActivate,
		CWnd* pActivateWnd, CWnd* pDeactivateWnd);

// Menu loop notification messages
	afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu);
	afx_msg void OnMenuRButtonUp(UINT nPos, CMenu* pMenu);
	afx_msg UINT OnMenuDrag(UINT nPos, CMenu* pMenu);
	afx_msg UINT OnMenuGetObject(MENUGETOBJECTINFO* pMenuGetObjectInfo);
	afx_msg void OnMenuCommand(UINT nPos, CMenu* pMenu);
	afx_msg void OnNextMenu(UINT nKey, LPMDINEXTMENU lpMdiNextMenu);

// Win4 messages
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnMoving(UINT nSide, LPRECT lpRect);
	afx_msg void OnEnterSizeMove();
	afx_msg void OnExitSizeMove();
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);

// Desktop Windows Manager messages
	afx_msg void OnCompositionChanged();
	afx_msg void OnNcRenderingChanged(BOOL bIsRendering);
	afx_msg void OnColorizationColorChanged(DWORD dwColorizationColor, BOOL bOpacity);
	afx_msg void OnWindowMaximizedChange(BOOL bIsMaximized);

// touch and gesture messages:
#if (NTDDI_VERSION >= NTDDI_WIN7)
	afx_msg LRESULT OnTouchMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTabletQuerySystemGestureStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGesture(WPARAM wParam, LPARAM lParam);
#endif

// Overridables and other helpers (for implementation of derived classes)
protected:
	// for deriving from a standard control
	virtual WNDPROC* GetSuperWndProcAddr();

	// for dialog data exchange and validation
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	// for modality
	virtual void BeginModalState();
	virtual void EndModalState();

	// for translating Windows messages in main message pump
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// for ambient properties exposed to contained OLE controls
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid,
		VARIANT* pvar);

	// for touch:

	/// <summary>
	/// Register/Unregister window Windows touch support</summary>
	/// <returns>
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="bRegister">TRUE - register Windows touch support; FALSE - otherwise.</param>
	/// <param name="ulFlags">A set of bit flags that specify optional modifications. This field may contain 0 or one of the following values: TWF_FINETOUCH; TWF_WANTPALM</param>
	BOOL RegisterTouchWindow(BOOL bRegister = TRUE, ULONG ulFlags = 0);

	/// <summary>
	/// Specifies whether CWnd has touch support</summary>
	/// <returns>
	/// TRUE if CWnd has touch support; otherwise FALSE.</returns>
	BOOL IsTouchWindow() const;

	// gesture:
#if (NTDDI_VERSION >= NTDDI_WIN7)
	/// <summary>
	/// Set gesture touch parameters</summary>
	/// <returns>
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="pConfig">Pointer to CGestureConfig. Cannot be NULL.</param>
	BOOL SetGestureConfig(CGestureConfig* pConfig);

	/// <summary>
	/// Get gesture touch parameters</summary>
	/// <returns>
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="pConfig">Pointer to CGestureConfig. Cannot be NULL.</param>
	BOOL GetGestureConfig(CGestureConfig* pConfig);

	/// <summary>
	/// Returns the current gesture information (PGESTUREINFO)</summary>
	/// <returns>
	/// Pointer to the current gesture info.</returns>
	const PGESTUREINFO GetCurrentGestureInfo() const
	{
		return m_pCurrentGestureInfo;
	}
#endif

	// controls dynamic layout:

	/// <summary>
	/// Enables or disables layout manager for a window.</summary>
	/// <param name="bEnable"> TRUE - enable layout management, FALSE - disable layout management.</param>
	void EnableDynamicLayout(BOOL bEnable = TRUE);

	/// <summary>
	/// This function returns TRUE, if layout management is enabled for a window; otherwise FALSE.</summary>
	/// <remarks>
	/// Call EnableDynamicLayout in order to enable or disable layout management for a window.</remarks>
	/// <returns>
	/// TRUE, if layout management is enabled for a window; otherwise FALSE.</returns>
	BOOL IsDynamicLayoutEnabled() const { return m_pDynamicLayout != NULL; }

	/// <summary>
	/// Call this function to retrieve a pointer to layout manager.</summary>
	/// <remarks>
	/// Call EnableDynamicLayout in order to enable or disable layout management for a window.</remarks>
	/// <returns>
	/// Returns a pointer to the window layout manager or NULL if layout wasn't enabled.</returns>
	CMFCDynamicLayout* GetDynamicLayout() { return m_pDynamicLayout; }

protected:
	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	// for handling default processing
	LRESULT Default();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	// for custom cleanup after WM_NCDESTROY
	virtual void PostNcDestroy();

	// for notifications from parent
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		// return TRUE if parent should not process this message
	BOOL ReflectChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	static BOOL PASCAL ReflectLastMsg(HWND hWndChild, LRESULT* pResult = NULL);

	// for touch:
	BOOL m_bIsTouchWindowRegistered;

	/// <summary>
	/// Process inputs from Windows touch</summary>
	/// <returns>
	/// TRUE if application processes Windows touch inputs; otherwise FALSE.</returns>
	/// <param name="nInputsCount">total number of Windows touch inputs.</param>
	/// <param name="pInputs">array of TOUCHINPUT.</param>
	virtual BOOL OnTouchInputs(UINT nInputsCount, PTOUCHINPUT pInputs);

	/// <summary>
	/// Process single input from Windows touch</summary>
	/// <returns>
	/// TRUE if application processes Windows touch input; otherwise FALSE.</returns>
	/// <param name="pt">point where screen has been tocuhed (in the client coordinates).</param>
	/// <param name="nInputNumber">number of touch input.</param>
	/// <param name="nInputsCount">total number of touch inputs.</param>
	/// <param name="pInput">pointer to TOUCHINPUT structure.</param>
	virtual BOOL OnTouchInput(CPoint pt, int nInputNumber, int nInputsCount, PTOUCHINPUT pInput);

	/// <summary>
	/// The methods is called when the system asks a window which system gestures it would like to receive</summary>
	/// <returns>
	/// A value indicating which system gestures the window would like to receive (TABLET_* flags, see WM_TABLET_QUERYSYSTEMGESTURESTATUS message).</returns>
	/// <param name="ptTouch">point where screen has been tocuhed (in the client coordinates).</param>
	virtual ULONG GetGestureStatus(CPoint ptTouch);

	// for gesture:
	CPoint		 m_ptGestureFrom;
	ULONGLONG	 m_ulGestureArg;
	BOOL		 m_bGestureInited;
	PGESTUREINFO m_pCurrentGestureInfo;

	/// <summary>
	/// The method is called upon gesture zoom event</summary>
	/// <returns>
	/// TRUE if application processes this event; otherwise FALSE.</returns>
	/// <param name="ptCenter">Zoom center point. In client coordinates</param>
	/// <param name="lDelta">The distance from the center point. In pixels</param>
	virtual BOOL OnGestureZoom(CPoint ptCenter, long lDelta);

	/// <summary>
	/// The method is called upon gesture pan event</summary>
	/// <returns>
	/// TRUE if application processes this event; otherwise FALSE.</returns>
	/// <param name="ptFrom">Pan starting point. In client coordinates</param>
	/// <param name="ptTo">Pan current point. In client coordinates</param>
	virtual BOOL OnGesturePan(CPoint ptFrom, CPoint ptTo);

	/// <summary>
	/// The method is called upon gesture rotate event</summary>
	/// <returns>
	/// TRUE if application processes this event; otherwise FALSE.</returns>
	/// <param name="ptCenter">Rotation center point. In client coordinates</param>
	/// <param name="dblAngle">Rotation angle. In radians</param>
	virtual BOOL OnGestureRotate(CPoint ptCenter, double dblAngle);

	/// <summary>
	/// The method is called upon gesture 2 finger tap event</summary>
	/// <returns>
	/// TRUE if application processes this event; otherwise FALSE.</returns>
	/// <param name="ptCenter">Center point between 2 fingers. In client coordinates</param>
	virtual BOOL OnGestureTwoFingerTap(CPoint ptCenter);

	/// <summary>
	/// The method is called upon gesture press and tap event</summary>
	/// <returns>
	/// TRUE if application processes this event; otherwise FALSE.</returns>
	/// <param name="ptPress">"Pressed" point. In client coordinates</param>
	/// <param name="lDelta">The distance from the "pressed" point. In pixels</param>
	virtual BOOL OnGesturePressAndTap(CPoint ptPress, long lDelta);

	// controls layouts:
	CMFCDynamicLayout* m_pDynamicLayout;

	/// <summary>
	/// The method is called to adjust positions of child controls.
	/// It recalculates positions of child controls if layout management is enabled for a window.</summary>
	virtual void ResizeDynamicLayout();
	void InitDynamicLayout();
	BOOL LoadDynamicLayoutResource(LPCTSTR lpszResourceName);

// Implementation
public:
	virtual ~CWnd();
	virtual BOOL CheckAutoCenter();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	static BOOL PASCAL GrayCtlColor(HDC hDC, HWND hWnd, UINT nCtlColor,
		HBRUSH hbrGray, COLORREF clrText);

	// helper routines for implementation
	BOOL HandleFloatingSysCommand(UINT nID, LPARAM lParam);
	BOOL IsTopParentActive() const;
	void ActivateTopParent();
	static BOOL PASCAL WalkPreTranslateTree(HWND hWndStop, MSG* pMsg);
	static CWnd* PASCAL GetDescendantWindow(HWND hWnd, int nID,
		BOOL bOnlyPerm);
	static void PASCAL SendMessageToDescendants(HWND hWnd, UINT message,
		WPARAM wParam, LPARAM lParam, BOOL bDeep, BOOL bOnlyPerm);
	virtual BOOL IsFrameWnd() const; // IsKindOf(RUNTIME_CLASS(CFrameWnd)))
	virtual void OnFinalRelease();
	BOOL PreTranslateInput(LPMSG lpMsg);
	static BOOL PASCAL ModifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd,
		UINT nFlags);
	static BOOL PASCAL ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd,
		UINT nFlags);
	static void PASCAL _FilterToolTipMessage(MSG* pMsg, CWnd* pWnd);
	BOOL _EnableToolTips(BOOL bEnable, UINT nFlag);
	static HWND PASCAL GetSafeOwner_(HWND hWnd, HWND* pWndTop);
	void PrepareForHelp();

	COleControlContainer* GetControlContainer();

public:
	HWND m_hWndOwner;   // implementation of SetOwner and GetOwner
	UINT m_nFlags;      // see WF_ flags above

protected:
	WNDPROC m_pfnSuper; // for subclassing of controls
	static const UINT m_nMsgDragList;
	int m_nModalResult; // for return values from CWnd::RunModalLoop

	COleDropTarget* m_pDropTarget;  // for automatic cleanup of drop target
	friend class COleDropTarget;
	friend class CFrameWnd;

	// for creating dialogs and dialog-like windows
	BOOL CreateDlg(LPCTSTR lpszTemplateName, CWnd* pParentWnd);
	BOOL CreateDlgIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd, HINSTANCE hInst);
	BOOL CreateRunDlgIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd, HINSTANCE hInst);

	COleControlContainer* m_pCtrlCont;  // for containing OLE controls
	COleControlSite* m_pCtrlSite;       // for wrapping an OLE control
	friend class COccManager;
	friend class COleControlSite;
	friend class COleControlContainer;
	BOOL InitControlContainer(BOOL bCreateFromResource=FALSE);
	virtual BOOL CreateControlContainer(COleControlContainer** ppContainer);
	virtual BOOL CreateControlSite(COleControlContainer* pContainer,
		COleControlSite** ppSite, UINT nID, REFCLSID clsid);
	virtual BOOL SetOccDialogInfo(struct _AFX_OCC_DIALOG_INFO* pOccDialogInfo);
	virtual _AFX_OCC_DIALOG_INFO* GetOccDialogInfo();
	void AttachControlSite(CHandleMap* pMap);
public:
	void AttachControlSite(CWnd* pWndParent, UINT nIDC = 0);
	COleControlContainer* GetControlContainer() const
	{
		return m_pCtrlCont;
	}
	COleControlSite* GetControlSite() const
	{
		return m_pCtrlSite;
	}

public:
	CMFCControlContainer* GetMFCControlContainer() const
	{
		return m_pMFCCtrlContainer;
	}

protected:
	CMFCControlContainer* m_pMFCCtrlContainer;  // for containing MFC Feature Pack controls

protected:
	// implementation of message dispatch/hooking
	friend LRESULT CALLBACK _AfxSendMsgHook(int, WPARAM, LPARAM);
	friend void AFXAPI _AfxStandardSubclass(HWND);
	friend LRESULT CALLBACK _AfxCbtFilterHook(int, WPARAM, LPARAM);
	friend LRESULT AFXAPI AfxCallWndProc(CWnd*, HWND, UINT, WPARAM, LPARAM);

	// standard message implementation
	afx_msg LRESULT OnNTCtlColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDisplayChange(UINT nImageDepth, int cxScreen, int cyScreen);
	afx_msg LRESULT OnDragList(WPARAM, LPARAM);

	// Helper functions for retrieving Text from windows message / structure
	// -----------------------------------------------------------------------
	// errCode  - the errCode for the window message, uMsg
	// pszText  - buffer to grow and retrieve the text (do not allocate when calling, the function will allocate)
	// cch      - size of the buffer in TCHAR to be pass to the windows message, uMsg
	// cchBegin - initial size to allocate
	// cchEnd   - maximum size to allocate
	// uMsg     - window message
	// lParam   - the LPARAM of the message.  This is pass by reference because it could potentially be alias of pszText/cch for some messages.
	// wParam   - the WPARAM of the message.  This is pass by reference because it could potentially be alias of pszText/cch for some messages.
	// strOut   - the CString containing the received text

	template <class TReturnType, class TCchType >
	TReturnType EnlargeBufferGetText(_In_ TReturnType errCode, LPTSTR& pszText, TCchType& cch, TCchType cchBegin, TCchType cchEnd, UINT uMsg, WPARAM& wParam, LPARAM& lParam, CString& strOut) const
	{
		ENSURE(::IsWindow(m_hWnd));
		ENSURE(cchBegin < cchEnd);
		ENSURE(cchEnd <= INT_MAX); // CString only support up to INT_MAX
		TReturnType retCode = errCode;
		strOut = CString();
		cch = cchBegin;
		do
		{
			pszText = strOut.GetBufferSetLength(cch);
			retCode = static_cast<TReturnType>(this->SendMessage(uMsg, wParam, lParam));
			strOut.ReleaseBuffer();
			pszText = NULL;

			if (retCode == errCode)
			{
				// error clear the string and return error
				strOut = CString();
				cch=0;
				break;
			}
			if (static_cast<TCchType>(strOut.GetLength()) < cch-1)
			{
				cch = strOut.GetLength();
				break;
			}
		}
		while( (::ATL::AtlMultiply(&cch, cch, 2) == S_OK) && (cch < cchEnd));
		return retCode;
	}


	template <class TReturnType>
	inline TReturnType EnlargeBufferGetText(TReturnType errCode, LPTSTR& pszText, int& pcch, UINT uMsg, WPARAM& wParam, LPARAM& lParam, CString& strOut) const
	{
		return EnlargeBufferGetText<TReturnType, int>(errCode, pszText, pcch, 256, INT_MAX, uMsg, wParam, lParam, strOut);
	}

	template <class TReturnType>
	inline TReturnType EnlargeBufferGetText(TReturnType errCode, LPTSTR& pszText, UINT& pcch, UINT uMsg, WPARAM& wParam, LPARAM& lParam, CString& strOut) const
	{
		// using INT_MAX instead of UINT_MAX here because CString has a INT_MAX limit
		return EnlargeBufferGetText<TReturnType, UINT>(errCode, pszText, pcch, 256, INT_MAX, uMsg, wParam, lParam, strOut);
	}

	DECLARE_MESSAGE_MAP()

private:
	CWnd(HWND hWnd);    // just for special initialization

// Support for D2D:
public:
	/// <summary>
	/// Enable/disable window D2D support This method should be called prior to initialization of the main window .</summary>
	/// <param name="bEnable">Specifies whether to turn on, or off D2D support.</param>
	/// <param name="bUseDCRenderTarget">Specifies whether to use CHwndRenderTarget or CDCRenderTarget.</param>
	void EnableD2DSupport(BOOL bEnable = TRUE, BOOL bUseDCRenderTarget = FALSE);

	/// <summary>
	/// Determines whether the D2D support is enabled</summary>
	/// <returns>
	/// TRUE if the feature is enabled; otherwise FALSE.</returns>
	BOOL IsD2DSupportEnabled();

	/// <summary>
	/// Get HWND render target associated with this window</summary>
	/// <returns>
	/// Pointer to the render target or NULL.</returns>
	CHwndRenderTarget* GetRenderTarget();

	/// <summary>
	/// Get DC render target associated with this window</summary>
	/// <returns>
	/// Pointer to the render target or NULL.</returns>
	CDCRenderTarget* GetDCRenderTarget();

	// Obsolete D2D methods:
	CHwndRenderTarget* LockRenderTarget() { return GetRenderTarget(); }
	void UnlockRenderTarget() {}

protected:
	CRenderTarget* m_pRenderTarget;
	BOOL DoD2DPaint();
};

// helpers for registering your own WNDCLASSes
LPCTSTR AFXAPI AfxRegisterWndClass(UINT nClassStyle,
	HCURSOR hCursor = 0, HBRUSH hbrBackground = 0, HICON hIcon = 0);

BOOL AFXAPI AfxRegisterClass(WNDCLASS* lpWndClass);

// helper to initialize rich edit 1.0 control
BOOL AFXAPI AfxInitRichEdit();
// helper to initialize rich edit 2.0 control
BOOL AFXAPI AfxInitRichEdit2();

#ifdef _UNICODE
// helper to initialize rich edit 4.1 control
BOOL AFXAPI AfxInitRichEdit5();
#endif
// Implementation
LRESULT CALLBACK AfxWndProc(HWND, UINT, WPARAM, LPARAM);

WNDPROC AFXAPI AfxGetAfxWndProc();
#define AfxWndProc (*AfxGetAfxWndProc())

typedef void (AFX_MSG_CALL CWnd::*AFX_PMSGW)(void);
	// like 'AFX_PMSG' but for CWnd derived classes only

typedef void (AFX_MSG_CALL CWinThread::*AFX_PMSGT)(void);
	// like 'AFX_PMSG' but for CWinThread-derived classes only

/*============================================================================*/
// CDialog - a modal or modeless dialog
class CDialog : public CWnd
{
	DECLARE_DYNAMIC(CDialog)

	// Modeless construct
public:
	CDialog();
	void Initialize();

	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual BOOL CreateIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd = NULL,
		void* lpDialogInit = NULL);
	virtual BOOL CreateIndirect(HGLOBAL hDialogTemplate, CWnd* pParentWnd = NULL);

	// Modal construct
public:
	explicit CDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	explicit CDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	BOOL InitModalIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd = NULL,
		void* lpDialogInit = NULL);
	BOOL InitModalIndirect(HGLOBAL hDialogTemplate, CWnd* pParentWnd = NULL);

// Attributes
public:
	void MapDialogRect(LPRECT lpRect) const;
	void SetHelpID(UINT nIDR);

// Operations
public:
	// modal processing
	virtual INT_PTR DoModal();

	// support for passing on tab control - use 'PostMessage' if needed
	void NextDlgCtrl() const;
	void PrevDlgCtrl() const;
	void GotoDlgCtrl(CWnd* pWndCtrl);

	// default button access
	void SetDefID(UINT nID);
	DWORD GetDefID() const;

	// termination
	void EndDialog(int nResult);

// Overridables (special message map entries)
	virtual BOOL OnInitDialog();
	virtual void OnSetFont(CFont* pFont);
protected:
	virtual void OnOK();
	virtual void OnCancel();

// Implementation
public:
	virtual ~CDialog();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL CheckAutoCenter();

protected:
	UINT m_nIDHelp;                 // Help ID (0 for none, see HID_BASE_RESOURCE)

	// parameters for 'DoModal'
	LPCTSTR m_lpszTemplateName;     // name or MAKEINTRESOURCE
	HGLOBAL m_hDialogTemplate;      // indirect (m_lpDialogTemplate == NULL)
	LPCDLGTEMPLATE m_lpDialogTemplate;  // indirect if (m_lpszTemplateName == NULL)
	void* m_lpDialogInit;           // DLGINIT resource data
	CWnd* m_pParentWnd;             // parent/owner window
	HWND m_hWndTop;                 // top level parent window (may be disabled)
	BOOL m_bClosedByEndDialog;		// indicates that the dialog was closed by calling EndDialog method

	_AFX_OCC_DIALOG_INFO* m_pOccDialogInfo;
	virtual BOOL SetOccDialogInfo(_AFX_OCC_DIALOG_INFO* pOccDialogInfo);
	virtual _AFX_OCC_DIALOG_INFO* GetOccDialogInfo();
	virtual void PreInitDialog();

	// implementation helpers
	HWND PreModal();
	void PostModal();

	BOOL CreateIndirect(LPCDLGTEMPLATE lpDialogTemplate, CWnd* pParentWnd,
		void* lpDialogInit, HINSTANCE hInst);
	BOOL CreateIndirect(HGLOBAL hDialogTemplate, CWnd* pParentWnd,
		HINSTANCE hInst);

protected:
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg void OnPaint();
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);

	DECLARE_MESSAGE_MAP()
};

// all CModalDialog functionality is now in CDialog
#define CModalDialog    CDialog

/*============================================================================*/
// Standard Windows controls

class CStatic : public CWnd
{
	DECLARE_DYNAMIC(CStatic)

// Constructors
public:
	CStatic();
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

// Operations
	HICON SetIcon(HICON hIcon);
	HICON GetIcon() const;

	HENHMETAFILE SetEnhMetaFile(HENHMETAFILE hMetaFile);
	HENHMETAFILE GetEnhMetaFile() const;
	HBITMAP SetBitmap(HBITMAP hBitmap);
	HBITMAP GetBitmap() const;
	HCURSOR SetCursor(HCURSOR hCursor);
	HCURSOR GetCursor();

// Overridables (for owner draw only)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CStatic();
protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

class CButton : public CWnd
{
	DECLARE_DYNAMIC(CButton)

// Constructors
public:
	CButton();
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	UINT GetState() const;
	void SetState(BOOL bHighlight);
	int GetCheck() const;
	void SetCheck(int nCheck);
	UINT GetButtonStyle() const;
	void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE);

	HICON SetIcon(HICON hIcon);
	HICON GetIcon() const;
	HBITMAP SetBitmap(HBITMAP hBitmap);
	HBITMAP GetBitmap() const;
	HCURSOR SetCursor(HCURSOR hCursor);
	HCURSOR GetCursor();

	AFX_ANSI_DEPRECATED BOOL GetIdealSize(_Out_ LPSIZE psize) const;
	AFX_ANSI_DEPRECATED BOOL SetImageList(_In_ PBUTTON_IMAGELIST pbuttonImagelist);
	AFX_ANSI_DEPRECATED BOOL GetImageList(_In_ PBUTTON_IMAGELIST pbuttonImagelist) const;
	AFX_ANSI_DEPRECATED BOOL SetTextMargin(_In_ LPRECT pmargin);
	AFX_ANSI_DEPRECATED BOOL GetTextMargin(_Out_ LPRECT pmargin) const;

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	CString GetNote() const;
	_Check_return_ BOOL GetNote(_Out_writes_z_(*pcchNote) LPTSTR lpszNote, _Inout_ UINT* pcchNote) const;
	BOOL SetNote(_In_z_ LPCTSTR lpszNote);
	UINT GetNoteLength() const;
	BOOL GetSplitInfo(_Out_ PBUTTON_SPLITINFO pInfo) const;
	BOOL SetSplitInfo(_In_ PBUTTON_SPLITINFO pInfo);
	UINT GetSplitStyle() const;
	BOOL SetSplitStyle(_In_ UINT nStyle);
	BOOL GetSplitSize(_Out_ LPSIZE pSize) const;
	BOOL SetSplitSize(_In_ LPSIZE pSize);
	CImageList* GetSplitImageList() const;
	BOOL SetSplitImageList(_In_ CImageList* pSplitImageList);
	TCHAR GetSplitGlyph() const;
	BOOL SetSplitGlyph(_In_ TCHAR chGlyph);
	BOOL SetDropDownState(_In_ BOOL fDropDown);

	// Sets whether the action associated with the button requires elevated permissions.
	// If elevated permissions are required then the button should display an elevated icon.
	HICON SetShield(_In_ BOOL fElevationRequired);
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

// Overridables (for owner draw only)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~CButton();
protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

#if defined(UNICODE)
class CSplitButton : public CButton
{
	DECLARE_DYNAMIC(CSplitButton)

// Constructors
public:
	CSplitButton();
	CSplitButton(UINT nMenuId, UINT nSubMenuId);
	CSplitButton(CMenu* pMenu);

	~CSplitButton();

	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle,
				const RECT& rect, CWnd* pParentWnd, UINT nID);

	void SetDropDownMenu(UINT nMenuId, UINT nSubMenuId);
	void SetDropDownMenu(CMenu* pMenu);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDown(NMHDR* /*pNMHDR*/, LRESULT *pResult);

	void Cleanup();

	CMenu * m_pMenu;
	UINT m_nMenuId;
	UINT m_nSubMenuId;
};

#endif // defined(UNICODE)

class CListBox : public CWnd
{
	DECLARE_DYNAMIC(CListBox)

// Constructors
public:
	CListBox();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes

	// for entire listbox
	int GetCount() const;
	int GetHorizontalExtent() const;
	void SetHorizontalExtent(int cxExtent);
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
	int InitStorage(int nItems, UINT nBytes);
	UINT ItemFromPoint(CPoint pt, BOOL& bOutside) const;

	DWORD GetListBoxInfo() const;

	// for single-selection listboxes
	int GetCurSel() const;
	int SetCurSel(int nSelect);

	// for multiple-selection listboxes
	int GetSel(int nIndex) const;           // also works for single-selection
	int SetSel(int nIndex, BOOL bSelect = TRUE);
	int GetSelCount() const;
	int GetSelItems(int nMaxItems, LPINT rgIndex) const;
	void SetAnchorIndex(int nIndex);
	int GetAnchorIndex() const;

	// for listbox items
	DWORD_PTR GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD_PTR dwItemData);
	void* GetItemDataPtr(int nIndex) const;
	int SetItemDataPtr(int nIndex, void* pData);
	int GetItemRect(int nIndex, LPRECT lpRect) const;
	int GetText(_In_ int nIndex, _Pre_notnull_ _Post_z_ LPTSTR lpszBuffer) const;
	void GetText(int nIndex, CString& rString) const;
	int GetTextLen(int nIndex) const;

	// Settable only attributes
	void SetColumnWidth(int cxWidth);
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops);
	void SetTabStops();
	BOOL SetTabStops(const int& cxEachStop);    // takes an 'int'

	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int GetCaretIndex() const;
	int SetCaretIndex(int nIndex, BOOL bScroll = TRUE);

// Operations
	// manipulating listbox items
	int AddString(LPCTSTR lpszItem);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszItem);
	void ResetContent();
	int Dir(UINT attr, LPCTSTR lpszWildCard);

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszItem) const;
	int SelectString(int nStartAfter, LPCTSTR lpszItem);
	int SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem);

// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual int VKeyToItem(UINT nKey, UINT nIndex);
	virtual int CharToItem(UINT nKey, UINT nIndex);

// Implementation
public:
	virtual ~CListBox();
protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

class CCheckListBox : public CListBox
{
	DECLARE_DYNAMIC(CCheckListBox)

// Constructors
public:
	CCheckListBox();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	void SetCheckStyle(UINT nStyle);
	UINT GetCheckStyle();
	void SetCheck(int nIndex, int nCheck);
	int GetCheck(int nIndex);
	void Enable(int nIndex, BOOL bEnabled = TRUE);
	BOOL IsEnabled(int nIndex);

	virtual CRect OnGetCheckPosition(CRect rectItem, CRect rectCheckBox);

// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

// Implementation
protected:
	void PreDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void PreMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	int PreCompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	void PreDeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	bool PreDrawItemThemed(CDC* pDC, DRAWITEMSTRUCT &drawItem, int nCheck, int cyItem);
	void PreDrawItemNonThemed(CDC* pDC, DRAWITEMSTRUCT &drawItem, int nCheck, int cyItem);
	void PreDrawItemHelper(LPDRAWITEMSTRUCT lpDrawItemStruct);

	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);

   void SetSelectionCheck( int nCheck );

// Active Accessibility
	virtual HRESULT get_accRole(VARIANT varChild, VARIANT *pvarRole);
	virtual HRESULT get_accState(VARIANT varChild, VARIANT *pvarState);
	virtual HRESULT get_accDefaultAction(VARIANT varChild, BSTR *pszDefaultAction);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);

#ifdef _DEBUG
	virtual void PreSubclassWindow();
#endif

	int CalcMinimumItemHeight();
	void InvalidateCheck(int nIndex);
	void InvalidateItem(int nIndex);
	int CheckFromPoint(CPoint point, BOOL& bInCheck);

	int m_cyText;
	UINT m_nStyle;

	// Message map functions
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetFont(CFont* pFont, BOOL bRedraw);
	afx_msg LRESULT OnLBAddString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBFindString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBFindStringExact(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBGetItemData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSelectString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSetItemData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLBSetItemHeight(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

class CComboBox : public CWnd
{
	DECLARE_DYNAMIC(CComboBox)

// Constructors
public:
	CComboBox();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	// for entire combo box
	int GetCount() const;
	int GetCurSel() const;
	int SetCurSel(int nSelect);
	LCID GetLocale() const;
	LCID SetLocale(LCID nNewLocale);
// Win4
	int GetTopIndex() const;
	int SetTopIndex(int nIndex);
	int InitStorage(int nItems, UINT nBytes);
	void SetHorizontalExtent(UINT nExtent);
	UINT GetHorizontalExtent() const;
	int SetDroppedWidth(UINT nWidth);
	int GetDroppedWidth() const;

	BOOL GetComboBoxInfo(PCOMBOBOXINFO pcbi) const;

#if defined(UNICODE)
	// Sets the minimum number of visible items in the drop-down list of the combo box.
	BOOL SetMinVisibleItems(_In_ int iMinVisible);

	// Retrieves the minimum number of visible items in the drop-down list of the combo box.
	int GetMinVisible() const;

#ifdef CB_SETCUEBANNER
	// REVIEW: Sets the cue banner text displayed in the edit control of the combo box.
	BOOL SetCueBanner(_In_z_ LPCTSTR lpszText);

	// REVIEW: Retrieves the cue banner text displayed in the edit control of the combo box.
	CString GetCueBanner() const;
	BOOL GetCueBanner(_Out_writes_z_(cchText) LPTSTR lpszText, _In_ int cchText) const;
#endif  // CB_SETCUEBANNER
#endif  // defined(UNICODE)

	// for edit control
	DWORD GetEditSel() const;
	BOOL LimitText(int nMaxChars);
	BOOL SetEditSel(_In_ int nStartChar, _In_ int nEndChar);

	// for combobox item
	DWORD_PTR GetItemData(int nIndex) const;
	int SetItemData(int nIndex, DWORD_PTR dwItemData);
	void* GetItemDataPtr(int nIndex) const;
	int SetItemDataPtr(int nIndex, void* pData);
	int GetLBText(_In_ int nIndex, _Pre_notnull_ _Post_z_ LPTSTR lpszText) const;
	void GetLBText(int nIndex, CString& rString) const;
	int GetLBTextLen(int nIndex) const;

	int SetItemHeight(int nIndex, UINT cyItemHeight);
	int GetItemHeight(int nIndex) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
	int SetExtendedUI(BOOL bExtended = TRUE);
	BOOL GetExtendedUI() const;
	void GetDroppedControlRect(LPRECT lprect) const;
	BOOL GetDroppedState() const;

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE);

	// manipulating listbox items
	int AddString(LPCTSTR lpszString);
	int DeleteString(UINT nIndex);
	int InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString);
	void ResetContent();
	int Dir(_In_ UINT attr, _In_ LPCTSTR lpszWildCard);

	// selection helpers
	int FindString(_In_ int nStartAfter, _In_z_ LPCTSTR lpszString) const;
	int SelectString(int nStartAfter, LPCTSTR lpszString);

	// Clipboard operations
	void Clear();
	void Copy();
	void Cut();
	void Paste();

// Overridables (must override draw, measure and compare for owner draw)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

// Implementation
public:
	virtual ~CComboBox();
protected:
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
};

// NOTE: This class must remain a binary-compatible subset
// of CEditView. Do not add data members or virtual functions
// directly to this class.
class CEdit : public CWnd
{
	// DECLARE_DYNAMIC virtual OK - CWnd already has DECLARE_DYNAMIC
	DECLARE_DYNAMIC(CEdit)

// Constructors
public:
	CEdit();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	BOOL CanUndo() const;
	int GetLineCount() const;
	BOOL GetModify() const;
	void SetModify(BOOL bModified = TRUE);
	void GetRect(LPRECT lpRect) const;
	DWORD GetSel() const;
	void GetSel(int& nStartChar, int& nEndChar) const;
	HLOCAL GetHandle() const;
	void SetHandle(HLOCAL hBuffer);
	void SetMargins(UINT nLeft, UINT nRight);
	DWORD GetMargins() const;
	void SetLimitText(UINT nMax);
	UINT GetLimitText() const;
	CPoint PosFromChar(UINT nChar) const;
	int CharFromPos(CPoint pt) const;

	// NOTE: first word in lpszBuffer must contain the size of the buffer!
	// NOTE: may not return null character
	int GetLine(_In_ int nIndex, _Out_ LPTSTR lpszBuffer) const;
	// NOTE: may not return null character
	int GetLine(_In_ int nIndex, _Out_writes_to_(nMaxLength, return) LPTSTR lpszBuffer, _In_ int nMaxLength) const;

	AFX_ANSI_DEPRECATED BOOL SetCueBanner(_In_z_ LPCWSTR lpszText, _In_ BOOL fDrawIfFocused = FALSE);
	AFX_ANSI_DEPRECATED BOOL GetCueBanner(_Out_writes_z_(cchText) LPWSTR lpszText, _In_ int cchText) const;

#if defined(UNICODE)
	CString GetCueBanner() const;

	BOOL ShowBalloonTip(_In_z_ LPCWSTR lpszTitle, _In_z_ LPCWSTR lpszText, _In_ INT ttiIcon = TTI_NONE);
	BOOL ShowBalloonTip(_In_ PEDITBALLOONTIP pEditBalloonTip);
	BOOL HideBalloonTip();
#endif  // (UNICODE)

#if defined(UNICODE)
	// REVIEW: Sets the characters in the edit control that are highlighted.
	void SetHighlight(_In_ int ichStart, _In_ int ichEnd);

	// REVIEW: Retrieves the characters in the edit control that are highlighted.
	BOOL GetHighlight(_Out_ int* pichStart, _Out_ int* pichEnd) const;
#endif  // defined(UNICODE)

// Operations
	void EmptyUndoBuffer();
	BOOL FmtLines(BOOL bAddEOL);

	void LimitText(int nChars = 0);
	int LineFromChar(int nIndex = -1) const;
	int LineIndex(int nLine = -1) const;
	int LineLength(int nLine = -1) const;
	void LineScroll(int nLines, int nChars = 0);
	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
	void SetPasswordChar(TCHAR ch);
	void SetRect(LPCRECT lpRect);
	void SetRectNP(LPCRECT lpRect);
	void SetSel(DWORD dwSelection, BOOL bNoScroll = FALSE);
	void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops);
	void SetTabStops();
	BOOL SetTabStops(const int& cxEachStop);    // takes an 'int'

	// Clipboard operations
	BOOL Undo();
	void Clear();
	void Copy();
	void Cut();
	void Paste();

	BOOL SetReadOnly(BOOL bReadOnly = TRUE);
	int GetFirstVisibleLine() const;
	TCHAR GetPasswordChar() const;

// Implementation
public:
	// virtual OK here - ~CWnd already virtual
	virtual ~CEdit();
};

class CScrollBar : public CWnd
{
	DECLARE_DYNAMIC(CScrollBar)

// Constructors
public:
	CScrollBar();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
	int GetScrollPos() const;
	int SetScrollPos(int nPos, BOOL bRedraw = TRUE);
	void GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const;
	void SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE);
	void ShowScrollBar(BOOL bShow = TRUE);

	BOOL EnableScrollBar(UINT nArrowFlags = ESB_ENABLE_BOTH);

	BOOL SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE);
	BOOL GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL);
	int GetScrollLimit();
	BOOL GetScrollBarInfo(PSCROLLBARINFO pScrollInfo) const;

// Implementation
public:
	virtual ~CScrollBar();
};

#define AFX_PREVIEW_VIEW_ID 1

/// <summary>
/// This class implements a window that is placed on a host window provided by the Shell
/// for Rich Preview. </summary>
class CMFCPreviewCtrlImpl : public CWnd
{
	DECLARE_DYNAMIC(CMFCPreviewCtrlImpl)

public:
	/// <summary>
	/// Constructs a preview control object.</summary>
	CMFCPreviewCtrlImpl();
	/// <summary>
	/// Destructs a preview control object.</summary>
	virtual ~CMFCPreviewCtrlImpl();

	/// <summary>
	/// Called by preview handler to create a relationship between document implementation and
	/// preview control.</summary>
	/// <param name="pDocument" A pointer to document implementation.</param>
	void SetDocument(ATL::IDocument* pDocument)
	{
		m_pDocument = pDocument;
	}

	/// <summary>
	/// Returns a document connected to this preview control.</summary>
	/// <returns> A pointer to a document, whose content is previewed in the control.</returns>
	ATL::IDocument* GetDocument()
	{
		return m_pDocument;
	}

	/// <summary>
	/// Called by a Rich Preview handler to create the Windows window. </summary>
	/// <returns> TRUE if creation succeeded; otherwise FALSE.</returns>
	/// <param name="hWndParent"> A handle to the host window supplied by the Shell for Rich Preview. </param>
	/// <param name="prc"> Specifies initial size and position of the window. </param>
	virtual BOOL Create(HWND hWndParent, const RECT* prc);

	/// <summary>
	/// Called by a Rich Preview handler to create the Windows window. </summary>
	/// <returns>TRUE if creation succeeded; otherwise FALSE.</returns>
	/// <param name="hWndParent"> A handle to the host window supplied by the Shell for Rich Preview. </param>
	/// <param name="prc"> Specifies initial size and position of the window. </param>
	/// <param name="pContext"> A pointer to a creation context.</param>
	virtual BOOL Create(HWND hWndParent, const RECT* prc, CCreateContext* pContext);

	/// <summary>
	/// Called by a Rich Preview handler when it needs to set visuals of rich preview content.</summary>
	/// <param name="clrBack"> Background color of preview window. </param>
	/// <param name="clrText"> Text color of preview window. </param>
	/// <param name="plf"> Font used to display texts in preview window. </param>
	virtual void SetPreviewVisuals(COLORREF clrBack, COLORREF clrText, const LOGFONTW *plf);

	/// <summary>
	/// Called by a Rich Preview handler when it needs to destroy this control.</summary>
	virtual void Destroy();

	/// <summary>
	/// Sets a new parent for this control. </summary>
	/// <param name="hWndParent">A handle to the new parent window.</param>
	virtual void SetHost(HWND hWndParent);

	/// <summary>
	/// Tells this control to redraw. </summary>
	virtual void Redraw();

	/// <summary>
	/// Sets a new bounding rectangle for this control. </summary>
	/// <remarks> Usually new bounding rectangle is set when the host control is resized.</remarks>
	/// <param name="prc">Specifies the new size and position of preview control.</param>
	/// <param name="bRedraw">Specifies whether the control should be redrawn.</param>
	virtual void SetRect(const RECT* prc, BOOL bRedraw);

	/// <summary>
	/// Sets input focus to this control. </summary>
	virtual void Focus();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	/// <summary> Called by the framework to render the preview.</summary>
	/// <param name="pDC">A pointer to a device context for painting.</param>
	virtual void DoPaint(CPaintDC* pDC);

protected:
	/// <summary> Background color of preview window. </summary>
	COLORREF m_clrBackColor;
	/// <summary> Text color of preview window. </summary>
	COLORREF m_clrTextColor;
	/// <summary> Font used to display texts in preview window. </summary>
	CFont    m_font;
	/// <summary> A pointer to a document whose content is previewed in the control.</summary>
	ATL::IDocument* m_pDocument;
};

/*============================================================================*/
// CFrameWnd - base class for SDI and other frame windows

// Frame window styles
#define FWS_ADDTOTITLE  0x00008000L // modify title based on content
#define FWS_PREFIXTITLE 0x00004000L // show document name before app name
#define FWS_SNAPTOBARS  0x00002000L // snap size to size of contained bars

// Frame window menu bar visibility styles
#define AFX_MBV_KEEPVISIBLE    0x01L // always visible
#define AFX_MBV_DISPLAYONFOCUS 0x02L // toggle state on ALT
#define AFX_MBV_DISPLAYONF10   0x04L // display on F10

// Frame window menu bar visibility states
#define AFX_MBS_VISIBLE 0x01L // visible
#define AFX_MBS_HIDDEN  0x02L // hidden

struct CPrintPreviewState;  // forward reference (see afxext.h)
class CControlBar;          // forward reference (see afxext.h)
class CReBar;               // forward reference (see afxext.h)

class CDockBar;             // forward reference (see afxpriv.h)
class CMiniDockFrameWnd;    // forward reference (see afxpriv.h)
class CDockState;           // forward reference (see afxpriv.h)

class COleFrameHook;        // forward reference (see ..\src\oleimpl2.h)

class CFrameWnd : public CWnd
{
	DECLARE_DYNCREATE(CFrameWnd)

// Constructors
public:
	static AFX_DATA const CRect rectDefault;
	CFrameWnd();

	BOOL LoadAccelTable(LPCTSTR lpszResourceName);
	virtual BOOL Create(LPCTSTR lpszClassName,
				LPCTSTR lpszWindowName,
				DWORD dwStyle = WS_OVERLAPPEDWINDOW,
				const RECT& rect = rectDefault,
				CWnd* pParentWnd = NULL,        // != NULL for popups
				LPCTSTR lpszMenuName = NULL,
				DWORD dwExStyle = 0,
				CCreateContext* pContext = NULL);

	// dynamic creation - load frame and associated resources
	virtual BOOL LoadFrame(UINT nIDResource,
				DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
				CWnd* pParentWnd = NULL,
				CCreateContext* pContext = NULL);

	// special helper for view creation
	CWnd* CreateView(CCreateContext* pContext, UINT nID = AFX_IDW_PANE_FIRST);

// Attributes
	virtual CDocument* GetActiveDocument();

	// Active child view maintenance
	CView* GetActiveView() const;           // active view or NULL
	void SetActiveView(CView* pViewNew, BOOL bNotify = TRUE);
		// active view or NULL, bNotify == FALSE if focus should not be set

	// Active frame (for frames within frames -- MDI)
	virtual CFrameWnd* GetActiveFrame();

	// For customizing the default messages on the status bar
	virtual void GetMessageString(UINT nID, CString& rMessage) const;

	BOOL m_bAutoMenuEnable;
		// TRUE => menu items without handlers will be disabled

	BOOL IsTracking();

// Operations
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual void ActivateFrame(int nCmdShow = -1);
	void InitialUpdateFrame(CDocument* pDoc, BOOL bMakeVisible);
	void SetTitle(LPCTSTR lpszTitle);
	CString GetTitle() const;
	virtual CMenu* GetMenu() const;
	virtual BOOL SetMenu(CMenu* pMenu);
	virtual UINT GetTrackingID() { return m_nIDTracking; }

	// set/get menu bar visibility style
	virtual void SetMenuBarVisibility(DWORD dwStyle);
	virtual DWORD GetMenuBarVisibility() const;

	// set/get menu bar visibility state
	virtual BOOL SetMenuBarState(DWORD dwState);
	virtual DWORD GetMenuBarState() const;

#if (NTDDI_VERSION >= NTDDI_WIN7)
	/// <summary>
	/// Sets range for Windows 7 progress bar displayed on taskbar. </summary>
	/// <param name="nRangeMin">Minimal value.</param>
	/// <param name="nRangeMax">Maximal value.</param>
	void SetProgressBarRange(int nRangeMin, int nRangeMax);
	/// <summary>
	/// Sets current position for Windows 7 progress bar displayed on taskbar.</summary>
	/// <param name="nProgressPos">Specifies the position to set. It must be within range set by SetProgressBarRange.</param>
	void SetProgressBarPosition(int nProgressPos);
	/// <summary>Sets the type and state of the progress indicator displayed on a taskbar button.</summary>
	/// <param name="tbpFlags">Flags that control the current state of the progress button. Specify only one of the following flags; all states are mutually exclusive of all others:
	/// TBPF_NOPROGRESS, TBPF_INDETERMINATE, TBPF_NORMAL, TBPF_ERROR, TBPF_PAUSED.</param>
	void SetProgressBarState(TBPFLAG tbpFlags);
	/// <summary>
	/// Applies an overlay to a taskbar button to indicate application status or a notification to the user.</summary>
	/// <returns> TRUE if successful; FALSE if OS version is less than Windows 7 or if an error occurs setting the icon.</returns>
	/// <param name="nIDResource">Specifies Resource ID of an icon to use as the overlay. See description for hIcon for details.</param>
	/// <param name="lpcszDescr">A pointer to a string that provides an alt text version of the information conveyed by the overlay, for accessibility purposes.</param>
	BOOL SetTaskbarOverlayIcon(UINT nIDResource, LPCTSTR lpcszDescr);
	/// <summary>
	/// Applies an overlay to a taskbar button to indicate application status or a notification to the user.</summary>
	/// <returns> TRUE if successful; FALSE if OS version is less than Windows 7 or if an error occurs setting the icon.</returns>
	/// <param name="hIcon"> The handle of an icon to use as the overlay. This should be a small icon, measuring 16x16 pixels at 96 dots per inch (dpi).
	/// If an overlay icon is already applied to the taskbar button, that existing overlay is replaced.
	/// This value can be NULL. How a NULL value is handled depends on whether the taskbar button represents a single window or a group of windows.
	/// It is the responsibility of the calling application to free hIcon when it is no longer needed.</param>
	/// <param name="lpcszDescr">A pointer to a string that provides an alt text version of the information conveyed by the overlay, for accessibility purposes.</param>
	BOOL SetTaskbarOverlayIcon(HICON hIcon, LPCTSTR lpcszDescr);
#endif

	BOOL GetMenuBarInfo(LONG idObject, LONG idItem, PMENUBARINFO pmbi) const;

	/// <summary> Designates the specified view to be the active view for Rich Preview.</summary>
	/// <param name="pViewNew"> A pointer to a view that should be activated.</param>
	void SetActivePreviewView(CView* pViewNew);

	// to set text of standard status bar
	void SetMessageText(LPCTSTR lpszText);
	void SetMessageText(UINT nID);

	// control bar docking
	void EnableDocking(DWORD dwDockStyle);
	void DockControlBar(CControlBar* pBar, UINT nDockBarID = 0,
		LPCRECT lpRect = NULL);
	void FloatControlBar(CControlBar* pBar, CPoint point,
		DWORD dwStyle = CBRS_ALIGN_TOP);
	CControlBar* GetControlBar(UINT nID);

	// frame window based modality
	virtual void BeginModalState();
	virtual void EndModalState();
	BOOL InModalState() const;
	void ShowOwnedWindows(BOOL bShow);

	// saving and loading control bar state
	void LoadBarState(LPCTSTR lpszProfileName);
	void SaveBarState(LPCTSTR lpszProfileName) const;
	void ShowControlBar(CControlBar* pBar, BOOL bShow, BOOL bDelay);
	void SetDockState(const CDockState& state);
	void GetDockState(CDockState& state) const;

// Overridables
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	virtual CWnd* GetMessageBar();

	// border space negotiation
	enum BorderCmd
		{ borderGet = 1, borderRequest = 2, borderSet = 3 };
	virtual BOOL NegotiateBorderSpace(UINT nBorderCmd, LPRECT lpRectBorder);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// Command Handlers
public:
	afx_msg void OnContextHelp();   // for Shift+F1 help
	afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnBarCheck(UINT nID);

// Implementation
public:
	virtual ~CFrameWnd();
	int m_nWindow;  // general purpose window number - display as ":n"
					// -1 => unknown, 0 => only window viewing document
					// 1 => first of many windows viewing document, 2=> second

	HMENU m_hMenuDefault;       // default menu resource for this frame
	HACCEL m_hAccelTable;       // accelerator table
	DWORD m_dwPromptContext;    // current help prompt context for message box
	BOOL m_bHelpMode;           // if TRUE, then Shift+F1 help mode is active
	CFrameWnd* m_pNextFrameWnd; // next CFrameWnd in app global list
	CRect m_rectBorder;         // for OLE border space negotiation
	COleFrameHook* m_pNotifyHook;

	CPtrList m_listControlBars; // array of all control bars that have this
								// window as their dock site
	int m_nShowDelay;           // SW_ command for delay show/hide

	CMiniDockFrameWnd* CreateFloatingFrame(DWORD dwStyle);
	DWORD CanDock(CRect rect, DWORD dwDockStyle,
		CDockBar** ppDockBar = NULL); // called by CDockContext
	void AddControlBar(CControlBar *pBar);
	void RemoveControlBar(CControlBar *pBar);
	void DockControlBar(CControlBar* pBar, CDockBar* pDockBar,
		LPCRECT lpRect = NULL);
	void ReDockControlBar(CControlBar* pBar, CDockBar* pDockBar,
		LPCRECT lpRect = NULL);
	void NotifyFloatingWindows(DWORD dwFlags);
	void DestroyDockBars();

protected:
	UINT m_nIDHelp;             // Help ID (0 for none, see HID_BASE_RESOURCE)
	UINT m_nIDTracking;         // tracking command ID or string IDS
	UINT m_nIDLastMessage;      // last displayed message string IDS
	CView* m_pViewActive;       // current active view
	BOOL (CALLBACK* m_lpfnCloseProc)(CFrameWnd* pFrameWnd);
	UINT m_cModalStack;         // BeginModalState depth
	HWND* m_phWndDisable;       // windows disabled because of BeginModalState
	HMENU m_hMenuAlt;           // menu to update to (NULL means default)
	CString m_strTitle;         // default title (original)
	BOOL m_bInRecalcLayout;     // avoid recursion in RecalcLayout
	CRuntimeClass* m_pFloatingFrameClass;
	static const DWORD dwDockBarMap[4][2];
    DWORD m_dwMenuBarVisibility;      // menu bar visibility style
	DWORD m_dwMenuBarState;           // menu bar visibility state
	HMENU m_hMenu;                    // backed menu for restoring from the hidden state
	BOOL  m_bTempShowMenu;            // temporarily show the menu bar to enable menu access keys
	BOOL  m_bMouseHitMenu;            // if TRUE, the mouse is hitting the menu bar

	int m_nProgressBarRangeMin; // Win7 taskbar support - min progress range
	int m_nProgressBarRangeMax; // Win7 taskbar support - max progress range

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL IsFrameWnd() const;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);
	virtual HACCEL GetDefaultAccelerator();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// idle update of frame user interface
	enum IdleFlags
		{ idleMenu = 1, idleTitle = 2, idleNotify = 4, idleLayout = 8 };
	UINT m_nIdleFlags;          // set of bit flags for idle processing
	virtual void DelayUpdateFrameMenu(HMENU hMenuAlt);
	void DelayUpdateFrameTitle();
	void DelayRecalcLayout(BOOL bNotify = TRUE);

	// for Shift+F1 help support
	BOOL CanEnterHelpMode();
	virtual void ExitHelpMode();

	// implementation helpers
public:
	void UpdateFrameTitleForDocument(LPCTSTR lpszDocName);
protected:
	LPCTSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();   // default to delete this.
	int OnCreateHelper(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	void BringToTop(int nCmdShow);
		// bring window to top for SW_ commands which affect z-order

	// implementation helpers for Shift+F1 help mode
	BOOL ProcessHelpMsg(MSG& msg, DWORD* pContext);
	HWND SetHelpCapture(POINT point, BOOL* pbDescendant);

	// CFrameWnd list management
	void AddFrameWnd();
	void RemoveFrameWnd();

	// called before changing the menu bar visibility state
	virtual void OnShowMenuBar();
	virtual void OnHideMenuBar();

	friend class CWnd;  // for access to m_bModalDisable
	friend class CReBar; // for access to m_bInRecalcLayout

	// Windows messages
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnInitMenu(CMenu*);
	afx_msg void OnInitMenuPopup(CMenu*, UINT, BOOL);
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg LRESULT OnPopMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHelpPromptAddr(WPARAM wParam, LPARAM lParam);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg void OnEnterIdle(UINT nWhy, CWnd* pWho);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnActivateTopLevel(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	// standard commands
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateKeyIndicator(CCmdUI* pCmdUI);
	afx_msg void OnHelp();
	afx_msg void OnUpdateContextHelp(CCmdUI* pCmdUI);
	afx_msg BOOL OnChevronPushed(UINT id, NMHDR *pnm, LRESULT *result);
	afx_msg void OnDDEInitiate(CWnd* pWndClient, UINT uiAtomApp, UINT uiAtomTopic);
	afx_msg void OnDDEExecute(CWnd* pWndClient, HANDLE handle);
	afx_msg void OnDDETerminate(CWnd* pWnd);

	DECLARE_MESSAGE_MAP()

	friend class CWinApp;
};

/*============================================================================*/
// MDI Support

class CMDIFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CMDIFrameWnd)

public:
// Constructors
	CMDIFrameWnd();

// Operations
	void MDIActivate(CWnd* pWndActivate);
	CMDIChildWnd* MDIGetActive(BOOL* pbMaximized = NULL) const;
	void MDIIconArrange();
	void MDIMaximize(CWnd* pWnd);
	void MDINext();
	void MDIPrev();
	void MDIRestore(CWnd* pWnd);
	CMenu* MDISetMenu(CMenu* pFrameMenu, CMenu* pWindowMenu);
	void MDITile();
	void MDICascade();
	void MDITile(int nType);
	void MDICascade(int nType);
	CMDIChildWnd* CreateNewChild(CRuntimeClass* pClass, UINT nResource,
		HMENU hMenu = NULL, HACCEL hAccel = NULL);

// Overridables
	// MFC 1.0 backward compatible CreateClient hook (called by OnCreateClient)
	virtual BOOL CreateClient(LPCREATESTRUCT lpCreateStruct, CMenu* pWindowMenu);
	// customize if using an 'Window' menu with non-standard IDs
	virtual HMENU GetWindowMenuPopup(HMENU hMenuBar);

// Implementation
public:
	HWND m_hWndMDIClient;       // MDI Client window handle

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource,
				DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
				CWnd* pParentWnd = NULL,
				CCreateContext* pContext = NULL);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);
	virtual void DelayUpdateFrameMenu(HMENU hMenuAlt);
	virtual CFrameWnd* GetActiveFrame();
	virtual void SetMenuBarVisibility(DWORD dwStyle);
	virtual BOOL SetMenuBarState(DWORD dwState);

protected:
	virtual LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateMDIWindowCmd(CCmdUI* pCmdUI);
	afx_msg BOOL OnMDIWindowCmd(UINT nID);
	afx_msg void OnWindowNew();
	afx_msg LRESULT OnCommandHelp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT, CMenu*);

	DECLARE_MESSAGE_MAP()
};

class CMDIChildWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CMDIChildWnd)

// Constructors
public:
	CMDIChildWnd();

	virtual BOOL Create(LPCTSTR lpszClassName,
				LPCTSTR lpszWindowName,
				DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
				const RECT& rect = rectDefault,
				CMDIFrameWnd* pParentWnd = NULL,
				CCreateContext* pContext = NULL);

// Attributes
	CMDIFrameWnd* GetMDIFrame();

// Operations
	void MDIDestroy();
	void MDIActivate();
	void MDIMaximize();
	void MDIRestore();
	void SetHandles(HMENU hMenu, HACCEL hAccel);
	virtual UINT GetTrackingID();

// Implementation
protected:
	HMENU m_hMenuShared;        // menu when we are active

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
					CWnd* pParentWnd, CCreateContext* pContext = NULL);
		// 'pParentWnd' parameter is required for MDI Child
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual void OnUpdateFrameMenu(BOOL bActive, CWnd* pActivateWnd,
		HMENU hMenuAlt);

	virtual BOOL IsTabbedMDIChild() { return FALSE; }

	BOOL m_bPseudoInactive;     // TRUE if window is MDI active according to
								//  windows, but not according to MFC...

protected:
	virtual CWnd* GetMessageBar();
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	BOOL UpdateClientEdge(LPRECT lpRect = NULL);

	afx_msg void OnMDIActivate(BOOL bActivate, CWnd*, CWnd*);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnDestroy();
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CMiniFrameWnd

// MiniFrame window styles
#define MFS_SYNCACTIVE      0x00000100L // syncronize activation w/ parent
#define MFS_4THICKFRAME     0x00000200L // thick frame all around (no tiles)
#define MFS_THICKFRAME      0x00000400L // use instead of WS_THICKFRAME
#define MFS_MOVEFRAME       0x00000800L // no sizing, just moving
#define MFS_BLOCKSYSMENU    0x00001000L // block hit testing on system menu

#pragma warning( push )

#pragma warning( disable: 4263 )
#pragma warning( disable: 4264 )
class CMiniFrameWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CMiniFrameWnd)

// Constructors
public:
	CMiniFrameWnd();
	virtual BOOL Create(LPCTSTR lpClassName, LPCTSTR lpWindowName,
		DWORD dwStyle, const RECT& rect,
		CWnd* pParentWnd = NULL, UINT nID = 0);
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName,
		DWORD dwStyle, const RECT& rect,
		CWnd* pParentWnd = NULL, UINT nID = 0);

// Implementation
public:
	~CMiniFrameWnd();

	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* pMMI);
	afx_msg LRESULT OnFloatStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQueryCenterWnd(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);

	DECLARE_MESSAGE_MAP()

public:
	static void PASCAL CalcBorders(LPRECT lpClientRect,
		DWORD dwStyle = WS_THICKFRAME | WS_CAPTION, DWORD dwExStyle = 0);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	BOOL m_bSysTracking;
	BOOL m_bInSys;
	BOOL m_bActive;
	CString m_strCaption;
};

#pragma warning( pop )

/*============================================================================*/
// class CView is the client area UI for a document

class CPrintDialog;     // forward reference (see afxdlgs.h)
class CPreviewView;     // forward reference (see afxpriv.h)
class CSplitterWnd;     // forward reference (see afxext.h)
class COleServerDoc;    // forward reference (see afxole.h)

typedef DWORD DROPEFFECT;
class COleDataObject;   // forward reference (see afxole.h)

class AFX_NOVTABLE CView : public CWnd
{
	friend class CWinAppEx;

	DECLARE_DYNAMIC(CView)

// Constructors
protected:
	CView();

// Attributes
public:
	CDocument* GetDocument() const;

// Operations
public:
	// for standard printing setup (override OnPreparePrinting)
	BOOL DoPreparePrinting(CPrintInfo* pInfo);

// Overridables
public:
	virtual BOOL IsSelected(const CObject* pDocItem) const; // support for OLE

	// OLE scrolling support (used for drag/drop as well)
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);

	// OLE drag/drop support
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDropEx(COleDataObject* pDataObject,
		DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point);
	virtual DROPEFFECT OnDragScroll(DWORD dwKeyState, CPoint point);

	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	virtual void OnInitialUpdate(); // called first time after construct

protected:
	// Activation
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);
	virtual void OnActivateFrame(UINT nState, CFrameWnd* pFrameWnd);

	// General drawing/updating
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC) = 0;

	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
		// must override to enable printing and print preview

	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Advanced: end print preview mode, move to point
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point,
		CPreviewView* pView);

// Implementation
public:
	virtual ~CView() = 0;
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

	// Advanced: for implementing custom print preview
	BOOL DoPrintPreview(UINT nIDResource, CView* pPrintView,
			CRuntimeClass* pPreviewViewClass, CPrintPreviewState* pState);

	virtual void CalcWindowRect(LPRECT lpClientRect,
		UINT nAdjustType = adjustBorder);
	virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
	static CSplitterWnd* PASCAL GetParentSplitter(
		const CWnd* pWnd, BOOL bAnyState);

protected:
	CDocument* m_pDocument;
	BOOL m_bInitialRedraw;

public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();

	// friend classes that call protected CView overridables
	friend class CDocument;
	friend class CDocTemplate;
	friend class CPreviewView;
	friend class CFrameWnd;
	friend class CMDIFrameWnd;
	friend class CMDIChildWnd;
	friend class CSplitterWnd;
	friend class COleServerDoc;
	friend class CDocObjectServer;

	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	// Print client support: used for interaction with Windows task bar
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	// commands
	afx_msg void OnUpdateSplitCmd(CCmdUI* pCmdUI);
	afx_msg BOOL OnSplitCmd(UINT nID);
	afx_msg void OnUpdateNextPaneMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnNextPaneCmd(UINT nID);

	// not mapped commands - must be mapped in derived class
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// class CCtrlView allows almost any control to be a view

class AFX_NOVTABLE CCtrlView : public CView
{
	DECLARE_DYNCREATE(CCtrlView)

public:
	CCtrlView(LPCTSTR lpszClass, DWORD dwStyle);

// Attributes
protected:
	CString m_strClass;
	DWORD m_dwDefaultStyle;

// Overrides
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	~CCtrlView() = 0;
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

protected:
	afx_msg void OnPaint();
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// class CScrollView supports simple scrolling and scaling

class _AFX_MOUSEANCHORWND;

class CScrollView : public CView
{
	DECLARE_DYNAMIC(CScrollView)

// Constructors
protected:
	CScrollView();

public:
	static AFX_DATA const SIZE sizeDefault;
		// used to specify default calculated page and line sizes

	// in logical units - call one of the following Set routines
	void SetScaleToFitSize(SIZE sizeTotal);
	void SetScrollSizes(int nMapMode, SIZE sizeTotal,
				const SIZE& sizePage = sizeDefault,
				const SIZE& sizeLine = sizeDefault);

// Attributes
public:
	CPoint GetScrollPosition() const;       // upper corner of scrolling
	CSize GetTotalSize() const;             // logical size

	void CheckScrollBars(BOOL& bHasHorzBar, BOOL& bHasVertBar) const;

	// for device units
	CPoint GetDeviceScrollPosition() const;
	void GetDeviceScrollSizes(int& nMapMode, SIZE& sizeTotal,
			SIZE& sizePage, SIZE& sizeLine) const;

// Operations
public:
	void ScrollToPosition(POINT pt);    // set upper left position
	void FillOutsideRect(CDC* pDC, CBrush* pBrush);
	void ResizeParentToFit(BOOL bShrinkOnly = TRUE);
	BOOL DoMouseWheel(UINT fFlags, short zDelta, CPoint point);

// Implementation
protected:
	_AFX_MOUSEANCHORWND* m_pAnchorWindow; // window for wheel mouse anchor
	friend class _AFX_MOUSEANCHORWND;
	int m_nMapMode;				 // mapping mode for window creation
	CSize m_totalLog;           // total size in logical units (no rounding)
	CSize m_totalDev;           // total size in device units
	CSize m_pageDev;            // per page scroll size in device units
	CSize m_lineDev;            // per line scroll size in device units

	BOOL m_bCenter;             // Center output if larger than total size
	BOOL m_bInsideUpdate;       // internal state for OnSize callback
	void CenterOnPoint(CPoint ptCenter);
	void ScrollToDevicePosition(POINT ptDev); // explicit scrolling no checking

protected:
	virtual void OnDraw(CDC* pDC) = 0;      // pass on pure virtual

	void UpdateBars();          // adjust scrollbars etc
	BOOL GetTrueClientSize(CSize& size, CSize& sizeSb);
		// size with no bars
	void GetScrollBarSizes(CSize& sizeSb);
	void GetScrollBarState(CSize sizeClient, CSize& needSb,
		CSize& sizeRange, CPoint& ptMove, BOOL bInsideClient);

public:
	virtual ~CScrollView();
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG
	virtual void CalcWindowRect(LPRECT lpClientRect,
		UINT nAdjustType = adjustBorder);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	virtual CSize GetWheelScrollDistance(CSize sizeDistance,
		BOOL bHorz, BOOL bVert);

	// scrolling implementation support for OLE
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	afx_msg LRESULT HandleMButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()
};

/*============================================================================*/
// CWinThread

typedef UINT (AFX_CDECL *AFX_THREADPROC)(LPVOID);

class COleMessageFilter;        // forward reference (see afxole.h)

BOOL AFXAPI AfxPumpMessage();
LRESULT AFXAPI AfxProcessWndProcException(CException*, const MSG* pMsg);
BOOL __cdecl AfxPreTranslateMessage(MSG* pMsg);
BOOL __cdecl AfxIsIdleMessage(MSG* pMsg);

class CWinThread : public CCmdTarget
{
	DECLARE_DYNAMIC(CWinThread)

	friend BOOL AfxInternalPreTranslateMessage(MSG* pMsg);

public:
// Constructors
	CWinThread();
	BOOL CreateThread(DWORD dwCreateFlags = 0, UINT nStackSize = 0,
		LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

// Attributes
	CWnd* m_pMainWnd;       // main window (usually same AfxGetApp()->m_pMainWnd)
	CWnd* m_pActiveWnd;     // active main window (may not be m_pMainWnd)
	BOOL m_bAutoDelete;     // enables 'delete this' after thread termination

	// only valid while running
	HANDLE m_hThread;       // this thread's HANDLE
	operator HANDLE() const;
	DWORD m_nThreadID;      // this thread's ID

	int GetThreadPriority();
	BOOL SetThreadPriority(int nPriority);

// Operations
	DWORD SuspendThread();
	DWORD ResumeThread();
	BOOL PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);

// Overridables
	// thread initialization
	virtual BOOL InitInstance();

	// running and idle processing
	virtual int Run();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PumpMessage();     // low level message pump
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
	virtual BOOL IsIdleMessage(MSG* pMsg);  // checks for special messages

	// thread termination
	virtual int ExitInstance(); // default will 'delete this'

	// Advanced: exception handling
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);

	// Advanced: handling messages sent to message filter hook
	virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);

	// Advanced: virtual access to m_pMainWnd
	virtual CWnd* GetMainWnd();

// Implementation
public:
	virtual ~CWinThread();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void CommonConstruct();
	virtual void Delete();
		// 'delete this' only if m_bAutoDelete == TRUE

public:
	// constructor used by implementation of AfxBeginThread
	CWinThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam);

	// valid after construction
	LPVOID m_pThreadParams; // generic parameters passed to starting function
	AFX_THREADPROC m_pfnThreadProc;

	// set after OLE is initialized
	void (AFXAPI* m_lpfnOleTermOrFreeLib)(BOOL, BOOL);
	COleMessageFilter* m_pMessageFilter;

protected:
	BOOL DispatchThreadMessageEx(MSG* msg);  // helper
	void DispatchThreadMessage(MSG* msg);  // obsolete
};

// global helpers for threads

CWinThread* AFXAPI AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam,
	int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
	DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
CWinThread* AFXAPI AfxBeginThread(CRuntimeClass* pThreadClass,
	int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,
	DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

CWinThread* AFXAPI AfxGetThread();
MSG* AFXAPI AfxGetCurrentMessage();

void AFXAPI AfxEndThread(UINT nExitCode, BOOL bDelete = TRUE);

void AFXAPI AfxInitThread();
void AFXAPI AfxTermThread(HINSTANCE hInstTerm = NULL);

/*============================================================================*/
// Global functions for access to the one and only CWinApp

#define afxCurrentWinApp    AfxGetModuleState()->m_pCurrentWinApp
#define afxCurrentInstanceHandle    AfxGetModuleState()->m_hCurrentInstanceHandle
#define afxCurrentResourceHandle    AfxGetModuleState()->m_hCurrentResourceHandle
#define afxCurrentAppName   AfxGetModuleState()->m_lpszCurrentAppName
#define afxContextIsDLL     AfxGetModuleState()->m_bDLL
#define afxRegisteredClasses    AfxGetModuleState()->m_fRegisteredClasses

#define afxOccManager   AfxGetModuleState()->m_pOccManager

// Advanced initialization: for overriding default WinMain
BOOL AFXAPI AfxWinInit(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_z_ LPTSTR lpCmdLine, _In_ int nCmdShow);
void AFXAPI AfxWinTerm();

// Global Windows state data helper functions (inlines)
#ifdef _AFXDLL
ULONG AFXAPI AfxGetDllVersion();
#endif

CWinApp* AFXAPI AfxGetApp();
CWnd* AFXAPI AfxGetMainWnd();
HINSTANCE AFXAPI AfxGetInstanceHandle();
HINSTANCE AFXAPI AfxGetResourceHandle();
void AFXAPI AfxSetResourceHandle(HINSTANCE hInstResource);
LPCTSTR AFXAPI AfxGetAppName();
AFX_DEPRECATED("AfxLoadLangResourceDLL(LPCTSTR pszFormat) has been deprecated, use AfxLoadLangResourceDLL(LPCTSTR pszFormat, LPCTSTR pszPath) instead")
	HINSTANCE AFXAPI AfxLoadLangResourceDLL(LPCTSTR pszFormat);
HINSTANCE AFXAPI AfxLoadLangResourceDLL(LPCTSTR pszFormat, LPCTSTR pszPath);

// Use instead of PostQuitMessage in OLE server applications
void AFXAPI AfxPostQuitMessage(int nExitCode);

// Use AfxFindResourceHandle to find resource in chain of extension DLLs
#ifndef _AFXDLL
#define AfxFindResourceHandle(lpszResource, lpszType) AfxGetResourceHandle()
#else
HINSTANCE AFXAPI AfxFindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType);
#endif

/// <summary>
/// Deletes the subkeys and values of the specified key recursively.</summary>
/// <returns>
/// If the function succeeds, the return value is ERROR_SUCCESS. If the function fails, the return value is a nonzero error code defined in Winerror.h</returns>
/// <param name="hKey">A handle to an open registry key.</param>
/// <param name="lpSubKey">The name of the key to be deleted.</param>
/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
LONG AFXAPI AfxDelRegTreeHelper(HKEY hParentKey, const CString& strKeyName, CAtlTransactionManager* pTM = NULL);

class CRecentFileList;          // forward reference (see afxadv.h)
class CDataRecoveryHandler;     // forward reference (see afxdatarecovery.h)

// access to message filter in CWinApp
COleMessageFilter* AFXAPI AfxOleGetMessageFilter();

/*============================================================================*/
// CCommandLineInfo

class CCommandLineInfo : public CObject
{
public:
	// Sets default values
	CCommandLineInfo();

	// plain char* version on UNICODE for source-code backwards compatibility
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
#ifdef _UNICODE
	virtual void ParseParam(const char* pszParam, BOOL bFlag, BOOL bLast);
#endif

	BOOL m_bShowSplash;
	BOOL m_bRunEmbedded;
	BOOL m_bRunAutomated;
	BOOL m_bRegisterPerUser;
	enum { FileNew, FileOpen, FilePrint, FilePrintTo, FileDDE, FileDDENoShow, AppRegister,
		AppUnregister, RestartByRestartManager, FileNothing = -1 } m_nShellCommand;

	// not valid for FileNew
	CString m_strFileName;

	// valid only for FilePrintTo
	CString m_strPrinterName;
	CString m_strDriverName;
	CString m_strPortName;

	// valid only for RestartByRestartManager
	CString m_strRestartIdentifier;

	~CCommandLineInfo();
// Implementation
protected:
	void ParseParamFlag(const char* pszParam);
	void ParseParamNotFlag(const TCHAR* pszParam);
#ifdef _UNICODE
	void ParseParamNotFlag(const char* pszParam);
#endif
	void ParseLast(BOOL bLast);
};

/*============================================================================*/
// CDocManager

class CDocManager : public CObject
{
	DECLARE_DYNAMIC(CDocManager)
public:

// Constructor
	CDocManager();

	//Document functions
	virtual void AddDocTemplate(CDocTemplate* pTemplate);
	virtual POSITION GetFirstDocTemplatePosition() const;
	virtual CDocTemplate* GetNextDocTemplate(POSITION& pos) const;
	virtual void RegisterShellFileTypes(BOOL bCompat);
	void UnregisterShellFileTypes();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName); // open named file
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName, BOOL bAddToMRU); // open named file
	virtual BOOL SaveAllModified(); // save before exit
	virtual void CloseAllDocuments(BOOL bEndSession); // close documents before exiting
	virtual int GetOpenDocumentCount();
	virtual CDocTemplate* GetBestTemplate(LPCTSTR lpszFileName); // return best template named file

	// helper for standard commdlg dialogs
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);

//Commands
	// Advanced: process async DDE request
	virtual BOOL OnDDECommand(_In_z_ LPTSTR lpszCommand);
	virtual void OnFileNew();
	virtual void OnFileOpen();

// Implementation
protected:
	CPtrList m_templateList;
	int GetDocumentCount(); // helper to count number of total documents

public:
	static CPtrList* pStaticList;       // for static CDocTemplate objects
	static BOOL bStaticInit;            // TRUE during static initialization
	static CDocManager* pStaticDocManager;  // for static CDocTemplate objects

public:
	virtual ~CDocManager();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/*============================================================================*/
// CWinApp - the root of all Windows applications

#define _AFX_MRU_COUNT   4      // default support for 4 entries in file MRU
#define _AFX_MRU_MAX_COUNT 16   // currently allocated id range supports 16

#define _AFX_SYSPOLICY_NOTINITIALIZED			0
#define _AFX_SYSPOLICY_NORUN					1
#define _AFX_SYSPOLICY_NODRIVES					2
#define _AFX_SYSPOLICY_RESTRICTRUN				4
#define _AFX_SYSPOLICY_NONETCONNECTDISCONNECTD	8
#define _AFX_SYSPOLICY_NOENTIRENETWORK			16
#define _AFX_SYSPOLICY_NORECENTDOCHISTORY		32
#define _AFX_SYSPOLICY_NOCLOSE					64
#define _AFX_SYSPOLICY_NOPLACESBAR				128
#define _AFX_SYSPOLICY_NOBACKBUTTON				256
#define _AFX_SYSPOLICY_NOFILEMRU				512

struct _AfxSysPolicyData
{
	LPCTSTR szPolicyName;
	DWORD dwID;
};

struct _AfxSysPolicies
{
	LPCTSTR szPolicyKey;
	_AfxSysPolicyData *pData;
};

// Restart Manager support flags
#define AFX_RESTART_MANAGER_SUPPORT_RESTART				0x01  // restart support, means application is registered via RegisterApplicationRestart
#define AFX_RESTART_MANAGER_SUPPORT_RECOVERY			0x02  // recovery support, means application is registered via RegisterApplicationRecoveryCallback
#define AFX_RESTART_MANAGER_AUTOSAVE_AT_RESTART			0x04  // auto-save support is enabled, documents will be autosaved at restart by restart manager
#define AFX_RESTART_MANAGER_AUTOSAVE_AT_INTERVAL		0x08  // auto-save support is enabled, documents will be autosaved periodically for crash recovery
#define AFX_RESTART_MANAGER_REOPEN_PREVIOUS_FILES		0x10  // reopen of previously opened documents is enabled, on restart all previous documents will be opened
#define AFX_RESTART_MANAGER_RESTORE_AUTOSAVED_FILES		0x20  // restoration of auto-saved documents is enabled, on restart user will be prompted to open auto-saved documents intead of last saved
#define AFX_RESTART_MANAGER_SUPPORT_NO_AUTOSAVE			AFX_RESTART_MANAGER_SUPPORT_RESTART | AFX_RESTART_MANAGER_SUPPORT_RECOVERY | AFX_RESTART_MANAGER_REOPEN_PREVIOUS_FILES
#define AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS			AFX_RESTART_MANAGER_SUPPORT_NO_AUTOSAVE | AFX_RESTART_MANAGER_AUTOSAVE_AT_RESTART | AFX_RESTART_MANAGER_AUTOSAVE_AT_INTERVAL | AFX_RESTART_MANAGER_RESTORE_AUTOSAVED_FILES
#define AFX_RESTART_MANAGER_SUPPORT_RESTART_ASPECTS		AFX_RESTART_MANAGER_SUPPORT_RESTART | AFX_RESTART_MANAGER_AUTOSAVE_AT_RESTART | AFX_RESTART_MANAGER_REOPEN_PREVIOUS_FILES | AFX_RESTART_MANAGER_RESTORE_AUTOSAVED_FILES
#define AFX_RESTART_MANAGER_SUPPORT_RECOVERY_ASPECTS	AFX_RESTART_MANAGER_SUPPORT_RECOVERY | AFX_RESTART_MANAGER_AUTOSAVE_AT_INTERVAL | AFX_RESTART_MANAGER_REOPEN_PREVIOUS_FILES | AFX_RESTART_MANAGER_RESTORE_AUTOSAVED_FILES

class CWinApp : public CWinThread
{
	DECLARE_DYNAMIC(CWinApp)
public:

// Constructor
	explicit CWinApp(LPCTSTR lpszAppName = NULL);     // app name defaults to EXE name

// Attributes
	// Startup args (do not change)

	// This module's hInstance.
	HINSTANCE m_hInstance;

	// Pointer to the command-line.
	LPTSTR m_lpCmdLine;

	// Initial state of the application's window; normally,
	// this is an argument to ShowWindow().
	int m_nCmdShow;

	// Running args (can be changed in InitInstance)

	// Human-redable name of the application. Normally set in
	// constructor or retreived from AFX_IDS_APP_TITLE.
	LPCTSTR m_pszAppName;

	/// <summary>
	/// Application User Model ID.</summary>
	LPCTSTR m_pszAppID;

	// Name of registry key for this application. See
	// SetRegistryKey() member function.
	LPCTSTR m_pszRegistryKey;

	// Pointer to CDocManager used to manage document templates
	// for this application instance.
	CDocManager* m_pDocManager;

	// Support for Shift+F1 help mode.

	// TRUE if we're in SHIFT+F1 mode.
	BOOL m_bHelpMode;

public:
	// set in constructor to override default

	// Executable name (no spaces).
	LPCTSTR m_pszExeName;

	// Default based on this module's path.
	LPCTSTR m_pszHelpFilePath;

	// Default based on this application's name.
	LPCTSTR m_pszProfileName;

	// Sets and initializes usage of HtmlHelp instead of WinHelp.
	void EnableHtmlHelp();

	// Sets and initializes usage of HtmlHelp instead of WinHelp.
	void SetHelpMode( AFX_HELP_TYPE eHelpType );
	AFX_HELP_TYPE GetHelpMode();

	// help mode used by the app
	AFX_HELP_TYPE m_eHelpType;

	// Returns TRUE is application is running under Windows 7 or higher
	BOOL IsWindows7();

// Initialization Operations - should be done in InitInstance
protected:
	// Load MRU file list and last preview state.
	void LoadStdProfileSettings(UINT nMaxMRU = _AFX_MRU_COUNT);

	void EnableShellOpen();

	// SetDialogBkColor is no longer supported.
	// To change dialog background and text color, handle WM_CTLCOLORDLG instead.
	AFX_DEPRECATED("CWinApp::SetDialogBkColor is no longer supported. Instead, handle WM_CTLCOLORDLG in your dialog")
			void SetDialogBkColor(COLORREF clrCtlBk = RGB(192, 192, 192), COLORREF clrCtlText = RGB(0, 0, 0));

	// Set registry key name to be used by CWinApp's
	// profile member functions; prevents writing to an INI file.
	void SetRegistryKey(LPCTSTR lpszRegistryKey);
	void SetRegistryKey(UINT nIDRegistryKey);

	/// <summary>
	/// Explicitly sets Application User Model ID for the application. This method should be called before any user interface
	/// is presented to user (the best place is the application constructor).</summary>
	/// <param name="lpcszAppID">Specifies the Application User Model ID.</param>
	void SetAppID(LPCTSTR lpcszAppID);

	// Enable3dControls and Enable3dControlsStatic are no longer necessary.
	AFX_DEPRECATED("CWinApp::Enable3dControls is no longer needed. You should remove this call.")
			BOOL Enable3dControls();
#ifndef _AFXDLL
	AFX_DEPRECATED("CWinApp::Enable3dControlsStatic is no longer needed. You should remove this call.")
			BOOL Enable3dControlsStatic();
#endif

	void RegisterShellFileTypes(BOOL bCompat = FALSE);

	// call after all doc templates are registered
	void UnregisterShellFileTypes();

public:
	// Loads a cursor resource.
	HCURSOR LoadCursor(LPCTSTR lpszResourceName) const;
	HCURSOR LoadCursor(UINT nIDResource) const;

	// Loads a stock cursor resource; for for IDC_* values.
	HCURSOR LoadStandardCursor(LPCTSTR lpszCursorName) const;

	// Loads an OEM cursor; for all OCR_* values.
	HCURSOR LoadOEMCursor(UINT nIDCursor) const;

	// Loads an icon resource.
	HICON LoadIcon(LPCTSTR lpszResourceName) const;
	HICON LoadIcon(UINT nIDResource) const;

	// Loads an icon resource; for stock IDI_ values.
	HICON LoadStandardIcon(LPCTSTR lpszIconName) const;

	// Loads an OEM icon resource; for all OIC_* values.
	HICON LoadOEMIcon(UINT nIDIcon) const;

	// Retrieve an integer value from INI file or registry.
	virtual UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);

	// Sets an integer value to INI file or registry.
	virtual BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue);

	// Retrieve a string value from INI file or registry.
	virtual CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);

	// Sets a string value to INI file or registry.
	virtual BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue);

	// Retrieve an arbitrary binary value from INI file or registry.
	virtual BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);

	// Sets an arbitrary binary value to INI file or registry.
	virtual BOOL WriteProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);

	// Override in derived class.
	virtual void InitLibId();

	// Register
	virtual BOOL Register();

	// Unregisters everything this app was known to register.
	virtual BOOL Unregister();

	/// <summary>
	/// Deletes the subkeys and values of the specified key recursively.</summary>
	/// <returns>
	/// If the function succeeds, the return value is ERROR_SUCCESS. If the function fails, the return value is a nonzero error code defined in Winerror.h</returns>
	/// <param name="hParentKey">A handle to an open registry key.</param>
	/// <param name="strKeyName">The name of the key to be deleted.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	LONG DelRegTree(HKEY hParentKey, const CString& strKeyName, CAtlTransactionManager* pTM = NULL);

// Running Operations - to be done on a running application
	// Dealing with document templates
	void AddDocTemplate(CDocTemplate* pTemplate);
	POSITION GetFirstDocTemplatePosition() const;
	CDocTemplate* GetNextDocTemplate(POSITION& pos) const;

	// Open named file, trying to match a regsitered
	// document template to it.
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName, BOOL bAddToMRU);

	// Add a string to the recent file list. Remove oldest string,
	// if no space left.
	virtual void AddToRecentFileList(LPCTSTR lpszPathName);

	// Printer DC Setup routine, 'struct tagPD' is a PRINTDLG structure.
	void SelectPrinter(HANDLE hDevNames, HANDLE hDevMode,
		BOOL bFreeOld = TRUE);

	// Create a DC for the system default printer.
	BOOL CreatePrinterDC(CDC& dc);


	BOOL GetPrinterDeviceDefaults(PRINTDLG* pPrintDlg);

	// Run this app as an embedded object.
	BOOL RunEmbedded();

	// Run this app as an OLE automation server.
	BOOL RunAutomated();

	// Parse the command line for stock options and commands.
	void ParseCommandLine(CCommandLineInfo& rCmdInfo);

	// React to a shell-issued command line directive.
	BOOL ProcessShellCommand(CCommandLineInfo& rCmdInfo);

// Overridables

	// Hooks for your initialization code
	virtual BOOL InitApplication();

	// exiting
	virtual BOOL SaveAllModified(); // save before exit
	void HideApplication();
	void CloseAllDocuments(BOOL bEndSession); // close documents before exiting

	// Advanced: to override message boxes and other hooks
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	virtual void DoWaitCursor(int nCode); // 0 => restore, 1=> begin, -1=> end

	// Advanced: process async DDE request
	virtual BOOL OnDDECommand(_In_z_ LPTSTR lpszCommand);

	// Advanced: Help support
	virtual void WinHelp(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);

	// Restart Manager support
	virtual HRESULT RegisterWithRestartManager(BOOL bRegisterRecoveryCallback, const CString &strRestartIdentifier);
	virtual HRESULT RegisterWithRestartManager(LPCWSTR pwzCommandLineArgs, DWORD dwRestartFlags, APPLICATION_RECOVERY_CALLBACK pRecoveryCallback, LPVOID lpvParam, DWORD dwPingInterval, DWORD dwCallbackFlags);
	virtual DWORD ApplicationRecoveryCallback(LPVOID lpvParam);

	virtual BOOL SupportsRestartManager() const { return m_dwRestartManagerSupportFlags & AFX_RESTART_MANAGER_SUPPORT_RESTART; }
	virtual BOOL SupportsApplicationRecovery() const { return m_dwRestartManagerSupportFlags & AFX_RESTART_MANAGER_SUPPORT_RECOVERY; }
	virtual BOOL SupportsAutosaveAtRestart() const { return m_dwRestartManagerSupportFlags & AFX_RESTART_MANAGER_AUTOSAVE_AT_RESTART; }
	virtual BOOL SupportsAutosaveAtInterval() const { return m_dwRestartManagerSupportFlags & AFX_RESTART_MANAGER_AUTOSAVE_AT_INTERVAL; }
	virtual BOOL ReopenPreviousFilesAtRestart() const { return m_dwRestartManagerSupportFlags & AFX_RESTART_MANAGER_REOPEN_PREVIOUS_FILES; }
	virtual BOOL RestoreAutosavedFilesAtRestart() const { return m_dwRestartManagerSupportFlags & AFX_RESTART_MANAGER_RESTORE_AUTOSAVED_FILES; }

	virtual DWORD GetApplicationRestartFlags() { return 0; }  // Flags for RegisterApplicationRestart: default is none of RESTART_NO_CRASH/RESTART_NO_HANG/RESTART_NO_PATCH/RESTART_NO_REBOOT
	virtual LPVOID GetApplicationRecoveryParameter() { return NULL; }  // Parameter to be passed along to RegisterApplicationRecoveryCallback
	virtual DWORD GetApplicationRecoveryPingInterval() { return RECOVERY_DEFAULT_PING_INTERVAL; }  // Ping interval for RegisterApplicationRecoveryCallback

// Command Handlers
protected:
	// map to the following for file new/open
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();

	// map to the following to enable print setup
	afx_msg void OnFilePrintSetup();

	// map to the following to enable help
	afx_msg void OnContextHelp();   // shift-F1
	afx_msg void OnHelp();          // F1 (uses current context)
	afx_msg void OnHelpIndex();     // ID_HELP_INDEX
	afx_msg void OnHelpFinder();    // ID_HELP_FINDER, ID_DEFAULT_HELP
	afx_msg void OnHelpUsing();     // ID_HELP_USING

// Implementation
protected:
	HGLOBAL m_hDevMode;             // printer Dev Mode
	HGLOBAL m_hDevNames;            // printer Device Names
	DWORD m_dwPromptContext;        // help context override for message box

	HINSTANCE m_hLangResourceDLL;  // Satellite resource DLL

	int m_nWaitCursorCount;         // for wait cursor (>0 => waiting)
	HCURSOR m_hcurWaitCursorRestore; // old cursor to restore after wait cursor

	CRecentFileList* m_pRecentFileList;
	CDataRecoveryHandler* m_pDataRecoveryHandler;

	void UpdatePrinterSelection(BOOL bForceDefaults);
	void SaveStdProfileSettings();  // save options to .INI file

public: // public for implementation access
	CCommandLineInfo* m_pCmdInfo;

	ATOM m_atomApp, m_atomSystemTopic;   // for DDE open
	UINT m_nNumPreviewPages;        // number of default printed pages

	size_t  m_nSafetyPoolSize;      // ideal size

	void (AFXAPI* m_lpfnDaoTerm)();

	void DevModeChange(_In_z_ LPTSTR lpDeviceName);
	void SetCurrentHandles();

	// Finds number of opened CDocument items owned by templates
	// registered with the doc manager.
	int GetOpenDocumentCount();

	virtual CDataRecoveryHandler *GetDataRecoveryHandler();
	/// <summary>
	/// Tells whether Windows 7 Taskbar interaction is enabled.</summary>
	/// <returns>
	/// Returns TRUE if EnableTaskbarInteraction has been called and Operation System is
	/// Windows 7 or higher.</returns>
	/// <remarks> Taskbar interaction means that MDI application displays the content of MDI children
	/// in separate tabbed thumbnails that appear when mouse pointer is over application taskbar button.</remarks>
	virtual BOOL IsTaskbarInteractionEnabled();

	/// <summary>
	/// Enables Taskbar interaction</summary>
	/// <returns>
	/// Returns TRUE if taskbar interaction can be enabled or disabled (e.g. this method was called before creation of main window).</returns>
	/// <remarks>
	/// This method must be called before creation of main window, otherwise it asserts and returns FALSE.</remarks>
	/// <param name="bEnable"> Specifies whether interaction with Windows 7 taskbar should be enabled (TRUE), or disabled (FALSE).</param>
	BOOL EnableTaskbarInteraction(BOOL bEnable = TRUE);

#if (NTDDI_VERSION >= NTDDI_WIN7)
	/// <summary>
	/// Creates and stores in the global data a pointer to ITaskBarList interface.</summary>
	/// <returns>A pointer to ITaskbarList interface if creation of a task bar list object succeeds, or NULL if creation fails or current
	/// Operation System is less than Windows 7.</returns>
	ITaskbarList  *GetITaskbarList();

	/// <summary>
	/// Creates and stores in the global data a pointer to ITaskBarList3 interface.</summary>
	/// <returns>A pointer to ITaskbarList3 interface if creation creation of a task bar list object succeeds, or NULL if creation fails or current
	/// Operation System is less than Windows 7.</returns>
	ITaskbarList3 *GetITaskbarList3();

	/// <summary>
	/// Releases interfaces obtained through GetITaskbarList and GetITaskbarList3 methods.</summary>
	void ReleaseTaskBarRefs();
#endif

	// helpers for standard commdlg dialogs
	BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
	INT_PTR DoPrintDialog(CPrintDialog* pPD);

	void EnableModeless(BOOL bEnable); // to disable OLE in-place dialogs

	// overrides for implementation
	virtual BOOL InitInstance();
	virtual int ExitInstance(); // return app exit code
	virtual BOOL RestartInstance(); // handle restart by Restart Manager
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);
	virtual HINSTANCE LoadAppLangResourceDLL();

    // Helper for message boxes; can work when no CWinApp can be found
	static int ShowAppMessageBox(CWinApp *pApp, LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	static void DoEnableModeless(BOOL bEnable); // to disable OLE in-place dialogs

public:
	virtual ~CWinApp();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// helpers for registration

	/// <summary>
	/// returns key for HKEY_CURRENT_USER\"Software"\RegistryKey\AppName\lpszSection.</summary>
	/// <returns>
	/// Section key if the function succeeds, NULL - otherwise</returns>
	/// <param name="lpszSection">The name of the key to be obtained.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	HKEY GetSectionKey(LPCTSTR lpszSection, CAtlTransactionManager* pTM = NULL);

	/// <summary>
	/// returns key for HKEY_CURRENT_USER\"Software"\RegistryKey\ProfileName.</summary>
	/// <returns>
	/// Applixation key if the function succeeds, NULL - otherwise</returns>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	HKEY GetAppRegistryKey(CAtlTransactionManager* pTM = NULL);

	/// <summary>
	/// Enable application D2D support. This method should be called prior to initialization of the main window .</summary>
	/// <returns>
	/// Returns TRUE if D2D support was enabled, FALSE - otherwise</returns>
	/// <param name="d2dFactoryType">The threading model of the D2D factory and the resources it creates.</param>
	/// <param name="writeFactoryType">A value that specifies whether the write factory object will be shared or isolated</param>
	BOOL EnableD2DSupport(D2D1_FACTORY_TYPE d2dFactoryType = D2D1_FACTORY_TYPE_SINGLE_THREADED, DWRITE_FACTORY_TYPE writeFactoryType = DWRITE_FACTORY_TYPE_SHARED);

protected:
	afx_msg void OnAppExit();
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnOpenRecentFile(UINT nID);

	DECLARE_MESSAGE_MAP()
public :
	// System Policy Settings
	virtual BOOL LoadSysPolicies(); // Override to load policies other than the system policies that MFC loads.
	BOOL GetSysPolicyValue(DWORD dwPolicyID, BOOL *pbValue); // returns the policy's setting in the out parameter
protected :
	BOOL _LoadSysPolicies() throw(); // Implementation helper
	DWORD m_dwPolicies;				// block for storing boolean system policies

	// Restart Manager support
	DWORD m_dwRestartManagerSupportFlags; // What aspects of restart/recovery does the application support?
	int   m_nAutosaveInterval;            // How frequently are documents autosaved? (value in milliseconds)

	BOOL m_bTaskbarInteractionEnabled;
	BOOL m_bIsWindows7;

	BOOL m_bComInitialized;
	BOOL m_bTaskBarInterfacesAvailable;
	IUnknown* m_pTaskbarList;
	IUnknown* m_pTaskbarList3;
};

/*============================================================================*/
// class CWaitCursor

class CWaitCursor
{
// Construction/Destruction
public:
	CWaitCursor();
	~CWaitCursor();

// Operations
public:
	void Restore();
};

/*============================================================================*/
// class CDocTemplate creates documents

class AFX_NOVTABLE CDocTemplate : public CCmdTarget
{
	DECLARE_DYNAMIC(CDocTemplate)

// Constructors
protected:
	CDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

public:
	virtual void LoadTemplate();

// Attributes
public:
	// setup for OLE containers
	void SetContainerInfo(UINT nIDOleInPlaceContainer);

	// setup for OLE servers
	void SetServerInfo(UINT nIDOleEmbedding, UINT nIDOleInPlaceServer = 0,
		CRuntimeClass* pOleFrameClass = NULL, CRuntimeClass* pOleViewClass = NULL);

	/// <summary>
	/// Setups out of process preview handler. </summary>
	/// <param name="nIDPreviewFrame">Specifies a resource ID of preview frame.</param>
	/// <param name="pPreviewFrameClass">Specifies a pointer to a runtime class information of preview frame.</param>
	/// <param name="pPreviewViewClass">Specifies a pointer to a runtime class information of preview view.</param>
	void SetPreviewInfo(UINT nIDPreviewFrame, CRuntimeClass* pPreviewFrameClass = NULL, CRuntimeClass* pPreviewViewClass = NULL);

	// iterating over open documents
	virtual POSITION GetFirstDocPosition() const = 0;
	virtual CDocument* GetNextDoc(POSITION& rPos) const = 0;

// Operations
public:
	virtual void AddDocument(CDocument* pDoc);      // must override
	virtual void RemoveDocument(CDocument* pDoc);   // must override

	enum DocStringIndex
	{
		windowTitle,        // default window title
		docName,            // user visible name for default document
		fileNewName,        // user visible name for FileNew
		// for file based documents:
		filterName,         // user visible name for FileOpen
		filterExt,          // user visible extension for FileOpen
		// for file based documents with Shell open support:
		regFileTypeId,      // REGEDIT visible registered file type identifier
		regFileTypeName,    // Shell visible registered file type name
	};
	virtual BOOL GetDocString(CString& rString,
		enum DocStringIndex index) const; // get one of the info strings
	CFrameWnd* CreateOleFrame(CWnd* pParentWnd, CDocument* pDoc,
		BOOL bCreateView);
	/// <summary>
	/// Creates a child frame used for Rich Preview.</summary>
	/// <returns> A valid pointer to a CFrameWnd object, or NULL if creation fails. </returns>
	/// <param name="pParentWnd"> A pointer to a parent window (usually provided by the Shell).</param>
	/// <param name="pDoc"> A pointer to a document object, whose content will be previewed. </param>
	CFrameWnd* CreatePreviewFrame(CWnd* pParentWnd, CDocument* pDoc);

// Overridables
public:
	enum Confidence
	{
		noAttempt,
		maybeAttemptForeign,
		maybeAttemptNative,
		yesAttemptForeign,
		yesAttemptNative,
		yesAlreadyOpen
	};
	virtual Confidence MatchDocType(LPCTSTR lpszPathName,
					CDocument*& rpDocMatch);
	virtual CDocument* CreateNewDocument();
	virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
		BOOL bMakeVisible = TRUE);
	virtual BOOL SaveAllModified();     // for all documents
	virtual void CloseAllDocuments(BOOL bEndSession);
	// open named file; if lpszPathName == NULL => create new file with this type
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE) = 0;
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible) = 0;
	virtual void SetDefaultTitle(CDocument* pDocument) = 0;

// Implementation
public:
	BOOL m_bAutoDelete;
	virtual ~CDocTemplate() = 0;

	// back pointer to OLE or other server (NULL if none or disabled)
	CObject* m_pAttachedFactory;

	// Class ID for preview handler - used for registration
	CString m_strCLSID;
	CLSID   m_clsid;

	// menu & accelerator resources for in-place container
	HMENU m_hMenuInPlace;
	HACCEL m_hAccelInPlace;

	// menu & accelerator resource for server editing embedding
	HMENU m_hMenuEmbedding;
	HACCEL m_hAccelEmbedding;

	// menu & accelerator resource for server editing in-place
	HMENU m_hMenuInPlaceServer;
	HACCEL m_hAccelInPlaceServer;

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif
	virtual void OnIdle();             // for all documents
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	UINT m_nIDResource;						// IDR_ for frame/menu/accel as well
	UINT m_nIDServerResource;				// IDR_ for OLE inplace frame/menu/accel
	UINT m_nIDEmbeddingResource;			// IDR_ for OLE open frame/menu/accel
	UINT m_nIDContainerResource;			// IDR_ for container frame/menu/accel
	UINT m_nIDPreviewResource;				// IDR_ for preview frame. Do not load menu/accel

	CRuntimeClass* m_pDocClass;				// class for creating new documents
	CRuntimeClass* m_pFrameClass;			// class for creating new frames
	CRuntimeClass* m_pViewClass;			// class for creating new views
	CRuntimeClass* m_pOleFrameClass;		// class for creating in-place frame
	CRuntimeClass* m_pOleViewClass;			// class for creating in-place view
	CRuntimeClass* m_pPreviewFrameClass;	// class for creating in-place preview frame
	CRuntimeClass* m_pPreviewViewClass;		// class for creating in-place preview view

	CString m_strDocStrings;    // '\n' separated names
		// The document names sub-strings are represented as _one_ string:
		// windowTitle\ndocName\n ... (see DocStringIndex enum)

	// need for preview handler. CDocument finds template by m_pDocClass in order to instantiate related frame/view
	friend class CDocument;
};

// SDI support (1 document only)
class CSingleDocTemplate : public CDocTemplate
{
	DECLARE_DYNAMIC(CSingleDocTemplate)

// Constructors
public:
	CSingleDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

// Implementation
public:
	virtual ~CSingleDocTemplate();
	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
	virtual POSITION GetFirstDocPosition() const;
	virtual CDocument* GetNextDoc(POSITION& rPos) const;
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible);
	virtual void SetDefaultTitle(CDocument* pDocument);

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

protected:  // standard implementation
	CDocument* m_pOnlyDoc;
};

// MDI support (zero or more documents)
class CMultiDocTemplate : public CDocTemplate
{
	DECLARE_DYNAMIC(CMultiDocTemplate)

// Constructors
public:
	CMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

// Implementation
public:
	// Menu and accel table for MDI Child windows of this type
	HMENU m_hMenuShared;
	HACCEL m_hAccelTable;

	virtual ~CMultiDocTemplate();
	virtual void LoadTemplate();
	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
	virtual POSITION GetFirstDocPosition() const;
	virtual CDocument* GetNextDoc(POSITION& rPos) const;
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible);
	virtual void SetDefaultTitle(CDocument* pDocument);

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG

protected:  // standard implementation
	CPtrList m_docList;          // open documents of this type
	UINT m_nUntitledCount;   // start at 0, for "Document1" title
};

/*============================================================================*/
// class CDocument is the main document data abstraction

class AFX_NOVTABLE CDocument : public CCmdTarget
{
	DECLARE_DYNAMIC(CDocument)

public:
// Constructors
	CDocument();

	/// <summary>
	/// This class implements IDocument interface required for Search and Organize handlers.</summary>
	/// <remarks>
	/// Search and Organize handlers are implemented in ATL DLLs, which can be MFC or not-MFC based.
	/// Internally handlers refer to IDocument interface, whose implementation in the common case should be
	/// supplied by a developer. CDocumentAdapter provides this implementation for MFC and basically calls
	///	the appropriate methods of the parent CDocument.</remarks>
	class CDocumentAdapter : public ATL::IDocument
	{
		friend class CDocument;
	public:
		/// <summary>
		/// Constructs a CDocumentAdapter object.</summary>
		/// <param name="pParentDoc"> A pointer to a related document.</param>
		CDocumentAdapter(CDocument* pParentDoc) : m_pParentDoc(pParentDoc)
		{

		}
		/// <summary>
		/// A destructor. Also deletes a related (parent) CDocument.</summary>
		virtual ~CDocumentAdapter()
		{
			if (m_pParentDoc != NULL)
			{
				delete m_pParentDoc;
				m_pParentDoc = NULL;
			}
		}

		/// <summary>
		/// Loads document data from a stream </summary>
		/// <returns>
		/// S_OK if succeeds; otherwise failed HRESULT.</returns>
		/// <param name="pStream">A pointer to a stream. This stream is supplied by the Shell.</param>
		/// <param name="grfMode">Access mode to the stream.</param>
		virtual HRESULT LoadFromStream(_Inout_ IStream* pStream, _In_ DWORD grfMode)
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::LoadFromStream is called, but its CDocument is NULL.");
				return E_POINTER;
			}
			m_pParentDoc->m_bPreviewHandlerMode = TRUE;
			m_pParentDoc->m_bEmbedded = TRUE;
			m_pParentDoc->OnNewDocument();
			return m_pParentDoc->LoadDocumentFromStream(pStream, grfMode);
		}

		/// <summary>
		/// Initializes search content. </summary>
		virtual void InitializeSearchContent()
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::InitializeSearchContent is called, but its CDocument is NULL.");
				return;
			}

			m_pParentDoc->InitializeSearchContent();
		}
		/// <summary>
		/// Clears the chunk list. </summary>
		virtual void ClearChunkList()
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::ClearChunkList is called, but its CDocument is NULL.");
				return;
			}
			m_pParentDoc->ClearChunkList ();
		}
		/// <summary>
		/// Sets a chunk value. </summary>
		/// <returns>
		/// Nonzero if succeeds. Otherwise 0.</returns>
		/// <param name="pValue"> Specifies a chunk value to set.</param>
		virtual BOOL SetChunkValue(_In_ ATL::IFilterChunkValue* pValue)
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::SetChunkValue is called, but its CDocument is NULL.");
				return FALSE;
			}
			return m_pParentDoc->SetChunkValue(pValue);
		}

		/// <summary>
		/// Initializes chunk reading. </summary>
		virtual void BeginReadChunks()
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::BeginReadChunks is called, but its CDocument is NULL.");
				return;
			}
			m_pParentDoc->BeginReadChunks();
		}

		/// <summary>
		/// Reads next chunk value. </summary>
		/// <returns>
		/// Nonzero if succeeds. Otherwise 0.</returns>
		/// <param name="ppValue"> Output parameter. When the function returns ppValue contains the value.</param>
		virtual _Success_(return != FALSE) BOOL ReadNextChunkValue(_Outptr_result_maybenull_ ATL::IFilterChunkValue** ppValue)
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::ReadNextChunkValue is called, but its CDocument is NULL.");
				return FALSE;
			}
			return m_pParentDoc->ReadNextChunkValue(ppValue);
		}

		/// <summary>
		/// Removes a chunk with specified GUID </summary>
		/// <param name="guid> Specifies GUID of a chunk to be removed.</param>
		/// <param name="pid> Specifies PID of a chunk to be removed.</param>
		virtual void RemoveChunk(_In_ REFCLSID guid, _In_ DWORD pid)
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::RemoveChunk is called, but its CDocument is NULL.");
				return;
			}
			m_pParentDoc->RemoveChunk(guid, pid);
		}

		/// <summary>
		/// Looks for a chunk with specified GUID </summary>
		/// <returns>
		/// Position in the internal chunk list if succeeds. Otherwise NULL.</returns>
		/// <param name="guid> Specifies GUID of a chunk to find.</param>
		/// <param name="pid> Specifies PID of a chunk to find.</param>
		virtual POSITION FindChunk(_In_ REFCLSID guid, _In_ DWORD pid)
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::FindChunk is called, but its CDocument is NULL.");
				return NULL;
			}
			return m_pParentDoc->FindChunk(guid, pid);
		}

		/// <summary>
		/// Returns a pointer to a parent CDocument.</summary>
		/// <returns> Returns a pointer to a parent (container) CDocument.</returns>
		virtual LPVOID GetContainer() const
		{
			return m_pParentDoc;
		}

		/// <summary>
		/// Creates a bitmap for Thumbnail provider.</summary>
		/// <returns>Returns TRUE if a bitmap for thumbnail was created successfully; otherwise FALSE.</returns>
		/// <param name="cx">Specifies width and height of the bitmap.</param>
		/// <param name="phbmp">Should contain a handle to a bitmap, when the function returns. </param>
		/// <param name="pdwAlpha">Should contain a DWORD specifying alpha channel value, when the function returns.</param>
		virtual _Success_(return != FALSE) BOOL GetThumbnail(_In_ UINT cx, _Out_ HBITMAP* phbmp, _Out_ WTS_ALPHATYPE* pdwAlpha)
		{
			ASSERT_VALID(m_pParentDoc);
			if (m_pParentDoc == NULL)
			{
				TRACE0("CDocumentAdapter::GetThumbnail is called, but its CDocument is NULL.");
				return FALSE;
			}
			return m_pParentDoc->GetThumbnail(cx, phbmp, pdwAlpha);
		}

	protected:
		/// <summary>
		/// A pointer to a parent document (container).</summary>
		CDocument* m_pParentDoc;
	};

	friend class CDocumentAdapter;
public:
	/// <summary>
	/// Returns a pointer to object implementing IDocument interface. </summary>
	/// <returns>
	/// A pointer to object implementing IDocument interface. </returns>
	virtual ATL::IDocument* GetAdapter()
	{
		if (m_pDocumentAdapter == NULL)
		{
			ATLTRY(m_pDocumentAdapter = new CDocumentAdapter(this));
		}

		if (m_pDocumentAdapter != NULL)
		{
			m_pDocumentAdapter->AddRef();
		}

		return m_pDocumentAdapter;
	}

// Attributes
public:
	const CString& GetTitle() const;
	virtual void SetTitle(LPCTSTR lpszTitle);
	const CString& GetPathName() const;
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void ClearPathName();

	CDocTemplate* GetDocTemplate() const;
	virtual BOOL IsModified();
	virtual void SetModifiedFlag(BOOL bModified = TRUE);

	/// <summary>
	/// Tells whether this instance of CDocument object was created for Search & Organize handler.</summary>
	/// <returns>
	/// Returns TRUE if this instance of CDocument was created for Search & Organize handler.</returns>
	/// <remarks>
	/// Currently this function returns TRUE only for Rich Preview handler implemented in out of process server.
	/// You can set the appropriate flags (m_bPreviewHandlerMode, m_bSearchMode, m_bGetThumbnailMode) at your application level
	/// to make this function returning TRUE.</remarks>
	BOOL IsSearchAndOrganizeHandler() const;

// Operations
	void AddView(CView* pView);
	void RemoveView(CView* pView);
	virtual POSITION GetFirstViewPosition() const;
	virtual CView* GetNextView(POSITION& rPosition) const;

	// Update Views (simple update - DAG only)
	void UpdateAllViews(CView* pSender, LPARAM lHint = 0L,
		CObject* pHint = NULL);

// Overridables
	// Special notifications
	virtual void OnChangedViewList(); // after Add or Remove view
	virtual void DeleteContents(); // delete doc items etc

	// File helpers
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void ReportSaveLoadException(LPCTSTR lpszPathName,
				CException* e, BOOL bSaving, UINT nIDPDefault);
	virtual CFile* GetFile(LPCTSTR lpszFileName, UINT nOpenFlags,
		CFileException* pError);
	virtual void ReleaseFile(CFile* pFile, BOOL bAbort);

	// search/organize/preview/thumbnail notifications
	/// <summary>
	/// Called before Rich Preview font is changed. </summary>
	virtual void OnBeforeRichPreviewFontChanged(){}
	/// <summary>
	/// Called when Rich Preview font has changed. </summary>
	virtual void OnRichPreviewFontChanged(){}
	/// <summary>
	/// Called when Rich Preview text color has changed. </summary>
	virtual void OnRichPreviewTextColorChanged(){}
	/// <summary>
	/// Called when Rich Preview background color has changed. </summary>
	virtual void OnRichPreviewBackColorChanged(){}
	/// <summary>
	/// Called when Rich Preview site has changed. </summary>
	virtual void OnRichPreviewSiteChanged(){}
	/// <summary>
	/// Called when Rich Preview is unloaded.</summary>
	/// <remarks>
	/// OnUnloadHandler allows to take some additional on handler unload.
	/// Overriding this method allows complete control on clean up process if you don't call the
	/// base implementation, which releases preview handler site, destroys preview frame and
	/// releases the stream.</remarks>
	virtual void OnRichPreviewUnload();

	// search/organize/preview/thumbnail helpers

	/// <summary>
	/// Called to load document data from stream. </summary>
	/// <returns>S_OK if the load operation succeeds, otherwise HRESULT with error code.</returns>
	/// <remarks>
	/// You can override this method in a derived class to customize loading data from the stream. </remarks>
	/// <param name="pStream">A pointer to a stream. This stream is supplied by the Shell.</param>
	/// <param name="grfMode">Access mode to the stream.</param>
	virtual HRESULT LoadDocumentFromStream(IStream* pStream, DWORD dwGrfMode);

	/// <summary>
	/// Called to create a bitmap to be used by thumbnail provider to display thumbnail. </summary>
	/// <returns>Returns TRUE if a bitmap for thumbnail was created successfully; otherwise FALSE.</returns>
	/// <param name="cx">Specifies width and height of the bitmap.</param>
	/// <param name="phbmp">Should contain a handle to a bitmap, when the function returns. </param>
	/// <param name="pdwAlpha">Should contain a DWORD specifying alpha channel value, when the function returns.</param>
	virtual BOOL GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha);

	// search/organize/preview/thumbnail support - search and chunk management
	/// <summary>
	/// Called to initialize search content for Search Handler. </summary>
	/// <remarks>
	/// You should override this method in a derived class to initialize search content.
	/// The content should be a string with parts delimited by ';'. For example, "point; rectangle; ole item". </remarks>
	virtual void InitializeSearchContent ()
	{
		TRACE0("Override this method in a derived class to initialize chunk list for search filter.");
	}

	/// <summary>
	/// Clears the chunk list. </summary>
	virtual void ClearChunkList ();

	/// <summary>
	/// Sets a chunk value. </summary>
	/// <returns>
	/// Nonzero if succeeds. Otherwise 0.</returns>
	/// <param name="pValue"> Specifies a chunk value to set.</param>
	virtual BOOL SetChunkValue (ATL::IFilterChunkValue* pValue);

	/// <summary>
	/// Initializes chunk reading. </summary>
	virtual void BeginReadChunks ();

	/// <summary>
	/// Reads next chunk value. </summary>
	/// <returns>
	/// Nonzero if succeeds. Otherwise 0.</returns>
	/// <param name="ppValue"> Output parameter. When the function returns ppValue contains the value.</param>
	virtual BOOL ReadNextChunkValue(ATL::IFilterChunkValue** ppValue);

	/// <summary>
	/// Removes a chunk with specified GUID </summary>
	/// <param name="guid"> Specifies GUID of a chunk to be removed.</param>
	/// <param name="pid"> Specifies PID of a chunk to be removed.</param>
	virtual void RemoveChunk(REFCLSID guid, DWORD pid);

	/// <summary>
	/// Looks for a chunk with specified GUID </summary>
	/// <returns>
	/// Position in the internal chunk list if succeeds. Otherwise NULL.</returns>
	/// <param name="guid"> Specifies GUID of a chunk to find.</param>
	/// <param name="pid"> Specifies PID of a chunk to find.</param>
	virtual POSITION FindChunk(REFCLSID guid, DWORD pid);

	// Document event notifications
	enum DocumentEvent
	{
		onAfterNewDocument = 0,
		onAfterOpenDocument = 1,
		onAfterSaveDocument = 2,
		onAfterCloseDocument = 3
	};

	virtual void OnDocumentEvent(DocumentEvent deEvent);

	// advanced overridables, closing down frame/doc, etc.
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL SaveModified(); // return TRUE if ok to continue
	virtual void PreCloseFrame(CFrameWnd* pFrame);

// Implementation
protected:
	// default implementation
	CString m_strTitle;
	CString m_strPathName;
	CDocTemplate* m_pDocTemplate;
	CPtrList m_viewList;                // list of views
	BOOL m_bModified;                   // changed since last saved
	BOOL m_bFinalRelease;               // we're in CDocument's FinalRelease
	BOOL m_bOLELocked;                  // AfxOleLockApp has been called

	// search/organize/preview/thumbnail data
	HWND m_hWndHost; // a handle to window (host) supplied by prevhost.exe
	CRect m_rectHost; // window rectangle of host. Our preview frame should fit this rectangle
	IStream* m_pStream; // stream to load the document data from
	DWORD m_grfMode; // stream opening mode
	CFrameWnd* m_pPreviewFrame; // frame to display the rich preview

	ATL::CComPtr<IUnknown> m_spUnkSite; // SetSite called through IObjectWithSite

	// search support
	CPtrList m_lstChunks;
	POSITION m_posReadChunk;

	/// <summary>
	/// A pointer to embedded document adapter - an object implementing IDocument interface.</summary>
	CDocumentAdapter* m_pDocumentAdapter;

public:
	BOOL m_bAutoDelete;         // TRUE => delete document when no more views
	BOOL m_bEmbedded;           // TRUE => document is being created by OLE
	/// <summary>
	/// Specifies that CDocument object was created by dllhost for thumbnails. Should be checked in CView::OnDraw.</summary>
	BOOL m_bGetThumbnailMode;   // TRUE => document was created by dllhost for thumbnails. Should be checked in CView::OnDraw
	/// <summary>
	/// Specifies that CDocument object was created by prevhost for Rich Preview. Should be checked in CView::OnDraw.</summary>
	BOOL m_bPreviewHandlerMode; // TRUE => document was created by prevhost for rich preview
	/// <summary>
	/// Specifies that CDocument object was created by indexer or other search application.</summary>
	BOOL m_bSearchMode;         // TRUE => document was created by indexer or other search application

	// rich preview visuals
	/// <summary>
	/// Specifies background color of Rich Preview window. This color is set by host.</summary>
	COLORREF m_clrRichPreviewBackColor;
	/// <summary>
	/// Specifies foreground color of Rich Preview window. This color is set by host.</summary>
	COLORREF m_clrRichPreviewTextColor;
	/// <summary>
	/// Specifies text font for Rich Preview window. This font information is set by host.</summary>
	CFont    m_lfRichPreviewFont;

#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
	virtual void AssertValid() const;
#endif //_DEBUG
	virtual ~CDocument() = 0;

	// implementation helpers
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
	virtual BOOL DoFileSave();
	virtual void UpdateFrameCounts();
	void DisconnectViews();
	void SendInitialUpdate();

	// overridables for implementation
	virtual HMENU GetDefaultMenu(); // get menu depending on state
	virtual HACCEL GetDefaultAccelerator();
	virtual void OnIdle();
	virtual void OnFinalRelease();

	// search/organize/preview/thumbnail overridables
	// Load document from IStream
	/// <summary>
	/// Called by the framework when it needs to load the document data from stream.</summary>
	/// <returns> S_OK if load succeeds, otherwise error code.</returns>
	/// <param name="pStream">A pointer to incoming stream.</param>
	/// <param name="grfMode">Access mode to the stream.</param>
	virtual HRESULT OnLoadDocumentFromStream(IStream* pStream, DWORD grfMode);
	/// <summary>
	/// Directs the preview handler to return the HWND from calling the GetFocus Function. </summary>
	/// <returns> Returns S_OK if successful, or an error value otherwise. </returns>
	/// <param name="phwnd"> [out] When this method returns, contains a pointer to the HWND returned from calling the GetFocus Function from the preview handler's foreground thread.</param>
	virtual HRESULT OnPreviewHandlerQueryFocus(HWND* phwnd);
	/// <summary>
	/// Directs the preview handler to handle a keystroke passed up from the message pump of the process in which the preview handler is running.</summary>
	/// <returns>If the keystroke message can be processed by the preview handler, the handler will
	/// process it and return S_OK. If the preview handler cannot process the keystroke message,
	/// it will offer it to the host via IPreviewHandlerFrame::TranslateAccelerator. If the host processes the message, this method will return S_OK. If the host does not process the message,
	/// this method will return S_FALSE.</returns>
	/// <param name="pmsg">[in] A pointer to a window message.</param>
	virtual HRESULT OnPreviewHandlerTranslateAccelerator(MSG* pmsg);
	/// <summary>
	/// Called by the framework when it needs to create a preview frame for Rich Preview.</summary>
	/// <returns> Returns TRUE if the frame is created successfully; otherwise FALSE.</returns>
	virtual BOOL OnCreatePreviewFrame();
	/// <summary>
	/// Called by the framework when the preview handler is beign unloaded.</summary>
	virtual void OnUnloadHandler();

	/// <summary>
	/// Override this method in a derived class to draw content of thumbnail </summary>
	/// <param name="dc"> A reference to a device context.</param>
	/// <param name="lprcBounds"> Specifies a bounding rectangle of area where the thumbnail should be drawn.</param>
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);

	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
	friend class CDocTemplate;

protected:
	// file menu commands
	afx_msg void OnFileClose();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	// mail enabling
	afx_msg void OnFileSendMail();
	afx_msg void OnUpdateFileSendMail(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

public:
	BEGIN_INTERFACE_PART(InitializeWithStream, IInitializeWithStream)
		INIT_INTERFACE_PART(CDocument, InitWithStream)
		STDMETHOD(Initialize)(IStream* pStream, DWORD grfMode);
	END_INTERFACE_PART(InitializeWithStream)

	BEGIN_INTERFACE_PART(PreviewHandler, IPreviewHandler)
		INIT_INTERFACE_PART(CDocument, PreviewHandler)
		STDMETHOD(SetWindow)(HWND hwnd, const RECT *prc);
		STDMETHOD(SetRect)(const RECT *prc);
		STDMETHOD(DoPreview)();
		STDMETHOD(Unload)();
		STDMETHOD(SetFocus)();
		STDMETHOD(QueryFocus)(HWND *phwnd);
		STDMETHOD(TranslateAccelerator)(MSG *pmsg);
	END_INTERFACE_PART(PreviewHandler)

	BEGIN_INTERFACE_PART(PreviewHandlerVisuals, IPreviewHandlerVisuals)
		INIT_INTERFACE_PART(CDocument, PreviewHandlerVisuals)
		STDMETHOD(SetBackgroundColor)(COLORREF color);
		STDMETHOD(SetFont)(const LOGFONTW *plf);
		STDMETHOD(SetTextColor)(COLORREF color);
	END_INTERFACE_PART(PreviewHandlerVisuals)

	BEGIN_INTERFACE_PART(ObjectWithSite, IObjectWithSite)
		INIT_INTERFACE_PART(CDocument, ObjectWithSite)
		STDMETHOD(SetSite)(IUnknown *punkSite);
		STDMETHOD(GetSite)(REFIID riid, void **ppvSite);
	END_INTERFACE_PART(ObjectWithSite)

	BEGIN_INTERFACE_PART(OleWindow, IOleWindow)
		INIT_INTERFACE_PART(CDocument, ObjectWithSite)
		STDMETHOD(GetWindow)(HWND *phwnd);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
	END_INTERFACE_PART(OleWindow)

	DECLARE_INTERFACE_MAP()

protected:
	IPreviewHandlerFrame* m_pPreviewHandlerSite; // Ole frame for rich preview
};

/// <summary>
/// This is a class which simplifies both chunk and property value pair logic. </summary>
/// <remarks>
/// To use, you simply create a CMFCFilterChunkValueImpl class of the right kind
/// Example:
///      CMFCFilterChunkValueImpl chunk;
///      hr = chunk.SetBoolValue(PKEY_IsAttachment, true);
///      or
///      hr = chunk.SetFileTimeValue(PKEY_ItemDate, ftLastModified);</remarks>
class CMFCFilterChunkValueImpl : public ATL::IFilterChunkValue
{
public:
	/// <summary>
	/// Constructs the object</summary>
	CMFCFilterChunkValueImpl();
	/// <summary>
	/// Destructs the object</summary>
	virtual ~CMFCFilterChunkValueImpl();

	///<summary>
	/// Clear the ChunkValue. </summary>
	void Clear();

	/// <summary>
	/// Checks whether this property value valid, or not. </summary>
	/// <returns>TRUE if the current chunk value is valid; otherwise FALSE.</returns>
	BOOL IsValid() const
	{
		return m_fIsValid;
	}

	/// <summary>
	/// Gets the value as an allocated propvariant. </summary>
	/// <returns>S_OK if PROPVARIANT was allocated successfully and the chunk value was successfully copied to ppPropVariant; otherwise error code.</returns>
	/// <param name="ppPropVariant">When the function returns it contains chunk value.</param>
	HRESULT GetValue(PROPVARIANT **ppPropVariant);

	/// <summary>
	/// Returns non-allocated (internal value) value. </summary>
	/// <returns>Returns the current chunk value.</returns>
	PROPVARIANT GetValueNoAlloc ()
	{
		return m_propVariant;
	}

	/// <summary>
	/// Gets the string value. </summary>
	/// <returns> A string containing the chunk value. </returns>
	CString &GetString()
	{
		return m_strValue;
	};

	/// <summary>
	/// Initializes this chunk value from the other value. </summary>
	/// <param name="pValue"> Specifies the source value to copy from. </param>
	void CopyFrom (ATL::IFilterChunkValue* pValue);

	/// <summary>
	/// Copies this chunk to a structure describing the characteristics of a chunk. </summary>
	/// <param name="pStatChunk"> A pointer to destination value describing the characteristics of chunk. </param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT CopyChunk(STAT_CHUNK *pStatChunk);

	/// <summary>
	/// Gets chunk type. </summary>
	/// <returns> A CHUNKSTATE enumerated value, which specifies whether the current chunk is a text-type property or a value-type property</returns>
	CHUNKSTATE GetChunkType() const
	{
		return m_chunk.flags;
	}

	/// <summary>
	/// Gets chunk GUID. </summary>
	/// <returns> A reference to GUID identifying the chunk.</returns>
	REFGUID GetChunkGUID() const
	{
		return m_chunk.attribute.guidPropSet;
	}

	/// <summary>
	/// Gets chunk PID (property ID). </summary>
	/// <returns> A DWORD value containing property ID.</returns>
	DWORD GetChunkPID() const
	{
		return m_chunk.attribute.psProperty.propid;
	}

	/// <summary>
	/// Set the property by key to a Unicode string. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="pszValue">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetTextValue(REFPROPERTYKEY pkey, LPCTSTR pszValue, CHUNKSTATE chunkType = CHUNK_VALUE,
		LCID locale = 0, DWORD cwcLenSource = 0, DWORD cwcStartSource = 0,
		CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to a bool. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="bVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetBoolValue(REFPROPERTYKEY pkey, BOOL bVal, CHUNKSTATE chunkType = CHUNK_VALUE, LCID locale = 0,
		DWORD cwcLenSource = 0, DWORD cwcStartSource = 0, CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to a variant bool. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="bVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetBoolValue(REFPROPERTYKEY pkey, VARIANT_BOOL bVal, CHUNKSTATE chunkType = CHUNK_VALUE, LCID locale = 0,
		DWORD cwcLenSource = 0, DWORD cwcStartSource = 0, CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to an int. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="nVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetIntValue(REFPROPERTYKEY pkey, int nVal, CHUNKSTATE chunkType = CHUNK_VALUE,
		LCID locale = 0, DWORD cwcLenSource = 0, DWORD cwcStartSource = 0,
		CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to a LONG. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="lVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetLongValue(REFPROPERTYKEY pkey, long lVal, CHUNKSTATE chunkType = CHUNK_VALUE, LCID locale = 0,
		DWORD cwcLenSource = 0, DWORD cwcStartSource = 0, CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to a DWORD. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="dwVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetDwordValue(REFPROPERTYKEY pkey, DWORD dwVal, CHUNKSTATE chunkType = CHUNK_VALUE, LCID locale = 0,
		DWORD cwcLenSource = 0, DWORD cwcStartSource = 0, CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to an int64. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="nVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetInt64Value(REFPROPERTYKEY pkey, __int64 nVal, CHUNKSTATE chunkType = CHUNK_VALUE, LCID locale = 0,
		DWORD cwcLenSource = 0, DWORD cwcStartSource = 0, CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to a SystemTime. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="systemTime">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetSystemTimeValue(REFPROPERTYKEY pkey, const SYSTEMTIME &systemTime, CHUNKSTATE chunkType = CHUNK_VALUE, LCID locale=0, DWORD cwcLenSource=0, DWORD cwcStartSource=0, CHUNK_BREAKTYPE chunkBreakType=CHUNK_NO_BREAK);

	/// <summary>
	/// Set the property by key to a filetime. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="dtVal">Specifies the chunk value to set</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetFileTimeValue(REFPROPERTYKEY pkey, FILETIME dtVal, CHUNKSTATE chunkType = CHUNK_VALUE,
		LCID locale = 0, DWORD cwcLenSource = 0, DWORD cwcStartSource = 0,
		CHUNK_BREAKTYPE chunkBreakType = CHUNK_NO_BREAK);

protected:
	/// <summary>
	/// A helper function that sets chunk's common properties. </summary>
	/// <param name="pkey">Specifies a property key.</param>
	/// <param name="chunkType">Flags indicate whether this chunk contains a text-type or a value-type property. Flag values are taken from the CHUNKSTATE enumeration.</param>
	/// <param name="locale">The language and sublanguage associated with a chunk of text. Chunk locale is used by document indexers to perform proper word breaking of text. If the chunk is neither text-type nor a value-type with data type VT_LPWSTR, VT_LPSTR or VT_BSTR, this field is ignored.</param>
	/// <param name="cwcLenSource">The length in characters of the source text from which the current chunk was derived. A zero value signifies character-by-character correspondence between the source text and the derived text. A nonzero value means that no such direct correspondence exists.</param>
	/// <param name="cwcStartSource">The offset from which the source text for a derived chunk starts in the source chunk. </param>
	/// <param name="chunkBreakType">The type of break that separates the previous chunk from the current chunk. Values are from the CHUNK_BREAKTYPE enumeration.</param>
	/// <returns> S_OK if success; otherwise error code.</returns>
	HRESULT SetChunk(REFPROPERTYKEY pkey, CHUNKSTATE chunkType=CHUNK_VALUE, LCID locale=0, DWORD cwcLenSource=0, DWORD cwcStartSource=0, CHUNK_BREAKTYPE chunkBreakType=CHUNK_NO_BREAK);

	// member variables
private:
	BOOL m_fIsValid;
	STAT_CHUNK  m_chunk;
	PROPVARIANT m_propVariant;
	CString m_strValue;
};


/*============================================================================*/
// Extra diagnostic tracing options

#ifdef _DEBUG
extern AFX_DATA UINT afxTraceFlags;
#endif // _DEBUG

#ifdef _DEBUG
#define DECLARE_AFX_TRACE_CATEGORY( name ) extern AFX_DATA ATL::CTraceCategory name;
#else
#define DECLARE_AFX_TRACE_CATEGORY( name ) const DWORD_PTR name = 0;
#endif

DECLARE_AFX_TRACE_CATEGORY( traceAppMsg )        // main message pump trace (includes DDE)
DECLARE_AFX_TRACE_CATEGORY( traceCmdRouting )    // Windows command routing trace
DECLARE_AFX_TRACE_CATEGORY( traceOle )          // special OLE callback trace
DECLARE_AFX_TRACE_CATEGORY( traceDatabase )     // special database trace
DECLARE_AFX_TRACE_CATEGORY( traceInternet )     // special Internet client trace
DECLARE_AFX_TRACE_CATEGORY( traceHtml )			// Html traces

/*============================================================================*/
// MessageBox helpers

void AFXAPI AfxFormatString1(CString& rString, UINT nIDS, LPCTSTR lpsz1);
void AFXAPI AfxFormatString2(CString& rString, UINT nIDS,
				LPCTSTR lpsz1, LPCTSTR lpsz2);
int AFXAPI AfxMessageBox(LPCTSTR lpszText, UINT nType = MB_OK,
				UINT nIDHelp = 0);
int AFXAPI AfxMessageBox(UINT nIDPrompt, UINT nType = MB_OK,
				UINT nIDHelp = (UINT)-1);

// Implementation string helpers
void AFXAPI AfxFormatStrings(CString& rString, UINT nIDS,
				LPCTSTR const* rglpsz, int nString);
void AFXAPI AfxFormatStrings(CString& rString, LPCTSTR lpszFormat,
				LPCTSTR const* rglpsz, int nString);
BOOL AFXAPI AfxExtractSubString(CString& rString, LPCTSTR lpszFullString,
				int iSubString, TCHAR chSep = '\n');

/*============================================================================*/
// Special target variant APIs

#ifdef _AFXDLL
	#include <afxdll_.h>
#endif

// Windows Version compatibility (obsolete)
#define AfxEnableWin30Compatibility()
#define AfxEnableWin31Compatibility()
#define AfxEnableWin40Compatibility()

// Temporary map management (locks temp map on current thread)
void AFXAPI AfxLockTempMaps();
BOOL AFXAPI AfxUnlockTempMaps(BOOL bDeleteTemps = TRUE);

/*============================================================================*/
// Special OLE related functions (see OLELOCK.CPP)

void AFXAPI AfxOleOnReleaseAllObjects();
BOOL AFXAPI AfxOleCanExitApp();
void AFXAPI AfxOleLockApp();
void AFXAPI AfxOleUnlockApp();

void AFXAPI AfxOleSetUserCtrl(BOOL bUserCtrl);
BOOL AFXAPI AfxOleGetUserCtrl();

BOOL AFXAPI AfxOleLockControl(REFCLSID clsid);
BOOL AFXAPI AfxOleUnlockControl(REFCLSID clsid);
BOOL AFXAPI AfxOleLockControl(LPCTSTR lpszProgID);
BOOL AFXAPI AfxOleUnlockControl(LPCTSTR lpszProgID);
void AFXAPI AfxOleUnlockAllControls();

/*============================================================================*/
// Use version 1.0 of the RichEdit control

#define _RICHEDIT_VER 0x0210

/*============================================================================*/
// Inline function declarations

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXWIN_INLINE AFX_INLINE
#include <afxwin1.inl>
#include <afxwin2.inl>
#include <afxwin3.inl>
#endif

#include <afxwin4.inl>
#include <afxlayout.h>

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

/*============================================================================*/

#else //RC_INVOKED
#include <afxres.h>     // standard resource IDs
#endif //RC_INVOKED


#ifdef _M_CEE
    #include <atliface.h>
    #include <afxole.h>
#endif

#pragma warning( pop )

#endif //__AFXWIN_H__

/*============================================================================*/


