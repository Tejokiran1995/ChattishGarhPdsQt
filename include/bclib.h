/**
*
* bclib.h
*
*  Biodata Conversion Library interface.
      BCLib provides the interface for biometric data conversions
      between AuthenTec proprietary formats, BioAPI and ANSI/ISO formats
*
* (c) AuthenTec 2006-2011
*
* Author: 
*
**/

#ifndef __BCLIB_H__
#define __BCLIB_H__

    


//------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------


#if (defined(_WIN32) || defined(_WIN32_WCE)) && !defined(WIN32)
#define WIN32
#endif

#if defined(BC_PLATFORM_TBX16) && !defined(_WIN16)
#define _WIN16
#endif

#if defined(_WIN16) && !defined(WIN16)
#define WIN16
#endif

#if defined(WIN16)

// Disable some level 4 warnings for MSVC 1.5

#pragma warning (disable: 4001)
#pragma warning (disable: 4705)

#endif //#if defined(WIN16)

// Macro used for compile time checking

#define BC_COMPILE_TIME_ASSERT(cond, msg)  \
    typedef char msg[(cond) ? 1 : -1]
/*
 * This is a definition which has sole purpose of
 * helping readability.  It indicates that formal
 * parameter is an output parameter.
*/
#ifndef IN
#define IN
#endif


/*
 * This is a definition which has sole purpose of
 * helping readability.  It indicates that formal
 * parameter is an output parameter.
*/
#ifndef OUT
#define OUT
#endif

/*
 * This is a definition which has sole purpose of
 * helping readability.  It indicates that formal
 * parameter is both input and output parameter.
*/
#ifndef INOUT
#define INOUT
#endif


//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------

#ifdef __cplusplus

    #define BEGIN_EXTERNC extern "C" {
    #define END_EXTERNC   }

#else  // #ifdef __cplusplus

    #define BEGIN_EXTERNC
    #define END_EXTERNC

#endif // #ifdef __cplusplus

// Macros for declaration and definition of API functions 

#if defined(__SYMBIAN32__) && !defined(BC_FORCE_DYNAMIC)

#define BC_DECL_DLLEXP(ReturnType, Name, Params) ReturnType Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) ReturnType Name Params

#else

#if defined(_WIN64) && defined(BC_FORCE_DYNAMIC)
#define BC_DECL_DLLEXP(ReturnType, Name, Params) __declspec (dllexport) ReturnType __stdcall Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) __declspec (dllimport) ReturnType __stdcall Name Params
#endif //#if defined(WIN32) && defined(BC_FORCE_DYNAMIC)      

#if defined(WIN32) && defined(BC_FORCE_DYNAMIC)
#define BC_DECL_DLLEXP(ReturnType, Name, Params) __declspec (dllexport) ReturnType __stdcall Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) __declspec (dllimport) ReturnType __stdcall Name Params
#endif //#if defined(WIN32) && defined(BC_FORCE_DYNAMIC)      

#if defined(WIN16) && defined(BC_FORCE_DYNAMIC)
#define BC_DECL_DLLEXP(ReturnType, Name, Params) ReturnType __export Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) ReturnType Name Params
#endif //#if defined(WIN16) && defined(BC_FORCE_DYNAMIC)

#if defined(WIN32) && !defined(BC_FORCE_DYNAMIC)
#if defined(BC_PLATFORM_WIN32CE)
#define BC_DECL_DLLEXP(ReturnType, Name, Params) ReturnType __cdecl Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) ReturnType __cdecl Name Params
#else
#define BC_DECL_DLLEXP(ReturnType, Name, Params) ReturnType __stdcall Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) ReturnType __stdcall Name Params
#endif 
#endif // static lib under win32/win32ce

#if defined(WIN16) && !defined(FORCE_DYNAMIC)
#define BC_DECL_DLLEXP(ReturnType, Name, Params) ReturnType Name Params
#define BC_DECL_DLLIMP(ReturnType, Name, Params) ReturnType Name Params
#endif

