/***
*user.cpp - E.H. functions only called by the client programs
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Exception handling functions only called by the client programs,
*       not by the C/C++ runtime itself.
*
*       Entry Points:
*       * set_terminate
*       * set_unexpected
*       * _set_seh_translator
****/

#include <eh.h>
#include <ehhooks.h>
#include <stdlib.h>

#pragma warning(disable: 4535) // calling _set_se_translator() requires /EHa

/////////////////////////////////////////////////////////////////////////////
//
// _set_se_translator - install a new SE to C++ EH translator.
//
// The 'new' seh translator may be nullptr, because the default one is.
//

_se_translator_function __cdecl
_set_se_translator( _se_translator_function pNew )
{
    _se_translator_function pOld = __pSETranslator;

    __pSETranslator = pNew;
    return pOld;
}

_se_translator_function _set_se_translator(int _I)
{
    if (_I == 0)
    {
        return _set_se_translator((_se_translator_function)0);
    }
    return 0; /* nullptr */
}
