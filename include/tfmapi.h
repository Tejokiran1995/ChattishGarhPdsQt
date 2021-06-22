
/** 
* @file iaapi.h
*
* @brief Image Access library.
*
* Copyright (c) UPEK 2008
*
*/

#ifndef IAAPI_H
#define IAAPI_H

#include "tfmtypes.h"
#if !defined(IAAPI_DLL)
	#if defined(BUILD_IAAPI_DLL)
		#if defined(_MSC_VER) 
            #if defined(IAAPI_STATIC)
                #if defined __cplusplus 
                    #define IAAPI_DLL extern "C" 
                #else
                    #define IAAPI_DLL
                #endif
            #else
                #if defined __cplusplus 
                    #define IAAPI_DLL extern "C"
                #else
                    #define IAAPI_DLL extern
                #endif
            #endif
		#else
			#define IAAPI_DLL
		#endif
	#else
		#if defined(_MSC_VER)
            #if defined(IAAPI_STATIC)
                #if defined __cplusplus 
                    #define IAAPI_DLL extern "C"
                #else
                    #define IAAPI_DLL
                #endif
            #else    
    			#if defined __cplusplus 
	    			#define IAAPI_DLL extern "C" __declspec(dllimport)
		    	#else
			    	#define IAAPI_DLL __declspec(dllimport)
			    #endif
            #endif
		#else
			#if defined __cplusplus
				#define IAAPI_DLL extern "C"
			#else
				#define IAAPI_DLL
            		#endif
		#endif
	#endif
#endif


/*--------------------------------------------------------------------------------------------------*/


/** 
 * @name Application General Functions
 * The Application General Functions allow to initialize the PTAPI library, create 
 * and close logical connections to FM, set callbacks and perform other general 
 * operations.
 */

/** 
 * Initialize the PTAPI library. Must be called before any other function.
 * 
 * @param pMemoryFuncs Struct of pointers to the memory allocation and unallocation routines.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTInitialize(
    IN PT_MEMORY_FUNCS *pMemoryFuncs
);


/** 
 * Initialize the PTAPI library with possibility to set some additional flags which can modify
 * behavior of the library. This function may not be available on some platforms.
 * 
 * @param pMemoryFuncs Struct of pointers to the memory allocation and unallocation routines.
 * @param dwFlags Additional flags to modify behavior of the PTAPI library, see PT_INITEX_FLAG_xxxx values.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTInitializeEx(
    IN PT_MEMORY_FUNCS *pMemoryFuncs,
    IN PT_DWORD dwFlags
);


/** 
 * Uninitialize the PTAPI library. Must not be called while any connection
 * is still open. It is not obligatory to call this function, if the PTAPI library should
 * be kept initialized until the program exits.
 * 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTTerminate(
      void
);


/** 
 * Return information about the PTAPI library version and other global
 * information, independent on any connection.
 * 
 * @param ppGlobalInfo Address of a pointer which will be set to point to a global info block.
 * The global info block is dynamically allocated by the API library and must be
 * freed by the application by a call to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGlobalInfo(
    OUT PT_GLOBAL_INFO **ppGlobalInfo
);


/** 
 * Check for device configuration change (device inserting or removing). Function will block until
 * there is some change in device configuration, defined timeout elapses or function is canceled by
 * call of PTDevNotifyInterrupt().
 * 
 * @param hNotify Notification handle, currently must be used PT_DEV_NOTIFY_GLOBAL_HANDLE.
 * @param pdwStamp Stamp for marking current state of device configuration. For the first call it 
 * has to be set to 0, for each other call it must contain the value returned by previous call of 
 * PTDevNotifyCheck().
 * @param lTimeout Timeout in milliseconds. If value 0 is used then function will return immediately, 
 * value -1 means infinite timeout.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTDevNotifyCheck(
    IN PT_DEVNOTIFY hNotify,
    INOUT PT_DWORD *pdwStamp,
    IN PT_LONG lTimeout
);


/** 
 * Force currently running function PTDevNotifyCheck() to return. Also all following
 * calls of PTDevNotifyCheck() will be canceled until PTDevNotifyInterrupt() is called
 * with flag PT_DEVNOTIFY_FLAG_CLEAR_INTERRUPT to clear the interruption state.
 * 
 * @param hNotify Notification handle, currently must be used PT_DEV_NOTIFY_GLOBAL_HANDLE.
 * @param dwFlags Additional flags (see PT_DEVNOTIFY_FLAG_XXXX values). Use flag
 * PT_DEVNOTIFY_FLAG_CLEAR_INTERRUPT to clear the interruption state.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTDevNotifyInterrupt(
    IN PT_DEVNOTIFY hNotify,
    IN PT_DWORD dwFlags
);


/** 
 * Open a new connection to a FM connected to this host.
 * 
 * @param pszDsn Zero-terminated ASCII string describing the FM connection parameters,
 * e.g. port number etc.
 * Pointer to connection parameters.
 * @param phConnection Resulting connection handle. At the end of the connection it should
 * be closed using PTClose.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTOpen(
    IN PT_CHAR *pszDsn,
    OUT PT_CONNECTION *phConnection
);


/** 
 * Close a connection previously opened by PTOpen().
 * 
 * @param hConnection Connection handle of the connection to be closed.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTClose(
    IN PT_CONNECTION hConnection
);


/** 
 * Open and prepare connection interface with FM
 * 
 * @param pszDsn Zero-terminated ASCII string describing the FM connection parameters, 
 * e.g. port number etc. 
 * @param phConnection Resulting interface connection handle. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTOpenDevice(
    IN PT_CHAR *pszDsn,
    OUT PT_CONNECTION *phConnection
);


/** 
 * Open communication session with FM.
 * 
 * @param hConnection Handle to connection interface previously opened by PTOpenDevice().
 * @param pszDsn Currently not used, set it to NULL.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTOpenSession(
    IN PT_CONNECTION hConnection,
    IN PT_CHAR *pszDsn
);


/** 
 * Close communication session previously opened by PTOpenSession().
 * 
 * @param hConnection Connection handle of the session to be closed.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTCloseSession(
    IN PT_CONNECTION hConnection
);


/** 
 * Close connection interface.
 * 
 * @param hConnection Connection handle of the interface to be closed.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTCloseDevice(
    IN PT_CONNECTION hConnection
);


/** 
 * Enumerate currently connected fingerprint devices.
 * 
 * @param pszEnumDsn Zero terminated ASCII string describing fingerprint device connection interface, 
 * where enumeration should be performed. 
 * @param ppDeviceList Address of the pointer, which will be set to point to 
 * the list of found TFM/ESS devices. The data has to be 
 * freed by a call to PTFree.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTEnumerateDevices(
    IN PT_CHAR *pszEnumDsn,
    OUT PT_DEVICE_LIST **ppDeviceList
);


/** 
 * Open connection to a remote instance of PTAPI library.
 * 
 * @param pfnCommCallback Application-provided callback used to pass data packet to/from the remote library instance.
 * @param pCommCallbackCtx A general value passed to the callback and remembered for given connection handle. From now 
 * on this value will be used in all callbacks generated by calls using given connection handle.
 * @param phConnection Resulting connection handle. At the end of the
 * connection it should be closed using PTClose.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTOpenRemote(
    IN PT_COMM_CALLBACK pfnCommCallback,
    IN void *pCommCallbackCtx,
    OUT PT_CONNECTION *phConnection
);


/** 
 * Send the request data packet to the FM, wait for a response 
 * and get the response data packet.
 * 
 * @param hConnection Handle to the opened connection to FM.
 * @param pInPkt Packet, which has to be submitted to FM.
 * @param ppOutPkt Address of pointer which will be set to the response 
 * packet data. The data has to be discarded by a call 
 * to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTTransact(
    IN PT_CONNECTION hConnection,
    IN PT_DATA *pInPkt,
    OUT PT_DATA **ppOutPkt
);


/** 
 * Sets the address of the callback routine to be called if any called function 
 * involves displaying a biometric user interface. The callback functionality is 
 * described below.
 * 
 * @param hConnection Connection handle.
 * @param pfnGuiStreamingCallback Pointer to an application callback to deal with 
 * the presentation of biometric streaming data. 
 * Reserved for future used, currently not 
 * implemented. Use NULL for this parameter. 
 * @param pGuiStreamingCallbackCtx Generic pointer to context information 
 * provided by the application that will be 
 * presented on the callback. Reserved for future 
 * used, currently not implemented. Use NULL 
 * for this parameter. 
 * @param pfnGuiStateCallback Pointer to an application callback to deal with 
 * GUI state changes. See description of 
 * PT_GUI_STATE_CALLBACK. 
 * @param pGuiStateCallbackCtx Generic pointer to context information 
 * provided by the application that will be 
 * presented to the callback. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetGUICallbacks(
    IN PT_CONNECTION hConnection,
    IN PT_GUI_STREAMING_CALLBACK pfnGuiStreamingCallback,
    IN void *pGuiStreamingCallbackCtx,
    IN PT_GUI_STATE_CALLBACK pfnGuiStateCallback,
    IN void *pGuiStateCallbackCtx
);


/** 
 * Standard PTAPI GUI callback. This callback is default, until
 * other callback is specified by PTSetGUICallbacks
 * 
 * @param pGuiStateCallbackCtx A generic pointer to context information that was provided by the original requester.
 * @param dwGuiState A bitmask indicating the current GUI state plus an indication
 * of what others parameters are available.
 * @param pbyResponse The response from the application back to the PTAPI on return from the callback.
 * @param dwMessage The number of a message to display to the user.
 * @param byProgress A value that indicates the progress of performed biometric operation.
 * @param pSampleBuffer The current sample buffer for the application to display.
 * @param pData Optional data, which may be available for some GUI message codes.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PT_STD_GUI_STATE_CALLBACK(
    IN void *pGuiStateCallbackCtx,
    IN PT_DWORD dwGuiState,
    OUT PT_BYTE *pbyResponse,
    IN PT_DWORD dwMessage,
    IN PT_BYTE byProgress,
    IN void *pSampleBuffer,
    IN PT_DATA *pData
);


/** 
 * Cancel currently running PTAPI command. Also all following
 * commands will be canceled until PTCancel() is called with flag
 * PT_CANCEL_FLAG_CLEAR_CANCEL to clear the cancellation state.
 * 
 * @param hConnection Connection handle to a session on which the cancel has to be performed.
 * @param dwFlags Additional flags. (see PT_CANCEL_FLAG_XXXX values). Use flag
 * PT_CANCEL_FLAG_CLEAR_CANCEL to clear the cancellation state.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTCancel(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwFlags
);


/** 
 * Frees memory block using deallocation function passed to PTAPI by 
 * PTInitialize() call. This function may be used for releasing structures 
 * allocated by other PTAPI functions.
 * 
 * @param Memblock Address of a memory block to be released. If value 
 * of this parameter is NULL then no action is taken.
 */
