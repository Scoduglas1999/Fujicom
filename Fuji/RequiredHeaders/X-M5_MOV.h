///////////////////////////////////////////////////
//  X-M5_MOV.h
//     Version 1.0.0.0
//
//     Copyright (C) 2024 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_MOV_H
#include "XAPIOpt_MOV.h"
#define _XAPIOPT_MOV_H
#endif


//
// Structure defines
//

// Movie AFC Custom
//typedef struct SDK_MOVIE_AFC_CUSTOM {
//	long lTracking;
//	long lSpeed;
//} SDK_MOVIE_AFC_CUSTOM;
typedef  SDK_MOVIE_AFC_CUSTOM     XM5_SDK_MOVIE_AFC_CUSTOM, *PXM5_SDK_MOVIE_AFC_CUSTOM;

// Mic Level Indicator
//typedef    struct SDK_MICLEVEL_INDICATOR{
//	long lDSC_L_Peak;
//	long lDSC_L_PeakHold;
//	long lDSC_R_Peak;
//	long lDSC_R_PeakHold;
//	long lXLR_1_Peak;
//	long lXLR_1_PeakHold;
//	long lXLR_2_Peak;
//	long lXLR_2_PeakHold;
//	long lWarning1;
//	long lWarning2;
//	long lMicLine;
//} SDK_MICLEVEL_INDICATOR;
typedef  SDK_MICLEVEL_INDICATOR     XM5_SDK_MICLEVEL_INDICATOR, *PXM5_SDK_MICLEVEL_INDICATOR;

// PlayBack Object Info
//typedef    struct SDK_PlayBackObjectInformation{
//	long lFormat;
//	long long llDataSize;
//	long lThumbSize;
//	char strFileName[32];
//	char strcapturedata[20];
//	long lImagePixHeight;
//	long lImagePixWidth;
//	long lImageBitDepth;
//	long Duration;
//} SDK_PlayBackObjectInformation;
typedef  SDK_PlayBackObjectInformation     XM5_SDK_PlayBackObjectInformation, *PXM5_SDK_PlayBackObjectInformation;

// Focus Map Data
//typedef	struct _SDK_FocusMapData {
//	long lDistance;
//  long lColorR;
//	long lColorG;
//	long lColorB;
//	long lAlpha;
//} SDK_FocusMapData;
typedef  SDK_FocusMapData     XM5_SDK_FocusMapData, *PXM5_SDK_FocusMapData;

// Tracking Af Frame Info
//typedef struct SDK_TrackingAfFrameInfo {
//	long lX;
//	long lY;
//	long lLength_H;
//	long lLength_V;
//	long lColorR;
//	long lColorG;
//	long lColorB;
//	long lAlpha;
//} SDK_TrackingAfFrameInfo;
typedef  SDK_TrackingAfFrameInfo     XM5_SDK_TrackingAfFrameInfo, *PXM5_SDK_TrackingAfFrameInfo;

// Histogram Data
//typedef struct SDK_HistogramData {
//	long lLuminance;
//	long lColorR;
//	long lColorG;
//	long lColorB;
//} SDK_HistogramData;
typedef  SDK_HistogramData     XM5_SDK_HistogramData, *PXM5_SDK_HistogramData;

// Movie Transparent Frame Info
//typedef struct{
//	long    lX;
//	long    lY;
//	long    lLength_H;
//	long    lLength_V;
//	long    lAlpha;
//} SDK_MovieTransparentFrameInfo;
typedef  SDK_MovieTransparentFrameInfo     XM5_SDK_MovieTransparentFrameInfo, *PXM5_SDK_MovieTransparentFrameInfo;


