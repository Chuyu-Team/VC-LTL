.model	flat


;_strtoi64 与 strtoll行为完全等价
extern __imp___strtoi64:dword

.code

_strtoll PROC
jmp __imp___strtoi64
_strtoll ENDP

end