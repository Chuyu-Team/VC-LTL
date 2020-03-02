/*

setlocal.cpp 的修正实现，利用现有msvcrt 函数生成新的


*/

#include <corecrt_internal.h>
#include <locale.h>
#include "winapi_thunks.h"

#include <mbctype.h>
#include <functional>
#include "msvcrt_IAT.h"

//static __crt_locale_pointers* ___StaticLocale;

//const char* __clocalestr;/* (___StaticLocale->locinfo->lc_category[1].locale)*/
//const lconv* p__lconv_c;
//lconv __lconv_c_data;
//#define __lconv_c_data (*p__lconv_c)
//const _multibyte_data_msvcrt* p__initialmbcinfo;
//const _locale_data_msvcrt* p__initiallocinfo;
//const _lc_time_data_msvcrt* p__lc_time_c;
//#define __lconv_c (*p__lconv_c)

//#define __initialmbcinfo (*p__initialmbcinfo)
//#define __initiallocinfo (*p__initiallocinfo)
//#define __lc_time_c (*p__lc_time_c)

struct _locale_data_msvcrt_winxp
{
	long                      refcount;

	//1
	unsigned int              _locale_lc_codepage;
	//2
	unsigned int              lc_collate_cp;
	//3
	unsigned long lc_handle[6]; /* LCID */
	//9
	int                        lc_clike;
	//10
	_Field_range_(1, 2) int    _locale_mb_cur_max;
	//11
	long* lconv_intl_refcount;
	//12
	long* lconv_num_refcount;
	//13
	long* lconv_mon_refcount;
	//14
	struct lconv* lconv;
	//0x3C 15
	struct lconv* lconv_intl;
	//16
	long* ctype1_refcount;
	//17
	unsigned short* ctype1;
	//18
	unsigned short const* _locale_pctype;
	//19
	struct _lc_time_data_msvcrt* lc_time_curr;
	//20
	struct _lc_time_data_msvcrt* lc_time_intl;
};


#define _CTABSIZE   257     /* size of ctype tables */

#if _CRT_NTDDI_MIN < NTDDI_VISTA 

