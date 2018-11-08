
#include <corecrt_internal.h>
#include "msvcrt_IAT.h"

//需要对lib修正，直接转发atexit
//extern "C" int __cdecl _crt_atexit(_PVFV const function);


extern "C" int __cdecl _initialize_onexit_table_downlevel(_onexit_table_t* const table)
{
	if (!table)
	{
		return -1;
	}

	// If the table has already been initialized, do not do anything.  Note that
	// this handles both the case where the table was value initialized and where
	// the table was initialized with encoded null pointers.
	if (table->_first != table->_end)
	{
		return 0;
	}

	_PVFV* const encoded_nullptr = __crt_fast_encode_pointer(nullptr);

	table->_first = encoded_nullptr;
	table->_last = encoded_nullptr;
	table->_end = encoded_nullptr;

	return 0;
}

_LCRT_DEFINE_IAT_SYMBOL(_initialize_onexit_table_downlevel);

extern "C" __declspec(dllimport) _onexit_t __cdecl __dllonexit(
	_onexit_t func,
	_PVFV **  pbegin,
	_PVFV **  pend
	);

// Appends the given 'function' to the given onexit 'table'.  Returns 0 on
// success; returns -1 on failure.  In general, failures are considered fatal
// in calling code.
extern "C" int __cdecl _register_onexit_function_downlevel(_onexit_table_t* const table, _onexit_t const function)
{
	if (!table)
		return -1;

	return __acrt_lock_and_call(__acrt_exit_lock, [&]()
	{
		_PVFV* first = __crt_fast_decode_pointer(table->_first);
		_PVFV* end = __crt_fast_decode_pointer(table->_end);

		if (first == nullptr && end == nullptr)
		{
			//初始化table，最初分配32个元素
			first = (_PVFV*)malloc(32 * sizeof(_PVFV));

			//内存申请失败
			if (!first)
				return -1;

			end = first;
		}

		auto retval = __dllonexit(function, &first, &end);

		if (!retval)
			return -1;

		//操作成功，重新加密原始指针
		table->_first = __crt_fast_encode_pointer(first);
		table->_end = __crt_fast_encode_pointer(end);
		//无需使用_last
		//table->_last
		return 0;
	});
}

_LCRT_DEFINE_IAT_SYMBOL(_register_onexit_function_downlevel);


// This function executes a table of _onexit()/atexit() functions.  The
// terminators are executed in reverse order, to give the required LIFO
// execution order.  If the table is uninitialized, this function has no
// effect.  After executing the terminators, this function resets the table
// so that it is uninitialized.  Returns 0 on success; -1 on failure.
extern "C" int __cdecl _execute_onexit_table_downlevel(_onexit_table_t* const table)
{
	if (!table)
	{
		return -1;
	}

	return __acrt_lock_and_call(__acrt_exit_lock, [&]()
	{
		_PVFV* first = __crt_fast_decode_pointer(table->_first);
		_PVFV* end = __crt_fast_decode_pointer(table->_end);
		if (!first || first == reinterpret_cast<_PVFV*>(-1))
		{
			return 0;
		}

		_PVFV const encoded_nullptr = __crt_fast_encode_pointer(nullptr);

		for (auto last=end-1; last>= first;--last)
		{
			if (*last != encoded_nullptr)
			{
				(*last)();
			}
		}

		//释放内存
		free(first);

		//清空table
		table->_first = (_PVFV*)encoded_nullptr;
		table->_last = (_PVFV*)encoded_nullptr;
		table->_end = (_PVFV*)encoded_nullptr;

		return 0;
	});
}

_LCRT_DEFINE_IAT_SYMBOL(_execute_onexit_table_downlevel);

extern "C" _onexit_table_t __acrt_at_quick_exit_table{};


extern "C" int __cdecl _crt_at_quick_exit_downlevel(_PVFV const function)
{
	return _register_onexit_function(&__acrt_at_quick_exit_table, reinterpret_cast<_onexit_t>(function));
}

_LCRT_DEFINE_IAT_SYMBOL(_crt_at_quick_exit_downlevel);
