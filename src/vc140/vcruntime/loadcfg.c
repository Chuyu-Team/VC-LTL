/***
*loadcfg.c
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines the default load config struct linked into images.
*
*******************************************************************************/

#include <windows.h>

extern UINT_PTR __security_cookie;  /* /GS security cookie */
extern PVOID __guard_check_icall_fptr;
#if defined(_AMD64_)
extern PVOID __guard_dispatch_icall_fptr;
#endif

#if defined(_M_IX86) || defined(_X86_)
/*
 * The following two names are automatically created by the linker for any
 * image that has the safe exception table present.
 */

extern PVOID __safe_se_handler_table[]; /* base of safe handler entry table */
extern BYTE  __safe_se_handler_count;   /* absolute symbol whose address is
                                           the count of table entries */
#endif
extern PVOID __guard_fids_table[];
extern ULONG __guard_fids_count;
extern ULONG __guard_flags;

const
DECLSPEC_SELECTANY
IMAGE_LOAD_CONFIG_DIRECTORY
_load_config_used = {
    sizeof(_load_config_used),
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (SIZE_T)&__security_cookie,

#if defined(_M_IX86) || defined(_X86_)

    (SIZE_T)__safe_se_handler_table,
    (SIZE_T)&__safe_se_handler_count,

#else

    0,
    0,

#endif

    (SIZE_T)&__guard_check_icall_fptr,

#if defined(_AMD64_)

    (SIZE_T)&__guard_dispatch_icall_fptr,

#else

    0,

#endif

    (SIZE_T)&__guard_fids_table,
    (SIZE_T)&__guard_fids_count,
    (ULONG)(SIZE_T)&__guard_flags
};
