
;std::exception::exception(char const* const _Message) -> exception::exception(char const* const& _Message)
extern __imp_??0exception@@QEAA@AEBQEBD@Z:qword

.code

??0exception@std@@QEAA@QEBD@Z PROC
mov     qword ptr [rsp+10h],rdx
sub     rsp,28h
lea     rdx, [rsp+ 28h +10h]
call    qword ptr[__imp_??0exception@@QEAA@AEBQEBD@Z]
add     rsp,28h
ret
??0exception@std@@QEAA@QEBD@Z ENDP

end