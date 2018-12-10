//
// corecrt_internal.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// CoreCRT Internals
//
#pragma once

#include <corecrt.h>
#include <corecrt_startup.h>
#include <corecrt_terminate.h>
#include <crtdbg.h>
#include <errno.h>
#include <excpt.h>
#include <internal_shared.h>
#include <limits.h>
#include <malloc.h>
#include <process.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vcruntime_startup.h>
#include <windows.h>
#include <appmodel.h>

#ifdef __cplusplus
    #include <roapi.h>
#endif

_CRT_BEGIN_C_HEADER



#define _DEFINE_SET_FUNCTION(function_name, type, variable_name) \
    __inline void function_name(type value)                      \
    {                                                            \
        __pragma(warning(push))                                  \
        __pragma(warning(disable:4996))                          \
        variable_name = value;                                   \
        __pragma(warning(pop))                                   \
    }



#if defined _M_IX86
    #define _CRT_LINKER_SYMBOL_PREFIX "_"
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64
    #define _CRT_LINKER_SYMBOL_PREFIX ""
#else
    #error Unsupported architecture
#endif

#define _CRT_LINKER_FORCE_INCLUDE(name) \
    __pragma(comment(linker,            \
        "/include:"                     \
        _CRT_LINKER_SYMBOL_PREFIX #name \
        ))



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CRT SAL Annotations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This macro can be used to annotate a buffer when it has the option that
// SIZE_MAX may be passed as it's size in order to invoke unsafe behavior.
// void example(
//    _Maybe_unsafe_(_Out_writes_z_, buffer_count) char * const buffer,
//    _In_                                         size_t const buffer_size
// )
#define _Maybe_unsafe_(buffer_annotation, expr)                                                 \
        _When_((expr < static_cast<size_t>(-1)), buffer_annotation(expr))                       \
        _When_((expr >= static_cast<size_t>(-1)), buffer_annotation(_Inexpressible_("unsafe")))



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Forward declarations of __crt_state_management types
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __cplusplus
extern "C++"
{
    namespace __crt_state_management
    {
        template <typename T>
        class dual_state_global;
    }
}
#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Dual-state globals (that shouldn't be exported directly)
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __cplusplus
    __declspec(dllimport) extern int           _fmode;
    __declspec(dllimport) extern unsigned char _mbctype[];
    __declspec(dllimport) extern unsigned char _mbcasemap[];
#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Dynamic Initialization Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define _CORECRT_GENERATE_FORWARDER(prefix, callconv, name, callee_name)                     \
    __pragma(warning(push))                                                                  \
    __pragma(warning(disable: 4100)) /* unreferenced formal parameter */                     \
    template <typename... Params>                                                            \
    prefix auto callconv name(Params&&... args) throw() -> decltype(callee_name(args...))    \
    {                                                                                        \
        _BEGIN_SECURE_CRT_DEPRECATION_DISABLE                                                \
        return callee_name(args...);                                                         \
        _END_SECURE_CRT_DEPRECATION_DISABLE                                                  \
    }                                                                                        \
    __pragma(warning(pop))



_Ret_z_
__inline char const* _get_sys_err_msg(size_t const m)
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    if (m >= (size_t)_sys_nerr)
        return _sys_errlist[_sys_nerr];

    return _sys_errlist[m];
    _END_SECURE_CRT_DEPRECATION_DISABLE
}

#ifdef __cplusplus
    // The maximum number of characters (including null terminator) of a system
    // error message.  Note:  This value must correspond to the number of characters
    // in the longest message in the error list.
    size_t const max_system_error_message_count = 38;

    // The number of characters in the per-thread strerror buffer.  This has room
    // for a 94 character user-provided prefix, a ": " delimiter, and one of the
    // system error messages.
    size_t const strerror_buffer_count = (94 + max_system_error_message_count + 2);
#endif



_Success_(return != 0)
char* __cdecl __acrt_getpath(
    _In_z_                       char const* delimited_paths,
    _Out_writes_z_(buffer_count) char*       buffer,
    _In_                         size_t      buffer_count
    );

_Success_(return != 0)
wchar_t* __cdecl __acrt_wgetpath(
    _In_z_                       wchar_t const* delimited_paths,
    _Out_writes_z_(buffer_count) wchar_t*       buffer,
    _In_                         size_t         buffer_count
    );

_Success_(return == 0)
errno_t __acrt_expand_narrow_argv_wildcards(
    _In_z_               char**  argv,
    _Out_ _Deref_post_z_ char*** result
    );

_Success_(return == 0)
errno_t __acrt_expand_wide_argv_wildcards(
    _In_                 wchar_t**  argv,
    _Out_ _Deref_post_z_ wchar_t*** result
    );

_Success_(return == 0)
_Ret_range_(-1, 0)
int __cdecl __acrt_pack_narrow_command_line_and_environment(
    _In_z_                    char const* const* argv,
    _In_opt_z_                char const* const* envp,
    _Outptr_result_maybenull_ char**             command_line_result,
    _Outptr_result_maybenull_ char**             environment_block_result
    );

_Success_(return == 0)
_Ret_range_(-1, 0)
int __cdecl __acrt_pack_wide_command_line_and_environment(
    _In_z_                    wchar_t const* const* argv,
    _In_opt_z_                wchar_t const* const* envp,
    _Outptr_result_maybenull_ wchar_t**             command_line_result,
    _Outptr_result_maybenull_ wchar_t**             environment_block_result
    );

_Ret_z_
_Success_(return != 0)
char** __acrt_capture_narrow_argv(
    _In_                             va_list*    arglist,
    _In_z_                           char const* first_argument,
    _When_(return == caller_array, _Post_z_)
    _Out_writes_(caller_array_count) char**      caller_array,
    _In_                             size_t      caller_array_count
    );

_Ret_z_
_Success_(return != 0)
wchar_t** __acrt_capture_wide_argv(
    _In_                             va_list*       arglist,
    _In_z_                           wchar_t const* first_argument,
    _When_(return == caller_array, _Post_z_)
    _Out_writes_(caller_array_count) wchar_t**      caller_array,
    _In_                             size_t         caller_array_count
    );

void __cdecl __acrt_call_reportfault(
    int   nDbgHookCode,
    DWORD dwExceptionCode,
    DWORD dwExceptionFlags
    );



#ifndef _M_CEE_PURE
    _DEFINE_SET_FUNCTION(_set_pgmptr,  _In_z_ char*,    _pgmptr)
    _DEFINE_SET_FUNCTION(_set_wpgmptr, _In_z_ wchar_t*, _wpgmptr)
#endif



extern _CRT_ALLOC_HOOK _pfnAllocHook;

BOOL __cdecl __acrt_CreateProcessA(
    _In_opt_    LPCSTR                lpApplicationName,
    _Inout_opt_ LPSTR                 lpCommandLine,
    _In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_        BOOL                  bInheritHandles,
    _In_        DWORD                 dwCreationFlags,
    _In_opt_    LPVOID                lpEnvironment,
    _In_opt_    LPCSTR                lpCurrentDirectory,
    _In_        LPSTARTUPINFOW        lpStartupInfo,
    _Out_       LPPROCESS_INFORMATION lpProcessInformation
    );

_Success_(return > 0)
DWORD __cdecl __acrt_GetTempPathA(
    DWORD nBufferLength,
    _Out_writes_to_(nBufferLength, return + 1) PSTR lpBuffer
    );

HANDLE __cdecl __acrt_FindFirstFileExA(
    _In_       LPCSTR             lpFileName,
    _In_       FINDEX_INFO_LEVELS fInfoLevelId,
    _Out_      LPVOID             lpFindFileData,
    _In_       FINDEX_SEARCH_OPS  fSearchOp,
    _Reserved_ LPVOID             lpSearchFilter,
    _In_       DWORD              dwAdditionalFlags
    );

BOOL __cdecl __acrt_FindNextFileA(
    _In_  HANDLE             hFindFile,
    _Out_ WIN32_FIND_DATAA * lpFindFileData
    );

DWORD __cdecl __acrt_GetModuleFileNameA(
    _In_opt_ HMODULE hModule,
    _Out_    LPSTR  lpFilename,
    _In_     DWORD   nSize
    );

HMODULE __cdecl __acrt_LoadLibraryExA(
    _In_       LPCSTR lpFileName,
    _Reserved_ HANDLE  hFile,
    _In_       DWORD   dwFlags
    );

BOOL __cdecl __acrt_SetEnvironmentVariableA(
    _In_     LPCSTR lpName,
    _In_opt_ LPCSTR lpValue
    );

BOOL __cdecl __acrt_SetCurrentDirectoryA(
    _In_ LPCSTR lpPathName
    );


// Adding some defines which are used in dbgrpt.c
#define DBGRPT_MAX_MSG 4096
#define DBGRPT_TOOLONGMSG "_CrtDbgReport: String too long or IO Error"
#define DBGRPT_INVALIDMSG "_CrtDbgReport: String too long or Invalid characters in String"

