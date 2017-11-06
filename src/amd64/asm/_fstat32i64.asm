

;新编译器_fstati64已经改名为_fstat32i64
extern __imp__fstati64:qword


.code

_fstat32i64 PROC
jmp __imp__fstati64
_fstat32i64 ENDP

end