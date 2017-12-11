// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFX_H__
#define __AFX_H__

#ifndef __cplusplus
	#error MFC requires C++ compilation (use a .cpp suffix)
#endif

#pragma once

/////////////////////////////////////////////////////////////////////////////

#ifdef _DLL
#ifndef _AFXDLL
#error Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]
#endif
#endif

#ifndef NO_WARN_MBCS_MFC_DEPRECATION
#ifdef _MBCS
// Warn about MBCS support being deprecated: see http://go.microsoft.com/fwlink/p/?LinkId=279048 for more information.
#pragma warning(push)
#pragma warning(1 : 4996)
inline __declspec(deprecated("MBCS support in MFC is deprecated and may be removed in a future version of MFC.")) void MBCS_Support_Deprecated_In_MFC() { }

class MBCS_Deprecated_MFC
{
public:
	MBCS_Deprecated_MFC() { MBCS_Support_Deprecated_In_MFC(); }
};
#pragma warning(pop)
#endif
#endif

#if !defined(_M_IX86) && !defined(_M_AMD64) && !defined(_M_ARM) && !defined(_M_ARM64)
	#error Compiling for unsupported platform.  Only x86, x64, ARM, and ARM64 platforms are supported by MFC.
#endif

#if defined(_MANAGED) && !defined(_M_IX86) && !defined(_M_AMD64)
	#error Compiling for unsupported platform.  Managed MFC only supports x86 and x64 platforms.
#endif

// Since MFC itself is built with wchar_t as a native type, it will not have
// the correct type info for types built with wchar_t typedef'd to unsigned
// short.  Make sure that the user's app builds this type info in this case.
#ifndef _NATIVE_WCHAR_T_DEFINED
#define _AFX_FULLTYPEINFO
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif 
 
#ifdef __ATLDBGMEM_H__
#error <atldbgmem.h> cannot be used in MFC projects. See AfxEnableMemoryTracking
#endif

#ifndef _ATL_DISABLE_NOTHROW_NEW
#define _ATL_DISABLE_NOTHROW_NEW
#endif

#if defined(_MFC_DLL_BLD) && defined(_DEBUG)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#endif

#ifndef _INC_NEW
	#include <new.h>
#endif

#if defined(_MFC_BLD)
// For MFC DLLs and static libraries, enable the
// isolation-aware APIs in the Windows headers
#define ISOLATION_AWARE_ENABLED 1
#endif

#include <afxver_.h>        // Target version control

#if defined(_WIN64) || defined(_M_ARM)
#ifndef _AFX_NO_DAO_SUPPORT
#define _AFX_NO_DAO_SUPPORT
#endif
#endif

#ifndef _AFX_NOFORCE_LIBS

/*============================================================================*/
// Win32 libraries

#ifndef _AFXDLL
	#ifdef _AFX_NO_MFC_CONTROLS_IN_DIALOGS
		#ifdef _DEBUG
			#pragma comment(lib, "afxnmcdd.lib")
		#else
			#pragma comment(lib, "afxnmcd.lib")
		#endif
		#pragma comment(linker, "/include:__afxNoMFCControlSupportInDialogs")
		#pragma comment(linker, "/include:__afxNoMFCControlContainerInDialogs")
	#endif
	#ifndef _UNICODE
		#ifdef _DEBUG
			#pragma comment(lib, "nafxcwd.lib")
		#else
			#pragma comment(lib, "nafxcw.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "uafxcwd.lib")
		#else
			#pragma comment(lib, "uafxcw.lib")
		#endif
	#endif
#else
	#ifndef _UNICODE
		#ifdef _DEBUG
			#pragma comment(lib, "mfc" _MFC_FILENAME_VER "d.lib")
			#pragma comment(lib, "mfcs" _MFC_FILENAME_VER "d.lib")
		#else
			#pragma comment(lib, "mfc" _MFC_FILENAME_VER ".lib")
			#pragma comment(lib, "mfcs" _MFC_FILENAME_VER ".lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "mfc" _MFC_FILENAME_VER "ud.lib")
			#pragma comment(lib, "mfcs" _MFC_FILENAME_VER "ud.lib")
		#else
			#pragma comment(lib, "mfc" _MFC_FILENAME_VER "u.lib")
			#pragma comment(lib, "mfcs" _MFC_FILENAME_VER "u.lib")
		#endif
	#endif
#endif

#ifdef _DLL
	#if defined(_DEBUG)
		#pragma comment(lib, "msvcrtd.lib")
	#else
		#pragma comment(lib, "msvcrt.lib")
	#endif
#else
	#if defined(_DEBUG)
		#pragma comment(lib, "libcmtd.lib")
	#else
		#pragma comment(lib, "libcmt.lib")
	#endif
#endif

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "windowscodecs.lib")

// force inclusion of NOLIB.OBJ for /disallowlib directives
#pragma comment(linker, "/include:__afxForceEXCLUDE")

// force inclusion of DLLMODUL.OBJ for _USRDLL
#ifdef _USRDLL
#pragma comment(linker, "/include:__afxForceUSRDLL")
#endif

// force inclusion of STDAFX.OBJ for precompiled types
#ifdef _AFXDLL
#pragma comment(linker, "/include:__afxForceSTDAFX")
#endif

#endif //!_AFX_NOFORCE_LIBS

#ifdef _MANAGED

#ifndef AFX_NO_CLR_COINIT_STA
#pragma comment(linker, "/CLRTHREADATTRIBUTE:STA")
#endif 

#endif //_MANAGED
/*============================================================================*/
// Classes declared in this file
//   in addition to standard primitive data types and various helper macros

struct CRuntimeClass;          // object type information

class CObject;                        // the root of all objects classes

	class CException;                 // the root of all exceptions
		class CArchiveException;      // archive exception
		class CFileException;         // file exception
		class CSimpleException;
			class CMemoryException;       // out-of-memory exception
			class CNotSupportedException; // feature not supported exception
			class CInvalidArgException;	  // one of the parameters to the function is invalid

	class CFile;                      // raw binary file
		class CStdioFile;             // buffered stdio text/binary file
		class CMemFile;               // memory based file

// Non CObject classes
struct CFileStatus;                   // file status information
struct CMemoryState;                  // diagnostic memory support

class CArchive;                       // object persistence tool
class CDumpContext;                   // object diagnostic dumping

/*============================================================================*/
// Other includes from standard "C" runtimes

#ifndef _INC_STRING
	#include <string.h>
#endif
#ifndef _INC_STDIO
	#include <stdio.h>
#endif
#ifndef _INC_STDLIB
	#include <stdlib.h>
#endif
#ifndef _INC_TIME
	#include <time.h>
#endif
#ifndef _INC_LIMITS
	#include <limits.h>
#endif
#ifndef _INC_STDDEF
	#include <stddef.h>
#endif
#ifndef _INC_STDARG
	#include <stdarg.h>
#endif
#ifndef _INC_ERRNO 
#include <errno.h>
#endif

#include <malloc.h>

#ifndef _INC_CRTDBG
	#include <crtdbg.h>
#endif

#ifdef _AFX_OLD_EXCEPTIONS
#error MFC no longer supports setjmp/longjmp exception handling.
#endif

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

/*============================================================================*/
// Basic types
// abstract iteration position
struct __POSITION {};
typedef __POSITION* POSITION;

// Standard constants
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0

/*============================================================================*/
// _AFX_FUNCNAME definition
#ifdef UNICODE
#define _AFX_FUNCNAME(_Name) _Name##W
#else
#define _AFX_FUNCNAME(_Name) _Name##A
#endif

/*============================================================================*/
// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the AFX include files

#pragma warning(disable: 4505)  // unreferenced local function has been removed
#pragma warning(disable: 4511)  // private copy constructors are good to have
#pragma warning(disable: 4512)  // private operator= are good to have
#pragma warning(disable: 4514)  // unreferenced inlines are common
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4127)  // constant expression used in macros do while(0);

