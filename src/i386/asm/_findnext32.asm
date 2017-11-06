.model	flat

;新编译器_findnext已经改名为_findnext32
extern __imp___findnext:dword

.code

__findnext32 PROC
jmp __imp___findnext
__findnext32 ENDP

end