
/**
 * @file tfmerror.h
 *
 * PTAPI error codes
 *
 * Copyright (C) 2001-2009 UPEK Inc.
 */

#ifndef TFMERROR_H
#define TFMERROR_H


/**
 * @name Public TFM API error codes
 * Those are valid values of @ref PT_STATUS return type.
 */

/*@{*/


/** 
 * Success return status
 */
#define PT_STATUS_OK      (0)



/** 
 * General or unknown error status. It is also possible that the function  only partially succeeded, and that the device is in an inconsistent state.
 */
#define PT_STATUS_GENERAL_ERROR      (-1001)



/** 
 * PerfectTrust API wasn't initialized
 */
#define PT_STATUS_API_NOT_INIT      (-1002)


/** 
 * PerfectTrust API has been already initialized
 */
#define PT_STATUS_API_ALREADY_INITIALIZED      (-1003)



/** 
 * Invalid parameter error
 */
#define PT_STATUS_INVALID_PARAMETER      (-1004)



/** 
 * Invalid handle error
 */
#define PT_STATUS_INVALID_HANDLE      (-1005)



/** 
 *  Not enough memory to process given operation
 */
#define PT_STATUS_NOT_ENOUGH_MEMORY      (-1006)



/** 
 * Failure of extern memory allocation function
 */
#define PT_STATUS_MALLOC_FAILED      (-1007)



/** 
 * Passed data are too large
 */
#define PT_STATUS_DATA_TOO_LARGE      (-1008)



/** 
 * Not enough permanent memory to store data
 */
#define PT_STATUS_NOT_ENOUGH_PERMANENT_MEMORY      (-1009)



/** 
 * There is more data to return than the supplied buffer can contain
 */
#define PT_STATUS_MORE_DATA      (-1010)



/** 
 * Function failed
 */
#define PT_STATUS_FUNCTION_FAILED      (-1033)



/** 
 * Invalid form of PT_INPUT_BIR structure
 */
#define PT_STATUS_INVALID_INPUT_BIR_FORM      (-1036)



/** 
 * TFM has returned wrong or unexpected response
 */
#define PT_STATUS_WRONG_RESPONSE      (-1037)



/** 
 * Not enough memory on TFM to process given operation
 */
#define PT_STATUS_NOT_ENOUGH_TFM_MEMORY      (-1038)



/** 
 * Connection is already opened
 */
#define PT_STATUS_ALREADY_OPENED      (-1039)



/** 
 * Cannot connect to TFM
 */
#define PT_STATUS_CANNOT_CONNECT      (-1040)



/** 
 * Timeout elapsed
 */
#define PT_STATUS_TIMEOUT      (-1041)



/** 
 * Bad biometric template
 */
#define PT_STATUS_BAD_BIO_TEMPLATE      (-1042)



/** 
 * Requested slot was not found
 */
#define PT_STATUS_SLOT_NOT_FOUND      (-1043)



/** 
 * Attempt to export antispoofing info from TFM
 */
#define PT_STATUS_ANTISPOOFING_EXPORT      (-1044)



/** 
 * Attempt to import antispoofing info to TFM
 */
#define PT_STATUS_ANTISPOOFING_IMPORT      (-1045)



/** 
 * Access to operation is denied
 */
#define PT_STATUS_ACCESS_DENIED      (-1046)



/** 
 * No template was captured in current session
 */
#define PT_STATUS_NO_TEMPLATE      (-1049)



/** 
 * Timeout for biometric operation has expired
 */
#define PT_STATUS_BIOMETRIC_TIMEOUT      (-1050)



/** 
 * Failure of template consolidation
 */
#define PT_STATUS_CONSOLIDATION_FAILED      (-1051)



/** 
 * Biometric operation canceled
 */
#define PT_STATUS_BIO_OPERATION_CANCELED      (-1052)



/** 
 * Authentification failed
 */
#define PT_STATUS_AUTHENTIFICATION_FAILED      (-1053)



/** 
 * Unknown command
 */
#define PT_STATUS_UNKNOWN_COMMAND      (-1054)



/** 
 * Power off attempt failed
 */
#define PT_STATUS_GOING_TO_SLEEP      (-1055)



/** 
 * Function or service is not implemented
 */
#define PT_STATUS_NOT_IMPLEMENTED      (-1056)



/** 
 * General communication error
 */
#define PT_STATUS_COMM_ERROR      (-1057)



/** 
 * Session was terminated
 */
#define PT_STATUS_SESSION_TERMINATED      (-1058)



/** 
 * Touch chip error occured
 */
#define PT_STATUS_TOUCH_CHIP_ERROR      (-1059)



/** 
 * I2C EEPROM error occured
 */
#define PT_STATUS_I2C_EEPROM_ERROR      (-1060)



/** 
 * Purpose parameter (or BIR's purpose) is invalid for given operation
 */
#define PT_STATUS_INVALID_PURPOSE      (-1061)



/** 
 * Finger swipe is too bad for image reconstruction
 */
#define PT_STATUS_SWIPE_TOO_BAD      (-1062)



