/***
* zmminitrin.h - Meta Header file for Intel(R) Architecture intrinsic functions.
*
* Copyright (C) 2007-2016 Intel Corporation.  All rights reserved.
*
* The information and source code contained herein is the exclusive
* property of Intel Corporation and may not be disclosed, examined
* or reproduced in whole or in part without explicit written authorization
* from the company.
*
*
*******************************************************************************/

#ifndef _ZMMINTRIN_H_INCLUDED
#define _ZMMINTRIN_H_INCLUDED

#ifndef _INCLUDED_IMM
#error "Header should only be included from <immintrin.h>."
#endif

//
// Definitions and declarations for use with 512-bit compiler intrinsics.
//

//
// A word about intrinsic function naming conventions.  Most 512-bit
// vector instructions have names such as v<operation><type>.  For
// example "vaddps" is an addition operation (add) on packed single-
// precision (ps) values.  The corresponding intrinsic is usually
// (not always) named _mm512_<operation>_<type>. For example, the
// "_mm512_add_ps" function generates VADDPS.  The corresponding
// masked flavor adds "_mask" to the name, e.g. "_mm512_mask_add_ps".
//
// The types include:
//
//    ps    -- packed single precision
//    pd    -- packed double precision
//    epi32 -- packed 32-bit integers
//    epu32 -- packed 32-bit unsigned integers
//    epi64 -- packed 64-bit integers
//

typedef unsigned char       __mmask8;
typedef unsigned short      __mmask16;
typedef unsigned int        __mmask32;
typedef unsigned __int64    __mmask64;

typedef union __declspec(intrin_type) /* __declspec(align(64)) */ __m512 {
    float m512_f32[16];
} __m512;

typedef struct __declspec(intrin_type) /* __declspec(align(64)) */ __m512d {
    double m512d_f64[8];
} __m512d;

typedef union  __declspec(intrin_type) /* __declspec(align(64)) */ __m512i {
    __int8              m512i_i8[64];
    __int16             m512i_i16[32];
    __int32             m512i_i32[16];
    __int64             m512i_i64[8];
    unsigned __int8     m512i_u8[64];
    unsigned __int16    m512i_u16[32];
    unsigned __int32    m512i_u32[16];
    unsigned __int64    m512i_u64[8];
} __m512i;


