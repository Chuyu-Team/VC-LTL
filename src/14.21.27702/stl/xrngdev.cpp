// xrngdev: random device for TR1 random number generators
#include <stdexcept> // for out_of_range

//  #include <random>
_STD_BEGIN
_CRTIMP2_PURE unsigned int __CLRCALL_PURE_OR_CDECL _Random_device();

_CRTIMP2_PURE unsigned int __CLRCALL_PURE_OR_CDECL _Random_device() { // return a random value
    unsigned int ans;
    if (_CSTD rand_s(&ans)) {
        _Xout_of_range("invalid random_device value");
    }

    return ans;
}

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