/** 
 * Requested functionality or value of parameter is not supported
 */
#define PT_STATUS_NOT_SUPPORTED      (-1063)



/** 
 * Calibration failed
 */
#define PT_STATUS_CALIBRATION_FAILED      (-1064)



/** 
 * Antispoofing data were not captured
 */
#define PT_STATUS_ANTISPOOFING_NOT_CAPTURED      (-1065)



/** 
 * Sensor latch-up event detected
 */
#define PT_STATUS_LATCHUP_DETECTED      (-1066)



/** 
 * Diagnostics failed
 */
#define PT_STATUS_DIAGNOSTICS_FAILED      (-1067)



/** 
 * Attempt to upgrade to same firmware version
 */
#define PT_STATUS_SAME_VERSION      (-1068)



/** 
 * No sensor
 */
#define PT_STATUS_NO_SENSOR      (-1069)



/** 
 * The measured values are out of allowable limits
 */
#define PT_STATUS_SENSOR_OUT_OF_LIMITS      (-1070)



/** 
 * Too many bad lines
 */
#define PT_STATUS_TOO_MANY_BAD_LINES      (-1071)



/** 
 * Sensor is not repairable
 */
#define PT_STATUS_SENSOR_NOT_REPAIRABLE      (-1072)



/** 
 * Gain offset calibration error
 */
#define PT_STATUS_GAIN_OFFSET      (-1073)



/** 
 * Asynchronous power shut down
 */
#define PT_STATUS_POWER_SHUTOFF      (-1074)



/** 
 * Attempt to upgrade to older firmware version
 */
#define PT_STATUS_OLD_VERSION      (-1075)



/** 
 * Connection interrupted because of suspend request
 */
#define PT_STATUS_SUSPEND      (-1076)



/** 
 * Device not found
 */
#define PT_STATUS_DEVICE_NOT_FOUND      (-1077)



/** 
 * Device doesn't work as expected
 */
#define PT_STATUS_DEVICE_SICK      (-1078)



/** 
 * Host hardware doesn't support requested communication speed
 */
#define PT_STATUS_UNSUPPORTED_SPEED      (-1079)



/** 
 * Sensor is not calibrated
 */
#define PT_STATUS_SENSOR_NOT_CALIBRATED      (-1080)



/** 
 * Firmware is missing or corrupted, device is running in safe mode
 */
#define PT_STATUS_SAFE_MODE      (-1081)



/** 
 * Sensor hardware error occured
 */
#define PT_STATUS_SENSOR_HW_ERROR      (-1082)



/** 
 * Session was not authenticated yet
 */
#define PT_STATUS_SESSION_NOT_AUTHENTICATED      (-1083)



/** 
 * Secure channel has been already established
 */
#define PT_STATUS_SECURE_CHANNEL_ALREADY_ESTABLISHED      (-1084)



/** 
 * Overflow of One Time Password sequence number
 */
#define PT_STATUS_OTP_SEQUENCE_NUMBER_OVERFLOW      (-1085)



/** 
 * General NVM error
 */
#define PT_STATUS_NVM_ERROR      (-1086)



/** 
 * NVM write operation failed
 */
#define PT_STATUS_NVM_CANNOT_WRITE      (-1087)



/** 
 * NVM read operation failed
 */
#define PT_STATUS_NVM_CANNOT_READ      (-1088)



/** 
 * Attempt to access non-existing internal NVM file
 */
#define PT_STATUS_NVM_INVALID_FILE_ID      (-1089)



/** 
 * General crypto error
 */
#define PT_STATUS_CRYPTO_ERROR      (-1090)



/** 
 * Requested cryptographic mechanism is not supported
 */
#define PT_STATUS_CRYPTO_MECHANISM_NOT_SUPPORTED      (-1091)



/** 
 * Padding error detected during crypto operation
 */
#define PT_STATUS_CRYPTO_PADDING_ERROR      (-1092)



/** 
 * Key too long (probably due to the export regulations)
 */
#define PT_STATUS_CRYPTO_KEY_TOO_LONG      (-1093)



/** 
 * Bad symmetric key used
 */
#define PT_STATUS_CRYPTO_SYM_BAD_KEY      (-1094)



/** 
 * HW random number generator initialization failed
 */
#define PT_STATUS_HW_RNG_INIT_ERROR      (-1095)



/** 
 * Device is already opened for exclusive access by somebody else
 */
#define PT_STATUS_EX_ACCESS_VIOLATION      (-1096)



/** 
 * Used wrong finger data access rights
 */
#define PT_STATUS_WRONG_FINGER_DATA_ACCESS_RIGHTS      (-1097)



/** 
 * Last match data don't exist
 */
#define PT_STATUS_NO_LAST_MATCH_DATA      (-1098)



/** 
 * No data found
 */
#define PT_STATUS_NO_DATA      (-1099)



/** 
 * RSA key is not consistent
 */
#define PT_STATUS_CRYPTO_RSA_KEY_CHECK      (-1100)



/** 
 * Bad RSA public key supplied
 */
#define PT_STATUS_CRYPTO_RSA_BAD_PUBLIC_KEY      (-1101)



