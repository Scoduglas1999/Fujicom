/*
 *  XAPIOpt.h
 *  
 *    XSDK Shooting SDK Header file
 *
 *    Version 1.10.0.0
 *
 *  Copyright (C) 2014-2022 FUJIFILM Corporation.   
 *
 */

#ifndef __XAPI_OPT_H__
#define __XAPI_OPT_H__

//
// Structure defines
//

// Focus Area
#pragma pack(1)
typedef struct{
    long    h;
    long    v;
    long    size;
} SDK_FocusArea;
#pragma pack()

// Frame Guideline Grid information
#pragma pack(1)
typedef struct{
	// lGridH[] : Drawing position (in percent of the LCD/EVF height) of horizontal lines (up to 4 lines, 0..3 is available)
	//    The value is used with denominator 1024 inside the camera.
	//    0: not to draw the line
	//    1-1023: position of horizontal lines ( 1: almost top of the LCD/EVF, 1023: almost bottom of the LCD/EVF )
	// lGridV[] : Drawing position (in percent of the LCD/EVF width) of vertical lines (up to 4 lines, 0..3 is available)
	//    The value is used with denominator 1024 inside the camera.
	//    0: not to draw the line
	//    1-1023: position of vertical lines ( 1: almost leftmost of the LCD/EVF, 1023: almost rightmost of the LCD/EVF )
    // lLineWidthH : Line width (in percent of the LCD/EVF height) of the horizontal lines.
	//    The value is used with denominator 1024 inside the camera.
	//    0: not to draw the line
	//    1-127: line width of the horizontal lines.
    // lLineWidthV : Line width (in percent of the LCD/EVF width) of the vertical lines.
	//    The value is used with denominator 1024 inside the camera.
	//    0: not to draw the line
	//    1-127: line width of the horizontal lines.
    // lLineColorIndex : Line color (0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE)
    // lLineAlpha : Transparency ratio of the line ( 0:0%(solid)  1:12.5%  2:25%  3: 37.5% 4: 50%  5: 62.5%  6:75%  7:87.5% )
    long    lGridH[5];
    long    lGridV[5];
    long    lLineWidthH;
    long    lLineWidthV;
    long    lLineColorIndex;
    long    lLineAlpha;
} SDK_FrameGuideGridInfo;
#pragma pack()

// ISO Auto Setting
#pragma pack(1)
typedef struct{
    long    defaultISO;         //default sensitivity
    long    maxISO;             //max. sensitivity
    long    minShutterSpeed;    //min. shutter speed
    char    pName[32];          //(reserved) name of the ISO auto setting set
} SDK_ISOAuto;
#pragma pack()

#pragma pack(1)
typedef struct _SDK_FOCUS_POS_CAP {
    long lSizeFocusPosCap;			// sizeof( SDK_FOCUS_POS_CAP )
    long lStructVer;				// Fixed to 0x00010000 on this version
    long lFocusPlsINF;				// INFINITY position
    long lFocusPlsMOD;				// MOD position
    long lFocusOverSearchPlsINF;	// Over search for INFINITY
    long lFocusOverSearchPlsMOD;	// Over search for MOD
    long lFocusPlsFCSDepthCap;		// DOF. If lFocusPlsFCSDepthCap==0, the information of this structure is invalid.
    long lMinDriveStepMFDriveEndThresh;   // Minimum drive steps
} SDK_FOCUS_POS_CAP, *PSDK_FOCUS_POS_CAP;
#pragma pack()

#pragma pack(1)
typedef struct _SDK_FOLDER_INFO {
    char pFoldernameSuffix[6];		// Folder name suffix. 6 bytes include NULL termination (5 characters).
    long lFolderNumber;				// Folder number.
    long lMaxFrameNumber;			// The maximum frame number in the folder.
    long lStatus;					// Folder Status  Valid:1 ,   Invalid: 0
} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
#pragma pack()

// Crop Area Frame Information
#pragma pack(1)
typedef struct{
    long    lX;
    long    lY;
    long    lLength_H;
    long    lLength_V;
    long    lColorR;
    long    lColorG;
    long    lColorB;
    long    lAlpha;
} SDK_CropAreaFrameInfo;
#pragma pack()

// Face Frame Information
#pragma pack(1)
typedef struct{
    long    lID;
    long    lTime;
    long    lX;
    long    lY;
    long    lLength_H;
    long    lLength_V;
    long    lColorR;
    long    lColorG;
    long    lColorB;
	long    lAlpha;
    long    lType;
	long    lLikeness;
    long    lDisp;
    long    lSelected;
} SDK_FaceFrameInfo;
#pragma pack()

// Custom White Balance Information
#pragma pack(1)
typedef struct{
    long    lX;
    long    lY;
    long    lSize;
    long    lMode;
} SDK_CustomWBArea;
#pragma pack()


// Focus Limiter
#pragma pack(1)
typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
	long	lCurrent;
	long	lDOF_Near;
	long	lDOF_Far;
	long	lPos_A;
	long	lPos_B;
	long	lStatus;
} SDK_FOCUS_LIMITER_INDICATOR;
#pragma pack()

// Focus Limiter Range
#pragma pack(1)
typedef struct _SDK_FOCUS_LIMITER {
	long	lPos_A;
	long	lPos_B;
} SDK_FOCUS_LIMITER;
#pragma pack()

// AFZoneCustom
#pragma	pack(1)
typedef	struct _SDK_AFZoneCustom {
	long	h;
	long	v;
} SDK_AFZoneCustom;
#pragma	pack()

#pragma	pack(1)
typedef	struct _SDK_AFZoneCustomCapablity {
	long				mode;
	SDK_AFZoneCustom	min;
	SDK_AFZoneCustom	max;
} SDK_AFZoneCustomCapablity;
#pragma	pack()

/////////////////////////////////////////////////////////////////////////////////////////////
//  API Code
enum{
    // Exposure control
    API_CODE_CapHighFrequencyFlickerlessMode = 0x2063,
    API_CODE_SetHighFrequencyFlickerlessMode = 0x2064,
    API_CODE_GetHighFrequencyFlickerlessMode = 0x2065,

    // Shooting condition setting
    API_CODE_SetImageSize               = 0x2101,
    API_CODE_GetImageSize               = 0x2102,
    API_CODE_SetSharpness               = 0x2103,
    API_CODE_GetSharpness               = 0x2104,
    API_CODE_SetColorMode               = 0x2105,
    API_CODE_GetColorMode               = 0x2106,

    API_CODE_SetFilmSimulationMode      = 0x2121,
    API_CODE_GetFilmSimulationMode      = 0x2122,
    API_CODE_SetColorSpace              = 0x2127,
    API_CODE_GetColorSpace              = 0x2128,
    API_CODE_SetImageQuality            = 0x2129,
    API_CODE_GetImageQuality            = 0x2130,
    API_CODE_SetNoiseReduction          = 0x2131,
    API_CODE_GetNoiseReduction          = 0x2132,
    API_CODE_SetFaceDetectionMode       = 0x2135,
    API_CODE_GetFaceDetectionMode       = 0x2136,
    API_CODE_SetMacroMode               = 0x2139,
    API_CODE_GetMacroMode               = 0x2140,
    API_CODE_SetHighLightTone           = 0x2141,
    API_CODE_GetHighLightTone           = 0x2142,
    API_CODE_SetShadowTone              = 0x2143,
    API_CODE_GetShadowTone              = 0x2144,
    API_CODE_SetLongExposureNR          = 0x2145,
    API_CODE_GetLongExposureNR          = 0x2146,
    API_CODE_SetFullTimeManualFocus     = 0x2148,
    API_CODE_GetFullTimeManualFocus     = 0x2149,
	API_CODE_SetRAWCompression			= 0x2150,
	API_CODE_GetRAWCompression			= 0x2151,
	API_CODE_SetGrainEffect				= 0x2152,
	API_CODE_GetGrainEffect				= 0x2153,
	API_CODE_SetShadowing				= 0x2154,
	API_CODE_GetShadowing				= 0x2155,
	API_CODE_SetWideDynamicRange		= 0x2156,
	API_CODE_GetWideDynamicRange		= 0x2157,
	API_CODE_SetBlackImageTone		    = 0x2158,
	API_CODE_GetBlackImageTone  		= 0x2159,
	API_CODE_SetRAWOutputDepth			= 0x2160,
	API_CODE_GetRAWOutputDepth			= 0x2161,
	API_CODE_SetSmoothSkinEffect		= 0x2162,
	API_CODE_GetSmoothSkinEffect		= 0x2163,
	API_CODE_GetDetectedFaceFrame		= 0x2166,
	API_CODE_SetDetectedFaceFrame		= 0x2167,

	API_CODE_SetColorChromeBlue			= 0x2168,
	API_CODE_GetColorChromeBlue			= 0x2169,
	API_CODE_SetMonochromaticColor		= 0x216A,
	API_CODE_GetMonochromaticColor		= 0x216B,

	API_CODE_SetClarityMode				= 0x216C,
	API_CODE_GetClarityMode				= 0x216D,
	API_CODE_GetCommandDialStatus		= 0x216E,

	API_CODE_CapImageSize				= 0x2180,
	API_CODE_CapSharpness				= 0x2181,
	API_CODE_CapColorMode				= 0x2182,
	API_CODE_CapFilmSimulationMode		= 0x2183,
	API_CODE_CapColorSpace				= 0x2184,
	API_CODE_CapImageQuality			= 0x2185,
	API_CODE_CapNoiseReduction			= 0x2186,
	API_CODE_CapFaceDetectionMode		= 0x2187,
	API_CODE_CapHighLightTone			= 0x2188,
	API_CODE_CapShadowTone				= 0x2189,
	API_CODE_CapLongExposureNR			= 0x218A,
	API_CODE_CapCustomSettingAutoUpdate	= 0x218B,
	API_CODE_SetCustomSettingAutoUpdate	= 0x218C,
	API_CODE_GetCustomSettingAutoUpdate	= 0x218D,
	API_CODE_CapFullTimeManualFocus		= 0x218E,
	API_CODE_CapRAWCompression			= 0x218F,
	API_CODE_CapGrainEffect				= 0x2190,
	API_CODE_CapShadowing				= 0x2191,
	API_CODE_CapWideDynamicRange		= 0x2192,
	API_CODE_CapRAWOutputDepth			= 0x2193,
	API_CODE_CapSmoothSkinEffect		= 0x2194,
	API_CODE_CapColorChromeBlue			= 0x2195,
	API_CODE_CapMonochromaticColor		= 0x2196,
	API_CODE_CapClarityMode				= 0x2197,
    API_CODE_CapImageFormat             = 0x219D,
    API_CODE_SetImageFormat             = 0x219E,
    API_CODE_GetImageFormat             = 0x219F,
	API_CODE_CapPortraitEnhancer		= 0x21A0,
	API_CODE_SetPortraitEnhancer		= 0x21A1,
	API_CODE_GetPortraitEnhancer		= 0x21A2,