//
// API_PARAM for X-M5
//
enum {
	XM5_API_PARAM_CapMovieHighFrequencyFlickerlessMode = 2,
	XM5_API_PARAM_SetMovieHighFrequencyFlickerlessMode = 1,
	XM5_API_PARAM_GetMovieHighFrequencyFlickerlessMode = 1,
	XM5_API_PARAM_CapMovieAFMode                       = 2,
	XM5_API_PARAM_SetMovieAFMode                       = 1,
	XM5_API_PARAM_GetMovieAFMode                       = 1,
	XM5_API_PARAM_CapMovieFocusArea                    = 3,
	XM5_API_PARAM_SetMovieFocusArea                    = 2,
	XM5_API_PARAM_GetMovieFocusArea                    = 2,
	XM5_API_PARAM_GetTrackingAfFrameInfo               = 1,
	XM5_API_PARAM_CapMovieResolution                   = 2,
	XM5_API_PARAM_SetMovieResolution                   = 1,
	XM5_API_PARAM_GetMovieResolution                   = 1,
	XM5_API_PARAM_CapMovieFrameRate                    = 2,
	XM5_API_PARAM_SetMovieFrameRate                    = 1,
	XM5_API_PARAM_GetMovieFrameRate                    = 1,
	XM5_API_PARAM_CapMovieBitRate                      = 2,
	XM5_API_PARAM_SetMovieBitRate                      = 1,
	XM5_API_PARAM_GetMovieBitRate                      = 1,
	XM5_API_PARAM_CapMovieFileFormat                   = 2,
	XM5_API_PARAM_SetMovieFileFormat                   = 1,
	XM5_API_PARAM_GetMovieFileFormat                   = 1,
	XM5_API_PARAM_CapHighSpeedRecMode                  = 2,
	XM5_API_PARAM_SetHighSpeedRecMode                  = 1,
	XM5_API_PARAM_GetHighSpeedRecMode                  = 1,
	XM5_API_PARAM_CapHighSpeedRecResolution            = 2,
	XM5_API_PARAM_SetHighSpeedRecResolution            = 1,
	XM5_API_PARAM_GetHighSpeedRecResolution            = 1,
	XM5_API_PARAM_CapHighSpeedRecFrameRate             = 2,
	XM5_API_PARAM_SetHighSpeedRecFrameRate             = 1,
	XM5_API_PARAM_GetHighSpeedRecFrameRate             = 1,
	XM5_API_PARAM_CapHighSpeedRecPlayBackFrameRate     = 2,
	XM5_API_PARAM_SetHighSpeedRecPlayBackFrameRate     = 1,
	XM5_API_PARAM_GetHighSpeedRecPlayBackFrameRate     = 1,
	XM5_API_PARAM_CapMovieMediaRecord                  = 2,
	XM5_API_PARAM_SetMovieMediaRecord                  = 1,
	XM5_API_PARAM_GetMovieMediaRecord                  = 1,
	XM5_API_PARAM_CapMovieMediaRecordProRes            = 2,
	XM5_API_PARAM_SetMovieMediaRecordProRes            = 1,
	XM5_API_PARAM_GetMovieMediaRecordProRes            = 1,
	XM5_API_PARAM_CapMovieHDMIOutputRAW                = 2,
	XM5_API_PARAM_SetMovieHDMIOutputRAW                = 1,
	XM5_API_PARAM_GetMovieHDMIOutputRAW                = 1,
	XM5_API_PARAM_CapMovieHDMIOutputRAWResolution      = 2,
	XM5_API_PARAM_SetMovieHDMIOutputRAWResolution      = 1,
	XM5_API_PARAM_GetMovieHDMIOutputRAWResolution      = 1,
	XM5_API_PARAM_CapMovieHDMIOutputRAWFrameRate       = 2,
	XM5_API_PARAM_SetMovieHDMIOutputRAWFrameRate       = 1,
	XM5_API_PARAM_GetMovieHDMIOutputRAWFrameRate       = 1,
	XM5_API_PARAM_CapFlogRecording                     = 2,
	XM5_API_PARAM_SetFlogRecording                     = 1,
	XM5_API_PARAM_GetFlogRecording                     = 1,
	XM5_API_PARAM_CapMovieCropMagnification            = 2,
	XM5_API_PARAM_SetMovieCropMagnification            = 1,
	XM5_API_PARAM_GetMovieCropMagnification            = 1,
	XM5_API_PARAM_CapMovieHDMIOutputInfoDisplay        = 2,
	XM5_API_PARAM_SetMovieHDMIOutputInfoDisplay        = 1,
	XM5_API_PARAM_GetMovieHDMIOutputInfoDisplay        = 1,
	XM5_API_PARAM_CapMovieHDMIRecControl               = 2,
	XM5_API_PARAM_SetMovieHDMIRecControl               = 1,
	XM5_API_PARAM_GetMovieHDMIRecControl               = 1,
	XM5_API_PARAM_CapMovieIsMode                       = 2,
	XM5_API_PARAM_SetMovieIsMode                       = 1,
	XM5_API_PARAM_GetMovieIsMode                       = 1,
	XM5_API_PARAM_CapMovieIsModeBoost                  = 2,
	XM5_API_PARAM_SetMovieIsModeBoost                  = 1,
	XM5_API_PARAM_GetMovieIsModeBoost                  = 1,
	XM5_API_PARAM_CapMovieZebraSetting                 = 2,
	XM5_API_PARAM_SetMovieZebraSetting                 = 1,
	XM5_API_PARAM_GetMovieZebraSetting                 = 1,
	XM5_API_PARAM_CapMovieZebraLevel                   = 2,
	XM5_API_PARAM_SetMovieZebraLevel                   = 1,
	XM5_API_PARAM_GetMovieZebraLevel                   = 1,
	XM5_API_PARAM_CapMovieOptimizedControl             = 2,
	XM5_API_PARAM_SetMovieOptimizedControl             = 1,
	XM5_API_PARAM_GetMovieOptimizedControl             = 1,
	XM5_API_PARAM_CapMovieTallyLight                   = 2,
	XM5_API_PARAM_SetMovieTallyLight                   = 1,
	XM5_API_PARAM_GetMovieTallyLight                   = 1,
	XM5_API_PARAM_GetMediaEjectWarning                 = 1,
	XM5_API_PARAM_CapMovieDataLevelSetting             = 2,
	XM5_API_PARAM_SetMovieDataLevelSetting             = 1,
	XM5_API_PARAM_GetMovieDataLevelSetting             = 1,
	XM5_API_PARAM_GetMovieCropMagnificationValue       = 1,
	XM5_API_PARAM_CapMovieFullTimeManual               = 2,
	XM5_API_PARAM_SetMovieFullTimeManual               = 1,
	XM5_API_PARAM_GetMovieFullTimeManual               = 1,
	XM5_API_PARAM_CapMovieDigitalZoom                  = 2,
	XM5_API_PARAM_SetMovieDigitalZoom                  = 1,
	XM5_API_PARAM_GetMovieDigitalZoom                  = 1,
	XM5_API_PARAM_GetMovieDigitalZoomRange             = 3,
	XM5_API_PARAM_CapMovieImageFormat                  = 2,
	XM5_API_PARAM_SetMovieImageFormat                  = 1,
	XM5_API_PARAM_GetMovieImageFormat                  = 1,
	XM5_API_PARAM_GetHistogramData                     = 2,
	XM5_API_PARAM_GetFocusMapData                      = 2,
	XM5_API_PARAM_CapWaveFormVectorScope               = 2,
	XM5_API_PARAM_SetWaveFormVectorScope               = 1,
	XM5_API_PARAM_GetWaveFormVectorScope               = 1,
	XM5_API_PARAM_GetWaveFormData                      = 2,
	XM5_API_PARAM_GetVectorScopeData                   = 2,
	XM5_API_PARAM_GetParadeData                        = 2,
	XM5_API_PARAM_CapRecFrameIndicator                 = 2,
	XM5_API_PARAM_SetRecFrameIndicator                 = 1,
	XM5_API_PARAM_GetRecFrameIndicator                 = 1,
	XM5_API_PARAM_CapMovieCaptureDelay                 = 2,
	XM5_API_PARAM_SetMovieCaptureDelay                 = 1,
	XM5_API_PARAM_GetMovieCaptureDelay                 = 1,
	XM5_API_PARAM_CapAnamorphicDesqueezeDisplay        = 2,
	XM5_API_PARAM_SetAnamorphicDesqueezeDisplay        = 1,
	XM5_API_PARAM_GetAnamorphicDesqueezeDisplay        = 1,
	XM5_API_PARAM_CapAnamorphicMagnification           = 2,
	XM5_API_PARAM_SetAnamorphicMagnification           = 1,
	XM5_API_PARAM_GetAnamorphicMagnification           = 1,
	XM5_API_PARAM_CapWaveFormSetting                   = 2,
	XM5_API_PARAM_SetWaveFormSetting                   = 1,
	XM5_API_PARAM_GetWaveFormSetting                   = 1,
	XM5_API_PARAM_CapVectorScopeSetting                = 2,
	XM5_API_PARAM_SetVectorScopeSetting                = 1,
	XM5_API_PARAM_GetVectorScopeSetting                = 1,
	XM5_API_PARAM_CapParadeSettingDisplay              = 2,
	XM5_API_PARAM_SetParadeSettingDisplay              = 1,
	XM5_API_PARAM_GetParadeSettingDisplay              = 1,
	XM5_API_PARAM_CapParadeSettingColor                = 2,
	XM5_API_PARAM_SetParadeSettingColor                = 1,
	XM5_API_PARAM_GetParadeSettingColor                = 1,
	XM5_API_PARAM_CapFlogDRangePriority                = 2,
	XM5_API_PARAM_SetFlogDRangePriority                = 1,
	XM5_API_PARAM_GetFlogDRangePriority                = 1,
	XM5_API_PARAM_CapMovieWhiteBalanceTune             = 5,
	XM5_API_PARAM_SetMovieWhiteBalanceTune             = 3,
	XM5_API_PARAM_GetMovieWhiteBalanceTune             = 3,
	XM5_API_PARAM_CapMovieFilmSimulationMode           = 2,
	XM5_API_PARAM_SetMovieFilmSimulationMode           = 1,
	XM5_API_PARAM_GetMovieFilmSimulationMode           = 1,
	XM5_API_PARAM_CapMovieMonochromaticColor           = 4,
	XM5_API_PARAM_SetMovieMonochromaticColor           = 2,
	XM5_API_PARAM_GetMovieMonochromaticColor           = 2,
	XM5_API_PARAM_CapMovieHighLightTone                = 2,
	XM5_API_PARAM_SetMovieHighLightTone                = 1,
	XM5_API_PARAM_GetMovieHighLightTone                = 1,
	XM5_API_PARAM_CapMovieShadowTone                   = 2,
	XM5_API_PARAM_SetMovieShadowTone                   = 1,
	XM5_API_PARAM_GetMovieShadowTone                   = 1,
	XM5_API_PARAM_CapMovieSharpness                    = 2,
	XM5_API_PARAM_SetMovieSharpness                    = 1,
	XM5_API_PARAM_GetMovieSharpness                    = 1,
	XM5_API_PARAM_CapMovieColorMode                    = 2,
	XM5_API_PARAM_SetMovieColorMode                    = 1,
	XM5_API_PARAM_GetMovieColorMode                    = 1,
	XM5_API_PARAM_CapMovieNoiseReduction               = 2,
	XM5_API_PARAM_SetMovieNoiseReduction               = 1,
	XM5_API_PARAM_GetMovieNoiseReduction               = 1,
	XM5_API_PARAM_CapInterFrameNR                      = 2,
	XM5_API_PARAM_SetInterFrameNR                      = 1,
	XM5_API_PARAM_GetInterFrameNR                      = 1,
	XM5_API_PARAM_CapMoviePeripheralLightCorrection    = 2,
	XM5_API_PARAM_SetMoviePeripheralLightCorrection    = 1,
	XM5_API_PARAM_GetMoviePeripheralLightCorrection    = 1,
	XM5_API_PARAM_CapMovieFaceDetectionMode            = 2,
	XM5_API_PARAM_SetMovieFaceDetectionMode            = 1,
	XM5_API_PARAM_GetMovieFaceDetectionMode            = 1,
	XM5_API_PARAM_CapMovieEyeAFMode                    = 2,
	XM5_API_PARAM_SetMovieEyeAFMode                    = 1,
	XM5_API_PARAM_GetMovieEyeAFMode                    = 1,
	XM5_API_PARAM_CapMovieSubjectDetectionMode         = 2,
	XM5_API_PARAM_SetMovieSubjectDetectionMode         = 1,
	XM5_API_PARAM_GetMovieSubjectDetectionMode         = 1,
	XM5_API_PARAM_CapMovieAFCCustom                    = 2,
	XM5_API_PARAM_SetMovieAFCCustom                    = 1,
	XM5_API_PARAM_GetMovieAFCCustom                    = 1,
	XM5_API_PARAM_CapMovieMFAssistMode                 = 2,
	XM5_API_PARAM_SetMovieMFAssistMode                 = 1,
	XM5_API_PARAM_GetMovieMFAssistMode                 = 1,
	XM5_API_PARAM_GetMovieFocusMeter                   = 3,
	XM5_API_PARAM_CapMovieFocusCheckMode               = 2,
	XM5_API_PARAM_SetMovieFocusCheckMode               = 1,
	XM5_API_PARAM_GetMovieFocusCheckMode               = 1,
	XM5_API_PARAM_CapMovieFocusCheckLock               = 2,
	XM5_API_PARAM_SetMovieFocusCheckLock               = 1,
	XM5_API_PARAM_GetMovieFocusCheckLock               = 1,
	XM5_API_PARAM_GetMovieRecordingTime                = 3,
	XM5_API_PARAM_GetMovieRemainingTime                = 3,
	XM5_API_PARAM_GetTimeCode                          = 4,
	XM5_API_PARAM_CapTimeCodeDisplay                   = 2,
	XM5_API_PARAM_SetTimeCodeDisplay                   = 1,
	XM5_API_PARAM_GetTimeCodeDisplay                   = 1,
	XM5_API_PARAM_CapTimeCodeStartSetting              = 2,
	XM5_API_PARAM_SetTimeCodeStartSetting              = 5,
	XM5_API_PARAM_CapTimeCodeCountUp                   = 2,
	XM5_API_PARAM_SetTimeCodeCountUp                   = 1,
	XM5_API_PARAM_GetTimeCodeCountUp                   = 1,
	XM5_API_PARAM_CapTimeCodeDropFrame                 = 2,
	XM5_API_PARAM_SetTimeCodeDropFrame                 = 1,
	XM5_API_PARAM_GetTimeCodeDropFrame                 = 1,
	XM5_API_PARAM_CapTimeCodeHDMIOutput                = 2,
	XM5_API_PARAM_SetTimeCodeHDMIOutput                = 1,
	XM5_API_PARAM_GetTimeCodeHDMIOutput                = 1,
	XM5_API_PARAM_GetTimeCodeCurrentValue              = 4,
	XM5_API_PARAM_CapATOMOSAirGluConnection            = 2,
	XM5_API_PARAM_SetATOMOSAirGluConnection            = 1,
	XM5_API_PARAM_GetATOMOSAirGluConnection            = 1,
	XM5_API_PARAM_GetTimeCodeStatus                    = 1,
	XM5_API_PARAM_CapInternalMicLevel                  = 2,
	XM5_API_PARAM_SetInternalMicLevel                  = 1,
	XM5_API_PARAM_GetInternalMicLevel                  = 1,
	XM5_API_PARAM_CapInternalMicLevelManual            = 2,
	XM5_API_PARAM_SetInternalMicLevelManual            = 1,
	XM5_API_PARAM_GetInternalMicLevelManual            = 1,
	XM5_API_PARAM_CapExternalMicLevel                  = 2,
	XM5_API_PARAM_SetExternalMicLevel                  = 1,
	XM5_API_PARAM_GetExternalMicLevel                  = 1,
	XM5_API_PARAM_CapExternalMicLevelManual            = 2,
	XM5_API_PARAM_SetExternalMicLevelManual            = 1,
	XM5_API_PARAM_GetExternalMicLevelManual            = 1,
	XM5_API_PARAM_CapMicLevelLimiter                   = 2,
	XM5_API_PARAM_SetMicLevelLimiter                   = 1,
	XM5_API_PARAM_GetMicLevelLimiter                   = 1,
	XM5_API_PARAM_CapWindFilter                        = 2,
	XM5_API_PARAM_SetWindFilter                        = 1,
	XM5_API_PARAM_GetWindFilter                        = 1,
	XM5_API_PARAM_CapLowCutFilter                      = 2,
	XM5_API_PARAM_SetLowCutFilter                      = 1,
	XM5_API_PARAM_GetLowCutFilter                      = 1,
	XM5_API_PARAM_CapHeadPhonesVolume                  = 2,
	XM5_API_PARAM_SetHeadPhonesVolume                  = 1,
	XM5_API_PARAM_GetHeadPhonesVolume                  = 1,
	XM5_API_PARAM_CapXLRAdapterMicSource               = 2,
	XM5_API_PARAM_SetXLRAdapterMicSource               = 1,
	XM5_API_PARAM_GetXLRAdapterMicSource               = 1,
	XM5_API_PARAM_CapXLRAdapterMoniteringSource        = 2,
	XM5_API_PARAM_SetXLRAdapterMoniteringSource        = 1,
	XM5_API_PARAM_GetXLRAdapterMoniteringSource        = 1,
	XM5_API_PARAM_CapXLRAdapterHDMIOutputSource        = 2,
	XM5_API_PARAM_SetXLRAdapterHDMIOutputSource        = 1,
	XM5_API_PARAM_GetXLRAdapterHDMIOutputSource        = 1,
	XM5_API_PARAM_GetMicLevelIndicator                 = 1,
	XM5_API_PARAM_GetMovieTransparentFrameInfo         = 1,
	XM5_API_PARAM_CapShortMovieSecond                  = 2,
	XM5_API_PARAM_SetShortMovieSecond                  = 1,
	XM5_API_PARAM_GetShortMovieSecond                  = 1,
	XM5_API_PARAM_CapMoviePortraitEnhancer             = 2,
	XM5_API_PARAM_SetMoviePortraitEnhancer             = 1,
	XM5_API_PARAM_GetMoviePortraitEnhancer             = 1,
	XM5_API_PARAM_CapDirectionalMic                    = 2,
	XM5_API_PARAM_SetDirectionalMic                    = 1,
	XM5_API_PARAM_GetDirectionalMic                    = 1,
	XM5_API_PARAM_CapEnvSoundsNR                       = 2,
	XM5_API_PARAM_SetEnvSoundsNR                       = 1,
	XM5_API_PARAM_GetEnvSoundsNR                       = 1,
	XM5_API_PARAM_GetBodyTemperatureWarning            = 1,
	XM5_API_PARAM_GetPlayBackMediaInfo                 = -1,
	XM5_API_PARAM_GetPlayBackObjectInfo                = -1,
	XM5_API_PARAM_GetPlayBackThumb                     = -1,
	XM5_API_PARAM_GetPlayBackPartialObject             = -1,
	XM5_API_PARAM_CapMovieRecVolume                    = 2,
	XM5_API_PARAM_SetMovieRecVolume                    = 1,
	XM5_API_PARAM_GetMovieRecVolume                    = 1,
};

