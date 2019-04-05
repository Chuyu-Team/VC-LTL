/***
*ehdata.h -
*
*	Copyright (c) 1993-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*	Declare misc. types, macros, etc. for implementation
*	of C++ Exception Handling for the run-time and the compiler.
*	Hardware independent, assumes Windows NT.
*
* Portions of this header file can be disabled by defining the following
* macros:
*	_EHDATA_NOHEADERS - suppresses inclusion of standard header files
*		If this is specified, then appropriate typedefs or macros must
*		be provided by some other means.
*	_EHDATA_NOTHROW - suppresses definitions used only to describe a throw
*	_EHDATA_NOFUNCINFO - suppresses definitions for the frame descriptor
*	_EHDATA_NONT - suppresses definitions of our version of NT's stuff
*
* Other conditional compilation macros:
*    CC_EXPLICITFRAME - if true, representation of registration node includes
*	the value of the frame-pointer for that frame, making the location
*	of the registration node on the frame flexible.  This is intended
*	primarily for early testing.
*
*       [Internal]
*
*Revision History:
*       05-20-93  BS	Module Created.
*       03-01-94  CFW	Remove CONTEXT def for x86 for TiborL.
*       03-03-94  TL	Mips (_M_MRX000 >= 4000) changes
*       09-02-94  SKS	This header file added.
*       09-12-94  GJF	Merged in changes from/for DEC (Al Doser, dated 6/20,
*                       and Bill Baxter, dated 6/28).
*       11-06-94  GJF	Changed pack pragma to 8 byte alignment.
*       02-14-95  CFW   Clean up Mac merge.
*       03-22-95  PML   Add const for read-only structs
*       03-29-95  CFW   Add error message to internal headers.
*       04-14-95  JWM	Added EH_ABORT_FRAME_UNWIND_PART for EH/SEH exception handling.
*       04-20-95  TGL	Added iFrameNestLevel field to MIPS FuncInfo
*       04-27-95  JWM	EH_ABORT_FRAME_UNWIND_PART now #ifdef ALLOW_UNWIND_ABORT.
*       06-08-95  JWM	Merged CRT version of ehdata.h into langapi source.
*       01-25-00  GB	Declared _CxxThrowException __declspec(noreturn)
*       02-08-00  GB	Added HT_ISCOMPLUSEH
*       03-27-00  PML	Remove CC_P7_SOFT25, which is now on permanently.
*       09-20-00  GB	Add __cdecl to URT C++ EH support routines (vs7#89576)
*       09-18-01  GB	Added Support for ExceptionSpecification. Changes mainly made by Arturl.
*       09-20-01  PML   Buildfix - can't rename EH_MAGIC_NUMBER1, call the new
*                       code EH_MAGIC_NUMBER2 instead.
*       11-14-02  GB    Added RtlUnwindEx of IA64
*       01-31-03  GB    Added support for Pure C++ catch(...) which does not catch seh.
*       02-28-03  SSM   Remove _RUNTIME_FUNCTION for AMD64 & IA64. Now exposed in Winnt.h
*       06-09-04  GB    Fixed the pure C++ excpetion handling for 64 bit platforms.
*       06-29-04  SJ    HandlerType changed to be a ptr on x86 & a 32 bit int on 64 bit
*                       platforms. Corresponding macros also fixed.
*       01-26-05  SJ    Fix longjmp behavior VSW#411664
*       03-19-05  PML   Throw from pure region can't be caught by native/mixed
*                       and vice-versa (VSW#422533)
*
****/

#ifndef _INC_EHDATA
#define _INC_EHDATA

#ifdef ONLY_VALUES
#define _EHDATA_NOHEADERS
#endif

#ifndef _CRTBLD
#ifndef _VC_VER_INC
//#include "vcver.h"
#endif
#endif /* _CRTBLD */


#if defined(_M_IX86) && _M_IX86 >= 300 /*IFSTRIP=IGN*/
# ifndef CC_EXPLICITFRAME
#  define CC_EXPLICITFRAME	0	// If non-zero, we're using a hack version of the
								// registration node.
# endif
#endif

#if defined(_M_CEE_PURE) || defined(BUILDING_C1XX_FORCEINCLUDE)
#define _EH_RELATIVE_TYPEINFO 0
#define _EH_RELATIVE_FUNCINFO 0
#define _RTTI_RELATIVE_TYPEINFO 0
#elif defined(_CHPE_X86_ARM64_EH_)
#define _EH_RELATIVE_TYPEINFO 0
#define _EH_RELATIVE_FUNCINFO 1
#define _RTTI_RELATIVE_TYPEINFO 0
#elif defined(_M_ARM)
#define _EH_RELATIVE_TYPEINFO 1
#define _EH_RELATIVE_FUNCINFO 1
#define _RTTI_RELATIVE_TYPEINFO 0
#elif defined(_M_AMD64) || defined(_M_ARM64)
#define _EH_RELATIVE_TYPEINFO 1
#define _EH_RELATIVE_FUNCINFO 1
#define _RTTI_RELATIVE_TYPEINFO 1
#else
#define _EH_RELATIVE_TYPEINFO 0
#define _EH_RELATIVE_FUNCINFO 0
#define _RTTI_RELATIVE_TYPEINFO 0
#endif

#define _EH_RELATIVE_OFFSETS _EH_RELATIVE_TYPEINFO

#if defined(_WIN64)
#define _EH_PTR64 __ptr64
#else
#define _EH_PTR64
#endif

#ifndef _EHDATA_NOHEADERS
#include <stddef.h>
#include <excpt.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <nowin.h>
#endif
#endif /* _EHDATA_NOHEADERS */

#pragma pack(push, ehdata, 4)

#define EH_EXCEPTION_NUMBER	('msc' | 0xE0000000)	// The NT Exception # that we use

// As magic numbers increase, we have to keep track of the versions that we are
// backwards compatible with.  The top 3 bits of the magic number DWORD are
// used for other purposes, so while the magic number started as a YYYYMMDD
// date, it can't represent the year 2000 or later.  Old CRTs also recognize
// unknown magic numbers with a >= test.  Therefore, we just increment the
// the magic number by one every time we need another.
//
// EH_MAGIC_NUMBER1     The original, still used in exception records for
//                      native or mixed C++ thrown objects.
// EH_MAGIC_NUMBER2     Used in the FuncInfo if exception specifications are
//                      supported and FuncInfo::pESTypeList is valid.
// EH_MAGIC_NUMBER3     Used in the FuncInfo if FuncInfo::EHFlags is valid, so
//                      we can check if the function was compiled -EHs or -EHa.

#define EH_MAGIC_NUMBER1        0x19930520
#define EH_MAGIC_NUMBER2        0x19930521
#define EH_MAGIC_NUMBER3        0x19930522

// We use a different magic number in the thrown object's exception record to
// indicate it arose from a pure region.  Pure throws can't be caught by native
// or mixed code, and vice-versa.  Note that the pure magic number is only 7
// digits, not 8, so it will be less than any of the native EH magic numbers
// and won't be detected as an unknown newer magic number by the native EH
// handlers.

#define EH_PURE_MAGIC_NUMBER1   0x01994000

