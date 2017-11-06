

;新编译器_findfirst已经改名为_findfirst32
extern __imp__findfirst:qword


.code

_findfirst32 PROC
jmp __imp__findfirst
_findfirst32 ENDP

end