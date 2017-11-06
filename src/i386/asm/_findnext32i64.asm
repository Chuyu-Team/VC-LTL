.model	flat


;新编译器_findnexti64已经改名为_findnext32i64
extern __imp___findnexti64:dword


.code

__findnext32i64 PROC
jmp __imp___findnexti64
__findnext32i64 ENDP

end