#define FI_EHS_FLAG             0x00000001
#define FI_DYNSTKALIGN_FLAG     0x00000002

#if (EH_MAGIC_NUMBER2 <= EH_MAGIC_NUMBER1) || (EH_MAGIC_NUMBER3 <= EH_MAGIC_NUMBER2)
#error new magic number must be greater than the old one
#endif

#if (EH_MAGIC_NUMBER1 & 0xE0000000) || (EH_MAGIC_NUMBER2 & 0xE0000000) || (EH_MAGIC_NUMBER3 & 0xE0000000)
#error magic number too big -- must fit into 29 bits
#endif

#if (EH_PURE_MAGIC_NUMBER1 >= EH_MAGIC_NUMBER1)
#error pure EH magic number must be less than native one
#endif

#define EH_MAGIC_HAS_ES	EH_MAGIC_NUMBER2	// Magic number is greater or equal than that 
											// indicates presence of exception specification

#if _EH_RELATIVE_OFFSETS
#define EH_EXCEPTION_PARAMETERS 4			// Number of parameters in exception record
#else
#define EH_EXCEPTION_PARAMETERS 3			// Number of parameters in exception record
#endif

#ifdef ALLOW_UNWIND_ABORT
#define EH_ABORT_FRAME_UNWIND_PART EH_EXCEPTION_NUMBER+1
#endif

#define EH_EMPTY_STATE	-1

#ifndef ONLY_VALUES

//
// PMD - Pointer to Member Data: generalized pointer-to-member descriptor
//

typedef struct PMD
{
#if defined(_WIN64)	 /*IFSTRIP=IGN*/
	__int32		mdisp;		// Offset of intended data within base
	__int32		pdisp;		// Displacement to virtual base pointer
	__int32		vdisp;		// Index within vbTable to offset of base
#else
	ptrdiff_t	mdisp;		// Offset of intended data within base
	ptrdiff_t	pdisp;		// Displacement to virtual base pointer
	ptrdiff_t	vdisp;		// Index within vbTable to offset of base
#endif
	} PMD;

//
// PMFN - Pointer to Member Function
//			M00REVIEW: we may need something more than this, but this will do for now.
//

#ifndef WANT_NO_TYPES
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)/*IFSTRIP=IGN*/
typedef	__int32	PMFN;					// Image relative offset of Member Function
#else
typedef void (* _EH_PTR64 PMFN)(void* _EH_PTR64);
#endif
#endif // WANT_NO_TYPES

typedef void* (__stdcall *PGETWINRT_OOM_EXCEPTION)();

//
// TypeDescriptor - per-type record which uniquely identifies the type.
//
// Each type has a decorated name which uniquely identifies it, and a hash
// value which is computed by the compiler.  The hash function used is not
// important; the only thing which is essential is that it be the same for
// all time.
//
// The special type '...' (ellipsis) is represented by a null name.
//
#pragma warning(disable:4200)		// get rid of obnoxious nonstandard extension warning

#if defined(_M_ALPHA64) || defined(_M_IA64) || defined(VERSP_IA64) || defined(_M_AMD64) || defined(_M_ARM64)
#pragma pack(push, TypeDescriptor, 8)
#endif

#ifndef WANT_NO_TYPES
typedef struct TypeDescriptor
{
#if defined(_WIN64) || defined(_RTTI) /*IFSTRIP=IGN*/
	const void * _EH_PTR64	pVFTable;	// Field overloaded by RTTI
#else
	DWORD	hash;			// Hash value computed from type's decorated name
#endif
	void * _EH_PTR64			spare;		// reserved, possible for RTTI
	char	name[];			// The decorated name of the type; 0 terminated.
	} TypeDescriptor;
#endif // WANT_NO_TYPES

#if defined(_M_ALPHA64) || defined(_M_IA64) || defined(VERSP_IA64) || defined(_M_AMD64)
#pragma pack(pop, TypeDescriptor)
#endif
#pragma warning(default:4200)

#define TD_HASH(td)		((td).hash)
#define TD_NAME(td)		((td).name)

#define TD_IS_TYPE_ELLIPSIS(td) ((td == NULL) || (TD_NAME(*td)[0] == '\0'))


#ifndef _EHDATA_NOTHROW

/////////////////////////////////////////////////////////////////////////////
//
// Description of the thrown object.  (M00REVIEW: not final)
//
// This information is broken down into three levels, to allow for maximum
// comdat folding (at the cost of some extra pointers).
//
// ThrowInfo is the head of the description, and contains information about
// 				the particular variant thrown.
// CatchableTypeArray is an array of pointers to type descriptors.  It will
//				be shared between objects thrown by reference but with varying
//				qualifiers.
// CatchableType is the description of an individual type, and how to effect
//				the conversion from a given type.
//
//---------------------------------------------------------------------------


//
// CatchableType - description of a type that can be caught.
//
// Note:  although isSimpleType can be part of ThrowInfo, it is more
//		  convenient for the run-time to have it here.
//

#ifndef WANT_NO_TYPES
typedef const struct _s_CatchableType {
	unsigned int	properties;				// Catchable Type properties (Bit field)
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			pType;					// Image relative offset of TypeDescriptor
#else
	TypeDescriptor * _EH_PTR64 pType;		// Pointer to the type descriptor for this type
#endif
	PMD 			thisDisplacement;		// Pointer to instance of catch type within
											//		thrown object.
	int				sizeOrOffset;			// Size of simple-type object or offset into
											//  buffer of 'this' pointer for catch object
	PMFN			copyFunction;			// Copy constructor or CC-closure
} CatchableType;
#endif // WANT_NO_TYPES

#define CT_IsSimpleType			0x00000001		// type is a simple type
#define CT_ByReferenceOnly		0x00000002		// type must be caught by reference
#define CT_HasVirtualBase		0x00000004		// type is a class with virtual bases
#define CT_IsWinRTHandle		0x00000008		// type is a winrt handle
#define CT_IsStdBadAlloc		0x00000010		// type is a a std::bad_alloc

#define CT_PROPERTIES(ct)	((ct).properties)
#if _EH_RELATIVE_OFFSETS && !defined(_M_CEE_PURE)
#define CT_PTD_IB(ct,ib)		((TypeDescriptor *)((ib) + (ct).pType))
#define CT_COPYFUNC_IB(ct,ib)	((void (* _EH_PTR64)(void* _EH_PTR64))((ib) + (ct).copyFunction))
#define CT_NAME_IB(ct,ib)		(TD_NAME(*CT_PTD_IB(ct,ib)))
#else
#define CT_PTD(ct)			((ct).pType)
#define CT_COPYFUNC(ct)		((ct).copyFunction)
#endif
#define CT_THISDISP(ct)		((ct).thisDisplacement)
#define CT_SIZE(ct)			((ct).sizeOrOffset)
#define CT_OFFSET(ct)		((ct).sizeOrOffset)
#define CT_HASH(ct)			(TD_HASH(*CT_PTD(ct)))
#define CT_NAME(ct)			(TD_NAME(*CT_PTD(ct)))

