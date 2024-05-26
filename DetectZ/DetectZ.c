#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#pragma warning ( disable : 4995 )
#include <stdlib.h>
#include <ntstrsafe.h>
#include "DetectZ.h"

#pragma prefast(disable:__WARNING_ENCODE_MEMBER_FUNCTION_POINTER, "Not valid for kernel mode drivers")

PFLT_FILTER gFilterHandle = NULL;

//
//  Assign text sections for each routine.
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, InstanceSetup)
#pragma alloc_text(PAGE, CleanupVolumeContext)
#pragma alloc_text(PAGE, InstanceQueryTeardown)
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(INIT, ReadDriverParameters)
#pragma alloc_text(PAGE, FilterUnload)
#endif

//
//  Operation we currently care about.
//

CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_CREATE,
      0,
      DetectZPreCreate,
      DetectZPostCreate },

	{ IRP_MJ_CLOSE,
      0,
      DetectZPreClose,
      NULL },

	{ IRP_MJ_CLEANUP,
      0,
      DetectZPreCleanup,
      NULL },

	{ IRP_MJ_READ,
      0,
	  DetectZPreRead,
      NULL },

    { IRP_MJ_WRITE,
      0,
      DetectZPreWrite,
      NULL },

    { IRP_MJ_SET_INFORMATION,
      0,
      DetectZPreSetInfo,
      NULL },

    { IRP_MJ_OPERATION_END }
};

//
//  Context definitions we currently care about.  Note that the system will
//  create a lookAside list for the volume context because an explicit size
//  of the context is specified.
//

CONST FLT_CONTEXT_REGISTRATION ContextNotifications[] = {

     { FLT_VOLUME_CONTEXT,
       0,
       CleanupVolumeContext,
       sizeof(VOLUME_CONTEXT),
       CONTEXT_TAG },
       
     { FLT_STREAMHANDLE_CONTEXT,        //ContextType
       0,                               //Flags
       CleanupAccessData,				//ContextCleanupCallback
       sizeof(FILE_ACCESS_DATA),     	//Size
       CONTEXT_TAG },     				//PoolTag

     { FLT_CONTEXT_END }
};

//
//  This defines what we want to filter with FltMgr
//

CONST FLT_REGISTRATION FilterRegistration = {

    sizeof( FLT_REGISTRATION ),         //  Size
    FLT_REGISTRATION_VERSION,           //  Version
    0,                                  //  Flags

    ContextNotifications,               //  Context
    Callbacks,                          //  Operation callbacks

    FilterUnload,                       //  MiniFilterUnload

    InstanceSetup,                      //  InstanceSetup
    InstanceQueryTeardown,              //  InstanceQueryTeardown
    NULL,                               //  InstanceTeardownStart
    NULL,                               //  InstanceTeardownComplete

    NULL,                               //  GenerateFileName
    NULL,                               //  GenerateDestinationFileName
    NULL                                //  NormalizeNameComponent

};

/*************************************************************************
    Debug tracing information
*************************************************************************/

//
//  Definitions to display log messages.  The registry DWORD entry:
//  "hklm\system\CurrentControlSet\Services\Swapbuffers\DebugFlags" defines
//  the default state of these logging flags
//

#define LOGFL_ERRORS    0x00000001  // if set, display error messages
#define LOGFL_READ      0x00000002  // if set, display READ operation info
#define LOGFL_WRITE     0x00000004  // if set, display WRITE operation info
#define LOGFL_DIRCTRL   0x00000008  // if set, display DIRCTRL operation info
#define LOGFL_VOLCTX    0x00000010  // if set, display VOLCTX operation info

ULONG LoggingFlags = 0;             // all disabled by default

#define LOG_PRINT( _logFlag, _string )                              \
    (FlagOn(LoggingFlags,(_logFlag)) ?                              \
        DbgPrint _string  :                                         \
        ((void)0))

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//                      Routines
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


NTSTATUS
InstanceSetup (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_SETUP_FLAGS Flags,
    __in DEVICE_TYPE VolumeDeviceType,
    __in FLT_FILESYSTEM_TYPE VolumeFilesystemType
    )
/*++

Routine Description:

    This routine is called whenever a new instance is created on a volume.

    By default we want to attach to all volumes.  This routine will try and
    get a "DOS" name for the given volume.  If it can't, it will try and
    get the "NT" name for the volume (which is what happens on network
    volumes).  If a name is retrieved a volume context will be created with
    that name.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Flags describing the reason for this attach request.

Return Value:

    STATUS_SUCCESS - attach
    STATUS_FLT_DO_NOT_ATTACH - do not attach

--*/
{

    PDEVICE_OBJECT devObj = NULL;
    PVOLUME_CONTEXT ctx = NULL;
    NTSTATUS status;
    ULONG retLen;
    PUNICODE_STRING workingName;
    USHORT size;
    UCHAR volPropBuffer[sizeof(FLT_VOLUME_PROPERTIES)+512];
    PFLT_VOLUME_PROPERTIES volProp = (PFLT_VOLUME_PROPERTIES)volPropBuffer;

    PAGED_CODE();

    UNREFERENCED_PARAMETER( Flags );
    UNREFERENCED_PARAMETER( VolumeDeviceType );
    UNREFERENCED_PARAMETER( VolumeFilesystemType );

    //　コンテキストのメモリ確保
    //  Allocate a volume context structure.
    //

    status = FltAllocateContext( FltObjects->Filter,
                                 FLT_VOLUME_CONTEXT,
                                 sizeof(VOLUME_CONTEXT),
                                 NonPagedPool,
                                 &ctx );

    if (NT_SUCCESS(status)) {

	    //	ボリュームのプロパティ取得
	    //  Always get the volume properties, so I can get a sector size
	    //

	    status = FltGetVolumeProperties( FltObjects->Volume,
	                                     volProp,
	                                     sizeof(volPropBuffer),
	                                     &retLen );
		
    }

    if (NT_SUCCESS(status)) {

	    //
	    //  Save the sector size in the context for later use.  Note that
	    //  we will pick a minimum sector size if a sector size is not
	    //  specified.
	    //

	    ASSERT((volProp->SectorSize == 0) || (volProp->SectorSize >= MIN_SECTOR_SIZE));

	    ctx->SectorSize = max(volProp->SectorSize,MIN_SECTOR_SIZE);

	    //
	    //  Init the buffer field (which may be allocated later).
	    //

	    ctx->Name.Buffer = NULL;

	    //  対象ディスクのデバイスオブジェクト取得
	    //  Get the storage device object we want a name for.
	    //

	    status = FltGetDiskDeviceObject( FltObjects->Volume, &devObj );
	    

    }

    if (NT_SUCCESS(status)) {

	    //	
	    //  Try and get the DOS name.  If it succeeds we will have
	    //  an allocated name buffer.  If not, it will be NULL
	    //

	    status = RtlVolumeDeviceToDosName( devObj, &ctx->Name );

        if (NT_SUCCESS(status)) {

		    //  コンテキストを設定する
		    //  Set the context
		    //

		    status = FltSetVolumeContext( FltObjects->Volume,
		                                  FLT_SET_CONTEXT_KEEP_IF_EXISTS,
		                                  ctx,
		                                  NULL );

		    //
		    //  Log debug info
		    //

		    LOG_PRINT( LOGFL_VOLCTX,
		               ("SwapBuffers!InstanceSetup:                  Real SectSize=0x%04x, Used SectSize=0x%04x, Name=\"%wZ\"\n",
		                volProp->SectorSize,
		                ctx->SectorSize,
		                &ctx->Name) );

		    //
		    //  It is OK for the context to already be defined.
		    //

		    if (status == STATUS_FLT_CONTEXT_ALREADY_DEFINED) {

		        status = STATUS_SUCCESS;
		    }
        	
        }
        else {
        	// Dos名が取得できない場合、ネットワークのためアタッチしない
        	status = STATUS_FLT_DO_NOT_ATTACH;
        }


        //
	    //  Remove the reference added to the device object by
	    //  FltGetDiskDeviceObject.
	    //

	    if (devObj) {

	        ObDereferenceObject( devObj );
		}
	}

    //
    //  Always release the context.  If the set failed, it will free the
    //  context.  If not, it will remove the reference added by the set.
    //  Note that the name buffer in the ctx will get freed by the context
    //  cleanup routine.
    //

    if (ctx) {

        FltReleaseContext( ctx );
    }
    return status;
}


