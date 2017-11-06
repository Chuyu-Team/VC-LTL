.model	flat



;_wcstoi64 与 wcstoll行为完全等价
extern __imp___wcstoi64:dword

.code

_wcstoll PROC
jmp __imp___wcstoi64
_wcstoll ENDP

end