#endif

// Trivial definitions of DECL_DLLEXP and DECL_DLLIMP, if not defined

#if !defined(BC_DECL_DLLEXP)
#define BC_DECL_DLLEXP(ReturnType, Name, Params) ReturnType Name Params
#endif 

#if !defined(BC_DECL_DLLIMP)
#define BC_DECL_DLLIMP(ReturnType, Name, Params) ReturnType Name Params
#endif 

// Defining and declaring macros for functions

#if defined(__SYMBIAN32__)

#define BC_DEFN_API_FUNC(ReturnType, Name, Params) ReturnType Name Params

#if defined(__cplusplus)

#if defined(BCLIB_EXPORT)
#define BC_DECL_API_FUNC(ReturnType, Name, Params) __declspec (dllexport) ReturnType Name Params
#else
#define BC_DECL_API_FUNC(ReturnType, Name, Params) __declspec (dllexport) ReturnType Name Params
#endif

#else

#if defined(BCLIB_EXPORT)
#define BC_DECL_API_FUNC(ReturnType, Name, Params) __declspec (dllexport) ReturnType Name Params
#else
#define BC_DECL_API_FUNC(ReturnType, Name, Params) __declspec (dllexport) ReturnType Name Params
#endif

#endif

#else   // #ifdef __SYMBIAN32__

#ifdef BCLIB_EXPORT
#define BC_DEFN_API_FUNC(ReturnType, Name, Params) BC_DECL_DLLEXP(ReturnType, Name, Params)
#define BC_DECL_API_FUNC(ReturnType, Name, Params) BC_DECL_DLLEXP(ReturnType, Name, Params)
#else
#define BC_DEFN_API_FUNC(ReturnType, Name, Params) BC_DECL_DLLIMP(ReturnType, Name, Params)
#define BC_DECL_API_FUNC(ReturnType, Name, Params) BC_DECL_DLLIMP(ReturnType, Name, Params)
#endif  // #ifdef BCLIB_EXPORT

#endif  // #ifdef __SYMBIAN32__



// ---- Error codes -----------------------

/*
BCLib error codes that can be returned by any API function.

All functions of the API are designed the way that they return
a sint32 value as a return code. In all the cases the return code
signals only the success or failure of the operation. All eventual
outputs of the function are passed via output arguments.

In order to allow maximum flexibility the error codes of BCLib
are offset. Hopefully this allows the caller to better fit it
into its own error code space. 
*/

/*
This is the standard offset of all codes. This symbol allows to eventually change the offset of BCLib error
codes if required.
*/
#define BCERR_STD_OFFSET (-30000)

/*
OK status.

Signals a successful completion of the operation. This is the only
code which is fixed and not affected by BCERR_STD_OFFSET value.
*/
#define BCERR_OK (0)

/*
Generic error.
 
 A code for an unknown or unexpected error. It should not be returned in
 a non-experimental version of the library. If it is not true, please contact
 the authors.
*/
#define BCERR_ERROR (BCERR_STD_OFFSET - 1)

/*
Memory allocation failure.

This error code signals inability to allocate a block of memory. Specifically,
the code can be returned in the following situations:
- The library was not able to allocate some of the memory pools, which are used
  for the internal dynamic memory allocation during the execution of the given
  API function call.
- The internal memory allocation exceeded the space available in the given
  internal memory pool.
- Library was forced to allocate a memory block of zero size.
*/
#define BCERR_MEMORY_ALLOCATION (BCERR_STD_OFFSET - 2)

/*
Bad parameter was provided.

This code indicates that an incorrect input was provided to the API function.
Most frequently it is returned when some argument of the function is passed
as NULL when not allowed.
*/
#define BCERR_BAD_PARAMETER (BCERR_STD_OFFSET - 3)

