
;std::exception::exception(char const* const _Message) -> exception::exception(char const* const& _Message)

	EXPORT	|??0exception@std@@QAA@QBD@Z|
	IMPORT  |??0exception@@QAA@ABQBD@Z|

|??0exception@std@@QAA@QBD@Z| PROC
	push        {r0-r3}
	push        {r3,lr}
	add         r1,sp,#0xC
	bl          |??0exception@@QAA@ABQBD@Z|
	add         sp,sp,#4
	ldr         pc,[sp],#0x14
	ENDP

	END