extern "C" extern unsigned char const __newclmap[384]
{
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    0x00,   // 00 (NUL)
    0x01,   // 01 (SOH)
    0x02,   // 02 (STX)
    0x03,   // 03 (ETX)
    0x04,   // 04 (EOT)
    0x05,   // 05 (ENQ)
    0x06,   // 06 (ACK)
    0x07,   // 07 (BEL)
    0x08,   // 08 (BS)
    0x09,   // 09 (HT)
    0x0A,   // 0A (LF)
    0x0B,   // 0B (VT)
    0x0C,   // 0C (FF)
    0x0D,   // 0D (CR)
    0x0E,   // 0E (SI)
    0x0F,   // 0F (SO)
    0x10,   // 10 (DLE)
    0x11,   // 11 (DC1)
    0x12,   // 12 (DC2)
    0x13,   // 13 (DC3)
    0x14,   // 14 (DC4)
    0x15,   // 15 (NAK)
    0x16,   // 16 (SYN)
    0x17,   // 17 (ETB)
    0x18,   // 18 (CAN)
    0x19,   // 19 (EM)
    0x1A,   // 1A (SUB)
    0x1B,   // 1B (ESC)
    0x1C,   // 1C (FS)
    0x1D,   // 1D (GS)
    0x1E,   // 1E (RS)
    0x1F,   // 1F (US)
    0x20,   // 20 SPACE
    0x21,   // 21 !
    0x22,   // 22 "
    0x23,   // 23 #
    0x24,   // 24 $
    0x25,   // 25 %
    0x26,   // 26 &
    0x27,   // 27 '
    0x28,   // 28 (
    0x29,   // 29 )
    0x2A,   // 2A *
    0x2B,   // 2B +
    0x2C,   // 2C ,
    0x2D,   // 2D -
    0x2E,   // 2E .
    0x2F,   // 2F /
    0x30,   // 30 0
    0x31,   // 31 1
    0x32,   // 32 2
    0x33,   // 33 3
    0x34,   // 34 4
    0x35,   // 35 5
    0x36,   // 36 6
    0x37,   // 37 7
    0x38,   // 38 8
    0x39,   // 39 9
    0x3A,   // 3A :
    0x3B,   // 3B ;
    0x3C,   // 3C <
    0x3D,   // 3D =
    0x3E,   // 3E >
    0x3F,   // 3F ?
    0x40,   // 40 @
    0x61,   // 41 A
    0x62,   // 42 B
    0x63,   // 43 C
    0x64,   // 44 D
    0x65,   // 45 E
    0x66,   // 46 F
    0x67,   // 47 G
    0x68,   // 48 H
    0x69,   // 49 I
    0x6A,   // 4A J
    0x6B,   // 4B K
    0x6C,   // 4C L
    0x6D,   // 4D M
    0x6E,   // 4E N
    0x6F,   // 4F O
    0x70,   // 50 P
    0x71,   // 51 Q
    0x72,   // 52 R
    0x73,   // 53 S
    0x74,   // 54 T
    0x75,   // 55 U
    0x76,   // 56 V
    0x77,   // 57 W
    0x78,   // 58 X
    0x79,   // 59 Y
    0x7A,   // 5A Z
    0x5B,   // 5B [
    0x5C,   // 5C \ 
    0x5D,   // 5D ]
    0x5E,   // 5E ^
    0x5F,   // 5F _
    0x60,   // 60 `
    0x61,   // 61 a
    0x62,   // 62 b
    0x63,   // 63 c
    0x64,   // 64 d
    0x65,   // 65 e
    0x66,   // 66 f
    0x67,   // 67 g
    0x68,   // 68 h
    0x69,   // 69 i
    0x6A,   // 6A j
    0x6B,   // 6B k
    0x6C,   // 6C l
    0x6D,   // 6D m
    0x6E,   // 6E n
    0x6F,   // 6F o
    0x70,   // 70 p
    0x71,   // 71 q
    0x72,   // 72 r
    0x73,   // 73 s
    0x74,   // 74 t
    0x75,   // 75 u
    0x76,   // 76 v
    0x77,   // 77 w
    0x78,   // 78 x
    0x79,   // 79 y
    0x7A,   // 7A z
    0x7B,   // 7B {
    0x7C,   // 7C |
    0x7D,   // 7D }
    0x7E,   // 7E ~
    0x7F,   // 7F (DEL)
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

extern "C" extern unsigned char const __newcumap[384]
{
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    0x00,   // 00 (NUL)
    0x01,   // 01 (SOH)
    0x02,   // 02 (STX)
    0x03,   // 03 (ETX)
    0x04,   // 04 (EOT)
    0x05,   // 05 (ENQ)
    0x06,   // 06 (ACK)
    0x07,   // 07 (BEL)
    0x08,   // 08 (BS)
    0x09,   // 09 (HT)
    0x0A,   // 0A (LF)
    0x0B,   // 0B (VT)
    0x0C,   // 0C (FF)
    0x0D,   // 0D (CR)
    0x0E,   // 0E (SI)
    0x0F,   // 0F (SO)
    0x10,   // 10 (DLE)
    0x11,   // 11 (DC1)
    0x12,   // 12 (DC2)
    0x13,   // 13 (DC3)
    0x14,   // 14 (DC4)
    0x15,   // 15 (NAK)
    0x16,   // 16 (SYN)
    0x17,   // 17 (ETB)
    0x18,   // 18 (CAN)
    0x19,   // 19 (EM)
    0x1A,   // 1A (SUB)
    0x1B,   // 1B (ESC)
    0x1C,   // 1C (FS)
    0x1D,   // 1D (GS)
    0x1E,   // 1E (RS)
    0x1F,   // 1F (US)
    0x20,   // 20 SPACE
    0x21,   // 21 !
    0x22,   // 22 "
    0x23,   // 23 #
    0x24,   // 24 $
    0x25,   // 25 %
    0x26,   // 26 &
    0x27,   // 27 '
    0x28,   // 28 (
    0x29,   // 29 )
    0x2A,   // 2A *
    0x2B,   // 2B +
    0x2C,   // 2C ,
    0x2D,   // 2D -
    0x2E,   // 2E .
    0x2F,   // 2F /
    0x30,   // 30 0
    0x31,   // 31 1
    0x32,   // 32 2
    0x33,   // 33 3
    0x34,   // 34 4
    0x35,   // 35 5
    0x36,   // 36 6
    0x37,   // 37 7
    0x38,   // 38 8
    0x39,   // 39 9
    0x3A,   // 3A :
    0x3B,   // 3B ;
    0x3C,   // 3C <
    0x3D,   // 3D =
    0x3E,   // 3E >
    0x3F,   // 3F ?
    0x40,   // 40 @
    0x41,   // 41 A
    0x42,   // 42 B
    0x43,   // 43 C
    0x44,   // 44 D
    0x45,   // 45 E
    0x46,   // 46 F
    0x47,   // 47 G
    0x48,   // 48 H
    0x49,   // 49 I
    0x4A,   // 4A J
    0x4B,   // 4B K
    0x4C,   // 4C L
    0x4D,   // 4D M
    0x4E,   // 4E N
    0x4F,   // 4F O
    0x50,   // 50 P
    0x51,   // 51 Q
    0x52,   // 52 R
    0x53,   // 53 S
    0x54,   // 54 T
    0x55,   // 55 U
    0x56,   // 56 V
    0x57,   // 57 W
    0x58,   // 58 X
    0x59,   // 59 Y
    0x5A,   // 5A Z
    0x5B,   // 5B [
    0x5C,   // 5C \ 
    0x5D,   // 5D ]
    0x5E,   // 5E ^
    0x5F,   // 5F _
    0x60,   // 60 `
    0x41,   // 61 a
    0x42,   // 62 b
    0x43,   // 63 c
    0x44,   // 64 d
    0x45,   // 65 e
    0x46,   // 66 f
    0x47,   // 67 g
    0x48,   // 68 h
    0x49,   // 69 i
    0x4A,   // 6A j
    0x4B,   // 6B k
    0x4C,   // 6C l
    0x4D,   // 6D m
    0x4E,   // 6E n
    0x4F,   // 6F o
    0x50,   // 70 p
    0x51,   // 71 q
    0x52,   // 72 r
    0x53,   // 73 s
    0x54,   // 74 t
    0x55,   // 75 u
    0x56,   // 76 v
    0x57,   // 77 w
    0x58,   // 78 x
    0x59,   // 79 y
    0x5A,   // 7A z
    0x7B,   // 7B {
    0x7C,   // 7C |
    0x7D,   // 7D }
    0x7E,   // 7E ~
    0x7F,   // 7F (DEL)
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

#endif

namespace Thunks
{
	extern "C" __declspec(dllimport) int* __cdecl _errno(void);

	namespace WinXP
	{
		extern "C" __declspec(dllimport) int(__cdecl __crtLCMapStringA)
		(
			_In_ LCID _Locale,
			_In_ DWORD _DwMapFlag,
			_In_count_(_CchSrc) LPCSTR _LpSrcStr,
			_In_ int _CchSrc,
			_Out_opt_cap_(_CchDest) LPSTR _LpDestStr,
			_In_ int _CchDest,
			_In_ int _Code_page,
			_In_ BOOL _BError
		);
	}
}

__declspec(noinline) __inline bool __fastcall IsPointerInMsvcrtDll(const void* p)
{
	if (p == nullptr)
		return false;

	static const void* pBegin = nullptr;
	static const void* pEnd = nullptr;

	if (pBegin == nullptr || pEnd == nullptr)
	{
		MEMORY_BASIC_INFORMATION MemoryBasicInfo;

		if (VirtualQuery(&Thunks::_errno, &MemoryBasicInfo, sizeof(MemoryBasicInfo)) == 0)
		{
			return false;
		}

		pBegin = MemoryBasicInfo.AllocationBase;
		pEnd = (char*)MemoryBasicInfo.AllocationBase + MemoryBasicInfo.RegionSize;
	}

	return pBegin >= p && p < pEnd;
}


static void __cdecl __addlocaleref(__crt_locale_data* ptloci)
{
	InterlockedIncrement(&(ptloci->refcount));
	if (ptloci->lconv_intl_refcount != NULL)
		InterlockedIncrement(ptloci->lconv_intl_refcount);

	if (ptloci->lconv_mon_refcount != NULL)
		InterlockedIncrement(ptloci->lconv_mon_refcount);

	if (ptloci->lconv_num_refcount != NULL)
		InterlockedIncrement(ptloci->lconv_num_refcount);

	if (ptloci->ctype1_refcount != NULL)
		InterlockedIncrement(ptloci->ctype1_refcount);

	for (auto& category : ptloci->lc_category)
	{
		if (category.locale != /*__clocalestr*/NULL
			&& IsPointerInMsvcrtDll(category.locale) == false
			&& category.refcount != NULL)
		{
			InterlockedIncrement(category.refcount);
		}
		if (category.wlocale != NULL
			&& IsPointerInMsvcrtDll(category.wlocale) == false
			&& category.wrefcount != NULL)
		{
			InterlockedIncrement(category.wrefcount);
		}
	}
	InterlockedIncrement(&(ptloci->lc_time_curr->refcount));
}

static void * __cdecl __removelocaleref(__crt_locale_data* ptloci)
{
	if (ptloci != NULL)
	{
		InterlockedDecrement(&(ptloci->refcount));

		if (ptloci->lconv_intl_refcount != NULL)
			InterlockedDecrement(ptloci->lconv_intl_refcount);

		if (ptloci->lconv_mon_refcount != NULL)
			InterlockedDecrement(ptloci->lconv_mon_refcount);

		if (ptloci->lconv_num_refcount != NULL)
			InterlockedDecrement(ptloci->lconv_num_refcount);

		if (ptloci->ctype1_refcount != NULL)
			InterlockedDecrement(ptloci->ctype1_refcount);

		for (auto& category : ptloci->lc_category)
		{
			if (category.locale != /*__clocalestr*/NULL
				&& IsPointerInMsvcrtDll(category.locale) == false
				&& category.refcount != NULL)
			{
				InterlockedDecrement(category.refcount);
			}

			if (category.wlocale != NULL
				&& IsPointerInMsvcrtDll(category.wlocale) == false
				&& category.wrefcount != NULL)
			{
				InterlockedDecrement(category.wrefcount);
			}
		}
		InterlockedDecrement(&(ptloci->lc_time_curr->refcount));
	}
	return ptloci;
}

/*
微软本身还判断了是否是静态字符串，但是看了使用规则。当使用内部字符串时 引用的指针为null_ptr

其他函数都同理，如果未来发现问题。可以加入msvcrt.dll 地址范围检测

*/
static void __free_lconv_mon(lconv* plconv)
{
	if (!plconv)
		return;

	//0xC
	if (plconv->int_curr_symbol && IsPointerInMsvcrtDll(plconv->int_curr_symbol) == false /*!= __lconv_c_data.int_curr_symbol*/)
	{
		free(plconv->int_curr_symbol);
	}

	//0x10
	if (plconv->currency_symbol && IsPointerInMsvcrtDll(plconv->currency_symbol) == false /*!= __lconv_c_data.currency_symbol*/)
	{
		free(plconv->currency_symbol);
	}

	//0x14
	if (plconv->mon_decimal_point && IsPointerInMsvcrtDll(plconv->mon_decimal_point) == false /*!= __lconv_c_data.mon_decimal_point*/)
	{
		free(plconv->mon_decimal_point);
	}

	//0x18
	if (plconv->mon_thousands_sep && IsPointerInMsvcrtDll(plconv->mon_thousands_sep) == false /*!= __lconv_c_data.mon_thousands_sep*/)
	{
		free(plconv->mon_thousands_sep);
	}


	//0x1C
	if (plconv->mon_grouping && IsPointerInMsvcrtDll(plconv->mon_grouping) == false /*!= __lconv_c_data.mon_grouping*/)
	{
		free(plconv->mon_grouping);
	}


	//0x20
	if (plconv->positive_sign && IsPointerInMsvcrtDll(plconv->positive_sign) == false /*!= __lconv_c_data.positive_sign*/)
	{
		free(plconv->positive_sign);
	}

	//0x24
	if (plconv->negative_sign && IsPointerInMsvcrtDll(plconv->negative_sign) == false /*!= __lconv_c_data.negative_sign*/)
	{
		free(plconv->negative_sign);
	}

	if (__LTL_GetOsMinVersion() >= MakeMiniVersion(6, 1))
	{
		//0x38
		if (plconv->lconv_Win7._W_int_curr_symbol && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_int_curr_symbol) == false /*!= __lconv_c_data._W_int_curr_symbol*/)
		{
			free(plconv->lconv_Win7._W_int_curr_symbol);
		}

		//0x3C
		if (plconv->lconv_Win7._W_currency_symbol && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_currency_symbol) == false /*!= __lconv_c_data._W_currency_symbol*/)
		{
			free(plconv->lconv_Win7._W_currency_symbol);
		}

		//0x40
		if (plconv->lconv_Win7._W_mon_decimal_point && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_mon_decimal_point) == false /*!= __lconv_c_data._W_mon_decimal_point*/)
		{
			free(plconv->lconv_Win7._W_mon_decimal_point);
		}

		//0x44
		if (plconv->lconv_Win7._W_mon_thousands_sep && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_mon_thousands_sep) == false /*!= __lconv_c_data._W_mon_thousands_sep*/)
		{
			free(plconv->lconv_Win7._W_mon_thousands_sep);
		}

		//0x48
		if (plconv->lconv_Win7._W_positive_sign && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_positive_sign) == false /*!= __lconv_c_data._W_positive_sign*/)
		{
			free(plconv->lconv_Win7._W_positive_sign);
		}

		//0x4C
		if (plconv->lconv_Win7._W_negative_sign && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_negative_sign) == false /*!= __lconv_c_data._W_negative_sign*/)
		{
			free(plconv->lconv_Win7._W_negative_sign);
		}
	}
}