// warnings specific to _AFXDLL version
#ifdef _AFXDLL
#pragma warning(disable: 4275)  // deriving exported class from non-exported
#pragma warning(disable: 4251)  // using non-exported as public in exported
#endif

#ifdef _AFX_ALL_WARNINGS
#pragma warning( push )
#endif

// warnings generated with common MFC/Windows code
#pragma warning(disable: 4201)  // nameless unions are part of C++
#pragma warning(disable: 4191)  // pointer-to-function casting
// warnings caused by normal optimizations
#ifndef _DEBUG
#pragma warning(disable: 4701)  // local variable *may* be used without init
#pragma warning(disable: 4702)  // unreachable code caused by optimizations
#pragma warning(disable: 4189)  // initialized but unused variable
#pragma warning(disable: 4390)  // empty controlled statement
#endif
// warnings specific to _AFXDLL version
#ifdef _AFXDLL
#pragma warning(disable: 4204)  // non-constant aggregate initializer
#endif
#pragma warning(disable: 4263 4264)  // base class method is hidden

/*============================================================================*/
// Diagnostic support

#ifdef _DEBUG

inline BOOL AFXAPI AfxAssertFailedLine(LPCSTR lpszFileName, int nLine)
{
	// we remove WM_QUIT because if it is in the queue then the message box
	// won't display
	MSG msg;
	BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	BOOL bResult = _CrtDbgReport(_CRT_ASSERT, lpszFileName, nLine, NULL, NULL);
	if (bQuit)
		PostQuitMessage((int)msg.wParam);
	return bResult;
}

void AFX_CDECL AfxTrace(LPCTSTR lpszFormat, ...);
// Note: file names are still ANSI strings (filenames rarely need UNICODE)
void AFXAPI AfxAssertValidObject(const CObject* pOb,
				LPCSTR lpszFileName, int nLine);
void AFXAPI AfxDump(const CObject* pOb); // Dump an object from CodeView

#include <atltrace.h>

// extern ATL::CTrace TRACE;
#define TRACE ATLTRACE

#define THIS_FILE          __FILE__
#define VERIFY(f)          ASSERT(f)
#define DEBUG_ONLY(f)      (f)

// The following trace macros are provided for backward compatiblity
//  (they also take a fixed number of parameters which provides
//   some amount of extra error checking)
#define TRACE0(sz)              TRACE(_T("%Ts"), _T(sz))
#define TRACE1(sz, p1)          TRACE(_T(sz), p1)
#define TRACE2(sz, p1, p2)      TRACE(_T(sz), p1, p2)
#define TRACE3(sz, p1, p2, p3)  TRACE(_T(sz), p1, p2, p3)

// These AFX_DUMP macros also provided for backward compatibility
#define AFX_DUMP0(dc, sz)   dc << _T(sz)
#define AFX_DUMP1(dc, sz, p1) dc << _T(sz) << p1

#else   // _DEBUG

#define VERIFY(f)          ((void)(f))
#define DEBUG_ONLY(f)      ((void)0)
#pragma warning(push)
#pragma warning(disable : 4793)
inline void AFX_CDECL AfxTrace(...) { }
#pragma warning(pop)
#define TRACE              __noop
#define TRACE0(sz)
#define TRACE1(sz, p1)
#define TRACE2(sz, p1, p2)
#define TRACE3(sz, p1, p2, p3)

#endif // !_DEBUG

#define ASSERT(f)          DEBUG_ONLY((void) ((f) || !::AfxAssertFailedLine(THIS_FILE, __LINE__) || (AfxDebugBreak(), 0)))
/* see ATL headers for commentary on this */
/* We use the name AFXASSUME to avoid name clashes */

#if defined(_PREFAST_) || defined (_DEBUG)
#define AFXASSUME(cond)			do { bool __afx_condVal=!!(cond); ASSERT(__afx_condVal); _Analysis_assume_(__afx_condVal); } while(0) 
#else
#define AFXASSUME(cond)			((void)0)
#endif

#define ASSERT_VALID(pOb)  DEBUG_ONLY((::AfxAssertValidObject(pOb, THIS_FILE, __LINE__)))

// Debug ASSERTs then throws. Retail throws if condition not met
#define ENSURE_THROW(cond, exception)	\
	do { int __afx_condVal=!!(cond); ASSERT(__afx_condVal); if (!(__afx_condVal)){exception;} } while (false)
#define ENSURE(cond)		ENSURE_THROW(cond, ::AfxThrowInvalidArgException() )
#define ENSURE_ARG(cond)	ENSURE_THROW(cond, ::AfxThrowInvalidArgException() )

// Debug ASSERT_VALIDs then throws. Retail throws if pOb is NULL
#define ENSURE_VALID_THROW(pOb, exception)	\
	do { ASSERT_VALID(pOb); if (!(pOb)){exception;} } while (false)
#define ENSURE_VALID(pOb)	ENSURE_VALID_THROW(pOb, ::AfxThrowInvalidArgException() )

#define ASSERT_POINTER(p, type) \
	ASSERT(((p) != NULL) && AfxIsValidAddress((p), sizeof(type), FALSE))

#define ASSERT_NULL_OR_POINTER(p, type) \
	ASSERT(((p) == NULL) || AfxIsValidAddress((p), sizeof(type), FALSE))

#ifdef _DEBUG
#define UNUSED(x)
#else
#define UNUSED(x) UNREFERENCED_PARAMETER(x)
#endif
#define UNUSED_ALWAYS(x) UNREFERENCED_PARAMETER(x)

#ifdef _DEBUG
#define REPORT_EXCEPTION(pException, szMsg) \
	do { \
		TCHAR szErrorMessage[512]; \
		if (pException->GetErrorMessage(szErrorMessage, sizeof(szErrorMessage)/sizeof(*szErrorMessage), 0)) \
			TRACE(traceAppMsg, 0, _T("%Ts (%Ts:%d)\n%Ts\n"), szMsg, _T(__FILE__), __LINE__, szErrorMessage); \
		else \
			TRACE(traceAppMsg, 0, _T("%Ts (%Ts:%d)\n"), szMsg, _T(__FILE__), __LINE__); \
		ASSERT(FALSE); \
	} while (0)
#else
#define REPORT_EXCEPTION(pException, szMsg) \
	do { \
		CString strMsg; \
		TCHAR  szErrorMessage[512]; \
		if (pException->GetErrorMessage(szErrorMessage, sizeof(szErrorMessage)/sizeof(*szErrorMessage), 0)) \
			strMsg.Format(_T("%Ts (%Ts:%d)\n%Ts"), szMsg, _T(__FILE__), __LINE__, szErrorMessage); \
		else \
			strMsg.Format(_T("%Ts (%Ts:%d)"), szMsg, _T(__FILE__), __LINE__); \
		AfxMessageBox(strMsg); \
	} while (0)
#endif

#define EXCEPTION_IN_DTOR(pException) \
	do { \
		REPORT_EXCEPTION((pException), _T("Exception thrown in destructor")); \
		delete pException; \
	} while (0)
	
#define AFX_BEGIN_DESTRUCTOR try {
#define AFX_END_DESTRUCTOR   } catch (CException *pException) { EXCEPTION_IN_DTOR(pException); }

/*============================================================================*/
// Other implementation helpers

#define BEFORE_START_POSITION ((POSITION)-1L)

/*============================================================================*/
// explicit initialization for general purpose classes

BOOL AFXAPI AfxInitialize(BOOL bDLL = FALSE, DWORD dwVersion = _MFC_VER);

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/*============================================================================*/
// Basic object model

// generate static object constructor for class registration
void AFXAPI AfxClassInit(CRuntimeClass* pNewClass);
struct AFX_CLASSINIT
	{ AFX_CLASSINIT(CRuntimeClass* pNewClass) { AfxClassInit(pNewClass); } };

