#define __Build_OBJ
#undef _CRT_BEST_PRACTICES_USAGE

#include <msvcrt_IAT.h>
#include <Windows.h>
#include <internal_shared.h>
#include <corecrt_internal.h>
#include <winapi_thunks.h>

//切换到系统的msvrct后无需初始化onexit_table，msvcrt.dll内部会初始化
//EXTERN_C bool __cdecl __scrt_initialize_onexit_tables(int const module_type)
//{
//	return true;
//}

//_LCRT_DEFINE_IAT_SYMBOL(__scrt_initialize_onexit_tables);


EXTERN_C BOOL __cdecl __vcrt_InitializeCriticalSectionEx(
	LPCRITICAL_SECTION const critical_section,
	DWORD              const spin_count,
	DWORD              const flags
    )
{
#ifdef _ATL_XP_TARGETING
	return InitializeCriticalSectionAndSpinCount(critical_section, spin_count);
#else
	return InitializeCriticalSectionEx(critical_section, spin_count, flags);
#endif
}

//_LCRT_DEFINE_IAT_SYMBOL(__vcrt_InitializeCriticalSectionEx);

EXTERN_C PVOID __fastcall __CRT_DecodePointer(
	PVOID Ptr
)
{
	return __crt_fast_decode_pointer(Ptr);
}

EXTERN_C PVOID __fastcall __CRT_EncodePointer(PVOID const Ptr)
{
	return __crt_fast_encode_pointer(Ptr);
}


#if defined _X86_ && _CRT_NTDDI_MIN >= NTDDI_VISTA

typedef void(__fastcall *PCOOKIE_CHECK)(UINT_PTR);

EXTERN_C __declspec(dllimport) EXCEPTION_DISPOSITION __cdecl _except_handler4_common(
	IN PUINT_PTR                        CookiePointer,
	IN PCOOKIE_CHECK                    CookieCheckFunction,
	IN PEXCEPTION_RECORD                ExceptionRecord,
	IN PEXCEPTION_REGISTRATION_RECORD   EstablisherFrame,
	IN OUT PCONTEXT                     ContextRecord,
	IN OUT PVOID                        DispatcherContext
	);

EXTERN_C
DECLSPEC_GUARD_SUPPRESS
EXCEPTION_DISPOSITION
__cdecl
_except_handler4(
	IN PEXCEPTION_RECORD                ExceptionRecord,
	IN PEXCEPTION_REGISTRATION_RECORD   EstablisherFrame,
	IN OUT PCONTEXT                     ContextRecord,
	IN OUT PVOID                        DispatcherContext
	)
{
	return _except_handler4_common(&__security_cookie, &__security_check_cookie, ExceptionRecord, EstablisherFrame, ContextRecord, DispatcherContext);
}

#endif


/*
从msvcrt.dll中获取ptd指针。
函数通过获取errno地址换算出ptd地址。函数行为在某些平台不能保证统一。
在WinXP中，如果msvcrt.dll内部内存申请失败那么将导致整个程序退出。
XP以上平台则不会发生此问题，能保证在内存申请失败时返回null。
*/
EXTERN_C _ptd_msvcrt* __cdecl _getptd_noexit(void)
{
	//_errno()不可能返回null
	auto ptd = (_ptd_msvcrt*)(((byte*)_errno()) - FIELD_OFFSET(_ptd_msvcrt, _terrno));

	/*
	当 _thandle = -1，这表明此线程的ptd通过msvcrt.dll begin_thread 或者 __getptd_noexit 创建。
	当 _thandle = 0，这表明此线程的ptd通过msvcrt.dll的DllMain创建。
	当 _thandle = 其他，这表明msvcrt.dll内部内存已经申请失败。
	*/
	return (ptd->_thandle == (uintptr_t)-1/*Current Thread Handle*/ || ptd->_thandle == 0) ? ptd : nullptr;
}


EXTERN_C __declspec(dllimport) void __cdecl _amsg_exit(
	int rterrnum
);

EXTERN_C _ptd_msvcrt* __cdecl __acrt_getptd(void)  /* return address of per-thread CRT data */
{
	auto ptd = _getptd_noexit();

	if (!ptd)
	{
		_amsg_exit(16);
	}

	return ptd;
}

#pragma pack(push)
#pragma pack()

namespace
{
	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;
	typedef const UNICODE_STRING *PCUNICODE_STRING;

	typedef struct _PEB_LDR_DATA * PPEB_LDR_DATA;
	typedef struct _RTL_USER_PROCESS_PARAMETERS * PRTL_USER_PROCESS_PARAMETERS;

