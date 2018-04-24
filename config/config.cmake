cmake_minimum_required(VERSION 3.5.2)


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
	message("VC-LTL not load in Debug!!!")
	set(SupportLTL "false")
endif()


if(${SupportLTL} STREQUAL "true")

	include(CheckSymbolExists)

	check_symbol_exists("_M_AMD64" "" _M_AMD64)

	if(_M_AMD64)
		set(PlatformShortName "x64")
	else()
		check_symbol_exists("_M_IX86" "" _M_IX86)
		if(_M_IX86)
			set(PlatformShortName "x86")
		else()
			message("VC-LTL not load, Unknown Platform!!!")
			set(SupportLTL "false")
		endif()
	endif()
endif()


if(${SupportLTL} STREQUAL "true")
	#获取VC版本号
	if(NOT VC-LTLUsedToolsVersion)
		if(${MSVC_VERSION} EQUAL 1900)
			set(__DefaultVCToolsVersion "14.0.23918")

			#VC 2015只能自己读取注册表解决
			GET_FILENAME_COMPONENT(VCToolsVersion "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B0791F3A-6A88-3650-AECF-8AFBE227EC53};DisplayVersion]" NAME)
			if (${VCToolsVersion} STREQUAL "registry")
				GET_FILENAME_COMPONENT(VCToolsVersion "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B0791F3A-6A88-3650-AECF-8AFBE227EC53};DisplayVersion]" NAME)
			endif()

			if (${VCToolsVersion} STREQUAL "registry")
				GET_FILENAME_COMPONENT(VCToolsVersion "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09};DisplayVersion]" NAME)
			endif()
			if (${VCToolsVersion} STREQUAL "registry")
				GET_FILENAME_COMPONENT(VCToolsVersion "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4B1849F2-3D49-325F-B997-4AD0BF5B8A09};DisplayVersion]" NAME)
			endif()

			if (${VCToolsVersion} STREQUAL "registry")
				GET_FILENAME_COMPONENT(VCToolsVersion "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A};DisplayVersion]" NAME)
			endif()
			if (${VCToolsVersion} STREQUAL "registry")
				GET_FILENAME_COMPONENT(VCToolsVersion "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{729FD64C-2AE0-3E25-83A8-A93520DCDE7A};DisplayVersion]" NAME)
			endif()

			if (NOT ${VCToolsVersion} STREQUAL "registry")
				set(VC-LTLUsedToolsVersion ${VCToolsVersion})
			endif()

		elseif(${MSVC_VERSION} LESS 1920)
			#VC2017
			set(__DefaultVCToolsVersion "14.10.25017")

			if(DEFINED ENV{VCToolsVersion})
				set(VC-LTLUsedToolsVersion $ENV{VCToolsVersion})
			endif()

		else()
			message("VC-LTL not load, Unknown MSVC_VERSION=" ${MSVC_VERSION})
			set(SupportLTL "false")
		endif()

		if(${SupportLTL} STREQUAL "true")
			if(NOT VC-LTLUsedToolsVersion)
				set(VC-LTLUsedToolsVersion ${__DefaultVCToolsVersion})
			else()
				find_file(FOUND_FILE ${VC-LTLUsedToolsVersion} ${VC_LTL_Root}/VC)
				if(${FOUND_FILE} STREQUAL "FOUND_FILE-NOTFOUND")
					set(VC-LTLUsedToolsVersion ${__DefaultVCToolsVersion})
				endif()

			endif()
		endif()

	endif()
endif()


if(${SupportLTL} STREQUAL "true")

	#获取UCRT版本，如果当前环境变量UCRTVersion说对应的版本VC-LTL支持，那么使用该版本，否则使用10.0.10240.0
   if(NOT VC-LTLTargetUniversalCRTVersion)
	   if(DEFINED ENV{UCRTVersion})
	   		find_file(FOUND_FILE $ENV{UCRTVersion} ${VC_LTL_Root}/ucrt)
			if(NOT ${FOUND_FILE} STREQUAL "FOUND_FILE-NOTFOUND")
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
    set(VC_LTL_Library ${VC_LTL_Root}/${PlatformShortName};${VC_LTL_Root}/${PlatformShortName}/${OsPlatformName}/${LTL_Mode};${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion}/lib/${PlatformShortName};${VC_LTL_Root}/VC/${VC-LTLUsedToolsVersion}/lib/${PlatformShortName}/${OsPlatformName};${VC_LTL_Root}/ucrt/${VC-LTLTargetUniversalCRTVersion}/lib/${PlatformShortName})

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
		else()#x64
			set(CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.02")
			set(CMAKE_CREATE_CONSOLE_EXE "/subsystem:console,5.02")
		endif()
    endif()
endif()