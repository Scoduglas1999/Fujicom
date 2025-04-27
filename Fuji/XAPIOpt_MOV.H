/*
 *  XAPIOpt_MOV.h
 *  
 *    XSDK Shooting SDK Header file
 *
 *    Version 1.10.0.0
 *
 *  Copyright (C) 2014-2021 FUJIFILM Corporation.   
 *
 */

#ifndef __XAPI_OPT_MOV_H__
#define __XAPI_OPT_MOV_H__

//
// Structure defines
//

// Movie AFC Custom
#pragma pack(1)
typedef struct SDK_MOVIE_AFC_CUSTOM {
	long lTracking;
	long lSpeed;
} SDK_MOVIE_AFC_CUSTOM;
#pragma pack()

// Mic Level Indicator
#pragma    pack(1)
typedef    struct SDK_MICLEVEL_INDICATOR{
	long lDSC_L_Peak;
	long lDSC_L_PeakHold;
	long lDSC_R_Peak;
	long lDSC_R_PeakHold;
	long lXLR_1_Peak;
	long lXLR_1_PeakHold;
	long lXLR_2_Peak;
	long lXLR_2_PeakHold;
	long lWarning1;
	long lWarning2;
	long lMicLine;
} SDK_MICLEVEL_INDICATOR;
#pragma pack()

// PlayBack Object Info
#pragma    pack(1)
typedef    struct SDK_PlayBackObjectInformation{
	long lFormat;
	long long llDataSize;
	long lThumbSize;
	char strFileName[32];
	char strcapturedata[20];
	long lImagePixHeight;
	long lImagePixWidth;
	long lImageBitDepth;
	long Duration;
} SDK_PlayBackObjectInformation;
#pragma pack()

// Focus Map Data
#pragma	pack(1)
typedef	struct _SDK_FocusMapData {
	long lDistance;
  long lColorR;
	long lColorG;
	long lColorB;
	long lAlpha;
} SDK_FocusMapData;
#pragma	pack()

// Tracking Af Frame Info
#pragma pack(1)
typedef struct SDK_TrackingAfFrameInfo {
	long lX;
	long lY;
	long lLength_H;
	long lLength_V;
	long lColorR;
	long lColorG;
	long lColorB;
	long lAlpha;
} SDK_TrackingAfFrameInfo;
#pragma pack()

// Histogram Data
#pragma pack(1)
typedef struct SDK_HistogramData {
	long lLuminance;
	long lColorR;
	long lColorG;
	long lColorB;
} SDK_HistogramData;
#pragma pack()

// Movie Transparent Frame Info
#pragma pack(1)
typedef struct{
	long    lX;
	long    lY;
	long    lLength_H;
	long    lLength_V;
	long    lAlpha;
} SDK_MovieTransparentFrameInfo;
#pragma pack()


/////////////////////////////////////////////////////////////////////////////////////////////
//  API Code
enum{
    // Exposure control
    API_CODE_CapMovieHighFrequencyFlickerlessMode	= 0x2066,
    API_CODE_SetMovieHighFrequencyFlickerlessMode	= 0x2067,
    API_CODE_GetMovieHighFrequencyFlickerlessMode	= 0x2068,

    // Lens & Focus control
	API_CODE_CapMovieAFMode							= 0x220C,
	API_CODE_SetMovieAFMode							= 0x220D,
	API_CODE_GetMovieAFMode							= 0x220E,
	API_CODE_CapMovieFocusArea						= 0x2213,
	API_CODE_SetMovieFocusArea						= 0x2214,
	API_CODE_GetMovieFocusArea						= 0x2215,
	API_CODE_GetTrackingAfFrameInfo					= 0x228A,

	// Whitebalance control
	API_CODE_SetMovieCustomWBArea					= 0x2357,
	API_CODE_GetMovieCustomWBArea					= 0x2358,

