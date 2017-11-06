.model	flat


;新编译器_fstat已经改名为_fstat32
extern __imp___fstat:dword

.code

__fstat32 PROC
jmp __imp___fstat
__fstat32 ENDP

end