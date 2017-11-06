.model	flat


;新编译器_stati64已经改名为_stat32i64
extern __imp___stati64:dword

.code

__stat32i64 PROC
jmp __imp___stati64
__stat32i64 ENDP

end
