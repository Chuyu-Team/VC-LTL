

;_strtoi64 与 strtoll行为完全等价
extern __imp__strtoi64:qword

.code

strtoll PROC
jmp __imp__strtoi64
strtoll ENDP

end