.model	flat


;新编译器_wfindnext已经改名为_wfindnext32
extern __imp___wfindnext:dword


.code

__wfindnext32 PROC
jmp __imp___wfindnext
__wfindnext32 ENDP

end