struct CRuntimeClass
{
// Attributes
	LPCSTR m_lpszClassName;
	int m_nObjectSize;
	UINT m_wSchema; // schema number of the loaded class
	CObject* (PASCAL* m_pfnCreateObject)(); // NULL => abstract class
#ifdef _AFXDLL
	CRuntimeClass* (PASCAL* m_pfnGetBaseClass)();
#else
	CRuntimeClass* m_pBaseClass;
#endif

// Operations
	CObject* CreateObject();
	BOOL IsDerivedFrom(const CRuntimeClass* pBaseClass) const;

	// dynamic name lookup and creation
	static CRuntimeClass* PASCAL FromName(LPCSTR lpszClassName);
	static CRuntimeClass* PASCAL FromName(LPCWSTR lpszClassName);
	static CObject* PASCAL CreateObject(LPCSTR lpszClassName);
	static CObject* PASCAL CreateObject(LPCWSTR lpszClassName);

// Implementation
	void Store(CArchive& ar) const;
	static CRuntimeClass* PASCAL Load(CArchive& ar, UINT* pwSchemaNum);

	// CRuntimeClass objects linked together in simple list
	CRuntimeClass* m_pNextClass;       // linked list of registered classes
	const AFX_CLASSINIT* m_pClassInit;
};

/*============================================================================*/
// Standard exception throws

void __declspec(noreturn) AFXAPI AfxThrowMemoryException();
void __declspec(noreturn) AFXAPI AfxThrowNotSupportedException();
void __declspec(noreturn) AFXAPI AfxThrowInvalidArgException();
void __declspec(noreturn) AFXAPI AfxThrowArchiveException(int cause,
	LPCTSTR lpszArchiveName = NULL);
void __declspec(noreturn) AFXAPI AfxThrowFileException(int cause, LONG lOsError = -1,
	LPCTSTR lpszFileName = NULL);
void __declspec(noreturn) AFXAPI AfxThrowOleException(LONG sc);

/*============================================================================*/
// CRT functions

inline errno_t AfxCrtErrorCheck(errno_t error)
{
	switch(error)
	{
	case ENOMEM:
		AfxThrowMemoryException();
		break;
	case EINVAL:
	case ERANGE:
		AfxThrowInvalidArgException();
		break;
	case STRUNCATE:
	case 0:
		break;
	default:
		AfxThrowInvalidArgException();
		break;
	}
	return error;
}

#define AFX_CRT_ERRORCHECK(expr) \
	AfxCrtErrorCheck(expr)

inline void __cdecl Afx_clearerr_s(FILE *stream)
{
	AFX_CRT_ERRORCHECK(::clearerr_s(stream));
}

/*============================================================================*/
// Strings

#ifndef _OLEAUTO_H_
	typedef _Null_terminated_ LPWSTR BSTR;// must (semantically) match typedef in oleauto.h
#endif

/*============================================================================*/
// class CObject is the root of all compliant objects

class AFX_NOVTABLE CObject
{
public:

// Object model (types, destruction, allocation)
	virtual CRuntimeClass* GetRuntimeClass() const;
	virtual ~CObject() = 0;  // virtual destructors are necessary

	// Diagnostic allocations
	void* PASCAL operator new(size_t nSize);
	void* PASCAL operator new(size_t, void* p);
	void PASCAL operator delete(void* p);
	void PASCAL operator delete(void* p, void* pPlace);

#if defined(_DEBUG)
	// for file name/line number tracking using DEBUG_NEW
	void* PASCAL operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
	void PASCAL operator delete(void *p, LPCSTR lpszFileName, int nLine);
#endif

	// Disable the copy constructor and assignment by default so you will get
	//   compiler errors instead of unexpected behaviour if you pass objects
	//   by value or assign objects.
protected:
	CObject();
private:
	CObject(const CObject& objectSrc);              // no implementation
	void operator=(const CObject& objectSrc);       // no implementation

// Attributes
public:
	BOOL IsSerializable() const;
	BOOL IsKindOf(const CRuntimeClass* pClass) const;

// Overridables
	virtual void Serialize(CArchive& ar);

#if defined(_DEBUG) || defined(_AFXDLL)
	// Diagnostic Support
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
public:
	static const CRuntimeClass classCObject;
#ifdef _AFXDLL
	static CRuntimeClass* PASCAL _GetBaseClass();
	static CRuntimeClass* PASCAL GetThisClass();
#endif
};

// Helper macros
#define _RUNTIME_CLASS(class_name) ((CRuntimeClass*)(&class_name::class##class_name))
#ifdef _AFXDLL
#define RUNTIME_CLASS(class_name) (class_name::GetThisClass())
#else
#define RUNTIME_CLASS(class_name) _RUNTIME_CLASS(class_name)
#endif
#define ASSERT_KINDOF(class_name, object) \
	ASSERT((object)->IsKindOf(RUNTIME_CLASS(class_name)))

// RTTI helper macros/functions
const CObject* AFX_CDECL AfxDynamicDownCast(CRuntimeClass* pClass, const CObject* pObject);
CObject* AFX_CDECL AfxDynamicDownCast(CRuntimeClass* pClass, CObject* pObject);
#define DYNAMIC_DOWNCAST(class_name, object) \
	(class_name*)AfxDynamicDownCast(RUNTIME_CLASS(class_name), object)

#ifdef _DEBUG
const CObject* AFX_CDECL AfxStaticDownCast(CRuntimeClass* pClass, const CObject* pObject);
CObject* AFX_CDECL AfxStaticDownCast(CRuntimeClass* pClass, CObject* pObject);
#define STATIC_DOWNCAST(class_name, object) \
	(static_cast<class_name*>(AfxStaticDownCast(RUNTIME_CLASS(class_name), object)))
#else
#define STATIC_DOWNCAST(class_name, object) (static_cast<class_name*>(object))
#endif

/*============================================================================*/
// Helper macros for declaring CRuntimeClass compatible classes

#ifdef _AFXDLL
#define DECLARE_DYNAMIC(class_name) \
protected: \
	static CRuntimeClass* PASCAL _GetBaseClass(); \
public: \
	static const CRuntimeClass class##class_name; \
	static CRuntimeClass* PASCAL GetThisClass(); \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define _DECLARE_DYNAMIC(class_name) \
protected: \
	static CRuntimeClass* PASCAL _GetBaseClass(); \
public: \
	static CRuntimeClass class##class_name; \
	static CRuntimeClass* PASCAL GetThisClass(); \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#else
#define DECLARE_DYNAMIC(class_name) \
public: \
	static const CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#define _DECLARE_DYNAMIC(class_name) \
public: \
	static CRuntimeClass class##class_name; \
	virtual CRuntimeClass* GetRuntimeClass() const; \

#endif

// not serializable, but dynamically constructable
#define DECLARE_DYNCREATE(class_name) \
	DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define _DECLARE_DYNCREATE(class_name) \
	_DECLARE_DYNAMIC(class_name) \
	static CObject* PASCAL CreateObject();

#define DECLARE_SERIAL(class_name) \
	_DECLARE_DYNCREATE(class_name) \
	AFX_API friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);

#ifdef _AFXDLL
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init) \
	CRuntimeClass* PASCAL class_name::_GetBaseClass() \
		{ return RUNTIME_CLASS(base_class_name); } \
	AFX_COMDAT const CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			&class_name::_GetBaseClass, NULL, class_init }; \
	CRuntimeClass* PASCAL class_name::GetThisClass() \
		{ return _RUNTIME_CLASS(class_name); } \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return _RUNTIME_CLASS(class_name); }

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init) \
	CRuntimeClass* PASCAL class_name::_GetBaseClass() \
		{ return RUNTIME_CLASS(base_class_name); } \
	AFX_COMDAT CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			&class_name::_GetBaseClass, NULL, class_init }; \
	CRuntimeClass* PASCAL class_name::GetThisClass() \
		{ return _RUNTIME_CLASS(class_name); } \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return _RUNTIME_CLASS(class_name); }

#else
#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init) \
	AFX_COMDAT const CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); }

#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew, class_init) \
	AFX_COMDAT CRuntimeClass class_name::class##class_name = { \
		#class_name, sizeof(class class_name), wSchema, pfnNew, \
			RUNTIME_CLASS(base_class_name), NULL, class_init }; \
	CRuntimeClass* class_name::GetRuntimeClass() const \
		{ return RUNTIME_CLASS(class_name); }

