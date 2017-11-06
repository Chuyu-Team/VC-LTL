.model	flat


;_acos_default -> acos
extern __imp__acos:dword


.code

__acos_default PROC
jmp __imp__acos
__acos_default ENDP

end