#ifdef __cplusplus
extern "C++"
{
    template <typename Character>
    struct __crt_report_hook_node
    {
        using hook_type = int(__CRTDECL*)(int, Character*, int*);

        __crt_report_hook_node* prev;
        __crt_report_hook_node* next;
        unsigned                refcount;
        hook_type               hook;
    };
}
#endif

_Success_(return != 0)
unsigned char* __cdecl __acrt_allocate_buffer_for_argv(
    _In_ size_t argument_count,
    _In_ size_t character_count,
    _In_ size_t character_size
    );

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CoreCRT Shared Initialization Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef bool (__cdecl* __acrt_initialize_pft  )(void);
typedef bool (__cdecl* __acrt_uninitialize_pft)(bool);

typedef struct __acrt_initializer
{
    __acrt_initialize_pft   _initialize;
    __acrt_uninitialize_pft _uninitialize;
} __acrt_initializer;

bool __cdecl __acrt_execute_initializers(
    _In_reads_(last - first) _In_ __acrt_initializer const* first,
    _In_reads_(0) __acrt_initializer const* last
    );

bool __cdecl __acrt_execute_uninitializers(
    _In_reads_(last - first) __acrt_initializer const* first,
    _In_reads_(0) __acrt_initializer const* last
    );



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CoreCRT Fatal Runtime Error Reporting
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void __cdecl __acrt_report_runtime_error(_In_z_ wchar_t const* message);

int __cdecl __acrt_show_narrow_message_box(
    _In_opt_z_ char const* text,
    _In_opt_z_ char const* caption,
    _In_       unsigned    type
    );

int __cdecl __acrt_show_wide_message_box(
    _In_opt_z_ wchar_t const* text,
    _In_opt_z_ wchar_t const* caption,
    _In_       unsigned       type
    );



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Initialization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool __cdecl __acrt_initialize_winapi_thunks(void);
bool __cdecl __acrt_uninitialize_winapi_thunks(_In_ bool terminating);

bool __cdecl __acrt_initialize_locks(void);
bool __cdecl __acrt_uninitialize_locks(_In_ bool terminating);

bool __cdecl __acrt_initialize_heap(void);
bool __cdecl __acrt_uninitialize_heap(_In_ bool terminating);

bool __cdecl __acrt_initialize_ptd(void);
bool __cdecl __acrt_uninitialize_ptd(_In_ bool terminating);

bool __cdecl __acrt_initialize_lowio(void);
bool __cdecl __acrt_uninitialize_lowio(_In_ bool terminating);

bool __cdecl __acrt_initialize_command_line(void);
bool __cdecl __acrt_uninitialize_command_line(_In_ bool terminating);

void __cdecl __acrt_initialize_invalid_parameter_handler(_In_opt_ void* encoded_null);
void __cdecl __acrt_initialize_new_handler(_In_opt_ void* encoded_null);
void __cdecl __acrt_initialize_signal_handlers(_In_opt_ void* encoded_null);
void __cdecl __acrt_initialize_thread_local_exit_callback(_In_opt_ void * encoded_null);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Locale
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// The offset to where ctype will point.  Look in initctype.cpp for how it is
// being used.  It was introduced so that pctype can work with unsigned char
// types and EOF.  It is used only in initctype and setlocale.
#define _COFFSET 127

// Maximum lengths for the language name, country name, and full locale name.
#define MAX_LANG_LEN 64
#define MAX_CTRY_LEN 64
#define MAX_LC_LEN   (MAX_LANG_LEN + MAX_CTRY_LEN + 3)

#define MAX_CP_LEN   16  // Maximum code page name length
#define CATNAMES_LEN 57  // "LC_COLLATE=;LC_CTYPE=;..." length

#define LC_INT_TYPE  0
#define LC_STR_TYPE  1
#define LC_WSTR_TYPE 2

#define _PER_THREAD_LOCALE_BIT  0x2
#define _GLOBAL_LOCALE_BIT      0x1



typedef struct __crt_locale_string_table
{
    wchar_t* szName;
    wchar_t  chAbbrev[4];
} __crt_locale_string_table;

typedef struct __crt_locale_strings
{
    wchar_t szLanguage  [MAX_LANG_LEN];
    wchar_t szCountry   [MAX_CTRY_LEN];
    wchar_t szCodePage  [MAX_CP_LEN];
    wchar_t szLocaleName[LOCALE_NAME_MAX_LENGTH];
} __crt_locale_strings;

typedef struct _lc_time_data_msvcrt
{
    char*    wday_abbr [ 7];
    char*    wday      [ 7];
    char*    month_abbr[12];
    char*    month     [12];
    char*    ampm      [ 2];
    char*    ww_sdatefmt;
    char*    ww_ldatefmt;
    char*    ww_timefmt;

	LCID ww_lcid;  //for msvcrt等效_W_ww_locale_name

    int      ww_caltype;
    long     refcount;

	//以下成员仅Win7以及更高版本存在
    wchar_t* _W_wday_abbr [ 7];
    wchar_t* _W_wday      [ 7];
    wchar_t* _W_month_abbr[12];
    wchar_t* _W_month     [12];
    wchar_t* _W_ampm      [ 2];
    wchar_t* _W_ww_sdatefmt;
    wchar_t* _W_ww_ldatefmt;
    wchar_t* _W_ww_timefmt;
    //wchar_t* _W_ww_locale_name;
} _lc_time_data_msvcrt, __crt_lc_time_data;

typedef struct __crt_ctype_compatibility_data
{
    unsigned long id;
    int           is_clike;
} __crt_ctype_compatibility_data;

typedef struct _setloc_struct_msvcrt
{
    // Static data for qualified locale code
    char const*    pchLanguage;
	char const*    pchCountry;
    int            iLocState;
    int            iPrimaryLen;
    BOOL           bAbbrevLanguage;
    BOOL           bAbbrevCountry;
	LCID           lcidLanguage;
	LCID           lcidCountry;
	/* expand_locale static variables */
	LC_ID          _cacheid; //等效_cacheLocaleName

    UINT           _cachecp;
	char           _cachein [MAX_LC_LEN];
	char           _cacheout[MAX_LC_LEN];

    // Static data for LC_CTYPE
    __crt_ctype_compatibility_data _Loc_c[5];
    //wchar_t                        _cacheLocaleName[LOCALE_NAME_MAX_LENGTH];
} _setloc_struct_msvcrt,_setloc_struct,__crt_qualified_locale_data;

typedef struct __crt_qualified_locale_data_downlevel
{
    // Static data for the downlevel qualified locale code
    int iLcidState;
    LCID lcidLanguage;
    LCID lcidCountry;
} __crt_qualified_locale_data_downlevel;

typedef struct _multibyte_data_msvcrt
{
    long           refcount;
    int            mbcodepage;
    int            ismbcodepage;
	int mblcid; //for msvcrt
    unsigned short mbulinfo[6];
    unsigned char  mbctype[257];
    unsigned char  mbcasemap[256];
    //wchar_t const* mblocalename;
} _multibyte_data_msvcrt, _multibyte_data,__crt_multibyte_data;

/*定义转移至corecrt.h
typedef struct __crt_locale_refcount
{
    char*    locale;
    wchar_t* wlocale;
    long*    refcount;
    long*    wrefcount;
} __crt_locale_refcount;

typedef struct __crt_locale_data
{
    __crt_locale_data_public  _public;
    long                      refcount;
    unsigned int              lc_collate_cp;
    unsigned int              lc_time_cp;
    int                       lc_clike;
    __crt_locale_refcount     lc_category[6];
    long*                     lconv_intl_refcount;
    long*                     lconv_num_refcount;
    long*                     lconv_mon_refcount;
    struct lconv*             lconv;
    long*                     ctype1_refcount;
    unsigned short*           ctype1;
    unsigned char const*      pclmap;
    unsigned char const*      pcumap;
    __crt_lc_time_data const* lc_time_curr;
    wchar_t*                  locale_name[6];
} __crt_locale_data;*/

// Unusual: < 0 => string length
//           else scan up to specified size or string length, whichever comes first
// The name is based on _In_NLS_string_.
#define _In_CRT_NLS_string_(size) _When_((size) < 0,  _In_z_)                               \
                                  _When_((size) >= _String_length_(_Curr_), _Pre_z_)        \
                                  _When_((size) < _String_length_(_Curr_), _In_reads_(size))

// Wrappers for locale-related Windows API functionality
/*int __cdecl __acrt_CompareStringA(
    _In_opt_               _locale_t _Plocinfo,
    _In_                   LPCWSTR   _LocaleName,
    _In_                   DWORD     _DwCmpFlags,
    _In_CRT_NLS_string_(_CchCount1) PCCH _LpString1,
    _In_                   int       _CchCount1,
    _In_CRT_NLS_string_(_CchCount2) PCCH _LpString2,
    _In_                   int       _CchCount2,
    _In_                   int       _CodePage
    );

int __cdecl __acrt_CompareStringW(
    _In_                   LPCWSTR  _LocaleName,
    _In_                   DWORD    _DwCmpFlags,
    _In_CRT_NLS_string_(_CchCount1) PCWCH _LpString1,
    _In_                   int      _CchCount1,
    _In_CRT_NLS_string_(_CchCount2) PCWCH _LpString2,
    _In_                   int      _CchCount2
    );*/

