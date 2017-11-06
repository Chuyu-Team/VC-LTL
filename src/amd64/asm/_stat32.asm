


;新编译器_stat已经改名为_stat32
extern __imp__stat:qword

.code

_stat32 PROC
jmp __imp__stat
_stat32 ENDP

end