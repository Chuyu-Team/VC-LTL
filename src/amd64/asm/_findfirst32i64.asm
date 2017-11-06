

;新编译器_findfirsti64已经改名为_findfirst32i64
extern __imp__findfirsti64:qword


.code

_findfirst32i64 PROC
jmp __imp__findfirsti64
_findfirst32i64 ENDP

end