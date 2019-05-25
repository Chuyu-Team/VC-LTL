// yvals_core.h internal header (core)
#pragma once
#ifndef _YVALS_CORE_H
#define _YVALS_CORE_H
#ifndef RC_INVOKED

// _HAS_CXX20 directly controls:
// P0318R1 unwrap_reference, unwrap_ref_decay
// P0457R2 starts_with()/ends_with() For basic_string/basic_string_view
// P0458R2 contains() For Ordered And Unordered Associative Containers
// P0550R2 remove_cvref
// P0646R1 list/forward_list remove()/remove_if()/unique() Return size_type
// P0768R1 Library Support For The Spaceship Comparison Operator <=>
//     (partially implemented)
// P0769R2 shift_left(), shift_right()
// P0887R1 type_identity
// P0898R3 Standard Library Concepts
//     * std::identity
//     (other components not yet implemented)
// P????R? directory_entry::clear_cache()

// _HAS_CXX17 directly controls:
// P0005R4 not_fn()
// P0024R2 Parallel Algorithms
// P0025R1 clamp()
// P0030R1 hypot(x, y, z)
// P0031R0 constexpr For <array> (Again) And <iterator>
// P0032R3 Homogeneous Interface For variant/any/optional
// P0040R3 Extending Memory Management Tools
// P0067R5 Elementary String Conversions
//     (partially implemented)
// P0083R3 Splicing Maps And Sets
// P0084R2 Emplace Return Type
// P0088R3 <variant>
// P0137R1 launder()
// P0152R1 atomic::is_always_lock_free
// P0154R1 hardware_destructive_interference_size, etc.
// P0156R2 scoped_lock
// P0163R0 shared_ptr::weak_type
// P0185R1 is_swappable, is_nothrow_swappable
// P0209R2 make_from_tuple()
// P0218R1 <filesystem>
// P0220R1 <any>, <memory_resource>, <optional>, <string_view>, apply(), sample(), Boyer-Moore search()
// P0253R1 Fixing Searcher Return Types
// P0254R2 Integrating string_view And std::string
// P0258R2 has_unique_object_representations
// P0272R1 Non-const basic_string::data()
// P0295R0 gcd(), lcm()
// P0307R2 Making Optional Greater Equal Again
// P0336R1 Renaming Parallel Execution Policies
// P0337R0 Deleting polymorphic_allocator Assignment
// P0358R1 Fixes For not_fn()
// P0393R3 Making Variant Greater Equal
// P0394R4 Parallel Algorithms Should terminate() For Exceptions
// P0403R1 UDLs For <string_view> ("meow"sv, etc.)
// P0426R1 constexpr For char_traits
// P0433R2 Deduction Guides For The STL
// P0452R1 Unifying <numeric> Parallel Algorithms
// P0504R0 Revisiting in_place_t/in_place_type_t<T>/in_place_index_t<I>
// P0505R0 constexpr For <chrono> (Again)
// P0508R0 Clarifying insert_return_type
// P0510R0 Rejecting variants Of Nothing, Arrays, References, And Incomplete Types
// P0602R4 Propagating Copy/Move Triviality In variant/optional
// P0604R0 invoke_result, is_invocable, is_nothrow_invocable
// P0607R0 Inline Variables For The STL
// P0682R1 Repairing Elementary String Conversions
// P0739R0 Improving Class Template Argument Deduction For The STL
// P0858R0 Constexpr Iterator Requirements

// _HAS_CXX17 indirectly controls:
// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
// P0003R5 Removing Dynamic Exception Specifications
// P0004R1 Removing Deprecated Iostreams Aliases
// P0226R1 Mathematical Special Functions
// P0298R3 std::byte
// P0302R1 Removing Allocator Support In std::function
// LWG 2385 function::assign allocator argument doesn't make sense
// LWG 2921 packaged_task and type-erased allocators
// LWG 2976 Dangling uses_allocator specialization for packaged_task
// The non-Standard std::tr1 namespace and TR1-only machinery
// The non-Standard std::tr2::sys namespace
// Enforcement of matching allocator value_types
// [[nodiscard]] attributes on STL functions

// _HAS_CXX17 and _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS control:
// P0174R2 Deprecating Vestigial Library Parts
// P0521R0 Deprecating shared_ptr::unique()
// P0618R0 Deprecating <codecvt>
// Other C++17 deprecation warnings

