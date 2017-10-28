//
// corecrt_internal_traits.h
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This internal header defines template-based utilities for implementing sets
// of functions that are largely similar but which operate on different kinds of
// strings (narrow or wide) or different kinds of integers (32-bit or 64-bit).
// It is similar in some respects to the macro-based <tchar.h>.
//
#pragma once

#include <corecrt_internal.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push, _CRT_PACKING)



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Character Traits
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template <typename Character>
struct __crt_char_traits;

#define _CORECRT_APPLY_TO_MAPPINGS(_APPLY)                                                                                                       \
    _APPLY(capture_argv,                       __acrt_capture_narrow_argv,                       __acrt_capture_wide_argv                      ) \
    _APPLY(create_process,                     CreateProcessA,                                   CreateProcessW                                ) \
    _APPLY(find_first_file_ex,                 FindFirstFileExA,                                 FindFirstFileExW                              ) \
    _APPLY(find_next_file,                     FindNextFileA,                                    FindNextFileW                                 ) \
    _APPLY(free_environment_strings,           FreeEnvironmentStringsA,                          FreeEnvironmentStringsW                       ) \
    _APPLY(ftprintf,                           fprintf,                                          fwprintf                                      ) \
    _APPLY(get_current_directory,              GetCurrentDirectoryA,                             GetCurrentDirectoryW                          ) \
    _APPLY(get_environment_from_os,            __dcrt_get_narrow_environment_from_os,            __dcrt_get_wide_environment_from_os           ) \
    _APPLY(get_module_file_name,               GetModuleFileNameA,                               GetModuleFileNameW                            ) \
    _APPLY(get_or_create_environment_nolock,   __dcrt_get_or_create_narrow_environment_nolock,   __dcrt_get_or_create_wide_environment_nolock  ) \
    _APPLY(get_temp_path,                      __acrt_GetTempPathA,                              GetTempPathW                                  ) \
    _APPLY(getc_nolock,                        _getc_nolock,                                     _getwc_nolock                                 ) \
    _APPLY(gettche_nolock,                     _getche_nolock,                                   _getwche_nolock                               ) \
    _APPLY(initialize_environment_nolock,      _initialize_narrow_environment,                   _initialize_wide_environment                  ) \
    _APPLY(istspace,                           isspace,                                          iswspace                                      ) \
    _APPLY(itot_s,                             _itoa_s,                                          _itow_s                                       ) \
    _APPLY(message_box,                        __acrt_MessageBoxA,                               __acrt_MessageBoxW                            ) \
    _APPLY(open_file,                          _openfile,                                        _wopenfile                                    ) \
    _APPLY(output_debug_string,                OutputDebugStringA,                               OutputDebugStringW                            ) \
    _APPLY(pack_command_line_and_environment,  __acrt_pack_narrow_command_line_and_environment,  __acrt_pack_wide_command_line_and_environment ) \
    _APPLY(puttc_nolock,                       _fputc_nolock,                                    _fputwc_nolock                                ) \
    _APPLY(puttch_nolock,                      _putch_nolock,                                    _putwch_nolock                                ) \
    _APPLY(set_current_directory,              SetCurrentDirectoryA,                             SetCurrentDirectoryW                          ) \
    _APPLY(set_environment_variable,           SetEnvironmentVariableA,                          SetEnvironmentVariableW                       ) \
    _APPLY(set_program_name,                   _set_pgmptr,                                      _set_wpgmptr                                  ) \
    _APPLY(set_variable_in_environment_nolock, __dcrt_set_variable_in_narrow_environment_nolock, __dcrt_set_variable_in_wide_environment_nolock) \
    _APPLY(show_message_box,                   __acrt_show_narrow_message_box,                   __acrt_show_wide_message_box                  ) \
    _APPLY(sntprintf_s,                        _snprintf_s,                                      _snwprintf_s                                  ) \
    _APPLY(taccess_s,                          _access_s,                                        _waccess_s                                    ) \
    _APPLY(tasctime,                           asctime,                                          _wasctime                                     ) \
    _APPLY(tasctime_s,                         asctime_s,                                        _wasctime_s                                   ) \
    _APPLY(tcscat_s,                           strcat_s,                                         wcscat_s                                      ) \
    _APPLY(tcschr,                             strchr,                                           wcschr                                        ) \
    _APPLY(tcscmp,                             strcmp,                                           wcscmp                                        ) \
    _APPLY(tcscpy_s,                           strcpy_s,                                         wcscpy_s                                      ) \
    _APPLY(tcserror_s,                         strerror_s,                                       _wcserror_s                                   ) \
    _APPLY(tcsicmp,                            _stricmp,                                         _wcsicmp                                      ) \
    _APPLY(tcslen,                             strlen,                                           wcslen                                        ) \
    _APPLY(tcsncat_s,                          strncat_s,                                        wcsncat_s                                     ) \
    _APPLY(tcsncmp,                            strncmp,                                          wcsncmp                                       ) \
    _APPLY(tcsncpy_s,                          strncpy_s,                                        wcsncpy_s                                     ) \
    _APPLY(tcsnicmp,                           _strnicmp,                                        _wcsnicmp                                     ) \
    _APPLY(tcsnicoll,                          _strnicoll,                                       _wcsnicoll                                    ) \
    _APPLY(tcsnlen,                            strnlen,                                          wcsnlen                                       ) \
    _APPLY(tcspbrk,                            strpbrk,                                          wcspbrk                                       ) \
    _APPLY(tcsrchr,                            strrchr,                                          wcsrchr                                       ) \
    _APPLY(tcstol,                             strtol,                                           wcstol                                        ) \
    _APPLY(tcstoul,                            strtoul,                                          wcstoul                                       ) \
    _APPLY(tcstoull,                           strtoull,                                         wcstoull                                      ) \
    _APPLY(tdupenv_s_crt,                      _dupenv_s_crt,                                    _wdupenv_s_crt                                ) \
    _APPLY(texecve,                            _execve,                                          _wexecve                                      ) \
    _APPLY(tfdopen,                            _fdopen,                                          _wfdopen                                      ) \
    _APPLY(tfullpath,                          _fullpath,                                        _wfullpath                                    ) \
    _APPLY(tgetcwd,                            _getcwd,                                          _wgetcwd                                      ) \
    _APPLY(tgetpath,                           __acrt_getpath,                                   __acrt_wgetpath                               ) \
    _APPLY(tmktemp_s,                          _mktemp_s,                                        _wmktemp_s                                    ) \
    _APPLY(tsopen_nolock,                      _sopen_nolock,                                    _wsopen_nolock                                ) \
    _APPLY(tsopen_s,                           _sopen_s,                                         _wsopen_s                                     ) \
    _APPLY(tspawnve,                           _spawnve,                                         _wspawnve                                     ) \
    _APPLY(tspawnvpe,                          _spawnvpe,                                        _wspawnvpe                                    ) \
    _APPLY(ulltot_s,                           _ui64toa_s,                                       _ui64tow_s                                    ) \
    _APPLY(ultot_s,                            _ultoa_s,                                         _ultow_s                                      ) \
    _APPLY(ungettc_nolock,                     _ungetc_nolock,                                   _ungetwc_nolock                               ) \
    _APPLY(ungettch_nolock,                    _ungetch_nolock,                                  _ungetwch_nolock                              )

