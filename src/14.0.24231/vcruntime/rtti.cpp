/***
*rtti.cxx - C++ runtime type information
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Implementation of C++ standard runtime type information
****/

#ifdef _M_CEE_PURE
    #undef CRTDLL
    #undef MRTDLL
#endif // _M_CEE_PURE

#include <rtti.h>
#include <vcruntime_typeinfo.h>

#include <windows.h>

#if defined(_WIN64) && !defined(_M_CEE_PURE)
static unsigned __int64 GetImageBase(PVOID);
#undef BCD_PTD
#undef BCD_PCHD
#undef CHD_PBCA
#undef CHD_PBCD
#undef COL_PTD
#undef COL_PCHD
#define BCD_PTD(bcd)    BCD_PTD_IB((bcd),_ImageBase)
#define BCD_PCHD(bcd)   BCD_PCHD_IB((bcd),_ImageBase)
#define CHD_PBCA(chd)   CHD_PBCA_IB((chd),_ImageBase)
#define CHD_PBCD(chd)   CHD_PBCD_IB((chd),_ImageBase)
#define COL_PTD(col)    COL_PTD_IB((col),_ImageBase)
#define COL_PCHD(col)   COL_PCHD_IB((col),_ImageBase)
#endif

static PVOID __CLRCALL_OR_CDECL FindCompleteObject(PVOID *);
static _RTTIBaseClassDescriptor * __CLRCALL_OR_CDECL
    FindSITargetTypeInstance(_RTTICompleteObjectLocator *,
                             _RTTITypeDescriptor *,
                             _RTTITypeDescriptor *
#if defined(_WIN64) && !defined(_M_CEE_PURE)
                             , unsigned __int64
#endif
                             );
static _RTTIBaseClassDescriptor * __CLRCALL_OR_CDECL
    FindMITargetTypeInstance(PVOID,
                             _RTTICompleteObjectLocator *,
                             _RTTITypeDescriptor *,
                             ptrdiff_t,
                             _RTTITypeDescriptor *
#if defined(_WIN64) && !defined(_M_CEE_PURE)
                             , unsigned __int64
#endif
                             );
static _RTTIBaseClassDescriptor * __CLRCALL_OR_CDECL
    FindVITargetTypeInstance(PVOID,
                             _RTTICompleteObjectLocator *,
                             _RTTITypeDescriptor *,
                             ptrdiff_t,
                             _RTTITypeDescriptor *
#if defined(_WIN64) && !defined(_M_CEE_PURE)
                             , unsigned __int64
#endif
                             );
static ptrdiff_t __CLRCALL_OR_CDECL PMDtoOffset(PVOID, const PMD&);

/////////////////////////////////////////////////////////////////////////////
//
// __RTCastToVoid - Implements dynamic_cast<void*>
//
// Output: Pointer to complete object containing *inptr
//
// Side-effects: NONE.
//