#endif

#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, NULL, NULL)

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, 0xFFFF, \
		class_name::CreateObject, NULL)

#define IMPLEMENT_SERIAL(class_name, base_class_name, wSchema) \
	CObject* PASCAL class_name::CreateObject() \
		{ return new class_name; } \
	extern AFX_CLASSINIT _init_##class_name; \
	_IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, \
		class_name::CreateObject, &_init_##class_name) \
	AFX_CLASSINIT _init_##class_name(RUNTIME_CLASS(class_name)); \
	CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb) \
		{ pOb = (class_name*) ar.ReadObject(RUNTIME_CLASS(class_name)); \
			return ar; }

// optional bit for schema number that enables object versioning
#define VERSIONABLE_SCHEMA  (0x80000000)

/*============================================================================*/
// Exceptions

class AFX_NOVTABLE CException : public CObject
{
	// abstract class for dynamic type checking
	DECLARE_DYNAMIC(CException)

public:
// Constructors
	CException();   // sets m_bAutoDelete = TRUE
	explicit CException(BOOL bAutoDelete);   // sets m_bAutoDelete = bAutoDelete

// Operations
	void Delete();  // use to delete exception in 'catch' block

	virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
		_Out_opt_ PUINT pnHelpContext = NULL) const ;
	virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
		_Out_opt_ PUINT pnHelpContext = NULL);
	virtual int ReportError(UINT nType = MB_OK, UINT nMessageID = 0);

// Implementation (setting m_bAutoDelete to FALSE is advanced)
public:
	virtual ~CException() = 0;
	BOOL m_bAutoDelete;
#ifdef _DEBUG
	void PASCAL operator delete(void* pbData);
	void PASCAL operator delete(void* pbData, LPCSTR lpszFileName, int nLine);
protected:
	BOOL m_bReadyForDelete;
#endif
};

#include <afxstr.h>

// ATL Classes

class CSimpleException : public CException
{
	DECLARE_DYNAMIC(CSimpleException)
	
	// base class for resource-critical MFC exceptions
	// handles ownership and initialization of an error message

public:
// Constructors
	CSimpleException();
	explicit CSimpleException(BOOL bAutoDelete);

// Operations
	virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
		_Out_opt_ PUINT pnHelpContext = NULL) const;

// Implementation (setting m_bAutoDelete to FALSE is advanced)
public:
	virtual ~CSimpleException() = 0;
	BOOL m_bAutoDelete;

	void InitString();      // used during MFC initialization

protected:
	BOOL m_bInitialized;
	BOOL m_bLoaded;
	TCHAR m_szMessage[128];
	UINT m_nResourceID;

#ifdef _DEBUG
	BOOL m_bReadyForDelete;
#endif
};

// helper routines for non-C++ EH implementations
	// for THROW_LAST auto-delete backward compatiblity
	void AFXAPI AfxThrowLastCleanup();

// other out-of-line helper functions
void AFXAPI AfxTryCleanup();

#ifndef _AFX_JUMPBUF
// Use portable 'jmp_buf' defined by ANSI by default.
#define _AFX_JUMPBUF jmp_buf
#endif

// Placed on frame for EXCEPTION linkage, or CException cleanup
struct AFX_EXCEPTION_LINK
{
	AFX_EXCEPTION_LINK* m_pLinkPrev;    // previous top, next in handler chain
	CException* m_pException;   // current exception (NULL in TRY block)

	AFX_EXCEPTION_LINK();       // for initialization and linking
	~AFX_EXCEPTION_LINK()       // for cleanup and unlinking
		{ AfxTryCleanup(); };
};

// Exception global state - never access directly
struct AFX_EXCEPTION_CONTEXT
{
	AFX_EXCEPTION_LINK* m_pLinkTop;

	// Note: most of the exception context is now in the AFX_EXCEPTION_LINK
};

#ifndef _PNH_DEFINED
typedef int (__cdecl * _PNH)( size_t );
#define _PNH_DEFINED
#endif

_PNH AFXAPI AfxGetNewHandler();
_PNH AFXAPI AfxSetNewHandler(_PNH pfnNewHandler);
int AFX_CDECL AfxNewHandler(size_t nSize);

void AFXAPI AfxAbort();


/*============================================================================*/
// Exception macros using try, catch and throw
//  (for backward compatibility to previous versions of MFC)

#define TRY { AFX_EXCEPTION_LINK _afxExceptionLink; try {

