// yvals.h internal header
#pragma once
#ifndef _YVALS
#define _YVALS
#ifndef RC_INVOKED

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
// P0604R0 invoke_result, is_invocable, is_nothrow_invocable
// P0607R0 Inline Variables For The STL
// P0682R1 Repairing Elementary String Conversions
// P0739R0 Improving Class Template Argument Deduction For The STL

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
// The non-Standard std::identity struct
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
// * for_each
// * for_each_n
// * inclusive_scan
// * mismatch
// * none_of
// * reduce
// * remove
// * remove_if
// * search
// * search_n
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
// * copy_backward
// * copy_n
// * fill
// * fill_n
// * move
// * move_backward
// * remove
// * remove_if
// * replace
// * replace_if
// * reverse
// * reverse_copy
// * rotate
// * rotate_copy
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
// * is_heap
// * is_heap_until
// * is_partitioned
// * is_sorted
// * is_sorted_until
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
// * set_difference
// * set_intersection
// * set_symmetric_difference
// * set_union
// * stable_partition
// * unique
// * unique_copy

#ifndef _HAS_CXX17
 #ifdef _MSVC_LANG
  #if _MSVC_LANG > 201402
   #define _HAS_CXX17	1
  #else /* _MSVC_LANG > 201402 */
   #define _HAS_CXX17	0
  #endif /* _MSVC_LANG > 201402 */
 #else /* _MSVC_LANG */
  #if __cplusplus > 201402
   #define _HAS_CXX17	1
  #else /* __cplusplus > 201402 */
   #define _HAS_CXX17	0
  #endif /* __cplusplus > 201402 */
 #endif /* _MSVC_LANG */
#endif /* _HAS_CXX17 */

#include <xkeycheck.h>	// _HAS_CXX17 must be defined before including this
#include <crtdefs.h>

#ifndef _STL_WARNING_LEVEL
 #if defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4
  #define _STL_WARNING_LEVEL	4
 #else /* defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4 */
  #define _STL_WARNING_LEVEL	3
 #endif /* defined(_MSVC_WARNING_LEVEL) && _MSVC_WARNING_LEVEL >= 4 */
#endif /* _STL_WARNING_LEVEL */

#if _STL_WARNING_LEVEL < 3
 #error _STL_WARNING_LEVEL cannot be less than 3.
#endif /* _STL_WARNING_LEVEL < 3 */

#if _STL_WARNING_LEVEL > 4
 #error _STL_WARNING_LEVEL cannot be greater than 4.
#endif /* _STL_WARNING_LEVEL > 4 */

// warning C4577: 'noexcept' used with no exception handling mode specified;
// termination on exception is not guaranteed. Specify /EHsc
#if _HAS_EXCEPTIONS
 #define _STL_DISABLED_WARNING_C4577
#else /* _HAS_EXCEPTIONS */
 #define _STL_DISABLED_WARNING_C4577	4577
#endif /* _HAS_EXCEPTIONS */

#ifndef _STL_EXTRA_DISABLED_WARNINGS
 #define _STL_EXTRA_DISABLED_WARNINGS
#endif /* _STL_EXTRA_DISABLED_WARNINGS */

// warning C4455: literal suffix identifiers that do not start with an underscore are reserved
// warning C4494: Ignoring __declspec(allocator) because the function return
// type is not a pointer or reference
// warning C4702: unreachable code
// error C4984: 'if constexpr' is a C++17 language extension
// warning C4988: variable declared outside class/function scope (/Wall /d1WarnOnGlobals)
#ifndef _STL_DISABLED_WARNINGS
 #define _STL_DISABLED_WARNINGS 4455 4494 _STL_DISABLED_WARNING_C4577 4702 4984 4988 _STL_EXTRA_DISABLED_WARNINGS
#endif /* _STL_DISABLED_WARNINGS */

// warning: constexpr if is a C++17 extension [-Wc++17-extensions]
// warning: user-defined literal suffixes not starting with '_' are reserved [-Wuser-defined-literals]
#ifndef _STL_DISABLE_CLANG_WARNINGS
 #ifdef __clang__
  #define _STL_DISABLE_CLANG_WARNINGS	_Pragma("clang diagnostic push")	\
	_Pragma("clang diagnostic ignored \"-Wc++17-extensions\"")				\
	_Pragma("clang diagnostic ignored \"-Wuser-defined-literals\"")			\
	_Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"")
 #else /* __clang__ */
  #define _STL_DISABLE_CLANG_WARNINGS
 #endif /* __clang__ */
#endif /* _STL_DISABLE_CLANG_WARNINGS */

#ifndef _STL_RESTORE_CLANG_WARNINGS
 #ifdef __clang__
  #define _STL_RESTORE_CLANG_WARNINGS	_Pragma("clang diagnostic pop")
 #else /* __clang__ */
  #define _STL_RESTORE_CLANG_WARNINGS
 #endif /* __clang__ */
#endif /* _STL_RESTORE_CLANG_WARNINGS */

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,_STL_WARNING_LEVEL)
#pragma warning(disable: _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#define _CPPLIB_VER	650
#define _MSVC_STL_VERSION 141
#define _MSVC_STL_UPDATE 201803

#ifndef _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
 #ifdef __EDG__
  // not attempting to detect __EDG_VERSION__ being less than expected
 #elif defined(__clang__)
  #if __clang_major__ < 6	// Coarse-grained, not inspecting __clang_minor__ and __clang_patchlevel__
   #error STL1000: Unexpected compiler version, expected Clang 6 or newer.
  #endif /* ^^^ old Clang ^^^ */
 #elif defined(_MSC_VER)
  #if _MSC_VER < 1914	// Coarse-grained, not inspecting _MSC_FULL_VER
   #error STL1001: Unexpected compiler version, expected MSVC 19.14 or newer.
  #endif /* ^^^ old MSVC ^^^ */
 #else /* vvv other compilers vvv */
  // not attempting to detect other compilers
 #endif /* ^^^ other compilers ^^^ */
#endif /* _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH */

#define _NOEXCEPT				noexcept

#if _HAS_EXCEPTIONS
 #define _NOEXCEPT_COND(...)	noexcept(__VA_ARGS__)
 #define _NOEXCEPT_OPER(...)	noexcept(__VA_ARGS__)
#else /* _HAS_EXCEPTIONS */
 #define _NOEXCEPT_COND(...)	noexcept
 #define _NOEXCEPT_OPER(...)	true
#endif /* _HAS_EXCEPTIONS */

#ifndef _HAS_STATIC_RTTI
 #define _HAS_STATIC_RTTI	1
#endif /* _HAS_STATIC_RTTI */

#if defined(_CPPRTTI) && !_HAS_STATIC_RTTI
 #error /GR implies _HAS_STATIC_RTTI.
#endif /* defined(_CPPRTTI) && !_HAS_STATIC_RTTI */

// C++17 constexpr additions
#if _HAS_CXX17
 #define _CONSTEXPR17 constexpr
#else /* ^^^ has C++17 constexpr additions ^^^ / vvv no C++17 constexpr additions vvv */
 #define _CONSTEXPR17 inline
#endif /* _HAS_CXX17 */

// N4190 Removing auto_ptr, random_shuffle(), And Old <functional> Stuff
#ifndef _HAS_AUTO_PTR_ETC
 #define _HAS_AUTO_PTR_ETC	(!_HAS_CXX17)
#endif /* _HAS_AUTO_PTR_ETC */

// P0003R5 Removing Dynamic Exception Specifications
#ifndef _HAS_UNEXPECTED
 #define _HAS_UNEXPECTED	(!_HAS_CXX17)
#endif /* _HAS_UNEXPECTED */

// P0004R1 Removing Deprecated Iostreams Aliases
#ifndef _HAS_OLD_IOSTREAMS_MEMBERS
 #define _HAS_OLD_IOSTREAMS_MEMBERS	(!_HAS_CXX17)
#endif /* _HAS_OLD_IOSTREAMS_MEMBERS */

// P0226R1 Mathematical Special Functions
#ifndef _HAS_SPECIAL_MATH
 #define _HAS_SPECIAL_MATH	_HAS_CXX17
#endif /* _HAS_SPECIAL_MATH */

// P0298R3 std::byte
#ifndef _HAS_STD_BYTE
 #define _HAS_STD_BYTE	_HAS_CXX17	// inspected by GSL, do not remove
#endif /* _HAS_STD_BYTE */

// P0302R1 Removing Allocator Support In std::function
// LWG 2385 function::assign allocator argument doesn't make sense
// LWG 2921 packaged_task and type-erased allocators
// LWG 2976 Dangling uses_allocator specialization for packaged_task
#ifndef _HAS_FUNCTION_ALLOCATOR_SUPPORT
 #define _HAS_FUNCTION_ALLOCATOR_SUPPORT	(!_HAS_CXX17)
#endif /* _HAS_FUNCTION_ALLOCATOR_SUPPORT */

// The non-Standard std::tr1 namespace and TR1-only machinery
#ifndef _HAS_TR1_NAMESPACE
 #define _HAS_TR1_NAMESPACE	(!_HAS_CXX17)
#endif /* _HAS_TR1_NAMESPACE */

#if _HAS_TR1_NAMESPACE
 #ifdef _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
  #define _DEPRECATE_TR1_NAMESPACE
 #else /* _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING */
  #define _DEPRECATE_TR1_NAMESPACE	[[deprecated("warning STL4002: " \
	"The non-Standard std::tr1 namespace and TR1-only machinery are deprecated and will be REMOVED. You can " \
	"define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING to acknowledge that you have received this warning.")]]
 #endif /* _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING */
#endif /* _HAS_TR1_NAMESPACE */

// The non-Standard std::identity struct
#ifndef _HAS_IDENTITY_STRUCT
 #define _HAS_IDENTITY_STRUCT	(!_HAS_CXX17)
#endif /* _HAS_IDENTITY_STRUCT */

#if _HAS_IDENTITY_STRUCT
 #ifdef _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING
  #define _DEPRECATE_IDENTITY_STRUCT
 #else /* _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING */
  #define _DEPRECATE_IDENTITY_STRUCT	[[deprecated("warning STL4003: " \
	"The non-Standard std::identity struct is deprecated and will be REMOVED. You can " \
	"define _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING to acknowledge that you have received this warning.")]]
 #endif /* _SILENCE_IDENTITY_STRUCT_DEPRECATION_WARNING */
#endif /* _HAS_IDENTITY_STRUCT */

// The non-Standard std::tr2::sys namespace
#ifndef _HAS_TR2_SYS_NAMESPACE
 #define _HAS_TR2_SYS_NAMESPACE	(!_HAS_CXX17)
#endif /* _HAS_TR2_SYS_NAMESPACE */

#if _HAS_TR2_SYS_NAMESPACE
 #ifdef _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING
  #define _DEPRECATE_TR2_SYS_NAMESPACE
 #else /* _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING */
  #define _DEPRECATE_TR2_SYS_NAMESPACE	[[deprecated("warning STL4018: " \
	"The non-Standard std::tr2::sys namespace is deprecated and will be REMOVED. " \
	"It is superseded by std::experimental::filesystem. You can " \
	"define _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING to acknowledge that you have received this warning.")]]
 #endif /* _SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING */
#endif /* _HAS_TR2_SYS_NAMESPACE */

// Enforcement of matching allocator value_types
#ifndef _ENFORCE_MATCHING_ALLOCATORS
 #define _ENFORCE_MATCHING_ALLOCATORS	_HAS_CXX17
#endif /* _ENFORCE_MATCHING_ALLOCATORS */

#define _MISMATCHED_ALLOCATOR_MESSAGE(_CONTAINER, _VALUE_TYPE) \
	_CONTAINER " requires that Allocator's value_type match " _VALUE_TYPE \
	" (See N4659 26.2.1 [container.requirements.general]/16 allocator_type)" \
	" Either fix the allocator value_type or define _ENFORCE_MATCHING_ALLOCATORS=0" \
	" to suppress this diagnostic."