PTAPI_DLL void PTAPI PTFree(
    IN void *Memblock
);


/** 
 * Perform specified escape operation. The set of available operations and format of input/output parameters
 * are defined separately for each platform. This function may not be available on some platforms.
 * 
 * @param dwOperation Code of operation to be executed.
 * @param pInData Block of data with input parameters for given operation. Can be NULL for operations, which have no input parameters.
 * @param ppOutData Address of a pointer which will be set to point to a block of data with output parameters of given operation.
 * The data block must be released by call of function PTFree(). Can be NULL for operations, which have no output parameters.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTEscape(
    IN PT_DWORD dwOperation,
    IN PT_DATA *pInData,
    OUT PT_DATA **ppOutData
);




/** 
 * @name Biometric Functions
 * This section has been strongly inspired by the BioAPI standard. Many function calls are
 * practically the same as in BioAPI, except for different naming of the parameter types.
 * BioAPI is using its own type definitions, too specialized to be used in PTAPI. Also some
 * parameters were simplified and some handles replaced by pointers to binary data blocks.
 */

/** 
 * Scan the live finger and return the scanned image.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param byType Requested type of the returned data. The 
 * data may be compressed, subsampled, etc. 
 * Use one of the PT_GRAB_TYPE_xxxx values.
 * Value of this parameter must be always set to PT_GRAB_TYPE_IMGFORMAT (255) in PTAPI Lite.
 * @param lTimeout Timeout in milliseconds. "-1" means default timeout.
 * "-2" means infinite timeout (ESS 2.2, TCD50 and SONLY only).
 * Timeout does not include the time needed for image processing
 *  and transfer to the host.
 * 
 * Area sensors: The timeout is used only if 
 * boWaitForAcceptableFinger = PT_TRUE. It 
 * means the max. time PTGrab waits for an 
 * acceptable finger. 
 * 
 * Swipe sensors: The timeout is the max. time 
 * PTGrab waits for the user to start finger 
 * swipe. Once the swipe starts, it will not be 
 * interrupted even if the timeout expires.
 * @param boWaitForAcceptableFinger If PT_TRUE, the function verifies the quality of 
 * the finger image and returns only when the 
 * finger quality would be acceptable for other 
 * biometric functions. 
 * 
 * Area sensors: If PT_FALSE, the function returns 
 * immediately without verifying image quality. 
 * 
 * Swipe sensors: If PT_TRUE, the function may 
 * require multiple swipes of a finger until an 
 * acceptable image is obtained; user is 
 * prompted by GUI callbacks. If PT_FALSE, the 
 * function always returns after a single swipe. If 
 * the quality of this swipe is so low that no 
 * image can be reconstructed, the function 
 * returns with the error code 
 * PT_STATUS_SWIPE_TOO_BAD. The exact 
 * cause of refusing the swipe will be 
 * communicated via GUI callbacks.
 * @param ppGrabbedData Address of the pointer, which will be set to 
 * point to the resulting grabbed data. The data 
 * has to be discarded by a call to PTFree().
 * @param pSignData TFM, ESS 2.1: Reserved, use NULL. 
 * 
 * ESS 2.2, TCD50, SONLY: Pointer to a data block 
 * used for signature. If it is NULL then no 
 * signature is created. 
 * This parameter is ignored in PTAPI Lite.
 * @param ppSignature TFM, ESS 2.1: Reserved, use NULL. 
 * 
 * ESS 2.2, TCD50, SONLY: Address of the pointer, 
 * which will be set to point to the resulting 
 * signature data. If this parameter is NULL then 
 * no signature is returned. The data has to be 
 * discarded by a call to PTFree(). 
 * This parameter is ignored in PTAPI Lite.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGrab(
    IN PT_CONNECTION hConnection,
    IN PT_BYTE byType,
    IN PT_LONG lTimeout,
    IN PT_BOOL boWaitForAcceptableFinger,
    OUT PT_DATA **ppGrabbedData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Switches FM to the standby (finger detect) mode. If a finger is detected, the
 * FM wakes up and immediately starts an image grabbing operation. This
 * function is especially useful for low-power image grabbing.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pfnCallback Callback function, called periodically all the
 * time FM is sleeping. Optional, can be NULL.
 * See the description of PT_IDLE_CALLBACK.
 * @param pIdleCallbackCtx A generic pointer to callback's context data. This pointer
 * will be supplied to the callback function every time it is called.
 * @param byGrabType Requested type of the returned data. The 
 * data may be compressed, subsampled, etc. 
 * Use one of the PT_GRAB_TYPE_xxxx values.
 * @param lTimeout Timeout in milliseconds. "-1" means default 
 * timeout. "-2" means infinite timeout (ESS2.2/SONLY/TCD50 only).
 * @param boAssessImageQuality If PT_TRUE, the function will after a 
 * successful grab perform also image quality 
 * analyses. If there is any problem with the 
 * image quality, it will be reported via the 
 * pdwGrabGuiMessage. This parameter controls 
 * the same image quality checks, which are 
 * controlled in PTGrab by 
 * boWaitForAcceptableFinger. 
 * @param pdwWakeupCause The cause of wakeup. Currently the following
 * causes are possible: 
 * PT_WAKEUP_CAUSE_HOST (0) (signal 
 * from the Host) or 
 * PT_WAKEUP_CAUSE_FINGER (1) (a 
 * finger was detected) 
 * @param pdwGrabGuiMessage Assessment of the quality of the grab result. 
 * It is used to indicate problems like too fast, 
 * too skewed or too short finger swipe, or 
 * image quality problems (e.g. too right, too 
 * left etc.). The codes returned are the same as 
 * the GUI message codes in GUI callbacks 
 * (PT_GUIMSG_xxxx). Good image is indicated 
 * by a code PT_GUIMSG_GOOD_IMAGE. 
 * pdwGrabGuiMessage is valid only if 
 * *pdwWakeupCause is 
 * PT_WAKEUP_CAUSE_FINGER. 
 * @param ppGrabbedData Address of the pointer, which will be set to 
 * point to the resulting grabbed data. The data 
 * has to be discarded by a call to PTFree(). 
 * Valid only if *pdwWakeupCause is 
 * PT_WAKEUP_CAUSE_FINGER. 
 * @param pSignData ESS2.1: Reserved, use NULL. 
 * 
 * ESS2.2, SONLY, TCD50: Pointer to a data block 
 * used for signature. If it is NULL then no 
 * signature is created. 
 * @param ppSignature ESS2.1: Reserved, use NULL. 
 * 
 * ESS2.2, SONLY, TCD50: Address of the pointer, 
 * which will be set to point to the resulting 
 * signature data. If this parameter is NULL then 
 * no signature is returned. The data has to be 
 * discarded by a call to PTFree(). 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSleepThenGrab(
    IN PT_CONNECTION hConnection,
    IN PT_IDLE_CALLBACK pfnCallback,
    IN void *pIdleCallbackCtx,
    IN PT_BYTE byGrabType,
    IN PT_LONG lTimeout,
    IN PT_BOOL boAssessImageQuality,
    OUT PT_DWORD *pdwWakeupCause,
    OUT PT_DWORD *pdwGrabGuiMessage,
    OUT PT_DATA **ppGrabbedData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Scan the live finger and return the scanned image. Only a defined window
 * on the sensor will be returned.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param byCompressionType Requested type of compression of the data. 
 * Currently the only defined compression type 
 * is PT_GRAB_COMPRESSION_TYPE_NONE (0) 
 * – no compression. 
 * @param lTimeout Timeout in milliseconds. "-1" means default 
 * timeout. 
 * @param boWaitForAcceptableFinger If PT_TRUE, the function verifies the quality of 
 * the finger image and returns only when the 
 * finger quality would be acceptable for other 
 * biometric functions. 
 * 
 * If PT_FALSE, the function returns immediately 
 * without verifying image quality. 
 * @param lRows Number of rows to grab. This is the 
 * number of actually grabbed rows, taking into 
 * account the subsampling step. 
 * @param lRowOffset First row to grab. 
 * @param lRowDelta Subsampling step = scan every Nth 
 * row. 1=Normal full image.
 * @param lCols Number of columns to grab. This is the 
 * number of actually grabbed columns, taking into 
 * account the subsampling step. 
 * @param lColOffset First column to grab. 
 * @param lColDelta Subsampling step = scan every Nth 
 * column. 1=Normal full image.
 * @param ppGrabbedData Address of the pointer, which will be set to 
 * point to the resulting grabbed data. The data 
 * has to be discarded by a call to PTFree(). 
 * @param pSignData Reserved, use NULL.
 * @param ppSignature Reserved, use NULL.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGrabWindow(
    IN PT_CONNECTION hConnection,
    IN PT_BYTE byCompressionType,
    IN PT_LONG lTimeout,
    IN PT_BOOL boWaitForAcceptableFinger,
    IN PT_LONG lRows,
    IN PT_LONG lRowOffset,
    IN PT_LONG lRowDelta,
    IN PT_LONG lCols,
    IN PT_LONG lColOffset,
    IN PT_LONG lColDelta,
    OUT PT_DATA **ppGrabbedData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Scan the live finger, process it into a fingerprint template and optionally 
 * return it to the caller.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param byPurpose Purpose of the capture as defined by BioAPI (see PT_PURPOSE_xxxx).
 * @param ppCapturedTemplate Address of the pointer, which will be set to 
 * point to the resulting template (BIR). The 
 * template has to be discarded by a call to 
 * PTFree(). If the template should be only 
 * remembered for use of next functions, leave 
 * this parameter NULL.
 * @param lTimeout Timeout in milliseconds. "-1" means default 
 * timeout. "-2" means infinite timeout (ESS2.2/SONLY/TCD50 only).
 * @param ppAuditData Reserved, use NULL.
 * This parameter is ignored in PTAPI Lite.
 * @param pSignData TFM, ESS 2.1: Reserved, use NULL. 
 * 
 * ESS 2.2, SONLY, TCD50: Pointer to a data block 
 * used for signature. If it is NULL then no 
 * signature is created. 
 * This parameter is ignored in PTAPI Lite.
 * @param ppSignature TFM, ESS 2.1: Reserved, use NULL. 
 * 
 * ESS 2.2, SONLY, TCD50: Address of the pointer, 
 * which will be set to point to the resulting 
 * signature data. If this parameter is NULL then 
 * no signature is returned. The data has to be 
 * discarded by a call to PTFree().
 * This parameter is ignored in PTAPI Lite.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTCapture(
    IN PT_CONNECTION hConnection,
    IN PT_BYTE byPurpose,
    OUT PT_BIR **ppCapturedTemplate,
    IN PT_LONG lTimeout,
    OUT PT_BIR **ppAuditData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Switches FM to the standby (finger detect) mode. If a finger is detected, the 
 * FM wakes up and immediately scans the finger and processes it into a 
 * template. This function is especially useful for low-power image grabbing. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pfnCallback Callback function, called periodically all the 
 * time FM is sleeping. Optional, can be NULL. 
 * See the description of PT_IDLE_CALLBACK.
 * @param pIdleCallbackCtx A generic pointer to callback's context data. 
 * This pointer will be supplied to the callback 
 * function every time it is called.
 * @param byPurpose Purpose of the capture as defined by BioAPI 
 * (see PT_PURPOSE_xxxx). Supported values: 
 * PT_PURPOSE_VERIFY (1); 
 * PT_PURPOSE_ENROLL (3); 
 * PT_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY (4)
 * @param lTimeout Timeout in milliseconds. "-1" means default 
 * timeout. "-2" means infinite timeout. 
 * @param pdwWakeupCause The cause of wakeup. Currently the following 
 * causes are possible: 
 * PT_WAKEUP_CAUSE_HOST (0) (signal from the Host) 
 * PT_WAKEUP_CAUSE_FINGER (1) (a 
 * finger was detected) 
 * @param pdwCaptureGuiMessage Assessment of the quality of the swipe. It is 
 * used to indicate problems like too fast, too 
 * skewed or too short finger swipe, or image 
 * quality problems (e.g. too right, too left etc.). 
 * The codes returned are the same as the GUI 
 * message codes in GUI callbacks 
 * (PT_GUIMSG_xxxx). Good image is indicated 
 * by a code PT_GUIMSG_GOOD_IMAGE. 
 * 
 *  pdwCaptureGuiMessage is valid only if 
 * *pdwWakeupCause is PT_WAKEUP_CAUSE_FINGER.
 * @param ppCapturedTemplate Address of the pointer, which will be set to 
 * point to the resulting template (BIR). The 
 * template has to be discarded by a call to 
 * PTFree(). Returned data are valid only if 
 * *pdwWakeupCause is PT_WAKEUP_CAUSE_FINGER. If the template 
 * should be only remembered for use of next 
 * functions, leave this parameter NULL. 
 * @param ppAuditData Reserved, use NULL.
 * @param pSignData Pointer to a data block used for signature. 
 * If it is NULL then no signature is created.
 * @param ppSignature Address of the pointer, 
 * which will be set to point to the resulting 
 * signature data. If this parameter is NULL then 
 * no signature is returned. The data has to be 
 * discarded by a call to PTPTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSleepThenCapture(
    IN PT_CONNECTION hConnection,
    IN PT_IDLE_CALLBACK pfnCallback,
    IN void *pIdleCallbackCtx,
    IN PT_BYTE byPurpose,
    IN PT_LONG lTimeout,
    OUT PT_DWORD *pdwWakeupCause,
    OUT PT_DWORD *pdwCaptureGuiMessage,
    OUT PT_BIR **ppCapturedTemplate,
    OUT PT_BIR **ppAuditData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Scan the live finger once or several times, depending on the session settings, 
 * and combine the images into one enrollment template. The enrollment template can be 
 * henceforward used as the biometric ID of given person.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param byPurpose Purpose of the enrollment as defined by BioAPI (see PT_PURPOSE_xxxx). 
 * @param pStoredTemplate Reserved, use NULL.
 * This parameter is ignored in PTAPI Lite.
 * @param ppNewTemplate Address of the pointer, which will be set to point to the resulting 
 * template (BIR). The template is allocated by the PTAPI library and has 
 * to be discarded by a call to PTFree().
 * If the template should be stored only in FM's non-volatile memory, 
 * leave this parameter NULL.
 * If the template should be only remembered for use of next functions,
 * leave this parameter NULL.
 * @param plSlotNr 
	    Used, when the resulting template should be stored in FM's template database. 
	    It is a pointer to variable, where will be stored the slot number (i.e. an ID) 
	    of the template stored in FM. The slot number is assigned by FM. If the template 
	    should not be stored in FM, leave this parameter NULL.
      
 * This parameter is ignored in PTAPI Lite.
 * @param pPayload Data to be embedded into the resulting template. Payload data is an output parameter 
 * from PTVerify and its variants when successful match is achieved. 
 * @param lTimeout Timeout in milliseconds. "-1" means default timeout.
 * "-2" means infinite timeout (ESS2.2/SONLY/TCD50 only).
 * @param ppAuditData Reserved, use NULL.
 * This parameter is ignored in PTAPI Lite.
 * @param pSignData TFM, ESS 2.1: Reserved, use NULL.
 * 
 * ESS 2.2, SONLY, TCD50: Pointer to a data block used for signature. If it is NULL then no signature is created.
 * This parameter is ignored in PTAPI Lite.
 * @param ppSignature TFM, ESS 2.1: Reserved, use NULL.
 * 
 * ESS 2.2, SONLY, TCD50: Address of the pointer, which will be set to point 
 * to the resulting signature data. If this parameter is NULL then no signature is returned. 
 * The data has to be discarded by a call to PTFree 
 * This parameter is ignored in PTAPI Lite.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTEnroll(
    IN PT_CONNECTION hConnection,
    IN PT_BYTE byPurpose,
    IN PT_INPUT_BIR *pStoredTemplate,
    OUT PT_BIR **ppNewTemplate,
    OUT PT_LONG *plSlotNr,
    IN PT_DATA *pPayload,
    IN PT_LONG lTimeout,
    OUT PT_BIR **ppAuditData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Match the supplied captured verification template against the supplied
 * enrollment template.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param plMaxFARRequested Pointer to max. FAR (False Accept Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FAR value N/(2^31-1) – the 
 * larger the value, the worse the result. 
 * 
 * Currently this parameter is ignored and FM is 
 * using a different mechanism to control the 
 * exactness of matching – see 
 * PTSetSessionCfgEx, PT_SESSION_CFG item 
 * SecuritySetting.
 * @param plMaxFRRRequested Pointer to max. FRR (False Reject Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FRR value N/(2^31-1) – the 
 * large the value, the worse the result. 
 * 
 * This parameter is currently ignored.
 * @param pboFARPrecedence If both FAR and FRR are provided, this 
 * parameter decides which of them takes 
 * precedence.
 * 
 * PT_TRUE->FAR, PT_FALSE->FRR. 
 * 
 * This parameter is currently ignored. 
 * @param pCapturedTemplate The template to verify – BIR data or slot ID of 
 * one of the templates stored in NVM. If NULL, 
 * the Last Good Template (result of the latest 
 * interactive biometric function) will be used. 
 * 
 * PT_TRUE->FAR, PT_FALSE->FRR.
 * 
 * This parameter is currently ignored.
 * @param pStoredTemplate The enrollment template to be verified against 
 * – BIR data or slot ID of one of the templates 
 * stored in the template database. The Last 
 * Good Template cannot be used here.
 * @param ppAdaptedTemplate Reserved, use NULL.
 * @param pboResult The result: Match (PT_TRUE)/No match 
 * (PT_FALSE).
 * @param plFARAchieved The value of FAR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL.
 * @param plFRRAchieved The value of FRR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL. 
 * @param ppPayload Address of the pointer, which will be set to 
 * point to the payload data, originally 
 * embedded in the StoredTemplate. Payload 
 * data is available only when successful match 
 * is achieved.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTVerifyMatch(
    IN PT_CONNECTION hConnection,
    IN PT_LONG *plMaxFARRequested,
    IN PT_LONG *plMaxFRRRequested,
    IN PT_BOOL *pboFARPrecedence,
    IN PT_INPUT_BIR *pCapturedTemplate,
    IN PT_INPUT_BIR *pStoredTemplate,
    OUT PT_BIR **ppAdaptedTemplate,
    OUT PT_BOOL *pboResult,
    OUT PT_LONG *plFARAchieved,
    OUT PT_LONG *plFRRAchieved,
    OUT PT_DATA **ppPayload
);


/** 
 * Match the supplied captured template against the supplied enrollment 
 * template. Unlike PTVerifyMatch this function is completely performed on 
 * host without communication with FM.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param plMaxFARRequested Pointer to max. FAR (False Accept Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FAR value N/(2^31-1) – the
 * large the value, the worse the result. 
 * 
 * Currently this parameter is ignored and FM is 
 * using a different mechanism to control the 
 * exactness of matching – see 
 * PTSetSessionCfgEx, PT_SESSION_CFG item 
 * SecuritySetting.
 * @param plMaxFRRRequested Pointer to max. FRR (False Reject Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FRR value N/(2^31-1) – the 
 * large the value, the worse the result. 
 * 
 * This parameter is currently ignored.
 * @param pboFARPrecedence If both FAR and FRR are provided, this 
 * parameter decides which of them takes 
 * precedence.
 * 
 * PT_TRUE->FAR, PT_FALSE->FRR. 
 * 
 * This parameter is currently ignored. 
 * @param pCapturedTemplate The template to verify – can be only standard 
 * BIR data; wrapped BIR or slot ID is not accepted.
 * @param pStoredTemplate The enrollment template to be verified against 
 * – can be only standard BIR data; wrapped 
 * BIR or slot ID is not accepted.
 * @param ppAdaptedTemplate Reserved, use NULL.
 * @param pboResult The result: Match (PT_TRUE)/No match 
 * (PT_FALSE).
 * @param plFARAchieved The value of FAR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL.
 * @param plFRRAchieved The value of FRR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL.
 * @param ppPayload This parameter is currently ignored, we 
 * recommend to use NULL.
 * @param pMatchParams Pointer to PT_MATCH_ON_HOST_PARAMS 
 * structure with additional parameters for 
 * matching.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTVerifyMatchOnHost(
    IN PT_CONNECTION hConnection,
    IN PT_LONG *plMaxFARRequested,
    IN PT_LONG *plMaxFRRRequested,
    IN PT_BOOL *pboFARPrecedence,
    IN PT_INPUT_BIR *pCapturedTemplate,
    IN PT_INPUT_BIR *pStoredTemplate,
    OUT PT_BIR **ppAdaptedTemplate,
    OUT PT_BOOL *pboResult,
    OUT PT_LONG *plFARAchieved,
    OUT PT_LONG *plFRRAchieved,
    OUT PT_DATA **ppPayload,
    IN PT_MATCH_ON_HOST_PARAMS *pMatchParams
);


/** 
 * Match the supplied captured template against the supplied enrollment 
 * templates. Unlike PTVerifyEx this function is completely performed on 
 * host without communication with FM.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param plMaxFARRequested Pointer to max. FAR (False Accept Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FAR value N/(2^31-1) – the
 * large the value, the worse the result. 
 * 
 * Currently this parameter is ignored and FM is 
 * using a different mechanism to control the 
 * exactness of matching – see 
 * PTSetSessionCfgEx, PT_SESSION_CFG item 
 * SecuritySetting.
 * @param plMaxFRRRequested Pointer to max. FRR (False Reject Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FRR value N/(2^31-1) – the 
 * large the value, the worse the result. 
 * 
 * This parameter is currently ignored.
 * @param pboFARPrecedence If both FAR and FRR are provided, this 
 * parameter decides which of them takes 
 * precedence.
 * 
 * PT_TRUE->FAR, PT_FALSE->FRR. 
 * 
 * This parameter is currently ignored. 
 * @param pCapturedTemplate The template to verify – can be only standard 
 * BIR data; wrapped BIR or slot ID is not accepted.
 * @param pStoredTemplates The enrollment template to be verified against 
 * – can be only standard BIR data; wrapped 
 * BIR or slot ID is not accepted.
 * @param byNrTemplates Number of templates in StoredTemplates.
 * @param ppAdaptedTemplate Reserved, use NULL.
 * @param pshResult The result: The zero-based index of the best 
 * matching template or –1 if no match.
 * @param plFARAchieved The value of FAR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL.
 * @param plFRRAchieved The value of FRR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL.
 * @param ppPayload This parameter is currently ignored, we 
 * recommend to use NULL.
 * @param pMatchParams Pointer to PT_MATCH_ON_HOST_PARAMS 
 * structure with additional parameters for 
 * matching.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTIdentifyMatchOnHost(
    IN PT_CONNECTION hConnection,
    IN PT_LONG *plMaxFARRequested,
    IN PT_LONG *plMaxFRRRequested,
    IN PT_BOOL *pboFARPrecedence,
    IN PT_INPUT_BIR *pCapturedTemplate,
    IN PT_INPUT_BIR *pStoredTemplates,
    IN PT_BYTE byNrTemplates,
    OUT PT_BIR **ppAdaptedTemplate,
    OUT PT_SHORT *pshResult,
    OUT PT_LONG *plFARAchieved,
    OUT PT_LONG *plFRRAchieved,
    OUT PT_DATA **ppPayload,
    IN PT_MATCH_ON_HOST_PARAMS *pMatchParams
);


/** 
 * Scan the live finger or use the last captured finger data and try to match it
 * against the supplied enrollment template. If the functions scans live finger, the
 * template obtained will be remembered throughout the session and can be used by
 * other biometric matching functions. This function can call GUI callbacks (unless
 * boCaptured is FALSE)
 * 
 * @param hConnection Handle to the connection to FM.
 * @param plMaxFARRequested Pointer to max. FAR (False Accept Ratio) requested by the caller.
 *  A 32-bit value (N) corresponds to FAR value N/(2^31-1) – the larger the value,
 * the worse the result.
 * 
 * Currently this parameter is ignored and FM is using a different mechanism
 * to control the exactness of matching – see PTSetSessionCfgEx,
 * PT_SESSION_CFG item SecuritySetting.
 * @param plMaxFRRRequested Pointer to max. FRR (False Reject Ratio) requested by the caller.
 * A 32-bit value (N) corresponds to FRR value N/(2^31-1) – the larger the value,
 * the worse the result.
 * 
 * This parameter is currently ignored.
 * @param pboFARPrecedence If both FAR and FRR are provided, this 
 * parameter decides which of them takes 
 * precedence.
 * 
 * PT_TRUE->FAR, PT_FALSE->FRR. 
 * 
 * This parameter is currently ignored. 
 * @param pStoredTemplate The template to be verified against - BIR data or slot ID of one
 * of the templates stored in the template database.
 * @param ppAdaptedTemplate Reserved, use NULL
 * @param pboResult The result: Match (PT_TRUE)/No match 
 * (PT_FALSE).
 * @param plFARAchieved The value of FAR achieved. Currently no meaningful value
 * is returned – we recommend to use NULL.
 * @param plFRRAchieved The value of FRR achieved. Currently no meaningful value
 * is returned – we recommend to use NULL.
 * @param ppPayload Address of the pointer, which will be set to 
 * point to the payload data, originally 
 * embedded in the StoredTemplate. Payload 
 * data is available only when successful match 
 * is achieved.
 * @param lTimeout Timeout in milliseconds. "-1" means default timeout, "-2" means infinitetimeout (ESS2.2/SONLY/TCD50 only). 
 * @param boCapture If PT_TRUE, PTVerify at first captures live 
 * fingerprint. If PT_FALSE, the Last Good 
 * Template (result of the last interactive 
 * biometric function) will be used. 
 * @param ppAuditData Reserved, use NULL.
 * @param pSignData Reserved, use NULL.
 * @param ppSignature Reserved, use NULL. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTVerify(
    IN PT_CONNECTION hConnection,
    IN PT_LONG *plMaxFARRequested,
    IN PT_LONG *plMaxFRRRequested,
    IN PT_BOOL *pboFARPrecedence,
    IN PT_INPUT_BIR *pStoredTemplate,
    OUT PT_BIR **ppAdaptedTemplate,
    OUT PT_BOOL *pboResult,
    OUT PT_LONG *plFARAchieved,
    OUT PT_LONG *plFRRAchieved,
    OUT PT_DATA **ppPayload,
    IN PT_LONG lTimeout,
    IN PT_BOOL boCapture,
    OUT PT_BIR **ppAuditData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Scan the live finger or use the last captured finger data (Last Good 
 * Template) and try to match it against the supplied enrollment template.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param plMaxFARRequested 
        Pointer to max. FAR (False Accept Ratio) 
        requested by the caller. A 32-bit value (N) 
        corresponds to FAR value N/(2^31-1) – the 
        larger the value, the worse the result. 
        
        Currently this parameter is ignored and FM is 
        using a different mechanism to control the 
        exactness of matching – see 
        PTSetSessionCfgEx, PT_SESSION_CFG item 
        SecuritySetting.
      
 * This parameter is ignored in PTAPI Lite.
 * @param plMaxFRRRequested 
        Pointer to max. FRR (False Reject Ratio) 
        requested by the caller. A 32-bit value (N) 
        corresponds to FRR value N/(2^31-1) – the 
        larger the value, the worse the result. 
        
        This parameter is currently ignored.
      
 * This parameter is ignored in PTAPI Lite.
 * @param pboFARPrecedence 
        If both FAR and FRR are provided, this 
        parameter decides which of them takes 
        precedence.
        
        PT_TRUE->FAR, PT_FALSE->FRR. 
        
        This parameter is currently ignored. 
      
 * This parameter is ignored in PTAPI Lite.
 * @param pStoredTemplates 
        An array of enrollment templates to be 
        verified against – each item is BIR data or slot 
        ID of one of the templates stored in the FM's 
        template database. It is possible to freely mix 
        the two types in the array. 
        An array of enrollment templates to be 
      
 * verified against – only BIR data can be used.
 * @param byNrTemplates Number of templates in StoredTemplates.
 * @param ppAdaptedTemplate Reserved, use NULL.
 * This parameter is ignored in PTAPI Lite.
 * @param pshResult The result: The zero-based index of the best 
 * matching template or –1 if no match. 
 * @param plFARAchieved 
 	    The value of FAR achieved. Currently no 
	    meaningful value is returned – we 
	    recommend to use NULL.
      
 * This parameter is ignored in PTAPI Lite.
 * @param plFRRAchieved 
	    The value of FRR achieved. Currently no 
	    meaningful value is returned – we 
	    recommend to use NULL. 
      
 * This parameter is ignored in PTAPI Lite.
 * @param ppPayload Address of the pointer, which will be set to 
 * point to the payload data, originally 
 * embedded in the Stored Template. Payload 
 * data is available only when successful match 
 * is achieved.
 * @param lTimeout Timeout in milliseconds. "-1" means default timeout, "-2" means infinite timeout (ESS2.2/SONLY only). 
 * @param boCapture If PT_TRUE, PTVerify at first captures live 
 * fingerprint. If PT_FALSE, the Last Good 
 * Template (result of the last interactive 
 * biometric function) will be used. 
 * @param ppAuditData Reserved, use NULL.
 * This parameter is ignored in PTAPI Lite.
 * @param pSignData Reserved, use NULL.
 * This parameter is ignored in PTAPI Lite.
 * @param ppSignature Reserved, use NULL. 
 * This parameter is ignored in PTAPI Lite.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTVerifyEx(
    IN PT_CONNECTION hConnection,
    IN PT_LONG *plMaxFARRequested,
    IN PT_LONG *plMaxFRRRequested,
    IN PT_BOOL *pboFARPrecedence,
    IN PT_INPUT_BIR *pStoredTemplates,
    IN PT_BYTE byNrTemplates,
    OUT PT_BIR **ppAdaptedTemplate,
    OUT PT_SHORT *pshResult,
    OUT PT_LONG *plFARAchieved,
    OUT PT_LONG *plFRRAchieved,
    OUT PT_DATA **ppPayload,
    IN PT_LONG lTimeout,
    IN PT_BOOL boCapture,
    OUT PT_BIR **ppAuditData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Convert template to specified format.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pInputBIR Source template.
 * @param dwTargetType Target template type.
 * @param boReturnPayload If TRUE then the converted template is returned with its payload.
 * @param ppOutputBIR Address of the pointer, which will be set to point to the converted template (BIR). The template has to be discarded by a call to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTConvertTemplate(
    IN PT_CONNECTION hConnection,
    IN PT_INPUT_BIR *pInputBIR,
    IN PT_DWORD dwTargetType,
    IN PT_BOOL boReturnPayload,
    OUT PT_BIR **ppOutputBIR
);


/** 
 * Convert template to specified format including standard ANSI/ISO formats.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwSourceType Source format type (see PT_TEMPLATE_TYPE_xxxx). If PT_TEMPLATE_TYPE_AUTO is used then source template type is detected automatically.
 * @param dwSourceEnvelope Source template envelope (see PT_TEMPLATE_ENVELOPE_xxxx).
 * @param pInputData Source template data.
 * @param dwTargetType Target template type (see PT_TEMPLATE_TYPE_xxxx).If PT_TEMPLATE_TYPE_AUTO is used then target template type is choosed automatically.
 * @param dwTargetEnvelope Target template envelope (see PT_TEMPLATE_ENVELOPE_xxxx).
 * @param pSupplementaryData Reserved for future use, set to NULL.
 * @param dwFlags Additional flags (see PT_CONVTEMPL_FLAG_xxxx).
 * @param ppOutputData Address of the pointer, which will be set to point to the converted template in the target format. The template has to be discarded by a call to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTConvertTemplateEx(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwSourceType,
    IN PT_DWORD dwSourceEnvelope,
    IN PT_DATA *pInputData,
    IN PT_DWORD dwTargetType,
    IN PT_DWORD dwTargetEnvelope,
    IN PT_DATA *pSupplementaryData,
    IN PT_DWORD dwFlags,
    OUT PT_DATA **ppOutputData
);


/** 
 * Scan the live finger or use the last captured finger data (Last Good 
 * Template) and try to match it against all the templates stored in FM's 
 * permanent template database. Return the slot number of the best matching 
 * template or –1 if no match.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param plMaxFARRequested Pointer to max. FAR (False Accept Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FAR value N/(2^31-1) – the 
 * larger the value, the worse the result. 
 * 
 * Currently this parameter is ignored and FM is 
 * using a different mechanism to control the 
 * exactness of matching – see 
 * PTPTSetSessionCfgEx, PT_SESSION_CFG item 
 * SecuritySetting.
 * @param plMaxFRRRequested Pointer to max. FRR (False Reject Ratio) 
 * requested by the caller. A 32-bit value (N) 
 * corresponds to FRR value N/(2^31-1) – the 
 * larger the value, the worse the result. 
 * 
 * This parameter is currently ignored.
 * @param pboFARPrecedence If both FAR and FRR are provided, this 
 * parameter decides which of them takes 
 * precedence.
 * 
 * PT_TRUE->FAR, PT_FALSE->FRR. 
 * This parameter is currently ignored. 
 * @param ppAdaptedTemplate Reserved, use NULL.
 * @param plResult The result: The slot number (handle) of the 
 * best matching template or –1 if no match. 
 * @param plFARAchieved The value of FAR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL.
 * @param plFRRAchieved The value of FRR achieved. Currently no 
 * meaningful value is returned – we 
 * recommend to use NULL. 
 * @param ppPayload Address of the pointer, which will be set to 
 * point to the payload data, originally 
 * embedded in the StoredTemplate. Payload 
 * data is available only when successful match 
 * is achieved.
 * @param lTimeout Timeout in milliseconds (used if boCapture = 
 * PT_TRUE). "-1" means default timeout, "-2" 
 * means infinite timeout (ESS2.2/SONLY/TCD50 only). 
 * @param boCapture If PT_TRUE, PTVerify at first captures live 
 * fingerprint. If PT_FALSE, the Last Good 
 * Template (result of the last interactive 
 * biometric function) will be used. 
 * @param ppAuditData Reserved, use NULL.
 * @param pSignData Reserved, use NULL.
 * @param ppSignature Reserved, use NULL. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTVerifyAll(
    IN PT_CONNECTION hConnection,
    IN PT_LONG *plMaxFARRequested,
    IN PT_LONG *plMaxFRRRequested,
    IN PT_BOOL *pboFARPrecedence,
    OUT PT_BIR **ppAdaptedTemplate,
    OUT PT_LONG *plResult,
    OUT PT_LONG *plFARAchieved,
    OUT PT_LONG *plFRRAchieved,
    OUT PT_DATA **ppPayload,
    IN PT_LONG lTimeout,
    IN PT_BOOL boCapture,
    OUT PT_BIR **ppAuditData,
    IN PT_DATA *pSignData,
    OUT PT_DATA **ppSignature
);


/** 
 * Verifies if there is a finger on the sensor. If Timeout is nonzero, the function 
 * waits for the specified time interval until the required conditions are met.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lTimeout Timeout in milliseconds. "-1" means default 
 * timeout. "0" is an acceptable value for this 
 * function, it means "test if the current state 
 * meets the required conditions and return 
 * immediately". 
 * @param dwFlags Bit mask, determining the behavior of the 
 * function and the conditions for which the 
 * function waits (see PT_DETECT_xxxx values).
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTDetectFingerEx(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lTimeout,
    IN PT_DWORD dwFlags
);


/** 
 * Set image format for following image acquiring or navigation operations.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwOperation Type of biometric operation for which the format 
 * is set (one of PT_IMGFORMAT_OPER_xxxx values).
 * @param pFormat Pointer to image format structure to be set.
 * @param dwDataSections Bit mask specifying which additional sections of 
 * image data which should be returned with image data 
 * (combination of PT_IMGDATA_SECTION_xxxx flags). 
 * Used only for grab operations which return image 
 * data (PTGrab/PTSleepThenGrab). If selected section is 
 * not available then empty data section is returned. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetImageFormat(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwOperation,
    IN PT_IMGFORMAT *pFormat,
    IN PT_DWORD dwDataSections
);


/** 
 * Get current value of image format set for image acquiring or navigation operations.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwOperation Type of biometric operation which image format setting 
 * is requested (one of PT_IMGFORMAT_OPER_xxxx values).
 * @param ppFormat Returns pointer to image format which is currently 
 * set for specified operation type. The data has to be 
 * freed by a call to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetImageFormat(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwOperation,
    OUT PT_IMGFORMAT **ppFormat
);


/** 
 * Return list of available image formats which can be used for biometric operations after setting by calling PTSetImageFormat.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwSelection Specifies which image formats should be listed 
 * (one of PT_IMGFORMAT_LIST_xxxx values).
 * @param ppFormatList Returns pointer to image format list. The data has to be 
 * freed by a call to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTListImageFormats(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwSelection,
    OUT PT_IMGFORMAT_LIST **ppFormatList
);


/** 
 * Perform anti-latent operation.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwMode Mode of operation (see PT_LATENT_MODE_xxxx).
 * @param dwInFlags Additional input flags. Reserved for future use (set to 0).
 * @param pImageData Pointer to image data which should be used for latent check. Note that only Dynamic Image Format can be used. Can be NULL if captured template is provided through 'pCapturedTemplate' parameter.
 * @param pCapturedTemplate Pointer to captured template which should be used for latent check. Only PT_FULLBIR_INPUT format of template can be used. Can be NULL if image data is provided through 'pImageData' parameter.
 * @param pboLatentDetected Returns result of the latent check - PT_TRUE if latent image was detected, PT_FALSE otherwise. The returned value is valid only if PT_LATENT_MODE_CHECK or PT_LATENT_MODE_CHECK_AND_STORE operation is performed.
 * @param pdwOutFlags Optionally can return additional result flags, can be set to NULL. Reserved for future use (0 is returned).
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTCheckLatent(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwMode,
    IN PT_DWORD dwInFlags,
    IN PT_DATA *pImageData,
    IN PT_INPUT_BIR *pCapturedTemplate,
    OUT PT_BOOL *pboLatentDetected,
    OUT PT_DWORD *pdwOutFlags
);


/** 
 * Stores given fingerprint template in a slot in the FM's permanent template database.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pTemplate Template (BIR) to be stored in the template 
 * database. If NULL, the Last Good Template 
 * (result of the latest interactive biometric 
 * function) will be used. 
 * @param plSlotNr Pointer to a variable, which will receive the 
 * slot number where the template was stored. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTStoreFinger(
    IN PT_CONNECTION hConnection,
    IN PT_INPUT_BIR *pTemplate,
    OUT PT_LONG *plSlotNr
);


/** 
 * Delete the given fingerprint template (identified by its slot number) from the 
 * fingerprint database in FM's non-volatile memory. Additional public data 
 * assigned to this slot by PTSetFingerData() or PTSetFingerDataEx() are 
 * deleted too. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lSlotNr The slot number of the template to be deleted.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTDeleteFinger(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lSlotNr
);


/** 
 * Delete all the fingerprint templates from the fingerprint database in FM's 
 * non-volatile memory. All additional public data assigned by 
 * PTSetFingerData() or PTSetFingerDataEx() are deleted too. 
 * deleted too. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTDeleteAllFingers(
    IN PT_CONNECTION hConnection
);


/** 
 * Assign an additional public data to a finger template stored in FM's 
 * fingerprint database. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lSlotNr The slot number of the template to be associated with data.
 * @param pFingerData The data to be stored together with the 
 * template. If the data length is zero, the 
 * existing application data associated with given 
 * fingerprint (if any) will be deleted. Max. 
 * length of finger data is 
 * PT_MAX_FINGER_DATA_LENGTH = 96. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetFingerData(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lSlotNr,
    IN PT_DATA *pFingerData
);


/** 
 * Read the additional public data associated with a finger template stored in 
 * FM's template database. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lSlotNr The slot number of the template whose public data should be read.
 * @param ppFingerData Address of the pointer, which will be set to 
 * point to the public data associated with given 
 * fingerprint. If no data is associated with the 
 * fingerprint, the result will be a data block with 
 * zero length. The data has to be freed by a call 
 * to PTFree(). 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetFingerData(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lSlotNr,
    OUT PT_DATA **ppFingerData
);


/** 
 * Return the list of all fingers stored in the FM's finger database together with 
 * their associated public data. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param ppFingerList Address of the pointer, which will be set to 
 * point to the list of stored fingerprints and 
 * their associated public data. The data has to 
 * be freed by a call to PTFree(). 
 * 
 * See the description of PT_FINGER_LIST for 
 * the format of returned data. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTListAllFingers(
    IN PT_CONNECTION hConnection,
    OUT PT_FINGER_LIST **ppFingerList
);


/** 
 * Read template data from FM's finger database stored in given slot.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lSlotNr Number of slot from which the template 
 * should be read. 
 * @param boReturnPayload If TRUE then the template is returned with its 
 * payload. Otherwise payload data are cut-off 
 * from the template. 
 * @param ppStoredTemplate Address of the pointer, which will be set to 
 * point to the stored template (BIR). The 
 * template is allocated by the API library and 
 * has to be discarded by a call to PTFree(). 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTLoadFinger(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lSlotNr,
    IN PT_BOOL boReturnPayload,
    OUT PT_BIR **ppStoredTemplate
);


/** 
 * Change payload data of a template stored in FM's finger database in the given slot.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lSlotNr The slot number of the template which 
 * payload data has to be set. 
 * @param pPayload New payload data. If it has zero length then 
 * payload data in specified slot are deleted. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetFingerPayload(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lSlotNr,
    IN PT_DATA *pPayload
);


/** 
 * Update payload data of a template stored in FM's finger database in given 
 * slot. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lSlotNr The slot number of the template which 
 * payload data has to be changed. 
 * @param lOffset Offset from which are the payload data 
 * changed. 
 * @param pData Data to be stored at the given offset in 
 * payload. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTUpdateFingerPayload(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lSlotNr,
    IN PT_LONG lOffset,
    IN PT_DATA *pData
);


/** 
 * Return the scanning quality of the last acquired fingerprint image. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pdwScanQuality Pointer to a variable, which will be set to the 
 * quality of the last acquired fingerprint image. 
 * It is expressed in percents, 100 = max. 
 * possible quality. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTScanQuality(
    IN PT_CONNECTION hConnection,
    OUT PT_DWORD *pdwScanQuality
);


/** 
 * Return detailed information about last finger swipe. This function can be also used for systems with area sensor, where it returns information about the last fingerprint scan.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param ppSwipeInfo Address of the pointer, which will be set to 
 * point to the resulting PT_SWIPE_INFO 
 * structure with last swipe information. The 
 * structure has to be discarded by a call to 
 * PTFree(). 
 * @param ppSkewInfo Address of the pointer, which will be set to 
 * point to the structure with last swipe skew 
 * information. The structure has to be discarded 
 * by a call to PTFree(). If this parameter is 
 * NULL then no skew information is returned. 
 * @param ppSpeedInfo Address of the pointer, which will be set to 
 * point to the structure with last swipe speed 
 * information. The structure has to be discarded 
 * by a call to PTFree(). If this parameter is 
 * NULL then no speed information is returned. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetSwipeInfo(
    IN PT_CONNECTION hConnection,
    OUT PT_SWIPE_INFO **ppSwipeInfo,
    OUT PT_DATA **ppSkewInfo,
    OUT PT_DATA **ppSpeedInfo
);


/** 
 * Switch FM to navigation mode (a.k.a. biometric mouse). In this mode FM 
 * will periodically provide navigation info to the host, i.e. information about 
 * the direction and length of finger movement since the last navigation info. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lEventPeriod Delay in milliseconds between sending 
 * navigation data from FM. FM will send one 
 * packet per period, with all the navigation data 
 * accumulated over the period. 
 * If lEventPeriod is set to "-1", FM will use a bidirectional 
 * mode (see Remarks). 
 * @param pfnCallback Callback function, called every time when a 
 * navigation data packet arrives to the host. 
 * See the description of PT_NAVIGATION_CALLBACK. 
 * @param pNavigationCallbackCtx A generic pointer to context data. This pointer 
 * will be supplied to the callback function every 
 * time it is called. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTNavigate(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lEventPeriod,
    IN PT_NAVIGATION_CALLBACK pfnCallback,
    IN void *pNavigationCallbackCtx
);


/** 
 * Switch FM to navigation mode (a.k.a. biometric mouse). In this mode FM will provide 
 * navigation info to the host. If finger is not present on the sensor for specified 
 * time, FM is switched to power-saving mode until finger is again put on the sensor.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param lEventPeriod Delay in milliseconds between sending 
 * navigation data from FM. FM will send one 
 * packet per period, with all the navigation data 
 * accumulated over the period. 
 * If lEventPeriod is set to "-1", FM will use a on-demand
 * mode (see Remarks). 
 * @param lIdleTimeout Timeout after which is FM switched to power-saving mode if no finger
 * is detected. Value "-1" means default timeout, which is 5 seconds.
 * @param pfnCallback Callback function, called every time when a 
 * navigation data packet arrives to the host. 
 * @param pNavigationCallbackCtx A generic pointer to context data. This pointer 
 * will be supplied to the callback function every 
 * time it is called. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTNavigateWithSleep(
    IN PT_CONNECTION hConnection,
    IN PT_LONG lEventPeriod,
    IN PT_LONG lIdleTimeout,
    IN PT_NAVIGATION_CALLBACK pfnCallback,
    IN void *pNavigationCallbackCtx
);


/** 
 * Starts or stops 'on-demand' navigation mode or return navigation data when 'on-demand' navigation is running.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwAction Action to be performed (see PT_NAVIG_ACTION_xxxx). 
 * @param ppNavigationData Address of the pointer, which will be set to point to the structure with navigation
 * data. The returned pointer is valid only for action PT_NAVIG_ACTION_GET_DATA.
 * @param pdwTimeStampMs Returns timestamp in milliseconds associated with the navigation data. The value
 * is valid only for action PT_NAVIG_ACTION_GET_DATA.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetNavigationData(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwAction,
    OUT PT_NAVIGATION_DATA **ppNavigationData,
    OUT PT_DWORD *pdwTimeStampMs
);


/** 
 * Extract supplied fingerprint image into a biometric template.
 * Operation is completely performed on host without communication with FM.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pImageData Pointer to data of fingerprint image to be extracted.
 * @param pImageInfo Pointer to a structure containing information about the supplied image.
 * @param byPurpose Purpose of the extracted template (see PT_PURPOSE_xxxx).
 * @param dwTemplateType Type of extracted template (see PT_TEMPLATE_TYPE_xxxx).
 * @param ppTemplate Address of the pointer, which will be set to 
 * point to the resulting template (BIR). The 
 * template has to be discarded by a call to PTFree().
 * @param ppTemplateInfo Reserved, use NULL.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTExtractTemplate(
    IN PT_CONNECTION hConnection,
    IN PT_DATA *pImageData,
    IN PT_EXTRTMPL_IMGINFO *pImageInfo,
    IN PT_BYTE byPurpose,
    IN PT_DWORD dwTemplateType,
    OUT PT_BIR **ppTemplate,
    OUT PT_EXTRTMPL_TMPLINFO **ppTemplateInfo
);




/** 
 * @name Miscellaneous Functions
 */