    // Lens & Focus control
    API_CODE_SetFocusMode               = 0x2201,
    API_CODE_GetFocusMode               = 0x2202,
    API_CODE_SetAFMode                  = 0x2203,
    API_CODE_GetAFMode                  = 0x2204,
    API_CODE_SetFocusArea               = 0x2205,
    API_CODE_GetFocusArea               = 0x2206,
	API_CODE_SetFocusPos				= 0x2207,
	API_CODE_GetFocusPos				= 0x2208,
	API_CODE_CapFocusMode				= 0x2209,
	API_CODE_GetAFStatus				= 0x220A,
    API_CODE_SetShutterPriorityMode     = 0x2217,
    API_CODE_GetShutterPriorityMode     = 0x2218,
    API_CODE_SetInstantAFMode           = 0x2219,
    API_CODE_GetInstantAFMode           = 0x2220,
    API_CODE_SetPreAFMode               = 0x2221,
    API_CODE_GetPreAFMode               = 0x2222,
    API_CODE_SetAFIlluminator           = 0x2223,
    API_CODE_GetAFIlluminator           = 0x2224,
    API_CODE_SetLensISSwitch            = 0x2225,
    API_CODE_GetLensISSwitch            = 0x2226,
    API_CODE_SetISMode                  = 0x2227,
    API_CODE_GetISMode                  = 0x2228,
    API_CODE_SetLMOMode                 = 0x2229,
    API_CODE_GetLMOMode                 = 0x2230,
	API_CODE_GetTNumber                 = 0x2233,

	API_CODE_CapAFMode                  = 0x2234,
	API_CODE_CapFocusArea               = 0x2235,
	API_CODE_CapAFStatus                = 0x2236,
	API_CODE_CapShutterPriorityMode     = 0x2237,
	API_CODE_CapInstantAFMode           = 0x2238,
	API_CODE_CapPreAFMode               = 0x2239,
	API_CODE_CapAFIlluminator           = 0x223A,
	API_CODE_CapISMode                  = 0x223B,
	API_CODE_CapLMOMode                 = 0x223C,
	API_CODE_CapEyeAFMode               = 0x223D,
	API_CODE_CapFocusPoints             = 0x223E,
	API_CODE_CapMFAssistMode            = 0x223F,
	API_CODE_CapFocusCheckMode          = 0x2240,
	API_CODE_CapInterlockAEAFArea       = 0x2241,
	API_CODE_CapCropMode                = 0x2242,
	API_CODE_CapFocusLimiterPos         = 0x2243,
	API_CODE_CapFocusLimiterMode        = 0x2244,
	API_CODE_CapSubjectDetectionMode    = 0x2245,
	API_CODE_SetSubjectDetectionMode	= 0x2246,
	API_CODE_GetSubjectDetectionMode	= 0x2247,

	API_CODE_SetEyeAFMode				= 0x2255,
	API_CODE_GetEyeAFMode				= 0x2256,
	API_CODE_SetFocusPoints				= 0x2257,
	API_CODE_GetFocusPoints				= 0x2258,
	API_CODE_CapFocusPos				= 0x2259,
	API_CODE_CapLensISSwitch			= 0x2260,
	API_CODE_SetMFAssistMode			= 0x2261,
	API_CODE_GetMFAssistMode			= 0x2262,
	API_CODE_SetFocusCheckMode			= 0x2263,
	API_CODE_GetFocusCheckMode			= 0x2264,
	API_CODE_SetInterlockAEAFArea		= 0x2265,
	API_CODE_GetInterlockAEAFArea		= 0x2266,
	API_CODE_SetCropMode				= 0x2267,
	API_CODE_GetCropMode				= 0x2268,
	API_CODE_GetCropAreaFrameInfo		= 0x2269,

	API_CODE_SetFocusLimiterPos			= 0x226A,
	API_CODE_GetFocusLimiterIndicator	= 0x226B,
	API_CODE_GetFocusLimiterRange		= 0x226C,
	API_CODE_SetFocusLimiterMode		= 0x226D,
	API_CODE_GetFocusLimiterMode		= 0x226E,
	API_CODE_CapCropZoom				= 0x226F,
	API_CODE_SetCropZoom				= 0x2270,
	API_CODE_GetCropZoom				= 0x2271,
	API_CODE_CapZoomOperation			= 0x2272,
	API_CODE_SetZoomOperation			= 0x2273,
	API_CODE_CapFocusOperation			= 0x2274,
	API_CODE_SetFocusOperation			= 0x2275,
	API_CODE_CapZoomSpeed				= 0x2279,
	API_CODE_SetZoomSpeed				= 0x227A,
	API_CODE_GetZoomSpeed				= 0x227B,
	API_CODE_CapFocusSpeed				= 0x227C,
	API_CODE_SetFocusSpeed				= 0x227D,
	API_CODE_GetFocusSpeed				= 0x227E,
	API_CODE_GetTiltShiftLensStatus		= 0x227F,
	API_CODE_CapAFZoneCustom			= 0x2287,
	API_CODE_SetAFZoneCustom			= 0x2288,
	API_CODE_GetAFZoneCustom			= 0x2289,

    // Whitebalance control
    API_CODE_SetWhiteBalanceMode        = 0x2301,
    API_CODE_GetWhiteBalanceMode        = 0x2302,
    API_CODE_SetWhiteBalanceTune        = 0x2304,
    API_CODE_GetWhiteBalanceTune        = 0x2305,
    API_CODE_CapWhiteBalanceTune        = 0x2324,
    API_CODE_SetCustomWBArea            = 0x2353,
    API_CODE_GetCustomWBArea            = 0x2354,

    // Shoot
    API_CODE_SetCaptureDelay            = 0x3021,
    API_CODE_GetCaptureDelay            = 0x3022,
    API_CODE_CapCaptureDelay            = 0x3025,

    // Live view control
    API_CODE_StartLiveView              = 0x3301,
    API_CODE_StopLiveView               = 0x3302,
    API_CODE_SetLiveViewImageQuality    = 0x3323,
    API_CODE_GetLiveViewImageQuality    = 0x3324,
    API_CODE_SetLiveViewImageSize       = 0x3325,
    API_CODE_GetLiveViewImageSize       = 0x3326,
	API_CODE_SetThroughImageZoom		= 0x3327,
	API_CODE_GetThroughImageZoom		= 0x3328,
    API_CODE_CapLiveViewImageQuality    = 0x3329,
    API_CODE_CapLiveViewImageSize       = 0x332A,
	API_CODE_CapThroughImageZoom		= 0x332B,
	API_CODE_CapLiveViewStatus          = 0x332C,
	API_CODE_GetLiveViewStatus          = 0x332D,
	API_CODE_CapLiveViewMode			= 0x332E,
	API_CODE_SetLiveViewMode			= 0x332F,
	API_CODE_GetLiveViewMode			= 0x3330,
	API_CODE_CapLiveViewImageRatio		= 0x3331,
	API_CODE_SetLiveViewImageRatio		= 0x3332,
	API_CODE_GetLiveViewImageRatio		= 0x3333,

    // Utility
    API_CODE_SetDateTime                = 0x4001,
    API_CODE_GetDateTime                = 0x4002,
    API_CODE_SetDateTimeDispFormat      = 0x4003,
    API_CODE_GetDateTimeDispFormat      = 0x4004,
    API_CODE_SetWorldClock              = 0x4005,
    API_CODE_GetWorldClock              = 0x4006,
    API_CODE_SetTimeDifference          = 0x4007,
    API_CODE_GetTimeDifference          = 0x4008,

	API_CODE_CapWorldClock              = 0x4011,
	API_CODE_CapTimeDifference			= 0x4012,
	API_CODE_CapSummerTime              = 0x4013,
	API_CODE_SetSummerTime              = 0x4014,
	API_CODE_GetSummerTime              = 0x4015,
	API_CODE_CapDateTimeDispFormat      = 0x4016,

    API_CODE_ResetSetting               = 0x4020,
    API_CODE_SetSilentMode              = 0x4021,
    API_CODE_GetSilentMode              = 0x4022,
    API_CODE_SetBeep                    = 0x4025,
    API_CODE_GetBeep                    = 0x4026,
    API_CODE_CapResetSetting            = 0x4029,

    API_CODE_SetFunctionLock            = 0x4039,
    API_CODE_GetFunctionLock            = 0x4040,
    API_CODE_SetFunctionLockCategory    = 0x4041,
    API_CODE_GetFunctionLockCategory    = 0x4042,
    API_CODE_SetComment                 = 0x4043,
    API_CODE_GetComment                 = 0x4044,
    API_CODE_SetCopyright               = 0x4045,
    API_CODE_GetCopyright               = 0x4046,
    API_CODE_SetFilenamePrefix          = 0x4047,
    API_CODE_GetFilenamePrefix          = 0x4048,
    API_CODE_CheckBatteryInfo           = 0x4055,
    API_CODE_SetFrameNumberSequence     = 0x4058,
    API_CODE_GetFrameNumberSequence     = 0x4059,
    API_CODE_SetUSBMode                 = 0x4062,
    API_CODE_GetUSBMode                 = 0x4063,
    API_CODE_FormatMemoryCard           = 0x4064,
    API_CODE_SDK_SetMediaRecord         = 0x4066,
    API_CODE_SDK_GetMediaRecord         = 0x4067,
    API_CODE_GetMediaCapacity           = 0x4068,
    API_CODE_GetMediaStatus             = 0x4070,
    API_CODE_SetFoldernameSuffix        = 0x4074,
    API_CODE_GetFoldernameSuffix        = 0x4075,
    API_CODE_GetFoldernameList          = 0x4076,

    API_CODE_CapFunctionLock            = 0x4077,
    API_CODE_CapFunctionLockCategory    = 0x4078,
    API_CODE_CapFormatMemoryCard        = 0x4079,
    API_CODE_CapPixelShiftSettings      = 0x407A,
    API_CODE_SetPixelShiftSettings      = 0x407B,
    API_CODE_GetPixelShiftSettings      = 0x407C,
    API_CODE_SetArtist                  = 0x407D,
    API_CODE_GetArtist                  = 0x407E,
    API_CODE_CapFrameNumberSequence     = 0x407F,

    API_CODE_GetShutterCount            = 0x4101,
    API_CODE_SetSensorCleanTiming       = 0x4108,
    API_CODE_GetSensorCleanTiming       = 0x4109,
    API_CODE_GetShutterCountEx          = 0x4113,

    API_CODE_CapUSBPowerSupplyCommunication = 0x4117,
    API_CODE_SetUSBPowerSupplyCommunication = 0x4118,
    API_CODE_GetUSBPowerSupplyCommunication = 0x4119,
    API_CODE_CapAutoPowerOffSetting     = 0x411A,
    API_CODE_SetAutoPowerOffSetting     = 0x411B,
    API_CODE_GetAutoPowerOffSetting     = 0x411C,