VOID
CleanupVolumeContext(
    __in PFLT_CONTEXT Context,
    __in FLT_CONTEXT_TYPE ContextType
    )
/*++

Routine Description:

    The given context is being freed.
    Free the allocated name buffer if there one.

Arguments:

    Context - The context being freed

    ContextType - The type of context this is

Return Value:

    None

--*/
{
    PVOLUME_CONTEXT ctx = Context;

    PAGED_CODE();

    UNREFERENCED_PARAMETER( ContextType );

    ASSERT(ContextType == FLT_VOLUME_CONTEXT);

    if (ctx->Name.Buffer != NULL) {

        ExFreePool(ctx->Name.Buffer);
        ctx->Name.Buffer = NULL;
    }
}



VOID
CleanupAccessData(
    __in PFLT_CONTEXT Context,
    __in FLT_CONTEXT_TYPE ContextType
    )
/*++

Routine Description:

    The given context is being freed.
    Free the allocated name buffer if there one.

Arguments:

    Context - The context being freed

    ContextType - The type of context this is

Return Value:

    None

--*/
{
    PFILE_ACCESS_DATA pFileAccessData = Context;

    PAGED_CODE();

    UNREFERENCED_PARAMETER( ContextType );

    ASSERT(ContextType == FLT_STREAMHANDLE_CONTEXT);

	// 確保したメモリの解放
    if (pFileAccessData->FullPath.Buffer != NULL) {

        ExFreePool(pFileAccessData->FullPath.Buffer);
        pFileAccessData->FullPath.Buffer = NULL;
    }
    
    if (pFileAccessData->ProcessName != NULL) {
        ExFreePool(pFileAccessData->ProcessName);
        pFileAccessData->ProcessName = NULL;
    }
}

NTSTATUS
InstanceQueryTeardown (
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    )
/*++

Routine Description:

    This is called when an instance is being manually deleted by a
    call to FltDetachVolume or FilterDetach.  We always return it is OK to
    detach.

Arguments:

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance and its associated volume.

    Flags - Indicating where this detach request came from.

Return Value:

    Always succeed.

--*/
{
    PAGED_CODE();
    
	KdPrint(("DetectZ : InstanceQueryTeardown Enter\n"));

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( Flags );
    
	KdPrint(("DetectZ : InstanceQueryTeardown Exit\n"));

    return STATUS_SUCCESS;
}

