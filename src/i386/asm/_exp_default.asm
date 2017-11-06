.model	flat


;_exp_default -> exp
extern __imp__exp:dword


.code

__exp_default PROC
jmp __imp__exp
__exp_default ENDP

end