// Implemented unconditionally:
// N3911 void_t
// N4089 Safe Conversions In unique_ptr<T[]>
// N4169 invoke()
// N4258 noexcept Cleanups
// N4259 uncaught_exceptions()
// N4277 Trivially Copyable reference_wrapper
// N4279 insert_or_assign()/try_emplace() For map/unordered_map
// N4280 size(), empty(), data()
// N4366 Precisely Constraining unique_ptr Assignment
// N4387 Improving pair And tuple
// N4389 bool_constant
// N4508 shared_mutex (Untimed)
// N4510 Supporting Incomplete Types In vector/list/forward_list
// P0006R0 Variable Templates For Type Traits (is_same_v, etc.)
// P0007R1 as_const()
// P0013R1 Logical Operator Type Traits (conjunction, etc.)
// P0033R1 Rewording enable_shared_from_this
// P0063R3 C11 Standard Library
// P0074R0 owner_less<>
// P0092R1 <chrono> floor(), ceil(), round(), abs()
// P0414R2 shared_ptr<T[]>, shared_ptr<T[N]>
// P0418R2 atomic compare_exchange memory_order Requirements
// P0435R1 Overhauling common_type
// P0497R0 Fixing shared_ptr For Arrays
// P0513R0 Poisoning hash
// P0516R0 Marking shared_future Copying As noexcept
// P0517R0 Constructing future_error From future_errc
// P0548R1 Tweaking common_type And duration
// P0558R1 Resolving atomic<T> Named Base Class Inconsistencies
// P0599R1 noexcept hash
// P0777R1 Avoiding Unnecessary decay
// P0809R0 Comparing Unordered Containers
// P0941R2 Feature-Test Macros
// P0972R0 noexcept For <chrono> zero(), min(), max()
// D1164R0 Make create_directory() Intuitive

// Parallel Algorithms Notes
// C++ allows an implementation to implement parallel algorithms as calls to the
// serial algorithms. This implementation parallelizes several common algorithm
// calls, but not all.
//
// The following algorithms are parallelized.
// * adjacent_difference
// * adjacent_find
// * all_of
// * any_of
// * count
// * count_if
// * equal
// * exclusive_scan
// * find
// * find_end
// * find_first_of
// * find_if
// * find_if_not
// * for_each
// * for_each_n
// * inclusive_scan
// * is_heap
// * is_heap_until
// * is_partitioned
// * is_sorted
// * is_sorted_until
// * mismatch
// * none_of
// * partition
// * reduce
// * remove
// * remove_if
// * replace
// * replace_if
// * search
// * search_n
// * set_difference
// * set_intersection
// * sort
// * stable_sort
// * transform
// * transform_exclusive_scan
// * transform_inclusive_scan
// * transform_reduce
//
// The following are not presently parallelized:
//
// No apparent parallelism performance improvement on target hardware; all
// algorithms which merely copy or permute elements with no branches are
// typically memory bandwidth limited.
// * copy
// * copy_n
// * fill
// * fill_n
// * move
// * reverse
// * reverse_copy
// * rotate
// * rotate_copy
// * shift_left
// * shift_right
// * swap_ranges
//
// Confusion over user parallelism requirements exists; likely in the above
// category anyway.
// * generate
// * generate_n
//
// Effective parallelism suspected to be infeasible.
// * partial_sort
// * partial_sort_copy
//
// Not yet evaluated; parallelism may be implemented in a future release and is
// suspected to be beneficial.
// * copy_if
// * includes
// * inplace_merge
// * lexicographical_compare
// * max_element
// * merge
// * min_element
// * minmax_element
// * nth_element
// * partition_copy
// * remove_copy
// * remove_copy_if
// * replace_copy
// * replace_copy_if
// * set_symmetric_difference
// * set_union
// * stable_partition
// * unique
// * unique_copy

#if !defined(_HAS_CXX17) && !defined(_HAS_CXX20)
#if defined(_MSVC_LANG)
#define _STL_LANG _MSVC_LANG
#else // ^^^ use _MSVC_LANG ^^^ / vvv use __cplusplus vvv
#define _STL_LANG __cplusplus
#endif // ^^^ use __cplusplus ^^^

#if _STL_LANG > 201703L
#define _HAS_CXX17 1
#define _HAS_CXX20 1
#elif _STL_LANG > 201402L
#define _HAS_CXX17 1
#define _HAS_CXX20 0
#else // _STL_LANG <= 201402L
#define _HAS_CXX17 0
#define _HAS_CXX20 0
#endif // Use the value of _STL_LANG to define _HAS_CXX17 and _HAS_CXX20

#undef _STL_LANG
#endif // !defined(_HAS_CXX17) && !defined(_HAS_CXX20)