template <>
struct __crt_char_traits<char>
{
    typedef char           char_type;
    typedef unsigned char  unsigned_char_type;
    typedef wchar_t        other_char_type;
    typedef int            int_type;
    typedef STARTUPINFOA   startup_info_type;
    typedef WIN32_FIND_DATAA win32_find_data_type;

    #define _APPLY(name, narrow_name, wide_name) _CORECRT_GENERATE_FORWARDER(static, __cdecl, name, narrow_name)
    _CORECRT_APPLY_TO_MAPPINGS(_APPLY)
    #undef _APPLY
};

template <>
struct __crt_char_traits<wchar_t>
{
    typedef wchar_t        char_type;
    typedef wchar_t        unsigned_char_type;
    typedef char           other_char_type;
    typedef wint_t         int_type;
    typedef STARTUPINFOW   startup_info_type;
    typedef WIN32_FIND_DATAW win32_find_data_type;

    #define _APPLY(name, narrow_name, wide_name) _CORECRT_GENERATE_FORWARDER(static, __cdecl, name, wide_name)
    _CORECRT_APPLY_TO_MAPPINGS(_APPLY)
    #undef _APPLY
};

#undef _CORECRT_APPLY_TO_MAPPINGS



#define _GENERATE_TCHAR_STRING_FUNCTIONS(name, string)                                                        \
    static char    const* __cdecl _CRT_CONCATENATE(get_, name)(char)    throw() { return string; }            \
    static wchar_t const* __cdecl _CRT_CONCATENATE(get_, name)(wchar_t) throw() { return _CRT_WIDE(string); } \
    static size_t const _CRT_CONCATENATE(name, _length) = _countof(string) - 1;                               \
    static size_t const _CRT_CONCATENATE(name, _count ) = _countof(string);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Integer Traits
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
template <typename Integer>
struct __crt_integer_traits;