int __cdecl __acrt_GetLocaleInfoA(
    _In_opt_ _locale_t _Locale,
    _In_     int       _LCType,
    _In_     LPCWSTR   _LocaleName,
    _In_     LCTYPE    _FieldType,
    _Out_    void*     _Address
    );

_Success_(return)
BOOL __cdecl __acrt_GetStringTypeA(
    _In_opt_            _locale_t _Plocinfo,
    _In_                DWORD     _DWInfoType,
    _In_NLS_string_(_CchSrc) PCCH _LpSrcStr,
    _In_                int       _CchSrc,
    _Out_               LPWORD    _LpCharType,
    _In_                int       _CodePage,
    _In_                BOOL      _BError
    );

_Success_(return)
BOOL __cdecl __acrt_GetStringTypeW(
    _In_                DWORD       _DWInfoType,
    _In_NLS_string_(_CchSrc) PCWCH  _LpSrcStr,
    _In_                int         _CchSrc,
    _Out_               LPWORD      _LpCharType
);

/*_Success_(return != 0)
int __cdecl __acrt_LCMapStringA(
    _In_opt_                   _locale_t _Plocinfo,
    _In_                       LPCWSTR   _LocaleName,
    _In_                       DWORD     _DwMapFlag,
    _In_CRT_NLS_string_(_CchSrc) PCCH    _LpSrcStr,
    _In_                       int       _CchSrc,
    _Out_writes_opt_(_CchDest) PCH       _LpDestStr,
    _In_                       int       _CchDest,
    _In_                       int       _CodePage,
    _In_                       BOOL      _BError
    );

_Success_(return != 0)
int __cdecl __acrt_LCMapStringW(
    _In_                       LPCWSTR _LocaleName,
    _In_                       DWORD   _DWMapFlag,
    _In_CRT_NLS_string_(_CchSrc) PCWCH _LpSrcStr,
    _In_                       int     _CchSrc,
    _Out_writes_opt_(_CchDest) PWCH    _LpDestStr,
    _In_                       int     _CchDest
    );

_Success_(return != 0)
int __cdecl __acrt_MultiByteToWideChar(
    _In_                           UINT    _CodePage,
    _In_                           DWORD   _DWFlags,
    _In_                           LPCSTR  _LpMultiByteStr,
    _In_                           int     _CbMultiByte,
    _Out_writes_opt_(_CchWideChar) LPWSTR  _LpWideCharStr,
    _In_                           int     _CchWideChar
    );*/
#define __acrt_MultiByteToWideChar MultiByteToWideChar

/*_Success_(return != 0)
int __cdecl __acrt_WideCharToMultiByte(
    _In_                           UINT    _CodePage,
    _In_                           DWORD   _DWFlags,
    _In_                           LPCWSTR _LpWideCharStr,
    _In_                           int     _CchWideChar,
    _Out_writes_opt_(_CbMultiByte) LPSTR   _LpMultiByteStr,
    _In_                           int     _CbMultiByte,
    _In_opt_                       LPCSTR  _LpDefaultChar,
    _Out_opt_                      LPBOOL  _LpUsedDefaultChar
    );*/
#define __acrt_WideCharToMultiByte WideCharToMultiByte

// Case-insensitive ASCII comparisons
_Check_return_
int __cdecl __ascii_memicmp(
    _In_reads_bytes_opt_(size) void const* buffer1,
    _In_reads_bytes_opt_(size) void const* buffer2,
    _In_                       size_t      size
    );

_Check_return_
int __cdecl __ascii_stricmp(
    _In_z_ char const* string1,
    _In_z_ char const* string2
    );

_Check_return_
int __cdecl __ascii_strnicmp(
    _In_reads_or_z_(max_count) char const* string1,
    _In_reads_or_z_(max_count) char const* string2,
    _In_                       size_t      max_count
    );



// Locale reference counting
void __cdecl __acrt_add_locale_ref    (__crt_locale_data*);
void __cdecl __acrt_release_locale_ref(__crt_locale_data*);
void __cdecl __acrt_free_locale       (__crt_locale_data*);

long __cdecl __acrt_locale_add_lc_time_reference(
    _In_opt_ __crt_lc_time_data const* lc_time
    );

long __cdecl __acrt_locale_release_lc_time_reference(
    _In_opt_ __crt_lc_time_data const* lc_time
    );

void __cdecl __acrt_locale_free_lc_time_if_unreferenced(
    _In_opt_ __crt_lc_time_data const* lc_time
    );

// Initialization and cleanup functions for locale categories
_Success_(return == 0)
int __cdecl __acrt_locale_initialize_ctype(
    _Inout_ __crt_locale_data* locale_data
    );

_Success_(return == 0)
int __cdecl __acrt_locale_initialize_monetary(
    _Inout_ __crt_locale_data* locale_data
    );

_Success_(return == 0)
int __cdecl __acrt_locale_initialize_numeric(
    _Inout_ __crt_locale_data* locale_data
    );

_Success_(return == 0)
int __cdecl __acrt_locale_initialize_time(
    _Inout_ __crt_locale_data* locale_data
    );

void __cdecl __acrt_locale_free_monetary(
    _Inout_ struct lconv* lconv
    );

void __cdecl __acrt_locale_free_numeric(
    _Inout_ struct lconv* lconv
    );

void __cdecl __acrt_locale_free_time(
    _Inout_opt_ __crt_lc_time_data* lc_time_data
    );



// Locale synchronization
__crt_locale_data*    __cdecl __acrt_update_thread_locale_data   (void);
__crt_multibyte_data* __cdecl __acrt_update_thread_multibyte_data(void);



_Success_(return != 0)
_Ret_z_
_Check_return_
wchar_t* __cdecl __acrt_copy_locale_name(
    _In_z_ wchar_t const* locale_name
    );

_Success_(return != 0)
BOOL __cdecl __acrt_get_qualified_locale(
    _In_      __crt_locale_strings const* input_strings,
    _Out_opt_ UINT*                       code_page,
    _Out_opt_ __crt_locale_strings*       result_strings
    );

_Success_(return != 0)
BOOL __cdecl __acrt_get_qualified_locale_downlevel(
    _In_      __crt_locale_strings const* input_strings,
    _Out_opt_ UINT*                       code_page,
    _Out_opt_ __crt_locale_strings*       result_strings
    );


// Global variable that is nonzero if the locale has been changed on any thread.
// Do not touch this global variable; call __acrt_locale_changed instead.
/*extern long __acrt_locale_changed_data;

#ifdef __cplusplus

    // Returns true if the locale has been changed on any thread.
    __inline bool __cdecl __acrt_locale_changed()
    {
        // No need for __crt_interlocked_read, since __acrt_locale_changed_data
        // is a 4 byte natural aligned memory, guaranteed to be atomic
        // accessed on all platforms.
        return __acrt_locale_changed_data != FALSE;
    }

#endif

void __cdecl __acrt_set_locale_changed(void);


// Non-NLS language and country/region string tables:
extern __crt_locale_string_table const __acrt_rg_language[];
extern size_t                    const __acrt_rg_language_count;

extern __crt_locale_string_table const __acrt_rg_country[];
extern size_t                    const __acrt_rg_country_count;

// The lconv and LC TIME structures for the C locale:
extern struct lconv             __acrt_lconv_c;
extern __crt_lc_time_data const __lc_time_c;

// The initial and current locale states:
extern __crt_multibyte_data  __acrt_initial_multibyte_data;
extern __crt_locale_data     __acrt_initial_locale_data;
extern __crt_locale_pointers __acrt_initial_locale_pointers;*/

#ifdef __cplusplus

    extern __crt_state_management::dual_state_global<
        __crt_locale_data*
    > __acrt_current_locale_data;

    extern __crt_state_management::dual_state_global<
        __crt_multibyte_data*
    > __acrt_current_multibyte_data;

#endif


// The current lconv structure:
extern struct lconv* __acrt_lconv;

// Character tables:
extern unsigned short const __newctype[];
extern unsigned char  const __newclmap[];
extern unsigned char  const __newcumap[];

// The name of the C locale, as a wide string ("C")
extern wchar_t __acrt_wide_c_locale_string[];



// The global locale status, to support the threadlocale.obj link option
extern int __globallocalestatus;



bool __cdecl __acrt_initialize_multibyte(void);



