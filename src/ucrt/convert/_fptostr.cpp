//
// _fptostr.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the internal function that writes a mantissa (in a STRFLT) into a
// buffer.  This function puts all of the digits into the buffer, handles
// rounding based on the requested precision (digits), and updates the decimal
// point position in the STRFLT object.  Note that this function does not change
// the mantissa field of the STRFLT, so callers of this function may rely on it
// being unmodified.
//
#include <corecrt_internal.h>
#include <corecrt_internal_fltintrn.h>
#include <string.h>
#include <stddef.h>


#pragma warning(disable:__WARNING_UNRELATED_LOOP_TERMINATION) // 22102
#pragma warning(disable:__WARNING_BUFFER_UNDERFLOW) // 26001

extern "C" errno_t __cdecl __acrt_fp_strflt_to_string(
    char*  const buffer,
    size_t const buffer_count,
    int          digits,
    STRFLT const pflt
    )
{
    _VALIDATE_RETURN_ERRCODE(buffer != nullptr, EINVAL);
    _VALIDATE_RETURN_ERRCODE(buffer_count > 0,  EINVAL);
    buffer[0] = '\0';

    _VALIDATE_RETURN_ERRCODE(buffer_count > static_cast<size_t>((digits > 0 ? digits : 0) + 1), ERANGE);
    _VALIDATE_RETURN_ERRCODE(pflt != nullptr, EINVAL);

    char* buffer_it   = buffer;
    char* mantissa_it = pflt->mantissa;

    // The buffer will contain 'digits' decimal digits plus an optional overflow
    // digit for the rounding.

    // Initialize the first digit in the buffer to '0' (Note: not '\0') and set
    // the pointer to the second digit of the buffer.  The first digit is used
    // to handle overflow on rounding (e.g. 9.999... becomes 10.000...), which
    // requires a carry into the first digit.
    *buffer_it++ = '0';

    // Copy the digits of the value into the buffer (with '0' padding) and
    // insert the null terminator:
    while (digits > 0)
    {
        *buffer_it++ = *mantissa_it ? *mantissa_it++ : '0';
        --digits;
    }

    *buffer_it = '\0';

    // Do any rounding which may be needed.  Note:  if digits < 0, we don't do
    // any rounding because in this case, the rounding occurs in a digit which
    // will not be output because of the precision requested.
    if (digits >= 0 && *mantissa_it >= '5')
    {
        buffer_it--;

        while (*buffer_it == '9')
        {
            *buffer_it-- = '0';
        }

        *buffer_it += 1;
    }

    if (*buffer == '1')
    {
        // The rounding caused overflow into the leading digit (e.g. 9.999...
        // became 10.000...), so increment the decimal point position by 1:
        pflt->decpt++;
    }
    else
    {
        // Move the entire string to the left one digit to remove the unused
        // overflow digit:
        memmove(buffer, buffer + 1, strlen(buffer + 1) + 1);
    }

    return 0;
}