	typedef struct _PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;
		union
		{
			BOOLEAN BitField;
			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN IsLongPathAwareProcess : 1;
			};
		};

		HANDLE Mutant;

		PVOID ImageBaseAddress;
		PPEB_LDR_DATA Ldr;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID SubSystemData;
		PVOID ProcessHeap;
		PRTL_CRITICAL_SECTION FastPebLock;
		PVOID AtlThunkSListPtr;
		PVOID IFEOKey;
		union
		{
			ULONG CrossProcessFlags;
			struct
			{
				ULONG ProcessInJob : 1;
				ULONG ProcessInitializing : 1;
				ULONG ProcessUsingVEH : 1;
				ULONG ProcessUsingVCH : 1;
				ULONG ProcessUsingFTH : 1;
				ULONG ReservedBits0 : 27;
			};
		};
		union
		{
			PVOID KernelCallbackTable;
			PVOID UserSharedInfoPtr;
		};
		ULONG SystemReserved[1];
		ULONG AtlThunkSListPtr32;
		PVOID ApiSetMap;
		ULONG TlsExpansionCounter;
		PVOID TlsBitmap;
		ULONG TlsBitmapBits[2];
		PVOID ReadOnlySharedMemoryBase;
		PVOID HotpatchInformation;
		PVOID *ReadOnlyStaticServerData;
		PVOID AnsiCodePageData;
		PVOID OemCodePageData;
		PVOID UnicodeCaseTableData;

		ULONG NumberOfProcessors;
		ULONG NtGlobalFlag;

		LARGE_INTEGER CriticalSectionTimeout;
		SIZE_T HeapSegmentReserve;
		SIZE_T HeapSegmentCommit;
		SIZE_T HeapDeCommitTotalFreeThreshold;
		SIZE_T HeapDeCommitFreeBlockThreshold;

		ULONG NumberOfHeaps;
		ULONG MaximumNumberOfHeaps;
		PVOID *ProcessHeaps;

		PVOID GdiSharedHandleTable;
		PVOID ProcessStarterHelper;
		ULONG GdiDCAttributeList;

		PRTL_CRITICAL_SECTION LoaderLock;

		ULONG OSMajorVersion;
		ULONG OSMinorVersion;
		USHORT OSBuildNumber;
		USHORT OSCSDVersion;
		ULONG OSPlatformId;
		ULONG ImageSubsystem;
		ULONG ImageSubsystemMajorVersion;
		ULONG ImageSubsystemMinorVersion;
		ULONG_PTR ActiveProcessAffinityMask;
#ifdef _AMD64_
		ULONG GdiHandleBuffer[60];
#else
		ULONG GdiHandleBuffer[34];