    API_CODE_SetPreviewTime             = 0x4201,
    API_CODE_GetPreviewTime             = 0x4202,
    API_CODE_SetEVFDispAutoRotate       = 0x4203,
    API_CODE_GetEVFDispAutoRotate       = 0x4204,
    API_CODE_SetExposurePreview         = 0x4205,
    API_CODE_GetExposurePreview         = 0x4206,
    API_CODE_SetDispBrightness          = 0x4207,
    API_CODE_GetDispBrightness          = 0x4208,
    API_CODE_SetFrameGuideMode          = 0x4209,
    API_CODE_GetFrameGuideMode          = 0x4210,
    API_CODE_SetFrameGuideGridInfo      = 0x4211,
    API_CODE_GetFrameGuideGridInfo      = 0x4212,
    API_CODE_SetAutoImageRotation       = 0x4213,
    API_CODE_GetAutoImageRotation       = 0x4214,
    API_CODE_SetFocusScaleUnit          = 0x4215,
    API_CODE_GetFocusScaleUnit          = 0x4216,
    API_CODE_SetCustomDispInfo          = 0x4217,
    API_CODE_GetCustomDispInfo          = 0x4218,
    API_CODE_SetViewMode                = 0x4219,
    API_CODE_GetViewMode                = 0x4220,
    API_CODE_SetDispInfoMode            = 0x4221,
    API_CODE_GetDispInfoMode            = 0x4222,
    API_CODE_SetDispChroma              = 0x4227,
    API_CODE_GetDispChroma              = 0x4228,
    API_CODE_SetCustomAutoPowerOff      = 0x4229,
    API_CODE_GetCustomAutoPowerOff      = 0x4230,
    API_CODE_SetCustomStudioPowerSave   = 0x4231,
    API_CODE_GetCustomStudioPowerSave   = 0x4232,

    API_CODE_CapExposurePreview         = 0x4233,
    API_CODE_CapFrameGuideMode          = 0x4234,
    API_CODE_CapFocusScaleUnit          = 0x4235,
    API_CODE_CapViewMode                = 0x4236,
    API_CODE_CapCustomAutoPowerOff      = 0x4237,
    API_CODE_CapCustomStudioPowerSave   = 0x4238,
    API_CODE_CapLockButtonMode          = 0x4239,
    API_CODE_CapCustomDispInfo          = 0x423A,

    API_CODE_SetFunctionButton          = 0x4241,
    API_CODE_GetFunctionButton          = 0x4242,
    API_CODE_SetISODialHn               = 0x4243,
    API_CODE_GetISODialHn               = 0x4244,
    API_CODE_SetLockButtonMode          = 0x4245,
    API_CODE_GetLockButtonMode          = 0x4246,
    API_CODE_SetAFLockMode              = 0x4247,
    API_CODE_GetAFLockMode              = 0x4248,
    API_CODE_SetMicJackMode             = 0x4249,
    API_CODE_GetMicJackMode             = 0x4250,
    API_CODE_SetAeAfLockKeyAssign       = 0x4251,
    API_CODE_GetAeAfLockKeyAssign       = 0x4252,
    API_CODE_SetCrossKeyAssign          = 0x4253,
    API_CODE_GetCrossKeyAssign          = 0x4254,
    API_CODE_SetPerformanceSettings		= 0x4262,
	API_CODE_GetPerformanceSettings		= 0x4263,
	API_CODE_SetMicLineSetting			= 0x4264,
	API_CODE_GetMicLineSetting			= 0x4265,
	API_CODE_CapPerformanceSettings		= 0x4266,
	API_CODE_CapMicLineSetting			= 0x4267,
	API_CODE_CapFanSetting              = 0x4268,
	API_CODE_SetFanSetting              = 0x4269,
	API_CODE_GetFanSetting              = 0x426A,
	API_CODE_CapElectronicLevelSetting  = 0x426E,
	API_CODE_SetElectronicLevelSetting  = 0x426F,
	API_CODE_GetElectronicLevelSetting  = 0x4270,
	API_CODE_CapApertureUnit            = 0x4271,
	API_CODE_SetApertureUnit            = 0x4272,
	API_CODE_GetApertureUnit            = 0x4273,
};

// Still Image Size
#define SDK_IMAGESIZE_S_3_2                 1
#define SDK_IMAGESIZE_S_16_9                2
#define SDK_IMAGESIZE_S_1_1                 3
#define SDK_IMAGESIZE_M_3_2                 4
#define SDK_IMAGESIZE_M_16_9                5
#define SDK_IMAGESIZE_M_1_1                 6
#define SDK_IMAGESIZE_L_3_2                 7
#define SDK_IMAGESIZE_L_16_9                8
#define SDK_IMAGESIZE_L_1_1                 9
#define	SDK_IMAGESIZE_S_4_3                 10
#define	SDK_IMAGESIZE_S_65_24               11
#define	SDK_IMAGESIZE_S_5_4                 12
#define	SDK_IMAGESIZE_S_7_6                 13
#define	SDK_IMAGESIZE_L_4_3                 14
#define	SDK_IMAGESIZE_L_65_24               15
#define	SDK_IMAGESIZE_L_5_4                 16
#define	SDK_IMAGESIZE_L_7_6                 17
#define	SDK_IMAGESIZE_M_4_3                 18
#define	SDK_IMAGESIZE_M_65_24               19
#define	SDK_IMAGESIZE_M_5_4                 20
#define	SDK_IMAGESIZE_M_7_6                 21

// Still Image Quality
#define SDK_IMAGEQUALITY_RAW                0x0001      //  RAW
#define SDK_IMAGEQUALITY_FINE               0x0002      //  JPEG Fine
#define SDK_IMAGEQUALITY_NORMAL             0x0003      //  JPEG Normal
#define SDK_IMAGEQUALITY_RAW_FINE           0x0004      //  RAW + JPEG Fine
#define SDK_IMAGEQUALITY_RAW_NORMAL         0x0005      //  RAW + JPEG Normal
#define	SDK_IMAGEQUALITY_SUPERFINE          0x0006      //  JPEG Super Fine
#define SDK_IMAGEQUALITY_RAW_SUPERFINE      0x0007      //  RAW + JPEG Super Fine

// Image Format
#define SDK_IMAGEFORMAT_JPEG                0x0007
#define SDK_IMAGEFORMAT_HEIF                0x0012

// RAW Image Quality
#define SDK_RAWOUTPUTDEPTH_14BIT            0x0001
#define SDK_RAWOUTPUTDEPTH_16BIT            0x0002

// LiveView Mode
#define SDK_LIVEVIEW_MODE1                  0x0001
#define SDK_LIVEVIEW_MODE2                  0x0002

// LiveView Ratio
#define SDK_LIVEVIEW_RATIO_FIXED            0x0001
#define SDK_LIVEVIEW_RATIO_VARIABLE         0x0002

// LiveView Image Quality
#define SDK_LIVEVIEW_QUALITY_FINE           0x0001      //  Fine
#define SDK_LIVEVIEW_QUALITY_NORMAL         0x0002      //  Normal
#define SDK_LIVEVIEW_QUALITY_BASIC          0x0003      //  Basic
#define SDK_LIVE_QUALITY_FINE           SDK_LIVEVIEW_QUALITY_FINE      //  Fine
#define SDK_LIVE_QUALITY_NORMAL         SDK_LIVEVIEW_QUALITY_NORMAL      //  Normal
#define SDK_LIVE_QUALITY_BASIC          SDK_LIVEVIEW_QUALITY_BASIC      //  Basic

// LiveView Image Size
#define SDK_LIVEVIEW_SIZE_L                 0x0001      //  L(1280)
#define SDK_LIVEVIEW_SIZE_M                 0x0002      //  M(800)
#define SDK_LIVEVIEW_SIZE_S                 0x0003      //  S(640)
#define SDK_LIVE_SIZE_L                 SDK_LIVEVIEW_SIZE_L      //  L(1280)
#define SDK_LIVE_SIZE_M                 SDK_LIVEVIEW_SIZE_M      //  M(800)
#define SDK_LIVE_SIZE_S                 SDK_LIVEVIEW_SIZE_S      //  S(640)
#define	SDK_LIVE_SIZE_1024				SDK_LIVE_SIZE_L		//	L(1280)
#define	SDK_LIVE_SIZE_640				SDK_LIVE_SIZE_M		//	M(800)
#define	SDK_LIVE_SIZE_320				SDK_LIVE_SIZE_S		//	S(640)

// Through Image Zoom
#define	SDK_THROUGH_ZOOM_10					0x0001		//	x1.0
#define	SDK_THROUGH_ZOOM_25					0x0002		//	x2.5 at single AF position
#define	SDK_THROUGH_ZOOM_60					0x0003		//	x6.0 at single AF position
#define	SDK_THROUGH_ZOOM_40					0x0004		//	x4.0 at single AF position
#define	SDK_THROUGH_ZOOM_80					0x0005		//	x8.0 at single AF position
#define	SDK_THROUGH_ZOOM_160				0x0006		//	x16.0 at single AF position
#define	SDK_THROUGH_ZOOM_20					0x0007		//	x2.0 at single AF position
#define	SDK_THROUGH_ZOOM_33					0x0008		//	x3.3 at single AF position
#define	SDK_THROUGH_ZOOM_66					0x0009		//	x6.6 at single AF position
#define	SDK_THROUGH_ZOOM_131				0x000A		//	x13.1 at single AF position
#define	SDK_THROUGH_ZOOM_240				0x000B		//	x24.0 at single AF position
#define	SDK_THROUGH_ZOOM_197				0x000C		//	x19.7 at single AF position
#define	SDK_THROUGH_ZOOM_83					0x000D		//	x8.3 at single AF position
#define	SDK_THROUGH_ZOOM_170				0x000E		//	x17.0 at single AF position
#define	SDK_THROUGH_ZOOM_68					0x000F		//	x6.8 at single AF position
#define	SDK_THROUGH_ZOOM_140				0x0010		//	x14.0 at single AF position
#define	SDK_THROUGH_ZOOM_120				0x0011		//	x12.0 at single AF position

// D Range
#define	SDK_DRANGE_AUTO						 0xFFFF	//	AUTO
#define	SDK_DRANGE_100						 100	//	100%
#define	SDK_DRANGE_200						 200	//	200%
#define	SDK_DRANGE_400						 400	//	400%
#define	SDK_DRANGE_800						 800	//	800%

// Color Space
#define SDK_COLORSPACE_sRGB                 0x0001  //  sRGB
#define SDK_COLORSPACE_AdobeRGB             0x0002  //  AdobeRGB

// White balance
#define SDK_WB_AUTO                         0x0002
#define	SDK_WB_AUTO_WHITE_PRIORITY			0x8020
#define	SDK_WB_AUTO_AMBIENCE_PRIORITY		0x8021
#define SDK_WB_DAYLIGHT                     0x0004
#define SDK_WB_INCANDESCENT                 0x0006
#define SDK_WB_UNDER_WATER                  0x0008
#define SDK_WB_FLUORESCENT1                 0x8001
#define SDK_WB_FLUORESCENT2                 0x8002
#define SDK_WB_FLUORESCENT3                 0x8003
#define SDK_WB_SHADE                        0x8006
#define SDK_WB_COLORTEMP                    0x8007
#define SDK_WB_CUSTOM1                      0x8008
#define SDK_WB_CUSTOM2                      0x8009
#define SDK_WB_CUSTOM3                      0x800A
#define SDK_WB_CUSTOM4                      0x800B
#define SDK_WB_CUSTOM5                      0x800C