#if !defined(_HAS_CXX17) || !defined(_HAS_CXX20) || _HAS_CXX20 && !_HAS_CXX17
#error _HAS_CXX17 and _HAS_CXX20 must both be defined, and _HAS_CXX20 must imply _HAS_CXX17
#endif // Verify _HAS_CXX17 and _HAS_CXX20

#include <crtdefs.h>
#include <xkeycheck.h> // The _HAS_CXX tags must be defined before including this.

#ifndef _STL_WARNING_LEVEL
#if defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
#define _STL_WARNING_LEVEL 4
#else // defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
#define _STL_WARNING_LEVEL 3
#endif // defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
#endif // _STL_WARNING_LEVEL

#if _STL_WARNING_LEVEL < 3
#error _STL_WARNING_LEVEL cannot be less than 3.
#endif // _STL_WARNING_LEVEL < 3

#if _STL_WARNING_LEVEL > 4
#error _STL_WARNING_LEVEL cannot be greater than 4.
#endif // _STL_WARNING_LEVEL > 4

// Controls whether the STL uses "if constexpr" internally
#ifndef _HAS_IF_CONSTEXPR
#ifdef __NVCC__
#define _HAS_IF_CONSTEXPR 0
#else // __NVCC__
#define _HAS_IF_CONSTEXPR 1
#endif // __NVCC__
#endif // _HAS_IF_CONSTEXPR

// warning C4577: 'noexcept' used with no exception handling mode specified;
// termination on exception is not guaranteed. Specify /EHsc
#if _HAS_EXCEPTIONS
#define _STL_DISABLED_WARNING_C4577
#else // _HAS_EXCEPTIONS
#define _STL_DISABLED_WARNING_C4577 4577
#endif // _HAS_EXCEPTIONS

// error C4984: 'if constexpr' is a C++17 language extension
#if !_HAS_CXX17 && _HAS_IF_CONSTEXPR
#define _STL_DISABLED_WARNING_C4984 4984
#else // !_HAS_CXX17 && _HAS_IF_CONSTEXPR
#define _STL_DISABLED_WARNING_C4984
#endif // !_HAS_CXX17 && _HAS_IF_CONSTEXPR

#ifndef _STL_EXTRA_DISABLED_WARNINGS
#define _STL_EXTRA_DISABLED_WARNINGS
#endif // _STL_EXTRA_DISABLED_WARNINGS

// warning C4455: literal suffix identifiers that do not start with an underscore are reserved
// warning C4494: Ignoring __declspec(allocator) because the function return
// type is not a pointer or reference
// warning C4583: 'union': destructor not implicitly called
// warning C4619: #pragma warning: there is no warning number 'number'
// warning C4643: Forward declaring 'meow' in namespace std is not permitted by the C++ Standard.
// warning C4702: unreachable code
// warning C4793: function compiled as native
// warning C4988: variable declared outside class/function scope (/Wall /d1WarnOnGlobals)
#ifndef _STL_DISABLED_WARNINGS
// clang-format off
#define _STL_DISABLED_WARNINGS         \
    4455 4494 4583 4619 4643 4702 4793 4988 \
    _STL_DISABLED_WARNING_C4577        \
    _STL_DISABLED_WARNING_C4984        \
    _STL_EXTRA_DISABLED_WARNINGS
// clang-format on
#endif // _STL_DISABLED_WARNINGS

// warning: constexpr if is a C++17 extension [-Wc++17-extensions]
// warning: user-defined literal suffixes not starting with '_' are reserved [-Wuser-defined-literals]
// warning: unknown pragma ignored [-Wunknown-pragmas]
#ifndef _STL_DISABLE_CLANG_WARNINGS
#ifdef __clang__
// clang-format off
#define _STL_DISABLE_CLANG_WARNINGS                                 \
    _Pragma("clang diagnostic push")                                \
    _Pragma("clang diagnostic ignored \"-Wc++17-extensions\"")      \
    _Pragma("clang diagnostic ignored \"-Wuser-defined-literals\"") \
    _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")
// clang-format on
#else // __clang__
#define _STL_DISABLE_CLANG_WARNINGS
#endif // __clang__
#endif // _STL_DISABLE_CLANG_WARNINGS

#ifndef _STL_RESTORE_CLANG_WARNINGS
#ifdef __clang__
#define _STL_RESTORE_CLANG_WARNINGS _Pragma("clang diagnostic pop")
#else // __clang__
#define _STL_RESTORE_CLANG_WARNINGS
#endif // __clang__
#endif // _STL_RESTORE_CLANG_WARNINGS

