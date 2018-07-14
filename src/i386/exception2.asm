.model	flat

;std::exception::exception(char const* const _Message, int) -> exception::exception(char const* const& _Message, int)
extern __imp_??0exception@@QAE@ABQBDH@Z:dword

.code
_Message = 8
bFree    = 12
??0exception@std@@QAE@QBDH@Z PROC
push    ebp
mov     ebp,esp
lea     eax, [ebp+_Message]
push    0
push    eax
call    dword ptr ds:__imp_??0exception@@QAE@ABQBDH@Z
mov     esp,ebp
pop     ebp
retn    8
??0exception@std@@QAE@QBDH@Z ENDP

end