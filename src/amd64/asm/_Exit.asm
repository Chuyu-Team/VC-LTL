
;_Exit行为与_exit完全等价
extern __imp__exit:qword

.code

_Exit PROC
jmp __imp__exit
_Exit ENDP

end