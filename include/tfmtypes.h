/**
 * @file tfmtypes.h
 *
 * PTAPI types and definitions
 *
 * Copyright (C) 2001-2007 UPEK Inc.
 */

#ifndef tfmtypes_h
#define tfmtypes_h

#if defined(_MSC_VER)
#ifndef _DOS
#include <windows.h>
#endif
#endif

/*------------ miscellaneous -----------------------------------------------*/

/**
 * Linkage specification macro
 */
#if !defined(PTAPI_DLL)
    #if defined(BUILD_PTAPI_DLL)
        #if defined(_MSC_VER) 
            #if defined(PTAPI_STATIC)
                #if defined __cplusplus 
                    #define PTAPI_DLL extern "C" 
                #else
                    #define PTAPI_DLL
                #endif
            #else
                #if defined __cplusplus 
                    #define PTAPI_DLL extern "C"
                #else
                    #define PTAPI_DLL extern
                #endif
            #endif
        #elif defined(__SYMBIAN32__)
            #if defined(PTAPI_STATIC)
                #if defined(__cplusplus)
                    #define PTAPI_DLL extern "C"
                #else
                    #define PTAPI_DLL
                #endif
            #else
                #if defined(__cplusplus)
                    #define PTAPI_DLL extern "C" EXPORT_C
                #else
                    #define PTAPI_DLL EXPORT_C
                #endif
            #endif
        #else
            #if defined(__cplusplus)
                #define PTAPI_DLL extern "C"
            #else
                #define PTAPI_DLL
            #endif
        #endif
    #else
        #if defined(_MSC_VER)
            #if defined(PTAPI_STATIC)
                #if defined __cplusplus 
                    #define PTAPI_DLL extern "C"
                #else
                    #define PTAPI_DLL
                #endif
            #else    
                #if defined __cplusplus 
                    #define PTAPI_DLL extern "C" __declspec(dllimport)
                #else
                    #define PTAPI_DLL __declspec(dllimport)
                #endif
            #endif
        #elif defined(__SYMBIAN32__)
            #if defined(PTAPI_STATIC)
                #if defined(__cplusplus)
                    #define PTAPI_DLL extern "C"
                #else
                    #define PTAPI_DLL
                #endif
            #else
                #if defined(__cplusplus)
                    #define PTAPI_DLL extern "C" IMPORT_C
                #else
                    #define PTAPI_DLL IMPORT_C
                #endif
            #endif
        #else
            #if defined(__cplusplus)
                #define PTAPI_DLL extern "C"
            #else
                #define PTAPI_DLL
            #endif
        #endif
    #endif
#endif



/**
 * Macro for standard call convention
 */
#ifndef PTAPI
#if defined(_MSC_VER)
#if defined(_DOS)
#define PTAPI far
#else
#define PTAPI __stdcall
#endif
#else
#define PTAPI
#endif
#endif



/*
nothing doing macros used for better documenting parameter specification
*/

/** 
 * This is a definition which has sole purpose of 
 * helping readability.  It indicates that formal
 * parameter is an input parameter.
 */
#ifndef IN
#define IN
#endif

/** 
 * This is a definition which has sole purpose of 
 * helping readability.  It indicates that formal
 * parameter is an output parameter.  
 */
#ifndef OUT
#define OUT
#endif

/** 
 * This is a definition which has sole purpose of 
 * helping readability.  It indicates that formal
 * parameter is both input and output parameter.  
 */
#ifndef INOUT 
#define INOUT
#endif

/** 
 * This is a definition which has sole purpose of 
 * helping readability.  It indicates that formal
 * parameter is an optional parameter.  
 */
#ifndef OPTIONAL
#define OPTIONAL
#endif





/*------------ single types -------------------------------------------------*/



#ifdef _WIN32   /* Windows */

    
/** 
 * signed byte
 */
typedef char     PT_CHAR;


/** 
 * unsigned byte
 */
typedef unsigned char     PT_BYTE;


/** 
 * signed 2 bytes
 */
typedef signed short     PT_SHORT;


/** 
 * unsigned 2 bytes
 */
typedef unsigned short     PT_WORD;


/** 
 * signed 4 bytes
 */
typedef signed long     PT_LONG;


/** 
 * unsigned 4 bytes
 */
typedef unsigned long     PT_DWORD;



#elif defined(_DOS)     /* DOS */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(__ICCAVR__) /* AVR IAR C/C++ Compiler */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(__CW_FREESCALE_HC12__) /* CodeWarrior used for Freescale HC12 */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(_ARC)     /* ARC */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(__arm)    /* ARM */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(__linux__) /* Linux */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed int          PT_LONG;
typedef unsigned int        PT_DWORD;
#elif defined(__APPLE__)  &&  defined(__MACH__) /* Mac OS X */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed int          PT_LONG;
typedef unsigned int        PT_DWORD;
#elif defined(__SYMBIAN32__) /* Symbian */
typedef char                PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(_32BIT_EMBEDDED) /* for generic 32-bit embedded platform */
typedef signed char         PT_CHAR;
typedef unsigned char       PT_BYTE;
typedef signed short        PT_SHORT;
typedef unsigned short      PT_WORD;
typedef signed long         PT_LONG;
typedef unsigned long       PT_DWORD;
#elif defined(_LINT_TRACE_CHECK) /* for LINT check */
typedef signed long long    PT_CHAR;
typedef unsigned long long  PT_BYTE;
typedef signed long long    PT_SHORT;
typedef unsigned long long  PT_WORD;
typedef signed long long    PT_LONG;
typedef unsigned long long  PT_DWORD;
#else
#error "Unknown platform. Please add definition of basic PTAPI types (PT_CHAR, PT_BYTE, PT_SHORT, PT_WORD, PT_LONG and PT_DWORD)."
#endif



// Compile time checks of basic PTAPI types sizes
typedef char PT_CHAR_SIZE_CHECK__[sizeof(PT_CHAR) == 1 ? 1 : -1]; /* Error 'negative subscript' on this line means that PT_CHAR type is defined with wrong size (size of PT_CHAR must be 1 byte). */
typedef char PT_BYTE_SIZE_CHECK__[sizeof(PT_BYTE) == 1 ? 1 : -1]; /* Error 'negative subscript' on this line means that PT_BYTE type is defined with wrong size (size of PT_BYTE must be 1 byte). */
typedef char PT_SHORT_SIZE_CHECK__[sizeof(PT_SHORT) == 2 ? 1 : -1]; /* Error 'negative subscript' on this line means that PT_SHORT type is defined with wrong size (size of PT_SHORT must be 2 bytes). */
typedef char PT_WORD_SIZE_CHECK__[sizeof(PT_WORD) == 2 ? 1 : -1]; /* Error 'negative subscript' on this line means that PT_WORD type is defined with wrong size (size of PT_WORD must be 2 bytes). */
typedef char PT_LONG_SIZE_CHECK__[sizeof(PT_LONG) == 4 ? 1 : -1]; /* Error 'negative subscript' on this line means that PT_LONG type is defined with wrong size (size of PT_LONG must be 4 bytes). */
typedef char PT_DWORD_SIZE_CHECK__[sizeof(PT_DWORD) == 4 ? 1 : -1]; /* Error 'negative subscript' on this line means that PT_DWORD type is defined with wrong size (size of PT_DWORD must be 4 bytes). */


/** 
 * Boolean value (zero, non-zero)
 */
typedef PT_DWORD     PT_BOOL;

    
/** 
 * standard result value
 */
typedef PT_LONG     PT_STATUS;


/** 
 * connection to FM
 */
typedef PT_DWORD     PT_CONNECTION;


/**
 * Prototype of memory allocating function
 */

typedef void* (PTAPI *PT_MALLOC) (PT_DWORD Size);

/**
 * Prototype of memory freeing function
 */

typedef void (PTAPI *PT_FREE) (void *Memblock);


/**
 * Boolean false value
 */
#define	PT_FALSE		(0)

/**
 * Boolean true value
 */
#define	PT_TRUE			(1)


/** 
 * Extended initialization flags
 */

#define PT_INITEX_FLAG_NO_MSG_MONITOR    0x00000001    ///< Flag that PTAPI library will not monitor windows messages on its own (MS Windows platform only).
#define PT_INITEX_FLAG_NO_VCH    0x00000002    ///< No more supported.




/** 
 * The global information about this PTAPI library, especially the version info.
 */
typedef struct pt_global_info {
  PT_DWORD ApiVersion; 	///< Version of PTAPI. Highest byte = major version, second highest byte = minor version, low word = subversions
  PT_DWORD Functionality; 	///< Bitmask, defining which blocks of functions are supported. See PT_GIFUNC_xxxx. 
  PT_DWORD Flags; 	///< Additional flag bitmask (e.g. encryption strength) 
} PT_GLOBAL_INFO;

/** 
 * PT_GIFUNC_xxxxx
 * Global Information Flags
 */

#define PT_GIFUNC_STANDARD    0x00000001    ///< Standard embedded bio
#define PT_GIFUNC_SENSORONLY    0x00000002    ///< SONLY functionality
#define PT_GIFUNC_HYBRID    0x00000004    ///< Hybrid PTAPI interface
#define PT_GIFUNC_PARTNER    0x80000000    ///< Functions available only to partners
#define PT_GIFUNC_STONLY    0x40000000    ///< Functions available only to ST (manufacturer)

/** 
 * PT_GIFLAGS_xxxx
 */

#define PT_GIFLAGS_STRONGCRYPTO    0x00000001    ///< Strong cryptography is supported
#define PT_GIFLAGS_MATCH_ON_HOST    0x00000002    ///< Match-on-host functionality is supported
#define PT_GIFLAGS_CONST_AUTH_CHALLENGE    0x80000000    ///< Constant challenge for authentification (for testing purposes) functionality is supported




/** 
 * Definition of lengths of arrays in PT_DEVICE_LIST_ITEM
 */

#define DSNSUBSTR_MAXLEN    260
#define DISPLAYINFO_MAXLEN    128
#define OTHERINFO_MAXLEN    96



/** 
 * Item of the device info list
 */
typedef struct pt_device_list_item {
  PT_CHAR DisplayInfo[DISPLAYINFO_MAXLEN]; 	///< String with displayable device info
  PT_CHAR DsnSubString[DSNSUBSTR_MAXLEN]; 	///< String usable as part of DSN for PTOpen() to connect to this device
  PT_CHAR OtherInfo[OTHERINFO_MAXLEN]; 	///< String with other info
  PT_DWORD DeviceType; 	///< Device type
  PT_BYTE reserved[28]; 	///< Reserved for future use
} PT_DEVICE_LIST_ITEM;



/** 
 * The format of the data returned by PTEnumerateDevices, it contains info about all
 * enumerated devices. Please note, that the real output parameter from PTEnumerateDevices has 
 * variable length - array List[] has NumDevices items.
 */
typedef struct pt_device_list {
  PT_DWORD NumDevices; 	///< Number of devices in the list
  PT_DEVICE_LIST_ITEM List[1]; 	///< The list of devices, variable length
} PT_DEVICE_LIST;



/** 
 * Device notification handle.
 */
typedef PT_DWORD     PT_DEVNOTIFY;


/** 
 * 
 * Global device notification handle
 */

#define PT_DEV_NOTIFY_GLOBAL_HANDLE    0



/** 
 * 
 * Flags for PTDevNotifyInterrupt().
 */

#define PT_DEVNOTIFY_FLAG_CLEAR_INTERRUPT    0x00000001    ///< Clear interruption state of device notification.




/** 
 * 
 * Flags for PTCancel().
 */

#define PT_CANCEL_FLAG_CLEAR_CANCEL    0x00000001    ///< Clear session cancellation state.




/** 
 * The PT_DATA structure is used to associate any arbitrary long data block with the length information.
 */
typedef struct pt_data {
  PT_DWORD Length; 	///< Length of the Data field in bytes
  PT_BYTE Data[1]; 	///< The data itself, variable length
} PT_DATA;



/** 
 * This structure is used to hand over to PerfectTrust the set of memory
 * allocation/freeing routines, which will be then used for all dynamic memory management.
 */
typedef struct pt_memory_funcs {
  PT_MALLOC pfnMalloc; 	///< Memory allocating function
  PT_FREE pfnFree; 	///< Memory freeing function
} PT_MEMORY_FUNCS;



/** 
 * The header of the BIR. This type is exactly equivalent to BioAPI's BioAPI_BIR_HEADER.
 * Please refer to BioAPI documentation for details.
 * In the typical use the BIR is handled as an opaque data, it is not necessary to know the
 * structure of its header. However, we document it here for completeness. The values
 * provided below are the standard values used in BIRs produced by the FM.
 */
typedef struct pt_bir_header {
  PT_DWORD Length; 	///< Length of Header + Opaque Data
  PT_BYTE HeaderVersion; 	///< Current version is 1
  PT_BYTE Type; 	///< Type=4 (BioAPI_BIR_DATA_TYPE_PROCESSED)
  PT_WORD FormatOwner; 	///< FormatOwner=0x12 (STMicroelectronics)
  PT_WORD FormatID; 	///< FormatID=0
  PT_CHAR Quality; 	///< Quality=-2 (BioAPI_QUALITY is not supported)
  PT_BYTE Purpose; 	///< Purpose=3 (PT_PURPOSE_ENROLL)
  PT_DWORD FactorsMask; 	///< FactorsMask=0x08 (BioAPI_FACTOR_FINGERPRINT)
} PT_BIR_HEADER;



/** 
 * A container for biometric data. BIR = Biometric Identification Record. It typically
 * represents a fingerprint template, but potentially can contain other data as well, e.g. audit
 * data. BIR consists of a header, followed by the opaque data and optionally by a signature.
 * This type is binary compatible with BioAPI's BioAPI_BIR. The only difference is, that in
 * BioAPI_BIR the data is divided into four separate memory blocks, while PT_BIR keeps all
 * the data together.
 */
typedef struct pt_bir {
  PT_BIR_HEADER Header; 	///< 
  PT_BYTE Data[1]; 	///< Minimal version required
} PT_BIR;



/** 
 * A structure used to input a BIR to the API. Such input can be either the actual
 * BIR data, or one of the predefined handles to the template cache.
 */
typedef struct pt_input_bir {
  PT_BYTE byForm; 	///< Form type, can be PT_FULLBIR_INPUT or PT_SLOT_INPUT.
  union {
		PT_BIR *pBIR; ///< Used when byForm = PT_FULLBIR_INPUT */
		PT_LONG lSlotNr; ///< Used when byForm = PT_SLOT_INPUT */
		PT_BYTE abyReserved[20]; /** For future use */
	} InputBIR;
  } PT_INPUT_BIR;


#define PT_FULLBIR_INPUT    (3)    ///< Input is BIR data.
#define PT_SLOT_INPUT    (128)    ///< Input is a template stored in NVM in slot lSlotNr.




/** 
 * BIR and biometric operations purposes
 */

#define PT_PURPOSE_VERIFY    (1)
#define PT_PURPOSE_IDENTIFY    (2)
#define PT_PURPOSE_ENROLL    (3)
#define PT_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY    (4)
#define PT_PURPOSE_ENROLL_FOR_IDENTIFICATION_ONLY    (5)
#define PT_PURPOSE_AUDIT    (6)




