

;_strtoui64 与 strtoull行为完全等价
extern __imp__strtoui64:qword


.code


strtoull PROC
jmp __imp__strtoui64
strtoull ENDP

end