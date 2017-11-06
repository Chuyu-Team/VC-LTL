

;_wcstoui64 与 wcstoull行为完全等价
extern __imp__wcstoui64:qword

.code

wcstoull PROC
jmp __imp__wcstoui64
wcstoull ENDP

end