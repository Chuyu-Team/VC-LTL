

.model	flat

;_wcstoui64 与 wcstoull行为完全等价

extern __imp___wcstoui64:PROC

.code

_wcstoull PROC
jmp __imp___wcstoui64
_wcstoull ENDP


END