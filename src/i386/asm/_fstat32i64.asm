.model	flat



;新编译器_fstati64已经改名为_fstat32i64
extern __imp___fstati64:dword


.code

__fstat32i64 PROC
jmp __imp___fstati64
__fstat32i64 ENDP

end