#define SET_CT_ISSIMPLETYPE(ct)		(CT_PROPERTIES(ct) |= CT_IsSimpleType)
#define SET_CT_BYREFONLY(ct)		(CT_PROPERTIES(ct) |= CT_ByReferenceOnly)
#define SET_CT_HASVB(ct)			(CT_PROPERTIES(ct) |= CT_HasVirtualBase)
#define SET_CT_ISWINRTHANDLE(ct)	(CT_PROPERTIES(ct) |= CT_IsWinRTHandle)
#define SET_CT_ISSTDBADALLOC(ct)	(CT_PROPERTIES(ct) |= CT_IsStdBadAlloc)

#define CT_ISSIMPLETYPE(ct)			(CT_PROPERTIES(ct) & CT_IsSimpleType)		// Is it a simple type?
#define CT_BYREFONLY(ct)			(CT_PROPERTIES(ct) & CT_ByReferenceOnly)	// Must it be caught by reference?
#define CT_HASVB(ct)				(CT_PROPERTIES(ct) & CT_HasVirtualBase)		// Is this type a class with virtual bases?
#define CT_ISWINRTHANDLE(ct)		(CT_PROPERTIES(ct) & CT_IsWinRTHandle)		// Is it a winrt handle?
#define CT_ISSTDBADALLOC(ct)		(CT_PROPERTIES(ct) & CT_IsStdBadAlloc)		// Is it a std::bad_alloc?

//
// CatchableTypeArray - array of pointers to catchable types, with length
//
#pragma warning(disable:4200)		// get rid of obnoxious nonstandard extension warning
#ifndef WANT_NO_TYPES
typedef const struct _s_CatchableTypeArray {
	int	nCatchableTypes;
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			arrayOfCatchableTypes[];	// Image relative offset of Catchable Types
#else
	CatchableType	* _EH_PTR64 arrayOfCatchableTypes[];
#endif
	} CatchableTypeArray;
#endif // WANT_NO_TYPES
#pragma warning(default:4200)

//
// ThrowInfo - information describing the thrown object, staticly built
// at the throw site.
//
// pExceptionObject (the dynamic part of the throw; see below) is always a
// reference, whether or not it is logically one.  If 'isSimpleType' is true,
// it is a reference to the simple type, which is 'size' bytes long.  If
// 'isReference' and 'isSimpleType' are both false, then it's a UDT or
// a pointer to any type (ie pExceptionObject points to a pointer).  If it's
// a pointer, copyFunction is NULL, otherwise it is a pointer to a copy
// constructor or copy constructor closure.
//
// The pForwardCompat function pointer is intended to be filled in by future
// versions, so that if say a DLL built with a newer version (say C10) throws,
// and a C9 frame attempts a catch, the frame handler attempting the catch (C9)
// can let the version that knows all the latest stuff do the work.
//

#ifndef WANT_NO_TYPES
typedef const struct _s_ThrowInfo {
	unsigned int	attributes;			// Throw Info attributes (Bit field)
	PMFN			pmfnUnwind;			// Destructor to call when exception
										// has been handled or aborted.
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			pForwardCompat;		// Image relative offset of Forward compatibility frame handler
	__int32			pCatchableTypeArray;// Image relative offset of CatchableTypeArray
#else
#if defined(__cplusplus)
	int	(__cdecl* _EH_PTR64 pForwardCompat)(...);	// Forward compatibility frame handler
#else
	int	(__cdecl* _EH_PTR64 pForwardCompat)();	// Forward compatibility frame handler
#endif
	CatchableTypeArray	* _EH_PTR64 pCatchableTypeArray;	// Pointer to list of pointers to types.
#endif
} ThrowInfo;
#endif // WANT_NO_TYPES

#define TI_IsConst			0x00000001		// thrown object has const qualifier
#define TI_IsVolatile		0x00000002		// thrown object has volatile qualifier
#define TI_IsUnaligned		0x00000004		// thrown object has unaligned qualifier
#define TI_IsPure			0x00000008		// object thrown from a pure module
#define TI_IsWinRT          0x00000010      // object thrown is a WinRT Exception

#define THROW_ATTRS(t)			((t).attributes)
#if _EH_RELATIVE_OFFSETS && !defined(_M_CEE_PURE)
#define THROW_UNWINDFUNC_IB(t,ib)		((void (* _EH_PTR64)(void* _EH_PTR64))((ib) + (t).pmfnUnwind))
#define THROW_FORWARDCOMPAT_IB(t,ib)	((int(__cdecl * _EH_PTR64)(...))((ib) + (t).pForwardCompat))
#define THROW_CTARRAY_IB(t,ib)			((CatchableTypeArray*)((ib) + (t).pCatchableTypeArray))
#define THROW_COUNT_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->nCatchableTypes)
#define THROW_CTLIST_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->arrayOfCatchableTypes)
#else
#define THROW_FORWARDCOMPAT(t)	((t).pForwardCompat)
#define THROW_COUNT(t)			((t).pCatchableTypeArray->nCatchableTypes)
#define THROW_CTLIST(t)			((t).pCatchableTypeArray->arrayOfCatchableTypes)
#endif
#define THROW_UNWINDFUNC(t)		((t).pmfnUnwind)
#define THROW_PCTLIST(t)		(&THROW_CTLIST(t))
#define THROW_CT(t, n)			(*THROW_CTLIST(t)[n])
#define THROW_PCT(t, n)			(THROW_CTLIST(t)[n])

#define SET_TI_ISCONST(t)		(THROW_ATTRS(t) |= TI_IsConst)		// Is the object thrown 'const' qualified
#define SET_TI_ISVOLATILE(t)	(THROW_ATTRS(t) |= TI_IsVolatile)	// Is the object thrown 'volatile' qualified
#define SET_TI_ISUNALIGNED(t)	(THROW_ATTRS(t) |= TI_IsUnaligned)	// Is the object thrown 'unaligned' qualified
#define SET_TI_ISPURE(t)		(THROW_ATTRS(t) |= TI_IsPure)		// Is the object thrown from a pure module
#define SET_TI_WINRT(t)		    (THROW_ATTRS(t) |= TI_IsWinRT)		// Is the object thrown a WinRT Exception

#define THROW_ISCONST(t)		(THROW_ATTRS(t) & TI_IsConst)
#define THROW_ISVOLATILE(t)		(THROW_ATTRS(t) & TI_IsVolatile)
#define THROW_ISUNALIGNED(t)	(THROW_ATTRS(t) & TI_IsUnaligned)
#define THROW_ISPURE(t)			(THROW_ATTRS(t) & TI_IsPure)
#define THROW_ISWINRT(t)		(THROW_ATTRS(t) & TI_IsWinRT)
//
// Here's how to throw:
// M00HACK: _ThrowInfo is the name of the type that is 'pre-injected' into the
// compiler; since this prototype is known to the FE along with the pre-injected
// types, it has to match exactly.
//
__declspec (noreturn) extern "C" void __stdcall _CxxThrowException(void* pExceptionObject, _ThrowInfo* pThrowInfo);

#ifndef WANT_NO_TYPES
extern "C" int __cdecl __CxxExceptionFilter(void*, void*, int, void *);

#ifdef prepifdef
	prepifdef _MANAGED
	int __clrcall ___CxxExceptionFilter(void*, void*, int, void *);
	prependif	// _MANAGED
