;VC-LTL amd64汇编部分



_TEXT	SEGMENT

;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题
extern __imp_?terminate@@YAXXZ:qword
terminate PROC
jmp __imp_?terminate@@YAXXZ
terminate ENDP

;__std_terminate -> C++ void terminate()
__std_terminate PROC
jmp __imp_?terminate@@YAXXZ
__std_terminate ENDP

;_wcstoui64 与 wcstoull行为完全等价
extern __imp__wcstoui64:qword
wcstoull PROC
jmp __imp__wcstoui64
wcstoull ENDP

;_wcstoi64 与 wcstoll行为完全等价
extern __imp__wcstoi64:qword
wcstoll PROC
jmp __imp__wcstoi64
wcstoll ENDP

;_strtoi64 与 strtoll行为完全等价
extern __imp__strtoi64:qword
strtoll PROC
jmp __imp__strtoi64
strtoll ENDP

;_strtoui64 与 strtoull行为完全等价
extern __imp__strtoui64:qword
strtoull PROC
jmp __imp__strtoui64
strtoull ENDP

;_matherr 与 __acrt_invoke_user_matherr行为完全等价
extern _matherr:qword
__acrt_invoke_user_matherr PROC
jmp _matherr
__acrt_invoke_user_matherr ENDP


;新编译器_fstat已经改名为_fstat32
extern __imp__fstat:qword
_fstat32 PROC
jmp __imp__fstat
_fstat32 ENDP


;新编译器_fstati64已经改名为_fstat32i64
extern __imp__fstati64:qword
_fstat32i64 PROC
jmp __imp__fstati64
_fstat32i64 ENDP


;新编译器_stat已经改名为_stat32
extern __imp__stat:qword
_stat32 PROC
jmp __imp__stat
_stat32 ENDP


;新编译器_stati64已经改名为_stat32i64
extern __imp__stati64:qword
_stat32i64 PROC
jmp __imp__stati64
_stat32i64 ENDP

;新编译器_wstat已经改名为_wstat32
extern __imp__wstat:qword
_wstat32 PROC
jmp __imp__wstat
_wstat32 ENDP

;新编译器_wstati64已经改名为_wstat32i64
extern __imp__wstati64:qword
_wstat32i64 PROC
jmp __imp__wstati64
_wstat32i64 ENDP

;新编译器_findfirst已经改名为_findfirst32
extern __imp__findfirst:qword
_findfirst32 PROC
jmp __imp__findfirst
_findfirst32 ENDP

;新编译器_findnext已经改名为_findnext32
extern __imp__findnext:qword
_findnext32 PROC
jmp __imp__findnext
_findnext32 ENDP

;新编译器_findfirsti64已经改名为_findfirst32i64
extern __imp__findfirsti64:qword
_findfirst32i64 PROC
jmp __imp__findfirsti64
_findfirst32i64 ENDP

;新编译器_findnexti64已经改名为_findnext32i64
extern __imp__findnexti64:qword
_findnext32i64 PROC
jmp __imp__findnexti64
_findnext32i64 ENDP

;新编译器_wfindfirst已经改名为_wfindfirst32
extern __imp__wfindfirst:qword
_wfindfirst32 PROC
jmp __imp__wfindfirst
_wfindfirst32 ENDP

;新编译器_wfindnext已经改名为_wfindnext32
extern __imp__wfindnext:qword
_wfindnext32 PROC
jmp __imp__wfindnext
_wfindnext32 ENDP

;新编译器_wfindfirsti64已经改名为_wfindfirst32i64
extern __imp__wfindfirsti64:qword
_wfindfirst32i64 PROC
jmp __imp__wfindfirsti64
_wfindfirst32i64 ENDP


;新编译器_wfindnexti64已经改名为_wfindnext32i64
extern __imp__wfindnexti64:qword
_wfindnext32i64 PROC
jmp __imp__wfindnexti64
_wfindnext32i64 ENDP

_TEXT	ENDS

END