static void __free_lconv_num(lconv* plconv)
{
	if (!plconv)
		return;

	//0x00
	if (plconv->decimal_point && IsPointerInMsvcrtDll(plconv->decimal_point) == false/* != __lconv_c_data.decimal_point*/)
	{
		free(plconv->decimal_point);
	}

	//0x04
	if (plconv->thousands_sep && IsPointerInMsvcrtDll(plconv->thousands_sep) == false /*!= __lconv_c_data.thousands_sep*/)
	{
		free(plconv->thousands_sep);
	}

	//0x08
	if (plconv->grouping && IsPointerInMsvcrtDll(plconv->grouping) == false /*!= __lconv_c_data.grouping*/)
	{
		free(plconv->grouping);
	}

	if (__LTL_GetOsMinVersion() >= 0x00060001)
	{
		//0x30
		if (plconv->lconv_Win7._W_decimal_point && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_decimal_point) == false/*!= __lconv_c_data._W_decimal_point*/)
		{
			free(plconv->lconv_Win7._W_decimal_point);
		}

		//0x34
		if (plconv->lconv_Win7._W_thousands_sep && IsPointerInMsvcrtDll(plconv->lconv_Win7._W_thousands_sep) == false/*!= __lconv_c_data._W_thousands_sep*/)
		{
			free(plconv->lconv_Win7._W_thousands_sep);
		}
	}
}


