/*
 *  XAPI.h
 *  
 *    FUJIFILM X Series Digital Camera Shooting SDK Header file
 *
 *    Version 1.10.0.0
 *
 *  Copyright (C) 2014-2022 FUJIFILM Corporation.   
 *
 */

#ifndef __XAPI_H__
#define __XAPI_H__

//
// Type defines
//
#if defined (_WIN32)  // for Windows
    #include <windows.h>
    typedef HINSTANCE           LIB_HANDLE;
    typedef HANDLE              XSDK_HANDLE;
    #ifdef  _XSDK_INTERNAL_
        #define XSDK_APIENTRY  __declspec(dllexport)    long
    #else
        #define XSDK_APIENTRY  __declspec(dllimport)    long
    #endif
#elif defined (__linux__)
	#include <cstdint>
	typedef	void*			LIB_HANDLE;
	typedef	uint8_t			BYTE;
	typedef	char*			LPSTR;
	typedef const char*		LPCSTR;
	typedef void*			HANDLE;
	typedef	void*			XSDK_HANDLE;
	typedef	long			XSDK_APIENTRY;
#else           // for Macintosh
    #import <CoreFoundation/CoreFoundation.h>
    typedef CFBundleRef         LIB_HANDLE;
    typedef UInt8               BYTE;
    typedef char*               LPSTR;
    typedef const char*         LPCSTR;
    typedef void*               PVOID;
    typedef Handle              XSDK_HANDLE;
    typedef long                XSDK_APIENTRY;
#endif  // _WIN32

//
// Structure defines
//

// Camera List
#pragma pack(1)
typedef struct{
    char    strProduct[256];    // The model name
    char    strSerialNo[256];   // The serial number (USB only)
    char    strIPAddress[256];  // The IPv4 address (network connection only)
    char    strFramework[256];  // USB / ETHER(Ethernet) / IP(Wi-Fi)
    bool    bValid;             // true: valid, false: invalid
} XSDK_CameraList;
#pragma pack()

// Device infomation
#pragma pack(1)
typedef struct{
    char    strVendor[256];
    char    strManufacturer[256];
    char    strProduct[256];
    char    strFirmware[256];
    char    strDeviceType[256];
    char    strSerialNo[256];
    char    strFramework[256];
    BYTE    bDeviceId;
    char    strDeviceName[32];
    char    strYNo[32];
} XSDK_DeviceInformation;
#pragma pack()

// Changed Property
#pragma pack(1)
typedef struct{
    long    n1;
    long    n2;
    long    n3;
    long    n4;
    long    n5;
    long    n6;
    long    n7;
    long    n8;
    long    n9;
    long    n10;
} XSDK_DATA_INT;
#pragma pack()

#pragma pack(1)
typedef union {
    XSDK_DATA_INT   n;
    char            c[40];
} XSDK_DATA;
#pragma pack()

#pragma pack(1)
typedef struct{
    long        apiCode;
    XSDK_DATA   data;
} XSDK_PROPERTY;
#pragma pack()

#pragma pack(1)
typedef struct{
    unsigned short  propCnt;
    XSDK_PROPERTY   prop[200];
} XSDK_CHANGEDPROPERTY;
#pragma pack()

// Image information
#pragma pack(1)
typedef struct{
    char        strInternalName[32];
    long        lFormat;
    long        lDataSize;
    long        lImagePixHeight;
    long        lImagePixWidth;
    long        lImageBitDepth;
    long        lPreviewSize;
    XSDK_HANDLE hImage;
} XSDK_ImageInformation;
#pragma pack()

// Lens Info
#pragma pack(1)
typedef struct{
    char    strModel[20];
    char    strProductName[100];
    char    strSerialNo[20];
    long    lISCapability;
    long    lMFCapability;
    long    lZoomPosCapability;
} XSDK_LensInformation;
#pragma pack()

/////////////////////////////////////////////////////////////////////////////////////////////
//  API Code
enum{
    // Initialize/Finalize
    API_CODE_Init                       = 0x1001,
    API_CODE_Exit                       = 0x1002,

    // Enumeration
    API_CODE_Detect                     = 0x1010,
	API_CODE_Append                     = 0x1012,

    // Session management
    API_CODE_Close                      = 0x1022,
    API_CODE_PowerOFF                   = 0x1023,
    API_CODE_OpenEx                     = 0x1024,

    // Basic functions
    API_CODE_GetErrorNumber             = 0x1031,
    API_CODE_GetVersionString           = 0x1032,
    API_CODE_GetErrorDetails            = 0x1033,

    // Device Information
    API_CODE_GetDeviceInfo              = 0x1041,
    API_CODE_WriteDeviceName            = 0x1042,
    API_CODE_GetFirmwareVersion         = 0x1043,
    API_CODE_GetLensInfo                = 0x1044,
    API_CODE_GetLensVersion             = 0x1045,
    API_CODE_GetDeviceInfoEx            = 0x1047,

    // Camera Operation Mode
    API_CODE_CapPriorityMode            = 0x1101,
    API_CODE_SetPriorityMode            = 0x1102,
    API_CODE_GetPriorityMode            = 0x1103,

    // Release Control
    API_CODE_CapRelease                 = 0x1111,
    API_CODE_Release                    = 0x1112,
    API_CODE_GetReleaseStatus           = 0x1113,
    API_CODE_CapReleaseEx               = 0x1115,
    API_CODE_ReleaseEx                  = 0x1116,
    API_CODE_CapReleaseStatus           = 0x1118,

    // Image Acquisition 
    API_CODE_GetBufferCapacity          = 0x1200,
    API_CODE_ReadImageInfo              = 0x1201,
    API_CODE_ReadPreview                = 0x1202,
    API_CODE_ReadImage                  = 0x1203,
    API_CODE_DeleteImage                = 0x1204,

    // Exposure control
    API_CODE_CapAEMode                  = 0x1301,
    API_CODE_SetAEMode                  = 0x1302,
    API_CODE_GetAEMode                  = 0x1303,
    API_CODE_CapShutterSpeed            = 0x1304,
    API_CODE_SetShutterSpeed            = 0x1305,
    API_CODE_GetShutterSpeed            = 0x1306,
    API_CODE_CapExposureBias            = 0x1307,
    API_CODE_SetExposureBias            = 0x1308,
    API_CODE_GetExposureBias            = 0x1309,
    API_CODE_CapSensitivity             = 0x1311,
    API_CODE_SetSensitivity             = 0x1312,
    API_CODE_GetSensitivity             = 0x1313,
    API_CODE_CapDynamicRange            = 0x1314,
    API_CODE_SetDynamicRange            = 0x1315,
    API_CODE_GetDynamicRange            = 0x1316,
    API_CODE_CapMeteringMode            = 0x1317,
    API_CODE_SetMeteringMode            = 0x1318,
    API_CODE_GetMeteringMode            = 0x1319,

    API_CODE_CapLensZoomPos             = 0x1321,
    API_CODE_SetLensZoomPos             = 0x1322,
    API_CODE_GetLensZoomPos             = 0x1323,
    API_CODE_CapAperture                = 0x1324,
    API_CODE_SetAperture                = 0x1325,
    API_CODE_GetAperture                = 0x1326,

    API_CODE_CapWBMode                  = 0x1331,
    API_CODE_SetWBMode                  = 0x1332,
    API_CODE_GetWBMode                  = 0x1333,
    API_CODE_CapWBColorTemp             = 0x1334,
    API_CODE_SetWBColorTemp             = 0x1335,
    API_CODE_GetWBColorTemp             = 0x1336,

    // Media control
    API_CODE_CapMediaRecord             = 0x1351,
    API_CODE_SetMediaRecord             = 0x1352,
    API_CODE_GetMediaRecord             = 0x1353,

    // Force Mode
    API_CODE_CapForceMode               = 0x1371,
    API_CODE_SetForceMode               = 0x1372,

    // BackupSettings
    API_CODE_SetBackupSettings          = 0x1375,
    API_CODE_GetBackupSettings          = 0x1376,

	// Drive Mode
	API_CODE_SetDriveMode               = 0x1377,
	API_CODE_GetDriveMode               = 0x1378,
	API_CODE_CapDriveMode               = 0x1379,

	// Mode
	API_CODE_CapMode                    = 0x137A,
	API_CODE_SetMode                    = 0x137B,
	API_CODE_GetMode                    = 0x137C,
    
    //Model Dependent Function Call I/F
    API_CODE_CapProp                    = 0x1401,
    API_CODE_SetProp                    = 0x1402,
    API_CODE_GetProp                    = 0x1403,
};


//
// Constants defines
//

// DSC connect I/F
#define XSDK_DSC_IF_USB                     0x00000001  // USB
#define XSDK_DSC_IF_WIFI_LOCAL              0x00000010  // Wi-Fi within local segment
#define XSDK_DSC_IF_WIFI_IP                 0x00000020  // Wi-Fi specified IPv4 address

// Camera Mode
#define XSDK_DSC_MODE_TETHER                0x0001
#define XSDK_DSC_MODE_RAW                   0x0002
#define XSDK_DSC_MODE_BR                    0x0004
#define XSDK_DSC_MODE_WEBCAM                0x0008

// Priority Mode
#define XSDK_PRIORITY_CAMERA                0x0001
#define XSDK_PRIORITY_PC                    0x0002

// Release Mode
#define XSDK_RELEASE_MASK_ONMODE            0xFF00
#define XSDK_RELEASE_MASK_OFFMODE           0x00FF
// (On Mode)
#define XSDK_RELEASE_SHOOT                  0x0100
#define XSDK_RELEASE_S1ON                   0x0200
#define XSDK_RELEASE_S2                     0x0300
#define XSDK_RELEASE_BULB_ON                0x0400
#define XSDK_RELEASE_REC_START				0x0800
#define XSDK_RELEASE_BULBS2_ON              0x0500
#define XSDK_RELEASE_PIXELSHIFT             0x4000
#define XSDK_RELEASE_CUSWB                  0x8000
#define XSDK_RELEASE_AEON                   0x9000
#define XSDK_RELEASE_AFON                   0x9100
#define XSDK_RELEASE_AFAEON                 0x9200
#define XSDK_RELEASE_AF                     0x9300
#define XSDK_RELEASE_INSTANTAF              0xA000
#define XSDK_RELEASE_WBLON					0x9200

// (Off Mode)
#define XSDK_RELEASE_N_AFOFF                0x0001
#define XSDK_RELEASE_N_AEOFF                0x0002
#define XSDK_RELEASE_N_AFAEOFF              0x0003
#define XSDK_RELEASE_N_S1OFF                0x0004
#define XSDK_RELEASE_N_AF                   0x0020
#define XSDK_RELEASE_N_INSTANTAF            0x0010
#define XSDK_RELEASE_N_BULBOFF              0x0008
#define XSDK_RELEASE_REC_STOP				0x0080
#define XSDK_RELEASE_N_BULBS2OFF            0x0008
#define XSDK_RELEASE_N_BULBAFOFF            (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFOFF)
#define XSDK_RELEASE_N_BULBAEOFF            (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AEOFF)
#define XSDK_RELEASE_N_BULBAFAEOFF          (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFAEOFF)
#define XSDK_RELEASE_N_BULBS1OFF            (XSDK_RELEASE_N_BULBS2OFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_N_BULBAFS1OFF          (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_N_BULBAES1OFF          (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_N_BULBAFAES1OFF        (XSDK_RELEASE_N_BULBOFF | XSDK_RELEASE_N_AFAEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_CANCEL                 0x000F
#define XSDK_RELEASE_N_WBLOFF				0x0040

// (On and Off Mode)
#define XSDK_RELEASE_SHOOT_S1OFF            (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_SHOOT_AEOFF            (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_AEOFF)
#define XSDK_RELEASE_SHOOT_AFOFF            (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_AFOFF)
#define XSDK_RELEASE_SHOOT_AFAEOFF          (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_AFAEOFF)
#define XSDK_RELEASE_S2_S1OFF               (XSDK_RELEASE_S2 | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_S2_AFOFF               (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFOFF)                
#define XSDK_RELEASE_S2_AEOFF               (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AEOFF)                
#define XSDK_RELEASE_S2_AFAEOFF             (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFAEOFF)              
#define XSDK_RELEASE_S2_AFS1OFF             (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_S2_AES1OFF             (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_S2_AFAES1OFF           (XSDK_RELEASE_S2 | XSDK_RELEASE_N_AFOFF | XSDK_RELEASE_N_AEOFF | XSDK_RELEASE_N_S1OFF)
#define XSDK_RELEASE_REC_START_S1OFF        (XSDK_RELEASE_REC_START | XSDK_RELEASE_N_S1OFF)

// Release Mode Ex
// (On Mode)
#define XSDK_RELEASE_EX_S1_ON               0x00010000
#define XSDK_RELEASE_EX_S2_ON               0x00020000
#define XSDK_RELEASE_EX_REC_START           0x00040000
#define XSDK_RELEASE_EX_CUSWB_ON            0x00080000
#define XSDK_RELEASE_EX_INSTANTAF_ON        0x01000000
#define XSDK_RELEASE_EX_AEL_ON              0x02000000
#define XSDK_RELEASE_EX_AFL_ON              0x04000000
#define XSDK_RELEASE_EX_AFON_ON             0x08000000
#define XSDK_RELEASE_EX_WBL_ON              0x10000000

// (Off Mode)
#define XSDK_RELEASE_EX_S1_OFF              0x00000001
#define XSDK_RELEASE_EX_S2_OFF              0x00000002
#define XSDK_RELEASE_EX_REC_STOP            0x00000004
#define XSDK_RELEASE_EX_CUSWB_OFF           0x00000008
#define XSDK_RELEASE_EX_CANCEL              0x00000010
#define XSDK_RELEASE_EX_INSTANTAF_OFF       0x00000100
#define XSDK_RELEASE_EX_AEL_OFF             0x00000200
#define XSDK_RELEASE_EX_AFL_OFF             0x00000400
#define XSDK_RELEASE_EX_AFON_OFF            0x00000800
#define XSDK_RELEASE_EX_WBL_OFF             0x00001000

#define XSDK_RELEASE_EX_S2_OFF_S1_OFF       0x00000003

// (On and Off Mode)
#define XSDK_RELEASE_EX_S1_ON_S2_ON_S2_OFF_S1_OFF (XSDK_RELEASE_EX_S1_ON | XSDK_RELEASE_EX_S2_ON | XSDK_RELEASE_EX_S2_OFF | XSDK_RELEASE_EX_S1_OFF)
#define XSDK_RELEASE_EX_S2_ON_S2_OFF_S1_OFF       (XSDK_RELEASE_EX_S2_ON | XSDK_RELEASE_EX_S2_OFF | XSDK_RELEASE_EX_S1_OFF)


// Release AFStatus
#define XSDK_RELEASE_OK                     1           // AF OK or CWB_AE_NOERR
#define XSDK_RELEASE_AF_FAILURE             0           // !AF
#define XSDK_RELEASE_AF_UNCHECK             2           // AF not checked
#define XSDK_RELEASE_AF_NOMOVE              4           // AF no Movement

#define XSDK_RELEASE_CWB_AE_OVER            2           // CWB Exposure Over
#define XSDK_RELEASE_CWB_AE_UNDER           3           // CWB Exposure Under


// Release Status (return in bitmap)
#define XSDK_RELEASE_STATUS_S1              0x0200
#define XSDK_RELEASE_STATUS_BULB            0x0400
#define XSDK_RELEASE_STATUS_AF              0x0800
#define XSDK_RELEASE_STATUS_AEL             0x4000
#define XSDK_RELEASE_STATUS_AFL             0x8000
#define XSDK_RELEASE_STATUS_WBL             0x2000
#define XSDK_RELEASE_STATUS_SHOOTING        0x0100

// Image Format(ReadImageInfo)
#define XSDK_IMAGEFORMAT_RAW                1
#define XSDK_IMAGEFORMAT_LIVE               4
#define XSDK_IMAGEFORMAT_NONE               5
#define XSDK_IMAGEFORMAT_JPEG               7
#define XSDK_IMAGEFORMAT_HEIF               0x0012
#define XSDK_IMAGEFORMAT_JPEG_90            0x0607
#define XSDK_IMAGEFORMAT_JPEG_180           0x0307
#define XSDK_IMAGEFORMAT_JPEG_270           0x0807
#define XSDK_IMAGEFORMAT_RAW_90             0x0601
#define	XSDK_IMAGEFORMAT_RAW_180            0x0301
#define	XSDK_IMAGEFORMAT_RAW_270            0x0801
#define	XSDK_IMAGEFORMAT_LIVE_90            0x0604
#define	XSDK_IMAGEFORMAT_LIVE_180           0x0304
#define	XSDK_IMAGEFORMAT_LIVE_270           0x0804
#define XSDK_IMAGEFORMAT_HEIF_90            0x0612
#define XSDK_IMAGEFORMAT_HEIF_180           0x0312
#define XSDK_IMAGEFORMAT_HEIF_270           0x0812

// AE mode
#define XSDK_AE_OFF                         0x0001      //  Manual Mode
#define XSDK_AE_APERTURE_PRIORITY           0x0003      //  Aperture Priority Mode
#define XSDK_AE_SHUTTER_PRIORITY            0x0004      //  Shutter Priority Mode
#define XSDK_AE_PROGRAM                     0x0006      //  Program Mode

// Aperture
#define XSDK_IRIS_NONE                      0           // F0 or F-- such as MOUNT ADAPTER
#define XSDK_IRIS_AUTO                      0xFFFF      // AUTO, Only for Get

