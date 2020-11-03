#include <Windows.h>
#include <corecrt_wstdio.h>
#include <stdio.h>
#include <corecrt_internal_stdio.h>

extern "C" __declspec(dllimport) extern _iobuf _iob[_IOB_ENTRIES];


namespace VC_LTL_Thunks
{
	extern "C" __declspec(dllimport)  wint_t __cdecl fputwc(
		_In_    wchar_t _Character,
		_Inout_ FILE* _Stream);

	extern "C" __declspec(dllimport) size_t __cdecl fwrite(
        _In_reads_bytes_(_ElementSize * _ElementCount) void const* _Buffer,
        _In_                                           size_t      _ElementSize,
        _In_                                           size_t      _ElementCount,
        _Inout_                                        FILE*       _Stream
        );

	extern "C" __declspec(dllimport) int __cdecl fputc(
        _In_    int   _Character,
        _Inout_ FILE* _Stream
        );
}

extern "C" wint_t __cdecl fputwc(
	wchar_t _Character,
	FILE* _Stream)
{
	if ((_Character & (~0x7F))
		&&(_Stream == /*stdout*/&_iob[1] || _Stream == /*stderr*/&_iob[2]))
	{
		_lock_file(_Stream);
		auto Success = __acrt_stdio_begin_temporary_buffering_nolock(_Stream);
		wint_t result = VC_LTL_Thunks::fputwc(_Character, _Stream);
		__acrt_stdio_end_temporary_buffering_nolock(Success, _Stream);
		_unlock_file(_Stream);

		return result;
	}
	
	return VC_LTL_Thunks::fputwc(_Character, _Stream);
}

extern "C" wint_t __cdecl putwc(
        wchar_t _Character,
        FILE*   _Stream
        )
{
	return fputwc(_Character, _Stream);
}

extern "C" wint_t __cdecl _fputwchar(
	wchar_t _Character
	)
{
	return fputwc(_Character,/*stdout*/&_iob[1]);
}

extern "C"  wint_t __cdecl putwchar(
        wchar_t _Character
        )
 {
	return _fputwchar(_Character);
 }

extern "C" size_t __cdecl fwrite(
    void const* const buffer,
    size_t      const size,
    size_t      const count,
    FILE*       const stream
    )
{
	if (stream == /*stdout*/&_iob[1] || stream == /*stderr*/&_iob[2])
	{
		_lock_file(stream);
		auto Success = __acrt_stdio_begin_temporary_buffering_nolock(stream);
		wint_t result = VC_LTL_Thunks::fwrite(buffer, size, count, stream);
		__acrt_stdio_end_temporary_buffering_nolock(Success, stream);
		_unlock_file(stream);

		return result;
	}
	else
	{
		return VC_LTL_Thunks::fwrite(buffer, size, count, stream);
	}
}

extern "C" int __cdecl fputc(int const c, FILE* const stream)
{
	if ((c & 0x80)
		&&(stream == /*stdout*/&_iob[1] || stream == /*stderr*/&_iob[2]))
	{
		_lock_file(stream);
		static bool stdout_buffer_used = false;
		static bool stderr_buffer_used = false;

		static char stdout_buffer[2];
		
		static char stderr_buffer[2];


		bool* buffer_used;
		char* buffer;
		if (stream == /*stdout*/&_iob[1])
		{
			buffer_used = &stdout_buffer_used;
			buffer = stdout_buffer;
		}
		else
		{
			buffer_used = &stderr_buffer_used;
			buffer = stderr_buffer;
		}

		int nRet;
		if (*buffer_used)
		{
			buffer[1] = c;
			auto Success = __acrt_stdio_begin_temporary_buffering_nolock(stream);
			auto result = VC_LTL_Thunks::fwrite(buffer, 1, 2, stream);
			__acrt_stdio_end_temporary_buffering_nolock(Success, stream);

			*buffer_used = false;


			nRet = result ? c : EOF;
		}
		else if (isleadbyte(c))
		{
			*buffer_used = true;
			buffer[0] = c;
			nRet = c;
		}
		else
		{
			nRet = VC_LTL_Thunks::fputc(c, stream);
		}

		_unlock_file(stream);

		return nRet;
	}

	return VC_LTL_Thunks::fputc(c, stream);
}



extern "C" int __cdecl putc(
        int   _Character,
        FILE* _Stream
        )
{
	return fputc(_Character, _Stream);
}

extern "C" int __cdecl _fputchar(
        int _Character
        )
{
	return fputc(_Character, /*stdout*/&_iob[1]);
}

extern "C" int __cdecl putchar(
        int _Character
        )
{
	return _fputchar(_Character);
}