	// Movie
	API_CODE_CapMovieResolution						= 0x3501,
	API_CODE_SetMovieResolution						= 0x3502,
	API_CODE_GetMovieResolution						= 0x3503,
	API_CODE_CapMovieFrameRate						= 0x3504,
	API_CODE_SetMovieFrameRate						= 0x3505,
	API_CODE_GetMovieFrameRate						= 0x3506,
	API_CODE_CapMovieBitRate						= 0x3507,
	API_CODE_SetMovieBitRate						= 0x3508,
	API_CODE_GetMovieBitRate						= 0x3509,
	API_CODE_CapMovieFileFormat						= 0x350A,
	API_CODE_SetMovieFileFormat						= 0x350B,
	API_CODE_GetMovieFileFormat						= 0x350C,
	API_CODE_CapHighSpeedRecMode					= 0x350D,
	API_CODE_SetHighSpeedRecMode					= 0x350E,
	API_CODE_GetHighSpeedRecMode					= 0x350F,
	API_CODE_CapHighSpeedRecResolution				= 0x3510,
	API_CODE_SetHighSpeedRecResolution				= 0x3511,
	API_CODE_GetHighSpeedRecResolution				= 0x3512,
	API_CODE_CapHighSpeedRecFrameRate				= 0x3513,
	API_CODE_SetHighSpeedRecFrameRate				= 0x3514,
	API_CODE_GetHighSpeedRecFrameRate				= 0x3515,
	API_CODE_CapHighSpeedRecPlayBackFrameRate		= 0x3516,
	API_CODE_SetHighSpeedRecPlayBackFrameRate		= 0x3517,
	API_CODE_GetHighSpeedRecPlayBackFrameRate		= 0x3518,
	API_CODE_CapMovieMediaRecord					= 0x3519,
	API_CODE_SetMovieMediaRecord					= 0x351A,
	API_CODE_GetMovieMediaRecord					= 0x351B,
	API_CODE_CapMovieMediaRecordProRes				= 0x351C,
	API_CODE_SetMovieMediaRecordProRes				= 0x351D,
	API_CODE_GetMovieMediaRecordProRes				= 0x351E,
	API_CODE_CapMovieHDMIOutputRAW					= 0x351F,
	API_CODE_SetMovieHDMIOutputRAW					= 0x3520,
	API_CODE_GetMovieHDMIOutputRAW					= 0x3521,
	API_CODE_CapMovieHDMIOutputRAWResolution		= 0x3522,
	API_CODE_SetMovieHDMIOutputRAWResolution		= 0x3523,
	API_CODE_GetMovieHDMIOutputRAWResolution		= 0x3524,
	API_CODE_CapMovieHDMIOutputRAWFrameRate			= 0x3525,
	API_CODE_SetMovieHDMIOutputRAWFrameRate			= 0x3526,
	API_CODE_GetMovieHDMIOutputRAWFrameRate			= 0x3527,
	API_CODE_CapFlogRecording						= 0x3528,
	API_CODE_SetFlogRecording						= 0x3529,
	API_CODE_GetFlogRecording						= 0x352A,
	API_CODE_CapMovieCropMagnification				= 0x352B,
	API_CODE_SetMovieCropMagnification				= 0x352C,
	API_CODE_GetMovieCropMagnification				= 0x352D,
	API_CODE_CapMovieHDMIOutputInfoDisplay			= 0x352E,
	API_CODE_SetMovieHDMIOutputInfoDisplay			= 0x352F,
	API_CODE_GetMovieHDMIOutputInfoDisplay			= 0x3530,
	API_CODE_CapMovieHDMIRecControl					= 0x3531,
	API_CODE_SetMovieHDMIRecControl					= 0x3532,
	API_CODE_GetMovieHDMIRecControl					= 0x3533,
	API_CODE_CapMovieIsMode							= 0x3534,
	API_CODE_SetMovieIsMode							= 0x3535,
	API_CODE_GetMovieIsMode							= 0x3536,
	API_CODE_CapMovieIsModeBoost					= 0x3537,
	API_CODE_SetMovieIsModeBoost					= 0x3538,
	API_CODE_GetMovieIsModeBoost					= 0x3539,
	API_CODE_CapMovieZebraSetting					= 0x353A,
	API_CODE_SetMovieZebraSetting					= 0x353B,
	API_CODE_GetMovieZebraSetting					= 0x353C,
	API_CODE_CapMovieZebraLevel						= 0x353D,
	API_CODE_SetMovieZebraLevel						= 0x353E,
	API_CODE_GetMovieZebraLevel						= 0x353F,
	API_CODE_CapMovieOptimizedControl				= 0x3540,
	API_CODE_SetMovieOptimizedControl				= 0x3541,
	API_CODE_GetMovieOptimizedControl				= 0x3542,
	API_CODE_CapMovieTallyLight						= 0x3543,
	API_CODE_SetMovieTallyLight						= 0x3544,
	API_CODE_GetMovieTallyLight						= 0x3545,
	API_CODE_GetMediaEjectWarning					= 0x3546,
	API_CODE_CapMovieDataLevelSetting				= 0x3547,
	API_CODE_SetMovieDataLevelSetting				= 0x3548,
	API_CODE_GetMovieDataLevelSetting				= 0x3549,
	API_CODE_GetMovieCropMagnificationValue 		= 0x354A,
	API_CODE_CapMovieFullTimeManual					= 0x354B,
	API_CODE_SetMovieFullTimeManual					= 0x354C,
	API_CODE_GetMovieFullTimeManual					= 0x354D,
	API_CODE_CapMovieDigitalZoom					= 0x354E,
	API_CODE_SetMovieDigitalZoom					= 0x354F,
	API_CODE_GetMovieDigitalZoom					= 0x3550,
	API_CODE_GetMovieDigitalZoomRange				= 0x3551,
	API_CODE_CapMovieImageFormat					= 0x3552,
	API_CODE_SetMovieImageFormat					= 0x3553,
	API_CODE_GetMovieImageFormat					= 0x3554,
	API_CODE_GetHistogramData						= 0x3555,
	API_CODE_GetFocusMapData						= 0x3556,
	API_CODE_CapWaveFormVectorScope					= 0x3557,
	API_CODE_SetWaveFormVectorScope					= 0x3558,
	API_CODE_GetWaveFormVectorScope					= 0x3559,
	API_CODE_GetWaveFormData						= 0x355A,
	API_CODE_GetVectorScopeData						= 0x355B,
	API_CODE_GetParadeData							= 0x355C,
	API_CODE_CapRecFrameIndicator					= 0x355D,
	API_CODE_SetRecFrameIndicator					= 0x355E,
	API_CODE_GetRecFrameIndicator					= 0x355F,
	API_CODE_CapMovieCaptureDelay					= 0x3560,
	API_CODE_SetMovieCaptureDelay					= 0x3561,
	API_CODE_GetMovieCaptureDelay					= 0x3562,
	API_CODE_CapAnamorphicDesqueezeDisplay			= 0x3563,
	API_CODE_SetAnamorphicDesqueezeDisplay			= 0x3564,
	API_CODE_GetAnamorphicDesqueezeDisplay			= 0x3565,
	API_CODE_CapAnamorphicMagnification				= 0x3566,
	API_CODE_SetAnamorphicMagnification				= 0x3567,
    API_CODE_GetAnamorphicMagnification				= 0x3568,
    API_CODE_CapWaveFormSetting						= 0x3569,
    API_CODE_SetWaveFormSetting						= 0x356A,
    API_CODE_GetWaveFormSetting						= 0x356B,
    API_CODE_CapVectorScopeSetting					= 0x356C,
    API_CODE_SetVectorScopeSetting					= 0x356D,
    API_CODE_GetVectorScopeSetting					= 0x356E,
    API_CODE_CapParadeSettingDisplay				= 0x356F,
    API_CODE_SetParadeSettingDisplay				= 0x3570,
    API_CODE_GetParadeSettingDisplay				= 0x3571,
    API_CODE_CapParadeSettingColor					= 0x3572,
    API_CODE_SetParadeSettingColor					= 0x3573,
    API_CODE_GetParadeSettingColor					= 0x3574,
    API_CODE_CapFlogDRangePriority					= 0x3575,
    API_CODE_SetFlogDRangePriority					= 0x3576,
    API_CODE_GetFlogDRangePriority					= 0x3577,
	API_CODE_CapMovieWhiteBalanceTune				= 0x3601,
	API_CODE_SetMovieWhiteBalanceTune				= 0x3602,
	API_CODE_GetMovieWhiteBalanceTune				= 0x3603,
	API_CODE_CapMovieFilmSimulationMode				= 0x3604,
	API_CODE_SetMovieFilmSimulationMode				= 0x3605,
	API_CODE_GetMovieFilmSimulationMode				= 0x3606,
	API_CODE_CapMovieMonochromaticColor				= 0x3607,
	API_CODE_SetMovieMonochromaticColor				= 0x3608,
	API_CODE_GetMovieMonochromaticColor				= 0x3609,
	API_CODE_CapMovieHighLightTone					= 0x360A,
	API_CODE_SetMovieHighLightTone					= 0x360B,
	API_CODE_GetMovieHighLightTone					= 0x360C,
	API_CODE_CapMovieShadowTone						= 0x360D,
	API_CODE_SetMovieShadowTone						= 0x360E,
	API_CODE_GetMovieShadowTone						= 0x360F,
	API_CODE_CapMovieSharpness						= 0x3610,
	API_CODE_SetMovieSharpness						= 0x3611,
	API_CODE_GetMovieSharpness						= 0x3612,
	API_CODE_CapMovieColorMode						= 0x3613,
	API_CODE_SetMovieColorMode						= 0x3614,
	API_CODE_GetMovieColorMode						= 0x3615,
	API_CODE_CapMovieNoiseReduction					= 0x3616,
	API_CODE_SetMovieNoiseReduction					= 0x3617,
	API_CODE_GetMovieNoiseReduction					= 0x3618,
	API_CODE_CapInterFrameNR						= 0x3619,
	API_CODE_SetInterFrameNR						= 0x361A,
	API_CODE_GetInterFrameNR						= 0x361B,
	API_CODE_CapMoviePeripheralLightCorrection		= 0x361C,
	API_CODE_SetMoviePeripheralLightCorrection		= 0x361D,
	API_CODE_GetMoviePeripheralLightCorrection		= 0x361E,
	API_CODE_CapMovieFaceDetectionMode				= 0x361F,
	API_CODE_SetMovieFaceDetectionMode				= 0x3620,
	API_CODE_GetMovieFaceDetectionMode				= 0x3621,
	API_CODE_CapMovieEyeAFMode						= 0x3622,
	API_CODE_SetMovieEyeAFMode						= 0x3623,
	API_CODE_GetMovieEyeAFMode						= 0x3624,
	API_CODE_CapMovieSubjectDetectionMode			= 0x3625,
	API_CODE_SetMovieSubjectDetectionMode			= 0x3626,
	API_CODE_GetMovieSubjectDetectionMode			= 0x3627,
	API_CODE_CapMovieAFCCustom						= 0x362B,
	API_CODE_SetMovieAFCCustom						= 0x362C,
	API_CODE_GetMovieAFCCustom						= 0x362D,
	API_CODE_CapMovieMFAssistMode					= 0x362E,
	API_CODE_SetMovieMFAssistMode					= 0x362F,
	API_CODE_GetMovieMFAssistMode					= 0x3630,
	API_CODE_GetMovieFocusMeter						= 0x3632,
	API_CODE_CapMovieFocusCheckMode					= 0x3633,
	API_CODE_SetMovieFocusCheckMode					= 0x3634,
	API_CODE_GetMovieFocusCheckMode					= 0x3635,
	API_CODE_CapMovieFocusCheckLock					= 0x3636,
	API_CODE_SetMovieFocusCheckLock					= 0x3637,
	API_CODE_GetMovieFocusCheckLock					= 0x3638,
	API_CODE_GetMovieRecordingTime					= 0x3639,
	API_CODE_GetMovieRemainingTime					= 0x363A,
	API_CODE_GetTimeCode							= 0x3801,
	API_CODE_CapTimeCodeDisplay						= 0x3802,
	API_CODE_SetTimeCodeDisplay						= 0x3803,
	API_CODE_GetTimeCodeDisplay						= 0x3804,
	API_CODE_CapTimeCodeStartSetting				= 0x3805,
	API_CODE_SetTimeCodeStartSetting				= 0x3806,
	API_CODE_CapTimeCodeCountUp						= 0x3807,
	API_CODE_SetTimeCodeCountUp						= 0x3808,
	API_CODE_GetTimeCodeCountUp						= 0x3809,
	API_CODE_CapTimeCodeDropFrame					= 0x380A,
	API_CODE_SetTimeCodeDropFrame					= 0x380B,
	API_CODE_GetTimeCodeDropFrame					= 0x380C,
	API_CODE_CapTimeCodeHDMIOutput					= 0x380D,
	API_CODE_SetTimeCodeHDMIOutput					= 0x380E,
	API_CODE_GetTimeCodeHDMIOutput					= 0x380F,
	API_CODE_GetTimeCodeCurrentValue				= 0x3810,
	API_CODE_CapATOMOSAirGluConnection				= 0x3811,
	API_CODE_SetATOMOSAirGluConnection				= 0x3812,
	API_CODE_GetATOMOSAirGluConnection				= 0x3813,
	API_CODE_GetTimeCodeStatus						= 0x3814,
	API_CODE_CapInternalMicLevel					= 0x3901,
	API_CODE_SetInternalMicLevel					= 0x3902,
	API_CODE_GetInternalMicLevel					= 0x3903,
	API_CODE_CapInternalMicLevelManual				= 0x3904,
	API_CODE_SetInternalMicLevelManual				= 0x3905,
	API_CODE_GetInternalMicLevelManual				= 0x3906,
	API_CODE_CapExternalMicLevel					= 0x3907,
	API_CODE_SetExternalMicLevel					= 0x3908,
	API_CODE_GetExternalMicLevel					= 0x3909,
	API_CODE_CapExternalMicLevelManual				= 0x390A,
	API_CODE_SetExternalMicLevelManual				= 0x390B,
	API_CODE_GetExternalMicLevelManual				= 0x390C,
	API_CODE_CapMicLevelLimiter						= 0x390D,
	API_CODE_SetMicLevelLimiter						= 0x390E,
	API_CODE_GetMicLevelLimiter						= 0x390F,
	API_CODE_CapWindFilter							= 0x3910,
	API_CODE_SetWindFilter							= 0x3911,
	API_CODE_GetWindFilter							= 0x3912,
	API_CODE_CapLowCutFilter						= 0x3913,
	API_CODE_SetLowCutFilter						= 0x3914,
	API_CODE_GetLowCutFilter						= 0x3915,
	API_CODE_CapHeadPhonesVolume					= 0x3916,
	API_CODE_SetHeadPhonesVolume					= 0x3917,
	API_CODE_GetHeadPhonesVolume					= 0x3918,
	API_CODE_CapXLRAdapterMicSource					= 0x3919,
	API_CODE_SetXLRAdapterMicSource					= 0x391A,
	API_CODE_GetXLRAdapterMicSource					= 0x391B,
	API_CODE_CapXLRAdapterMoniteringSource			= 0x391C,
	API_CODE_SetXLRAdapterMoniteringSource			= 0x391D,
	API_CODE_GetXLRAdapterMoniteringSource			= 0x391E,
	API_CODE_CapXLRAdapterHDMIOutputSource			= 0x391F,
	API_CODE_SetXLRAdapterHDMIOutputSource			= 0x3920,
	API_CODE_GetXLRAdapterHDMIOutputSource			= 0x3921,
	API_CODE_GetMicLevelIndicator					= 0x3922,
	API_CODE_GetMovieTransparentFrameInfo			= 0x357B,
	API_CODE_CapShortMovieSecond					= 0x357C,
	API_CODE_SetShortMovieSecond					= 0x357D,
	API_CODE_GetShortMovieSecond					= 0x357E,
	API_CODE_CapMoviePortraitEnhancer				= 0x363B,
	API_CODE_SetMoviePortraitEnhancer				= 0x363C,
	API_CODE_GetMoviePortraitEnhancer				= 0x363D,
	API_CODE_CapDirectionalMic						= 0x3923,
	API_CODE_SetDirectionalMic						= 0x3924,
	API_CODE_GetDirectionalMic						= 0x3925,
	API_CODE_CapEnvSoundsNR							= 0x3926,
	API_CODE_SetEnvSoundsNR							= 0x3927,
	API_CODE_GetEnvSoundsNR							= 0x3928,
	API_CODE_GetBodyTemperatureWarning				= 0x4278,
	API_CODE_GetPlayBackMediaInfo					= 0x4279,
	API_CODE_GetPlayBackObjectInfo					= 0x427A,
	API_CODE_GetPlayBackThumb						= 0x427B,
	API_CODE_GetPlayBackPartialObject				= 0x427C,
	API_CODE_CapMovieRecVolume						= 0x427D,
	API_CODE_SetMovieRecVolume						= 0x427E,
	API_CODE_GetMovieRecVolume						= 0x427F,
};


