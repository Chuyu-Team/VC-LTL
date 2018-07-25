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


static void __cdecl __addlocaleref(__crt_locale_data* ptloci)
{
	int category;
	InterlockedIncrement(&(ptloci->refcount));
	if (ptloci->lconv_intl_refcount != NULL)
		InterlockedIncrement(ptloci->lconv_intl_refcount);

	if (ptloci->lconv_mon_refcount != NULL)
		InterlockedIncrement(ptloci->lconv_mon_refcount);

	if (ptloci->lconv_num_refcount != NULL)
		InterlockedIncrement(ptloci->lconv_num_refcount);

	if (ptloci->ctype1_refcount != NULL)
		InterlockedIncrement(ptloci->ctype1_refcount);

	for (category = LC_MIN; category <= LC_MAX; ++category)
	{
		if (ptloci->lc_category[category].locale != /*__clocalestr*/NULL &&
			ptloci->lc_category[category].refcount != NULL)
		{
			InterlockedIncrement(ptloci->lc_category[category].refcount);
		}
		if (ptloci->lc_category[category].wlocale != NULL &&
			ptloci->lc_category[category].wrefcount != NULL)
		{
			InterlockedIncrement(ptloci->lc_category[category].wrefcount);
		}
	}
	InterlockedIncrement(&(ptloci->lc_time_curr->refcount));
}