#else
	#ifdef _MANAGED
	int __clrcall ___CxxExceptionFilter(void*, void*, int, void *);
	#endif		// _MANAGED
#endif

// Returns true if the object is really a C++ exception
// If it is, stores the previous exception in *storage, and saves the current one
// This is needed to keep track of the current exception object (used for rethrow & destruction)
extern "C" int __cdecl __CxxRegisterExceptionObject(void *exception, void *storage);

#ifdef prepifdef
	prepifdef _MANAGED
	int __clrcall ___CxxRegisterExceptionObject(void *exception, void *storage);
	prependif	// _MANAGED
#else
	#ifdef _MANAGED
	int __clrcall ___CxxRegisterExceptionObject(void *exception, void *storage);
	#endif		// _MANAGED
#endif

// Returns true if exception is a C++ rethrown exception
// This is needed, so Unregister can know whether or not to destroy the object
extern "C" int __cdecl __CxxDetectRethrow(void *exception);

#ifdef prepifdef
	prepifdef _MANAGED
	int __clrcall ___CxxDetectRethrow(void *exception);
	prependif	// _MANAGED
#else
	#ifdef _MANAGED
	int __clrcall ___CxxDetectRethrow(void *exception);
	#endif		// _MANAGED
#endif

// Returns the byte count of stack space required to store the exception info
extern "C" int __cdecl __CxxQueryExceptionSize(void);

#ifdef prepifdef
	prepifdef _MANAGED
	int __clrcall ___CxxQueryExceptionSize(void);
	prependif	// _MANAGED
#else
	#ifdef _MANAGED
	int __clrcall ___CxxQueryExceptionSize(void);
	#endif		// _MANAGED
#endif

// Pops the current exception, restoring the previous one from *storage
// This detects whether or not the exception object needs to be destroyed
extern "C" void __cdecl __CxxUnregisterExceptionObject(void *storage, int rethrow);

#ifdef prepifdef
	prepifdef _MANAGED
	void __clrcall ___CxxUnregisterExceptionObject(void *storage, int rethrow);
	prependif	// _MANAGED
#else
	#ifdef _MANAGED
	void __clrcall ___CxxUnregisterExceptionObject(void *storage, int rethrow);
	#endif		// _MANAGED
#endif

#endif // WANT_NO_TYPES

#endif /* _EHDATA_NOTHROW */


#ifndef _EHDATA_NOFUNCINFO

/////////////////////////////////////////////////////////////////////////////
//
// Describing 'try/catch' blocks:
//
//---------------------------------------------------------------------------

//
// Current state of a function.
// -1 is the 'blank' state, ie there is nothing to unwind, no try blocks active.
//

typedef int __ehstate_t;		// The type of a state index


//
// HandlerType - description of a single 'catch'
//

#ifndef WANT_NO_TYPES
typedef const struct _s_HandlerType {
	unsigned int	adjectives;			// Handler Type adjectives (bitfield)
#if _EH_RELATIVE_OFFSETS && !defined(_M_CEE_PURE)
	__int32			dispType;			// Image relative offset of the corresponding type descriptor
	__int32			dispCatchObj;		// Displacement of catch object from base
	__int32			dispOfHandler;		// Image relative offset of 'catch' code
#if defined(_M_AMD64) || defined(_M_ARM64)
    __int32         dispFrame;          // displacement of address of function frame wrt establisher frame
#endif
#else
	TypeDescriptor	*pType;				// Pointer to the corresponding type descriptor
#if defined(_M_ALPHA64)
	__int32			dispCatchObj;		// Displacement of catch object from base
#else
	ptrdiff_t		dispCatchObj;		// Displacement of catch object from base
#endif
#if (_M_MRX000 >= 4000) || defined(_M_ARM)	 /*IFSTRIP=IGN*/
	ULONG			frameNestLevel;		// The static nesting level of parent function
#endif
	void *			addressOfHandler;	// Address of 'catch' code
#endif
} HandlerType;
#endif

#define HT_IsConst			0x00000001		// type referenced is 'const' qualified
#define HT_IsVolatile		0x00000002		// type referenced is 'volatile' qualified
#define HT_IsUnaligned		0x00000004		// type referenced is 'unaligned' qualified
#define HT_IsReference		0x00000008		// catch type is by reference
#define HT_IsResumable		0x00000010		// the catch may choose to resume (Reserved)
#define HT_IsStdDotDot      0x00000040      // the catch is std C++ catch(...) which is suppose to catch only C++ exception.
#define HT_IsBadAllocCompat 0x00000080      // the WinRT type can catch a std::bad_alloc
#define HT_IsComplusEh      0x80000000      // Is handling within complus eh.

#define HT_ADJECTIVES(ht)		((ht).adjectives)
#if _EH_RELATIVE_OFFSETS && !defined(_M_CEE_PURE)
#define HT_PTD_IB(ht,ib)		((TypeDescriptor*)((ib) + (ht).dispType))
#define HT_HANDLER_IB(ht,ib)	((void* _EH_PTR64)((ib) + (ht).dispOfHandler))
#else
#define HT_PTD(ht)				((ht).pType)
#define HT_HANDLER(ht)			((ht).addressOfHandler)
#endif
#define HT_DISPCATCH(ht)		((ht).dispCatchObj)
#if ((_M_MRX000 >= 4000) || defined(_M_ARM)) && !defined(_M_ARM_NT)	 /*IFSTRIP=IGN*/
#define HT_FRAMENEST(ht)		((ht).frameNestLevel)
#endif
#define HT_NAME(ht)				(TD_NAME(*HT_PTD(ht)))
#define HT_HASH(ht)				(TD_HASH(*HT_PTD(ht)))
#define HT_IS_TYPE_ELLIPSIS(ht)	TD_IS_TYPE_ELLIPSIS(HT_PTD(ht))

#define SET_HT_ISCONST(ht)		(HT_ADJECTIVES(ht) |= HT_IsConst)
#define SET_HT_ISVOLATILE(ht)	(HT_ADJECTIVES(ht) |= HT_IsVolatile)
#define SET_HT_ISUNALIGNED(ht)	(HT_ADJECTIVES(ht) |= HT_IsUnaligned)
#define SET_HT_ISREFERENCE(ht)	(HT_ADJECTIVES(ht) |= HT_IsReference)
#define SET_HT_ISRESUMABLE(ht)	(HT_ADJECTIVES(ht) |= HT_IsResumable)
#define SET_HT_ISCOMPLUSEH(ht)  (HT_ADJECTIVES(ht) |= HT_IsComplusEh)

#define HT_ISCONST(ht)			(HT_ADJECTIVES(ht) & HT_IsConst)		// Is the type referenced 'const' qualified
#define HT_ISVOLATILE(ht)		(HT_ADJECTIVES(ht) & HT_IsVolatile)		// Is the type referenced 'volatile' qualified
#define HT_ISUNALIGNED(ht)		(HT_ADJECTIVES(ht) & HT_IsUnaligned)	// Is the type referenced 'unaligned' qualified
#define HT_ISREFERENCE(ht)		(HT_ADJECTIVES(ht) & HT_IsReference)	// Is the catch type by reference
#define HT_ISRESUMABLE(ht)		(HT_ADJECTIVES(ht) & HT_IsResumable)	// Might the catch choose to resume (Reserved)
#define HT_ISCOMPLUSEH(ht)      (HT_ADJECTIVES(ht) & HT_IsComplusEh)