// ShutterSpeed
#define XSDK_SHUTTER_UNKNOWN                0           //  PB mode or SET-UP menu is in use
#define XSDK_SHUTTER_1_180000               5           //  1/180000"
#define XSDK_SHUTTER_1_160000               6           //  1/160000"
#define XSDK_SHUTTER_1_128000               7           //  1/128000"
#define XSDK_SHUTTER_1_102400               9           //  1/102400"
#define XSDK_SHUTTER_1_80000                12          //  1/80000"
#define XSDK_SHUTTER_1_65000                15          //  1/65000"
#define XSDK_SHUTTER_1_64000                15          //  1/64000"
#define XSDK_SHUTTER_1_60000                15          //  1/64000"
#define XSDK_SHUTTER_1_51200                19          //  1/51200"
#define XSDK_SHUTTER_1_50000                19          //  1/53000"
#define XSDK_SHUTTER_1_40000                24          //  1/40000"
#define XSDK_SHUTTER_1_32000                30          //  1/32000"
#define XSDK_SHUTTER_1_25600                38          //  1/25600"
#define XSDK_SHUTTER_1_25000                38          //  1/25000"
#define XSDK_SHUTTER_1_24000                43          //  1/24000"
#define XSDK_SHUTTER_1_20000                48          //  1/20000"
#define XSDK_SHUTTER_1_16000                61          //  1/16000"
#define XSDK_SHUTTER_1_13000                76          //  1/13000"
#define XSDK_SHUTTER_1_12800                76          //  1/12800"
#define XSDK_SHUTTER_1_12000                86          //  1/12000"
#define XSDK_SHUTTER_1_10000                96          //  1/10000"
#define XSDK_SHUTTER_1_8000                 122         //  1/8000"
#define XSDK_SHUTTER_1_6400                 153         //  1/6400"
#define XSDK_SHUTTER_1_6000                 172         //  1/6000"
#define XSDK_SHUTTER_1_5000                 193         //  1/5000"
#define XSDK_SHUTTER_1_4000                 244         //  1/4000"
#define XSDK_SHUTTER_1_3200                 307         //  1/3200"
#define XSDK_SHUTTER_1_3000                 345         //  1/3000"
#define XSDK_SHUTTER_1_2500                 387         //  1/2500"
#define XSDK_SHUTTER_1_2000                 488         //  1/2000"
#define XSDK_SHUTTER_1_1600                 615         //  1/1600"
#define XSDK_SHUTTER_1_1500                 690         //  1/1500"
#define XSDK_SHUTTER_1_1250                 775         //  1/1250"
#define XSDK_SHUTTER_1_1000                 976         //  1/1000"
#define XSDK_SHUTTER_1_800                  1230        //  1/800"
#define XSDK_SHUTTER_1_750                  1381        //  1/750"
#define XSDK_SHUTTER_1_640                  1550        //  1/640"
#define XSDK_SHUTTER_1_500                  1953        //  1/500"
#define XSDK_SHUTTER_1_400                  2460        //  1/400"
#define XSDK_SHUTTER_1_350                  2762        //  1/350"
#define XSDK_SHUTTER_1_320                  3100        //  1/320"
#define XSDK_SHUTTER_1_250                  3906        //  1/250"
#define XSDK_SHUTTER_1_200                  4921        //  1/200"
#define XSDK_SHUTTER_1_180                  5524        //  1/180"
#define XSDK_SHUTTER_1_160                  6200        //  1/160"
#define XSDK_SHUTTER_1_125                  7812        //  1/125"
#define XSDK_SHUTTER_1_100                  9843        //  1/100"
#define XSDK_SHUTTER_1_90                   11048       //  1/90"
#define XSDK_SHUTTER_1_80                   12401       //  1/80"
#define XSDK_SHUTTER_1_60                   15625       //  1/60"
#define XSDK_SHUTTER_1_50                   19686       //  1/50"
#define XSDK_SHUTTER_1_45                   22097       //  1/45"
#define XSDK_SHUTTER_1_40                   24803       //  1/40"
#define XSDK_SHUTTER_1_30                   31250       //  1/30"
#define XSDK_SHUTTER_1_25                   39372       //  1/25"
#define XSDK_SHUTTER_1_20H                  44194       //  1/20"
#define XSDK_SHUTTER_1_20                   49606       //  1/20"
#define XSDK_SHUTTER_1_15                   62500       //  1/15"
#define XSDK_SHUTTER_1_13                   78745       //  1/13"
#define XSDK_SHUTTER_1_10H                  88388       //  1/10"
#define XSDK_SHUTTER_1_10                   99212       //  1/10"
#define XSDK_SHUTTER_1_8                    125000      //  1/8"
#define XSDK_SHUTTER_1_6                    157490      //  1/6"
#define XSDK_SHUTTER_1_6H                   176776      //  1/6"
#define XSDK_SHUTTER_1_5                    198425      //  1/5"
#define XSDK_SHUTTER_1_4                    250000      //  1/4"
#define XSDK_SHUTTER_1_3                    314980      //  1/3"
#define XSDK_SHUTTER_1_3H                   353553      //  1/3"
#define XSDK_SHUTTER_1_2P5                  396850      //  1/2.5"
#define XSDK_SHUTTER_1_2                    500000      //  1/2"
#define XSDK_SHUTTER_1_1P6                  629960      //  1/1.6"
#define XSDK_SHUTTER_1_1P5                  707106      //  1/1.5"
#define XSDK_SHUTTER_1_1P3                  793700      //  1/1.3"
#define XSDK_SHUTTER_1                      1000000     //  1"
#define XSDK_SHUTTER_1P3                    1259921     //  1.3"
#define XSDK_SHUTTER_1P5                    1414213     //  1.5" (X-T1 only)
#define XSDK_SHUTTER_1P5A                   1587401     //  1.5"
#define XSDK_SHUTTER_1P6                    1587401     //  1.6"
#define XSDK_SHUTTER_2                      2000000     //  2"
#define XSDK_SHUTTER_2P5                    2519842     //  2.5"
#define XSDK_SHUTTER_3H                     2828427     //  3"
#define XSDK_SHUTTER_3                      3174802     //  3"
#define XSDK_SHUTTER_4                      4000000     //  4"
#define XSDK_SHUTTER_5                      5039684     //  5"
#define XSDK_SHUTTER_6H                     5656854     //  6"
#define XSDK_SHUTTER_6                      6349604     //  6"
#define XSDK_SHUTTER_6P5                    6349604     //  6"
#define XSDK_SHUTTER_8                      8000000     //  8"
#define XSDK_SHUTTER_10                     10079368    //  10"
#define XSDK_SHUTTER_10H                    11313708    //  10"
#define XSDK_SHUTTER_13                     12699208    //  13"
#define XSDK_SHUTTER_15                     16000000    //  15"
#define XSDK_SHUTTER_20                     20158736    //  20"
#define XSDK_SHUTTER_20H                    22627416    //  20"
#define XSDK_SHUTTER_25                     25398416    //  25"
#define XSDK_SHUTTER_30                     32000000    //  30"
#define XSDK_SHUTTER_35S                    35000000    //  35"
#define XSDK_SHUTTER_40S                    40000000    //  40"
#define XSDK_SHUTTER_40                     40317473    //  40"
#define XSDK_SHUTTER_45S                    45000000    //  45"
#define XSDK_SHUTTER_50S                    50000000    //  50"
#define XSDK_SHUTTER_50                     50796833    //  52"
#define XSDK_SHUTTER_55S                    55000000    //  55"
#define XSDK_SHUTTER_60S                    60000000    //  60"
#define XSDK_SHUTTER_60                     64000000    //  60"
#define XSDK_SHUTTER_80                     80634947    //  80"
#define XSDK_SHUTTER_100                    101593667   //  110"
#define XSDK_SHUTTER_2M                     64000030    //  120"
#define XSDK_SHUTTER_125                    128000000   //  140"
#define XSDK_SHUTTER_160                    161269894   //  170"
#define XSDK_SHUTTER_200                    203187334   //  210"
#define XSDK_SHUTTER_4M                     64000060    //  240"
#define XSDK_SHUTTER_250                    256000000   //  250"
#define XSDK_SHUTTER_320                    322539788   //  320"
#define XSDK_SHUTTER_400                    406374669   //  420"
#define XSDK_SHUTTER_8M                     64000090    //  480"
#define XSDK_SHUTTER_500                    512000000   //  500"
#define XSDK_SHUTTER_640                    645079577   //  640"
#define XSDK_SHUTTER_800                    812749338   //  850"
#define XSDK_SHUTTER_15M                    64000120    //  900"
#define XSDK_SHUTTER_1000                   1024000000  //  1000"
#define XSDK_SHUTTER_1300                   1290159155  //  1300"
#define XSDK_SHUTTER_1600                   1625498677  //  1700"
#define XSDK_SHUTTER_30M                    64000150    //  1800"
#define XSDK_SHUTTER_2000                   2048000000  //  2000"
#define XSDK_SHUTTER_60M                    64000180    //  3600"
#define XSDK_SHUTTER_BULB                   -1          //  BULB

