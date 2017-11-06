.model	flat


;_sin_default -> sin
extern __imp__sin:dword


.code

__sin_default PROC
jmp __imp__sin
__sin_default ENDP

end