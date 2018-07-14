
;std::exception::exception(char const* const _Message, int) -> exception::exception(char const* const& _Message, int)
extern __imp_??0exception@@QEAA@AEBQEBDH@Z:qword

.code
??0exception@std@@QEAA@QEBDH@Z PROC
mov     qword ptr [rsp+10h],rdx
sub     rsp,28h
lea     rdx, [rsp+28h+10h]
xor     r8d,r8d
call    qword ptr[__imp_??0exception@@QEAA@AEBQEBDH@Z]
add     rsp,28h
ret
??0exception@std@@QEAA@QEBDH@Z ENDP

end