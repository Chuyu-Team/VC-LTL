//
// app_appinit.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This object is part of the import library for the Windows Store app CRT.  It
// coordinates the call to RoInitialize with vccorlib, to ensure that either
// vccorlib or the CRT (but not both) call RoInitialize.  This object is linked
// into the client app if and only if it does not use vccorlib.
//
#include <roapi.h>



extern "C" int __crtWinrtInitType = RO_INIT_MULTITHREADED;



#pragma detect_mismatch("vccorlib_lib_should_be_specified_before_msvcrt_lib_to_linker", "0")