VOID
ReadDriverParameters (
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This routine tries to read the driver-specific parameters from
    the registry.  These values will be found in the registry location
    indicated by the RegistryPath passed in.

Arguments:

    RegistryPath - the path key passed to the driver during driver entry.

Return Value:

    None.

--*/
{
    OBJECT_ATTRIBUTES attributes;
    HANDLE driverRegKey;
    NTSTATUS status;
    ULONG resultLength;
    UNICODE_STRING valueName;
    UCHAR buffer[sizeof( KEY_VALUE_PARTIAL_INFORMATION ) + sizeof( LONG )];

    //
    //  If this value is not zero then somebody has already explicitly set it
    //  so don't override those settings.
    //

    if (0 == LoggingFlags) {

        //
        //  Open the desired registry key
        //

        InitializeObjectAttributes( &attributes,
                                    RegistryPath,
                                    OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                    NULL,
                                    NULL );

        status = ZwOpenKey( &driverRegKey,
                            KEY_READ,
                            &attributes );

        if (!NT_SUCCESS( status )) {

            return;
        }

        //
        // Read the given value from the registry.
        //

        RtlInitUnicodeString( &valueName, L"DebugFlags" );

        status = ZwQueryValueKey( driverRegKey,
                                  &valueName,
                                  KeyValuePartialInformation,
                                  buffer,
                                  sizeof(buffer),
                                  &resultLength );

        if (NT_SUCCESS( status )) {

            LoggingFlags = *((PULONG) &(((PKEY_VALUE_PARTIAL_INFORMATION)buffer)->Data));
        }

        //
        //  Close the registry entry
        //

        ZwClose(driverRegKey);
    }
}

void UTIL_UnicodeStringUpper(WCHAR *UniStr)
{
#ifdef FREE_LOG
	DbgPrint("UTIL_UnicodeStringUpper(%8.8lx)\n", (ULONG)UniStr);
#endif

	WCHAR *CurrentPtr = UniStr;

	while(TRUE){
		if (CurrentPtr[0] == L'\0'){
			break;
		}else{
			if ((CurrentPtr[0] >= L'a') && (CurrentPtr[0] <= L'z')){
				USHORT Dif = L'z' - CurrentPtr[0];
				CurrentPtr[0] = L'Z' - Dif;
			}
		}
		CurrentPtr++;
	}
}


/*************************************************************************
    Initialization and unload routines.
*************************************************************************/

NTSTATUS
DriverEntry (
    __in PDRIVER_OBJECT DriverObject,
    __in PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    This is the initialization routine.  This registers with FltMgr and
    initializes all global data structures.

Arguments:

    DriverObject - Pointer to driver object created by the system to
        represent this driver.

    RegistryPath - Unicode string identifying where the parameters for this
        driver are located in the registry.

Return Value:

    Status of the operation

--*/
{
	
    NTSTATUS status;
	KdPrint(("DetectZ : DriverEntry Enter\n"));

    //
    //  Get debug trace flags
    //

    ReadDriverParameters( RegistryPath );


    //　FltMgrに本モジュールを登録
    //  Register with FltMgr
    //
    status = FltRegisterFilter( DriverObject,
                                &FilterRegistration,
                                &gFilterHandle );

    if (NT_SUCCESS( status )) {

        //　フィルタ開始
        //  Start filtering i/o
        //

        status = FltStartFiltering( gFilterHandle );
        
        if( NT_SUCCESS(status)){
        	// ゼロデイ攻撃対象設定取得
        	// 設定が無くても監視するため、エラー値はチェックしない
        	InitializeListHead(&gDetectList.ListEntry);
        	GetAndSetDetectSetting();
        }
    }
    
 	if (NT_SUCCESS( status ) == FALSE) {
 		// いずれかの処理に失敗
        FltUnregisterFilter( gFilterHandle );
    }
 
	KdPrint(("DetectZ : DriverEntry Exit\n"));

    return status;
}


NTSTATUS
FilterUnload (
    __in FLT_FILTER_UNLOAD_FLAGS Flags
    )
/*++

Routine Description:

    Called when this mini-filter is about to be unloaded.  We unregister
    from the FltMgr and then return it is OK to unload

Arguments:

    Flags - Indicating if this is a mandatory unload.

Return Value:

    Returns the final status of this operation.

--*/
{
    PAGED_CODE();
    
	KdPrint(("DetectZ : FilterUnload Enter\n"));

    UNREFERENCED_PARAMETER( Flags );

	if(gFilterHandle != NULL){
	    //
	    //  Unregister from FLT mgr
	    //

	    FltUnregisterFilter( gFilterHandle );
	}


	KdPrint(("DetectZ : FilterUnload Exit\n"));

    return STATUS_SUCCESS;
}


/*************************************************************************
    MiniFilter callback routines.
*************************************************************************/

FLT_PREOP_CALLBACK_STATUS
DetectZPreCreate(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is called on Pre-CREATE operation.
    It doesn't do anything for now.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - Receives the context that will be passed to the
        post-operation callback.

Return Value:

    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    
--*/
{
	// 今版では何もしない

    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
    
    
    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
    
}



FLT_POSTOP_CALLBACK_STATUS
DetectZPostCreate(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __in PVOID CompletionContext,
    __in FLT_POST_OPERATION_FLAGS Flags
    )
/*++

Routine Description:

	This routine is called on POST-CREATE operation.
	It aquires file access information, then store it
    on the context which FltMgr Manage.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - The completion context set in the pre-operation routine.

    Flags - Denotes whether the completion is successful or is being drained.

Return Value:

    FLT_POSTOP_FINISHED_PROCESSING

--*/
{
    NTSTATUS 			status;
    PFILE_ACCESS_DATA 	AccessData = NULL;

    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( CompletionContext );
    UNREFERENCED_PARAMETER( Flags );

    //
    //  This system won't draining an operation with swapped buffers, verify
    //  the draining flag is not set.
    //

    ASSERT(!FlagOn(Flags, FLTFL_POST_OPERATION_DRAINING));

	// コンテキスト用のメモリ確保
	status = FltAllocateContext(
				FltObjects->Filter,
				FLT_STREAMHANDLE_CONTEXT,
				sizeof(FILE_ACCESS_DATA),
				NonPagedPool,
				&AccessData);
	
	if( NT_SUCCESS(status)){
		// ファイル情報取得
		status = MakeFileAccessData( Data, FltObjects, (PFLT_CONTEXT)AccessData);
		
		if( NT_SUCCESS(status)){
			// アクセス制御情報取得
			status = GetAccessCondition( AccessData);
			if( NT_SUCCESS(status)){
				// コンテキストに情報を登録
				status = FltSetStreamHandleContext(
									FltObjects->Instance,
									FltObjects->FileObject,
		    						FLT_SET_CONTEXT_REPLACE_IF_EXISTS,
		    						AccessData,
		    						NULL);
			}
		}
		// コンテキストリリース
    	FltReleaseContext( AccessData );
	}
	else {
		status = STATUS_INSUFFICIENT_RESOURCES;
	}
	
	
	return FLT_POSTOP_FINISHED_PROCESSING;
}



FLT_PREOP_CALLBACK_STATUS
DetectZPreClose(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is called on Pre-CLOSE operation.
    It doesn't do anything for now.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - Receives the context that will be passed to the
        post-operation callback.

Return Value:

    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback

--*/
{

    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
    
   
    return FLT_PREOP_SUCCESS_NO_CALLBACK;
    
}

FLT_PREOP_CALLBACK_STATUS
DetectZPreCleanup(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is called on Pre-CLEANUP operation.
    It doesn't do anything for now.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - Receives the context that will be passed to the
        post-operation callback.

Return Value:

    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback

--*/
{
	// 今版では何もしない

    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );

   
    return FLT_PREOP_SUCCESS_NO_CALLBACK;
    
}

FLT_PREOP_CALLBACK_STATUS
DetectZPreRead(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is called on Pre-READ operation.
    It sets READ flag to the Context.
    If file acess matches the Zero-Day Attack condition,
    it proceeds notification process.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - Receives the context that will be passed to the
        post-operation callback.

Return Value:

    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback

--*/
{
	NTSTATUS			status;
	PFILE_ACCESS_DATA	pFileAccessData = NULL;

	KdPrint(("DetectZ : DetectZPreRead Enter\n"));

    UNREFERENCED_PARAMETER( CompletionContext );
    
    // 引数有効？
    if(Data != NULL && FltObjects != NULL){
    	// アクセス情報取得
	    status = FltGetStreamHandleContext(
									FltObjects->Instance,
									FltObjects->FileObject,
									&pFileAccessData);
	    if(NT_SUCCESS(status)){
	        // 書き込みフラグ設定
	        pFileAccessData->AccessRecord |= READ_OPERATION;
	        
	        // ゼロデイ攻撃？
	        if(IsZeroAttack( pFileAccessData, READ_OPERATION) == TRUE){
	        	// ゼロデイ攻撃通知
	        	status = NotifyAttackData( Data, pFileAccessData, READ_OPERATION);
	        }
	        
	        // アクセス情報上書き
	        FltSetFileContext(
						FltObjects->Instance,
						FltObjects->FileObject,
						FLT_SET_CONTEXT_REPLACE_IF_EXISTS,
	        			pFileAccessData,
	        			NULL); 
	        
	    	// Setした分のリファレンスカウントダウン
	    	FltReleaseContext(pFileAccessData);
	        
	    }
	}
    
	KdPrint(("DetectZ : DetectZPreRead Exit\n"));
    
    return FLT_PREOP_SUCCESS_NO_CALLBACK;

    
}


FLT_PREOP_CALLBACK_STATUS
DetectZPreWrite(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is called on Pre-WRITE operation.
    It sets WRITE flag to the Context.
    If file acess matches the Zero-Day Attack condition,
    it proceeds notification process.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - Receives the context that will be passed to the
        post-operation callback.

Return Value:

    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback

--*/
{
	NTSTATUS			status;
	PFILE_ACCESS_DATA	pFileAccessData = NULL;

	KdPrint(("DetectZ : DetectZPreWrite Enter\n"));

    UNREFERENCED_PARAMETER( CompletionContext );
    
    // 引数有効？
    if( Data != NULL && FltObjects != NULL){
    	// アクセス情報取得
	    status = FltGetStreamHandleContext(
							FltObjects->Instance,
							FltObjects->FileObject,
	    					&pFileAccessData);
	    if(NT_SUCCESS(status)){
	        // 書き込みフラグ設定
	        pFileAccessData->AccessRecord |= WRITE_OPERATION;
	        
	        // ゼロデイ攻撃？
	        if(IsZeroAttack( pFileAccessData, WRITE_OPERATION) == TRUE){
	        	// ゼロデイ攻撃通知
	        	status = NotifyAttackData( Data, pFileAccessData, WRITE_OPERATION);
	        }
	        
	        // アクセス情報上書き
	        FltSetFileContext(
						FltObjects->Instance,
						FltObjects->FileObject,
	        			FLT_SET_CONTEXT_REPLACE_IF_EXISTS,
	        			pFileAccessData,
	        			NULL); 
	        
	        // コンテキストリリース
	        FltReleaseContext(pFileAccessData);
	        
	    }
	}
    
	KdPrint(("DetectZ : DetectZPreWrite Exit\n"));
    
    return FLT_PREOP_SUCCESS_NO_CALLBACK;
    
}


FLT_PREOP_CALLBACK_STATUS
DetectZPreSetInfo(
    __inout PFLT_CALLBACK_DATA Data,
    __in PCFLT_RELATED_OBJECTS FltObjects,
    __deref_out_opt PVOID *CompletionContext
    )
/*++

Routine Description:

    This routine is called on Pre-SET_INFORMATION operation.
    It doesn't do anything for now.

Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    FltObjects - Pointer to the FLT_RELATED_OBJECTS data structure containing
        opaque handles to this filter, instance, its associated volume and
        file object.

    CompletionContext - Receives the context that will be passed to the
        post-operation callback.

Return Value:

    FLT_PREOP_SUCCESS_WITH_CALLBACK - we want a postOpeation callback
    FLT_PREOP_SUCCESS_NO_CALLBACK - we don't want a postOperation callback

--*/
{
	// 今版では何もしない
	KdPrint(("DetectZ : DetectZPreSetInfo Enter\n"));

    UNREFERENCED_PARAMETER( Data );
    UNREFERENCED_PARAMETER( FltObjects );
    UNREFERENCED_PARAMETER( CompletionContext );
  	KdPrint(("DetectZ : DetectZPreSetInfo Exit\n"));
  
    return FLT_PREOP_SUCCESS_NO_CALLBACK;
    
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//		FileAccessData
//////////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS
MakeFileAccessData(
    __in 	PFLT_CALLBACK_DATA		Data,
    __in PCFLT_RELATED_OBJECTS 		FltObjects,
    __inout PFLT_CONTEXT			pContext
    )
/*++

Routine Description:

    It acquires the information of the file access that is
    currently processing. For the objects of the context that
    is just a pointer, it allocates memory.
    
Arguments:

    Data - Pointer to the filter callbackData that is passed to us.

    pContext - Pointer to the FLT_CONTEXT data structure
        
Return Value:

    STATUS_SUCCESS
    STATUS_INVALID_PARAMETER
    STATUS_UNSUCCESS
    
--*/
{
	NTSTATUS 			status;
	PFILE_ACCESS_DATA	pFileAccessData;
	
	

	// 引数は有効？
    if( Data != NULL && pContext != NULL && FltObjects != NULL){
    	pFileAccessData = (PFILE_ACCESS_DATA)pContext;
    	RtlZeroMemory(pFileAccessData, sizeof(FILE_ACCESS_DATA));
    	
    	// ディレクトリか否かを取得
    	status = FltIsDirectory(
    				Data->Iopb->TargetFileObject,
    				Data->Iopb->TargetInstance,
    				&pFileAccessData->IsDirectory);
    	
    	if(NT_SUCCESS(status)){
    		// プロセス名用のメモリ確保
    		pFileAccessData->ProcessName = ExAllocatePoolWithTag(NonPagedPool,
											NT_PROCNAMELEN,
											'CORP');
			if(pFileAccessData->ProcessName == NULL){
				// メモリ確保に失敗
				status = STATUS_INSUFFICIENT_RESOURCES;
			}
			else{
	    		// プロセス名の取得
	    		status = GetProcessName(pFileAccessData->ProcessName);
	    	}
    	}
    	if(NT_SUCCESS(status)){
    		// ファイル名の取得
    		status = ConstructFileName( Data, FltObjects, pFileAccessData);
    		if(NT_SUCCESS(status) == FALSE){
    			// 失敗したらプロセス名用のメモリを解放する
    			ExFreePool(pFileAccessData->ProcessName);
    			pFileAccessData->ProcessName = NULL;
    		}

    	}
    }
    else {
    	status = STATUS_INVALID_PARAMETER;
    }
    
    return status;
}

NTSTATUS GetProcessName(
	PWCHAR	ProcessName)
{
// 全体的に変わるので元データ削除
	NTSTATUS	Status = STATUS_SUCCESS;
	char		*nameptr;

	// 引数は有効？
	if (ProcessName != NULL){

		// 仮にプロセス名が取得できなかった場合、プロセス名格納用メモリを
		// 他処理で参照しても問題ないよう、ターミネータを入れる
		ProcessName[0] = '\0';
		
		Status = GetProcessFullName(ProcessName, NT_PROCNAMELEN);
	/*	if(!NT_SUCCESS(Status)){
	
			// 日本語未対応のプロセス名取得
			PEPROCESS	curprc = PsGetCurrentProcess() ;
			nameptr = (PCHAR) curprc + ProcessNameOffset;

			if (nameptr != NULL){
				// プロセス名の前後に[]を付加する
				strcpy(ProcessName, "[");
				strcat(ProcessName, nameptr);
				strcat(ProcessName, "]");
				UTIL_AnsiStringUpper(ProcessName);
			} else {
				// プロセス名取得できない場合は[unknown]にする
				strcpy(ProcessName, "[unknown]");
			}
		}
		Status = STATUS_SUCCESS;
	*/
	}else{
		Status = STATUS_INVALID_PARAMETER;
	}
	return Status;
}


NTSTATUS GetProcessFullName(
	PWCHAR	ProcessNameBuffer, 
	ULONG 	BufferLen)
{
	NTSTATUS		Status = STATUS_UNSUCCESSFUL;	// 戻り値を格納する
	char			*ProcBuf = NULL;				// プロセス情報取得用メモリ
	PPROCESS_BASIC_INFORMATION ProcInfo;			// プロセス情報保持ポインタ
	ULONG			nReturned;
	PRTL_USER_PROCESS_PARAMETERS UserParams = NULL;	// プロセス名を保持するパラメーター

	// 引数は有効？
	if(ProcessNameBuffer != NULL){
		// プロセス情報取得用メモリ確保
		ProcBuf  = (char*)ExAllocatePoolWithTag( PagedPool, sizeof(PROCESS_BASIC_INFORMATION), '61CN') ;
		if (ProcBuf != NULL){
			
			// プロセス情報取得
			ZwQueryInformationProcess(
							NtCurrentProcess(),
							ProcessBasicInformation,
							(PVOID)ProcBuf,
							sizeof(PROCESS_BASIC_INFORMATION),
							&nReturned);
			ProcInfo = (PPROCESS_BASIC_INFORMATION)ProcBuf;

			if (ProcInfo){
				if (ProcInfo->PebBaseAddress){
					if (ProcInfo->PebBaseAddress->ProcessParameters){
						// プロセス情報は有効
						UserParams = ProcInfo->PebBaseAddress->ProcessParameters;
					}
				}
			}
		} else {
			// プロセス情報取得用メモリ確保失敗
			Status = STATUS_INSUFFICIENT_RESOURCES;
		}
	} else {
		Status = STATUS_INVALID_PARAMETER;
	}

	if(UserParams != NULL){
		// 取得したプロセス情報が有効

		// プロセス名の開始と終了の位置取得
		WCHAR *wBeginPos = NULL;	// 一番最後に'\\'が出現する位置
		// 最後の文字の位置(NULLは含んでいない)
		WCHAR *wLastPos = &UserParams->ImagePathName.Buffer[UserParams->ImagePathName.Length / 2];

		if (!MmIsAddressValid(UserParams->ImagePathName.Buffer)){
		} else {
			// 最後の\\の位置を探す
			WCHAR *wProcessFullName = UserParams->ImagePathName.Buffer;
			LONG i;
			for (i = UserParams->ImagePathName.Length / 2 - 1; i >= 0; i --){
				if (wBeginPos == NULL){
					if (UserParams->ImagePathName.Buffer[i] == L'\\'){
						wBeginPos = &UserParams->ImagePathName.Buffer[i];
						wBeginPos ++;	// 現在の場所は\\なので、１文字進めてEXE名の先頭にする
						break;
					}
				}
			}
		}

		if (wBeginPos && wLastPos){
			ULONG CopyLen = (ULONG)wLastPos - (ULONG)wBeginPos;

			if (BufferLen > CopyLen){
				// プロセス名の格納
				RtlCopyMemory(ProcessNameBuffer, wBeginPos, CopyLen);	// 文字列部分のコピー
				ProcessNameBuffer[CopyLen / 2] = L'\0';					// 最後のNULL
				UTIL_UnicodeStringUpper(ProcessNameBuffer);
				Status = STATUS_SUCCESS;
			}
		}
	}
	if(ProcBuf != NULL){
		ExFreePool(ProcBuf);
	}
	return Status;
}

NTSTATUS ConstructFileName(
	PFLT_CALLBACK_DATA			Data, 
	PCFLT_RELATED_OBJECTS 		FltObjects,
	PFLT_CONTEXT				pContext)
{
	NTSTATUS					status 		= STATUS_SUCCESS;
	PVOLUME_CONTEXT 			ctx 		= NULL;
	PFILE_ACCESS_DATA			pAccessData = NULL;
	PFLT_FILE_NAME_INFORMATION 	pFileName 	= NULL;
	PWCHAR						pStartPtr 	= NULL;
	PWCHAR						pEndPtr 	= NULL;
	BOOLEAN						bContinue;
	int							nCount;
	PWCHAR						pTemp;
	
	
	// 引数は有効？
	if (Data != NULL && FltObjects != NULL && pContext != NULL){
		
		// ドライブ名の取得
		status = FltGetVolumeContext( 
					FltObjects->Filter,
					FltObjects->Volume,
					&ctx);
		
        if (NT_SUCCESS(status)) {
        	// ファイル名の取得
        	status = FltGetFileNameInformation(
        				Data,
        				FLT_FILE_NAME_NORMALIZED,
        				&pFileName);
        	
        	if(NT_SUCCESS(status)){
				// ファイル名の構築
				
				// 取得したファイル名は"\Device\HarddiskVolume1\Documents and Settings・・"となるため、
				// 3つ目の\からのファイル名を取得する
				pStartPtr = pFileName->Name.Buffer;
				bContinue = TRUE;
				nCount = 0;
				while(bContinue == TRUE){
					if(*pStartPtr == L'\\'){
						nCount++;
						if(nCount == 3){
							bContinue = FALSE;
							break;
						}
					}
					pStartPtr++;
				}
				
				// ファイル名の末尾に":stream1"が存在するケースのため、削除する
				pEndPtr = pStartPtr;
				bContinue = TRUE;
				// ファイル名の最後を指す
				pTemp = pFileName->Name.Buffer + (pFileName->Name.Length/sizeof(WCHAR)) -1;
				while(bContinue == TRUE){
					if( (*pEndPtr == L':') || (pEndPtr >= pTemp) ){
						// ：を発見または、ファイル名長分の文字を確認したので終わり
						bContinue = FALSE;
					}
					pEndPtr++;
				}
				
				pTemp = ExAllocatePoolWithTag(NonPagedPool,
												(int)( ((char*)pEndPtr - (char*)pStartPtr) + sizeof(WCHAR)),
												'PMET');
				if(pTemp != NULL){
					RtlCopyMemory(pTemp, pStartPtr, (int)((char*)pEndPtr - (char*)pStartPtr));
					pTemp[((char*)pEndPtr - (char*)pStartPtr)/sizeof(WCHAR)] = L'\0';
					UTIL_UnicodeStringUpper(pTemp);
					
					// ファイル名用のメモリ確保
					pAccessData = (PFILE_ACCESS_DATA)pContext;
					pAccessData->FullPath.Length = 0;
					pAccessData->FullPath.MaximumLength = pFileName->Name.MaximumLength;
					pAccessData->FullPath.Buffer = ExAllocatePoolWithTag( 
														NonPagedPool,
														pAccessData->FullPath.MaximumLength,
														'TSET');
					if(pAccessData->FullPath.Buffer != NULL){
						// フルパスの構築
						RtlCopyUnicodeString( &pAccessData->FullPath, &ctx->Name);
						RtlAppendUnicodeToString( &pAccessData->FullPath, pTemp);
					}
					else {
						// フルパス用のメモリ確保失敗
						status = STATUS_INSUFFICIENT_RESOURCES;
					}
					ExFreePool(pTemp);
				}
				else {
					// 一時メモリ確保失敗
					status = STATUS_INSUFFICIENT_RESOURCES;
				}
				// ファイル名のメモリをリリース
				FltReleaseFileNameInformation( pFileName);
        	}
    		// 取得したコンテキストのリリース
			FltReleaseContext( ctx );
        }

	}
	else {
		// 引数が不正
		status = STATUS_INVALID_PARAMETER;
	}
	
	return status;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//		Zero-day Attack
//////////////////////////////////////////////////////////////////////////////////////////////////////

NTSTATUS	GetAndSetDetectSetting()
{
	NTSTATUS 		status;
	PCHAR			pBuffer = NULL;
	BOOLEAN			bContinue;
	UNICODE_STRING	SettingFile;
	ULONG			ulReadSize;
	ULONG			ulReadOffset;

	
	// 読み込み用メモリ確保
	pBuffer = ExAllocatePoolWithTag( NonPagedPool,
									READ_DATA_SIZE + 1,
									'DAER');
	if(pBuffer != NULL){
		bContinue = TRUE;
		ulReadSize = READ_DATA_SIZE;
		ulReadOffset = 0;
		RtlInitUnicodeString( &SettingFile, L"\\DosDevices\\C:\\DetectZSetting.txt");
		while(bContinue == TRUE){
			status = ReadFileData( SettingFile,
									&ulReadSize,
									&ulReadOffset,
									pBuffer);
			if( NT_SUCCESS(status)){
				// 制御対象情報設定
				status = SetDetectSetting(	pBuffer,
											&ulReadOffset);
			}
			else{
				// アクセス情報取得失敗
				break;
			}
			
			if(ulReadSize < READ_DATA_SIZE || ulReadOffset == 0){
				// データの読み込みまたは設定で最後まで完了したと判断
				bContinue = FALSE;
			}
			else {
				// 引き続き、設定情報を取得する
				ulReadSize = READ_DATA_SIZE;
			}
		}
		
		ExFreePool(pBuffer);
	}
	else{
		status = STATUS_INSUFFICIENT_RESOURCES;
	}
	return status;
}

NTSTATUS	ReadFileData(
	UNICODE_STRING		FileName,
	PULONG				pulReadSize,
	PULONG				pulReadOffset,
	PVOID				pReadBuffer)
{
	NTSTATUS			status;
	HANDLE				hFile;
	OBJECT_ATTRIBUTES	ObjectAttributes;
	IO_STATUS_BLOCK		IoStatusBlock;
	LARGE_INTEGER 		offset;	
	
	// 引数は有効？
	if( FileName.Buffer != NULL && pulReadSize != NULL && pulReadOffset != NULL && pReadBuffer != NULL){
		// Passiveレベル？
		if(KeGetCurrentIrql() == PASSIVE_LEVEL){
			status = STATUS_SUCCESS;
		}
		else{
			// 不正なIRQL
			status = STATUS_UNSUCCESSFUL;
		}
	}
	else {
		// 引数が無効
		status = STATUS_INVALID_PARAMETER;
	}
	
	if( NT_SUCCESS(status)){
		InitializeObjectAttributes(
					&ObjectAttributes,
					&FileName,
					OBJ_CASE_INSENSITIVE,
					NULL,
					NULL);

		// ファイルを開く
		status = FltCreateFile(
					gFilterHandle,		// Filter
					NULL,				// Instance OPTIONAL
					&hFile,				// FileHandle
					GENERIC_READ,		// DesiredAccess
					&ObjectAttributes,	// ObjectAttributes
					&IoStatusBlock,		// IoStatusBlock
					NULL,				// AllocationSize OPTIONAL
					0L,					// FileAttributes
					FILE_SHARE_READ,	// ShareAccess
					FILE_OPEN,			// CreateDisposition
					FILE_NON_DIRECTORY_FILE, // CreateOptions
					NULL,				// EaBuffer OPTIONAL
					0L,					// EaLength
					0					// Flags
					);

		if(NT_SUCCESS(status)){

			offset.QuadPart = *pulReadOffset;
			// ファイルを読み込む
			status = ZwReadFile( 
						hFile,
						NULL,
						NULL,
						NULL,
						&IoStatusBlock,
						pReadBuffer,
						READ_DATA_SIZE,
						&offset,
						NULL);

			if(NT_SUCCESS(status) == TRUE){
				*pulReadSize = IoStatusBlock.Information;
			}
			else if(status == STATUS_END_OF_FILE){
				// 読みはじめが、EOFを超えている場合は成功にし、読み込んだサイズを0にする
				status = STATUS_SUCCESS;
				*pulReadSize = 0;
			}
			// ファイルを読み込む
			FltClose(hFile);
		}
	}
	   	
	return status;
}

NTSTATUS SetDetectSetting(
				PVOID		pReadBuffer,
				PULONG		pulReadOffset)
{
	NTSTATUS	status = STATUS_SUCCESS;
	char		szSearch[] = ",";
	int			nSearchCount;
	PDETECT_SETTING_LIST	pSettingList;
    KIRQL oldIrql;
	PCHAR		pStartPtr;
	PCHAR		pSearchPtr = NULL;
	char		szTemp[_MAX_PATH];
	WCHAR		wcsTemp[_MAX_PATH];
	PLIST_ENTRY	pList;
	
	// 引数は有効？
	if( pReadBuffer != NULL && pulReadOffset!= NULL){
		nSearchCount = 1;
		pStartPtr = (PCHAR)pReadBuffer;
		pSearchPtr = strchr((char*)pReadBuffer, szSearch[0]);
		pSettingList = ExAllocatePoolWithTag(NonPagedPool, sizeof(DETECT_SETTING_LIST), 'TSIL');
		
		while( (pSearchPtr != NULL) && (pSettingList!= NULL) ){
			RtlCopyMemory(szTemp, pStartPtr, (int)((char*)pSearchPtr - (char*)pStartPtr));
			szTemp[pSearchPtr - pStartPtr] = '\0';
			switch(nSearchCount){
				case 1:
					// リストをゼロクリアする
					RtlZeroMemory(pSettingList, sizeof(DETECT_SETTING_LIST));
					// プロセス名を格納する。
					pSettingList->ProcessName = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, (strlen(szTemp)+ 1) * sizeof(WCHAR), 'TSET');
					if( pSettingList->ProcessName != NULL){
						RtlZeroMemory(pSettingList->ProcessName, (strlen(szTemp)+ 1) * sizeof(WCHAR));
						RtlMultiByteToUnicodeN( wcsTemp, _MAX_PATH*sizeof(WCHAR), NULL, szTemp, strlen(szTemp)+1);
						wcsTemp[strlen(szTemp)] =0;
						UTIL_UnicodeStringUpper(wcsTemp);
						RtlStringCchCopyW(pSettingList->ProcessName, (strlen(szTemp)+ 1) * sizeof(WCHAR), wcsTemp);	
					}
					break;
					
				case 2:
					// プロセス属性を格納する
					pSettingList->ProcessAttributes = atoi(szTemp);
					break;
					
				case 3:
				case 4:
					// ファイル名を格納する
					if(strlen(szTemp) != 0){
						pSettingList->FullPath.Buffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, (strlen(szTemp)+ 1) * sizeof(WCHAR),'TSET');
						if( pSettingList->FullPath.Buffer != NULL){
							RtlZeroMemory( pSettingList->FullPath.Buffer,  (strlen(szTemp)+ 1) * sizeof(WCHAR));
							RtlMultiByteToUnicodeN( wcsTemp, _MAX_PATH*sizeof(WCHAR), NULL, szTemp, strlen(szTemp)+1);
							wcsTemp[strlen(szTemp)] =0;
							UTIL_UnicodeStringUpper(wcsTemp);
							pSettingList->FullPath.Length = 0;
							pSettingList->FullPath.MaximumLength = (strlen(szTemp)+ 1) * sizeof(WCHAR);
							RtlAppendUnicodeToString( &pSettingList->FullPath, wcsTemp);
							//RtlCopyUnicodeString( &pSettingList->FullPath, wcsTemp);
						}
					}
					if(nSearchCount == 4){
						szSearch[0] = ';';
					}
					break;
					
				case 5:
					// アクセス属性を格納する
					pSettingList->AccessAttributes = atoi(szTemp);
					
					// リストに情報を追加する
					KeAcquireSpinLock(&gDetectListLick, &oldIrql);
					InsertHeadList(&gDetectList.ListEntry, &pSettingList->ListEntry);
		            KeReleaseSpinLock( &gDetectListLick, oldIrql );
		            
		            // オフセットを設定する
		            *pulReadOffset = pSearchPtr - (PCHAR)pReadBuffer;
		            // 検索カウントを0にする
					nSearchCount = 0;
					// 検索文字を「,」にする
					szSearch[0] = ',';
					// 次のリスト分メモリを確保する
					pSettingList = ExAllocatePoolWithTag(NonPagedPool, sizeof(DETECT_SETTING_LIST), 'TSIL');
					break;
				default:
					break;
			}
			nSearchCount++;
			pSearchPtr++;
			// 先頭のスペース、タブ、改行は無視する
			while( *pSearchPtr == ' ' || *pSearchPtr == '\t' || *pSearchPtr == '\n' || *pSearchPtr == '\r' ){
				pSearchPtr++;
			}
			pStartPtr = pSearchPtr;
			pSearchPtr = strchr((char*)pSearchPtr, szSearch[0]);				
		}
		
		// 最後の文字列が存在するかどうかか確認する
		szSearch[0] = ';';
		if( pSearchPtr == NULL){
			// 最後の文字列の場合、前回のポインタを使用する
			pSearchPtr = pStartPtr;
		}
		pSearchPtr = strchr((char*)pSearchPtr, szSearch[0]);
		if(pSearchPtr != NULL){
			// 最後の文字列発見
			*pulReadOffset = 0;
		}
		else {
			// 最後の文字列無いためそのまま進む。リスト用のメモリは解放する。
			if(pSettingList != NULL){
				ExFreePool(pSettingList);
			}
		}
	}
	else{
		status = STATUS_INVALID_PARAMETER;
	}
	
	return status;
}

NTSTATUS GetAccessCondition(
			PFILE_ACCESS_DATA	pFileAccessData)
{
	NTSTATUS					status;
	PDETECT_SETTING_LIST	pSettingList;
	
	
	// 引数は有効？
	if( pFileAccessData != NULL){
		status = STATUS_SUCCESS;
		pSettingList = (PDETECT_SETTING_LIST)gDetectList.ListEntry.Flink;
		
		// 次のリストが存在する間、ループする
		while((pSettingList != NULL) && (pSettingList != &gDetectList)){
		
			// プロセス名が一致する？(完全一致)
			if(wcslen(pSettingList->ProcessName) == wcslen(pFileAccessData->ProcessName)){
				if(RtlCompareMemory( pSettingList->ProcessName, pFileAccessData->ProcessName, wcslen(pFileAccessData->ProcessName) * sizeof(WCHAR) + sizeof(WCHAR))
						==  wcslen(pFileAccessData->ProcessName) * sizeof(WCHAR) + sizeof(WCHAR)){
			
					// プロセス名一致
					// ファイル名が一致する？(前方一致)
					if( RtlCompareMemory( pSettingList->FullPath.Buffer, pFileAccessData->FullPath.Buffer, pSettingList->FullPath.Length)
						==  pSettingList->FullPath.Length){
			
						// ファイル名が一致する。
						// リストの情報を対象ファイルアクセスデータに格納する
						pFileAccessData->AccessAttributes = pSettingList->AccessAttributes;
						pFileAccessData->ProcessAttributes = pSettingList->ProcessAttributes;
						break;
					}
					else{
						// ファイル名が一致しない
					}
				}
				else{
					// プロセス名が一致しない
				}
			}
			else {
				// プロセス名長が一致しない
			}
			pSettingList = (PDETECT_SETTING_LIST)pSettingList->ListEntry.Flink;
		}
	}
	else{
		// 引数が無効
		status = STATUS_INVALID_PARAMETER;
	}

	return status;
}

BOOLEAN IsZeroAttack(
			PFILE_ACCESS_DATA	pFileAccessData,
			ULONG				ulAccessType)
{
	BOOLEAN		bIsZeroAttack = FALSE;
	PDETECT_SETTING_LIST	pSettingList;
	
	
	// 引数は有効？
	if( pFileAccessData != NULL){
		//　ファイルのアクセス属性
		switch( pFileAccessData->AccessAttributes){
		case DISABLE_WRITE:
			if(ulAccessType == WRITE_OPERATION){
				// 書き込み要求
				bIsZeroAttack = TRUE;
			}
			break;
		case DISABLE_READ:
			bIsZeroAttack = TRUE;
			break;
		default:
			break;
		}
	}
	else {
		// 引数無効。何もしない
	}

	return bIsZeroAttack;
}

NTSTATUS	NotifyAttackData(
			PFLT_CALLBACK_DATA	Data,
			PFILE_ACCESS_DATA	pFileAccessData,
			ULONG				ulAccessType)
{
	NTSTATUS		status;
	PCHAR			pWriteData;
	ULONG			ulAllocSize;
	UNICODE_STRING	FileName;
	
	// 引数は有効？
	if( Data != NULL && pFileAccessData != NULL){
		// 書き込むデータを保持するメモリを確保
		ulAllocSize = sizeof(FILE_ACCESS_DATA) + TIME_SIZE + wcslen(pFileAccessData->ProcessName) + pFileAccessData->FullPath.MaximumLength;
		pWriteData = ExAllocatePoolWithTag( NonPagedPool, ulAllocSize , 'ETIR');
		if(pWriteData != NULL){
			// 書き込むデータの構築
			RtlZeroMemory(pWriteData, ulAllocSize);
			status = ConstructWriteAttackData( pWriteData, pFileAccessData, ulAccessType);
			RtlInitUnicodeString( &FileName, L"\\DosDevices\\C:\\test\\DetectZAttack.txt");
			// データの書き込み
			RtlStringCbLengthA( pWriteData, ulAllocSize, &ulAllocSize);
			status = WriteFileData( FileName, &ulAllocSize, pWriteData);
			ExFreePool(pWriteData);
		}
		else{
			status = STATUS_INSUFFICIENT_RESOURCES;
		}

//		DbgPrint("Detected Zeroday Attack:%wZ FileName=%wZ, ProcessName=%ws, FileAttributes=%8.8lx, FileAccess=%8.8lx\n", 
//				pFileAccessData->FullPath, pFileAccessData->ProcessName, pFileAccessData->ProcessAttributes, ulAccessType);
		
	}
	else {
		// 引数は無効
		status = STATUS_INVALID_PARAMETER;
	}
	
	return status;
}

NTSTATUS	ConstructWriteAttackData(
			PCHAR				pWriteData,
			PFILE_ACCESS_DATA	pFileAccessData,
			ULONG				ulAccessType)
{
	NTSTATUS	status = STATUS_UNSUCCESSFUL;
	ULONG		ulAllocSize;
	PCHAR		pTemp;
	
	KdPrint(("DetectZ : NotifyAttackData Enter\n"));
	
	// 引数は有効？
	if( pWriteData != NULL && pFileAccessData != NULL){
		// Passiveレベル？
		if(KeGetCurrentIrql() == PASSIVE_LEVEL){
			// 一時メモリ確保
			ulAllocSize = max( wcslen(pFileAccessData->ProcessName) * sizeof(WCHAR), pFileAccessData->FullPath.Length);
			pTemp = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, ulAllocSize, 'PMET');
			if(pTemp != NULL){
				ulAllocSize = sizeof(FILE_ACCESS_DATA) + TIME_SIZE + wcslen(pFileAccessData->ProcessName) + pFileAccessData->FullPath.MaximumLength;
				// 書き込むデータ生成
				// 日時取得
				status = GetCurrentTime(pWriteData);
				RtlStringCchCatA( pWriteData, ulAllocSize, ",");
				// アクセス内容
				sprintf( pTemp, "%d,", ulAccessType);
				RtlStringCchCatA( pWriteData, ulAllocSize, pTemp);
				// プロセス名
				wcstombs(pTemp, pFileAccessData->ProcessName, wcslen(pFileAccessData->ProcessName) * sizeof(WCHAR));
				RtlStringCchCatA( pWriteData, ulAllocSize, pTemp);
				RtlStringCchCatA( pWriteData, ulAllocSize, ",");
				// プロセスのアクセス属性
				sprintf( pTemp, "%d,", pFileAccessData->ProcessAttributes);
				RtlStringCchCatA( pWriteData, ulAllocSize, pTemp);
				// ファイル名
				wcstombs(pTemp, pFileAccessData->FullPath.Buffer, pFileAccessData->FullPath.Length);
				RtlStringCchCatA( pWriteData, ulAllocSize, pTemp);
				RtlStringCchCatA( pWriteData, ulAllocSize, ",");
				// ファイルの属性
				sprintf( pTemp, "%d\r\n", pFileAccessData->AccessAttributes);
				RtlStringCchCatA( pWriteData, ulAllocSize, pTemp);
				
				ExFreePool(pTemp);
			}
			else {
				// 一時メモリ確保失敗
				status = STATUS_INSUFFICIENT_RESOURCES;
			}
		}
	}
	else {
		// 引数は無効
		status = STATUS_INVALID_PARAMETER;
	}
	
	KdPrint(("DetectZ : NotifyAttackData Exit\n"));
	
	return status;
}

NTSTATUS GetCurrentTime(
		PCHAR	pTime)
{
	NTSTATUS		status = STATUS_SUCCESS;
	LARGE_INTEGER	Time;
    TIME_FIELDS 	TimeFields;

	KdPrint(("DetectZ : NotifyAttackData Exit\n"));
	// 引数は有効？
	if(pTime != NULL){
		// 現在の時刻取得
		KeQuerySystemTime(&Time);
		ExSystemTimeToLocalTime(&Time, &Time);
 	    
 	    // TimeFieldに変換
 	    (VOID)RtlTimeToTimeFields( &Time, &TimeFields );

		sprintf(pTime,"%4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d.%3.3d", 
			TimeFields.Year, TimeFields.Month, TimeFields.Day, TimeFields.Hour, TimeFields.Minute, TimeFields.Second, TimeFields.Milliseconds);
	}
	else {
		// 引数は無効
		status = STATUS_INVALID_PARAMETER;
	}

	KdPrint(("DetectZ : NotifyAttackData Exit\n"));
	
	return status;
}

NTSTATUS	WriteFileData(
	UNICODE_STRING		FileName,
	PULONG				pulWriteSize,
	PVOID				pWriteBuffer)
{
	NTSTATUS			status;
	HANDLE				hFile;
	OBJECT_ATTRIBUTES	ObjectAttributes;
	IO_STATUS_BLOCK		IoStatusBlock;
	LARGE_INTEGER 		offset;	
	
	KdPrint(("DetectZ : WriteFileData Enter\n"));
	status = STATUS_SUCCESS;

	// 引数は有効？
	if( FileName.Buffer != NULL && pulWriteSize != NULL && pWriteBuffer != NULL){
		// Passiveレベル？
		if(KeGetCurrentIrql() == PASSIVE_LEVEL){
			status = STATUS_SUCCESS;
		}
		else{
			// 不正なIRQL
			status = STATUS_UNSUCCESSFUL;
		}
	}
	else {
		// 引数が無効
		status = STATUS_INVALID_PARAMETER;
	}
	
	if( NT_SUCCESS(status)){
		
		InitializeObjectAttributes(
					&ObjectAttributes,
					&FileName,
					OBJ_CASE_INSENSITIVE,
					NULL,
					NULL);
		
		status = FltCreateFile(
					gFilterHandle,		// Filter
					NULL,				// Instance OPTIONAL
					&hFile,				// FileHandle
					FILE_APPEND_DATA,	// DesiredAccess
					&ObjectAttributes,	// ObjectAttributes
					&IoStatusBlock,		// IoStatusBlock
					NULL,				// AllocationSize OPTIONAL
					0L,					// FileAttributes
					FILE_SHARE_WRITE,	// ShareAccess
					FILE_OPEN_IF,		// CreateDisposition
					//FILE_NON_DIRECTORY_FILE, // CreateOptions
					FILE_SYNCHRONOUS_IO_NONALERT, // CreateOptions
					NULL,				// EaBuffer OPTIONAL
					0L,					// EaLength
					0					// Flags
					);
		if(NT_SUCCESS(status)){
			
			offset.HighPart = -1;
			offset.LowPart = FILE_WRITE_TO_END_OF_FILE;

			// ファイルを書き込む
			status = ZwWriteFile( 
						hFile,
						NULL,
						NULL,
						NULL,
						&IoStatusBlock,
						pWriteBuffer,
						*pulWriteSize,
						&offset,
						NULL);
			FltClose(hFile);
		}
	}
	   	
	KdPrint(("DetectZ : WriteFileData Exit\n"));

	return status;
}