/** 
 * Return a set of information about the connected FM, including 
 * the version and configuration of FM.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param ppInfo Address of a pointer which will be set to point to an info block.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTInfo(
    IN PT_CONNECTION hConnection,
    OUT PT_INFO **ppInfo
);


/** 
 * Return an additional information about the FM
 * 
 * @param hConnection Handle to the connection to FM.
 * @param ppExtInfo Address of the pointer, which will be set to point to the structure
 * with extended FM information. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTExtendedInfo(
    IN PT_CONNECTION hConnection,
    OUT PT_EXTENDED_INFO **ppExtInfo
);


/** 
 * Check that the connected device is responding.
 * 
 * @param hConnection Connection handle to a device which has to be tested.
 * @param dwFlags Additional flags. Currently not used, set it to 0.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTPing(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwFlags
);


/** 
 * Set the session parameters of the FM. The parameters influence behavior of
 * many FM's functions, especially the biometrics. See the description of PT_SESSION_CFG
 * structures for detailed info about the session configuration data.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param wCfgVersion Version of the configuration data. Versioning 
 * is provided to keep backward compatibility 
 * with older host software.
 * @param wCfgLength Length of the provided configuration data.
 * @param pSessionCfg The new session configuration data.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetSessionCfgEx(
    IN PT_CONNECTION hConnection,
    IN PT_WORD wCfgVersion,
    IN PT_WORD wCfgLength,
    IN PT_SESSION_CFG *pSessionCfg
);


/** 
 * Get the current session parameters of the FM. The parameters influence 
 * behavior of many FM's functions, especially the biometrics. See the 
 * description of PT_SESSION_CFG structures for detailed info about the session 
 * configuration data. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param wCfgVersion Requested version of the configuration data. Versioning 
 * is provided to keep backward compatibility 
 * with older host software.
 * @param pwCfgLength Pointer to the length of the received 
 * configuration data. 
 * @param ppSessionCfg Address of a pointer which will be set to point to the session
 * config block. The data has to be freed by a call to PTFree().
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetSessionCfgEx(
    IN PT_CONNECTION hConnection,
    IN PT_WORD wCfgVersion,
    OUT PT_WORD *pwCfgLength,
    OUT PT_SESSION_CFG **ppSessionCfg
);


/** 
 * Switches FM todeep sleep or standby mode. This function is especially 
 * useful for low-power waiting for a touch of a finger. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwSleepMode Sleep mode to be used. Possible values are 
 * PT_SLEEP_MODE_DEEPSLEEP (0) 
 * (fingerprint sensor is powered down, 
 * finger detect is not active); 
 * PT_SLEEP_MODE_STANDBY (1) (finger 
 * detect is active). 
 * Value of this parameter must be always set to PT_SLEEP_MODE_STANDBY (1) in PTAPI Lite.
 * @param pfnCallback Callback function, called periodically all the 
 * time FM is sleeping. Optional, can be NULL. 
 * See the description of PT_IDLE_CALLBACK. 
 * @param pIdleCallbackCtx A generic pointer to callback's context data. 
 * This pointer will be supplied to the callback 
 * function every time it is called. 
 * @param pdwWakeupCause The cause of wakeup. Currently the following 
 * causes are possible: 
 * PT_WAKEUP_CAUSE_HOST (0) (signal from the Host) ; 
 * PT_WAKEUP_CAUSE_FINGER (1) (a finger was detected).
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSleep(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwSleepMode,
    IN PT_IDLE_CALLBACK pfnCallback,
    IN void *pIdleCallbackCtx,
    OUT PT_DWORD *pdwWakeupCause
);


/** 
 * Returns the size in bytes of the remaining EEPROM memory on the FM 
 * available for application's usage. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwType Type of the memory, see PT_MEMTYPE_xxxx values.
 * @param pdwAvailableMemory Amount of available memory of given type in 
 * bytes. PTGetAvailableMemory will return 
 * value 0xFFFFFFFF (-1), if memory of 
 * requested type is not present or not 
 * accessible. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetAvailableMemory(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwType,
    OUT PT_DWORD *pdwAvailableMemory
);


/** 
 * This function allows the application to store a block of arbitrary binary data 
 * in FM's non-volatile memory. There are several areas, shared by all 
 * applications. The difference between areas is in access rights – see below. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwArea Area to write. One of the PT_AREA_xxxx values.
 * @param pAppData The data to be stored in NVM. If the data length is zero, 
 * the current application data will be deleted. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetAppData(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwArea,
    IN PT_DATA *pAppData
);


/** 
 * Read the application data stored in FM's non-volatile memory. There are 
 * several areas, shared by all applications. The difference between areas is in 
 * access rights. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwArea Area to read. One of the PT_AREA_xxxx values.
 * @param ppAppData Address of the pointer, which will be set to 
 * point to the application data. If no data are 
 * stored in NVM, the result will be a data block 
 * with zero length. The data is allocated by the 
 * API library and has to be freed by a call to PTFree(). 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetAppData(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwArea,
    OUT PT_DATA **ppAppData
);


/** 
 * This function allows the application to control the state and behavior of the 
 * two user interface LEDs, which can be optionally connected to the FM. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwMode Mode of the LEDs. Different modes define 
 * different behavior of the LEDs during specific 
 * operations, especially the biometrics. Defined modes: 
 * PT_LED_MODE_MANUAL (0) – Allows a 
 * full control of the LEDs by the 
 * application, without any influence by 
 * other FM operations. 
 * 
 * PT_LED_MODE_BIO (1) – State of 
 * LEDs is fully controlled by FM 
 * depending on the progress of currently 
 * running biometric operation. 
 * Parameters dwLED1 and dwLED2 have 
 * no effect. 
 * Supported in ESS2.2 and TCD50. 
 * 
 * PT_LED_MODE_READER (2) – State of 
 * LEDs is fully controlled by FM depending on the current
 * state of the device. Parameters dwLED1 and dwLED2 have
 * no effect. Supported in ESS2.2 (EIKON variant), TCD50 and SONLY.
 * @param dwLED1 Parameter defining the detailed behavior of 
 * the first LED. This parameter is mode-specific. 
 * @param dwLED2 Parameter defining the detailed behavior of 
 * the second LED. This parameter is mode-specific. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetLED(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwMode,
    IN PT_DWORD dwLED1,
    IN PT_DWORD dwLED2
);


/** 
 * This function allows the application to query the state and behavior of the 
 * two user interface LEDs, which can be optionally connected to the FM. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pdwMode Returns a mode of the LEDs. See PTSetLED for details. 
 * @param pdwLED1 Returns a value defining the detailed behavior 
 * of the first LED. This parameter is modespecific. 
 * See PTSetLED for details. 
 * @param pdwLED2 Returns a value defining the detailed behavior 
 * of the second LED. This parameter is modespecific. 
 * See PTSetLED for details. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetLED(
    IN PT_CONNECTION hConnection,
    OUT PT_DWORD *pdwMode,
    OUT PT_DWORD *pdwLED1,
    OUT PT_DWORD *pdwLED2
);


/** 
 * This function allows the application to control the state and behavior of the 
 * available user interface LEDs. This is an extended version of PTSetLED function.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwMode Mode of the LEDs. Different modes define 
 * different behavior of the LEDs during specific 
 * operations, especially the biometrics.
 * @param pLedParams Block of additional LED parameters specific for given mode (see PT_LED_PARAMS_xxxx),
 * NULL for modes which have no additional parameters.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSetLedEx(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwMode,
    IN PT_DATA *pLedParams
);


/** 
 * This function allows the application to query the state and behavior of the available 
 * user interface LEDs. This is an extended version of PTGetLED function.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param pdwMode Returns current mode of the LEDs. See PTSetLedEx for details. 
 * @param ppLedParams Address of the pointer, which will be set to point to the additional
 * LED parameters specific for current LED mode. If no additional parameters 
 * are set for current mode, the result will be a data block 
 * with zero length. The data is allocated by the 
 * API library and has to be freed by a call to PTFree(). 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetLedEx(
    IN PT_CONNECTION hConnection,
    OUT PT_DWORD *pdwMode,
    OUT PT_DATA **ppLedParams
);





/** 
 * @name Security Functions
 */

