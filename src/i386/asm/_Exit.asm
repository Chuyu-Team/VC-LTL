.model	flat


;_Exit行为与_exit完全等价
extern __imp___exit:dword

.code

__Exit PROC
jmp __imp___exit
__Exit ENDP

end