static void __free_lc_time(_lc_time_data_msvcrt* lc_time)
{
	if (!lc_time)
		return;

	for (auto i = 0; i != _countof(lc_time->wday_abbr); ++i)
		free(lc_time->wday_abbr[i]);

	for (auto i = 0; i != _countof(lc_time->wday); ++i)
		free(lc_time->wday[i]);

	for (auto i = 0; i != _countof(lc_time->month_abbr); ++i)
		free(lc_time->month_abbr[i]);

	for (auto i = 0; i != _countof(lc_time->month); ++i)
		free(lc_time->month[i]);

	for (auto i = 0; i != _countof(lc_time->ampm); ++i)
		free(lc_time->ampm[i]);


	free(lc_time->ww_sdatefmt);
	free(lc_time->ww_ldatefmt);
	free(lc_time->ww_timefmt);


	if (__LTL_GetOsMinVersion() >= MakeMiniVersion(6, 1))
	{
		//Win7以及更高版本才需要释放

		for (auto i = 0; i != _countof(lc_time->_W_wday_abbr); ++i)
			free(lc_time->_W_wday_abbr[i]);

		for (auto i = 0; i != _countof(lc_time->_W_wday); ++i)
			free(lc_time->_W_wday[i]);

		for (auto i = 0; i != _countof(lc_time->_W_month_abbr); ++i)
			free(lc_time->_W_month_abbr[i]);

		for (auto i = 0; i != _countof(lc_time->_W_month); ++i)
			free(lc_time->_W_month[i]);

		for (auto i = 0; i != _countof(lc_time->_W_ampm); ++i)
			free(lc_time->_W_ampm[i]);

		free(lc_time->_W_ww_sdatefmt);
		free(lc_time->_W_ww_ldatefmt);
		free(lc_time->_W_ww_timefmt);

	}


}

