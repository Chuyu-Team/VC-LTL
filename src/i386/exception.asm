.model	flat

;std::exception::exception(char const* const _Message) -> exception::exception(char const* const& _Message)
extern __imp_??0exception@@QAE@ABQBD@Z:dword

.code
_Message = 8

??0exception@std@@QAE@QBD@Z PROC
push    ebp
mov     ebp,esp
lea     eax, [ebp+_Message]
push    eax
call    dword ptr ds:__imp_??0exception@@QAE@ABQBD@Z
mov     esp,ebp
pop     ebp
retn    4
??0exception@std@@QAE@QBD@Z ENDP

end