//
// Constants defines
//


// Movie Af Mode
#define	SDK_MOVIE_AF_MULTI							0x0001
#define	SDK_MOVIE_AF_AREA							0x0002
#define SDK_MOVIE_AF_WIDETRACKING					0x0003

// Short Movie Second
#define	SDK_SHORT_MOVIE_SECOND_OFF					0x0001
#define	SDK_SHORT_MOVIE_SECOND_15S					0x0002
#define	SDK_SHORT_MOVIE_SECOND_30S					0x0003
#define	SDK_SHORT_MOVIE_SECOND_60S					0x0004

// Movie Resolution
// High Speed Rec Resolution
#define	SDK_MOVIE_RESOLUTION_6P2K_3_2				0x0001
#define	SDK_MOVIE_RESOLUTION_8K_16_9				0x0002
#define	SDK_MOVIE_RESOLUTION_6K_16_9				0x0003
#define	SDK_MOVIE_RESOLUTION_4KHQ_16_9				0x0004
#define	SDK_MOVIE_RESOLUTION_4K_16_9				0x0005
#define	SDK_MOVIE_RESOLUTION_DCIHQ_17_9				0x0006
#define	SDK_MOVIE_RESOLUTION_DCI_17_9				0x0007
#define	SDK_MOVIE_RESOLUTION_FULLHD_16_9			0x0008
#define	SDK_MOVIE_RESOLUTION_FULLHD_17_9			0x0009
#define	SDK_MOVIE_RESOLUTION_5K_17_9				0x000A
#define	SDK_MOVIE_RESOLUTION_DCI_8K_17_9			0x000B
#define	SDK_MOVIE_RESOLUTION_CINESCO_2P35_1			0x000C
#define	SDK_MOVIE_RESOLUTION_OPENGATE_3_2			0x000D
#define	SDK_MOVIE_RESOLUTION_35MM_16_9				0x000E
#define	SDK_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1		0x000F
#define	SDK_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1		0x0010
#define	SDK_MOVIE_RESOLUTION_FULLFRAME_3_2			0x0011
#define	SDK_MOVIE_RESOLUTION_FULLHD_LP_16_9			0x0012
#define	SDK_MOVIE_RESOLUTION_FULLHD_LP_17_9			0x0013
#define	SDK_MOVIE_RESOLUTION_4K_LP_16_9				0x0014
#define	SDK_MOVIE_RESOLUTION_FULLHD_9_16			0x0015