#ifdef __cplusplus
extern "C" {
// Intrinsic functions use C name-mangling.
#endif /* __cplusplus */

/* Conversion from one type to another, no change in value. */
extern __m256  __cdecl _mm512_castps512_ps256(__m512);
extern __m512  __cdecl _mm512_castpd_ps(__m512d);
extern __m512  __cdecl _mm512_castps256_ps512(__m256);
extern __m512  __cdecl _mm512_castsi512_ps(__m512i);
extern __m512  __cdecl _mm512_castps128_ps512(__m128);

extern __m256d __cdecl _mm512_castpd512_pd256(__m512d);
extern __m512d __cdecl _mm512_castpd256_pd512(__m256d);
extern __m512d __cdecl _mm512_castps_pd(__m512);
extern __m512d __cdecl _mm512_castsi512_pd(__m512i);
extern __m512d __cdecl _mm512_castpd128_pd512(__m128d);

extern __m256i __cdecl _mm512_castsi512_si256(__m512i);
extern __m512i __cdecl _mm512_castpd_si512(__m512d);
extern __m512i __cdecl _mm512_castps_si512(__m512);
extern __m512i __cdecl _mm512_castsi256_si512(__m256i);

/* Constant for special read-only mask register 'k0'. */
#define _MM_K0_REG (0xffff)

/*
 * Constants for rounding mode.
 * These names beginnig with "_MM_ROUND" are deprecated.
 * Use the names beginning with "_MM_FROUND" going forward.
 */
typedef enum {
    _MM_ROUND_MODE_NEAREST,             /* round to nearest (even) */
    _MM_ROUND_MODE_DOWN,                /* round toward negative infinity */
    _MM_ROUND_MODE_UP,                  /* round toward positive infinity */
    _MM_ROUND_MODE_TOWARD_ZERO,         /* round toward zero */
    _MM_ROUND_MODE_DEFAULT              /* round mode from MXCSR */
} _MM_ROUND_MODE_ENUM;


/*
* Intel(R) AVX-512 intrinsic functions
*/
extern __m512  __cdecl _mm512_setzero_ps(void);
extern __m512d __cdecl _mm512_setzero_pd(void);

extern __m512  __cdecl _mm512_set_ps(float /* e15 */, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float /* e0 */);
extern __m512d __cdecl _mm512_set_pd(double /* e7 */, double, double, double, double, double, double, double /* e0 */);

extern __m512  __cdecl _mm512_set1_ps(float);
extern __m512d __cdecl _mm512_set1_pd(double);

extern __m512  __cdecl _mm512_maskz_load_ps(__mmask16, void const*);
extern __m512d __cdecl _mm512_maskz_load_pd(__mmask8, void const*);
extern __m512  __cdecl _mm512_mask_load_ps(__m512, __mmask16, void const*);
extern __m512d __cdecl _mm512_mask_load_pd(__m512d, __mmask8, void const*);
extern __m512  __cdecl _mm512_maskz_loadu_ps(__mmask16, void const*);
extern __m512d __cdecl _mm512_maskz_loadu_pd(__mmask8, void const*);
extern __m512  __cdecl _mm512_mask_loadu_ps(__m512, __mmask16, void const*);
extern __m512d __cdecl _mm512_mask_loadu_pd(__m512d, __mmask8, void const*);

extern void    __cdecl _mm512_storeu_ps(void*, __m512);
extern void    __cdecl _mm512_storeu_pd(void*, __m512d);
extern void    __cdecl _mm512_mask_store_ps(void*, __mmask16, __m512);
extern void    __cdecl _mm512_mask_store_pd(void*, __mmask8, __m512d);
extern void    __cdecl _mm512_mask_storeu_ps(void*, __mmask16, __m512);
extern void    __cdecl _mm512_mask_storeu_pd(void*, __mmask8, __m512d);

extern __m512  __cdecl _mm512_maskz_add_round_ps(__mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_add_round_pd(__mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_add_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_add_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_maskz_sub_round_ps(__mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_sub_round_pd(__mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_sub_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_sub_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_maskz_mul_round_ps(__mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_mul_round_pd(__mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_mul_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_mul_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);

extern __m512d __cdecl _mm512_maskz_div_round_pd(__mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_div_round_ps(__mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_div_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_div_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_mask_fmadd_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask3_fmadd_round_ps(__m512, __m512, __m512, __mmask16, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_fmadd_round_ps(__mmask16, __m512, __m512, __m512, const int);
extern __m512d __cdecl _mm512_mask_fmadd_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask3_fmadd_round_pd(__m512d, __m512d, __m512d, __mmask8, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_fmadd_round_pd(__mmask8, __m512d, __m512d, __m512d, const int);

extern __m512  __cdecl _mm512_mask_fmsub_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask3_fmsub_round_ps(__m512, __m512, __m512, __mmask16, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_fmsub_round_ps(__mmask16, __m512, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_fmsub_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask3_fmsub_round_pd(__m512d, __m512d, __m512d, __mmask8, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_fmsub_round_pd(__mmask8, __m512d, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_mask_fmaddsub_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask3_fmaddsub_round_ps(__m512, __m512, __m512, __mmask16, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_fmaddsub_round_ps(__mmask16, __m512, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_fmaddsub_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask3_fmaddsub_round_pd(__m512d, __m512d, __m512d, __mmask8, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_fmaddsub_round_pd(__mmask8, __m512d, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_mask_fmsubadd_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask3_fmsubadd_round_ps(__m512, __m512, __m512, __mmask16, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_fmsubadd_round_ps(__mmask16, __m512, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_fmsubadd_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask3_fmsubadd_round_pd(__m512d, __m512d, __m512d, __mmask8, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_fmsubadd_round_pd(__mmask8, __m512d, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_mask_fnmadd_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask3_fnmadd_round_ps(__m512, __m512, __m512, __mmask16, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_fnmadd_round_ps(__mmask16, __m512, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_fnmadd_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask3_fnmadd_round_pd(__m512d, __m512d, __m512d, __mmask8, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_fnmadd_round_pd(__mmask8, __m512d, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_mask_fnmsub_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512  __cdecl _mm512_mask3_fnmsub_round_ps(__m512, __m512, __m512, __mmask16, const int /* rounding */);
extern __m512  __cdecl _mm512_maskz_fnmsub_round_ps(__mmask16, __m512, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_fnmsub_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask3_fnmsub_round_pd(__m512d, __m512d, __m512d, __mmask8, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_fnmsub_round_pd(__mmask8, __m512d, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_maskz_sqrt_round_ps(__mmask16, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_sqrt_round_pd(__mmask8, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_sqrt_round_ps(__m512, __mmask16, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_sqrt_round_pd(__m512d, __mmask8, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_maskz_abs_ps(__mmask16, __m512);
extern __m512  __cdecl _mm512_mask_abs_ps(__m512, __mmask16, __m512);
extern __m512d __cdecl _mm512_maskz_abs_pd(__mmask8, __m512d);
extern __m512d __cdecl _mm512_mask_abs_pd(__m512d, __mmask8, __m512d);

extern __m512  __cdecl _mm512_maskz_max_round_ps(__mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_max_round_pd(__mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_max_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_max_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_maskz_min_round_ps(__mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_maskz_min_round_pd(__mmask8, __m512d, __m512d, const int /* rounding */);
extern __m512  __cdecl _mm512_mask_min_round_ps(__m512, __mmask16, __m512, __m512, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_min_round_pd(__m512d, __mmask8, __m512d, __m512d, const int /* rounding */);

extern __m512  __cdecl _mm512_maskz_rcp14_ps(__mmask16, __m512);
extern __m512d __cdecl _mm512_maskz_rcp14_pd(__mmask8, __m512d);
extern __m512  __cdecl _mm512_mask_rcp14_ps(__m512, __mmask16, __m512);
extern __m512d __cdecl _mm512_mask_rcp14_pd(__m512d, __mmask8, __m512d);

extern __m512  __cdecl _mm512_maskz_rsqrt14_ps(__mmask16, __m512);
extern __m512d __cdecl _mm512_maskz_rsqrt14_pd(__mmask8, __m512d);
extern __m512  __cdecl _mm512_mask_rsqrt14_ps(__m512, __mmask16, __m512);
extern __m512d __cdecl _mm512_mask_rsqrt14_pd(__m512d, __mmask8, __m512d);

extern __m512  __cdecl _mm512_mask_rcp28_round_ps(__m512, __mmask16, __m512, const int);
extern __m512  __cdecl _mm512_maskz_rcp28_round_ps(__mmask16, __m512, const int);
extern __m512d __cdecl _mm512_mask_rcp28_round_pd(__m512d, __mmask8, __m512d, const int);
extern __m512d __cdecl _mm512_maskz_rcp28_round_pd(__mmask8, __m512d, const int);
extern __m512  __cdecl _mm512_mask_rsqrt28_round_ps(__m512, __mmask16, __m512, const int);
extern __m512  __cdecl _mm512_maskz_rsqrt28_round_ps(__mmask16, __m512, const int);
extern __m512d __cdecl _mm512_mask_rsqrt28_round_pd(__m512d, __mmask8, __m512d, const int);
extern __m512d __cdecl _mm512_maskz_rsqrt28_round_pd(__mmask8, __m512d, const int);

extern __m512d __cdecl _mm512_maskz_cvt_roundps_pd(__mmask8, __m256, const int /* rounding */);
extern __m256  __cdecl _mm512_maskz_cvt_roundpd_ps(__mmask8, __m512d, const int /* rounding */);
extern __m512d __cdecl _mm512_mask_cvt_roundps_pd(__m512d, __mmask8, __m256, const int /* rounding */);
extern __m256  __cdecl _mm512_mask_cvt_roundpd_ps(__m256, __mmask8, __m512d, const int /* rounding */);

extern __mmask16 __cdecl _mm512_mask_cmp_round_ps_mask(__mmask16, __m512, __m512, const int, const int /* rounding */);
extern __mmask8  __cdecl _mm512_mask_cmp_round_pd_mask(__mmask8, __m512d, __m512d, const int, const int /* rounding */);

extern __m512  __cdecl _mm512_mask_broadcast_f32x2(__m512, __mmask16, __m128);
extern __m512  __cdecl _mm512_maskz_broadcast_f32x2(__mmask16, __m128);
extern __m512  __cdecl _mm512_mask_broadcast_f32x4(__m512, __mmask16, __m128);
extern __m512  __cdecl _mm512_maskz_broadcast_f32x4(__mmask16, __m128);
extern __m512  __cdecl _mm512_mask_broadcast_f32x8(__m512, __mmask16, __m256);
extern __m512  __cdecl _mm512_maskz_broadcast_f32x8(__mmask16, __m256);
extern __m512d __cdecl _mm512_mask_broadcast_f64x2(__m512d, __mmask8, __m128d);
extern __m512d __cdecl _mm512_maskz_broadcast_f64x2(__mmask8, __m128d);
extern __m512d __cdecl _mm512_mask_broadcast_f64x4(__m512d, __mmask8, __m256d);
extern __m512d __cdecl _mm512_maskz_broadcast_f64x4(__mmask8, __m256d);
extern __m512d __cdecl _mm512_mask_broadcastsd_pd(__m512d, __mmask8, __m128d);
extern __m512d __cdecl _mm512_maskz_broadcastsd_pd(__mmask8, __m128d);
extern __m512  __cdecl _mm512_mask_broadcastss_ps(__m512, __mmask16, __m128);
extern __m512  __cdecl _mm512_maskz_broadcastss_ps(__mmask16, __m128);

extern __m128  __cdecl _mm512_mask_extractf32x4_ps(__m128, __mmask8, __m512, const int);
extern __m128  __cdecl _mm512_maskz_extractf32x4_ps(__mmask8, __m512, int);
extern __m256  __cdecl _mm512_mask_extractf32x8_ps(__m256, __mmask8, __m512, const int);
extern __m256  __cdecl _mm512_maskz_extractf32x8_ps(__mmask8, __m512, int);
extern __m128d __cdecl _mm512_mask_extractf64x2_pd(__m128d, __mmask8, __m512d, const int);
extern __m128d __cdecl _mm512_maskz_extractf64x2_pd(__mmask8, __m512d, int);
extern __m256d __cdecl _mm512_mask_extractf64x4_pd(__m256d, __mmask8, __m512d, const int);
extern __m256d __cdecl _mm512_maskz_extractf64x4_pd(__mmask8, __m512d, int);

extern __m512  __cdecl _mm512_mask_insertf32x4(__m512, __mmask16, __m512, __m128, const int);
extern __m512  __cdecl _mm512_maskz_insertf32x4(__mmask16, __m512, __m128, int);
extern __m512  __cdecl _mm512_mask_insertf32x8(__m512, __mmask16, __m512, __m256, const int);
extern __m512  __cdecl _mm512_maskz_insertf32x8(__mmask16, __m512, __m256, int);
extern __m512d __cdecl _mm512_mask_insertf64x2(__m512d, __mmask8, __m512d, __m128d, const int);
extern __m512d __cdecl _mm512_maskz_insertf64x2(__mmask8, __m512d, __m128d, int);
extern __m512d __cdecl _mm512_mask_insertf64x4(__m512d, __mmask8, __m512d, __m256d, const int);
extern __m512d __cdecl _mm512_maskz_insertf64x4(__mmask8, __m512d, __m256d, int);

extern __m512  __cdecl _mm512_mask_shuffle_f32x4(__m512, __mmask16, __m512, __m512, const int);
extern __m512  __cdecl _mm512_maskz_shuffle_f32x4(__mmask16, __m512, __m512, const int);
extern __m512d __cdecl _mm512_mask_shuffle_f64x2(__m512d, __mmask8, __m512d, __m512d, const int);
extern __m512d __cdecl _mm512_maskz_shuffle_f64x2(__mmask8, __m512d, __m512d, const int);
extern __m512d __cdecl _mm512_mask_shuffle_pd(__m512d, __mmask8, __m512d, __m512d, const int);
extern __m512d __cdecl _mm512_maskz_shuffle_pd(__mmask8, __m512d, __m512d, const int);
extern __m512  __cdecl _mm512_mask_shuffle_ps(__m512, __mmask16, __m512, __m512, const int);
extern __m512  __cdecl _mm512_maskz_shuffle_ps(__mmask16, __m512, __m512, const int);


#define _mm512_setzero() _mm512_setzero_ps()
#define _mm512_undefined() _mm512_setzero()
#define _mm512_undefined_pd() _mm512_setzero_pd()
#define _mm512_undefined_ps() _mm512_undefined()

#define _mm512_set4_ps(a,b,c,d) \
    _mm512_set_ps((a), (b), (c), (d), (a), (b), (c), (d), \
    (a), (b), (c), (d), (a), (b), (c), (d))

#define _mm512_set4_pd(a,b,c,d) \
    _mm512_set_pd((a), (b), (c), (d), (a), (b), (c), (d))

#define _mm512_setr_ps(e0,e1,e2,e3,e4,e5,e6,e7,e8, \
    e9, e10, e11, e12, e13, e14, e15) \
    _mm512_set_ps((e15), (e14), (e13), (e12), (e11), (e10), \
    (e9), (e8), (e7), (e6), (e5), (e4), (e3), (e2), (e1), (e0))

#define _mm512_set_16to16_ps(e0,e1,e2,e3,e4,e5,e6,e7,e8, \
    e9, e10, e11, e12, e13, e14, e15) \
    _mm512_set_ps((e0), (e1), (e2), (e3), (e4), (e5), (e6), (e7), \
    (e8), (e9), (e10), (e11), (e12), (e13), (e14), (e15))

#define _mm512_setr_pd(e0,e1,e2,e3,e4,e5,e6,e7) \
    _mm512_set_pd((e7), (e6), (e5), (e4), (e3), (e2), (e1), (e0))

#define _mm512_set_8to8_pd(e0,e1,e2,e3,e4,e5,e6,e7) \
    _mm512_set_pd((e0), (e1), (e2), (e3), (e4), (e5), (e6), (e7))

#define _mm512_setr4_ps(a,b,c,d) \
    _mm512_set4_ps((d), (c), (b), (a))

#define _mm512_set_4to16_ps(a,b,c,d) \
    _mm512_set4_ps((d), (c), (b), (a))

#define _mm512_setr4_pd(a,b,c,d) \
    _mm512_set4_pd((d), (c), (b), (a))

#define _mm512_set_4to8_pd(a,b,c,d) \
    _mm512_set4_pd((d), (c), (b), (a))

#define _mm512_set_1to16_ps(x) _mm512_set1_ps((x))
#define _mm512_set_1to8_pd(x) _mm512_set1_pd((x))

#define _mm512_load_ps(x) _mm512_maskz_load_ps(_MM_K0_REG, (x))
#define _mm512_load_pd(x) _mm512_maskz_load_pd((__mmask8)_MM_K0_REG, (x))
#define _mm512_loadu_ps(x) _mm512_maskz_loadu_ps(_MM_K0_REG, (x))
#define _mm512_loadu_pd(x) _mm512_maskz_loadu_pd((__mmask8)_MM_K0_REG, (x))

#define _mm512_store_ps(x, v) _mm512_mask_store_ps((x), _MM_K0_REG, (v))
#define _mm512_store_pd(x, v) _mm512_mask_store_pd((x), (__mmask8)_MM_K0_REG, (v))

#define _mm512_add_ps(v1, v2) \
    _mm512_maskz_add_round_ps(_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_add_round_ps(v1, v2, e3) \
    _mm512_maskz_add_round_ps(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_add_ps(k1, v2, v3) \
    _mm512_maskz_add_round_ps((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_add_ps(v1, k2, v3, v4) \
    _mm512_mask_add_round_ps((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_add_pd(v1, v2) \
    _mm512_maskz_add_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_add_round_pd(v1, v2, e3) \
    _mm512_maskz_add_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_add_pd(k1, v2, v3) \
    _mm512_maskz_add_round_pd((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_add_pd(v1, k2, v3, v4) \
    _mm512_mask_add_round_pd((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)

#define _mm512_sub_ps(v1, v2) \
    _mm512_maskz_sub_round_ps(_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_sub_round_ps(v1, v2, e3) \
    _mm512_maskz_sub_round_ps(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_sub_ps(k1, v2, v3) \
    _mm512_maskz_sub_round_ps((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_sub_ps(v1, k2, v3, v4) \
    _mm512_mask_sub_round_ps((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_sub_pd(v1, v2) \
    _mm512_maskz_sub_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_sub_round_pd(v1, v2, e3) \
    _mm512_maskz_sub_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_sub_pd(k1, v2, v3) \
    _mm512_maskz_sub_round_pd((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_sub_pd(v1, k2, v3, v4) \
    _mm512_mask_sub_round_pd((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)

#define _mm512_mul_ps(v1, v2) \
    _mm512_maskz_mul_round_ps(_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mul_round_ps(v1, v2, e3) \
    _mm512_maskz_mul_round_ps(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_mul_ps(k1, v2, v3) \
    _mm512_maskz_mul_round_ps((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_mul_ps(v1, k2, v3, v4) \
    _mm512_mask_mul_round_ps((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mul_pd(v1, v2) \
    _mm512_maskz_mul_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mul_round_pd(v1, v2, e3) \
    _mm512_maskz_mul_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_mul_pd(k1, v2, v3) \
    _mm512_maskz_mul_round_pd((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_mul_pd(v1, k2, v3, v4) \
    _mm512_mask_mul_round_pd((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)

#define _mm512_div_ps(v1, v2) \
    _mm512_maskz_div_round_ps(_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_div_round_ps(v1, v2, e3) \
    _mm512_maskz_div_round_ps(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_div_ps(k1, v2, v3) \
    _mm512_maskz_div_round_ps((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_div_ps(v1, k2, v3, v4) \
    _mm512_mask_div_round_ps((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_div_pd(v1, v2) \
    _mm512_maskz_div_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_div_round_pd(v1, v2, e3) \
    _mm512_maskz_div_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_maskz_div_pd(k1, v2, v3) \
    _mm512_maskz_div_round_pd((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_div_pd(v1, k2, v3, v4) \
    _mm512_mask_div_round_pd((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)

#define _mm512_fmadd_round_ps(v1, v2, v3, e4) \
    _mm512_maskz_fmadd_round_ps(_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fmadd_ps(v1, v2, v3) \
    _mm512_fmadd_round_ps((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fmadd_ps(v1, k1, v2, v3) \
    _mm512_mask_fmadd_round_ps((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fmadd_ps(k1, v1, v2, v3) \
    _mm512_maskz_fmadd_round_ps((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_fmadd_round_pd(v1, v2, v3, e4) \
    _mm512_maskz_fmadd_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fmadd_pd(v1, v2, v3) \
    _mm512_fmadd_round_pd((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fmadd_pd(v1, k1, v2, v3) \
    _mm512_mask_fmadd_round_pd((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fmadd_pd(k1, v1, v2, v3) \
    _mm512_maskz_fmadd_round_pd((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)

#define _mm512_fmsub_round_ps(v1, v2, v3, e4) \
    _mm512_maskz_fmsub_round_ps(_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fmsub_ps(v1, v2, v3) \
    _mm512_fmsub_round_ps((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fmsub_ps(v1, k1, v2, v3) \
    _mm512_mask_fmsub_round_ps((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fmsub_ps(k1, v1, v2, v3) \
    _mm512_maskz_fmsub_round_ps((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_fmsub_round_pd(v1, v2, v3, e4) \
    _mm512_maskz_fmsub_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fmsub_pd(v1, v2, v3) \
    _mm512_fmsub_round_pd((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fmsub_pd(v1, k1, v2, v3) \
    _mm512_mask_fmsub_round_pd((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fmsub_pd(k1, v1, v2, v3) \
    _mm512_maskz_fmsub_round_pd((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)

#define _mm512_fnmadd_round_ps(v1, v2, v3, e4) \
    _mm512_maskz_fnmadd_round_ps(_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fnmadd_ps(v1, v2, v3) \
    _mm512_fnmadd_round_ps((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fnmadd_ps(v1, k1, v2, v3) \
    _mm512_mask_fnmadd_round_ps((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fnmadd_ps(k1, v1, v2, v3) \
    _mm512_maskz_fnmadd_round_ps((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_fnmadd_round_pd(v1, v2, v3, e4) \
    _mm512_maskz_fnmadd_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fnmadd_pd(v1, v2, v3) \
    _mm512_fnmadd_round_pd((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fnmadd_pd(v1, k1, v2, v3) \
    _mm512_mask_fnmadd_round_pd((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fnmadd_pd(k1, v1, v2, v3) \
    _mm512_maskz_fnmadd_round_pd((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)

#define _mm512_fnmsub_round_ps(v1, v2, v3, e4) \
    _mm512_maskz_fnmsub_round_ps(_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fnmsub_ps(v1, v2, v3) \
    _mm512_fnmsub_round_ps((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fnmsub_ps(v1, k1, v2, v3) \
    _mm512_mask_fnmsub_round_ps((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fnmsub_ps(k1, v1, v2, v3) \
    _mm512_maskz_fnmsub_round_ps((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_fnmsub_round_pd(v1, v2, v3, e4) \
    _mm512_maskz_fnmsub_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), (v3), (e4))
#define _mm512_fnmsub_pd(v1, v2, v3) \
    _mm512_fnmsub_round_pd((v1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_fnmsub_pd(v1, k1, v2, v3) \
    _mm512_mask_fnmsub_round_pd((v1), (k1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_fnmsub_pd(k1, v1, v2, v3) \
    _mm512_maskz_fnmsub_round_pd((k1), (v1), (v2), (v3), \
    _MM_FROUND_CUR_DIRECTION)

#define _mm512_sqrt_ps(v1) \
    _mm512_maskz_sqrt_round_ps(_MM_K0_REG, (v1), _MM_FROUND_CUR_DIRECTION)
#define _mm512_sqrt_round_ps(v1, e2) \
    _mm512_maskz_sqrt_round_ps(_MM_K0_REG, (v1), e2)
#define _mm512_mask_sqrt_ps(v1, k2, v2) \
    _mm512_mask_sqrt_round_ps(v1, k2, v2, _MM_FROUND_CUR_DIRECTION);
#define _mm512_maskz_sqrt_ps(k1, v1) \
    _mm512_maskz_sqrt_round_ps((k1), (v1), _MM_FROUND_CUR_DIRECTION)
#define _mm512_sqrt_pd(v1) \
    _mm512_maskz_sqrt_round_pd((__mmask8)_MM_K0_REG, (v1), _MM_FROUND_CUR_DIRECTION)
#define _mm512_sqrt_round_pd(v1, e2) \
    _mm512_maskz_sqrt_round_pd((__mmask8)_MM_K0_REG, (v1), e2)
#define _mm512_mask_sqrt_pd(v1, k2, v2) \
    _mm512_mask_sqrt_round_pd(v1, k2, v2, _MM_FROUND_CUR_DIRECTION);
#define _mm512_maskz_sqrt_pd(k1, v1) \
    _mm512_maskz_sqrt_round_pd((k1), (v1), _MM_FROUND_CUR_DIRECTION)

#define _mm512_abs_ps(v1) \
    _mm512_maskz_abs_ps(_MM_K0_REG, (v1))
#define _mm512_abs_pd(v1) \
    _mm512_maskz_abs_pd((__mmask8)_MM_K0_REG, (v1))

#define _mm512_max_ps(v1, v2) \
    _mm512_maskz_max_round_ps(_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_max_round_ps(v1, v2, e3) \
    _mm512_maskz_max_round_ps(_MM_K0_REG, (v1), (v2), e3)
#define _mm512_mask_max_ps(v1, k2, v3, v4) \
    _mm512_mask_max_round_ps((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_max_ps(k1, v2, v3) \
    _mm512_maskz_max_round_ps((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_max_pd(v1, v2) \
    _mm512_maskz_max_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_max_round_pd(v1, v2, e3) \
    _mm512_maskz_max_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), e3)
#define _mm512_mask_max_pd(v1, k2, v3, v4) \
    _mm512_mask_max_round_pd((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_max_pd(k1, v2, v3) \
    _mm512_maskz_max_round_pd((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)

#define _mm512_min_ps(v1, v2) \
    _mm512_maskz_min_round_ps(_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_min_round_ps(v1, v2, e3) \
    _mm512_maskz_min_round_ps(_MM_K0_REG, (v1), (v2), e3)
#define _mm512_mask_min_ps(v1, k2, v3, v4) \
    _mm512_mask_min_round_ps((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_min_ps(k1, v2, v3) \
    _mm512_maskz_min_round_ps((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_min_pd(v1, v2) \
    _mm512_maskz_min_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_min_round_pd(v1, v2, e3) \
    _mm512_maskz_min_round_pd((__mmask8)_MM_K0_REG, (v1), (v2), e3)
#define _mm512_mask_min_pd(v1, k2, v3, v4) \
    _mm512_mask_min_round_pd((v1), (k2), (v3), (v4), _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_min_pd(k1, v2, v3) \
    _mm512_maskz_min_round_pd((k1), (v2), (v3), _MM_FROUND_CUR_DIRECTION)

#define _mm512_rcp14_ps(v1) \
    _mm512_maskz_rcp14_ps(_MM_K0_REG, v1);
#define _mm512_rcp14_pd(v1) \
    _mm512_maskz_rcp14_pd((__mmask8)_MM_K0_REG, v1);

#define _mm512_rsqrt14_ps(v1) \
    _mm512_maskz_rsqrt14_ps(_MM_K0_REG, v1);
#define _mm512_rsqrt14_pd(v1) \
    _mm512_maskz_rsqrt14_pd((__mmask8)_MM_K0_REG, v1);

#define _mm512_cvt_roundps_pd(v1, e2) \
    _mm512_maskz_cvt_roundps_pd((__mmask8)_MM_K0_REG, (v1), e2)
#define _mm512_cvtps_pd(v1) \
    _mm512_cvt_roundps_pd((v1), _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_cvtps_pd(k1, v2) \
    _mm512_maskz_cvt_roundps_pd((k1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_cvtps_pd(v1, k2, v3) \
    _mm512_mask_cvt_roundps_pd((v1), (k2), (v3), _MM_FROUND_CUR_DIRECTION)
#define _mm512_cvt_roundpd_ps(v1, e2) \
    _mm512_maskz_cvt_roundpd_ps((__mmask8)_MM_K0_REG, (v1), e2)
#define _mm512_cvtpd_ps(v1) \
    _mm512_cvt_roundpd_ps((v1), _MM_FROUND_CUR_DIRECTION)
#define _mm512_maskz_cvtpd_ps(k1, v2) \
    _mm512_maskz_cvt_roundpd_ps((k1), (v2), _MM_FROUND_CUR_DIRECTION)
#define _mm512_mask_cvtpd_ps(v1_old, k1, v2) \
    _mm512_mask_cvt_roundpd_ps((v1_old), (k1), (v2), _MM_FROUND_CUR_DIRECTION)

#define _mm512_cmp_ps_mask(v1, v2, i3) \
    _mm512_mask_cmp_round_ps_mask(_MM_K0_REG, v1, v2, i3, _MM_FROUND_CUR_DIRECTION)
#define _mm512_cmp_round_ps_mask(v1, v2, i3, e4) \
    _mm512_mask_cmp_round_ps_mask(_MM_K0_REG, v1, v2, i3, e4)
#define _mm512_mask_cmp_ps_mask(k1, v2, v3, i4) \
    _mm512_mask_cmp_round_ps_mask(k1, v2, v3, i4, _MM_FROUND_CUR_DIRECTION)
#define _mm512_cmp_pd_mask(v1, v2, i3) \
    _mm512_mask_cmp_round_pd_mask((__mmask8)_MM_K0_REG, v1, v2, i3, _MM_FROUND_CUR_DIRECTION)
#define _mm512_cmp_round_pd_mask(v1, v2, i3, e4) \
    _mm512_mask_cmp_round_pd_mask((__mmask8)_MM_K0_REG, v1, v2, i3, e4)
#define _mm512_mask_cmp_pd_mask(k1, v2, v3, i4) \
    _mm512_mask_cmp_round_pd_mask(k1, v2, v3, i4, _MM_FROUND_CUR_DIRECTION)

#define _mm512_cmpeq_ps_mask(v1, v2) _mm512_cmp_ps_mask((v1), (v2), _CMP_EQ_OQ)
#define _mm512_mask_cmpeq_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_EQ_OQ)
#define _mm512_cmplt_ps_mask(v1, v2) _mm512_cmp_ps_mask((v1), (v2), _CMP_LT_OS)
#define _mm512_mask_cmplt_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_LT_OS)
#define _mm512_cmple_ps_mask(v1, v2) _mm512_cmp_ps_mask((v1), (v2), _CMP_LE_OS)
#define _mm512_mask_cmple_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_LE_OS)
#define _mm512_cmpunord_ps_mask(v1, v2) \
    _mm512_cmp_ps_mask((v1), (v2), _CMP_UNORD_Q)
#define _mm512_mask_cmpunord_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_UNORD_Q)
#define _mm512_cmpneq_ps_mask(v1, v2) \
    _mm512_cmp_ps_mask((v1), (v2), _CMP_NEQ_UQ)
#define _mm512_mask_cmpneq_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_NEQ_UQ)
#define _mm512_cmpnlt_ps_mask(v1, v2) \
    _mm512_cmp_ps_mask((v1), (v2), _CMP_NLT_US)
#define _mm512_mask_cmpnlt_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_NLT_US)
#define _mm512_cmpnle_ps_mask(v1, v2) \
    _mm512_cmp_ps_mask((v1), (v2), _CMP_NLE_US)
#define _mm512_mask_cmpnle_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_NLE_US)
#define _mm512_cmpord_ps_mask(v1, v2) \
    _mm512_cmp_ps_mask((v1), (v2), _CMP_ORD_Q)
#define _mm512_mask_cmpord_ps_mask(k1, v1, v2) \
    _mm512_mask_cmp_ps_mask((k1), (v1), (v2), _CMP_ORD_Q)

#define _mm512_cmpeq_pd_mask(v1, v2) _mm512_cmp_pd_mask((v1), (v2), _CMP_EQ_OQ)
#define _mm512_mask_cmpeq_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_EQ_OQ)
#define _mm512_cmplt_pd_mask(v1, v2) _mm512_cmp_pd_mask((v1), (v2), _CMP_LT_OS)
#define _mm512_mask_cmplt_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_LT_OS)
#define _mm512_cmple_pd_mask(v1, v2) _mm512_cmp_pd_mask((v1), (v2), _CMP_LE_OS)
#define _mm512_mask_cmple_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_LE_OS)
#define _mm512_cmpunord_pd_mask(v1, v2) \
    _mm512_cmp_pd_mask((v1), (v2), _CMP_UNORD_Q)
#define _mm512_mask_cmpunord_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_UNORD_Q)
#define _mm512_cmpneq_pd_mask(v1, v2) \
    _mm512_cmp_pd_mask((v1), (v2), _CMP_NEQ_UQ)
#define _mm512_mask_cmpneq_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_NEQ_UQ)
#define _mm512_cmpnlt_pd_mask(v1, v2) \
    _mm512_cmp_pd_mask((v1), (v2), _CMP_NLT_US)
#define _mm512_mask_cmpnlt_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_NLT_US)
#define _mm512_cmpnle_pd_mask(v1, v2) \
    _mm512_cmp_pd_mask((v1), (v2), _CMP_NLE_US)
#define _mm512_mask_cmpnle_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_NLE_US)
#define _mm512_cmpord_pd_mask(v1, v2) \
    _mm512_cmp_pd_mask((v1), (v2), _CMP_ORD_Q)
#define _mm512_mask_cmpord_pd_mask(k1, v1, v2) \
    _mm512_mask_cmp_pd_mask((k1), (v1), (v2), _CMP_ORD_Q)

#define _mm512_broadcast_f32x2(v1) \
    _mm512_maskz_broadcast_f32x2(_MM_K0_REG, (v1))
#define _mm512_broadcast_f32x4(v1) \
    _mm512_maskz_broadcast_f32x4(_MM_K0_REG, (v1))
#define _mm512_broadcast_f32x8(v1) \
    _mm512_maskz_broadcast_f32x8(_MM_K0_REG, (v1))
#define _mm512_broadcast_f64x2(v1) \
    _mm512_maskz_broadcast_f64x2((__mmask8)_MM_K0_REG, (v1))
#define _mm512_broadcast_f64x4(v1) \
    _mm512_maskz_broadcast_f64x4((__mmask8)_MM_K0_REG, (v1))
#define _mm512_broadcastsd_pd(v1) \
    _mm512_maskz_broadcastsd_pd((__mmask8)_MM_K0_REG, (v1))
#define _mm512_broadcastss_ps(v1) \
    _mm512_maskz_broadcastss_ps(_MM_K0_REG, (v1))

#define _mm512_extractf32x4_ps(v1, e2) \
    _mm512_maskz_extractf32x4_ps((__mmask8)_MM_K0_REG, (v1), (e2))
#define _mm512_extractf32x8_ps(v1, e2) \
    _mm512_maskz_extractf32x8_ps((__mmask8)_MM_K0_REG, (v1), (e2))
#define _mm512_extractf64x2_pd(v1, e2) \
    _mm512_maskz_extractf64x2_pd((__mmask8)_MM_K0_REG, (v1), (e2))
#define _mm512_extractf64x4_pd(v1, e2) \
    _mm512_maskz_extractf64x4_pd((__mmask8)_MM_K0_REG, (v1), (e2))

#define _mm512_insertf32x4(v1, v2, e3) \
    _mm512_maskz_insertf32x4(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_insertf32x8(v1, v2, e3) \
    _mm512_maskz_insertf32x8(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_insertf64x2(v1, v2, e3) \
    _mm512_maskz_insertf64x2((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_insertf64x4(v1, v2, e3) \
    _mm512_maskz_insertf64x4((__mmask8)_MM_K0_REG, (v1), (v2), (e3))

#define _mm512_shuffle_f32x4(v1, v2, e3) \
    _mm512_maskz_shuffle_f32x4(_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_shuffle_f64x2(v1, v2, e3) \
    _mm512_maskz_shuffle_f64x2((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_shuffle_pd(v1, v2, e3) \
    _mm512_maskz_shuffle_pd((__mmask8)_MM_K0_REG, (v1), (v2), (e3))
#define _mm512_shuffle_ps(v1, v2, e3) \
    _mm512_maskz_shuffle_ps(_MM_K0_REG, (v1), (v2), (e3))

#define _mm512_kand(k1, k2) \
    ((__mmask16) ((k1) & (k2)))
#define _mm512_kandn(k1, k2) \
    ((__mmask16) (~(k1) & (k2)))
#define _mm512_kor(k1, k2) \
    ((__mmask16) ((k1) | (k2)))
#define _mm512_kxor(k1, k2) \
    ((__mmask16) ((k1) ^ (k2)))
#define _mm512_kxnor(k1, k2) \
    ((__mmask16) (~((k1) ^ (k2))))
#define _mm512_knot(k1) \
    ((__mmask16) (~(k1)))

#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif /* _ZMMINTRIN_H_INCLUDED */
