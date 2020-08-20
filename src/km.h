#pragma once

#ifndef SDK_KM_H
#define SDK_KM_H

#ifndef UMDF_USING_NTSTATUS
#define UMDF_USING_NTSTATUS
#endif
#include <ntstatus.h>
#include <Windows.h>
#include <winnt.h>

//#pragma comment(lib,"ntdll.lib")


#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#define OBJ_VALID_ATTRIBUTES    0x000007F2L

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_REMOTE_INSTANCE               0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

#if (NTDDI_VERSION >= NTDDI_WIN7)
#define FILE_OPEN_REQUIRING_OPLOCK              0x00010000
#define FILE_DISALLOW_EXCLUSIVE                 0x00020000
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */

#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000

#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036
#define FILE_SHARE_VALID_FLAGS	                0x00000007

//
// Define special ByteOffset parameters for read and write operations
//

#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe

//
// Define alignment requirement values
//

#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff

//
// Define the maximum length of a filename string
//

#define MAXIMUM_FILENAME_LENGTH         256

#define NtGetCurrentProcess() (HANDLE)-1

// begin_access
#define DUPLICATE_CLOSE_SOURCE      0x00000001  
#define DUPLICATE_SAME_ACCESS       0x00000002  
#define DUPLICATE_SAME_ATTRIBUTES   0x00000004

#ifndef _Reserved_
#define _Reserved_
#endif

typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS *PNTSTATUS;


#if defined __cplusplus && !defined _Disallow_YY_KM_Namespace
namespace YY {
#endif

#if _MSC_VER <= 1500

typedef enum _FIRMWARE_TYPE {
	FirmwareTypeUnknown,
	FirmwareTypeBios,
	FirmwareTypeUefi,
	FirmwareTypeMax
} FIRMWARE_TYPE, *PFIRMWARE_TYPE;

#endif

typedef ULONG LOGICAL;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _ANSI_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PSTR  Buffer;
} ANSI_STRING, STRING, *PSTRING, *PANSI_STRING;

typedef const ANSI_STRING* PCANSI_STRING;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
	    }

typedef struct _OBJECT_ATTRIBUTES {
	ULONG           Length;
	HANDLE          RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG           Attributes;
	PVOID           SecurityDescriptor;
	PVOID           SecurityQualityOfService;
}  OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;



//#pragma pack(push,1)
typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID Pointer;
	} DUMMYUNIONNAME;

	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
	USHORT Flags;
	USHORT Length;
	ULONG TimeStamp;
	STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	HANDLE Handle;
} CURDIR, *PCURDIR;

typedef short CSHORT;

//
//  Time conversion routines
//

typedef struct _TIME_FIELDS {
	CSHORT Year;        // range [1601...]
	CSHORT Month;       // range [1..12]
	CSHORT Day;         // range [1..31]
	CSHORT Hour;        // range [0..23]
	CSHORT Minute;      // range [0..59]
	CSHORT Second;      // range [0..59]
	CSHORT Milliseconds;// range [0..999]
	CSHORT Weekday;     // range [0..6] == [Sunday..Saturday]
} TIME_FIELDS;
typedef TIME_FIELDS *PTIME_FIELDS;

typedef struct _RTL_USER_PROCESS_PARAMETERS_XP
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StdInputHandle;
	HANDLE StdOutputHandle;
	HANDLE StdErrorHandle;
	UNICODE_STRING CurrentDirectoryPath;
	HANDLE CurrentDirectoryHandle;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingPositionLeft;
	ULONG StartingPositionTop;
	ULONG Width;
	ULONG Height;
	ULONG CharWidth;
	ULONG CharHeight;
	ULONG ConsoleTextAttributes;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopName;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} RTL_USER_PROCESS_PARAMETERS_XP, *PRTL_USER_PROCESS_PARAMETERS_XP, *PPROCESS_PARAMETERS_XP;

typedef struct _RTL_USER_PROCESS_PARAMETERS_VISTA
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StdInputHandle;
	HANDLE StdOutputHandle;
	HANDLE StdErrorHandle;
	CURDIR CurrentDirectoryPath;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingPositionLeft;
	ULONG StartingPositionTop;
	ULONG Width;
	ULONG Height;
	ULONG CharWidth;
	ULONG CharHeight;
	ULONG ConsoleTextAttributes;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopName;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
	volatile ULONG_PTR EnvironmentSize;
} RTL_USER_PROCESS_PARAMETERS_VISTA, *PRTL_USER_PROCESS_PARAMETERS_VISTA, *PPROCESS_PARAMETERS_VISTA;

typedef struct _RTL_USER_PROCESS_PARAMETERS_7
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StdInputHandle;
	HANDLE StdOutputHandle;
	HANDLE StdErrorHandle;
	CURDIR CurrentDirectoryPath;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingPositionLeft;
	ULONG StartingPositionTop;
	ULONG Width;
	ULONG Height;
	ULONG CharWidth;
	ULONG CharHeight;
	ULONG ConsoleTextAttributes;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopName;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
	volatile ULONG_PTR EnvironmentSize;
	volatile ULONG_PTR EnvironmentVersion;
} RTL_USER_PROCESS_PARAMETERS_7, *PRTL_USER_PROCESS_PARAMETERS_7, *PPROCESS_PARAMETERS_7;

typedef struct _INITIAL_TEB {
	PVOID FixedStackBase;
	PVOID FixedStackLimit;
	PVOID StackBase;
	PVOID StackLimit;
	PVOID AllocatedStackBase;
} INITIAL_TEB, *PINITIAL_TEB;


typedef struct _PEB_LDR_DATA_XP {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} PEB_LDR_DATA_XP, *PPEB_LDR_DATA_XP;

typedef struct _PEB_LDR_DATA_VISTA_7 {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
	BOOLEAN ShutdownInProgress;
	PVOID ShutdownThread;
} PEB_LDR_DATA_VISTA_7, *PPEB_LDR_DATA_VISTA_7;

