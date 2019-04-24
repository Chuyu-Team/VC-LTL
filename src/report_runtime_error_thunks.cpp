#include <corecrt_startup.h>
#include "msvcrt_IAT.h"



extern "C" __declspec(dllimport) void __cdecl __set_app_type(
	_In_ _crt_app_type _Type
	);

static _crt_app_type __acrt_app_type = _crt_unknown_app;

extern "C" void __cdecl _set_app_type(
	_In_ _crt_app_type new_app_type
	)
{
	//֪ͨmsvcrt.dll
	__set_app_type(new_app_type);

	__acrt_app_type = new_app_type;
}


extern "C" _crt_app_type __cdecl _query_app_type()
{
	return __acrt_app_type;
}
