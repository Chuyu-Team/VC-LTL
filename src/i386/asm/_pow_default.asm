.model	flat


;_pow_default -> pow
extern __imp__pow:dword


.code

__pow_default PROC
jmp __imp__pow
__pow_default ENDP

end