.model	flat


;新编译器_stat已经改名为_stat32
extern __imp___stat:dword

.code


__stat32 PROC
jmp __imp___stat
__stat32 ENDP

end