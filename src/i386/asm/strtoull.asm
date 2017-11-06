.model	flat

;_strtoui64 与 strtoull行为完全等价

extern __imp___strtoui64:dword

.code

_strtoull PROC
jmp __imp___strtoui64
_strtoull ENDP

end