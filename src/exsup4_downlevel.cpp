#include <vcruntime.h>
#include <Windows.h>
#include <intrin.h>

#ifdef _M_IX86

#if 0
struct _JMP_BUF
{
	unsigned saved_ebp;
	unsigned saved_ebx;
	unsigned saved_edi;
	unsigned saved_esi;
	unsigned saved_esp;
	unsigned saved_return;
	unsigned saved_xregistration;
	unsigned saved_trylevel;
	//following only found in C9.0 or later jmp_buf
	unsigned version_cookie;
	unsigned unwind_func;
	unsigned unwind_data[6];
};

extern "C" int __cdecl _local_unwind4(unsigned* unwind_data, unsigned saved_xregistration, unsigned saved_trylevel);

extern "C" void __cdecl _NLG_Notify(int Type);

extern "C" __declspec(naked) int __stdcall _seh_longjmp_unwind4_downlevel(_JMP_BUF* pBuf)
{
	__asm
	{
		//备份原始ebp
		push ebp

		mov ecx, [esp + 4 + 4/*pBuf*/]

		mov ebp, [ecx]

		push    dword ptr[ecx + 1Ch] //saved_trylevel
		push    dword ptr[ecx + 18h] //saved_xregistration
		push    dword ptr[ecx + 28h] //unwind_data
		call    _local_unwind4
		add     esp, 0Ch

		//恢复ebp
		pop  ebp
		retn 4
	}
}


extern "C" __declspec(naked) int __fastcall _EH4_CallFilterFunc(void* FilterFunc,unsigned FramePointer)
{
	__asm
	{
		push    ebp
		push    esi
		push    edi
		push    ebx
		mov     ebp, edx/*FramePointer*/
		xor     eax, eax
		xor     ebx, ebx
		xor     edx, edx
		xor     esi, esi
		xor     edi, edi
		call    ecx/*FilterFunc*/
		pop     ebx
		pop     edi
		pop     esi
		pop     ebp
		retn
	}
}

extern "C" __declspec(naked) void __fastcall _EH4_TransferToHandler(void* HandlerAddress, unsigned FramePointer)
{
	__asm
	{
		mov     ebp, edx
		mov     esi, ecx
		mov     eax, ecx
		push    1
		call    _NLG_Notify
		xor     eax, eax
		xor     ebx, ebx
		xor     ecx, ecx
		xor     edx, edx
		xor     edi, edi
		jmp     esi
	}
}

#endif

extern "C" __declspec(naked) void __fastcall
_EH4_GlobalUnwind2(
	_In_opt_ PEXCEPTION_REGISTRATION_RECORD  EstablisherFrame,
	_In_opt_ PEXCEPTION_RECORD               ExceptionRecord
	)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		push    ebx
		push    esi
		push    edi

		push    0                 ; ReturnValue
		push    edx                 ; ExceptionRecord
		push    offset ReturnPoint; _AddressOfReturnAddress()
		push    ecx               ; EstablisherFrame

		//RtlUnwind(EstablisherFrame, _AddressOfReturnAddress(), nullptr, nullptr);
		call RtlUnwind

	ReturnPoint:
		pop     edi
		pop     esi
		pop     ebx
		pop     ebp
		retn
	}
}


#endif