.model	flat


;_matherr 与 __acrt_invoke_user_matherr行为完全等价
extern __matherr:dword

.code

___acrt_invoke_user_matherr PROC
jmp __matherr
___acrt_invoke_user_matherr ENDP

end