#define _CPPLIB_VER 650
#define _MSVC_STL_VERSION 142
#define _MSVC_STL_UPDATE 201903L

#ifndef _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#ifdef __EDG__
// not attempting to detect __EDG_VERSION__ being less than expected
#elif defined(__clang__)
#if __clang_major__ < 7 // Coarse-grained, not inspecting __clang_minor__ and __clang_patchlevel__
#error STL1000: Unexpected compiler version, expected Clang 7 or newer.
#endif // ^^^ old Clang ^^^
#elif defined(_MSC_VER)
#if _MSC_VER < 1920 // Coarse-grained, not inspecting _MSC_FULL_VER
#error STL1001: Unexpected compiler version, expected MSVC 19.20 or newer.
#endif // ^^^ old MSVC ^^^
#else // vvv other compilers vvv
// not attempting to detect other compilers
#endif // ^^^ other compilers ^^^
#endif // _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH

#if _HAS_EXCEPTIONS
#define _NOEXCEPT_COND(...) noexcept(__VA_ARGS__)
#define _NOEXCEPT_OPER(...) noexcept(__VA_ARGS__)
#else // _HAS_EXCEPTIONS
#define _NOEXCEPT_COND(...) noexcept
#define _NOEXCEPT_OPER(...) true
#endif // _HAS_EXCEPTIONS

#ifndef _HAS_STATIC_RTTI
#define _HAS_STATIC_RTTI 1
#endif // _HAS_STATIC_RTTI

#if defined(_CPPRTTI) && !_HAS_STATIC_RTTI
#error /GR implies _HAS_STATIC_RTTI.
#endif // defined(_CPPRTTI) && !_HAS_STATIC_RTTI

// C++17 constexpr additions
#if _HAS_CXX17
#define _CONSTEXPR17 constexpr
#else // ^^^ has C++17 constexpr additions ^^^ / vvv no C++17 constexpr additions vvv
#define _CONSTEXPR17 inline
#endif // _HAS_CXX17

// P0607R0 Inline Variables For The STL
#if _HAS_CXX17
#define _INLINE_VAR inline
#else // _HAS_CXX17
#define _INLINE_VAR
#endif // _HAS_CXX17

// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
#ifndef _HAS_AUTO_PTR_ETC
#define _HAS_AUTO_PTR_ETC (!_HAS_CXX17)
#endif // _HAS_AUTO_PTR_ETC

// P0003R5 Removing Dynamic Exception Specifications
#ifndef _HAS_UNEXPECTED
#define _HAS_UNEXPECTED (!_HAS_CXX17)
#endif // _HAS_UNEXPECTED

// P0004R1 Removing Deprecated Iostreams Aliases
#ifndef _HAS_OLD_IOSTREAMS_MEMBERS
#define _HAS_OLD_IOSTREAMS_MEMBERS (!_HAS_CXX17)
#endif // _HAS_OLD_IOSTREAMS_MEMBERS

// P0067R5 Elementary String Conversions
#ifndef _HAS_COMPLETE_CHARCONV
#define _HAS_COMPLETE_CHARCONV 0
#endif // _HAS_COMPLETE_CHARCONV

// P0226R1 Mathematical Special Functions
#ifndef _HAS_SPECIAL_MATH
#define _HAS_SPECIAL_MATH _HAS_CXX17
#endif // _HAS_SPECIAL_MATH

// P0298R3 std::byte
#ifndef _HAS_STD_BYTE
#define _HAS_STD_BYTE _HAS_CXX17 // inspected by GSL, do not remove
#endif // _HAS_STD_BYTE

// P0302R1 Removing Allocator Support In std::function
// LWG 2385 function::assign allocator argument doesn't make sense
// LWG 2921 packaged_task and type-erased allocators
// LWG 2976 Dangling uses_allocator specialization for packaged_task
#ifndef _HAS_FUNCTION_ALLOCATOR_SUPPORT
#define _HAS_FUNCTION_ALLOCATOR_SUPPORT (!_HAS_CXX17)
#endif // _HAS_FUNCTION_ALLOCATOR_SUPPORT

// The non-Standard std::tr1 namespace and TR1-only machinery
#ifndef _HAS_TR1_NAMESPACE
#define _HAS_TR1_NAMESPACE (!_HAS_CXX17)
#endif // _HAS_TR1_NAMESPACE

