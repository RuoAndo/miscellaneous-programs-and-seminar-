/*************************************************************************
    Pool Tags
*************************************************************************/

#define BUFFER_SWAP_TAG     'bdBS'
#define CONTEXT_TAG         'xcBS'
#define NAME_TAG            'mnBS'
#define PRE_2_POST_TAG      'ppBS'

/*************************************************************************
    Local structures
*************************************************************************/

//
//  This is a volume context, one of these are attached to each volume
//  we monitor.  This is used to get a "DOS" name for debug display.
//

typedef struct _VOLUME_CONTEXT {

    //
    //  Holds the name to display
    //

    UNICODE_STRING Name;

    //
    //  Holds the sector size for this volume.
    //

    ULONG SectorSize;

} VOLUME_CONTEXT, *PVOLUME_CONTEXT;

/////////////////////////////////////////////////////////////////////////
//		Definitions
/////////////////////////////////////////////////////////////////////////
#define MIN_SECTOR_SIZE 	0x200
#define NT_PROCNAMELEN		260 * 2	// マルチバイトを考慮して、拡大しておく
#define	READ_DATA_SIZE		1024

// ファイルアクセス許可禁止
#define	DISABLE_WRITE	0x01
#define DISABLE_READ	0x02

// ファイルアクセス
#define	WRITE_OPERATION 0x01
#define	READ_OPERATION	0x02

// 時間情報を保持するサイズ
#define TIME_SIZE	24
//
//  This is a context structure that is used to pass state from our
//  pre-operation callback to our post-operation callback.
//

typedef struct _PRE_2_POST_CONTEXT {

    //
    //  Pointer to our volume context structure.  We always get the context
    //  in the preOperation path because you can not safely get it at DPC
    //  level.  We then release it in the postOperation path.  It is safe
    //  to release contexts at DPC level.
    //

    PVOLUME_CONTEXT VolCtx;

    //
    //  Since the post-operation parameters always receive the "original"
    //  parameters passed to the operation, we need to pass our new destination
    //  buffer to our post operation routine so we can free it.
    //

    PVOID SwappedBuffer;

} PRE_2_POST_CONTEXT, *PPRE_2_POST_CONTEXT;

//
// ファイルアクセス毎の情報を保持する構造体
//

typedef struct _FILE_ACCESS_DATA {
	
	BOOLEAN			IsDirectory;
	UNICODE_STRING	FullPath;
	ULONG			AccessRecord;
	ULONG			AccessAttributes;
	ULONG			ProcessID;
	PWCHAR			ProcessName;
	ULONG			ProcessAttributes;
} FILE_ACCESS_DATA, *PFILE_ACCESS_DATA;


//
// ゼロデイ攻撃対象の情報を保持する構造体
//

typedef struct _DETECT_SETTING_LIST{
	LIST_ENTRY 		ListEntry;
	PWCHAR			ProcessName;
	ULONG			ProcessAttributes;
	UNICODE_STRING	FullPath;
	ULONG			AccessAttributes;
} DETECT_SETTING_LIST, *PDETECT_SETTING_LIST;


//
//  This is a lookAside list used to allocate our pre-2-post structure.
//

DETECT_SETTING_LIST		gDetectList;
KSPIN_LOCK				gDetectListLick;


/*************************************************************************
    Prototypes
*************************************************************************/

NTSTATUS
InstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

VOID
CleanupVolumeContext(
    __in PFLT_CONTEXT Context,
    __in FLT_CONTEXT_TYPE ContextType
    );

VOID
CleanupAccessData(
    __in PFLT_CONTEXT Context,
    __in FLT_CONTEXT_TYPE ContextType
    );

NTSTATUS
InstanceQueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    );

NTSTATUS
FilterUnload (
    __in FLT_FILTER_UNLOAD_FLAGS Flags
    );

