


;新编译器_wfindfirst已经改名为_wfindfirst32
extern __imp__wfindfirst:qword


.code

_wfindfirst32 PROC
jmp __imp__wfindfirst
_wfindfirst32 ENDP

end