#include <corecrt_internal_stdio.h>

// Buffer pointers for stdout and stderr
extern "C" void* __acrt_stdout_buffer = nullptr;
extern "C" void* __acrt_stderr_buffer = nullptr;

extern "C" __declspec(dllimport) extern _iobuf _iob[_IOB_ENTRIES];

extern "C" bool __cdecl __acrt_stdio_begin_temporary_buffering_nolock(
    FILE* const public_stream
    )
{
	// Do nothing if the stream is not backed by a TTY device
	if (!_isatty(_fileno(public_stream)))
		return false;


	void** buffer;
	if (public_stream == /*stdout*/&_iob[1])
		buffer = &__acrt_stdout_buffer;
	else if (public_stream == /*stderr*/&_iob[2])
		buffer = &__acrt_stderr_buffer;
	else
		return false;

	//_IOBUFFER_SETVBUF | 
	if (public_stream->_flags & 0x10C)
		return false;

	public_stream->_flags |= 0x1102u;

	if (*buffer == nullptr)
	{
		auto tmp = malloc(_INTERNAL_BUFSIZ);

		if (tmp == nullptr)
		{
			// If we failed to allocate a buffer, use the small character buffer:
			public_stream->_base = reinterpret_cast<char*>(&public_stream->_charbuf);
			public_stream->_ptr = reinterpret_cast<char*>(&public_stream->_charbuf);
			public_stream->_cnt = 2;
			public_stream->_bufsiz = 2;
			return true;
		}

		*buffer = tmp;
	}

	// Otherwise, we have a new buffer, so set it up for use:
	public_stream->_base = reinterpret_cast<char*>(*buffer);
	public_stream->_ptr = reinterpret_cast<char*>(*buffer);
	public_stream->_cnt = _INTERNAL_BUFSIZ;
	public_stream->_bufsiz = _INTERNAL_BUFSIZ;
	return true;
}


// If the stream currently has a temporary buffer that was set via a call to
// __acrt_stdio_begin_temporary_buffering_nolock(), and if the flag value is
// 1, this function flushes the stream and disables buffering of the stream.
extern "C" void __cdecl __acrt_stdio_end_temporary_buffering_nolock(
    bool  const flag,
    FILE* const public_stream
    )
{
	if (!flag)
		return;

	if (public_stream->_flags & 0x1000)
	{
		//_flush(public_stream);，凑合用 fflush 吧，虽然会多进一次临界区但是影响不大。
		fflush(public_stream);

		public_stream->_flags &= 0xFFFFEEFF;
		public_stream->_bufsiz = 0;
		public_stream->_ptr = 0;
		public_stream->_base = 0;
	}
}