__inline int __CRTDECL __acrt_isleadbyte_l_noupdate(
    _In_ int       const c,
    _In_ _locale_t const locale
    )
{
	unsigned short const* _locale_pctype = locale ? locale->locinfo->_locale_pctype : __pctype_func();
    return _locale_pctype[(unsigned char)c] & _LEADBYTE;
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Math
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
void __cdecl __acrt_initialize_user_matherr(void* encoded_null);
bool __cdecl __acrt_has_user_matherr(void);
int  __cdecl __acrt_invoke_user_matherr(struct _exception*);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Threading
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct __acrt_thread_parameter
{
    // The thread procedure and context argument
    void*   _procedure;
    void*   _context;

    // The handle for the newly created thread.  This is initialized only from
    // _beginthread (not _beginthreadex).  When a thread created via _beginthread
    // exits, it frees this handle.
    HANDLE _thread_handle;

    // The handle for the module in which the user's thread procedure is defined.
    // This may be null if the handle could not be obtained.  This handle enables
    // us to bump the reference count of the user's module, to ensure that the
    // module will not be unloaded while the thread is executing.  When the thread
    // exits, it frees this handle.
    HMODULE _module_handle;

    // This flag is true if RoInitialized was called on the thread to initialize
    // it into the MTA.
    bool    _initialized_apartment;
} __acrt_thread_parameter;



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Per-Thread Data
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct _ptd_msvcrt
{
	unsigned long   _tid;       /* thread ID */


	uintptr_t _thandle;         /* thread handle */

	int                  _terrno;          // errno value
	unsigned long        _tdoserrno;       // _doserrno value
	unsigned int    _fpds;      /* Floating Point data segment */

	//unsigned long   _holdrand;  /* rand() seed value */
	unsigned int         _rand_state;      // Previous value of rand()

	//char *      _token;         /* ptr to strtok() token */
	//wchar_t *   _wtoken;        /* ptr to wcstok() token */
	//unsigned char * _mtoken;    /* ptr to _mbstok() token */
	
	// Per-thread strtok(), wcstok(), and mbstok() data:
	char*                _strtok_token;
	wchar_t*             _wcstok_token;
	unsigned char*       _mbstok_token;

	// Per-thread error message data:
	char*      _strerror_buffer;            // Pointer to strerror()  / _strerror()  buffer _errmsg
	wchar_t*   _wcserror_buffer;            // Pointer to _wcserror() / __wcserror() buffer _werrmsg

	//0x2C
	//char *      _namebuf0;      /* ptr to tmpnam() buffer */
	//wchar_t *   _wnamebuf0;     /* ptr to _wtmpnam() buffer */
	// Per-thread tmpnam() data:
	char*                _tmpnam_narrow_buffer;
	wchar_t*             _tmpnam_wide_buffer;
	//0x34
	char *      _namebuf1;      /* ptr to tmpfile() buffer */
	wchar_t *   _wnamebuf1;     /* ptr to _wtmpfile() buffer */
								//char *      _asctimebuf;    /* ptr to asctime() buffer */
								//wchar_t *   _wasctimebuf;   /* ptr to _wasctime() buffer */
								//void *      _gmtimebuf;     /* ptr to gmtime() structure */
								//char *      _cvtbuf;        /* ptr to ecvt()/fcvt buffer */

								// Per-thread time library data:
	char*                _asctime_buffer;  // Pointer to asctime() buffer
	wchar_t*             _wasctime_buffer; // Pointer to _wasctime() buffer
	//0x44
	struct tm*           _gmtime_buffer;   // Pointer to gmtime() structure

	//0x48
	char*                _cvtbuf;          // Pointer to the buffer used by ecvt() and fcvt().

	//0x4C
	union
	{
		struct
		{
												   /* following fields are needed by _beginthread code */
			void *      _initaddr;      /* initial user thread address */
			void *      _initarg;       /* initial user thread argument */

										/* following three fields are needed to support signal handling and
										* runtime errors */
			struct __crt_signal_action_t* _pxcptacttab;     // Pointer to the exception-action table
			EXCEPTION_POINTERS*           _tpxcptinfoptrs;  // Pointer to the exception info pointers
			int                           _tfpecode;        // Last floating point exception code

			//0x60

			/* pointer to the copy of the multibyte character information used by
			* the thread */
			//struct __crt_multibyte_data*  ptmbcinfo;
			struct _multibyte_data_msvcrt_winxp*                  _multibyte_info;

			//0x64
			/* pointer to the copy of the locale informaton used by the thead */
			struct _locale_data_msvcrt_winxp*                     _locale_info;

			//0x68
			/* following field is needed by NLG routines */
			unsigned long   _NLG_dwCode;

			/*
			* Per-Thread data needed by C++ Exception Handling
			*/
			//0x6C
			terminate_handler      _terminate;    // terminate() routine
			//0x70
			void *      _unexpected;    /* unexpected() routine */
			void *      _translator;    /* S.E. translator */
			void *      _curexception;  /* current exception */
			void *      _curcontext;    /* current exception context */

			//0x80
			int         _ProcessingThrow; /* for uncaught_exception */
#if defined(_M_IX86)
			//0x84
			void *      _pFrameInfoChain;
#elif defined(_M_AMD64)
			void *      _curexcspec;    /* for handling exceptions thrown from std::unexpected */
			void *      _pExitContext;
			void *      _pUnwindContext;
			//0x110
			void *      _pFrameInfoChain;
			void *      _reserved1;     /* nothing */
			void *      _reserved2;     /* nothing */
			void *      _reserved3;     /* nothing */
			//0x130
#elif defined (_M_MRX000)
			void *      _pFrameInfoChain;
			void *      _pUnwindContext;
			void *      _pExitContext;
			int         _MipsPtdDelta;
			int         _MipsPtdEpsilon;
#elif defined (_M_PPC)
			void *      _pExitContext;
			void *      _pUnwindContext;
			void *      _pFrameInfoChain;
			int         _FrameInfo[6];
#endif  /* defined (_M_PPC) */

		} XP_msvcrt; //XP专属数据结构，如果是XP，必须使用此成员

		struct
		{

			//unsigned char _con_ch_buf[MB_LEN_MAX];
			/* ptr to putch() buffer */
			//unsigned short _ch_buf_used;   /* if the _con_ch_buf is used */

			// The buffer used by _putch(), and the flag indicating whether the buffer
			// is currently in use or not.
			unsigned char  _putch_buffer[MB_LEN_MAX];
			unsigned short _putch_buffer_used;

			/* following fields are needed by _beginthread code */
			void *      _initaddr;      /* initial user thread address */
			void *      _initarg;       /* initial user thread argument */

										/* following three fields are needed to support signal handling and
										* runtime errors */
			struct __crt_signal_action_t* _pxcptacttab;     // Pointer to the exception-action table
			EXCEPTION_POINTERS*           _tpxcptinfoptrs;  // Pointer to the exception info pointers
			int                           _tfpecode;        // Last floating point exception code

										/* pointer to the copy of the multibyte character information used by
										* the thread */
										//struct __crt_multibyte_data*  ptmbcinfo;
			_multibyte_data_msvcrt*                  _multibyte_info;

			/* pointer to the copy of the locale informaton used by the thead */
			//__crt_locale_data*  ptlocinfo;
			_locale_data_msvcrt*                     _locale_info;

			int                                    _own_locale;   // If 1, this thread owns its locale

										/* following field is needed by NLG routines */
			unsigned long   _NLG_dwCode;

			/*
			* Per-Thread data needed by C++ Exception Handling
			*/
			terminate_handler      _terminate;    // terminate() routine
			void *      _unexpected;    /* unexpected() routine */
			void *      _translator;    /* S.E. translator */
			void *      _purecall;      /* called when pure virtual happens */
			void *      _curexception;  /* current exception */
			void *      _curcontext;    /* current exception context */
			int         _ProcessingThrow; /* for uncaught_exception */
			void *              _curexcspec;    /* for handling exceptions thrown from std::unexpected */
	#if defined (_M_IA64) || defined (_M_AMD64) || defined (_M_ARM64) || defined (_M_ARM)
			void *      _pExitContext;
			void *      _pUnwindContext;
			void *      _pFrameInfoChain;
			uintptr_t   _ImageBase;
	#if defined (_M_IA64)
			unsigned __int64    _TargetGp;
	#endif  /* defined (_M_IA64) */
			uintptr_t   _ThrowImageBase;
			void *      _pForeignException;
	#elif defined (_M_IX86)
			void *      _pFrameInfoChain;
	#endif  /* defined (_M_IX86) */
			__crt_qualified_locale_data            _setloc_data;

	#ifdef _M_IX86
			void *      _encode_ptr;    /* EncodePointer() routine */
			void *      _decode_ptr;    /* DecodePointer() routine */
	#endif  /* _M_IX86 */

			void *      _reserved1;     /* nothing */
			void *      _reserved2;     /* nothing */
			void *      _reserved3;     /* nothing */

			int _cxxReThrow;        /* Set to True if it's a rethrown C++ Exception */

			unsigned long __initDomain;     /* initial domain used by _beginthread[ex] for managed function */
		}VistaOrLater_msvcrt; //Vista以及以后系统专属数据结构，必须判断系统版本号才能使用

	};
#if 0
    // These three data members support signal handling and runtime errors
    struct __crt_signal_action_t* _pxcptacttab;     // Pointer to the exception-action table
    EXCEPTION_POINTERS*           _tpxcptinfoptrs;  // Pointer to the exception info pointers
    int                           _tfpecode;        // Last floating point exception code

    terminate_handler  _terminate;  // terminate() routine

    int                  _terrno;          // errno value
    unsigned long        _tdoserrno;       // _doserrno value

    unsigned int         _rand_state;      // Previous value of rand()

    // Per-thread strtok(), wcstok(), and mbstok() data:
    char*                _strtok_token;
    unsigned char*       _mbstok_token;
    wchar_t*             _wcstok_token;

    // Per-thread tmpnam() data:
    char*                _tmpnam_narrow_buffer;
    wchar_t*             _tmpnam_wide_buffer;

    // Per-thread time library data:
    char*                _asctime_buffer;  // Pointer to asctime() buffer
    wchar_t*             _wasctime_buffer; // Pointer to _wasctime() buffer
    struct tm*           _gmtime_buffer;   // Pointer to gmtime() structure

    char*                _cvtbuf;          // Pointer to the buffer used by ecvt() and fcvt().

    // Per-thread error message data:
    char*      _strerror_buffer;            // Pointer to strerror()  / _strerror()  buffer
    wchar_t*   _wcserror_buffer;            // Pointer to _wcserror() / __wcserror() buffer

    // Locale data:
    __crt_multibyte_data*                  _multibyte_info;
    __crt_locale_data*                     _locale_info;
    __crt_qualified_locale_data            _setloc_data;
    __crt_qualified_locale_data_downlevel* _setloc_downlevel_data;
    int                                    _own_locale;   // If 1, this thread owns its locale

    // The buffer used by _putch(), and the flag indicating whether the buffer
    // is currently in use or not.
    unsigned char  _putch_buffer[MB_LEN_MAX];
    unsigned short _putch_buffer_used;

    // The thread-local invalid parameter handler
    _invalid_parameter_handler _thread_local_iph;

    // If this thread was started by the CRT (_beginthread or _beginthreadex),
    // then this points to the context with which the thread was created.  If
    // this thread was not started by the CRT, this pointer is null.
    __acrt_thread_parameter* _beginthread_context;
#endif
} _ptd_msvcrt,__acrt_ptd;

__declspec(dllimport) void __cdecl _lock(
	int locknum
);
__declspec(dllimport) void __cdecl _unlock(
	int locknum
);


__acrt_ptd* __cdecl _getptd_noexit(void);

__acrt_ptd* __cdecl __acrt_getptd(void);
__acrt_ptd* __cdecl __acrt_getptd_head(void);
__forceinline __acrt_ptd* __cdecl __acrt_getptd_noexit(void)
{
	return _getptd_noexit();
}
void        __cdecl __acrt_freeptd(void);



void __cdecl __acrt_errno_map_os_error (unsigned long);
int  __cdecl __acrt_errno_from_os_error(unsigned long);


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Multi-Threading and Synchronization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef enum __acrt_lock_id
{
    __acrt_heap_lock = 4,
    __acrt_debug_lock =15,
    __acrt_exit_lock=8,
    __acrt_signal_lock =0,
    __acrt_locale_lock =12,
    __acrt_multibyte_cp_lock=13,
    __acrt_time_lock=6,
    //__acrt_lowio_index_lock,
    //__acrt_stdio_index_lock,
    __acrt_conio_lock =3,
    __acrt_popen_lock=9,
    __acrt_environment_lock=7,
    __acrt_tempnam_lock=2,
    //__acrt_os_exit_lock,
    //__acrt_lock_count
} __acrt_lock_id;

#define _CORECRT_SPINCOUNT 4000

//void __cdecl __acrt_lock(_In_ __acrt_lock_id lock);
//void __cdecl __acrt_unlock(_In_ __acrt_lock_id lock);
#define __acrt_lock _lock
#define __acrt_unlock _unlock


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CoreCRT SEH Encapsulation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __cplusplus
extern "C++"
{
    template <typename Action>
    auto __acrt_lock_and_call(__acrt_lock_id const lock_id, Action&& action) throw()
        -> decltype(action())
    {
        return __crt_seh_guarded_call<decltype(action())>()(
            [lock_id]() { __acrt_lock(lock_id); },
            action,
            [lock_id]() { __acrt_unlock(lock_id); });
    }
}
#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Heap Internals and Debug Heap Internals
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//extern HANDLE __acrt_heap;

_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size)
void* __cdecl _expand_base(
    _Pre_notnull_ void*  block,
    _In_          size_t size
    );