/** 
 * NVM has invalid format or is not formatted at all
 */
#define PT_STATUS_NVM_INVALID_FORMAT      (-1102)



/** 
 * NVM has unsupported version
 */
#define PT_STATUS_NVM_UNSUPPORTED_VERSION      (-1103)



/** 
 * Java callback throws java.rmi.RemoteException
 */
#define PT_STATUS_JAVA_CB_REMOTE      (-1104)



/** 
 * Java callback throws java.lang.Error
 */
#define PT_STATUS_JAVA_CB_ERROR      (-1105)



/** 
 * Access to device not granted
 */
#define PT_STATUS_DEVICE_NOT_GRANTED      (-1106)



/** 
 * Can not convert some parameter to proper endian. This can happen
 only if the paramater is pointer to memory whose meaning is context
 dependent and if the host uses other then little endian.
 */
#define PT_STATUS_ENDIAN_ERROR      (-1107)



/** 
 * Private KEYEX key was not set.
 */
#define PT_STATUS_KEYEX_KEY_NOT_SET      (-1108)



/** 
 * Private SIGN key was not set.
 */
#define PT_STATUS_SIGN_KEY_NOT_SET      (-1109)



/** 
 * Invalid OTP seed.
 */
#define PT_STATUS_INVALID_OTP_SEED      (-1110)



/** 
 * Operation was canceled.
 */
#define PT_STATUS_OPERATION_CANCELED      (-1111)



/** 
 * Virtual Channel connection has been lost.
 */
#define PT_STATUS_VCH_LOST      (-1112)



/** 
 * SONLY session is already opened.
 */
#define PT_STATUS_SONLY_ALREADY_OPENED      (-1113)



/** 
 * Private SECCHAN key was not set.
 */
#define PT_STATUS_SECCHAN_KEY_NOT_SET      (-1114)



/** 
 * TCS overcurrent calibration error.
 */
#define PT_STATUS_TCS_OVERCURRENT      (-1115)



/** 
 * Cancel request was ignored.
 */
#define PT_STATUS_CANCEL_IGNORED      (-1116)



/** 
 * Error in communication with sensor due to bus congestion.
 */
#define PT_STATUS_SENSOR_BUS_CONGESTION      (-1117)



/** 
 * Emulated NVM is not formatted.
 */
#define PT_STATUS_EMULATED_NVM_INVALID_FORMAT      (-1118)



/** 
 * General Virtual Channel error.
 */
#define PT_STATUS_VCH_ERROR      (-1119)



/** 
 * Virtual Channel implementation structure contains invalid entries.
 */
#define PT_STATUS_VCH_INVALID_IMPLEMENTATION      (-1120)



/** 
 * Virtual Channel implementation has been already registered.
 */
#define PT_STATUS_VCH_ALREADY_REGISTERED      (-1121)



/** 
 * Virtual Channel implementation has not been registered.
 */
#define PT_STATUS_VCH_NOT_REGISTERED      (-1122)



/** 
 * An external module required by Virtual Channel is missing.
 */
#define PT_STATUS_VCH_MISSING_DEPENDENCIES      (-1123)



/** 
 * Virtual Channel communication error has occured.
 */
#define PT_STATUS_VCH_COMM_ERROR      (-1124)



/** 
 * Virtual Channel timeout has expired.
 */
#define PT_STATUS_VCH_TIMEOUT      (-1125)



/** 
 * Virtual Channel invalid handle.
 */
#define PT_STATUS_VCH_INVALID_HANDLE      (-1126)



/** 
 * Virtual Channel job has been aborted.
 */
#define PT_STATUS_VCH_JOB_ABORTED      (-1127)



/** 
 * Invalid version of the Virtual Channel protocol or internal protocol error.
 */
#define PT_STATUS_VCH_INVALID_PROTOCOL      (-1128)



/** 
 * Virtual Channel not available (the max number of simultaneously opened channels has been reached).
 */
#define PT_STATUS_VCH_CHANNEL_NOT_AVAILABLE      (-1129)



/** 
 * The OS denied to perform the operation.
 */
#define PT_STATUS_OS_ACCESS_DENIED      (-1130)



/** 
 * AS offset out of range.
 */
#define PT_STATUS_AS_OFFSET_OUT_OF_RANGE      (-1131)



/** 
 * AS background out of range.
 */
#define PT_STATUS_AS_BG_OUT_OF_RANGE      (-1132)



/** 
 * General smart-card error
 */
#define PT_STATUS_SC_ERROR      (-3100)



/** 
 * Communication with this card is not supported
 */
#define PT_STATUS_SC_NOT_SUPPORTED      (-3101)



/** 
 * Failure during communication with the card
 */
#define PT_STATUS_SC_COMM_FAIL      (-3102)



/** 
 * Incorrect parameter detected
 */
#define PT_STATUS_SC_BAD_PARAM      (-3103)



/** 
 * the card is not present in the reader
 */
#define PT_STATUS_SC_NO_CARD      (-3104)




#endif /* #ifndef TFMERROR_H */