static void __cdecl __freetlocinfo(
	__crt_locale_data* ptloci
)
{
	/*
	* Free up lconv struct
	*/
	if (ptloci->lconv
		&& ptloci->lconv_intl_refcount
		&& *(ptloci->lconv_intl_refcount) == 0
		&& IsPointerInMsvcrtDll(ptloci->lconv) == false /*(ptloci->lconv != &__lconv_c) &&*/
		&& IsPointerInMsvcrtDll(ptloci->lconv_intl_refcount) == false)
	{
		if ((ptloci->lconv_mon_refcount != NULL)
			&&(*(ptloci->lconv_mon_refcount) == 0)
			&& IsPointerInMsvcrtDll(ptloci->lconv_mon_refcount) == false)
		{
			_free_crt(ptloci->lconv_mon_refcount);
			__free_lconv_mon(ptloci->lconv);
		}

		if ((ptloci->lconv_num_refcount != NULL)
			&& (*(ptloci->lconv_num_refcount) == 0)
			&& IsPointerInMsvcrtDll(ptloci->lconv_num_refcount) == false)
		{
			_free_crt(ptloci->lconv_num_refcount);
			__free_lconv_num(ptloci->lconv);
		}

		_free_crt(ptloci->lconv_intl_refcount);
		_free_crt(ptloci->lconv);
	}

	/*
	* Free up ctype tables
	*/
	if (ptloci->ctype1_refcount
		&& (*(ptloci->ctype1_refcount) == 0)
		&& IsPointerInMsvcrtDll(ptloci->ctype1_refcount) == false)
	{
		_free_crt(ptloci->ctype1 - _COFFSET);
		if (auto clmap = (unsigned char*)(ptloci->pclmap - _COFFSET - 1))
		{
#if _CRT_NTDDI_MIN < NTDDI_VISTA 
			if(clmap != __newclmap)
#endif
				_free_crt(clmap);
		}

		if (auto cumap = (unsigned char*)(ptloci->pcumap - _COFFSET - 1))
		{
#if _CRT_NTDDI_MIN < NTDDI_VISTA 
			if (cumap != __newcumap)
#endif
				_free_crt(cumap);
		}

		_free_crt(ptloci->ctype1_refcount);
	}

	/*
	* Free up the __lc_time_data struct
	*/
	if (ptloci->lc_time_curr /*&__lc_time_c*/
		&& ((ptloci->lc_time_curr->refcount) == 0)
		&& IsPointerInMsvcrtDll(ptloci->lc_time_curr) == false)
	{
		__free_lc_time(ptloci->lc_time_curr);
		_free_crt(ptloci->lc_time_curr);
	}

	for (auto & category : ptloci->lc_category)
	{
		if ((category.locale != NULL/*__clocalestr*/) &&
			IsPointerInMsvcrtDll(category.locale) == false &&
			(category.refcount != NULL) &&
			(category.refcount == 0))
		{
			_free_crt(category.refcount);
		}

		if ((category.wlocale != NULL) &&
			IsPointerInMsvcrtDll(category.wlocale) == false &&
			(category.wrefcount != NULL) &&
			(*category.wrefcount == 0))
		{
			_free_crt(category.wrefcount);
		}
	}

	/*
	* Free up the threadlocinfo struct
	*/
	_free_crt(ptloci);
}

#if _CRT_NTDDI_MIN <= NTDDI_WIN7