// ShutterSpeed (High Frequency Flickerless)
#define XSDK_SHUTTER_1_8192P0               1000819200
#define XSDK_SHUTTER_1_6888P6               1000688860
#define XSDK_SHUTTER_1_5792P6               1000579260
#define XSDK_SHUTTER_1_4871P0               1000487100
#define XSDK_SHUTTER_1_4096P0               1000409600
#define XSDK_SHUTTER_1_3756P0               1000375600
#define XSDK_SHUTTER_1_3444P3               1000344430
#define XSDK_SHUTTER_1_3158P4               1000315840
#define XSDK_SHUTTER_1_2896P3               1000289630
#define XSDK_SHUTTER_1_2655P9               1000265590
#define XSDK_SHUTTER_1_2435P5               1000243550
#define XSDK_SHUTTER_1_2233P4               1000223340
#define XSDK_SHUTTER_1_2048P0               1000204800
#define XSDK_SHUTTER_1_1961P2               1000196120
#define XSDK_SHUTTER_1_1878P0               1000187800
#define XSDK_SHUTTER_1_1798P4               1000179840
#define XSDK_SHUTTER_1_1722P2               1000172220
#define XSDK_SHUTTER_1_1649P1               1000164910
#define XSDK_SHUTTER_1_1579P2               1000157920
#define XSDK_SHUTTER_1_1512P3               1000151230
#define XSDK_SHUTTER_1_1448P2               1000144820
#define XSDK_SHUTTER_1_1386P8               1000138680
#define XSDK_SHUTTER_1_1328P0               1000132800
#define XSDK_SHUTTER_1_1271P7               1000127170
#define XSDK_SHUTTER_1_1217P7               1000121770
#define XSDK_SHUTTER_1_1166P1               1000116610
#define XSDK_SHUTTER_1_1116P7               1000111670
#define XSDK_SHUTTER_1_1069P3               1000106930
#define XSDK_SHUTTER_1_1024P0               1000102400
#define XSDK_SHUTTER_1_1002P1               1000100210
#define XSDK_SHUTTER_1_980P6                1000098060
#define XSDK_SHUTTER_1_959P6                1000095960
#define XSDK_SHUTTER_1_939P0                1000093900
#define XSDK_SHUTTER_1_918P9                1000091890
#define XSDK_SHUTTER_1_899P2                1000089920
#define XSDK_SHUTTER_1_879P9                1000087990
#define XSDK_SHUTTER_1_861P1                1000086110
#define XSDK_SHUTTER_1_842P6                1000084260
#define XSDK_SHUTTER_1_824P6                1000082460
#define XSDK_SHUTTER_1_806P9                1000080690
#define XSDK_SHUTTER_1_789P6                1000078960
#define XSDK_SHUTTER_1_772P7                1000077270
#define XSDK_SHUTTER_1_756P1                1000075610
#define XSDK_SHUTTER_1_739P9                1000073990
#define XSDK_SHUTTER_1_724P1                1000072410
#define XSDK_SHUTTER_1_708P6                1000070860
#define XSDK_SHUTTER_1_693P4                1000069340
#define XSDK_SHUTTER_1_678P5                1000067850
#define XSDK_SHUTTER_1_664P0                1000066400
#define XSDK_SHUTTER_1_649P8                1000064980
#define XSDK_SHUTTER_1_635P8                1000063580
#define XSDK_SHUTTER_1_622P2                1000062220
#define XSDK_SHUTTER_1_608P9                1000060890
#define XSDK_SHUTTER_1_595P8                1000059580
#define XSDK_SHUTTER_1_583P1                1000058310
#define XSDK_SHUTTER_1_570P6                1000057060
#define XSDK_SHUTTER_1_558P3                1000055830
#define XSDK_SHUTTER_1_546P4                1000054640
#define XSDK_SHUTTER_1_534P7                1000053470
#define XSDK_SHUTTER_1_523P2                1000052320
#define XSDK_SHUTTER_1_512P0                1000051200
#define XSDK_SHUTTER_1_506P5                1000050650
#define XSDK_SHUTTER_1_501P0                1000050100
#define XSDK_SHUTTER_1_495P6                1000049560
#define XSDK_SHUTTER_1_490P3                1000049030
#define XSDK_SHUTTER_1_485P0                1000048500
#define XSDK_SHUTTER_1_479P8                1000047980
#define XSDK_SHUTTER_1_474P6                1000047460
#define XSDK_SHUTTER_1_469P5                1000046950
#define XSDK_SHUTTER_1_464P4                1000046440
#define XSDK_SHUTTER_1_459P4                1000045940
#define XSDK_SHUTTER_1_454P5                1000045450
#define XSDK_SHUTTER_1_449P6                1000044960
#define XSDK_SHUTTER_1_444P8                1000044480
#define XSDK_SHUTTER_1_440P0                1000044000
#define XSDK_SHUTTER_1_435P2                1000043520
#define XSDK_SHUTTER_1_430P5                1000043050
#define XSDK_SHUTTER_1_425P9                1000042590
#define XSDK_SHUTTER_1_421P3                1000042130
#define XSDK_SHUTTER_1_416P8                1000041680
#define XSDK_SHUTTER_1_412P3                1000041230
#define XSDK_SHUTTER_1_407P8                1000040780
#define XSDK_SHUTTER_1_403P5                1000040350
#define XSDK_SHUTTER_1_399P1                1000039910
#define XSDK_SHUTTER_1_394P8                1000039480
#define XSDK_SHUTTER_1_390P6                1000039060
#define XSDK_SHUTTER_1_386P3                1000038630
#define XSDK_SHUTTER_1_382P2                1000038220
#define XSDK_SHUTTER_1_378P1                1000037810
#define XSDK_SHUTTER_1_374P0                1000037400
#define XSDK_SHUTTER_1_370P0                1000037000
#define XSDK_SHUTTER_1_366P0                1000036600
#define XSDK_SHUTTER_1_362P0                1000036200
#define XSDK_SHUTTER_1_358P1                1000035810
#define XSDK_SHUTTER_1_354P3                1000035430
#define XSDK_SHUTTER_1_350P5                1000035050
#define XSDK_SHUTTER_1_346P7                1000034670
#define XSDK_SHUTTER_1_343P0                1000034300
#define XSDK_SHUTTER_1_339P3                1000033930
#define XSDK_SHUTTER_1_335P6                1000033560
#define XSDK_SHUTTER_1_332P0                1000033200
#define XSDK_SHUTTER_1_328P4                1000032840
#define XSDK_SHUTTER_1_324P9                1000032490
#define XSDK_SHUTTER_1_321P4                1000032140
#define XSDK_SHUTTER_1_317P9                1000031790
#define XSDK_SHUTTER_1_314P5                1000031450
#define XSDK_SHUTTER_1_311P1                1000031110
#define XSDK_SHUTTER_1_307P8                1000030780
#define XSDK_SHUTTER_1_304P4                1000030440
#define XSDK_SHUTTER_1_301P2                1000030120
#define XSDK_SHUTTER_1_297P9                1000029790
#define XSDK_SHUTTER_1_294P7                1000029470
#define XSDK_SHUTTER_1_291P5                1000029150
#define XSDK_SHUTTER_1_288P4                1000028840
#define XSDK_SHUTTER_1_285P3                1000028530
#define XSDK_SHUTTER_1_282P2                1000028220
#define XSDK_SHUTTER_1_279P2                1000027920
#define XSDK_SHUTTER_1_276P2                1000027620
#define XSDK_SHUTTER_1_273P2                1000027320
#define XSDK_SHUTTER_1_270P2                1000027020
#define XSDK_SHUTTER_1_267P3                1000026730
#define XSDK_SHUTTER_1_264P5                1000026450
#define XSDK_SHUTTER_1_261P6                1000026160
#define XSDK_SHUTTER_1_258P8                1000025880
#define XSDK_SHUTTER_1_256P0                1000025600
#define XSDK_SHUTTER_1_254P6                1000025460
#define XSDK_SHUTTER_1_253P2                1000025320
#define XSDK_SHUTTER_1_251P9                1000025190
#define XSDK_SHUTTER_1_250P5                1000025050
#define XSDK_SHUTTER_1_249P2                1000024920
#define XSDK_SHUTTER_1_247P8                1000024780
#define XSDK_SHUTTER_1_246P5                1000024650
#define XSDK_SHUTTER_1_245P1                1000024510
#define XSDK_SHUTTER_1_243P8                1000024380
#define XSDK_SHUTTER_1_242P5                1000024250
#define XSDK_SHUTTER_1_241P2                1000024120
#define XSDK_SHUTTER_1_239P9                1000023990
#define XSDK_SHUTTER_1_238P6                1000023860
#define XSDK_SHUTTER_1_237P3                1000023730
#define XSDK_SHUTTER_1_236P0                1000023600
#define XSDK_SHUTTER_1_234P8                1000023480
#define XSDK_SHUTTER_1_233P5                1000023350
#define XSDK_SHUTTER_1_232P2                1000023220
#define XSDK_SHUTTER_1_231P0                1000023100
#define XSDK_SHUTTER_1_229P7                1000022970
#define XSDK_SHUTTER_1_228P5                1000022850
#define XSDK_SHUTTER_1_227P2                1000022720
#define XSDK_SHUTTER_1_226P0                1000022600
#define XSDK_SHUTTER_1_224P8                1000022480
#define XSDK_SHUTTER_1_223P6                1000022360
#define XSDK_SHUTTER_1_222P4                1000022240
#define XSDK_SHUTTER_1_221P2                1000022120
#define XSDK_SHUTTER_1_220P0                1000022000
#define XSDK_SHUTTER_1_218P8                1000021880
#define XSDK_SHUTTER_1_217P6                1000021760
#define XSDK_SHUTTER_1_216P4                1000021640
#define XSDK_SHUTTER_1_215P3                1000021530
#define XSDK_SHUTTER_1_214P1                1000021410
#define XSDK_SHUTTER_1_213P0                1000021300
#define XSDK_SHUTTER_1_211P8                1000021180
#define XSDK_SHUTTER_1_210P7                1000021070
#define XSDK_SHUTTER_1_209P5                1000020950
#define XSDK_SHUTTER_1_208P4                1000020840
#define XSDK_SHUTTER_1_207P3                1000020730
#define XSDK_SHUTTER_1_206P1                1000020610
#define XSDK_SHUTTER_1_205P0                1000020500
#define XSDK_SHUTTER_1_203P9                1000020390
#define XSDK_SHUTTER_1_202P8                1000020280
#define XSDK_SHUTTER_1_201P7                1000020170
#define XSDK_SHUTTER_1_200P6                1000020060
#define XSDK_SHUTTER_1_199P6                1000019960
#define XSDK_SHUTTER_1_198P5                1000019850
#define XSDK_SHUTTER_1_197P4                1000019740
#define XSDK_SHUTTER_1_196P3                1000019630
#define XSDK_SHUTTER_1_195P3                1000019530
#define XSDK_SHUTTER_1_194P2                1000019420
#define XSDK_SHUTTER_1_193P2                1000019320
#define XSDK_SHUTTER_1_192P1                1000019210
#define XSDK_SHUTTER_1_191P1                1000019110
#define XSDK_SHUTTER_1_190P1                1000019010
#define XSDK_SHUTTER_1_189P0                1000018900
#define XSDK_SHUTTER_1_188P0                1000018800
#define XSDK_SHUTTER_1_187P0                1000018700
#define XSDK_SHUTTER_1_186P0                1000018600
#define XSDK_SHUTTER_1_185P0                1000018500
#define XSDK_SHUTTER_1_184P0                1000018400
#define XSDK_SHUTTER_1_183P0                1000018300
#define XSDK_SHUTTER_1_182P0                1000018200
#define XSDK_SHUTTER_1_181P0                1000018100
#define XSDK_SHUTTER_1_180P0                1000018000
#define XSDK_SHUTTER_1_179P1                1000017910
#define XSDK_SHUTTER_1_178P1                1000017810
#define XSDK_SHUTTER_1_177P1                1000017710
#define XSDK_SHUTTER_1_176P2                1000017620
#define XSDK_SHUTTER_1_175P2                1000017520
#define XSDK_SHUTTER_1_174P3                1000017430
#define XSDK_SHUTTER_1_173P3                1000017330
#define XSDK_SHUTTER_1_172P4                1000017240
#define XSDK_SHUTTER_1_171P5                1000017150
#define XSDK_SHUTTER_1_170P6                1000017060
#define XSDK_SHUTTER_1_169P6                1000016960
#define XSDK_SHUTTER_1_168P7                1000016870
#define XSDK_SHUTTER_1_167P8                1000016780
#define XSDK_SHUTTER_1_166P9                1000016690
#define XSDK_SHUTTER_1_166P0                1000016600
#define XSDK_SHUTTER_1_165P1                1000016510
#define XSDK_SHUTTER_1_164P2                1000016420
#define XSDK_SHUTTER_1_163P3                1000016330
#define XSDK_SHUTTER_1_162P4                1000016240
#define XSDK_SHUTTER_1_161P6                1000016160
#define XSDK_SHUTTER_1_160P7                1000016070
#define XSDK_SHUTTER_1_159P8                1000015980
#define XSDK_SHUTTER_1_159P0                1000015900
#define XSDK_SHUTTER_1_158P1                1000015810
#define XSDK_SHUTTER_1_157P2                1000015720
#define XSDK_SHUTTER_1_156P4                1000015640
#define XSDK_SHUTTER_1_155P6                1000015560
#define XSDK_SHUTTER_1_154P7                1000015470
#define XSDK_SHUTTER_1_153P9                1000015390
#define XSDK_SHUTTER_1_153P0                1000015300
#define XSDK_SHUTTER_1_152P2                1000015220
#define XSDK_SHUTTER_1_151P4                1000015140
#define XSDK_SHUTTER_1_150P6                1000015060
#define XSDK_SHUTTER_1_149P8                1000014980
#define XSDK_SHUTTER_1_149P0                1000014900
#define XSDK_SHUTTER_1_148P2                1000014820
#define XSDK_SHUTTER_1_147P4                1000014740
#define XSDK_SHUTTER_1_146P6                1000014660
#define XSDK_SHUTTER_1_145P8                1000014580
#define XSDK_SHUTTER_1_145P0                1000014500
#define XSDK_SHUTTER_1_144P2                1000014420
#define XSDK_SHUTTER_1_143P4                1000014340
#define XSDK_SHUTTER_1_142P6                1000014260
#define XSDK_SHUTTER_1_141P9                1000014190
#define XSDK_SHUTTER_1_141P1                1000014110
#define XSDK_SHUTTER_1_140P3                1000014030
#define XSDK_SHUTTER_1_139P6                1000013960
#define XSDK_SHUTTER_1_138P8                1000013880
#define XSDK_SHUTTER_1_138P1                1000013810
#define XSDK_SHUTTER_1_137P3                1000013730
#define XSDK_SHUTTER_1_136P6                1000013660
#define XSDK_SHUTTER_1_135P9                1000013590
#define XSDK_SHUTTER_1_135P1                1000013510
#define XSDK_SHUTTER_1_134P4                1000013440
#define XSDK_SHUTTER_1_133P7                1000013370
#define XSDK_SHUTTER_1_132P9                1000013290
#define XSDK_SHUTTER_1_132P2                1000013220
#define XSDK_SHUTTER_1_131P5                1000013150
#define XSDK_SHUTTER_1_130P8                1000013080
#define XSDK_SHUTTER_1_130P1                1000013010
#define XSDK_SHUTTER_1_129P4                1000012940
#define XSDK_SHUTTER_1_128P7                1000012870
#define XSDK_SHUTTER_1_128P0                1000012800
#define XSDK_SHUTTER_1_127P7                1000012770
#define XSDK_SHUTTER_1_127P3                1000012730
#define XSDK_SHUTTER_1_127P0                1000012700
#define XSDK_SHUTTER_1_126P6                1000012660
#define XSDK_SHUTTER_1_126P3                1000012630
#define XSDK_SHUTTER_1_125P9                1000012590
#define XSDK_SHUTTER_1_125P6                1000012560
#define XSDK_SHUTTER_1_125P3                1000012530
#define XSDK_SHUTTER_1_124P9                1000012490
#define XSDK_SHUTTER_1_124P6                1000012460
#define XSDK_SHUTTER_1_124P2                1000012420
#define XSDK_SHUTTER_1_123P9                1000012390
#define XSDK_SHUTTER_1_123P6                1000012360
#define XSDK_SHUTTER_1_123P2                1000012320
#define XSDK_SHUTTER_1_122P9                1000012290
#define XSDK_SHUTTER_1_122P6                1000012260
#define XSDK_SHUTTER_1_122P2                1000012220
#define XSDK_SHUTTER_1_121P9                1000012190
#define XSDK_SHUTTER_1_121P6                1000012160
#define XSDK_SHUTTER_1_121P3                1000012130
#define XSDK_SHUTTER_1_120P9                1000012090
#define XSDK_SHUTTER_1_120P6                1000012060
#define XSDK_SHUTTER_1_120P3                1000012030
#define XSDK_SHUTTER_1_119P9                1000011990
#define XSDK_SHUTTER_1_119P6                1000011960
#define XSDK_SHUTTER_1_119P3                1000011930
#define XSDK_SHUTTER_1_119P0                1000011900
#define XSDK_SHUTTER_1_118P7                1000011870
#define XSDK_SHUTTER_1_118P3                1000011830
#define XSDK_SHUTTER_1_118P0                1000011800
#define XSDK_SHUTTER_1_117P7                1000011770
#define XSDK_SHUTTER_1_117P4                1000011740
#define XSDK_SHUTTER_1_117P1                1000011710
#define XSDK_SHUTTER_1_116P7                1000011670
#define XSDK_SHUTTER_1_116P4                1000011640
#define XSDK_SHUTTER_1_116P1                1000011610
#define XSDK_SHUTTER_1_115P8                1000011580
#define XSDK_SHUTTER_1_115P5                1000011550
#define XSDK_SHUTTER_1_115P2                1000011520
#define XSDK_SHUTTER_1_114P9                1000011490
#define XSDK_SHUTTER_1_114P6                1000011460
#define XSDK_SHUTTER_1_114P2                1000011420
#define XSDK_SHUTTER_1_113P9                1000011390
#define XSDK_SHUTTER_1_113P6                1000011360
#define XSDK_SHUTTER_1_113P3                1000011330
#define XSDK_SHUTTER_1_113P0                1000011300
#define XSDK_SHUTTER_1_112P7                1000011270
#define XSDK_SHUTTER_1_112P4                1000011240
#define XSDK_SHUTTER_1_112P1                1000011210
#define XSDK_SHUTTER_1_111P8                1000011180
#define XSDK_SHUTTER_1_111P5                1000011150
#define XSDK_SHUTTER_1_111P2                1000011120
#define XSDK_SHUTTER_1_110P9                1000011090
#define XSDK_SHUTTER_1_110P6                1000011060
#define XSDK_SHUTTER_1_110P3                1000011030
#define XSDK_SHUTTER_1_110P0                1000011000
#define XSDK_SHUTTER_1_109P7                1000010970
#define XSDK_SHUTTER_1_109P4                1000010940
#define XSDK_SHUTTER_1_109P1                1000010910
#define XSDK_SHUTTER_1_108P8                1000010880
#define XSDK_SHUTTER_1_108P5                1000010850
#define XSDK_SHUTTER_1_108P2                1000010820
#define XSDK_SHUTTER_1_107P9                1000010790
#define XSDK_SHUTTER_1_107P6                1000010760
#define XSDK_SHUTTER_1_107P3                1000010730
#define XSDK_SHUTTER_1_107P1                1000010710
#define XSDK_SHUTTER_1_106P8                1000010680
#define XSDK_SHUTTER_1_106P5                1000010650
#define XSDK_SHUTTER_1_106P2                1000010620
#define XSDK_SHUTTER_1_105P9                1000010590
#define XSDK_SHUTTER_1_105P6                1000010560
#define XSDK_SHUTTER_1_105P3                1000010530
#define XSDK_SHUTTER_1_105P0                1000010500
#define XSDK_SHUTTER_1_104P8                1000010480
#define XSDK_SHUTTER_1_104P5                1000010450
#define XSDK_SHUTTER_1_104P2                1000010420
#define XSDK_SHUTTER_1_103P9                1000010390
#define XSDK_SHUTTER_1_103P6                1000010360
#define XSDK_SHUTTER_1_103P4                1000010340
#define XSDK_SHUTTER_1_103P1                1000010310
#define XSDK_SHUTTER_1_102P8                1000010280
#define XSDK_SHUTTER_1_102P5                1000010250
#define XSDK_SHUTTER_1_102P2                1000010220
#define XSDK_SHUTTER_1_102P0                1000010200
#define XSDK_SHUTTER_1_101P7                1000010170
#define XSDK_SHUTTER_1_101P4                1000010140
#define XSDK_SHUTTER_1_101P1                1000010110
#define XSDK_SHUTTER_1_100P9                1000010090
#define XSDK_SHUTTER_1_100P6                1000010060
#define XSDK_SHUTTER_1_100P3                1000010030
#define XSDK_SHUTTER_1_100P0                1000010000
#define XSDK_SHUTTER_1_99P8                 1000009980
#define XSDK_SHUTTER_1_99P5                 1000009950
#define XSDK_SHUTTER_1_99P2                 1000009920
#define XSDK_SHUTTER_1_99P0                 1000009900
#define XSDK_SHUTTER_1_98P7                 1000009870
#define XSDK_SHUTTER_1_98P4                 1000009840
#define XSDK_SHUTTER_1_98P2                 1000009820
#define XSDK_SHUTTER_1_97P9                 1000009790
#define XSDK_SHUTTER_1_97P6                 1000009760
#define XSDK_SHUTTER_1_97P4                 1000009740
#define XSDK_SHUTTER_1_97P1                 1000009710
#define XSDK_SHUTTER_1_96P8                 1000009680
#define XSDK_SHUTTER_1_96P6                 1000009660
#define XSDK_SHUTTER_1_96P3                 1000009630
#define XSDK_SHUTTER_1_96P1                 1000009610
#define XSDK_SHUTTER_1_95P8                 1000009580
#define XSDK_SHUTTER_1_95P5                 1000009550
#define XSDK_SHUTTER_1_95P3                 1000009530
#define XSDK_SHUTTER_1_95P0                 1000009500
#define XSDK_SHUTTER_1_94P8                 1000009480
#define XSDK_SHUTTER_1_94P5                 1000009450
#define XSDK_SHUTTER_1_94P3                 1000009430
#define XSDK_SHUTTER_1_94P0                 1000009400
#define XSDK_SHUTTER_1_93P8                 1000009380
#define XSDK_SHUTTER_1_93P5                 1000009350
#define XSDK_SHUTTER_1_93P2                 1000009320
#define XSDK_SHUTTER_1_93P0                 1000009300
#define XSDK_SHUTTER_1_92P7                 1000009270
#define XSDK_SHUTTER_1_92P5                 1000009250
#define XSDK_SHUTTER_1_92P2                 1000009220
#define XSDK_SHUTTER_1_92P0                 1000009200
#define XSDK_SHUTTER_1_91P7                 1000009170
#define XSDK_SHUTTER_1_91P5                 1000009150
#define XSDK_SHUTTER_1_91P2                 1000009120
#define XSDK_SHUTTER_1_91P0                 1000009100
#define XSDK_SHUTTER_1_90P8                 1000009080
#define XSDK_SHUTTER_1_90P5                 1000009050
#define XSDK_SHUTTER_1_90P3                 1000009030
#define XSDK_SHUTTER_1_90P0                 1000009000
#define XSDK_SHUTTER_1_89P8                 1000008980
#define XSDK_SHUTTER_1_89P5                 1000008950
#define XSDK_SHUTTER_1_89P3                 1000008930
#define XSDK_SHUTTER_1_89P1                 1000008910
#define XSDK_SHUTTER_1_88P8                 1000008880
#define XSDK_SHUTTER_1_88P6                 1000008860
#define XSDK_SHUTTER_1_88P3                 1000008830
#define XSDK_SHUTTER_1_88P1                 1000008810
#define XSDK_SHUTTER_1_87P9                 1000008790
#define XSDK_SHUTTER_1_87P6                 1000008760
#define XSDK_SHUTTER_1_87P4                 1000008740
#define XSDK_SHUTTER_1_87P1                 1000008710
#define XSDK_SHUTTER_1_86P9                 1000008690
#define XSDK_SHUTTER_1_86P7                 1000008670
#define XSDK_SHUTTER_1_86P4                 1000008640
#define XSDK_SHUTTER_1_86P2                 1000008620
#define XSDK_SHUTTER_1_86P0                 1000008600
#define XSDK_SHUTTER_1_85P7                 1000008570
#define XSDK_SHUTTER_1_85P5                 1000008550
#define XSDK_SHUTTER_1_85P3                 1000008530
#define XSDK_SHUTTER_1_85P0                 1000008500
#define XSDK_SHUTTER_1_84P8                 1000008480
#define XSDK_SHUTTER_1_84P6                 1000008460
#define XSDK_SHUTTER_1_84P4                 1000008440
#define XSDK_SHUTTER_1_84P1                 1000008410
#define XSDK_SHUTTER_1_83P9                 1000008390
#define XSDK_SHUTTER_1_83P7                 1000008370
#define XSDK_SHUTTER_1_83P4                 1000008340
#define XSDK_SHUTTER_1_83P2                 1000008320
#define XSDK_SHUTTER_1_83P0                 1000008300
#define XSDK_SHUTTER_1_82P8                 1000008280
#define XSDK_SHUTTER_1_82P5                 1000008250
#define XSDK_SHUTTER_1_82P3                 1000008230
#define XSDK_SHUTTER_1_82P1                 1000008210
#define XSDK_SHUTTER_1_81P9                 1000008190
#define XSDK_SHUTTER_1_81P7                 1000008170
#define XSDK_SHUTTER_1_81P4                 1000008140
#define XSDK_SHUTTER_1_81P2                 1000008120
#define XSDK_SHUTTER_1_81P0                 1000008100
#define XSDK_SHUTTER_1_80P8                 1000008080
#define XSDK_SHUTTER_1_80P6                 1000008060
#define XSDK_SHUTTER_1_80P3                 1000008030
#define XSDK_SHUTTER_1_80P1                 1000008010
#define XSDK_SHUTTER_1_79P9                 1000007990
#define XSDK_SHUTTER_1_79P7                 1000007970
#define XSDK_SHUTTER_1_79P5                 1000007950
#define XSDK_SHUTTER_1_79P3                 1000007930
#define XSDK_SHUTTER_1_79P0                 1000007900
#define XSDK_SHUTTER_1_78P8                 1000007880
#define XSDK_SHUTTER_1_78P6                 1000007860
#define XSDK_SHUTTER_1_78P4                 1000007840
#define XSDK_SHUTTER_1_78P2                 1000007820
#define XSDK_SHUTTER_1_78P0                 1000007800
#define XSDK_SHUTTER_1_77P8                 1000007780
#define XSDK_SHUTTER_1_77P6                 1000007760
#define XSDK_SHUTTER_1_77P4                 1000007740
#define XSDK_SHUTTER_1_77P1                 1000007710
#define XSDK_SHUTTER_1_76P9                 1000007690
#define XSDK_SHUTTER_1_76P7                 1000007670
#define XSDK_SHUTTER_1_76P5                 1000007650
#define XSDK_SHUTTER_1_76P3                 1000007630
#define XSDK_SHUTTER_1_76P1                 1000007610
#define XSDK_SHUTTER_1_75P9                 1000007590
#define XSDK_SHUTTER_1_75P7                 1000007570
#define XSDK_SHUTTER_1_75P5                 1000007550
#define XSDK_SHUTTER_1_75P3                 1000007530
#define XSDK_SHUTTER_1_75P1                 1000007510
#define XSDK_SHUTTER_1_74P9                 1000007490
#define XSDK_SHUTTER_1_74P7                 1000007470
#define XSDK_SHUTTER_1_74P5                 1000007450
#define XSDK_SHUTTER_1_74P3                 1000007430
#define XSDK_SHUTTER_1_74P1                 1000007410
#define XSDK_SHUTTER_1_73P9                 1000007390
#define XSDK_SHUTTER_1_73P7                 1000007370
#define XSDK_SHUTTER_1_73P5                 1000007350
#define XSDK_SHUTTER_1_73P3                 1000007330
#define XSDK_SHUTTER_1_73P1                 1000007310
#define XSDK_SHUTTER_1_72P9                 1000007290
#define XSDK_SHUTTER_1_72P7                 1000007270
#define XSDK_SHUTTER_1_72P5                 1000007250
#define XSDK_SHUTTER_1_72P3                 1000007230
#define XSDK_SHUTTER_1_72P1                 1000007210
#define XSDK_SHUTTER_1_71P9                 1000007190
#define XSDK_SHUTTER_1_71P7                 1000007170
#define XSDK_SHUTTER_1_71P5                 1000007150
#define XSDK_SHUTTER_1_71P3                 1000007130
#define XSDK_SHUTTER_1_71P1                 1000007110
#define XSDK_SHUTTER_1_70P9                 1000007090
#define XSDK_SHUTTER_1_70P7                 1000007070
#define XSDK_SHUTTER_1_70P6                 1000007060
#define XSDK_SHUTTER_1_70P4                 1000007040
#define XSDK_SHUTTER_1_70P2                 1000007020
#define XSDK_SHUTTER_1_70P0                 1000007000
#define XSDK_SHUTTER_1_69P8                 1000006980
#define XSDK_SHUTTER_1_69P6                 1000006960
#define XSDK_SHUTTER_1_69P4                 1000006940
#define XSDK_SHUTTER_1_69P2                 1000006920
#define XSDK_SHUTTER_1_69P0                 1000006900
#define XSDK_SHUTTER_1_68P9                 1000006890
#define XSDK_SHUTTER_1_68P7                 1000006870
#define XSDK_SHUTTER_1_68P5                 1000006850
#define XSDK_SHUTTER_1_68P3                 1000006830
#define XSDK_SHUTTER_1_68P1                 1000006810
#define XSDK_SHUTTER_1_67P9                 1000006790
#define XSDK_SHUTTER_1_67P7                 1000006770
#define XSDK_SHUTTER_1_67P6                 1000006760
#define XSDK_SHUTTER_1_67P4                 1000006740
#define XSDK_SHUTTER_1_67P2                 1000006720
#define XSDK_SHUTTER_1_67P0                 1000006700
#define XSDK_SHUTTER_1_66P8                 1000006680
#define XSDK_SHUTTER_1_66P7                 1000006670
#define XSDK_SHUTTER_1_66P5                 1000006650
#define XSDK_SHUTTER_1_66P3                 1000006630
#define XSDK_SHUTTER_1_66P1                 1000006610
#define XSDK_SHUTTER_1_65P9                 1000006590
#define XSDK_SHUTTER_1_65P8                 1000006580
#define XSDK_SHUTTER_1_65P6                 1000006560
#define XSDK_SHUTTER_1_65P4                 1000006540
#define XSDK_SHUTTER_1_65P2                 1000006520
#define XSDK_SHUTTER_1_65P0                 1000006500
#define XSDK_SHUTTER_1_64P9                 1000006490
#define XSDK_SHUTTER_1_64P7                 1000006470
#define XSDK_SHUTTER_1_64P5                 1000006450
#define XSDK_SHUTTER_1_64P3                 1000006430
#define XSDK_SHUTTER_1_64P2                 1000006420
#define XSDK_SHUTTER_1_64P0                 1000006400
#define XSDK_SHUTTER_1_63P8                 1000006380
#define XSDK_SHUTTER_1_63P7                 1000006370
#define XSDK_SHUTTER_1_63P5                 1000006350
#define XSDK_SHUTTER_1_63P3                 1000006330
#define XSDK_SHUTTER_1_63P1                 1000006310
#define XSDK_SHUTTER_1_63P0                 1000006300
#define XSDK_SHUTTER_1_62P8                 1000006280
#define XSDK_SHUTTER_1_62P6                 1000006260
#define XSDK_SHUTTER_1_62P5                 1000006250
#define XSDK_SHUTTER_1_62P3                 1000006230
#define XSDK_SHUTTER_1_62P1                 1000006210
#define XSDK_SHUTTER_1_62P0                 1000006200
#define XSDK_SHUTTER_1_61P8                 1000006180
#define XSDK_SHUTTER_1_61P6                 1000006160
#define XSDK_SHUTTER_1_61P5                 1000006150
#define XSDK_SHUTTER_1_61P3                 1000006130
#define XSDK_SHUTTER_1_61P1                 1000006110
#define XSDK_SHUTTER_1_61P0                 1000006100
#define XSDK_SHUTTER_1_60P8                 1000006080
#define XSDK_SHUTTER_1_60P6                 1000006060
#define XSDK_SHUTTER_1_60P5                 1000006050
#define XSDK_SHUTTER_1_60P3                 1000006030
#define XSDK_SHUTTER_1_60P1                 1000006010
#define XSDK_SHUTTER_1_60P0                 1000006000
#define XSDK_SHUTTER_1_59P8                 1000005980
#define XSDK_SHUTTER_1_59P6                 1000005960
#define XSDK_SHUTTER_1_59P5                 1000005950
#define XSDK_SHUTTER_1_59P3                 1000005930
#define XSDK_SHUTTER_1_59P2                 1000005920
#define XSDK_SHUTTER_1_59P0                 1000005900
#define XSDK_SHUTTER_1_58P8                 1000005880
#define XSDK_SHUTTER_1_58P7                 1000005870
#define XSDK_SHUTTER_1_58P5                 1000005850
#define XSDK_SHUTTER_1_58P4                 1000005840
#define XSDK_SHUTTER_1_58P2                 1000005820
#define XSDK_SHUTTER_1_58P1                 1000005810
#define XSDK_SHUTTER_1_57P9                 1000005790
#define XSDK_SHUTTER_1_57P7                 1000005770
#define XSDK_SHUTTER_1_57P6                 1000005760
#define XSDK_SHUTTER_1_57P4                 1000005740
#define XSDK_SHUTTER_1_57P3                 1000005730
#define XSDK_SHUTTER_1_57P1                 1000005710
#define XSDK_SHUTTER_1_57P0                 1000005700
#define XSDK_SHUTTER_1_56P8                 1000005680
#define XSDK_SHUTTER_1_56P7                 1000005670
#define XSDK_SHUTTER_1_56P5                 1000005650
#define XSDK_SHUTTER_1_56P4                 1000005640
#define XSDK_SHUTTER_1_56P2                 1000005620
#define XSDK_SHUTTER_1_56P0                 1000005600
#define XSDK_SHUTTER_1_55P9                 1000005590
#define XSDK_SHUTTER_1_55P7                 1000005570
#define XSDK_SHUTTER_1_55P6                 1000005560
#define XSDK_SHUTTER_1_55P4                 1000005540
#define XSDK_SHUTTER_1_55P3                 1000005530
#define XSDK_SHUTTER_1_55P1                 1000005510
#define XSDK_SHUTTER_1_55P0                 1000005500
#define XSDK_SHUTTER_1_54P8                 1000005480
#define XSDK_SHUTTER_1_54P7                 1000005470
#define XSDK_SHUTTER_1_54P6                 1000005460
#define XSDK_SHUTTER_1_54P4                 1000005440
#define XSDK_SHUTTER_1_54P3                 1000005430
#define XSDK_SHUTTER_1_54P1                 1000005410
#define XSDK_SHUTTER_1_54P0                 1000005400
#define XSDK_SHUTTER_1_53P8                 1000005380
#define XSDK_SHUTTER_1_53P7                 1000005370
#define XSDK_SHUTTER_1_53P5                 1000005350
#define XSDK_SHUTTER_1_53P4                 1000005340
#define XSDK_SHUTTER_1_53P2                 1000005320
#define XSDK_SHUTTER_1_53P1                 1000005310
#define XSDK_SHUTTER_1_53P0                 1000005300
#define XSDK_SHUTTER_1_52P8                 1000005280
#define XSDK_SHUTTER_1_52P7                 1000005270
#define XSDK_SHUTTER_1_52P5                 1000005250
#define XSDK_SHUTTER_1_52P4                 1000005240
#define XSDK_SHUTTER_1_52P2                 1000005220
#define XSDK_SHUTTER_1_52P1                 1000005210
#define XSDK_SHUTTER_1_52P0                 1000005200
#define XSDK_SHUTTER_1_51P8                 1000005180
#define XSDK_SHUTTER_1_51P7                 1000005170
#define XSDK_SHUTTER_1_51P5                 1000005150
#define XSDK_SHUTTER_1_51P4                 1000005140
#define XSDK_SHUTTER_1_51P3                 1000005130
#define XSDK_SHUTTER_1_51P1                 1000005110
#define XSDK_SHUTTER_1_51P0                 1000005100
#define XSDK_SHUTTER_1_50P8                 1000005080
#define XSDK_SHUTTER_1_50P7                 1000005070
#define XSDK_SHUTTER_1_50P6                 1000005060
#define XSDK_SHUTTER_1_50P4                 1000005040
#define XSDK_SHUTTER_1_50P3                 1000005030
#define XSDK_SHUTTER_1_50P2                 1000005020
#define XSDK_SHUTTER_1_50P0                 1000005000

