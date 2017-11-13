/*


用于实现各种 _l 后缀的函数



*/


#include <corecrt_internal.h>
#include <locale.h>
#include <corecrt_wstdlib.h>
#include <time.h>
#include <mbctype.h>


class LocaleAutoUpdate
{
public:
	__acrt_ptd* ptd;
	__crt_locale_pointers Backup;
	int _own_locale;

	LocaleAutoUpdate(_locale_t _Locale)
		:ptd(nullptr)
	{
		if (_Locale)
		{
			ptd = __acrt_getptd();

			_own_locale = ptd->_own_locale;

			//强制启用线程区域信息
			ptd->_own_locale |= _PER_THREAD_LOCALE_BIT;

			//备份当前区域信息
			Backup.locinfo = ptd->_locale_info;
			Backup.mbcinfo = ptd->_multibyte_info;

			//替换当前区域信息
			ptd->_locale_info = _Locale->locinfo;
			ptd->_multibyte_info = _Locale->mbcinfo;
		}
	}

	~LocaleAutoUpdate()
	{
		if (ptd)
		{
			//恢复当前区域信息
			ptd->_locale_info = Backup.locinfo;
			ptd->_multibyte_info = Backup.mbcinfo;

			//回滚区域线程状态
			ptd->_own_locale = _own_locale;
		}
	}
};

EXTERN_C double __cdecl _wcstod_l(
	_In_z_                   wchar_t const* _String,
	_Out_opt_ _Deref_post_z_ wchar_t**      _EndPtr,
	_In_opt_                 _locale_t      _Locale
	)
{
	LocaleAutoUpdate Tmp(_Locale);

	return wcstod(_String, _EndPtr);
}

EXTERN_C float __cdecl _wcstof_l(
	_In_z_                   wchar_t const* _String,
	_Out_opt_ _Deref_post_z_ wchar_t**      _EndPtr,
	_In_opt_                 _locale_t      _Locale
	)
{
	return _wcstod_l(_String, _EndPtr, _Locale);
}

EXTERN_C size_t __cdecl _strftime_l(
    _Out_writes_z_(_MaxSize)      char*            _Buffer,
    _In_                          size_t           _MaxSize,
    _In_z_ _Printf_format_string_ char const*      _Format,
    _In_                          struct tm const* _Tm,
    _In_opt_                      _locale_t        _Locale
    )
{
	LocaleAutoUpdate Tmp(_Locale);

	return strftime(_Buffer, _MaxSize, _Format, _Tm);
}