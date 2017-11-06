;VC-LTL x86汇编部分

.model	flat

_TEXT	SEGMENT

;定义此函数是因为新版本中terminate规范变成了C，此代码用于解决terminate命名冲突问题
extern __imp_?terminate@@YAXXZ:dword
_terminate PROC
jmp __imp_?terminate@@YAXXZ
_terminate ENDP

;__std_terminate -> C++ void terminate()
___std_terminate PROC
jmp __imp_?terminate@@YAXXZ
___std_terminate ENDP

;_wcstoui64 与 wcstoull行为完全等价
extern __imp___wcstoui64:dword
_wcstoull PROC
jmp __imp___wcstoui64
_wcstoull ENDP


;_wcstoi64 与 wcstoll行为完全等价
extern __imp___wcstoi64:dword
_wcstoll PROC
jmp __imp___wcstoi64
_wcstoll ENDP

;_strtoi64 与 strtoll行为完全等价
extern __imp___strtoi64:dword
_strtoll PROC
jmp __imp___strtoi64
_strtoll ENDP

;_strtoui64 与 strtoull行为完全等价
extern __imp___strtoui64:dword
_strtoull PROC
jmp __imp___strtoui64
_strtoull ENDP

;_matherr 与 __acrt_invoke_user_matherr行为完全等价
extern __matherr:dword
___acrt_invoke_user_matherr PROC
jmp __matherr
___acrt_invoke_user_matherr ENDP


;新编译器_fstat已经改名为_fstat32
extern __imp___fstat:dword
__fstat32 PROC
jmp __imp___fstat
__fstat32 ENDP


;新编译器_fstati64已经改名为_fstat32i64
extern __imp___fstati64:dword
__fstat32i64 PROC
jmp __imp___fstati64
__fstat32i64 ENDP


;新编译器_stat已经改名为_stat32
extern __imp___stat:dword
__stat32 PROC
jmp __imp___stat
__stat32 ENDP


;新编译器_stati64已经改名为_stat32i64
extern __imp___stati64:dword
__stat32i64 PROC
jmp __imp___stati64
__stat32i64 ENDP

;新编译器_wstat已经改名为_wstat32
extern __imp___wstat:dword
__wstat32 PROC
jmp __imp___wstat
__wstat32 ENDP

;新编译器_wstati64已经改名为_wstat32i64
extern __imp___wstati64:dword
__wstat32i64 PROC
jmp __imp___wstati64
__wstat32i64 ENDP


;新编译器_findfirst已经改名为_findfirst32
extern __imp___findfirst:dword
__findfirst32 PROC
jmp __imp___findfirst
__findfirst32 ENDP

;新编译器_findnext已经改名为_findnext32
extern __imp___findnext:dword
__findnext32 PROC
jmp __imp___findnext
__findnext32 ENDP

;新编译器_findfirsti64已经改名为_findfirst32i64
extern __imp___findfirsti64:dword
__findfirst32i64 PROC
jmp __imp___findfirsti64
__findfirst32i64 ENDP

;新编译器_findnexti64已经改名为_findnext32i64
extern __imp___findnexti64:dword
__findnext32i64 PROC
jmp __imp___findnexti64
__findnext32i64 ENDP

;新编译器_wfindfirst已经改名为_wfindfirst32
extern __imp___wfindfirst:dword
__wfindfirst32 PROC
jmp __imp___wfindfirst
__wfindfirst32 ENDP

;新编译器_wfindnext已经改名为_wfindnext32
extern __imp___wfindnext:dword
__wfindnext32 PROC
jmp __imp___wfindnext
__wfindnext32 ENDP

;新编译器_wfindfirsti64已经改名为_wfindfirst32i64
extern __imp___wfindfirsti64:dword
__wfindfirst32i64 PROC
jmp __imp___wfindfirsti64
__wfindfirst32i64 ENDP


;新编译器_wfindnexti64已经改名为_wfindnext32i64
extern __imp___wfindnexti64:dword
__wfindnext32i64 PROC
jmp __imp___wfindnexti64
__wfindnext32i64 ENDP

;以下函数仅32位存在

;_acos_default -> acos
extern __imp__acos:dword
__acos_default PROC
jmp __imp__acos
__acos_default ENDP

;_asin_default -> asin
extern __imp__asin:dword
__asin_default PROC
jmp __imp__asin
__asin_default ENDP

;_atan_default -> atan
extern __imp__atan:dword
__atan_default PROC
jmp __imp__atan
__atan_default ENDP

;_cos_default -> cos
extern __imp__cos:dword
__cos_default PROC
jmp __imp__cos
__cos_default ENDP

;_exp_default -> exp
extern __imp__exp:dword
__exp_default PROC
jmp __imp__exp
__exp_default ENDP

;_pow_default -> pow
extern __imp__pow:dword
__pow_default PROC
jmp __imp__pow
__pow_default ENDP

;_sin_default -> sin
extern __imp__sin:dword
__sin_default PROC
jmp __imp__sin
__sin_default ENDP

;_tan_default -> tan
extern __imp__tan:dword
__tan_default PROC
jmp __imp__tan
__tan_default ENDP

;_log_default -> log
extern __imp__log:dword
__log_default PROC
jmp __imp__log
__log_default ENDP

;_log10_default -> log10
extern __imp__log10:dword
__log10_default PROC
jmp __imp__log10
__log10_default ENDP

_TEXT	ENDS

END