// S1 Lock ShutterSpeed (1/10EV Step)
#define XSDK_SS_2000                        -2048000000 //  2000"
#define XSDK_SS_1900                        -1910851566 //  1900"
#define XSDK_SS_1800                        -1824560574 //  1800"
#define XSDK_SS_1700                        -1625498677 //  1700"
#define XSDK_SS_1600                        -1552093764 //  1600"
#define XSDK_SS_1500                        -1448154687 //  1500"
#define XSDK_SS_1400                        -1351176100 //  1400"
#define XSDK_SS_1300                        -1290159155 //  1300"
#define XSDK_SS_1200                        -1149401137 //  1200"
#define XSDK_SS_1100                        -1097496025 //  1100"
#define XSDK_SS_1000                        -1024000000 //  1000"
#define XSDK_SS_950                         -955425783  //  950"
#define XSDK_SS_900                         -912280287  //  900"
#define XSDK_SS_850                         -812749338  //  850"
#define XSDK_SS_800                         -776046882  //  800"
#define XSDK_SS_750                         -724077343  //  750"
#define XSDK_SS_680                         -675588050  //  680"
#define XSDK_SS_640                         -645079577  //  640"
#define XSDK_SS_600                         -574700568  //  600"
#define XSDK_SS_550                         -548748012  //  550"
#define XSDK_SS_500                         -512000000  //  500"
#define XSDK_SS_480                         -477712891  //  480"
#define XSDK_SS_450                         -456140143  //  450"
#define XSDK_SS_420                         -406374669  //  420"
#define XSDK_SS_400                         -388023441  //  400"
#define XSDK_SS_350                         -362038671  //  350"
#define XSDK_SS_340                         -337794025  //  340"
#define XSDK_SS_320                         -322539788  //  320"
#define XSDK_SS_300                         -287350284  //  300"
#define XSDK_SS_280                         -274374006  //  280"
#define XSDK_SS_250                         -256000000  //  250"
#define XSDK_SS_240                         -238856445  //  240"
#define XSDK_SS_220                         -228070071  //  220"
#define XSDK_SS_210                         -203187334  //  210"
#define XSDK_SS_200                         -194011720  //  200"
#define XSDK_SS_180                         -181019335  //  180"
#define XSDK_SS_170                         -161269894  //  170"
#define XSDK_SS_160                         -143675142  //  160"
#define XSDK_SS_150                         -137187003  //  150"
#define XSDK_SS_140                         -128000000  //  140"
#define XSDK_SS_125                         -119428222  //  125"
#define XSDK_SS_120                         -114035035  //  120"
#define XSDK_SS_110                         -101593667  //  110"
#define XSDK_SS_100                         -97005860   //  100"
#define XSDK_SS_90                          -90509667   //  90"
#define XSDK_SS_85                          -84448506   //  85"
#define XSDK_SS_80                          -80634947   //  80"
#define XSDK_SS_75                          -71837571   //  75"
#define XSDK_SS_70                          -68593501   //  70"
#define XSDK_SS_60                          -64000000   //  60"
#define XSDK_SS_58                          -59714111   //  58"
#define XSDK_SS_56                          -57017517   //  56"
#define XSDK_SS_52                          -50796833   //  52"
#define XSDK_SS_50                          -48502930   //  50"
#define XSDK_SS_45                          -45254833   //  45"
#define XSDK_SS_42                          -42224253   //  42"
#define XSDK_SS_40                          -40317473   //  40"
#define XSDK_SS_38                          -35918785   //  38"
#define XSDK_SS_34                          -34296750   //  34"
#define XSDK_SS_30                          -32000000   //  30Åh
#define XSDK_SS_28                          -29857055   //  28Åh
#define XSDK_SS_27                          -28508758   //  27Åh
#define XSDK_SS_26                          -25398416   //  26Åh
#define XSDK_SS_25                          -24251465   //  25Åh
#define XSDK_SS_20                          -22627416   //  20Åh
#define XSDK_SS_18                          -17959392   //  18Åh
#define XSDK_SS_17                          -17148375   //  17Åh
#define XSDK_SS_15                          -16000000   //  15Åh
#define XSDK_SS_14                          -14254379   //  14Åh
#define XSDK_SS_13                          -12699208   //  13Åh
#define XSDK_SS_12                          -12125732   //  12Åh
#define XSDK_SS_10                          -11313708   //  10Åh
#define XSDK_SS_9                           -8979696    //  9Åh
#define XSDK_SS_8P5                         -8574187    //  8.5Åh
#define XSDK_SS_8                           -8000000    //  8Åh
#define XSDK_SS_7P5                         -7464263    //  7.5Åh
#define XSDK_SS_7                           -7127189    //  7Åh
#define XSDK_SS_6P5                         -6349604    //  6.5Åh
#define XSDK_SS_6                           -6062866    //  6Åh
#define XSDK_SS_5P3                         -5278031    //  5.3Åh
#define XSDK_SS_5                           -5039684    //  5Åh
#define XSDK_SS_4P5                         -4489848    //  4.5Åh
#define XSDK_SS_4P3                         -4287093    //  4.3Åh
#define XSDK_SS_4                           -4000000    //  4Åh
#define XSDK_SS_3P7                         -3732131    //  3.7Åh
#define XSDK_SS_3P5                         -3563594    //  3.5Åh
#define XSDK_SS_3P2                         -3174802    //  3.2Åh
#define XSDK_SS_3P1                         -3031433    //  3.1Åh
#define XSDK_SS_3                           -2828427    //  3Åh
#define XSDK_SS_2P6                         -2639015    //  2.6Åh
#define XSDK_SS_2P5                         -2519842    //  2.5Åh
#define XSDK_SS_2P3                         -2244924    //  2.3Åh
#define XSDK_SS_2P1                         -2143546    //  2.1Åh
#define XSDK_SS_2                           -2000000    //  2Åh
#define XSDK_SS_1P9                         -1866065    //  1.9Åh
#define XSDK_SS_1P8                         -1781797    //  1.8Åh
#define XSDK_SS_1P7                         -1587401    //  1.7Åh
#define XSDK_SS_1P6                         -1515716    //  1.6Åh
#define XSDK_SS_1P5                         -1414213    //  1.5Åh
#define XSDK_SS_1P4                         -1319507    //  1.4Åh
#define XSDK_SS_1P3                         -1259921    //  1.3Åh
#define XSDK_SS_1P2                         -1122462    //  1.2Åh
#define XSDK_SS_1P1                         -1071773    //  1.1Åh
#define XSDK_SS_1                           -1000000    //  1Åh
#define XSDK_SS_1_1P1                       -933032     //  1/1.1"
#define XSDK_SS_1_1P2                       -890898     //  1/1.2"
#define XSDK_SS_1_1P3                       -793700     //  1/1.3"
#define XSDK_SS_1_1P4                       -757858     //  1/1.4"
#define XSDK_SS_1_1P5                       -707106     //  1/1.5"
#define XSDK_SS_1_1P6                       -659753     //  1/1.6"
#define XSDK_SS_1_1P7                       -629960     //  1/1.7"
#define XSDK_SS_1_1P8                       -561231     //  1/1.8"
#define XSDK_SS_1_1P9                       -535886     //  1/1.9"
#define XSDK_SS_1_2                         -500000     //  1/2"
#define XSDK_SS_1_2P1                       -466516     //  1/2.1"
#define XSDK_SS_1_2P3                       -445449     //  1/2.3"
#define XSDK_SS_1_2P5                       -396850     //  1/2.5"
#define XSDK_SS_1_2P6                       -378929     //  1/2.6"
#define XSDK_SS_1_2P8                       -353553     //  1/2.8"
#define XSDK_SS_1_3                         -329876     //  1/3"
#define XSDK_SS_1_3P1                       -314980     //  1/3.1"
#define XSDK_SS_1_3P2                       -280615     //  1/3.2"
#define XSDK_SS_1_3P5                       -267943     //  1/3.5"
#define XSDK_SS_1_4                         -250000     //  1/4"
#define XSDK_SS_1_4P3                       -233258     //  1/4.3"
#define XSDK_SS_1_4P5                       -222724     //  1/4.5"
#define XSDK_SS_1_5                         -198425     //  1/5"
#define XSDK_SS_1_5P3                       -189464     //  1/5.3"
#define XSDK_SS_1_6                         -176776     //  1/6"
#define XSDK_SS_1_6P5                       -157490     //  1/6.5"
#define XSDK_SS_1_7                         -140307     //  1/7"
#define XSDK_SS_1_7P5                       -133971     //  1/7.5"
#define XSDK_SS_1_8                         -125000     //  1/8"
#define XSDK_SS_1_8P5                       -116629     //  1/8.5"
#define XSDK_SS_1_9                         -111362     //  1/9"
#define XSDK_SS_1_10                        -99212      //  1/10"
#define XSDK_SS_1_12                        -82469      //  1/12"
#define XSDK_SS_1_13                        -78745      //  1/13"
#define XSDK_SS_1_14                        -70153      //  1/14"
#define XSDK_SS_1_15                        -66985      //  1/15"
#define XSDK_SS_1_17                        -58314      //  1/17"
#define XSDK_SS_1_18                        -55681      //  1/18"
#define XSDK_SS_1_20                        -49606      //  1/20"
#define XSDK_SS_1_25                        -41234      //  1/25"
#define XSDK_SS_1_26                        -39372      //  1/26"
#define XSDK_SS_1_27                        -35076      //  1/27"
#define XSDK_SS_1_28                        -33492      //  1/28"
#define XSDK_SS_1_30                        -31250      //  1/30"
#define XSDK_SS_1_34                        -29157      //  1/34"
#define XSDK_SS_1_38                        -27840      //  1/38"
#define XSDK_SS_1_40                        -24803      //  1/40"
#define XSDK_SS_1_42                        -23683      //  1/42"
#define XSDK_SS_1_45                        -22097      //  1/45"
#define XSDK_SS_1_50                        -20617      //  1/50"
#define XSDK_SS_1_52                        -19686      //  1/52"
#define XSDK_SS_1_56                        -17538      //  1/56"
#define XSDK_SS_1_58                        -16746      //  1/58"
#define XSDK_SS_1_60                        -15625      //  1/60"
#define XSDK_SS_1_70                        -14578      //  1/70"
#define XSDK_SS_1_75                        -13920      //  1/75"
#define XSDK_SS_1_80                        -12401      //  1/80"
#define XSDK_SS_1_85                        -11841      //  1/85"
#define XSDK_SS_1_90                        -11048      //  1/90"
#define XSDK_SS_1_100                       -10308      //  1/100"
#define XSDK_SS_1_105                       -9843       //  1/105"
#define XSDK_SS_1_110                       -8769       //  1/110"
#define XSDK_SS_1_120                       -8373       //  1/120"
#define XSDK_SS_1_125                       -7812       //  1/125"
#define XSDK_SS_1_140                       -7289       //  1/140"
#define XSDK_SS_1_150                       -6960       //  1/150"
#define XSDK_SS_1_160                       -6200       //  1/160"
#define XSDK_SS_1_170                       -5920       //  1/170"
#define XSDK_SS_1_180                       -5524       //  1/180"
#define XSDK_SS_1_200                       -5154       //  1/200"
#define XSDK_SS_1_210                       -4921       //  1/210"
#define XSDK_SS_1_220                       -4384       //  1/220"
#define XSDK_SS_1_240                       -4186       //  1/240"
#define XSDK_SS_1_250                       -3906       //  1/250"
#define XSDK_SS_1_280                       -3644       //  1/280"
#define XSDK_SS_1_300                       -3480       //  1/300"
#define XSDK_SS_1_320                       -3100       //  1/320"
#define XSDK_SS_1_340                       -2960       //  1/340"
#define XSDK_SS_1_350                       -2762       //  1/350"
#define XSDK_SS_1_400                       -2577       //  1/400"
#define XSDK_SS_1_420                       -2460       //  1/420"
#define XSDK_SS_1_450                       -2192       //  1/450"
#define XSDK_SS_1_480                       -2093       //  1/480"
#define XSDK_SS_1_500                       -1953       //  1/500"
#define XSDK_SS_1_550                       -1822       //  1/550"
#define XSDK_SS_1_600                       -1740       //  1/600"
#define XSDK_SS_1_640                       -1550       //  1/640"
#define XSDK_SS_1_680                       -1480       //  1/680"
#define XSDK_SS_1_750                       -1381       //  1/750"
#define XSDK_SS_1_800                       -1288       //  1/800"
#define XSDK_SS_1_850                       -1230       //  1/850"
#define XSDK_SS_1_900                       -1096       //  1/900"
#define XSDK_SS_1_950                       -1046       //  1/950"
#define XSDK_SS_1_1000                      -976        //  1/1000"
#define XSDK_SS_1_1100                      -911        //  1/1100"
#define XSDK_SS_1_1250                      -870        //  1/1250"
#define XSDK_SS_1_1300                      -775        //  1/1300"
#define XSDK_SS_1_1400                      -740        //  1/1400"
#define XSDK_SS_1_1500                      -690        //  1/1500"
#define XSDK_SS_1_1600                      -644        //  1/1600"
#define XSDK_SS_1_1700                      -615        //  1/1700"
#define XSDK_SS_1_1800                      -548        //  1/1800"
#define XSDK_SS_1_1900                      -523        //  1/1900"
#define XSDK_SS_1_2000                      -488        //  1/2000"
#define XSDK_SS_1_2200                      -455        //  1/2200"
#define XSDK_SS_1_2400                      -435        //  1/2400"
#define XSDK_SS_1_2500                      -387        //  1/2500"
#define XSDK_SS_1_2700                      -370        //  1/2700"
#define XSDK_SS_1_2900                      -345        //  1/2900"
#define XSDK_SS_1_3000                      -322        //  1/3000"
#define XSDK_SS_1_3200                      -307        //  1/3200"
#define XSDK_SS_1_3500                      -274        //  1/3500"
#define XSDK_SS_1_3800                      -261        //  1/3800"
#define XSDK_SS_1_4000                      -244        //  1/4000"
#define XSDK_SS_1_4400                      -227        //  1/4400"
#define XSDK_SS_1_4700                      -217        //  1/4700"
#define XSDK_SS_1_5000                      -193        //  1/5000"
#define XSDK_SS_1_5400                      -185        //  1/5400"
#define XSDK_SS_1_5800                      -172        //  1/5800"
#define XSDK_SS_1_6000                      -161        //  1/6000"
#define XSDK_SS_1_6400                      -153        //  1/6400"
#define XSDK_SS_1_7000                      -137        //  1/7000"
#define XSDK_SS_1_7500                      -130        //  1/7500"
#define XSDK_SS_1_8000                      -122        //  1/8000"
#define XSDK_SS_1_8500                      -113        //  1/8500"
#define XSDK_SS_1_9000                      -108        //  1/9000"
#define XSDK_SS_1_10000                     -96         //  1/10000"
#define XSDK_SS_1_10500                     -92         //  1/10500"
#define XSDK_SS_1_11000                     -86         //  1/11000"
#define XSDK_SS_1_12000                     -80         //  1/12000"
#define XSDK_SS_1_13000                     -76         //  1/13000"
#define XSDK_SS_1_14000                     -68         //  1/14000"
#define XSDK_SS_1_15000                     -65         //  1/15000"
#define XSDK_SS_1_16000                     -61         //  1/16000"
#define XSDK_SS_1_17000                     -56         //  1/17000"
#define XSDK_SS_1_18000                     -54         //  1/18000"
#define XSDK_SS_1_20000                     -48         //  1/20000"
#define XSDK_SS_1_22000                     -46         //  1/22000"
#define XSDK_SS_1_23000                     -43         //  1/23000"
#define XSDK_SS_1_25000                     -40         //  1/25000"
#define XSDK_SS_1_26000                     -38         //  1/26000"
#define XSDK_SS_1_28000                     -34         //  1/28000"
#define XSDK_SS_1_30000                     -32         //  1/30000"
#define XSDK_SS_1_32000                     -30         //  1/32000"
#define XSDK_SS_1_35000                     -28         //  1/35000"
#define XSDK_SS_1_38000                     -27         //  1/38000"
#define XSDK_SS_1_40000                     -24         //  1/40000"
#define XSDK_SS_1_43000                     -23         //  1/43000"
#define XSDK_SS_1_46000                     -21         //  1/46000"
#define XSDK_SS_1_50000                     -20         //  1/50000"
#define XSDK_SS_1_53000                     -19         //  1/53000"
#define XSDK_SS_1_57000                     -17         //  1/57000"
#define XSDK_SS_1_60000                     -16         //  1/60000"
#define XSDK_SS_1_64000                     -15         //  1/64000"
#define XSDK_SS_1_80000                     -12         //  1/80000"
#define XSDK_SS_1_102400                    -9          //  1/102400"
#define XSDK_SS_1_128000                    -7          //  1/128000"
#define XSDK_SS_1_160000                    -6          //  1/160000"
#define XSDK_SS_1_180000                    -5          //  1/180000"