// White balance Color Temptune
#define SDK_WB_COLORTEMP_2500              2500
#define SDK_WB_COLORTEMP_2550              2550
#define SDK_WB_COLORTEMP_2650              2650
#define SDK_WB_COLORTEMP_2700              2700
#define SDK_WB_COLORTEMP_2800              2800
#define SDK_WB_COLORTEMP_2850              2850
#define SDK_WB_COLORTEMP_2950              2950
#define SDK_WB_COLORTEMP_3000              3000
#define SDK_WB_COLORTEMP_3100              3100
#define SDK_WB_COLORTEMP_3200              3200
#define SDK_WB_COLORTEMP_3300              3300
#define SDK_WB_COLORTEMP_3400              3400
#define SDK_WB_COLORTEMP_3600              3600
#define SDK_WB_COLORTEMP_3700              3700
#define SDK_WB_COLORTEMP_3800              3800
#define SDK_WB_COLORTEMP_4000              4000
#define SDK_WB_COLORTEMP_4200              4200
#define SDK_WB_COLORTEMP_4300              4300
#define SDK_WB_COLORTEMP_4500              4500
#define SDK_WB_COLORTEMP_4800              4800
#define SDK_WB_COLORTEMP_5000              5000
#define SDK_WB_COLORTEMP_5300              5300
#define SDK_WB_COLORTEMP_5600              5600
#define SDK_WB_COLORTEMP_5900              5900
#define SDK_WB_COLORTEMP_6300              6300
#define SDK_WB_COLORTEMP_6700              6700
#define SDK_WB_COLORTEMP_7100              7100
#define SDK_WB_COLORTEMP_7700              7700
#define SDK_WB_COLORTEMP_8300              8300
#define SDK_WB_COLORTEMP_9100              9100
#define SDK_WB_COLORTEMP_10000            10000
#define SDK_WB_COLORTEMP_CURRENT              0

// White balance Shift
#define SDK_WB_R_SHIFT_MIN                  -9
#define SDK_WB_R_SHIFT_MAX                   9
#define SDK_WB_B_SHIFT_MIN                  -9
#define SDK_WB_B_SHIFT_MAX                   9

// Custom White balance Information
#define SDK_CUSTOM_WB_MODE_LIVEVIEW         1
#define SDK_CUSTOM_WB_MODE_PLAY             2

// Film	Simulation
#define	SDK_FILMSIMULATION_PROVIA			1
#define	SDK_FILMSIMULATION_STD				SDK_FILMSIMULATION_PROVIA
#define	SDK_FILMSIMULATION_VELVIA			2
#define	SDK_FILMSIMULATION_ASTIA			3
#define	SDK_FILMSIMULATION_NEGHI			4
#define	SDK_FILMSIMULATION_NEGSTD			5
#define	SDK_FILMSIMULATION_MONOCHRO			6
#define	SDK_FILMSIMULATION_MONOCHRO_Y		7
#define	SDK_FILMSIMULATION_MONOCHRO_R		8
#define	SDK_FILMSIMULATION_MONOCHRO_G		9
#define	SDK_FILMSIMULATION_SEPIA			10
#define	SDK_FILMSIMULATION_CLASSIC_CHROME	11
#define	SDK_FILMSIMULATION_ACROS				0x000C
#define	SDK_FILMSIMULATION_ACROS_Y				0x000D
#define	SDK_FILMSIMULATION_ACROS_R				0x000E
#define	SDK_FILMSIMULATION_ACROS_G				0x000F
#define	SDK_FILMSIMULATION_ETERNA				0x0010
#define	SDK_FILMSIMULATION_CLASSICNEG			0x0011
#define	SDK_FILMSIMULATION_BLEACH_BYPASS		0x0012
#define	SDK_FILMSIMULATION_NOSTALGICNEG			0x0013
#define	SDK_FILMSIMULATION_REALA_ACE			0x0014
#define	SDK_FILMSIMULATION_AUTO					0x8000

// Color mode
#define SDK_COLOR_HIGH                       20     // HIGH
#define SDK_COLOR_MEDIUM_HIGH                10     // MEDIUM HIGH
#define SDK_COLOR_STANDARD                   0      // MID
#define SDK_COLOR_MEDIUM_LOW                -10     // MEDIUM LOW
#define SDK_COLOR_LOW                       -20     // LOW

// Color mode
#define	SDK_COLOR_P4						 40
#define	SDK_COLOR_P3						 30
#define	SDK_COLOR_P2						 SDK_COLOR_HIGH			// HIGH
#define	SDK_COLOR_P1						 SDK_COLOR_MEDIUM_HIGH	// MEDIUM HIGH
#define	SDK_COLOR_0							 SDK_COLOR_STANDARD		// MID
#define	SDK_COLOR_M1						 SDK_COLOR_MEDIUM_LOW	// MEDIUM LOW
#define	SDK_COLOR_M2						 SDK_COLOR_LOW			// LOW
#define	SDK_COLOR_M3						-30
#define	SDK_COLOR_M4						-40

// Monochromatic Color WC
#define SDK_MONOCHROMATICCOLOR_WC_P180		 180
#define SDK_MONOCHROMATICCOLOR_WC_P170		 170
#define SDK_MONOCHROMATICCOLOR_WC_P160		 160
#define SDK_MONOCHROMATICCOLOR_WC_P150		 150
#define SDK_MONOCHROMATICCOLOR_WC_P140		 140
#define SDK_MONOCHROMATICCOLOR_WC_P130		 130
#define SDK_MONOCHROMATICCOLOR_WC_P120		 120
#define SDK_MONOCHROMATICCOLOR_WC_P110		 110
#define SDK_MONOCHROMATICCOLOR_WC_P100		 100
#define SDK_MONOCHROMATICCOLOR_WC_P90		  90
#define SDK_MONOCHROMATICCOLOR_WC_P80		  80
#define SDK_MONOCHROMATICCOLOR_WC_P70		  70
#define SDK_MONOCHROMATICCOLOR_WC_P60		  60
#define SDK_MONOCHROMATICCOLOR_WC_P50		  50
#define SDK_MONOCHROMATICCOLOR_WC_P40		  40
#define SDK_MONOCHROMATICCOLOR_WC_P30		  30
#define SDK_MONOCHROMATICCOLOR_WC_P20		  20
#define SDK_MONOCHROMATICCOLOR_WC_P10		  10
#define SDK_MONOCHROMATICCOLOR_WC_0			   0
#define SDK_MONOCHROMATICCOLOR_WC_M10		 -10
#define SDK_MONOCHROMATICCOLOR_WC_M20		 -20
#define SDK_MONOCHROMATICCOLOR_WC_M30		 -30
#define SDK_MONOCHROMATICCOLOR_WC_M40		 -40
#define SDK_MONOCHROMATICCOLOR_WC_M50		 -50
#define SDK_MONOCHROMATICCOLOR_WC_M60		 -60
#define SDK_MONOCHROMATICCOLOR_WC_M70		 -70
#define SDK_MONOCHROMATICCOLOR_WC_M80		 -80
#define SDK_MONOCHROMATICCOLOR_WC_M90		 -90
#define SDK_MONOCHROMATICCOLOR_WC_M100		-100
#define SDK_MONOCHROMATICCOLOR_WC_M110		-110
#define SDK_MONOCHROMATICCOLOR_WC_M120		-120
#define SDK_MONOCHROMATICCOLOR_WC_M130		-130
#define SDK_MONOCHROMATICCOLOR_WC_M140		-140
#define SDK_MONOCHROMATICCOLOR_WC_M150		-150
#define SDK_MONOCHROMATICCOLOR_WC_M160		-160
#define SDK_MONOCHROMATICCOLOR_WC_M170		-170
#define SDK_MONOCHROMATICCOLOR_WC_M180		-180

// Monochromatic Color Red Green
#define SDK_MONOCHROMATICCOLOR_RG_P180		 180
#define SDK_MONOCHROMATICCOLOR_RG_P170		 170
#define SDK_MONOCHROMATICCOLOR_RG_P160		 160
#define SDK_MONOCHROMATICCOLOR_RG_P150		 150
#define SDK_MONOCHROMATICCOLOR_RG_P140		 140
#define SDK_MONOCHROMATICCOLOR_RG_P130		 130
#define SDK_MONOCHROMATICCOLOR_RG_P120		 120
#define SDK_MONOCHROMATICCOLOR_RG_P110		 110
#define SDK_MONOCHROMATICCOLOR_RG_P100		 100
#define SDK_MONOCHROMATICCOLOR_RG_P90		  90
#define SDK_MONOCHROMATICCOLOR_RG_P80		  80
#define SDK_MONOCHROMATICCOLOR_RG_P70		  70
#define SDK_MONOCHROMATICCOLOR_RG_P60		  60
#define SDK_MONOCHROMATICCOLOR_RG_P50		  50
#define SDK_MONOCHROMATICCOLOR_RG_P40		  40
#define SDK_MONOCHROMATICCOLOR_RG_P30		  30
#define SDK_MONOCHROMATICCOLOR_RG_P20		  20
#define SDK_MONOCHROMATICCOLOR_RG_P10		  10
#define SDK_MONOCHROMATICCOLOR_RG_0			   0
#define SDK_MONOCHROMATICCOLOR_RG_M10		 -10
#define SDK_MONOCHROMATICCOLOR_RG_M20		 -20
#define SDK_MONOCHROMATICCOLOR_RG_M30		 -30
#define SDK_MONOCHROMATICCOLOR_RG_M40		 -40
#define SDK_MONOCHROMATICCOLOR_RG_M50		 -50
#define SDK_MONOCHROMATICCOLOR_RG_M60		 -60
#define SDK_MONOCHROMATICCOLOR_RG_M70		 -70
#define SDK_MONOCHROMATICCOLOR_RG_M80		 -80
#define SDK_MONOCHROMATICCOLOR_RG_M90		 -90
#define SDK_MONOCHROMATICCOLOR_RG_M100		-100
#define SDK_MONOCHROMATICCOLOR_RG_M110		-110
#define SDK_MONOCHROMATICCOLOR_RG_M120		-120
#define SDK_MONOCHROMATICCOLOR_RG_M130		-130
#define SDK_MONOCHROMATICCOLOR_RG_M140		-140
#define SDK_MONOCHROMATICCOLOR_RG_M150		-150
#define SDK_MONOCHROMATICCOLOR_RG_M160		-160
#define SDK_MONOCHROMATICCOLOR_RG_M170		-170
#define SDK_MONOCHROMATICCOLOR_RG_M180		-180

// Sharpness
#define SDK_SHARPNESSTYPE_HARD               20     // HARD
#define SDK_SHARPNESSTYPE_MEDIUM_HARD        10     // MEDIUM HARD
#define SDK_SHARPNESSTYPE_STANDARD            0     // STANDARD
#define SDK_SHARPNESSTYPE_MEDIUM_SOFT       -10     // MEDIUM SOFT
#define SDK_SHARPNESSTYPE_SOFT              -20     // SOFT

