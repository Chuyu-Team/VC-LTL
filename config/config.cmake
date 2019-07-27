cmake_minimum_required(VERSION 3.5.2)

#VC-LTL核心版本号，由于4.X并不兼容3.X。此值可以用于兼容性判断。
set(LTL_CoreVersion 4)

if(NOT SupportWinXP)
	set(SupportWinXP "false")
endif()

if(NOT DisableAdvancedSupport)
	set(DisableAdvancedSupport "false")
endif()

if(NOT SupportLTL)
	set(SupportLTL "true")
endif()

if(${SupportLTL} STREQUAL "true")
	if(NOT CMAKE_BUILD_TYPE)
		message(WARNING "VC-LTL not load, because CMAKE_BUILD_TYPE is not defined!!!")
		set(SupportLTL "false")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
		message(WARNING "VC-LTL not load in Debug!!!")
		set(SupportLTL "false")
	endif()
endif()

if(${SupportLTL} STREQUAL "true")
	if(NOT CMAKE_SYSTEM_NAME)
		message(WARNING "VC-LTL not load, because CMAKE_SYSTEM_NAME is not defined!!!")
		set(SupportLTL "false")
	elseif(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		message(WARNING "VC-LTL not load, because ${CMAKE_SYSTEM_NAME} is not supported!!!")
		set(SupportLTL "false")
	endif()
endif()

if(${SupportLTL} STREQUAL "true")
	if(NOT CMAKE_MFC_FLAG)
		#不是MFC
	elseif(${CMAKE_MFC_FLAG} GREATER 0)
		message(WARNING "VC-LTL not load, because MFC is not supported!!!")
		set(SupportLTL "false")
	endif()
endif()

if(${SupportLTL} STREQUAL "true")
	if(PlatformShortName)
		#外部已经定义
	elseif(CMAKE_GENERATOR_PLATFORM)
		# -A 参数已经传递，仅在 CMake 3.1更高版本中支持
		message("CMAKE_GENERATOR_PLATFORM = " ${CMAKE_GENERATOR_PLATFORM})
		if(${CMAKE_GENERATOR_PLATFORM} STREQUAL "Win32")
			set(PlatformShortName "x86")
		elseif(${CMAKE_GENERATOR_PLATFORM} STREQUAL "x64")
			set(PlatformShortName "x64")
		elseif(${CMAKE_GENERATOR_PLATFORM} STREQUAL "arm")
			set(PlatformShortName "arm")
		elseif(${CMAKE_GENERATOR_PLATFORM} STREQUAL "arm64")
			set(PlatformShortName "arm64")
		else()
			message(WARNING "VC-LTL not load, Unknown Platform!!!")
			set(SupportLTL "false")
		endif()
	elseif(CMAKE_VS_PLATFORM_NAME)
		# CMake 3.1以及更早版本不支持 -A 参数，因此通过 CMAKE_VS_PLATFORM_NAME解决
		message("CMAKE_VS_PLATFORM_NAME = " ${CMAKE_VS_PLATFORM_NAME})
		if(${CMAKE_VS_PLATFORM_NAME} STREQUAL "Win32")
			set(PlatformShortName "x86")
		elseif(${CMAKE_VS_PLATFORM_NAME} STREQUAL "x64")
			set(PlatformShortName "x64")
		elseif(${CMAKE_VS_PLATFORM_NAME} STREQUAL "arm")
			set(PlatformShortName "arm")
		elseif(${CMAKE_VS_PLATFORM_NAME} STREQUAL "arm64")
			set(PlatformShortName "arm64")
		else()
			message(WARNING "VC-LTL not load, Unknown Platform!!!")
			set(SupportLTL "false")
		endif()
	elseif(MSVC_VERSION)
		message("load CheckSymbolExists......")

		include(CheckSymbolExists)

		check_symbol_exists("_M_IX86" "" _M_IX86)
		check_symbol_exists("_M_AMD64" "" _M_AMD64)
		check_symbol_exists("_M_ARM" "" _M_ARM)
		check_symbol_exists("_M_ARM64" "" _M_ARM64)

		if(_M_AMD64)
			set(PlatformShortName "x64")
		elseif(_M_IX86)
			set(PlatformShortName "x86")
		elseif(_M_ARM)
			set(PlatformShortName "arm")
		elseif(_M_ARM64)
			set(PlatformShortName "arm64")
		else()
			message(WARNING "VC-LTL not load, Unknown Platform!!!")
			set(SupportLTL "false")
		endif()
	elseif(DEFINED ENV{VSCMD_ARG_TGT_ARCH})
		#VSCMD_ARG_TGT_ARCH参数只有2017才有，因此兼容性更差
		message("VSCMD_ARG_TGT_ARCH = " $ENV{VSCMD_ARG_TGT_ARCH})
		set(PlatformShortName $ENV{VSCMD_ARG_TGT_ARCH})
	elseif(DEFINED ENV{LIB})
		#采用更加奇葩发方式，检测lib目录是否包含特定后缀
		message("LIB = $ENV{LIB}")

		string(TOLOWER "$ENV{LIB}" LTL_LIB_TMP)

		if("${LTL_LIB_TMP}" MATCHES "\\x64;")
			set(PlatformShortName "x64")
		elseif("${LTL_LIB_TMP}" MATCHES "\\x86;")
			set(PlatformShortName "x86")
		elseif("${LTL_LIB_TMP}" MATCHES "\\arm;")
			set(PlatformShortName "arm")
		elseif("${LTL_LIB_TMP}" MATCHES "\\arm64;")
			set(PlatformShortName "arm64")
		elseif("${LTL_LIB_TMP}" MATCHES "\\lib;")
			#为了兼容VS 2015
			set(PlatformShortName "x86")
		else()
			message(WARNING "VC-LTL not load, Unknown Platform!!!")
			set(SupportLTL "false")
		endif()
	else()
		message(WARNING "VC-LTL not load, Unknown Platform!!!")
		set(SupportLTL "false")
	endif()
endif()

if(${SupportLTL} STREQUAL "true")
	#获取VC版本号
	if(NOT VCLTLToolsVersion)
		if(MSVC_VERSION)
			if(${MSVC_VERSION} EQUAL 1900)
				set(VCLTLVisualStudioVersionTmp "14.0")
			elseif(${MSVC_VERSION} LESS 1920)
				set(VCLTLVisualStudioVersionTmp "15.0")
            elseif(${MSVC_VERSION} LESS 1930)
				set(VCLTLVisualStudioVersionTmp "16.0")
			else()
				set(VCLTLVisualStudioVersionTmp "")
				message(WARNING "VC-LTL not load, Unknown MSVC_VERSION = " ${MSVC_VERSION})
			endif()
		elseif(DEFINED ENV{VisualStudioVersion})
			set(VCLTLVisualStudioVersionTmp $ENV{VisualStudioVersion})
		else()
			set(VCLTLVisualStudioVersionTmp "")
			message(WARNING "VC-LTL not load, Unknown MSVC_VERSION")
		endif()

		if(${VCLTLVisualStudioVersionTmp} STREQUAL "14.0")
			set(DefaultVCLTLToolsVersion "14.0.24231")

			#VC 2015只能自己读取注册表解决

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24234（Visual Studio 2017 15.7中的2015平台工具集），与14.0.24231完全一致
			if(NOT VCLTLToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{BDE574B5-6CFE-32B2-9854-C827567E9D6F};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{BDE574B5-6CFE-32B2-9854-C827567E9D6F};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VCLTLToolsVersion "14.0.24231")
				endif()
			endif()

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24231（Visual Studio 2017 15.6中的2015平台工具集）
			if(NOT VCLTLToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B0791F3A-6A88-3650-AECF-8AFBE227EC53};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B0791F3A-6A88-3650-AECF-8AFBE227EC53};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VCLTLToolsVersion "14.0.24231")
				endif()
			endif()

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24225（Visual Studio 2017 15.5中的2015平台工具集）
			if(NOT VCLTLToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VCLTLToolsVersion "14.0.24225")
				endif()
			endif()

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24210（正统Visual Studio 2015）
			if(NOT VCLTLToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VCLTLToolsVersion "14.0.24210")
				endif()
			endif()
		elseif(${VCLTLVisualStudioVersionTmp} STREQUAL "15.0")
			#VC2017
			set(DefaultVCLTLToolsVersion "14.16.27023")

			if(DEFINED ENV{VCToolsVersion})
				set(VCLTLToolsVersion $ENV{VCToolsVersion})
			endif()
		elseif(${VCLTLVisualStudioVersionTmp} STREQUAL "16.0")
			#VC2017
			set(DefaultVCLTLToolsVersion "14.22.27905")

			if(DEFINED ENV{VCToolsVersion})
				set(VCLTLToolsVersion $ENV{VCToolsVersion})
			endif()
		else()
			set(SupportLTL "false")
		endif()

		if(${SupportLTL} STREQUAL "true")
			if(NOT VCLTLToolsVersion)
				set(VCLTLToolsVersion ${DefaultVCLTLToolsVersion})
			elseif(NOT EXISTS ${VC_LTL_Root}/VC/${VCLTLToolsVersion})
				set(VCLTLToolsVersion ${DefaultVCLTLToolsVersion})
			endif()
		endif()

	endif()