/** 
 * Types of grabbed fingerprint images
 */

#define PT_GRAB_TYPE_UPPER_HALF    (0)
#define PT_GRAB_TYPE_BOTTOM_HALF    (1)
#define PT_GRAB_TYPE_THREE_QUARTERS_SUBSAMPLE    (2)
#define PT_GRAB_TYPE_381_381_8    (2)    ///< alias for PT_GRAB_TYPE_THREE_QUARTERS_SUBSAMPLE
#define PT_GRAB_TYPE_ONE_HALF_SUBSAMPLE    (3)
#define PT_GRAB_TYPE_254_254_8    (3)    ///< alias for PT_GRAB_TYPE_ONE_HALF_SUBSAMPLE
#define PT_GRAB_TYPE_THREE_QUARTERS_SUBSAMPLE_BINARIZED    (4)
#define PT_GRAB_TYPE_381_381_8_BINARIZED    (4)    ///< alias for PT_GRAB_TYPE_THREE_QUARTERS_SUBSAMPLE_BINARIZED
#define PT_GRAB_TYPE_508_254_8    (5)
#define PT_GRAB_TYPE_508_508_4    (6)
#define PT_GRAB_TYPE_381_381_4    (7)
#define PT_GRAB_TYPE_508_254_4    (8)
#define PT_GRAB_TYPE_254_254_4    (9)
#define PT_GRAB_TYPE_508_508_8_WIDTH208    (10)
#define PT_GRAB_TYPE_508_508_8_COMPRESS1    (11)
#define PT_GRAB_TYPE_508_508_4_SCAN4    (12)
#define PT_GRAB_TYPE_381_381_8_FAST    (13)
#define PT_GRAB_TYPE_508_254_4_SCAN4    (14)
#define PT_GRAB_TYPE_254_254_4_SCAN4    (15)
#define PT_GRAB_TYPE_381_381_4_FAST    (16)
#define PT_GRAB_TYPE_381_381_8_BINARIZED_FAST    (17)
#define PT_GRAB_TYPE_508_508_8_COMPRESS2    (18)
#define PT_GRAB_TYPE_381_381_8_SCAN381    (19)
#define PT_GRAB_TYPE_381_381_4_SCAN381    (20)
#define PT_GRAB_TYPE_381_381_8_BINARIZED_SCAN381    (21)
#define PT_GRAB_TYPE_381_381_8_LP    (22)    ///< Low power
#define PT_GRAB_TYPE_381_381_4_LP    (23)    ///< Low power
#define PT_GRAB_TYPE_381_381_8_BINARIZED_LP    (24)    ///< Low power
#define PT_GRAB_TYPE_381_381_8_VLP    (25)    ///< Very low power
#define PT_GRAB_TYPE_381_381_4_VLP    (26)    ///< Very low power
#define PT_GRAB_TYPE_381_381_8_BINARIZED_VLP    (27)    ///< Very low power
#define PT_GRAB_TYPE_381_381_8_SCAN381_381_4    (28)
#define PT_GRAB_TYPE_381_381_4_SCAN381_381_4    (29)
#define PT_GRAB_TYPE_381_381_8_BINARIZED_SCAN381_381_4    (30)
#define PT_GRAB_TYPE_381_381_8_SCAN381_254_4    (31)
#define PT_GRAB_TYPE_381_381_4_SCAN381_254_4    (32)
#define PT_GRAB_TYPE_381_381_8_BINARIZED_SCAN381_254_4    (33)
#define PT_GRAB_TYPE_508_508_8_SCAN508_508_8    (34)
#define PT_GRAB_TYPE_508_508_4_SCAN508_508_8    (35)
#define PT_GRAB_TYPE_508_508_8_BINARIZED_SCAN508_508_8    (36)
#define PT_GRAB_TYPE_508_508_8_SCAN4    (37)
#define PT_GRAB_TYPE_508_508_8_BINARIZED_SCAN4    (38)
#define PT_GRAB_TYPE_508_508_8_COMPRESS3    (39)
#define PT_GRAB_TYPE_508_254_8_LP    (40)
#define PT_GRAB_TYPE_508_254_4_LP    (41)
#define PT_GRAB_TYPE_381_381_8_FAST_LP    (42)
#define PT_GRAB_TYPE_381_381_4_FAST_LP    (43)
#define PT_GRAB_TYPE_381_381_8_BINARIZED_FAST_LP    (44)
#define PT_GRAB_TYPE_381_381_8_COMPRESS4    (45)
#define PT_GRAB_TYPE_381_381_8_SCAN254_381_8    (46)
#define PT_GRAB_TYPE_381_381_8_BINARIZED_SCAN254_381_8    (47)
#define PT_GRAB_TYPE_381_381_8_SCAN381_381_6    (48)    ///< Supports variable bandwidth, input format 381/381/6 or 254/381/6 depending on available bandwidth. Subject to 'ImageFormatPolicy' session parameter.
#define PT_GRAB_TYPE_381_381_8_SCAN254_381_6    (49)    ///< Supports variable bandwidth, input format fixed to 254/381/6. Subject to 'ImageFormatPolicy' session parameter.
#define PT_GRAB_TYPE_IMGFORMAT    (255)




/** 
 * Types of compression of grabbed fingerprint images
 */

#define PT_GRAB_COMPRESSION_TYPE_NONE    (0)    ///< No compression




/** 
 * Image format structure used by PTSetImageFormat to select image format for following biometric operations.
 */
typedef struct pt_imgformat {
  PT_WORD InXDpi; 	///< Input X resolution.
  PT_WORD InYDpi; 	///< Input Y resolution.
  PT_WORD OutXDpi; 	///< Output X resolution.
  PT_WORD OutYDpi; 	///< Output Y resolution.
  PT_BYTE InBpp; 	///< Input color depth.
  PT_BYTE OutBpp; 	///< Output Color depth.
  PT_WORD PowerMode; 	///< Scanning power mode.
  PT_WORD CompressMode; 	///< Data compression mode.
  PT_WORD Flags; 	///< Additional flags (see PT_IMGFORMAT_FLAG_xxxx values).
} PT_IMGFORMAT;

/** 
 * Image format additional flags
 */

#define PT_IMGFORMAT_FLAG_HWNAV    0x00000001    ///< Format is using HW navigation.



/** 
 * Image format info flags.
 */

#define PT_IMGFORMAT_INFO_ENABLED    0x00000001    ///< Image format is enabled.
#define PT_IMGFORMAT_INFO_CALIBRATED    0x00000002    ///< Image format is calibrated.
#define PT_IMGFORMAT_INFO_GRAB    0x00000004    ///< Image format can be used for grab operations.
#define PT_IMGFORMAT_INFO_BIOMETRIC    0x00000008    ///< Image format can be used for biometric operations (enroll and verification).
#define PT_IMGFORMAT_INFO_NAVIGATION    0x00000010    ///< Image format can be used for navigation operation.
#define PT_IMGFORMAT_INFO_VARIABLE_INPUT_FORMAT    0x00000020    ///< Image format has variable input format and is subject to 'ImageFormatPolicy' session configuration parameter.
#define PT_IMGFORMAT_INFO_VARIABLE_BANDWIDTH    0x00000040    ///< Image format has variable bandwidth and is subject to 'ImageFormatPolicy' session configuration parameter.




/** 
 * Image format list item structure.
 */
typedef struct pt_imgformat_list_item {
  PT_IMGFORMAT Format; 	///< Image format structure.
  PT_DWORD InfoFlags; 	///< Image format info flags (see PT_IMGFORMAT_INFO_xxxx values).
  PT_DWORD DataSections; 	///< Bit mask of available data sections for this format (see PT_IMGDATA_SECTION_xxxx values).
  PT_DWORD SupportMask; 	///< Image format support mask. Can be used to enable this format in FW configuration.
} PT_IMGFORMAT_LIST_ITEM;




/** 
 * The format of the data returned by PTListImageFormats, it contains info about available image formats.
 * Note that real version and size of elements in the list is given by fields 'ItemVersion' and 'Item'Size'.
 */
typedef struct pt_imgformat_list {
  PT_DWORD NumFormats; 	///< Number of formats returned in the list.
  PT_WORD ItemVersion; 	///< Version of returned list item structures, currently 1.
  PT_WORD ItemSize; 	///< Size in bytes of one returned list item structure, currently sizeof(PT_IMGFORMAT_LIST_ITEM).
  PT_IMGFORMAT_LIST_ITEM List[1]; 	///< List of image formats. Number of elements in the array is 'NumFormats'. Size of one item is 'ItemSize'.
} PT_IMGFORMAT_LIST;




/** 
 * Operation type for PTSetImageFormat/PTGetImageFormat.
 */

#define PT_IMGFORMAT_OPER_GRAB    0    ///< Image format set for grab operations.
#define PT_IMGFORMAT_OPER_BIOMETRIC    1    ///< Image format set for biometric operations (enroll and verification).
#define PT_IMGFORMAT_OPER_NAVIGATION    2    ///< Image format set for navigation.



/** 
 * Types of image data section.
 */

#define PT_IMGDATA_SECTION_SKEW    0x00000001    ///< Return skew info data section.
#define PT_IMGDATA_SECTION_SPEED    0x00000002    ///< Return speed info data section.
#define PT_IMGDATA_SECTION_GRADIENT    0x00000004    ///< Return gradient data section.
#define PT_IMGDATA_SECTION_STRIATION_REMOVAL    0x00000008    ///< Return striation removal section.
#define PT_IMGDATA_SECTION_NATIVE    0x00000010    ///< Return native data section.




/** 
 * Format selection options for PTListImageFormats.
 */

#define PT_IMGFORMAT_LIST_CALIBRATED    0    ///< Return list of calibrated formats only.
#define PT_IMGFORMAT_LIST_ENABLED    1    ///< Return list of enabled formats only.
#define PT_IMGFORMAT_LIST_IMPLEMENTED    2    ///< Return list of all implemented formats.




/** 
 * Types of data sections in Dynamic Image Format.
 */

#define PT_DIF_SECTTYPE_IMAGE_HEADER    0    ///< Image data header section.
#define PT_DIF_SECTTYPE_IMAGE_DATA    1    ///< Image data section.
#define PT_DIF_SECTTYPE_SKEW    2    ///< Skew data section.
#define PT_DIF_SECTTYPE_SPEED    3    ///< Speed data section.
#define PT_DIF_SECTTYPE_GRADIENT    4    ///< Gradient data section.
#define PT_DIF_SECTTYPE_STRIATION_REMOVAL    5    ///< Striation removal data section.
#define PT_DIF_SECTTYPE_NATIVE    6    ///< Native data of 3rd party sensors and technologies.



/** 
 * Header of Dynamic Image Format.
 */
typedef struct pt_dif_header {
  PT_DWORD Length; 	///< Complete length of the whole data.
  PT_WORD Version; 	///< Header format version, currently 1.
  PT_WORD NumSections; 	///< Number of contained data sections.
} PT_DIF_HEADER;



/** 
 * Header of Dynamic Image Format Section.
 */
typedef struct pt_dif_section_header {
  PT_DWORD Length; 	///< Complete length of the section.
  PT_DWORD SectionType; 	///< Section type (one of PT_DIF_SECTTYPE_xxxx values).
} PT_DIF_SECTION_HEADER;

  
/** 
 * Image data header section of Dynamic Image Format.
 */
typedef struct pt_dif_section_image_header {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_WORD InXDpi; 	///< Input (scanner) X resolution. 0=unknown.
  PT_WORD InYDpi; 	///< Input (scanner) Y resolution. 0=unknown.
  PT_WORD OutXDpi; 	///< Output (image) X resolution. 0=unknown.
  PT_WORD OutYDpi; 	///< Output (image) Y resolution. 0=unknown.
  PT_BYTE InBpp; 	///< Input (scanner) color depth. 0=unknown.
  PT_BYTE OutBpp; 	///< Output (image) color depth. 0=unknown.
  PT_WORD ImagePixelWidth; 	///< Image Width in output pixels.
  PT_WORD ImagePixelHeight; 	///< Image Height in output pixels.
  PT_WORD ImageByteWidth; 	///< Image Width in bytes. 0 if not provided.
  PT_WORD FingerprintMaxWidth; 	///< Maximum fingerprint width in pixels.
  PT_WORD FingerprintHeight; 	///< Fingerprint height in pixels.
  PT_DWORD PackingAndCompressType; 	///< Packaging and compression type of data in the IMAGE DATA section.
  PT_DWORD RecErrorStates; 	///< Bitmask of reconstruction error states (like too skewed, too fast, ...).
  PT_DWORD RecFlags; 	///< Reconstruction flags. See PT_DIFREC_FLAG_xxxx constants.
  PT_WORD RoiPos; 	///< Region of interest position. 0 if not known.
  PT_WORD RoiLength; 	///< Region of interest length. 0 if not known.
  PT_WORD BackgroundColor; 	///< Background color. 0xffff if not known.
  PT_BYTE RecScore; 	///< Reconstruction score (percents). 255 if not known.
  PT_BYTE QualScore; 	///< Image quality score (percents). 255 if not known.
  PT_DWORD QualMessage; 	///< GUI message resulting from image quality test (see PT_GUIMSG_xxxx), 0xffffffff if not known.
  PT_DWORD ExtendedVendorCode; 	///< Info about the sensor used to capture the image.
  PT_DWORD Flags; 	///< Additional flags (not used yet).
  PT_BYTE reserved[4]; 	///< Reserved for future use.
} PT_DIF_SECTION_IMAGE_HEADER;



/** 
 * Image data section of Dynamic Image Format.
 */
typedef struct pt_dif_section_image_data {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_BYTE Data[1]; 	///< Image data.
} PT_DIF_SECTION_IMAGE_DATA;



/** 
 * Skew data section of Dynamic Image Format.
 */
typedef struct pt_dif_section_skew {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_BYTE Data[1]; 	///< Skew data.
} PT_DIF_SECTION_SKEW;



/** 
 * Speed data section of Dynamic Image Format.
 */
typedef struct pt_dif_section_speed {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_BYTE Data[1]; 	///< Speed data.
} PT_DIF_SECTION_SPEED;



/** 
 * Gradient data section of Dynamic Image Format.
 */
typedef struct pt_dif_section_gradient {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_BYTE Data[1]; 	///< Gradient data.
} PT_DIF_SECTION_GRADIENT;



/** 
 * Striation removal data section of Dynamic Image Format.
 */
typedef struct pt_dif_section_striation_removal {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_BYTE Data[1]; 	///< Striation removal data.
} PT_DIF_SECTION_STRIATION_REMOVAL;



/** 
 * Native data section of Dynamic Image Format.
 */
typedef struct pt_dif_section_native {
  PT_DIF_SECTION_HEADER Header; 	///< Header of the section.
  PT_BYTE Data[1]; 	///< Native data.
} PT_DIF_SECTION_NATIVE;




/** 
 * Reconstruction flags returned in header of dynamic image format
 */

