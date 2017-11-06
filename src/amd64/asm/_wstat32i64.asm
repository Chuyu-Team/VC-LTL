

;新编译器_wstati64已经改名为_wstat32i64
extern __imp__wstati64:qword

.code

_wstat32i64 PROC
jmp __imp__wstati64
_wstat32i64 ENDP

end