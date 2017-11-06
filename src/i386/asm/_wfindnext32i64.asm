.model	flat


;新编译器_wfindnexti64已经改名为_wfindnext32i64
extern __imp___wfindnexti64:dword


.code

__wfindnext32i64 PROC
jmp __imp___wfindnexti64
__wfindnext32i64 ENDP

end