// For detection of heap mismatch between MSVCRT and UCRT
#define _UCRT_HEAP_MISMATCH_DETECTION   0
#define _UCRT_HEAP_MISMATCH_RECOVERY    0
#define _UCRT_HEAP_MISMATCH_BREAK       0

#define _UCRT_HEAP_MISMATCH_ANY (_UCRT_HEAP_MISMATCH_DETECTION || _UCRT_HEAP_MISMATCH_RECOVERY || _UCRT_HEAP_MISMATCH_BREAK)

#if _UCRT_HEAP_MISMATCH_ANY && (defined _M_IX86 || defined _M_AMD64)
    HANDLE __cdecl __acrt_get_msvcrt_heap_handle(void);
#endif


// CRT Allocation macros:
#ifndef _DEBUG
    #define _expand_crt    _expand

    #define _strdup_crt    _strdup

    #define _dupenv_s_crt  _dupenv_s
    #define _wdupenv_s_crt _wdupenv_s

#else
    #define _expand_crt(p, s)      _expand_dbg(p, s, _CRT_BLOCK)

    #define _strdup_crt(s)         _strdup_dbg(s, _CRT_BLOCK, __FILE__, __LINE__)

    #define _dupenv_s_crt(...)     _dupenv_s_dbg(__VA_ARGS__, _CRT_BLOCK, __FILE__, __LINE__)
    #define _wdupenv_s_crt(...)    _wdupenv_s_dbg(__VA_ARGS__, _CRT_BLOCK, __FILE__, __LINE__)

#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CoreCRT Scoped HANDLE Wrappers
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// These classes are similar to the WRL HandleT and its traits.  We cannot use
// HandleT because it has a virtual function, InternalClose().  (Every object
// that uses a HandleT instantiation also has the RTTI data for that HandleT
// instantiation. This RTTI data causes the object to depend on type_info, which
// causes an indirect dependency on operator delete.  This breaks static linking
// of the CRT, when operator delete is replaced in a user-provided object.
#ifdef __cplusplus
extern "C++"
{
    struct __crt_handle_traits
    {
        typedef HANDLE type;

        static bool close(_In_ type handle) throw()
        {
            return ::CloseHandle(handle) != FALSE;
        }

        static type get_invalid_value() throw()
        {
            return INVALID_HANDLE_VALUE;
        }
    };

    struct __crt_hmodule_traits
    {
        typedef HMODULE type;

        static bool close(_In_ type handle) throw()
        {
            return ::FreeLibrary(handle) != FALSE;
        }

        static type get_invalid_value() throw()
        {
            return nullptr;
        }
    };

    struct __crt_findfile_traits
    {
        typedef HANDLE type;

        static bool close(_In_ type handle) throw()
        {
            return ::FindClose(handle) != FALSE;
        }

        static type get_invalid_value() throw()
        {
            return INVALID_HANDLE_VALUE;
        }
    };



    template <typename Traits>
    class __crt_unique_handle_t
    {
    public:

        typedef Traits                traits_type;
        typedef typename Traits::type handle_type;

        explicit __crt_unique_handle_t(handle_type const h = traits_type::get_invalid_value()) throw()
            : _handle(h)
        {
        }

        __crt_unique_handle_t(_Inout_ __crt_unique_handle_t&& h) throw()
            : _handle(h._handle)
        {
            h._handle = traits_type::get_invalid_value();
        }

        ~__crt_unique_handle_t() throw()
        {
            close();
        }

        __crt_unique_handle_t& operator=(_Inout_ __crt_unique_handle_t&& h) throw()
        {
            close();
            _handle = h._handle;
            h._handle = traits_type::get_invalid_value();
            return *this;
        }

        void attach(handle_type h) throw()
        {
            if (h != _handle)
            {
                close();
                _handle = h;
            }
        }

        handle_type detach() throw()
        {
            handle_type h = _handle;
            _handle = traits_type::get_invalid_value();
            return h;
        }

        handle_type get() const throw()
        {
            return _handle;
        }

        void close() throw()
        {
            if (_handle == traits_type::get_invalid_value())
                return;

            traits_type::close(_handle);
            _handle = traits_type::get_invalid_value();
        }

        bool is_valid() const throw()
        {
            return _handle != traits_type::get_invalid_value();
        }

        operator bool() const throw()
        {
            return is_valid();
        }

        handle_type* get_address_of() throw()
        {
            return &_handle;
        }

        handle_type* release_and_get_address_of() throw()
        {
            close();
            return &_handle;
        }

    private:

        __crt_unique_handle_t(__crt_unique_handle_t const&) throw();
        __crt_unique_handle_t& operator=(__crt_unique_handle_t const&) throw();

        handle_type _handle;
    };

    typedef __crt_unique_handle_t<__crt_handle_traits>   __crt_unique_handle;
    typedef __crt_unique_handle_t<__crt_hmodule_traits>  __crt_unique_hmodule;
    typedef __crt_unique_handle_t<__crt_findfile_traits> __crt_findfile_handle;

} // extern "C++"
#endif // __cplusplus



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Windows API Thunks
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define __acrt_AreFileApisANSI AreFileApisANSI

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_CompareStringEx(
    _In_opt_                       LPCWSTR          locale_name,
    _In_                           DWORD            flags,
    _In_NLS_string_(string1_count) LPCWCH           string1,
    _In_                           int              string1_count,
    _In_NLS_string_(string2_count) LPCWCH           string2,
    _In_                           int              string2_count,
    _Reserved_                     LPNLSVERSIONINFO version,
    _Reserved_                     LPVOID           reserved,
    _Reserved_                     LPARAM           param
    );