/*
Functionality is not supported by this library version.

Functionality or type of information is not supported by the given version or
variant of the library.
*/
#define BCERR_NOT_SUPPORTED (BCERR_STD_OFFSET - 5)

/*
Provided buffer is too small.

This code signals when output buffer provided to the function is too small
so the potential result of the operation cannot fit it. 
*/
#define BCERR_BUFFER_TOO_SMALL (BCERR_STD_OFFSET - 6)

/*
Unknown envelope type. 
Unknown envelope type was provided or requested.
*/
#define BCERR_UNKNOWN_ENVELOPE (BCERR_STD_OFFSET - 7)

/*
Unknown data type.
Unknown data type was provided or requested.
*/
#define BCERR_UNKNOWN_DATA (BCERR_STD_OFFSET - 8)

/*
Error in input decoding.
*/
#define BCERR_DECODING_INPUT (BCERR_STD_OFFSET - 9)

/*
Error in output encoding.
*/
#define BCERR_ENCODING_OUTPUT (BCERR_STD_OFFSET - 10)

/*
Error in data format.
*/
#define BCERR_BAD_DATA_FORMAT (BCERR_STD_OFFSET - 11)

/*
Missing information.
Some necessary information is missing in input data and suplementary
structure was not provided.
*/
#define BCERR_MISSING_INFORMATION (BCERR_STD_OFFSET - 12)

/*
Templates or images cannot be combined.
*/
#define BCERR_INCOMPATIBLE_DATA (BCERR_STD_OFFSET - 13)

/*
Internal error.
*/
#define BCERR_INTERNAL  (BCERR_STD_OFFSET - 99)


// ---- ANSI and ISO values. -----------------------

/*
Values to be used in BCLib template conversion
   ANSI and ISO values for finger position, impression type, and finger quality
   
*/

// Finger quality values.
#define QUAL_POOR 20// NFIQ value 5

#define QUAL_FAIR 40// NFIQ value 4

#define QUAL_GOOD 60// NFIQ value 3

#define QUAL_VGOOD 80// NFIQ value 2

#define QUAL_EXCELLENT 100// NFIQ value 1



// Impression type codes.
#define IMPTYPE_LP 0x00// Live-scan plain

#define IMPTYPE_NP 0x02// Nonlive-scan plain

#define IMPTYPE_LS 0x08// Live-scan swipe



// Finger position codes
#define FINGPOS_UK 0x00// Unknown finger

#define FINGPOS_RT 0x01// Right thumb

#define FINGPOS_RI 0x02// Right index finger

#define FINGPOS_RM 0x03// Right middle finger

#define FINGPOS_RR 0x04// Right ring finger

#define FINGPOS_RL 0x05// Right little finger

#define FINGPOS_LT 0x06// Left thumb

#define FINGPOS_LI 0x07// Left index finger

#define FINGPOS_LM 0x08// Left middle finger

#define FINGPOS_LR 0x09// Left ring finger

#define FINGPOS_LL 0x0A// Left little finger




// ---- Types -----------------------
  
#ifndef BC_USE_EXTERNAL_TYPES

// Define necessary types

#if defined(__GNUC__)

// GNU compiler used

typedef signed int          sint32;
typedef unsigned int        uint32;

#else

// Other platforms

typedef signed long         sint32;
typedef unsigned long       uint32;

#endif

typedef signed short        sint16;
typedef unsigned short      uint16;
typedef float               float32;

#endif


// Checking proper size of types

BC_COMPILE_TIME_ASSERT(sizeof(uint32) == 4, bc_uint32IsNot32bit);
BC_COMPILE_TIME_ASSERT(sizeof(sint32) == 4, bc_sint32IsNot32bit);
BC_COMPILE_TIME_ASSERT(sizeof(uint16) == 2, bc_uint16IsNot16bit);
BC_COMPILE_TIME_ASSERT(sizeof(sint16) == 2, bc_sint16IsNot16bit);
BC_COMPILE_TIME_ASSERT(sizeof(float32) == 4, bc_float32IsNot32bit);