// Movie FrameRate
// High Speed Rec PlayBack FrameRate
#define	SDK_MOVIE_FRAMERATE_59_94P					0x0001
#define	SDK_MOVIE_FRAMERATE_50P						0x0002
#define	SDK_MOVIE_FRAMERATE_29_97P					0x0003
#define	SDK_MOVIE_FRAMERATE_25P						0x0004
#define	SDK_MOVIE_FRAMERATE_24P						0x0005
#define	SDK_MOVIE_FRAMERATE_23_98P					0x0006

// Movie BitRate
#define	SDK_MOVIE_BITRATE_720MBPS					0x0001
#define	SDK_MOVIE_BITRATE_400MBPS					0x0002
#define	SDK_MOVIE_BITRATE_360MBPS					0x0003
#define	SDK_MOVIE_BITRATE_200MBPS					0x0004
#define	SDK_MOVIE_BITRATE_100MBPS					0x0005
#define	SDK_MOVIE_BITRATE_50MBPS					0x0006
#define	SDK_MOVIE_BITRATE_25MBPS					0x0007
#define	SDK_MOVIE_BITRATE_8MBPS						0x0008

// Movie FileFormat
#define	SDK_MOVIE_FORMAT_H264_ALL_I_MOV				0x0001
#define	SDK_MOVIE_FORMAT_H264_LONGGOP_I_MOV			0x0002
#define	SDK_MOVIE_FORMAT_H264_LONGGOP_MP4			0x0003
#define	SDK_MOVIE_FORMAT_H265_4_2_0_ALL_I			0x0004
#define	SDK_MOVIE_FORMAT_H265_4_2_0_LONGGOP			0x0005
#define	SDK_MOVIE_FORMAT_H265_4_2_2_ALL_I			0x0006
#define	SDK_MOVIE_FORMAT_H265_4_2_2_LONGGOP			0x0007
#define	SDK_MOVIE_FORMAT_PRORESHQ					0x0008
#define	SDK_MOVIE_FORMAT_PRORES						0x0009
#define	SDK_MOVIE_FORMAT_PRORESLT					0x000A

