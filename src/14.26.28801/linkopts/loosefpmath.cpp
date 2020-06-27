//
// loosefpmath.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that sets the DAZ and FTZ bits for SSE2 architectures.
//
#include <float.h>



extern "C" void __CRTDECL _initialize_denormal_control()
{
    _controlfp_s(nullptr, _DN_FLUSH, _MCW_DN);
}
