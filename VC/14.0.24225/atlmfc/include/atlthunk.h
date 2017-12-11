// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MANAGED
struct AtlThunkData_t { };
#else
struct AtlThunkData_t; // opaque
typedef struct AtlThunkData_t AtlThunkData_t;
#endif

AtlThunkData_t*
__stdcall
AtlThunk_AllocateData(
    void
    );

void
__stdcall
AtlThunk_InitData(
    AtlThunkData_t* Thunk,
    void* /*WNDPROC*/ Proc,
    size_t FirstParameter
    );

WNDPROC
__stdcall
AtlThunk_DataToCode(
    AtlThunkData_t*
    );

void
__stdcall
AtlThunk_FreeData(
    AtlThunkData_t* Thunk
    );

void
__stdcall
AtlThunk_CleanupAll(
    void
    );

#ifdef __cplusplus
} /* extern "C" */
#endif