#define PT_DIFREC_FLAG_SRA_TD    0x00000001    ///< Striation removal Top-Down.
#define PT_DIFREC_FLAG_SRA_BU    0x00000002    ///< Striation removal Bottom-Up.
#define PT_DIFREC_FLAG_SKEW    0x00000004    ///< Skew compensation.
#define PT_DIFREC_FLAG_GRADIENT    0x00000008    ///< Gradient compensation.
#define PT_DIFREC_FLAG_DGAIN    0x00000010    ///< Digital gain appliance.
#define PT_DIFREC_FLAG_ROBUST_MODE    0x00010000    ///< Robust reconstruction mode.
#define PT_DIFREC_FLAG_INV_SWIPE    0x00020000    ///< Inverted swipe direction.




/** 
 * Flags for PTDetectFingerEx
 */

#define PT_DETECT_GUI    0x00000001    ///< Display GUI
#define PT_DETECT_PURPOSE_MASK    0x000000fe    ///< Finger detection purpose mask, can be one of PT_DETECT_PURPOSE_xxxx values
#define PT_DETECT_PURPOSE_TOUCH    0x00000000    ///< Detection of simple finger touch
#define PT_DETECT_PURPOSE_LIFT    0x00000002    ///< Detection of finger lift
#define PT_DETECT_PURPOSE_QUALITY_TOUCH    0x00000004    ///< Detection of finger touch with acceptable image quality (area sensors only)
#define PT_DETECT_PURPOSE_LONG_TOUCH    0x00000006    ///< Detection of long finger touch (SONLY only)
#define PT_DETECT_PURPOSE_SWIPE    0x00000008    ///< Detection of finger swipe (strip sensors SONLY only)
#define PT_DETECT_PURPOSE_GRAB    0x0000000a    ///< Detection of beginning of grab operation (SONLY only)
#define PT_DETECT_NO_FINGER    PT_DETECT_PURPOSE_LIFT    ///< Alias for PT_DETECT_PURPOSE_LIFT
#define PT_DETECT_ACCEPTABLE    PT_DETECT_PURPOSE_QUALITY_TOUCH    ///< Alias for PT_DETECT_PURPOSE_QUALITY_TOUCH




/** 
 * Maximum length of finger data
 */

#define PT_MAX_FINGER_DATA_LENGTH    (96)    ///< Max. length of public data associated with a fingerprint in the FM's internal database in NVM.



/** 
 * The format of the data returned by PTListAllFingers as the info about all finger templates 
 */
typedef struct pt_finger_list_item {
  PT_LONG SlotNr; 	///< ID of slot, where is the finger stored
  PT_DWORD FingerDataLength; 	///< Length of data associated with the finger
  PT_BYTE FingerData[PT_MAX_FINGER_DATA_LENGTH]; 	///< Data associated with the finger
} PT_FINGER_LIST_ITEM;




/** 
 * The format of the data returned by PTListAllFingers as the info about all finger templates
 * stored in the FM's internal database in NVM. Please note, that the real output parameter
 * from PTListAllFingers has a variable length, array List[] has NumFingers items.
 */
typedef struct pt_finger_list {
  PT_DWORD NumFingers; 	///< Number of fingers in the list
  PT_FINGER_LIST_ITEM List[1]; 	///< Array of the fingers, it has NumFingers items
} PT_FINGER_LIST;




/** 
 * A structure used by Info to return status-independent
 * information about FM.
 */
typedef struct pt_info {
  PT_DWORD FwVersion; 	///< Version of the FM firmware; highest byte = major version, second highest byte = minor version, low word = subversions/revisions.
  PT_DWORD FwMinNextVersion; 	///< Minimal version required for future firmware update
  PT_DWORD FwVariant; 	///< Variant of firmware. Standard TFM variant is 0, standard ESS/TCD50 variant is 2.
  PT_DWORD FwFunctionality; 	///< Blocks of functionality included in firmware. See PT_FWFUNC_xxxx.
  PT_DWORD FwConfig; 	///< FW's configuration flags, set up during manufacturing See PT_FWCFG_xxxx
  PT_DWORD Id; 	///< If used, allows to assign unique ID to every FM piece. Otherwise 0.
  PT_DWORD AuthentifyId; 	///< ID of the Authentify group, used in PTAuthentify, value 0 means that authentication is not required.
  PT_DWORD Usage; 	///< Type of the reader in which the FM is used. 0 = unspecified usage (for details see PT_USAGE_xxxx).
  PT_DWORD SensorType; 	///< Type and version of the sensor.
  PT_WORD ImageWidth; 	///< Standard image width. Note that this value doesn't apply to width of dynamic image format. Also note that this value can be different from real sensor width.
  PT_WORD ImageHeight; 	///< Standard image height (for strip sensor the max. image height, see description of PTGrab). Note that this value doesn't apply to height of dynamic image format.
  PT_DWORD MaxGrabWindow; 	///< TFM: Max. guaranteed length of the output data for PTGrabWindow. Can be 0 -> the guaranteed length is not defined. ESS: Reserved for future use.
  PT_DWORD CompanionVendorCode; 	///< Companion's chip vendor code
} PT_INFO;

/** 
 * Functionality flags
 */

#define PT_FWFUNC_STANDARD    0x00000001    ///< Std. embedded biometrics
#define PT_FWFUNC_EXTEEPROM    0x00000002    ///< Supports external EEPROM as a fingerprint template store
#define PT_FWFUNC_ANTISPOOF    0x00000004    ///< Antispoofing support
#define PT_FWFUNC_STRIP    0x00000008    ///< Strip sensor support (if missing, area sensor support is included)
#define PT_FWFUNC_SC    0x00000010    ///< Smart card reader support
#define PT_FWFUNC_FINGER_DETECT    0x00000020    ///< Hardware finger detect support
#define PT_FWFUNC_TURBOMODE    0x00000040    ///< Turbomode support
#define PT_FWFUNC_SECCHAN_V2    0x00000080    ///< Secure channel protocol version 2 support
#define PT_FWFUNC_SECCHAN_V3    0x00000100    ///< Secure channel protocol version 3 support
#define PT_FWFUNC_SMART_NVM    0x00001000    ///< Smart NVM support
#define PT_FWFUNC_SIO    0x00010000    ///< Serial line support
#define PT_FWFUNC_USB    0x00020000    ///< USB support
#define PT_FWFUNC_SPI    0x00040000    ///< SPI communication support
#define PT_FWFUNC_HECI    0x00080000    ///< HECI communication support
#define PT_FWFUNC_ME    0x40000000    ///< ME device functionality
#define PT_FWFUNC_SONLY    0x80000000    ///< SONLY functionality

/** 
 * Firmware configuration flags
 */

#define PT_FWCFG_EXTEEPROM    0x00000001    ///< Use external EEPROM as a fingerprint template store
#define PT_FWCFG_LED    0x00000002    ///< LED user interface is enabled
#define PT_FWCFG_SC    0x00000004    ///< The smartcard reader module is enabled. Please note that as TFM does not have this bit, its smardcard reader cannot be disabled.
#define PT_FWCFG_ANTISPOOF    0x00000008    ///< Antispoofing support is enabled
#define PT_FWCFG_AWAKE    0x00000010    ///< ESS only: AWAKE (DSR) signal is enabled. AWAKE is used to signal to the host when FM is sleeping and when is awake.
#define PT_FWCFG_POA    0x00000020    ///< Power On Authentication is enabled.
#define PT_FWCFG_NOSLEEP    0x00004000    ///< Do not use the RTS/WAKEUP signal as wakeup. Consequently, if serial line is used, do not enter sleep when session is terminated
#define PT_FWCFG_NORTS    0x00008000    ///< Do not use the RTS/WAKEUP signal for serial line RTS handshaking. TFM only: On TFM, the RTS/WAKEUP signal is shared with GPIO0 signal. If both PT_FWCFG_NORTS and PT_FWCFG_NOSLEEP are set, the RTS/WAKEUP signal is not used and the GPIO0 signal is free for other use.
#define PT_FWCFG_SIO    0x00010000    ///< Use serial line communication channel
#define PT_FWCFG_USB    0x00020000    ///< Use USB communication channel. Note: If both PT_FWCFG_SIO and PT_FWCFG_USB is defined, perform comm. channel detect during startup
#define PT_FWCFG_SPI    0x00040000    ///< Use SPI communication channel.
#define PT_FWCFG_HECI    0x00080000    ///< Use HECI communication channel.
#define PT_FWCFG_CFGLOCKED    0x80000000    ///< Firmware configuration cannot be changed anymore
#define PT_FWCFG_FWLOCKED    0x40000000    ///< Firmware cannot be updated anymore

/** 
 * Sensor usage flags
 */

#define PT_USAGE_INTERNAL    0x80000000    ///< Internal sensor.
#define PT_USAGE_EXTERNAL    0x40000000    ///< External sensor.
#define PT_USAGE_LED_MASK    0x000000ff    ///< Bit array mask for specification of a LED policy which should be used by the application (see PT_USAGE_LED_xxxx values for individual policies).
#define PT_USAGE_LED_READER    0x00000000    ///< Application should use PT_LED_MODE_READER automatic behavior.
#define PT_USAGE_LED_BIO    0x00000001    ///< Application should use PT_LED_MODE_BIO automatic behavior.
#define PT_USAGE_LED_LENOVO    0x00000002    ///< Application should use the 'Lenovo' policy combined with PT_LED_MODE_READER automatic behavior.
#define PT_USAGE_LED_LENOVO_CHINA    0x00000003    ///< Application should use the 'Lenovo China' policy combined with PT_LED_MODE_READER automatic behavior.
#define PT_USAGE_LED_LENOVO_CHINA2    0x00000004    ///< Application should use the 'Lenovo China' policy (alternative) combined with PT_LED_MODE_READER automatic behavior.
#define PT_USAGE_LED_EIKON_TOUCH    0x00000005    ///< Application should use the 'Eikon Touch' policy combined with PT_LED_MODE_READER automatic behavior.
#define PT_USAGE_LED_UNKNOWN    0x000000ff    ///< The policy is unknown (not set yet), LEDs are turned off.

/** 
 * Sensor type flags
 */

#define PT_SENSORBIT_STRIP_SENSOR    0x80000000    ///< Strip sensor
#define PT_SENSORBIT_TURBO_MODE    0x40000000    ///< Turbo mode is activated (EIM Lite or EIM Full mode for area sensors)
#define PT_SENSORBIT_CALIBRATED    0x20000000    ///< Calibration for full power formats was done
#define PT_SENSORBIT_CALIBRATED_LP    0x10000000    ///< Calibration for low power formats was done
#define PT_SENSORBIT_CALIBRATED_MP    0x08000000    ///< Calibration for medium power formats was done
#define PT_SENSORBIT_CALIBRATED_381_381_8    0x04000000    ///< Calibration for 381/381/8 formats was done
#define PT_SENSORBIT_CALIBRATED_508_508_8    0x02000000    ///< Calibration for 508/508/8 formats was done
#define PT_SENSORBIT_CALIBRATED_508_254_8    0x01000000    ///< Calibration for 508/254/8 formats was done
#define PT_SENSORBIT_CALIBRATED_254_254_8    0x00800000    ///< Calibration for 254/254/8 formats was done
#define PT_SENSORBIT_CALIBRATED_381_381_4    0x00400000    ///< Calibration for 381/381/4 formats was done
#define PT_SENSORBIT_CALIBRATED_381_254_4    0x00200000    ///< Calibration for 381/254/4 formats was done
#define PT_SENSORBIT_CALIBRATED_381_381_8_LP    0x00100000    ///< Calibration for 381/381/8 low-power formats was done
#define PT_SENSORBIT_CALIBRATED_FULL_EIM    0x00080000    ///< EIM Full mode is actived for area sensor
#define PT_SENSORBIT_CALIBRATED_SCAN_6BPP    0x00040000    ///< Calibration for formats scanning in 6 bpp was done




#define PT_GUID_SIZE    16    ///< Size of GUID



#define PT_SYSTEM_NAME_MAX_LEN    64    ///< Maximal possible length of SystemName string



/** 
 * A structure used by PTExtendedInfo to return more status-independent information about FM (version 1).
 */
typedef struct pt_extended_info_v1 {
  PT_DWORD Version; 	///< Version of this structure (1)
  PT_BYTE GUID[PT_GUID_SIZE]; 	///< TFM/ESS GUID
  PT_DWORD SystemId; 	///< System identification bits
  PT_CHAR SystemName[PT_SYSTEM_NAME_MAX_LEN]; 	///< System identification name
} PT_EXTENDED_INFO_V1;



/** 
 * By default PT_EXTENDED_INFO is defined as version 1
 */
typedef PT_EXTENDED_INFO_V1     PT_EXTENDED_INFO;



/** 
 * A structure used by PTExtendedInfo to return additional information about FM (version 2).
 */
typedef struct pt_extended_info_v2 {
  PT_DWORD Version; 	///< Version of this structure (2)
  PT_BYTE GUID[PT_GUID_SIZE]; 	///< TFM/ESS GUID
  PT_DWORD SystemId; 	///< System identification bits
  PT_CHAR SystemName[PT_SYSTEM_NAME_MAX_LEN]; 	///< System identification name
  PT_DWORD FwFunctionality2; 	///< Additional flags of supported functionality (see PT_FWFUNC2_xxxx)
  PT_DWORD BioLibVersion; 	///< Version of used BioLib
  PT_DWORD TemplateTypeFlags; 	///< Flags of all supported template formats (see PT_TEMPLATEBIT_xxxx)
  PT_WORD FwUpdateCount; 	///< Number of performed firmware updates
  PT_WORD SensorVariant; 	///< Sensor variant info (currently: 0 = polyimide coating, 1 = screenprint coating)
  PT_DWORD ReaderVersion; 	///< Firmware version for non-chipset devices with firmware (0 for chipsets and devices without firmware)
  PT_BYTE LedInfo; 	///< Additional info about LED support (see PT_LEDINFO_xxxx values).
  PT_BYTE reserve[7]; 	///< Reserve, currently set to 0.
} PT_EXTENDED_INFO_V2;



  
/** 
 * Additional firmware functionality flags.
 */

#define PT_FWFUNC2_EMULATED_NVM    0x00000001    ///< Device uses emulated NVM.
#define PT_FWFUNC2_SECURID    0x00000002    ///< SecurID algorithm supported.
#define PT_FWFUNC2_DYNAMIC_ENROLLMENT    0x00000004    ///< Dynamic enrollment supported.
#define PT_FWFUNC2_DYNAMIC_IMAGE_FORMAT    0x00000008    ///< Dynamic image format supported.
#define PT_FWFUNC2_POA    0x00000010    ///< Power On Authentication supported.
#define PT_FWFUNC2_READER_SECURITY    0x00000020    ///< Reader security is supported.
#define PT_FWFUNC2_SMART_FDET    0x00000040    ///< Smart finger detect is supported.




/** 
 * Supported template format flags
 */

#define PT_TEMPLATEBIT_ACCEPT_LEGACY    0x01    ///< Support for accepting LEGACY templates
#define PT_TEMPLATEBIT_EXTRACT_LEGACY    0x02    ///< Support for extracting and conversion to LEGACY templates
#define PT_TEMPLATEBIT_ACCEPT_ALPHA    0x04    ///< Support for accepting ALPHA templates
#define PT_TEMPLATEBIT_EXTRACT_ALPHA    0x08    ///< Support for extracting and conversion to ALPHA templates
#define PT_TEMPLATEBIT_ACCEPT_BETA    0x10    ///< Support for accepting BETA templates
#define PT_TEMPLATEBIT_EXTRACT_BETA    0x20    ///< Support for extracting and conversion to BETA templates
#define PT_TEMPLATEBIT_ACCEPT_ALPHA_MULTI    0x40    ///< Support for accepting ALPHA multi-templates
#define PT_TEMPLATEBIT_EXTRACT_ALPHA_MULTI    0x80    ///< Support for extracting and conversion to ALPHA multi-templates