EXTERN_C _locale_t __cdecl _get_current_locale(void)
{
#if _CRT_NTDDI_MIN < NTDDI_VISTA
	const auto OSVersion = __LTL_GetOsMinVersion();
#if defined(_M_IX86)
	if (OSVersion < 0x00050001)
	{
		errno = ENOSYS;
		return nullptr;
	}
#endif
#endif
	auto retval = (__crt_locale_pointers*)malloc(sizeof(__crt_locale_pointers));
	if (!retval)
	{
		errno = ENOMEM;
		return nullptr;
	}
#if _CRT_NTDDI_MIN < NTDDI_VISTA
	if (OSVersion < 0x00060000)
	{
		_locale_data_msvcrt* locinfo = (_locale_data_msvcrt*)malloc(sizeof(_locale_data_msvcrt));

		if (locinfo == nullptr)
		{
			free(retval);

			errno = ENOMEM;
			return nullptr;
		}


		int __errno = EPERM;

		_lock(_SETLOCALE_LOCK);

		auto locinfo_xp = (_locale_data_msvcrt_winxp*)((byte*)___lc_handle_func() - UFIELD_OFFSET(_locale_data_msvcrt_winxp, lc_handle));

		locinfo->refcount = 1;
		locinfo->_locale_lc_codepage = locinfo_xp->_locale_lc_codepage;
		locinfo->lc_collate_cp = locinfo_xp->lc_collate_cp;
		memcpy(locinfo->lc_handle, locinfo_xp->lc_handle,sizeof(locinfo_xp->lc_handle));

		//没看到 lc_id 跟 lc_category 在使用，暂时就这样设置为 0 吧。
		memset(locinfo->lc_id,0,sizeof(locinfo->lc_id));
		memset(locinfo->lc_category, 0, sizeof(locinfo->lc_category));

		locinfo->lc_clike = locinfo_xp->lc_clike;
		locinfo->_locale_mb_cur_max = locinfo_xp->_locale_mb_cur_max;

		if (locinfo->lc_handle[LC_CTYPE] == 0 || locinfo->ctype1_refcount == nullptr || locinfo->ctype1 == nullptr)
		{
			locinfo->pclmap = __newclmap + 1 + _COFFSET;
			locinfo->pcumap = __newcumap + 1 + _COFFSET;

			__errno = 0;
		}
		else
		{
			auto newclmap = (unsigned char*)_calloc_crt(_COFFSET + _CTABSIZE, sizeof(unsigned char));
			auto newcumap = (unsigned char*)_calloc_crt(_COFFSET + _CTABSIZE, sizeof(unsigned char));

			do
			{
				if (newclmap == nullptr || newcumap == nullptr)
				{
					__errno = ENOMEM;
					break;
				}
				
				unsigned char TmpBuffer[_CTABSIZE];

				for (unsigned i = 0; i < _CTABSIZE - 1; i++)
					TmpBuffer[i] = (unsigned char)i;

				TmpBuffer[_CTABSIZE - 1] = '\0';



				CPINFO cpInfo;                      /* struct for use with GetCPInfo */

				if (GetCPInfo(locinfo->_locale_lc_codepage, &cpInfo) == FALSE)
					break;

				if (cpInfo.MaxCharSize > MB_LEN_MAX)
					break;

				if (cpInfo.MaxCharSize > 1)
				{
					// use the lead byte table to mark off the appropriate bytes
					for (auto cp = (unsigned char*)cpInfo.LeadByte; cp[0] && cp[1]; cp += 2)
					{
						for (int i = cp[0]; i <= cp[1]; i++)
							TmpBuffer[i] = ' ';
					}
				}

				if ((Thunks::WinXP::__crtLCMapStringA)(locinfo->lc_handle[LC_CTYPE],
					LCMAP_LOWERCASE,
					(char*)TmpBuffer + 1,
					_CTABSIZE - 2,
					(char*)newclmap + 2 + _COFFSET,
					_CTABSIZE - 2,
					locinfo->_locale_lc_codepage,
					FALSE) == FALSE)
				{
					break;
				}

				if ((Thunks::WinXP::__crtLCMapStringA)(locinfo->lc_handle[LC_CTYPE],
					LCMAP_UPPERCASE,
					(char*)TmpBuffer + 1,
					_CTABSIZE - 2,
					(char*)newclmap + 2 + _COFFSET,
					_CTABSIZE - 2,
					locinfo->_locale_lc_codepage,
					FALSE) == FALSE)
				{
					break;
				}

				newclmap[_COFFSET] = 0;
				newcumap[_COFFSET] = 0;
				newclmap[_COFFSET + 1] = 0; /* entry for null */
				newcumap[_COFFSET + 1] = 0; /* entry for null */

				if (cpInfo.MaxCharSize > 1)
				{
					for (auto cp = (unsigned char*)cpInfo.LeadByte; cp[0] && cp[1]; cp += 2)
					{
						for (int i = cp[0]; i <= cp[1]; i++)
						{
							//newctype1[_COFFSET + i + 1] = _LEADBYTE;
							newclmap[_COFFSET + i + 1] = static_cast<unsigned char>(i);
							newcumap[_COFFSET + i + 1] = static_cast<unsigned char>(i);
						}
					}
				}

				/* copy last-1 _COFFSET unsigned short to front
				* note -1, we don't really want to copy 0xff
				*/
				//memcpy(newctype1,newctype1+_CTABSIZE-1,_COFFSET*sizeof(unsigned short));
				memcpy(newclmap, newclmap + _CTABSIZE - 1, _COFFSET * sizeof(char));
				memcpy(newcumap, newcumap + _CTABSIZE - 1, _COFFSET * sizeof(char));

				locinfo->pclmap = newclmap + 1 + _COFFSET;
				locinfo->pcumap = newcumap + 1 + _COFFSET;

				newclmap = nullptr;
				newcumap = nullptr;
				__errno = 0;
			} while (false);

			if (newcumap)
				free(newcumap);

			if (newclmap)
				free(newclmap);
		}

		if (__errno == 0)
		{
			if (locinfo_xp->lconv_intl == nullptr)
			{
				locinfo->lconv = locinfo_xp->lconv;
				locinfo->lconv_intl_refcount = nullptr;
				locinfo->lconv_num_refcount = nullptr;
				locinfo->lconv_mon_refcount = nullptr;
			}
			else
			{
				locinfo->lconv = locinfo_xp->lconv_intl;
				if (locinfo->lconv_intl_refcount = locinfo_xp->lconv_intl_refcount)
				{
					InterlockedIncrement(locinfo->lconv_intl_refcount);
				}

				if (locinfo->lconv_num_refcount = locinfo_xp->lconv_num_refcount)
				{
					InterlockedIncrement(locinfo->lconv_num_refcount);
				}

				if (locinfo->lconv_mon_refcount = locinfo_xp->lconv_mon_refcount)
				{
					InterlockedIncrement(locinfo->lconv_mon_refcount);
				}
			}

			if (locinfo->ctype1_refcount = locinfo_xp->ctype1_refcount)
			{
				InterlockedIncrement(locinfo->ctype1_refcount);
			}

			locinfo->ctype1 = locinfo_xp->ctype1;
			locinfo->_locale_pctype = locinfo_xp->_locale_pctype;



			locinfo->lc_time_curr = locinfo_xp->lc_time_curr;
			InterlockedIncrement(&locinfo->lc_time_curr->refcount);
		}

		_unlock(_SETLOCALE_LOCK);
			
		if (__errno)
		{
			free(locinfo);
			free(retval);

			errno = __errno;
			return nullptr;
		}
		retval->locinfo = locinfo;

		_lock(_MB_CP_LOCK);

#if defined(_X86_)
		auto mbcinfo = (_multibyte_data_msvcrt*)(_mbctype - 0x18);
#elif defined(_AMD64_)
		auto mbcinfo = (_multibyte_data_msvcrt*)(_mbctype - 0x1C);
#else
#error 你编译条件错误了吧？
#endif
		InterlockedIncrement(&(mbcinfo->refcount));

		_unlock(_MB_CP_LOCK);

		retval->mbcinfo = mbcinfo;
		
		return retval;
	}
#endif

	/*触发
	__updatetlocinfo();
	__updatetmbcinfo();*/
	_getmbcp();

	auto ptd = (_ptd_msvcrt_win6_shared*)__acrt_getptd();

	retval->locinfo = ptd->_locale_info;
	retval->mbcinfo = ptd->_multibyte_info;

	//锁定区域
	_lock(_SETLOCALE_LOCK);

	__addlocaleref(retval->locinfo);

	//解锁区域
	_unlock(_SETLOCALE_LOCK);


	//锁定multibyte code page
	_lock(_MB_CP_LOCK);


	InterlockedIncrement(&(retval->mbcinfo->refcount));

	//解锁multibyte code page
	_unlock(_MB_CP_LOCK);

	return retval;
}

