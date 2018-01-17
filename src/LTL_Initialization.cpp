//初始化内部对象

#include <corecrt_internal.h>

static void __cdecl __LTL_Unitialization() throw()
{
	//__acrt_uninitialize_locks(false);


	__acrt_uninitialize_winapi_thunks(false);

}

EXTERN_C int __cdecl __LTL_Initialization() throw()
{
	//先初始化高级指令支持
	__isa_available_init();

	//需要支持XP时才需要初始化，Vista以上系统直接使用新接口
	__acrt_initialize_winapi_thunks();
	
	//__acrt_initialize_locks();


	atexit(__LTL_Unitialization);

	return 0;
}


_CRTALLOC(".CRT$XIC") static _PIFV ___Initialization =  __LTL_Initialization;