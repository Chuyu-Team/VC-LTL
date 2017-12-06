//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"

int __cdecl main(Platform::Array<Platform::String^>^ arg); //User defined main function

#if defined(_GUARDED_CRT)
// define entry point wrappers for the main above

extern "C" void _guard_init();

int _guardES_main(Platform::Array<Platform::String^>^)
{
    // initialize the runtime
    _guard_init();

    return 0;
}


int _guardEE_main(Platform::Array<Platform::String^>^)
{
    // argv and env should stay around even after main returns
    return 0;
}
#endif


namespace Platform {
    namespace Details {

        wchar_t** GetCmdArguments(_Out_ int* argc);

    } }

int __cdecl _main()
{
    int argc = 0;
    wchar_t **argv = Platform::Details::GetCmdArguments(&argc);
    auto arg = ref new Platform::Array<Platform::String^>(argc);

    for(int i = 0; i < argc; i++)
    {
        arg->set(i, ref new Platform::String(argv[i]));
    }

    int ret = 0;
    try {
        ret = main(arg);
    }
    catch( ::Platform::Exception^ e)
    {
        ::Platform::Details::ReportUnhandledError(e);
        throw e;
    }

    return ret;
}