// Sharpness
#define	SDK_SHARPNESS_P4					 40								// EXTRA HARD
#define	SDK_SHARPNESS_P3					 30								// SUPER HARD
#define	SDK_SHARPNESS_P2					 SDK_SHARPNESSTYPE_HARD			// HARD
#define	SDK_SHARPNESS_P1					 SDK_SHARPNESSTYPE_MEDIUM_HARD	// MEDIUM HARD
#define	SDK_SHARPNESS_0						 SDK_SHARPNESSTYPE_STANDARD		// STANDARD
#define	SDK_SHARPNESS_M1					 SDK_SHARPNESSTYPE_MEDIUM_SOFT	// MEDIUM SOFT
#define	SDK_SHARPNESS_M2					 SDK_SHARPNESSTYPE_SOFT			// SOFT
#define	SDK_SHARPNESS_M3				    -30								// SUPER SOFT
#define	SDK_SHARPNESS_M4				    -40								// EXTRA SOFT

// HIGH LIGHT TONE
#define SDK_HIGHLIGHT_TONE_HARD              20     // HARD
#define SDK_HIGHLIGHT_TONE_MEDIUM_HARD       10     // MEDIUM HARD
#define SDK_HIGHLIGHT_TONE_STANDARD           0     // STANDARD
#define SDK_HIGHLIGHT_TONE_MEDIUM_SOFT      -10     // MEDIUM SOFT
#define SDK_HIGHLIGHT_TONE_SOFT             -20     // SOFT

// HIGH	LIGHT TONE
#define SDK_HIGHLIGHT_TONE_P4				 40									// EXTRA HARD
#define SDK_HIGHLIGHT_TONE_P3_5				 35									//
#define SDK_HIGHLIGHT_TONE_P3				 30									// SUPER HARD
#define SDK_HIGHLIGHT_TONE_P2_5				 25									//
#define SDK_HIGHLIGHT_TONE_P2				 SDK_HIGHLIGHT_TONE_HARD			// HARD
#define SDK_HIGHLIGHT_TONE_P1_5				 15									//
#define SDK_HIGHLIGHT_TONE_P1				 SDK_HIGHLIGHT_TONE_MEDIUM_HARD		// MEDIUM HARD
#define SDK_HIGHLIGHT_TONE_P0_5				 5									//
#define SDK_HIGHLIGHT_TONE_0				 SDK_HIGHLIGHT_TONE_STANDARD		// STANDARD
#define SDK_HIGHLIGHT_TONE_M0_5				-5									//
#define SDK_HIGHLIGHT_TONE_M1				 SDK_HIGHLIGHT_TONE_MEDIUM_SOFT		// MEDIUM SOFT
#define SDK_HIGHLIGHT_TONE_M1_5				-15									//
#define SDK_HIGHLIGHT_TONE_M2				 SDK_HIGHLIGHT_TONE_SOFT			// SOFT

// SHADOW TONE
#define SDK_SHADOW_TONE_HARD                 20     // HARD
#define SDK_SHADOW_TONE_MEDIUM_HARD          10     // MEDIUM HARD
#define SDK_SHADOW_TONE_STANDARD              0     // STD
#define SDK_SHADOW_TONE_MEDIUM_SOFT         -10     // MEDIUM SOFT
#define SDK_SHADOW_TONE_SOFT                -20     // SOFT

// SHADOW TONE
#define	SDK_SHADOW_TONE_P4					 40								// EXTRA HARD
#define	SDK_SHADOW_TONE_P3_5				 35								//
#define	SDK_SHADOW_TONE_P3					 30								// SUPER HARD
#define	SDK_SHADOW_TONE_P2_5				 25								//
#define	SDK_SHADOW_TONE_P2					 SDK_SHADOW_TONE_HARD			// HARD
#define	SDK_SHADOW_TONE_P1_5				 15								//
#define	SDK_SHADOW_TONE_P1					 SDK_SHADOW_TONE_MEDIUM_HARD	// MEDIUM HARD
#define	SDK_SHADOW_TONE_P0_5				 5								//
#define	SDK_SHADOW_TONE_0					 SDK_SHADOW_TONE_STANDARD		// STD
#define	SDK_SHADOW_TONE_M0_5				-5								//
#define	SDK_SHADOW_TONE_M1					 SDK_SHADOW_TONE_MEDIUM_SOFT	// MEDIUM SOFT
#define	SDK_SHADOW_TONE_M1_5				-15								//
#define	SDK_SHADOW_TONE_M2					 SDK_SHADOW_TONE_SOFT			// SOFT

// Noise Reduction
#define SDK_NOISEREDUCTION_HIGH             0x0000  //  HIGH
#define SDK_NOISEREDUCTION_MEDIUM_HIGH      0x1000  //  MEDIUM HIGH
#define SDK_NOISEREDUCTION_STANDARD         0x2000  //  STANDARD
#define SDK_NOISEREDUCTION_MEDIUM_LOW       0x3000  //  MEDIUM LOW
#define SDK_NOISEREDUCTION_LOW              0x4000  //  LOW

// Noise Reduction
#define	SDK_NOISEREDUCTION_P4				0x5000							//Å@EXTRA HIGH
#define	SDK_NOISEREDUCTION_P3				0x6000							//Å@SUPER HIGH
#define	SDK_NOISEREDUCTION_P2				SDK_NOISEREDUCTION_HIGH			//	HIGH
#define	SDK_NOISEREDUCTION_P1				SDK_NOISEREDUCTION_MEDIUM_HIGH	//	MEDIUM HIGH
#define	SDK_NOISEREDUCTION_0				SDK_NOISEREDUCTION_STANDARD		//	STANDARD
#define	SDK_NOISEREDUCTION_M1				SDK_NOISEREDUCTION_MEDIUM_LOW	//	MEDIUM LOW
#define	SDK_NOISEREDUCTION_M2				SDK_NOISEREDUCTION_LOW			//	LOW
#define	SDK_NOISEREDUCTION_M3				0x7000							//	SUPER LOW
#define	SDK_NOISEREDUCTION_M4				0x8000							//	EXTRA LOW

#define SDK_CUSTOM_SETTING_CUSTOM1          1
#define SDK_CUSTOM_SETTING_CUSTOM2          2
#define SDK_CUSTOM_SETTING_CUSTOM3          3
#define SDK_CUSTOM_SETTING_CUSTOM4          4
#define SDK_CUSTOM_SETTING_CUSTOM5          5
#define SDK_CUSTOM_SETTING_CUSTOM6          6
#define SDK_CUSTOM_SETTING_CUSTOM7          7

// RAW Compression
#define SDK_RAW_COMPRESSION_OFF				0x0001	// Uncompressed
#define SDK_RAW_COMPRESSION_LOSSLESS		0x0002	// Lossless Compression
#define SDK_RAW_COMPRESSION_LOSSY			0x0003	// 

// Grain Effect
#define SDK_GRAIN_EFFECT_OFF				0x0001					// Off
#define SDK_GRAIN_EFFECT_WEAK				0x0002					// Weak
#define SDK_GRAIN_EFFECT_P1					SDK_GRAIN_EFFECT_WEAK
#define SDK_GRAIN_EFFECT_STRONG				0x0003					// Strong
#define SDK_GRAIN_EFFECT_P2					SDK_GRAIN_EFFECT_STRONG
#define SDK_GRAIN_EFFECT_OFF_SMALL			SDK_GRAIN_EFFECT_OFF
#define SDK_GRAIN_EFFECT_WEAK_SMALL			SDK_GRAIN_EFFECT_WEAK
#define SDK_GRAIN_EFFECT_STRONG_SMALL		SDK_GRAIN_EFFECT_STRONG
#define SDK_GRAIN_EFFECT_OFF_LARGE			0x0007
#define SDK_GRAIN_EFFECT_WEAK_LARGE			0x0004
#define SDK_GRAIN_EFFECT_STRONG_LARGE		0x0005

// Clarity Mode
#define SDK_CLARITY_P5						50
#define SDK_CLARITY_P4						40
#define SDK_CLARITY_P3						30
#define SDK_CLARITY_P2						20
#define SDK_CLARITY_P1						10
#define SDK_CLARITY_0						0
#define SDK_CLARITY_M1						-10
#define SDK_CLARITY_M2						-20
#define SDK_CLARITY_M3						-30
#define SDK_CLARITY_M4						-40
#define SDK_CLARITY_M5						-50

// Shadowing
#define SDK_SHADOWING_0						0x0001					// Off
#define SDK_SHADOWING_P1					0x0002					// Weak
#define SDK_SHADOWING_P2					0x0003					// Strong

// ColorChrome Blue
#define SDK_COLORCHROME_BLUE_0				0x0001					// Off
#define SDK_COLORCHROME_BLUE_P1				0x0002					// Weak
#define SDK_COLORCHROME_BLUE_P2				0x0003					// Strong

// Smooth Skin Effect
#define SDK_SMOOTHSKIN_EFFECT_OFF			0x0001					// Off
#define SDK_SMOOTHSKIN_EFFECT_P1			0x0002					// Weak
#define SDK_SMOOTHSKIN_EFFECT_P2			0x0003					// Strong

// Capture Delay(ms)
#define SDK_CAPTUREDELAY_10                   10000
#define SDK_CAPTUREDELAY_2                     2000
#define SDK_CAPTUREDELAY_OFF                      0

// Focus mode
#define SDK_FOCUS_MANUAL                      0x0001
#define SDK_FOCUS_AFS                         0x8001
#define SDK_FOCUS_AFC                         0x8002

// Focus Limiter
#define	SDK_FOCUS_LIMITER_OFF				  0x0001
#define	SDK_FOCUS_LIMITER_FULL				  SDK_FOCUS_LIMITER_OFF
#define	SDK_FOCUS_LIMITER_MOD_MID			  0x0002
#define	SDK_FOCUS_LIMITER_MID_INF			  0x0003

// Focus Limiter Pos
#define	SDK_FOCUS_LIMITER_POS_A					0x0001
#define	SDK_FOCUS_LIMITER_POS_B					0x0002

// Focus Limiter Status
#define	SDK_FOCUS_LIMITER_STATUS_VALID			0x0001
#define	SDK_FOCUS_LIMITER_STATUS_INVALID		0x0000

// Focus Limiter No
#define	SDK_FOCUS_LIMITER_1						0x0002
#define	SDK_FOCUS_LIMITER_2						0x0003
#define	SDK_FOCUS_LIMITER_3						0x0004

// Crop Zoom
#define	SDK_CROP_ZOOM_OFF						0
#define	SDK_CROP_ZOOM_10						100
#define	SDK_CROP_ZOOM_11						110
#define	SDK_CROP_ZOOM_12						120
#define	SDK_CROP_ZOOM_13						130
#define	SDK_CROP_ZOOM_14						140
#define	SDK_CROP_ZOOM_15						150
#define	SDK_CROP_ZOOM_16						160
#define	SDK_CROP_ZOOM_17						170
#define	SDK_CROP_ZOOM_18						180
#define	SDK_CROP_ZOOM_19						190
#define	SDK_CROP_ZOOM_20						200