/** 
 * LED support additional info
 */

#define PT_LEDINFO_VARIANT_MASK    0x0f    ///< Mask for LED support variant (values PT_LEDINFO_VARIANT_xxxx)
#define PT_LEDINFO_VARIANT_CHIPSET_BASIC    0x00    ///< Basic chipset LED support
#define PT_LEDINFO_VARIANT_SONLY_BASIC    0x01    ///< Basic SONLY LED support
#define PT_LEDINFO_VARIANT_EXTENDED    0x02    ///< Extended LED support (supported PTSetLedEx and PTGetLedEx)




/** 
 * Accounts
 */

#define PT_ACCOUNT_ANONYMOUS    0    ///< Anonymous account
#define PT_ACCOUNT_1    1    ///< Account 1
#define PT_ACCOUNT_2    2    ///< Account 2
#define PT_ACCOUNT_3    3    ///< Account 3
#define PT_ACCOUNT_4    4    ///< Account 4
#define PT_ACCOUNT_5    5    ///< Account 5
#define PT_ACCOUNT_6    6    ///< Account 6
#define PT_ACCOUNT_7    7    ///< Account 7
#define PT_ACCOUNT_8    8    ///< Account 8
#define PT_ACCOUNT_9    9    ///< Account 9
#define PT_ACCOUNT_10    10    ///< Account 10
#define PT_ACCOUNT_11    11    ///< Account 11
#define PT_ACCOUNT_12    12    ///< Account 12
#define PT_ACCOUNT_13    13    ///< Account 13
#define PT_ACCOUNT_14    14    ///< Account 14
#define PT_ACCOUNT_15    15    ///< Account 15
#define PT_ACCOUNT_16    16    ///< Account 16
#define PT_ACCOUNT_17    17    ///< Account 17
#define PT_ACCOUNT_18    18    ///< Account 18
#define PT_ACCOUNT_19    19    ///< Account 19
#define PT_ACCOUNT_WRP    (-1)    ///< Wrapped profile account



/** 
 * Access rights flags
 */

#define PT_ACC_ENABLE_SCANNER    (0)
#define PT_ACC_ENABLE_ENROLL    (1)
#define PT_ACC_ENABLE_VERIFY_CAPTURED    (2)
#define PT_ACC_ENABLE_VERIFY_MATCH    (3)
#define PT_ACC_ENABLE_USE_NVM_TEMPLATE    (4)
#define PT_ACC_ENABLE_USE_EXTERN_TEMPLATE    (5)
#define PT_ACC_ENABLE_UPDATE_NVM_TEMPLATE    (6)
#define PT_ACC_ENABLE_GET_TEMPLATE_WITH_PAYLOAD    (7)
#define PT_ACC_ENABLE_GET_TEMPLATE_WITHOUT_PAYLOAD    (8)
#define PT_ACC_ENABLE_NAVIGATION    (9)
#define PT_ACC_ENABLE_SET_APP_DATA_APP    (10)
#define PT_ACC_ENABLE_SET_APP_DATA_OEM    (11)
#define PT_ACC_ENABLE_SET_APP_DATA_PARTNER    (12)
#define PT_ACC_ENABLE_GET_APP_DATA_APP    (13)
#define PT_ACC_ENABLE_GET_APP_DATA_OEM    (14)
#define PT_ACC_ENABLE_GET_APP_DATA_PARTNER    (15)
#define PT_ACC_ENABLE_OTHER_FUNCS    (16)
#define PT_ACC_ENABLE_MANUFACTURING_FUNCS    (17)
#define PT_ACC_ENABLE_SET_AUTHKEY_1    (18)
#define PT_ACC_ENABLE_SET_AUTHKEY_2    (19)
#define PT_ACC_ENABLE_SET_AUTHKEY_3    (20)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_ANONYMOUS    (21)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_1    (22)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_2    (23)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_3    (24)
#define PT_ACC_ENABLE_GET_VERIFY_MATCH_PAYLOAD    (25)
#define PT_ACC_ENABLE_SECURE_CHANNEL    (26)
#define PT_ACC_ENABLE_ANTISPOOFING    (27)
#define PT_ACC_ENABLE_CHANGE_ANTISPOOFING_LEVEL    (28)
#define PT_ACC_ENABLE_GET_ANTISPOOFING_INFO    (29)
#define PT_ACC_ENABLE_OTP_SET_SECRET_KEY    (30)
#define PT_ACC_ENABLE_OTP_SET_USER_KEY    (31)
#define PT_ACC_ENABLE_OTP_SET_SEQUENCE_NUMBER    (32)
#define PT_ACC_ENABLE_OTP_GENERATE    (33)
#define PT_ACC_ENABLE_SET_FINGER_PAYLOAD    (34)
#define PT_ACC_ENABLE_UPDATE_FINGER_PAYLOAD    (35)
#define PT_ACC_ENABLE_ACCESS_FPDATAEX    (36)
#define PT_ACC_ENABLE_CONTROL_LMD    (37)
#define PT_ACC_ENABLE_USE_WRAPPED_TEMPLATE    (38)
#define PT_ACC_ENABLE_UPDATE_WRAPPED_TEMPLATE    (39)
#define PT_ACC_ENABLE_CREATE_WRAPPED_PROFILE    (40)
#define PT_ACC_ENABLE_SET_APP_DATA_APP_EX    (41)
#define PT_ACC_ENABLE_SET_APP_DATA_CERT_KEYEX    (42)
#define PT_ACC_ENABLE_SET_APP_DATA_CERT_SIGN    (43)
#define PT_ACC_ENABLE_SET_APP_DATA_PUBKEY_KEYEX    (44)
#define PT_ACC_ENABLE_SET_APP_DATA_PUBKEY_SIGN    (45)
#define PT_ACC_ENABLE_GET_APP_DATA_APP_EX    (46)
#define PT_ACC_ENABLE_GET_APP_DATA_CERT_KEYEX    (47)
#define PT_ACC_ENABLE_GET_APP_DATA_CERT_SIGN    (48)
#define PT_ACC_ENABLE_GET_APP_DATA_PUBKEY_KEYEX    (49)
#define PT_ACC_ENABLE_GET_APP_DATA_PUBKEY_SIGN    (50)
#define PT_ACC_ENABLE_SET_DEVICE_KEY_KEYEX    (51)
#define PT_ACC_ENABLE_SET_DEVICE_KEY_SIGN    (52)
#define PT_ACC_ENABLE_GENERATE_DEVICE_KEY_KEYEX    (53)
#define PT_ACC_ENABLE_GENERATE_DEVICE_KEY_SIGN    (54)
#define PT_ACC_ENABLE_USE_LMD     (55)
#define PT_ACC_ENABLE_SET_AUTHKEY_4    (64)
#define PT_ACC_ENABLE_SET_AUTHKEY_5    (65)
#define PT_ACC_ENABLE_SET_AUTHKEY_6    (66)
#define PT_ACC_ENABLE_SET_AUTHKEY_7    (67)
#define PT_ACC_ENABLE_SET_AUTHKEY_8    (68)
#define PT_ACC_ENABLE_SET_AUTHKEY_9    (69)
#define PT_ACC_ENABLE_SET_AUTHKEY_10    (70)
#define PT_ACC_ENABLE_SET_AUTHKEY_11    (71)
#define PT_ACC_ENABLE_SET_AUTHKEY_12    (72)
#define PT_ACC_ENABLE_SET_AUTHKEY_13    (73)
#define PT_ACC_ENABLE_SET_AUTHKEY_14    (74)
#define PT_ACC_ENABLE_SET_AUTHKEY_15    (75)
#define PT_ACC_ENABLE_SET_AUTHKEY_16    (76)
#define PT_ACC_ENABLE_SET_AUTHKEY_17    (77)
#define PT_ACC_ENABLE_SET_AUTHKEY_18    (78)
#define PT_ACC_ENABLE_SET_AUTHKEY_19    (79)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_4    (80)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_5    (81)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_6    (82)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_7    (83)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_8    (84)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_9    (85)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_10    (86)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_11    (87)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_12    (88)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_13    (89)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_14    (90)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_15    (91)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_16    (92)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_17    (93)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_18    (94)
#define PT_ACC_ENABLE_SET_ACCESS_RIGHTS_19    (95)
#define PT_ACC_ENABLE_USE_NVM_SAB    (96)
#define PT_ACC_ENABLE_UPDATE_NVM_SAB    (97)
#define PT_ACC_ENABLE_USE_WRAPPED_SAB    (98)
#define PT_ACC_ENABLE_UPDATE_WRAPPED_SAB    (99)
#define PT_ACC_ENABLE_USE_DEVICE_KEY_KEYEX    (100)
#define PT_ACC_ENABLE_USE_DEVICE_KEY_SIGN    (101)
#define PT_ACC_ENABLE_SET_APP_DATA_CERT_SECCHAN    (102)
#define PT_ACC_ENABLE_SET_APP_DATA_PUBKEY_SECCHAN    (103)
#define PT_ACC_ENABLE_GET_APP_DATA_CERT_SECCHAN    (104)
#define PT_ACC_ENABLE_GET_APP_DATA_PUBKEY_SECCHAN    (105)
#define PT_ACC_ENABLE_SET_DEVICE_KEY_SECCHAN    (106)
#define PT_ACC_ENABLE_GENERATE_DEVICE_KEY_SECCHAN    (107)
#define PT_ACC_ENABLE_USE_DEVICE_KEY_SECCHAN    (108)
#define PT_ACC_ENABLE_ATRBOP_PKI    (109)
#define PT_ACC_ENABLE_ATRBOP_OTP    (110)
#define PT_ACC_ENABLE_CONTROL_POA    (111)
#define PT_ACC_ENABLE_CHECK_LATENT    (112)




/**
 * Macro for setting one of the access rights bit.
 * pAccessRights is a pointer to access rights byte array.
 * Bit is the number of access right to be set.
 */
#define SET_ACCESS_RIGHTS_BIT(pAccessRights, Bit)    (((PT_BYTE*)pAccessRights)[Bit>>3] |= (1 << (Bit&7)))

/**
 * Macro for clearing one of the access rights bit.
 * pAccessRights is a pointer to access rights byte array.
 * Bit is the number of access right to be cleared.
 */
#define CLEAR_ACCESS_RIGHTS_BIT(pAccessRights, Bit)  (((PT_BYTE*)pAccessRights)[Bit>>3] &= ~(1 << (Bit&7)))

/**
 * Macro for getting state of one of the access rights bit
 * pAccessRights is a pointer to access rights byte array.
 * Bit is the number of access right to be read.
 */
#define GET_ACCESS_RIGHTS_BIT(pAccessRights, Bit)    ((((PT_BYTE*)pAccessRights)[Bit>>3] >> (Bit&7)) & 1)



/** 
 * Crypto mechanisms for secure channel
 */

#define PT_SECURECHANNEL_MECH_NONE    0x00000000    ///< Disables channel encryption
#define PT_SECURECHANNEL_MECH_DES_CBC    0x00000021    ///< DES in CBC mode, max possible key length is 56 bits
#define PT_SECURECHANNEL_MECH_AES_CBC    0x00000031    ///< AES in CBC mode, max possible key length is 256 bits



/** 
 * Secure channel flags
 */

#define PT_SECURECHANNELBIT_DISABLE_GUI_ENCRYPTION    0x00000001    ///< Disable encryption of GUI callbacks
#define PT_SECURECHANNELBIT_DISABLE_ASYNC_DATA_ENCRYPTION    0x00000002    ///< Disable encryption of asynchronous data transfer.




/** 
 * Areas for storing application data
 */

#define PT_AREA_APP    (1)    ///< Application area
#define PT_AREA_OEM    (2)    ///< OEM area
#define PT_AREA_PARTNER    (3)    ///< Partner area
#define PT_AREA_LATENT    (4)    ///< Latent area is used in PerfectPrint emulation to store the latent data
#define PT_AREA_APP_EX    (5)    ///< Second application area
#define PT_AREA_CERT_KEYEX    (6)    ///< Area for certificate of key exchange key
#define PT_AREA_CERT_SIGN    (7)    ///< Area for certificate of signing key
#define PT_AREA_PUBKEY_KEYEX    (8)    ///< Area for public key exchange key
#define PT_AREA_PUBKEY_SIGN    (9)    ///< Area for public signing key
#define PT_AREA_CERT_SECCHAN    (10)    ///< Area for certificate of secure channel key
#define PT_AREA_PUBKEY_SECCHAN    (11)    ///< Area for public secure channel key
#define PT_AREA_APP_READER    (12)    ///< Reader's application area (available only for area-sensor readers)



/** 
 * Types of EEPROM memory for call of PTGetAvailableMemory
 */

#define PT_MEMTYPE_ALL    0xffffffff    ///< Total of all currently free memory
#define PT_MEMTYPE_ALL_LIMIT    0xfffffffe    ///< Limit of all available memory (including currently occupied space), supported by SONLY only
#define PT_MEMTYPE_INTERNAL    0x0    ///< Currently free Internal EEPROM
#define PT_MEMTYPE_EXTERNAL    0x1    ///< Currently free External EEPROM
#define PT_MEMTYPE_INTERNAL_LIMIT    0x2    ///< Limit of available Internal EEPROM (including currently occupied space), supported by SONLY only
#define PT_MEMTYPE_EXTERNAL_LIMIT    0x3    ///< Limit of available External EEPROM (including currently occupied space), supported by SONLY only




/** 
 * Modes for LEDs control
 */

#define PT_LED_MODE_MANUAL    0    ///< Full control of the LEDs
#define PT_LED_MODE_BIO    1    ///< Bio feedback mode (TCD42 and TCD50 only)
#define PT_LED_MODE_READER    2    ///< Reader mode (behavior depends on current LED policy)
#define PT_LED_MODE_MANUAL2    3    ///< Full control of the LEDs (SONLY with TCS5D only)




/** 
 * Structure with additional LED parameters for PT_LED_MODE_MANUAL mode (legacy mode) used by PTSetLedEx.
 */
typedef struct pt_led_params_manual {
  PT_DWORD Version; 	///< Version of this structure (1)
  PT_DWORD Led1; 	///< LED1 parameters
  PT_DWORD Led2; 	///< LED2 parameters
  PT_DWORD Flags; 	///< Additional flags (see PT_LEDPARAM_FLAG_xxxx)
} PT_LED_PARAMS_MANUAL;




/** 
 * Structure with additional LED parameters for PT_LED_MODE_READER mode used by PTSetLedEx.
 */
typedef struct pt_led_params_reader {
  PT_DWORD Version; 	///< Version of this structure (1)
  PT_DWORD Policy; 	///< LED policy to be used (see PT_LED_POLICY_xxxx)
  PT_DWORD Variant; 	///< LED variant to be used (see PT_LED_VARIANT_xxxx)
  PT_DWORD Flags; 	///< Additional flags (see PT_LEDPARAM_FLAG_xxxx)
} PT_LED_PARAMS_READER;




