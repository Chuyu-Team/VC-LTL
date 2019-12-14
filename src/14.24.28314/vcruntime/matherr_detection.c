//
// matherr_detection.c
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This source file allows us to detect the presence of a user-defined _matherr
// function.  If the user does not define a _matherr function, the definition
// of _matherr in matherr.cpp will be linked in, which will also define the
// __scrt_default_matherr flag to a value other than zero.  If the user does
// define a _matherr function, then matherr.cpp will not be linked in, so that
// other definition of __scrt_default_matherr will not be present, and the local
// definition will be used instead.  This has a value of zero.
//
// This relies on a feature of the C compiler known as "communal variables."
// This does not work in C++, and the linker's alternatename features is not
// sufficient here.
//
#pragma warning(disable: 4132) // const object should be initialized
int const __scrt_default_matherr;

int __scrt_is_user_matherr_present()
{
    return __scrt_default_matherr == 0;
}
