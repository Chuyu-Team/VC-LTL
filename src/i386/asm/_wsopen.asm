.model	flat


;定义此函数是因为新版本中_wsopen有一份C++版，此代码用于解决_wsopen命名冲突问题
extern __imp___wsopen:dword

.code

?_wsopen@@YAHPB_WHHH@Z PROC
jmp __imp___wsopen
?_wsopen@@YAHPB_WHHH@Z ENDP

end