//
// API_CODE for X-M5
//
enum {
	XM5_API_CODE_CapMovieHighFrequencyFlickerlessMode   = API_CODE_CapMovieHighFrequencyFlickerlessMode,
	XM5_API_CODE_SetMovieHighFrequencyFlickerlessMode   = API_CODE_SetMovieHighFrequencyFlickerlessMode,
	XM5_API_CODE_GetMovieHighFrequencyFlickerlessMode   = API_CODE_GetMovieHighFrequencyFlickerlessMode,
	XM5_API_CODE_CapMovieAFMode                         = API_CODE_CapMovieAFMode,
	XM5_API_CODE_SetMovieAFMode                         = API_CODE_SetMovieAFMode,
	XM5_API_CODE_GetMovieAFMode                         = API_CODE_GetMovieAFMode,
	XM5_API_CODE_CapMovieFocusArea                      = API_CODE_CapMovieFocusArea,
	XM5_API_CODE_SetMovieFocusArea                      = API_CODE_SetMovieFocusArea,
	XM5_API_CODE_GetMovieFocusArea                      = API_CODE_GetMovieFocusArea,
	XM5_API_CODE_GetTrackingAfFrameInfo                 = API_CODE_GetTrackingAfFrameInfo,
	XM5_API_CODE_CapMovieResolution                     = API_CODE_CapMovieResolution,
	XM5_API_CODE_SetMovieResolution                     = API_CODE_SetMovieResolution,
	XM5_API_CODE_GetMovieResolution                     = API_CODE_GetMovieResolution,
	XM5_API_CODE_CapMovieFrameRate                      = API_CODE_CapMovieFrameRate,
	XM5_API_CODE_SetMovieFrameRate                      = API_CODE_SetMovieFrameRate,
	XM5_API_CODE_GetMovieFrameRate                      = API_CODE_GetMovieFrameRate,
	XM5_API_CODE_CapMovieBitRate                        = API_CODE_CapMovieBitRate,
	XM5_API_CODE_SetMovieBitRate                        = API_CODE_SetMovieBitRate,
	XM5_API_CODE_GetMovieBitRate                        = API_CODE_GetMovieBitRate,
	XM5_API_CODE_CapMovieFileFormat                     = API_CODE_CapMovieFileFormat,
	XM5_API_CODE_SetMovieFileFormat                     = API_CODE_SetMovieFileFormat,
	XM5_API_CODE_GetMovieFileFormat                     = API_CODE_GetMovieFileFormat,
	XM5_API_CODE_CapHighSpeedRecMode                    = API_CODE_CapHighSpeedRecMode,
	XM5_API_CODE_SetHighSpeedRecMode                    = API_CODE_SetHighSpeedRecMode,
	XM5_API_CODE_GetHighSpeedRecMode                    = API_CODE_GetHighSpeedRecMode,
	XM5_API_CODE_CapHighSpeedRecResolution              = API_CODE_CapHighSpeedRecResolution,
	XM5_API_CODE_SetHighSpeedRecResolution              = API_CODE_SetHighSpeedRecResolution,
	XM5_API_CODE_GetHighSpeedRecResolution              = API_CODE_GetHighSpeedRecResolution,
	XM5_API_CODE_CapHighSpeedRecFrameRate               = API_CODE_CapHighSpeedRecFrameRate,
	XM5_API_CODE_SetHighSpeedRecFrameRate               = API_CODE_SetHighSpeedRecFrameRate,
	XM5_API_CODE_GetHighSpeedRecFrameRate               = API_CODE_GetHighSpeedRecFrameRate,
	XM5_API_CODE_CapHighSpeedRecPlayBackFrameRate       = API_CODE_CapHighSpeedRecPlayBackFrameRate,
	XM5_API_CODE_SetHighSpeedRecPlayBackFrameRate       = API_CODE_SetHighSpeedRecPlayBackFrameRate,
	XM5_API_CODE_GetHighSpeedRecPlayBackFrameRate       = API_CODE_GetHighSpeedRecPlayBackFrameRate,
	XM5_API_CODE_CapMovieMediaRecord                    = API_CODE_CapMovieMediaRecord,
	XM5_API_CODE_SetMovieMediaRecord                    = API_CODE_SetMovieMediaRecord,
	XM5_API_CODE_GetMovieMediaRecord                    = API_CODE_GetMovieMediaRecord,
	XM5_API_CODE_CapMovieMediaRecordProRes              = API_CODE_CapMovieMediaRecordProRes,
	XM5_API_CODE_SetMovieMediaRecordProRes              = API_CODE_SetMovieMediaRecordProRes,
	XM5_API_CODE_GetMovieMediaRecordProRes              = API_CODE_GetMovieMediaRecordProRes,
	XM5_API_CODE_CapMovieHDMIOutputRAW                  = API_CODE_CapMovieHDMIOutputRAW,
	XM5_API_CODE_SetMovieHDMIOutputRAW                  = API_CODE_SetMovieHDMIOutputRAW,
	XM5_API_CODE_GetMovieHDMIOutputRAW                  = API_CODE_GetMovieHDMIOutputRAW,
	XM5_API_CODE_CapMovieHDMIOutputRAWResolution        = API_CODE_CapMovieHDMIOutputRAWResolution,
	XM5_API_CODE_SetMovieHDMIOutputRAWResolution        = API_CODE_SetMovieHDMIOutputRAWResolution,
	XM5_API_CODE_GetMovieHDMIOutputRAWResolution        = API_CODE_GetMovieHDMIOutputRAWResolution,
	XM5_API_CODE_CapMovieHDMIOutputRAWFrameRate         = API_CODE_CapMovieHDMIOutputRAWFrameRate,
	XM5_API_CODE_SetMovieHDMIOutputRAWFrameRate         = API_CODE_SetMovieHDMIOutputRAWFrameRate,
	XM5_API_CODE_GetMovieHDMIOutputRAWFrameRate         = API_CODE_GetMovieHDMIOutputRAWFrameRate,
	XM5_API_CODE_CapFlogRecording                       = API_CODE_CapFlogRecording,
	XM5_API_CODE_SetFlogRecording                       = API_CODE_SetFlogRecording,
	XM5_API_CODE_GetFlogRecording                       = API_CODE_GetFlogRecording,
	XM5_API_CODE_CapMovieCropMagnification              = API_CODE_CapMovieCropMagnification,
	XM5_API_CODE_SetMovieCropMagnification              = API_CODE_SetMovieCropMagnification,
	XM5_API_CODE_GetMovieCropMagnification              = API_CODE_GetMovieCropMagnification,
	XM5_API_CODE_CapMovieHDMIOutputInfoDisplay          = API_CODE_CapMovieHDMIOutputInfoDisplay,
	XM5_API_CODE_SetMovieHDMIOutputInfoDisplay          = API_CODE_SetMovieHDMIOutputInfoDisplay,
	XM5_API_CODE_GetMovieHDMIOutputInfoDisplay          = API_CODE_GetMovieHDMIOutputInfoDisplay,
	XM5_API_CODE_CapMovieHDMIRecControl                 = API_CODE_CapMovieHDMIRecControl,
	XM5_API_CODE_SetMovieHDMIRecControl                 = API_CODE_SetMovieHDMIRecControl,
	XM5_API_CODE_GetMovieHDMIRecControl                 = API_CODE_GetMovieHDMIRecControl,
	XM5_API_CODE_CapMovieIsMode                         = API_CODE_CapMovieIsMode,
	XM5_API_CODE_SetMovieIsMode                         = API_CODE_SetMovieIsMode,
	XM5_API_CODE_GetMovieIsMode                         = API_CODE_GetMovieIsMode,
	XM5_API_CODE_CapMovieIsModeBoost                    = API_CODE_CapMovieIsModeBoost,
	XM5_API_CODE_SetMovieIsModeBoost                    = API_CODE_SetMovieIsModeBoost,
	XM5_API_CODE_GetMovieIsModeBoost                    = API_CODE_GetMovieIsModeBoost,
	XM5_API_CODE_CapMovieZebraSetting                   = API_CODE_CapMovieZebraSetting,
	XM5_API_CODE_SetMovieZebraSetting                   = API_CODE_SetMovieZebraSetting,
	XM5_API_CODE_GetMovieZebraSetting                   = API_CODE_GetMovieZebraSetting,
	XM5_API_CODE_CapMovieZebraLevel                     = API_CODE_CapMovieZebraLevel,
	XM5_API_CODE_SetMovieZebraLevel                     = API_CODE_SetMovieZebraLevel,
	XM5_API_CODE_GetMovieZebraLevel                     = API_CODE_GetMovieZebraLevel,
	XM5_API_CODE_CapMovieOptimizedControl               = API_CODE_CapMovieOptimizedControl,
	XM5_API_CODE_SetMovieOptimizedControl               = API_CODE_SetMovieOptimizedControl,
	XM5_API_CODE_GetMovieOptimizedControl               = API_CODE_GetMovieOptimizedControl,
	XM5_API_CODE_CapMovieTallyLight                     = API_CODE_CapMovieTallyLight,
	XM5_API_CODE_SetMovieTallyLight                     = API_CODE_SetMovieTallyLight,
	XM5_API_CODE_GetMovieTallyLight                     = API_CODE_GetMovieTallyLight,
	XM5_API_CODE_GetMediaEjectWarning                   = API_CODE_GetMediaEjectWarning,
	XM5_API_CODE_CapMovieDataLevelSetting               = API_CODE_CapMovieDataLevelSetting,
	XM5_API_CODE_SetMovieDataLevelSetting               = API_CODE_SetMovieDataLevelSetting,
	XM5_API_CODE_GetMovieDataLevelSetting               = API_CODE_GetMovieDataLevelSetting,
	XM5_API_CODE_GetMovieCropMagnificationValue         = API_CODE_GetMovieCropMagnificationValue,
	XM5_API_CODE_CapMovieFullTimeManual                 = API_CODE_CapMovieFullTimeManual,
	XM5_API_CODE_SetMovieFullTimeManual                 = API_CODE_SetMovieFullTimeManual,
	XM5_API_CODE_GetMovieFullTimeManual                 = API_CODE_GetMovieFullTimeManual,
	XM5_API_CODE_CapMovieDigitalZoom                    = API_CODE_CapMovieDigitalZoom,
	XM5_API_CODE_SetMovieDigitalZoom                    = API_CODE_SetMovieDigitalZoom,
	XM5_API_CODE_GetMovieDigitalZoom                    = API_CODE_GetMovieDigitalZoom,
	XM5_API_CODE_GetMovieDigitalZoomRange               = API_CODE_GetMovieDigitalZoomRange,
	XM5_API_CODE_CapMovieImageFormat                    = API_CODE_CapMovieImageFormat,
	XM5_API_CODE_SetMovieImageFormat                    = API_CODE_SetMovieImageFormat,
	XM5_API_CODE_GetMovieImageFormat                    = API_CODE_GetMovieImageFormat,
	XM5_API_CODE_GetHistogramData                       = API_CODE_GetHistogramData,
	XM5_API_CODE_GetFocusMapData                        = API_CODE_GetFocusMapData,
	XM5_API_CODE_CapWaveFormVectorScope                 = API_CODE_CapWaveFormVectorScope,
	XM5_API_CODE_SetWaveFormVectorScope                 = API_CODE_SetWaveFormVectorScope,
	XM5_API_CODE_GetWaveFormVectorScope                 = API_CODE_GetWaveFormVectorScope,
	XM5_API_CODE_GetWaveFormData                        = API_CODE_GetWaveFormData,
	XM5_API_CODE_GetVectorScopeData                     = API_CODE_GetVectorScopeData,
	XM5_API_CODE_GetParadeData                          = API_CODE_GetParadeData,
	XM5_API_CODE_CapRecFrameIndicator                   = API_CODE_CapRecFrameIndicator,
	XM5_API_CODE_SetRecFrameIndicator                   = API_CODE_SetRecFrameIndicator,
	XM5_API_CODE_GetRecFrameIndicator                   = API_CODE_GetRecFrameIndicator,
	XM5_API_CODE_CapMovieCaptureDelay                   = API_CODE_CapMovieCaptureDelay,
	XM5_API_CODE_SetMovieCaptureDelay                   = API_CODE_SetMovieCaptureDelay,
	XM5_API_CODE_GetMovieCaptureDelay                   = API_CODE_GetMovieCaptureDelay,
	XM5_API_CODE_CapAnamorphicDesqueezeDisplay          = API_CODE_CapAnamorphicDesqueezeDisplay,
	XM5_API_CODE_SetAnamorphicDesqueezeDisplay          = API_CODE_SetAnamorphicDesqueezeDisplay,
	XM5_API_CODE_GetAnamorphicDesqueezeDisplay          = API_CODE_GetAnamorphicDesqueezeDisplay,
	XM5_API_CODE_CapAnamorphicMagnification             = API_CODE_CapAnamorphicMagnification,
	XM5_API_CODE_SetAnamorphicMagnification             = API_CODE_SetAnamorphicMagnification,
	XM5_API_CODE_GetAnamorphicMagnification             = API_CODE_GetAnamorphicMagnification,
	XM5_API_CODE_CapWaveFormSetting                     = API_CODE_CapWaveFormSetting,
	XM5_API_CODE_SetWaveFormSetting                     = API_CODE_SetWaveFormSetting,
	XM5_API_CODE_GetWaveFormSetting                     = API_CODE_GetWaveFormSetting,
	XM5_API_CODE_CapVectorScopeSetting                  = API_CODE_CapVectorScopeSetting,
	XM5_API_CODE_SetVectorScopeSetting                  = API_CODE_SetVectorScopeSetting,
	XM5_API_CODE_GetVectorScopeSetting                  = API_CODE_GetVectorScopeSetting,
	XM5_API_CODE_CapParadeSettingDisplay                = API_CODE_CapParadeSettingDisplay,
	XM5_API_CODE_SetParadeSettingDisplay                = API_CODE_SetParadeSettingDisplay,
	XM5_API_CODE_GetParadeSettingDisplay                = API_CODE_GetParadeSettingDisplay,
	XM5_API_CODE_CapParadeSettingColor                  = API_CODE_CapParadeSettingColor,
	XM5_API_CODE_SetParadeSettingColor                  = API_CODE_SetParadeSettingColor,
	XM5_API_CODE_GetParadeSettingColor                  = API_CODE_GetParadeSettingColor,
	XM5_API_CODE_CapFlogDRangePriority                  = API_CODE_CapFlogDRangePriority,
	XM5_API_CODE_SetFlogDRangePriority                  = API_CODE_SetFlogDRangePriority,
	XM5_API_CODE_GetFlogDRangePriority                  = API_CODE_GetFlogDRangePriority,
	XM5_API_CODE_CapMovieWhiteBalanceTune               = API_CODE_CapMovieWhiteBalanceTune,
	XM5_API_CODE_SetMovieWhiteBalanceTune               = API_CODE_SetMovieWhiteBalanceTune,
	XM5_API_CODE_GetMovieWhiteBalanceTune               = API_CODE_GetMovieWhiteBalanceTune,
	XM5_API_CODE_CapMovieFilmSimulationMode             = API_CODE_CapMovieFilmSimulationMode,
	XM5_API_CODE_SetMovieFilmSimulationMode             = API_CODE_SetMovieFilmSimulationMode,
	XM5_API_CODE_GetMovieFilmSimulationMode             = API_CODE_GetMovieFilmSimulationMode,
	XM5_API_CODE_CapMovieMonochromaticColor             = API_CODE_CapMovieMonochromaticColor,
	XM5_API_CODE_SetMovieMonochromaticColor             = API_CODE_SetMovieMonochromaticColor,
	XM5_API_CODE_GetMovieMonochromaticColor             = API_CODE_GetMovieMonochromaticColor,
	XM5_API_CODE_CapMovieHighLightTone                  = API_CODE_CapMovieHighLightTone,
	XM5_API_CODE_SetMovieHighLightTone                  = API_CODE_SetMovieHighLightTone,
	XM5_API_CODE_GetMovieHighLightTone                  = API_CODE_GetMovieHighLightTone,
	XM5_API_CODE_CapMovieShadowTone                     = API_CODE_CapMovieShadowTone,
	XM5_API_CODE_SetMovieShadowTone                     = API_CODE_SetMovieShadowTone,
	XM5_API_CODE_GetMovieShadowTone                     = API_CODE_GetMovieShadowTone,
	XM5_API_CODE_CapMovieSharpness                      = API_CODE_CapMovieSharpness,
	XM5_API_CODE_SetMovieSharpness                      = API_CODE_SetMovieSharpness,
	XM5_API_CODE_GetMovieSharpness                      = API_CODE_GetMovieSharpness,
	XM5_API_CODE_CapMovieColorMode                      = API_CODE_CapMovieColorMode,
	XM5_API_CODE_SetMovieColorMode                      = API_CODE_SetMovieColorMode,
	XM5_API_CODE_GetMovieColorMode                      = API_CODE_GetMovieColorMode,
	XM5_API_CODE_CapMovieNoiseReduction                 = API_CODE_CapMovieNoiseReduction,
	XM5_API_CODE_SetMovieNoiseReduction                 = API_CODE_SetMovieNoiseReduction,
	XM5_API_CODE_GetMovieNoiseReduction                 = API_CODE_GetMovieNoiseReduction,
	XM5_API_CODE_CapInterFrameNR                        = API_CODE_CapInterFrameNR,
	XM5_API_CODE_SetInterFrameNR                        = API_CODE_SetInterFrameNR,
	XM5_API_CODE_GetInterFrameNR                        = API_CODE_GetInterFrameNR,
	XM5_API_CODE_CapMoviePeripheralLightCorrection      = API_CODE_CapMoviePeripheralLightCorrection,
	XM5_API_CODE_SetMoviePeripheralLightCorrection      = API_CODE_SetMoviePeripheralLightCorrection,
	XM5_API_CODE_GetMoviePeripheralLightCorrection      = API_CODE_GetMoviePeripheralLightCorrection,
	XM5_API_CODE_CapMovieFaceDetectionMode              = API_CODE_CapMovieFaceDetectionMode,
	XM5_API_CODE_SetMovieFaceDetectionMode              = API_CODE_SetMovieFaceDetectionMode,
	XM5_API_CODE_GetMovieFaceDetectionMode              = API_CODE_GetMovieFaceDetectionMode,
	XM5_API_CODE_CapMovieEyeAFMode                      = API_CODE_CapMovieEyeAFMode,
	XM5_API_CODE_SetMovieEyeAFMode                      = API_CODE_SetMovieEyeAFMode,
	XM5_API_CODE_GetMovieEyeAFMode                      = API_CODE_GetMovieEyeAFMode,
	XM5_API_CODE_CapMovieSubjectDetectionMode           = API_CODE_CapMovieSubjectDetectionMode,
	XM5_API_CODE_SetMovieSubjectDetectionMode           = API_CODE_SetMovieSubjectDetectionMode,
	XM5_API_CODE_GetMovieSubjectDetectionMode           = API_CODE_GetMovieSubjectDetectionMode,
	XM5_API_CODE_CapMovieAFCCustom                      = API_CODE_CapMovieAFCCustom,
	XM5_API_CODE_SetMovieAFCCustom                      = API_CODE_SetMovieAFCCustom,
	XM5_API_CODE_GetMovieAFCCustom                      = API_CODE_GetMovieAFCCustom,
	XM5_API_CODE_CapMovieMFAssistMode                   = API_CODE_CapMovieMFAssistMode,
	XM5_API_CODE_SetMovieMFAssistMode                   = API_CODE_SetMovieMFAssistMode,
	XM5_API_CODE_GetMovieMFAssistMode                   = API_CODE_GetMovieMFAssistMode,
	XM5_API_CODE_GetMovieFocusMeter                     = API_CODE_GetMovieFocusMeter,
	XM5_API_CODE_CapMovieFocusCheckMode                 = API_CODE_CapMovieFocusCheckMode,
	XM5_API_CODE_SetMovieFocusCheckMode                 = API_CODE_SetMovieFocusCheckMode,
	XM5_API_CODE_GetMovieFocusCheckMode                 = API_CODE_GetMovieFocusCheckMode,
	XM5_API_CODE_CapMovieFocusCheckLock                 = API_CODE_CapMovieFocusCheckLock,
	XM5_API_CODE_SetMovieFocusCheckLock                 = API_CODE_SetMovieFocusCheckLock,
	XM5_API_CODE_GetMovieFocusCheckLock                 = API_CODE_GetMovieFocusCheckLock,
	XM5_API_CODE_GetMovieRecordingTime                  = API_CODE_GetMovieRecordingTime,
	XM5_API_CODE_GetMovieRemainingTime                  = API_CODE_GetMovieRemainingTime,
	XM5_API_CODE_GetTimeCode                            = API_CODE_GetTimeCode,
	XM5_API_CODE_CapTimeCodeDisplay                     = API_CODE_CapTimeCodeDisplay,
	XM5_API_CODE_SetTimeCodeDisplay                     = API_CODE_SetTimeCodeDisplay,
	XM5_API_CODE_GetTimeCodeDisplay                     = API_CODE_GetTimeCodeDisplay,
	XM5_API_CODE_CapTimeCodeStartSetting                = API_CODE_CapTimeCodeStartSetting,
	XM5_API_CODE_SetTimeCodeStartSetting                = API_CODE_SetTimeCodeStartSetting,
	XM5_API_CODE_CapTimeCodeCountUp                     = API_CODE_CapTimeCodeCountUp,
	XM5_API_CODE_SetTimeCodeCountUp                     = API_CODE_SetTimeCodeCountUp,
	XM5_API_CODE_GetTimeCodeCountUp                     = API_CODE_GetTimeCodeCountUp,
	XM5_API_CODE_CapTimeCodeDropFrame                   = API_CODE_CapTimeCodeDropFrame,
	XM5_API_CODE_SetTimeCodeDropFrame                   = API_CODE_SetTimeCodeDropFrame,
	XM5_API_CODE_GetTimeCodeDropFrame                   = API_CODE_GetTimeCodeDropFrame,
	XM5_API_CODE_CapTimeCodeHDMIOutput                  = API_CODE_CapTimeCodeHDMIOutput,
	XM5_API_CODE_SetTimeCodeHDMIOutput                  = API_CODE_SetTimeCodeHDMIOutput,
	XM5_API_CODE_GetTimeCodeHDMIOutput                  = API_CODE_GetTimeCodeHDMIOutput,
	XM5_API_CODE_GetTimeCodeCurrentValue                = API_CODE_GetTimeCodeCurrentValue,
	XM5_API_CODE_CapATOMOSAirGluConnection              = API_CODE_CapATOMOSAirGluConnection,
	XM5_API_CODE_SetATOMOSAirGluConnection              = API_CODE_SetATOMOSAirGluConnection,
	XM5_API_CODE_GetATOMOSAirGluConnection              = API_CODE_GetATOMOSAirGluConnection,
	XM5_API_CODE_GetTimeCodeStatus                      = API_CODE_GetTimeCodeStatus,
	XM5_API_CODE_CapInternalMicLevel                    = API_CODE_CapInternalMicLevel,
	XM5_API_CODE_SetInternalMicLevel                    = API_CODE_SetInternalMicLevel,
	XM5_API_CODE_GetInternalMicLevel                    = API_CODE_GetInternalMicLevel,
	XM5_API_CODE_CapInternalMicLevelManual              = API_CODE_CapInternalMicLevelManual,
	XM5_API_CODE_SetInternalMicLevelManual              = API_CODE_SetInternalMicLevelManual,
	XM5_API_CODE_GetInternalMicLevelManual              = API_CODE_GetInternalMicLevelManual,
	XM5_API_CODE_CapExternalMicLevel                    = API_CODE_CapExternalMicLevel,
	XM5_API_CODE_SetExternalMicLevel                    = API_CODE_SetExternalMicLevel,
	XM5_API_CODE_GetExternalMicLevel                    = API_CODE_GetExternalMicLevel,
	XM5_API_CODE_CapExternalMicLevelManual              = API_CODE_CapExternalMicLevelManual,
	XM5_API_CODE_SetExternalMicLevelManual              = API_CODE_SetExternalMicLevelManual,
	XM5_API_CODE_GetExternalMicLevelManual              = API_CODE_GetExternalMicLevelManual,
	XM5_API_CODE_CapMicLevelLimiter                     = API_CODE_CapMicLevelLimiter,
	XM5_API_CODE_SetMicLevelLimiter                     = API_CODE_SetMicLevelLimiter,
	XM5_API_CODE_GetMicLevelLimiter                     = API_CODE_GetMicLevelLimiter,
	XM5_API_CODE_CapWindFilter                          = API_CODE_CapWindFilter,
	XM5_API_CODE_SetWindFilter                          = API_CODE_SetWindFilter,
	XM5_API_CODE_GetWindFilter                          = API_CODE_GetWindFilter,
	XM5_API_CODE_CapLowCutFilter                        = API_CODE_CapLowCutFilter,
	XM5_API_CODE_SetLowCutFilter                        = API_CODE_SetLowCutFilter,
	XM5_API_CODE_GetLowCutFilter                        = API_CODE_GetLowCutFilter,
	XM5_API_CODE_CapHeadPhonesVolume                    = API_CODE_CapHeadPhonesVolume,
	XM5_API_CODE_SetHeadPhonesVolume                    = API_CODE_SetHeadPhonesVolume,
	XM5_API_CODE_GetHeadPhonesVolume                    = API_CODE_GetHeadPhonesVolume,
	XM5_API_CODE_CapXLRAdapterMicSource                 = API_CODE_CapXLRAdapterMicSource,
	XM5_API_CODE_SetXLRAdapterMicSource                 = API_CODE_SetXLRAdapterMicSource,
	XM5_API_CODE_GetXLRAdapterMicSource                 = API_CODE_GetXLRAdapterMicSource,
	XM5_API_CODE_CapXLRAdapterMoniteringSource          = API_CODE_CapXLRAdapterMoniteringSource,
	XM5_API_CODE_SetXLRAdapterMoniteringSource          = API_CODE_SetXLRAdapterMoniteringSource,
	XM5_API_CODE_GetXLRAdapterMoniteringSource          = API_CODE_GetXLRAdapterMoniteringSource,
	XM5_API_CODE_CapXLRAdapterHDMIOutputSource          = API_CODE_CapXLRAdapterHDMIOutputSource,
	XM5_API_CODE_SetXLRAdapterHDMIOutputSource          = API_CODE_SetXLRAdapterHDMIOutputSource,
	XM5_API_CODE_GetXLRAdapterHDMIOutputSource          = API_CODE_GetXLRAdapterHDMIOutputSource,
	XM5_API_CODE_GetMicLevelIndicator                   = API_CODE_GetMicLevelIndicator,
	XM5_API_CODE_GetMovieTransparentFrameInfo           = API_CODE_GetMovieTransparentFrameInfo,
	XM5_API_CODE_CapShortMovieSecond                    = API_CODE_CapShortMovieSecond,
	XM5_API_CODE_SetShortMovieSecond                    = API_CODE_SetShortMovieSecond,
	XM5_API_CODE_GetShortMovieSecond                    = API_CODE_GetShortMovieSecond,
	XM5_API_CODE_CapMoviePortraitEnhancer               = API_CODE_CapMoviePortraitEnhancer,
	XM5_API_CODE_SetMoviePortraitEnhancer               = API_CODE_SetMoviePortraitEnhancer,
	XM5_API_CODE_GetMoviePortraitEnhancer               = API_CODE_GetMoviePortraitEnhancer,
	XM5_API_CODE_CapDirectionalMic                      = API_CODE_CapDirectionalMic,
	XM5_API_CODE_SetDirectionalMic                      = API_CODE_SetDirectionalMic,
	XM5_API_CODE_GetDirectionalMic                      = API_CODE_GetDirectionalMic,
	XM5_API_CODE_CapEnvSoundsNR                         = API_CODE_CapEnvSoundsNR,
	XM5_API_CODE_SetEnvSoundsNR                         = API_CODE_SetEnvSoundsNR,
	XM5_API_CODE_GetEnvSoundsNR                         = API_CODE_GetEnvSoundsNR,
	XM5_API_CODE_GetBodyTemperatureWarning              = API_CODE_GetBodyTemperatureWarning,
	XM5_API_CODE_GetPlayBackMediaInfo                   = API_CODE_GetPlayBackMediaInfo,
	XM5_API_CODE_GetPlayBackObjectInfo                  = API_CODE_GetPlayBackObjectInfo,
	XM5_API_CODE_GetPlayBackThumb                       = API_CODE_GetPlayBackThumb,
	XM5_API_CODE_GetPlayBackPartialObject               = API_CODE_GetPlayBackPartialObject,
	XM5_API_CODE_CapMovieRecVolume                      = API_CODE_CapMovieRecVolume,
	XM5_API_CODE_SetMovieRecVolume                      = API_CODE_SetMovieRecVolume,
	XM5_API_CODE_GetMovieRecVolume                      = API_CODE_GetMovieRecVolume,
};

