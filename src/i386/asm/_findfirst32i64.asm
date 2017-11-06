.model	flat


;新编译器_findfirsti64已经改名为_findfirst32i64
extern __imp___findfirsti64:dword


.code

__findfirst32i64 PROC
jmp __imp___findfirsti64
__findfirst32i64 ENDP

end