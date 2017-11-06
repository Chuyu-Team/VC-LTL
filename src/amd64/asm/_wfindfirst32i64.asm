

;新编译器_wfindfirsti64已经改名为_wfindfirst32i64
extern __imp__wfindfirsti64:qword


.code

_wfindfirst32i64 PROC
jmp __imp__wfindfirsti64
_wfindfirst32i64 ENDP

end