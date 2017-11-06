.model	flat

;新编译器_wfindfirst已经改名为_wfindfirst32
extern __imp___wfindfirst:dword

.code

__wfindfirst32 PROC
jmp __imp___wfindfirst
__wfindfirst32 ENDP

end