// Zoom Speed
#define	SDK_LENS_ZOOM_SPEED_1					10
#define	SDK_LENS_ZOOM_SPEED_2					20
#define	SDK_LENS_ZOOM_SPEED_3					30
#define	SDK_LENS_ZOOM_SPEED_4					40
#define	SDK_LENS_ZOOM_SPEED_5					50
#define	SDK_LENS_ZOOM_SPEED_6					60
#define	SDK_LENS_ZOOM_SPEED_7					70
#define	SDK_LENS_ZOOM_SPEED_8					80

// Zoom Operation
#define	SDK_ZOOM_OPERATION_START				0x0001
#define	SDK_ZOOM_OPERATION_STOP					0x0002
#define	SDK_ZOOM_DIRECTION_WIDE					0x0000
#define	SDK_ZOOM_DIRECTION_TELE					0x0001

// Focus Speed
#define	SDK_LENS_FOCUS_SPEED_1					10
#define	SDK_LENS_FOCUS_SPEED_2					20
#define	SDK_LENS_FOCUS_SPEED_3					30
#define	SDK_LENS_FOCUS_SPEED_4					40
#define	SDK_LENS_FOCUS_SPEED_5					50
#define	SDK_LENS_FOCUS_SPEED_6					60
#define	SDK_LENS_FOCUS_SPEED_7					70
#define	SDK_LENS_FOCUS_SPEED_8					80

// Focus Operation
#define	SDK_FOCUS_OPERATION_START				0x0001
#define	SDK_FOCUS_OPERATION_STOP				0x0002
#define	SDK_FOCUS_DIRECTION_NEAR				0x0000
#define	SDK_FOCUS_DIRECTION_FAR					0x0001

// Focus Points
#define	SDK_FOCUS_POINTS_13X7				  0x0001
#define	SDK_FOCUS_POINTS_25X13				  0x0002
#define	SDK_FOCUS_POINTS_13X9				  0x0003
#define	SDK_FOCUS_POINTS_25X17				  0x0004

// AF mode
#define SDK_AF_AREA                     0x8001      // AF Area
#define SDK_AF_ZONE                     0x8002      // AF Zone
#define SDK_AF_WIDETRACKING             0x8003      // Wide/Tracking
#define SDK_AF_MULTI                    0x0002      // Multi-Spot
#define	SDK_AF_SINGLE					SDK_AF_AREA	// AF Single
#define SDK_AF_ALL						0x8004		// AF ALL

// AF Status
#define	SDK_AF_STATUS_OPERATING			0x0001
#define	SDK_AF_STATUS_SUCCESS			0x0002
#define	SDK_AF_STATUS_FAIL				0x0003
#define	SDK_AF_STATUS_NO_OPERATION		0x0004

// Eye AF Mode
#define	SDK_EYE_AF_OFF					0x0001		// Off
#define	SDK_EYE_AF_AUTO					0x0002		// Auto
#define	SDK_EYE_AF_RIGHT_PRIORITY		0x0003		// Right priority
#define SDK_EYE_AF_LEFT_PRIORITY		0x0004		// Left priority

// Face Frame Information(Type)
#define	SDK_FRAMEINFO_FACE				0x0001
#define	SDK_FRAMEINFO_EYE_RIGHT			0x0002
#define	SDK_FRAMEINFO_EYE_LEFT			0x0003

// Face Frame Information(Selected)
#define	SDK_FACEFRAMEINFO_NON			0x0001
#define	SDK_FACEFRAMEINFO_AUTO			0x0002
#define	SDK_FACEFRAMEINFO_MANUAL		0x0003

// MF Assist Mode
#define SDK_MF_ASSIST_STANDARD			0x0001		
#define SDK_MF_ASSIST_SPLIT_BW			0x0002		
#define SDK_MF_ASSIST_SPLIT_COLOR		0x0003
#define SDK_MF_ASSIST_PEAK_WHITE_L		0x0004
#define SDK_MF_ASSIST_PEAK_WHITE_H		0x0005
#define SDK_MF_ASSIST_PEAK_RED_L		0x0006
#define SDK_MF_ASSIST_PEAK_RED_H		0x0007
#define SDK_MF_ASSIST_PEAK_BLUE_L		0x0008
#define SDK_MF_ASSIST_PEAK_BLUE_H		0x0009
#define SDK_MF_ASSIST_PEAK_YELLOW_L		0x000A
#define SDK_MF_ASSIST_PEAK_YELLOW_H		0x000B
#define SDK_MF_ASSIST_MICROPRISM		0x000C
#define SDK_MF_ASSIST_FOCUSMETER		0x1000
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L	0x1004
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H	0x1005
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_L		0x1006
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_H		0x1007
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L	0x1008
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H	0x1009
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L	0x100A
#define SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H	0x100B
#define SDK_MF_ASSIST_FOCUSMAP_BW				0x100C
#define SDK_MF_ASSIST_FOCUSMAP_COLOR			0x100D

// Focus Area
#define SDK_FOCUSAREA_H_MIN             -3
#define SDK_FOCUSAREA_H_MAX              3
#define SDK_FOCUSAREA_V_MIN             -3
#define SDK_FOCUSAREA_V_MAX              3
#define SDK_FOCUSAREA_SIZE_MIN           1
#define SDK_FOCUSAREA_SIZE_MAX           5

// Face Detection mode
#define SDK_FACE_DETECTION_ON           0x0001  // ON
#define SDK_FACE_DETECTION_OFF          0x0002  // OFF

// Macro Mode
#define SDK_MACRO_MODE_OFF              0x0001      // Off
#define SDK_MACRO_MODE                  0x0002      // Macro
#define SDK_MACRO_MODE_ON               SDK_MACRO_MODE

// DRIVE MODE
#define SDK_DRIVE_MODE_S                0x0004      // S
#define SDK_DRIVE_MODE_MOVIE            0x0008      // MOVIE

// USB MODE
#define SDK_USB_PCSHOOTAUTO             0x0001
#define SDK_USB_PCSHOOT                 0x0002
//#define   SDK_USB_PTP                     0x0003

// Frame Number Sequence
#define SDK_FRAMENUMBERSEQUENCE_ON      0x0001
#define SDK_FRAMENUMBERSEQUENCE_OFF     0x0002

// Beep Volume
#define SDK_BEEP_HIGH                   0x0003
#define SDK_BEEP_MID                    0x0002
#define SDK_BEEP_LOW                    0x0001
#define SDK_BEEP_OFF                    0x0000

// Preview Time
#define SDK_PREVIEWTIME_CONTINUOUS      0xFFFF
#define SDK_PREVIEWTIME_1P5SEC          15
#define SDK_PREVIEWTIME_0P5SEC          5
#define SDK_PREVIEWTIME_OFF             0

// VIEW MODE
#define SDK_VIEW_MODE_EYE               0x0001      // Eye Sensor
#define SDK_VIEW_MODE_EVF               0x0002      // EVF Only
#define SDK_VIEW_MODE_LCD               0x0003      // LCD Only
#define SDK_VIEW_MODE_EVF_EYE           0x0004      // EVF Only + Eye Sensor 
#define	SDK_VIEW_MODE_LCDPOSTVIEW		0x0005		// EYE SENSOR+LCD POSTVIEW
#define SDK_VIEW_MODE_OVF               0x0006      // OVF
#define SDK_VIEW_MODE_ERF               0x0007      // ERF
#define SDK_VIEW_MODE_EYESENSOR_ON      0x0008      // Eye Sensor On
#define SDK_VIEW_MODE_EYESENSOR_OFF     0x0009      // Eye Sensor Off

// DISP INFO MODE(LCD)
#define SDK_LCD_DISPINFO_MODE_INFO          0x0001      // Info Display
#define SDK_LCD_DISPINFO_MODE_STD           0x0002      // Standard
#define SDK_LCD_DISPINFO_MODE_OFF           0x0003      // Info Off
#define SDK_LCD_DISPINFO_MODE_CUSTOM        0x0004      // Custom 
#define SDK_LCD_DISPINFO_MODE_DUAL          0x0005      // Dual

// DISP INFO MODE(EVF)
#define SDK_EVF_DISPINFO_MODE_FULL_CUSTOM   0x0001      // Full(Custom)
#define SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM 0x0002      // Normal(Custom)
#define SDK_EVF_DISPINFO_MODE_DUAL          0x0003      // Dual
#define SDK_EVF_DISPINFO_MODE_FULL_OFF      0x0004      // Full(Info OFF) 
#define SDK_EVF_DISPINFO_MODE_NORMAL_OFF    0x0005      // Normal(Info OFF)


// RELEASE/FOCUS PRIORITY
#define SDK_AFPRIORITY_RELEASE          0x0001
#define SDK_AFPRIORITY_FOCUS            0x0002

// INSTANT AF
#define SDK_INSTANT_AF_MODE_AFS         0x0001
#define SDK_INSTANT_AF_MODE_AFC         0x0002

// AE/AF LOCK BUTTON MODE
#define SDK_LOCKBUTTON_MODE_PRESSING    0x0001
#define SDK_LOCKBUTTON_MODE_SWITCH      0x0002

// AF LOCK MODE
#define SDK_AFLOCK_MODE_AF              0x0001
#define SDK_AFLOCK_MODE_AEAF            0x0002

// MIC JACK MODE
#define SDK_MICJACK_MODE_MIC            0x0001
#define SDK_MICJACK_MODE_REMOTE         0x0002

// Micline Setting
#define	SDK_MICLINE_SETTING_MIC			0x0001
#define	SDK_MICLINE_SETTING_LINE		0x0002

// AF/AF LCOK KEY ASSIGN
#define SDK_AEAFLKEY_AE_AF              0x0001
#define SDK_AEAFLKEY_AF_AE              0x0002

// CROSS KEY ASSIGN
#define SDK_CROSSKEY_FOCUSAREA          0x0001
#define SDK_CROSSKEY_FUNCTION           0x0002

// IS MODE
#define SDK_IS_MODE_CONTINUOUS          0x0001
#define SDK_IS_MODE_SHOOT               0x0002
#define SDK_IS_MODE_OFF                 0x0003
#define SDK_IS_MODE_S1_SHOOT            0x0004
#define SDK_IS_MODE_CONTINUOUS_MOTION   0x0005
#define SDK_IS_MODE_SHOOT_MOTION        0x0006

// DATA FORMAT
#define SDK_DATE_FORMAT_YMD             0x0001
#define SDK_DATE_FORMAT_DMY             0x0002
#define SDK_DATE_FORMAT_MDY             0x0003

// TIME DIFFERENCE
#define SDK_TIMEDIFF_HOME               0x0001
#define SDK_TIMEDIFF_LOCAL              0x0002

// Language
#define SDK_LANGUAGE_JA                 0x0000

// ExposurePreview Mode
#define SDK_EXPOSURE_PREVIEW_ME_MWB     1
#define SDK_EXPOSURE_PREVIEW_AE_MWB     2
#define SDK_EXPOSURE_PREVIEW_AE_AWB     3

// LCD BRIGHTNESS
#define SDK_LCDBRIGHTNESS_MIN          -5
#define SDK_LCDBRIGHTNESS_MAX           5

