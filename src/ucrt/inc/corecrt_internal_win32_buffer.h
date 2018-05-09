//
// corecrt_internal_win32_buffer.h
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This internal header defines template-based utilities for handling call-twice
// Win32 APIs, where you first call the Win32 API with null or a fixed sized buffer,
// and if there is not enough space, allocate a dynamically sized buffer.
#pragma once
#include <corecrt_internal.h>
#include <locale.h>

#pragma pack(push, _CRT_PACKING)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// __crt_win32_buffer_debug_info
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This is a class that can be used to describe the block_use, file_name, line_number
// debug data that is sometimes shuffled around between function calls.

class __crt_win32_buffer_debug_info
{
#ifndef _DEBUG
public:
    __crt_win32_buffer_debug_info(int, char const *, int)
    {
    }
#else /* ^^^^ Release ^^^^ / vvvv Debug vvvv */
public:
    __crt_win32_buffer_debug_info(
        int const          initial_block_use,
        char const * const initial_file_name,
        int const          initial_line_number
        )
        : _block_use(initial_block_use),
          _file_name(initial_file_name),
          _line_number(initial_line_number)
    {
    }

    int block_use() const
    {
        return _block_use;
    }

    char const * file_name() const
    {
        return _file_name;
    }

    int line_number() const
    {
        return _line_number;
    }

private:
    int          _block_use;
    char const * _file_name;
    int          _line_number;
#endif /* _DEBUG */
};

class __crt_win32_buffer_empty_debug_info
{
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// __crt_win32_buffer resize policies
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// These classes are used to describe the different resize policies that a
// __crt_win32_buffer can have.

struct __crt_win32_buffer_internal_dynamic_resizing
{
    using debug_info_type = __crt_win32_buffer_empty_debug_info;

    _Check_return_
    static errno_t allocate(void ** const address, size_t const size, debug_info_type const&)
    {
        void * const ret = _malloc_crt(size);
        *address = ret;
        if (ret == nullptr) {
            return ENOMEM;
        }
        return 0;
    }

    static void deallocate(void * const ptr, debug_info_type const&)
    {
        _free_crt(ptr);
    }
};

struct __crt_win32_buffer_public_dynamic_resizing
{
    using debug_info_type = __crt_win32_buffer_debug_info;

    _Check_return_
    static errno_t allocate(void ** const address, size_t const size, debug_info_type const& debug_info)
    {
        UNREFERENCED_PARAMETER(debug_info); // only used in debug mode
        void * const ret = _malloc_dbg(
            size,
            debug_info.block_use(),
            debug_info.file_name(),
            debug_info.line_number()
            );
        *address = ret;
        if (ret == nullptr) {
            return ENOMEM;
        }
        return 0;
    }

    static void deallocate(void * const ptr, debug_info_type const& debug_info)
    {
        UNREFERENCED_PARAMETER(debug_info); // only used in debug mode
        _free_dbg(ptr, debug_info.block_use());
    }
};

struct __crt_win32_buffer_no_resizing
{
    using debug_info_type = __crt_win32_buffer_empty_debug_info;

    _Check_return_
    static errno_t allocate(void ** const, size_t const, debug_info_type const&)
    {
        errno = ERANGE; // buffer not large enough
        return ERANGE;
    }

    static void deallocate(void * const, debug_info_type const&)
    {
    }
};

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// __crt_win32_buffer, __crt_internal_win32_buffer
// __crt_public_win32_buffer, __crt_no_alloc_win32_buffer
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Class and typedefs for buffers that support calling a win32 function and automatically
// resizing if needed.

template <typename Character, typename ResizePolicy>
class __crt_win32_buffer : private ResizePolicy::debug_info_type
{   // Buffer type to enable Win32 call-twice-if-not-enough-space APIs.
    // Using this type allows us to use a local buffer if possible and allocate if needed.
public:
    using char_type = Character;
    using debug_info_type = typename ResizePolicy::debug_info_type;