/** 
 * LED reader mode policies used in PT_LED_PARAMS_READER
 */

#define PT_LED_POLICY_READER    0x00000000    ///< Not supported
#define PT_LED_POLICY_BIO    0x00000001    ///< Not supported
#define PT_LED_POLICY_LENOVO    0x00000002    ///< 'Lenovo Intl.' policy
#define PT_LED_POLICY_LENOVO_CHINA    0x00000003    ///< 'Lenovo China' policy
#define PT_LED_POLICY_LENOVO_CHINA2    0x00000004    ///< 'Lenovo China' policy (alternative)
#define PT_LED_POLICY_EIKON_TOUCH    0x00000005    ///< 'Eikon Touch' policy
#define PT_LED_POLICY_LEDS_OFF    0x000000ff    ///< LEDs turned off
#define PT_LED_POLICY_DEFAULT    0xffffffff    ///< Default policy




/** 
 * LED reader mode variants used in PT_LED_PARAMS_READER
 */

#define PT_LED_VARIANT_PROMPT    0x00000000    ///< Variant with visual prompting
#define PT_LED_VARIANT_NO_PROMPT    0x00000001    ///< Variant without visual prompting
#define PT_LED_VARIANT_DEFAULT    0xffffffff    ///< Default variant




/** 
 * Additional LED parameters for PT_LED_MODE_MANUAL2 mode (TCS5D mode) used by PTSetLedEx.
 */
typedef struct pt_led_params_manual2 {
  PT_DWORD Version; 	///< Version of this structure (1)
  PT_WORD Drv1_OnPeriod; 	///< Driver 1: Length of "ON" period in 4ms units (range 0-2047)
  PT_WORD Drv1_OffPeriod; 	///< Driver 1: Length of "OFF" period in 4ms units (range 0-2047)
  PT_BYTE Drv1_BrCapHi; 	///< Driver 1: Maximum brightness value (range 0-63)
  PT_BYTE Drv1_BrCapLo; 	///< Driver 1: Minimum brightness value (range 0-63)
  PT_BYTE Drv1_BrStepX; 	///< Driver 1: Length of brightness change step in 4ms units (range 0-63, +1 added)
  PT_BYTE Drv1_BrStepY; 	///< Driver 1: Amount levels the LED brightness level will change at each 'BrStepX' (range 0-63)
  PT_DWORD Drv1_Flags; 	///< Driver 1: Additional flags (see PT_LEDDRV_FLAG_xxxx)
  PT_WORD Drv2_OnPeriod; 	///< Driver 2: Length of "ON" period in 4ms units (range 0-2047)
  PT_WORD Drv2_OffPeriod; 	///< Driver 2: Length of "OFF" period in 4ms units (range 0-2047)
  PT_BYTE Drv2_BrCapHi; 	///< Driver 2: Maximum brightness value (range 0-63)
  PT_BYTE Drv2_BrCapLo; 	///< Driver 2: Minimum brightness value (range 0-63)
  PT_BYTE Drv2_BrStepX; 	///< Driver 2: Length of brightness change step in 4ms units (range 0-63, +1 added)
  PT_BYTE Drv2_BrStepY; 	///< Driver 2: Amount levels the LED brightness level will change at each 'BrStepX' (range 0-63)
  PT_DWORD Drv2_Flags; 	///< Driver 2: Additional flags (see PT_LEDDRV_FLAG_xxxx)
  PT_BYTE Drv3_TruthTable; 	///< Definition of the Boolean combination of Driver 1 and Driver 2 that generates Driver 3:
  PT_BYTE Led1_Mapping; 	///< LED1 mapping - 0=Drv1, 1=Drv3
  PT_BYTE Led2_Mapping; 	///< LED2 mapping - 0=Drv2, 1=Drv3
  PT_BYTE Led3_Mapping; 	///< LED3 mapping - 0=off, 1=Drv1, 2=Drv2, 3=Drv3
  PT_DWORD Flags; 	///< Additional flags (see PT_LEDPARAM_FLAG_xxxx)
} PT_LED_PARAMS_MANUAL2;




/** 
 * Additional flags for LED drivers used in PT_LED_PARAMS_MANUAL2
 */

#define PT_LEDDRV_FLAG_START_ON    0x00000001    ///< If set, state machine will start from the beginning of "ON" period
#define PT_LEDDRV_FLAG_STOP_AFTER_OFF    0x00000002    ///< If set, state machine will stop after first "OFF" period finishes
#define PT_LEDDRV_FLAG_BR_INVERT    0x00000004    ///< If set, brightness output is inverted
#define PT_LEDDRV_FLAG_TABLE_COSEXP    0x00000008    ///< If set, brightness level is defined as CosExp function, otherwise linear function is used




/** 
 * Common LED parameter flags used in PT_LED_PARAMS_xxxx
 */

#define PT_LEDPARAM_FLAG_TEST_PARAMS    0x80000000    ///< If set then LED parameters are checked for validity, but they are not applied




/** 
 * Reconstruction error policies
 */

#define PT_SCANQUALQUERY_GRAB    0x0001    ///< Set scan quality query mode for PTGrab function
#define PT_SCANQUALQUERY_ENROLL    0x0002    ///< Set scan quality query mode for PTEnroll function and PTCapture with purpose = enroll
#define PT_SCANQUALQUERY_VERIFY    0x0004    ///< Set scan quality query mode for PTVerify and all other finger verification/matching biometric functions
#define PT_IMAGEQUALQUERY_GRAB    0x0008    ///< Set image quality query mode for PTGrab function when parameter boWaitForAcceptableFinger is PT_FALSE
#define PT_IMAGEQUALQUERY_GRAB_WAIT    0x0010    ///< Set image quality query mode for PTGrab function when parameter boWaitForAcceptableFinger is PT_TRUE
#define PT_IMAGEQUALQUERY_ENROLL    0x0020    ///< Set image quality query mode for PTEnroll function and PTCapture with purpose = enroll
#define PT_IMAGEQUALQUERY_VERIFY    0x0040    ///< Set image quality query mode for PTVerify and all other finger verification/matching biometric functions




/** 
 * Reconstruction input types for biometrics
 */

#define PT_BIO_INPUT_TYPE_NORMAL    0    ///< Normal input
#define PT_BIO_INPUT_TYPE_FAST    1    ///< Fast input
#define PT_BIO_INPUT_TYPE_381    2    ///< Use 381/381 dpi 8 bpp as input format
#define PT_BIO_INPUT_TYPE_LP    3    ///< Low power input
#define PT_BIO_INPUT_TYPE_VLP    4    ///< Very low power input
#define PT_BIO_INPUT_TYPE_381_381_4    5     ///< Use 381/381 dpi 4 bpp as input format
#define PT_BIO_INPUT_TYPE_381_254_4    6     ///< Use 381/254 dpi 4 bpp as input format
#define PT_BIO_INPUT_TYPE_FAST_LP    7    ///< Fast low-power input
#define PT_BIO_INPUT_TYPE_508_508_8    8    ///< Use 508/508 dpi 8 bpp as input format
#define PT_BIO_INPUT_TYPE_254_381_8    9    ///< Use 254/381 dpi 8 bpp as input format
#define PT_BIO_INPUT_TYPE_381_381_6    10    ///< Use 381/381 dpi 6 bpp as input format. This format is subject to 'ImageFormatPolicy' session parameter.
#define PT_BIO_INPUT_TYPE_IMGFORMAT    255    ///< Use image format selected by PTSetImageFormat




/** 
 * Security levels for template matching
 */

#define PT_MATCH_MIN_SECURITY    1     ///< Minimal security setting
#define PT_MATCH_LOWER_SECURITY    2     ///< Lower security setting
#define PT_MATCH_MEDIUM_SECURITY    3     ///< Medium and default security setting
#define PT_MATCH_HIGHER_SECURITY    4     ///< Higher security setting
#define PT_MATCH_MAX_SECURITY    5     ///< Maximal security setting



/** 
 * Security level type
 */
typedef PT_DWORD     PT_SECURITY_LEVEL;


/** 
 * Antispoofing security levels
 */

#define PT_ANTISPOOFING_MIN_SECURITY    1     ///< Minimal security setting
#define PT_ANTISPOOFING_LOWER_SECURITY    2     ///< Lower security setting
#define PT_ANTISPOOFING_MEDIUM_SECURITY    3     ///< Medium and default security setting
#define PT_ANTISPOOFING_HIGHER_SECURITY    4     ///< Higher security setting
#define PT_ANTISPOOFING_MAX_SECURITY    5     ///< Maximal security setting



/** 
 * Antispoofing level type
 */
typedef PT_DWORD     PT_ANTISPOOFING_LEVEL;


/** 
 * Types of called GUI callbacks
 */

#define CALLBACKS_NONE    0    ///< No callbacks at all
#define CALLBACKS_SIMPLE    1    ///< Only put/lift finger
#define CALLBACKS_ADVANCED    2    ///< Positioning feedback
#define CALLBACKS_FULL    3    ///< Begin/end + positioning feedback



/** 
 * Additional flags describing behavior of GUI callbacks
 */

#define CALLBACKSBIT_SWIPE_IN_PROGRESS    0x00010000    ///< Flag enabling PT_GUIMSG_SWIPE_IN_PROGRESS GUI message
#define CALLBACKSBIT_SAMPLE_IMAGE    0x00020000    ///< Flag enabling sending of sample image
#define CALLBACKSBIT_NO_REPEATING_MSG    0x00040000    ///< Flag disabling periodical sending of GUI messages during finger detection and undetection (SONLY only)



/** 
 * GUI callbacks level type
 */
typedef PT_DWORD     PT_CALLBACK_LEVEL;


/** 
 * Consolidation types
 */

#define PT_CONSOLIDATION_NORMAL    0    ///< Normal consolidation policy - enrollment template is constructed from a subset of collected templates.
#define PT_CONSOLIDATION_SMART    1    ///< Smart consolidation policy - it has a built-in heuristic, which decides if to use the normal policy or to just use one (best) of provided templates.
#define PT_CONSOLIDATION_STRICT    2    ///< Strict consolidation policy - similar to smart policy, except that all collected templates must match each other (i.e. the same finger has to be used for all acquisitions).
#define PT_CONSOLIDATION_CONVENIENT    3    ///< Convenient consolidation policy - similar to smart policy with relaxed criteria for image/template acceptance for entering the enrollment process.




/** 
 * Finger scan termination policies
 */

#define PT_SCAN_TERMINATION_BASIC    0    ///< When the scanned image is longer than allowed number of rows, return the beginning of the image from the scan start on.
#define PT_SCAN_TERMINATION_FINGERTIP    1    ///< When the scanned image is longer than allowed number of rows, return the end of the image up to the fingertip.
#define PT_SCAN_TERMINATION_CORE    2    ///< When the scanned image is longer than allowed number of rows, return the most valuable part of the image from biometrical viewpoint (typically the fingerprint's core).
#define PT_SCAN_TERMINATION_CORE_PLUS    3    ///< When the scanned image is longer than allowed number of rows, return the most valuable part of the image from biometrical viewpoint (typically the fingerprint's core), while skipping finger joint.



/** 
 * Retuning options
 */

#define PT_RETUNING_DISABLED    0    ///< Retuning disabled
#define PT_RETUNING_ENABLED    1    ///< Retuning enabled



/** 
 * Swipe direction options
 */

#define PT_SWIPEDIR_STANDARD    0    ///< Standard direction for given mounting
#define PT_SWIPEDIR_INVERTED    1    ///< Inverted direction
#define PT_SWIPEDIR_AUTODETECT    2    ///< Direction is detected at the begining of swipe
#define PT_SWIPEDIR_STANDARD_WARN    3    ///< Standard direction with warning for backward swipe
#define PT_SWIPEDIR_INVERTED_WARN    4    ///< Inverted direction with warning for backward swipe



/** 
 * Navigation modes
 */

#define PT_NAVTYPE_508_8    0    ///< Navigation is using 508 DPI resolution, 8 bits/pixel (best quality)
#define PT_NAVTYPE_508_4    1    ///< Navigation is using 508 DPI resolution, 4 bits/pixel (best quality)
#define PT_NAVTYPE_254_8_LOPOWER    2    ///< Navigation is using 254 DPI resolution, 8 bits/pixel. Companion chip and sensor are both running in low-power slow-clock mode. Power saving mode with lower quality of navigation.
#define PT_NAVTYPE_254_4_LOPOWER    3    ///< Navigation is using 254 DPI resolution, 4 bits/pixel. Companion chip and sensor are both running in low-power slow-clock mode. Power saving mode with lower quality of navigation. 
#define PT_NAVTYPE_381_8    4    ///< Navigation is using 381 DPI resolution, 8 bits/pixel.
#define PT_NAVTYPE_254_8_VLOPOWER    2    ///< Navigation is using 254 DPI resolution, 8 bits/pixel. Companion chip and sensor are both running in low-power slow-clock mode. Power saving mode with lower quality of navigation.
#define PT_NAVTYPE_381_4    5    ///< Navigation is using 381 DPI resolution, 4 bits/pixel.
#define PT_NAVTYPE_381_8_FULLPOWER    6    ///< Navigation is using 381 DPI resolution, 8 bits/pixel, Companion chip and sensor are both running in full-power mode.
#define PT_NAVTYPE_381_8_HW    7    ///< Navigation is using 381 DPI resolution, 8 bits/pixel, hardware mode.
#define PT_NAVTYPE_381_6    8    ///< Navigation is using 381 DPI resolution, 6 bits/pixel. This format is subject to 'ImageFormatPolicy' session parameter.
#define PT_NAVTYPE_IMGFORMAT    255    ///< Navigation is using image format selected by PTSetImageFormat.



/** 
 * Navigation orientation
 */

#define PT_NAVORIENT_STANDARD    0    ///< Standard orientation of navigation
#define PT_NAVORIENT_BYSWIPE    1    ///< Navigation orientation is controlled by direction of last swipe



/** 
 * Navigation flags
 */

#define PT_NAVIG_TAPPING_CLICK    0x0001    ///< Enable click by tapping
#define PT_NAVIG_PRESSURE_CLICK    0x0002    ///< Enable click by pressure
#define PT_NAVIG_REPORT_CHANGES_ONLY    0x0004    ///< If set, navigation callbacks are sent only if the state of the output has changed




/** 
 * Finger detection flags
 */

#define PT_FDETFLAG_SMART_ENABLE    0x0004    ///< Enable smart finger detection
#define PT_FDETFLAG_SMART_DISABLE_NAV    0x0008    ///< Disable use of HW navigation in smart finger detection (used only if PT_FDETFLAG_SMART_ENABLE flag is set)
#define PT_FDETFLAG_SMART_DISABLE_LIVE_DET    0x0010    ///< Disable polling of Live_det in smart finger detection (used only if PT_FDETFLAG_SMART_ENABLE flag is set)




/** 
 * Device suspend during sleep commands options
 */

#define PT_SUSPEND_IN_SLEEP_DISABLED    0    ///< Device suspend disabled
#define PT_SUSPEND_IN_SLEEP_ENABLED    1    ///< Device suspend enabled