// Movie Af Mode
enum{
	XM5_MOVIE_AF_MULTI                     = SDK_MOVIE_AF_MULTI,
	XM5_MOVIE_AF_AREA                      = SDK_MOVIE_AF_AREA,
	XM5_MOVIE_AF_WIDETRACKING              = SDK_MOVIE_AF_WIDETRACKING,
};

// Short Movie Second
enum{
	XM5_SHORT_MOVIE_SECOND_OFF             = SDK_SHORT_MOVIE_SECOND_OFF,
	XM5_SHORT_MOVIE_SECOND_15S             = SDK_SHORT_MOVIE_SECOND_15S,
	XM5_SHORT_MOVIE_SECOND_30S             = SDK_SHORT_MOVIE_SECOND_30S,
	XM5_SHORT_MOVIE_SECOND_60S             = SDK_SHORT_MOVIE_SECOND_60S,
};

// Movie Resolution
// High Speed Rec Resolution
enum{
	XM5_MOVIE_RESOLUTION_6P2K_3_2          = SDK_MOVIE_RESOLUTION_6P2K_3_2,
	XM5_MOVIE_RESOLUTION_8K_16_9           = SDK_MOVIE_RESOLUTION_8K_16_9,
	XM5_MOVIE_RESOLUTION_6K_16_9           = SDK_MOVIE_RESOLUTION_6K_16_9,
	XM5_MOVIE_RESOLUTION_4KHQ_16_9         = SDK_MOVIE_RESOLUTION_4KHQ_16_9,
	XM5_MOVIE_RESOLUTION_4K_16_9           = SDK_MOVIE_RESOLUTION_4K_16_9,
	XM5_MOVIE_RESOLUTION_DCIHQ_17_9        = SDK_MOVIE_RESOLUTION_DCIHQ_17_9,
	XM5_MOVIE_RESOLUTION_DCI_17_9          = SDK_MOVIE_RESOLUTION_DCI_17_9,
	XM5_MOVIE_RESOLUTION_FULLHD_16_9       = SDK_MOVIE_RESOLUTION_FULLHD_16_9,
	XM5_MOVIE_RESOLUTION_FULLHD_17_9       = SDK_MOVIE_RESOLUTION_FULLHD_17_9,
	XM5_MOVIE_RESOLUTION_5K_17_9           = SDK_MOVIE_RESOLUTION_5K_17_9,
	XM5_MOVIE_RESOLUTION_DCI_8K_17_9       = SDK_MOVIE_RESOLUTION_DCI_8K_17_9,
	XM5_MOVIE_RESOLUTION_CINESCO_2P35_1    = SDK_MOVIE_RESOLUTION_CINESCO_2P35_1,
	XM5_MOVIE_RESOLUTION_OPENGATE_3_2      = SDK_MOVIE_RESOLUTION_OPENGATE_3_2,
	XM5_MOVIE_RESOLUTION_35MM_16_9         = SDK_MOVIE_RESOLUTION_35MM_16_9,
	XM5_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1,
	XM5_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1,
	XM5_MOVIE_RESOLUTION_FULLFRAME_3_2     = SDK_MOVIE_RESOLUTION_FULLFRAME_3_2,
	XM5_MOVIE_RESOLUTION_FULLHD_LP_16_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_16_9,
	XM5_MOVIE_RESOLUTION_FULLHD_LP_17_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_17_9,
	XM5_MOVIE_RESOLUTION_4K_LP_16_9        = SDK_MOVIE_RESOLUTION_4K_LP_16_9,
	XM5_MOVIE_RESOLUTION_FULLHD_9_16       = SDK_MOVIE_RESOLUTION_FULLHD_9_16,
};