#if _HAS_TR1_NAMESPACE
#ifdef _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _DEPRECATE_TR1_NAMESPACE
#else // _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _DEPRECATE_TR1_NAMESPACE                                                                                  \
    [[deprecated(                                                                                                 \
        "warning STL4002: "                                                                                       \
        "The non-Standard std::tr1 namespace and TR1-only machinery are deprecated and will be REMOVED. You can " \
        "define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING to acknowledge that you have received this warning.")]]
#endif // _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#endif // _HAS_TR1_NAMESPACE

// STL4003 was "The non-Standard std::identity struct is deprecated and will be REMOVED."

// The non-Standard std::tr2::sys namespace
#ifndef _HAS_TR2_SYS_NAMESPACE
#define _HAS_TR2_SYS_NAMESPACE (!_HAS_CXX17)
#endif // _HAS_TR2_SYS_NAMESPACE

#if _HAS_TR2_SYS_NAMESPACE
#ifdef _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING
#define _DEPRECATE_TR2_SYS_NAMESPACE
#else // _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING
#define _DEPRECATE_TR2_SYS_NAMESPACE                                                   \
    [[deprecated(                                                                      \
        "warning STL4018: "                                                            \
        "The non-Standard std::tr2::sys namespace is deprecated and will be REMOVED. " \
        "It is superseded by std::filesystem. You can "                                \
        "define _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING to acknowledge that you have received this warning.")]]
#endif // _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING
#endif // _HAS_TR2_SYS_NAMESPACE

// Enforcement of matching allocator value_types
#ifndef _ENFORCE_MATCHING_ALLOCATORS
#define _ENFORCE_MATCHING_ALLOCATORS _HAS_CXX17
#endif // _ENFORCE_MATCHING_ALLOCATORS

#define _MISMATCHED_ALLOCATOR_MESSAGE(_CONTAINER, _VALUE_TYPE)                                 \
    _CONTAINER " requires that Allocator's value_type match " _VALUE_TYPE                      \
               " (See N4659 26.2.1 [container.requirements.general]/16 allocator_type)"        \
               " Either fix the allocator value_type or define _ENFORCE_MATCHING_ALLOCATORS=0" \
               " to suppress this diagnostic."

// Enforcement of Standard facet specializations
#ifndef _ENFORCE_FACET_SPECIALIZATIONS
#define _ENFORCE_FACET_SPECIALIZATIONS 0
#endif // _ENFORCE_FACET_SPECIALIZATIONS

#define _FACET_SPECIALIZATION_MESSAGE                                                  \
    "Unsupported facet specialization; see N4800 27.3.1.1.1 [locale.category]. "       \
    "Either use a Standard specialization or define _ENFORCE_FACET_SPECIALIZATIONS=0 " \
    "to suppress this diagnostic."

// [[nodiscard]] attributes on STL functions
#ifndef _HAS_NODISCARD
#define _HAS_NODISCARD _HAS_CXX17
#endif // _HAS_NODISCARD

#if _HAS_NODISCARD
#define _NODISCARD [[nodiscard]]
#define _NODISCARD_PERF
#else // ^^^ CAN HAZ [[nodiscard]] ^^^ // vvv NO CAN HAZ [[nodiscard]] vvv
#define _NODISCARD
#define _NODISCARD_PERF
#endif // _HAS_NODISCARD

#if _HAS_IF_CONSTEXPR
#define _CONSTEXPR_IF constexpr
#else // _HAS_IF_CONSTEXPR
#define _CONSTEXPR_IF
#endif // _HAS_IF_CONSTEXPR

// Controls whether the STL will force /fp:fast to enable vectorization of algorithms defined
// in the standard as special cases; such as reduce, transform_reduce, inclusive_scan, exclusive_scan
#ifndef _STD_VECTORIZE_WITH_FLOAT_CONTROL
#ifdef _M_FP_EXCEPT
#define _STD_VECTORIZE_WITH_FLOAT_CONTROL 0
#else // ^^^ floating point exceptions enabled ^^^ // vvv floating point exceptions disabled (default) vvv
#define _STD_VECTORIZE_WITH_FLOAT_CONTROL 1
#endif // _M_FP_EXCEPT
#endif // _STD_VECTORIZE_WITH_FLOAT_CONTROL

// P0174R2 Deprecating Vestigial Library Parts
// P0521R0 Deprecating shared_ptr::unique()
// Other C++17 deprecation warnings