// EVF BRIGHTNESS
#define SDK_EVFBRIGHTNESS_MIN          -2
#define SDK_EVFBRIGHTNESS_MAX           2

// FRAMING GUIDELINE
#define SDK_FRAMEGUIDE_GRID_9           0x0001
#define SDK_FRAMEGUIDE_GRID_24          0x0002
#define	SDK_FRAMEGUIDE_GRID_HD			0x0003

// FRAMING GUIDELINE COLOR
#define SDK_COLORINDEX_BLACK            0x0000
#define SDK_COLORINDEX_BLUE             0x0001
#define SDK_COLORINDEX_GREEN            0x0002
#define SDK_COLORINDEX_CYAN             0x0003
#define SDK_COLORINDEX_RED              0x0004
#define SDK_COLORINDEX_MAGENTA          0x0005
#define SDK_COLORINDEX_YELLOW           0x0006
#define SDK_COLORINDEX_WHITE            0x0007

// FOCUS SCALE UNIT
#define SDK_SCALEUNIT_M                 0x0001
#define SDK_SCALEUNIT_FT                0x0002

// MEDIA RECORD
#define SDK_MEDIAREC_RAWJPEG          0x0001
#define SDK_MEDIAREC_RAW              0x0002
#define SDK_MEDIAREC_JPEG               0x0003
#define SDK_MEDIAREC_OFF                0x0004
//#define SDK_MEDIAREC_ON               SDK_MEDIAREC_RAWJPEG

// Battery Info
#define SDK_POWERCAPACITY_EMPTY         0x0000
#define SDK_POWERCAPACITY_END           0x0001
#define SDK_POWERCAPACITY_PREEND        0x0002
#define SDK_POWERCAPACITY_HALF          0x0003
#define SDK_POWERCAPACITY_FULL          0x0004
#define SDK_POWERCAPACITY_HIGH          0x0005
#define	SDK_POWERCAPACITY_PREEND5		0x0007
#define	SDK_POWERCAPACITY_20			0x0008
#define	SDK_POWERCAPACITY_40			0x0009
#define	SDK_POWERCAPACITY_60			0x000A
#define	SDK_POWERCAPACITY_80			0x000B
#define	SDK_POWERCAPACITY_100			0x000C
#define	SDK_POWERCAPACITY_DC_CHARGE     0x000D
#define SDK_POWERCAPACITY_DC            0x00FF

//SENSOR CLEANING
#define SDK_SENSORCLEANING_NONE         0x0000
#define SDK_SENSORCLEANING_POWERON      0x0001
#define SDK_SENSORCLEANING_POWEROFF     0x0002
#define SDK_SENSORCLEANING_POWERONOFF	0x0003

// Fn Button FUNCTION
#define SDK_FUNCTION_DRV                0x0001
#define SDK_FUNCTION_MACRO              0x0002
#define SDK_FUNCTION_DEPTHPREVIEW       0x0003
#define SDK_FUNCTION_ISOAUTOSETTING     0x0004
#define SDK_FUNCTION_SELFTIMER          0x0005
#define SDK_FUNCTION_IMAGESIZE          0x0006
#define SDK_FUNCTION_IMAGEQUALITY       0x0007
#define SDK_FUNCTION_DRANGE             0x0008
#define SDK_FUNCTION_FILMSIMULATION     0x0009
#define SDK_FUNCTION_WB                 0x000A
#define SDK_FUNCTION_AFMODE             0x000B
#define SDK_FUNCTION_FOCUSAREA          0x000C
#define SDK_FUNCTION_CUSTOMSETTING      0x000D
#define SDK_FUNCTION_FACEDETECT         0x000E
#define SDK_FUNCTION_RAW                0x000F
#define SDK_FUNCTION_APERTURE           0x0010
#define SDK_FUNCTION_WIRELESS           0x0011
#define SDK_FUNCTION_EXPOSURE_PREVIEW   0x0012

// CUSTOM DISP INFO
#define SDK_CUSTOMDISPINFO_FRAMEGUIDE                   0x00000001
#define SDK_CUSTOMDISPINFO_ELECTRONLEVEL                0x00000002
#define SDK_CUSTOMDISPINFO_AFDISTANCE                   0x00000004
#define SDK_CUSTOMDISPINFO_MFDISTANCE                   0x00000008
#define SDK_CUSTOMDISPINFO_HISTOGRAM                    0x00000010
#define SDK_CUSTOMDISPINFO_EXPOSUREPARAM                0x00000020
#define SDK_CUSTOMDISPINFO_EXPOSUREBIAS                 0x00000040
#define SDK_CUSTOMDISPINFO_PHOTOMETRY                   0x00000080
#define SDK_CUSTOMDISPINFO_FLASH                        0x00000100
#define SDK_CUSTOMDISPINFO_WB                           0x00000200
#define SDK_CUSTOMDISPINFO_FILMSIMULATION               0x00000400
#define SDK_CUSTOMDISPINFO_DRANGE                       0x00000800
#define SDK_CUSTOMDISPINFO_FRAMESREMAIN                 0x00001000
#define SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY             0x00002000
#define SDK_CUSTOMDISPINFO_BATTERY                      0x00004000
#define SDK_CUSTOMDISPINFO_FOCUSFRAME                   0x00008000
#define SDK_CUSTOMDISPINFO_SHOOTINGMODE                 0x00010000
#define SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND        0x00020000
#define SDK_CUSTOMDISPINFO_FOCUSMODE                    0x00040000
#define SDK_CUSTOMDISPINFO_SHUTTERTYPE                  0x00080000
#define SDK_CUSTOMDISPINFO_CONTINUOUSMODE               0x00100000
#define SDK_CUSTOMDISPINFO_DUALISMODE                   0x00200000
#define SDK_CUSTOMDISPINFO_MOVIEMODE                    0x00400000
#define SDK_CUSTOMDISPINFO_BLURWARNING                  0x00800000
#define SDK_CUSTOMDISPINFO_LIVEVIEWHIGHT                0x01000000
#define SDK_CUSTOMDISPINFO_EXPOSUREBIASDIGIT            0x02000000
#define SDK_CUSTOMDISPINFO_TOUCHSCREENMODE              0x04000000
#define SDK_CUSTOMDISPINFO_BOOSTMODE                    0x08000000
#define SDK_CUSTOMDISPINFO_IMAGETRANSFERORDER           0x10000000
#define SDK_CUSTOMDISPINFO_MICLEVEL                     0x20000000
#define SDK_CUSTOMDISPINFO_GUIDANCEMESSAGE              0x40000000
#define SDK_CUSTOMDISPINFO_FRAMEOUTLINE                 0x80000000
#define SDK_CUSTOMDISPINFO_35MMFORMAT                   0x00000001
#define SDK_CUSTOMDISPINFO_COOLINGFANSETTING            0x00000002
#define SDK_CUSTOMDISPINFO_DIGITALTELECONV              0x00000004
#define SDK_CUSTOMDISPINFO_DIGITALZOOM                  0x00000008
#define SDK_CUSTOMDISPINFO_FOCUSINDICATOR               0x00000010
#define SDK_CUSTOMDISPINFO_NOCARDWARNING                0x00000020
#define SDK_CUSTOMDISPINFO_DATETIME                     0x00000040
#define SDK_CUSTOMDISPINFO_LENSSHIFT                    0x00000080
#define SDK_CUSTOMDISPINFO_LENSTILT                     0x00000100
#define SDK_CUSTOMDISPINFO_LENSREVOLVING                0x00000200
#define SDK_CUSTOMDISPINFO_SSD                          0x00000400
#define SDK_CUSTOMDISPINFO_VLOGMODE                     0x00000800

// Function Lock
#define SDK_FUNCTIONLOCK_FREE                           0x0001
#define SDK_FUNCTIONLOCK_ALL                            0x0002
#define SDK_FUNCTIONLOCK_CATEGORY                       0x0003
// Function Lock Category1
#define SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            0x00000001
#define SDK_FUNCTIONLOCK_CATEGORY1_APERTURE             0x00000002
#define SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         0x00000004
#define SDK_FUNCTIONLOCK_CATEGORY1_ISO                  0x00000008
#define SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         0x00000010
#define SDK_FUNCTIONLOCK_CATEGORY1_DRV                  0x00000020
#define SDK_FUNCTIONLOCK_CATEGORY1_AEMODE               0x00000040
#define SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON              0x00000080
#define SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH             0x00000100
#define SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         0x00000200
#define SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE             0x00000400
#define SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK             0x00000800
#define SDK_FUNCTIONLOCK_CATEGORY1_AELOCK               0x00001000
#define SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK               0x00002000
#define SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          0x00004000
#define SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC             0x00008000
#define SDK_FUNCTIONLOCK_CATEGORY1_UP                   0x00010000
#define SDK_FUNCTIONLOCK_CATEGORY1_RIGHT                0x00020000
#define SDK_FUNCTIONLOCK_CATEGORY1_LEFT                 0x00040000
#define SDK_FUNCTIONLOCK_CATEGORY1_DOWN                 0x00080000
#define SDK_FUNCTIONLOCK_CATEGORY1_FN1                  0x00100000
#define SDK_FUNCTIONLOCK_CATEGORY1_FN2                  0x00200000
#define SDK_FUNCTIONLOCK_CATEGORY1_AFMODE               0x00400000
#define SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT           0x00800000
#define SDK_FUNCTIONLOCK_CATEGORY1_OTHERQMENU                           0x01000000  //(Reserved)
#define SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         0x02000000
#define SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          0x04000000
#define SDK_FUNCTIONLOCK_CATEGORY1_ERASE                0x08000000
#define SDK_FUNCTIONLOCK_CATEGORY1_DATETIME             0x10000000
#define SDK_FUNCTIONLOCK_CATEGORY1_RESET                0x20000000
#define SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE           0x40000000
#define SDK_FUNCTIONLOCK_CATEGORY1_SOUND                0x80000000
// Function Lock Category2
#define SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP           0x00000001
#define SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC                             0x00000002  //(Reserved)
#define SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE           0x00000004
#define SDK_FUNCTIONLOCK_CATEGORY2_SETUP                0x00000008
#define SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           SDK_FUNCTIONLOCK_CATEGORY2_SETUP
#define SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         0x00000010
#define SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       0x00000020
#define	SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK			0x00000040
#define	SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR	0x00000080
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN3					0x00000100
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN4					0x00000200
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN5					0x00000400
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN10					0x00000800
#define	SDK_FUNCTIONLOCK_CATEGORY2_RDIAL				SDK_FUNCTIONLOCK_CATEGORY2_FN10
#define	SDK_FUNCTIONLOCK_CATEGORY2_AFON					0x00001000
#define	SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE			0x00002000
#define	SDK_FUNCTIONLOCK_CATEGORY2_TFN1					0x00004000
#define	SDK_FUNCTIONLOCK_CATEGORY2_TFN2					0x00008000
#define	SDK_FUNCTIONLOCK_CATEGORY2_TFN3					0x00010000
#define	SDK_FUNCTIONLOCK_CATEGORY2_TFN4					0x00020000
#define	SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP				0x00040000
#define	SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V				0x00080000
#define	SDK_FUNCTIONLOCK_CATEGORY2_AFON_V				0x00100000
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN1_V				0x00200000
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN2_V				0x00400000
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN3_V				0x00800000
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN4_V				0x01000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V				0x02000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_LEVER				0x04000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER	0x08000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_MODEDIAL				0x10000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_FDIAL				0x20000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_FN_DIAL				0x40000000
#define	SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT		0x80000000
// Function Lock Category3
#define	SDK_FUNCTIONLOCK_CATEGORY3_ISOBUTTON			0x00000001
#define	SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE		0x00000002
#define	SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE			0x00000004
#define	SDK_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU		0x00000008
#define	SDK_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE			0x00000010
#define	SDK_FUNCTIONLOCK_CATEGORY3_WBBUTTON				0x00000020
#define	SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING		0x00000040
#define	SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTH			0x00000080
#define	SDK_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT		0x00000100
#define	SDK_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING	0x00000200
#define	SDK_FUNCTIONLOCK_CATEGORY3_FM1					0x00000400
#define	SDK_FUNCTIONLOCK_CATEGORY3_FM2					0x00000800
#define	SDK_FUNCTIONLOCK_CATEGORY3_FM3					0x00001000
#define	SDK_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION 0x00002000
#define	SDK_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP		0x00004000
#define	SDK_FUNCTIONLOCK_CATEGORY3_FN6					0x00008000
#define	SDK_FUNCTIONLOCK_CATEGORY3_FSIM_DIAL			0x00010000
#define	SDK_FUNCTIONLOCK_CATEGORY3_FSIM_DIAL_SETTING	0x00020000

