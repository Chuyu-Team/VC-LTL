.model	flat


;新编译器_wfindfirsti64已经改名为_wfindfirst32i64
extern __imp___wfindfirsti64:dword


.code

__wfindfirst32i64 PROC
jmp __imp___wfindfirsti64
__wfindfirst32i64 ENDP

end