// Movie FrameRate
// High Speed Rec PlayBack FrameRate
enum{
	XM5_MOVIE_FRAMERATE_59_94P             = SDK_MOVIE_FRAMERATE_59_94P,
	XM5_MOVIE_FRAMERATE_50P                = SDK_MOVIE_FRAMERATE_50P,
	XM5_MOVIE_FRAMERATE_29_97P             = SDK_MOVIE_FRAMERATE_29_97P,
	XM5_MOVIE_FRAMERATE_25P                = SDK_MOVIE_FRAMERATE_25P,
	XM5_MOVIE_FRAMERATE_24P                = SDK_MOVIE_FRAMERATE_24P,
	XM5_MOVIE_FRAMERATE_23_98P             = SDK_MOVIE_FRAMERATE_23_98P,
};

// Movie BitRate
enum{
	XM5_MOVIE_BITRATE_720MBPS              = SDK_MOVIE_BITRATE_720MBPS,
	XM5_MOVIE_BITRATE_400MBPS              = SDK_MOVIE_BITRATE_400MBPS,
	XM5_MOVIE_BITRATE_360MBPS              = SDK_MOVIE_BITRATE_360MBPS,
	XM5_MOVIE_BITRATE_200MBPS              = SDK_MOVIE_BITRATE_200MBPS,
	XM5_MOVIE_BITRATE_100MBPS              = SDK_MOVIE_BITRATE_100MBPS,
	XM5_MOVIE_BITRATE_50MBPS               = SDK_MOVIE_BITRATE_50MBPS,
	XM5_MOVIE_BITRATE_25MBPS               = SDK_MOVIE_BITRATE_25MBPS,
	XM5_MOVIE_BITRATE_8MBPS                = SDK_MOVIE_BITRATE_8MBPS,
};

