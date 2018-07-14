
#if defined _ATL_XP_TARGETING && defined _M_IX86

extern "C" extern int __isa_available;


extern "C" __declspec(naked) int __cdecl _ftol2_downlevel(double x)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		sub     esp, 20h
		and     esp, 0FFFFFFF0h
		fld     st
		fst     dword ptr[esp + 18h]
		fistp   qword ptr[esp + 10h]
		fild    qword ptr[esp + 10h]
		mov     edx, [esp + 18h]
		mov     eax, [esp + 10h]
		test    eax, eax
		jz      short integer_QnaN_or_zero

		arg_is_not_integer_QnaN :
		fsubp   st(1), st
			test    edx, edx
			jns     short positive
			fstp    dword ptr[esp]
			mov     ecx, [esp]
			xor ecx, 80000000h
			add     ecx, 7FFFFFFFh
			adc     eax, 0
			mov     edx, [esp + 14h]
			adc     edx, 0
			jmp     short localexit

			positive :
		fstp    dword ptr[esp]
			mov     ecx, [esp]
			add     ecx, 7FFFFFFFh
			sbb     eax, 0
			mov     edx, [esp + 14h]
			sbb     edx, 0
			jmp     short localexit

			integer_QnaN_or_zero :
		mov     edx, [esp + 14h]
			test    edx, 7FFFFFFFh
			jnz     short arg_is_not_integer_QnaN
			fstp    dword ptr[esp + 18h]
			fstp    dword ptr[esp + 18h]

			localexit :
			leave
			retn
	}
}

extern "C" __declspec(naked) int __cdecl _ftol2_pentium4_downlevel(double x)
{
	__asm
	{
		push    ebp
		mov     ebp, esp
		sub     esp, 8
		and esp, 0FFFFFFF8h
		fstp    qword ptr[esp]
		cvttsd2si eax, qword ptr[esp]
		leave
		retn
	}
}

extern "C" __declspec(naked) int __cdecl _ftol2_sse_downlevel(double x)
{
	/*if (__isa_available == 0)
	{
	return _ftol2_downlevel(x);
	}
	else
	{
	return _ftol2_pentium4_downlevel(x);
	}*/

	//使用内联汇编提高性能
	__asm
	{
		cmp __isa_available, 0
		jz short _ftol2_downlevel

		jmp _ftol2_pentium4_downlevel
	}
}

extern "C" __declspec(naked) int __cdecl _ftol2_sse_excpt_downlevel(double x)
{
	/*if (__isa_available)
	{
		short currentState;

		__asm { fstcw currentState }

		if ((currentState & 0x7F) == 0x7F)
		{
			return _ftol2_pentium4_downlevel(x);
		}
	}

	return _ftol2_downlevel(x);*/

	//使用内联汇编提高性能

	__asm
	{
		cmp     __isa_available, 0
		jz      _ftol2_downlevel

		sub     esp, 4
		fnstcw[esp]
		pop     eax
		and     ax, 7Fh
		cmp     ax, 7Fh
		jz      _ftol2_pentium4_downlevel
		jmp _ftol2_downlevel
	}
}



#endif