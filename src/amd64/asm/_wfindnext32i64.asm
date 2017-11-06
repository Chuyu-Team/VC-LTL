

;新编译器_wfindnexti64已经改名为_wfindnext32i64
extern __imp__wfindnexti64:qword


.code

_wfindnext32i64 PROC
jmp __imp__wfindnexti64
_wfindnext32i64 ENDP

end