endif()


if(${SupportLTL} STREQUAL "true")

	#获取UCRT版本，如果当前环境变量UCRTVersion说对应的版本VC-LTL支持，那么使用该版本，否则使用10.0.10240.0
   if(NOT VCLTLTargetUniversalCRTVersion)
	   if(DEFINED ENV{UCRTVersion})
			if(EXISTS ${VC_LTL_Root}/ucrt/$ENV{UCRTVersion})
				set(VCLTLTargetUniversalCRTVersion $ENV{UCRTVersion})
			endif()
		endif()

		if(NOT VCLTLTargetUniversalCRTVersion)
			#设置默认UCRT版本号
			set(VCLTLTargetUniversalCRTVersion "10.0.10240.0")
		endif()
	endif()

	if (NOT EXISTS ${VC_LTL_Root}/lib/${PlatformShortName})
		message(FATAL_ERROR "VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
	endif()
	if (NOT EXISTS ${VC_LTL_Root}/VC/${VCLTLToolsVersion}/lib)
		message(FATAL_ERROR "VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
	endif()
	if (NOT EXISTS ${VC_LTL_Root}/ucrt/${VCLTLTargetUniversalCRTVersion}/lib/${PlatformShortName})
		message(FATAL_ERROR "VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
	endif()

	#设置VCLTLPlatformName
	if(${SupportWinXP} STREQUAL "true")
		set(VCLTLPlatformName "WinXP")
	else()
		set(VCLTLPlatformName "Vista")
	endif()

	if(${PlatformShortName} STREQUAL "arm")
		set(VCLTLPlatformName "Vista")
	endif()
	if(${PlatformShortName} STREQUAL "arm64")
		set(VCLTLPlatformName "Vista")
	endif()

	#设置LTL_Mode
	if(${DisableAdvancedSupport} STREQUAL "true")
		set(LTL_Mode "Light")
	else()
		set(LTL_Mode "Advanced")
	endif()

	#打印VC-LTL图标
	message("###################################################################################################")
	message("#                                                                                                 #")
	message("#                 *         *      * *             *        * * * * *  *                          #")
	message("#                  *       *     *                 *            *      *                          #")
	message("#                   *     *     *       * * * * *  *            *      *                          #")
	message("#                    *   *       *                 *            *      *                          #")
	message("#                      *           * *             * * * *      *      * * * *                    #")
	message("#                                                                                                 #")
	message("###################################################################################################")

	message("")

	#打印VC-LTL基本信息
	message(" VC-LTL Path          :" ${VC_LTL_Root})
	message(" VC-LTL Tools Version :" ${VCLTLToolsVersion})
	message(" VC-LTL UCRT Version  :" ${VCLTLTargetUniversalCRTVersion})
	message(" Platform             :" ${PlatformShortName})
	message(" Using VC-LTL " ${VCLTLPlatformName} " " ${LTL_Mode} " Mode")
	message("")

    set(VC_LTL_Include ${VC_LTL_Root}/config/${VCLTLPlatformName};${VC_LTL_Root}/VC/${VCLTLToolsVersion}/include;${VC_LTL_Root}/VC/${VCLTLToolsVersion}/atlmfc/include;${VC_LTL_Root}/ucrt/${VCLTLTargetUniversalCRTVersion})
    set(VC_LTL_Library ${VC_LTL_Root}/lib/${PlatformShortName}/${VCLTLPlatformName};${VC_LTL_Root}/lib/${PlatformShortName}/${VCLTLPlatformName}/${LTL_Mode};${VC_LTL_Root}/VC/${VCLTLToolsVersion}/lib/${PlatformShortName};${VC_LTL_Root}/VC/${VCLTLToolsVersion}/lib/${PlatformShortName}/${VCLTLPlatformName};${VC_LTL_Root}/ucrt/${VCLTLTargetUniversalCRTVersion}/lib/${PlatformShortName})

	#message("INCLUDE " $ENV{INCLUDE})
	#message("LIB " $ENV{LIB})

	#set( ENV{INCLUDE} ${VC_LTL_Include};$ENV{INCLUDE})
	#set( ENV{LIB} ${VC_LTL_Library};$ENV{LIB})


	#message("INCLUDE " $ENV{INCLUDE})
	#message("LIB " $ENV{LIB})

    include_directories(BEFORE SYSTEM ${VC_LTL_Include})
    link_directories(${VC_LTL_Library})

	#message("INCLUDE " $ENV{INCLUDE})
	#message("LIB " $ENV{LIB})

    if(${SupportWinXP} STREQUAL "true")
       if(${PlatformShortName} STREQUAL "x86")
			set(LTL_CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.01")
			set(LTL_CMAKE_CREATE_CONSOLE_EXE "/subsystem:console,5.01")

			set(CMAKE_CREATE_WIN32_EXE "${CMAKE_CREATE_WIN32_EXE} ${LTL_CMAKE_CREATE_WIN32_EXE}")
			set(CMAKE_CREATE_CONSOLE_EXE "${CMAKE_CREATE_CONSOLE_EXE} ${LTL_CMAKE_CREATE_CONSOLE_EXE}")
		elseif(${PlatformShortName} STREQUAL "x64")
			set(LTL_CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.02")
			set(LTL_CMAKE_CREATE_CONSOLE_EXE "/subsystem:console,5.02")

			set(CMAKE_CREATE_WIN32_EXE "${CMAKE_CREATE_WIN32_EXE} ${LTL_CMAKE_CREATE_WIN32_EXE}")
			set(CMAKE_CREATE_CONSOLE_EXE "${CMAKE_CREATE_CONSOLE_EXE} ${LTL_CMAKE_CREATE_CONSOLE_EXE}")
		endif()
    endif()
endif()