static void * __cdecl __removelocaleref(__crt_locale_data* ptloci)
{
	int category;
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

		for (category = LC_MIN; category <= LC_MAX; ++category) {
			if (ptloci->lc_category[category].locale != /*__clocalestr*/NULL &&
				ptloci->lc_category[category].refcount != NULL)
				InterlockedDecrement(ptloci->lc_category[category].refcount);
			if (ptloci->lc_category[category].wlocale != NULL &&
				ptloci->lc_category[category].wrefcount != NULL)
				InterlockedDecrement(ptloci->lc_category[category].wrefcount);
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
	if (plconv->int_curr_symbol /*!= __lconv_c_data.int_curr_symbol*/)
	{
		free(plconv->int_curr_symbol);
	}

	//0x10
	if (plconv->currency_symbol /*!= __lconv_c_data.currency_symbol*/)
	{
		free(plconv->currency_symbol);
	}

	//0x14
	if (plconv->mon_decimal_point /*!= __lconv_c_data.mon_decimal_point*/)
	{
		free(plconv->mon_decimal_point);
	}

	//0x18
	if (plconv->mon_thousands_sep /*!= __lconv_c_data.mon_thousands_sep*/)
	{
		free(plconv->mon_thousands_sep);
	}


	//0x1C
	if (plconv->mon_grouping /*!= __lconv_c_data.mon_grouping*/)
	{
		free(plconv->mon_grouping);
	}


	//0x20
	if (plconv->positive_sign /*!= __lconv_c_data.positive_sign*/)
	{
		free(plconv->positive_sign);
	}

	//0x24
	if (plconv->negative_sign /*!= __lconv_c_data.negative_sign*/)
	{
		free(plconv->negative_sign);
	}

	if (__LTL_GetOsMinVersion() >= MakeMiniVersion(6, 1))
	{
		//0x38
		if (plconv->lconv_Win7._W_int_curr_symbol /*!= __lconv_c_data._W_int_curr_symbol*/)
		{
			free(plconv->lconv_Win7._W_int_curr_symbol);
		}

		//0x3C
		if (plconv->lconv_Win7._W_currency_symbol /*!= __lconv_c_data._W_currency_symbol*/)
		{
			free(plconv->lconv_Win7._W_currency_symbol);
		}

		//0x40
		if (plconv->lconv_Win7._W_mon_decimal_point /*!= __lconv_c_data._W_mon_decimal_point*/)
		{
			free(plconv->lconv_Win7._W_mon_decimal_point);
		}

		//0x44
		if (plconv->lconv_Win7._W_mon_thousands_sep /*!= __lconv_c_data._W_mon_thousands_sep*/)
		{
			free(plconv->lconv_Win7._W_mon_thousands_sep);
		}

		//0x48
		if (plconv->lconv_Win7._W_positive_sign /*!= __lconv_c_data._W_positive_sign*/)
		{
			free(plconv->lconv_Win7._W_positive_sign);
		}

		//0x4C
		if (plconv->lconv_Win7._W_negative_sign /*!= __lconv_c_data._W_negative_sign*/)
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
	if (plconv->decimal_point/* != __lconv_c_data.decimal_point*/)
	{
		free(plconv->decimal_point);
	}

	//0x04
	if (plconv->thousands_sep /*!= __lconv_c_data.thousands_sep*/)
	{
		free(plconv->thousands_sep);
	}

	//0x08
	if (plconv->grouping /*!= __lconv_c_data.grouping*/)
	{
		free(plconv->grouping);
	}

	if (__LTL_GetOsMinVersion() >= 0x00060001)
	{
		//0x30
		if (plconv->lconv_Win7._W_decimal_point /*!= __lconv_c_data._W_decimal_point*/)
		{
			free(plconv->lconv_Win7._W_decimal_point);
		}

		//0x34
		if (plconv->lconv_Win7._W_thousands_sep /*!= __lconv_c_data._W_thousands_sep*/)
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
	int category;
	/*
	* Free up lconv struct
	*/
	if ((ptloci->lconv != NULL) &&
		/*(ptloci->lconv != &__lconv_c) &&*/
		((ptloci->lconv_intl_refcount != NULL) &&
		(*(ptloci->lconv_intl_refcount) == 0)))
	{
		if ((ptloci->lconv_mon_refcount != NULL) &&
			(*(ptloci->lconv_mon_refcount) == 0))
		{
			_free_crt(ptloci->lconv_mon_refcount);
			__free_lconv_mon(ptloci->lconv);
		}

		if ((ptloci->lconv_num_refcount != NULL) &&
			(*(ptloci->lconv_num_refcount) == 0))
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
	if ((ptloci->ctype1_refcount != NULL) &&
		(*(ptloci->ctype1_refcount) == 0))
	{
		_free_crt(ptloci->ctype1 - _COFFSET);
		_free_crt((char *)(ptloci->pclmap - _COFFSET - 1));
		_free_crt((char *)(ptloci->pcumap - _COFFSET - 1));
		_free_crt(ptloci->ctype1_refcount);
	}

	/*
	* Free up the __lc_time_data struct
	*/
	if (ptloci->lc_time_curr != NULL/*&__lc_time_c*/ &&
		((ptloci->lc_time_curr->refcount) == 0))
	{
		__free_lc_time(ptloci->lc_time_curr);
		_free_crt(ptloci->lc_time_curr);
	}

	for (category = LC_MIN; category <= LC_MAX; ++category) {
		if ((ptloci->lc_category[category].locale != NULL/*__clocalestr*/) &&
			(ptloci->lc_category[category].refcount != NULL) &&
			(*ptloci->lc_category[category].refcount == 0))
		{
			_free_crt(ptloci->lc_category[category].refcount);
		}
		_ASSERTE(((ptloci->lc_category[category].wlocale != NULL) && (ptloci->lc_category[category].wrefcount != NULL)) ||
			((ptloci->lc_category[category].wlocale == NULL) && (ptloci->lc_category[category].wrefcount == NULL)));
		if ((ptloci->lc_category[category].wlocale != NULL) &&
			(ptloci->lc_category[category].wrefcount != NULL) &&
			(*ptloci->lc_category[category].wrefcount == 0))
		{
			_free_crt(ptloci->lc_category[category].wrefcount);
		}
	}

	/*
	* Free up the threadlocinfo struct
	*/
	_free_crt(ptloci);
}

#if _CRT_NTDDI_MIN >= NTDDI_VISTA && _CRT_NTDDI_MIN <= NTDDI_WIN7

//WinXP不支持此接口
EXTERN_C _locale_t __cdecl _get_current_locale_downlevel(void)
{
	auto retval = (__crt_locale_pointers*)calloc(sizeof(__crt_locale_pointers), 1);
	if (!retval)
	{
		errno = ENOMEM;
		return nullptr;
	}

	/*触发
	__updatetlocinfo();
	__updatetmbcinfo();*/
	_getmbcp();

	auto ptd = __acrt_getptd();

	retval->locinfo = ptd->VistaOrLater_msvcrt._locale_info;
	retval->mbcinfo = ptd->VistaOrLater_msvcrt._multibyte_info;

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

_LCRT_DEFINE_IAT_SYMBOL(_get_current_locale_downlevel);

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

	_locale_t plocaleOld = _get_current_locale();
	if (!plocaleOld)
	{
		return nullptr;
	}


	_locale_t plocale = nullptr;

	auto ptd = __acrt_getptd();

	
	auto _own_locale = ptd->VistaOrLater_msvcrt._own_locale;

	//启用线程locale信息
	ptd->VistaOrLater_msvcrt._own_locale |= _PER_THREAD_LOCALE_BIT;


	if (common_setlocale(_category, _locale)!=nullptr && _setmbcp(ptd->VistaOrLater_msvcrt._locale_info->_locale_lc_codepage)==0)
	{
		//设置区域信息成功后，获取新的区域信息
		plocale = _get_current_locale();
	}

	//恢复线程配置状态
	ptd->VistaOrLater_msvcrt._own_locale = _own_locale;

	//恢复以前的区域信息
	if (plocaleOld->locinfo != ptd->VistaOrLater_msvcrt._locale_info)
	{
		std::swap(plocaleOld->locinfo, ptd->VistaOrLater_msvcrt._locale_info);
	}

	if (plocaleOld->mbcinfo != ptd->VistaOrLater_msvcrt._multibyte_info)
	{
		std::swap(plocaleOld->mbcinfo, ptd->VistaOrLater_msvcrt._multibyte_info);
	}

	//释放不需要的区域
	_free_locale(plocaleOld);


	return plocale;
}

#if _CRT_NTDDI_MIN >= NTDDI_VISTA && _CRT_NTDDI_MIN <= NTDDI_WIN7

//WinXP不支持此接口
EXTERN_C _locale_t __cdecl _create_locale_downlevel(
	_In_   int         _Category,
	_In_z_ char const* _Locale
)
{
	return common_create_locale(_Category, _Locale);
}

_LCRT_DEFINE_IAT_SYMBOL(_create_locale_downlevel);

#endif

#if _CRT_NTDDI_MIN >= NTDDI_VISTA

//WinXP不支持此接口
EXTERN_C _locale_t __cdecl _wcreate_locale_downlevel(
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

_LCRT_DEFINE_IAT_SYMBOL(_wcreate_locale_downlevel);

#endif

#if _CRT_NTDDI_MIN >= NTDDI_VISTA

//WinXP不支持此接口
EXTERN_C int __cdecl _configthreadlocale_downlevel(int i)
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
	auto ptd = __acrt_getptd();

	auto& _own_locale = ptd->VistaOrLater_msvcrt._own_locale;

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

_LCRT_DEFINE_IAT_SYMBOL(_configthreadlocale_downlevel);

#endif

#if _CRT_NTDDI_MIN <= NTDDI_WIN7

EXTERN_C void __cdecl _free_locale_downlevel(
	_In_opt_ _locale_t plocinfo
)
{
	if (plocinfo != NULL)
	{
		_lock(_MB_CP_LOCK);
		__try
		{
			if (plocinfo->mbcinfo != NULL &&
				InterlockedDecrement(&(plocinfo->mbcinfo->refcount)) == 0 /*&&
				plocinfo->mbcinfo != &__initialmbcinfo*/)
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
					(plocinfo->locinfo->refcount == 0) /*&&
					(plocinfo->locinfo != &__initiallocinfo)*/)
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

_LCRT_DEFINE_IAT_SYMBOL(_free_locale_downlevel);

#endif

EXTERN_C int __cdecl ___mb_cur_max_l_func_downlevel(_locale_t locale)
{
	return locale == nullptr
		? ___mb_cur_max_func()
		: locale->locinfo->_locale_mb_cur_max;

}

_LCRT_DEFINE_IAT_SYMBOL(___mb_cur_max_l_func_downlevel);