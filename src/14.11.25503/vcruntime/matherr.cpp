//
// matherr.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This object provides a default definition of the _matherr function.  This
// definition may be substituted by the client by redefining it.  The flag
// __scrt_default_matherr has a value of 1 if this object is used (it has a
// value of 0 otherwise, via an alternate definition.
//
#include <vcruntime.h>
#include <math.h>



extern "C" int __scrt_default_matherr = 1;



extern "C" int __CRTDECL _matherr(_exception*)
{
    return 0;
}