#endif
		PVOID PostProcessInitRoutine;

		PVOID TlsExpansionBitmap;
		ULONG TlsExpansionBitmapBits[32];

		ULONG SessionId;

		ULARGE_INTEGER AppCompatFlags;
		ULARGE_INTEGER AppCompatFlagsUser;
		PVOID pShimData;
		PVOID AppCompatInfo;

		UNICODE_STRING CSDVersion;

		PVOID ActivationContextData;
		PVOID ProcessAssemblyStorageMap;
		PVOID SystemDefaultActivationContextData;
		PVOID SystemAssemblyStorageMap;

		SIZE_T MinimumStackCommit;

		PVOID *FlsCallback;
		LIST_ENTRY FlsListHead;
		PVOID FlsBitmap;
		ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
		ULONG FlsHighIndex;

		PVOID WerRegistrationData;
		PVOID WerShipAssertPtr;
		PVOID pContextData;
		PVOID pImageHeaderHash;
		union
		{
			ULONG TracingFlags;
			struct
			{
				ULONG HeapTracingEnabled : 1;
				ULONG CritSecTracingEnabled : 1;
				ULONG LibLoaderTracingEnabled : 1;
				ULONG SpareTracingBits : 29;
			};
		};
		ULONGLONG CsrServerReadOnlySharedMemoryBase;
		PVOID TppWorkerpListLock;
		LIST_ENTRY TppWorkerpList;
		PVOID WaitOnAddressHashTable[128];
	} PEB, *PPEB;

	typedef struct _TEB_ACTIVE_FRAME *PTEB_ACTIVE_FRAME;

	typedef struct _CLIENT_ID {
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	} CLIENT_ID, *PCLIENT_ID;

	typedef struct _GDI_TEB_BATCH
	{
		ULONG Offset;
		HDC hdc;
		ULONG buffer[310];
	} GDI_TEB_BATCH;

	typedef struct _TEB
	{
		NT_TIB NtTib;

		PVOID EnvironmentPointer;
		CLIENT_ID ClientId;
		PVOID ActiveRpcHandle;
		PVOID ThreadLocalStoragePointer;
		PPEB ProcessEnvironmentBlock;

		ULONG LastErrorValue;
		ULONG CountOfOwnedCriticalSections;
		PVOID CsrClientThread;
		PVOID Win32ThreadInfo;
		ULONG User32Reserved[26];
		ULONG UserReserved[5];
		PVOID WOW32Reserved;
		LCID CurrentLocale;
		ULONG FpSoftwareStatusRegister;
		PVOID ReservedForDebuggerInstrumentation[16];
		PVOID SystemReserved1[37];
		UCHAR WorkingOnBehalfTicket[8];
		NTSTATUS ExceptionCode;

		PVOID ActivationContextStackPointer;
		ULONG_PTR InstrumentationCallbackSp;
		ULONG_PTR InstrumentationCallbackPreviousPc;
		ULONG_PTR InstrumentationCallbackPreviousSp;
		ULONG TxFsContext;

		BOOLEAN InstrumentationCallbackDisabled;
		GDI_TEB_BATCH GdiTebBatch;
		CLIENT_ID RealClientId;
		HANDLE GdiCachedProcessHandle;
		ULONG GdiClientPID;
		ULONG GdiClientTID;
		PVOID GdiThreadLocalInfo;
		ULONG_PTR Win32ClientInfo[62];
		PVOID glDispatchTable[233];
		ULONG_PTR glReserved1[29];
		PVOID glReserved2;
		PVOID glSectionInfo;
		PVOID glSection;
		PVOID glTable;
		PVOID glCurrentRC;
		PVOID glContext;

		NTSTATUS LastStatusValue;
		UNICODE_STRING StaticUnicodeString;
		WCHAR StaticUnicodeBuffer[261];

		PVOID DeallocationStack;
		PVOID TlsSlots[64];
		LIST_ENTRY TlsLinks;

		PVOID Vdm;
		PVOID ReservedForNtRpc;
		PVOID DbgSsReserved[2];

		ULONG HardErrorMode;
#ifdef _WIN64
		PVOID Instrumentation[11];
#else
		PVOID Instrumentation[9];
#endif
		GUID ActivityId;

		PVOID SubProcessTag;
		PVOID PerflibData;
		PVOID EtwTraceData;
		PVOID WinSockData;
		ULONG GdiBatchCount;

		union
		{
			PROCESSOR_NUMBER CurrentIdealProcessor;
			ULONG IdealProcessorValue;
			struct
			{
				UCHAR ReservedPad0;
				UCHAR ReservedPad1;
				UCHAR ReservedPad2;
				UCHAR IdealProcessor;
			};
		};

		ULONG GuaranteedStackBytes;
		PVOID ReservedForPerf;
		PVOID ReservedForOle;
		ULONG WaitingOnLoaderLock;
		PVOID SavedPriorityState;
		ULONG_PTR ReservedForCodeCoverage;
		PVOID ThreadPoolData;
		PVOID *TlsExpansionSlots;
#ifdef _WIN64
		PVOID DeallocationBStore;
		PVOID BStoreLimit;
#endif
		ULONG MuiGeneration;
		ULONG IsImpersonating;
		PVOID NlsCache;
		PVOID pShimData;
		USHORT HeapVirtualAffinity;
		USHORT LowFragHeapDataSlot;
		HANDLE CurrentTransactionHandle;
		PTEB_ACTIVE_FRAME ActiveFrame;
		PVOID FlsData;

		PVOID PreferredLanguages;
		PVOID UserPrefLanguages;
		PVOID MergedPrefLanguages;
		ULONG MuiImpersonation;

		union
		{
			USHORT CrossTebFlags;
			USHORT SpareCrossTebBits : 16;
		};
		union
		{
			USHORT SameTebFlags;
			struct
			{
				USHORT SafeThunkCall : 1;
				USHORT InDebugPrint : 1;
				USHORT HasFiberData : 1;
				USHORT SkipThreadAttach : 1;
				USHORT WerInShipAssertCode : 1;
				USHORT RanProcessInit : 1;
				USHORT ClonedThread : 1;
				USHORT SuppressDebugMsg : 1;
				USHORT DisableUserStackWalk : 1;
				USHORT RtlExceptionAttached : 1;
				USHORT InitialThread : 1;
				USHORT SessionAware : 1;
				USHORT LoadOwner : 1;
				USHORT LoaderWorker : 1;
				USHORT SpareSameTebBits : 2;
			};
		};

		PVOID TxnScopeEnterCallback;
		PVOID TxnScopeExitCallback;
		PVOID TxnScopeContext;
		ULONG LockCount;
		LONG WowTebOffset;
		PVOID ResourceRetValue;
		PVOID ReservedForWdf;
		ULONGLONG ReservedForCrt;
		GUID EffectiveContainerId;
	} TEB, *PTEB;

}