#define HT_ISBADALLOCCOMPAT(ht) (HT_ADJECTIVES(ht) & HT_IsBadAllocCompat)

#define HT_IS_STD_DOTDOT(ht)    (HT_ADJECTIVES(ht) & HT_IsStdDotDot)

//
// HandlerMapEntry - associates a handler list (sequence of catches) with a
//	range of eh-states.
//

#ifndef WANT_NO_TYPES
typedef const struct _s_TryBlockMapEntry {
	__ehstate_t	tryLow;				// Lowest state index of try
	__ehstate_t	tryHigh;			// Highest state index of try
#if !defined(_M_ALPHA)
	__ehstate_t	catchHigh;			// Highest state index of any associated catch
#endif
	int			nCatches;			// Number of entries in array
#if _EH_RELATIVE_OFFSETS	/*IFSTRIP=IGN*/
	__int32		dispHandlerArray;	// Image relative offset of list of handlers for this try
#else
	HandlerType *pHandlerArray;		// List of handlers for this try
#endif
} TryBlockMapEntry;
#endif // WANT_NO_TYPES

#define TBME_LOW(hm)		((hm).tryLow)
#define TBME_HIGH(hm)		((hm).tryHigh)
#define TBME_CATCHHIGH(hm)	((hm).catchHigh)
#define TBME_NCATCHES(hm)	((hm).nCatches)
#if _EH_RELATIVE_OFFSETS	/*IFSTRIP=IGN*/
#define TBME_PLIST(hm,ib)	((HandlerType*)((ib) + (hm).dispHandlerArray))
#define TBME_CATCH(hm,n,ib)	(TBME_PLIST(hm,ib)[n])
#define TBME_PCATCH(hm,n,ib)(&(TBME_PLIST(hm,ib)[n]))
#else
#define TBME_PLIST(hm)		((hm).pHandlerArray)
#define TBME_CATCH(hm, n)	(TBME_PLIST(hm)[n])
#define TBME_PCATCH(hm, n)	(&(TBME_PLIST(hm)[n]))
#endif


/////////////////////////////////////////////////////////////////////////////
//
// Description of the function:
//
//---------------------------------------------------------------------------

//
// UnwindMapEntry - Description of each state transition for unwinding
//	the stack (ie destructing objects).
//
// The unwind map is an array, indexed by current state.  Each entry specifies
// the state to go to during unwind, and the action required to get there.
// Note that states are represented by a signed integer, and that the 'blank'
// state is -1 so that the array remains 0-based (because by definition there
// is never any unwind action to be performed from state -1).  It is also
// assumed that state indices will be dense, ie that there will be no gaps of
// unused state indices in a function.
//

typedef const struct _s_UnwindMapEntry {
	__ehstate_t		toState;			// State this action takes us to
#if _EH_RELATIVE_OFFSETS	/*IFSTRIP=IGN*/
	__int32			action;				// Image relative offset of funclet
#else
	void			(*action)(void);	// Funclet to call to effect state change
#endif
} UnwindMapEntry;

#define UWE_TOSTATE(uwe)	((uwe).toState)
#if _EH_RELATIVE_OFFSETS
#define UWE_ACTION_IB(uwe,ib)	((void (*_EH_PTR64)(void))((ib) + (uwe).action))
#else
#define UWE_ACTION(uwe)			((uwe).action)
#endif

#if _M_MRX000 >= 4000 || defined(_M_MPPC) || defined(_M_PPC) || defined(_M_IA64) || defined(_M_AMD64) || defined(_M_ARM) || defined(_M_ARM64)	 /*IFSTRIP=IGN*/
typedef struct IptoStateMapEntry {
#if _EH_RELATIVE_OFFSETS	/*IFSTRIP=IGN*/
	__int32		Ip;		// Image relative offset of IP
#else
	ULONG		Ip;
#endif
	__ehstate_t	State;
} IptoStateMapEntry;
#endif

#ifndef WANT_NO_TYPES
typedef const struct _s_ESTypeList
{
	int nCount;					// how many types are there
#if _EH_RELATIVE_OFFSETS
	__int32 dispTypeArray; // offset of list of types in exception specification
#else
	HandlerType *pTypeArray;	// List of types in exception specification
#endif
}ESTypeList;

#define EST_COUNT(x)    ((x)->nCount)
#if _EH_RELATIVE_OFFSETS
#define EST_ARRAY_IB(estl, ib, n)  (&(((HandlerType*)((ib) + (estl)->dispTypeArray))[n]))
#else
#define EST_ARRAY(x,n)  (&((x)->pTypeArray[n]))
#endif
#endif // WANT_NO_TYPES

//
// FuncInfo - all the information that describes a function with exception
//	handling information.
//

// bbtFlags values
#define BBT_UNIQUE_FUNCINFO 1

#ifndef WANT_NO_TYPES
/*
 * The magicNumber here is incremented with every compiler change that does not
 * break backwards compatiblity. If for some reason backward compatibilty
 * should be broken, then we will add new handler. What this means is that
 * current handler functions can assume that the structure layout that they
 * know about will remain the same and so even if magicNumber > my_magicNumber,
 * the handler can assume that what all it needs is there. The magicNumber will
 * be revised every time new data is added at the end of this structure.
 */
typedef const struct _s_FuncInfo
{
    unsigned int		magicNumber:29;		// Identifies version of compiler
    unsigned int        bbtFlags:3;         // flags that may be set by BBT processing
	__ehstate_t			maxState;			// Highest state number plus one (thus
											// number of entries in unwind map)
#if _EH_RELATIVE_OFFSETS	/*IFSTRIP=IGN*/
	__int32				dispUnwindMap;		// Image relative offset of the unwind map
	unsigned int		nTryBlocks;			// Number of 'try' blocks in this function
	__int32				dispTryBlockMap;	// Image relative offset of the handler map
	unsigned int		nIPMapEntries;		// # entries in the IP-to-state map. NYI (reserved)
	__int32				dispIPtoStateMap;	// Image relative offset of the IP to state map
	__int32				dispUwindHelp;		// Displacement of unwind helpers from base
	__int32				dispESTypeList;		// Image relative list of types for exception specifications
#else
	UnwindMapEntry		*pUnwindMap;		// Where the unwind map is
	unsigned int		nTryBlocks;			// Number of 'try' blocks in this function
	TryBlockMapEntry	*pTryBlockMap;		// Where the handler map is
#if defined(_M_ALPHA)
    signed int          EHContextDelta;     // Frame offset of EHContext record
#endif
	unsigned int		nIPMapEntries;		// # entries in the IP-to-state map. NYI (reserved)
#if _M_MRX000 >= 4000	 /*IFSTRIP=IGN*/
	IptoStateMapEntry	*pIPtoStateMap;     // An IP to state map..
	ptrdiff_t			dispUnwindHelp;		// Displacement of unwind helpers from base
	int					iTryBlockIndex;		// Used by catch functions only
	int					iFrameNestLevel;	// The static nesting level of parent function
#elif defined(_M_MPPC) || defined(_M_PPC) || defined (_M_ARM)
	IptoStateMapEntry	*pIPtoStateMap;		// An IP to state map..
#else
	void				*pIPtoStateMap;		// An IP to state map.  NYI (reserved).
#endif
	ESTypeList			*pESTypeList;		// List of types for exception specifications
#endif
    __int32             EHFlags;            // Flags for some features.
#if defined(_M_ARM) && !defined(_M_ARM_NT)
    union
    {
        __int32         alignStack;             // Dynamic stack alignment
        __int32         gs_cookie_offset_noalign;// /GS cookie offset if no alignment present
    } funcInfoAlign;
    __int32             offsetAlignStack;       // Offset from frame pointer before alignment
#endif
#if defined(_M_ARM) || defined(_M_MIPS) || defined(_M_SH)
    __int32             gs_cookie_offset;       // (optional) Offset from the (aligned) FP of the /GS cookie
#endif
} FuncInfo;
#endif // WANT_NO_TYPES

