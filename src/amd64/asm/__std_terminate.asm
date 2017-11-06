
;__std_terminate -> C++ void terminate()
extern __imp_?terminate@@YAXXZ:qword

.code

__std_terminate PROC
jmp __imp_?terminate@@YAXXZ
__std_terminate ENDP

end