    __crt_win32_buffer()
        : debug_info_type(),
          _initial_string(nullptr),
          _initial_capacity(0),
          _string(nullptr),
          _capacity(0),
          _size(0),
          _is_dynamic(false)

    {
    }

    explicit __crt_win32_buffer(debug_info_type const& debug_info)
        : debug_info_type(debug_info),
          _initial_string(nullptr),
          _initial_capacity(0),
          _string(nullptr),
          _capacity(0),
          _size(0),
          _is_dynamic(false)

    {
    }

    template <size_t N>
    __crt_win32_buffer(Character (&buffer)[N])
        : debug_info_type(),
          _initial_string(buffer),
          _initial_capacity(N),
          _string(buffer),
          _capacity(N),
          _size(0),
          _is_dynamic(false)
    {
    }

    template <size_t N>
    __crt_win32_buffer(Character (&buffer)[N], debug_info_type const& debug_info)
        : debug_info_type(debug_info),
          _initial_string(buffer),
          _initial_capacity(N),
          _string(buffer),
          _capacity(N),
          _size(0),
          _is_dynamic(false)
    {
    }

    __crt_win32_buffer(
        Character * const buffer,
        size_t const buffer_capacity
        )
        : debug_info_type(),
          _initial_string(buffer),
          _initial_capacity(buffer_capacity),
          _string(buffer),
          _capacity(buffer_capacity),
          _size(0),
          _is_dynamic(false)
    {
    }

    __crt_win32_buffer(
        Character * const buffer,
        size_t const buffer_capacity,
        debug_info_type const& debug_info
        )
        : debug_info_type(debug_info),
          _initial_string(buffer),
          _initial_capacity(buffer_capacity),
          _string(buffer),
          _capacity(buffer_capacity),
          _size(0),
          _is_dynamic(false)
    {
    }

    ~__crt_win32_buffer()
    {
        _deallocate();
    }

    __crt_win32_buffer(__crt_win32_buffer const&) = delete;
    __crt_win32_buffer& operator=(__crt_win32_buffer const&) = delete;

    __crt_win32_buffer(__crt_win32_buffer&&) = delete;
    __crt_win32_buffer& operator=(__crt_win32_buffer&&) = delete;

    char_type * data()
    {
        return _string;
    }

    char_type const * data() const
    {
        return _string;
    }

    size_t size() const
    {
        return _size;
    }

    void size(size_t new_size)
    {
        _size = new_size;
    }

    size_t capacity() const
    {
        return _capacity;
    }

    void reset()
    {
        _deallocate();
        _reset_no_dealloc();
    }

    char_type * detach()
    {
        if (_string == nullptr || _size == 0) {
            return nullptr;
        }

        char_type * return_val{};

        if (!_is_dynamic && _size > 0) {
            // This pointer needs to live longer than the stack buffer
            // Allocate + Copy
            ResizePolicy::allocate(
                reinterpret_cast<void **>(&return_val),
                _size * sizeof(Character),
                debug_info()
                );
            memcpy_s(return_val, _size, _string, _capacity);
        } else {
            return_val = _string;
        }

        _reset_no_dealloc();
        return return_val;
    }

    template <typename Win32Function>
    errno_t call_win32_function(Win32Function const& win32_function)
    {   // Suitable for more Win32 calls, where a size is returned
        // if there is not enough space.

        size_t const size_required = win32_function(_string, static_cast<DWORD>(_capacity));
        if (size_required == 0) {
            __acrt_errno_map_os_error(GetLastError());
            return errno;
        }

        if (size_required <= _capacity) {
            // Had enough space, data was written, save size and return
            _size = size_required;
            return 0;
        }

        errno_t const alloc_err = allocate(size_required);
        if (alloc_err)
        {
            return alloc_err;
        }

        // Upon success, return value is number of characters written, minus the null terminator.
        size_t const size_required2 = win32_function(_string, static_cast<DWORD>(_capacity));
        if (size_required2 == 0) {
            __acrt_errno_map_os_error(GetLastError());
            return errno;
        }

        // Capacity should be large enough at this point.
        _size = size_required2;
        return 0;
    }

