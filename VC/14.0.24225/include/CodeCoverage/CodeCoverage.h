/*=++=
*CodeCoverage.h
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the macros which are used to add the cpp code coverage filters
*
=--=*/


#pragma once

#ifndef MS_CPP_CODECOVERAGEFRAMEWORK
#define MS_CPP_CODECOVERAGEFRAMEWORK

/* Sample:

// If CODE_COVERAGE_DISABLE_ALL_FILTERS was defined, filter information will not be emitted into binary.
// #define CODE_COVERAGE_DISABLE_ALL_FILTERS

#include <CodeCoverage\CodeCoverage.h>

// Exclude by source file/path:
//  1. UNC is supported
//  2. file/path is case-insensitive
//  3. relative path ("." and "..") is not supported
//  4. path will not be normalized (e.g. "a/b" will not match "a\\b")

ExcludeSourceFromCodeCoverage(MySourceFilter1, L"*\\CAuto*.?pp")
ExcludeSourceFromCodeCoverage(MySourceFilter2, L"*\\boost\\src\\*")
ExcludeSourceFromCodeCoverage(MySourceFilter3, L"\\\\192.168.0.1\\src\\*")
ExcludeSourceFromCodeCoverage(MySourceFilter4, L"\\\\winsrv2008\\src\\filter\\*")

// Exclude by function name:
//  1. function name is case-sensitive
//  2. function name will not be normalized (e.g. "vector< int >" will not match "vector<int>")

ExcludeFromCodeCoverage(MyFunctionFilter1, L"functionfilter")
ExcludeFromCodeCoverage(MyFunctionFilter2, L"MyClass<*>::*")
ExcludeFromCodeCoverage(MyFunctionFilter3, L"*::~*")

*/

/* Code coverage is only supported for x86 and amd64.
*/
#if defined(_M_IX86) || defined(_M_AMD64)

/* Helper macros
*/
#define CODE_COVERAGE_CONCAT(X, Y) X##Y
#define CODE_COVERAGE_CONCAT2(X, Y) CODE_COVERAGE_CONCAT(X, Y)
#define CODE_COVERAGE_TOSTRING(X) #X
#define CODE_COVERAGE_TOSTRING2(X) CODE_COVERAGE_TOSTRING(X)

#else

#if !defined(CODE_COVERAGE_DISABLE_ALL_FILTERS)
#define CODE_COVERAGE_DISABLE_ALL_FILTERS
#endif /* CODE_COVERAGE_DISABLE_ALL_FILTERS */

#endif

#if defined(CODE_COVERAGE_DISABLE_ALL_FILTERS)

/* If CODE_COVERAGE_DISABLE_ALL_FILTERS was defined, filter information will not be emitted into binary.
   The generated namespaces were kept for diagnostic purpose in combination with "cl.exe /E".
*/
#define ExcludeSourceFromCodeCoverage(CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN) \
    namespace Microsoft{ namespace VisualStudio{ namespace CppCodeCoverageFramework{ namespace CODE_COVERAGE_FILTER_NAME { \
    }}}}

#define ExcludeFromCodeCoverage(CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN) \
    namespace Microsoft{ namespace VisualStudio{ namespace CppCodeCoverageFramework{ namespace CODE_COVERAGE_FILTER_NAME { \
    }}}}

#else /* CODE_COVERAGE_DISABLE_ALL_FILTERS */

/* Macros used by cpp code coverage internally
*/
#define CODE_COVERAGE_FILTER(CODE_COVERAGE_FILTER_CATEGORY, CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN) \
    namespace Microsoft{ namespace VisualStudio{ namespace CppCodeCoverageFramework{ namespace CODE_COVERAGE_FILTER_CATEGORY{ \
        __pragma(optimize("", off)) \
        __declspec(dllexport) __declspec(noinline) void* CODE_COVERAGE_FILTER_NAME() \
        { \
            __annotation( \
                CODE_COVERAGE_CONCAT2(L, CODE_COVERAGE_TOSTRING2(CODE_COVERAGE_FILTER_NAME)) \
                L" " \
                CODE_COVERAGE_FILTER_PATTERN); \
            return CODE_COVERAGE_TOSTRING2(CODE_COVERAGE_FILTER_NAME); \
        } \
        __pragma(optimize("", on)) \
    }}}}

/* Macros to define your customized source file/path filters
*/
#define ExcludeSourceFromCodeCoverage(CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN) \
    CODE_COVERAGE_FILTER(SourceFilter, CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN)

/* Macros to define your customized function/method filters
*/
#define ExcludeFromCodeCoverage(CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN) \
    CODE_COVERAGE_FILTER(FunctionFilter, CODE_COVERAGE_FILTER_NAME, CODE_COVERAGE_FILTER_PATTERN)

#endif /* CODE_COVERAGE_DISABLE_ALL_FILTERS */

#endif /* MS_CPP_CODECOVERAGEFRAMEWORK */