#else
#define __acrt_CompareStringEx CompareStringEx
#endif

#ifdef _ATL_XP_TARGETING
BOOL WINAPI __acrt_EnumSystemLocalesEx(
    _In_     LOCALE_ENUMPROCEX enum_proc,
    _In_     DWORD             flags,
    _In_     LPARAM            param,
    _In_opt_ LPVOID            reserved
    );
#else
#define __acrt_EnumSystemLocalesEx EnumSystemLocalesEx
#endif

#ifdef _ATL_XP_TARGETING
DWORD WINAPI __acrt_FlsAlloc(
    _In_opt_ PFLS_CALLBACK_FUNCTION lpCallback
    );
#else
#define __acrt_FlsAlloc FlsAlloc
#endif

#ifdef _ATL_XP_TARGETING
BOOL WINAPI __acrt_FlsFree(
    _In_ DWORD dwFlsIndex
    );
#else
#define __acrt_FlsFree FlsFree
#endif

#ifdef _ATL_XP_TARGETING
PVOID WINAPI __acrt_FlsGetValue(
    _In_ DWORD dwFlsIndex
    );
#else
#define __acrt_FlsGetValue FlsGetValue
#endif

#ifdef _ATL_XP_TARGETING
BOOL WINAPI __acrt_FlsSetValue(
    _In_     DWORD dwFlsIndex,
    _In_opt_ PVOID lpFlsData
    );
#else
#define __acrt_FlsSetValue FlsGetValue
#endif

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_GetDateFormatEx(
    _In_opt_                       LPCWSTR           locale_name,
    _In_                           DWORD             flags,
    _In_opt_                       SYSTEMTIME CONST* date,
    _In_opt_                       LPCWSTR           format,
    _Out_writes_opt_(buffer_count) LPWSTR            buffer,
    _In_opt_                       int               buffer_count,
    _In_opt_                       LPCWSTR           calendar
    );
#else
#define __acrt_GetDateFormatEx GetDateFormatEx
#endif

DWORD64 WINAPI __acrt_GetEnabledXStateFeatures(void);

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_GetLocaleInfoEx(
    _In_opt_                       LPCWSTR locale_name,
    _In_                           LCTYPE  lc_type,
    _Out_writes_opt_(data_count)   LPWSTR  data,
    _In_                           int     data_count
    );
#else
#define __acrt_GetLocaleInfoEx GetLocaleInfoEx
#endif

#if _CRT_NTDDI_MIN < NTDDI_WIN7
VOID WINAPI __acrt_GetSystemTimePreciseAsFileTime(
    _Out_ LPFILETIME system_time
    );
#else
#define __acrt_GetSystemTimePreciseAsFileTime GetSystemTimePreciseAsFileTime
#endif

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_GetTimeFormatEx(
    _In_opt_                         LPCWSTR           locale_name,
    _In_                             DWORD             flags,
    _In_opt_                         SYSTEMTIME CONST* time,
    _In_opt_                         LPCWSTR           format,
    _Out_writes_opt_(buffer_count)   LPWSTR            buffer,
    _In_opt_                         int               buffer_count
    );
#else
#define __acrt_GetTimeFormatEx GetTimeFormatEx
#endif

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_GetUserDefaultLocaleName(
    _Out_writes_(locale_name_count) LPWSTR  locale_name,
    _In_                            int     locale_name_count
    );
#else
#define __acrt_GetUserDefaultLocaleName GetUserDefaultLocaleName
#endif

_Must_inspect_result_
BOOL WINAPI __acrt_GetXStateFeaturesMask(
    _In_  PCONTEXT context,
    _Out_ PDWORD64 feature_mask
    );

#ifdef _ATL_XP_TARGETING
#define __acrt_InitializeCriticalSectionEx(critical_section,spin_count,flags) InitializeCriticalSectionAndSpinCount(critical_section,spin_count)
#else
#define __acrt_InitializeCriticalSectionEx InitializeCriticalSectionEx
#endif

#ifdef _ATL_XP_TARGETING
BOOL WINAPI __acrt_IsValidLocaleName(
    _In_ LPCWSTR locale_name
    );
#else
#define __acrt_IsValidLocaleName IsValidLocaleName
#endif

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_LCMapStringEx(
    _In_opt_                            LPCWSTR          locale_name,
    _In_                                DWORD            flags,
    _In_CRT_NLS_string_(source_count)   LPCWSTR          source,
    _In_                                int              source_count,
    _Out_writes_opt_(destination_count) LPWSTR           destination,
    _In_                                int              destination_count,
    _In_opt_                            LPNLSVERSIONINFO version,
    _In_opt_                            LPVOID           reserved,
    _In_opt_                            LPARAM           sort_handle
    );
#else
#define __acrt_LCMapStringEx LCMapStringEx
#endif

#ifdef _ATL_XP_TARGETING
int WINAPI __acrt_LCIDToLocaleName(
    _In_                         LCID   locale,
    _Out_writes_opt_(name_count) LPWSTR name,
    _In_                         int    name_count,
    _In_                         DWORD  flags
    );
#else
#define __acrt_LCIDToLocaleName LCIDToLocaleName
#endif

#ifdef _ATL_XP_TARGETING
DWORD WINAPI __acrt_LocaleNameToLCID(
    _In_ LPCWSTR name,
    _In_ DWORD   flags
    );
#else
#define __acrt_LocaleNameToLCID LocaleNameToLCID
#endif

_Success_(return != NULL)
PVOID WINAPI __acrt_LocateXStateFeature(
    _In_      PCONTEXT content,
    _In_      DWORD    feature_id,
    _Out_opt_ PDWORD   length
    );

#define __acrt_MessageBoxA MessageBoxA

#define __acrt_MessageBoxW MessageBoxW

#define __acrt_OutputDebugStringA OutputDebugStringA

#define __acrt_OutputDebugStringW OutputDebugStringW

#ifdef __cplusplus
    HRESULT WINAPI __acrt_RoInitialize(
        _In_ RO_INIT_TYPE init_type
        );
#endif

void WINAPI __acrt_RoUninitialize(void);

_Success_(return != 0)
BOOLEAN WINAPI __acrt_RtlGenRandom(
    _Out_writes_bytes_(buffer_count) PVOID buffer,
    _In_                             ULONG buffer_count
    );

LONG WINAPI __acrt_AppPolicyGetProcessTerminationMethodInternal(_Out_ AppPolicyProcessTerminationMethod* policy);
LONG WINAPI __acrt_AppPolicyGetThreadInitializationTypeInternal(_Out_ AppPolicyThreadInitializationType* policy);
LONG WINAPI __acrt_AppPolicyGetShowDeveloperDiagnosticInternal(_Out_ AppPolicyShowDeveloperDiagnostic* policy);
LONG WINAPI __acrt_AppPolicyGetWindowingModelInternal(_Out_ AppPolicyWindowingModel* policy);


#if defined _ATL_XP_TARGETING && defined _X86_
BOOL WINAPI __acrt_SetThreadStackGuarantee(
    _Inout_ PULONG stack_size_in_bytes
    );
#else
#define __acrt_SetThreadStackGuarantee SetThreadStackGuarantee
#endif

bool __cdecl __acrt_can_show_message_box(void);
#ifdef _ATL_XP_TARGETING
bool __cdecl __acrt_can_use_vista_locale_apis(void);
#else
#define __acrt_can_use_vista_locale_apis() true
#endif
void __cdecl __acrt_eagerly_load_locale_apis(void);
bool __cdecl __acrt_can_use_xstate_apis(void);
HWND __cdecl __acrt_get_parent_window(void);
bool __cdecl __acrt_is_interactive(void);


#ifdef _ATL_XP_TARGETING
LCID __cdecl __acrt_DownlevelLocaleNameToLCID(
    _In_opt_ LPCWSTR localeName
    );
