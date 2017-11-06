.model	flat


;新编译器_findfirst已经改名为_findfirst32
extern __imp___findfirst:dword

.code

__findfirst32 PROC
jmp __imp___findfirst
__findfirst32 ENDP

end