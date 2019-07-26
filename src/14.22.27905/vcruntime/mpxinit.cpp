/***
*mpxinit.cpp - Helper routine to initialize the bounds of statically initialized global pointers
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This function is called by the compiler in a dynamic initializer
*       to initialize the bounds of global pointers which are statically
*       initialized. It takes two arguments - 1) a list of triplets which
*       is created and passed by the compiler and 2) the number of triplets
*       in the list. This function loops through each element in the
*       list, makes and stores the bounds
****/

#include <intrin.h>

/*      This structure represents a single entry in the list created and
*       passed by the compiler.
*/
typedef struct _MPXDATA
{
    void **ptr;     // Address of the pointer whose bounds are to be initialized
    void *lo_bnd;   // Lower bound of the pointer
    void *hi_bnd;   // Upper bound of the pointer
} MPXDATA;

extern "C" void __fastcall __mpx_init_static_rep(MPXDATA* pMPXdata, size_t size)
{
    if (!pMPXdata)
        return;

    for (size_t i = size; i > 0; i--)
    {
        // Get the allocated size used to create bounds
        size_t size = (size_t)((char*)pMPXdata->hi_bnd - (char*)pMPXdata->lo_bnd);
        // Make the bounds.
        void* pointer = _bnd_set_ptr_bounds(pMPXdata->lo_bnd, size);
        // Make sure pointer has the value that is passed the compiler (*(pMPXdata->ptr))
        pointer = _bnd_copy_ptr_bounds(*(pMPXdata->ptr), pointer);
        // Store the bounds in the bounds table
        _bnd_store_ptr_bounds((const void **)(pMPXdata->ptr), pointer);
        // Move on to next entry in the list.
        pMPXdata++;
    }
}