    debug_info_type const& debug_info() const
    {
        return static_cast<debug_info_type const&>(*this);
    }

    errno_t allocate(size_t requested_size)
    {
        _deallocate();

        errno_t err = ResizePolicy::allocate(
            reinterpret_cast<void **>(&_string),
            requested_size * sizeof(Character),
            debug_info()
            );

        if (err) {
            _is_dynamic = false;
            _capacity = 0;
            return err;
        }

        _is_dynamic = true;
        _capacity = requested_size;
        return 0;
    }

    void set_to_nullptr()
    {
        _deallocate();

        _string = nullptr;
        _capacity = 0;
        _size = 0;
    }

private:
    void _reset_no_dealloc()
    {
        _string = _initial_string;
        _capacity = _initial_capacity;
        _size = 0;
    }

    void _deallocate()
    {
        if (_is_dynamic) {
            ResizePolicy::deallocate(_string, debug_info());
            _is_dynamic = false;
        }
    }

    char_type * const _initial_string;
    size_t            _initial_capacity;
    char_type *       _string;
    size_t            _capacity;
    size_t            _size;
    bool              _is_dynamic;
};

template <typename Character>
using __crt_internal_win32_buffer = __crt_win32_buffer<Character, __crt_win32_buffer_internal_dynamic_resizing>;

template <typename Character>
using __crt_public_win32_buffer = __crt_win32_buffer<Character, __crt_win32_buffer_public_dynamic_resizing>;

template <typename Character>
using __crt_no_alloc_win32_buffer = __crt_win32_buffer<Character, __crt_win32_buffer_no_resizing>;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// UTF-8 or ACP Helper
//
// Some POSIX functions have historically used the ACP for doing narrow->wide
// conversions. In order to support UTF-8 with these functions, they've been
// modified so that they use CP_UTF8 when current locale is UTF-8, but the ACP
// otherwise in order to preserve backwards compatibility.
//
// These POSIX functions can call __acrt_get_utf8_acp_compatibility_codepage to grab
// the code page they should use for their conversions.
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
inline unsigned int __acrt_get_utf8_acp_compatibility_codepage()
{
    //_LocaleUpdate locale_update(nullptr);
    unsigned int const current_code_page = ___lc_codepage_func();

    if (current_code_page == CP_UTF8) {
        return CP_UTF8;
    }

    bool const use_oem_code_page = !__acrt_AreFileApisANSI();

    if (use_oem_code_page) {
        return CP_OEMCP;
    }

    return CP_ACP;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Win32 APIs using __crt_win32_buffer
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// See complete list of internal conversion functions in corecrt_internal_traits.h

template <typename FromChar, typename ToChar, typename CvtFunction, typename ResizePolicy>
errno_t __acrt_convert_wcs_mbs(
    FromChar const * const                    null_terminated_input_string,
    __crt_win32_buffer<ToChar, ResizePolicy>& win32_buffer,
    CvtFunction const&                        cvt_func,
    _locale_t                                 locale
    )
{
    if (null_terminated_input_string == nullptr) {
        win32_buffer.set_to_nullptr();
        return 0;
    }

    size_t const required_size = cvt_func(nullptr, null_terminated_input_string, 0, locale);

    if (required_size == static_cast<size_t>(-1)) {
        return errno;
    }

    size_t const required_size_plus_null_terminator = required_size + 1;

    if (required_size_plus_null_terminator > win32_buffer.capacity()) {
        errno_t const alloc_err = win32_buffer.allocate(required_size_plus_null_terminator);
        if (alloc_err != 0) {
            return alloc_err;
        }
    }

    size_t const chars_converted = cvt_func(win32_buffer.data(), null_terminated_input_string, win32_buffer.capacity(), locale);
    if (chars_converted == static_cast<size_t>(-1) || chars_converted == win32_buffer.capacity()) {
        // check for error or if output is not null terminated
        return errno;
    }

    win32_buffer.size(chars_converted + 1); // +1 for null terminator
    return 0;
}

template <typename FromChar, typename ToChar, typename CvtFunction, typename ResizePolicy>
errno_t __acrt_convert_wcs_mbs_cp(
    FromChar const * const                    null_terminated_input_string,
    __crt_win32_buffer<ToChar, ResizePolicy>& win32_buffer,
    CvtFunction const&                        cvt_func,
    unsigned int const                        code_page
    )
{
    size_t const required_size = cvt_func(
        code_page,
        null_terminated_input_string,
        nullptr,
        0
        );

    if (required_size == 0) {
        __acrt_errno_map_os_error(::GetLastError());
        return errno;
    }

    if (required_size > win32_buffer.capacity()) {
        errno_t alloc_err = win32_buffer.allocate(required_size);
        if (alloc_err != 0) {
            return alloc_err;
        }
    }

    size_t const chars_converted = cvt_func(
        code_page,
        null_terminated_input_string,
        win32_buffer.data(),
        win32_buffer.capacity()
        );

    if (chars_converted == 0) {
        __acrt_errno_map_os_error(::GetLastError());
        return errno;
    }

    win32_buffer.size(chars_converted);
    return 0;
}

template <typename ResizePolicy>
errno_t __acrt_wcs_to_mbs(
    wchar_t const * const                   null_terminated_input_string,
    __crt_win32_buffer<char, ResizePolicy>& win32_buffer,
    _locale_t                               locale = nullptr
    )
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return __acrt_convert_wcs_mbs(
            null_terminated_input_string,
            win32_buffer,
            _wcstombs_l,
            locale
            );
    _END_SECURE_CRT_DEPRECATION_DISABLE
}

template <typename ResizePolicy>
errno_t __acrt_wcs_to_mbs_cp(
    wchar_t const * const                   null_terminated_input_string,
    __crt_win32_buffer<char, ResizePolicy>& win32_buffer,
    unsigned int const                      code_page
    )
{
    auto const wcs_to_mbs = [](
        unsigned int const    code_page,
        wchar_t const * const null_terminated_input_string,
        char * const          buffer,
        size_t const          buffer_size)
    {
        return __acrt_WideCharToMultiByte(
            code_page,
            0,
            null_terminated_input_string,
            -1,
            buffer,
            static_cast<int>(buffer_size),
            nullptr,
            nullptr
            );
    };

    return __acrt_convert_wcs_mbs_cp(
        null_terminated_input_string,
        win32_buffer,
        wcs_to_mbs,
        code_page
        );
}

template <size_t N>
size_t __acrt_wcs_to_mbs_array(
    wchar_t const * const null_terminated_input_string,
    char                  (&buffer)[N],
    _locale_t             locale = nullptr
    )
{
    __crt_no_alloc_win32_buffer<char> win32_buffer(buffer);
    if (__acrt_wcs_to_mbs(null_terminated_input_string, win32_buffer, locale) != 0) {
        return 0;
    }

    return win32_buffer.size();
}

template <size_t N>
size_t __acrt_wcs_to_mbs_cp_array(
    wchar_t const * const null_terminated_input_string,
    char                  (&buffer)[N],
    unsigned int const    code_page
    )
{
    __crt_no_alloc_win32_buffer<char> win32_buffer(buffer);
    if (__acrt_wcs_to_mbs_cp(null_terminated_input_string, win32_buffer, code_page) != 0) {
        return 0;
    }

    return win32_buffer.size();
}

template <typename ResizePolicy>
errno_t __acrt_mbs_to_wcs(
    char const * const                         null_terminated_input_string,
    __crt_win32_buffer<wchar_t, ResizePolicy>& win32_buffer,
    _locale_t                                  locale = nullptr
    )
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
        return __acrt_convert_wcs_mbs(
            null_terminated_input_string,
            win32_buffer,
            _mbstowcs_l,
            locale
            );
    _END_SECURE_CRT_DEPRECATION_DISABLE
}