#endif

#ifdef _ATL_XP_TARGETING
_Success_(return != 0)
int __cdecl __acrt_DownlevelLCIDToLocaleName(
    _In_      LCID   lcid,
    _Out_writes_opt_z_(cchLocaleName) LPWSTR outLocaleName,
    _In_      int    cchLocaleName
    );
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Signal Handling and Exception Filter
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// Pointer to a signal handler
typedef void (__cdecl* __crt_signal_handler_t)(int);

// Exception-action table used by the C runtime to identify and dispose of
// exceptions corresponding to C runtime errors or C signals.
struct __crt_signal_action_t
{
    // Exception code or number; defined by the host OS.
    unsigned long _exception_number;

    // Signal code or number; defined by the CRT.
    int _signal_number;

    // Exception action code. Either a special code or the address of
    // a handler function. Always determines how the exception filter
    // should dispose of the exception.
    __crt_signal_handler_t _action;
};

extern struct __crt_signal_action_t const __acrt_exception_action_table[];

// Number of entries in the exception-action table
extern size_t const __acrt_signal_action_table_count;

// Size of exception-action table (in bytes)
extern size_t const __acrt_signal_action_table_size;

// Index of the first floating point exception entry
extern size_t const __acrt_signal_action_first_fpe_index;

// Number of FPE entries
extern size_t const __acrt_signal_action_fpe_count;

__crt_signal_handler_t __cdecl __acrt_get_sigabrt_handler(void);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// DesktopCRT Environment
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __cplusplus
    extern __crt_state_management::dual_state_global<char**>    _environ_table;
    extern __crt_state_management::dual_state_global<wchar_t**> _wenviron_table;
#endif

extern wchar_t** __dcrt_initial_wide_environment;
extern char**    __dcrt_initial_narrow_environment;

_Ret_opt_z_
char*    __cdecl __dcrt_get_narrow_environment_from_os(void);

_Ret_opt_z_
wchar_t* __cdecl __dcrt_get_wide_environment_from_os(void);

_Deref_ret_opt_z_
char**    __cdecl __dcrt_get_or_create_narrow_environment_nolock(void);

_Deref_ret_opt_z_
wchar_t** __cdecl __dcrt_get_or_create_wide_environment_nolock(void);

void __cdecl __dcrt_uninitialize_environments_nolock(void);


int __cdecl __dcrt_set_variable_in_narrow_environment_nolock(
    _In_ _Post_invalid_ char* option,
    _In_                int   is_top_level_call
    );

