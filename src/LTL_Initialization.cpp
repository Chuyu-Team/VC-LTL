//初始化内部对象

#include <corecrt_internal.h>


//#pragma section(".CRT$XIU", long, read)
//#pragma section(".CRT$XPU", long, read)

static void __cdecl __LTL_Unitialization() throw()
{
	__acrt_uninitialize_locks(false);

	//__acrt_uninitialize_winapi_thunks(false);
}

EXTERN_C int __cdecl __LTL_Initialization() throw()
{

	//__acrt_initialize_winapi_thunks();

	
	__acrt_initialize_locks();


	atexit(__LTL_Unitialization);

	return 0;
}


//#pragma init_seg(".CRT$XIU")
//static auto ____LTL_Initialization = __LTL_Initialization();
_CRTALLOC(".CRT$XIC") static _PIFV ___Initialization =  __LTL_Initialization;
//_CRTALLOC(".CRT$XPU") _PVFV _____LTL_Unitialization[] = { __LTL_Unitialization };


//#pragma comment(linker, "/merge:.CRT=.rdata")