.model	flat


;_atan_default -> atan
extern __imp__atan:dword


.code

__atan_default PROC
jmp __imp__atan
__atan_default ENDP

end