// S1 Lock ShutterSpeed (High Frequency Flickerless)
#define XSDK_SS_1_8192P0                    -1000819200
#define XSDK_SS_1_6888P6                    -1000688860
#define XSDK_SS_1_5792P6                    -1000579260
#define XSDK_SS_1_4871P0                    -1000487100
#define XSDK_SS_1_4096P0                    -1000409600
#define XSDK_SS_1_3756P0                    -1000375600
#define XSDK_SS_1_3444P3                    -1000344430
#define XSDK_SS_1_3158P4                    -1000315840
#define XSDK_SS_1_2896P3                    -1000289630
#define XSDK_SS_1_2655P9                    -1000265590
#define XSDK_SS_1_2435P5                    -1000243550
#define XSDK_SS_1_2233P4                    -1000223340
#define XSDK_SS_1_2048P0                    -1000204800
#define XSDK_SS_1_1961P2                    -1000196120
#define XSDK_SS_1_1878P0                    -1000187800
#define XSDK_SS_1_1798P4                    -1000179840
#define XSDK_SS_1_1722P2                    -1000172220
#define XSDK_SS_1_1649P1                    -1000164910
#define XSDK_SS_1_1579P2                    -1000157920
#define XSDK_SS_1_1512P3                    -1000151230
#define XSDK_SS_1_1448P2                    -1000144820
#define XSDK_SS_1_1386P8                    -1000138680
#define XSDK_SS_1_1328P0                    -1000132800
#define XSDK_SS_1_1271P7                    -1000127170
#define XSDK_SS_1_1217P7                    -1000121770
#define XSDK_SS_1_1166P1                    -1000116610
#define XSDK_SS_1_1116P7                    -1000111670
#define XSDK_SS_1_1069P3                    -1000106930
#define XSDK_SS_1_1024P0                    -1000102400
#define XSDK_SS_1_1002P1                    -1000100210
#define XSDK_SS_1_980P6                     -1000098060
#define XSDK_SS_1_959P6                     -1000095960
#define XSDK_SS_1_939P0                     -1000093900
#define XSDK_SS_1_918P9                     -1000091890
#define XSDK_SS_1_899P2                     -1000089920
#define XSDK_SS_1_879P9                     -1000087990
#define XSDK_SS_1_861P1                     -1000086110
#define XSDK_SS_1_842P6                     -1000084260
#define XSDK_SS_1_824P6                     -1000082460
#define XSDK_SS_1_806P9                     -1000080690
#define XSDK_SS_1_789P6                     -1000078960
#define XSDK_SS_1_772P7                     -1000077270
#define XSDK_SS_1_756P1                     -1000075610
#define XSDK_SS_1_739P9                     -1000073990
#define XSDK_SS_1_724P1                     -1000072410
#define XSDK_SS_1_708P6                     -1000070860
#define XSDK_SS_1_693P4                     -1000069340
#define XSDK_SS_1_678P5                     -1000067850
#define XSDK_SS_1_664P0                     -1000066400
#define XSDK_SS_1_649P8                     -1000064980
#define XSDK_SS_1_635P8                     -1000063580
#define XSDK_SS_1_622P2                     -1000062220
#define XSDK_SS_1_608P9                     -1000060890
#define XSDK_SS_1_595P8                     -1000059580
#define XSDK_SS_1_583P1                     -1000058310
#define XSDK_SS_1_570P6                     -1000057060
#define XSDK_SS_1_558P3                     -1000055830
#define XSDK_SS_1_546P4                     -1000054640
#define XSDK_SS_1_534P7                     -1000053470
#define XSDK_SS_1_523P2                     -1000052320
#define XSDK_SS_1_512P0                     -1000051200
#define XSDK_SS_1_506P5                     -1000050650
#define XSDK_SS_1_501P0                     -1000050100
#define XSDK_SS_1_495P6                     -1000049560
#define XSDK_SS_1_490P3                     -1000049030
#define XSDK_SS_1_485P0                     -1000048500
#define XSDK_SS_1_479P8                     -1000047980
#define XSDK_SS_1_474P6                     -1000047460
#define XSDK_SS_1_469P5                     -1000046950
#define XSDK_SS_1_464P4                     -1000046440
#define XSDK_SS_1_459P4                     -1000045940
#define XSDK_SS_1_454P5                     -1000045450
#define XSDK_SS_1_449P6                     -1000044960
#define XSDK_SS_1_444P8                     -1000044480
#define XSDK_SS_1_440P0                     -1000044000
#define XSDK_SS_1_435P2                     -1000043520
#define XSDK_SS_1_430P5                     -1000043050
#define XSDK_SS_1_425P9                     -1000042590
#define XSDK_SS_1_421P3                     -1000042130
#define XSDK_SS_1_416P8                     -1000041680
#define XSDK_SS_1_412P3                     -1000041230
#define XSDK_SS_1_407P8                     -1000040780
#define XSDK_SS_1_403P5                     -1000040350
#define XSDK_SS_1_399P1                     -1000039910
#define XSDK_SS_1_394P8                     -1000039480
#define XSDK_SS_1_390P6                     -1000039060
#define XSDK_SS_1_386P3                     -1000038630
#define XSDK_SS_1_382P2                     -1000038220
#define XSDK_SS_1_378P1                     -1000037810
#define XSDK_SS_1_374P0                     -1000037400
#define XSDK_SS_1_370P0                     -1000037000
#define XSDK_SS_1_366P0                     -1000036600
#define XSDK_SS_1_362P0                     -1000036200
#define XSDK_SS_1_358P1                     -1000035810
#define XSDK_SS_1_354P3                     -1000035430
#define XSDK_SS_1_350P5                     -1000035050
#define XSDK_SS_1_346P7                     -1000034670
#define XSDK_SS_1_343P0                     -1000034300
#define XSDK_SS_1_339P3                     -1000033930
#define XSDK_SS_1_335P6                     -1000033560
#define XSDK_SS_1_332P0                     -1000033200
#define XSDK_SS_1_328P4                     -1000032840
#define XSDK_SS_1_324P9                     -1000032490
#define XSDK_SS_1_321P4                     -1000032140
#define XSDK_SS_1_317P9                     -1000031790
#define XSDK_SS_1_314P5                     -1000031450
#define XSDK_SS_1_311P1                     -1000031110
#define XSDK_SS_1_307P8                     -1000030780
#define XSDK_SS_1_304P4                     -1000030440
#define XSDK_SS_1_301P2                     -1000030120
#define XSDK_SS_1_297P9                     -1000029790
#define XSDK_SS_1_294P7                     -1000029470
#define XSDK_SS_1_291P5                     -1000029150
#define XSDK_SS_1_288P4                     -1000028840
#define XSDK_SS_1_285P3                     -1000028530
#define XSDK_SS_1_282P2                     -1000028220
#define XSDK_SS_1_279P2                     -1000027920
#define XSDK_SS_1_276P2                     -1000027620
#define XSDK_SS_1_273P2                     -1000027320
#define XSDK_SS_1_270P2                     -1000027020
#define XSDK_SS_1_267P3                     -1000026730
#define XSDK_SS_1_264P5                     -1000026450
#define XSDK_SS_1_261P6                     -1000026160
#define XSDK_SS_1_258P8                     -1000025880
#define XSDK_SS_1_256P0                     -1000025600
#define XSDK_SS_1_254P6                     -1000025460
#define XSDK_SS_1_253P2                     -1000025320
#define XSDK_SS_1_251P9                     -1000025190
#define XSDK_SS_1_250P5                     -1000025050
#define XSDK_SS_1_249P2                     -1000024920
#define XSDK_SS_1_247P8                     -1000024780
#define XSDK_SS_1_246P5                     -1000024650
#define XSDK_SS_1_245P1                     -1000024510
#define XSDK_SS_1_243P8                     -1000024380
#define XSDK_SS_1_242P5                     -1000024250
#define XSDK_SS_1_241P2                     -1000024120
#define XSDK_SS_1_239P9                     -1000023990
#define XSDK_SS_1_238P6                     -1000023860
#define XSDK_SS_1_237P3                     -1000023730
#define XSDK_SS_1_236P0                     -1000023600
#define XSDK_SS_1_234P8                     -1000023480
#define XSDK_SS_1_233P5                     -1000023350
#define XSDK_SS_1_232P2                     -1000023220
#define XSDK_SS_1_231P0                     -1000023100
#define XSDK_SS_1_229P7                     -1000022970
#define XSDK_SS_1_228P5                     -1000022850
#define XSDK_SS_1_227P2                     -1000022720
#define XSDK_SS_1_226P0                     -1000022600
#define XSDK_SS_1_224P8                     -1000022480
#define XSDK_SS_1_223P6                     -1000022360
#define XSDK_SS_1_222P4                     -1000022240
#define XSDK_SS_1_221P2                     -1000022120
#define XSDK_SS_1_220P0                     -1000022000
#define XSDK_SS_1_218P8                     -1000021880
#define XSDK_SS_1_217P6                     -1000021760
#define XSDK_SS_1_216P4                     -1000021640
#define XSDK_SS_1_215P3                     -1000021530
#define XSDK_SS_1_214P1                     -1000021410
#define XSDK_SS_1_213P0                     -1000021300
#define XSDK_SS_1_211P8                     -1000021180
#define XSDK_SS_1_210P7                     -1000021070
#define XSDK_SS_1_209P5                     -1000020950
#define XSDK_SS_1_208P4                     -1000020840
#define XSDK_SS_1_207P3                     -1000020730
#define XSDK_SS_1_206P1                     -1000020610
#define XSDK_SS_1_205P0                     -1000020500
#define XSDK_SS_1_203P9                     -1000020390
#define XSDK_SS_1_202P8                     -1000020280
#define XSDK_SS_1_201P7                     -1000020170
#define XSDK_SS_1_200P6                     -1000020060
#define XSDK_SS_1_199P6                     -1000019960
#define XSDK_SS_1_198P5                     -1000019850
#define XSDK_SS_1_197P4                     -1000019740
#define XSDK_SS_1_196P3                     -1000019630
#define XSDK_SS_1_195P3                     -1000019530
#define XSDK_SS_1_194P2                     -1000019420
#define XSDK_SS_1_193P2                     -1000019320
#define XSDK_SS_1_192P1                     -1000019210
#define XSDK_SS_1_191P1                     -1000019110
#define XSDK_SS_1_190P1                     -1000019010
#define XSDK_SS_1_189P0                     -1000018900
#define XSDK_SS_1_188P0                     -1000018800
#define XSDK_SS_1_187P0                     -1000018700
#define XSDK_SS_1_186P0                     -1000018600
#define XSDK_SS_1_185P0                     -1000018500
#define XSDK_SS_1_184P0                     -1000018400
#define XSDK_SS_1_183P0                     -1000018300
#define XSDK_SS_1_182P0                     -1000018200
#define XSDK_SS_1_181P0                     -1000018100
#define XSDK_SS_1_180P0                     -1000018000
#define XSDK_SS_1_179P1                     -1000017910
#define XSDK_SS_1_178P1                     -1000017810
#define XSDK_SS_1_177P1                     -1000017710
#define XSDK_SS_1_176P2                     -1000017620
#define XSDK_SS_1_175P2                     -1000017520
#define XSDK_SS_1_174P3                     -1000017430
#define XSDK_SS_1_173P3                     -1000017330
#define XSDK_SS_1_172P4                     -1000017240
#define XSDK_SS_1_171P5                     -1000017150
#define XSDK_SS_1_170P6                     -1000017060
#define XSDK_SS_1_169P6                     -1000016960
#define XSDK_SS_1_168P7                     -1000016870
#define XSDK_SS_1_167P8                     -1000016780
#define XSDK_SS_1_166P9                     -1000016690
#define XSDK_SS_1_166P0                     -1000016600
#define XSDK_SS_1_165P1                     -1000016510
#define XSDK_SS_1_164P2                     -1000016420
#define XSDK_SS_1_163P3                     -1000016330
#define XSDK_SS_1_162P4                     -1000016240
#define XSDK_SS_1_161P6                     -1000016160
#define XSDK_SS_1_160P7                     -1000016070
#define XSDK_SS_1_159P8                     -1000015980
#define XSDK_SS_1_159P0                     -1000015900
#define XSDK_SS_1_158P1                     -1000015810
#define XSDK_SS_1_157P2                     -1000015720
#define XSDK_SS_1_156P4                     -1000015640
#define XSDK_SS_1_155P6                     -1000015560
#define XSDK_SS_1_154P7                     -1000015470
#define XSDK_SS_1_153P9                     -1000015390
#define XSDK_SS_1_153P0                     -1000015300
#define XSDK_SS_1_152P2                     -1000015220
#define XSDK_SS_1_151P4                     -1000015140
#define XSDK_SS_1_150P6                     -1000015060
#define XSDK_SS_1_149P8                     -1000014980
#define XSDK_SS_1_149P0                     -1000014900
#define XSDK_SS_1_148P2                     -1000014820
#define XSDK_SS_1_147P4                     -1000014740
#define XSDK_SS_1_146P6                     -1000014660
#define XSDK_SS_1_145P8                     -1000014580
#define XSDK_SS_1_145P0                     -1000014500
#define XSDK_SS_1_144P2                     -1000014420
#define XSDK_SS_1_143P4                     -1000014340
#define XSDK_SS_1_142P6                     -1000014260
#define XSDK_SS_1_141P9                     -1000014190
#define XSDK_SS_1_141P1                     -1000014110
#define XSDK_SS_1_140P3                     -1000014030
#define XSDK_SS_1_139P6                     -1000013960
#define XSDK_SS_1_138P8                     -1000013880
#define XSDK_SS_1_138P1                     -1000013810
#define XSDK_SS_1_137P3                     -1000013730
#define XSDK_SS_1_136P6                     -1000013660
#define XSDK_SS_1_135P9                     -1000013590
#define XSDK_SS_1_135P1                     -1000013510
#define XSDK_SS_1_134P4                     -1000013440
#define XSDK_SS_1_133P7                     -1000013370
#define XSDK_SS_1_132P9                     -1000013290
#define XSDK_SS_1_132P2                     -1000013220
#define XSDK_SS_1_131P5                     -1000013150
#define XSDK_SS_1_130P8                     -1000013080
#define XSDK_SS_1_130P1                     -1000013010
#define XSDK_SS_1_129P4                     -1000012940
#define XSDK_SS_1_128P7                     -1000012870
#define XSDK_SS_1_128P0                     -1000012800
#define XSDK_SS_1_127P7                     -1000012770
#define XSDK_SS_1_127P3                     -1000012730
#define XSDK_SS_1_127P0                     -1000012700
#define XSDK_SS_1_126P6                     -1000012660
#define XSDK_SS_1_126P3                     -1000012630
#define XSDK_SS_1_125P9                     -1000012590
#define XSDK_SS_1_125P6                     -1000012560
#define XSDK_SS_1_125P3                     -1000012530
#define XSDK_SS_1_124P9                     -1000012490
#define XSDK_SS_1_124P6                     -1000012460
#define XSDK_SS_1_124P2                     -1000012420
#define XSDK_SS_1_123P9                     -1000012390
#define XSDK_SS_1_123P6                     -1000012360
#define XSDK_SS_1_123P2                     -1000012320
#define XSDK_SS_1_122P9                     -1000012290
#define XSDK_SS_1_122P6                     -1000012260
#define XSDK_SS_1_122P2                     -1000012220
#define XSDK_SS_1_121P9                     -1000012190
#define XSDK_SS_1_121P6                     -1000012160
#define XSDK_SS_1_121P3                     -1000012130
#define XSDK_SS_1_120P9                     -1000012090
#define XSDK_SS_1_120P6                     -1000012060
#define XSDK_SS_1_120P3                     -1000012030
#define XSDK_SS_1_119P9                     -1000011990
#define XSDK_SS_1_119P6                     -1000011960
#define XSDK_SS_1_119P3                     -1000011930
#define XSDK_SS_1_119P0                     -1000011900
#define XSDK_SS_1_118P7                     -1000011870
#define XSDK_SS_1_118P3                     -1000011830
#define XSDK_SS_1_118P0                     -1000011800
#define XSDK_SS_1_117P7                     -1000011770
#define XSDK_SS_1_117P4                     -1000011740
#define XSDK_SS_1_117P1                     -1000011710
#define XSDK_SS_1_116P7                     -1000011670
#define XSDK_SS_1_116P4                     -1000011640
#define XSDK_SS_1_116P1                     -1000011610
#define XSDK_SS_1_115P8                     -1000011580
#define XSDK_SS_1_115P5                     -1000011550
#define XSDK_SS_1_115P2                     -1000011520
#define XSDK_SS_1_114P9                     -1000011490
#define XSDK_SS_1_114P6                     -1000011460
#define XSDK_SS_1_114P2                     -1000011420
#define XSDK_SS_1_113P9                     -1000011390
#define XSDK_SS_1_113P6                     -1000011360
#define XSDK_SS_1_113P3                     -1000011330
#define XSDK_SS_1_113P0                     -1000011300
#define XSDK_SS_1_112P7                     -1000011270
#define XSDK_SS_1_112P4                     -1000011240
#define XSDK_SS_1_112P1                     -1000011210
#define XSDK_SS_1_111P8                     -1000011180
#define XSDK_SS_1_111P5                     -1000011150
#define XSDK_SS_1_111P2                     -1000011120
#define XSDK_SS_1_110P9                     -1000011090
#define XSDK_SS_1_110P6                     -1000011060
#define XSDK_SS_1_110P3                     -1000011030
#define XSDK_SS_1_110P0                     -1000011000
#define XSDK_SS_1_109P7                     -1000010970
#define XSDK_SS_1_109P4                     -1000010940
#define XSDK_SS_1_109P1                     -1000010910
#define XSDK_SS_1_108P8                     -1000010880
#define XSDK_SS_1_108P5                     -1000010850
#define XSDK_SS_1_108P2                     -1000010820
#define XSDK_SS_1_107P9                     -1000010790
#define XSDK_SS_1_107P6                     -1000010760
#define XSDK_SS_1_107P3                     -1000010730
#define XSDK_SS_1_107P1                     -1000010710
#define XSDK_SS_1_106P8                     -1000010680
#define XSDK_SS_1_106P5                     -1000010650
#define XSDK_SS_1_106P2                     -1000010620
#define XSDK_SS_1_105P9                     -1000010590
#define XSDK_SS_1_105P6                     -1000010560
#define XSDK_SS_1_105P3                     -1000010530
#define XSDK_SS_1_105P0                     -1000010500
#define XSDK_SS_1_104P8                     -1000010480
#define XSDK_SS_1_104P5                     -1000010450
#define XSDK_SS_1_104P2                     -1000010420
#define XSDK_SS_1_103P9                     -1000010390
#define XSDK_SS_1_103P6                     -1000010360
#define XSDK_SS_1_103P4                     -1000010340
#define XSDK_SS_1_103P1                     -1000010310
#define XSDK_SS_1_102P8                     -1000010280
#define XSDK_SS_1_102P5                     -1000010250
#define XSDK_SS_1_102P2                     -1000010220
#define XSDK_SS_1_102P0                     -1000010200
#define XSDK_SS_1_101P7                     -1000010170
#define XSDK_SS_1_101P4                     -1000010140
#define XSDK_SS_1_101P1                     -1000010110
#define XSDK_SS_1_100P9                     -1000010090
#define XSDK_SS_1_100P6                     -1000010060
#define XSDK_SS_1_100P3                     -1000010030
#define XSDK_SS_1_100P0                     -1000010000
#define XSDK_SS_1_99P8                      -1000009980
#define XSDK_SS_1_99P5                      -1000009950
#define XSDK_SS_1_99P2                      -1000009920
#define XSDK_SS_1_99P0                      -1000009900
#define XSDK_SS_1_98P7                      -1000009870
#define XSDK_SS_1_98P4                      -1000009840
#define XSDK_SS_1_98P2                      -1000009820
#define XSDK_SS_1_97P9                      -1000009790
#define XSDK_SS_1_97P6                      -1000009760
#define XSDK_SS_1_97P4                      -1000009740
#define XSDK_SS_1_97P1                      -1000009710
#define XSDK_SS_1_96P8                      -1000009680
#define XSDK_SS_1_96P6                      -1000009660
#define XSDK_SS_1_96P3                      -1000009630
#define XSDK_SS_1_96P1                      -1000009610
#define XSDK_SS_1_95P8                      -1000009580
#define XSDK_SS_1_95P5                      -1000009550
#define XSDK_SS_1_95P3                      -1000009530
#define XSDK_SS_1_95P0                      -1000009500
#define XSDK_SS_1_94P8                      -1000009480
#define XSDK_SS_1_94P5                      -1000009450
#define XSDK_SS_1_94P3                      -1000009430
#define XSDK_SS_1_94P0                      -1000009400
#define XSDK_SS_1_93P8                      -1000009380
#define XSDK_SS_1_93P5                      -1000009350
#define XSDK_SS_1_93P2                      -1000009320
#define XSDK_SS_1_93P0                      -1000009300
#define XSDK_SS_1_92P7                      -1000009270
#define XSDK_SS_1_92P5                      -1000009250
#define XSDK_SS_1_92P2                      -1000009220
#define XSDK_SS_1_92P0                      -1000009200
#define XSDK_SS_1_91P7                      -1000009170
#define XSDK_SS_1_91P5                      -1000009150
#define XSDK_SS_1_91P2                      -1000009120
#define XSDK_SS_1_91P0                      -1000009100
#define XSDK_SS_1_90P8                      -1000009080
#define XSDK_SS_1_90P5                      -1000009050
#define XSDK_SS_1_90P3                      -1000009030
#define XSDK_SS_1_90P0                      -1000009000
#define XSDK_SS_1_89P8                      -1000008980
#define XSDK_SS_1_89P5                      -1000008950
#define XSDK_SS_1_89P3                      -1000008930
#define XSDK_SS_1_89P1                      -1000008910
#define XSDK_SS_1_88P8                      -1000008880
#define XSDK_SS_1_88P6                      -1000008860
#define XSDK_SS_1_88P3                      -1000008830
#define XSDK_SS_1_88P1                      -1000008810
#define XSDK_SS_1_87P9                      -1000008790
#define XSDK_SS_1_87P6                      -1000008760
#define XSDK_SS_1_87P4                      -1000008740
#define XSDK_SS_1_87P1                      -1000008710
#define XSDK_SS_1_86P9                      -1000008690
#define XSDK_SS_1_86P7                      -1000008670
#define XSDK_SS_1_86P4                      -1000008640
#define XSDK_SS_1_86P2                      -1000008620
#define XSDK_SS_1_86P0                      -1000008600
#define XSDK_SS_1_85P7                      -1000008570
#define XSDK_SS_1_85P5                      -1000008550
#define XSDK_SS_1_85P3                      -1000008530
#define XSDK_SS_1_85P0                      -1000008500
#define XSDK_SS_1_84P8                      -1000008480
#define XSDK_SS_1_84P6                      -1000008460
#define XSDK_SS_1_84P4                      -1000008440
#define XSDK_SS_1_84P1                      -1000008410
#define XSDK_SS_1_83P9                      -1000008390
#define XSDK_SS_1_83P7                      -1000008370
#define XSDK_SS_1_83P4                      -1000008340
#define XSDK_SS_1_83P2                      -1000008320
#define XSDK_SS_1_83P0                      -1000008300
#define XSDK_SS_1_82P8                      -1000008280
#define XSDK_SS_1_82P5                      -1000008250
#define XSDK_SS_1_82P3                      -1000008230
#define XSDK_SS_1_82P1                      -1000008210
#define XSDK_SS_1_81P9                      -1000008190
#define XSDK_SS_1_81P7                      -1000008170
#define XSDK_SS_1_81P4                      -1000008140
#define XSDK_SS_1_81P2                      -1000008120
#define XSDK_SS_1_81P0                      -1000008100
#define XSDK_SS_1_80P8                      -1000008080
#define XSDK_SS_1_80P6                      -1000008060
#define XSDK_SS_1_80P3                      -1000008030
#define XSDK_SS_1_80P1                      -1000008010
#define XSDK_SS_1_79P9                      -1000007990
#define XSDK_SS_1_79P7                      -1000007970
#define XSDK_SS_1_79P5                      -1000007950
#define XSDK_SS_1_79P3                      -1000007930
#define XSDK_SS_1_79P0                      -1000007900
#define XSDK_SS_1_78P8                      -1000007880
#define XSDK_SS_1_78P6                      -1000007860
#define XSDK_SS_1_78P4                      -1000007840
#define XSDK_SS_1_78P2                      -1000007820
#define XSDK_SS_1_78P0                      -1000007800
#define XSDK_SS_1_77P8                      -1000007780
#define XSDK_SS_1_77P6                      -1000007760
#define XSDK_SS_1_77P4                      -1000007740
#define XSDK_SS_1_77P1                      -1000007710
#define XSDK_SS_1_76P9                      -1000007690
#define XSDK_SS_1_76P7                      -1000007670
#define XSDK_SS_1_76P5                      -1000007650
#define XSDK_SS_1_76P3                      -1000007630
#define XSDK_SS_1_76P1                      -1000007610
#define XSDK_SS_1_75P9                      -1000007590
#define XSDK_SS_1_75P7                      -1000007570
#define XSDK_SS_1_75P5                      -1000007550
#define XSDK_SS_1_75P3                      -1000007530
#define XSDK_SS_1_75P1                      -1000007510
#define XSDK_SS_1_74P9                      -1000007490
#define XSDK_SS_1_74P7                      -1000007470
#define XSDK_SS_1_74P5                      -1000007450
#define XSDK_SS_1_74P3                      -1000007430
#define XSDK_SS_1_74P1                      -1000007410
#define XSDK_SS_1_73P9                      -1000007390
#define XSDK_SS_1_73P7                      -1000007370
#define XSDK_SS_1_73P5                      -1000007350
#define XSDK_SS_1_73P3                      -1000007330
#define XSDK_SS_1_73P1                      -1000007310
#define XSDK_SS_1_72P9                      -1000007290
#define XSDK_SS_1_72P7                      -1000007270
#define XSDK_SS_1_72P5                      -1000007250
#define XSDK_SS_1_72P3                      -1000007230
#define XSDK_SS_1_72P1                      -1000007210
#define XSDK_SS_1_71P9                      -1000007190
#define XSDK_SS_1_71P7                      -1000007170
#define XSDK_SS_1_71P5                      -1000007150
#define XSDK_SS_1_71P3                      -1000007130
#define XSDK_SS_1_71P1                      -1000007110
#define XSDK_SS_1_70P9                      -1000007090
#define XSDK_SS_1_70P7                      -1000007070
#define XSDK_SS_1_70P6                      -1000007060
#define XSDK_SS_1_70P4                      -1000007040
#define XSDK_SS_1_70P2                      -1000007020
#define XSDK_SS_1_70P0                      -1000007000
#define XSDK_SS_1_69P8                      -1000006980
#define XSDK_SS_1_69P6                      -1000006960
#define XSDK_SS_1_69P4                      -1000006940
#define XSDK_SS_1_69P2                      -1000006920
#define XSDK_SS_1_69P0                      -1000006900
#define XSDK_SS_1_68P9                      -1000006890
#define XSDK_SS_1_68P7                      -1000006870
#define XSDK_SS_1_68P5                      -1000006850
#define XSDK_SS_1_68P3                      -1000006830
#define XSDK_SS_1_68P1                      -1000006810
#define XSDK_SS_1_67P9                      -1000006790
#define XSDK_SS_1_67P7                      -1000006770
#define XSDK_SS_1_67P6                      -1000006760
#define XSDK_SS_1_67P4                      -1000006740
#define XSDK_SS_1_67P2                      -1000006720
#define XSDK_SS_1_67P0                      -1000006700
#define XSDK_SS_1_66P8                      -1000006680
#define XSDK_SS_1_66P7                      -1000006670
#define XSDK_SS_1_66P5                      -1000006650
#define XSDK_SS_1_66P3                      -1000006630
#define XSDK_SS_1_66P1                      -1000006610
#define XSDK_SS_1_65P9                      -1000006590
#define XSDK_SS_1_65P8                      -1000006580
#define XSDK_SS_1_65P6                      -1000006560
#define XSDK_SS_1_65P4                      -1000006540
#define XSDK_SS_1_65P2                      -1000006520
#define XSDK_SS_1_65P0                      -1000006500
#define XSDK_SS_1_64P9                      -1000006490
#define XSDK_SS_1_64P7                      -1000006470
#define XSDK_SS_1_64P5                      -1000006450
#define XSDK_SS_1_64P3                      -1000006430
#define XSDK_SS_1_64P2                      -1000006420
#define XSDK_SS_1_64P0                      -1000006400
#define XSDK_SS_1_63P8                      -1000006380
#define XSDK_SS_1_63P7                      -1000006370
#define XSDK_SS_1_63P5                      -1000006350
#define XSDK_SS_1_63P3                      -1000006330
#define XSDK_SS_1_63P1                      -1000006310
#define XSDK_SS_1_63P0                      -1000006300
#define XSDK_SS_1_62P8                      -1000006280
#define XSDK_SS_1_62P6                      -1000006260
#define XSDK_SS_1_62P5                      -1000006250
#define XSDK_SS_1_62P3                      -1000006230
#define XSDK_SS_1_62P1                      -1000006210
#define XSDK_SS_1_62P0                      -1000006200
#define XSDK_SS_1_61P8                      -1000006180
#define XSDK_SS_1_61P6                      -1000006160
#define XSDK_SS_1_61P5                      -1000006150
#define XSDK_SS_1_61P3                      -1000006130
#define XSDK_SS_1_61P1                      -1000006110
#define XSDK_SS_1_61P0                      -1000006100
#define XSDK_SS_1_60P8                      -1000006080
#define XSDK_SS_1_60P6                      -1000006060
#define XSDK_SS_1_60P5                      -1000006050
#define XSDK_SS_1_60P3                      -1000006030
#define XSDK_SS_1_60P1                      -1000006010
#define XSDK_SS_1_60P0                      -1000006000
#define XSDK_SS_1_59P8                      -1000005980
#define XSDK_SS_1_59P6                      -1000005960
#define XSDK_SS_1_59P5                      -1000005950
#define XSDK_SS_1_59P3                      -1000005930
#define XSDK_SS_1_59P2                      -1000005920
#define XSDK_SS_1_59P0                      -1000005900
#define XSDK_SS_1_58P8                      -1000005880
#define XSDK_SS_1_58P7                      -1000005870
#define XSDK_SS_1_58P5                      -1000005850
#define XSDK_SS_1_58P4                      -1000005840
#define XSDK_SS_1_58P2                      -1000005820
#define XSDK_SS_1_58P1                      -1000005810
#define XSDK_SS_1_57P9                      -1000005790
#define XSDK_SS_1_57P7                      -1000005770
#define XSDK_SS_1_57P6                      -1000005760
#define XSDK_SS_1_57P4                      -1000005740
#define XSDK_SS_1_57P3                      -1000005730
#define XSDK_SS_1_57P1                      -1000005710
#define XSDK_SS_1_57P0                      -1000005700
#define XSDK_SS_1_56P8                      -1000005680
#define XSDK_SS_1_56P7                      -1000005670
#define XSDK_SS_1_56P5                      -1000005650
#define XSDK_SS_1_56P4                      -1000005640
#define XSDK_SS_1_56P2                      -1000005620
#define XSDK_SS_1_56P0                      -1000005600
#define XSDK_SS_1_55P9                      -1000005590
#define XSDK_SS_1_55P7                      -1000005570
#define XSDK_SS_1_55P6                      -1000005560
#define XSDK_SS_1_55P4                      -1000005540
#define XSDK_SS_1_55P3                      -1000005530
#define XSDK_SS_1_55P1                      -1000005510
#define XSDK_SS_1_55P0                      -1000005500
#define XSDK_SS_1_54P8                      -1000005480
#define XSDK_SS_1_54P7                      -1000005470
#define XSDK_SS_1_54P6                      -1000005460
#define XSDK_SS_1_54P4                      -1000005440
#define XSDK_SS_1_54P3                      -1000005430
#define XSDK_SS_1_54P1                      -1000005410
#define XSDK_SS_1_54P0                      -1000005400
#define XSDK_SS_1_53P8                      -1000005380
#define XSDK_SS_1_53P7                      -1000005370
#define XSDK_SS_1_53P5                      -1000005350
#define XSDK_SS_1_53P4                      -1000005340
#define XSDK_SS_1_53P2                      -1000005320
#define XSDK_SS_1_53P1                      -1000005310
#define XSDK_SS_1_53P0                      -1000005300
#define XSDK_SS_1_52P8                      -1000005280
#define XSDK_SS_1_52P7                      -1000005270
#define XSDK_SS_1_52P5                      -1000005250
#define XSDK_SS_1_52P4                      -1000005240
#define XSDK_SS_1_52P2                      -1000005220
#define XSDK_SS_1_52P1                      -1000005210
#define XSDK_SS_1_52P0                      -1000005200
#define XSDK_SS_1_51P8                      -1000005180
#define XSDK_SS_1_51P7                      -1000005170
#define XSDK_SS_1_51P5                      -1000005150
#define XSDK_SS_1_51P4                      -1000005140
#define XSDK_SS_1_51P3                      -1000005130
#define XSDK_SS_1_51P1                      -1000005110
#define XSDK_SS_1_51P0                      -1000005100
#define XSDK_SS_1_50P8                      -1000005080
#define XSDK_SS_1_50P7                      -1000005070
#define XSDK_SS_1_50P6                      -1000005060
#define XSDK_SS_1_50P4                      -1000005040
#define XSDK_SS_1_50P3                      -1000005030
#define XSDK_SS_1_50P2                      -1000005020
#define XSDK_SS_1_50P0                      -1000005000

