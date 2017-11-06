.model	flat


;__std_terminate -> C++ void terminate()
extern __imp_?terminate@@YAXXZ:dword

.code

___std_terminate PROC
jmp __imp_?terminate@@YAXXZ
___std_terminate ENDP

end