/** 
 * Noise robustness options
 */

#define PT_NOISE_ROBUSTNESS_DISABLED    0    ///< Noise robustness is switched off
#define PT_NOISE_ROBUSTNESS_FORCED    1    ///< Noise robustness is always switched on
#define PT_NOISE_ROBUSTNESS_ON_DETECTION    2    ///< Noise robustness is in auto detection mode, detection is performed only after low quality swipes
#define PT_NOISE_ROBUSTNESS_ON_FORCED_DETECTION    3    ///< SONLY only: Noise robustness is in auto detection mode, detection is performed after each swipe




/** 
 * Sensor security modes
 */

#define PT_SSM_DISABLED    0    ///< Sensor security is disabled
#define PT_SSM_ENCRYPT    1    ///< Sensor security is set to "encryption" mode
#define PT_SSM_SIGN_ALL    2    ///< Sensor security is set to "sign all" mode
#define PT_SSM_SIGN_PARTIAL_V1    3    ///< Sensor security is set to "sign partial ver. 1" (faster than ver. 2, but less secure)
#define PT_SSM_SIGN_PARTIAL_V2    4    ///< Sensor security is set to "sign partial ver. 2" (slower than ver. 1, but more secure)




/** 
 * Reader security modes
 */

#define PT_RSM_DISABLED    0    ///< Reader security is disabled
#define PT_RSM_ENCRYPT    1    ///< Reader security is set to "encryption" mode




/** 
 * Image format policies
 */

#define PT_IMGPOLICY_FIXED_INPUT_FORMAT_FIXED_BANDWIDTH    0    ///< Input image format will be fixed to the first one specified for image format. Bandwidth used will be fixed to 'Bandwidth' session parameter and cannot be retuned by PTAPI library on-the-fly.
#define PT_IMGPOLICY_FIXED_INPUT_FORMAT_VARIABLE_BANDWIDTH    1    ///< Input image format will be fixed to the one specified for image format. Library can make statistics of available bandwidth and decrease the bandwidth on-the-fly (with 'Bandwidth' session parameter being lower bound value), resulting with slower swipe speed but constant image quality.
#define PT_IMGPOLICY_VARIABLE_INPUT_FORMAT_VARIABLE_BANDWIDTH    2    ///< Library can make statistics of available bandwidth and switch from 381/381/6 to 254/381/6 input format and decrease the bandwidth on-the-fly (with 'Bandwidth' session parameter being lower bound value), resulting with worse image quality and slower swipe speed.




/** 
 * Reconstruction flags
 */

#define PT_RECFLAG_SRA_DOWN    0x0001    ///< Enable top-down striation removal algorithm
#define PT_RECFLAG_SRA_UP    0x0002    ///< Enable bottom-up striation removal algorithm
#define PT_RECFLAG_SKEW    0x0004    ///< Enable skew compensation
#define PT_RECFLAG_GRADIENT    0x0008    ///< Enable gradient compensation
#define PT_RECFLAG_DGAIN    0x0010    ///< Enable digital gain application




/** 
 * Power management flags
 */

#define PT_POWERFLAG_LOW_POWER    0x0001    ///< TCD50v3 only: If set power consumption is less than 100mA.




/** 
 * PT_SESSION_CFG structure version 1
 */
typedef struct pt_session_cfg_v1 {
  PT_SECURITY_LEVEL SecuritySetting; 	///< Security level for templates matching (see PT_MATCH_xxxx values)
  PT_DWORD AntispoofingLevel; 	///< Level of anti-spoofing security (see PT_ANTISPOOFING_xxxx values)
  PT_BOOL MultipleEnroll; 	///< Indicates whether or not enrollment should use multiple finger images
  PT_BOOL LatentDetect; 	///< Indicates whether or not latent detection should be activated (valid only for systems with area sensor)
  PT_BOOL SubSample; 	///< Indicates whether or not extraction should sub-sample images
  PT_BOOL SensorDisabling; 	///< Indicates whether or not put sensor into sleep mode after each biometric operation
  PT_CALLBACK_LEVEL CallbackLevel; 	///< Indicates what amount of GUI callbacks is received during biometric operations (see CALLBACKS_xxxx values)
  PT_BYTE Reserved[20]; 	///< Not used
} PT_SESSION_CFG_V1;



/** 
 * PT_SESSION_CFG structure version 2
 */
typedef struct pt_session_cfg_v2 {
  PT_SECURITY_LEVEL SecuritySetting; 	///< Security level for templates matching (see PT_MATCH_xxxx values)
  PT_DWORD AntispoofingLevel; 	///< Level of anti-spoofing security (see PT_ANTISPOOFING_xxxx values)
  PT_BOOL MultipleEnroll; 	///< Indicates whether or not enrollment should use multiple finger images
  PT_BOOL LatentDetect; 	///< Indicates whether or not latent detection should be activated (valid only for systems with area sensor)
  PT_BOOL SubSample; 	///< Indicates whether or not extraction should sub-sample images
  PT_BOOL SensorDisabling; 	///< Indicates whether or not put sensor into sleep mode after each biometric operation
  PT_CALLBACK_LEVEL CallbackLevel; 	///< Indicates what amount of GUI callbacks is received during biometric operations (see CALLBACKS_xxxx values)
  PT_BOOL WakeUpByFinger; 	///< If PT_TRUE, FM can be woken up from deep sleep by a finger on sensor
  PT_BYTE SubWindowArea; 	///< Area of the subwindow in percents of the full area used for extractor
  PT_BYTE reserve1[3]; 	///< Not used
  PT_BOOL WffUseHwDetection; 	///< Use Hardware finger detection
  PT_WORD WffFingerDownDelay; 	///< Timing for finger touch
  PT_WORD WffFingerUpDelay; 	///< Timing for finger lift
  PT_BOOL RecUseHwDetection; 	///< Use Hardware finger detection
  PT_WORD RecFingerDownDelay; 	///< Timing for finger touch
  PT_WORD RecFingerUpDelay; 	///< Timing for finger lift
  PT_BOOL RecRemoveTopdown; 	///< Remove striation from top
  PT_BOOL RecRemoveBottomup; 	///< Remove striation from bottom
  PT_BOOL NavUseHwDetection; 	///< Use Hardware finger detection
  PT_WORD NavFingerDownDelay; 	///< Timing for finger touch
  PT_WORD NavFingerUpDelay; 	///< Timing for finger lift
  PT_WORD NavClickTimeMin; 	///< Minimum elapsed time to detect a click
  PT_WORD NavClickTimeMax; 	///< Maximum elapsed time to detect a click 
  PT_WORD NavMovementDelay; 	///< Delay before movement detection
  PT_BYTE reserve2[2]; 	///< Not used
  PT_DWORD NavClickAllowedMovement; 	///< Maximum allowed movement for click
} PT_SESSION_CFG_V2;



/** 
 * PT_SESSION_CFG structure version 3
 */
typedef struct pt_session_cfg_v3 {
  PT_SECURITY_LEVEL SecuritySetting; 	///< Security level for templates matching (see PT_MATCH_xxxx values)
  PT_DWORD AntispoofingLevel; 	///< Level of anti-spoofing security (see PT_ANTISPOOFING_xxxx values)
  PT_BOOL MultipleEnroll; 	///< Indicates whether or not enrollment should use multiple finger images
  PT_BOOL LatentDetect; 	///< Indicates whether or not latent detection should be activated (valid only for systems with area sensor)
  PT_BOOL SubSample; 	///< Indicates whether or not extraction should sub-sample images.
  PT_BOOL SensorDisabling; 	///< Indicates whether or not put sensor into sleep mode after each biometric operation
  PT_CALLBACK_LEVEL CallbackLevel; 	///< Indicates what amount of GUI callbacks is received during biometric operations (see CALLBACKS_xxxx values)
  PT_BOOL WakeUpByFinger; 	///< If PT_TRUE, FM can be woken up from deep sleep by a finger on sensor. 
  PT_DWORD WakeUpByFingerTimeout; 	///< Timeout for returning to standby after wake-up by finger (in milliseconds). 
  PT_BYTE SubWindowArea; 	///< Area of the subwindow in percents of the full area used for extractor.
  PT_BYTE ConsolidationType; 	///< Type of consolidation in multienrollment (see PT_CONSOLIDATION_xxxx values).
  PT_BYTE reserve1[2]; 	///< Not used
  PT_BOOL WffUseHwDetection; 	///< Use Hardware finger detection.
  PT_WORD WffFingerDownDelay; 	///< Timing for finger touch.
  PT_WORD WffFingerUpDelay; 	///< Timing for finger lift.
  PT_BOOL RecUseHwDetection; 	///< Use Hardware finger detection.
  PT_WORD RecFingerDownDelay; 	///< Timing for finger touch.
  PT_WORD RecFingerUpDelay; 	///< Timing for finger lift. 
  PT_WORD RecTerminationPolicy; 	///< Defines the way, how end of finger scan is determined (see PT_SCAN_TERMINATION_xxxx values)
  PT_BYTE RecRetuning; 	///< Enable retuning (see PT_RETUNING_xxxx values).
  PT_BYTE RecDigitalGain; 	///< Digital gain. 
  PT_BOOL RecRemoveTopdown; 	///< Remove striation from top.
  PT_BOOL RecRemoveBottomup; 	///< Remove striation from bottom.
  PT_BOOL NavUseHwDetection; 	///< Use Hardware finger detection.
  PT_WORD NavFingerDownDelay; 	///< Timing for finger touch.
  PT_WORD NavFingerUpDelay; 	///< Timing for finger lift.
  PT_WORD NavClickTimeMin; 	///< Minimum elapsed time to detect a click.
  PT_WORD NavClickTimeMax; 	///< Maximum elapsed time to detect a click 
  PT_WORD NavMovementDelay; 	///< Delay before movement detection.
  PT_BYTE reserve2[2]; 	///< Not used
  PT_DWORD NavClickAllowedMovement; 	///< Maximum allowed movement for click
  PT_WORD NavNavigationType; 	///< Navigation type (see PT_NAVTYPE_xxxx values).
  PT_WORD BioEnrollInputType; 	///< Finger scanning type for enrollment, see PT_BIO_INPUT_TYPE_xxxx.
  PT_WORD BioVerifyInputType; 	///< Finger scanning type for verification, see PT_BIO_INPUT_TYPE_xxxx.
  PT_WORD EnableScanQualityQuery; 	///< Bitmask of scan quality modes (see PT_SCANQUALQUERY_xxxx values).
} PT_SESSION_CFG_V3;



/** 
 * PT_SESSION_CFG structure version 4
 */
typedef struct pt_session_cfg_v4 {
  PT_DWORD SecuritySetting; 	///< Security level for templates matching (see PT_MATCH_xxxx values).
  PT_WORD AntispoofingEnable; 	///< If PT_TRUE, antispoofing is enabled.
  PT_WORD AntispoofingSecurityLevel; 	///< Level of anti-spoofing security (see PT_ANTISPOOFING_xxxx values).
  PT_BOOL MultipleEnroll; 	///< Indicates whether or not enrollment should use multiple finger images (consolidated enrollment).
  PT_BOOL LatentDetect; 	///< Indicates whether or not latent detection should be enabled (not zero) or disabled (zero) (valid only for systems with area sensor).
  PT_BOOL SubSample; 	///< Indicates whether or not extraction should sub-sample images (not zero).
  PT_BOOL SensorDisabling; 	///< Indicates whether or not put sensor into sleep mode after each biometric operation.
  PT_DWORD CallbackLevel; 	///< Indicates what amount of GUI callbacks is received during biometric operations (see CALLBACKS_xxxx values).
  PT_BOOL WakeUpByFinger; 	///< If not zero, FM enters upon end of session the StandBy mode and can be woken up by a finger on the sensor.
  PT_DWORD WakeUpByFingerTimeout; 	///< Timeout for returning to standby after wake-up by finger (in milliseconds).
  PT_BYTE SubWindowArea; 	///< Area of the subwindow in percents of the full area used for extractor.
  PT_BYTE ConsolidationType; 	///< Type of consolidation in multienrollment (see PT_CONSOLIDATION_xxxx values).
  PT_BYTE CreateWrappedBIRs; 	///< Indicates whether or not wrapped BIRs should be created by biometric functions.
  PT_BYTE SignatureType; 	///< Type of signing method (see PT_SIGNATURE_TYPE_xxxx values).
  PT_BOOL RestartBioTimeout; 	///< Restart biometric timeout for each fingerprint scan.
  PT_BYTE IdentificationThreshold; 	///< Threshold for biometric identification.
  PT_BYTE ExtractionMaxMinu; 	///< Max number of minutiae in biometric template from extraction.
  PT_BYTE EnrollMaxMinu; 	///< Max number of minutiae in biometric template after enroll.
  PT_BYTE TemplateType; 	///< Type of templates created by FM (see PT_TEMPLATE_TYPE_xxxx values)
  PT_BOOL WffUseHwDetection; 	///< Use Hardware finger detection.
  PT_WORD WffFingerDownDelay; 	///< Timing for finger touch.
  PT_WORD WffFingerUpDelay; 	///< Timing for finger lift.
  PT_BOOL RecUseHwDetection; 	///< Use Hardware finger detection.
  PT_BYTE RecSwipeDirection; 	///< Swipe direction (see PT_SWIPEDIR_xxxx values).
  PT_BYTE RecNoiseRobustness; 	///< Noise robustness setting (see PT_NOISE_ROBUSTNESS_xxxx).(SONLY only)
  PT_BYTE Reserved1[2]; 	///< Not used
  PT_LONG RecSwipeTimeout; 	///< Timeout (in milliseconds) for swipe termination.
  PT_LONG RecNoMovementTimeout; 	///< If no movement is detected for that period [ms], the swipe is terminated regardless on the finger presence. This feature is disabled if set to 0. 
  PT_LONG RecNoMovementResetTimeout; 	///< If no movement is detected for that period [ms] and image is very short, the reconstruction is not restarted any more. This feature is disabled if set to 0.
  PT_WORD RecTerminationPolicy; 	///< Defines the way, how end of finger scan is determined (see PT_SCAN_TERMINATION_xxxx values).
  PT_BYTE Reserved2[2]; 	///< Not used
  PT_BOOL RecRemoveTopdown; 	///< Remove striation from top.
  PT_BOOL RecRemoveBottomup; 	///< Remove striation from bottom.
  PT_BOOL NavUseHwDetection; 	///< Use Hardware finger detection.
  PT_WORD NavFingerDownDelay; 	///< Timing for finger touch.
  PT_WORD NavFingerUpDelay; 	///< Timing for finger lift.
  PT_WORD NavClickTimeMin; 	///< Minimum elapsed time to detect a click.
  PT_WORD NavClickTimeMax; 	///< Maximum elapsed time to detect a click.
  PT_WORD NavMovementDelay; 	///< Delay in ms before detecting any movement in navigation.
  PT_WORD NavFlags; 	///< Navigation flags (see PT_NAVIG_xxxx values).
  PT_DWORD NavClickAllowedMovement; 	///< Maximum movement that can be detected during a click.
  PT_WORD NavNavigationType; 	///< Navigation type (see PT_NAVTYPE_xxxx values).
  PT_BYTE NavOrientation; 	///< Orientation of navigation (see PT_NAVORIENT_xxxx values).
  PT_BYTE Reserved4[1]; 	///< Not used
  PT_BOOL NavSubPixelPrecision; 	///< If PT_TRUE then sub-pixel precision is used for navigation.
  PT_WORD BioEnrollInputType; 	///< Reconstruction type for enrollment (see PT_BIO_INPUT_TYPE_xxxx values.)
  PT_WORD BioVerifyInputType; 	///< Reconstruction type for verification (see PT_BIO_INPUT_TYPE_xxxx values).
  PT_WORD BioFingerDetectInputType; 	///< Reconstruction type for finger detection commands (see PT_BIO_INPUT_TYPE_xxxx values).
  PT_WORD EnableScanQualityQuery; 	///< Bitmask of scan quality modes (see PT_SCANQUALQUERY_xxxx and PT_IMAGEQUALQUERY_xxxx values).
  PT_BOOL UseHwFingerDetection; 	///< If PT_TRUE then hardware finger detection is used for biometric functions.
  PT_BYTE Retuning; 	///< Enable retuning (see PT_RETUNING_xxxx values).
  PT_BYTE SuspendInSleep; 	///< Enable device suspend during PTSleep/PTSleepThenGrab/PTSleepThenCapture (see PT_SUSPEND_IN_SLEEP_xxxx values) (this parameter affects SONLY)
  PT_BYTE SensorSecurityMode; 	///< Sensor security mode (see PT_SSM_xxxx values) (this parameter affects SONLY)
  PT_BYTE Reserved5[17]; 	///< Not used
} PT_SESSION_CFG_V4;