int __cdecl __dcrt_set_variable_in_wide_environment_nolock(
    _In_ _Post_invalid_ wchar_t* option,
    _In_                int      is_top_level_call
    );



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Invalid Parameter Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Invoke Watson if _ExpressionError is not 0; otherwise simply return
// _EspressionError.
_CRT_SECURITYCRITICAL_ATTRIBUTE
__forceinline void _invoke_watson_if_error(
    _In_       errno_t        expression_error,
    _In_opt_z_ wchar_t const* expression,
    _In_opt_z_ wchar_t const* function_name,
    _In_opt_z_ wchar_t const* file_name,
    _In_       unsigned int   line_number,
    _In_       uintptr_t      reserved
    )
{
    if (expression_error == 0)
    {
        return;
    }

    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

// Invoke Watson if _ExpressionError is not 0 and equal to _ErrorValue1 or
// _ErrorValue2; otherwise simply return _EspressionError.
__forceinline errno_t _invoke_watson_if_oneof(
    _In_       errno_t        expression_error,
    _In_       errno_t        error_value1,
    _In_       errno_t        error_value2,
    _In_opt_z_ wchar_t const* expression,
    _In_opt_z_ wchar_t const* function_name,
    _In_opt_z_ wchar_t const* file_name,
    _In_       unsigned int   line_number,
    _In_       uintptr_t      reserved
    )
{
    if (expression_error == 0 || (expression_error != error_value1 && expression_error != error_value2))
    {
        return expression_error;
    }

    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

#ifdef _DEBUG
    #define _INVOKE_WATSON_IF_ERROR(expr) _invoke_watson_if_error((expr), _CRT_WIDE(#expr), __FUNCTIONW__, __FILEW__, __LINE__, 0)
    #define _INVOKE_WATSON_IF_ONEOF(expr, errvalue1, errvalue2) _invoke_watson_if_oneof(expr, (errvalue1), (errvalue2), _CRT_WIDE(#expr), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else
    #define _INVOKE_WATSON_IF_ERROR(expr) _invoke_watson_if_error(expr, NULL, NULL, NULL, 0, 0)
    #define _INVOKE_WATSON_IF_ONEOF(expr, errvalue1, errvalue2) _invoke_watson_if_oneof((expr), (errvalue1), (errvalue2), NULL, NULL, NULL, 0, 0)
#endif

#define _ERRCHECK(e)               _INVOKE_WATSON_IF_ERROR(e)
#define _ERRCHECK_EINVAL(e)        _INVOKE_WATSON_IF_ONEOF(e, EINVAL, EINVAL)
#define _ERRCHECK_EINVAL_ERANGE(e) _INVOKE_WATSON_IF_ONEOF(e, EINVAL, ERANGE)

#define _ERRCHECK_SPRINTF(_PrintfCall)      \
    {                                       \
        errno_t _SaveErrno = errno;         \
        errno = 0;                          \
        if ((_PrintfCall) < 0)              \
        {                                   \
            _ERRCHECK_EINVAL_ERANGE(errno); \
        }                                   \
        errno = _SaveErrno;                 \
    }



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// SecureCRT
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// We completely fill the buffer only in debug (see _SECURECRT__FILL_STRING and
// _SECURECRT__FILL_BYTE macros).
#ifndef _SECURECRT_FILL_BUFFER
    #ifdef _DEBUG
        #define _SECURECRT_FILL_BUFFER 1
    #else
        #define _SECURECRT_FILL_BUFFER 0
    #endif
#endif

#ifndef _SECURECRT_FILL_BUFFER_THRESHOLD
// _SECURECRT_FILL_BUFFER_THRESHOLD must be a constant for Prefast due to the
// double evaluation. Making it something unique like 42 would pollute Prefast's warnings.
    #if defined _PREFAST_ || !defined _DEBUG
        #define _SECURECRT_FILL_BUFFER_THRESHOLD ((size_t)0)
    #else
        #define _SECURECRT_FILL_BUFFER_THRESHOLD (_CrtGetDebugFillThreshold())
    #endif
#endif

// TODO make this a template so that _CrtGetDebugFillThreshold is not called multiple times.
#if _SECURECRT_FILL_BUFFER
    #define _SECURECRT__FILL_STRING(_String, _Count, _Offset)                       \
        if ((_Count) != ((size_t)-1) &&                                             \
            (_Count) != INT_MAX &&                                                  \
            ((size_t)(_Offset)) < (_Count))                                         \
        {                                                                           \
            size_t max_buffer_count = _SECURECRT_FILL_BUFFER_THRESHOLD;             \
            memset(                                                                 \
                (_String) + (_Offset),                                              \
                _SECURECRT_FILL_BUFFER_PATTERN,                                     \
                (max_buffer_count < ((size_t)((_Count) - (_Offset)))                \
                    ? max_buffer_count                                              \
                    : ((_Count) - (_Offset))) * sizeof(*(_String)));                \
        }
#else
    #define _SECURECRT__FILL_STRING(_String, _Count, _Offset)
#endif

#if _SECURECRT_FILL_BUFFER
    #define _SECURECRT__FILL_BYTE(_Position)                \
        if (_SECURECRT_FILL_BUFFER_THRESHOLD > 0)           \
        {                                                   \
            (_Position) = _SECURECRT_FILL_BUFFER_PATTERN;   \
        }
#else
    #define _SECURECRT__FILL_BYTE(_Position)
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Precondition Validation Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// We use _VALIDATE_STREAM_ANSI_RETURN to ensure that ANSI file operations (e.g.
// fprintf) aren't called on files opened as UNICODE. We do this check only if
// it's an actual FILE pointer & not a string.
#if 0
//msvcrt.dll for xp 本身就不支持这些功能
#define _VALIDATE_STREAM_ANSI_RETURN(stream, errorcode, retexpr)               \
    {                                                                          \
        __crt_stdio_stream const _Stream(stream);                              \
        int fn;                                                                \
        _VALIDATE_RETURN((                                                     \
            (_Stream.is_string_backed()) ||                                    \
            (fn = _fileno(_Stream.public_stream()),                            \
                ((_textmode_safe(fn) == __crt_lowio_text_mode::ansi) &&        \
                !_tm_unicode_safe(fn)))),                                      \
            errorcode, retexpr)                                                \
    }
#else
#define _VALIDATE_STREAM_ANSI_RETURN(stream, errorcode, retexpr)               \
    {                                                                          \
         _VALIDATE_RETURN(stream != NULL,errorcode,retexpr)                    \
    }
#endif

#define _CHECK_FH_RETURN( handle, errorcode, retexpr )                         \
    {                                                                          \
        if(handle == _NO_CONSOLE_FILENO)                                       \
        {                                                                      \
            errno = errorcode;                                                 \
            return ( retexpr );                                                \
        }                                                                      \
    }

#define _CHECK_FH_CLEAR_OSSERR_RETURN( handle, errorcode, retexpr )            \
    {                                                                          \
        if(handle == _NO_CONSOLE_FILENO)                                       \
        {                                                                      \
            _doserrno = 0L;                                                    \
            errno = errorcode;                                                 \
            return ( retexpr );                                                \
        }                                                                      \
    }

#define _CHECK_FH_CLEAR_OSSERR_RETURN_ERRCODE( handle, retexpr )               \
    {                                                                          \
        if(handle == _NO_CONSOLE_FILENO)                                       \
        {                                                                      \
            _doserrno = 0L;                                                    \
            return ( retexpr );                                                \
        }                                                                      \
    }



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Redirect-to-Locale Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __cplusplus
    #define _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE extern "C"
#else
    #define _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE
#endif

#define _REDIRECT_TO_L_VERSION_1(_ReturnType, _FunctionName, _Type1)                          \
    _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE                                                      \
    _ReturnType __cdecl _FunctionName(_Type1 _Arg1)                                           \
    {                                                                                         \
        return _FunctionName##_l(_Arg1, NULL);                                                \
    }

#define _REDIRECT_TO_L_VERSION_2(_ReturnType, _FunctionName, _Type1, _Type2)                  \
    _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE                                                      \
    _ReturnType __cdecl _FunctionName(_Type1 _Arg1, _Type2 _Arg2)                             \
    {                                                                                         \
        return _FunctionName##_l(_Arg1, _Arg2, NULL);                                         \
    }

#define _REDIRECT_TO_L_VERSION_3(_ReturnType, _FunctionName, _Type1, _Type2, _Type3)          \
    _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE                                                      \
    _ReturnType __cdecl _FunctionName(_Type1 _Arg1, _Type2 _Arg2, _Type3 _Arg3)               \
    {                                                                                         \
        return _FunctionName##_l(_Arg1, _Arg2, _Arg3, NULL);                                  \
    }

#define _REDIRECT_TO_L_VERSION_3_downlevel(_ReturnType, _FunctionName, _Type1, _Type2, _Type3)\
    _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE                                                      \
    _ReturnType __cdecl _FunctionName##_downlevel(_Type1 _Arg1, _Type2 _Arg2, _Type3 _Arg3)   \
    {                                                                                         \
        return _FunctionName##_l(_Arg1, _Arg2, _Arg3, NULL);                        \
    }


#define _REDIRECT_TO_L_VERSION_4(_ReturnType, _FunctionName, _Type1, _Type2, _Type3, _Type4)  \
    _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE                                                      \
    _ReturnType __cdecl _FunctionName(_Type1 _Arg1, _Type2 _Arg2, _Type3 _Arg3, _Type4 _Arg4) \
    {                                                                                         \
        return _FunctionName##_l(_Arg1, _Arg2, _Arg3, _Arg4, NULL);                           \
    }

#define _REDIRECT_TO_L_VERSION_4_downlevel(_ReturnType, _FunctionName, _Type1, _Type2, _Type3, _Type4)    \
    _REDIRECT_TO_L_VERSION_FUNC_PROLOGUE                                                                  \
    _ReturnType __cdecl _FunctionName##_downlevel(_Type1 _Arg1, _Type2 _Arg2, _Type3 _Arg3, _Type4 _Arg4) \
    {                                                                                                     \
        return _FunctionName##_l(_Arg1, _Arg2, _Arg3, _Arg4, NULL);                                       \
    }


#ifdef __cplusplus
extern "C++"
{
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //
    // Locale Update
    //
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    extern "C" void __acrt_update_locale_info(
        _In_     __acrt_ptd*        const ptd,
        _Inout_ __crt_locale_data** const data
        );

    extern "C" void __acrt_update_multibyte_info(
        _In_ __acrt_ptd*               const ptd,
        _Inout_ __crt_multibyte_data** const data
        );

    /*class _LocaleUpdate
    {
    public:

        _LocaleUpdate(_locale_t const locale) throw()
            : _updated(false)
        {
            if (locale)
            {
                _locale_pointers = *locale;
            }
            else if (!__acrt_locale_changed())
            {
                _locale_pointers = __acrt_initial_locale_pointers;
            }
            else
            {
                _ptd = __acrt_getptd();
                _locale_pointers.locinfo = _ptd->_locale_info;
                _locale_pointers.mbcinfo = _ptd->_multibyte_info;

                __acrt_update_locale_info   (_ptd, &_locale_pointers.locinfo);
                __acrt_update_multibyte_info(_ptd, &_locale_pointers.mbcinfo);
                if ((_ptd->_own_locale & _PER_THREAD_LOCALE_BIT) == 0)
                {
                    _ptd->_own_locale |= _PER_THREAD_LOCALE_BIT;
                    _updated = true;
                }
            }
        }

        ~_LocaleUpdate() throw()
        {
            if (_updated)
            {
                _ptd->_own_locale = _ptd->_own_locale & ~_PER_THREAD_LOCALE_BIT;
            }
        }

        _locale_t GetLocaleT() throw()
        {
            return &_locale_pointers;
        }

    private:

        __acrt_ptd*           _ptd;
        __crt_locale_pointers _locale_pointers;
        bool                  _updated;

    };*/



    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //
    // errno Cache
    //
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // Some operations in this file may need to set or query errno many times.  Each
    // use of errno requires us to get the PTD from FLS.  To avoid having to do this
    // many times, we cache the pointer to errno.  This class encapsulates that
    // caching and also defers the initial acquisition of &errno until the first
    // time that errno is actually needed (when we aren't calling functions that may
    // set errno, there's no need to acquire errno at all).
    class __crt_deferred_errno_cache
    {
    public:

        __crt_deferred_errno_cache() throw()
            : _cached_errno{}
        {
        }

        errno_t& get() throw()
        {
            if (!_cached_errno)
            {
                _cached_errno = &errno;
            }

            return *_cached_errno;
        }

    private:

        errno_t* _cached_errno;
    };



    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //
    // errno and LastError Reset
    //
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    #ifndef _M_CEE
    struct __crt_errno_guard
    {
    public:

        __crt_errno_guard(errno_t* const errno_address = &errno) throw()
            : _errno_address(errno_address), _stored_errno(*errno_address)
        {
            *errno_address = 0;
        }

        ~__crt_errno_guard() throw()
        {
            if (*_errno_address == 0 && _stored_errno != 0)
                *_errno_address = _stored_errno;
        }

    private:

        __crt_errno_guard(__crt_errno_guard const&) throw(); // not implemented
        void operator=(__crt_errno_guard const&) throw();    // not implemented

        errno_t* _errno_address;
        errno_t  _stored_errno;
    };

    // Resets the operating system last error (GetLastError) to its original
    // value on scope exit.
    class __crt_scoped_get_last_error_reset
    {
    public:

        __crt_scoped_get_last_error_reset() throw()
        {
            _old_last_error = GetLastError();
        }

        ~__crt_scoped_get_last_error_reset() throw()
        {
            SetLastError(_old_last_error);
        }

    private:
        __crt_scoped_get_last_error_reset(__crt_scoped_get_last_error_reset const&);
        __crt_scoped_get_last_error_reset& operator=(__crt_scoped_get_last_error_reset const&);

        DWORD _old_last_error;
    };
    #endif // _M_CEE
} // extern "C++"
#endif // __cplusplus


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Windows policy APIs
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// Specifies method used to end a process
typedef enum
{
    process_end_policy_terminate_process,
    process_end_policy_exit_process
} process_end_policy;

process_end_policy __cdecl __acrt_get_process_end_policy(void);

// Specifies whether RoInitialize() should be called when creating a thread
typedef enum
{
    begin_thread_init_policy_unknown,
    begin_thread_init_policy_none,
    begin_thread_init_policy_ro_initialize
} begin_thread_init_policy;

begin_thread_init_policy __cdecl __acrt_get_begin_thread_init_policy(void);

// Specifies whether the Assert dialog should be shown
typedef enum
{
    developer_information_policy_unknown,
    developer_information_policy_none,
    developer_information_policy_ui
} developer_information_policy;

developer_information_policy __cdecl __acrt_get_developer_information_policy(void);

// Specifies what type of Windowing support is available
typedef enum
{
    windowing_model_policy_unknown,
    windowing_model_policy_hwnd,
    windowing_model_policy_corewindow,
    windowing_model_policy_legacyphone,
    windowing_model_policy_none
} windowing_model_policy;

windowing_model_policy __cdecl __acrt_get_windowing_model_policy(void);

_CRT_END_C_HEADER

//#include <corecrt_internal_state_isolation.h>
