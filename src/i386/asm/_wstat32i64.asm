.model	flat


;新编译器_wstati64已经改名为_wstat32i64
extern __imp___wstati64:dword


.code

__wstat32i64 PROC
jmp __imp___wstati64
__wstat32i64 ENDP

end