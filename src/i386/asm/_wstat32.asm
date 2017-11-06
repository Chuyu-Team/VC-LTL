.model	flat


;新编译器_wstat已经改名为_wstat32
extern __imp___wstat:dword


.code


__wstat32 PROC
jmp __imp___wstat
__wstat32 ENDP

end