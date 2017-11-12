

;定义此函数是因为新版本中_sopen有一份C++版，此代码用于解决_sopen命名冲突问题
extern __imp__sopen:qword

.code

?_sopen@@YAHQEBDHHH@Z PROC
jmp __imp__sopen
?_sopen@@YAHQEBDHHH@Z ENDP

end