#define FUNC_MAGICNUM(fi)		((fi).magicNumber)
#define FUNC_MAXSTATE(fi)		((fi).maxState)
#define FUNC_NTRYBLOCKS(fi)		((fi).nTryBlocks)
#define FUNC_NIPMAPENT(fi)		((fi).nIPMapEntries)
#define FUNC_FLAGS(fi)			((fi).EHFlags)
#if _EH_RELATIVE_OFFSETS
#define FUNC_PUNWINDMAP(fi,ib)	((UnwindMapEntry*)((ib) + (fi).dispUnwindMap))
#define FUNC_PHANDLERMAP(fi,ib)	((TryBlockMapEntry*)((ib) + (fi).dispTryBlockMap))
#define FUNC_IPMAP(fi,ib)		((IptoStateMapEntry*)((ib) + (fi).dispIPtoStateMap))
#define FUNC_UNWIND(fi,st,ib)	(FUNC_PUNWINDMAP(fi,ib)[st])
#define FUNC_PUNWIND(fi,st,ib)	(&FUNC_UNWIND(fi,st,ib))
#define FUNC_TRYBLOCK(fi,n,ib)	(FUNC_PHANDLERMAP(fi,ib)[n])
#define FUNC_PTRYBLOCK(fi,n,ib)	(&FUNC_TRYBLOCK(fi,n,ib))
#define FUNC_ESTYPES_IB(fi, ib)	((ESTypeList*)((ib) + (fi).dispESTypeList))
#define FUNC_PESTYPES_IB(fi, ib)	FUNC_ESTYPES_IB((*fi), ib)
#else
#define FUNC_PUNWINDMAP(fi)		((fi).pUnwindMap)
#define FUNC_PHANDLERMAP(fi)	((fi).pTryBlockMap)
#define FUNC_IPMAP(fi)			((fi).pIPtoStateMap)
#define FUNC_UNWIND(fi, st)		((fi).pUnwindMap[st])
#define FUNC_PUNWIND(fi, st)	(&FUNC_UNWIND(fi, st))
#define FUNC_TRYBLOCK(fi,n)		((fi).pTryBlockMap[n])
#define FUNC_PTRYBLOCK(fi,n)	(&FUNC_TRYBLOCK(fi, n))
#define FUNC_ESTYPES(fi)		((fi).pESTypeList)
#define FUNC_PESTYPES(fi)		(FUNC_ESTYPES(*fi))
#endif
#if defined(_M_ALPHA)
#define FUNC_EHCONTEXTDELTA(fi) ((fi).EHContextDelta)
#endif
#if _M_MRX000 >= 4000		 /*IFSTRIP=IGN*/
#define FUNC_IPTOSTATE(fi,n)	((fi).pIPtoStateMap[n])
#define FUNC_PIPTOSTATE(fi,n)	(&FUNC_IPTOSTATE(fi,n))
#define FUNC_DISPUNWINDHELP(fi)	((fi).dispUnwindHelp)
#define FUNC_TRYBLOCKINDEX(fi)	((fi).iTryBlockIndex)
#define FUNC_FRAMENEST(fi)		((fi).iFrameNestLevel)
#elif defined(_M_MPPC) || defined(_M_PPC) || (defined(_M_ARM) && !defined(_M_ARM_NT))
#define FUNC_IPTOSTATE(fi,n)	((fi).pIPtoStateMap[n])
#define FUNC_PIPTOSTATE(fi,n)	(&FUNC_IPTOSTATE(fi,n))
#elif _EH_RELATIVE_OFFSETS
#define FUNC_IPTOSTATE(fi,n,ib)	(FUNC_IPMAP(fi,ib)[n])
#define FUNC_PIPTOSTATE(fi,n,ib)(&FUNC_IPTOSTATE(fi,n,ib))
#define FUNC_DISPUNWINDHELP(fi)	((fi).dispUwindHelp)
#else
#define FUNC_IPTOSTATE(fi,n) 	__ERROR_NYI__
#endif
#if defined(_M_ARM)
#define FUNC_STKALIGN(fi)       ((fi).alignStack)
#define FUNC_OFFSTKALIGN(fi)    ((fi).offsetAlignStack)
#endif

#endif /* _EHDATA_NOFUNCINFO */

#ifndef _EHDATA_NONT

/////////////////////////////////////////////////////////////////////////////
//
// Data types that are variants of data used by NT (and Chicago) to manage
// exception handling.
//
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// A stack registration node (i386 only)
//

#if defined(_M_IX86) && _M_IX86 >= 300 /*IFSTRIP=IGN*/

struct EHRegistrationNode;
typedef struct EHRegistrationNode EHRegistrationNode;

struct EHRegistrationNode {
	/* void *			stackPtr */		// Stack ptr at entry to try (below address point)
	EHRegistrationNode	*pNext;			// Next node in the chain
	void *				frameHandler;	// The handler function for this frame
	__ehstate_t			state;			// The current state of this function
#if CC_EXPLICITFRAME
	void *				frame;			// Value of ebp for this frame
#endif
};

#if !CC_EXPLICITFRAME
				// Cannonical offset
# define FRAME_OFFSET	sizeof(EHRegistrationNode)
#endif

#define PRN_NEXT(prn)		((prn)->pNext)
#define PRN_HANDLER(prn)	((prn)->frameHandler)
#define PRN_STATE(prn)		((prn)->state)
#define PRN_STACK(prn)		(((void**)(prn))[-1])
#if CC_EXPLICITFRAME
# define PRN_FRAME(prn)		((prn)->frame)
#else
# define PRN_FRAME(prn)		((void*)(((char*)prn) + FRAME_OFFSET))
#endif

typedef void DispatcherContext;		// Meaningless on Intel

