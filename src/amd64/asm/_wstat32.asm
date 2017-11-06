


;新编译器_wstat已经改名为_wstat32
extern __imp__wstat:qword


.code

_wstat32 PROC
jmp __imp__wstat
_wstat32 ENDP

end