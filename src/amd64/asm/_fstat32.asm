


;新编译器_fstat已经改名为_fstat32
extern __imp__fstat:qword


.code

_fstat32 PROC
jmp __imp__fstat
_fstat32 ENDP

end