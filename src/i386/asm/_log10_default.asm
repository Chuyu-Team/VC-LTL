.model	flat


;_log10_default -> log10
extern __imp__log10:PROC

.code

__log10_default PROC
jmp __imp__log10
__log10_default ENDP


END