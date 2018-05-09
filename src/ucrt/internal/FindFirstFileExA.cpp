//
// FindFirstFileExA.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definition of __acrt_FindFirstFileExA.
//

#include <corecrt_internal_win32_buffer.h>

HANDLE __cdecl __acrt_FindFirstFileExA(
    LPCSTR const             lpFileName,
    FINDEX_INFO_LEVELS const fInfoLevelId,
    LPVOID const             lpFindFileData,
    FINDEX_SEARCH_OPS const  fSearchOp,
    LPVOID const             lpSearchFilter,
    DWORD const              dwAdditionalFlags
    )
{
    __crt_internal_win32_buffer<wchar_t> wide_file_name;

    errno_t const cvt1 = __acrt_mbs_to_wcs(
        lpFileName,
        wide_file_name
        );

    if (cvt1 != 0) {
        return INVALID_HANDLE_VALUE;
    }

    return ::FindFirstFileExW(
        wide_file_name.data(),
        fInfoLevelId,
        lpFindFileData,
        fSearchOp,
        lpSearchFilter,
        dwAdditionalFlags
        );
}
