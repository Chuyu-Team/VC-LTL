/***
*mehvecdtr.cxx - EH-aware version of destructor iterator helper function
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       These functions are called when constructing and destructing arrays of
*       objects.  Their purpose is to assure that constructed elements get
*       destructed if the constructor for one of the elements throws.
*
*       Must be compiled using "-d1Binl" to be able to specify __thiscall
*       at the user level
****/

#undef CRTDLL
#undef MRTDLL

#include "ehvecdtr.cpp"