/** 
 * Authentify the caller (host) to the FM, allow access to FM functions.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwAuthentifyId ID of the authentication group
 * @param pKey The authentication key
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTAuthentify(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwAuthentifyId,
    IN PT_DATA *pKey
);


/** 
 * Authentify the caller (host) to the FM, allow access to FM functions.
 * Caller is authenticated to specified account.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwAccountId ID of account for which authentication is requested.
 * @param pKey The authentication key
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTAuthentifyEx(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwAccountId,
    IN PT_DATA *pKey
);


/** 
 * Authentify the caller (host) to the specific FM account allowing access to 
 * functions enabled for that account. Optionally establish secure channel 
 * for the given connection. This function can also load and immediately use 
 * wrapped profile.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwAccountId ID of account for which authentication is requested.
 * @param pWrappedProfile Wrapped profile to be used, NULL if not used.
 * @param pAuthKey Authentication key, can be NULL if dwAccountId is PT_ACCOUNT_ANONYMOUS.
 * @param pPublicKey Public part of the SECCHAN key pair used for mutual authentication, can be NULL if not used. This parameter is ignored for SONLY.
 * @param dwMechanism Crypto mechanism used for channel encryption, use 
 * PT_SECURECHANNEL_MECH_NONE if secure channel is not needed.
 * @param dwKeyLength Length of key (in bits) used for channel encryption. Note that only some key
 * lengths are allowed for certain crypto mechanisms.
 * @param dwFlags Additional flags for secure channel encryption (like enabling encryption of GUI callbacks).
 * @param dwVersion Version of secure channel protocol.
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTAuthentifyAdv(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwAccountId,
    IN PT_DATA *pWrappedProfile,
    IN PT_DATA *pAuthKey,
    IN PT_DATA *pPublicKey,
    IN PT_DWORD dwMechanism,
    IN PT_DWORD dwKeyLength,
    IN PT_DWORD dwFlags,
    IN PT_DWORD dwVersion
);


/** 
 * Establish encrypted communication between FM and host (secure channel) 
 * for the given connection. Secure channel can be established only after 
 * successful authentification via PTAuthentify when session key is generated 
 * from exchanged challenges. 
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwMechanism Crypto mechanism used for channel encryption.
 * @param dwKeyLength Length of key (in bits) used for channel encryption.
 * @param dwFlags Additional flags for channel encryption (like 
 * disabling encryption of GUI callbacks). 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSecureChannel(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwMechanism,
    IN PT_DWORD dwKeyLength,
    IN PT_DWORD dwFlags
);


/** 
 * Establish encrypted communication between ESS and host (secure channel) 
 * for the given connection. Secure channel can be established only after 
 * successful authentification via PTAuthentify when session key is generated 
 * from exchanged challenges. This function allows specify version of secure 
 * channel protocol to be used.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwMechanism Crypto mechanism used for channel encryption.
 * @param dwKeyLength Length of key (in bits) used for channel encryption.
 * @param dwFlags Additional flags for channel encryption (like 
 * disabling encryption of GUI callbacks). 
 * @param dwVersion Version of secure channel protocol. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTSecureChannelEx(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwMechanism,
    IN PT_DWORD dwKeyLength,
    IN PT_DWORD dwFlags,
    IN PT_DWORD dwVersion
);


/** 
 * Get access rights mask of given account.
 * 
 * @param hConnection Handle to the connection to FM.
 * @param dwAccountId ID of account which access rights mask has to be returned.
 * @param ppAccessRights Address of the pointer, which will be set to 
 * point to the access rights mask of given user's 
 * account. The data has to be freed by a call to PTFree. 
 * @return Status code.
 */
PTAPI_DLL PT_STATUS PTAPI PTGetAccessRights(
    IN PT_CONNECTION hConnection,
    IN PT_DWORD dwAccountId,
    OUT PT_DATA **ppAccessRights
);





#endif /* tfmapi_h */