template <typename ResizePolicy>
errno_t __acrt_mbs_to_wcs_cp(
    char const * const                         null_terminated_input_string,
    __crt_win32_buffer<wchar_t, ResizePolicy>& win32_buffer,
    unsigned int const                         code_page
    )
{
    auto const mbs_to_wcs = [](
        unsigned int const code_page,
        char const * const null_terminated_input_string,
        wchar_t * const    buffer,
        size_t const       buffer_size)
    {
        return __acrt_MultiByteToWideChar(
            code_page,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            null_terminated_input_string,
            -1,
            buffer,
            static_cast<int>(buffer_size)
            );
    };

    return __acrt_convert_wcs_mbs_cp(
        null_terminated_input_string,
        win32_buffer,
        mbs_to_wcs,
        code_page
        );
}

template <size_t N>
size_t __acrt_mbs_to_wcs_array(
    char const * const null_terminated_input_string,
    wchar_t            (&buffer)[N],
    _locale_t          locale = nullptr
    )
{
    __crt_no_alloc_win32_buffer<wchar_t> win32_buffer(buffer);
    if (__acrt_mbs_to_wcs(null_terminated_input_string, win32_buffer, locale) != 0) {
        return 0;
    }

    return win32_buffer.size();
}

template <size_t N>
size_t __acrt_mbs_to_wcs_cp_array(
    char const * const null_terminated_input_string,
    wchar_t            (&buffer)[N],
    unsigned int const code_page
    )
{
    __crt_no_alloc_win32_buffer<wchar_t> win32_buffer(buffer);
    if (__acrt_wcs_to_mbs_cp(null_terminated_input_string, win32_buffer, code_page) != 0) {
        return 0;
    }

    return win32_buffer.size();
}