_LTL_DLL_EXPORT_DOWNLEVEL(_get_current_locale);

#endif

static __forceinline char * __cdecl common_setlocale(
	int _category,
	const char *_locale
)
{
	return setlocale(_category, _locale);
}

static __forceinline wchar_t * __cdecl common_setlocale(
	int _category,
	const wchar_t *_locale
)
{
	return _wsetlocale(_category, _locale);
}

static __forceinline char* __cdecl StringDup(
	const char* str
	)
{
	return _strdup(str);
}

static __forceinline wchar_t* __cdecl StringDup(
	const wchar_t* str
)
{
	return _wcsdup(str);
}

/*

msvcrt并不支持创建区域，不过我们可以备份当前区域信息，再利用setlocale覆盖当前线程，最后还原来实现原有函数行为
*/

template<class TCHAR>
static _locale_t __cdecl common_create_locale(
	int _category,
	const TCHAR *_locale
)
{
	/* Validate input */
	if ((_category < LC_MIN) || (_category > LC_MAX) || _locale == NULL)
	{
		errno = EINVAL;

		return NULL;
	}

#if _CRT_NTDDI_MIN < NTDDI_VISTA
	const auto OSVersion = __LTL_GetOsMinVersion();
#if defined(_M_IX86)
	if (OSVersion < 0x00050001)
	{
		errno = ENOSYS;
		return nullptr;
	}
#endif

	if (OSVersion < 0x00060000)
	{
		_locale_t plocale = nullptr;
		int __errno = 0;

		_lock(_SETLOCALE_LOCK);
		_lock(_MB_CP_LOCK);

		TCHAR* locale_backup[6] = {};

		do
		{
			if (_category == LC_ALL)
			{
				bool Success = true;

				for (int i = 1; i <= LC_MAX; ++i)
				{
					auto old = StringDup(common_setlocale(i, (TCHAR*)nullptr));
					if (!old)
					{
						Success = false;
						break;
					}
					locale_backup[i] = old;
				}

				if (!Success)
					break;
			}
			else
			{
				auto old = StringDup(common_setlocale(_category, (TCHAR*)nullptr));
				if (!old)
					break;

				locale_backup[_category] = old;
			}

			if (common_setlocale(_category, _locale) == nullptr)
			{
				__errno = errno;
				break;
			}

			plocale = _get_current_locale();
			if(!plocale)
				__errno = errno;

			//恢复上下文
			for (int i = 1; i <= LC_MAX; ++i)
			{
				if (auto locale = locale_backup[i])
				{
					common_setlocale(i, locale);
				}
			}

		} while (false);

		for (auto& locale : locale_backup)
		{
			if (locale)
				free(locale);
		}

		_unlock(_MB_CP_LOCK);
		_unlock(_SETLOCALE_LOCK);
		

		errno = __errno;
		return plocale;
	}
#endif

	_locale_t plocaleOld = _get_current_locale();
	if (!plocaleOld)
	{
		return nullptr;
	}


	_locale_t plocale = nullptr;

	auto ptd = (_ptd_msvcrt_win6_shared*)__acrt_getptd();

	
	auto _own_locale = ptd->_own_locale;

	//启用线程locale信息
	ptd->_own_locale |= _PER_THREAD_LOCALE_BIT;


	if (common_setlocale(_category, _locale)!=nullptr && _setmbcp(ptd->_locale_info->_locale_lc_codepage)==0)
	{
		//设置区域信息成功后，获取新的区域信息
		plocale = _get_current_locale();
	}

	//恢复线程配置状态
	ptd->_own_locale = _own_locale;

	//恢复以前的区域信息
	if (plocaleOld->locinfo != ptd->_locale_info)
	{
		std::swap(plocaleOld->locinfo, ptd->_locale_info);
	}

	if (plocaleOld->mbcinfo != ptd->_multibyte_info)
	{
		std::swap(plocaleOld->mbcinfo, ptd->_multibyte_info);
	}

	//释放不需要的区域
	_free_locale(plocaleOld);


	return plocale;
}

#if _CRT_NTDDI_MIN <= NTDDI_WIN7

EXTERN_C _locale_t __cdecl _create_locale(
	_In_   int         _Category,
	_In_z_ char const* _Locale
)
{
	return common_create_locale(_Category, _Locale);
}

_LTL_DLL_EXPORT_DOWNLEVEL(_create_locale);

#endif


