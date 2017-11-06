.model	flat


;_tan_default -> tan
extern __imp__tan:dword


.code

__tan_default PROC
jmp __imp__tan
__tan_default ENDP

end