#elif _M_MRX000 >= 4000 /*IFSTRIP=IGN*/
//
// On MIPS we don't have a registration node, just a pointer to the stack frame base
//
typedef ULONG EHRegistrationNode;

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0
#if !defined(_NTSUBSET_)
typedef struct _RUNTIME_FUNCTION {
    ULONG BeginAddress;
    ULONG EndAddress;
    EXCEPTION_DISPOSITION (*ExceptionHandler)();
    PVOID HandlerData;
    ULONG PrologEndAddress;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;
#endif

typedef struct _xDISPATCHER_CONTEXT {
    ULONG ControlPc;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG EstablisherFrame;
    PCONTEXT ContextRecord;
} DispatcherContext;					// changed the case of the name to conform to EH conventions

#elif defined(_M_ARM) /*IFSTRIP=IGN*/

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0

#if !defined(_NTSUBSET_) && !defined(_NTARMH_)
typedef struct _UNWIND_INFO {
    unsigned __int16 Version;                  // Version Number
    unsigned __int16 Flags;                    // Flags
    unsigned __int32 DataLength;               // Length of Descriptor Data
} UNWIND_INFO, *PUNWIND_INFO;

#endif

typedef struct _xDISPATCHER_CONTEXT {
    ULONG ControlPc;
    ULONG ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG EstablisherFrame;
    ULONG TargetPc;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PVOID HistoryTable;
    ULONG ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    PUCHAR NonVolatileRegisters;
} DispatcherContext;					// changed the case of the name to conform to EH conventions


//
// On ARM we don't have a registration node, just a pointer to the stack frame base
//

typedef ULONG EHRegistrationNode;

#elif defined(_M_IA64) /*IFSTRIP=IGN*/

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0

typedef struct _UNWIND_HISTORY_TABLE* PUNWIND_HISTORY_TABLE;

typedef struct _xDISPATCHER_CONTEXT {
    FRAME_POINTERS EstablisherFrame;
    __int64 ControlPc;
    __int64 ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    PCONTEXT ContextRecord;
    PUNWIND_HISTORY_TABLE HistoryTable;
    unsigned __int64 TargetGp;
    unsigned long Index;
} DispatcherContext;

//
// On P7 we don't have a registration node, just a pointer to the stack frame base
//
typedef FRAME_POINTERS EHRegistrationNode;

#elif defined(_M_AMD64)/*IFSTRIP=IGN*/

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0

#pragma warning (push)
#pragma warning (disable: 4201)

#if !defined(_NTSUBSET_) && !defined(_NTAMD64_)
typedef union _UNWIND_CODE {
    struct {
        unsigned char CodeOffset;
        unsigned char UnwindOp : 4;
        unsigned char OpInfo : 4;
    };
    unsigned short FrameOffset;
} UNWIND_CODE, *PUNWIND_CODE;
typedef struct _UNWIND_INFO {
    unsigned char Version : 3;                 // Version Number
    unsigned char Flags   : 5;                 // Flags
    unsigned char SizeOfProlog;
    unsigned char CountOfCodes;
    unsigned FrameRegister : 4;
    unsigned FrameOffset   : 4;
    UNWIND_CODE UnwindCode[1];
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes+1)&~1)-1];
 *  union {
 *      OPTIONAL ULONG ExceptionHandler;
 *      OPTIONAL ULONG FunctionEntry;
 *  };
 *  OPTIONAL ULONG ExceptionData[];
 */
} UNWIND_INFO, *PUNWIND_INFO;
#endif

#pragma warning (pop)

typedef struct _UNWIND_HISTORY_TABLE* PUNWIND_HISTORY_TABLE;

typedef struct _xDISPATCHER_CONTEXT {
    __int64 ControlPc;
    __int64 ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG64 TargetIp;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
} DispatcherContext;					// changed the case of the name to conform to EH conventions

//
// On P7 we don't have a registration node, just a pointer to the stack frame base
//
typedef ULONG_PTR EHRegistrationNode;

#elif defined(_M_ARM64)/*IFSTRIP=IGN*/

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0


#pragma warning (push)
#pragma warning (disable: 4201)

typedef union _UNWIND_CODE {
    struct {
        unsigned char CodeOffset;
        unsigned char UnwindOp : 4;
        unsigned char OpInfo : 4;
    };
    unsigned short FrameOffset;
} UNWIND_CODE, *PUNWIND_CODE;

#pragma warning (pop)
typedef struct _UNWIND_INFO {
    unsigned char Version : 3;                 // Version Number
    unsigned char Flags   : 5;                 // Flags
    unsigned char SizeOfProlog;
    unsigned char CountOfCodes;
    unsigned FrameRegister : 4;
    unsigned FrameOffset   : 4;
    UNWIND_CODE UnwindCode[1];
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes+1)&~1)-1];
 *  union {
 *      OPTIONAL ULONG ExceptionHandler;
 *      OPTIONAL ULONG FunctionEntry;
 *  };
 *  OPTIONAL ULONG ExceptionData[];
 */
} UNWIND_INFO, *PUNWIND_INFO;

typedef struct _UNWIND_HISTORY_TABLE* PUNWIND_HISTORY_TABLE;

typedef struct _xDISPATCHER_CONTEXT {
    ULONG_PTR ControlPc;
    ULONG_PTR ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG_PTR TargetPc;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    DWORD ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    PKNONVOLATILE_CONTEXT_POINTERS NonVolatileRegisters;
} DispatcherContext;					// changed the case of the name to conform to EH conventions

//
// On P7 we don't have a registration node, just a pointer to the stack frame base
//
typedef ULONG_PTR EHRegistrationNode;

#elif defined(_M_ALPHA)
//
// On Alpha we don't have a registration node,
//     just a pointer to the stack frame base
//
typedef ULONG_PTR EHRegistrationNode;

#define PRN_NEXT(prn)           __ERROR__
#define PRN_HANDLER(prn)        __ERROR__
#define PRN_STATE(prn)          __ERROR__
#define PRN_STACK(prn)          __ERROR__
#define PRN_FRAME(prn)          __ERROR__

#if defined(_M_ALPHA64)	 /*IFSTRIP=IGN*/
#pragma pack(push, EHContext, 8)
#endif