// ExposureBias
#define XSDK_EXPOSURE_BIAS_P5P00            150         // +5EV
#define XSDK_EXPOSURE_BIAS_P4P67            140         // +4 2/3EV
#define XSDK_EXPOSURE_BIAS_P4P33            130         // +4 1/3EV
#define XSDK_EXPOSURE_BIAS_P4P00            120         // +4EV
#define XSDK_EXPOSURE_BIAS_P3P67            110         // +3 2/3EV
#define XSDK_EXPOSURE_BIAS_P3P33            100         // +3 1/3EV
#define XSDK_EXPOSURE_BIAS_P3P00             90         // +3EV
#define XSDK_EXPOSURE_BIAS_P2P67             80         // +2 2/3EV
#define XSDK_EXPOSURE_BIAS_P2P33             70         // +2 1/3EV
#define XSDK_EXPOSURE_BIAS_P2P00             60         // +2EV
#define XSDK_EXPOSURE_BIAS_P1P67             50         // +1 2/3EV
#define XSDK_EXPOSURE_BIAS_P1P33             40         // +1 1/3EV
#define XSDK_EXPOSURE_BIAS_P1P00             30         // +1EV
#define XSDK_EXPOSURE_BIAS_P0P67             20         // +2/3EV
#define XSDK_EXPOSURE_BIAS_P0P33             10         // +1/3EV
#define XSDK_EXPOSURE_BIAS_0                  0         // Å}0EV
#define XSDK_EXPOSURE_BIAS_M0P33            -10         // -1/3EV
#define XSDK_EXPOSURE_BIAS_M0P67            -20         // -2/3EV
#define XSDK_EXPOSURE_BIAS_M1P00            -30         // -1EV
#define XSDK_EXPOSURE_BIAS_M1P33            -40         // -1 1/3EV
#define XSDK_EXPOSURE_BIAS_M1P67            -50         // -1 2/3EV
#define XSDK_EXPOSURE_BIAS_M2P00            -60         // -2EV
#define XSDK_EXPOSURE_BIAS_M2P33            -70         // -2 1/3EV
#define XSDK_EXPOSURE_BIAS_M2P67            -80         // -2 2/3EV
#define XSDK_EXPOSURE_BIAS_M3P00            -90         // -3EV
#define XSDK_EXPOSURE_BIAS_M3P33           -100         // -3 1/3EV
#define XSDK_EXPOSURE_BIAS_M3P67           -110         // -3 2/3EV
#define XSDK_EXPOSURE_BIAS_M4P00           -120         // -4EV
#define XSDK_EXPOSURE_BIAS_M4P33           -130         // -4 1/3EV
#define XSDK_EXPOSURE_BIAS_M4P67           -140         // -4 2/3EV
#define XSDK_EXPOSURE_BIAS_M5P00           -150         // -5EV