template <typename ResizePolicy>
errno_t __acrt_get_current_directory(
     __crt_win32_buffer<wchar_t, ResizePolicy>& win32_buffer
    )
{
    return win32_buffer.call_win32_function([](wchar_t * buffer, DWORD buffer_length)
    {
        return ::GetCurrentDirectoryW(buffer_length, buffer);
    });
}

template <typename ResizePolicy>
errno_t __acrt_get_current_directory(
    __crt_win32_buffer<char, ResizePolicy>& win32_buffer
    )
{
    wchar_t default_buffer_space[_MAX_PATH];
    __crt_internal_win32_buffer<wchar_t> wide_buffer(default_buffer_space);

    errno_t const err = __acrt_get_current_directory(wide_buffer);

    if (err != 0) {
        return err;
    }

    return __acrt_wcs_to_mbs(
        wide_buffer.data(),
        win32_buffer
        );
}

template <typename ResizePolicy>
errno_t __acrt_get_full_path_name_cp(
    wchar_t const * const                      lpFileName,
    __crt_win32_buffer<wchar_t, ResizePolicy>& win32_buffer,
    unsigned int const                      /* code_page */
    )
{
    return win32_buffer.call_win32_function([lpFileName](wchar_t * buffer, DWORD buffer_length)
    {
        return ::GetFullPathNameW(
            lpFileName,
            buffer_length,
            buffer,
            nullptr
        );
    });
}

template <typename ResizePolicy>
errno_t __acrt_get_full_path_name_cp(
    char const * const                      lpFileName,
    __crt_win32_buffer<char, ResizePolicy>& win32_buffer,
    unsigned int const                      code_page
    )
{
    wchar_t default_buffer_space[_MAX_PATH];
    __crt_internal_win32_buffer<wchar_t> wide_buffer(default_buffer_space);

    wchar_t default_file_name_space[_MAX_PATH];
    __crt_internal_win32_buffer<wchar_t> wide_file_name(default_file_name_space);

    errno_t const cvt_err = __acrt_mbs_to_wcs_cp(
        lpFileName,
        wide_file_name,
        code_page
        );

    if (cvt_err != 0)
    {
        return cvt_err;
    }

    errno_t const err = __acrt_get_full_path_name_cp(wide_file_name.data(), wide_buffer, code_page);

    if (err != 0)
    {
        return err;
    }

    return __acrt_wcs_to_mbs_cp(
        wide_buffer.data(),
        win32_buffer,
        code_page
        );
}

#pragma pack(pop)