// Movie FileFormat
enum{
	XM5_MOVIE_FORMAT_H264_ALL_I_MOV        = SDK_MOVIE_FORMAT_H264_ALL_I_MOV,
	XM5_MOVIE_FORMAT_H264_LONGGOP_I_MOV    = SDK_MOVIE_FORMAT_H264_LONGGOP_I_MOV,
	XM5_MOVIE_FORMAT_H264_LONGGOP_MP4      = SDK_MOVIE_FORMAT_H264_LONGGOP_MP4,
	XM5_MOVIE_FORMAT_H265_4_2_0_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_0_ALL_I,
	XM5_MOVIE_FORMAT_H265_4_2_0_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_0_LONGGOP,
	XM5_MOVIE_FORMAT_H265_4_2_2_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_2_ALL_I,
	XM5_MOVIE_FORMAT_H265_4_2_2_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_2_LONGGOP,
	XM5_MOVIE_FORMAT_PRORESHQ              = SDK_MOVIE_FORMAT_PRORESHQ,
	XM5_MOVIE_FORMAT_PRORES                = SDK_MOVIE_FORMAT_PRORES,
	XM5_MOVIE_FORMAT_PRORESLT              = SDK_MOVIE_FORMAT_PRORESLT,
};

// High Speed RecMode
enum{
	XM5_HIGHSPEEDREC_OFF                   = SDK_HIGHSPEEDREC_OFF,
	XM5_HIGHSPEEDREC_ON                    = SDK_HIGHSPEEDREC_ON,
	XM5_HIGHSPEEDREC_ON_HDMI_ONLY          = SDK_HIGHSPEEDREC_ON_HDMI_ONLY,
};

