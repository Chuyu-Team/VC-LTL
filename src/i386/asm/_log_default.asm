.model	flat


;_log_default -> log
extern __imp__log:dword


.code

__log_default PROC
jmp __imp__log
__log_default ENDP

end