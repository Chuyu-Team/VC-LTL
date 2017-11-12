.model	flat


;定义此函数是因为新版本中_wopen有一份C++版，此代码用于解决_wopen命名冲突问题
extern __imp___wopen:dword

.code

?_wopen@@YAHPB_WHH@Z PROC
jmp __imp___wopen
?_wopen@@YAHPB_WHH@Z ENDP

end