// Template Envelopes
/* Envelope types for biometric templates
*/

typedef enum _tag_BCTemplateEnvelopeType {
     BCTE_NOT_SPECIFIED =  -1,    //Not specified.
     BCTE_NONE =  0,    //No envelope.
     BCTE_PT_BIR,    //PT_BIR
     BCTE_ABS_BIR,    //ABS_BIR
     BCTE_BIOAPI_BIR,    //BioAPI_BIR - currently not supported.
     BCTE_LAST   //Last value mark
} BCTemplateEnvelopeType;

// Image Envelopes
/* Envelope types for images
*/

typedef enum _tag_BCImageEnvelopeType {
     BCIE_NOT_SPECIFIED =  -1,    //Not specified
     BCIE_NONE =  0,    //No envelope
     BCIE_ABS_IMAGE =  1,    //ABS_SAMPLE_IMAGE
     BCIE_BIOAPI_GUI_BITMAP,    //BioAPI_GUI_BITMAP - currently not supported.
     BCIE_PT_DIF,    //PT_DIF
     BCIE_LAST   //Last value mark
} BCImageEnvelopeType;

// Template Data Types
/* Inner data types for biometric template
*/

typedef enum _tag_BCTemplateDataType {
     BCT_NOT_SPECIFIED =  -1,    //Not specified
     BCT_NONE =  0,    //None - no data
     BCT_UPEK_LEGACY,    //AuthenTec legacy
     BCT_UPEK_ALPHA,    //AuthenTec alpha
     BCT_UPEK_BETA,    //AuthenTec beta
     BCT_UPEK_AUTO,    //AuthenTec automatic - on input format is detected, on output the best format is chosen (alpha or beta)
     BCT_ANSI,    //ANSI template - not supported in the light variant of the library
     BCT_ISO_FMR,    //ISO template, Finger Minutiae Record format - not supported in the light variant of the library
     BCT_ISO_FMC_NORMAL,    //ISO template, Finger Minutiae Card format (normal) - not supported in the light variant of the library
     BCT_ISO_FMC_COMPACT,    //ISO template, Finger Minutiae Card format (compact) - not supported in the light variant of the library
     BCT_LAST   //Last value mark
} BCTemplateDataType;

// Image Data Types
/* Inner data types for image
*/

typedef enum _tag_BCImageDataType {
     BCI_NOT_SPECIFIED =  -1,    //Not specified
     BCI_NONE =  0,    //None - no data present.
     BCI_RAW,    //Raw image
     BCI_ANSI,    //ANSI image.
     BCI_ISO,    //ISO image
     BCI_LAST   //Last value mark
} BCImageDataType;

/* Inner data types for image
*/

typedef enum _tag_BCImageCompressionType {
     BCIC_NOT_SPECIFIED =  -1,    //Not specified
     BCIC_UNCOMPRESSED =  0,    //Uncompressed
     BCIC_UNCOMPRESSED_BITPACKED =  1,    //Uncompressed, bit packed - not supported.
     BCIC_COMPRESSED_WSQ =  2,    //Compressed via WSQ - not supported in the light variant of the library
     BCIC_COMPRESSED_JPEG =  3,    //Compressed via JPEG - not supported.
     BCIC_COMPRESSED_JPEG2000 =  4,    //Compressed via JPEG2000 - not supported in the light variant of the library
     BCIC_COMPRESSED_PNG =  5   //Compressed via PNG - not supported.
} BCImageCompressionType;

/* Image info type
*/

typedef enum _tag_BCImageType {
     BCIT_NOT_SPECIFIED =  -1,    //Information not provided.
     BCIT_UNKNOWN =  0,    //Unknown image type.
     BCIT_SWIPE_SENSOR =  1,    //Image captured by swipe sensor e.g. TCS5.
     BCIT_AREA_SENSOR =  2   //Image captured by area sensor e.g. TCS1.
} BCImageType;

