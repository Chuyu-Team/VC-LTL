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


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(WARNING "VC-LTL not load in Debug!!!")
	set(SupportLTL "false")
endif()


if(${SupportLTL} STREQUAL "true")

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
endif()

if(${SupportLTL} STREQUAL "true")
	if (NOT EXISTS ${VC_LTL_Root}/lib/${PlatformShortName})
		message(FATAL_ERROR "VC-LTL can't find lib files, please download the binary files from https://github.com/Chuyu-Team/VC-LTL/releases/latest then continue.")
	endif()
endif()

if(${SupportLTL} STREQUAL "true")
	#获取VC版本号
	if(NOT VC-LTLUsedToolsVersion)
		if(${MSVC_VERSION} EQUAL 1900)
			set(__DefaultVCToolsVersion "14.0.24231")

			#VC 2015只能自己读取注册表解决

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24234（Visual Studio 2017 15.7中的2015平台工具集），与14.0.24231完全一致
			if(NOT VC-LTLUsedToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{BDE574B5-6CFE-32B2-9854-C827567E9D6F};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{BDE574B5-6CFE-32B2-9854-C827567E9D6F};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VC-LTLUsedToolsVersion "14.0.24231")
				endif()
			endif()

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24231（Visual Studio 2017 15.6中的2015平台工具集）
			if(NOT VC-LTLUsedToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B0791F3A-6A88-3650-AECF-8AFBE227EC53};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B0791F3A-6A88-3650-AECF-8AFBE227EC53};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VC-LTLUsedToolsVersion "14.0.24231")
				endif()
			endif()

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24225（Visual Studio 2017 15.5中的2015平台工具集）
			if(NOT VC-LTLUsedToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VC-LTLUsedToolsVersion "14.0.24225")
				endif()
			endif()

			#计算机已经安装Visual Studio 2015 Update3 v14.0.24210（正统Visual Studio 2015）
			if(NOT VC-LTLUsedToolsVersion)
				GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A};DisplayVersion]" NAME)
				if (${LTLDisplayVersionTmp} STREQUAL "registry")
					GET_FILENAME_COMPONENT(LTLDisplayVersionTmp "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A};DisplayVersion]" NAME)
				endif()

				if (NOT ${LTLDisplayVersionTmp} STREQUAL "registry")
					set(VC-LTLUsedToolsVersion "14.0.24210")
				endif()
			endif()

		elseif(${MSVC_VERSION} LESS 1920)
			#VC2017
			set(__DefaultVCToolsVersion "14.16.27023")

			if(DEFINED ENV{VCToolsVersion})
				set(VC-LTLUsedToolsVersion $ENV{VCToolsVersion})
			endif()

		else()
			message(WARNING "VC-LTL not load, Unknown MSVC_VERSION=" ${MSVC_VERSION})
			set(SupportLTL "false")
		endif()

		if(${SupportLTL} STREQUAL "true")
			if(NOT VC-LTLUsedToolsVersion)
				set(VC-LTLUsedToolsVersion ${__DefaultVCToolsVersion})
			elseif(NOT EXISTS ${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion})
				set(VC-LTLUsedToolsVersion ${__DefaultVCToolsVersion})
			endif()
		endif()

	endif()
endif()


if(${SupportLTL} STREQUAL "true")

	#获取UCRT版本，如果当前环境变量UCRTVersion说对应的版本VC-LTL支持，那么使用该版本，否则使用10.0.10240.0
   if(NOT VC-LTLTargetUniversalCRTVersion)
	   if(DEFINED ENV{UCRTVersion})
			if(EXISTS ${VC_LTL_Root}/ucrt/$ENV{UCRTVersion})
				set(VC-LTLTargetUniversalCRTVersion $ENV{UCRTVersion})
			endif()
		endif()

		if(NOT VC-LTLTargetUniversalCRTVersion)
			#设置默认UCRT版本号
			set(VC-LTLTargetUniversalCRTVersion "10.0.10240.0")
		endif()
	endif()


	#设置OsPlatformName
	if(${SupportWinXP} STREQUAL "true")
		set(OsPlatformName "WinXP")
	else()
		set(OsPlatformName "Vista")
	endif()

	if(${PlatformShortName} STREQUAL "arm")
		set(OsPlatformName "Vista")
	endif()
	if(${PlatformShortName} STREQUAL "arm64")
		set(OsPlatformName "Vista")
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
	message(" VC-LTL Tools Version :" ${VC-LTLUsedToolsVersion})
	message(" VC-LTL UCRT Version  :" ${VC-LTLTargetUniversalCRTVersion})
	message(" Platform             :" ${PlatformShortName})
	message(" Using VC-LTL " ${OsPlatformName} " " ${LTL_Mode} " Mode")
	message("")

    set(VC_LTL_Include ${VC_LTL_Root}/config/${OsPlatformName};${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion}/include;${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion}/atlmfc/include;${VC_LTL_Root}/ucrt/${VC-LTLTargetUniversalCRTVersion})
    set(VC_LTL_Library ${VC_LTL_Root}/lib/${PlatformShortName}/${OsPlatformName};${VC_LTL_Root}/lib/${PlatformShortName}/${OsPlatformName}/${LTL_Mode};${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion}/lib/${PlatformShortName};${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion}/lib/${PlatformShortName}/${OsPlatformName};${VC_LTL_Root}/ucrt/${VC-LTLTargetUniversalCRTVersion}/lib/${PlatformShortName})

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
			set(CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.01")
			set(CMAKE_CREATE_CONSOLE_EXE "/subsystem:console,5.01")
		elseif(${PlatformShortName} STREQUAL "x64")
			set(CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.02")
			set(CMAKE_CREATE_CONSOLE_EXE "/subsystem:console,5.02")
		endif()
    endif()
endif()