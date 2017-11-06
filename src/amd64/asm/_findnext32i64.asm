

;新编译器_findnexti64已经改名为_findnext32i64
extern __imp__findnexti64:qword


.code

_findnext32i64 PROC
jmp __imp__findnexti64
_findnext32i64 ENDP

end