#define BC_VALUE_NOT_SPECIFIED 0xffffffff// Constant used in supplemetary structure for values not to be changed


// Image info type.
/* 
 This structure serves as a supplementary source of information for conversion,
 if some information requested by output format is not contained in input format.
 Data in this structure has higher priority than data contained in input template.
 Use value BC_VALUE_NOT_SPECIFIED for members which are not to be applied.
*/

typedef struct _tag_BCImageInfoType_1 {
  sint32   version; 
    //Structure version, must be 1.
  uint32   captureCBEFFPid; 
    //Capture system CBEFF product identifier.
  uint32   captureEquipmentCompliance; 
    //Compliance bits according to ANSI template standard paragraph 6.4.5.  Note: Only the lowest 4 bits are used.
  uint32   captureEquipmentId; 
    //Identification of equipment assigned by its manufacturer. Note: Only the lowest 12 bits are used.
  sint32   width; 
    //Image width.
  sint32   height; 
    //Image height.
  sint32   horizontalScanResolution; 
    //Horizontal scan resolution (dpi) of the image.
  sint32   verticalScanResolution; 
    //Vertical scan resolution (dpi) of the image.
  sint32   horizontalImageResolution; 
    //Horizontal image resolution (dpi).
  sint32   verticalImageResolution; 
    //Vertical image resolution (dpi).
  sint32   colorDepth; 
    //Color bit depth of the image.
  sint32   bgColor; 
    //The background color of the image.
  BCImageType   imageType; 
    //Type of the sensor used for image acquisition.
  BCImageCompressionType   compression; 
    //Compression algorithm used for input data.
  sint32   quality; 
    //Image quality (0..255).
} BCImageInfoType_1;


// Image info type.
/* 
Version 2: extended by outputWsqBitRate

This structure serves as a supplementary source of information for conversion,
if some information requested by output format is not contained in input format.
Data in this structure has higher priority than data contained in input template.
Use value BC_VALUE_NOT_SPECIFIED for members which are not to be applied.
            
*/

typedef struct _tag_BCImageInfoType_2 {
  sint32   version; 
    //Structure version, must be 2.
  uint32   captureCBEFFPid; 
    //Capture system CBEFF product identifier.
  uint32   captureEquipmentCompliance; 
    //Compliance bits according to ANSI template standard paragraph 6.4.5. Note: Only the lowest 4 bits are used.
  uint32   captureEquipmentId; 
    //Identification of equipment assigned by its manufacturer. Note: Only the lowest 12 bits are used.
  sint32   width; 
    //Image width.
  sint32   height; 
    //Image height.
  sint32   horizontalScanResolution; 
    //Horizontal scan resolution (dpi) of the image.
  sint32   verticalScanResolution; 
    //Vertical scan resolution (dpi) of the image.
  sint32   horizontalImageResolution; 
    //Horizontal image resolution (dpi).
  sint32   verticalImageResolution; 
    //Vertical image resolution (dpi).
  sint32   colorDepth; 
    //Color bit depth of the image.
  sint32   bgColor; 
    //The background color of the image.
  BCImageType   imageType; 
    //Type of the sensor used for image acquisition.
  BCImageCompressionType   compression; 
    //Compression algorithm used for input data.
  sint32   quality; 
    //Image quality (0..255).
  float32   outputWsqBitRate; 
    /*
    In case WSQ compression is requested for the output, this
parameter specifies the bitrate to be used. For other types of compression it
is ignored. It is also ignored if the input is already compressed by WSQ i.e.
no re-compression will happen.
    */

} BCImageInfoType_2;


// Template info type.
/* 
This structure serves as a supplementary source of information for conversion,
if some information requested by output format is not contained in input format.
Data in this structure has higher priority than data contained in input template.
Use value BC_VALUE_NOT_SPECIFIED for members which are not to be applied.
            
*/