// High Speed RecMode
#define	SDK_HIGHSPEEDREC_OFF						0x0001
#define	SDK_HIGHSPEEDREC_ON							0x0002
#define	SDK_HIGHSPEEDREC_ON_HDMI_ONLY				0x0003

// High Speed Rec FrameRate
#define	SDK_HIGHSPEEDREC_FRAMERATE_240P				0x0001
#define	SDK_HIGHSPEEDREC_FRAMERATE_200P				0x0002
#define	SDK_HIGHSPEEDREC_FRAMERATE_120P				0x0003
#define	SDK_HIGHSPEEDREC_FRAMERATE_100P				0x0004

// Movie Media Record
#define	SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2 0x0001
#define	SDK_MOVIE_MEDIARECORD_SLOT2					0x0002
#define	SDK_MOVIE_MEDIARECORD_SLOT1					0x0003
#define	SDK_MOVIE_MEDIARECORD_BACKUP				0x0004
#define	SDK_MOVIE_MEDIARECORD_SSD					0x0005
#define	SDK_MOVIE_MEDIARECORD_SSD_CF				0x0006
#define	SDK_MOVIE_MEDIARECORD_OFF					0x0007
#define	SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF		0x0008
#define	SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1 0x0008

// Movie Media Record ProRes
#define	SDK_MOVIE_MEDIARECORD_PRORES_OFF			0x0001
#define	SDK_MOVIE_MEDIARECORD_PRORES_H264			0x0002
#define	SDK_MOVIE_MEDIARECORD_PRORES_PROXY			0x0003

