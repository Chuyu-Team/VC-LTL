
#include <corecrt_terminate.h>
#include <ehdata.h>
#include <msvcrt_IAT.h>

#ifdef _M_IX86

extern "C" __declspec(dllimport) EXCEPTION_DISPOSITION __cdecl _except_handler3(
	_In_ struct _EXCEPTION_RECORD* _ExceptionRecord,
	_In_ void*                     _EstablisherFrame,
	_Inout_ struct _CONTEXT*       _ContextRecord,
	_Inout_ void*                  _DispatcherContext
	);

extern "C" EXCEPTION_DISPOSITION __cdecl _except_handler3_noexcept(
	_In_ struct _EXCEPTION_RECORD* _ExceptionRecord,
	_In_ void*                     _EstablisherFrame,
	_Inout_ struct _CONTEXT*       _ContextRecord,
	_Inout_ void*                  _DispatcherContext
	) noexcept
{
	auto Excption = _except_handler3(_ExceptionRecord, _EstablisherFrame, _ContextRecord, _DispatcherContext);

	if (IS_DISPATCHING(_ExceptionRecord->ExceptionFlags)
		&& _ExceptionRecord->ExceptionCode == EH_EXCEPTION_NUMBER
		&& Excption == ExceptionContinueSearch)
	{
		//½áÊø½ø³Ì
		terminate();
	}

	return Excption;
}

_LCRT_DEFINE_IAT_SYMBOL(_except_handler3_noexcept);

#endif