#define _CORECRT_APPLY_TO_MAPPINGS(_APPLY)                  \
    _APPLY(ftell_nolock, _ftell_nolock,   _ftelli64_nolock) \
    _APPLY(lseek,        _lseek,          _lseeki64       ) \
    _APPLY(lseek_nolock, _lseek_nolock,   _lseeki64_nolock) \
    _APPLY(futime,       _futime32,       _futime64       ) \
    _APPLY(gmtime_s,     _gmtime32_s,     _gmtime64_s     ) \
    _APPLY(localtime_s,  _localtime32_s,  _localtime64_s  ) \
    _APPLY(loctotime,    __loctotime32_t, __loctotime64_t ) \
    _APPLY(time,         _time32,         _time64         )

template <>
struct __crt_integer_traits<long>
{
    #define _APPLY(name, name_32, name_64) _CORECRT_GENERATE_FORWARDER(static, __cdecl, name, name_32)
    _CORECRT_APPLY_TO_MAPPINGS(_APPLY)
    #undef _APPLY
};

template <>
struct __crt_integer_traits<long long>
{
    #define _APPLY(name, name_32, name_64) _CORECRT_GENERATE_FORWARDER(static, __cdecl, name, name_64)
    _CORECRT_APPLY_TO_MAPPINGS(_APPLY)
    #undef _APPLY
};

#undef _CORECRT_APPLY_TO_MAPPINGS



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// char <=> wchar_t conversion utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// These functions wrap MultiByteToWideChar and WideCharToMultiByte in overloads
// with the same name, to allow them to be called from within function templates
// that may be instantiated to operate on either narrow or wide strings.
inline size_t __crt_compute_required_transform_buffer_count(
            unsigned    const code_page,
    _In_z_  char const* const string
    )
{
    return static_cast<size_t>(MultiByteToWideChar(code_page, 0, string, -1, nullptr, 0));
}

inline size_t __crt_compute_required_transform_buffer_count(
            unsigned        const code_page,
    _In_z_  wchar_t const*  const string
    )
{
    return static_cast<size_t>(WideCharToMultiByte(code_page, 0, string, -1, nullptr, 0, nullptr, nullptr));
}

_Success_(return > 0 && return <= buffer_count)
inline size_t __crt_transform_string(
                                    unsigned    const   code_page,
    _In_z_                          char const* const   string,
    _Out_writes_z_(buffer_count)    wchar_t*    const   buffer,
    size_t      const buffer_count
    )
{
    int const int_count = static_cast<int>(buffer_count);
    return static_cast<size_t>(MultiByteToWideChar(code_page, 0, string, -1, buffer, int_count));
}

_Success_(return > 0 && return <= buffer_count)
inline size_t __crt_transform_string(
                                    unsigned        const   code_page,
    _In_z_                          wchar_t const*  const   string,
    _Out_writes_z_(buffer_count)    char*           const   buffer,
    size_t                                          const   buffer_count
    )
{
    int const int_count = static_cast<int>(buffer_count);
    return static_cast<size_t>(WideCharToMultiByte(code_page, 0, string, -1, buffer, int_count, nullptr, nullptr));
}



#pragma pack(pop)
