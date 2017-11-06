



;新编译器_wfindnext已经改名为_wfindnext32
extern __imp__wfindnext:qword


.code

_wfindnext32 PROC
jmp __imp__wfindnext
_wfindnext32 ENDP

end