FLT_POSTOP_CALLBACK_STATUS
SwapPostReadBuffersWhenSafe (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
DetectZPreCreate(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_POSTOP_CALLBACK_STATUS
DetectZPostCreate (
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
DetectZPreClose(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_PREOP_CALLBACK_STATUS
DetectZPreCleanup(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_PREOP_CALLBACK_STATUS
DetectZPreRead(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_PREOP_CALLBACK_STATUS
DetectZPreWrite(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

FLT_PREOP_CALLBACK_STATUS
DetectZPreSetInfo(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    );

VOID
ReadDriverParameters (
    __in PUNICODE_STRING RegistryPath
    );

//////////////////////////////////////////////////////////////////////////////////////////////////////
//		FileAccessData
//////////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS
MakeFileAccessData(
    __in 	PFLT_CALLBACK_DATA		Data,
    __in PCFLT_RELATED_OBJECTS 		FltObjects,
    __inout PFLT_CONTEXT			pContext
    );

NTSTATUS GetProcessName(
	PWCHAR	ProcessName);

NTSTATUS GetProcessFullName(
	PWCHAR	ProcessNameBuffer, 
	ULONG 	BufferLen);

NTSTATUS ConstructFileName(
	PFLT_CALLBACK_DATA			Data, 
	PCFLT_RELATED_OBJECTS 		FltObjects,
	PFLT_CONTEXT				pContext);


//////////////////////////////////////////////////////////////////////////////////////////////////////
//		Zero-day Attack
//////////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS	
GetAndSetDetectSetting();

NTSTATUS	ReadFileData(
	UNICODE_STRING		FileName,
	PULONG				pulReadSize,
	PULONG				pulReadOffset,
	PVOID				pReadBuffer);

NTSTATUS SetDetectSetting(
				PVOID		pReadBuffer,
				PULONG		pulReadOffset);

NTSTATUS GetAccessCondition(
			PFILE_ACCESS_DATA	pFileAccessData);

BOOLEAN IsZeroAttack(
			PFILE_ACCESS_DATA	pFileAccessData,
			ULONG				ulAccessType);

NTSTATUS	NotifyAttackData(
			PFLT_CALLBACK_DATA	Data,
			PFILE_ACCESS_DATA	pFileAccessData,
			ULONG				ulAccessType);

NTSTATUS	ConstructWriteAttackData(
			PCHAR				pWriteData,
			PFILE_ACCESS_DATA	pFileAccessData,
			ULONG				ulAccessType);

NTSTATUS 	GetCurrentTime(
			PCHAR	pTime);

NTSTATUS	WriteFileData(
	UNICODE_STRING		FileName,
	PULONG				pulWriteSize,
	PVOID				pWriteBuffer);

// --> 外部から持ってきた関数ヘッダ
#pragma pack(1)

typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER   KernelTime;             // time spent in kernel mode
    LARGE_INTEGER   UserTime;               // time spent in user mode
    LARGE_INTEGER   CreateTime;             // thread creation time
    ULONG           WaitTime;               // wait time
    PVOID           StartAddress;           // start address
    CLIENT_ID       ClientId;               // thread and process IDs
    KPRIORITY       Priority;               // dynamic priority
    KPRIORITY       BasePriority;           // base priority
    ULONG           ContextSwitchCount;     // number of context switches
    LONG            State;                  // current state
    LONG            WaitReason;             // wait reason
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG           NextEntryDelta;         // offset to the next entry
    ULONG           ThreadCount;            // number of threads
    ULONG           Reserved1[6];           // reserved
    LARGE_INTEGER   CreateTime;             // process creation time
    LARGE_INTEGER   UserTime;               // time spent in user mode
    LARGE_INTEGER   KernelTime;             // time spent in kernel mode
    UNICODE_STRING  ProcessName;            // process name
    KPRIORITY       BasePriority;           // base process priority
    ULONG           ProcessId;              // process identifier
    ULONG           InheritedFromProcessId; // parent process identifier
    ULONG           HandleCount;            // number of handles
    ULONG           Reserved2[2];           // reserved
    VM_COUNTERS     VmCounters;             // virtual memory counters
#if _WIN32_WINNT >= 0x500
    IO_COUNTERS     IoCounters;             // i/o counters
#endif
    SYSTEM_THREAD_INFORMATION Threads[1];   // threads
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

typedef struct _PEB {
	UCHAR dummy[0x10];
	struct _RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
} PEB, *PPEB;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	ULONG                   MaximumLength;
	ULONG                   Length;
	ULONG                   Flags;
	ULONG                   DebugFlags;
	PVOID                   ConsoleHandle;
	ULONG                   ConsoleFlags;
	HANDLE                  StdInputHandle;
	HANDLE                  StdOutputHandle;
	HANDLE                  StdErrorHandle;
	UNICODE_STRING          CurrentDirectoryPath;
	HANDLE                  CurrentDirectoryHandle;
	UNICODE_STRING          DllPath;
	UNICODE_STRING          ImagePathName;
	UNICODE_STRING          CommandLine;
	PVOID                   Environment;
	ULONG                   StartingPositionLeft;
	ULONG                   StartingPositionTop;
	ULONG                   Width;
	ULONG                   Height;
	ULONG                   CharWidth;
	ULONG                   CharHeight;
	ULONG                   ConsoleTextAttributes;
	ULONG                   WindowFlags;
	ULONG                   ShowWindowFlags;
	UNICODE_STRING          WindowTitle;
	UNICODE_STRING          DesktopName;
	UNICODE_STRING          ShellInfo;
	UNICODE_STRING          RuntimeData;
//RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

#pragma pack()

//extern "C"
NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationProcess(
	HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength,
	PULONG ReturnLength
	);

void UTIL_UnicodeStringUpper(WCHAR *UniStr);
// <-- 外部から持ってきた関数ヘッダ