#define CATCH(class, e) } catch (class* e) \
	{ ASSERT(e->IsKindOf(RUNTIME_CLASS(class))); \
		_afxExceptionLink.m_pException = e;

#define AND_CATCH(class, e) } catch (class* e) \
	{ ASSERT(e->IsKindOf(RUNTIME_CLASS(class))); \
		_afxExceptionLink.m_pException = e;

#define END_CATCH } }

#define THROW(e) throw e
#define THROW_LAST() (AfxThrowLastCleanup(), throw)

// Advanced macros for smaller code
#define CATCH_ALL(e) } catch (CException* e) \
	{ { ASSERT(e->IsKindOf(RUNTIME_CLASS(CException))); \
		_afxExceptionLink.m_pException = e;

#define AND_CATCH_ALL(e) } catch (CException* e) \
	{ { ASSERT(e->IsKindOf(RUNTIME_CLASS(CException))); \
		_afxExceptionLink.m_pException = e;

#define END_CATCH_ALL } } }

#define END_TRY } catch (CException* e) \
	{ ASSERT(e->IsKindOf(RUNTIME_CLASS(CException))); \
		_afxExceptionLink.m_pException = e; } }


/*============================================================================*/
// Standard Exception classes

class CMemoryException : public CSimpleException
{
	DECLARE_DYNAMIC(CMemoryException)
public:
	CMemoryException();

// Implementation
public:
	explicit CMemoryException(BOOL bAutoDelete);
	CMemoryException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CMemoryException();
};

class CNotSupportedException : public CSimpleException
{
	DECLARE_DYNAMIC(CNotSupportedException)
public:
	CNotSupportedException();

// Implementation
public:
	explicit CNotSupportedException(BOOL bAutoDelete);
	CNotSupportedException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CNotSupportedException();
};
class CInvalidArgException : public CSimpleException
{
	DECLARE_DYNAMIC(CInvalidArgException)
public:
	CInvalidArgException();

// Implementation
public:
	CInvalidArgException(BOOL bAutoDelete);
	CInvalidArgException(BOOL bAutoDelete, UINT nResourceID);
	virtual ~CInvalidArgException();
};

class CArchiveException : public CException
{
	DECLARE_DYNAMIC(CArchiveException)
public:
	enum {
		none,
		genericException,
		readOnly,
		endOfFile,
		writeOnly,
		badIndex,
		badClass,
		badSchema,
		bufferFull
	};

#pragma warning(push)
#pragma warning(disable:4996)
	AFX_DEPRECATED("CArchiveException::generic clashes with future language keyword generic and should not be used. Use CArchiveException::genericException instead.") static const int __identifier(generic) = genericException;
#pragma warning(pop)

// Constructor
	explicit CArchiveException(int cause = CArchiveException::none,
		LPCTSTR lpszArchiveName = NULL);

// Attributes
	int m_cause;
	CString m_strFileName;

// Implementation
public:
	virtual ~CArchiveException();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
		_Out_opt_ PUINT pnHelpContext = NULL) const;
};

class CFileException : public CException
{
	DECLARE_DYNAMIC(CFileException)

public:
	enum {
		none,
		genericException,
		fileNotFound,
		badPath,
		tooManyOpenFiles,
		accessDenied,
		invalidFile,
		removeCurrentDir,
		directoryFull,
		badSeek,
		hardIO,
		sharingViolation,
		lockViolation,
		diskFull,
		endOfFile
	};

#pragma warning(push)
#pragma warning(disable:4996)
	AFX_DEPRECATED("CFileException::generic clashes with future language keyword generic and should not be used. Use CFileException::genericException instead.") static const int __identifier(generic) = genericException;
#pragma warning(pop)

// Constructor
	explicit CFileException(int cause = CFileException::none, LONG lOsError = -1,
		LPCTSTR lpszArchiveName = NULL);

// Attributes
	int     m_cause;
	LONG    m_lOsError;
	CString m_strFileName;

// Operations
	// convert a OS dependent error code to a Cause
	static int PASCAL OsErrorToException(LONG lOsError);
	static int PASCAL ErrnoToException(int nErrno);

	// helper functions to throw exception after converting to a Cause
	static void PASCAL ThrowOsError(LONG lOsError, LPCTSTR lpszFileName = NULL);
	static void PASCAL ThrowErrno(int nErrno, LPCTSTR lpszFileName = NULL);

// Implementation
public:
	virtual ~CFileException();
#ifdef _DEBUG
	virtual void Dump(CDumpContext&) const;
#endif
	virtual BOOL GetErrorMessage(_Out_writes_z_(nMaxError) LPTSTR lpszError, _In_ UINT nMaxError,
		_Out_opt_ PUINT pnHelpContext = NULL) const;
};

/*============================================================================*/
// File - raw unbuffered disk file I/O

#ifndef __ATLTRANSACTIONMANAGER_H__
#include <atltransactionmanager.h>
#endif

using ATL::CAtlTransactionManager;

class CFile : public CObject
{
	DECLARE_DYNAMIC(CFile)

public:
// Flag values
	enum OpenFlags {
		modeRead =         (int) 0x00000,
		modeWrite =        (int) 0x00001,
		modeReadWrite =    (int) 0x00002,
		shareCompat =      (int) 0x00000,
		shareExclusive =   (int) 0x00010,
		shareDenyWrite =   (int) 0x00020,
		shareDenyRead =    (int) 0x00030,
		shareDenyNone =    (int) 0x00040,
		modeNoInherit =    (int) 0x00080,
#ifdef _UNICODE
		typeUnicode =      (int) 0x00400, // used in derived classes (e.g. CStdioFile) only
#endif
		modeCreate =       (int) 0x01000,
		modeNoTruncate =   (int) 0x02000,
		typeText =         (int) 0x04000, // used in derived classes (e.g. CStdioFile) only
		typeBinary =       (int) 0x08000, // used in derived classes (e.g. CStdioFile) only
		osNoBuffer =       (int) 0x10000,
		osWriteThrough =   (int) 0x20000,
		osRandomAccess =   (int) 0x40000,
		osSequentialScan = (int) 0x80000,
		};

	enum Attribute {
		normal     = 0x00,                // note: not same as FILE_ATTRIBUTE_NORMAL
		readOnly   = FILE_ATTRIBUTE_READONLY,
		hidden     = FILE_ATTRIBUTE_HIDDEN,
		system     = FILE_ATTRIBUTE_SYSTEM,
		volume     = 0x08,
		directory  = FILE_ATTRIBUTE_DIRECTORY,
		archive    = FILE_ATTRIBUTE_ARCHIVE,
		device     = FILE_ATTRIBUTE_DEVICE,
		temporary  = FILE_ATTRIBUTE_TEMPORARY,
		sparse     = FILE_ATTRIBUTE_SPARSE_FILE,
		reparsePt  = FILE_ATTRIBUTE_REPARSE_POINT,
		compressed = FILE_ATTRIBUTE_COMPRESSED,
		offline    = FILE_ATTRIBUTE_OFFLINE,
		notIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
		encrypted  = FILE_ATTRIBUTE_ENCRYPTED
		};

	enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

	static AFX_DATA const HANDLE hFileNull;

// Constructors
	CFile();

	/// <summary>
	/// CFile constructor</summary>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	CFile(CAtlTransactionManager* pTM);

	CFile(HANDLE hFile);
	CFile(LPCTSTR lpszFileName, UINT nOpenFlags);

	/// <summary>
	/// CFile constructor</summary>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="nOpenFlags">Sharing and access mode. Specifies the action to take when opening the file. You can combine options listed below by using the bitwise-OR (|) operator. One access permission and one share option are required; the modeCreate and modeNoInherit modes are optional.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	CFile(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM);

// Attributes
	HANDLE m_hFile;
	operator HANDLE() const;

	virtual ULONGLONG GetPosition() const;
	BOOL GetStatus(CFileStatus& rStatus) const;
	virtual CString GetFileName() const;
	virtual CString GetFileTitle() const;
	virtual CString GetFilePath() const;
	virtual void SetFilePath(LPCTSTR lpszNewName);

// Operations
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);

	/// <summary>
	/// Open is designed for use with the default CFile constructor</summary>
	/// <returns> 
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="nOpenFlags">Sharing and access mode. Specifies the action to take when opening the file. You can combine options listed below by using the bitwise-OR (|) operator. One access permission and one share option are required; the modeCreate and modeNoInherit modes are optional.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	/// <param name="pError">A pointer to an existing file-exception object that will receive the status of a failed operation</param>
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM, CFileException* pError);

	/// <summary>
	/// This static function renames the specified file.</summary>
	/// <param name="lpszOldName">The old path.</param>
	/// <param name="lpszNewName">The new path.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	static void PASCAL Rename(LPCTSTR lpszOldName, LPCTSTR lpszNewName, CAtlTransactionManager* pTM = NULL);

	/// <summary>
	/// This static function deletes the file specified by the path.</summary>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	static void PASCAL Remove(LPCTSTR lpszFileName, CAtlTransactionManager* pTM = NULL); 

	/// <summary>
	/// This method retrieves status information related to a given CFile object instance or a given file path.</summary>
	/// <returns> 
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="rStatus">A reference to a user-supplied CFileStatus structure that will receive the status information.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	static BOOL PASCAL GetStatus(LPCTSTR lpszFileName, CFileStatus& rStatus, CAtlTransactionManager* pTM = NULL);

	/// <summary>
	/// Sets the status of the file associated with this file location.</summary>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="rStatus">The buffer containing the new status information.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	static void PASCAL SetStatus(LPCTSTR lpszFileName, const CFileStatus& status, CAtlTransactionManager* pTM = NULL);

	ULONGLONG SeekToEnd();
	void SeekToBegin();

// Overridables
	virtual CFile* Duplicate() const;

	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual ULONGLONG GetLength() const;

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);

	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);

	virtual void Abort();
	virtual void Flush();
	virtual void Close();

// Implementation
public:
	virtual ~CFile();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	enum BufferCommand { bufferRead, bufferWrite, bufferCommit, bufferCheck };
	enum BufferFlags 
	{ 
		bufferDirect = 0x01,
		bufferBlocking = 0x02
	};
	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
		void** ppBufStart = NULL, void** ppBufMax = NULL);

protected:
	void CommonBaseInit(HANDLE hFile, CAtlTransactionManager* pTM);
	void CommonInit(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM);

	BOOL m_bCloseOnDelete;
	CString m_strFileName;
	
	/// <summary>
	/// Pointer to CAtlTransactionManager object</summary>
	CAtlTransactionManager* m_pTM;
};

/*============================================================================*/
// STDIO file implementation

class CStdioFile : public CFile
{
	DECLARE_DYNAMIC(CStdioFile)

public:
// Constructors
	CStdioFile();

	/// <summary>
	/// CStdioFile constructor</summary>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	CStdioFile(CAtlTransactionManager* pTM);