typedef struct _tag_BCTemplateInfoType_1 {
  uint32   version; 
    //Structure version, must be 1.
  uint32   captureCBEFFPid; 
    //Capture system CBEFF product identifier.
  uint32   captureEquipmentCompliance; 
    //Compliance bits according to ANSI template standard paragraph 6.4.5. Note: Only the lowest 4 bits are used.
  uint32   captureEquipmentId; 
    //Identification of equipment assigned by its manufacturer. Note: Only the lowest 12 bits are used.
  uint32   imageWidth; 
    //Original image width (in pixels)
  uint32   imageHeight; 
    //Original image height (in pixels)
  uint32   imageHorizontalResolution; 
    //Original image horizontal resolution (in dpi)
  uint32   imageVerticalResolution; 
    //Original image vertical resolution (in dpi)
  uint32   fingerPosition; 
    //Finger position as in ANSI/ISO.
} BCTemplateInfoType_1;


// Template info type.
/* 
Version 2: extended by Finger Quality, Impression Type and dpiConversionSwitch

This structure serves as a supplementary source of information for conversion,
if some information requested by output format is not contained in input format.
Data in this structure has higher priority than data contained in input template.
Use value BC_VALUE_NOT_SPECIFIED for members which are not to be applied.
            
*/

typedef struct _tag_BCTemplateInfoType_2 {
  uint32   version; 
    //Structure version, must be 2.
  uint32   captureCBEFFPid; 
    //Capture system CBEFF product identifier.
  uint32   captureEquipmentCompliance; 
    //Compliance bits according to ANSI template standard paragraph 6.4.5. Note: Only the lowest 4 bits are used.
  uint32   captureEquipmentId; 
    //Identification of equipment assigned by its manufacturer. Note: Only the lowest 12 bits are used. 
  uint32   imageWidth; 
    //Original image width (in pixels)
  uint32   imageHeight; 
    //Original image height (in pixels)
  uint32   imageHorizontalResolution; 
    //Original image horizontal resolution (in dpi)
  uint32   imageVerticalResolution; 
    //Original image vertical resolution (in dpi)
  uint32   fingerPosition; 
    //Finger position as in ANSI/ISO (see FINGERPOS* constants).
  uint32   fingerQuality; 
    //Finger quality as in ANSI/ISO (overrides 'confidence' of Alpha template).
  uint32   impressionType; 
    //Impression type as in ANSI/ISO (IMPTYPE_LP / IMPTYPE_NP / IMPTYPE_LS).
  uint32   dpiConversionSwitch; 
    /*
    
DPI conversion switch: if positive, the minutia positions will be recalculated 
to the target resolution (imageHorizontalResolution, imageVerticalResolution).
If dpiConversionSwitch==0, the supplied resolution will be set in the template header,
but the minutia coordinates will not be recalculated.
    */

} BCTemplateInfoType_2;


/* 
This structure contains information on individual template finger view. The information
can be used for access to the split view information.
Version 1.
         
*/

typedef struct _tag_BCTemplateViewInfoType_1 {
  void *   pViewTemplate; 
    //Points to the template based on the given template view.
  uint32   viewTemplateLength; 
    //Length of the template based on the given template view.
} BCTemplateViewInfoType_1;



// ---- Function prototypes -----------------------

BEGIN_EXTERNC

// Returns the version of the library.
/* 
This function provides the basic information about the library version. 
It is the only function that can be called outside of the BCLib session (i.e. without prior call to BCInit()).
     
*/

BC_DECL_API_FUNC(sint32, BCGetVersion,

(
    OUT uint32 * pMajor,
    OUT uint32 * pMinor
));

// Converts biometric template data.
/* 
Converts template data from input type/envelope to output type/envelope.
Note: Conversion cannot use the same buffer both for input and output.
        
*/