// [[nodiscard]] attributes on STL functions
#ifndef _HAS_NODISCARD
 #define _HAS_NODISCARD	_HAS_CXX17
#endif /* _HAS_NODISCARD */

#if _HAS_NODISCARD
 #define _NODISCARD [[nodiscard]]
 #define _NODISCARD_PERF
#else /* ^^^ CAN HAZ [[nodiscard]] ^^^ // vvv NO CAN HAZ [[nodiscard]] vvv */
 #define _NODISCARD
 #define _NODISCARD_PERF
#endif /* _HAS_NODISCARD */

// Controls whether the STL uses "if constexpr" internally
#ifndef _HAS_IF_CONSTEXPR
 #define _HAS_IF_CONSTEXPR	_HAS_CXX17
#endif /* _HAS_IF_CONSTEXPR */

#if _HAS_IF_CONSTEXPR
 #define _CONSTEXPR_IF	constexpr
#else /* _HAS_IF_CONSTEXPR */
 #define _CONSTEXPR_IF
#endif /* _HAS_IF_CONSTEXPR */

// Controls whether the STL will force /fp:fast to enable vectorization of algorithms defined
// in the standard as special cases; such as reduce, transform_reduce, inclusive_scan, exclusive_scan
#ifndef _STD_VECTORIZE_WITH_FLOAT_CONTROL
 #ifdef _M_FP_EXCEPT
  #define _STD_VECTORIZE_WITH_FLOAT_CONTROL 0
 #else /* ^^^ floating point exceptions enabled ^^^ // vvv floating point exceptions disabled (default) vvv */
  #define _STD_VECTORIZE_WITH_FLOAT_CONTROL 1
 #endif /* _M_FP_EXCEPT */
#endif /* _STD_VECTORIZE_WITH_FLOAT_CONTROL */

// P0174R2 Deprecating Vestigial Library Parts
// P0521R0 Deprecating shared_ptr::unique()
// Other C++17 deprecation warnings