	CStdioFile(FILE* pOpenStream);
	CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	
	/// <summary>
	/// CStdioFile constructor</summary>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="nOpenFlags">Sharing and access mode. Specifies the action to take when opening the file. You can combine options listed below by using the bitwise-OR (|) operator. One access permission and one share option are required; the modeCreate and modeNoInherit modes are optional.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	CStdioFile(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM);

// Attributes
	FILE* m_pStream;    // stdio FILE
						// m_hFile from base class is _fileno(m_pStream)

// Operations
	// reading and writing strings
	virtual void WriteString(LPCTSTR lpsz);
	virtual LPTSTR ReadString(_Out_writes_z_(nMax) LPTSTR lpsz, _In_ UINT nMax);
	virtual BOOL ReadString(CString& rString);

// Implementation
public:
	virtual ~CStdioFile();
#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
	virtual ULONGLONG GetPosition() const;
	virtual ULONGLONG GetLength() const;
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL);

	/// <summary>
	/// Open is designed for use with the default CStdioFile constructor</summary>
	/// <returns> 
	/// TRUE if succeeds; otherwise FALSE.</returns>
	/// <param name="lpszFileName">A string that is the path to the desired file. The path can be relative or absolute.</param>
	/// <param name="nOpenFlags">Sharing and access mode. Specifies the action to take when opening the file. You can combine options listed below by using the bitwise-OR (|) operator. One access permission and one share option are required; the modeCreate and modeNoInherit modes are optional.</param>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	/// <param name="pError">A pointer to an existing file-exception object that will receive the status of a failed operation</param>
	virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM, CFileException* pError);

	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();

	// Unsupported APIs
	virtual CFile* Duplicate() const;
	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);

protected:
	void CommonBaseInit(FILE* pOpenStream, CAtlTransactionManager* pTM);
	void CommonInit(LPCTSTR lpszFileName, UINT nOpenFlags, CAtlTransactionManager* pTM);
};

/*============================================================================*/
// Memory based file implementation

class CMemFile : public CFile
{
	DECLARE_DYNAMIC(CMemFile)

public:
// Constructors
	explicit CMemFile(UINT nGrowBytes = 1024);
	CMemFile(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);

// Operations
	void Attach(BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0);
	BYTE* Detach();

// Advanced Overridables
protected:
	virtual BYTE* Alloc(SIZE_T nBytes);
	virtual BYTE* Realloc(BYTE* lpMem, SIZE_T nBytes);
	virtual BYTE* Memcpy(BYTE* lpMemTarget, const BYTE* lpMemSource, SIZE_T nBytes);
	virtual void Free(BYTE* lpMem);
	virtual void GrowFile(SIZE_T dwNewLen);

// Implementation
protected:
	SIZE_T m_nGrowBytes;
	SIZE_T m_nPosition;
	SIZE_T m_nBufferSize;
	SIZE_T m_nFileSize;
	BYTE* m_lpBuffer;
	BOOL m_bAutoDelete;

public:
	virtual ~CMemFile();
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
	virtual void AssertValid() const;
#endif
	virtual ULONGLONG GetPosition() const;
	BOOL GetStatus(CFileStatus& rStatus) const;
	virtual ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	virtual void SetLength(ULONGLONG dwNewLen);
	virtual UINT Read(void* lpBuf, UINT nCount);
	virtual void Write(const void* lpBuf, UINT nCount);
	virtual void Abort();
	virtual void Flush();
	virtual void Close();
	virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
		void** ppBufStart = NULL, void** ppBufMax = NULL);
	virtual ULONGLONG GetLength() const;

	// Unsupported APIs
	virtual CFile* Duplicate() const;
	virtual void LockRange(ULONGLONG dwPos, ULONGLONG dwCount);
	virtual void UnlockRange(ULONGLONG dwPos, ULONGLONG dwCount);
};

/*============================================================================*/
// Local file searches

#include <atltime.h>
using ATL::CTime;
using ATL::CTimeSpan;

class CFileFind : public CObject
{
public:
	CFileFind();

	/// <summary>
	/// CFileFind constructor</summary>
	/// <param name="pTM">Pointer to CAtlTransactionManager object</param>
	CFileFind(CAtlTransactionManager* pTM);
	virtual ~CFileFind();

// Attributes
public:
	ULONGLONG GetLength() const;
	virtual CString GetFileName() const;
	virtual CString GetFilePath() const;
	virtual CString GetFileTitle() const;
	virtual CString GetFileURL() const;
	virtual CString GetRoot() const;

	virtual BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetLastAccessTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetCreationTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetLastWriteTime(CTime& refTime) const;
	virtual BOOL GetLastAccessTime(CTime& refTime) const;
	virtual BOOL GetCreationTime(CTime& refTime) const;

	virtual BOOL MatchesMask(DWORD dwMask) const;

	virtual BOOL IsDots() const;
	// these aren't virtual because they all use MatchesMask(), which is
	BOOL IsReadOnly() const;
	BOOL IsDirectory() const;
	BOOL IsCompressed() const;
	BOOL IsSystem() const;
	BOOL IsHidden() const;
	BOOL IsTemporary() const;
	BOOL IsNormal() const;
	BOOL IsArchived() const;

// Operations
	void Close();
	virtual BOOL FindFile(LPCTSTR pstrName = NULL, DWORD dwUnused = 0);
	virtual BOOL FindNextFile();

protected:
	virtual void CloseContext();

// Implementation
protected:
	void* m_pFoundInfo;
	void* m_pNextInfo;
	HANDLE m_hContext;
	CString m_strRoot;
	TCHAR m_chDirSeparator;     // not '\\' for Internet classes

	/// <summary>
	/// Pointer to CAtlTransactionManager object</summary>
	CAtlTransactionManager* m_pTM;

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
	void AssertValid() const;
#endif

	DECLARE_DYNAMIC(CFileFind)
};

// CTimeSpan diagnostics and serialization
#ifdef _DEBUG
CDumpContext& AFXAPI operator<<(CDumpContext& dc,CTimeSpan dateSpanSrc);
#endif
CArchive& AFXAPI operator<<(CArchive& ar, CTimeSpan dateSpanSrc);
CArchive& AFXAPI operator>>(CArchive& ar, CTimeSpan& dateSpanSrc);

// CTime diagnostics and serialization
#ifdef _DEBUG
CDumpContext& AFXAPI operator<<(CDumpContext& dc, CTime dateSrc);
#endif
CArchive& AFXAPI operator<<(CArchive& ar, CTime dateSrc);
CArchive& AFXAPI operator>>(CArchive& ar, CTime& dateSrc);

/*============================================================================*/
// File status

struct CFileStatus
{
	CTime m_ctime;          // creation date/time of file
	CTime m_mtime;          // last modification date/time of file
	CTime m_atime;          // last access date/time of file
	ULONGLONG m_size;       // logical size of file in bytes
	DWORD m_attribute;      // logical OR of CFile::Attribute enum values
	TCHAR m_szFullName[_MAX_PATH]; // absolute path name

#ifdef _DEBUG
	void Dump(CDumpContext& dc) const;
#endif
};

/*============================================================================*/
// Diagnostic memory management routines

// Low level sanity checks for memory blocks
BOOL AFXAPI AfxIsValidAddress(const void* lp,
			UINT_PTR nBytes, BOOL bReadWrite = TRUE);
BOOL AFXAPI AfxIsValidString(LPCWSTR lpsz, int nLength = -1);
BOOL AFXAPI AfxIsValidString(LPCSTR lpsz, int nLength = -1);

// Sanity checks for ATOMs
BOOL AfxIsValidAtom(ATOM nAtom);
BOOL AfxIsValidAtom(LPCTSTR psz);

#if defined(_DEBUG)

#if _MSC_VER >= 1900 && !defined(__EDG__)
#define _AFX_DECLSPEC_ALLOCATOR __declspec(allocator)
#else
#define _AFX_DECLSPEC_ALLOCATOR
#endif

// Memory tracking allocation
void* AFX_CDECL operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
#define DEBUG_NEW new(THIS_FILE, __LINE__)
void AFX_CDECL operator delete(void* p, LPCSTR lpszFileName, int nLine);

_Ret_notnull_ _Post_writable_byte_size_(_Size) void * __cdecl operator new[](size_t _Size);
void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine);
void __cdecl operator delete[](void *);

_AFX_DECLSPEC_ALLOCATOR void* AFXAPI AfxAllocMemoryDebug(size_t nSize, BOOL bIsObject,
	LPCSTR lpszFileName, int nLine);
