


namespace downlevel
{

#undef __mb_cur_max
    EXTERN_C __declspec(dllimport) extern int __mb_cur_max;

    EXTERN_C int __cdecl ___mb_cur_max_func(void)
    {
        return __mb_cur_max;
    }

    _LCRT_DEFINE_IAT_SYMBOL(___mb_cur_max_func);


    EXTERN_C __declspec(dllimport) extern unsigned int __lc_codepage;

    EXTERN_C unsigned int __cdecl ___lc_codepage_func()
    {
        return __lc_codepage;
    }

    _LCRT_DEFINE_IAT_SYMBOL(___lc_codepage_func);


    EXTERN_C __declspec(dllimport) extern unsigned int __lc_handle[];

    EXTERN_C unsigned int* __cdecl ___lc_handle_func(void)
    {
        __try_call_msvcrt_fun(___lc_handle_func);

        return __lc_handle;
    }

    _LCRT_DEFINE_IAT_SYMBOL(___lc_handle_func);

}