// WideDynamicRange
#define	SDK_WIDEDYNAMICRANGE_0							0x0000
#define	SDK_WIDEDYNAMICRANGE_P1							0x0001
#define	SDK_WIDEDYNAMICRANGE_P2							0x0002
#define	SDK_WIDEDYNAMICRANGE_P3							0x0003
#define	SDK_WIDEDYNAMICRANGE_AUTO						0x8000

//BlackImageTone
#define SDK_BLACKIMAGETONE_P90							90
#define SDK_BLACKIMAGETONE_P80							80
#define SDK_BLACKIMAGETONE_P70							70
#define SDK_BLACKIMAGETONE_P60							60
#define SDK_BLACKIMAGETONE_P50							50
#define SDK_BLACKIMAGETONE_P40							40
#define SDK_BLACKIMAGETONE_P30							30
#define SDK_BLACKIMAGETONE_P20							20
#define SDK_BLACKIMAGETONE_P10							10
#define SDK_BLACKIMAGETONE_0							0
#define SDK_BLACKIMAGETONE_M10							-10
#define SDK_BLACKIMAGETONE_M20							-20
#define SDK_BLACKIMAGETONE_M30							-30
#define SDK_BLACKIMAGETONE_M40							-40
#define SDK_BLACKIMAGETONE_M50							-50
#define SDK_BLACKIMAGETONE_M60							-60
#define SDK_BLACKIMAGETONE_M70							-70
#define SDK_BLACKIMAGETONE_M80							-80
#define SDK_BLACKIMAGETONE_M90							-90

// CropMode
#define	SDK_CROPMODE_OFF								0x0000
#define	SDK_CROPMODE_35MM								0x0001
#define	SDK_CROPMODE_AUTO								0x8001
#define SDK_CROPMODE_SPORTSFINDER_125					0x0002


//Media Size Type
#define SDK_MEDIASIZE_1M                     0
#define SDK_MEDIASIZE_2M                     1
#define SDK_MEDIASIZE_4M                     2
#define SDK_MEDIASIZE_8M                     3
#define SDK_MEDIASIZE_16M                    4
#define SDK_MEDIASIZE_32M                    5
#define SDK_MEDIASIZE_64M                    6
#define SDK_MEDIASIZE_128M                   7
#define SDK_MEDIASIZE_256M                   8
#define SDK_MEDIASIZE_512M                   9
#define SDK_MEDIASIZE_1G                     10
#define SDK_MEDIASIZE_2G                     11
#define SDK_MEDIASIZE_4G                     12
#define SDK_MEDIASIZE_8G                     13
#define SDK_MEDIASIZE_16G                    14
#define SDK_MEDIASIZE_32G                    15
#define SDK_MEDIASIZE_32G_OVER               16

// Media Status
#define SDK_MEDIASTATUS_OK                    0x0001
#define SDK_MEDIASTATUS_WRITEPROTECTED        0x0002
#define SDK_MEDIASTATUS_NOCARD                0x0003
#define SDK_MEDIASTATUS_UNFORMATTED           0x0004
#define SDK_MEDIASTATUS_ERROR                 0x0005
#define SDK_MEDIASTATUS_MAXNO                 0x0006
#define SDK_MEDIASTATUS_FULL                  0x0007
#define SDK_MEDIASTATUS_ACCESSING             0x0008
#define SDK_MEDIASTATUS_INCOMPATIBLE          0x0009

// Shuttercount Type
#define	SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN			0x0001
#define	SDK_SHUTTERCOUNT_TYPE_REARCURTAIN			0x0002
#define	SDK_SHUTTERCOUNT_TYPE_TOTAL					0x0003

// Performance
#define	SDK_PERFORMANCE_NORMAL						0x0001
#define	SDK_PERFORMANCE_ECONOMY						0x0002
#define	SDK_PERFORMANCE_BOOST_LOWLIGHT				0x0003
#define	SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY	0x0004
#define	SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY	0x0005
#define	SDK_PERFORMANCE_BOOST_AFPRIORITY_NORMAL		0x0006
#define SDK_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION	0x0007

// PixelShift Settings
#define	SDK_PIXELSHIFT_INTERVAL_SHORTEST			0
#define	SDK_PIXELSHIFT_INTERVAL_1S					10
#define	SDK_PIXELSHIFT_INTERVAL_2S					20
#define	SDK_PIXELSHIFT_INTERVAL_5S					50
#define	SDK_PIXELSHIFT_INTERVAL_15S					150

// SubjectDetectionMode
#define	SDK_SUBJECT_DETECTION_OFF		0x00000001
#define	SDK_SUBJECT_DETECTION_ANIMAL	0x00000002
#define	SDK_SUBJECT_DETECTION_BIRD		0x00000003
#define	SDK_SUBJECT_DETECTION_CAR		0x00000004
#define	SDK_SUBJECT_DETECTION_BIKE		0x00000005
#define	SDK_SUBJECT_DETECTION_AIRPLANE	0x00000006
#define	SDK_SUBJECT_DETECTION_TRAIN		0x00000007
#define	SDK_SUBJECT_DETECTION_ALL		0x00008000

// FanSetting
#define	SDK_FAN_SETTING_OFF				0x0001
#define	SDK_FAN_SETTING_WEAK			0x0002
#define	SDK_FAN_SETTING_STRONG			0x0003
#define	SDK_FAN_SETTING_AUTO1			0x0004
#define	SDK_FAN_SETTING_AUTO2			0x0005

// ElectronicLevelSetting
#define	SDK_ELECTRONIC_LEVEL_SETTING_OFF	0x0001
#define	SDK_ELECTRONIC_LEVEL_SETTING_2D		0x0002
#define	SDK_ELECTRONIC_LEVEL_SETTING_3D		0x0003

// ApertureUnit
#define	SDK_APERTURE_UNIT_TNUMBER		0x0001
#define	SDK_APERTURE_UNIT_FNUMBER		0x0002

// USBPowerSupplyCommunication
#define	SDK_USB_POWER_SUPPLY_COMMUNICATION_AUTO	0x0001
#define	SDK_USB_POWER_SUPPLY_COMMUNICATION_ON	0x0002
#define	SDK_USB_POWER_SUPPLY_COMMUNICATION_OFF	0x0003

// AutoPowerOffSetting
#define	SDK_AUTOPOWEROFF_5MIN			0x0001
#define	SDK_AUTOPOWEROFF_2MIN			0x0002
#define	SDK_AUTOPOWEROFF_OFF			0x0003
#define	SDK_AUTOPOWEROFF_1MIN			0x0004
#define	SDK_AUTOPOWEROFF_30SEC			0x0005
#define	SDK_AUTOPOWEROFF_15SEC			0x0006

// AFZoneCustom
#define SDK_AF_ZONECUSTOM1				0x0001
#define SDK_AF_ZONECUSTOM2				0x0002
#define SDK_AF_ZONECUSTOM3				0x0003

// PortraitEnhancer
#define SDK_PORTRAIT_ENHANCER_OFF       0x0001
#define SDK_PORTRAIT_ENHANCER_SOFT      0x0002
#define SDK_PORTRAIT_ENHANCER_MEDIUM    0x0003
#define SDK_PORTRAIT_ENHANCER_HARD      0x0004

// ON/OFF Setting
#define SDK_ON                          0x0001
#define SDK_OFF                         0x0002

// setting targets
#define SDK_ITEM_DIRECTION_0            1
#define SDK_ITEM_DIRECTION_90           2
#define SDK_ITEM_DIRECTION_180          3
#define SDK_ITEM_DIRECTION_270          4
#define SDK_ITEM_ISODIAL_H1             1
#define SDK_ITEM_ISODIAL_H2             2
#define SDK_ITEM_VIEWMODE_SHOOT         1
#define SDK_ITEM_VIEWMODE_PLAYBACK      2
#define SDK_ITEM_DISPINFO_LCD           1
#define SDK_ITEM_DISPINFO_EVF           2
#define SDK_ITEM_AFPRIORITY_AFS         1
#define SDK_ITEM_AFPRIORITY_AFC         2
#define SDK_ITEM_RESET_SHOOTMENU        1
#define SDK_ITEM_RESET_SETUP            2
#define SDK_ITEM_RESET_MOVIEMENU        3
#define SDK_ITEM_BRIGHTNESS_LCD         1
#define SDK_ITEM_BRIGHTNESS_EVF         2
#define SDK_ITEM_CHROMA_LCD             1
#define SDK_ITEM_CHROMA_EVF             2
#define SDK_ITEM_FUNCBUTTON_FN1         1
#define SDK_ITEM_FUNCBUTTON_FN2         2
#define SDK_ITEM_FUNCBUTTON_FN3         3
#define SDK_ITEM_FUNCBUTTON_FN4         4
#define SDK_ITEM_FUNCBUTTON_FN5         5
#define SDK_ITEM_FUNCBUTTON_FN6         6
#define SDK_ITEM_FILENAME_sRGB          1
#define SDK_ITEM_FILENAME_AdobeRGB      2
#define SDK_ITEM_MEDIASLOT1				1
#define SDK_ITEM_MEDIASLOT2				2
#define SDK_ITEM_MEDIASLOT3             3
#define SDK_ITEM_HDMIOUTPUT             4
#define SDK_ITEM_DIRECTION_CURRENT      0
#define SDK_ITEM_FOLDERNAME_NOCATEGORY	1
#define SDK_NEW_FOLDER					0
#define SDK_FOLDERNUMBER_NIL			1

#endif  // __XAPI_OPT_H__