void AFXAPI AfxFreeMemoryDebug(void* pbData, BOOL bIsObject);

// Dump any memory leaks since program started
BOOL AFXAPI AfxDumpMemoryLeaks();

// Return TRUE if valid memory block of nBytes
BOOL AFXAPI AfxIsMemoryBlock(const void* p, UINT nBytes,
	LONG* plRequestNumber = NULL);

// Return TRUE if memory is sane or print out what is wrong
BOOL AFXAPI AfxCheckMemory();

#define afxMemDF _crtDbgFlag

enum AfxMemDF // memory debug/diagnostic flags
{
	allocMemDF          = _CRTDBG_ALLOC_MEM_DF,         // turn on debugging allocator
	delayFreeMemDF      = _CRTDBG_DELAY_FREE_MEM_DF,         // delay freeing memory
	checkAlwaysMemDF    = _CRTDBG_CHECK_ALWAYS_DF,          // AfxCheckMemory on every alloc/free
	checkEvery16MemDF	= _CRTDBG_CHECK_EVERY_16_DF,
	checkEvery128MemDF	= _CRTDBG_CHECK_EVERY_128_DF,
	checkEvery1024MemDF	= _CRTDBG_CHECK_EVERY_1024_DF,
	checkDefaultMemDF	= _CRTDBG_CHECK_DEFAULT_DF
};

#define AfxOutputDebugString TRACE

// turn on/off tracking for a short while
BOOL AFXAPI AfxEnableMemoryTracking(BOOL bTrack);

// turn on/off memory leak dump in _AFX_DEBUG_STATE destructor
BOOL AFXAPI AfxEnableMemoryLeakDump(BOOL bDump);

// Turn on/off the global flag _afxMemoryLeakOverride. if bEnable is TRUE
// then further calls to AfxEnableMemoryTracking() wont change the current
// memory tracking state, until AfxEnableMemoryLeakOverride(BOOL bEnable)
// is called again with bEnable == FALSE.
BOOL AFXAPI AfxEnableMemoryLeakOverride(BOOL bEnable);

// Advanced initialization: for overriding default diagnostics
BOOL AFXAPI AfxDiagnosticInit(void);

// A failure hook returns whether to permit allocation
typedef BOOL (AFXAPI* AFX_ALLOC_HOOK)(size_t nSize, BOOL bObject, LONG lRequestNumber);

// Set new hook, return old (never NULL)
AFX_ALLOC_HOOK AFXAPI AfxSetAllocHook(AFX_ALLOC_HOOK pfnAllocHook);

// Debugger hook on specified allocation request - Obsolete
void AFXAPI AfxSetAllocStop(LONG lRequestNumber);

// Memory state for snapshots/leak detection
struct CMemoryState
{
// Attributes
	enum blockUsage
	{
		freeBlock,    // memory not used
		objectBlock,  // contains a CObject derived class object
		bitBlock,     // contains ::operator new data
		crtBlock,
		ignoredBlock,
		nBlockUseMax  // total number of usages
	};

	_CrtMemState m_memState;
	LONG_PTR m_lCounts[nBlockUseMax];
	LONG_PTR m_lSizes[nBlockUseMax];
	LONG_PTR m_lHighWaterCount;
	LONG_PTR m_lTotalCount;

	CMemoryState();

// Operations
	void Checkpoint();  // fill with current state
	BOOL Difference(const CMemoryState& oldState,
					const CMemoryState& newState);  // fill with difference
	void UpdateData();

	// Output to afxDump
	void DumpStatistics() const;
	void DumpAllObjectsSince() const;
};

// Enumerate allocated objects or runtime classes
void AFXAPI AfxDoForAllObjects(void (AFX_CDECL *pfn)(CObject* pObject, void* pContext),
	void* pContext);
void AFXAPI AfxDoForAllClasses(void (AFX_CDECL *pfn)(const CRuntimeClass* pClass,
	void* pContext), void* pContext);

#else

// non-_DEBUG_ALLOC version that assume everything is OK
#define DEBUG_NEW new
#define AfxCheckMemory() TRUE
#define AfxIsMemoryBlock(p, nBytes) TRUE
#define AfxEnableMemoryTracking(bTrack) FALSE
#define AfxEnableMemoryLeakOverride(bEnable) TRUE
#define AfxOutputDebugString(lpsz) ::OutputDebugString(lpsz)

// diagnostic initialization
#ifndef _DEBUG
#define AfxDiagnosticInit() TRUE
#else
BOOL AFXAPI AfxDiagnosticInit(void);
#endif

#endif // _DEBUG

/*============================================================================*/
// Archives for serializing CObject data

// needed for implementation
template<class TYPE, class ARG_TYPE>
class CArray;
class CPtrArray;
class CMapPtrToPtr;
class CDocument;

class CArchive
{
protected:
	enum SchemaMapReservedRefs { objTypeArrayRef = 1 };
	enum LoadArrayObjType{ typeUndefined = 0, typeCRuntimeClass = 1, typeCObject = 2 };	
public:
// Flag values
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };

	CArchive(CFile* pFile, UINT nMode, int nBufSize = 4096, void* lpBuf = NULL);
	~CArchive();

// Attributes
	BOOL IsLoading() const;
	BOOL IsStoring() const;
	BOOL IsByteSwapping() const;
	BOOL IsBufferEmpty() const;

	CFile* GetFile() const;
	UINT GetObjectSchema(); // only valid when reading a CObject*
	void SetObjectSchema(UINT nSchema);

	// pointer to document being serialized -- must set to serialize
	//  COleClientItems in a document!
	CDocument* m_pDocument;

// Operations
	UINT Read(void* lpBuf, UINT nMax);
	void EnsureRead(void *lpBuf, UINT nCount);
	void Write(const void* lpBuf, UINT nMax);
	void Flush();
	void Close();
	void Abort();   // close and shutdown without exceptions

	// reading and writing strings
	void WriteString(LPCTSTR lpsz);
	LPTSTR ReadString(_Out_writes_z_(nMax+1) LPTSTR lpsz, _In_ UINT nMax);
	BOOL ReadString(CString& rString);

public:
	// Object I/O is pointer based to avoid added construction overhead.
	// Use the Serialize member function directly for embedded objects.
	friend CArchive& AFXAPI operator<<(CArchive& ar, const CObject* pOb);

	friend CArchive& AFXAPI operator>>(CArchive& ar, CObject*& pOb);
	friend CArchive& AFXAPI operator>>(CArchive& ar, const CObject*& pOb);

	// insertion operations
	CArchive& operator<<(BYTE by);
	CArchive& operator<<(WORD w);
	CArchive& operator<<(LONG l);
	CArchive& operator<<(DWORD dw);
	CArchive& operator<<(float f);
	CArchive& operator<<(double d);
	CArchive& operator<<(LONGLONG dwdw);
	CArchive& operator<<(ULONGLONG dwdw);

	CArchive& operator<<(int i);
	CArchive& operator<<(short w);
	CArchive& operator<<(char ch);
#ifdef _NATIVE_WCHAR_T_DEFINED
	CArchive& operator<<(wchar_t ch);
#endif
	CArchive& operator<<(unsigned u);

	template < typename BaseType , bool t_bMFCDLL>
	CArchive& operator<<(const ATL::CSimpleStringT<BaseType, t_bMFCDLL>& str);

	template< typename BaseType, class StringTraits >	
	CArchive& operator<<(const ATL::CStringT<BaseType, StringTraits>& str);
	
	template < typename BaseType , bool t_bMFCDLL>
	CArchive& operator>>(ATL::CSimpleStringT<BaseType, t_bMFCDLL>& str);

	template< typename BaseType, class StringTraits >
	CArchive& operator>>(ATL::CStringT<BaseType, StringTraits>& str);

	CArchive& operator<<(bool b);

	// extraction operations
	CArchive& operator>>(BYTE& by);
	CArchive& operator>>(WORD& w);
	CArchive& operator>>(DWORD& dw);
	CArchive& operator>>(LONG& l);
	CArchive& operator>>(float& f);
	CArchive& operator>>(double& d);
	CArchive& operator>>(LONGLONG& dwdw);
	CArchive& operator>>(ULONGLONG& dwdw);

	CArchive& operator>>(int& i);
	CArchive& operator>>(short& w);
	CArchive& operator>>(char& ch);
