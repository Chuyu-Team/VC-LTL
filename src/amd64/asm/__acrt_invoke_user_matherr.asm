

;_matherr 与 __acrt_invoke_user_matherr行为完全等价
extern _matherr:qword


.code

__acrt_invoke_user_matherr PROC
jmp _matherr
__acrt_invoke_user_matherr ENDP

end