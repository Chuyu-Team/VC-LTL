#include "msvcrt_IAT.h"


typedef enum _crt_app_type
{
	_crt_unknown_app,
	_crt_console_app,
	_crt_gui_app
} _crt_app_type;


extern "C" __declspec(dllimport) void __cdecl __set_app_type(
	_In_ _crt_app_type _Type
	);

static _crt_app_type __acrt_app_type = _crt_unknown_app;

extern "C" void __cdecl _set_app_type_downlevel(
	_In_ _crt_app_type new_app_type
	)
{
	//֪ͨmsvcrt.dll
	__set_app_type(new_app_type);

	__acrt_app_type = new_app_type;
}

_LCRT_DEFINE_IAT_SYMBOL(_set_app_type_downlevel);


extern "C" _crt_app_type __cdecl _query_app_type_downlevel()
{
	return __acrt_app_type;
}

_LCRT_DEFINE_IAT_SYMBOL(_query_app_type_downlevel);