// N4659 D.4 [depr.cpp.headers]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_C_HEADER	[[deprecated("warning STL4004: " \
	"<ccomplex>, <cstdalign>, <cstdbool>, and <ctgmath> are deprecated in C++17. " \
	"You can define _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_C_HEADER
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.6 [depr.str.strstreams]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_STRSTREAM	[[deprecated("warning STL4005: <strstream> is deprecated in C++17. " \
	"You can define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_STRSTREAM
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.7 [depr.uncaught]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_UNCAUGHT_EXCEPTION	[[deprecated("warning STL4006: " \
	"std::uncaught_exception() is deprecated in C++17. " \
	"It is superseded by std::uncaught_exceptions(), plural. " \
	"You can define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_UNCAUGHT_EXCEPTION
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.8.1 [depr.weak.result_type]
// N4659 D.8.2 [depr.func.adaptor.typedefs]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS	[[deprecated("warning STL4007: Many result_type typedefs " \
	"and all argument_type, first_argument_type, and second_argument_type typedefs are deprecated in C++17. " \
	"You can define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_ADAPTOR_TYPEDEFS
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.8.3 [depr.negators]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_NEGATORS	[[deprecated("warning STL4008: " \
	"std::not1(), std::not2(), std::unary_negate, and std::binary_negate are deprecated in C++17. " \
	"They are superseded by std::not_fn(). " \
	"You can define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_NEGATORS
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.9 [depr.default.allocator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_ALLOCATOR_VOID	[[deprecated("warning STL4009: " \
	"std::allocator<void> is deprecated in C++17. " \
	"You can define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_ALLOCATOR_VOID
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.9 [depr.default.allocator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS	[[deprecated("warning STL4010: " \
	"Various members of std::allocator are deprecated in C++17. " \
	"Use std::allocator_traits instead of accessing these members directly. " \
	"You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.10 [depr.storage.iterator]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_RAW_STORAGE_ITERATOR	[[deprecated("warning STL4011: " \
	"std::raw_storage_iterator is deprecated in C++17. " \
	"Consider using the std::uninitialized_copy() family of algorithms instead. " \
	"You can define _SILENCE_CXX17_RAW_STORAGE_ITERATOR_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_RAW_STORAGE_ITERATOR
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.11 [depr.temporary.buffer]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_TEMPORARY_BUFFER	[[deprecated("warning STL4012: " \
	"std::get_temporary_buffer() and std::return_temporary_buffer() are deprecated in C++17. " \
	"You can define _SILENCE_CXX17_TEMPORARY_BUFFER_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_TEMPORARY_BUFFER
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.12 [depr.meta.types]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_IS_LITERAL_TYPE	[[deprecated("warning STL4013: " \
	"std::is_literal_type and std::is_literal_type_v are deprecated in C++17. " \
	"You can define _SILENCE_CXX17_IS_LITERAL_TYPE_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_IS_LITERAL_TYPE
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.12 [depr.meta.types]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_RESULT_OF	[[deprecated("warning STL4014: " \
	"std::result_of and std::result_of_t are deprecated in C++17. " \
	"They are superseded by std::invoke_result and std::invoke_result_t. " \
	"You can define _SILENCE_CXX17_RESULT_OF_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_RESULT_OF
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.13 [depr.iterator.primitives]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_ITERATOR_BASE_CLASS	[[deprecated("warning STL4015: " \
	"The std::iterator class template (used as a base class to provide typedefs) is deprecated in C++17. " \
	"(The <iterator> header is NOT deprecated.) The C++ Standard has never required user-defined iterators to " \
	"derive from std::iterator. To fix this warning, stop deriving from std::iterator and start providing " \
	"publicly accessible typedefs named iterator_category, value_type, difference_type, pointer, and reference. " \
	"Note that value_type is required to be non-const, even for constant iterators. " \
	"You can define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_ITERATOR_BASE_CLASS
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.14 [depr.util.smartptr.shared.obs]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_SHARED_PTR_UNIQUE	[[deprecated("warning STL4016: " \
	"std::shared_ptr::unique() is deprecated in C++17. " \
	"You can define _SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_SHARED_PTR_UNIQUE
#endif /* ^^^ warning disabled ^^^ */

// N4659 D.15 [depr.locale.stdcvt]
// N4659 D.16 [depr.conversions]
#if _HAS_CXX17 && !defined(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING) \
	&& !defined(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
 #define _CXX17_DEPRECATE_CODECVT_HEADER	[[deprecated("warning STL4017: " \
	"std::wbuffer_convert, std::wstring_convert, and the <codecvt> header (containing std::codecvt_mode, " \
	"std::codecvt_utf8, std::codecvt_utf16, and std::codecvt_utf8_utf16) are deprecated in C++17. " \
	"(The std::codecvt class template is NOT deprecated.) " \
	"The C++ Standard doesn't provide equivalent non-deprecated functionality; " \
	"consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead. " \
	"You can define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING " \
	"or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.")]]
#else /* ^^^ warning enabled ^^^ / vvv warning disabled vvv */
 #define _CXX17_DEPRECATE_CODECVT_HEADER
#endif /* ^^^ warning disabled ^^^ */

#ifndef _HAS_LAUNDER
 #ifdef __has_builtin
  #if __has_builtin(__builtin_launder)
   #define _HAS_LAUNDER	_HAS_CXX17
  #else /* ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv */
   #define _HAS_LAUNDER	0
  #endif /* ^^^ compiler support unavailable ^^^ */
 #elif defined(_BUILTIN_LAUNDER_SUPPORTED)
  #define _HAS_LAUNDER	_HAS_CXX17
 #else /* ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv */
  #define _HAS_LAUNDER	0
 #endif /* ^^^ compiler support unavailable ^^^ */
#endif /* _HAS_LAUNDER */

#ifndef _HAS_HAS_UNIQUE_OBJECT_REPRESENTATIONS
 #define _HAS_HAS_UNIQUE_OBJECT_REPRESENTATIONS _HAS_CXX17
#endif /* _HAS_HAS_UNIQUE_OBJECT_REPRESENTATIONS */

#ifndef _HAS_CONSTEXPR_CHAR_TRAITS
 #if defined(__clang__) || defined(__EDG__) || defined(_CONSTEXPR_CHAR_TRAITS_SUPPORTED)
  #define _HAS_CONSTEXPR_CHAR_TRAITS	_HAS_CXX17
 #else /* ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv */
  #define _HAS_CONSTEXPR_CHAR_TRAITS	0	// TRANSITION, VSO#364512
 #endif /* ^^^ compiler support unavailable ^^^ */
#endif /* _HAS_CONSTEXPR_CHAR_TRAITS */

#if _HAS_CONSTEXPR_CHAR_TRAITS
 #define _CHAR_TRAITS_CONSTEXPR	_CONSTEXPR17
#else /* _HAS_CONSTEXPR_CHAR_TRAITS */
 #define _CHAR_TRAITS_CONSTEXPR	inline
#endif /* _HAS_CONSTEXPR_CHAR_TRAITS */

#ifndef _HAS_DEDUCTION_GUIDES
 #if defined(_DEDUCTION_GUIDES_SUPPORTED) || defined(__cpp_deduction_guides)
  #define _HAS_DEDUCTION_GUIDES	_HAS_CXX17
 #else /* ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv */
  #define _HAS_DEDUCTION_GUIDES	0
 #endif /* ^^^ compiler support unavailable ^^^ */
#endif /* _HAS_DEDUCTION_GUIDES */

#ifndef _HAS_INLINE_VARIABLES
 #if defined(_INLINE_VARIABLES_SUPPORTED) || defined(__cpp_inline_variables)
  #define _HAS_INLINE_VARIABLES	_HAS_CXX17
 #else /* ^^^ compiler support (always) available ^^^ // vvv compiler support unavailable vvv */
  #define _HAS_INLINE_VARIABLES	0
 #endif /* ^^^ compiler support unavailable ^^^ */
#endif /* _HAS_INLINE_VARIABLES */

#if _HAS_INLINE_VARIABLES
 #define _INLINE_VAR	inline
#else /* _HAS_INLINE_VARIABLES */
 #define _INLINE_VAR
#endif /* _HAS_INLINE_VARIABLES */

#ifndef _HAS_ALIGNED_NEW
 #if defined(_ALIGNED_NEW_SUPPORTED) || defined(__cpp_aligned_new)
  #define _HAS_ALIGNED_NEW 1
 #else /* ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv */
  #define _HAS_ALIGNED_NEW 0
 #endif /* ^^^ compiler support unavailable ^^^ */
#endif /* _HAS_ALIGNED_NEW */

#ifndef _HAS_NOEXCEPT_FUNCTION_TYPES
 #if defined(_NOEXCEPT_TYPES_SUPPORTED) || defined(__cpp_noexcept_function_type)
  #define _HAS_NOEXCEPT_FUNCTION_TYPES	1
 #else /* ^^^ compiler support available ^^^ // vvv compiler support unavailable vvv */
  #define _HAS_NOEXCEPT_FUNCTION_TYPES	0
 #endif /* ^^^ compiler support unavailable ^^^ */
#endif /* _HAS_NOEXCEPT_FUNCTION_TYPES */

#ifdef _RTC_CONVERSION_CHECKS_ENABLED
 #ifndef _ALLOW_RTCc_IN_STL
  #error /RTCc rejects conformant code, so it is not supported by the C++ Standard Library. Either remove this \
compiler option, or define _ALLOW_RTCc_IN_STL to acknowledge that you have received this warning.
 #endif /* _ALLOW_RTCc_IN_STL */
#endif /* _RTC_CONVERSION_CHECKS_ENABLED */

#ifndef _DECLSPEC_ALLOCATOR
 #ifdef __clang__
  #define _DECLSPEC_ALLOCATOR
 #else /* ^^^ Clang ^^^ // vvv non-Clang vvv */
  #define _DECLSPEC_ALLOCATOR	__declspec(allocator)
 #endif /* ^^^ non-Clang ^^^ */
#endif /* _DECLSPEC_ALLOCATOR */

/* Note on use of "deprecate":
 * Various places in this header and other headers use __declspec(deprecate) or macros that
 * have the term DEPRECATE in them. We use deprecate here ONLY to signal the compiler to
 * emit a warning about these items. The use of deprecate should NOT be taken to imply that
 * any standard committee has deprecated these functions from the relevant standards.
 * In fact, these functions are NOT deprecated from the standard.
 *
 * Full details can be found in our documentation by searching for "Checked Iterators".
*/

#if defined(MRTDLL) && defined(_CRTBLD)
/*
process-global is the default for code built with /clr or /clr:oldSyntax.
appdomain-global is the default for code built with /clr:pure.
Code in MSVCM is built with /clr, but is used by user code built with /clr:pure
so it must conform to the expectations of /clr:pure clients.
Use __PURE_APPDOMAIN_GLOBAL when a global needs to be appdomain-global for pure
clients and process-global for mixed clients.
*/
#define __PURE_APPDOMAIN_GLOBAL   __declspec(appdomain)
#else
#define __PURE_APPDOMAIN_GLOBAL
#endif

		/* CURRENT DLL NAMES */
#ifndef _CRT_MSVCP_CURRENT
	#ifdef _CRT_WINDOWS
		/* Windows */
		#ifdef _DEBUG
			#define _CRT_MSVCP_CURRENT "msvcpd_win.dll"
		#else
			#define _CRT_MSVCP_CURRENT "msvcp_win.dll"
		#endif
	#else
		/* Visual Studio */
		#ifdef _DEBUG
			#define _CRT_MSVCP_CURRENT "msvcp140d.dll"
		#else
			#define _CRT_MSVCP_CURRENT "msvcp140.dll"
		#endif
	#endif
#endif

#ifdef _ITERATOR_DEBUG_LEVEL /* A. _ITERATOR_DEBUG_LEVEL is already defined. */

	/* A1. Validate _ITERATOR_DEBUG_LEVEL. */
	#if _ITERATOR_DEBUG_LEVEL > 2 && defined(_DEBUG)
		#error _ITERATOR_DEBUG_LEVEL > 2 is not supported in debug mode.
	#elif _ITERATOR_DEBUG_LEVEL > 1 && !defined(_DEBUG)
		#error _ITERATOR_DEBUG_LEVEL > 1 is not supported in release mode.
	#endif

	/* A2. Inspect _HAS_ITERATOR_DEBUGGING. */
	#ifdef _HAS_ITERATOR_DEBUGGING /* A2i. _HAS_ITERATOR_DEBUGGING is already defined, validate it. */
		#if _ITERATOR_DEBUG_LEVEL == 2 && _HAS_ITERATOR_DEBUGGING != 1
			#error _ITERATOR_DEBUG_LEVEL == 2 must imply _HAS_ITERATOR_DEBUGGING == 1 .
		#elif _ITERATOR_DEBUG_LEVEL < 2 && _HAS_ITERATOR_DEBUGGING != 0
			#error _ITERATOR_DEBUG_LEVEL < 2 must imply _HAS_ITERATOR_DEBUGGING == 0 .
		#endif
	#else /* A2ii. _HAS_ITERATOR_DEBUGGING is not yet defined, derive it. */
		#if _ITERATOR_DEBUG_LEVEL == 2
			#define _HAS_ITERATOR_DEBUGGING 1
		#else
			#define _HAS_ITERATOR_DEBUGGING 0
		#endif
	#endif /* _HAS_ITERATOR_DEBUGGING */

	/* A3. Inspect _SECURE_SCL. */
	#ifdef _SECURE_SCL /* A3i. _SECURE_SCL is already defined, validate it. */
		#if _ITERATOR_DEBUG_LEVEL > 0 && _SECURE_SCL != 1
			#error _ITERATOR_DEBUG_LEVEL > 0 must imply _SECURE_SCL == 1 .
		#elif _ITERATOR_DEBUG_LEVEL == 0 && _SECURE_SCL != 0
			#error _ITERATOR_DEBUG_LEVEL == 0 must imply _SECURE_SCL == 0 .
		#endif
	#else /* A3ii. _SECURE_SCL is not yet defined, derive it. */
		#if _ITERATOR_DEBUG_LEVEL > 0
			#define _SECURE_SCL 1
		#else
			#define _SECURE_SCL 0
		#endif
	#endif /* _SECURE_SCL */

#else /* B. _ITERATOR_DEBUG_LEVEL is not yet defined. */

	/* B1. Inspect _HAS_ITERATOR_DEBUGGING. */
	#ifdef _HAS_ITERATOR_DEBUGGING /* B1i. _HAS_ITERATOR_DEBUGGING is already defined, validate it. */
		#if _HAS_ITERATOR_DEBUGGING > 1
			#error _HAS_ITERATOR_DEBUGGING must be either 0 or 1 .
		#elif _HAS_ITERATOR_DEBUGGING == 1 && !defined(_DEBUG)
			#error _HAS_ITERATOR_DEBUGGING == 1 is not supported in release mode.
		#endif
	#else /* B1ii. _HAS_ITERATOR_DEBUGGING is not yet defined, default it. */
		#ifdef _DEBUG
			#define _HAS_ITERATOR_DEBUGGING 1
		#else
			#define _HAS_ITERATOR_DEBUGGING 0
		#endif
	#endif /* _HAS_ITERATOR_DEBUGGING */

	/* B2. Inspect _SECURE_SCL. */
	#ifdef _SECURE_SCL /* B2i. _SECURE_SCL is already defined, validate it. */
		#if _SECURE_SCL > 1
			#error _SECURE_SCL must be either 0 or 1 .
		#endif
	#else /* B2ii. _SECURE_SCL is not yet defined, default it. */
		#if _HAS_ITERATOR_DEBUGGING == 1
			#define _SECURE_SCL 1
		#else
			#define _SECURE_SCL 0
		#endif
	#endif /* _SECURE_SCL */

	/* B3. Derive _ITERATOR_DEBUG_LEVEL. */
	#if _HAS_ITERATOR_DEBUGGING
		#define _ITERATOR_DEBUG_LEVEL 2
	#elif _SECURE_SCL
		#define _ITERATOR_DEBUG_LEVEL 1
	#else
		#define _ITERATOR_DEBUG_LEVEL 0
	#endif

#endif /* _ITERATOR_DEBUG_LEVEL */

#define _STRINGIZEX(x) #x
#define _STRINGIZE(x) _STRINGIZEX(x)

#ifdef __cplusplus
	#ifndef _ALLOW_MSC_VER_MISMATCH
		#pragma detect_mismatch("_MSC_VER", "1900")
	#endif /* _ALLOW_MSC_VER_MISMATCH */

	#ifndef _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH
		#pragma detect_mismatch("_ITERATOR_DEBUG_LEVEL", _STRINGIZE(_ITERATOR_DEBUG_LEVEL))
	#endif /* _ALLOW_ITERATOR_DEBUG_LEVEL_MISMATCH */

	#ifndef _ALLOW_RUNTIME_LIBRARY_MISMATCH
		#if !defined(_DLL) && !defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MT_StaticRelease")
		#elif !defined(_DLL) && defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MTd_StaticDebug")
		#elif defined(_DLL) && !defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MD_DynamicRelease")
		#elif defined(_DLL) && defined(_DEBUG)
			#pragma detect_mismatch("RuntimeLibrary", "MDd_DynamicDebug")
		#endif /* defined(_DLL) etc. */
	#endif /* _ALLOW_RUNTIME_LIBRARY_MISMATCH */
#endif /* __cplusplus */

#ifdef _ITERATOR_DEBUG_ARRAY_OVERLOADS
	#if _ITERATOR_DEBUG_ARRAY_OVERLOADS != 0 && _ITERATOR_DEBUG_ARRAY_OVERLOADS != 1
		#error _ITERATOR_DEBUG_ARRAY_OVERLOADS must be either 0 or 1 .
	#elif _ITERATOR_DEBUG_LEVEL == 0 && _ITERATOR_DEBUG_ARRAY_OVERLOADS == 1
		#error _ITERATOR_DEBUG_LEVEL == 0 must imply _ITERATOR_DEBUG_ARRAY_OVERLOADS == 0 .
	#endif
#else /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */
	#if _ITERATOR_DEBUG_LEVEL == 0
		#define _ITERATOR_DEBUG_ARRAY_OVERLOADS 0
	#else
		#define _ITERATOR_DEBUG_ARRAY_OVERLOADS 1
	#endif
#endif /* _ITERATOR_DEBUG_ARRAY_OVERLOADS */

/* See note on use of deprecate at the top of this file */
#if !defined(_SCL_SECURE_NO_WARNINGS) && defined(_SCL_SECURE_NO_DEPRECATE)
#define _SCL_SECURE_NO_WARNINGS
#endif

#if !defined (_SECURE_SCL_DEPRECATE)
#if defined(_SCL_SECURE_NO_WARNINGS)
#define _SECURE_SCL_DEPRECATE 0
#else
#define _SECURE_SCL_DEPRECATE 1
#endif
#endif

/* _SECURE_SCL switches: helper macros */
/* See note on use of deprecate at the top of this file */

#if _ITERATOR_DEBUG_LEVEL > 0 && _SECURE_SCL_DEPRECATE
#define _SCL_INSECURE_DEPRECATE_FN(_Func) \
	_CRT_DEPRECATE_TEXT( \
		"Call to 'std::" #_Func "' with parameters that may be unsafe - " \
		"this call relies on the caller to check that the passed values are correct. " \
		"To disable this warning, use -D_SCL_SECURE_NO_WARNINGS. " \
		"See documentation on how to use Visual C++ 'Checked Iterators'")
#else
#define _SCL_INSECURE_DEPRECATE_FN(_Func)
#endif

#ifndef _SCL_SECURE_INVALID_PARAMETER
 #define _SCL_SECURE_INVALID_PARAMETER(expr) _CRT_SECURE_INVALID_PARAMETER(expr)
#endif

 #define _SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT		_SCL_SECURE_INVALID_PARAMETER("invalid argument")
 #define _SCL_SECURE_OUT_OF_RANGE_NO_ASSERT			_SCL_SECURE_INVALID_PARAMETER("out of range")

 #define _SCL_SECURE_ALWAYS_VALIDATE(cond)				\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;		\
		}												\
		_Analysis_assume_(cond);						\
	}

 #define _SCL_SECURE_CRT_VALIDATE(cond, retvalue)		\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_INVALID_PARAMETER(cond);		\
			return (retvalue);							\
		}												\
	}

 #if _ITERATOR_DEBUG_LEVEL > 0

 #define _SCL_SECURE_VALIDATE(cond)						\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_INVALID_ARGUMENT_NO_ASSERT;		\
		}												\
		_Analysis_assume_(cond);						\
	}

 #define _SCL_SECURE_VALIDATE_RANGE(cond)				\
	{													\
		if (!(cond))									\
		{												\
			_ASSERTE(#cond && 0);						\
			_SCL_SECURE_OUT_OF_RANGE_NO_ASSERT;			\
		}												\
		_Analysis_assume_(cond);						\
	}

 #else /* _ITERATOR_DEBUG_LEVEL > 0 */

 #define _SCL_SECURE_VALIDATE(cond)
 #define _SCL_SECURE_VALIDATE_RANGE(cond)

 #endif /* _ITERATOR_DEBUG_LEVEL > 0 */

#if __STDC_WANT_SECURE_LIB__
#define _CRT_SECURE_MEMCPY(dest, destsize, source, count) ::memcpy_s((dest), (destsize), (source), (count))
#define _CRT_SECURE_WMEMCPY(dest, destsize, source, count) ::wmemcpy_s((dest), (destsize), (source), (count))
#else
#define _CRT_SECURE_MEMCPY(dest, destsize, source, count) ::memcpy((dest), (source), (count))
#define _CRT_SECURE_WMEMCPY(dest, destsize, source, count) ::wmemcpy((dest), (source), (count))
#endif

#include <use_ansi.h>

#define _WARNING_MESSAGE(NUMBER, MESSAGE) \
	__FILE__ "(" _CRT_STRINGIZE(__LINE__) "): warning " NUMBER ": " MESSAGE

#ifdef _STATIC_CPPLIB
    #ifndef _DISABLE_DEPRECATE_STATIC_CPPLIB
        #ifdef _DLL
            #pragma message(_WARNING_MESSAGE("STL4000", "_STATIC_CPPLIB is deprecated and will be REMOVED."))
        #endif
        #ifdef _M_CEE_MIXED
            #error _STATIC_CPPLIB is not supported while building with /clr
        #endif
    #endif // !_DISABLE_DEPRECATE_STATIC_CPPLIB
    #ifdef _M_CEE_PURE
        #error _STATIC_CPPLIB cannot be used with /clr:pure (the resulting assembly would not be pure)
    #endif
#endif // _STATIC_CPPLIB

#if defined(_M_CEE_PURE) && !defined(_SILENCE_CLR_PURE_DEPRECATION_WARNING)
	#pragma message(_WARNING_MESSAGE("STL4001", "/clr:pure is deprecated and will be REMOVED."))
#endif

 #ifndef _MRTIMP2_PURE
  #if defined(_M_CEE_PURE)
   #define _MRTIMP2_PURE
  #else
   #define _MRTIMP2_PURE _MRTIMP2
  #endif
 #endif

 #ifndef _MRTIMP2_PURE_NPURE
  #if defined(_M_CEE_PURE)
   #define _MRTIMP2_PURE_NPURE
  #else
   #define _MRTIMP2_PURE_NPURE _MRTIMP2_NPURE
  #endif
 #endif

/* Define _MRTIMP2_NPURE */
 #ifndef _MRTIMP2_NPURE
  #if   defined(MRTDLL) && defined(_CRTBLD)
    #if !defined(_M_CEE_PURE)
      #define _MRTIMP2_NPURE __declspec(dllexport)
    #else
      #define _MRTIMP2_NPURE
    #endif
  #else   /* ndef MRTDLL && _CRTBLD */
   #define _MRTIMP2_NPURE
  #endif  /* MRTDLL && _CRTBLD */
 #endif  /* _MRTIMP2_NPURE */

 #if defined(_DLL) && !defined(_STATIC_CPPLIB) && !defined(_M_CEE_PURE)
  #define _DLL_CPPLIB
 #endif

 #ifndef _CRTIMP2_PURE
  #if   defined(MRTDLL) && defined(_CRTBLD)
   #define _CRTIMP2_PURE
  #else
   #ifdef  _M_CEE_PURE
     #define _CRTIMP2_PURE
   #else
     #define _CRTIMP2_PURE _CRTIMP2
   #endif
  #endif
 #endif

 #ifdef _CRTBLD
/* These functions are for enabling STATIC_CPPLIB functionality */
  #define _cpp_stdin  (__acrt_iob_func(0))
  #define _cpp_stdout (__acrt_iob_func(1))
  #define _cpp_stderr (__acrt_iob_func(2))
  #define _cpp_isleadbyte(c) (__pctype_func()[(unsigned char)(c)] & _LEADBYTE)
 #endif  /* _CRTBLD */

#ifndef _CRTIMP2_IMPORT
 #if defined(CRTDLL2) && defined(_CRTBLD)
  #define _CRTIMP2_IMPORT __declspec(dllexport)
 #else
  #if defined(_DLL) && !defined(_STATIC_CPPLIB)
   #define _CRTIMP2_IMPORT __declspec(dllimport)
  #else
   #define _CRTIMP2_IMPORT
  #endif
 #endif
#endif

#ifndef _CRTIMP2_PURE_IMPORT
 #if defined(MRTDLL) && defined(_CRTBLD)
  #define _CRTIMP2_PURE_IMPORT
 #else
  #ifdef _M_CEE_PURE
   #define _CRTIMP2_PURE_IMPORT
  #else
   #define _CRTIMP2_PURE_IMPORT _CRTIMP2_IMPORT
  #endif
 #endif
#endif

#ifndef _CRTDATA2_IMPORT
 #if defined(MRTDLL) && defined(_CRTBLD)
  #define _CRTDATA2_IMPORT
 #else
  #define _CRTDATA2_IMPORT _CRTIMP2_IMPORT
 #endif
#endif

#ifndef _CRTIMP2_NCEEPURE_IMPORT
 #ifdef _M_CEE_PURE
  #define _CRTIMP2_NCEEPURE_IMPORT
 #else
  #define _CRTIMP2_NCEEPURE_IMPORT _CRTIMP2_IMPORT
 #endif
#endif

		/* NAMESPACE */
#define _STD_BEGIN	namespace std {
#define _STD_END	}
#define _STD	::std::

/*
We use the stdext (standard extension) namespace to contain extensions that are not part of the current standard
*/
#define _STDEXT_BEGIN	namespace stdext {
#define _STDEXT_END		}
#define _STDEXT			::stdext::

#ifdef __cplusplus
 #define _CSTD	::

 #define _EXTERN_C			extern "C" {
 #define _END_EXTERN_C		}
#else /* ^^^ __cplusplus ^^^ // vvv !__cplusplus vvv */
 #define _CSTD

 #define _EXTERN_C
 #define _END_EXTERN_C
#endif /* __cplusplus */

		/* INTEGER PROPERTIES */
#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG_V1	36
#define _MAX_SIG_DIG_V2	768

		/* STDIO PROPERTIES */
#define _Filet FILE

		/* MULTITHREAD PROPERTIES */
		/* LOCK MACROS */
#define _LOCK_LOCALE			0
#define _LOCK_MALLOC			1
#define _LOCK_STREAM			2
#define _LOCK_DEBUG				3
#define _LOCK_AT_THREAD_EXIT	4

 #ifdef __cplusplus
_STD_BEGIN
enum _Uninitialized
	{	// tag for suppressing initialization
	_Noinit
	};

		// CLASS _Lockit
class _CRTIMP2_PURE_IMPORT _Lockit
	{	// lock while object in existence -- MUST NEST
public:
 #if defined(_M_CEE_PURE) || defined(MRTDLL)
	__CLR_OR_THIS_CALL _Lockit()
		: _Locktype(0)
		{	// default construct
		_Lockit_ctor(this);
		}

	explicit __CLR_OR_THIS_CALL _Lockit(int _Kind)
		{	// set the lock
		_Lockit_ctor(this, _Kind);
		}

	__CLR_OR_THIS_CALL ~_Lockit() _NOEXCEPT
		{	// clear the lock
		_Lockit_dtor(this);
		}

 #else /* defined(_M_CEE_PURE) || defined(MRTDLL) */
	__thiscall _Lockit();	// default construct
	explicit __thiscall _Lockit(int);	// set the lock
	__thiscall ~_Lockit() _NOEXCEPT;	// clear the lock
 #endif /* defined(_M_CEE_PURE) || defined(MRTDLL) */

	static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(int);
	static _MRTIMP2_NPURE void __cdecl _Lockit_dtor(int);

private:
	static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(_Lockit *);
	static _MRTIMP2_NPURE void __cdecl _Lockit_ctor(_Lockit *, int);
	static _MRTIMP2_NPURE void __cdecl _Lockit_dtor(_Lockit *);

public:
	__CLR_OR_THIS_CALL _Lockit(const _Lockit&) = delete;
	_Lockit& __CLR_OR_THIS_CALL operator=(const _Lockit&) = delete;

private:
	int _Locktype;
	};

 #ifdef _M_CEE
class _CRTIMP2_PURE_IMPORT _EmptyLockit
	{	// empty lock class used for bin compat
private:
	int _Locktype;
	};

  #ifndef _PREPARE_CONSTRAINED_REGIONS
   #ifdef _M_CEE_PURE
    #define _PREPARE_CONSTRAINED_REGIONS	1
   #else /* _M_CEE_PURE */
    #define _PREPARE_CONSTRAINED_REGIONS	0
   #endif /* _M_CEE_PURE */
  #endif /* _PREPARE_CONSTRAINED_REGIONS */

  #if _PREPARE_CONSTRAINED_REGIONS
   #define _BEGIN_LOCK(_Kind) \
	{ \
	bool _MustReleaseLock = false; \
	int _LockKind = _Kind; \
	System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions(); \
	try \
		{ \
		System::Runtime::CompilerServices::RuntimeHelpers::PrepareConstrainedRegions(); \
		try \
			{ \
			} \
		finally \
			{ \
			_STD _Lockit::_Lockit_ctor(_LockKind); \
			_MustReleaseLock = true; \
			}

   #define _END_LOCK() \
		} \
	finally \
		{ \
		if (_MustReleaseLock) \
			{ \
			_STD _Lockit::_Lockit_dtor(_LockKind); \
			} \
		} \
	}

  #else /* _PREPARE_CONSTRAINED_REGIONS */
   #define _BEGIN_LOCK(_Kind) \
	{ \
		_STD _Lockit _Lock(_Kind);

   #define _END_LOCK() \
	}

  #endif /* _PREPARE_CONSTRAINED_REGIONS */

  #define _BEGIN_LOCINFO(_VarName) \
	_BEGIN_LOCK(_LOCK_LOCALE) \
	_Locinfo _VarName;

  #define _END_LOCINFO() \
	_END_LOCK()

  #define _RELIABILITY_CONTRACT \
	[System::Runtime::ConstrainedExecution::ReliabilityContract( \
		System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
		System::Runtime::ConstrainedExecution::Cer::Success)]

 #else /* _M_CEE */
  #define _BEGIN_LOCK(_Kind) \
	{ \
		_STD _Lockit _Lock(_Kind);

  #define _END_LOCK() \
	}

  #define _BEGIN_LOCINFO(_VarName) \
	{ \
		_Locinfo _VarName;

  #define _END_LOCINFO() \
	}

  #define _RELIABILITY_CONTRACT
 #endif /* _M_CEE */

class _CRTIMP2_PURE_IMPORT _Init_locks
	{	// initialize mutexes
public:
 #if defined(_M_CEE_PURE) || defined(MRTDLL)
	__CLR_OR_THIS_CALL _Init_locks()
		{	// default construct
		_Init_locks_ctor(this);
		}

	__CLR_OR_THIS_CALL ~_Init_locks() _NOEXCEPT
		{	// destroy the object
		_Init_locks_dtor(this);
		}

 #else /* defined(_M_CEE_PURE) || defined(MRTDLL) */
	__thiscall _Init_locks();
	__thiscall ~_Init_locks() _NOEXCEPT;
 #endif /* defined(_M_CEE_PURE) || defined(MRTDLL) */

private:
	static _MRTIMP2_NPURE void __cdecl _Init_locks_ctor(_Init_locks *);
	static _MRTIMP2_NPURE void __cdecl _Init_locks_dtor(_Init_locks *);
	};

_STD_END
 #endif /* __cplusplus */

#ifndef _RELIABILITY_CONTRACT
 #define _RELIABILITY_CONTRACT
#endif /* _RELIABILITY_CONTRACT */

		/* MISCELLANEOUS MACROS AND TYPES */
_MRTIMP2 void __cdecl _Atexit(void (__cdecl *)(void));

typedef unsigned long _Uint32t;

#define _Mbstinit(x)	mbstate_t x = {}

 #pragma pop_macro("new")
 _STL_RESTORE_CLANG_WARNINGS
 #pragma warning(pop)
 #pragma pack(pop)
#endif /* RC_INVOKED */
#endif /* _YVALS */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
