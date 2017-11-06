.model	flat


;_asin_default -> asin
extern __imp__asin:dword


.code

__asin_default PROC
jmp __imp__asin
__asin_default ENDP

end