// Sensitivity
#define XSDK_SENSITIVITY_ISO40              40          // ISO 40
#define XSDK_SENSITIVITY_ISO50              50          // ISO 50
#define XSDK_SENSITIVITY_ISO60              60          // ISO 60
#define XSDK_SENSITIVITY_ISO64              64          // ISO 64
#define XSDK_SENSITIVITY_ISO80              80          // ISO 80
#define XSDK_SENSITIVITY_ISO100             100         // ISO 100
#define XSDK_SENSITIVITY_ISO125             125         // ISO 125
#define XSDK_SENSITIVITY_ISO160             160         // ISO 160
#define XSDK_SENSITIVITY_ISO200             200         // ISO 200
#define XSDK_SENSITIVITY_ISO250             250         // ISO 250
#define XSDK_SENSITIVITY_ISO320             320         // ISO 320
#define XSDK_SENSITIVITY_ISO400             400         // ISO 400
#define XSDK_SENSITIVITY_ISO500             500         // ISO 500
#define XSDK_SENSITIVITY_ISO640             640         // ISO 640
#define XSDK_SENSITIVITY_ISO800             800         // ISO 800
#define XSDK_SENSITIVITY_ISO1000            1000        // ISO 1000
#define XSDK_SENSITIVITY_ISO1250            1250        // ISO 1250
#define XSDK_SENSITIVITY_ISO1600            1600        // ISO 1600
#define XSDK_SENSITIVITY_ISO2000            2000        // ISO 2000
#define XSDK_SENSITIVITY_ISO2500            2500        // ISO 2500
#define XSDK_SENSITIVITY_ISO3200            3200        // ISO 3200
#define XSDK_SENSITIVITY_ISO4000            4000        // ISO 4000
#define XSDK_SENSITIVITY_ISO5000            5000        // ISO 5000
#define XSDK_SENSITIVITY_ISO6400            6400        // ISO 6400
#define XSDK_SENSITIVITY_ISO8000            8000        // ISO 8000
#define XSDK_SENSITIVITY_ISO10000           10000       // ISO 10000
#define XSDK_SENSITIVITY_ISO12800           12800       // ISO 12800
#define XSDK_SENSITIVITY_ISO16000           16000       // ISO 16000
#define XSDK_SENSITIVITY_ISO20000           20000       // ISO 20000
#define XSDK_SENSITIVITY_ISO25600           25600       // ISO 25600
#define XSDK_SENSITIVITY_ISO32000           32000       // ISO 32000
#define XSDK_SENSITIVITY_ISO40000           40000       // ISO 40000
#define XSDK_SENSITIVITY_ISO51200           51200       // ISO 51200
#define XSDK_SENSITIVITY_ISO64000           64000       // ISO 64000
#define XSDK_SENSITIVITY_ISO80000           80000       // ISO 80000
#define XSDK_SENSITIVITY_ISO102400          102400      // ISO 102400
#define XSDK_SENSITIVITY_AUTO_1             -1          // ISO AUTO (1)
#define XSDK_SENSITIVITY_AUTO_2             -2          // ISO AUTO (2)
#define XSDK_SENSITIVITY_AUTO_3             -3          // ISO AUTO (3)
#define XSDK_SENSITIVITY_AUTO_4             -4          // ISO AUTO (4)
#define XSDK_SENSITIVITY_AUTO               -10         // ISO AUTO
#define XSDK_SENSITIVITY_AUTO400            -400        // ISO AUTO 400
#define XSDK_SENSITIVITY_AUTO800            -800        // ISO AUTO 800
#define XSDK_SENSITIVITY_AUTO1600           -1600       // ISO AUTO 1600
#define XSDK_SENSITIVITY_AUTO3200           -3200       // ISO AUTO 3200
#define XSDK_SENSITIVITY_AUTO6400           -6400       // ISO AUTO 6400

// D Range
#define XSDK_DRANGE_AUTO                    0xFFFF      //  AUTO
#define XSDK_DRANGE_100                     100         //  100%
#define XSDK_DRANGE_200                     200         //  200%
#define XSDK_DRANGE_400                     400         //  400%
#define XSDK_DRANGE_800                     800         //  800%

// Exposure bias
#define XSDK_AEBIAS_MIN                     -150        //  Min. value  ( -5.0EV)
#define XSDK_AEBIAS_MAX                      150        //  Max. value  ( +5.0EV)

// Metering Mode
#define XSDK_METERING_AVERAGE               0x0001
#define XSDK_METERING_MULTI                 0x0003
#define XSDK_METERING_CENTER                0x0004
#define XSDK_METERING_CENTER_WEIGHTED       0x0002

// Force Mode
#define XSDK_FORCESHOOTSTANDBY_SHOOT        0x0001      // Shoot Mode
#define XSDK_FORCESHOOTSTANDBY_PLAYBACK     0x0002      // Playback Mode

// DRIVE MODE
#define XSDK_DRIVE_MODE_INVALID             0xFFFF
#define XSDK_DRIVE_MODE_CH                  0x0002
#define XSDK_DRIVE_MODE_CL                  0x0003
#define XSDK_DRIVE_MODE_S                   0x0004      // S
#define XSDK_DRIVE_MODE_MULTI_EXPOSURE      0x0005
#define XSDK_DRIVE_MODE_ADVFILTER           0x0006
#define XSDK_DRIVE_MODE_PANORAMA            0x0007
#define XSDK_DRIVE_MODE_MOVIE               0x0008      // MOVIE
#define XSDK_DRIVE_MODE_HDR                 0x0009
#define XSDK_DRIVE_MODE_BKT_AE              0x000A
#define XSDK_DRIVE_MODE_BKT_ISO             0x000B
#define XSDK_DRIVE_MODE_BKT_FILMSIMULATION  0x000C
#define XSDK_DRIVE_MODE_BKT_WHITEBALANCE    0x000D
#define XSDK_DRIVE_MODE_BKT_DYNAMICRANGE    0x000E
#define XSDK_DRIVE_MODE_BKT_FOCUS           0x000F
#define XSDK_DRIVE_MODE_PIXELSHIFTMULTISHOT 0x0010
#define XSDK_DRIVE_MODE_CH_CROP             0x0011
#define XSDK_DRIVE_MODE_PIXELSHIFTMULTISHOT_FEWERFRAMES 0x0012

// MODE
#define XSDK_MODE_STILL_C0                  0x0001
#define XSDK_MODE_STILL_C1                  0x0002
#define XSDK_MODE_STILL_C2                  0x0003
#define XSDK_MODE_STILL_C3                  0x0004
#define XSDK_MODE_STILL_C4                  0x0005
#define XSDK_MODE_STILL_C5                  0x0006
#define XSDK_MODE_STILL_C6                  0x0007
#define XSDK_MODE_STILL_C7                  0x0008

#define XSDK_MODE_STILL_ADVFILTER           0x0081
#define XSDK_MODE_STILL_SP                  0x00B1
#define XSDK_MODE_STILL_AUTO                0x00F0

#define XSDK_MODE_MOVIE_C0                  0x0101
#define XSDK_MODE_MOVIE_C1                  0x0102
#define XSDK_MODE_MOVIE_C2                  0x0103
#define XSDK_MODE_MOVIE_C3                  0x0104
#define XSDK_MODE_MOVIE_C4                  0x0105
#define XSDK_MODE_MOVIE_C5                  0x0106
#define XSDK_MODE_MOVIE_C6                  0x0107
#define XSDK_MODE_MOVIE_C7                  0x0108

#define XSDK_MODE_MOVIE_VLOG                0x0161

// White balance
#define XSDK_WB_AUTO                        0x0002
#define	XSDK_WB_AUTO_WHITE_PRIORITY			0x8020
#define	XSDK_WB_AUTO_AMBIENCE_PRIORITY		0x8021
#define XSDK_WB_DAYLIGHT                    0x0004
#define XSDK_WB_INCANDESCENT                0x0006
#define XSDK_WB_UNDER_WATER                 0x0008
#define XSDK_WB_FLUORESCENT1                0x8001
#define XSDK_WB_FLUORESCENT2                0x8002
#define XSDK_WB_FLUORESCENT3                0x8003
#define XSDK_WB_SHADE                       0x8006
#define XSDK_WB_COLORTEMP                   0x8007
#define XSDK_WB_CUSTOM1                     0x8008
#define XSDK_WB_CUSTOM2                     0x8009
#define XSDK_WB_CUSTOM3                     0x800A
#define XSDK_WB_CUSTOM4                     0x800B
#define XSDK_WB_CUSTOM5                     0x800C

// White balance Color Temptune
#define XSDK_WB_COLORTEMP_2500              2500
#define XSDK_WB_COLORTEMP_2550              2550
#define XSDK_WB_COLORTEMP_2650              2650
#define XSDK_WB_COLORTEMP_2700              2700
#define XSDK_WB_COLORTEMP_2800              2800
#define XSDK_WB_COLORTEMP_2850              2850
#define XSDK_WB_COLORTEMP_2950              2950
#define XSDK_WB_COLORTEMP_3000              3000
#define XSDK_WB_COLORTEMP_3100              3100
#define XSDK_WB_COLORTEMP_3200              3200
#define XSDK_WB_COLORTEMP_3300              3300
#define XSDK_WB_COLORTEMP_3400              3400
#define XSDK_WB_COLORTEMP_3600              3600
#define XSDK_WB_COLORTEMP_3700              3700
#define XSDK_WB_COLORTEMP_3800              3800
#define XSDK_WB_COLORTEMP_4000              4000
#define XSDK_WB_COLORTEMP_4200              4200
#define XSDK_WB_COLORTEMP_4300              4300
#define XSDK_WB_COLORTEMP_4500              4500
#define XSDK_WB_COLORTEMP_4800              4800
#define XSDK_WB_COLORTEMP_5000              5000
#define XSDK_WB_COLORTEMP_5300              5300
#define XSDK_WB_COLORTEMP_5600              5600
#define XSDK_WB_COLORTEMP_5900              5900
#define XSDK_WB_COLORTEMP_6300              6300
#define XSDK_WB_COLORTEMP_6700              6700
#define XSDK_WB_COLORTEMP_7100              7100
#define XSDK_WB_COLORTEMP_7700              7700
#define XSDK_WB_COLORTEMP_8300              8300
#define XSDK_WB_COLORTEMP_9100              9100
#define XSDK_WB_COLORTEMP_10000             10000
#define XSDK_WB_COLORTEMP_CURRENT           0

// Media Record
#define XSDK_MEDIAREC_RAWJPEG               0x0001
#define XSDK_MEDIAREC_RAW                   0x0002
#define XSDK_MEDIAREC_JPEG                  0x0003
#define XSDK_MEDIAREC_OFF                   0x0004
#define XSDK_MEDIAREC_RAWJPEGHEIF           XSDK_MEDIAREC_RAWJPEG
#define XSDK_MEDIAREC_JPEGHEIF              XSDK_MEDIAREC_JPEG

// Capability
#define XSDK_YES                            0x0001
#define XSDK_NO                             0x0000

#define XSDK_ON                             0x0001
#define XSDK_OFF                            0x0002

// ErrorDetails
#define XSDK_ERROR_DETAIL_S1                0x00000001
#define XSDK_ERROR_DETAIL_AEL               0x00000002
#define XSDK_ERROR_DETAIL_AFL               0x00000004
#define XSDK_ERROR_DETAIL_INSTANTAF         0x00000008
#define XSDK_ERROR_DETAIL_AFON              0x00000010
#define XSDK_ERROR_DETAIL_SHOOTING          0x00000020
#define XSDK_ERROR_DETAIL_SHOOTINGCOUNTDOWN 0x00000040
#define XSDK_ERROR_DETAIL_RECORDING         0x00000080
#define XSDK_ERROR_DETAIL_LIVEVIEW          0x00000100
#define XSDK_ERROR_DETAIL_UNTRANSFERRED_IMAGE 0x00000200

//
// Error code (XSDK_GetErrorNumber)
//
enum {
    XSDK_ERRCODE_NOERR                = 0x00000000,
    XSDK_ERRCODE_SEQUENCE             = 0x00001001,
    XSDK_ERRCODE_PARAM                = 0x00001002,
    XSDK_ERRCODE_INVALID_CAMERA       = 0x00001003,
    XSDK_ERRCODE_LOADLIB              = 0x00001004,
    XSDK_ERRCODE_UNSUPPORTED          = 0x00001005,
    XSDK_ERRCODE_BUSY                 = 0x00001006,
    XSDK_ERRCODE_AF_TIMEOUT           = 0x00001007,
    XSDK_ERRCODE_SHOOT_ERROR          = 0x00001008,
    XSDK_ERRCODE_FRAME_FULL           = 0x00001009,
    XSDK_ERRCODE_STANDBY              = 0x00001010,
    XSDK_ERRCODE_NODRIVER             = 0x00001011,
    XSDK_ERRCODE_NO_MODEL_MODULE      = 0x00001012,
    XSDK_ERRCODE_API_NOTFOUND         = 0x00001013,
    XSDK_ERRCODE_API_MISMATCH         = 0x00001014,
    XSDK_ERRCODE_INVALID_USBMODE      = 0x00001015,
	XSDK_ERRCODE_FORCEMODE_BUSY       = 0x00001016,
	XSDK_ERRCODE_RUNNING_OTHER_FUNCTION = 0x00001017,
    XSDK_ERRCODE_COMMUNICATION        = 0x00002001,
    XSDK_ERRCODE_TIMEOUT              = 0x00002002,
    XSDK_ERRCODE_COMBINATION          = 0x00002003,
    XSDK_ERRCODE_WRITEERROR           = 0x00002004,
    XSDK_ERRCODE_CARDFULL             = 0x00002005,
    XSDK_ERRCODE_HARDWARE             = 0x00003001,
    XSDK_ERRCODE_INTERNAL             = 0x00009001,
    XSDK_ERRCODE_MEMFULL              = 0x00009002,
    XSDK_ERRCODE_UNKNOWN              = 0x00009100
};

//  SDK API RETURN VALUE
enum{
    XSDK_COMPLETE   =   0,
    XSDK_ERROR      = -1
};


#ifndef _WIN32
#pragma export on
#endif

