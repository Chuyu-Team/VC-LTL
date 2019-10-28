

#ifndef _LTL_ptd_downlevel

struct ptd_downlevel
{
	_invalid_parameter_handler _thread_local_iph;
#if defined (_M_IA64) || defined (_M_AMD64) || defined (_M_ARM64) || defined (_M_ARM)
	void* _pForeignException;
	uintptr_t   _ImageBase;
	uintptr_t _ThrowImageBase;
#endif
};

EXTERN_C ptd_downlevel* __fastcall __LTL_get_ptd_downlevel();

#endif