#pragma pack(pop)

EXTERN_C DWORD __cdecl __LTL_GetOsMinVersion()
{
	auto pPeb = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock;

	return MakeMiniVersion(pPeb->OSMajorVersion, pPeb->OSMinorVersion);
}


EXTERN_C UINT64 __cdecl __LTL_GetOsVersion()
{
	auto pPeb = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock;

	return MakeVersion(pPeb->OSMajorVersion, pPeb->OSMinorVersion, pPeb->OSBuildNumber, 0);
}

#undef _environ
#undef _wenviron

#if defined _M_ARM || defined _M_ARM64

#define _Static_Environ

//arm64的msvcrt未导出二个符号
EXTERN_C char **    _environ = nullptr;
EXTERN_C wchar_t ** _wenviron = nullptr;

_LCRT_DEFINE_IAT_SYMBOL(_environ);
_LCRT_DEFINE_IAT_SYMBOL(_wenviron);
#else
EXTERN_C __declspec(dllimport) extern char **    _environ;
EXTERN_C __declspec(dllimport) extern wchar_t ** _wenviron;
#endif


#pragma pack(push,_CRT_PACKING)
//结构体来自于 VC 2013 internal.h，逆向了下msvcrt.dll确实只有一个成员。
typedef struct
{
	int newmode;
} _startupinfo;

#pragma pack(pop)


extern "C" __declspec(dllimport) int __cdecl __getmainargs(
	_Out_ int * _Argc,
	_Outptr_result_buffer_(*_Argc) char *** _Argv,
	_Outptr_result_maybenull_ char *** _Env,
	_In_ int _DoWildCard,
	_In_ _startupinfo * _StartInfo
);

extern "C" __declspec(dllimport) int __cdecl __wgetmainargs(
	_Out_ int * _Argc,
	_Outptr_result_buffer_(*_Argc) wchar_t *** _Argv,
	_Outptr_result_maybenull_ wchar_t *** _Env,
	_In_ int _DoWildCard,
	_In_ _startupinfo * _StartInfo
);


extern "C" int __CRTDECL _get_startup_new_mode();

EXTERN_C errno_t __cdecl _configure_narrow_argv_downlevel(
	_In_ _crt_argv_mode mode
	)
{
	int _Argc;
	char ** _Argv;
#if !defined _Static_Environ
	char **    _environ;
#endif
	_startupinfo _StartInfon = { _get_startup_new_mode() };

	//不支持_crt_argv_no_arguments
	return __getmainargs(&_Argc, &_Argv, &_environ, _get_startup_argv_mode() == _crt_argv_mode::_crt_argv_unexpanded_arguments ? 0 : 1, &_StartInfon);
}

_LCRT_DEFINE_IAT_SYMBOL(_configure_narrow_argv_downlevel);

EXTERN_C errno_t __cdecl _configure_wide_argv_downlevel(
	_In_ _crt_argv_mode mode
	)
{
	int _Argc;
	wchar_t ** _Argv;
#if !defined _Static_Environ
	wchar_t ** _wenviron;
#endif

	_startupinfo _StartInfon = { _get_startup_new_mode() };

	//不支持_crt_argv_no_arguments
	return __wgetmainargs(&_Argc, &_Argv, &_wenviron, _get_startup_argv_mode() == _crt_argv_mode::_crt_argv_unexpanded_arguments ? 0 : 1, &_StartInfon);
}

_LCRT_DEFINE_IAT_SYMBOL(_configure_wide_argv_downlevel);

extern "C" int _commode = 0;


EXTERN_C char**    __cdecl _get_initial_narrow_environment(void)
{
	return _environ;
}

_LCRT_DEFINE_IAT_SYMBOL(_get_initial_narrow_environment);


EXTERN_C wchar_t** __cdecl _get_initial_wide_environment(void)
{
	return _wenviron;
}

_LCRT_DEFINE_IAT_SYMBOL(_get_initial_wide_environment);


typedef int (__cdecl * _crt_atexit_fun)(
	void(__cdecl* _Function)(void)
	);

//将atexit imp版转发到 _crt_atexit
extern "C" extern const _crt_atexit_fun _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(atexit);

extern "C" int __cdecl _crt_atexit(
	void(__cdecl* _Function)(void)
	)
{
	return _LCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(atexit)(_Function);
}