//
// Export API defines
//
#ifdef  __cplusplus
extern "C" {
#endif  // _cplusplus

// Initialize/Finalize
XSDK_APIENTRY XSDK_Init( LIB_HANDLE hLib );
XSDK_APIENTRY XSDK_Exit( void );

// Enumeration
XSDK_APIENTRY XSDK_Detect( long lInterface, LPSTR pInterface, LPSTR pDeviceName, long* plCount );
XSDK_APIENTRY XSDK_Append( long lInterface, LPSTR pInterface, LPSTR pDeviceName, long* plCount, XSDK_CameraList *pCameraList );
// Session management
XSDK_APIENTRY XSDK_Close( XSDK_HANDLE hCamera );
XSDK_APIENTRY XSDK_PowerOFF( XSDK_HANDLE hCamera );
XSDK_APIENTRY XSDK_OpenEx( LPSTR pDevice, XSDK_HANDLE* phCamera, long* plCameraMode, void* pOption );

// Basic functions
XSDK_APIENTRY XSDK_GetErrorNumber( XSDK_HANDLE hCamera, long* plAPICode, long* plERRCode );
XSDK_APIENTRY XSDK_GetVersionString( LPSTR pVersionString );
XSDK_APIENTRY XSDK_GetErrorDetails( XSDK_HANDLE hCamera, long* plERRCode );

// Device Information
XSDK_APIENTRY XSDK_GetDeviceInfo( XSDK_HANDLE hCamera, XSDK_DeviceInformation* pDevInfo );
XSDK_APIENTRY XSDK_WriteDeviceName( XSDK_HANDLE hCamera, LPCSTR pDeviceName );
XSDK_APIENTRY XSDK_GetFirmwareVersion( XSDK_HANDLE hCamera, LPSTR pFirmwareVersion );
XSDK_APIENTRY XSDK_GetLensInfo( XSDK_HANDLE hCamera, XSDK_LensInformation* pLensInfo );
XSDK_APIENTRY XSDK_GetLensVersion( XSDK_HANDLE hCamera, LPSTR pLensVersion );
XSDK_APIENTRY XSDK_GetDeviceInfoEx( XSDK_HANDLE hCamera, XSDK_DeviceInformation* pDevInfo, long* plNumAPICode, long* plAPICode );

// Camera Operation Mode
XSDK_APIENTRY XSDK_CapPriorityMode( XSDK_HANDLE hCamera, long* plNumPriorityMode, long* plPriorityMode );
XSDK_APIENTRY XSDK_SetPriorityMode( XSDK_HANDLE hCamera, long lPriorityMode );
XSDK_APIENTRY XSDK_GetPriorityMode( XSDK_HANDLE hCamera, long* plPriorityMode );

// Release Control
XSDK_APIENTRY XSDK_CapRelease( XSDK_HANDLE hCamera, long* plNumReleaseMode, long* plReleaseMode );
XSDK_APIENTRY XSDK_Release( XSDK_HANDLE hCamera, long lReleaseMode, long* plShotOpt, long* plAfStatus );
XSDK_APIENTRY XSDK_CapReleaseEx( XSDK_HANDLE hCamera, long* plNumReleaseMode, long* plReleaseMode );
XSDK_APIENTRY XSDK_ReleaseEx( XSDK_HANDLE hCamera, long lReleaseMode, long* plShotOpt, long* plAfStatus );
XSDK_APIENTRY XSDK_CapReleaseStatus( XSDK_HANDLE hCamera, long* plNum, long* plReleaseStatus );
XSDK_APIENTRY XSDK_GetReleaseStatus( XSDK_HANDLE hCamera, long* plReleaseStatus );

// Image acquisition
XSDK_APIENTRY XSDK_GetBufferCapacity( XSDK_HANDLE hCamera, long* plShootFrameNum, long* plTotalFrameNum );
XSDK_APIENTRY XSDK_ReadImageInfo( XSDK_HANDLE hCamera, XSDK_ImageInformation* pImgInfo );
XSDK_APIENTRY XSDK_ReadPreview(XSDK_HANDLE hCamera, unsigned char* pData, unsigned long lDataSize);
XSDK_APIENTRY XSDK_ReadImage(XSDK_HANDLE hCamera, unsigned char* pData, unsigned long lDataSize);
XSDK_APIENTRY XSDK_DeleteImage( XSDK_HANDLE hCamera );

// Exposure control
XSDK_APIENTRY XSDK_CapAEMode( XSDK_HANDLE hCamera, long* plNumAEMode, long* plAEMode );
XSDK_APIENTRY XSDK_SetAEMode( XSDK_HANDLE hCamera, long lAEMode );
XSDK_APIENTRY XSDK_GetAEMode( XSDK_HANDLE hCamera, long* plAEMode );

XSDK_APIENTRY XSDK_CapShutterSpeed( XSDK_HANDLE hCamera, long* plNumShutterSpeed, long* plShutterSpeed, long* plBulbCapable );
XSDK_APIENTRY XSDK_SetShutterSpeed( XSDK_HANDLE hCamera, long lShutterSpeed, long lBulb );
XSDK_APIENTRY XSDK_GetShutterSpeed( XSDK_HANDLE hCamera, long* plShutterSpeed, long* plBulb );

XSDK_APIENTRY XSDK_CapExposureBias( XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias );
XSDK_APIENTRY XSDK_SetExposureBias( XSDK_HANDLE hCamera, long lExposureBias );
XSDK_APIENTRY XSDK_GetExposureBias( XSDK_HANDLE hCamera, long* plExposureBias );

XSDK_APIENTRY XSDK_CapSensitivity( XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity );
XSDK_APIENTRY XSDK_SetSensitivity( XSDK_HANDLE hCamera, long lSensitivity );
XSDK_APIENTRY XSDK_GetSensitivity( XSDK_HANDLE hCamera, long* plSensitivity );

XSDK_APIENTRY XSDK_CapDynamicRange( XSDK_HANDLE hCamera, long* plNumDynamicRange, long* plDynamicRange );
XSDK_APIENTRY XSDK_SetDynamicRange( XSDK_HANDLE hCamera, long lDynamicRange );
XSDK_APIENTRY XSDK_GetDynamicRange( XSDK_HANDLE hCamera, long* plDynamicRange );

XSDK_APIENTRY XSDK_CapMeteringMode( XSDK_HANDLE hCamera, long* plNumMeteringMode, long* plMeteringMode );
XSDK_APIENTRY XSDK_SetMeteringMode( XSDK_HANDLE hCamera, long lMeteringMode );
XSDK_APIENTRY XSDK_GetMeteringMode( XSDK_HANDLE hCamera, long* plMeteringMode);

XSDK_APIENTRY XSDK_CapLensZoomPos( XSDK_HANDLE hCamera, long* plNumZoomPos, long* plZoomPos, long* plFocusLength, long* pl35mmFocusLength );
XSDK_APIENTRY XSDK_SetLensZoomPos( XSDK_HANDLE hCamera, long lZoomPos );
XSDK_APIENTRY XSDK_GetLensZoomPos( XSDK_HANDLE hCamera, long* plZoomPos );

XSDK_APIENTRY XSDK_CapAperture( XSDK_HANDLE hCamera, long lZoomPos, long* plNumAperture, long* plFNumber );
XSDK_APIENTRY XSDK_SetAperture( XSDK_HANDLE hCamera, long lFNumber );
XSDK_APIENTRY XSDK_GetAperture( XSDK_HANDLE hCamera, long* plFNumber);

XSDK_APIENTRY XSDK_CapWBMode( XSDK_HANDLE hCamera, long* plNumWBMode, long* plWBMode );
XSDK_APIENTRY XSDK_SetWBMode( XSDK_HANDLE hCamera, long lWBMode );
XSDK_APIENTRY XSDK_GetWBMode( XSDK_HANDLE hCamera, long* plWBMode );

XSDK_APIENTRY XSDK_CapWBColorTemp( XSDK_HANDLE hCamera, long* plNumWBColorTemp, long* plWBColorTemp );
XSDK_APIENTRY XSDK_SetWBColorTemp( XSDK_HANDLE hCamera, long lColorTemp );
XSDK_APIENTRY XSDK_GetWBColorTemp( XSDK_HANDLE hCamera, long* plColorTemp);

XSDK_APIENTRY XSDK_CapMediaRecord( XSDK_HANDLE hCamera, long* plNumMediaRecord, long* plMediaRecord );
XSDK_APIENTRY XSDK_SetMediaRecord( XSDK_HANDLE hCamera, long lMediaRecord );
XSDK_APIENTRY XSDK_GetMediaRecord( XSDK_HANDLE hCamera, long* plMediaRecord );

// Force Mode
XSDK_APIENTRY XSDK_CapForceMode( XSDK_HANDLE hCamera, long* plNumForceMode, long* plForceMode );
XSDK_APIENTRY XSDK_SetForceMode( XSDK_HANDLE hCamera, long lForceMode );

// Drive Mode
XSDK_APIENTRY XSDK_CapDriveMode( XSDK_HANDLE hCamera, long* plNum, long* pFunctionMode );
XSDK_APIENTRY XSDK_SetDriveMode( XSDK_HANDLE hCamera, long functionMode );
XSDK_APIENTRY XSDK_GetDriveMode( XSDK_HANDLE hCamera, long* pFunctionMode );

// Mode
XSDK_APIENTRY XSDK_CapMode( XSDK_HANDLE hCamera, long* plNum, long* plMode );
XSDK_APIENTRY XSDK_SetMode( XSDK_HANDLE hCamera, long lMode );
XSDK_APIENTRY XSDK_GetMode( XSDK_HANDLE hCamera, long* plMode );

// BackupSettings
XSDK_APIENTRY XSDK_SetBackupSettings( XSDK_HANDLE hCamera, long lSize, unsigned char* pBackup);
XSDK_APIENTRY XSDK_GetBackupSettings( XSDK_HANDLE hCamera, long* plSize, unsigned char* pBackup );

// Model Dependent SDK API Call I/F
XSDK_APIENTRY XSDK_CapProp( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
XSDK_APIENTRY XSDK_SetProp( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
XSDK_APIENTRY XSDK_GetProp( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );

#ifdef  __cplusplus
}
#endif  // _cplusplus

#ifndef _WIN32
#pragma export off
#endif

//
// Import function types define
//
// Initialize/Finalize
typedef XSDK_APIENTRY  (*_XSDK_Init)(LIB_HANDLE hLib);
typedef XSDK_APIENTRY  (*_XSDK_Exit)(void);

// Enumeration
typedef XSDK_APIENTRY  (*_XSDK_Detect)(long lInterface, LPSTR pInterface, LPSTR pDeviceName, long* plCount);
typedef XSDK_APIENTRY  (*_XSDK_Append)(long lInterface, LPSTR pInterface, LPSTR pDeviceName, long* plCount, XSDK_CameraList *pCameraList);

// Session management
typedef XSDK_APIENTRY  (*_XSDK_Close)(XSDK_HANDLE hCamera);
typedef XSDK_APIENTRY  (*_XSDK_PowerOFF)(XSDK_HANDLE hCamera);
typedef XSDK_APIENTRY  (*_XSDK_OpenEx)( LPSTR pDevice, XSDK_HANDLE* phCamera, long* plCameraMode, void* pOption );

// Basic functions
typedef XSDK_APIENTRY  (*_XSDK_GetErrorNumber)(XSDK_HANDLE hCamera, long* plAPICode, long* plERRCode);
typedef XSDK_APIENTRY  (*_XSDK_GetVersionString)(LPSTR pVersionString);
typedef XSDK_APIENTRY  (*_XSDK_GetErrorDetails)(XSDK_HANDLE hCamera, long* plERRCode);

// Device Information
typedef XSDK_APIENTRY  (*_XSDK_GetDeviceInfo)(XSDK_HANDLE hCamera,  XSDK_DeviceInformation* pDevInfo);
typedef XSDK_APIENTRY  (*_XSDK_WriteDeviceName)(XSDK_HANDLE hCamera, LPCSTR pDeviceName);
typedef XSDK_APIENTRY  (*_XSDK_GetFirmwareVersion)(XSDK_HANDLE  hCamera, LPSTR pFirmwareVersion);
typedef XSDK_APIENTRY  (*_XSDK_GetLensInfo)(XSDK_HANDLE hCamera, XSDK_LensInformation* pLensInfo);
typedef XSDK_APIENTRY  (*_XSDK_GetLensVersion)(XSDK_HANDLE hCamera, LPSTR pLensVersion);
typedef XSDK_APIENTRY  (*_XSDK_GetDeviceInfoEx)(XSDK_HANDLE hCamera, XSDK_DeviceInformation* pDevInfo, long* plNumAPICode, long* plAPICode);

// Camera Operation Mode
typedef XSDK_APIENTRY  (*_XSDK_CapPriorityMode)(XSDK_HANDLE hCamera, long* plNumPriorityMode, long* plPriorityMode);
typedef XSDK_APIENTRY  (*_XSDK_SetPriorityMode)(XSDK_HANDLE hCamera, long lPriorityMode);
typedef XSDK_APIENTRY  (*_XSDK_GetPriorityMode)(XSDK_HANDLE hCamera, long*  plPriorityMode);

// Release Control
typedef XSDK_APIENTRY  (*_XSDK_CapRelease)(XSDK_HANDLE hCamera, long* plNumReleaseMode, long* plReleaseMode);
typedef XSDK_APIENTRY  (*_XSDK_Release)(XSDK_HANDLE hCamera, long lReleaseMode, long* plShotOpt, long* plAfStatus);
typedef XSDK_APIENTRY  (*_XSDK_CapReleaseEx)(XSDK_HANDLE hCamera, long* plNumReleaseMode, long* plReleaseMode);
typedef XSDK_APIENTRY  (*_XSDK_ReleaseEx)(XSDK_HANDLE hCamera, long lReleaseMode, long* plShotOpt, long* plAfStatus);
typedef XSDK_APIENTRY  (*_XSDK_CapReleaseStatus)(XSDK_HANDLE hCamera, long* plNum, long* plReleaseStatus);
typedef XSDK_APIENTRY  (*_XSDK_GetReleaseStatus)(XSDK_HANDLE hCamera, long* plReleaseStatus);

// Image acquisition
typedef XSDK_APIENTRY  (*_XSDK_GetBufferCapacity)(XSDK_HANDLE hCamera, long* plShootFrameNum, long* plTotalFrameNum);
typedef XSDK_APIENTRY  (*_XSDK_ReadImageInfo)(XSDK_HANDLE hCamera,  XSDK_ImageInformation* pImgInfo);
typedef XSDK_APIENTRY  (*_XSDK_ReadPreview)(XSDK_HANDLE hCamera, unsigned char* pData, unsigned long lDataSize);
typedef XSDK_APIENTRY  (*_XSDK_ReadImage)(XSDK_HANDLE hCamera,  unsigned char* pData, unsigned long lDataSize);
typedef XSDK_APIENTRY  (*_XSDK_DeleteImage)(XSDK_HANDLE hCamera);

// Exposure control
typedef XSDK_APIENTRY  (*_XSDK_CapAEMode)(XSDK_HANDLE hCamera, long* plNumAEMode, long* plAEMode);
typedef XSDK_APIENTRY  (*_XSDK_SetAEMode)(XSDK_HANDLE hCamera,  long lAEMode);
typedef XSDK_APIENTRY  (*_XSDK_GetAEMode)(XSDK_HANDLE hCamera,  long* plAEMode);
typedef XSDK_APIENTRY  (*_XSDK_CapShutterSpeed)(XSDK_HANDLE hCamera, long* plNumShutterSpeed, long* plShutterSpeed, long* plBulbCapable);
typedef XSDK_APIENTRY  (*_XSDK_SetShutterSpeed)(XSDK_HANDLE hCamera, long lShutterSpeed, long lBulb);
typedef XSDK_APIENTRY  (*_XSDK_GetShutterSpeed)(XSDK_HANDLE hCamera, long* plShutterSpeed, long* plBulb);
typedef XSDK_APIENTRY  (*_XSDK_CapExposureBias)(XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias);
typedef XSDK_APIENTRY  (*_XSDK_SetExposureBias)(XSDK_HANDLE hCamera, long lExposureBias);
typedef XSDK_APIENTRY  (*_XSDK_GetExposureBias)(XSDK_HANDLE hCamera, long*  plExposureBias);
typedef XSDK_APIENTRY  (*_XSDK_CapSensitivity)(XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity);
typedef XSDK_APIENTRY  (*_XSDK_SetSensitivity)(XSDK_HANDLE  hCamera, long lSensitivity);
typedef XSDK_APIENTRY  (*_XSDK_GetSensitivity)(XSDK_HANDLE  hCamera, long* plSensitivity);
typedef XSDK_APIENTRY  (*_XSDK_CapDynamicRange)(XSDK_HANDLE hCamera, long* plNumDynamicRange, long* plDynamicRange);
typedef XSDK_APIENTRY  (*_XSDK_SetDynamicRange)(XSDK_HANDLE hCamera, long lDynamicRange);
typedef XSDK_APIENTRY  (*_XSDK_GetDynamicRange)(XSDK_HANDLE hCamera, long* plDynamicRange);
typedef XSDK_APIENTRY  (*_XSDK_CapMeteringMode)(XSDK_HANDLE hCamera, long* plNumMeteringMode, long* plMeteringMode);
typedef XSDK_APIENTRY  (*_XSDK_SetMeteringMode)(XSDK_HANDLE hCamera, long lMeteringMode);
typedef XSDK_APIENTRY  (*_XSDK_GetMeteringMode)(XSDK_HANDLE hCamera, long*  plMeteringMode);

typedef XSDK_APIENTRY  (*_XSDK_CapLensZoomPos)(XSDK_HANDLE hCamera, long* plNumZoomPos, long* plZoomPos, long* plFocusLength, long* pl35mmFocusLength);
typedef XSDK_APIENTRY  (*_XSDK_SetLensZoomPos)(XSDK_HANDLE hCamera, long lZoomPos);
typedef XSDK_APIENTRY  (*_XSDK_GetLensZoomPos)(XSDK_HANDLE hCamera, long* plZoomPos);
typedef XSDK_APIENTRY  (*_XSDK_CapAperture)(XSDK_HANDLE hCamera, long lZoomPos, long* plNumAperture, long* plFNumber);
typedef XSDK_APIENTRY  (*_XSDK_SetAperture)(XSDK_HANDLE hCamera, long lFNumber);
typedef XSDK_APIENTRY  (*_XSDK_GetAperture)(XSDK_HANDLE hCamera, long*  plFNumber);


typedef XSDK_APIENTRY  (*_XSDK_CapWBMode)(XSDK_HANDLE hCamera, long* plNumWBMode, long* plWBMode);
typedef XSDK_APIENTRY  (*_XSDK_SetWBMode)(XSDK_HANDLE hCamera, long lWBMode);
typedef XSDK_APIENTRY  (*_XSDK_GetWBMode)(XSDK_HANDLE hCamera, long* plWBMode);
typedef XSDK_APIENTRY  (*_XSDK_CapWBColorTemp)(XSDK_HANDLE hCamera, long* plNumWBColorTemp, long* plWBColorTemp);
typedef XSDK_APIENTRY  (*_XSDK_SetWBColorTemp)(XSDK_HANDLE hCamera, long lColorTemp);
typedef XSDK_APIENTRY  (*_XSDK_GetWBColorTemp)(XSDK_HANDLE hCamera, long* plColorTemp);

// Media control
typedef XSDK_APIENTRY  (*_XSDK_CapMediaRecord)(XSDK_HANDLE hCamera, long* plNumMediaRecord, long* plMediaRecord);
typedef XSDK_APIENTRY  (*_XSDK_SetMediaRecord)(XSDK_HANDLE hCamera, long lMediaRecord);
typedef XSDK_APIENTRY  (*_XSDK_GetMediaRecord)(XSDK_HANDLE hCamera, long* plMediaRecord);

// Force Mode
typedef XSDK_APIENTRY  (*_XSDK_CapForceMode)(XSDK_HANDLE hCamera, long* plNumForceMode, long* plForceMode);
typedef XSDK_APIENTRY  (*_XSDK_SetForceMode)(XSDK_HANDLE hCamera, long lForceMode);

// Drive Mode
typedef XSDK_APIENTRY  (*_XSDK_CapDriveMode)( XSDK_HANDLE hCamera, long* plNum, long* pFunctionMode );
typedef XSDK_APIENTRY  (*_XSDK_SetDriveMode)( XSDK_HANDLE hCamera, long functionMode );
typedef XSDK_APIENTRY  (*_XSDK_GetDriveMode)( XSDK_HANDLE hCamera, long* pFunctionMode );

// Mode
typedef XSDK_APIENTRY  (*_XSDK_CapMode)( XSDK_HANDLE hCamera, long* plNum, long* plMode );
typedef XSDK_APIENTRY  (*_XSDK_SetMode)( XSDK_HANDLE hCamera, long lMode );
typedef XSDK_APIENTRY  (*_XSDK_GetMode)( XSDK_HANDLE hCamera, long* plMode );

// BackupSettings
typedef XSDK_APIENTRY  (*_XSDK_SetBackupSettings)( XSDK_HANDLE hCamera, long lSize, unsigned char* pBackup);
typedef XSDK_APIENTRY  (*_XSDK_GetBackupSettings)( XSDK_HANDLE hCamera, long* plSize, unsigned char* pBackup );

// Model Dependent SDK API Call I/F
typedef XSDK_APIENTRY  (*_XSDK_CapProp)( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
typedef XSDK_APIENTRY  (*_XSDK_SetProp)( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );
typedef XSDK_APIENTRY  (*_XSDK_GetProp)( XSDK_HANDLE hCamera, long lAPICode, long lAPIParam, ... );

#endif  // __XAPI_H__