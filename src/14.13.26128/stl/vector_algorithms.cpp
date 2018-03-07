// Copyright (c) Microsoft Corporation. All rights reserved.
// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.
#if (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID)

#include <emmintrin.h>
#include <immintrin.h>
#include <intrin0.h>
#include <isa_availability.h>
#include <xmmintrin.h>

extern "C" long __isa_enabled;

template<class _BidIt>
	static inline void _Reverse_tail(_BidIt _First, _BidIt _Last) throw()
	{
	for (; _First != _Last && _First != --_Last; ++_First)
		{
		const auto _Temp = *_First;
		*_First = *_Last;
		*_Last = _Temp;
		}
	}

static inline size_t _Byte_length(void * _First, void * _Last) throw()
	{
	return (static_cast<unsigned char *>(_Last) - static_cast<unsigned char *>(_First));
	}

static inline void _Advance_bytes(void *& _Target, ptrdiff_t _Offset) throw()
	{
	_Target = static_cast<unsigned char *>(_Target) + _Offset;
	}

extern "C"
{

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_1(void * _First, void * _Last) noexcept
	{
	if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2))
		{
		const __m256i _Reverse_char_lanes_avx = _mm256_set_epi8(
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
		do
			{
			_Advance_bytes(_Last, -32);
			// vpermq to load left and right, and transpose the lanes
			const __m256i _Left = _mm256_permute4x64_epi64(_mm256_loadu_si256(static_cast<__m256i *>(_First)), 78);
			const __m256i _Right = _mm256_permute4x64_epi64(_mm256_loadu_si256(static_cast<__m256i *>(_Last)), 78);
			// transpose all the chars in the lanes
			const __m256i _Left_reversed = _mm256_shuffle_epi8(_Left, _Reverse_char_lanes_avx);
			const __m256i _Right_reversed = _mm256_shuffle_epi8(_Right, _Reverse_char_lanes_avx);
			_mm256_storeu_si256(static_cast<__m256i *>(_First), _Right_reversed);
			_mm256_storeu_si256(static_cast<__m256i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 32);
			}
		while (_First != _Stop_at);
		}

	if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE42))
		{
		const __m128i _Reverse_char_sse = _mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
		do
			{
			_Advance_bytes(_Last, -16);
			const __m128i _Left = _mm_loadu_si128(static_cast<__m128i *>(_First));
			const __m128i _Right = _mm_loadu_si128(static_cast<__m128i *>(_Last));
			const __m128i _Left_reversed = _mm_shuffle_epi8(_Left, _Reverse_char_sse); // SSSE3
			const __m128i _Right_reversed = _mm_shuffle_epi8(_Right, _Reverse_char_sse);
			_mm_storeu_si128(static_cast<__m128i *>(_First), _Right_reversed);
			_mm_storeu_si128(static_cast<__m128i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 16);
			}
		while (_First != _Stop_at);
		}

	_Reverse_tail(static_cast<unsigned char *>(_First), static_cast<unsigned char *>(_Last));
	}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_2(void * _First, void * _Last) noexcept
	{
	if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2))
		{
		const __m256i _Reverse_short_lanes_avx = _mm256_set_epi8(
			1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14,
			1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
		do
			{
			_Advance_bytes(_Last, -32);
			const __m256i _Left = _mm256_permute4x64_epi64(_mm256_loadu_si256(static_cast<__m256i *>(_First)), 78);
			const __m256i _Right = _mm256_permute4x64_epi64(_mm256_loadu_si256(static_cast<__m256i *>(_Last)), 78);
			const __m256i _Left_reversed = _mm256_shuffle_epi8(_Left, _Reverse_short_lanes_avx);
			const __m256i _Right_reversed = _mm256_shuffle_epi8(_Right, _Reverse_short_lanes_avx);
			_mm256_storeu_si256(static_cast<__m256i *>(_First), _Right_reversed);
			_mm256_storeu_si256(static_cast<__m256i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 32);
			}
		while (_First != _Stop_at);
		}

	if (_Byte_length(_First, _Last) >= 32 && _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE42))
		{
		const __m128i _Reverse_short_sse = _mm_set_epi8(1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
		do
			{
			_Advance_bytes(_Last, -16);
			const __m128i _Left = _mm_loadu_si128(static_cast<__m128i *>(_First));
			const __m128i _Right = _mm_loadu_si128(static_cast<__m128i *>(_Last));
			const __m128i _Left_reversed = _mm_shuffle_epi8(_Left, _Reverse_short_sse); // SSSE3
			const __m128i _Right_reversed = _mm_shuffle_epi8(_Right, _Reverse_short_sse);
			_mm_storeu_si128(static_cast<__m128i *>(_First), _Right_reversed);
			_mm_storeu_si128(static_cast<__m128i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 16);
			}
		while (_First != _Stop_at);
		}

	_Reverse_tail(static_cast<unsigned short *>(_First), static_cast<unsigned short *>(_Last));
	}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_4(void * _First, void * _Last) noexcept
	{
	if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2))
		{
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
		do
			{
			_Advance_bytes(_Last, -32);
			const __m256i _Left = _mm256_permute4x64_epi64(_mm256_loadu_si256(static_cast<__m256i *>(_First)), 78);
			const __m256i _Right = _mm256_permute4x64_epi64(_mm256_loadu_si256(static_cast<__m256i *>(_Last)), 78);
			const __m256i _Left_reversed = _mm256_shuffle_epi32(_Left, 27);
			const __m256i _Right_reversed = _mm256_shuffle_epi32(_Right, 27);
			_mm256_storeu_si256(static_cast<__m256i *>(_First), _Right_reversed);
			_mm256_storeu_si256(static_cast<__m256i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 32);
			}
		while (_First != _Stop_at);
		}

	if (_Byte_length(_First, _Last) >= 32
 #ifdef _M_IX86
		&& _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
 #endif /* _M_IX86 */
		)
		{
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
		do
			{
			_Advance_bytes(_Last, -16);
			const __m128i _Left = _mm_loadu_si128(static_cast<__m128i *>(_First));
			const __m128i _Right = _mm_loadu_si128(static_cast<__m128i *>(_Last));
			const __m128i _Left_reversed = _mm_shuffle_epi32(_Left, 27);
			const __m128i _Right_reversed = _mm_shuffle_epi32(_Right, 27);
			_mm_storeu_si128(static_cast<__m128i *>(_First), _Right_reversed);
			_mm_storeu_si128(static_cast<__m128i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 16);
			}
		while (_First != _Stop_at);
		}

	_Reverse_tail(static_cast<unsigned long *>(_First), static_cast<unsigned long *>(_Last));
	}

