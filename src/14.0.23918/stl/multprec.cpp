// multprec: implements multiprecision math for TR1 random number generators
#include <yvals.h>

//  #include <random>
  #include <limits>

_STD_BEGIN
#ifdef _ULONGLONG
typedef _ULONGLONG _Max_type;

#else /* _ULONGLONG */
typedef unsigned long _Max_type;
#endif /* _ULONGLONG */

static const int _MP_len = 5;
typedef _Max_type _MP_arr[_MP_len];

_CRTIMP2_PURE _Max_type __CLRCALL_PURE_OR_CDECL _MP_Get(_MP_arr);
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _MP_Add(_MP_arr, _Max_type);
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _MP_Mul(_MP_arr, _Max_type, _Max_type);
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _MP_Rem(_MP_arr, _Max_type);

static const int shift = _STD numeric_limits<_Max_type>::digits / 2;
static const _Max_type mask = ~(~_Max_type(0) << shift);
static const _Max_type max = mask + 1;

_Max_type __CLRCALL_PURE_OR_CDECL _MP_Get(_MP_arr u)
	{	// convert multi-word value to scalar value
	return ((u[1] << shift) + u[0]);
	}

static void add(_Max_type *u, int ulen, _Max_type *v, int vlen)
	{	// add multi-word value to multi-word value
	int i;
	_Max_type k = 0;
	for (i = 0; i < vlen; ++i)
		{	// add multi-word values
		u[i] += v[i] + k;
		k = u[i] >> shift;
		u[i] &= mask;
		}
	for (; k != 0 && i < ulen; ++i)
		{	// propagate carry
		u[i] += k;
		k = u[i] >> shift;
		u[i] &= mask;
		}
	}

void __CLRCALL_PURE_OR_CDECL _MP_Add(_MP_arr u, _Max_type v0)
	{	// add scalar value to multi-word value
	_Max_type v[2];
	v[0] = v0 & mask;
	v[1] = v0 >> shift;
	add(u, _MP_len, v, 2);
	}

static void mul(_Max_type *u, int ulen, _Max_type v0)
	{	// multiply multi-word value by single-word value
	_Max_type k = 0;
	for (int i = 0; i < ulen; ++i)
		{	// multiply and propagate carry
		u[i] = u[i] * v0 + k;
		k = u[i] >> shift;
		u[i] &= mask;
		}
	}

void __CLRCALL_PURE_OR_CDECL _MP_Mul(_MP_arr w, _Max_type u0, _Max_type v0)
	{	// multiply multi-word value by multi-word value
	static const int m = 2;
	static const int n = 2;
	_Max_type u[2];
	_Max_type v[2];
	u[0] = u0 & mask;
	u[1] = u0 >> shift;
	v[0] = v0 & mask;
	v[1] = v0 >> shift;

	// Knuth, vol. 2, p. 268, Algorithm M
	// M1: [Initialize.]
	for (int i = 0; i < m + n + 1; ++i)
		w[i] = 0;
	for (int j = 0; j < n; ++j)
		{	// M2: [Zero multiplier?]
		if (v[j] == 0)
			w[j + m] = 0;
		else
			{	// multiply by non-zero value
			_Max_type k = 0;
			int i;
			// M3: [Initialize i.]
			for (i = 0; i < m; ++i)
				{	// M4: [Multiply and add.]
				w[i + j] = u[i] * v[j] + w[i + j] + k;
				k = w[i + j] >> shift;
				w[i + j] &= mask;
				// M5: [Loop on i.]
				}
			w[i + j] = k;
			}
		// M6: [Loop on j.]
		}
	}

static void div(_MP_arr u, _Max_type v0)
	{	// divide multi-word value by scalar value (fits in lower half of _Max_type)
	_Max_type k = 0;
	int ulen = _MP_len;
	while (0 <= --ulen)
		{	// propagate remainder and divide
		_Max_type tmp = (k << shift) + u[ulen];
		u[ulen] = tmp / v0;
		k = tmp % v0;
		}
	}

static int limit(_Max_type *u, int ulen)
	{	// get index of last non-zero value
	while (u[ulen - 1] == 0)
		--ulen;
	return (ulen);
	}

void __CLRCALL_PURE_OR_CDECL _MP_Rem(_MP_arr u, _Max_type v0)
	{	// divide multi-word value by value, leaving remainder in u
	_Max_type v[2];
	v[0] = v0 & mask;
	v[1] = v0 >> shift;
	const int n = limit(v, 2);
	const int m = limit(u, _MP_len) - n;

	// Knuth, vol. 2, p. 272, Algorithm D
	// D1: [Normalize.]
	_Max_type d = max / (v[n - 1] + 1);
	if (d != 1)
		{	// scale numerator and divisor
		mul(u, _MP_len, d);
		mul(v, n, d);
		}
	// D2: [Initialize j.]
	for (int j = m; 0 <= j; --j)
		{	// D3: [Calculate qh.]
		_Max_type qh = ((u[j + n] << shift) + u[j + n - 1]) / v[n - 1];
		if (qh == 0)
			continue;
		_Max_type rh = ((u[j + n] << shift) + u[j + n - 1]) % v[n - 1];
		for (;;)
			if (qh < max && qh * v[n - 2] <= (rh << shift) + u[j + n - 2])
				break;
			else
				{	// reduce tentative value and retry
				--qh;
				rh += v[n - 1];
				if (max <= rh)
					break;
				}
		// D4: [Multiply and subtract.]
		_Max_type k = 0;
		int i;
		for (i = 0; i < n; ++i)
			{	// multiply and subtract
			u[j + i] -= qh * v[i] + k;
			k = u[j + i] >> shift;
			if (k)
				k = max - k;
			u[j + i] &= mask;
			}
		for (; k != 0 && j + i < _MP_len; ++i)
			{	// propagate borrow
			u[j + i] -= k;
			k = u[j + i] >> shift;
			if (k)
				k = max - k;
			u[j + i] &= mask;
			}
		// D5: [Test remainder.]
		if (k != 0)
			{	// D6: [Add back.]
			--qh;
			add(u + j, n + 1, v, n);
			}
		// D7: [Loop on j.]
		}
	// D8: [Unnormalize.]
	if (d != 1)
		div(u, d);
	}
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