// High Speed Rec FrameRate
enum{
	XM5_HIGHSPEEDREC_FRAMERATE_240P        = SDK_HIGHSPEEDREC_FRAMERATE_240P,
	XM5_HIGHSPEEDREC_FRAMERATE_200P        = SDK_HIGHSPEEDREC_FRAMERATE_200P,
	XM5_HIGHSPEEDREC_FRAMERATE_120P        = SDK_HIGHSPEEDREC_FRAMERATE_120P,
	XM5_HIGHSPEEDREC_FRAMERATE_100P        = SDK_HIGHSPEEDREC_FRAMERATE_100P,
};

// Movie Media Record
enum{
	XM5_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2,
	XM5_MOVIE_MEDIARECORD_SLOT2                  = SDK_MOVIE_MEDIARECORD_SLOT2,
	XM5_MOVIE_MEDIARECORD_SLOT1                  = SDK_MOVIE_MEDIARECORD_SLOT1,
	XM5_MOVIE_MEDIARECORD_BACKUP                 = SDK_MOVIE_MEDIARECORD_BACKUP,
	XM5_MOVIE_MEDIARECORD_SSD                    = SDK_MOVIE_MEDIARECORD_SSD,
	XM5_MOVIE_MEDIARECORD_SSD_CF                 = SDK_MOVIE_MEDIARECORD_SSD_CF,
	XM5_MOVIE_MEDIARECORD_OFF                    = SDK_MOVIE_MEDIARECORD_OFF,
	XM5_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF       = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF,
	XM5_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1,
};

// Movie Media Record ProRes
enum{
	XM5_MOVIE_MEDIARECORD_PRORES_OFF       = SDK_MOVIE_MEDIARECORD_PRORES_OFF,
	XM5_MOVIE_MEDIARECORD_PRORES_H264      = SDK_MOVIE_MEDIARECORD_PRORES_H264,
	XM5_MOVIE_MEDIARECORD_PRORES_PROXY     = SDK_MOVIE_MEDIARECORD_PRORES_PROXY,
};

// Movie HDMIOutput RAW
enum{
	XM5_MOVIE_HDMI_OUTPUT_RAW_OFF          = SDK_MOVIE_HDMI_OUTPUT_RAW_OFF,
	XM5_MOVIE_HDMI_OUTPUT_RAW_ATOMOS       = SDK_MOVIE_HDMI_OUTPUT_RAW_ATOMOS,
	XM5_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC   = SDK_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC,
};

// Movie HDMI Output RAW Resolution
enum{
	XM5_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K,
	XM5_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K,
	XM5_MOVIE_HDMI_OUTPUT_RESOLUTION_8K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_8K,
	XM5_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K,
	XM5_MOVIE_HDMI_OUTPUT_RESOLUTION_4K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4K,
	XM5_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K= SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K,
};

// Movie HDMI Output RAW FrameRate
enum{
	XM5_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P,
	XM5_MOVIE_HDMI_OUTPUT_FRAMERATE_50P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_50P,
	XM5_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P,
	XM5_MOVIE_HDMI_OUTPUT_FRAMERATE_25P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_25P,
	XM5_MOVIE_HDMI_OUTPUT_FRAMERATE_24P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_24P,
	XM5_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P,
};

// Flog Recording
enum{
	XM5_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM,
	XM5_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG,
	XM5_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2 = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2,
	XM5_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG,
	XM5_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2  = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2,
	XM5_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM,
	XM5_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM  = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM,
	XM5_MOVIERECORD_MEDIA_HLG_HDMI_HLG     = SDK_MOVIERECORD_MEDIA_HLG_HDMI_HLG,
};

// MovieIsMode
enum{
	XM5_MOVIE_IS_MODE_OFF                  = SDK_MOVIE_IS_MODE_OFF,
	XM5_MOVIE_IS_MODE_ON                   = SDK_MOVIE_IS_MODE_ON,
	XM5_MOVIE_IS_MODE_IBIS_OIS             = SDK_MOVIE_IS_MODE_IBIS_OIS,
	XM5_MOVIE_IS_MODE_IBIS_OIS_DIS         = SDK_MOVIE_IS_MODE_IBIS_OIS_DIS,
	XM5_MOVIE_IS_MODE_OIS                  = SDK_MOVIE_IS_MODE_OIS,
	XM5_MOVIE_IS_MODE_OIS_DIS              = SDK_MOVIE_IS_MODE_OIS_DIS,
};

// Movie Zebra Setting
enum{
	XM5_MOVIE_ZEBRA_SETTING_OFF            = SDK_MOVIE_ZEBRA_SETTING_OFF,
	XM5_MOVIE_ZEBRA_SETTING_RIGHT          = SDK_MOVIE_ZEBRA_SETTING_RIGHT,
	XM5_MOVIE_ZEBRA_SETTING_LEFT           = SDK_MOVIE_ZEBRA_SETTING_LEFT,
};

// Movie Tally Light
enum{
	XM5_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON      = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON,
	XM5_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK   = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK,
	XM5_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON       = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON,
	XM5_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF      = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF,
	XM5_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK,
	XM5_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF   = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF,
	XM5_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF     = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF,
};

// Media Eject Warning
enum{
	XM5_MEDIA_EJECT_WARNING_SLOT1          = SDK_MEDIA_EJECT_WARNING_SLOT1,
	XM5_MEDIA_EJECT_WARNING_SLOT2          = SDK_MEDIA_EJECT_WARNING_SLOT2,
	XM5_MEDIA_EJECT_WARNING_SLOT3          = SDK_MEDIA_EJECT_WARNING_SLOT3,
};

// Movie Data Level Setting
enum{
	XM5_MOVIE_DATA_LEVEL_SETTING_FULL      = SDK_MOVIE_DATA_LEVEL_SETTING_FULL,
	XM5_MOVIE_DATA_LEVEL_SETTING_VIDEO     = SDK_MOVIE_DATA_LEVEL_SETTING_VIDEO,
};