EXTERN_C _locale_t __cdecl _wcreate_locale(
	_In_   int            _category,
	_In_z_ wchar_t const* _locale
)
{
#if _CRT_NTDDI_MIN <= NTDDI_WIN7
	return common_create_locale(_category, _locale);
#else
	//Windows 8的msvcrt.dll仅存在 _create_locale
	_locale_t locale = nullptr;
	char* _Locale_ANSI = nullptr;

	if (_locale)
	{
		size_t _PtNumOfCharConverted = 0;

		auto __errno = wcstombs_s(&_PtNumOfCharConverted, nullptr, 0, _locale, _CRT_INT_MAX);
		if (__errno)
		{
			if(__errno == EINVAL || __errno == ERANGE)
				_invoke_watson(nullptr, nullptr, nullptr, 0, 0);

			return nullptr;
		}

		if (_PtNumOfCharConverted == 0)
			return nullptr;

		_Locale_ANSI = (char*)_malloca(_PtNumOfCharConverted);

		if (!_Locale_ANSI)
			return nullptr;


		__errno = wcstombs_s(nullptr, _Locale_ANSI, _PtNumOfCharConverted, _locale, _CRT_SIZE_MAX);

		if (__errno)
		{
			if (__errno == EINVAL || __errno == ERANGE)
				_invoke_watson(nullptr, nullptr, nullptr, 0, 0);

			goto __End;
		}
	}

	locale = _create_locale(_category, _Locale_ANSI);

__End:

	if (_Locale_ANSI)
		_freea(_Locale_ANSI);

	return locale;
#endif
}

#if _CRT_NTDDI_MIN >= NTDDI_VISTA

//WinXP不支持此接口
EXTERN_C int __cdecl _configthreadlocale(int i)
{
	/*
	* ownlocale flag struct:
	* bits: 000000000000000000000000000000P1
	* P is set when _ENABLE_PER_THREAD_LOCALE is called for this thread
	* or _ENABLE_PER_THREAD_LOCALE_NEW was set when this thread was created.
	*
	* __globallocalestatus structure:
	* bits: 11111111111111111111111111111N1G
	* G is set if _ENABLE_PER_THREAD_LOCALE_GLOBAL is set.
	* G is 0 if _ENABLE_PER_THREAD_LOCALE_GLOBAL is not set.
	* N is set if _ENABLE_PER_THREAD_LOCALE_NEW is set.
	* N is 0 if _ENABLE_PER_THREAD_LOCALE_NEW is not set.
	*/
	auto ptd = (_ptd_msvcrt_win6_shared*)__acrt_getptd();

	auto& _own_locale = ptd->_own_locale;

	int retval = (_own_locale & _PER_THREAD_LOCALE_BIT) == 0 ? _DISABLE_PER_THREAD_LOCALE : _ENABLE_PER_THREAD_LOCALE;

	switch (i)
	{
	case _ENABLE_PER_THREAD_LOCALE:
		_own_locale = _own_locale | _PER_THREAD_LOCALE_BIT;
		break;

	case _DISABLE_PER_THREAD_LOCALE:
		_own_locale = _own_locale & ~_PER_THREAD_LOCALE_BIT;
		break;

	case 0:
		break;

		/* used only during dll startup for linkopt */
	case -1:
		//__globallocalestatus = -1;

		//msvcrt此变量不可访问，如果调用 -1 ，则将程序崩溃处理。
		abort();

		break;

	default:
		_VALIDATE_RETURN(("Invalid parameter for _configthreadlocale", 0), EINVAL, -1);
		break;
	}

	return retval;

}

#endif

#if _CRT_NTDDI_MIN <= NTDDI_WIN7

EXTERN_C void __cdecl _free_locale(
	_In_opt_ _locale_t plocinfo
)
{
	if (plocinfo != NULL)
	{
		_lock(_MB_CP_LOCK);
		__try
		{
			if (plocinfo->mbcinfo != NULL &&
				InterlockedDecrement(&(plocinfo->mbcinfo->refcount)) == 0
				&& IsPointerInMsvcrtDll(plocinfo->mbcinfo) == false
				/*&& plocinfo->mbcinfo != &__initialmbcinfo*/)
			{
				_free_crt(plocinfo->mbcinfo);
			}
		}
		__finally
		{
			_unlock(_MB_CP_LOCK);
		}
		if (plocinfo->locinfo != NULL)
		{
			/*
			* this portion has to be in locale lock as there may be case when
			* not this thread but some other thread is still holding to this
			* locale and is also trying to free this locale. In this case
			* we may end up leaking memory.
			*/
			_lock(_SETLOCALE_LOCK);
			__try
			{
				__removelocaleref(plocinfo->locinfo);
				if ((plocinfo->locinfo != NULL) &&
					(plocinfo->locinfo->refcount == 0)
					&& IsPointerInMsvcrtDll(plocinfo->locinfo) == false
					/*&& (plocinfo->locinfo != &__initiallocinfo)*/)
					__freetlocinfo(plocinfo->locinfo);
			}
			__finally
			{
				_unlock(_SETLOCALE_LOCK);
			}
		}
		/*
		* set plocinfo structure to zero. This will reduce the chance of
		* using plocinfo after it is being freed.
		*/
		plocinfo->locinfo = (decltype(plocinfo->locinfo))(((char *)NULL) + 0xbaadf00d);
		plocinfo->mbcinfo = (decltype(plocinfo->mbcinfo))(((char *)NULL) + 0xbaadf00d);
		_free_crt(plocinfo);
	}
}

_LTL_DLL_EXPORT_DOWNLEVEL(_free_locale);

#endif

EXTERN_C int __cdecl ___mb_cur_max_l_func(_locale_t locale)
{
	return locale == nullptr
		? ___mb_cur_max_func()
		: locale->locinfo->_locale_mb_cur_max;

}