/** 
 * PT_SESSION_CFG structure version 5
 */
typedef struct pt_session_cfg_v5 {
  PT_BYTE SecuritySetting; 	///< Security level for templates matching (see PT_MATCH_xxxx values)
  PT_BYTE AntispoofingEnable; 	///< If PT_TRUE, antispoofing is enabled.
  PT_BYTE AntispoofingSecurityLevel; 	///< Level of anti-spoofing security (see PT_ANTISPOOFING_xxxx values).
  PT_BYTE AntispoofingRejectsEnrollment; 	///< If PT_TRUE, antispoofing rejects even enrollment captures.
  PT_BYTE MultipleEnroll; 	///< Indicates whether or not enrollment should use multiple finger images.
  PT_BYTE LatentDetect; 	///< Indicates whether or not latent detection should be activated (valid only for systems with area sensor).
  PT_BYTE SensorDisabling; 	///< Indicates whether or not put sensor into sleep mode after each biometric operation.
  PT_BYTE TemplateType; 	///< Type of templates created by FM (see PT_TEMPLATE_TYPE_xxxx values).
  PT_DWORD CallbackLevel; 	///< Indicates what amount of GUI callbacks is received during biometric operations (see CALLBACKS_xxxx values).
  PT_BYTE ConsolidationType; 	///< Type of consolidation in multienrollment (see PT_CONSOLIDATION_xxxx values).
  PT_BYTE ConsolidationSecurityLevel; 	///< Sets trade-off between security and convenience for consolidation process (for future use).
  PT_BYTE ConsolidationNumTemplates; 	///< Number of templates captured for consolidation when using static enrollment, ignored for dynamic enrollment.
  PT_BYTE CreateWrappedBIRs; 	///< Indicates whether or not wrapped BIRs should be created by biometric functions.
  PT_BYTE RestartBioTimeout; 	///< Restart biometric timeout for each fingerprint scan
  PT_BYTE IdentificationThreshold; 	///< Threshold for biometric identification.
  PT_BYTE ExtractionMaxMinu; 	///< Max number of minutiae in biometric template from extraction.
  PT_BYTE EnrollMaxMinu; 	///< Max number of minutiae in biometric template after enroll.
  PT_WORD ExtractionMaxSize; 	///< Max size (in bytes) of biometric data in template from extraction (must be bigger than 25).
  PT_WORD EnrollMaxSize; 	///< Max size (in bytes) of biometric data in template after enroll (must be bigger than 25)
  PT_DWORD WakeUpByFingerTimeout; 	///< Timeout for returning to standby after wake-up by finger (in milliseconds).
  PT_BYTE WakeUpByFinger; 	///< If PT_TRUE, FM can be woken up from deep sleep by a finger on sensor.
  PT_BYTE SignatureType; 	///< Type of signing method (see PT_SIGNATURE_TYPE_xxxx values).
  PT_WORD PowerFlags; 	///< Power management flags (see PT_POWERFLAG_xxxx values).
  PT_BYTE reserved1[4]; 	///< Not used.
  PT_DWORD FdetFlags; 	///< Finger detection flags (see PT_FDETFLAG_xxxx values).
  PT_WORD WffFingerDownDelay; 	///< Timing for finger touch (in milliseconds).
  PT_WORD WffFingerUpDelay; 	///< Timing for finger lift (in milliseconds).
  PT_BYTE RecSwipeDirection; 	///< Swipe direction (see PT_SWIPEDIR_xxxx values).
  PT_BYTE RecNoiseRobustness; 	///< Noise robustness setting (see PT_NOISE_ROBUSTNESS_xxxx).
  PT_BYTE RecNoiseRobustnessTrigger; 	///< Sets how many consecutive bad swipes triggers noise robustness. Value of 0 means no triggering by bad swipes.
  PT_BYTE RecTerminationPolicy; 	///< Defines the way, how end of finger scan is determined (see PT_SCAN_TERMINATION_xxxx values).
  PT_LONG RecSwipeTimeout; 	///< Timeout for swipe termination.
  PT_LONG RecNoMovementTimeout; 	///< If no movement is detected for that period [ms], the swipe is terminated regardless on the finger presence. This feature is disabled if set to 0. 
  PT_LONG RecNoMovementResetTimeout; 	///< If no movement is detected for that period [ms] and image is very short, the reconstruction is not restarted any more. This feature is disabled if set to 0.
  PT_DWORD RecFlags; 	///< Reconstruction control flags (see PT_RECFLAG_xxxx values).
  PT_BYTE reserved2[8]; 	///< Not used.
  PT_WORD NavFingerDownDelay; 	///< Timing for finger touch.
  PT_WORD NavFingerUpDelay; 	///< Timing for finger lift.
  PT_WORD NavClickTimeMin; 	///< Minimum elapsed time to detect a click.
  PT_WORD NavClickTimeMax; 	///< Maximum elapsed time to detect a click.
  PT_WORD NavMovementDelay; 	///< Delay before movement detection.
  PT_WORD NavFlags; 	///< Navigation flags (see PT_NAVIG_xxxx values).
  PT_DWORD NavClickAllowedMovement; 	///< Maximum allowed movement for click.
  PT_BYTE NavNavigationType; 	///< Navigation type (see PT_NAVTYPE_xxxx values).
  PT_BYTE NavOrientation; 	///< Orientation of navigation (see PT_NAVORIENT_xxxx values).
  PT_BYTE NavClickSensitivity; 	///< Click detection sensitivity setting.
  PT_BYTE reserved3[9]; 	///< Not used
  PT_BYTE BioEnrollInputType; 	///< Reconstruction type for enrollment (see PT_BIO_INPUT_TYPE_xxxx values).
  PT_BYTE BioVerifyInputType; 	///< Reconstruction type for verification (see PT_BIO_INPUT_TYPE_xxxx values).
  PT_BYTE BioFingerDetectInputType; 	///< Reconstruction type for finger detection commands (see PT_BIO_INPUT_TYPE_xxxx values).
  PT_BYTE UseHwFingerDetection; 	///< Use TCD based hardware finger detection for biometrics.
  PT_WORD EnableScanQualityQuery; 	///< Bitmask of scan quality modes (see PT_SCANQUALQUERY_xxxx values).
  PT_BYTE Retuning; 	///< Enable retuning (see PT_RETUNING_xxxx values).
  PT_BYTE SuspendInSleep; 	///< SONLY: Enable device suspend during PTSleep/PTSleepThenGrab/PTSleepThenCapture (see PT_SUSPEND_IN_SLEEP_xxxx values).
  PT_BYTE SensorSecurityMode; 	///< SONLY, TCD50: Sensor security mode (see PT_SSM_xxxx values).
  PT_BYTE ReaderSecurityMode; 	///< SONLY (TCRxx only): Reader security mode (see PT_RSM_xxxx values).
  PT_BYTE Bandwidth; 	///< SONLY (TCS5D only): Explicit value or lower bound (depending on policy) of bandwidth to be used in percentage of maximal available bandwidth for the sensor on given platform. Allowed values are 0-100, 0 means static maximum bandwidth.
  PT_BYTE ImageFormatPolicy; 	///< SONLY (TCRS5D only): Image format policy (see PT_IMGPOLICY_xxxx values).
  PT_BYTE EnrollMinTemplates; 	///< Do not change.
  PT_BYTE EnrollMaxTemplates; 	///< Do not change.
  PT_BYTE reserved4[14]; 	///< Not used.
} PT_SESSION_CFG_V5;




//#if ESS_VER >= 0x0300
//#define PT_CURRENT_SESSION_CFG  (5)         ///< For TCD50 is default PT_SESSION_CFG structure version 5
typedef PT_SESSION_CFG_V5 PT_SESSION_CFG;   ///< For TCD50 is default PT_SESSION_CFG structure version 5
/*#elif ESS_VER >= 0x0202
#define PT_CURRENT_SESSION_CFG  (4)         ///< For ESS 2.2 is default PT_SESSION_CFG structure version 4
typedef PT_SESSION_CFG_V4 PT_SESSION_CFG;   ///< For ESS 2.2 is default PT_SESSION_CFG structure version 4
#else
#define PT_CURRENT_SESSION_CFG  (3)         ///< For TFM 2.0 and ESS 2.1 is default PT_SESSION_CFG structure version 3
typedef PT_SESSION_CFG_V3 PT_SESSION_CFG;   ///< For TFM 2.0 and ESS 2.1 is default PT_SESSION_CFG structure version 3
#endif */


/** 
 * Slot number representing last matched slot template
 */

#define PT_SLOT_LASTMATCHED    (-1)



/** 
 * The navigation data passed to the PT_NAVIGATION_CALLBACK.
 */
typedef struct pt_navigation_data {
  PT_SHORT dx; 	///< Delta X since the last navigation data
  PT_SHORT dy; 	///< Delta Y since the last navigation data
  PT_WORD signalBits; 	///< Information bitmask, see PT_NAVIGBITS_xxxx
} PT_NAVIGATION_DATA;

/** 
 * Signal bits of PT_NAVIGATION_DATA
 */

#define PT_NAVIGBITS_FINGER    0x0001    ///< Finger is present
#define PT_NAVIGBITS_CLICK    0x0002    ///< At least one click since the last navigation data
#define PT_NAVIGBITS_CALIB_OVER    0x0004    ///< Calibration phase has enough data and can be completed. This bit is used only for PTClickCalibrate.
#define PT_NAVIGBITS_PRESSURE_CLICK    0x0008    ///< If a click occurred, this indicates its type. If zero, it is a tapping click; otherwise, a pressure click.




/** 
 * Actions for PTGetNavigationData()
 */

#define PT_NAVIG_ACTION_START    (0)    ///< Initialize and start the 'on-demand' navigation mode.
#define PT_NAVIG_ACTION_STOP    (1)    ///< Stop 'on-demand' navigation mode.
#define PT_NAVIG_ACTION_GET_DATA    (2)    ///< Return navigation data.




/** 
 * Sleep modes for PTSleep()
 */

#define PT_SLEEP_MODE_DEEPSLEEP    (0)    ///< Deep sleep without HW finger detect
#define PT_SLEEP_MODE_STANDBY    (1)    ///< Deep sleep with HW finger detect




/** 
 * Latent check operation modes
 */

#define PT_LATENT_MODE_CHECK    (0)    ///< 'Check only' operation mode.
#define PT_LATENT_MODE_STORE    (1)    ///< 'Store only' operation mode.
#define PT_LATENT_MODE_CHECK_AND_STORE    (2)    ///< 'Check and store' operation mode.




/** 
 * Wake up causes returned from sleep functions
 */

#define PT_WAKEUP_CAUSE_HOST    (0)    ///< Waken up by host
#define PT_WAKEUP_CAUSE_FINGER    (1)    ///< Waken up by finger on sensor



/** 
 * Response from IDLE callback
 */

#define PT_SLEEP_STOP    (0)    ///< Wake-up
#define PT_SLEEP_CONTINUE    (1)    ///< Continue in sleep



/** 
 * Predefined biometric time-out values
 */

#define PT_BIO_DEFAULT_TIMEOUT    (-1)    ///< Default timeout
#define PT_BIO_INFINITE_TIMEOUT    (-2)    ///< Infinite timeout



/** 
 * Swipe information structure.
 */
typedef struct pt_swipe_info {
  PT_SHORT RealHeight; 	///< Number of actually reconstructed lines.
  PT_SHORT BackgroundColor; 	///< Image background color.
  PT_SHORT ROIPos; 	///< Vertical position of the center of the region of interest (e.g. the core). Negative if no ROI was detected.
  PT_SHORT ROILength; 	///< Vertical length of the region of interest, zero if no ROI was detected.
  PT_WORD RecErrorStates; 	///< Bit array with signaled reconstruction errors (see PT_RECERRBIT_xxxx).
  PT_WORD QualityMessage; 	///< GUI message resulting from image quality test (see PT_GUIMSG_xxxx), 0xffff when image quality wasn't assessed.
  PT_CHAR RecScore; 	///< Reconstruction score.
  PT_CHAR QualityScore; 	///< Image quality score, negative when image quality wasn't assessed.
  PT_CHAR InvertedSwipe; 	///< Direction of swipe (0 = standard direction, 1 = inverted direction).
  PT_BYTE NoiseRobustnessFlags; 	///< Noise robustness flags.
  PT_BYTE reserved[8]; 	///< Reserved.
} PT_SWIPE_INFO;



/** 
 * Reconstruction errors
 */

#define PT_RECERRBIT_SWIPE_TOO_FAST    0x1    ///< Swipe too fast
#define PT_RECERRBIT_SWIPE_TOO_SKEWED    0x2    ///< Swipe too skewed
#define PT_RECERRBIT_BACKWARDS_MOVEMENT    0x4    ///< Backwards movement detected
#define PT_RECERRBIT_JOINT_DETECTED    0x8    ///< Finger joint detected
#define PT_RECERRBIT_TOO_SHORT    0x10    ///< Swipe too short
#define PT_RECERRBIT_TOO_LIGHT    0x20    ///< Image too light (area sensors)



/** 
 * Signature methods
 */

#define PT_SIGNATURE_TYPE_STANDARD    0    ///< Standard signature method (= HMAC-SHA-1)
#define PT_SIGNATURE_TYPE_HMACSHA1    0    ///< Signature with HMAC-SHA-1
#define PT_SIGNATURE_TYPE_RSA_SHA1    1    ///< Signature with SHA1 and RSA signing key
#define PT_SIGNATURE_TYPE_RSA_SHA256    2    ///< Signature with SHA256 and RSA signing key
#define PT_SIGNATURE_TYPE_HMACSHA256    3    ///< Signature with HMAC-SHA-256




