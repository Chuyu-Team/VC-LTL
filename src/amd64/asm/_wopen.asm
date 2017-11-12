

;定义此函数是因为新版本中_wopen有一份C++版，此代码用于解决_wopen命名冲突问题
extern __imp__wopen:qword

.code

?_wopen@@YAHPEB_WHH@Z PROC
jmp __imp__wopen
?_wopen@@YAHPEB_WHH@Z ENDP

end