// Movie HDMIOutput RAW
#define	SDK_MOVIE_HDMI_OUTPUT_RAW_OFF				0x0001
#define	SDK_MOVIE_HDMI_OUTPUT_RAW_ATOMOS			0x0002
#define	SDK_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC		0x0003

// Movie HDMI Output RAW Resolution
#define	SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K		0x0001
#define	SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K		0x0002
#define	SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_8K			0x0003
#define	SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K		0x0004
#define	SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4K			0x0005
#define	SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K		0x0006

// Movie HDMI Output RAW FrameRate
#define	SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P		0x0001
#define	SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_50P			0x0002
#define	SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P		0x0003
#define	SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_25P			0x0004
#define	SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_24P			0x0005
#define	SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P		0x0006

// Flog Recording
#define	SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM		0x0001
#define	SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG		0x0002
#define	SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2		0x0003
#define	SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG		0x0004
#define	SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2		0x0005
#define	SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM		0x0006
#define	SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM		0x0007
#define	SDK_MOVIERECORD_MEDIA_HLG_HDMI_HLG			0x0008

// MovieIsMode
#define	SDK_MOVIE_IS_MODE_OFF						0x0001
#define	SDK_MOVIE_IS_MODE_ON						0x0002
#define	SDK_MOVIE_IS_MODE_IBIS_OIS					0x0003
#define	SDK_MOVIE_IS_MODE_IBIS_OIS_DIS				0x0004
#define	SDK_MOVIE_IS_MODE_OIS						0x0005
#define	SDK_MOVIE_IS_MODE_OIS_DIS					0x0006