BC_DECL_API_FUNC(sint32, BCConvertTemplate,

(
    IN BCTemplateEnvelopeType inputEnvelope,
    IN BCTemplateDataType inputDataType,
    IN const void * pInput,
    IN uint32 inputLength,
    IN BCTemplateEnvelopeType outputEnvelope,
    IN BCTemplateDataType outputDataType,
    INOUT void * pOutput,
    INOUT uint32 * pOutputLength,
    IN const void * pSuplementaryInfo
));

// Rotates biometric template data.
/* 
Rotates minutiae within a template. The rotation angle is specified in 
steps of 1/256 of pi. Positive angle means clock-wise rotation. The operation 
preserves the data and envelope type.
Note: Operation cannot use the same buffer both for input and output.
        
*/

BC_DECL_API_FUNC(sint32, BCRotateTemplate,

(
    IN BCTemplateEnvelopeType inputEnvelope,
    IN BCTemplateDataType inputDataType,
    IN const void * pInput,
    IN uint32 inputLength,
    IN sint32 rotationAngle,
    INOUT void * pOutput,
    INOUT uint32 * pOutputLength,
    IN const void * pSuplementaryInfo
));

// Joins two templates into one template.
/* 
If the given format allows it, two templates can be stored as one template 
with multiple "views" of one or more fingers. It is possible to join only 
templates of the same type (both template and envelope). It is also assumed 
the data were captured by the same device.
Both input templates can already contain multiple views prior the operation.
*/

BC_DECL_API_FUNC(sint32, BCJoinTemplates,

(
    IN BCTemplateEnvelopeType envelope,
    IN BCTemplateDataType dataType,
    IN const void * pInput1,
    IN uint32 input1Length,
    IN const void * pInput2,
    IN uint32 input2Length,
    INOUT void * pOutput,
    INOUT uint32 * pOutputLength,
    IN const void * pSuplementaryInfo
));

// Splits a template containing multiple views into individual templates.
/* 
By this function it is possible to split such a template containing one or more finger views
back to the individual components. The type (both template and envelope) of the input data
is preserved for the output data.

The function returns all output templates in a contiguous buffer (pOutput). 
The access to individual templates is achieved via an array of information
structures containing pointers and lengths of the templates (pViewInfoArray).
        
*/

BC_DECL_API_FUNC(sint32, BCSplitTemplate,

(
    IN BCTemplateEnvelopeType envelope,
    IN BCTemplateDataType dataType,
    IN const void * pInput,
    IN uint32 inputLength,
    IN uint32 viewInfoVariant,
    INOUT void * pViewInfoArray,
    INOUT uint32 * pNumViews,
    INOUT void * pOutput,
    INOUT uint32 * pOutputLength,
    IN const void * pSuplementaryInfo
));

// Converts biometric image data.
/* 
Converts image data/envelope from input type to output type/envelope.
Note: Conversion cannot use the same buffer both for input and output.
    
*/

BC_DECL_API_FUNC(sint32, BCConvertImage,

(
    IN BCImageEnvelopeType inputEnvelope,
    IN BCImageDataType inputDataType,
    IN const void * pInput,
    IN uint32 inputLength,
    IN BCImageEnvelopeType outputEnvelope,
    IN BCImageDataType outputDataType,
    IN BCImageCompressionType outputCompression,
    INOUT void * pOutput,
    INOUT uint32 * pOutputLength,
    IN const void * pSuplementaryInfo
));

// Calculates NIST Fingerprint Image Quality for an image.
/* 
Calculates NIST Fingerprint Image Quality for the provided image of given 
data/envelope type. The input image is not modified.
    
*/

BC_DECL_API_FUNC(sint32, BCGetImageNfiq,

(
    IN BCImageEnvelopeType inputEnvelope,
    IN BCImageDataType inputDataType,
    IN const void * pInput,
    IN uint32 inputLength,
    IN const void * pSuplementaryInfo,
    OUT uint32 * pNfiq
));



END_EXTERNC

#endif // __BCLIB_H__
