// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// align.h
//
// Alignment / Packing definitions
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#define WORD_ALIGN 1
#define DWORD_ALIGN 3
#define QWORD_ALIGN 7
#define DQWORD_ALIGN 15

#ifdef _WIN64
#define P2_ALIGN 15
#else // !_WIN64
#define P2_ALIGN 7
#endif // _WIN64

#define ALIGNED_SIZE(size, alignment) (((size) + (alignment)) & ~(alignment))

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(x) ((sizeof(x))/(sizeof(x[0])))
#endif // SIZEOF_ARRAY