// Movie Zebra Setting
#define	SDK_MOVIE_ZEBRA_SETTING_OFF					0x0001
#define	SDK_MOVIE_ZEBRA_SETTING_RIGHT				0x0002
#define	SDK_MOVIE_ZEBRA_SETTING_LEFT				0x0003

// Movie Tally Light
#define	SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON		0x0001
#define	SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK	0x0002
#define	SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON		0x0003
#define	SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF		0x0004
#define	SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK	0x0005
#define	SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF	0x0006
#define	SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF		0x0007

// Media Eject Warning
#define	SDK_MEDIA_EJECT_WARNING_SLOT1				0x0001
#define	SDK_MEDIA_EJECT_WARNING_SLOT2				0x0002
#define	SDK_MEDIA_EJECT_WARNING_SLOT3				0x0004

// Movie Data Level Setting
#define	SDK_MOVIE_DATA_LEVEL_SETTING_FULL			0x0001
#define	SDK_MOVIE_DATA_LEVEL_SETTING_VIDEO			0x0002

// Movie ImageFormat
#define	SDK_MOVIE_IMAGEFORMAT_GF					0x0001
#define	SDK_MOVIE_IMAGEFORMAT_35MM					0x0002
#define	SDK_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM		0x0003
#define	SDK_MOVIE_IMAGEFORMAT_PREMISTA				0x0004

// WaveForm VectorScope
#define	SDK_WAVEFORM_VECTORSCOPE_OFF				0x0001
#define	SDK_WAVEFORM_VECTORSCOPE_WAVEFORM			0x0002
#define	SDK_WAVEFORM_VECTORSCOPE_PARADE				0x0003
#define	SDK_WAVEFORM_VECTORSCOPE_VECTORSCOPE		0x0004

// WaveForm Setting
#define	SDK_WAVEFORM_SETTING_PATTERN1				0x0001
#define	SDK_WAVEFORM_SETTING_PATTERN2				0x0002
#define	SDK_WAVEFORM_SETTING_PATTERN3				0x0003
#define	SDK_WAVEFORM_SETTING_PATTERN4				0x0004

// VectorScope Setting
#define	SDK_VECTORSCORE_SETTING_PATTERN1			0x0001
#define	SDK_VECTORSCORE_SETTING_PATTERN2			0x0002
#define	SDK_VECTORSCORE_SETTING_PATTERN3			0x0003
#define	SDK_VECTORSCORE_SETTING_PATTERN4			0x0004