// Movie ImageFormat
enum{
	XM5_MOVIE_IMAGEFORMAT_GF               = SDK_MOVIE_IMAGEFORMAT_GF,
	XM5_MOVIE_IMAGEFORMAT_35MM             = SDK_MOVIE_IMAGEFORMAT_35MM,
	XM5_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM  = SDK_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM,
	XM5_MOVIE_IMAGEFORMAT_PREMISTA         = SDK_MOVIE_IMAGEFORMAT_PREMISTA,
};

// WaveForm VectorScope
enum{
	XM5_WAVEFORM_VECTORSCOPE_OFF           = SDK_WAVEFORM_VECTORSCOPE_OFF,
	XM5_WAVEFORM_VECTORSCOPE_WAVEFORM      = SDK_WAVEFORM_VECTORSCOPE_WAVEFORM,
	XM5_WAVEFORM_VECTORSCOPE_PARADE        = SDK_WAVEFORM_VECTORSCOPE_PARADE,
	XM5_WAVEFORM_VECTORSCOPE_VECTORSCOPE   = SDK_WAVEFORM_VECTORSCOPE_VECTORSCOPE,
};

// WaveForm Setting
enum{
	XM5_WAVEFORM_SETTING_PATTERN1          = SDK_WAVEFORM_SETTING_PATTERN1,
	XM5_WAVEFORM_SETTING_PATTERN2          = SDK_WAVEFORM_SETTING_PATTERN2,
	XM5_WAVEFORM_SETTING_PATTERN3          = SDK_WAVEFORM_SETTING_PATTERN3,
	XM5_WAVEFORM_SETTING_PATTERN4          = SDK_WAVEFORM_SETTING_PATTERN4,
};

// VectorScope Setting
enum{
	XM5_VECTORSCORE_SETTING_PATTERN1       = SDK_VECTORSCORE_SETTING_PATTERN1,
	XM5_VECTORSCORE_SETTING_PATTERN2       = SDK_VECTORSCORE_SETTING_PATTERN2,
	XM5_VECTORSCORE_SETTING_PATTERN3       = SDK_VECTORSCORE_SETTING_PATTERN3,
	XM5_VECTORSCORE_SETTING_PATTERN4       = SDK_VECTORSCORE_SETTING_PATTERN4,
};

// Parade Setting Display
enum{
	XM5_PARADE_SETTING_DISPLAY_PATTERN1    = SDK_PARADE_SETTING_DISPLAY_PATTERN1,
	XM5_PARADE_SETTING_DISPLAY_PATTERN2    = SDK_PARADE_SETTING_DISPLAY_PATTERN2,
	XM5_PARADE_SETTING_DISPLAY_PATTERN3    = SDK_PARADE_SETTING_DISPLAY_PATTERN3,
	XM5_PARADE_SETTING_DISPLAY_PATTERN4    = SDK_PARADE_SETTING_DISPLAY_PATTERN4,
};

// Parade Setting Color
enum{
	XM5_PARADE_SETTING_COLOR_RGB           = SDK_PARADE_SETTING_COLOR_RGB,
	XM5_PARADE_SETTING_COLOR_WHITE         = SDK_PARADE_SETTING_COLOR_WHITE,
};

// Movie FocusMeter(display)
enum{
	XM5_FOCUSMETER_DISPLAY_OFF             = SDK_FOCUSMETER_DISPLAY_OFF,
	XM5_FOCUSMETER_DISPLAY_ON              = SDK_FOCUSMETER_DISPLAY_ON,
};

// Movie FocusMeter(color)
enum{
	XM5_FOCUSMETER_COLOR_WHITE             = SDK_FOCUSMETER_COLOR_WHITE,
	XM5_FOCUSMETER_COLOR_GREEN             = SDK_FOCUSMETER_COLOR_GREEN,
};

// TimeCode Start Setting
enum{
	XM5_TIMECODE_START_SETTING_MANUAL      = SDK_TIMECODE_START_SETTING_MANUAL,
	XM5_TIMECODE_START_SETTING_CURRENT     = SDK_TIMECODE_START_SETTING_CURRENT,
	XM5_TIMECODE_START_SETTING_RESET       = SDK_TIMECODE_START_SETTING_RESET,
};

// TimeCode CountUp
enum{
	XM5_TIMECODE_COUNTUP_RECRUN            = SDK_TIMECODE_COUNTUP_RECRUN,
	XM5_TIMECODE_COUNTUP_FREERUN           = SDK_TIMECODE_COUNTUP_FREERUN,
};

// TimeCode Status
enum{
	XM5_TIMECODE_STATUS_USE_DSC_ALONE      = SDK_TIMECODE_STATUS_USE_DSC_ALONE,
	XM5_TIMECODE_STATUS_SYNCING            = SDK_TIMECODE_STATUS_SYNCING,
	XM5_TIMECODE_STATUS_DISCONNECTED       = SDK_TIMECODE_STATUS_DISCONNECTED,
	XM5_TIMECODE_STATUS_NOT_SYNCED         = SDK_TIMECODE_STATUS_NOT_SYNCED,
	XM5_TIMECODE_STATUS_FRAMERATE_MISMATCH = SDK_TIMECODE_STATUS_FRAMERATE_MISMATCH,
};

// Internal Mic Level
// External Mic Level
enum{
	XM5_MIC_LEVEL_OFF                      = SDK_MIC_LEVEL_OFF,
	XM5_MIC_LEVEL_MANUAL                   = SDK_MIC_LEVEL_MANUAL,
	XM5_MIC_LEVEL_AUTO                     = SDK_MIC_LEVEL_AUTO,
};

// HeadPhones Volume
enum{
	XM5_HEADPHONES_VOLUME_0                = SDK_HEADPHONES_VOLUME_0,
	XM5_HEADPHONES_VOLUME_1                = SDK_HEADPHONES_VOLUME_1,
	XM5_HEADPHONES_VOLUME_2                = SDK_HEADPHONES_VOLUME_2,
	XM5_HEADPHONES_VOLUME_3                = SDK_HEADPHONES_VOLUME_3,
	XM5_HEADPHONES_VOLUME_4                = SDK_HEADPHONES_VOLUME_4,
	XM5_HEADPHONES_VOLUME_5                = SDK_HEADPHONES_VOLUME_5,
	XM5_HEADPHONES_VOLUME_6                = SDK_HEADPHONES_VOLUME_6,
	XM5_HEADPHONES_VOLUME_7                = SDK_HEADPHONES_VOLUME_7,
	XM5_HEADPHONES_VOLUME_8                = SDK_HEADPHONES_VOLUME_8,
	XM5_HEADPHONES_VOLUME_9                = SDK_HEADPHONES_VOLUME_9,
	XM5_HEADPHONES_VOLUME_10               = SDK_HEADPHONES_VOLUME_10,
};

// XLR Adapter Mic Source
enum{
	XM5_XLRADAPTER_MIC_SOURCE_4CH          = SDK_XLRADAPTER_MIC_SOURCE_4CH,
	XM5_XLRADAPTER_MIC_SOURCE_2CH          = SDK_XLRADAPTER_MIC_SOURCE_2CH,
};

// XLR Adapter Monitering Source
enum{
	XM5_XLRADAPTER_MONITER_SOURCE_XLR      = SDK_XLRADAPTER_MONITER_SOURCE_XLR,
	XM5_XLRADAPTER_MONITER_SOURCE_CAMERA   = SDK_XLRADAPTER_MONITER_SOURCE_CAMERA,
};

// XLR Adapter HDMI Output Source
enum{
	XM5_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR   = SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR,
	XM5_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA= SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA,
};

// Directional Mic
enum{
	XM5_DIRECTIONAL_MIC_AUTO               = SDK_DIRECTIONAL_MIC_AUTO,
	XM5_DIRECTIONAL_MIC_SURROUND           = SDK_DIRECTIONAL_MIC_SURROUND,
	XM5_DIRECTIONAL_MIC_FRONT              = SDK_DIRECTIONAL_MIC_FRONT,
	XM5_DIRECTIONAL_MIC_TRACKING           = SDK_DIRECTIONAL_MIC_TRACKING,
	XM5_DIRECTIONAL_MIC_BACK               = SDK_DIRECTIONAL_MIC_BACK,
	XM5_DIRECTIONAL_MIC_FRONT_BACK         = SDK_DIRECTIONAL_MIC_FRONT_BACK,
};

// Body Temperature Warning
enum{
	XM5_BODY_TEMPERATURE_WARNING_NONE      = SDK_BODY_TEMPERATURE_WARNING_NONE,
	XM5_BODY_TEMPERATURE_WARNING_YELLOW    = SDK_BODY_TEMPERATURE_WARNING_YELLOW,
	XM5_BODY_TEMPERATURE_WARNING_RED       = SDK_BODY_TEMPERATURE_WARNING_RED,
};

// Movie Rec Volume
enum{
	XM5_MOVIE_REC_VOLUME_OFF               = SDK_MOVIE_REC_VOLUME_OFF,
	XM5_MOVIE_REC_VOLUME_1                 = SDK_MOVIE_REC_VOLUME_1,
	XM5_MOVIE_REC_VOLUME_2                 = SDK_MOVIE_REC_VOLUME_2,
	XM5_MOVIE_REC_VOLUME_3                 = SDK_MOVIE_REC_VOLUME_3,
};
