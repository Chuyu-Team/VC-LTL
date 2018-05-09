//
// FindNextFileA.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definition of __acrt_FindNextFileA.
//

#include <corecrt_internal_win32_buffer.h>

BOOL __cdecl __acrt_FindNextFileA(
  HANDLE const              hFindFile,
  _WIN32_FIND_DATAA * const lpFindFileData
    )
{
    _WIN32_FIND_DATAW wide_find_file_data;

    BOOL const ret_val = ::FindNextFileW(hFindFile, &wide_find_file_data);

    if (ret_val == FALSE) {
        return ret_val;
    }

    lpFindFileData->dwFileAttributes = wide_find_file_data.dwFileAttributes;
    lpFindFileData->ftCreationTime   = wide_find_file_data.ftCreationTime;
    lpFindFileData->ftLastAccessTime = wide_find_file_data.ftLastAccessTime;
    lpFindFileData->ftLastWriteTime  = wide_find_file_data.ftLastWriteTime;
    lpFindFileData->nFileSizeHigh    = wide_find_file_data.nFileSizeHigh;
    lpFindFileData->nFileSizeLow     = wide_find_file_data.nFileSizeLow;
    lpFindFileData->dwReserved0      = wide_find_file_data.dwReserved0;
    lpFindFileData->dwReserved1      = wide_find_file_data.dwReserved1;

    __crt_no_alloc_win32_buffer<char> wide_file_name(lpFindFileData->cFileName);

    errno_t const cvt1 = __acrt_wcs_to_mbs(
        wide_find_file_data.cFileName,
        wide_file_name
        );

    if (cvt1 != 0) {
        return FALSE;
    }

    __crt_no_alloc_win32_buffer<char> wide_alternate_file_name(lpFindFileData->cAlternateFileName);

    errno_t const cvt2 = __acrt_wcs_to_mbs(
        wide_find_file_data.cAlternateFileName,
        wide_alternate_file_name
        );

    if (cvt2 != 0) {
        return FALSE;
    }

    return TRUE;
}
