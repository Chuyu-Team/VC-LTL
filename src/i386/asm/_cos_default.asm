.model	flat


;_cos_default -> cos
extern __imp__cos:dword


.code

__cos_default PROC
jmp __imp__cos
__cos_default ENDP

end