#define FRAME_OFFSET            0
#if !defined(_NTSUBSET_)
typedef struct _RUNTIME_FUNCTION {
    ULONG_PTR BeginAddress;
    ULONG_PTR EndAddress;
    EXCEPTION_DISPOSITION (*ExceptionHandler)();
    PVOID HandlerData;    // ptr to FuncInfo record
    ULONG_PTR PrologEndAddress;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;
#endif

typedef struct _xDISPATCHER_CONTEXT {
    ULONG_PTR ControlPc;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG_PTR EstablisherFrame;  // Virtual Frame Pointer
    PCONTEXT ContextRecord;
} DispatcherContext;            // changed the case of the name to conform to EH conventions

//
// _EHCONTEXT is a struct built in the frame by the compiler.
// On entry to a function, compiler generated code stores the
// address of the base of the fixed frame area (the so-called
// Real Frame Pointer) into the Rfp. On every state transition,
// compiler generated code stores the current state index into
// the State field.
//
// The FuncInfo record for the function contains the offset of
// the _EHCONTEXT record from the Virtual Frame Pointer - a
// pointer to the highest address of the frame so this offset
// is negative (frames grow down in the address space).
//
typedef struct _EHCONTEXT {
    ULONG State;
    PVOID Rfp;
} EHContext;
#if defined(_M_ALPHA64)	 /*IFSTRIP=IGN*/
#pragma pack(pop, EHContext)
#endif

#define VIRTUAL_FP(pDC) (pDC->EstablisherFrame)

#define REAL_FP(VirtualFP, pFuncInfo)           \
    (((EHContext *)((char *)VirtualFP           \
     + pFuncInfo->EHContextDelta)) -> Rfp)

#define EH_STATE(VirtualFP, pFuncInfo)          \
    (((EHContext *)((char *)VirtualFP           \
     + pFuncInfo->EHContextDelta)) -> State)

#elif defined(_M_M68K)
struct EHRegistrationNode {
/*	void * 				_sp;			// The stack pointer for the entry of try/catch	*/
	void *				frameHandler;	// The handler function for this frame
	__ehstate_t			state;			// The current state of this function
};

#define PRN_HANDLER(prn)	((prn)->frameHandler)
#define PRN_STATE(prn)		((prn)->state)

typedef void DispatcherContext;		// Meaningless on Mac


#elif defined(_M_PPC) || defined(_M_MPPC)
//
// On PowerPC we don't have a registration node, just a pointer to the stack
// frame base
//
typedef ULONG EHRegistrationNode;

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0

#if !defined(_NTSUBSET_)
typedef struct _RUNTIME_FUNCTION {
    ULONG BeginAddress;
    ULONG EndAddress;
    EXCEPTION_DISPOSITION (*ExceptionHandler)(...);
    PVOID HandlerData;
    ULONG PrologEndAddress;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;
#endif

typedef struct _xDISPATCHER_CONTEXT {
    ULONG ControlPc;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG EstablisherFrame;
    PCONTEXT ContextRecord;
} DispatcherContext;
    // changed the case of the name to conform to EH conventions

#if defined(_M_MPPC)
typedef struct _ftinfo {
    ULONG dwMagicNumber;                // magic number
    void *pFrameInfo;			// pointer to runtime frame info
    PRUNTIME_FUNCTION rgFuncTable;	// function table
    ULONG cFuncTable;			// number of function entry
    ULONG dwEntryCF;			// address of starting of the code fragment
    ULONG dwSizeCF;			// size of the code fragment
} FTINFO, *PFTINFO;

#define offsFTINFO              64
#endif

#else
#error "Machine not supported"
#endif

/////////////////////////////////////////////////////////////////////////////
//
// The NT Exception record that we use to pass information from the throw to
// the possible catches.
//
// The constants in the comments are the values we expect.
// This is based on the definition of EXCEPTION_RECORD in winnt.h.
//
#if defined(_M_IA64) || defined(_M_ALPHA64) || defined(_M_AMD64) || defined(_M_ARM64) /*IFSTRIP=IGN*/
#pragma pack(push, EHExceptionRecord, 8)
#endif
#ifndef WANT_NO_TYPES
typedef struct EHExceptionRecord {
	DWORD		ExceptionCode;			// The code of this exception. (= EH_EXCEPTION_NUMBER)
	DWORD		ExceptionFlags;			// Flags determined by NT
    struct _EXCEPTION_RECORD *ExceptionRecord;	// An extra exception record (not used)
    void * 		ExceptionAddress;		// Address at which exception occurred
    DWORD 		NumberParameters;		// Number of extended parameters. (= EH_EXCEPTION_PARAMETERS)
	struct EHParameters {
		DWORD		magicNumber;		// = EH_MAGIC_NUMBER1
		void *		pExceptionObject;	// Pointer to the actual object thrown
		ThrowInfo	*pThrowInfo;		// Description of thrown object
#if _EH_RELATIVE_OFFSETS
		void		*pThrowImageBase;	// Image base of thrown object
#endif
		} params;
} EHExceptionRecord;
#endif // WANT_NO_TYPES
#if defined(_M_IA64) || defined(_M_ALPHA64) || defined(_M_AMD64) || defined(_M_ARM64) /*IFSTRIP=IGN*/
#pragma pack(pop, EHExceptionRecord)
#endif

#define PER_CODE(per)		((per)->ExceptionCode)
#define PER_FLAGS(per)		((per)->ExceptionFlags)
#define PER_NEXT(per)		((per)->ExceptionRecord)
#define PER_ADDRESS(per)	((per)->ExceptionAddress)
#define PER_NPARAMS(per)	((per)->NumberParameters)
#define PER_MAGICNUM(per)	((per)->params.magicNumber)
#define PER_PEXCEPTOBJ(per)	((per)->params.pExceptionObject)
#define PER_PTHROW(per)		((per)->params.pThrowInfo)
#define PER_EXCEPTINFO(per)	((PULONG_PTR)&(PER_MAGICNUM(per)))
#if _EH_RELATIVE_OFFSETS
#define PER_PTHROWIB(per)	((per)->params.pThrowImageBase)
#endif
#define PER_THROW(per)		(*PER_PTHROW(per))

#define PER_ISSIMPLETYPE(t)	(PER_THROW(t).isSimpleType)
#define PER_ISREFERENCE(t)	(PER_THROW(t).isReference)
#define PER_ISCONST(t)		(PER_THROW(t).isConst)
#define PER_ISVOLATILE(t)	(PER_THROW(t).isVolatile)
#define PER_ISUNALIGNED(t)	(PER_THROW(t).isUnaligned)
#define PER_UNWINDFUNC(t)	(PER_THROW(t).pmfnUnwind)
#define PER_PCTLIST(t)		(PER_THROW(t).pCatchable)
#define PER_CTLIST(t)		(*PER_PCTLIST(t))

#if defined(_M_CEE_PURE)
#define PER_IS_MSVC_EH(per)	((PER_CODE(per) == EH_EXCEPTION_NUMBER) && 			\
							 (PER_NPARAMS(per) == EH_EXCEPTION_PARAMETERS) &&	\
							 ((PER_MAGICNUM(per) == EH_PURE_MAGIC_NUMBER1) ||	\
							  PER_PTHROW(per) == NULL))
#else
#define PER_IS_MSVC_EH(per)	((PER_CODE(per) == EH_EXCEPTION_NUMBER) && 			\
							 (PER_NPARAMS(per) == EH_EXCEPTION_PARAMETERS) &&	\
							 ((PER_MAGICNUM(per) == EH_MAGIC_NUMBER1) ||   \
							  (PER_MAGICNUM(per) == EH_MAGIC_NUMBER2) ||   \
							  (PER_MAGICNUM(per) == EH_MAGIC_NUMBER3)))
#endif

#define PER_IS_MSVC_PURE_OR_NATIVE_EH(per) \
							((PER_CODE(per) == EH_EXCEPTION_NUMBER) && 			\
							 (PER_NPARAMS(per) == EH_EXCEPTION_PARAMETERS) &&	\
							 ((PER_MAGICNUM(per) == EH_MAGIC_NUMBER1) ||   \
							  (PER_MAGICNUM(per) == EH_MAGIC_NUMBER2) ||   \
							  (PER_MAGICNUM(per) == EH_MAGIC_NUMBER3) ||   \
							  (PER_MAGICNUM(per) == EH_PURE_MAGIC_NUMBER1)))

/////////////////////////////////////////////////////////////////////////////
//
// NT kernel routines and definitions required to implement exception handling:
//
// (from ntxcapi.h, which is not a public header file)
//
//---------------------------------------------------------------------------


#endif /* _EHDATA_NONT */

#endif /* ONLY_VALUES */

#pragma pack(pop, ehdata)

#endif /* _INC_EHDATA */
