

;_wcstoi64 与 wcstoll行为完全等价
extern __imp__wcstoi64:qword

.code

wcstoll PROC
jmp __imp__wcstoi64
wcstoll ENDP

end