/** 
 * Template types
 */

#define PT_TEMPLATE_TYPE_LEGACY    0    ///< Legacy template type is maintained for the purpose of compatibility with older devices, where it was the only template type supported. This template type should be considered obsolete.
#define PT_TEMPLATE_TYPE_ALPHA    1    ///< The alpha template type contains the minimum sufficient set of biometric information needed for a reasonable biometric performance and represents the best trade-off between template size and biometric performance. Its content is intended as "core" information that would be included in any future template type and every future version of UPEK biometric systems has to be able to process alpha type content.
#define PT_TEMPLATE_TYPE_BETA    2    ///< The beta template type is an extension to the alpha type. Additionally it contains optional block(s) of additional biometric information. Beta templates are in general able to achieve better biometric performance, but at the cost of an increased template size. The full benefit of the beta template can be applied only if it is matched against another beta template.
#define PT_TEMPLATE_TYPE_ALPHA_MULTI    3    ///< This type represents a multi-template extension of alpha template type. It is intended and recomended for use as an enrollment template, where multiple views of the fingerprint (alpha templates), which are captured during the enrollment process, are stored together within a single data structure. This allows the highest possible biometric performance, but at the cost of a higher template size and a higher matching time.
#define PT_TEMPLATE_TYPE_ANSI    16    ///< ANSI_INCITS-378-2004 template (currently can be used only as a parameter for PTConvertTemplateEx).
#define PT_TEMPLATE_TYPE_ISO_FMR    17    ///< ISO/IEC 19794-2 FMR template (currently can be used only as a parameter for PTConvertTemplateEx).
#define PT_TEMPLATE_TYPE_ISO_FMC_NORMAL    18    ///< ISO/IEC 19794-2 normal FMC template (currently can be used only as a parameter for PTConvertTemplateEx).
#define PT_TEMPLATE_TYPE_ISO_FMC_COMPACT    19    ///< ISO/IEC 19794-2 compact FMC template (currently can be used only as a parameter for PTConvertTemplateEx).
#define PT_TEMPLATE_TYPE_AUTO    0xffffffff    ///< Automatic or default template type (can be used only as a parameter for PTConvertTemplateEx).




/** 
 * Template envelope types
 */

#define PT_TEMPLATE_ENVELOPE_NONE    0    ///< No template envelope (template in raw format).
#define PT_TEMPLATE_ENVELOPE_PTBIR    1    ///< PT_BIR template envelope.




/** 
 * Flags for PTConvertTemplateEx
 */

#define PT_CONVTEMPL_FLAG_KEEP_TEMPLATE_TYPE    0x00000001    ///< If set then output template type is the same as the source template type (parameter dwTargetType is ignored). This flag is currently ignored.
#define PT_CONVTEMPL_FLAG_RETURN_PAYLOAD    0x00000002    ///< If set then output template contains payload copied from the input template, otherwise output template is returned without payload. This flag is currently ignored.




/** 
 * Structure with additional parameters for on-host matching functions.
 */
typedef struct pt_match_on_host_params {
  PT_DWORD Length; 	///< Length of this structure, set to 'sizeof(PT_MATCH_ON_HOST_PARAMS)'
  PT_DWORD Version; 	///< Version of this structure (currently 1)
  PT_DWORD SecuritySetting; 	///< Security level for templates matching (see PT_MATCH_xxxx values)
  PT_BYTE IdentificationThreshold; 	///< Threshold for biometric identification
  PT_BYTE reserved[3]; 	///< Reserved, set to 0
} PT_MATCH_ON_HOST_PARAMS;




/** 
 * Image types used for template extraction
 */

#define PT_EXTRTMPL_IMGTYPE_STRIP_SENSOR    1    ///< A strip sensor was used for image acquisition.
#define PT_EXTRTMPL_IMGTYPE_AREA_SENSOR    2    ///< An area sensor was used for image acquisition.




/** 
 * Structure with image information needed for template extraction.
 */
typedef struct pt_extrtmpl_imginfo {
  PT_WORD Length; 	///< Length of this structure, set to 'sizeof(PT_EXTRTMPL_IMGINFO)'.
  PT_WORD Version; 	///< Version of this structure (set to '1')
  PT_WORD ImgWidth; 	///< Width of the image in pixels.
  PT_WORD ImgHeight; 	///< Height of the image in pixels.
  PT_WORD RowDpi; 	///< Horizontal resolution (dpi) of the image (currently supported values are 381 or 508 only).
  PT_WORD ColumnDpi; 	///< Vertical resolution (dpi) of the image (currently supported values are 381 or 508 only).
  PT_WORD ColorDepth; 	///< Color bit depth of the image (currently supported value is 8 only).
  PT_SHORT BgColor; 	///< Background color of the image. Value of -1 means unknown value and the color will be assessed automatically.
  PT_WORD ImgType; 	///< Type of the sensor used for image acquisition (see PT_EXTRTMPL_IMGTYPE_xxxx values).
  PT_BYTE reserved[14]; 	///< Reserved, set to 0
} PT_EXTRTMPL_IMGINFO;




/** 
 * Structure with information about extracted template.
 */
typedef struct pt_extrtmpl_tmplinfo {
  PT_WORD Length; 	///< Length of this structure
  PT_WORD Version; 	///< Version of this structure (currently always 1)
  PT_BYTE reserved[28]; 	///< Reserved, set to 0
} PT_EXTRTMPL_TMPLINFO;




/** 
 * Prototype of callback function that an application supplies to allow the FM
 * API to display GUI state information to the user, and to receive responses back.
 * 
 * @param pGuiStateCallbackCtx A generic pointer to context information that was provided by the original requester
 * and is being returned to its originator.
 * @param dwGuiState A bitmask indicating the current GUI state plus an indication
 * of what others parameters are available. It can be combined from values
 * PT_SAMPLE_AVAILABLE, PT_MESSAGE_PROVIDED and PT_PROGRESS_PROVIDED.
 * @param pbyResponse The response from the application back to the PerfectTrust Proxy
 * API on return from the callback. Can be one of values PT_CANCEL or PT_CONTINUE.
 * Other values are reserved for future use.
 * @param dwMessage The number of a message to display to the user. For message numbers
 * see PT_GUIMSG_XXXX. GuiState indicates if a Message is provided; if not
 * the parameter is 0.
 * @param byProgress A Value that indicates (as a percentage) the amount of progress
 * in the development of a Sample/BIR. The value may be used to display a progress
 * bar. GuiState indicates if a sample Progress value is provided in the call;
 * if not the parameter is 0. Currently used only to indicate progress of dynamic
 * enrollment operation.
 * @param pSampleBuffer The current sample buffer for the application to display.
 * GuiState indicates if a sample Buffer is provided; if not the parameter is
 * NULL. This parameter is reserved for future use, currently it is always NULL.
 * The buffer is allocated and controlled by PerfectTrust, it must not be freed.
 * @param pData Optional data, which may be available for some GUI message codes.
 * If no data is provided the parameter is NULL. The data is allocated and
 * controlled by PerfectTrust, it must not be freed.
 * @return Status code.
 */
typedef PT_STATUS (PTAPI  *PT_GUI_STATE_CALLBACK) (
    IN void *pGuiStateCallbackCtx,
    IN PT_DWORD dwGuiState,
    OUT PT_BYTE *pbyResponse,
    IN PT_DWORD dwMessage,
    IN PT_BYTE byProgress,
    IN void *pSampleBuffer,
    IN PT_DATA *pData);



 
/** 
 * A type of the callback function that the application must supply to the PTNavigate
 * and PTClickCalibrate functions to receive the navigation data.
 * 
 * @param pNavigationCallbackCtx A generic pointer to context information that was provided when calling PTNavigate or PTClickCalibrate.
 * @param pNavigationData The received navigation data.
 * @param pbyResponse The response from the application back to the PTAPI on return
 * from the callback. Can be one of the values PT_CANCEL or PT_CONTINUE. Other
 * values are reserved for future use.
 * @return Status code.
 */
typedef PT_STATUS (PTAPI  *PT_NAVIGATION_CALLBACK) (
    IN void *pNavigationCallbackCtx,
    IN PT_NAVIGATION_DATA *pNavigationData,
    OUT PT_BYTE *pbyResponse);





/** 
 * A type of the callback function that the application could optionally
 * supply to the PTSleep function to be periodically called while FM is
 * in a sleep or standby mode.
 * 
 * @param pIdleCallbackCtx A generic pointer to context information that was provided when calling PTSleep.
 * @param pbyResponse The response from the application back to the PTAPI on return
 * from the callback. Can be one of the values PT_SLEEP_STOP,
 * i.e. wakeup, or PT_SLEEP_CONTINUE, i.e. continue to sleep. Other
 * values are reserved for future use.
 * @return Status code.
 */
typedef PT_STATUS (PTAPI  *PT_IDLE_CALLBACK) (
    IN void *pIdleCallbackCtx,
    OUT PT_BYTE *pbyResponse);




                                           
/** 
 * Prototype of callback function that an application supplies to allow to send data
 * in the form of a sequence of packets to the remote instance of PTAPI library. This
 * is a blocking function, which does not return until the request is processed, or
 * a communication error is encountered.
 * 
 * @param pCommCallbackCtx A generic pointer to context information that was provided by the original
 * requester and is being returned to its originator.
 * @param pInPkt A pointer to a packet to be sent to the communicating partner. This data is
 * dynamically allocated and controlled by PTAPI, must not be freed by the application.
 * @param ppOutPkt An address of a pointer to be set to point to the packet received
 * back from the communicating partner. The data has to be allocated by the
 * callback function and will be released by the PTAPI library.
 * @return Status code.
 */
typedef PT_STATUS (PTAPI  *PT_COMM_CALLBACK) (
    IN void *pCommCallbackCtx,
    IN PT_DATA *pInPkt,
    OUT PT_DATA **ppOutPkt);





/** 
 * GUI state flags
 */

#define PT_MESSAGE_PROVIDED    (0x1)    ///< Value indicating that message parameter contains valid value.
#define PT_PROGRESS_PROVIDED    (0x2)    ///< Value indicating that progress parameter contains valid value with progress of the operation.
#define PT_SAMPLE_IMAGE_PROVIDED    (0x4)    ///< Value indicating that sample image is provided.
#define PT_DATA_PROVIDED    (0x8)    ///< Value indicating that additional data are provided.



/** 
 * GUI sample image
 */
typedef struct pt_gui_sample_image {
  PT_DWORD Width; 	///< Image width
  PT_DWORD Height; 	///< Image height
  PT_DATA ImageData; 	///< ImageData
} PT_GUI_SAMPLE_IMAGE;



/** 
 * GUI callback response values
 */

#define PT_CANCEL    0x00    ///< Cancel biometric operation.
#define PT_CONTINUE    0x01    ///< Continue with biometric operation.



/** 
 * GUI callback messages
 */

#define PT_GUIMSG_GOOD_IMAGE    0    ///< Scanned good image
#define PT_GUIMSG_NO_FINGER    1    ///< No finger detected
#define PT_GUIMSG_TOO_LIGHT    2    ///< Finger image is too light
#define PT_GUIMSG_TOO_DRY    3    ///< Finger is too dry
#define PT_GUIMSG_TOO_DARK    4    ///< Finger image is too dark
#define PT_GUIMSG_TOO_HIGH    5    ///< Finger is too high
#define PT_GUIMSG_TOO_LOW    6    ///< Finger is too low
#define PT_GUIMSG_TOO_LEFT    7    ///< Finger is too left
#define PT_GUIMSG_TOO_RIGHT    8    ///< Finger is too right
#define PT_GUIMSG_TOO_SMALL    9    ///< Finger image is too small
#define PT_GUIMSG_TOO_STRANGE    10    ///< Finger image is too strange
#define PT_GUIMSG_BAD_QUALITY    11    ///< Finger has bad quality
#define PT_GUIMSG_PUT_FINGER    12    ///< Put finger 1st time
#define PT_GUIMSG_PUT_FINGER2    13    ///< Put finger 2nd time
#define PT_GUIMSG_PUT_FINGER3    14    ///< Put finger 3rd time
#define PT_GUIMSG_REMOVE_FINGER    15    ///< Remove finger
#define PT_GUIMSG_CONSOLIDATION_FAIL    16    ///< Consolidation failed
#define PT_GUIMSG_CONSOLIDATION_SUCCEED    17    ///< Consolidation succeed
#define PT_GUIMSG_CLEAN_SENSOR    18    ///< Clean the sensor
#define PT_GUIMSG_KEEP_FINGER    19    ///< Keep finger on the sensor
#define PT_GUIMSG_START    20    ///< Non specific GUI start (not used now)
#define PT_GUIMSG_VERIFY_START    21    ///< GUI start for verification
#define PT_GUIMSG_ENROLL_START    22    ///< GUI start for enrollment
#define PT_GUIMSG_FINGER_DETECT_START    23    ///< GUI start for finger detection
#define PT_GUIMSG_GUI_FINISH    24    ///< GUI finished without specification of success of biometric operation
#define PT_GUIMSG_GUI_FINISH_SUCCEED    25    ///< GUI finished signaling success of biometric operation
#define PT_GUIMSG_GUI_FINISH_FAIL    26    ///< GUI finished signaling failure of biometric operation
#define PT_GUIMSG_CALIB_START    27    ///< GUI start for sensor calibration
#define PT_GUIMSG_TOO_FAST    28    ///< Finger was swiped too fast
#define PT_GUIMSG_TOO_SKEWED    29    ///< Finger was too skewed during swipe
#define PT_GUIMSG_TOO_SHORT    30    ///< Finger swipe was too short
#define PT_GUIMSG_TOUCH_SENSOR    31    ///< Touch sensor with finger
#define PT_GUIMSG_PROCESSING_IMAGE    32    ///< Image processing started
#define PT_GUIMSG_SWIPE_IN_PROGRESS    33    ///< Finger swipe is in progress
#define PT_GUIMSG_BACKWARD_MOVEMENT    34    ///< Backward movement detected
#define PT_GUIMSG_JOINT_DETECTED    35    ///< Finger joint detected
#define PT_GUIMSG_CENTER_AND_PRESS_HARDER    36    ///< Center finger and press harder
#define PT_GUIMSG_IMAGE_PROCESSED    37    ///< Image processing finished
#define PT_GUIMSG_PUT_FINGER4    38    ///< Put finger 4th time
#define PT_GUIMSG_PUT_FINGER5    39    ///< Put finger 5th time
#define PT_GUIMSG_NO_MATCH    45    ///< Finger doesn't match
#define PT_GUIMSG_ENROLL_PROGRESS    46    ///< Signal progress of dynamic enrollment


/*
#define PT_GUIMSG_PUT_FINGER6			40
#define PT_GUIMSG_PUT_FINGER7			41
#define PT_GUIMSG_PUT_FINGER8			42
#define PT_GUIMSG_PUT_FINGER9			43
#define PT_GUIMSG_PUT_FINGER10			44
*/

/**
 * Prototype of callback function which deals with the presentation of biometric
 * streaming data. Currently not implemented.
 */
typedef PT_STATUS (PTAPI *PT_GUI_STREAMING_CALLBACK) (void);



#endif /* tfmtypes_h */