// Parade Setting Display
#define	SDK_PARADE_SETTING_DISPLAY_PATTERN1			0x0001
#define	SDK_PARADE_SETTING_DISPLAY_PATTERN2			0x0002
#define	SDK_PARADE_SETTING_DISPLAY_PATTERN3			0x0003
#define	SDK_PARADE_SETTING_DISPLAY_PATTERN4			0x0004

// Parade Setting Color
#define	SDK_PARADE_SETTING_COLOR_RGB				0x0001
#define	SDK_PARADE_SETTING_COLOR_WHITE				0x0002

// Movie FocusMeter(display)
#define	SDK_FOCUSMETER_DISPLAY_OFF					0x0000
#define	SDK_FOCUSMETER_DISPLAY_ON					0x0001

// Movie FocusMeter(color)
#define	SDK_FOCUSMETER_COLOR_WHITE					0x0001
#define	SDK_FOCUSMETER_COLOR_GREEN					0x0002

// TimeCode Start Setting
#define	SDK_TIMECODE_START_SETTING_MANUAL			0x0000
#define	SDK_TIMECODE_START_SETTING_CURRENT			0x0001
#define	SDK_TIMECODE_START_SETTING_RESET			0x0002

// TimeCode CountUp
#define	SDK_TIMECODE_COUNTUP_RECRUN					0x0001
#define	SDK_TIMECODE_COUNTUP_FREERUN				0x0002

// TimeCode Status
#define	SDK_TIMECODE_STATUS_USE_DSC_ALONE			0x0001
#define	SDK_TIMECODE_STATUS_SYNCING					0x0002
#define	SDK_TIMECODE_STATUS_DISCONNECTED			0x0003
#define	SDK_TIMECODE_STATUS_NOT_SYNCED				0x0004
#define	SDK_TIMECODE_STATUS_FRAMERATE_MISMATCH		0x0005

// Internal Mic Level
// External Mic Level
#define	SDK_MIC_LEVEL_OFF							0x0001
#define	SDK_MIC_LEVEL_MANUAL						0x0002
#define	SDK_MIC_LEVEL_AUTO							0x0003

// HeadPhones Volume
#define	SDK_HEADPHONES_VOLUME_0						  0
#define	SDK_HEADPHONES_VOLUME_1						 10
#define	SDK_HEADPHONES_VOLUME_2						 20
#define	SDK_HEADPHONES_VOLUME_3						 30
#define	SDK_HEADPHONES_VOLUME_4						 40
#define	SDK_HEADPHONES_VOLUME_5						 50
#define	SDK_HEADPHONES_VOLUME_6						 60
#define	SDK_HEADPHONES_VOLUME_7						 70
#define	SDK_HEADPHONES_VOLUME_8						 80
#define	SDK_HEADPHONES_VOLUME_9						 90
#define	SDK_HEADPHONES_VOLUME_10					100

// XLR Adapter Mic Source
#define	SDK_XLRADAPTER_MIC_SOURCE_4CH				0x0001
#define	SDK_XLRADAPTER_MIC_SOURCE_2CH				0x0002

// XLR Adapter Monitering Source
#define	SDK_XLRADAPTER_MONITER_SOURCE_XLR			0x0001
#define	SDK_XLRADAPTER_MONITER_SOURCE_CAMERA		0x0002

// XLR Adapter HDMI Output Source
#define	SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR		0x0001
#define	SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA		0x0002

// Directional Mic
#define	SDK_DIRECTIONAL_MIC_AUTO					0x0001
#define	SDK_DIRECTIONAL_MIC_SURROUND				0x0002
#define	SDK_DIRECTIONAL_MIC_FRONT					0x0003
#define	SDK_DIRECTIONAL_MIC_TRACKING				0x0004
#define	SDK_DIRECTIONAL_MIC_BACK					0x0005
#define	SDK_DIRECTIONAL_MIC_FRONT_BACK				0x0006

// Body Temperature Warning
#define	SDK_BODY_TEMPERATURE_WARNING_NONE			0x0001
#define	SDK_BODY_TEMPERATURE_WARNING_YELLOW			0x0002
#define	SDK_BODY_TEMPERATURE_WARNING_RED			0x0003

// Movie Rec Volume
#define	SDK_MOVIE_REC_VOLUME_OFF					0x0001
#define	SDK_MOVIE_REC_VOLUME_1						0x0002
#define	SDK_MOVIE_REC_VOLUME_2						0x0003
#define	SDK_MOVIE_REC_VOLUME_3						0x0004

#endif  // __XAPI_OPT_MOV_H__