// N4659 D.4 [depr.cpp.headers]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_C_HEADER                                                               \
    [[deprecated("warning STL4004: "                                                            \
                 "<ccomplex>, <cstdalign>, <cstdbool>, and <ctgmath> are deprecated in C++17. " \
                 "You can define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING "                  \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_C_HEADER
#endif // ^^^ warning disabled ^^^

// N4659 D.6 [depr.str.strstreams]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_STRSTREAM                                              \
    [[deprecated("warning STL4005: <strstream> is deprecated in C++17. "        \
                 "You can define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_STRSTREAM
#endif // ^^^ warning disabled ^^^

// N4659 D.7 [depr.uncaught]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_UNCAUGHT_EXCEPTION                                              \
    [[deprecated("warning STL4006: "                                                     \
                 "std::uncaught_exception() is deprecated in C++17. "                    \
                 "It is superseded by std::uncaught_exceptions(), plural. "              \
                 "You can define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_UNCAUGHT_EXCEPTION
#endif // ^^^ warning disabled ^^^

// N4659 D.8.1 [depr.weak.result_type]
// N4659 D.8.2 [depr.func.adaptor.typedefs]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS                                                                         \
    [[deprecated(                                                                                                 \
        "warning STL4007: Many result_type typedefs "                                                             \
        "and all argument_type, first_argument_type, and second_argument_type typedefs are deprecated in C++17. " \
        "You can define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING "                                     \
        "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS
#endif // ^^^ warning disabled ^^^

// N4659 D.8.3 [depr.negators]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_NEGATORS                                                                                \
    [[deprecated("warning STL4008: "                                                                             \
                 "std::not1(), std::not2(), std::unary_negate, and std::binary_negate are deprecated in C++17. " \
                 "They are superseded by std::not_fn(). "                                                        \
                 "You can define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING "                                   \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_NEGATORS
#endif // ^^^ warning disabled ^^^

// STL4009 was "std::allocator<void> is deprecated in C++17"

// N4659 D.9 [depr.default.allocator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS                                              \
    [[deprecated("warning STL4010: "                                                        \
                 "Various members of std::allocator are deprecated in C++17. "              \
                 "Use std::allocator_traits instead of accessing these members directly. "  \
                 "You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS
#endif // ^^^ warning disabled ^^^

// N4659 D.10 [depr.storage.iterator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_RAW_STORAGE_ITERATOR                                                  \
    [[deprecated("warning STL4011: "                                                           \
                 "std::raw_storage_iterator is deprecated in C++17. "                          \
                 "Consider using the std::uninitialized_copy() family of algorithms instead. " \
                 "You can define _SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING "     \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_RAW_STORAGE_ITERATOR
#endif // ^^^ warning disabled ^^^

// N4659 D.11 [depr.temporary.buffer]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_TEMPORARY_BUFFER                                                                   \
    [[deprecated("warning STL4012: "                                                                        \
                 "std::get_temporary_buffer() and std::return_temporary_buffer() are deprecated in C++17. " \
                 "You can define _SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING "                      \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_TEMPORARY_BUFFER
#endif // ^^^ warning disabled ^^^

// N4659 D.12 [depr.meta.types]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_IS_LITERAL_TYPE                                                     \
    [[deprecated("warning STL4013: "                                                         \
                 "std::is_literal_type and std::is_literal_type_v are deprecated in C++17. " \
                 "You can define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING "        \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_IS_LITERAL_TYPE
#endif // ^^^ warning disabled ^^^

// N4659 D.12 [depr.meta.types]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_RESULT_OF                                                      \
    [[deprecated("warning STL4014: "                                                    \
                 "std::result_of and std::result_of_t are deprecated in C++17. "        \
                 "They are superseded by std::invoke_result and std::invoke_result_t. " \
                 "You can define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING "         \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_RESULT_OF
#endif // ^^^ warning disabled ^^^

// N4659 D.13 [depr.iterator.primitives]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_ITERATOR_BASE_CLASS                                                                          \
    [[deprecated(                                                                                                     \
        "warning STL4015: "                                                                                           \
        "The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17. "        \
        "(The <iterator> header is NOT deprecated.) The C++ Standard has never required user-defined iterators to "   \
        "derive from std::iterator. To fix this warning, stop deriving from std::iterator and start providing "       \
        "publicly accessible typedefs named iterator_category, value_type, difference_type, pointer, and reference. " \
        "Note that value_type is required to be non-const, even for constant iterators. "                             \
        "You can define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING "                                      \
        "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_ITERATOR_BASE_CLASS
#endif // ^^^ warning disabled ^^^

// N4659 D.14 [depr.util.smartptr.shared.obs]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_SHARED_PTR_UNIQUE                                              \
    [[deprecated("warning STL4016: "                                                    \
                 "std::shared_ptr::unique() is deprecated in C++17. "                   \
                 "You can define _SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING " \
                 "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_SHARED_PTR_UNIQUE
#endif // ^^^ warning disabled ^^^

// N4659 D.15 [depr.locale.stdcvt]
// N4659 D.16 [depr.conversions]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING) \
    && !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
#define _CXX17_DEPRECATE_CODECVT_HEADER                                                                        \
    [[deprecated(                                                                                              \
        "warning STL4017: "                                                                                    \
        "std::wbuffer_convert, std::wstring_convert, and the <codecvt> header (containing std::codecvt_mode, " \
        "std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17. "        \
        "(The std::codecvt class template is NOT deprecated.) "                                                \
        "The C++ Standard doesn't provide equivalent non-deprecated functionality; "                           \
        "consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead. "            \
        "You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING "                                    \
        "or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else // ^^^ warning enabled ^^^ / vvv warning disabled vvv
#define _CXX17_DEPRECATE_CODECVT_HEADER
#endif // ^^^ warning disabled ^^^

#ifdef _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING
#define _DEPRECATE_FPOS_SEEKPOS
#else // ^^^ warning disabled ^^^ // vvv warning enabled vvv
#define _DEPRECATE_FPOS_SEEKPOS                                                                                        \
    [[deprecated("warning STL4019: "                                                                                   \
                 "The member std::fpos::seekpos() is non-Standard, and is preserved only for compatibility with "      \
                 "workarounds for old versions of Visual C++. It will be removed in a future release, and in this "    \
                 "release always returns 0. Please use standards-conforming mechanisms to manipulate fpos, such as "   \
                 "conversions to and from streamoff, or an integral type, instead. If you are receiving this message " \
                 "while compiling Boost.IOStreams, a fix has been submitted upstream to make Boost use "               \
                 "standards-conforming mechanisms, as it does for other compilers. You can define "                    \
                 "_SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING to acknowledge that you have received this warning, "      \
                 "or define _REMOVE_FPOS_SEEKPOS to remove std::fpos::seekpos entirely.")]]
#endif // _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING

// next warning number: STL4020

#ifndef _HAS_LAUNDER
#ifdef __has_builtin
#if __has_builtin(__builtin_launder)
#define _HAS_LAUNDER _HAS_CXX17
#else // ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv
#define _HAS_LAUNDER 0
#endif // ^^^ compiler support unavailable ^^^
#elif defined(_BUILTIN_LAUNDER_SUPPORTED)
#define _HAS_LAUNDER _HAS_CXX17
#else // ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv
#define _HAS_LAUNDER 0
#endif // ^^^ compiler support unavailable ^^^
#endif // _HAS_LAUNDER


// LIBRARY FEATURE-TEST MACROS

// C++14
#define __cpp_lib_chrono_udls 201304L
#define __cpp_lib_complex_udls 201309L
#define __cpp_lib_exchange_function 201304L
#define __cpp_lib_generic_associative_lookup 201304L
#define __cpp_lib_integer_sequence 201304L
#define __cpp_lib_integral_constant_callable 201304L
#define __cpp_lib_is_final 201402L
#define __cpp_lib_is_null_pointer 201309L
#define __cpp_lib_make_reverse_iterator 201402L
#define __cpp_lib_make_unique 201304L
#define __cpp_lib_null_iterators 201304L
#define __cpp_lib_quoted_string_io 201304L
#define __cpp_lib_result_of_sfinae 201210L
#define __cpp_lib_robust_nonmodifying_seq_ops 201304L
#ifndef _M_CEE
#define __cpp_lib_shared_timed_mutex 201402L
#endif // _M_CEE
#define __cpp_lib_string_udls 201304L
#define __cpp_lib_transformation_trait_aliases 201304L
#define __cpp_lib_tuple_element_t 201402L
#define __cpp_lib_tuples_by_type 201304L

// C++17
#define __cpp_lib_addressof_constexpr 201603L
#define __cpp_lib_allocator_traits_is_always_equal 201411L
#define __cpp_lib_as_const 201510L
#define __cpp_lib_bool_constant 201505L
#define __cpp_lib_enable_shared_from_this 201603L
#define __cpp_lib_incomplete_container_elements 201505L
#define __cpp_lib_invoke 201411L
#define __cpp_lib_logical_traits 201510L
#define __cpp_lib_map_try_emplace 201411L
#define __cpp_lib_nonmember_container_access 201411L
#ifndef _USING_V110_SDK71_
#define __cpp_lib_shared_mutex 201505L
#endif // _USING_V110_SDK71_
#define __cpp_lib_shared_ptr_arrays 201611L
#define __cpp_lib_transparent_operators 201510L
#define __cpp_lib_type_trait_variable_templates 201510L
#define __cpp_lib_uncaught_exceptions 201411L
#define __cpp_lib_unordered_map_try_emplace 201411L
#define __cpp_lib_void_t 201411L

#if _HAS_CXX17
#define __cpp_lib_any 201606L
#define __cpp_lib_apply 201603L
#define __cpp_lib_array_constexpr 201603L
#define __cpp_lib_atomic_is_always_lock_free 201603L
#define __cpp_lib_boyer_moore_searcher 201603L
#define __cpp_lib_chrono 201611L
#define __cpp_lib_clamp 201603L
#ifndef _M_CEE
#define __cpp_lib_execution 201603L
#endif // _M_CEE
#define __cpp_lib_filesystem 201703L
#define __cpp_lib_gcd_lcm 201606L
#define __cpp_lib_hardware_interference_size 201703L
#define __cpp_lib_has_unique_object_representations 201606L
#define __cpp_lib_hypot 201603L
#define __cpp_lib_is_aggregate 201703L
#define __cpp_lib_is_invocable 201703L
#define __cpp_lib_is_swappable 201603L
#define __cpp_lib_make_from_tuple 201606L
#define __cpp_lib_memory_resource 201603L
#define __cpp_lib_node_extract 201606L
#define __cpp_lib_not_fn 201603L
#define __cpp_lib_optional 201606L
#ifndef _M_CEE
#define __cpp_lib_parallel_algorithm 201603L
#endif // _M_CEE
#define __cpp_lib_raw_memory_algorithms 201606L
#define __cpp_lib_sample 201603L
#define __cpp_lib_scoped_lock 201703L
#define __cpp_lib_shared_ptr_weak_type 201606L
#define __cpp_lib_string_view 201606L
#if _HAS_COMPLETE_CHARCONV
#define __cpp_lib_to_chars 201611L
#endif // _HAS_COMPLETE_CHARCONV
#define __cpp_lib_variant 201606L
#else // _HAS_CXX17
#define __cpp_lib_chrono 201510L
#endif // _HAS_CXX17

// C++20
#if _HAS_CXX20
#define __cpp_lib_list_remove_return_type 201806L
#endif // _HAS_CXX20

#if _HAS_STD_BYTE
#define __cpp_lib_byte 201603L
#endif // _HAS_STD_BYTE

#if _HAS_LAUNDER
#define __cpp_lib_launder 201606L
#endif // _HAS_LAUNDER

#if _HAS_SPECIAL_MATH
#define __cpp_lib_math_special_functions 201603L
#endif // _HAS_SPECIAL_MATH

// EXPERIMENTAL
#define __cpp_lib_experimental_erase_if 201411L
#define __cpp_lib_experimental_filesystem 201406L


#ifdef _RTC_CONVERSION_CHECKS_ENABLED
#ifndef _ALLOW_RTCc_IN_STL
#error /RTCc rejects conformant code, so it is not supported by the C++ Standard Library. Either remove this \
compiler option, or define _ALLOW_RTCc_IN_STL to acknowledge that you have received this warning.
#endif // _ALLOW_RTCc_IN_STL
#endif // _RTC_CONVERSION_CHECKS_ENABLED

#ifndef _DECLSPEC_ALLOCATOR
#ifdef __clang__
#define _DECLSPEC_ALLOCATOR
#else // ^^^ Clang ^^^ // vvv non-Clang vvv
#define _DECLSPEC_ALLOCATOR __declspec(allocator)
#endif // ^^^ non-Clang ^^^
#endif // _DECLSPEC_ALLOCATOR

#define _STRINGIZEX(x) #x
#define _STRINGIZE(x) _STRINGIZEX(x)
#define _EMPTY_ARGUMENT // for empty macro argument

// NAMESPACE
#define _STD_BEGIN namespace std {
#define _STD_END }
#define _STD ::std::

// We use the stdext (standard extension) namespace to contain extensions that are not part of the current standard
#define _STDEXT_BEGIN namespace stdext {
#define _STDEXT_END }
#define _STDEXT ::stdext::

#ifdef __cplusplus
#define _CSTD ::

#define _EXTERN_C extern "C" {
#define _END_EXTERN_C }
#else // ^^^ __cplusplus ^^^ // vvv !__cplusplus vvv
#define _CSTD

#define _EXTERN_C
#define _END_EXTERN_C
#endif // __cplusplus

#endif // RC_INVOKED
#endif // _YVALS_CORE_H

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