extern "C" PVOID __CLRCALL_OR_CDECL __RTCastToVoid (
    PVOID inptr)            // Pointer to polymorphic object
    throw(...)
{
    if (inptr == nullptr)
        return nullptr;

    __try {
        return FindCompleteObject((PVOID *)inptr);
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
              ? EXCEPTION_EXECUTE_HANDLER: EXCEPTION_CONTINUE_SEARCH)
    {
        throw std::__non_rtti_object::__construct_from_string_literal("Access violation - no RTTI data!");
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// __RTtypeid - Implements typeid() operator
//
// Output: Pointer to type descriptor of complete object containing *inptr
//
// Side-effects: NONE.
//

extern "C" PVOID __CLRCALL_OR_CDECL __RTtypeid (
    PVOID inptr)            // Pointer to polymorphic object
    throw(...)
{
    if (!inptr) {
        throw std::bad_typeid::__construct_from_string_literal("Attempted a typeid of nullptr pointer!");   // WP 5.2.7
    }

    __try {
        // Ptr to CompleteObjectLocator should be stored at vfptr[-1]
        _RTTICompleteObjectLocator *pCompleteLocator =
            (_RTTICompleteObjectLocator *) ((*((void***)inptr))[-1]);
#if defined(_WIN64) && !defined(_M_CEE_PURE)
        unsigned __int64 _ImageBase;
        if (COL_SIGNATURE(*pCompleteLocator) == COL_SIG_REV0) {
            _ImageBase = GetImageBase((PVOID)pCompleteLocator);
        }
        else {
            _ImageBase = ((unsigned __int64)pCompleteLocator - (unsigned __int64)COL_SELF(*pCompleteLocator));
        }
#endif

        if (((const void *)COL_PTD(*pCompleteLocator)) != nullptr)
        {
            return (PVOID) COL_PTD(*pCompleteLocator);
        }
        else {
            throw std::__non_rtti_object::__construct_from_string_literal("Bad read pointer - no RTTI data!");
        }
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
              ? EXCEPTION_EXECUTE_HANDLER: EXCEPTION_CONTINUE_SEARCH)
    {
        throw std::__non_rtti_object::__construct_from_string_literal("Access violation - no RTTI data!");
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// __RTDynamicCast - Runtime implementation of dynamic_cast<> operator
//
// Output: Pointer to the appropriate sub-object, if possible; nullptr otherwise
//
// Side-effects: Throws bad_cast() if cast fails & input of dynamic_cast<> is
// a reference
//
// Note: A normal runtime check can be generated for a down-cast (base to
// derived) and a cross-cast (a proper base of the complete object to some other
// proper base which is neither an accessible unambiguous base nor derived class
// of the first base).  But the compiler will also generate a runtime check for
// an up-cast (derived to base) that is disallowed because the target base class
// is not public or is ambiguous within the source derived class.  Such an
// invalid up-cast may be a valid cross-cast in the VI case, e.g.:
//
//      A
//     / \
//    B   C
//     \ /
//      D
//
//   class A { virtual ~A(); {} };
//   class B : virtual private A {};
//   class C : virtual public A {};
//   class D : public B, public C {};
//   ...
//   dynamic_cast<A*>((B*)new D);
//
// The up-cast B->A fails, since B inherits A privately.  But the cross-cast
// B->D->C->A succeeds with a runtime check.
//

extern "C" PVOID __CLRCALL_OR_CDECL __RTDynamicCast (
    PVOID inptr,            // Pointer to polymorphic object
    LONG VfDelta,           // Offset of vfptr in object
    PVOID SrcType,          // Static type of object pointed to by inptr
    PVOID TargetType,       // Desired result of cast
    BOOL isReference)       // TRUE if input is reference, FALSE if input is ptr
    throw(...)
{
    PVOID pResult=nullptr;
    _RTTIBaseClassDescriptor *pBaseClass;

    if (inptr == nullptr)
            return nullptr;

    __try {

        PVOID pCompleteObject = FindCompleteObject((PVOID *)inptr);
        _RTTICompleteObjectLocator *pCompleteLocator =
            (_RTTICompleteObjectLocator *) ((*((void***)inptr))[-1]);
#if defined(_WIN64) && !defined(_M_CEE_PURE)
        unsigned __int64 _ImageBase;
        if (COL_SIGNATURE(*pCompleteLocator) == COL_SIG_REV0) {
            _ImageBase = GetImageBase((PVOID)pCompleteLocator);
        }
        else {
            _ImageBase = ((unsigned __int64)pCompleteLocator - (unsigned __int64)COL_SELF(*pCompleteLocator));
        }
#endif

        // Adjust by vfptr displacement, if any
        inptr = (PVOID *) ((char *)inptr - VfDelta);

        // Calculate offset of source object in complete object
        ptrdiff_t inptr_delta = (char *)inptr - (char *)pCompleteObject;

        if (!(CHD_ATTRIBUTES(*COL_PCHD(*pCompleteLocator)) & CHD_MULTINH)) {
            // if not multiple inheritance
            pBaseClass = FindSITargetTypeInstance(
                            pCompleteLocator,
                            (_RTTITypeDescriptor *) SrcType,
                            (_RTTITypeDescriptor *) TargetType
#if defined(_WIN64) && !defined(_M_CEE_PURE)
                            , _ImageBase
#endif
                            );
        }
        else if (!(CHD_ATTRIBUTES(*COL_PCHD(*pCompleteLocator)) & CHD_VIRTINH)) {
            // if multiple, but not virtual, inheritance
            pBaseClass = FindMITargetTypeInstance(
                            pCompleteObject,
                            pCompleteLocator,
                            (_RTTITypeDescriptor *) SrcType,
                            inptr_delta,
                            (_RTTITypeDescriptor *) TargetType
#if defined(_WIN64) && !defined(_M_CEE_PURE)
                            , _ImageBase
#endif
                            );
        }
        else {
            // if virtual inheritance
            pBaseClass = FindVITargetTypeInstance(
                            pCompleteObject,
                            pCompleteLocator,
                            (_RTTITypeDescriptor *) SrcType,
                            inptr_delta,
                            (_RTTITypeDescriptor *) TargetType
#if defined(_WIN64) && !defined(_M_CEE_PURE)
                            , _ImageBase
#endif
                            );
        }

        if (pBaseClass != nullptr) {
            // Calculate ptr to result base class from pBaseClass->where
            pResult = ((char *) pCompleteObject) +
                      PMDtoOffset(pCompleteObject, BCD_WHERE(*pBaseClass));
        }
        else {
            pResult = nullptr;
            if (isReference)
                throw std::bad_cast::__construct_from_string_literal("Bad dynamic_cast!");
        }

    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
              ? EXCEPTION_EXECUTE_HANDLER: EXCEPTION_CONTINUE_SEARCH)
    {
        pResult = nullptr;
        throw std::__non_rtti_object::__construct_from_string_literal("Access violation - no RTTI data!");
    }

    return pResult;
}


/////////////////////////////////////////////////////////////////////////////
//
// FindCompleteObject - Calculate member offset from PMD & this
//
// Output: pointer to the complete object containing class *inptr
//
// Side-effects: NONE.
//

static PVOID __CLRCALL_OR_CDECL FindCompleteObject (
    PVOID *inptr)           // Pointer to polymorphic object
{
    // Ptr to CompleteObjectLocator should be stored at vfptr[-1]
    _RTTICompleteObjectLocator *pCompleteLocator =
        (_RTTICompleteObjectLocator *) ((*((void***)inptr))[-1]);
    char *pCompleteObject = (char *)inptr - COL_OFFSET(*pCompleteLocator);

    // Adjust by construction displacement, if any
    if (COL_CDOFFSET(*pCompleteLocator))
        pCompleteObject -= *(int *)((char *)inptr - COL_CDOFFSET(*pCompleteLocator));
    return (PVOID) pCompleteObject;
}


/////////////////////////////////////////////////////////////////////////////
//
// FindSITargetTypeInstance - workhorse routine of __RTDynamicCast() in a
// Single-Inheritance hierarchy
//
// Output: pointer to the appropriate sub-object of targetted type; nullptr if
// cast fails
//
// Side-effects: NONE.
//
// The C++ standard (5.2.7/8) describes the runtime check thusly (where v is
// the source expression, and T the desired destination class type):
//
// * If, in the most derived object pointed (referred) to by v, v points
//   (refers) to a public base class subobject of a T object, and if only one
//   object of type T is derived from the sub-object pointed (referred) to by
//   v, the result is a pointer (an lvalue referring) to that T object.
// * Otherwise, if v points (refers) to a public base class sub-object of the
//   most derived object, and the type of the most derived object has a base
//   class, of type T, that is unambiguous and public, the result is a pointer
//   (an lvalue referring) to the T sub-object of the most derived object.
// * Otherwise, the run-time check fails.
//
// For single inheritance, there is always only one, unambiguous instance of
// any particular type in the inheritance hierarchy.  In addition, cross-casts
// aren't possible, so a runtime check is generated only for down-casts, for
// up-casts which will fail because of accessibility problems, or for casts
// to or from types which aren't in the most derived object's class hierarchy.
// Given that, the runtime rules simplify to this:
//
// * If, in the most derived object pointed (referred) to by v, v points
//   (refers) to a public base class subobject of a T object, and if an object
//   of type T is derived from the sub-object pointed (referred) to by v, the
//   result is a pointer (an lvalue referring) to that T object.
// * Otherwise, the run-time check fails.
//

static _RTTIBaseClassDescriptor * __CLRCALL_OR_CDECL FindSITargetTypeInstance (
    _RTTICompleteObjectLocator *pCOLocator, // pointer to Locator of complete object
    _RTTITypeDescriptor *pSrcTypeID,        // pointer to TypeDescriptor of source object
    _RTTITypeDescriptor *pTargetTypeID      // pointer to TypeDescriptor of result of cast
#if defined(_WIN64) && !defined(_M_CEE_PURE)
    , unsigned __int64 _ImageBase
#endif
    )
{
    _RTTIBaseClassDescriptor *pBCD, *pSourceBCD;
    _RTTIBaseClassArray *pBaseClassArray = CHD_PBCA(*COL_PCHD(*pCOLocator));
    DWORD nCompleteObjectBases = CHD_NUMBASES(*COL_PCHD(*pCOLocator));
    DWORD i, j;

    // Walk the BaseClassArray, which for single inheritance lists the complete
    // object and its base classes in order from most to least derived,
    // searching for the desired target type.

    for (i = 0; i < nCompleteObjectBases; i++)
    {
        pBCD = CHD_PBCD(BCA_BCDA(*pBaseClassArray)[i]);

        if (TYPEIDS_EQ(BCD_PTD(*pBCD), pTargetTypeID))
        {
            // Single inheritance implies no ambiguous bases, so we know we've
            // found the desired target.  Now search base classes of the target
            // sub-object looking for the desired source type.  Since we've got
            // single inheritance, the target's base classes are simply the
            // remaining base classes in the complete object's hierarchy.

            for (j = i + 1; j < nCompleteObjectBases; j++)
            {
                pSourceBCD = CHD_PBCD(BCA_BCDA(*pBaseClassArray)[j]);

                if (BCD_ATTRIBUTES(*pSourceBCD) & BCD_PRIVORPROTBASE)
                {
                    // If we find any non-public derivation between the target
                    // and source types, the cast fails.

                    return nullptr;
                }

                if (TYPEIDS_EQ(BCD_PTD(*pSourceBCD), pSrcTypeID))
                {
                    // We found the accessible source instance, the down-cast
                    // succeeds.

                    return pBCD;
                }
            }

            // We did not find the source as a base of the target, which means
            // we do not have a down-cast.  Since we can't have cross-casts
            // with single inheritance, this was either a failed up-cast to a
            // non-public base (else the compiler would statically cast), or
            // a cast from a type not in the complete object's hierarachy (which
            // could happen if someone is playing games with reinterpret_cast).
            // In either case, the cast fails.

            return nullptr;
        }
    }

    // We never found the target type in the complete object's hierarchy, so
    // the cast fails.

    return nullptr;
}


/////////////////////////////////////////////////////////////////////////////
//
// FindMITargetTypeInstance - workhorse routine of __RTDynamicCast() in a
// Multiple-Inheritance hierarchy
//
// Output: pointer to the appropriate sub-object of targetted type; nullptr if
// cast fails
//
// Side-effects: NONE.
//
// The C++ standard (5.2.7/8) describes the runtime check thusly (where v is
// the source expression, and T the desired destination class type):
//
// * If, in the most derived object pointed (referred) to by v, v points
//   (refers) to a public base class subobject of a T object, and if only one
//   object of type T is derived from the sub-object pointed (referred) to by
//   v, the result is a pointer (an lvalue referring) to that T object.
// * Otherwise, if v points (refers) to a public base class sub-object of the
//   most derived object, and the type of the most derived object has a base
//   class, of type T, that is unambiguous and public, the result is a pointer
//   (an lvalue referring) to the T sub-object of the most derived object.
// * Otherwise, the run-time check fails.
//
// The first bullet item describes a successful down-cast, and the second a
// successful cross-cast.  For multiple inheritance, there can only be one
// possible object of the target type derived from the particular source
// sub-object addressed by the source expression, since there's only a single
// path from the source sub-object to the most derived object, and the target
// type will only appear on that path zero or one times (multiple paths would
// require virtual inheritance).  The runtime rules can thus be modestly
// simplified to this:
//
// * If, in the most derived object pointed (referred) to by v, v points
//   (refers) to a public base class subobject of a T object, and if an object
//   of type T is derived from the sub-object pointed (referred) to by v, the
//   result is a pointer (an lvalue referring) to that T object.
// * Otherwise, if v points (refers) to a public base class sub-object of the
//   most derived object, and the type of the most derived object has a base
//   class, of type T, that is unambiguous and public, the result is a pointer
//   (an lvalue referring) to the T sub-object of the most derived object.
// * Otherwise, the run-time check fails.
//

static _RTTIBaseClassDescriptor * __CLRCALL_OR_CDECL FindMITargetTypeInstance (
    PVOID pCompleteObject,                  // pointer to complete object
    _RTTICompleteObjectLocator *pCOLocator, // pointer to Locator of complete object
    _RTTITypeDescriptor *pSrcTypeID,        // pointer to TypeDescriptor of source object
    ptrdiff_t SrcOffset,                    // offset of source object in complete object
    _RTTITypeDescriptor *pTargetTypeID      // pointer to TypeDescriptor of result of cast
#if defined(_WIN64) && !defined(_M_CEE_PURE)
    , unsigned __int64 _ImageBase
#endif
    )
{
    _RTTIBaseClassDescriptor *pBCD;
    _RTTIBaseClassDescriptor *pTargetBCD = nullptr;
    _RTTIBaseClassDescriptor *pSourceBCD = nullptr;
    _RTTIBaseClassDescriptor *pSourceInTargetBCD;
    _RTTIBaseClassArray *pBaseClassArray = CHD_PBCA(*COL_PCHD(*pCOLocator));
    _RTTIBaseClassArray *pTargetBaseClassArray;
    DWORD i;
    DWORD nCompleteObjectBases = CHD_NUMBASES(*COL_PCHD(*pCOLocator));
    DWORD nTargetBases = 0;
    DWORD iTarget = (DWORD)-1;

    // Walk the BaseClassArray, which lists the complete object's base class
    // hierarchy in depth-first left-to-right base class order, with the type
    // of the complete object at pBaseClassArray[0].  Look for down-casts
    // (5.2.7/8 bullet 1) and cross-casts (bullet 2) in a single pass through
    // the array.  If we've got an up-cast that wasn't resolved statically
    // because the target base was inaccessible or ambiguous within the derived
    // source, we'll detect it as a failed cross-cast since the target will also
    // be inaccessible or ambiguous within the complete object.
    //
    // With multiple inheritance, we can stop our walk as soon as we've seen
    // both the source and target types, since for MI down-casts, an ambiguous
    // target isn't possible (thanks to the unambiguous path from the source
    // sub-object to the most derived object), and for cross-cast, all instances
    // of the target type will be marked ambiguous if there are more than one.
    // Compare that with the corresponding walk in the virtual inheritance case,
    // where we may have to walk the entire hierarchy.

    for (i = 0; i < nCompleteObjectBases; i++)
    {
        pBCD = CHD_PBCD(BCA_BCDA(*pBaseClassArray)[i]);

        // Test if we've found an instance of the target class.  We can skip
        // the type-id check while walking through any base classes of the
        // target class.

        if (i - iTarget > nTargetBases &&
            TYPEIDS_EQ(BCD_PTD(*pBCD), pTargetTypeID))
        {
            // If we've already found the source class instance, then we must
            // have either a cross-cast or an up-cast.  The target must be
            // public and unambiguous in the complete object, and the source
            // must be public in the complete object, or the cast fails.  The
            // cast will always fail in the up-cast case, otherwise a runtime
            // check would not have been generated by the compiler.

            if (pSourceBCD != nullptr)
            {
                if ((BCD_ATTRIBUTES(*pBCD) & (BCD_NOTVISIBLE | BCD_AMBIGUOUS)) ||
                    (BCD_ATTRIBUTES(*pSourceBCD) & BCD_NOTVISIBLE))
                {
                    return nullptr;
                }
                else
                {
                    return pBCD;
                }
            }

            // Remember where we found the most recent instance of the target
            // class, as well as how many base classes of that target we will
            // encounter.

            pTargetBCD = pBCD;
            iTarget = i;
            nTargetBases = BCD_NUMCONTBASES(*pBCD);
        }

        // Test if we've found the proper instance of the source class.

        if (TYPEIDS_EQ(BCD_PTD(*pBCD), pSrcTypeID) &&
            PMDtoOffset(pCompleteObject, BCD_WHERE(*pBCD)) == SrcOffset)
        {
            // If we've already found an instance of the target class, check
            // if we're within the base classes of that target instance.  If
            // yes, we've got a down-cast, otherwise we've got a cross-cast or
            // failed up-cast.

            if (pTargetBCD != nullptr)
            {
                if (i - iTarget <= nTargetBases)
                {
                    // It's a down-cast.  The source class must be a public
                    // base of the target class, or the cast fails.

                    if (!(BCD_ATTRIBUTES(*pTargetBCD) & BCD_HASPCHD))
                    {
                        // We've got an older form of the RTTI data without the
                        // link to the class hierarchy descriptor.  We can only
                        // test public visibility of the source within the
                        // target if we're casting to the complete object's type
                        // (when iTarget is zero), otherwise allow the cast to
                        // succeed.

                        if (iTarget == 0 &&
                            BCD_ATTRIBUTES(*pBCD) & BCD_NOTVISIBLE)
                        {
                            return nullptr;
                        }
                        else
                        {
                            return pTargetBCD;
                        }
                    }

                    // Check the target class' class hierarchy descriptor to
                    // determine if the source class is publicly visible from
                    // the target class.  We index into the BaseClassArray of
                    // the target type, which will be layed out in the same
                    // depth-first, left-to-right order as the corresponding
                    // subset of the complete object's BaseClassArray from
                    // pBaseClassArray[iTarget .. iTarget+nTargetBases].

                    pTargetBaseClassArray = CHD_PBCA(*BCD_PCHD(*pTargetBCD));
                    pSourceInTargetBCD = CHD_PBCD(BCA_BCDA(*pTargetBaseClassArray)[i-iTarget]);

                    if (BCD_ATTRIBUTES(*pSourceInTargetBCD) & BCD_NOTVISIBLE)
                    {
                        return nullptr;
                    }
                    else
                    {
                        return pTargetBCD;
                    }
                }
                else
                {
                    // It's a cross-cast or (failed) up-cast.  The target must
                    // be public and unambiguous in the complete object, and the
                    // source must be public in the complete object.

                    if ((BCD_ATTRIBUTES(*pTargetBCD) & (BCD_NOTVISIBLE | BCD_AMBIGUOUS)) ||
                        (BCD_ATTRIBUTES(*pBCD) & BCD_NOTVISIBLE))
                    {
                        return nullptr;
                    }
                    else
                    {
                        return pTargetBCD;
                    }
                }
            }

            // Remember that we've found the source class instance

            pSourceBCD = pBCD;
        }
    }

    // Either the complete object does not contain any instances of the target
    // class, or we never found the source instance (which only happens if
    // we've got corrupted RTTI info or the source pointer we were passed was
    // mistyped).  In either case, the cast fails.

    return nullptr;
}


/////////////////////////////////////////////////////////////////////////////
//
// FindVITargetTypeInstance - workhorse routine of __RTDynamicCast() in a
// Virtual-Inheritance hierarchy
//
// Output: pointer to the appropriate sub-object of targetted type; nullptr if
// cast fails
//
// Side-effects: NONE.
//
// The C++ standard (5.2.7/8) describes the runtime check thusly (where v is
// the source expression, and T the desired destination class type):
//
// * If, in the most derived object pointed (referred) to by v, v points
//   (refers) to a public base class subobject of a T object, and if only one
//   object of type T is derived from the sub-object pointed (referred) to by
//   v, the result is a pointer (an lvalue referring) to that T object.
// * Otherwise, if v points (refers) to a public base class sub-object of the
//   most derived object, and the type of the most derived object has a base
//   class, of type T, that is unambiguous and public, the result is a pointer
//   (an lvalue referring) to the T sub-object of the most derived object.
// * Otherwise, the run-time check fails.
//
// The first bullet item describes a successful down-cast, and the second a
// successful cross-cast.  For virtual inheritance, these rules must be
// followed without simplification.
//

static _RTTIBaseClassDescriptor * __CLRCALL_OR_CDECL FindVITargetTypeInstance (
    PVOID pCompleteObject,                  // pointer to complete object
    _RTTICompleteObjectLocator *pCOLocator, // pointer to Locator of complete object
    _RTTITypeDescriptor *pSrcTypeID,        // pointer to TypeDescriptor of source object
    ptrdiff_t SrcOffset,                    // offset of source object in complete object
    _RTTITypeDescriptor *pTargetTypeID      // pointer to TypeDescriptor of result of cast
#if defined(_WIN64) && !defined(_M_CEE_PURE)
    , unsigned __int64 _ImageBase
#endif
    )
{
    _RTTIBaseClassDescriptor *pBCD;
    _RTTIBaseClassDescriptor *pDownCastResultBCD = nullptr;
    _RTTIBaseClassDescriptor *pCrossCastSourceBCD = nullptr;
    _RTTIBaseClassDescriptor *pCrossCastTargetBCD = nullptr;
    _RTTIBaseClassDescriptor *pTargetBCD = nullptr;
    _RTTIBaseClassDescriptor *pSourceInTargetBCD;
    _RTTIBaseClassArray *pBaseClassArray = CHD_PBCA(*COL_PCHD(*pCOLocator));
    _RTTIBaseClassArray *pTargetBaseClassArray;
    DWORD i;
    DWORD nCompleteObjectBases = CHD_NUMBASES(*COL_PCHD(*pCOLocator));
    DWORD nTargetBases = 0;
    DWORD iTarget = (DWORD)-1;
    bool fDownCastAllowed = true;
    bool fDirectlyPublic;
    ptrdiff_t offsetTarget;
    ptrdiff_t offsetDownCastResult = -1;

    // Walk the BaseClassArray, which lists the complete object's base class
    // hierarchy in depth-first left-to-right base class order, with the type
    // of the complete object at pBaseClassArray[0].  Look for down-casts
    // (5.2.7/8 bullet 1) and cross-casts (bullet 2) in a single pass through
    // the array.  If we've got an up-cast that wasn't resolved statically
    // because the target base was inaccessible or ambiguous within the derived
    // source, we'll detect it as cross-cast, which may or may not succeed (see
    // the comments above in __RTDynamicCast).
    //
    // For virtual inheritance, we may have to walk the entire hierarchy even
    // after we see both the source and target types.  That's to detect a
    // down-cast to an ambiguous target because of an intervening virtual
    // derivation, e.g. dynamic_cast from A* to B* in this E hierarchy:
    //       A
    //      / \
    //     B   B
    //     |   |
    //     C   D
    //      \ /
    //       E

    for (i = 0; i < nCompleteObjectBases; i++)
    {
        pBCD = CHD_PBCD(BCA_BCDA(*pBaseClassArray)[i]);

        // Test if we've found an instance of the target class.  We can skip
        // the type-id check while walking through any base classes of the
        // target class.

        if (i - iTarget > nTargetBases &&
            TYPEIDS_EQ(BCD_PTD(*pBCD), pTargetTypeID))
        {
            // If this target instance is public and unambiguous within the
            // complete object, remember it as a potential target of a
            // cross-cast.

            if (!(BCD_ATTRIBUTES(*pBCD) & (BCD_NOTVISIBLE | BCD_AMBIGUOUS)))
            {
                pCrossCastTargetBCD = pBCD;
            }

            // Remember where we found the most recent instance of the target
            // class, as well as how many base classes of that target we will
            // encounter.

            pTargetBCD = pBCD;
            iTarget = i;
            nTargetBases = BCD_NUMCONTBASES(*pBCD);
        }

        // Test if we've found the proper instance of the source class.

        if (TYPEIDS_EQ(BCD_PTD(*pBCD), pSrcTypeID) &&
            PMDtoOffset(pCompleteObject, BCD_WHERE(*pBCD)) == SrcOffset)
        {
            // If we're within the base classes of a previously-seen instance
            // of the target class, then we've got a down-cast.

            if (i - iTarget <= nTargetBases)
            {
                // We can skip down-cast checking if we've previously
                // determined that a down-cast isn't allowed, while examining
                // an earlier instance of the source class sub-object which was
                // virtually derived, because that source instance wasn't
                // publicly visible from the target class.

                if (fDownCastAllowed)
                {
                    // A potential down-cast is valid if the source class is
                    // public within the target class, and if only one instance
                    // of the target class derives from the source instance.
                    // First check for public visibility of the source.

                    if (!(BCD_ATTRIBUTES(*pTargetBCD) & BCD_HASPCHD))
                    {
                        // We've got an older form of the RTTI data without the
                        // link to the class hierarchy descriptor.  We can only
                        // test public visibility of the source within the
                        // target if we're casting to the complete object's type
                        // (when iTarget is zero), otherwise allow the cast to
                        // succeed.

                        if (iTarget == 0 &&
                            (BCD_ATTRIBUTES(*pBCD) & BCD_NOTVISIBLE))
                        {
                            fDownCastAllowed = false;
                        }

                        // If BCD_HASPCHD wasn't set, then BCD_PRIVORPROTBASE
                        // won't be, either.

                        fDirectlyPublic = true;
                    }
                    else
                    {
                        // Check the target class' class hierarchy descriptor
                        // to determine if the source class is publicly visible
                        // from the target class.  We index into the
                        // BaseClassArray of the target type, which will be
                        // layed out in the same depth-first, left-to-right
                        // order as the corresponding subset of the complete
                        // object's BaseClassArray from pBaseClassArray[iTarget
                        // .. iTarget+nTargetBases].

                        pTargetBaseClassArray = CHD_PBCA(*BCD_PCHD(*pTargetBCD));
                        pSourceInTargetBCD = CHD_PBCD(BCA_BCDA(*pTargetBaseClassArray)[i-iTarget]);

                        if (BCD_ATTRIBUTES(*pSourceInTargetBCD) & BCD_NOTVISIBLE)
                        {
                            fDownCastAllowed = false;
                        }

                        // The above check for source visibility in the target
                        // may be wrong, thanks to incorrect compiler-generated
                        // RTTI data in cases where a base class is both
                        // virtually and non-virtually inherited in a class
                        // hierarchy.  E.g.:
                        //
                        //        A
                        //        |
                        //        B   A
                        //       / \ /
                        //      C   D
                        //       \ /
                        //        E
                        //
                        // If the virtual base at B::A is private from E, but
                        // the non-virtual D::A is public from E, the RTTI data
                        // can incorrectly state that both instances of A are
                        // visible.  We can work around some cases of this by
                        // checking if the source instance is directly inherited
                        // non-publicly.  Given dynamic_cast<C*>((A*)(B*)new E),
                        // we can properly fail the cast when B -> A is private
                        // (and everything else public) by checking
                        // BCD_PRIVORPROTBASE, but will still incorrectly allow
                        // the cast to succeed when C -> B and D -> B are both
                        // virtual private (and everything else public).

                        fDirectlyPublic = !(BCD_ATTRIBUTES(*pSourceInTargetBCD) & BCD_PRIVORPROTBASE);
                    }

                    if (fDownCastAllowed && fDirectlyPublic)
                    {
                        // The source instance is visible within the target, so
                        // now check if a different target instance has already
                        // been seen in a previous down-cast.

                        offsetTarget = PMDtoOffset(pCompleteObject, BCD_WHERE(*pTargetBCD));
                        if (pDownCastResultBCD != nullptr &&
                            offsetDownCastResult != offsetTarget)
                        {
                            // The source instance can down-cast to multiple
                            // separate instances of the target class, so
                            // down-casting fails.  But this also means the
                            // target type is ambiguous within the complete
                            // object, so cross-casting fails as well, and we
                            // can fail early.

                            return nullptr;
                        }

                        // We have an unambiguous target, so a down-cast is possible
                        // (so far).

                        pDownCastResultBCD = pTargetBCD;
                        offsetDownCastResult = offsetTarget;
                    }
                }
            }
            else
            {
                // If we're not within the base classes of a target instance,
                // then this source instance is a possible cross-cast source.
                // We do not check for a cross-cast if it's also a possible
                // down-cast.  First, it's not necessary - if a down-cast was
                // not possible because of accessibility, then the source
                // instance along the path through the target instance would
                // also not be public within the complete object, and not a
                // valid cross-cast source.  Second, testing in this order works
                // around a bug in the C++ compiler's generated RTTI data.
                // Consider this hierarchy:
                //
                //      struct A { virtual void a(); };
                //      struct B : virtual private A { virtual void b(); };
                //      struct C : public A { virtual void c(); };
                //      struct D : public B, public C { virtual void d(); };
                //
                //      A   A
                //      !   |
                //      B   C
                //       \ /
                //        D
                //
                // The virtual A at D::B::A is not accessible from D.  But the
                // RTTI info says it is accessible, because D::C::A is visible,
                // even though those are two different instances of A.
                //
                // As for the down-cast case (see the comments before setting
                // fDirectlyPublic above), we can check BCD_PRIVORPROTBASE to
                // work around the bad RTTI data in some, but not all, cases.

                if (!(BCD_ATTRIBUTES(*pBCD) & (BCD_NOTVISIBLE | BCD_PRIVORPROTBASE)))
                {
                    // If this source instance is public within the complete object,
                    // remember it as a potential source of a cross-cast.

                    pCrossCastSourceBCD = pBCD;
                }
            }
        }
    }

    // A down-cast is preferred to a cross-cast/up-cast, so check for that
    // first.

    if (fDownCastAllowed && pDownCastResultBCD != nullptr)
    {
        return pDownCastResultBCD;
    }

    // Return a successful cast if we've found both sides of a cross-cast.

    if (pCrossCastSourceBCD != nullptr && pCrossCastTargetBCD != nullptr)
    {
        return pCrossCastTargetBCD;
    }

    // Otherwise we didn't find both sides of a cross-cast which were legally
    // reachable from the complete object (possibly a failed up-cast that
    // generated a runtime check).

    return nullptr;
}


/////////////////////////////////////////////////////////////////////////////
//
// PMDtoOffset - Calculate member offset from PMD & this
//
// Output: The offset of the base within the complete object.
//
// Side-effects: NONE.
//

static ptrdiff_t __CLRCALL_OR_CDECL PMDtoOffset(
    PVOID pThis,            // ptr to complete object
    const PMD& pmd)         // pointer-to-member-data structure
{
    ptrdiff_t RetOff = 0;

    if (pmd.pdisp >= 0) {
        // if base is in the virtual part of class
        RetOff = pmd.pdisp;
        RetOff += *(__int32*)((char*)*(ptrdiff_t*)((char*)pThis + RetOff) +
                                pmd.vdisp);
    }

    RetOff += pmd.mdisp;

    return RetOff;
}

#if defined(_WIN64) && !defined(_M_CEE_PURE)
static unsigned __int64 GetImageBase(PVOID pCallerPC)
{
    unsigned __int64 _ImageBase;
    _ImageBase = (unsigned __int64)RtlPcToFileHeader(
        pCallerPC,
        (PVOID*)&_ImageBase);
    return _ImageBase;
}
#endif
