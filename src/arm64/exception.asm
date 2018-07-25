
;std::exception::exception(char const* const _Message) -> exception::exception(char const* const& _Message)

	EXPORT	|??0exception@std@@QEAA@QEBD@Z|
	IMPORT  |??0exception@@QEAA@AEBQEBD@Z|

|??0exception@std@@QEAA@QEBD@Z| PROC
    str         lr,[sp,#-0x10]!
	sub         sp,sp,#0x10
	str         x1,[sp]
	mov         x1,sp
	bl          |??0exception@@QEAA@AEBQEBD@Z|
	add         sp,sp,#0x10
	ldr         lr,[sp],#0x10
	ret
	ENDP

	END