typedef struct _LDR_DATA_TABLE_ENTRY_XP {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		PVOID SectionPointer;
	};
	ULONG Checksum;
	union
	{
		ULONG TimeDataStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY_XP, *PLDR_DATA_TABLE_ENTRY_XP, LDR_MODULE_XP, *PLDR_MODULE_XP;

typedef struct _LDR_DATA_TABLE_ENTRY_VISTA {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		PVOID SectionPointer;
	};
	ULONG Checksum;
	union
	{
		ULONG TimeDataStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
} LDR_DATA_TABLE_ENTRY_VISTA, *PLDR_DATA_TABLE_ENTRY_VISTA, LDR_MODULE_VISTA, *PLDR_MODULE_VISTA;

typedef struct _LDR_DATA_TABLE_ENTRY_7 {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		PVOID SectionPointer;
	};
	ULONG Checksum;
	union
	{
		ULONG TimeDataStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
	PVOID ContextInformation;
	ULONG_PTR OriginalBase;
	LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY_7, *PLDR_DATA_TABLE_ENTRY_7, LDR_MODULE_7, *PLDR_MODULE_7;

typedef
VOID
(NTAPI *PPS_POST_PROCESS_INIT_ROUTINE) (
	VOID
	);

typedef struct _PEB_FREE_BLOCK
{
	struct _PEB_FREE_BLOCK* Next;
	ULONG Size;
} PEB_FREE_BLOCK;

typedef struct PEB_LDR_DATA *PPEB_LDR_DATA;

typedef void (NTAPI *PIO_APC_ROUTINE)(PVOID, PIO_STATUS_BLOCK, ULONG);

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG MaximumLength;
	ULONG Length;

	ULONG Flags;
	ULONG DebugFlags;

	HANDLE ConsoleHandle;
	ULONG ConsoleFlags;
	HANDLE StandardInput;
	HANDLE StandardOutput;
	HANDLE StandardError;

	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;

	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;

	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

	ULONG EnvironmentSize;
	ULONG EnvironmentVersion;
	PVOID PackageDependencyData;
	ULONG ProcessGroupId;
	ULONG LoaderThreads;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION;
typedef struct PEB_LDR_DATA *PPEB_LDR_DATA;

// symbols
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

typedef
_Function_class_(RTL_HEAP_COMMIT_ROUTINE)
_IRQL_requires_same_
NTSTATUS
NTAPI
RTL_HEAP_COMMIT_ROUTINE(
    _In_ PVOID Base,
    _Inout_ PVOID *CommitAddress,
    _Inout_ PSIZE_T CommitSize
    );
typedef RTL_HEAP_COMMIT_ROUTINE *PRTL_HEAP_COMMIT_ROUTINE;

typedef struct _RTL_HEAP_PARAMETERS {
    ULONG Length;
    SIZE_T SegmentReserve;
    SIZE_T SegmentCommit;
    SIZE_T DeCommitFreeBlockThreshold;
    SIZE_T DeCommitTotalFreeThreshold;
    SIZE_T MaximumAllocationSize;
    SIZE_T VirtualMemoryThreshold;
    SIZE_T InitialCommit;
    SIZE_T InitialReserve;
    PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
    SIZE_T Reserved[ 2 ];
} RTL_HEAP_PARAMETERS, *PRTL_HEAP_PARAMETERS;


typedef enum _LPC_TYPE {
	LPC_NEW_MESSAGE, // A new message
	LPC_REQUEST, // A request message
	LPC_REPLY, // A reply to a request message
	LPC_DATAGRAM, //
	LPC_LOST_REPLY, //
	LPC_PORT_CLOSED, // Sent when port is deleted
	LPC_CLIENT_DIED, // Messages to thread termination ports
	LPC_EXCEPTION, // Messages to thread exception port
	LPC_DEBUG_EVENT, // Messages to thread debug port
	LPC_ERROR_EVENT, // Used by ZwRaiseHardError
	LPC_CONNECTION_REQUEST // Used by ZwConnectPort
} LPC_TYPE;

typedef struct _PORT_MESSAGE
{
	union {
		struct {
			SHORT DataLength;
			SHORT TotalLength;
		} s1;
		ULONG Length;
	} u1;
	union {
		struct {
			SHORT Type;
			SHORT DataInfoOffset;
		} s2;
		ULONG ZeroInit;
	} u2;
	union {
		CLIENT_ID ClientId;
		double DoNotUseThisField;       // Force quadword alignment
	} DUMMYUNIONNAME3;
	ULONG MessageId;
	union {
		SIZE_T ClientViewSize;          // Only valid on LPC_CONNECTION_REQUEST message
		ULONG CallbackId;                   // Only valid on LPC_REQUEST message
	} DUMMYUNIONNAME4;
	//  UCHAR Data[];
} PORT_MESSAGE, *PPORT_MESSAGE;

typedef struct _DEBUG_MESSAGE {
	PORT_MESSAGE PortMessage;
	ULONG EventCode;
	ULONG Status;
	union {
		struct {
			EXCEPTION_RECORD ExceptionRecord;
			ULONG FirstChance;
		} Exception;
		struct {
			ULONG Reserved;
			PVOID StartAddress;
		} CreateThread;
		struct {
			ULONG Reserved;
			HANDLE FileHandle;
			PVOID Base;
			ULONG PointerToSymbolTable;
			ULONG NumberOfSymbols;
			ULONG Reserved2;
			PVOID EntryPoint;
		} CreateProcess;
		struct {
			ULONG ExitCode;
		} ExitThread;
		struct {
			ULONG ExitCode;
		} ExitProcess;
		struct {
			HANDLE FileHandle;
			PVOID Base;
			ULONG PointerToSymbolTable;
			ULONG NumberOfSymbols;
		} LoadDll;
		struct {
			PVOID Base;
		} UnloadDll;
		struct {
			LPSTR lpDebugStringData;
			WORD fUnicode;
			WORD nDebugStringLength;
		} OutputDebugStringInfo;
		struct
		{
			DWORD dwError;
			DWORD dwType;
		}RipInfo;
	} u;
} DEBUG_MESSAGE, *PDEBUG_MESSAGE;

typedef enum _FILE_INFORMATION_CLASS
{
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,   // 2
	FileBothDirectoryInformation,   // 3
	FileBasicInformation,           // 4
	FileStandardInformation,        // 5
	FileInternalInformation,        // 6
	FileEaInformation,              // 7
	FileAccessInformation,          // 8
	FileNameInformation,            // 9
	FileRenameInformation,          // 10
	FileLinkInformation,            // 11
	FileNamesInformation,           // 12
	FileDispositionInformation,     // 13
	FilePositionInformation,        // 14
	FileFullEaInformation,          // 15
	FileModeInformation,            // 16
	FileAlignmentInformation,       // 17
	FileAllInformation,             // 18
	FileAllocationInformation,      // 19
	FileEndOfFileInformation,       // 20
	FileAlternateNameInformation,   // 21
	FileStreamInformation,          // 22
	FilePipeInformation,            // 23
	FilePipeLocalInformation,       // 24
	FilePipeRemoteInformation,      // 25
	FileMailslotQueryInformation,   // 26
	FileMailslotSetInformation,     // 27
	FileCompressionInformation,     // 28
	FileObjectIdInformation,        // 29
	FileCompletionInformation,      // 30
	FileMoveClusterInformation,     // 31
	FileQuotaInformation,           // 32
	FileReparsePointInformation,    // 33
	FileNetworkOpenInformation,     // 34
	FileAttributeTagInformation,    // 35
	FileTrackingInformation,        // 36
	FileIdBothDirectoryInformation, // 37
	FileIdFullDirectoryInformation, // 38
	FileValidDataLengthInformation, // 39
	FileShortNameInformation,       // 40
	FileIoCompletionNotificationInformation, // 41
	FileIoStatusBlockRangeInformation,       // 42
	FileIoPriorityHintInformation,           // 43
	FileSfioReserveInformation,              // 44
	FileSfioVolumeInformation,               // 45
	FileHardLinkInformation,                 // 46
	FileProcessIdsUsingFileInformation,      // 47
	FileNormalizedNameInformation,           // 48
	FileNetworkPhysicalNameInformation,      // 49
	FileIdGlobalTxDirectoryInformation,      // 50
	FileIsRemoteDeviceInformation,           // 51
	FileUnusedInformation,                   // 52
	FileNumaNodeInformation,                 // 53
	FileStandardLinkInformation,             // 54
	FileRemoteProtocolInformation,           // 55

	//
	//  These are special versions of these operations (defined earlier)
	//  which can be used by kernel mode drivers only to bypass security
	//  access checks for Rename and HardLink operations.  These operations
	//  are only recognized by the IOManager, a file system should never
	//  receive these.
	//
	FileRenameInformationBypassAccessCheck,  // 56
	FileLinkInformationBypassAccessCheck,    // 57
	FileVolumeNameInformation,               // 58
	FileIdInformation,                       // 59
	FileIdExtdDirectoryInformation,          // 60
	FileReplaceCompletionInformation,        // 61
	FileHardLinkFullIdInformation,           // 62
	FileIdExtdBothDirectoryInformation,      // 63
	FileMaximumInformation

} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _FILE_ALIGNMENT_INFORMATION {
	ULONG AlignmentRequirement;
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION;

typedef struct _FILE_NAME_INFORMATION {
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION {
	ULONG FileAttributes;
	ULONG ReparseTag;
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;


typedef struct _FILE_END_OF_FILE_INFORMATION {
	LARGE_INTEGER EndOfFile;
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION;

typedef struct _FILE_VALID_DATA_LENGTH_INFORMATION {
	LARGE_INTEGER ValidDataLength;
} FILE_VALID_DATA_LENGTH_INFORMATION, *PFILE_VALID_DATA_LENGTH_INFORMATION;

typedef struct _FILE_BASIC_INFORMATION {
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	ULONG FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;


typedef struct _FILE_POSITION_INFORMATION {
	LARGE_INTEGER CurrentByteOffset;
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;

typedef struct _FILE_NETWORK_OPEN_INFORMATION {
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG FileAttributes;
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;

typedef struct _FILE_FULL_EA_INFORMATION {
	ULONG NextEntryOffset;
	UCHAR Flags;
	UCHAR EaNameLength;
	USHORT EaValueLength;
	CHAR EaName[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

//
// Support to reserve bandwidth for a file handle.
//

typedef struct _FILE_SFIO_RESERVE_INFORMATION {
	ULONG RequestsPerPeriod;
	ULONG Period;
	BOOLEAN RetryFailures;
	BOOLEAN Discardable;
	ULONG RequestSize;
	ULONG NumOutstandingRequests;
} FILE_SFIO_RESERVE_INFORMATION, *PFILE_SFIO_RESERVE_INFORMATION;

//
// Support to query bandwidth properties of a volume.
//

typedef struct _FILE_SFIO_VOLUME_INFORMATION {
	ULONG MaximumRequestsPerPeriod;
	ULONG MinimumPeriod;
	ULONG MinimumTransferSize;
} FILE_SFIO_VOLUME_INFORMATION, *PFILE_SFIO_VOLUME_INFORMATION;

typedef struct _FILE_EA_INFORMATION {
	ULONG EaSize;
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
	ACCESS_MASK AccessFlags;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;

typedef struct _FILE_MODE_INFORMATION {
	ULONG Mode;
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION;

typedef struct _FILE_STANDARD_INFORMATION
{
	LARGE_INTEGER AllocationSize;
	LARGE_INTEGER EndOfFile;
	ULONG         NumberOfLinks;
	BOOLEAN       DeletePending;
	BOOLEAN       Directory;
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	WCHAR FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_BOTH_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	CCHAR ShortNameLength;
	WCHAR ShortName[12];
	WCHAR FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_INTERNAL_INFORMATION {
	LARGE_INTEGER IndexNumber;
} FILE_INTERNAL_INFORMATION, *PFILE_INTERNAL_INFORMATION;

typedef struct _FILE_ALL_INFORMATION {
	FILE_BASIC_INFORMATION BasicInformation;
	FILE_STANDARD_INFORMATION StandardInformation;
	FILE_INTERNAL_INFORMATION InternalInformation;
	FILE_EA_INFORMATION EaInformation;
	FILE_ACCESS_INFORMATION AccessInformation;
	FILE_POSITION_INFORMATION PositionInformation;
	FILE_MODE_INFORMATION ModeInformation;
	FILE_ALIGNMENT_INFORMATION AlignmentInformation;
	FILE_NAME_INFORMATION NameInformation;
} FILE_ALL_INFORMATION, *PFILE_ALL_INFORMATION;

typedef struct _FILE_ALLOCATION_INFORMATION {
	LARGE_INTEGER AllocationSize;
} FILE_ALLOCATION_INFORMATION, *PFILE_ALLOCATION_INFORMATION;

typedef struct _FILE_COMPRESSION_INFORMATION {
	LARGE_INTEGER CompressedFileSize;
	USHORT CompressionFormat;
	UCHAR CompressionUnitShift;
	UCHAR ChunkShift;
	UCHAR ClusterShift;
	UCHAR Reserved[3];
} FILE_COMPRESSION_INFORMATION, *PFILE_COMPRESSION_INFORMATION;



typedef struct _FILE_ID_FULL_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	LARGE_INTEGER FileId;
	WCHAR FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, *PFILE_ID_FULL_DIR_INFORMATION;


typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
	ULONG NextEntryOffset;
	ULONG FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	ULONG EaSize;
	CCHAR ShortNameLength;
	WCHAR ShortName[12];
	LARGE_INTEGER FileId;
	WCHAR FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;



typedef struct _FILE_DISPOSITION_INFORMATION
{
	BOOLEAN NeedDeleteFile;
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION;
//#pragma pack(pop)


typedef struct _FILE_DIRECTORY_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	WCHAR         FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;


typedef struct _FILE_MOVE_CLUSTER_INFORMATION {
	ULONG ClusterCount;
	HANDLE RootDirectory;
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_MOVE_CLUSTER_INFORMATION, *PFILE_MOVE_CLUSTER_INFORMATION;


typedef struct _FILE_RENAME_INFORMATION
{
	BOOLEAN ReplaceIfExists;
	HANDLE RootDirectory;
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_RENAME_INFORMATION, *PFILE_RENAME_INFORMATION, FILE_LINK_INFORMATION, *PFILE_LINK_INFORMATION;

typedef struct _FILE_STREAM_INFORMATION {
	ULONG NextEntryOffset;
	ULONG StreamNameLength;
	LARGE_INTEGER StreamSize;
	LARGE_INTEGER StreamAllocationSize;
	WCHAR StreamName[1];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

typedef struct _FILE_TRACKING_INFORMATION {
	HANDLE DestinationFile;
	ULONG ObjectInformationLength;
	CHAR ObjectInformation[1];
} FILE_TRACKING_INFORMATION, *PFILE_TRACKING_INFORMATION;

typedef struct _FILE_COMPLETION_INFORMATION {
	HANDLE Port;
	PVOID Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

typedef struct _FILE_PIPE_INFORMATION {
	ULONG ReadMode;
	ULONG CompletionMode;
} FILE_PIPE_INFORMATION, *PFILE_PIPE_INFORMATION;

typedef struct _FILE_PIPE_LOCAL_INFORMATION {
	ULONG NamedPipeType;
	ULONG NamedPipeConfiguration;
	ULONG MaximumInstances;
	ULONG CurrentInstances;
	ULONG InboundQuota;
	ULONG ReadDataAvailable;
	ULONG OutboundQuota;
	ULONG WriteQuotaAvailable;
	ULONG NamedPipeState;
	ULONG NamedPipeEnd;
} FILE_PIPE_LOCAL_INFORMATION, *PFILE_PIPE_LOCAL_INFORMATION;

typedef struct _FILE_PIPE_REMOTE_INFORMATION {
	LARGE_INTEGER CollectDataTime;
	ULONG MaximumCollectionCount;
} FILE_PIPE_REMOTE_INFORMATION, *PFILE_PIPE_REMOTE_INFORMATION;

typedef struct _FILE_MAILSLOT_QUERY_INFORMATION {
	ULONG MaximumMessageSize;
	ULONG MailslotQuota;
	ULONG NextMessageSize;
	ULONG MessagesAvailable;
	LARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_QUERY_INFORMATION, *PFILE_MAILSLOT_QUERY_INFORMATION;

typedef struct _FILE_MAILSLOT_SET_INFORMATION {
	PLARGE_INTEGER ReadTimeout;
} FILE_MAILSLOT_SET_INFORMATION, *PFILE_MAILSLOT_SET_INFORMATION;

typedef struct _FILE_REPARSE_POINT_INFORMATION {
	LONGLONG FileReference;
	ULONG Tag;
} FILE_REPARSE_POINT_INFORMATION, *PFILE_REPARSE_POINT_INFORMATION;

typedef struct _FILE_LINK_ENTRY_INFORMATION {
	ULONG NextEntryOffset;
	LONGLONG ParentFileId;
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_LINK_ENTRY_INFORMATION, *PFILE_LINK_ENTRY_INFORMATION;

typedef struct _FILE_LINKS_INFORMATION {
	ULONG BytesNeeded;
	ULONG EntriesReturned;
	FILE_LINK_ENTRY_INFORMATION Entry;
} FILE_LINKS_INFORMATION, *PFILE_LINKS_INFORMATION;

typedef struct _FILE_NETWORK_PHYSICAL_NAME_INFORMATION {
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_NETWORK_PHYSICAL_NAME_INFORMATION, *PFILE_NETWORK_PHYSICAL_NAME_INFORMATION;


enum SYSTEM_INFORMATION_CLASS
{
	//SystemCurrentEntryIdentifier= 0x5A,
	//SystemBootPartition=0x62, //UNICODE_STRING
	SystemBasicInformation = 0x0,
	SystemProcessorInformation = 0x1,
	SystemPerformanceInformation = 0x2,
	SystemTimeOfDayInformation = 0x3,
	SystemPathInformation = 0x4,		// Obsolete: Use KUSER_SHARED_DATA
	SystemProcessInformation = 0x5,
	SystemCallCountInformation = 0x6,
	SystemDeviceInformation = 0x7,
	SystemProcessorPerformanceInformation = 0x8,
	SystemFlagsInformation = 0x9,
	SystemCallTimeInformation = 0xa,
	SystemModuleInformation = 0xb,
	SystemLocksInformation = 0xc,
	SystemStackTraceInformation = 0xd,
	SystemPagedPoolInformation = 0xe,
	SystemNonPagedPoolInformation = 0xf,
	SystemHandleInformation = 0x10,
	SystemObjectInformation = 0x11,
	SystemPageFileInformation = 0x12,
	SystemVdmInstemulInformation = 0x13,
	SystemVdmBopInformation = 0x14,
	SystemFileCacheInformation = 0x15,
	SystemPoolTagInformation = 0x16,
	SystemInterruptInformation = 0x17,
	SystemDpcBehaviorInformation = 0x18,
	SystemFullMemoryInformation = 0x19,
	SystemLoadGdiDriverInformation = 0x1a,
	SystemUnloadGdiDriverInformation = 0x1b,
	SystemTimeAdjustmentInformation = 0x1c,
	SystemSummaryMemoryInformation = 0x1d,
	SystemMirrorMemoryInformation = 0x1e,
	SystemPerformanceTraceInformation = 0x1f,
	SystemObsolete0 = 0x20,
	SystemExceptionInformation = 0x21,
	SystemCrashDumpStateInformation = 0x22,
	SystemKernelDebuggerInformation = 0x23,
	SystemContextSwitchInformation = 0x24,
	SystemRegistryQuotaInformation = 0x25,
	SystemExtendServiceTableInformation = 0x26,
	SystemPrioritySeperation = 0x27,
	SystemVerifierAddDriverInformation = 0x28,
	SystemVerifierRemoveDriverInformation = 0x29,
	SystemProcessorIdleInformation = 0x2a,
	SystemLegacyDriverInformation = 0x2b,
	SystemCurrentTimeZoneInformation = 0x2c,
	SystemLookasideInformation = 0x2d,
	SystemTimeSlipNotification = 0x2e,
	SystemSessionCreate = 0x2f,
	SystemSessionDetach = 0x30,
	SystemSessionInformation = 0x31,
	SystemRangeStartInformation = 0x32,
	SystemVerifierInformation = 0x33,
	SystemVerifierThunkExtend = 0x34,
	SystemSessionProcessInformation = 0x35,
	SystemLoadGdiDriverInSystemSpace = 0x36,
	SystemNumaProcessorMap = 0x37,
	SystemPrefetcherInformation = 0x38,
	SystemExtendedProcessInformation = 0x39,
	SystemRecommendedSharedDataAlignment = 0x3a,
	SystemComPlusPackage = 0x3b,
	SystemNumaAvailableMemory = 0x3c,
	SystemProcessorPowerInformation = 0x3d,
	SystemEmulationBasicInformation = 0x3e,
	SystemEmulationProcessorInformation = 0x3f,
	SystemExtendedHandleInformation = 0x40,
	SystemLostDelayedWriteInformation = 0x41,
	SystemBigPoolInformation = 0x42,
	SystemSessionPoolTagInformation = 0x43,
	SystemSessionMappedViewInformation = 0x44,
	SystemHotpatchInformation = 0x45,
	SystemObjectSecurityMode = 0x46,
	SystemWatchdogTimerHandler = 0x47,
	SystemWatchdogTimerInformation = 0x48,
	SystemLogicalProcessorInformation = 0x49,
	SystemWow64SharedInformationObsolete = 0x4a,
	SystemRegisterFirmwareTableInformationHandler = 0x4b,
	SystemFirmwareTableInformation = 0x4c,
	SystemModuleInformationEx = 0x4d,
	SystemVerifierTriageInformation = 0x4e,
	SystemSuperfetchInformation = 0x4f,
	SystemMemoryListInformation = 0x50,
	SystemFileCacheInformationEx = 0x51,
	SystemThreadPriorityClientIdInformation = 0x52,
	SystemProcessorIdleCycleTimeInformation = 0x53,
	SystemVerifierCancellationInformation = 0x54,
	SystemProcessorPowerInformationEx = 0x55,
	SystemRefTraceInformation = 0x56,
	SystemSpecialPoolInformation = 0x57,
	SystemProcessIdInformation = 0x58,
	SystemErrorPortInformation = 0x59,
	SystemBootEnvironmentInformation = 0x5a,			//系统的启动信息
	SystemHypervisorInformation = 0x5b,
	SystemVerifierInformationEx = 0x5c,
	SystemTimeZoneInformation = 0x5d,
	SystemImageFileExecutionOptionsInformation = 0x5e,
	SystemCoverageInformation = 0x5f,
	SystemPrefetchPatchInformation = 0x60,
	SystemVerifierFaultsInformation = 0x61,
	MaxSystemInfoClassWinXP,                 //WinXP最大支持到此处
	SystemSystemPartitionInformation = 0x62,	//用于获取系统分区信息，其结构为UNICODE_STRING
	SystemSystemDiskInformation = 0x63,
	SystemProcessorPerformanceDistribution = 0x64,
	SystemNumaProximityNodeInformation = 0x65,
	SystemDynamicTimeZoneInformation = 0x66,
	SystemCodeIntegrityInformation = 0x67,
	SystemProcessorMicrocodeUpdateInformation = 0x68,
	SystemProcessorBrandString = 0x69,
	SystemVirtualAddressInformation = 0x6a,
	SystemLogicalProcessorAndGroupInformation = 0x6b,
	SystemProcessorCycleTimeInformation = 0x6c,
	SystemStoreInformation = 0x6d,
	SystemRegistryAppendString = 0x6e,
	SystemAitSamplingValue = 0x6f,
	SystemVhdBootInformation = 0x70,
	SystemCpuQuotaInformation = 0x71,
	SystemNativeBasicInformation = 0x72,
	SystemErrorPortTimeouts = 0x73,
	SystemLowPriorityIoInformation = 0x74,
	SystemBootEntropyInformation = 0x75,
	SystemVerifierCountersInformation = 0x76,
	SystemPagedPoolInformationEx = 0x77,
	SystemSystemPtesInformationEx = 0x78,
	SystemNodeDistanceInformation = 0x79,
	SystemAcpiAuditInformation = 0x7a,
	SystemBasicPerformanceInformation = 0x7b,
	SystemQueryPerformanceCounterInformation = 0x7c,
	SystemSessionBigPoolInformation = 0x7d,
	SystemBootGraphicsInformation = 0x7e,
	SystemScrubPhysicalMemoryInformation = 0x7f,
	SystemBadPageInformation = 0x80,
	SystemProcessorProfileControlArea = 0x81,
	SystemCombinePhysicalMemoryInformation = 0x82,
	SystemEntropyInterruptTimingInformation = 0x83,
	SystemConsoleInformation = 0x84,
	SystemPlatformBinaryInformation = 0x85,
	SystemThrottleNotificationInformation = 0x86,
	SystemHypervisorProcessorCountInformation = 0x87,
	SystemDeviceDataInformation = 0x88,
	SystemDeviceDataEnumerationInformation = 0x89,
	SystemMemoryTopologyInformation = 0x8a,
	SystemMemoryChannelInformation = 0x8b,
	SystemBootLogoInformation = 0x8c,
	SystemProcessorPerformanceInformationEx = 0x8d,
	SystemSpare0 = 0x8e,
	SystemSecureBootPolicyInformation = 0x8f,
	SystemPageFileInformationEx = 0x90,
	SystemSecureBootInformation = 0x91,
	SystemEntropyInterruptTimingRawInformation = 0x92,
	SystemPortableWorkspaceEfiLauncherInformation = 0x93,
	SystemFullProcessInformation = 0x94,
	SystemKernelDebuggerInformationEx = 0x95,
	SystemBootMetadataInformation = 0x96,
	SystemSoftRebootInformation = 0x97,
	SystemElamCertificateInformation = 0x98,
	SystemOfflineDumpConfigInformation = 0x99,
	SystemProcessorFeaturesInformation = 0x9a,
	SystemRegistryReconciliationInformation = 0x9b,
	MaxSystemInfoClassWindows9= 0x9c,
	SystemKernelVaShadowInformation = 0x00C4,
	SystemSpeculationControlInformation = 0x00C9,
	MaxSystemInfoClass,
};

typedef struct _SYSTEM_SPECULATION_CONTROL_INFORMATION
{
	struct
	{
		ULONG BpbEnabled : 1;
		ULONG BpbDisabledSystemPolicy : 1;
		ULONG BpbDisabledNoHardwareSupport : 1;
		ULONG SpecCtrlEnumerated : 1;
		ULONG SpecCmdEnumerated : 1;
		ULONG IbrsPresent : 1;
		ULONG StibpPresent : 1;
		ULONG SmepPresent : 1;
		ULONG Reserved : 24;
	} SpeculationControlFlags;
} SYSTEM_SPECULATION_CONTROL_INFORMATION, *PSYSTEM_SPECULATION_CONTROL_INFORMATION;

typedef struct _SYSTEM_KERNEL_VA_SHADOW_INFORMATION
{
	struct
	{
		ULONG KvaShadowEnabled : 1;
		ULONG KvaShadowUserGlobal : 1;
		ULONG KvaShadowPcid : 1;
		ULONG KvaShadowInvpcid : 1;
		ULONG Reserved : 28;
	} KvaShadowFlags;
} SYSTEM_KERNEL_VA_SHADOW_INFORMATION, *PSYSTEM_KERNEL_VA_SHADOW_INFORMATION;


struct SYSTEM_BOOT_ENVIRONMENT_INFORMATION
{
	GUID CurrentEntryIdentifier;
	FIRMWARE_TYPE FirmwareType;
	DWORD Unknow[10];
};


typedef struct _RTL_BUFFER {
	PUCHAR    Buffer;
	PUCHAR    StaticBuffer;
	SIZE_T    Size;
	SIZE_T    StaticSize;
	SIZE_T    ReservedForAllocatedSize; // for future doubling
	PVOID     ReservedForIMalloc; // for future pluggable growth
} RTL_BUFFER, *PRTL_BUFFER;

struct _RTL_UNICODE_STRING_BUFFER;

typedef struct _RTL_UNICODE_STRING_BUFFER {
	UNICODE_STRING String;
	RTL_BUFFER     ByteBuffer;
	UCHAR          MinimumStaticBufferForTerminalNul[sizeof(WCHAR)];
} RTL_UNICODE_STRING_BUFFER, *PRTL_UNICODE_STRING_BUFFER;


typedef struct _FILE_FS_OBJECTID_INFORMATION {
	UCHAR ObjectId[16];
	UCHAR ExtendedInfo[48];
} FILE_FS_OBJECTID_INFORMATION, *PFILE_FS_OBJECTID_INFORMATION;

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtCreateFile(
		_Out_     PHANDLE FileHandle,
		_In_      ACCESS_MASK DesiredAccess,
		_In_      POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_     PIO_STATUS_BLOCK IoStatusBlock,
		_In_opt_  PLARGE_INTEGER AllocationSize,
		_In_      ULONG FileAttributes,
		_In_      ULONG ShareAccess,
		_In_      ULONG CreateDisposition,
		_In_      ULONG CreateOptions,
		_In_opt_  PVOID EaBuffer,
		_In_      ULONG EaLength
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI ZwCreateFile(
		_Out_     PHANDLE FileHandle,
		_In_      ACCESS_MASK DesiredAccess,
		_In_      POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_     PIO_STATUS_BLOCK IoStatusBlock,
		_In_opt_  PLARGE_INTEGER AllocationSize,
		_In_      ULONG FileAttributes,
		_In_      ULONG ShareAccess,
		_In_      ULONG CreateDisposition,
		_In_      ULONG CreateOptions,
		_In_      PVOID EaBuffer,
		_In_      ULONG EaLength
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI ZwQueryDirectoryFile(
		_In_      HANDLE FileHandle,
		_In_opt_  HANDLE Event,
		_In_opt_  PVOID ApcRoutine,
		_In_opt_  PVOID ApcContext,
		_Out_     PIO_STATUS_BLOCK IoStatusBlock,
		_Out_     PVOID FileInformation,
		_In_      ULONG Length,
		_In_      FILE_INFORMATION_CLASS FileInformationClass,
		_In_      BOOLEAN ReturnSingleEntry,
		_In_opt_  PUNICODE_STRING FileName,
		_In_      BOOLEAN RestartScan
		);

#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryDirectoryFile (
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass,
    _In_ BOOLEAN ReturnSingleEntry,
    _In_opt_ PUNICODE_STRING FileName,
    _In_ BOOLEAN RestartScan
    );
#endif

	EXTERN_C
	NTSYSAPI VOID NTAPI RtlInitUnicodeString(
		_Out_     PUNICODE_STRING DestinationString,
		_In_opt_  PCWSTR SourceString
		);

	EXTERN_C
	NTSYSAPI VOID NTAPI RtlCopyUnicodeString(
		_Inout_  PUNICODE_STRING  DestinationString,
		_In_opt_ PCUNICODE_STRING SourceString
	);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtLoadKey2(
		IN POBJECT_ATTRIBUTES   DestinationKeyName,
		IN POBJECT_ATTRIBUTES   HiveFileName,
		IN ULONG Flags);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlGetLastNtStatus();

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtOpenFile(
		_Out_  PHANDLE FileHandle,
		_In_   ACCESS_MASK DesiredAccess,
		_In_   POBJECT_ATTRIBUTES ObjectAttributes,
		_Out_  PIO_STATUS_BLOCK IoStatusBlock,
		_In_   ULONG ShareAccess,
		_In_   ULONG OpenOptions
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtShutdownSystem(INT Type);


	
	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtDeleteFile(
		_In_  POBJECT_ATTRIBUTES ObjectAttributes
		);

	EXTERN_C
	NTSYSAPI NTSTATUS WINAPI RtlCreateProcessParameters(

		OUT PRTL_USER_PROCESS_PARAMETERS *pProcessParameters,

		IN PUNICODE_STRING ImagePathName,

		IN PUNICODE_STRING DllPath OPTIONAL,

		IN PUNICODE_STRING CurrentDirectory OPTIONAL,

		IN PUNICODE_STRING CommandLine OPTIONAL,

		IN PVOID Environment OPTIONAL,

		IN PUNICODE_STRING WindowTitle OPTIONAL,

		IN PUNICODE_STRING DesktopInfo OPTIONAL,

		IN PUNICODE_STRING ShellInfo OPTIONAL,

		IN PUNICODE_STRING RuntimeData OPTIONAL

		);

	EXTERN_C
	NTSYSAPI NTSTATUS WINAPI ZwQuerySymbolicLinkObject(
		_In_       HANDLE LinkHandle,
		_Inout_    PUNICODE_STRING LinkTarget,
		_Out_opt_  PULONG ReturnedLength
		);

	EXTERN_C
	NTSYSAPI NTSTATUS WINAPI ZwOpenSymbolicLinkObject(
		_Out_  PHANDLE LinkHandle,
		_In_   ACCESS_MASK DesiredAccess,
		_In_   POBJECT_ATTRIBUTES ObjectAttributes
		);

	EXTERN_C
	NTSYSAPI NTSTATUS WINAPI ZwClose(
		_In_  HANDLE Handle
		);

#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_AMBIGUOUS   (0x00000001)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_UNC         (0x00000002)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_DRIVE       (0x00000003)
#define RTL_NT_PATH_NAME_TO_DOS_PATH_NAME_ALREADY_DOS (0x00000004)

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlNtPathNameToDosPathName(
		__in ULONG Flags,
		__inout PRTL_UNICODE_STRING_BUFFER Path,
		__out_opt PULONG Disposition,
		__inout_opt PWSTR* FilePart);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI IoVolumeDeviceToDosName(
		_In_   PVOID VolumeDeviceObject,
		_Out_  PUNICODE_STRING DosName
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlpEnsureBufferSize(
		ULONG flags,
		PRTL_BUFFER pBuffer,
		SIZE_T requiredSize
		);

	enum OBJECT_INFORMATION_CLASS
	{
		ObjectBasicInformation,
		ObjectNameInformation,
		ObjectTypeInformation,
		ObjectAllInformation,
		ObjectDataInformation

	};

	typedef struct _OBJECT_NAME_INFORMATION
	{
		UNICODE_STRING Name;
	} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

	typedef struct __PUBLIC_OBJECT_TYPE_INFORMATION {
		UNICODE_STRING TypeName;
		ULONG Reserved[22];    // reserved for internal use
	} PUBLIC_OBJECT_TYPE_INFORMATION, *PPUBLIC_OBJECT_TYPE_INFORMATION;

	typedef struct _OBJECT_TYPE_INFORMATION {
		UNICODE_STRING          TypeName;
		ULONG                   TotalNumberOfHandles;
		ULONG                   TotalNumberOfObjects;
		UNICODE_STRING		  OtherName;
		UNICODE_STRING		  OtherName2;
		ULONG                   HighWaterNumberOfHandles;
		ULONG                   HighWaterNumberOfObjects;
		UNICODE_STRING		  OtherName3;
		UNICODE_STRING		  OtherName4;
		ACCESS_MASK             InvalidAttributes;
		GENERIC_MAPPING         GenericMapping;
		ACCESS_MASK             ValidAttributes;
		BOOLEAN                 SecurityRequired;
		BOOLEAN                 MaintainHandleCount;
		USHORT                  MaintainTypeList;
		ULONG               PoolType;
		ULONG                   DefaultPagedPoolCharge;
		ULONG                   DefaultNonPagedPoolCharge;
	} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtQueryObject(
		_In_opt_  HANDLE                   Handle,
		_In_      OBJECT_INFORMATION_CLASS ObjectInformationClass,
		_Out_opt_ PVOID                    ObjectInformation,
		_In_      ULONG                    ObjectInformationLength,
		_Out_opt_ PULONG                   ReturnLength
		);
	//
	// These must be converted to LUIDs before use.
	//

#define SE_MIN_WELL_KNOWN_PRIVILEGE         (2L)
#define SE_CREATE_TOKEN_PRIVILEGE           (2L)
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE     (3L)
#define SE_LOCK_MEMORY_PRIVILEGE            (4L)
#define SE_INCREASE_QUOTA_PRIVILEGE         (5L)

#define SE_MACHINE_ACCOUNT_PRIVILEGE        (6L)
#define SE_TCB_PRIVILEGE                    (7L)
#define SE_SECURITY_PRIVILEGE               (8L)
#define SE_TAKE_OWNERSHIP_PRIVILEGE         (9L)
#define SE_LOAD_DRIVER_PRIVILEGE            (10L)
#define SE_SYSTEM_PROFILE_PRIVILEGE         (11L)
#define SE_SYSTEMTIME_PRIVILEGE             (12L)
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE    (13L)
#define SE_INC_BASE_PRIORITY_PRIVILEGE      (14L)
#define SE_CREATE_PAGEFILE_PRIVILEGE        (15L)
#define SE_CREATE_PERMANENT_PRIVILEGE       (16L)
#define SE_BACKUP_PRIVILEGE                 (17L)
#define SE_RESTORE_PRIVILEGE                (18L)
#define SE_SHUTDOWN_PRIVILEGE               (19L)
#define SE_DEBUG_PRIVILEGE                  (20L)
#define SE_AUDIT_PRIVILEGE                  (21L)
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE     (22L)
#define SE_CHANGE_NOTIFY_PRIVILEGE          (23L)
#define SE_REMOTE_SHUTDOWN_PRIVILEGE        (24L)
#define SE_UNDOCK_PRIVILEGE                 (25L)
#define SE_SYNC_AGENT_PRIVILEGE             (26L)
#define SE_ENABLE_DELEGATION_PRIVILEGE      (27L)
#define SE_MANAGE_VOLUME_PRIVILEGE          (28L)
#define SE_IMPERSONATE_PRIVILEGE            (29L)
#define SE_CREATE_GLOBAL_PRIVILEGE          (30L)
#define SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE (31L)
#define SE_RELABEL_PRIVILEGE                (32L)
#define SE_INC_WORKING_SET_PRIVILEGE        (33L)
#define SE_TIME_ZONE_PRIVILEGE              (34L)
#define SE_CREATE_SYMBOLIC_LINK_PRIVILEGE   (35L)
#define SE_MAX_WELL_KNOWN_PRIVILEGE         (SE_CREATE_SYMBOLIC_LINK_PRIVILEGE)
	EXTERN_C NTSYSAPI NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);


	typedef enum _FSINFOCLASS {
		FileFsVolumeInformation = 1,
		FileFsLabelInformation,      // 2
		FileFsSizeInformation,       // 3
		FileFsDeviceInformation,     // 4
		FileFsAttributeInformation,  // 5
		FileFsControlInformation,    // 6
		FileFsFullSizeInformation,   // 7
		FileFsObjectIdInformation,   // 8
		FileFsDriverPathInformation, // 9
		FileFsVolumeFlagsInformation,// 10
		FileFsSectorSizeInformation, // 11
		FileFsDataCopyInformation,   // 12
		FileFsMaximumInformation
	} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwQueryVolumeInformationFile(
		_In_ HANDLE FileHandle,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		PVOID FsInformation,
		_In_ ULONG Length,
		_In_ FS_INFORMATION_CLASS FsInformationClass
		);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueryVolumeInformationFile(
			IN HANDLE FileHandle,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			OUT PVOID FsInformation,
			IN ULONG Length,
			IN FS_INFORMATION_CLASS FsInformationClass
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueryFullAttributesFile(
			IN POBJECT_ATTRIBUTES pObjAttrs,
			IN OUT PFILE_NETWORK_OPEN_INFORMATION pNetInf
			);

	typedef enum _KEY_INFORMATION_CLASS {
		KeyBasicInformation = 0,
		KeyNodeInformation = 1,
		KeyFullInformation = 2,
		KeyNameInformation = 3,
		KeyCachedInformation = 4,
		KeyFlagsInformation = 5,
		KeyVirtualizationInformation = 6,
		KeyHandleTagsInformation = 7,
		MaxKeyInfoClass = 8
	} KEY_INFORMATION_CLASS;


	typedef struct _KEY_VIRTUALIZATION_INFORMATION {
		ULONG VirtualizationCandidate: 1;
		ULONG VirtualizationEnabled : 1;
		ULONG VirtualTarget: 1;
		ULONG VirtualStore: 1;
		ULONG VirtualSource: 1;
		ULONG Reserved: 27;
	} KEY_VIRTUALIZATION_INFORMATION, *PKEY_VIRTUALIZATION_INFORMATION;

	EXTERN_C
	NTSYSAPI
	NTSTATUS NTAPI ZwQueryKey(
		_In_       HANDLE                KeyHandle,
		_In_       KEY_INFORMATION_CLASS KeyInformationClass,
		_Out_opt_  PVOID                 KeyInformation,
		_In_       ULONG                 Length,
		_Out_      PULONG                ResultLength
		);


	typedef struct _KEY_SET_VIRTUALIZATION_INFORMATION {
		ULONG   VirtualTarget : 1; // Tells if the key is a virtual target key.
		ULONG   VirtualStore : 1; // Tells if the key is a virtual store key.
		ULONG   VirtualSource : 1; // Tells if the key has been virtualized at least one (virtual hint)
		ULONG   Reserved : 29;
	} KEY_SET_VIRTUALIZATION_INFORMATION, *PKEY_SET_VIRTUALIZATION_INFORMATION;

	typedef enum _KEY_SET_INFORMATION_CLASS {
		KeyWriteTimeInformation,
		KeyWow64FlagsInformation,
		KeyControlFlagsInformation,
		KeySetVirtualizationInformation,
		KeySetDebugInformation,
		KeySetHandleTagsInformation,
		MaxKeySetInfoClass  // MaxKeySetInfoClass should always be the last enum
	} KEY_SET_INFORMATION_CLASS;

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetInformationKey(
		_In_ HANDLE KeyHandle,
		_In_ __drv_strictTypeMatch(__drv_typeConst)
		KEY_SET_INFORMATION_CLASS KeySetInformationClass,
		_In_reads_bytes_(KeySetInformationLength) PVOID KeySetInformation,
		_In_ ULONG KeySetInformationLength
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtSetInformationFile(
		_In_ HANDLE FileHandle,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_In_ PVOID FileInformation,
		_In_ ULONG Length,
		_In_ FILE_INFORMATION_CLASS FileInformationClass);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtQueryInformationFile(
		_In_ HANDLE FileHandle,
		_Out_ PIO_STATUS_BLOCK IoStatusBlock,
		_Out_ PVOID FileInformation,
		_In_ ULONG Length,
		_In_ FILE_INFORMATION_CLASS FileInformationClass
		);

	typedef struct _FILE_FS_LABEL_INFORMATION {
		ULONG VolumeLabelLength;
		WCHAR VolumeLabel[1];
	} FILE_FS_LABEL_INFORMATION, *PFILE_FS_LABEL_INFORMATION;

	typedef struct _FILE_FS_VOLUME_INFORMATION {
		LARGE_INTEGER VolumeCreationTime;
		ULONG VolumeSerialNumber;
		ULONG VolumeLabelLength;
		BOOLEAN SupportsObjects;
		WCHAR VolumeLabel[1];
	} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;

	typedef struct _FILE_FS_SIZE_INFORMATION {
		LARGE_INTEGER TotalAllocationUnits;
		LARGE_INTEGER AvailableAllocationUnits;
		ULONG SectorsPerAllocationUnit;
		ULONG BytesPerSector;
	} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

	typedef struct _FILE_FS_FULL_SIZE_INFORMATION {
		LARGE_INTEGER TotalAllocationUnits;
		LARGE_INTEGER CallerAvailableAllocationUnits;
		LARGE_INTEGER ActualAvailableAllocationUnits;
		ULONG SectorsPerAllocationUnit;
		ULONG BytesPerSector;
	} FILE_FS_FULL_SIZE_INFORMATION, *PFILE_FS_FULL_SIZE_INFORMATION;


	typedef struct _FILE_FS_DEVICE_INFORMATION {
		ULONG DeviceType;
		ULONG Characteristics;
	} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlCompressBuffer(
		_In_  USHORT CompressionFormatAndEngine,//COMPRESSION_FORMAT_NONE
		_In_  PUCHAR UncompressedBuffer,
		_In_  ULONG  UncompressedBufferSize,
		_Out_ PUCHAR CompressedBuffer,
		_In_  ULONG  CompressedBufferSize,
		_In_  ULONG  UncompressedChunkSize,
		_Out_ PULONG FinalCompressedSize,
		_In_  PVOID  WorkSpace
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlGetCompressionWorkSpaceSize(
		_In_  USHORT CompressionFormatAndEngine,
		_Out_ PULONG CompressBufferWorkSpaceSize,
		_Out_ PULONG CompressFragmentWorkSpaceSize
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI RtlDecompressBuffer(
		_In_  USHORT CompressionFormat,
		_Out_ PUCHAR UncompressedBuffer,
		_In_  ULONG  UncompressedBufferSize,
		_In_  PUCHAR CompressedBuffer,
		_In_  ULONG  CompressedBufferSize,
		_Out_ PULONG FinalUncompressedSize
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI	NtReadVirtualMemory(
		IN HANDLE ProcessHandle,
		IN PVOID BaseAddress,
		OUT PVOID Buffer,
		IN SIZE_T BufferLength,
		OUT PSIZE_T ReturnLength OPTIONAL);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI	NtWriteVirtualMemory(
		IN HANDLE               ProcessHandle,
		IN PVOID                BaseAddress,
		IN PVOID                Buffer,
		IN SIZE_T                NumberOfBytesToWrite,
		OUT PSIZE_T              NumberOfBytesWritten OPTIONAL);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtWow64ReadVirtualMemory64(
		__in HANDLE ProcessHandle,
		__in_opt PVOID64 BaseAddress,
		__out_bcount(BufferSize) PVOID Buffer,
		__in ULONGLONG BufferSize,
		__out_opt PULONGLONG NumberOfBytesRead);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI NtWow64WriteVirtualMemory64(
		__in HANDLE ProcessHandle,
		__in_opt PVOID64 BaseAddress,
		__in_bcount(BufferSize) PVOID Buffer,
		__in ULONGLONG BufferSize,
		__out_opt PULONGLONG NumberOfBytesWritten);

	typedef enum _PROCESSINFOCLASS
	{
		ProcessBasicInformation,
		ProcessQuotaLimits,
		ProcessIoCounters,
		ProcessVmCounters,
		ProcessTimes,
		ProcessBasePriority,
		ProcessRaisePriority,
		ProcessDebugPort,
		ProcessExceptionPort,
		ProcessAccessToken,
		ProcessLdtInformation,
		ProcessLdtSize,
		ProcessDefaultHardErrorMode,
		ProcessIoPortHandlers,
		ProcessPooledUsageAndLimits,
		ProcessWorkingSetWatch,
		ProcessUserModeIOPL,
		ProcessEnableAlignmentFaultFixup,
		ProcessPriorityClass,
		ProcessWx86Information,
		ProcessHandleCount,
		ProcessAffinityMask,
		ProcessPriorityBoost,
		ProcessDeviceMap,
		ProcessSessionInformation,
		ProcessForegroundInformation,
		ProcessWow64Information,
		ProcessImageFileName,			//获取进程的NT路径，结构为UNICODE_STRING
		ProcessLUIDDeviceMapsEnabled,
		ProcessBreakOnTermination,
		ProcessDebugObjectHandle,
		ProcessDebugFlags,
		ProcessHandleTracing,
		ProcessIoPriority,
		ProcessExecuteFlags,
		ProcessTlsInformation,			// invalid class
		ProcessCookie,
		MaxProcessInfoClassWinXP,
		ProcessImageInformation = MaxProcessInfoClassWinXP,
		ProcessCycleTime,
		ProcessPagePriority,
		ProcessInstrumentationCallback,
		ProcessThreadStackAllocation,
		ProcessWorkingSetWatchEx,
		ProcessImageFileNameWin32,			//获取进程的Dos路径，结构为UNICODE_STRING
		ProcessImageFileMapping,			// buffer is a pointer to a file handle open with SYNCHRONIZE | FILE_EXECUTE access, return value is whether the handle is the same used to start the process
		ProcessAffinityUpdateMode,
		ProcessMemoryAllocationMode,
		ProcessGroupInformation,
		ProcessTokenVirtualizationEnabled,	// invalid class
		ProcessConsoleHostProcess,			// retrieves the pid for the process' corresponding conhost process
		ProcessWindowInformation,			// returns the windowflags and windowtitle members of the process' peb->rtl_user_process_params
		MaxProcessInfoClass
	} PROCESSINFOCLASS;


	struct WOW64_PROCESS_BASIC_INFORMATION
	{
		NTSTATUS  ExitStatus;
		PVOID64  PebBaseAddress;
		KAFFINITY  AffinityMask;
		DWORD  BasePriority;
		HANDLE  UniqueProcessId;
		HANDLE  InheritedFromUniqueProcessId;

	};

	EXTERN_C
	NTSYSAPI
	NTSTATUS WINAPI NtQueryInformationProcess(
		_In_      HANDLE           ProcessHandle,
		_In_      PROCESSINFOCLASS ProcessInformationClass,
		_Out_     PVOID            ProcessInformation,
		_In_      ULONG            ProcessInformationLength,
		_Out_opt_ PULONG           ReturnLength
		);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI
		NtWow64QueryInformationProcess64(
		__in HANDLE ProcessHandle,
		__in PROCESSINFOCLASS ProcessInformationClass,
		__out_bcount(ProcessInformationLength) PVOID ProcessInformation64,
		__in ULONG ProcessInformationLength,
		__out_opt PULONG ReturnLength
		);


	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI	NtLoadKey2(	IN POBJECT_ATTRIBUTES DestinationKeyName,	IN POBJECT_ATTRIBUTES   HiveFileName,IN ULONG  Flags);

	typedef VOID(NTAPI * PLDR_CALLBACK)(PVOID CallbackContext, PCHAR Name);

	EXTERN_C
	NTSYSAPI NTSTATUS NTAPI LdrVerifyImageMatchesChecksum(IN HANDLE  FileHandle,
		IN PLDR_CALLBACK  Callback,
		IN PVOID  CallbackContext,
		OUT PUSHORT  ImageCharacteristics
		);

	EXTERN_C
	NTSYSAPI PIMAGE_NT_HEADERS NTAPI RtlImageNtHeader(_In_ PVOID  BaseAddress);

	EXTERN_C
	NTSYSAPI PVOID NTAPI RtlImageRvaToVa(_In_ PIMAGE_NT_HEADERS  NtHeader,
		_In_ PVOID  BaseAddress,
		_In_ ULONG  Rva,
		_Inout_opt_ PIMAGE_SECTION_HEADER *  SectionHeader);

	EXTERN_C
	NTSYSAPI BOOLEAN NTAPI RtlCreateUnicodeString(
			OUT PUNICODE_STRING  DestinationString,
			IN PCWSTR  SourceString
			);

#define HASH_STRING_ALGORITHM_DEFAULT   (0)
#define HASH_STRING_ALGORITHM_X65599    (1)
#define HASH_STRING_ALGORITHM_INVALID   (0xffffffff)
#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
	NTSYSAPI
		NTSTATUS
		NTAPI
		RtlHashUnicodeString(
			__in PCUNICODE_STRING String,
			__in BOOLEAN CaseInSensitive,
			__in ULONG HashAlgorithm,
			__out PULONG HashValue
			);
#endif

	EXTERN_C
	NTSYSAPI VOID NTAPI  RtlRunDecodeUnicodeString(IN UCHAR Hash, IN OUT PUNICODE_STRING String);

	EXTERN_C
	NTSYSAPI VOID NTAPI  RtlRunEncodeUnicodeString(IN OUT PUCHAR Hash, IN OUT PUNICODE_STRING String);

	EXTERN_C
	NTSYSAPI
		ULONG
		NTAPI
		RtlGetFullPathName_U(
			const WCHAR* name,
			ULONG size,
			WCHAR* buffer,
			WCHAR** file_part
			);

	typedef enum _EVENT_TYPE {
		NotificationEvent,
		SynchronizationEvent
	} EVENT_TYPE;

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtSetEvent(
			IN HANDLE EventHandle,
			OUT PLONG PreviousState OPTIONAL
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtCreateEvent(
			OUT PHANDLE  EventHandle,
			IN ACCESS_MASK  DesiredAccess,
			IN POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
			IN EVENT_TYPE  EventType,
			IN BOOLEAN  InitialState
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtWaitForSingleObject(
			IN HANDLE Handle,
			IN BOOLEAN Alertable,
			IN PLARGE_INTEGER Timeout OPTIONAL
			);

	//typedef VOID(CALLBACK*PIO_APC_ROUTINE)(VOID);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtWriteFile(
			IN HANDLE  FileHandle,
			IN HANDLE  Event  OPTIONAL,
			IN PIO_APC_ROUTINE  ApcRoutine  OPTIONAL,
			IN PVOID  ApcContext  OPTIONAL,
			OUT PIO_STATUS_BLOCK  IoStatusBlock,
			IN PVOID  Buffer,
			IN ULONG  Length,
			IN PLARGE_INTEGER  ByteOffset  OPTIONAL,
			IN PULONG  Key  OPTIONAL
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtReadFile(
			IN HANDLE  FileHandle,
			IN HANDLE  Event  OPTIONAL,
			IN PIO_APC_ROUTINE  ApcRoutine  OPTIONAL,
			IN PVOID  ApcContext  OPTIONAL,
			OUT PIO_STATUS_BLOCK  IoStatusBlock,
			OUT PVOID  Buffer,
			IN ULONG  Length,
			IN PLARGE_INTEGER  ByteOffset  OPTIONAL,
			IN PULONG  Key  OPTIONAL
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtTerminateThread(
			IN HANDLE ThreadHandle,
			IN NTSTATUS ExitStatus
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtAlertThread(
			IN HANDLE ThreadHandle
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtResumeThread(
			IN HANDLE ThreadHandle,
			OUT PULONG SuspendCount OPTIONAL
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtTerminateProcess(
			IN HANDLE ProcessHandle,
			IN NTSTATUS ExitStatus
			);

	typedef enum _HARDERROR_RESPONSE_OPTION {
		OptionAbortRetryIgnore,
		OptionOk,
		OptionOkCancel,
		OptionRetryCancel,
		OptionYesNo,
		OptionYesNoCancel,
		OptionShutdownSystem
	} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtRaiseHardError(
			IN NTSTATUS ErrorStatus,
			IN ULONG NumberOfParameters,
			IN ULONG UnicodeStringParameterMask,
			IN PULONG_PTR Parameters,
			IN HARDERROR_RESPONSE_OPTION ValidResponseOptions,
			OUT PHARDERROR_RESPONSE_OPTION Response
			);

	typedef enum _MEMORY_INFORMATION_CLASS
	{
		MemoryBasicInformation
	} MEMORY_INFORMATION_CLASS, *PMEMORY_INFORMATION_CLASS;

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueryVirtualMemory(
			IN HANDLE ProcessHandle,
			IN PVOID BaseAddress,
			IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
			OUT PVOID Buffer,
			IN ULONG Length,
			OUT PULONG ResultLength OPTIONAL
			);

	typedef enum _MEMORY_MAP_TYPE
	{
		Map_Process = 1
	} MEMORY_MAP_TYPE, *PMEMORY_MAP_TYPE;

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtLockVirtualMemory(
			IN HANDLE ProcessHandle,
			IN OUT PVOID* BaseAddress,
			IN PSIZE_T RegionSize,
			IN MEMORY_MAP_TYPE MapType
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtAllocateVirtualMemory(
			IN HANDLE  ProcessHandle,
			IN OUT PVOID  *BaseAddress,
			IN ULONG  ZeroBits,
			IN OUT PSIZE_T  RegionSize,
			IN ULONG  AllocationType,
			IN ULONG  Protect
			);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtFreeVirtualMemory(
			IN HANDLE  ProcessHandle,
			IN OUT PVOID  *BaseAddress,
			IN OUT PSIZE_T  RegionSize,
			IN ULONG  FreeType
			);

	typedef VOID(NTAPI*PPS_APC_ROUTINE)(PVOID, PVOID, PVOID);

	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueueApcThread(
			IN HANDLE ThreadHandle,
			IN PPS_APC_ROUTINE ApcRoutine,
			IN PVOID ApcArgument1,
			IN PVOID ApcArgument2,
			IN PVOID ApcArgument3
			);

#define NtCurrentProcess() ((HANDLE)(LONG_PTR) -1)  
#define ZwCurrentProcess() NtCurrentProcess()         
#define NtCurrentThread() ((HANDLE)(LONG_PTR) -2)   
#define ZwCurrentThread() NtCurrentThread()

	EXTERN_C
	NTSYSAPI
		SIZE_T
		NTAPI
		RtlCompactHeap(
			IN HANDLE hHeap,
			IN ULONG flags
			);

	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlDestroyHeap(
			IN HANDLE hHeap
			);

	EXTERN_C
	NTSYSAPI
		VOID
		NTAPI
		RtlEnterCriticalSection(
			IN LPCRITICAL_SECTION pSection
			);

	EXTERN_C
	NTSYSAPI
		VOID
		NTAPI
		RtlLeaveCriticalSection(
			IN LPCRITICAL_SECTION pSection
			);

	EXTERN_C
	NTSYSAPI
	VOID
		NTAPI
		RtlDeleteCriticalSection(
			IN LPCRITICAL_SECTION pSection
			);

	EXTERN_C
	NTSYSAPI
	ULONG
		NTAPI
		RtlNtStatusToDosError(
			IN NTSTATUS status
			);

	EXTERN_C
	NTSYSAPI
	VOID
		NTAPI
		RtlSetLastWin32Error(
			IN ULONG error
			);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
		NTAPI
		NtClose(
			IN HANDLE hObject
			);


#define DIRECTORY_QUERY                 (0x0001)
#define DIRECTORY_TRAVERSE              (0x0002)
#define DIRECTORY_CREATE_OBJECT         (0x0004)
#define DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)

#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtCreateDirectoryObject(
		OUT PHANDLE DirectoryHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtOpenDirectoryObject(
		OUT PHANDLE DirectoryHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes
		);

	typedef struct _OBJECT_DIRECTORY_INFORMATION
	{
		UNICODE_STRING Name;
		UNICODE_STRING TypeName;
	} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtQueryDirectoryObject(
		HANDLE DirectoryHandle,
		PVOID Buffer,
		ULONG Length,
		BOOLEAN ReturnSingleEntry,
		BOOLEAN RestartScan,
		PULONG Context,
		PULONG ReturnLength
		);


	EXTERN_C
	NTSYSAPI
	VOID
	NTAPI
	RtlInitAnsiString(
		PANSI_STRING DestinationString,
		PCSTR SourceString
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	RtlAnsiStringToUnicodeString(
	IN OUT PUNICODE_STRING DestinationString,
	IN PCANSI_STRING SourceString,
	IN BOOLEAN AllocateDestinationString
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	RtlDestroyProcessParameters(
		IN PPROCESS_PARAMETERS_VISTA ProcessParameters
		);

	EXTERN_C
	NTSYSAPI
	BOOLEAN
	NTAPI
	RtlDosPathNameToNtPathName_U(
		IN PCWSTR DosName,
		OUT PUNICODE_STRING NtName,
		OUT PCWSTR *DosFilePath OPTIONAL,
		OUT PUNICODE_STRING NtFilePath OPTIONAL
		);
	
	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	RtlDosPathNameToNtPathName_U_WithStatus(
		IN PCWSTR DosName,
		OUT PUNICODE_STRING NtName,
		OUT PCWSTR *DosFilePath OPTIONAL,
		OUT PUNICODE_STRING NtFilePath OPTIONAL
		);

	EXTERN_C
	NTSYSAPI
	BOOLEAN
	NTAPI
	RtlDosPathNameToRelativeNtPathName_U(
		IN PCWSTR DosName,
		OUT PUNICODE_STRING NtName,
		OUT PCWSTR *DosFilePath OPTIONAL,
		OUT PUNICODE_STRING NtFilePath OPTIONAL
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	RtlDosPathNameToRelativeNtPathName_U_WithStatus(
		IN PCWSTR DosName,
		OUT PUNICODE_STRING NtName,
		OUT PCWSTR *DosFilePath OPTIONAL,
		OUT PUNICODE_STRING NtFilePath OPTIONAL
		);

	enum RTL_PATH_TYPE
	{
		RtlPathTypeUnknown,
		RtlPathTypeUncAbsolute,
		RtlPathTypeDriveAbsolute,
		RtlPathTypeDriveRelative,
		RtlPathTypeRooted,
		RtlPathTypeRelative,
		RtlPathTypeLocalDevice,
		RtlPathTypeRootLocalDevice,
	};

	EXTERN_C
	NTSYSAPI
	ULONG
	NTAPI
	RtlDetermineDosPathNameType_U(
		IN PCWSTR Path
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	RtlInitUnicodeStringEx(
		OUT PUNICODE_STRING DestinationString,
		IN PCWSTR SourceString OPTIONAL
		);

	EXTERN_C
	NTSYSAPI
	VOID
	NTAPI
	RtlFreeUnicodeString(
		PUNICODE_STRING UnicodeString
		);

	EXTERN_C
	NTSYSAPI
	PIMAGE_NT_HEADERS
	NTAPI
	RtlImageNtHeader(
		__in PVOID baseAddress
		);

	EXTERN_C
	NTSYSAPI
	PVOID
	NTAPI
	RtlImageDirectoryEntryToData(
		__in PVOID pBaseAddress,
		__in BOOLEAN bImageLoaded,
		__in ULONG dwDirectory,
		__out PULONG pSize
		);

	EXTERN_C
	NTSYSAPI
	PVOID
	NTAPI
	RtlImageDirectoryEntryToDataEx(
		__in PVOID pBaseAddress,
		__in BOOLEAN bImageLoaded,
		__in ULONG dwDirectory,
		__out PULONG pSize,
		__out_opt PIMAGE_SECTION_HEADER pSection
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtCreateProcess(
		OUT PHANDLE ProcessHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes,
		IN HANDLE InheritFromProcessHandle,
		IN BOOLEAN InheritHandles,
		IN HANDLE SectionHandle OPTIONAL,
		IN HANDLE DebugPort OPTIONAL,
		IN HANDLE ExceptionPort OPTIONAL
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtCreatePort(
		OUT PHANDLE PortHandle,
		IN POBJECT_ATTRIBUTES ObjectAttributes,
		IN ULONG MaxDataSize,
		IN ULONG MaxMessageSize,
		IN ULONG Reserved
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtCreateWaitablePort(
		OUT PHANDLE PortHandle,
		IN POBJECT_ATTRIBUTES ObjectAttributes,
		IN ULONG MaxDataSize,
		IN ULONG MaxMessageSize,
		IN ULONG Reserved
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtReplyWaitReceivePort(
		IN HANDLE PortHandle,
		OUT PULONG PortIdentifier OPTIONAL,
		IN PPORT_MESSAGE ReplyMessage OPTIONAL,
		OUT PPORT_MESSAGE Message
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtReplyPort(
		IN HANDLE PortHandle,
		IN PPORT_MESSAGE ReplyMessage
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtCreateThread(
		OUT PHANDLE ThreadHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes,
		IN HANDLE ProcessHandle,
		OUT PCLIENT_ID ClientId,
		IN PCONTEXT ThreadContext,
		IN PINITIAL_TEB UserStack,
		IN BOOLEAN CreateSuspended
		);

	typedef enum _SECTION_INHERIT {
		ViewShare = 1,
		ViewUnmap = 2
	} SECTION_INHERIT;

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtCreateSection(
		__out PHANDLE SectionHandle,
		__in ACCESS_MASK DesiredAccess,
		__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
		__in_opt PLARGE_INTEGER MaximumSize,
		__in ULONG SectionPageProtection,
		__in ULONG AllocationAttributes,
		__in_opt HANDLE FileHandle
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtOpenSection(
		__out PHANDLE SectionHandle,
		__in ACCESS_MASK DesiredAccess,
		__in POBJECT_ATTRIBUTES ObjectAttributes
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtMapViewOfSection(
		__in HANDLE SectionHandle,
		__in HANDLE ProcessHandle,
		__inout PVOID *BaseAddress,
		__in ULONG_PTR ZeroBits,
		__in SIZE_T CommitSize,
		__inout_opt PLARGE_INTEGER SectionOffset,
		__inout PSIZE_T ViewSize,
		__in SECTION_INHERIT InheritDisposition,
		__in ULONG AllocationType,
		__in ULONG Win32Protect
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	NtUnmapViewOfSection(
		__in HANDLE ProcessHandle,
		__in_opt PVOID BaseAddress
		);

	

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	LdrLoadDll(
		IN PWCHAR SearchPath OPTIONAL,
		IN PULONG Flags OPTIONAL,
		IN PUNICODE_STRING ModuleFileName,
		OUT HMODULE *ModuleHandle
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	LdrUnloadDll(
		IN HANDLE ModuleHandle
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	LdrGetLoadDll(
		IN PWCHAR SearchPath OPTIONAL,
		IN PULONG Flags OPTIONAL,
		IN PUNICODE_STRING ModuleFileName,
		OUT HMODULE *ModuleHandle
		);

	EXTERN_C
	NTSYSAPI
	NTSTATUS
	NTAPI
	LdrGetProcedureAddress(
		IN HMODULE ModuleHandle,
		IN PANSI_STRING FunctionName OPTIONAL,
		IN WORD Oridinal OPTIONAL,
		OUT PVOID *FunctionAddress
		);

	typedef struct _PROCESS_WINDOW_INFORMATION
	{
		ULONG WindowFlags;
		USHORT titleLen;
		WCHAR title[0];
	} PROCESS_WINDOW_INFORMATION;

	typedef struct _PROCESS_IMAGE_INFORMATION
	{
		PVOID pEntryPoint;
		PVOID unk;
		ULONG_PTR stackReserve;
		ULONG_PTR stackCommit;
		USHORT subsystem;
		USHORT unk2;
		ULONG osVersion;
		ULONG imageVersion;
		USHORT dllCharacteristics;
		USHORT fileHeaderCharacteristics;
		USHORT machine;
		USHORT unk3;
		PVOID unk4;
		PVOID unk5;
		ULONG checksum;
	} PROCESS_IMAGE_INFORMATION;

	//
	// Process Information Structures
	//

	//
	// Working set page priority information.
	// Used with ProcessPagePriority and ThreadPagePriority
	//

	typedef struct _PAGE_PRIORITY_INFORMATION {
		ULONG PagePriority;
	} PAGE_PRIORITY_INFORMATION, *PPAGE_PRIORITY_INFORMATION;

	//
	// PageFaultHistory Information
	//  NtQueryInformationProcess using ProcessWorkingSetWatch
	//
	typedef struct _PROCESS_WS_WATCH_INFORMATION {
		PVOID FaultingPc;
		PVOID FaultingVa;
	} PROCESS_WS_WATCH_INFORMATION, *PPROCESS_WS_WATCH_INFORMATION;

	//
	// Basic and Extended Basic Process Information
	//  NtQueryInformationProcess using ProcessBasicInformation
	//

	typedef ULONG KPRIORITY;

	typedef struct _PROCESS_BASIC_INFORMATION {
		NTSTATUS ExitStatus;
		PPEB PebBaseAddress;
		ULONG_PTR AffinityMask;
		KPRIORITY BasePriority;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

	typedef struct _PROCESS_EXTENDED_BASIC_INFORMATION {
		SIZE_T Size;    // Must be set to structure size on input
		PROCESS_BASIC_INFORMATION BasicInfo;
		union {
			ULONG Flags;
			struct {
				ULONG IsProtectedProcess : 1;
				ULONG IsWow64Process : 1;
				ULONG IsProcessDeleting : 1;
				ULONG IsCrossSessionCreate : 1;
				ULONG SpareBits : 28;
			} DUMMYSTRUCTNAME;
		} DUMMYUNIONNAME;
	} PROCESS_EXTENDED_BASIC_INFORMATION, *PPROCESS_EXTENDED_BASIC_INFORMATION;

	//
	// Process Device Map information
	//  NtQueryInformationProcess using ProcessDeviceMap
	//  NtSetInformationProcess using ProcessDeviceMap
	//

	typedef struct _PROCESS_DEVICEMAP_INFORMATION {
		union {
			struct {
				HANDLE DirectoryHandle;
			} Set;
			struct {
				ULONG DriveMap;
				UCHAR DriveType[32];
			} Query;
		} DUMMYUNIONNAME;
	} PROCESS_DEVICEMAP_INFORMATION, *PPROCESS_DEVICEMAP_INFORMATION;

	typedef struct _PROCESS_DEVICEMAP_INFORMATION_EX {
		union {
			struct {
				HANDLE DirectoryHandle;
			} Set;
			struct {
				ULONG DriveMap;
				UCHAR DriveType[32];
			} Query;
		} DUMMYUNIONNAME;
		ULONG Flags;    // specifies that the query type
	} PROCESS_DEVICEMAP_INFORMATION_EX, *PPROCESS_DEVICEMAP_INFORMATION_EX;

	//
	// PROCESS_DEVICEMAP_INFORMATION_EX flags
	//
#define PROCESS_LUID_DOSDEVICES_ONLY 0x00000001

	//
	// Multi-User Session specific Process Information
	//  NtQueryInformationProcess using ProcessSessionInformation
	//

	typedef struct _PROCESS_SESSION_INFORMATION {
		ULONG SessionId;
	} PROCESS_SESSION_INFORMATION, *PPROCESS_SESSION_INFORMATION;

	typedef struct _PROCESS_HANDLE_TRACING_ENABLE {
		ULONG Flags;
	} PROCESS_HANDLE_TRACING_ENABLE, *PPROCESS_HANDLE_TRACING_ENABLE;

	typedef struct _PROCESS_HANDLE_TRACING_ENABLE_EX {
		ULONG Flags;
		ULONG TotalSlots;
	} PROCESS_HANDLE_TRACING_ENABLE_EX, *PPROCESS_HANDLE_TRACING_ENABLE_EX;


#define PROCESS_HANDLE_TRACING_MAX_STACKS 16

	typedef struct _PROCESS_HANDLE_TRACING_ENTRY {
		HANDLE Handle;
		CLIENT_ID ClientId;
		ULONG Type;
		PVOID Stacks[PROCESS_HANDLE_TRACING_MAX_STACKS];
	} PROCESS_HANDLE_TRACING_ENTRY, *PPROCESS_HANDLE_TRACING_ENTRY;

	typedef struct _PROCESS_HANDLE_TRACING_QUERY {
		HANDLE Handle;
		ULONG  TotalTraces;
		PROCESS_HANDLE_TRACING_ENTRY HandleTrace[1];
	} PROCESS_HANDLE_TRACING_QUERY, *PPROCESS_HANDLE_TRACING_QUERY;

	//
	// Process Quotas
	//  NtQueryInformationProcess using ProcessQuotaLimits
	//  NtQueryInformationProcess using ProcessPooledQuotaLimits
	//  NtSetInformationProcess using ProcessQuotaLimits
	//

#define QUOTA_LIMITS_HARDWS_MIN_ENABLE  0x00000001
#define QUOTA_LIMITS_HARDWS_MIN_DISABLE 0x00000002
#define QUOTA_LIMITS_HARDWS_MAX_ENABLE  0x00000004
#define QUOTA_LIMITS_HARDWS_MAX_DISABLE 0x00000008
#define QUOTA_LIMITS_USE_DEFAULT_LIMITS 0x00000010

	//
	// Process Security Context Information
	//  NtSetInformationProcess using ProcessAccessToken
	// PROCESS_SET_ACCESS_TOKEN access to the process is needed
	// to use this info level.
	//

	typedef struct _PROCESS_ACCESS_TOKEN {

		//
		// Handle to Primary token to assign to the process.
		// TOKEN_ASSIGN_PRIMARY access to this token is needed.
		//

		HANDLE Token;

		//
		// Handle to the initial thread of the process.
		// A process's access token can only be changed if the process has
		// no threads or a single thread that has not yet begun execution.
		//
		// N.B. This field is unused.
		//

		HANDLE Thread;

	} PROCESS_ACCESS_TOKEN, *PPROCESS_ACCESS_TOKEN;

	//
	// Process Exception Port Information
	//  NtSetInformationProcess using ProcessExceptionPort
	// PROCESS_SET_PORT access to the process is needed
	// to use this info level.
	//

#define PROCESS_EXCEPTION_PORT_ALL_STATE_BITS     0x00000003UL
#define PROCESS_EXCEPTION_PORT_ALL_STATE_FLAGS    ((ULONG_PTR)((1UL << PROCESS_EXCEPTION_PORT_ALL_STATE_BITS) - 1))

	typedef struct _PROCESS_EXCEPTION_PORT {

		//
		// Handle to the exception port. No particular access required.
		//

		IN HANDLE ExceptionPortHandle;

		//
		// Miscellaneous state flags to be cached along with the exception
		// port in the kernel.
		//

		IN OUT ULONG StateFlags;

	} PROCESS_EXCEPTION_PORT, *PPROCESS_EXCEPTION_PORT;

	//
	// Process/Thread System and User Time
	//  NtQueryInformationProcess using ProcessTimes
	//  NtQueryInformationThread using ThreadTimes
	//

	typedef struct _KERNEL_USER_TIMES {
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER ExitTime;
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
	} KERNEL_USER_TIMES;
	typedef KERNEL_USER_TIMES *PKERNEL_USER_TIMES;

	//
	// Thread Information Classes
	//

	typedef enum _THREADINFOCLASS {
		ThreadBasicInformation,
		ThreadTimes,
		ThreadPriority,
		ThreadBasePriority,
		ThreadAffinityMask,
		ThreadImpersonationToken,
		ThreadDescriptorTableEntry,
		ThreadEnableAlignmentFaultFixup,
		ThreadEventPair_Reusable,
		ThreadQuerySetWin32StartAddress,
		ThreadZeroTlsCell,
		ThreadPerformanceCount,
		ThreadAmILastThread,
		ThreadIdealProcessor,
		ThreadPriorityBoost,
		ThreadSetTlsArrayAddress,   // Obsolete
		ThreadIsIoPending,
		ThreadHideFromDebugger,
		ThreadBreakOnTermination,
		ThreadSwitchLegacyState,
		ThreadIsTerminated,
		ThreadLastSystemCall,
		ThreadIoPriority,
		ThreadCycleTime,
		ThreadPagePriority,
		ThreadActualBasePriority,
		ThreadTebInformation,
		ThreadCSwitchMon,          // Obsolete
		ThreadCSwitchPmu,
		ThreadWow64Context,
		ThreadGroupInformation,
		ThreadUmsInformation,      // UMS
		ThreadCounterProfiling,
		ThreadIdealProcessorEx,
		MaxThreadInfoClass
	} THREADINFOCLASS;

	typedef struct _THREAD_BASIC_INFORMATION
	{
		NTSTATUS ExitStatus;
		PVOID TebBaseAddress;
		CLIENT_ID ClientId;
		KAFFINITY AffinityMask;
		KPRIORITY Priority;
		KPRIORITY BasePriority;
	} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

	typedef struct _THREAD_TIMES_INFORMATION
	{
		LARGE_INTEGER CreationTime;
		LARGE_INTEGER ExitTime;
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
	} THREAD_TIMES_INFORMATION, *PTHREAD_TIMES_INFORMATION;

	typedef struct _SYSTEM_MODULE
	{
		ULONG                Reserved1;
		ULONG                Reserved2;
		PVOID                ImageBaseAddress;
		ULONG                ImageSize;
		ULONG                Flags;
		WORD                 Id;
		WORD                 Rank;
		WORD                 w018;
		WORD                 NameOffset;
		BYTE                 Name[256];
	} SYSTEM_MODULE, *PSYSTEM_MODULE;

	typedef struct _SYSTEM_MODULE_INFORMATION
	{
		ULONG                ModulesCount;
		SYSTEM_MODULE        Modules[0];
	} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

	EXTERN_C NTSYSAPI NTSTATUS NTAPI NtSetInformationProcess(
		HANDLE ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID ProcessInformation,
		ULONG ProcessInformationLength
		);

	EXTERN_C NTSYSAPI NTSTATUS NTAPI NtQueryInformationThread(
		IN HANDLE ThreadHandle,
		IN THREADINFOCLASS ThreadInformationClass,
		OUT PVOID ThreadInformation,
		IN ULONG ThreadInformationLength,
		OUT PULONG ReturnLength OPTIONAL
		);

	EXTERN_C NTSYSAPI NTSTATUS WINAPI NtQuerySystemInformation(
		_In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
		_Out_opt_  PVOID SystemInformation,
		_In_       ULONG SystemInformationLength,
		_Out_opt_  PULONG ReturnLength OPTIONAL
		);

	typedef struct _SYSTEM_REGISTRY_QUOTA_INFORMATION {
		ULONG RegistryQuotaAllowed;
		ULONG RegistryQuotaUsed;
		PVOID Reserved1;
	} SYSTEM_REGISTRY_QUOTA_INFORMATION, *PSYSTEM_REGISTRY_QUOTA_INFORMATION;

	typedef struct _SYSTEM_BASIC_INFORMATION {
		BYTE Reserved1[24];
		PVOID Reserved2[4];
		CCHAR NumberOfProcessors;
	} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

	typedef struct _SYSTEM_TIMEOFDAY_INFORMATION {
		BYTE Reserved1[48];
	} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

	typedef struct _SYSTEM_PERFORMANCE_INFORMATION {
		BYTE Reserved1[312];
	} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

	typedef struct _SYSTEM_EXCEPTION_INFORMATION {
		BYTE Reserved1[16];
	} SYSTEM_EXCEPTION_INFORMATION, *PSYSTEM_EXCEPTION_INFORMATION;

	typedef struct _SYSTEM_LOOKASIDE_INFORMATION {
		BYTE Reserved1[32];
	} SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;

	typedef struct _SYSTEM_INTERRUPT_INFORMATION {
		BYTE Reserved1[24];
	} SYSTEM_INTERRUPT_INFORMATION, *PSYSTEM_INTERRUPT_INFORMATION;

	typedef struct _SYSTEM_HANDLE_INFORMATION
	{
		ULONG	ProcessId;
		UCHAR	ObjectTypeNumber;
		UCHAR	Flags;
		USHORT	Handle;
		PVOID	Object;
		ACCESS_MASK	GrantedAccess;
	} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

	typedef struct _VM_COUNTERS {
		SIZE_T        PeakVirtualSize;
		SIZE_T        VirtualSize;
		ULONG         PageFaultCount;
		SIZE_T        PeakWorkingSetSize;
		SIZE_T        WorkingSetSize;
		SIZE_T        QuotaPeakPagedPoolUsage;
		SIZE_T        QuotaPagedPoolUsage;
		SIZE_T        QuotaPeakNonPagedPoolUsage;
		SIZE_T        QuotaNonPagedPoolUsage;
		SIZE_T        PagefileUsage;
		SIZE_T        PeakPagefileUsage;
		SIZE_T		  PrivatePageCount;
	} VM_COUNTERS;

	typedef struct _SYSTEM_THREAD_INFORMATION {
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG         WaitTime;
		PVOID         StartAddress;
		CLIENT_ID     ClientId;
		KPRIORITY     Priority;
		KPRIORITY     BasePriority;
		ULONG         ContextSwitchCount;
		LONG          State;
		LONG          WaitReason;
	} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

	typedef struct _SYSTEM_PROCESS_INFORMATION {
		ULONG             NextEntryDelta;
		ULONG             ThreadCount;
		LARGE_INTEGER     Reserved1[3];
		LARGE_INTEGER     CreateTime;
		LARGE_INTEGER     UserTime;
		LARGE_INTEGER     KernelTime;
		UNICODE_STRING    ProcessName;
		KPRIORITY         BasePriority;
		HANDLE            ProcessId;
		HANDLE            InheritedFromProcessId;
		ULONG             HandleCount;
		ULONG             SessionId;
		ULONG_PTR         PageDirectoryBase;
		VM_COUNTERS       VmCounters;
#if _WIN32_WINNT >= 0x500
		IO_COUNTERS       IoCounters;
#endif
		SYSTEM_THREAD_INFORMATION Threads[1];
	} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

	typedef struct _ACTIVATION_CONTEXT_STACK
	{
		ULONG Flags;
		ULONG NextCookieSequenceNumber;
		PVOID ActiveFrame;
		LIST_ENTRY FrameListCache;
	} ACTIVATION_CONTEXT_STACK;

	typedef struct _GDI_TEB_BATCH
	{
		ULONG Offset;
		HDC hdc;
		ULONG buffer[310];
	} GDI_TEB_BATCH;

	typedef struct _Wx86ThreadState
	{
		PULONG CallBx86Eip;
		PVOID DeallocationCpu;
		BOOLEAN UseKnownWx86Dll;
		BOOLEAN OleStubInvoked;
	} Wx86ThreadState;

	typedef struct _TEB_ACTIVE_FRAME_CONTEXT
	{
		ULONG Flags;
		char* FrameName;
	} TEB_ACTIVE_FRAME_CONTEXT;

	typedef struct _TEB_ACTIVE_FRAME
	{
		ULONG Flags;
		struct _TEB_ACTIVE_FRAME* Previous;
		TEB_ACTIVE_FRAME_CONTEXT* Context;
	} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

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


	__if_not_exists(NtCurrentTeb)
	{
		PTEB NTAPI NtCurrentTeb(void);
	}

#if _WIN32_WINNT <= 0x0500

	inline PPEB NTAPI RtlGetCurrentPeb(void)
	{
		PTEB pTeb = NtCurrentTeb();
		return pTeb->Peb;
	}

#else

	EXTERN_C NTSYSAPI PPEB NTAPI RtlGetCurrentPeb(void);
	EXTERN_C
	NTSYSAPI
		NTSTATUS
		NTAPI
		NtDuplicateObject(
			__in HANDLE SourceProcessHandle,
			__in HANDLE SourceHandle,
			__in_opt HANDLE TargetProcessHandle,
			__out_opt PHANDLE TargetHandle,
			__in ACCESS_MASK DesiredAccess,
			__in ULONG HandleAttributes,
			__in ULONG Options
			);

#endif
	//
	// This enumerated type is used as the function return value of the function
	// that is used to search the tree for a key. FoundNode indicates that the
	// function found the key. Insert as left indicates that the key was not found
	// and the node should be inserted as the left child of the parent. Insert as
	// right indicates that the key was not found and the node should be inserted
	//  as the right child of the parent.
	//
	typedef enum _TABLE_SEARCH_RESULT {
		TableEmptyTree,
		TableFoundNode,
		TableInsertAsLeft,
		TableInsertAsRight
	} TABLE_SEARCH_RESULT;

	//
	//  The results of a compare can be less than, equal, or greater than.
	//

	typedef enum _RTL_GENERIC_COMPARE_RESULTS {
		GenericLessThan,
		GenericGreaterThan,
		GenericEqual
	} RTL_GENERIC_COMPARE_RESULTS;

	//
	//  Define the Avl version of the generic table package.  Note a generic table
	//  should really be an opaque type.  We provide routines to manipulate the structure.
	//
	//  A generic table is package for inserting, deleting, and looking up elements
	//  in a table (e.g., in a symbol table).  To use this package the user
	//  defines the structure of the elements stored in the table, provides a
	//  comparison function, a memory allocation function, and a memory
	//  deallocation function.
	//
	//  Note: the user compare function must impose a complete ordering among
	//  all of the elements, and the table does not allow for duplicate entries.
	//

	//
	// Add an empty typedef so that functions can reference the
	// a pointer to the generic table struct before it is declared.
	//

	struct _RTL_AVL_TABLE;

	//
	//  The comparison function takes as input pointers to elements containing
	//  user defined structures and returns the results of comparing the two
	//  elements.
	//

	typedef
		__drv_sameIRQL
		__drv_functionClass(RTL_AVL_COMPARE_ROUTINE)
		RTL_GENERIC_COMPARE_RESULTS
		NTAPI
		RTL_AVL_COMPARE_ROUTINE(
			__in struct _RTL_AVL_TABLE *Table,
			__in PVOID FirstStruct,
			__in PVOID SecondStruct
			);
	typedef RTL_AVL_COMPARE_ROUTINE *PRTL_AVL_COMPARE_ROUTINE;

	//
	//  The allocation function is called by the generic table package whenever
	//  it needs to allocate memory for the table.
	//

	typedef
		__drv_sameIRQL
		__drv_functionClass(RTL_AVL_ALLOCATE_ROUTINE)
		__drv_allocatesMem(Mem)
		PVOID
		NTAPI
		RTL_AVL_ALLOCATE_ROUTINE(
			__in struct _RTL_AVL_TABLE *Table,
			__in LONG ByteSize
			);
	typedef RTL_AVL_ALLOCATE_ROUTINE *PRTL_AVL_ALLOCATE_ROUTINE;

	//
	//  The deallocation function is called by the generic table package whenever
	//  it needs to deallocate memory from the table that was allocated by calling
	//  the user supplied allocation function.
	//

	typedef
		__drv_sameIRQL
		__drv_functionClass(RTL_AVL_FREE_ROUTINE)
		VOID
		NTAPI
		RTL_AVL_FREE_ROUTINE(
			__in struct _RTL_AVL_TABLE *Table,
			__in __drv_freesMem(Mem) __post_invalid PVOID Buffer
			);
	typedef RTL_AVL_FREE_ROUTINE *PRTL_AVL_FREE_ROUTINE;

	//
	//  The match function takes as input the user data to be matched and a pointer
	//  to some match data, which was passed along with the function pointer.  It
	//  returns TRUE for a match and FALSE for no match.
	//
	//  RTL_AVL_MATCH_FUNCTION returns
	//      STATUS_SUCCESS if the IndexRow matches
	//      STATUS_NO_MATCH if the IndexRow does not match, but the enumeration should
	//          continue
	//      STATUS_NO_MORE_MATCHES if the IndexRow does not match, and the enumeration
	//          should terminate
	//


	typedef
		__drv_sameIRQL
		__drv_functionClass(RTL_AVL_MATCH_FUNCTION)
		NTSTATUS
		NTAPI
		RTL_AVL_MATCH_FUNCTION(
			__in struct _RTL_AVL_TABLE *Table,
			__in PVOID UserData,
			__in PVOID MatchData
			);
	typedef RTL_AVL_MATCH_FUNCTION *PRTL_AVL_MATCH_FUNCTION;

	//
	//  Define the balanced tree links and Balance field.  (No Rank field
	//  defined at this time.)
	//
	//  Callers should treat this structure as opaque!
	//
	//  The root of a balanced binary tree is not a real node in the tree
	//  but rather points to a real node which is the root.  It is always
	//  in the table below, and its fields are used as follows:
	//
	//      Parent      Pointer to self, to allow for detection of the root.
	//      LeftChild   NULL
	//      RightChild  Pointer to real root
	//      Balance     Undefined, however it is set to a convenient value
	//                  (depending on the algorithm) prior to rebalancing
	//                  in insert and delete routines.
	//

	typedef struct _RTL_BALANCED_LINKS {
		struct _RTL_BALANCED_LINKS *Parent;
		struct _RTL_BALANCED_LINKS *LeftChild;
		struct _RTL_BALANCED_LINKS *RightChild;
		CHAR Balance;
		UCHAR Reserved[3];
	} RTL_BALANCED_LINKS;
	typedef RTL_BALANCED_LINKS *PRTL_BALANCED_LINKS;

	//
	//  To use the generic table package the user declares a variable of type
	//  GENERIC_TABLE and then uses the routines described below to initialize
	//  the table and to manipulate the table.  Note that the generic table
	//  should really be an opaque type.
	//

	typedef struct _RTL_AVL_TABLE {
		RTL_BALANCED_LINKS BalancedRoot;
		PVOID OrderedPointer;
		ULONG WhichOrderedElement;
		ULONG NumberGenericTableElements;
		ULONG DepthOfTree;
		PRTL_BALANCED_LINKS RestartKey;
		ULONG DeleteCount;
		PRTL_AVL_COMPARE_ROUTINE CompareRoutine;
		PRTL_AVL_ALLOCATE_ROUTINE AllocateRoutine;
		PRTL_AVL_FREE_ROUTINE FreeRoutine;
		PVOID TableContext;
	} RTL_AVL_TABLE;
	typedef RTL_AVL_TABLE *PRTL_AVL_TABLE;

	//
	//  The procedure InitializeGenericTable takes as input an uninitialized
	//  generic table variable and pointers to the three user supplied routines.
	//  This must be called for every individual generic table variable before
	//  it can be used.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	NTSYSAPI
		VOID
		NTAPI
		RtlInitializeGenericTableAvl(
			__out PRTL_AVL_TABLE Table,
			__in PRTL_AVL_COMPARE_ROUTINE CompareRoutine,
			__in PRTL_AVL_ALLOCATE_ROUTINE AllocateRoutine,
			__in PRTL_AVL_FREE_ROUTINE FreeRoutine,
			__in_opt PVOID TableContext
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function InsertElementGenericTable will insert a new element
	//  in a table.  It does this by allocating space for the new element
	//  (this includes AVL links), inserting the element in the table, and
	//  then returning to the user a pointer to the new element.  If an element
	//  with the same key already exists in the table the return value is a pointer
	//  to the old element.  The optional output parameter NewElement is used
	//  to indicate if the element previously existed in the table.  Note: the user
	//  supplied Buffer is only used for searching the table, upon insertion its
	//  contents are copied to the newly created element.  This means that
	//  pointer to the input buffer will not point to the new element.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlInsertElementGenericTableAvl(
			__in PRTL_AVL_TABLE Table,
			__in_bcount(BufferSize) PVOID Buffer,
			__in LONG BufferSize,
			__out_opt PBOOLEAN NewElement
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function InsertElementGenericTableFull will insert a new element
	//  in a table.  It does this by allocating space for the new element
	//  (this includes AVL links), inserting the element in the table, and
	//  then returning to the user a pointer to the new element.  If an element
	//  with the same key already exists in the table the return value is a pointer
	//  to the old element.  The optional output parameter NewElement is used
	//  to indicate if the element previously existed in the table.  Note: the user
	//  supplied Buffer is only used for searching the table, upon insertion its
	//  contents are copied to the newly created element.  This means that
	//  pointer to the input buffer will not point to the new element.
	//  This routine is passed the NodeOrParent and SearchResult from a
	//  previous RtlLookupElementGenericTableFull.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlInsertElementGenericTableFullAvl(
			__in PRTL_AVL_TABLE Table,
			__in_bcount(BufferSize) PVOID Buffer,
			__in LONG BufferSize,
			__out_opt PBOOLEAN NewElement,
			__in PVOID NodeOrParent,
			__in TABLE_SEARCH_RESULT SearchResult
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function DeleteElementGenericTable will find and delete an element
	//  from a generic table.  If the element is located and deleted the return
	//  value is TRUE, otherwise if the element is not located the return value
	//  is FALSE.  The user supplied input buffer is only used as a key in
	//  locating the element in the table.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	NTSYSAPI
		BOOLEAN
		NTAPI
		RtlDeleteElementGenericTableAvl(
			__in PRTL_AVL_TABLE Table,
			__in PVOID Buffer
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function LookupElementGenericTable will find an element in a generic
	//  table.  If the element is located the return value is a pointer to
	//  the user defined structure associated with the element, otherwise if
	//  the element is not located the return value is NULL.  The user supplied
	//  input buffer is only used as a key in locating the element in the table.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlLookupElementGenericTableAvl(
			__in PRTL_AVL_TABLE Table,
			__in PVOID Buffer
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function LookupElementGenericTableFull will find an element in a generic
	//  table.  If the element is located the return value is a pointer to
	//  the user defined structure associated with the element.  If the element is not
	//  located then a pointer to the parent for the insert location is returned.  The
	//  user must look at the SearchResult value to determine which is being returned.
	//  The user can use the SearchResult and parent for a subsequent FullInsertElement
	//  call to optimize the insert.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlLookupElementGenericTableFullAvl(
			__in PRTL_AVL_TABLE Table,
			__in PVOID Buffer,
			__out PVOID *NodeOrParent,
			__out TABLE_SEARCH_RESULT *SearchResult
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function EnumerateGenericTable will return to the caller one-by-one
	//  the elements of of a table.  The return value is a pointer to the user
	//  defined structure associated with the element.  The input parameter
	//  Restart indicates if the enumeration should start from the beginning
	//  or should return the next element.  If the are no more new elements to
	//  return the return value is NULL.  As an example of its use, to enumerate
	//  all of the elements in a table the user would write:
	//
	//      for (ptr = EnumerateGenericTable(Table, TRUE);
	//           ptr != NULL;
	//           ptr = EnumerateGenericTable(Table, FALSE)) {
	//              :
	//      }
	//
	//  NOTE:   This routine does not modify the structure of the tree, but saves
	//          the last node returned in the generic table itself, and for this
	//          reason requires exclusive access to the table for the duration of
	//          the enumeration.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlEnumerateGenericTableAvl(
			__in PRTL_AVL_TABLE Table,
			__in BOOLEAN Restart
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function EnumerateGenericTableWithoutSplaying will return to the
	//  caller one-by-one the elements of of a table.  The return value is a
	//  pointer to the user defined structure associated with the element.
	//  The input parameter RestartKey indicates if the enumeration should
	//  start from the beginning or should return the next element.  If the
	//  are no more new elements to return the return value is NULL.  As an
	//  example of its use, to enumerate all of the elements in a table the
	//  user would write:
	//
	//      RestartKey = NULL;
	//      for (ptr = EnumerateGenericTableWithoutSplaying(Table, &RestartKey);
	//           ptr != NULL;
	//           ptr = EnumerateGenericTableWithoutSplaying(Table, &RestartKey)) {
	//              :
	//      }
	//
	//  If RestartKey is NULL, the package will start from the least entry in the
	//  table, otherwise it will start from the last entry returned.
	//
	//  NOTE:   This routine does not modify either the structure of the tree
	//          or the generic table itself, but must insure that no deletes
	//          occur for the duration of the enumeration, typically by having
	//          at least shared access to the table for the duration.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlEnumerateGenericTableWithoutSplayingAvl(
			__in PRTL_AVL_TABLE Table,
			__inout PVOID *RestartKey
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  RtlLookupFirstMatchingElementGenericTableAvl will return the left-most
	//  element in the tree matching the data in Buffer.  If, for example, the tree
	//  contains filenames there may exist several that differ only in case. A case-
	//  blind searcher can use this routine to position himself in the tree at the
	//  first match, and use an enumeration routine (such as RtlEnumerateGenericTableWithoutSplayingAvl
	//  to return each subsequent match.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlLookupFirstMatchingElementGenericTableAvl(
			__in PRTL_AVL_TABLE Table,
			__in PVOID Buffer,
			__out PVOID *RestartKey
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function EnumerateGenericTableLikeADirectory will return to the
	//  caller one-by-one the elements of of a table.  The return value is a
	//  pointer to the user defined structure associated with the element.
	//  The input parameter RestartKey indicates if the enumeration should
	//  start from the beginning or should return the next element.  If the
	//  are no more new elements to return the return value is NULL.  As an
	//  example of its use, to enumerate all of the elements in a table the
	//  user would write:
	//
	//      RestartKey = NULL;
	//      for (ptr = EnumerateGenericTableLikeADirectory(Table, &RestartKey, ...);
	//           ptr != NULL;
	//           ptr = EnumerateGenericTableLikeADirectory(Table, &RestartKey, ...)) {
	//              :
	//      }
	//
	//  If RestartKey is NULL, the package will start from the least entry in the
	//  table, otherwise it will start from the last entry returned.
	//
	//  NOTE:   This routine does not modify either the structure of the tree
	//          or the generic table itself.  The table must only be acquired
	//          shared for the duration of this call, and all synchronization
	//          may optionally be dropped between calls.  Enumeration is always
	//          correctly resumed in the most efficient manner possible via the
	//          IN OUT parameters provided.
	//
	//  ******  Explain NextFlag.  Directory enumeration resumes from a key
	//          requires more thought.  Also need the match pattern and IgnoreCase.
	//          Should some structure be introduced to carry it all?
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlEnumerateGenericTableLikeADirectory(
			__in PRTL_AVL_TABLE Table,
			__in_opt PRTL_AVL_MATCH_FUNCTION MatchFunction,
			__in_opt PVOID MatchData,
			__in ULONG NextFlag,
			__inout PVOID *RestartKey,
			__inout PULONG DeleteCount,
			__in PVOID Buffer
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	// The function GetElementGenericTable will return the i'th element
	// inserted in the generic table.  I = 0 implies the first element,
	// I = (RtlNumberGenericTableElements(Table)-1) will return the last element
	// inserted into the generic table.  The type of I is ULONG.  Values
	// of I > than (NumberGenericTableElements(Table)-1) will return NULL.  If
	// an arbitrary element is deleted from the generic table it will cause
	// all elements inserted after the deleted element to "move up".

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlGetElementGenericTableAvl(
			__in PRTL_AVL_TABLE Table,
			__in ULONG I
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	// The function NumberGenericTableElements returns a ULONG value
	// which is the number of generic table elements currently inserted
	// in the generic table.

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	NTSYSAPI
		ULONG
		NTAPI
		RtlNumberGenericTableElementsAvl(
			__in PRTL_AVL_TABLE Table
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  The function IsGenericTableEmpty will return to the caller TRUE if
	//  the input table is empty (i.e., does not contain any elements) and
	//  FALSE otherwise.
	//

#if (NTDDI_VERSION >= NTDDI_WINXP)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		BOOLEAN
		NTAPI
		RtlIsGenericTableEmptyAvl(
			__in PRTL_AVL_TABLE Table
			);
#endif // NTDDI_VERSION >= NTDDI_WINXP

	//
	//  As an aid to allowing existing generic table users to do (in most
	//  cases) a single-line edit to switch over to Avl table use, we
	//  have the following defines and inline routine definitions which
	//  redirect calls and types.  Note that the type override (performed
	//  by #define below) will not work in the unexpected event that someone
	//  has used a pointer or type specifier in their own #define, since
	//  #define processing is one pass and does not nest.  The __inline
	//  declarations below do not have this limitation, however.
	//
	//  To switch to using Avl tables, add the following line before your
	//  includes:
	//
	//  #define RTL_USE_AVL_TABLES 0
	//

#ifdef RTL_USE_AVL_TABLES

#undef PRTL_GENERIC_COMPARE_ROUTINE
#undef RTL_GENERIC_COMPARE_ROUTINE
#undef PRTL_GENERIC_ALLOCATE_ROUTINE
#undef RTL_GENERIC_ALLOCATE_ROUTINE
#undef PRTL_GENERIC_FREE_ROUTINE
#undef RTL_GENERIC_FREE_ROUTINE
#undef RTL_GENERIC_TABLE
#undef PRTL_GENERIC_TABLE

#define PRTL_GENERIC_COMPARE_ROUTINE PRTL_AVL_COMPARE_ROUTINE
#define RTL_GENERIC_COMPARE_ROUTINE RTL_AVL_COMPARE_ROUTINE
#define PRTL_GENERIC_ALLOCATE_ROUTINE PRTL_AVL_ALLOCATE_ROUTINE
#define RTL_GENERIC_ALLOCATE_ROUTINE RTL_AVL_ALLOCATE_ROUTINE
#define PRTL_GENERIC_FREE_ROUTINE PRTL_AVL_FREE_ROUTINE
#define RTL_GENERIC_FREE_ROUTINE RTL_AVL_FREE_ROUTINE
#define RTL_GENERIC_TABLE RTL_AVL_TABLE
#define PRTL_GENERIC_TABLE PRTL_AVL_TABLE

#define RtlInitializeGenericTable               RtlInitializeGenericTableAvl
#define RtlInsertElementGenericTable            RtlInsertElementGenericTableAvl
#define RtlInsertElementGenericTableFull        RtlInsertElementGenericTableFullAvl
#define RtlDeleteElementGenericTable            RtlDeleteElementGenericTableAvl
#define RtlLookupElementGenericTable            RtlLookupElementGenericTableAvl
#define RtlLookupElementGenericTableFull        RtlLookupElementGenericTableFullAvl
#define RtlEnumerateGenericTable                RtlEnumerateGenericTableAvl
#define RtlEnumerateGenericTableWithoutSplaying RtlEnumerateGenericTableWithoutSplayingAvl
#define RtlGetElementGenericTable               RtlGetElementGenericTableAvl
#define RtlNumberGenericTableElements           RtlNumberGenericTableElementsAvl
#define RtlIsGenericTableEmpty                  RtlIsGenericTableEmptyAvl

#endif // RTL_USE_AVL_TABLES

	//
	//  Define the generic table package.  Note a generic table should really
	//  be an opaque type.  We provide routines to manipulate the structure.
	//
	//  A generic table is package for inserting, deleting, and looking up elements
	//  in a table (e.g., in a symbol table).  To use this package the user
	//  defines the structure of the elements stored in the table, provides a
	//  comparison function, a memory allocation function, and a memory
	//  deallocation function.
	//
	//  Note: the user compare function must impose a complete ordering among
	//  all of the elements, and the table does not allow for duplicate entries.
	//

	//
	//  Do not do the following defines if using Avl
	//

#ifndef RTL_USE_AVL_TABLES

	struct _RTL_GENERIC_TABLE;

	//
	//  The comparison function takes as input pointers to elements containing
	//  user defined structures and returns the results of comparing the two
	//  elements.
	//

	typedef
		RTL_GENERIC_COMPARE_RESULTS
		NTAPI
		RTL_GENERIC_COMPARE_ROUTINE(
			__in struct _RTL_GENERIC_TABLE *Table,
			__in PVOID FirstStruct,
			__in PVOID SecondStruct
			);
	typedef RTL_GENERIC_COMPARE_ROUTINE *PRTL_GENERIC_COMPARE_ROUTINE;

	//
	//  The allocation function is called by the generic table package whenever
	//  it needs to allocate memory for the table.
	//

	typedef
		PVOID
		NTAPI
		RTL_GENERIC_ALLOCATE_ROUTINE(
			__in struct _RTL_GENERIC_TABLE *Table,
			__in LONG ByteSize
			);
	typedef RTL_GENERIC_ALLOCATE_ROUTINE *PRTL_GENERIC_ALLOCATE_ROUTINE;

	//
	//  The deallocation function is called by the generic table package whenever
	//  it needs to deallocate memory from the table that was allocated by calling
	//  the user supplied allocation function.
	//

	typedef
		VOID
		NTAPI
		RTL_GENERIC_FREE_ROUTINE(
			__in struct _RTL_GENERIC_TABLE *Table,
			__in PVOID Buffer
			);
	typedef RTL_GENERIC_FREE_ROUTINE *PRTL_GENERIC_FREE_ROUTINE;

	//
	//  To use the generic table package the user declares a variable of type
	//  GENERIC_TABLE and then uses the routines described below to initialize
	//  the table and to manipulate the table.  Note that the generic table
	//  should really be an opaque type.
	//

	typedef struct _RTL_SPLAY_LINKS {
		struct _RTL_SPLAY_LINKS *Parent;
		struct _RTL_SPLAY_LINKS *LeftChild;
		struct _RTL_SPLAY_LINKS *RightChild;
	} RTL_SPLAY_LINKS;
	typedef RTL_SPLAY_LINKS *PRTL_SPLAY_LINKS;

	typedef struct _RTL_GENERIC_TABLE {
		PRTL_SPLAY_LINKS TableRoot;
		LIST_ENTRY InsertOrderList;
		PLIST_ENTRY OrderedPointer;
		ULONG WhichOrderedElement;
		ULONG NumberGenericTableElements;
		PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine;
		PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine;
		PRTL_GENERIC_FREE_ROUTINE FreeRoutine;
		PVOID TableContext;
	} RTL_GENERIC_TABLE;
	typedef RTL_GENERIC_TABLE *PRTL_GENERIC_TABLE;

	EXTERN_C NTSYSAPI
		VOID
		NTAPI
		RtlInitializeGenericTable(
			__out PRTL_GENERIC_TABLE Table,
			__in PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine,
			__in PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine,
			__in PRTL_GENERIC_FREE_ROUTINE FreeRoutine,
			__in_opt PVOID TableContext
			);

	//
	//  The function InsertElementGenericTable will insert a new element
	//  in a table.  It does this by allocating space for the new element
	//  (this includes splay links), inserting the element in the table, and
	//  then returning to the user a pointer to the new element.  If an element
	//  with the same key already exists in the table the return value is a pointer
	//  to the old element.  The optional output parameter NewElement is used
	//  to indicate if the element previously existed in the table.  Note: the user
	//  supplied Buffer is only used for searching the table, upon insertion its
	//  contents are copied to the newly created element.  This means that
	//  pointer to the input buffer will not point to the new element.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlInsertElementGenericTable(
			__in PRTL_GENERIC_TABLE Table,
			__in_bcount(BufferSize) PVOID Buffer,
			__in LONG BufferSize,
			__out_opt PBOOLEAN NewElement
			);
#endif

	//
	//  The function InsertElementGenericTableFull will insert a new element
	//  in a table.  It does this by allocating space for the new element
	//  (this includes splay links), inserting the element in the table, and
	//  then returning to the user a pointer to the new element.  If an element
	//  with the same key already exists in the table the return value is a pointer
	//  to the old element.  The optional output parameter NewElement is used
	//  to indicate if the element previously existed in the table.  Note: the user
	//  supplied Buffer is only used for searching the table, upon insertion its
	//  contents are copied to the newly created element.  This means that
	//  pointer to the input buffer will not point to the new element.
	//  This routine is passed the NodeOrParent and SearchResult from a
	//  previous RtlLookupElementGenericTableFull.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlInsertElementGenericTableFull(
			__in PRTL_GENERIC_TABLE Table,
			__in_bcount(BufferSize) PVOID Buffer,
			__in LONG BufferSize,
			__out_opt PBOOLEAN NewElement,
			__in PVOID NodeOrParent,
			__in TABLE_SEARCH_RESULT SearchResult
			);
#endif

	//
	//  The function DeleteElementGenericTable will find and delete an element
	//  from a generic table.  If the element is located and deleted the return
	//  value is TRUE, otherwise if the element is not located the return value
	//  is FALSE.  The user supplied input buffer is only used as a key in
	//  locating the element in the table.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	NTSYSAPI
		BOOLEAN
		NTAPI
		RtlDeleteElementGenericTable(
			__in PRTL_GENERIC_TABLE Table,
			__in PVOID Buffer
			);
#endif

	//
	//  The function LookupElementGenericTable will find an element in a generic
	//  table.  If the element is located the return value is a pointer to
	//  the user defined structure associated with the element, otherwise if
	//  the element is not located the return value is NULL.  The user supplied
	//  input buffer is only used as a key in locating the element in the table.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlLookupElementGenericTable(
			__in PRTL_GENERIC_TABLE Table,
			__in PVOID Buffer
			);
#endif

	//
	//  The function LookupElementGenericTableFull will find an element in a generic
	//  table.  If the element is located the return value is a pointer to
	//  the user defined structure associated with the element.  If the element is not
	//  located then a pointer to the parent for the insert location is returned.  The
	//  user must look at the SearchResult value to determine which is being returned.
	//  The user can use the SearchResult and parent for a subsequent FullInsertElement
	//  call to optimize the insert.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	NTSYSAPI
		PVOID
		NTAPI
		RtlLookupElementGenericTableFull(
			__in PRTL_GENERIC_TABLE Table,
			__in PVOID Buffer,
			__out PVOID *NodeOrParent,
			__out TABLE_SEARCH_RESULT *SearchResult
			);
#endif

	//
	//  The function EnumerateGenericTable will return to the caller one-by-one
	//  the elements of of a table.  The return value is a pointer to the user
	//  defined structure associated with the element.  The input parameter
	//  Restart indicates if the enumeration should start from the beginning
	//  or should return the next element.  If the are no more new elements to
	//  return the return value is NULL.  As an example of its use, to enumerate
	//  all of the elements in a table the user would write:
	//
	//      for (ptr = EnumerateGenericTable(Table, TRUE);
	//           ptr != NULL;
	//           ptr = EnumerateGenericTable(Table, FALSE)) {
	//              :
	//      }
	//
	//
	//  PLEASE NOTE:
	//
	//      If you enumerate a GenericTable using RtlEnumerateGenericTable, you
	//      will flatten the table, turning it into a sorted linked list.
	//      To enumerate the table without perturbing the splay links, use
	//      RtlEnumerateGenericTableWithoutSplaying

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlEnumerateGenericTable(
			__in PRTL_GENERIC_TABLE Table,
			__in BOOLEAN Restart
			);
#endif

	//
	//  The function EnumerateGenericTableWithoutSplaying will return to the
	//  caller one-by-one the elements of of a table.  The return value is a
	//  pointer to the user defined structure associated with the element.
	//  The input parameter RestartKey indicates if the enumeration should
	//  start from the beginning or should return the next element.  If the
	//  are no more new elements to return the return value is NULL.  As an
	//  example of its use, to enumerate all of the elements in a table the
	//  user would write:
	//
	//      RestartKey = NULL;
	//      for (ptr = EnumerateGenericTableWithoutSplaying(Table, &RestartKey);
	//           ptr != NULL;
	//           ptr = EnumerateGenericTableWithoutSplaying(Table, &RestartKey)) {
	//              :
	//      }
	//
	//  If RestartKey is NULL, the package will start from the least entry in the
	//  table, otherwise it will start from the last entry returned.
	//
	//
	//  Note that unlike RtlEnumerateGenericTable, this routine will NOT perturb
	//  the splay order of the tree.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlEnumerateGenericTableWithoutSplaying(
			__in PRTL_GENERIC_TABLE Table,
			__inout PVOID *RestartKey
			);
#endif

	//
	// The function GetElementGenericTable will return the i'th element
	// inserted in the generic table.  I = 0 implies the first element,
	// I = (RtlNumberGenericTableElements(Table)-1) will return the last element
	// inserted into the generic table.  The type of I is ULONG.  Values
	// of I > than (NumberGenericTableElements(Table)-1) will return NULL.  If
	// an arbitrary element is deleted from the generic table it will cause
	// all elements inserted after the deleted element to "move up".

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		PVOID
		NTAPI
		RtlGetElementGenericTable(
			__in PRTL_GENERIC_TABLE Table,
			__in ULONG I
			);
#endif

	//
	// The function NumberGenericTableElements returns a ULONG value
	// which is the number of generic table elements currently inserted
	// in the generic table.

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	NTSYSAPI
		ULONG
		NTAPI
		RtlNumberGenericTableElements(
			__in PRTL_GENERIC_TABLE Table
			);
#endif

	//
	//  The function IsGenericTableEmpty will return to the caller TRUE if
	//  the input table is empty (i.e., does not contain any elements) and
	//  FALSE otherwise.
	//

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__checkReturn
		NTSYSAPI
		BOOLEAN
		NTAPI
		RtlIsGenericTableEmpty(
			__in PRTL_GENERIC_TABLE Table
			);
#endif

#endif // RTL_USE_AVL_TREES

	//
	// Notify filter values
	//


	//
	// Key query structures
	//

	typedef struct _KEY_BASIC_INFORMATION {
		LARGE_INTEGER LastWriteTime;
		ULONG   TitleIndex;
		ULONG   NameLength;
		WCHAR   Name[1];            // Variable length string
	} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

	typedef struct _KEY_NODE_INFORMATION {
		LARGE_INTEGER LastWriteTime;
		ULONG   TitleIndex;
		ULONG   ClassOffset;
		ULONG   ClassLength;
		ULONG   NameLength;
		WCHAR   Name[1];            // Variable length string
									//          Class[1];           // Variable length string not declared
	} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

	typedef struct _KEY_FULL_INFORMATION {
		LARGE_INTEGER LastWriteTime;
		ULONG   TitleIndex;
		ULONG   ClassOffset;
		ULONG   ClassLength;
		ULONG   SubKeys;
		ULONG   MaxNameLen;
		ULONG   MaxClassLen;
		ULONG   Values;
		ULONG   MaxValueNameLen;
		ULONG   MaxValueDataLen;
		WCHAR   Class[1];           // Variable length
	} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;


	typedef struct _KEY_WRITE_TIME_INFORMATION {
		LARGE_INTEGER LastWriteTime;
	} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

	typedef struct _KEY_WOW64_FLAGS_INFORMATION {
		ULONG   UserFlags;
	} KEY_WOW64_FLAGS_INFORMATION, *PKEY_WOW64_FLAGS_INFORMATION;

	typedef struct _KEY_HANDLE_TAGS_INFORMATION {
		ULONG   HandleTags;
	} KEY_HANDLE_TAGS_INFORMATION, *PKEY_HANDLE_TAGS_INFORMATION;

	typedef struct _KEY_CONTROL_FLAGS_INFORMATION {
		ULONG   ControlFlags;
	} KEY_CONTROL_FLAGS_INFORMATION, *PKEY_CONTROL_FLAGS_INFORMATION;


	//
	// Value entry query structures
	//

	typedef struct _KEY_VALUE_BASIC_INFORMATION {
		ULONG   TitleIndex;
		ULONG   Type;
		ULONG   NameLength;
		WCHAR   Name[1];            // Variable size
	} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

	typedef struct _KEY_VALUE_FULL_INFORMATION {
		ULONG   TitleIndex;
		ULONG   Type;
		ULONG   DataOffset;
		ULONG   DataLength;
		ULONG   NameLength;
		WCHAR   Name[1];            // Variable size
									//          Data[1];            // Variable size data not declared
	} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

	typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
		ULONG   TitleIndex;
		ULONG   Type;
		ULONG   DataLength;
		UCHAR   Data[1];            // Variable size
	} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

	typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
		ULONG   Type;
		ULONG   DataLength;
		UCHAR   Data[1];            // Variable size
	} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64;

	typedef struct _KEY_VALUE_ENTRY {
		PUNICODE_STRING ValueName;
		ULONG           DataLength;
		ULONG           DataOffset;
		ULONG           Type;
	} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

	typedef enum _KEY_VALUE_INFORMATION_CLASS {
		KeyValueBasicInformation,
		KeyValueFullInformation,
		KeyValuePartialInformation,
		KeyValueFullInformationAlign64,
		KeyValuePartialInformationAlign64,
		MaxKeyValueInfoClass  // MaxKeyValueInfoClass should always be the last enum
	} KEY_VALUE_INFORMATION_CLASS;

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtCreateKey(
		_Out_       PHANDLE KeyHandle,
		_In_        ACCESS_MASK DesiredAccess,
		_In_        POBJECT_ATTRIBUTES ObjectAttributes,
		_Reserved_  ULONG TitleIndex,
		_In_opt_    PUNICODE_STRING Class,
		_In_        ULONG CreateOptions,
		_Out_opt_   PULONG Disposition
		);

#endif

#if (NTDDI_VERSION >= NTDDI_VISTA)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NtCreateKeyTransacted(
			__out PHANDLE KeyHandle,
			__in ACCESS_MASK DesiredAccess,
			__in POBJECT_ATTRIBUTES ObjectAttributes,
			__reserved ULONG TitleIndex,
			__in_opt PUNICODE_STRING Class,
			__in ULONG CreateOptions,
			__in HANDLE TransactionHandle,
			__out_opt PULONG Disposition
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtOpenKey(
			__out PHANDLE KeyHandle,
			__in ACCESS_MASK DesiredAccess,
			__in POBJECT_ATTRIBUTES ObjectAttributes
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtOpenKeyEx(
			__out PHANDLE KeyHandle,
			__in ACCESS_MASK DesiredAccess,
			__in POBJECT_ATTRIBUTES ObjectAttributes,
			__in ULONG OpenOptions
			);
#endif



#if (NTDDI_VERSION >= NTDDI_VISTA)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtOpenKeyTransacted(
			__out PHANDLE KeyHandle,
			__in ACCESS_MASK DesiredAccess,
			__in POBJECT_ATTRIBUTES ObjectAttributes,
			__in HANDLE TransactionHandle
			);
#endif


#if (NTDDI_VERSION >= NTDDI_WIN7)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtOpenKeyTransactedEx(
			__out PHANDLE KeyHandle,
			__in ACCESS_MASK DesiredAccess,
			__in POBJECT_ATTRIBUTES ObjectAttributes,
			__in ULONG OpenOptions,
			__in HANDLE TransactionHandle
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtDeleteKey(
			__in HANDLE KeyHandle
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtDeleteValueKey(
			__in HANDLE KeyHandle,
			__in PUNICODE_STRING ValueName
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		__drv_when(Length == 0, __drv_valueIs(<0))
		__drv_when(Length>0, __drv_valueIs(<0; == 0))
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtEnumerateKey(
			__in HANDLE KeyHandle,
			__in ULONG Index,
			__in KEY_INFORMATION_CLASS KeyInformationClass,
			__out_bcount_opt(Length) PVOID KeyInformation,
			__in ULONG Length,
			__out PULONG ResultLength
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		__drv_when(Length == 0, __drv_valueIs(<0))
		__drv_when(Length>0, __drv_valueIs(<0; == 0))
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtEnumerateValueKey(
			__in HANDLE KeyHandle,
			__in ULONG Index,
			__in KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			__out_bcount_opt(Length) PVOID KeyValueInformation,
			__in ULONG Length,
			__out PULONG ResultLength
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtFlushKey(
			__in HANDLE KeyHandle
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtNotifyChangeMultipleKeys(
			__in HANDLE MasterKeyHandle,
			__in_opt ULONG Count,
			__in_ecount_opt(Count) OBJECT_ATTRIBUTES SubordinateObjects[],
			__in_opt HANDLE Event,
			__in_opt PIO_APC_ROUTINE ApcRoutine,
			__in_opt PVOID ApcContext,
			__out PIO_STATUS_BLOCK IoStatusBlock,
			__in ULONG CompletionFilter,
			__in BOOLEAN WatchTree,
			__out_bcount_opt(BufferSize) PVOID Buffer,
			__in ULONG BufferSize,
			__in BOOLEAN Asynchronous
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueryMultipleValueKey(
			__in HANDLE KeyHandle,
			__inout_ecount(EntryCount) PKEY_VALUE_ENTRY ValueEntries,
			__in ULONG EntryCount,
			__out_bcount(*BufferLength) PVOID ValueBuffer,
			__inout PULONG BufferLength,
			__out_opt PULONG RequiredBufferLength
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		__drv_when(Length == 0, __drv_valueIs(<0))
		__drv_when(Length>0, __drv_valueIs(<0; == 0))
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueryKey(
			__in HANDLE KeyHandle,
			__in KEY_INFORMATION_CLASS KeyInformationClass,
			__out_bcount_opt(Length) PVOID KeyInformation,
			__in ULONG Length,
			__out PULONG ResultLength
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		__drv_when(Length == 0, __drv_valueIs(<0))
		__drv_when(Length>0, __drv_valueIs(<0; == 0))
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtQueryValueKey(
			__in HANDLE KeyHandle,
			__in PUNICODE_STRING ValueName,
			__in KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
			__out_bcount_opt(Length) PVOID KeyValueInformation,
			__in ULONG Length,
			__out PULONG ResultLength
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtRenameKey(
			__in HANDLE           KeyHandle,
			__in PUNICODE_STRING  NewName
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtSetInformationKey(
			__in HANDLE KeyHandle,
			__in KEY_SET_INFORMATION_CLASS KeySetInformationClass,
			__in_bcount(KeySetInformationLength) PVOID KeySetInformation,
			__in ULONG KeySetInformationLength
			);
#endif

#if (NTDDI_VERSION >= NTDDI_WIN2K)
	EXTERN_C
	__drv_maxIRQL(PASSIVE_LEVEL)
		NTSYSAPI
		NTSTATUS
		NTAPI
		NtSetValueKey(
			__in HANDLE KeyHandle,
			__in PUNICODE_STRING ValueName,
			__in_opt ULONG TitleIndex,
			__in ULONG Type,
			__in_bcount_opt(DataSize) PVOID Data,
			__in ULONG DataSize
			);
#endif

#define HANDLE_DETACHED_PROCESS    (HANDLE)-1
#define HANDLE_CREATE_NEW_CONSOLE  (HANDLE)-2
#define HANDLE_CREATE_NO_WINDOW    (HANDLE)-4

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#define OBJ_VALID_ATTRIBUTES    0x000007F2L

#define LDR_IMAGE_IS_DLL                0x00000004
#define LDR_LOAD_IN_PROGRESS            0x00001000
#define LDR_UNLOAD_IN_PROGRESS          0x00002000
#define LDR_NO_DLL_CALLS                0x00040000
#define LDR_PROCESS_ATTACHED            0x00080000
#define LDR_MODULE_REBASED              0x00200000

	EXTERN_C NTSYSAPI NTSTATUS
		NTAPI
		RtlCreateUserThread(
			_In_ HANDLE processHandle,
			_In_ SECURITY_DESCRIPTOR* securityDescriptor,
			_In_ BOOLEAN createSuspended,
			_In_ ULONG stackZeroBits,
			_Inout_opt_ size_t* stackReserved,
			_Inout_opt_ size_t* stackCommit,
			_In_ const void* startAddress,
			_In_ void* startParameter,
			_Inout_ HANDLE* threadHandle,
			_Inout_opt_ CLIENT_ID* clientID
			);

	EXTERN_C NTSYSAPI VOID
		NTAPI
		RtlExitUserThread(
			_In_ NTSTATUS Status
			);

	EXTERN_C NTSYSAPI DWORD NTAPI RtlComputeCrc32
		(
			DWORD       dwInitial,
			const BYTE* pData,
			INT         iLen
			);

	EXTERN_C NTSYSAPI NTSTATUS NTAPI NtUnloadKey(IN POBJECT_ATTRIBUTES   DestinationKeyName);

	EXTERN_C NTSYSAPI NTSTATUS NTAPI NtUnloadKey2(IN POBJECT_ATTRIBUTES KeyObjectAttributes,IN BOOLEAN ForceUnload);

	EXTERN_C NTSYSAPI NTSTATUS NTAPI NtUnloadKeyEx( IN POBJECT_ATTRIBUTES KeyObjectAttributes,  IN HANDLE EventHandle OPTIONAL);

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	// begin_rev
#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED 0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH 0x00000002 // ?
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010 // ?
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET 0x00000020 // ?
#define THREAD_CREATE_FLAGS_INITIAL_THREAD 0x00000080

	typedef struct _PS_ATTRIBUTE
	{
		ULONG Attribute;
		SIZE_T Size;
		union
		{
			ULONG Value;
			PVOID ValuePtr;
		};
		PSIZE_T ReturnLength;
	} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

	typedef struct _PS_ATTRIBUTE_LIST
	{
		SIZE_T TotalLength;
		PS_ATTRIBUTE Attributes[1];
	} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;


	EXTERN_C NTSYSCALLAPI NTSTATUS NTAPI NtCreateThreadEx(
		_Out_ PHANDLE ThreadHandle,
		_In_ ACCESS_MASK DesiredAccess,
		_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
		_In_ HANDLE ProcessHandle,
		_In_ PVOID StartRoutine, // PUSER_THREAD_START_ROUTINE
		_In_opt_ PVOID Argument,
		_In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
		_In_ SIZE_T ZeroBits,
		_In_ SIZE_T StackSize,
		_In_ SIZE_T MaximumStackSize,
		_In_opt_ PPS_ATTRIBUTE_LIST AttributeList
		);
#endif

	//UEFI支持相关函数

	EXTERN_C NTSYSAPI NTSTATUS NTAPI ZwQueryBootEntryOrder(
			OUT PULONG Ids,
			IN OUT PULONG Count
		);

	// Firmware Boot Options
	typedef struct _BOOT_OPTIONS
	{
		ULONG Version;
		ULONG Length;
		ULONG Timeout;
		ULONG CurrentBootEntryId;
		ULONG NextBootEntryId;
		WCHAR HeadlessRedirection[1];
	} BOOT_OPTIONS, *PBOOT_OPTIONS;


	typedef struct _BOOT_ENTRY
	{
		//0
		ULONG NextBootEntry;
		//4


	}BOOT_ENTRY, *PBOOT_ENTRY;

	EXTERN_C NTSYSAPI NTSTATUS NTAPI ZwQueryBootOptions(
			OUT PBOOT_OPTIONS BootOptions,
			IN OUT PULONG BootOptionsLength
		);


	EXTERN_C NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetBootOptions(
			IN PBOOT_OPTIONS BootOptions,
			IN ULONG FieldsToChange
		);

	EXTERN_C NTSYSAPI
		NTSTATUS
		NTAPI
		ZwEnumerateBootEntries(
			__out_bcount_opt(*BufferLength) PVOID Buffer,
			__inout PULONG BufferLength
		);

	EXTERN_C NTSYSAPI
		NTSTATUS
		NTAPI
		ZwAddBootEntry(
			__in PBOOT_ENTRY BootEntry,
			__out_opt PULONG Id
		);


	EXTERN_C NTSYSAPI
		NTSTATUS
		NTAPI
		ZwModifyBootEntry(
			__in PBOOT_ENTRY BootEntry
		);

	EXTERN_C NTSYSAPI
		NTSTATUS
		NTAPI
		ZwSetBootEntryOrder(
			__in_ecount(Count) PULONG Ids,
			__in ULONG Count
		);


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwSetInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );
#endif

#if (NTDDI_VERSION >= NTDDI_WIN7)
EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwOpenKeyEx(
    _Out_ PHANDLE KeyHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ ULONG OpenOptions
    );
#endif


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
_When_(Length == 0, _Post_satisfies_(return < 0))
_When_(Length > 0, _Post_satisfies_(return <= 0))
_Success_(return == STATUS_SUCCESS)
_On_failure_(_When_(return == STATUS_BUFFER_OVERFLOW || return == STATUS_BUFFER_TOO_SMALL, _Post_satisfies_(*ResultLength > Length)))
NTSYSAPI
NTSTATUS
NTAPI
ZwQueryValueKey(
    _In_ HANDLE KeyHandle,
    _In_ PUNICODE_STRING ValueName,
    _In_ KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    _Out_writes_bytes_to_opt_(Length, *ResultLength) PVOID KeyValueInformation,
    _In_ ULONG Length,
    _Out_ PULONG ResultLength
    );
#endif


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwSetValueKey(
    _In_ HANDLE KeyHandle,
    _In_ PUNICODE_STRING ValueName,
    _In_opt_ ULONG TitleIndex,
    _In_ ULONG Type,
    _In_reads_bytes_opt_(DataSize) PVOID Data,
    _In_ ULONG DataSize
    );
#endif


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationFile(
    _In_ HANDLE FileHandle,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _Out_writes_bytes_(Length) PVOID FileInformation,
    _In_ ULONG Length,
    _In_ FILE_INFORMATION_CLASS FileInformationClass
    );
#endif


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
NTSYSAPI
_Must_inspect_result_
_Ret_maybenull_
_Post_writable_byte_size_(Size)
PVOID
NTAPI
RtlAllocateHeap(
    _In_ PVOID HeapHandle,
    _In_opt_ ULONG Flags,
    _In_ SIZE_T Size
    );
#endif


#if (NTDDI_VERSION >= NTDDI_WIN8)
EXTERN_C
_Success_(return != 0)
NTSYSAPI
LOGICAL
NTAPI
RtlFreeHeap(
    _In_ PVOID HeapHandle,
    _In_opt_ ULONG Flags,
    _Frees_ptr_opt_ PVOID BaseAddress
    );
#else
#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
_Success_(return != 0)
NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHeap(
    _In_ PVOID HeapHandle,
    _In_opt_ ULONG Flags,
    _Frees_ptr_opt_ PVOID BaseAddress
    );
#endif // NTDDI_VERSION >= NTDDI_WIN2K
#endif // NTDDI_VERSION >= NTDDI_WIN8

#if (NTDDI_VERSION >= NTDDI_WINXP)
EXTERN_C
_Must_inspect_result_
NTSYSAPI
PVOID
NTAPI
RtlCreateHeap(
    _In_     ULONG Flags,
    _In_opt_ PVOID HeapBase,
    _In_opt_ SIZE_T ReserveSize,
    _In_opt_ SIZE_T CommitSize,
    _In_opt_ PVOID Lock,
    _In_opt_ PRTL_HEAP_PARAMETERS Parameters
    );
#endif // NTDDI_VERSION >= NTDDI_WINXP

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtLoadKey(
	IN POBJECT_ATTRIBUTES   DestinationKeyName,
	IN POBJECT_ATTRIBUTES   HiveFileName);

EXTERN_C NTSYSAPI NTSTATUS NTAPI ZwLoadKey(
	IN POBJECT_ATTRIBUTES   DestinationKeyName,
	IN POBJECT_ATTRIBUTES   HiveFileName);

EXTERN_C NTSYSAPI NTSTATUS NTAPI	ZwUnloadKey(IN POBJECT_ATTRIBUTES   DestinationKeyName);

EXTERN_C NTSYSAPI NTSTATUS NTAPI	NtUnloadKey(IN POBJECT_ATTRIBUTES   DestinationKeyName);


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwWriteFile(
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_reads_bytes_(Length) PVOID Buffer,
    _In_ ULONG Length,
    _In_opt_ PLARGE_INTEGER ByteOffset,
    _In_opt_ PULONG Key
    );
#endif

EXTERN_C
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSYSAPI
NTSTATUS
NTAPI
ZwTerminateProcess(
    _In_opt_ HANDLE ProcessHandle,
    _In_ NTSTATUS ExitStatus
    );


#if (NTDDI_VERSION >= NTDDI_WIN2K)
EXTERN_C
__kernel_entry NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeviceIoControlFile (
    _In_ HANDLE FileHandle,
    _In_opt_ HANDLE Event,
    _In_opt_ PIO_APC_ROUTINE ApcRoutine,
    _In_opt_ PVOID ApcContext,
    _Out_ PIO_STATUS_BLOCK IoStatusBlock,
    _In_ ULONG IoControlCode,
    _In_reads_bytes_opt_(InputBufferLength) PVOID InputBuffer,
    _In_ ULONG InputBufferLength,
    _Out_writes_bytes_opt_(OutputBufferLength) PVOID OutputBuffer,
    _In_ ULONG OutputBufferLength
    );
#endif


typedef struct
{
	ULONG i[2];
	ULONG buf[4];
	unsigned char in[64];
	unsigned char digest[16];
} MD5_CTX;


EXTERN_C DECLSPEC_IMPORT void __stdcall MD5Init(MD5_CTX* context);

EXTERN_C DECLSPEC_IMPORT void __stdcall MD5Update(MD5_CTX* context,
	unsigned char* input,
	unsigned int inlen);

EXTERN_C DECLSPEC_IMPORT void __stdcall MD5Final(MD5_CTX* context);


EXTERN_C
DECLSPEC_IMPORT
NTSTATUS
NTAPI
RtlWow64EnableFsRedirectionEx(
	_In_opt_ PVOID Wow64FsEnableRedirection,
	_Out_    PVOID* OldFsRedirectionLevel
	);

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtCreateKeyedEvent(
	OUT PHANDLE handle,
	IN ACCESS_MASK access,
	IN POBJECT_ATTRIBUTES attr,
	IN ULONG flags
	);

EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtOpenKeyedEvent(
	OUT PHANDLE             KeyedEventHandle,
	IN ACCESS_MASK          DesiredAccess,
	IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL
	);


EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtWaitForKeyedEvent(
	IN HANDLE               KeyedEventHandle,
	IN PVOID                Key,
	IN BOOLEAN              Alertable,
	IN PLARGE_INTEGER       Timeout OPTIONAL
	);


EXTERN_C
NTSYSAPI
NTSTATUS
NTAPI
NtReleaseKeyedEvent(
	IN HANDLE               KeyedEventHandle,
	IN PVOID                Key,
	IN BOOLEAN              Alertable,
	IN PLARGE_INTEGER       Timeout OPTIONAL
	);

//指示进程是否正在进行退出。
EXTERN_C
NTSYSAPI
BOOLEAN
NTAPI
RtlDllShutdownInProgress();


EXTERN_C
NTSTATUS
NTAPI
LdrAddRefDll(
	IN ULONG  Flags,
	IN PVOID  BaseAddress
	);

#if defined __cplusplus && !defined _Disallow_YY_KM_Namespace
} //namespace YY
#endif

#endif