__declspec(noalias) void __cdecl __std_reverse_trivially_swappable_8(void * _First, void * _Last) noexcept
	{
	if (_Byte_length(_First, _Last) >= 64 && _bittest(&__isa_enabled, __ISA_AVAILABLE_AVX2))
		{
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 6 << 5);
		do
			{
			_Advance_bytes(_Last, -32);
			const __m256i _Left = _mm256_loadu_si256(static_cast<__m256i *>(_First));
			const __m256i _Right = _mm256_loadu_si256(static_cast<__m256i *>(_Last));
			const __m256i _Left_reversed = _mm256_permute4x64_epi64(_Left, 27);
			const __m256i _Right_reversed = _mm256_permute4x64_epi64(_Right, 27);
			_mm256_storeu_si256(static_cast<__m256i *>(_First), _Right_reversed);
			_mm256_storeu_si256(static_cast<__m256i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 32);
			}
		while (_First != _Stop_at);
		}

	if (_Byte_length(_First, _Last) >= 32
 #ifdef _M_IX86
		&& _bittest(&__isa_enabled, __ISA_AVAILABLE_SSE2)
 #endif /* _M_IX86 */
		)
		{
		void * _Stop_at = _First;
		_Advance_bytes(_Stop_at, _Byte_length(_First, _Last) >> 5 << 4);
		do
			{
			_Advance_bytes(_Last, -16);
			const __m128i _Left = _mm_loadu_si128(static_cast<__m128i *>(_First));
			const __m128i _Right = _mm_loadu_si128(static_cast<__m128i *>(_Last));
			const __m128i _Left_reversed = _mm_shuffle_epi32(_Left, 78);
			const __m128i _Right_reversed = _mm_shuffle_epi32(_Right, 78);
			_mm_storeu_si128(static_cast<__m128i *>(_First), _Right_reversed);
			_mm_storeu_si128(static_cast<__m128i *>(_Last), _Left_reversed);
			_Advance_bytes(_First, 16);
			}
		while (_First != _Stop_at);
		}

	_Reverse_tail(static_cast<unsigned long long *>(_First), static_cast<unsigned long long *>(_Last));
	}

} /* extern "C" */

#endif /* (defined(_M_IX86) || defined(_M_X64)) && !defined(_M_HYBRID) */
