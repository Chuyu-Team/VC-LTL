#
#  请不要直接使用此配置，应该使用VC-LTL helper for qmake.pri
#

SUPPORTED_PLATFORM_LIST = x86 x64 arm arm64

VC_TOOLS_VERSION_LIST = 14.0.24231:BDE574B5-6CFE-32B2-9854-C827567E9D6F \
                        14.0.24231:B0791F3A-6A88-3650-AECF-8AFBE227EC53 \
                        14.0.24225:4B1849F2-3D49-325F-B997-4AD0BF5B8A09 \
                        14.0.24210:729FD64C-2AE0-3E25-83A8-A93520DCDE7A


!exists($$VC_LTL_Root/lib/$$(Platform)) {
    error("VC-LTL: Cannot find lib files, please download latest pre-build packages from https://github.com/Chuyu-Team/VC-LTL/releases/latest")
}

PlatformShortName = $$(Platform)

isEmpty(PlatformShortName) {
    PlatformShortName = x86
} else {
    !contains(SUPPORTED_PLATFORM_LIST, $$PlatformShortName) {
        error("VC-LTL: Unsupported platform: $$PlatformShortName")
    }
}


# 默认VC工具集版本
DefaultVCLTLToolsVersion = $$(VisualStudioVersion)

equals(DefaultVCLTLToolsVersion, 14.0) {
    DefaultVCLTLToolsVersion = 14.0.24231
} else:equals(DefaultVCLTLToolsVersion, 15.0) {
    DefaultVCLTLToolsVersion = 14.16.27023
} else:equals(DefaultVCLTLToolsVersion, 16.0) {
    DefaultVCLTLToolsVersion = 14.22.27905
} else {
    error("VC-LTL: For Visual Studio 2015 , 2017 or 2019 only")
}


# 搜索VC工具集版本
VCLTLToolsVersion = $$(VCToolsVersion)

isEmpty(VCLTLToolsVersion) {
    for (i, VC_TOOLS_VERSION_LIST) {
        p = $$split(i, :)
        
        system(reg query HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{$$last(p)} /v DisplayVersion) {
            VCLTLToolsVersion = $$first(p)
			break()
        }
    }
}

# 如果找不到，那么指定为当前默认版本
isEmpty(VCLTLToolsVersion) | !exists($$VC_LTL_Root/VC/$$VCLTLToolsVersion) {
    VCLTLToolsVersion = $$DefaultVCLTLToolsVersion
}


# 搜索UCRT版本
VCLTLTargetUniversalCRTVersion = $$(UCRTVersion)

isEmpty(VCLTLTargetUniversalCRTVersion) | !exists($$VC_LTL_Root/ucrt/$$VCLTLTargetUniversalCRTVersion) {
    VCLTLTargetUniversalCRTVersion = 10.0.10240.0
}

# VC-LTL核心版本号，由于4.X并不兼容3.X。此值可以用于兼容性判断。
LTL_CoreVersion = 4

# 环境变量选项
t = $$(SupportWinXP)
equals(t, true) {
    VCLTLPlatformName = WinXP
} else {
    VCLTLPlatformName = Vista
}

equals(PlatformShortName, arm) | equals(PlatformShortName, arm64) {
    VCLTLPlatformName = Vista
}

t = $$(DisableAdvancedSupport)
equals(t, true) {
    LTL_Mode = Light
} else {
    LTL_Mode = Advanced
}

!exists($$VC_LTL_Root/lib/$$PlatformShortName) {
    error("VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
}
!exists($$VC_LTL_Root/VC/$$VCLTLToolsVersion/lib) {
    error("VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
}
!exists($$VC_LTL_Root/ucrt/$$VCLTLTargetUniversalCRTVersion/lib/$$PlatformShortName) {
    error("VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
}


# 打印信息
VC_LTL_Info = \
$$escape_expand(\\n)Using VC-LTL $$VCLTLPlatformName $$LTL_Mode Mode \
$$escape_expand(\\n)**************************************** \
$$escape_expand(\\n)VC-LTL Path : $$VC_LTL_Root \
$$escape_expand(\\n)VC-LTL Tools Version : $$VCLTLToolsVersion \
$$escape_expand(\\n)VC-LTL UCRT Version : $$VCLTLTargetUniversalCRTVersion \
$$escape_expand(\\n)Platform : $$PlatformShortName \
$$escape_expand(\\n)****************************************

message($$VC_LTL_Info)


# 修改头文件及库搜索路径
QMAKE_INCDIR += \
    $$VC_LTL_Root/config/$$VCLTLPlatformName \
    $$VC_LTL_Root/VC/$$VCLTLToolsVersion/include \
    $$VC_LTL_Root/VC/$$VCLTLToolsVersion/atlmfc/include \
    $$VC_LTL_Root/ucrt/$$VCLTLTargetUniversalCRTVersion

QMAKE_LIBS += \
    -L$$VC_LTL_Root/lib/$$PlatformShortName/$$VCLTLPlatformName \
    -L$$VC_LTL_Root/lib/$$PlatformShortName/$$VCLTLPlatformName/$$LTL_Mode \
    -L$$VC_LTL_Root/VC/$$VCLTLToolsVersion/lib/$$PlatformShortName \
    -L$$VC_LTL_Root/VC/$$VCLTLToolsVersion/lib/$$PlatformShortName/$$VCLTLPlatformName \
    -L$$VC_LTL_Root/ucrt/$$VCLTLTargetUniversalCRTVersion/lib/$$PlatformShortName