#ifdef _NATIVE_WCHAR_T_DEFINED
	CArchive& operator>>(wchar_t& ch);
#endif
	CArchive& operator>>(unsigned& u);
	CArchive& operator>>(bool& b);

	// object read/write
	CObject* ReadObject(const CRuntimeClass* pClass);
	void WriteObject(const CObject* pOb);
	// advanced object mapping (used for forced references)
	void MapObject(const CObject* pOb);

	// advanced versioning support
	void WriteClass(const CRuntimeClass* pClassRef);
	CRuntimeClass* ReadClass(const CRuntimeClass* pClassRefRequested = NULL,
		UINT* pSchema = NULL, DWORD* pObTag = NULL);
	void SerializeClass(const CRuntimeClass* pClassRef);

	// advanced operations (used when storing/loading many objects)
	void SetStoreParams(UINT nHashSize = 2053, UINT nBlockSize = 128);
	void SetLoadParams(UINT nGrowBy = 1024);

	void EnsureSchemaMapExists(CArray<LoadArrayObjType, const LoadArrayObjType&>** ppObjTypeArray = NULL);
// Implementation
public:
	BOOL m_bForceFlat;  // for COleClientItem implementation (default TRUE)
	BOOL m_bDirectBuffer;   // TRUE if m_pFile supports direct buffering
	BOOL m_bBlocking;  // TRUE if m_pFile can block for unbounded periods of time
	void FillBuffer(UINT nAdditionalBytesNeeded);
	void CheckCount();  // throw exception if m_nMapCount is too large

	// special functions for reading and writing (16-bit compatible) counts
	DWORD_PTR ReadCount();
	void WriteCount(DWORD_PTR dwCount);

	// public for advanced use
	UINT m_nObjectSchema;
	CString m_strFileName;

protected:
	// archive objects cannot be copied or assigned
	CArchive(const CArchive& arSrc);
	void operator=(const CArchive& arSrc);

	BOOL m_nMode;
	BOOL m_bUserBuf;
	int m_nBufSize;
	CFile* m_pFile;
	BYTE* m_lpBufCur;
	BYTE* m_lpBufMax;
	BYTE* m_lpBufStart;

	// array/map for CObject* and CRuntimeClass* load/store
	UINT m_nMapCount;
	union
	{
		CPtrArray* m_pLoadArray;
		CMapPtrToPtr* m_pStoreMap;
	};
	// map to keep track of mismatched schemas
	CMapPtrToPtr* m_pSchemaMap;

	// advanced parameters (controls performance with large archives)
	UINT m_nGrowSize;
	UINT m_nHashSize;
};

/*============================================================================*/
// Diagnostic dumping

// Note: AfxDumpStack is available in release builds, although it is always
//      statically linked so as to not negatively affect the size of MFCXX.DLL.

#define AFX_STACK_DUMP_TARGET_TRACE                     0x0001
#define AFX_STACK_DUMP_TARGET_CLIPBOARD 0x0002
#define AFX_STACK_DUMP_TARGET_BOTH                      0x0003
#define AFX_STACK_DUMP_TARGET_ODS                       0x0004
#ifdef _DEBUG
#define AFX_STACK_DUMP_TARGET_DEFAULT           AFX_STACK_DUMP_TARGET_TRACE
#else
#define AFX_STACK_DUMP_TARGET_DEFAULT           AFX_STACK_DUMP_TARGET_CLIPBOARD
#endif

void AFXAPI AfxDumpStack(DWORD dwFlags = AFX_STACK_DUMP_TARGET_DEFAULT);

class CDumpContext
{
public:
	CDumpContext(CFile* pFile = NULL);

// Attributes
	int GetDepth() const;      // 0 => this object, 1 => children objects
	void SetDepth(int nNewDepth);

// Operations
	CDumpContext& operator<<(LPCTSTR lpsz);
#ifdef _UNICODE
	CDumpContext& operator<<(LPCSTR lpsz);  // automatically widened
#else
	CDumpContext& operator<<(LPCWSTR lpsz); // automatically thinned
#endif
	template< typename BaseType, class StringTraits >
	CDumpContext& operator<<(const ATL::CStringT<BaseType, 
		StringTraits>& str)
	{
		*this << static_cast< const BaseType* >( str );
		return *this;
	}
	CDumpContext& operator<<(const void* lp);
	CDumpContext& operator<<(const CObject* pOb);
	CDumpContext& operator<<(const CObject& ob);
	CDumpContext& operator<<(BYTE by);
	CDumpContext& operator<<(WORD w);
	CDumpContext& DumpAsHex(BYTE b);
	CDumpContext& DumpAsHex(WORD w);
#ifdef _WIN64
	CDumpContext& operator<<(LONG l);
	CDumpContext& operator<<(DWORD dw);
	CDumpContext& operator<<(int n);
	CDumpContext& operator<<(UINT u);
	CDumpContext& DumpAsHex(LONG l);
	CDumpContext& DumpAsHex(DWORD dw);
	CDumpContext& DumpAsHex(int n);
	CDumpContext& DumpAsHex(UINT u);
#else
	CDumpContext& operator<<(LONG_PTR l);
	CDumpContext& operator<<(DWORD_PTR dw);
	CDumpContext& operator<<(INT_PTR n);
	CDumpContext& operator<<(UINT_PTR u);
	CDumpContext& DumpAsHex(LONG_PTR l);
	CDumpContext& DumpAsHex(DWORD_PTR dw);
	CDumpContext& DumpAsHex(INT_PTR n);
	CDumpContext& DumpAsHex(UINT_PTR u);
#endif
	CDumpContext& operator<<(float f);
	CDumpContext& operator<<(double d);
	CDumpContext& operator<<(LONGLONG n);
	CDumpContext& operator<<(ULONGLONG n);
	CDumpContext& DumpAsHex(LONGLONG n);
	CDumpContext& DumpAsHex(ULONGLONG n);
	CDumpContext& operator<<(HWND h);
	CDumpContext& operator<<(HDC h);
	CDumpContext& operator<<(HMENU h);
	CDumpContext& operator<<(HACCEL h);
	CDumpContext& operator<<(HFONT h);
	void HexDump(LPCTSTR lpszLine, BYTE* pby, int nBytes, int nWidth);
	void Flush();

// Implementation
protected:
	// dump context objects cannot be copied or assigned
	CDumpContext(const CDumpContext& dcSrc);
	void operator=(const CDumpContext& dcSrc);
	void OutputString(LPCTSTR lpsz);

	int m_nDepth;

public:
	CFile* m_pFile;
};

/////////////////////////////////////////////////////////////////////////////
int __cdecl _AfxInitManaged();

#ifdef _DEBUG
extern AFX_DATA CDumpContext afxDump;
extern AFX_DATA BOOL afxTraceEnabled;
#endif

#ifdef _DEBUG
#define AFXDUMP( exp ) (void)(afxDump<<exp)
#else
#define AFXDUMP( exp )
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifndef __AFXCOLL_H__
	#include <afxcoll.h>
	#ifndef __AFXSTATE_H__
		#include <afxstat_.h> // for _AFX_APP_STATE and _AFX_THREAD_STATE
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_ENABLE_INLINES
#define _AFX_INLINE AFX_INLINE

#if !defined(_AFX_CORE_IMPL) || !defined(_AFXDLL) || defined(_DEBUG)
#define _AFX_PUBLIC_INLINE AFX_INLINE
#else
#define _AFX_PUBLIC_INLINE
#endif

#endif

#include <afx.inl>


#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_ALL_WARNINGS
#pragma warning( pop )
#endif

#endif // __AFX_H__

/////////////////////////////////////////////////////////////////////////////
