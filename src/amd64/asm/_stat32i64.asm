

;新编译器_stati64已经改名为_stat32i64
extern __imp__stati64:qword


.code

_stat32i64 PROC
jmp __imp__stati64
_stat32i64 ENDP

end