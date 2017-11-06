

;新编译器_findnext已经改名为_findnext32
extern __imp__findnext:qword


.code

_findnext32 PROC
jmp __imp__findnext
_findnext32 ENDP

end