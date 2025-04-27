///////////////////////////////////////////////////
//  X-T5_MOV.h
//     Version 1.0.0.0
//
//     Copyright (C) 2022 FUJIFILM Corporation.
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
typedef  SDK_MOVIE_AFC_CUSTOM     XT5_SDK_MOVIE_AFC_CUSTOM, *PXT5_SDK_MOVIE_AFC_CUSTOM;

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
typedef  SDK_MICLEVEL_INDICATOR     XT5_SDK_MICLEVEL_INDICATOR, *PXT5_SDK_MICLEVEL_INDICATOR;

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
typedef  SDK_PlayBackObjectInformation     XT5_SDK_PlayBackObjectInformation, *PXT5_SDK_PlayBackObjectInformation;

// Focus Map Data
//typedef	struct _SDK_FocusMapData {
//	long lDistance;
//  long lColorR;
//	long lColorG;
//	long lColorB;
//	long lAlpha;
//} SDK_FocusMapData;
typedef  SDK_FocusMapData     XT5_SDK_FocusMapData, *PXT5_SDK_FocusMapData;

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
typedef  SDK_TrackingAfFrameInfo     XT5_SDK_TrackingAfFrameInfo, *PXT5_SDK_TrackingAfFrameInfo;

// Histogram Data
//typedef struct SDK_HistogramData {
//	long lLuminance;
//	long lColorR;
//	long lColorG;
//	long lColorB;
//} SDK_HistogramData;
typedef  SDK_HistogramData     XT5_SDK_HistogramData, *PXT5_SDK_HistogramData;


//
// API_PARAM for X-T5
//
enum {
	XT5_API_PARAM_CapMovieHighFrequencyFlickerlessMode = 2,
	XT5_API_PARAM_SetMovieHighFrequencyFlickerlessMode = 1,
	XT5_API_PARAM_GetMovieHighFrequencyFlickerlessMode = 1,
	XT5_API_PARAM_CapMovieAFMode                       = 2,
	XT5_API_PARAM_SetMovieAFMode                       = 1,
	XT5_API_PARAM_GetMovieAFMode                       = 1,
	XT5_API_PARAM_CapMovieFocusArea                    = 3,
	XT5_API_PARAM_SetMovieFocusArea                    = 2,
	XT5_API_PARAM_GetMovieFocusArea                    = 2,
	XT5_API_PARAM_GetTrackingAfFrameInfo               = 1,
	XT5_API_PARAM_CapMovieResolution                   = 2,
	XT5_API_PARAM_SetMovieResolution                   = 1,
	XT5_API_PARAM_GetMovieResolution                   = 1,
	XT5_API_PARAM_CapMovieFrameRate                    = 2,
	XT5_API_PARAM_SetMovieFrameRate                    = 1,
	XT5_API_PARAM_GetMovieFrameRate                    = 1,
	XT5_API_PARAM_CapMovieBitRate                      = 2,
	XT5_API_PARAM_SetMovieBitRate                      = 1,
	XT5_API_PARAM_GetMovieBitRate                      = 1,
	XT5_API_PARAM_CapMovieFileFormat                   = 2,
	XT5_API_PARAM_SetMovieFileFormat                   = 1,
	XT5_API_PARAM_GetMovieFileFormat                   = 1,
	XT5_API_PARAM_CapHighSpeedRecMode                  = 2,
	XT5_API_PARAM_SetHighSpeedRecMode                  = 1,
	XT5_API_PARAM_GetHighSpeedRecMode                  = 1,
	XT5_API_PARAM_CapHighSpeedRecResolution            = 2,
	XT5_API_PARAM_SetHighSpeedRecResolution            = 1,
	XT5_API_PARAM_GetHighSpeedRecResolution            = 1,
	XT5_API_PARAM_CapHighSpeedRecFrameRate             = 2,
	XT5_API_PARAM_SetHighSpeedRecFrameRate             = 1,
	XT5_API_PARAM_GetHighSpeedRecFrameRate             = 1,
	XT5_API_PARAM_CapHighSpeedRecPlayBackFrameRate     = 2,
	XT5_API_PARAM_SetHighSpeedRecPlayBackFrameRate     = 1,
	XT5_API_PARAM_GetHighSpeedRecPlayBackFrameRate     = 1,
	XT5_API_PARAM_CapMovieMediaRecord                  = 2,
	XT5_API_PARAM_SetMovieMediaRecord                  = 1,
	XT5_API_PARAM_GetMovieMediaRecord                  = 1,
	XT5_API_PARAM_CapMovieMediaRecordProRes            = 2,
	XT5_API_PARAM_SetMovieMediaRecordProRes            = 1,
	XT5_API_PARAM_GetMovieMediaRecordProRes            = 1,
	XT5_API_PARAM_CapMovieHDMIOutputRAW                = 2,
	XT5_API_PARAM_SetMovieHDMIOutputRAW                = 1,
	XT5_API_PARAM_GetMovieHDMIOutputRAW                = 1,
	XT5_API_PARAM_CapMovieHDMIOutputRAWResolution      = 2,
	XT5_API_PARAM_SetMovieHDMIOutputRAWResolution      = 1,
	XT5_API_PARAM_GetMovieHDMIOutputRAWResolution      = 1,
	XT5_API_PARAM_CapMovieHDMIOutputRAWFrameRate       = 2,
	XT5_API_PARAM_SetMovieHDMIOutputRAWFrameRate       = 1,
	XT5_API_PARAM_GetMovieHDMIOutputRAWFrameRate       = 1,
	XT5_API_PARAM_CapFlogRecording                     = 2,
	XT5_API_PARAM_SetFlogRecording                     = 1,
	XT5_API_PARAM_GetFlogRecording                     = 1,
	XT5_API_PARAM_CapMovieCropMagnification            = 2,
	XT5_API_PARAM_SetMovieCropMagnification            = 1,
	XT5_API_PARAM_GetMovieCropMagnification            = 1,
	XT5_API_PARAM_CapMovieHDMIOutputInfoDisplay        = 2,
	XT5_API_PARAM_SetMovieHDMIOutputInfoDisplay        = 1,
	XT5_API_PARAM_GetMovieHDMIOutputInfoDisplay        = 1,
	XT5_API_PARAM_CapMovieHDMIRecControl               = 2,
	XT5_API_PARAM_SetMovieHDMIRecControl               = 1,
	XT5_API_PARAM_GetMovieHDMIRecControl               = 1,
	XT5_API_PARAM_CapMovieIsMode                       = 2,
	XT5_API_PARAM_SetMovieIsMode                       = 1,
	XT5_API_PARAM_GetMovieIsMode                       = 1,
	XT5_API_PARAM_CapMovieIsModeBoost                  = 2,
	XT5_API_PARAM_SetMovieIsModeBoost                  = 1,
	XT5_API_PARAM_GetMovieIsModeBoost                  = 1,
	XT5_API_PARAM_CapMovieZebraSetting                 = 2,
	XT5_API_PARAM_SetMovieZebraSetting                 = 1,
	XT5_API_PARAM_GetMovieZebraSetting                 = 1,
	XT5_API_PARAM_CapMovieZebraLevel                   = 2,
	XT5_API_PARAM_SetMovieZebraLevel                   = 1,
	XT5_API_PARAM_GetMovieZebraLevel                   = 1,
	XT5_API_PARAM_CapMovieOptimizedControl             = 2,
	XT5_API_PARAM_SetMovieOptimizedControl             = 1,
	XT5_API_PARAM_GetMovieOptimizedControl             = 1,
	XT5_API_PARAM_CapMovieTallyLight                   = 2,
	XT5_API_PARAM_SetMovieTallyLight                   = 1,
	XT5_API_PARAM_GetMovieTallyLight                   = 1,
	XT5_API_PARAM_GetMediaEjectWarning                 = 1,
	XT5_API_PARAM_CapMovieDataLevelSetting             = 2,
	XT5_API_PARAM_SetMovieDataLevelSetting             = 1,
	XT5_API_PARAM_GetMovieDataLevelSetting             = 1,
	XT5_API_PARAM_GetMovieCropMagnificationValue       = 1,
	XT5_API_PARAM_CapMovieFullTimeManual               = 2,
	XT5_API_PARAM_SetMovieFullTimeManual               = 1,
	XT5_API_PARAM_GetMovieFullTimeManual               = 1,
	XT5_API_PARAM_CapMovieDigitalZoom                  = 2,
	XT5_API_PARAM_SetMovieDigitalZoom                  = 1,
	XT5_API_PARAM_GetMovieDigitalZoom                  = 1,
	XT5_API_PARAM_GetMovieDigitalZoomRange             = 3,
	XT5_API_PARAM_CapMovieImageFormat                  = 2,
	XT5_API_PARAM_SetMovieImageFormat                  = 1,
	XT5_API_PARAM_GetMovieImageFormat                  = 1,
	XT5_API_PARAM_GetHistogramData                     = 2,
	XT5_API_PARAM_GetFocusMapData                      = 2,
	XT5_API_PARAM_CapWaveFormVectorScope               = 2,
	XT5_API_PARAM_SetWaveFormVectorScope               = 1,
	XT5_API_PARAM_GetWaveFormVectorScope               = 1,
	XT5_API_PARAM_GetWaveFormData                      = 2,
	XT5_API_PARAM_GetVectorScopeData                   = 2,
	XT5_API_PARAM_GetParadeData                        = 2,
	XT5_API_PARAM_CapRecFrameIndicator                 = 2,
	XT5_API_PARAM_SetRecFrameIndicator                 = 1,
	XT5_API_PARAM_GetRecFrameIndicator                 = 1,
	XT5_API_PARAM_CapMovieCaptureDelay                 = 2,
	XT5_API_PARAM_SetMovieCaptureDelay                 = 1,
	XT5_API_PARAM_GetMovieCaptureDelay                 = 1,
	XT5_API_PARAM_CapAnamorphicDesqueezeDisplay        = 2,
	XT5_API_PARAM_SetAnamorphicDesqueezeDisplay        = 1,
	XT5_API_PARAM_GetAnamorphicDesqueezeDisplay        = 1,
	XT5_API_PARAM_CapAnamorphicMagnification           = 2,
	XT5_API_PARAM_SetAnamorphicMagnification           = 1,
	XT5_API_PARAM_GetAnamorphicMagnification           = 1,
	XT5_API_PARAM_CapWaveFormSetting                   = 2,
	XT5_API_PARAM_SetWaveFormSetting                   = 1,
	XT5_API_PARAM_GetWaveFormSetting                   = 1,
	XT5_API_PARAM_CapVectorScopeSetting                = 2,
	XT5_API_PARAM_SetVectorScopeSetting                = 1,
	XT5_API_PARAM_GetVectorScopeSetting                = 1,
	XT5_API_PARAM_CapParadeSettingDisplay              = 2,
	XT5_API_PARAM_SetParadeSettingDisplay              = 1,
	XT5_API_PARAM_GetParadeSettingDisplay              = 1,
	XT5_API_PARAM_CapParadeSettingColor                = 2,
	XT5_API_PARAM_SetParadeSettingColor                = 1,
	XT5_API_PARAM_GetParadeSettingColor                = 1,
	XT5_API_PARAM_CapFlogDRangePriority                = 2,
	XT5_API_PARAM_SetFlogDRangePriority                = 1,
	XT5_API_PARAM_GetFlogDRangePriority                = 1,
	XT5_API_PARAM_CapMovieWhiteBalanceTune             = 5,
	XT5_API_PARAM_SetMovieWhiteBalanceTune             = 3,
	XT5_API_PARAM_GetMovieWhiteBalanceTune             = 3,
	XT5_API_PARAM_CapMovieFilmSimulationMode           = 2,
	XT5_API_PARAM_SetMovieFilmSimulationMode           = 1,
	XT5_API_PARAM_GetMovieFilmSimulationMode           = 1,
	XT5_API_PARAM_CapMovieMonochromaticColor           = 4,
	XT5_API_PARAM_SetMovieMonochromaticColor           = 2,
	XT5_API_PARAM_GetMovieMonochromaticColor           = 2,
	XT5_API_PARAM_CapMovieHighLightTone                = 2,
	XT5_API_PARAM_SetMovieHighLightTone                = 1,
	XT5_API_PARAM_GetMovieHighLightTone                = 1,
	XT5_API_PARAM_CapMovieShadowTone                   = 2,
	XT5_API_PARAM_SetMovieShadowTone                   = 1,
	XT5_API_PARAM_GetMovieShadowTone                   = 1,
	XT5_API_PARAM_CapMovieSharpness                    = 2,
	XT5_API_PARAM_SetMovieSharpness                    = 1,
	XT5_API_PARAM_GetMovieSharpness                    = 1,
	XT5_API_PARAM_CapMovieColorMode                    = 2,
	XT5_API_PARAM_SetMovieColorMode                    = 1,
	XT5_API_PARAM_GetMovieColorMode                    = 1,
	XT5_API_PARAM_CapMovieNoiseReduction               = 2,
	XT5_API_PARAM_SetMovieNoiseReduction               = 1,
	XT5_API_PARAM_GetMovieNoiseReduction               = 1,
	XT5_API_PARAM_CapInterFrameNR                      = 2,
	XT5_API_PARAM_SetInterFrameNR                      = 1,
	XT5_API_PARAM_GetInterFrameNR                      = 1,
	XT5_API_PARAM_CapMoviePeripheralLightCorrection    = 2,
	XT5_API_PARAM_SetMoviePeripheralLightCorrection    = 1,
	XT5_API_PARAM_GetMoviePeripheralLightCorrection    = 1,
	XT5_API_PARAM_CapMovieFaceDetectionMode            = 2,
	XT5_API_PARAM_SetMovieFaceDetectionMode            = 1,
	XT5_API_PARAM_GetMovieFaceDetectionMode            = 1,
	XT5_API_PARAM_CapMovieEyeAFMode                    = 2,
	XT5_API_PARAM_SetMovieEyeAFMode                    = 1,
	XT5_API_PARAM_GetMovieEyeAFMode                    = 1,
	XT5_API_PARAM_CapMovieSubjectDetectionMode         = 2,
	XT5_API_PARAM_SetMovieSubjectDetectionMode         = 1,
	XT5_API_PARAM_GetMovieSubjectDetectionMode         = 1,
	XT5_API_PARAM_CapMovieAFCCustom                    = 2,
	XT5_API_PARAM_SetMovieAFCCustom                    = 1,
	XT5_API_PARAM_GetMovieAFCCustom                    = 1,
	XT5_API_PARAM_CapMovieMFAssistMode                 = 2,
	XT5_API_PARAM_SetMovieMFAssistMode                 = 1,
	XT5_API_PARAM_GetMovieMFAssistMode                 = 1,
	XT5_API_PARAM_GetMovieFocusMeter                   = 3,
	XT5_API_PARAM_CapMovieFocusCheckMode               = 2,
	XT5_API_PARAM_SetMovieFocusCheckMode               = 1,
	XT5_API_PARAM_GetMovieFocusCheckMode               = 1,
	XT5_API_PARAM_CapMovieFocusCheckLock               = 2,
	XT5_API_PARAM_SetMovieFocusCheckLock               = 1,
	XT5_API_PARAM_GetMovieFocusCheckLock               = 1,
	XT5_API_PARAM_GetMovieRecordingTime                = 3,
	XT5_API_PARAM_GetMovieRemainingTime                = 3,
	XT5_API_PARAM_GetTimeCode                          = 4,
	XT5_API_PARAM_CapTimeCodeDisplay                   = 2,
	XT5_API_PARAM_SetTimeCodeDisplay                   = 1,
	XT5_API_PARAM_GetTimeCodeDisplay                   = 1,
	XT5_API_PARAM_CapTimeCodeStartSetting              = 2,
	XT5_API_PARAM_SetTimeCodeStartSetting              = 5,
	XT5_API_PARAM_CapTimeCodeCountUp                   = 2,
	XT5_API_PARAM_SetTimeCodeCountUp                   = 1,
	XT5_API_PARAM_GetTimeCodeCountUp                   = 1,
	XT5_API_PARAM_CapTimeCodeDropFrame                 = 2,
	XT5_API_PARAM_SetTimeCodeDropFrame                 = 1,
	XT5_API_PARAM_GetTimeCodeDropFrame                 = 1,
	XT5_API_PARAM_CapTimeCodeHDMIOutput                = 2,
	XT5_API_PARAM_SetTimeCodeHDMIOutput                = 1,
	XT5_API_PARAM_GetTimeCodeHDMIOutput                = 1,
	XT5_API_PARAM_GetTimeCodeCurrentValue              = 4,
	XT5_API_PARAM_CapATOMOSAirGluConnection            = 2,
	XT5_API_PARAM_SetATOMOSAirGluConnection            = 1,
	XT5_API_PARAM_GetATOMOSAirGluConnection            = 1,
	XT5_API_PARAM_GetTimeCodeStatus                    = 1,
	XT5_API_PARAM_CapInternalMicLevel                  = 2,
	XT5_API_PARAM_SetInternalMicLevel                  = 1,
	XT5_API_PARAM_GetInternalMicLevel                  = 1,
	XT5_API_PARAM_CapInternalMicLevelManual            = 2,
	XT5_API_PARAM_SetInternalMicLevelManual            = 1,
	XT5_API_PARAM_GetInternalMicLevelManual            = 1,
	XT5_API_PARAM_CapExternalMicLevel                  = 2,
	XT5_API_PARAM_SetExternalMicLevel                  = 1,
	XT5_API_PARAM_GetExternalMicLevel                  = 1,
	XT5_API_PARAM_CapExternalMicLevelManual            = 2,
	XT5_API_PARAM_SetExternalMicLevelManual            = 1,
	XT5_API_PARAM_GetExternalMicLevelManual            = 1,
	XT5_API_PARAM_CapMicLevelLimiter                   = 2,
	XT5_API_PARAM_SetMicLevelLimiter                   = 1,
	XT5_API_PARAM_GetMicLevelLimiter                   = 1,
	XT5_API_PARAM_CapWindFilter                        = 2,
	XT5_API_PARAM_SetWindFilter                        = 1,
	XT5_API_PARAM_GetWindFilter                        = 1,
	XT5_API_PARAM_CapLowCutFilter                      = 2,
	XT5_API_PARAM_SetLowCutFilter                      = 1,
	XT5_API_PARAM_GetLowCutFilter                      = 1,
	XT5_API_PARAM_CapHeadPhonesVolume                  = 2,
	XT5_API_PARAM_SetHeadPhonesVolume                  = 1,
	XT5_API_PARAM_GetHeadPhonesVolume                  = 1,
	XT5_API_PARAM_CapXLRAdapterMicSource               = 2,
	XT5_API_PARAM_SetXLRAdapterMicSource               = 1,
	XT5_API_PARAM_GetXLRAdapterMicSource               = 1,
	XT5_API_PARAM_CapXLRAdapterMoniteringSource        = 2,
	XT5_API_PARAM_SetXLRAdapterMoniteringSource        = 1,
	XT5_API_PARAM_GetXLRAdapterMoniteringSource        = 1,
	XT5_API_PARAM_CapXLRAdapterHDMIOutputSource        = 2,
	XT5_API_PARAM_SetXLRAdapterHDMIOutputSource        = 1,
	XT5_API_PARAM_GetXLRAdapterHDMIOutputSource        = 1,
	XT5_API_PARAM_GetMicLevelIndicator                 = 1,
	XT5_API_PARAM_GetMovieTransparentFrameInfo         = -1,
	XT5_API_PARAM_CapShortMovieSecond                  = -1,
	XT5_API_PARAM_SetShortMovieSecond                  = -1,
	XT5_API_PARAM_GetShortMovieSecond                  = -1,
	XT5_API_PARAM_CapMoviePortraitEnhancer             = -1,
	XT5_API_PARAM_SetMoviePortraitEnhancer             = -1,
	XT5_API_PARAM_GetMoviePortraitEnhancer             = -1,
	XT5_API_PARAM_CapDirectionalMic                    = 2,
	XT5_API_PARAM_SetDirectionalMic                    = 1,
	XT5_API_PARAM_GetDirectionalMic                    = 1,
	XT5_API_PARAM_CapEnvSoundsNR                       = -1,
	XT5_API_PARAM_SetEnvSoundsNR                       = -1,
	XT5_API_PARAM_GetEnvSoundsNR                       = -1,
	XT5_API_PARAM_GetBodyTemperatureWarning            = 1,
	XT5_API_PARAM_GetPlayBackMediaInfo                 = -1,
	XT5_API_PARAM_GetPlayBackObjectInfo                = -1,
	XT5_API_PARAM_GetPlayBackThumb                     = -1,
	XT5_API_PARAM_GetPlayBackPartialObject             = -1,
	XT5_API_PARAM_CapMovieRecVolume                    = 2,
	XT5_API_PARAM_SetMovieRecVolume                    = 1,
	XT5_API_PARAM_GetMovieRecVolume                    = 1,
};

//
// API_CODE for X-T5
//
enum {
	XT5_API_CODE_CapMovieHighFrequencyFlickerlessMode   = API_CODE_CapMovieHighFrequencyFlickerlessMode,
	XT5_API_CODE_SetMovieHighFrequencyFlickerlessMode   = API_CODE_SetMovieHighFrequencyFlickerlessMode,
	XT5_API_CODE_GetMovieHighFrequencyFlickerlessMode   = API_CODE_GetMovieHighFrequencyFlickerlessMode,
	XT5_API_CODE_CapMovieAFMode                         = API_CODE_CapMovieAFMode,
	XT5_API_CODE_SetMovieAFMode                         = API_CODE_SetMovieAFMode,
	XT5_API_CODE_GetMovieAFMode                         = API_CODE_GetMovieAFMode,
	XT5_API_CODE_CapMovieFocusArea                      = API_CODE_CapMovieFocusArea,
	XT5_API_CODE_SetMovieFocusArea                      = API_CODE_SetMovieFocusArea,
	XT5_API_CODE_GetMovieFocusArea                      = API_CODE_GetMovieFocusArea,
	XT5_API_CODE_GetTrackingAfFrameInfo                 = API_CODE_GetTrackingAfFrameInfo,
	XT5_API_CODE_CapMovieResolution                     = API_CODE_CapMovieResolution,
	XT5_API_CODE_SetMovieResolution                     = API_CODE_SetMovieResolution,
	XT5_API_CODE_GetMovieResolution                     = API_CODE_GetMovieResolution,
	XT5_API_CODE_CapMovieFrameRate                      = API_CODE_CapMovieFrameRate,
	XT5_API_CODE_SetMovieFrameRate                      = API_CODE_SetMovieFrameRate,
	XT5_API_CODE_GetMovieFrameRate                      = API_CODE_GetMovieFrameRate,
	XT5_API_CODE_CapMovieBitRate                        = API_CODE_CapMovieBitRate,
	XT5_API_CODE_SetMovieBitRate                        = API_CODE_SetMovieBitRate,
	XT5_API_CODE_GetMovieBitRate                        = API_CODE_GetMovieBitRate,
	XT5_API_CODE_CapMovieFileFormat                     = API_CODE_CapMovieFileFormat,
	XT5_API_CODE_SetMovieFileFormat                     = API_CODE_SetMovieFileFormat,
	XT5_API_CODE_GetMovieFileFormat                     = API_CODE_GetMovieFileFormat,
	XT5_API_CODE_CapHighSpeedRecMode                    = API_CODE_CapHighSpeedRecMode,
	XT5_API_CODE_SetHighSpeedRecMode                    = API_CODE_SetHighSpeedRecMode,
	XT5_API_CODE_GetHighSpeedRecMode                    = API_CODE_GetHighSpeedRecMode,
	XT5_API_CODE_CapHighSpeedRecResolution              = API_CODE_CapHighSpeedRecResolution,
	XT5_API_CODE_SetHighSpeedRecResolution              = API_CODE_SetHighSpeedRecResolution,
	XT5_API_CODE_GetHighSpeedRecResolution              = API_CODE_GetHighSpeedRecResolution,
	XT5_API_CODE_CapHighSpeedRecFrameRate               = API_CODE_CapHighSpeedRecFrameRate,
	XT5_API_CODE_SetHighSpeedRecFrameRate               = API_CODE_SetHighSpeedRecFrameRate,
	XT5_API_CODE_GetHighSpeedRecFrameRate               = API_CODE_GetHighSpeedRecFrameRate,
	XT5_API_CODE_CapHighSpeedRecPlayBackFrameRate       = API_CODE_CapHighSpeedRecPlayBackFrameRate,
	XT5_API_CODE_SetHighSpeedRecPlayBackFrameRate       = API_CODE_SetHighSpeedRecPlayBackFrameRate,
	XT5_API_CODE_GetHighSpeedRecPlayBackFrameRate       = API_CODE_GetHighSpeedRecPlayBackFrameRate,
	XT5_API_CODE_CapMovieMediaRecord                    = API_CODE_CapMovieMediaRecord,
	XT5_API_CODE_SetMovieMediaRecord                    = API_CODE_SetMovieMediaRecord,
	XT5_API_CODE_GetMovieMediaRecord                    = API_CODE_GetMovieMediaRecord,
	XT5_API_CODE_CapMovieMediaRecordProRes              = API_CODE_CapMovieMediaRecordProRes,
	XT5_API_CODE_SetMovieMediaRecordProRes              = API_CODE_SetMovieMediaRecordProRes,
	XT5_API_CODE_GetMovieMediaRecordProRes              = API_CODE_GetMovieMediaRecordProRes,
	XT5_API_CODE_CapMovieHDMIOutputRAW                  = API_CODE_CapMovieHDMIOutputRAW,
	XT5_API_CODE_SetMovieHDMIOutputRAW                  = API_CODE_SetMovieHDMIOutputRAW,
	XT5_API_CODE_GetMovieHDMIOutputRAW                  = API_CODE_GetMovieHDMIOutputRAW,
	XT5_API_CODE_CapMovieHDMIOutputRAWResolution        = API_CODE_CapMovieHDMIOutputRAWResolution,
	XT5_API_CODE_SetMovieHDMIOutputRAWResolution        = API_CODE_SetMovieHDMIOutputRAWResolution,
	XT5_API_CODE_GetMovieHDMIOutputRAWResolution        = API_CODE_GetMovieHDMIOutputRAWResolution,
	XT5_API_CODE_CapMovieHDMIOutputRAWFrameRate         = API_CODE_CapMovieHDMIOutputRAWFrameRate,
	XT5_API_CODE_SetMovieHDMIOutputRAWFrameRate         = API_CODE_SetMovieHDMIOutputRAWFrameRate,
	XT5_API_CODE_GetMovieHDMIOutputRAWFrameRate         = API_CODE_GetMovieHDMIOutputRAWFrameRate,
	XT5_API_CODE_CapFlogRecording                       = API_CODE_CapFlogRecording,
	XT5_API_CODE_SetFlogRecording                       = API_CODE_SetFlogRecording,
	XT5_API_CODE_GetFlogRecording                       = API_CODE_GetFlogRecording,
	XT5_API_CODE_CapMovieCropMagnification              = API_CODE_CapMovieCropMagnification,
	XT5_API_CODE_SetMovieCropMagnification              = API_CODE_SetMovieCropMagnification,
	XT5_API_CODE_GetMovieCropMagnification              = API_CODE_GetMovieCropMagnification,
	XT5_API_CODE_CapMovieHDMIOutputInfoDisplay          = API_CODE_CapMovieHDMIOutputInfoDisplay,
	XT5_API_CODE_SetMovieHDMIOutputInfoDisplay          = API_CODE_SetMovieHDMIOutputInfoDisplay,
	XT5_API_CODE_GetMovieHDMIOutputInfoDisplay          = API_CODE_GetMovieHDMIOutputInfoDisplay,
	XT5_API_CODE_CapMovieHDMIRecControl                 = API_CODE_CapMovieHDMIRecControl,
	XT5_API_CODE_SetMovieHDMIRecControl                 = API_CODE_SetMovieHDMIRecControl,
	XT5_API_CODE_GetMovieHDMIRecControl                 = API_CODE_GetMovieHDMIRecControl,
	XT5_API_CODE_CapMovieIsMode                         = API_CODE_CapMovieIsMode,
	XT5_API_CODE_SetMovieIsMode                         = API_CODE_SetMovieIsMode,
	XT5_API_CODE_GetMovieIsMode                         = API_CODE_GetMovieIsMode,
	XT5_API_CODE_CapMovieIsModeBoost                    = API_CODE_CapMovieIsModeBoost,
	XT5_API_CODE_SetMovieIsModeBoost                    = API_CODE_SetMovieIsModeBoost,
	XT5_API_CODE_GetMovieIsModeBoost                    = API_CODE_GetMovieIsModeBoost,
	XT5_API_CODE_CapMovieZebraSetting                   = API_CODE_CapMovieZebraSetting,
	XT5_API_CODE_SetMovieZebraSetting                   = API_CODE_SetMovieZebraSetting,
	XT5_API_CODE_GetMovieZebraSetting                   = API_CODE_GetMovieZebraSetting,
	XT5_API_CODE_CapMovieZebraLevel                     = API_CODE_CapMovieZebraLevel,
	XT5_API_CODE_SetMovieZebraLevel                     = API_CODE_SetMovieZebraLevel,
	XT5_API_CODE_GetMovieZebraLevel                     = API_CODE_GetMovieZebraLevel,
	XT5_API_CODE_CapMovieOptimizedControl               = API_CODE_CapMovieOptimizedControl,
	XT5_API_CODE_SetMovieOptimizedControl               = API_CODE_SetMovieOptimizedControl,
	XT5_API_CODE_GetMovieOptimizedControl               = API_CODE_GetMovieOptimizedControl,
	XT5_API_CODE_CapMovieTallyLight                     = API_CODE_CapMovieTallyLight,
	XT5_API_CODE_SetMovieTallyLight                     = API_CODE_SetMovieTallyLight,
	XT5_API_CODE_GetMovieTallyLight                     = API_CODE_GetMovieTallyLight,
	XT5_API_CODE_GetMediaEjectWarning                   = API_CODE_GetMediaEjectWarning,
	XT5_API_CODE_CapMovieDataLevelSetting               = API_CODE_CapMovieDataLevelSetting,
	XT5_API_CODE_SetMovieDataLevelSetting               = API_CODE_SetMovieDataLevelSetting,
	XT5_API_CODE_GetMovieDataLevelSetting               = API_CODE_GetMovieDataLevelSetting,
	XT5_API_CODE_GetMovieCropMagnificationValue         = API_CODE_GetMovieCropMagnificationValue,
	XT5_API_CODE_CapMovieFullTimeManual                 = API_CODE_CapMovieFullTimeManual,
	XT5_API_CODE_SetMovieFullTimeManual                 = API_CODE_SetMovieFullTimeManual,
	XT5_API_CODE_GetMovieFullTimeManual                 = API_CODE_GetMovieFullTimeManual,
	XT5_API_CODE_CapMovieDigitalZoom                    = API_CODE_CapMovieDigitalZoom,
	XT5_API_CODE_SetMovieDigitalZoom                    = API_CODE_SetMovieDigitalZoom,
	XT5_API_CODE_GetMovieDigitalZoom                    = API_CODE_GetMovieDigitalZoom,
	XT5_API_CODE_GetMovieDigitalZoomRange               = API_CODE_GetMovieDigitalZoomRange,
	XT5_API_CODE_CapMovieImageFormat                    = API_CODE_CapMovieImageFormat,
	XT5_API_CODE_SetMovieImageFormat                    = API_CODE_SetMovieImageFormat,
	XT5_API_CODE_GetMovieImageFormat                    = API_CODE_GetMovieImageFormat,
	XT5_API_CODE_GetHistogramData                       = API_CODE_GetHistogramData,
	XT5_API_CODE_GetFocusMapData                        = API_CODE_GetFocusMapData,
	XT5_API_CODE_CapWaveFormVectorScope                 = API_CODE_CapWaveFormVectorScope,
	XT5_API_CODE_SetWaveFormVectorScope                 = API_CODE_SetWaveFormVectorScope,
	XT5_API_CODE_GetWaveFormVectorScope                 = API_CODE_GetWaveFormVectorScope,
	XT5_API_CODE_GetWaveFormData                        = API_CODE_GetWaveFormData,
	XT5_API_CODE_GetVectorScopeData                     = API_CODE_GetVectorScopeData,
	XT5_API_CODE_GetParadeData                          = API_CODE_GetParadeData,
	XT5_API_CODE_CapRecFrameIndicator                   = API_CODE_CapRecFrameIndicator,
	XT5_API_CODE_SetRecFrameIndicator                   = API_CODE_SetRecFrameIndicator,
	XT5_API_CODE_GetRecFrameIndicator                   = API_CODE_GetRecFrameIndicator,
	XT5_API_CODE_CapMovieCaptureDelay                   = API_CODE_CapMovieCaptureDelay,
	XT5_API_CODE_SetMovieCaptureDelay                   = API_CODE_SetMovieCaptureDelay,
	XT5_API_CODE_GetMovieCaptureDelay                   = API_CODE_GetMovieCaptureDelay,
	XT5_API_CODE_CapAnamorphicDesqueezeDisplay          = API_CODE_CapAnamorphicDesqueezeDisplay,
	XT5_API_CODE_SetAnamorphicDesqueezeDisplay          = API_CODE_SetAnamorphicDesqueezeDisplay,
	XT5_API_CODE_GetAnamorphicDesqueezeDisplay          = API_CODE_GetAnamorphicDesqueezeDisplay,
	XT5_API_CODE_CapAnamorphicMagnification             = API_CODE_CapAnamorphicMagnification,
	XT5_API_CODE_SetAnamorphicMagnification             = API_CODE_SetAnamorphicMagnification,
	XT5_API_CODE_GetAnamorphicMagnification             = API_CODE_GetAnamorphicMagnification,
	XT5_API_CODE_CapWaveFormSetting                     = API_CODE_CapWaveFormSetting,
	XT5_API_CODE_SetWaveFormSetting                     = API_CODE_SetWaveFormSetting,
	XT5_API_CODE_GetWaveFormSetting                     = API_CODE_GetWaveFormSetting,
	XT5_API_CODE_CapVectorScopeSetting                  = API_CODE_CapVectorScopeSetting,
	XT5_API_CODE_SetVectorScopeSetting                  = API_CODE_SetVectorScopeSetting,
	XT5_API_CODE_GetVectorScopeSetting                  = API_CODE_GetVectorScopeSetting,
	XT5_API_CODE_CapParadeSettingDisplay                = API_CODE_CapParadeSettingDisplay,
	XT5_API_CODE_SetParadeSettingDisplay                = API_CODE_SetParadeSettingDisplay,
	XT5_API_CODE_GetParadeSettingDisplay                = API_CODE_GetParadeSettingDisplay,
	XT5_API_CODE_CapParadeSettingColor                  = API_CODE_CapParadeSettingColor,
	XT5_API_CODE_SetParadeSettingColor                  = API_CODE_SetParadeSettingColor,
	XT5_API_CODE_GetParadeSettingColor                  = API_CODE_GetParadeSettingColor,
	XT5_API_CODE_CapFlogDRangePriority                  = API_CODE_CapFlogDRangePriority,
	XT5_API_CODE_SetFlogDRangePriority                  = API_CODE_SetFlogDRangePriority,
	XT5_API_CODE_GetFlogDRangePriority                  = API_CODE_GetFlogDRangePriority,
	XT5_API_CODE_CapMovieWhiteBalanceTune               = API_CODE_CapMovieWhiteBalanceTune,
	XT5_API_CODE_SetMovieWhiteBalanceTune               = API_CODE_SetMovieWhiteBalanceTune,
	XT5_API_CODE_GetMovieWhiteBalanceTune               = API_CODE_GetMovieWhiteBalanceTune,
	XT5_API_CODE_CapMovieFilmSimulationMode             = API_CODE_CapMovieFilmSimulationMode,
	XT5_API_CODE_SetMovieFilmSimulationMode             = API_CODE_SetMovieFilmSimulationMode,
	XT5_API_CODE_GetMovieFilmSimulationMode             = API_CODE_GetMovieFilmSimulationMode,
	XT5_API_CODE_CapMovieMonochromaticColor             = API_CODE_CapMovieMonochromaticColor,
	XT5_API_CODE_SetMovieMonochromaticColor             = API_CODE_SetMovieMonochromaticColor,
	XT5_API_CODE_GetMovieMonochromaticColor             = API_CODE_GetMovieMonochromaticColor,
	XT5_API_CODE_CapMovieHighLightTone                  = API_CODE_CapMovieHighLightTone,
	XT5_API_CODE_SetMovieHighLightTone                  = API_CODE_SetMovieHighLightTone,
	XT5_API_CODE_GetMovieHighLightTone                  = API_CODE_GetMovieHighLightTone,
	XT5_API_CODE_CapMovieShadowTone                     = API_CODE_CapMovieShadowTone,
	XT5_API_CODE_SetMovieShadowTone                     = API_CODE_SetMovieShadowTone,
	XT5_API_CODE_GetMovieShadowTone                     = API_CODE_GetMovieShadowTone,
	XT5_API_CODE_CapMovieSharpness                      = API_CODE_CapMovieSharpness,
	XT5_API_CODE_SetMovieSharpness                      = API_CODE_SetMovieSharpness,
	XT5_API_CODE_GetMovieSharpness                      = API_CODE_GetMovieSharpness,
	XT5_API_CODE_CapMovieColorMode                      = API_CODE_CapMovieColorMode,
	XT5_API_CODE_SetMovieColorMode                      = API_CODE_SetMovieColorMode,
	XT5_API_CODE_GetMovieColorMode                      = API_CODE_GetMovieColorMode,
	XT5_API_CODE_CapMovieNoiseReduction                 = API_CODE_CapMovieNoiseReduction,
	XT5_API_CODE_SetMovieNoiseReduction                 = API_CODE_SetMovieNoiseReduction,
	XT5_API_CODE_GetMovieNoiseReduction                 = API_CODE_GetMovieNoiseReduction,
	XT5_API_CODE_CapInterFrameNR                        = API_CODE_CapInterFrameNR,
	XT5_API_CODE_SetInterFrameNR                        = API_CODE_SetInterFrameNR,
	XT5_API_CODE_GetInterFrameNR                        = API_CODE_GetInterFrameNR,
	XT5_API_CODE_CapMoviePeripheralLightCorrection      = API_CODE_CapMoviePeripheralLightCorrection,
	XT5_API_CODE_SetMoviePeripheralLightCorrection      = API_CODE_SetMoviePeripheralLightCorrection,
	XT5_API_CODE_GetMoviePeripheralLightCorrection      = API_CODE_GetMoviePeripheralLightCorrection,
	XT5_API_CODE_CapMovieFaceDetectionMode              = API_CODE_CapMovieFaceDetectionMode,
	XT5_API_CODE_SetMovieFaceDetectionMode              = API_CODE_SetMovieFaceDetectionMode,
	XT5_API_CODE_GetMovieFaceDetectionMode              = API_CODE_GetMovieFaceDetectionMode,
	XT5_API_CODE_CapMovieEyeAFMode                      = API_CODE_CapMovieEyeAFMode,
	XT5_API_CODE_SetMovieEyeAFMode                      = API_CODE_SetMovieEyeAFMode,
	XT5_API_CODE_GetMovieEyeAFMode                      = API_CODE_GetMovieEyeAFMode,
	XT5_API_CODE_CapMovieSubjectDetectionMode           = API_CODE_CapMovieSubjectDetectionMode,
	XT5_API_CODE_SetMovieSubjectDetectionMode           = API_CODE_SetMovieSubjectDetectionMode,
	XT5_API_CODE_GetMovieSubjectDetectionMode           = API_CODE_GetMovieSubjectDetectionMode,
	XT5_API_CODE_CapMovieAFCCustom                      = API_CODE_CapMovieAFCCustom,
	XT5_API_CODE_SetMovieAFCCustom                      = API_CODE_SetMovieAFCCustom,
	XT5_API_CODE_GetMovieAFCCustom                      = API_CODE_GetMovieAFCCustom,
	XT5_API_CODE_CapMovieMFAssistMode                   = API_CODE_CapMovieMFAssistMode,
	XT5_API_CODE_SetMovieMFAssistMode                   = API_CODE_SetMovieMFAssistMode,
	XT5_API_CODE_GetMovieMFAssistMode                   = API_CODE_GetMovieMFAssistMode,
	XT5_API_CODE_GetMovieFocusMeter                     = API_CODE_GetMovieFocusMeter,
	XT5_API_CODE_CapMovieFocusCheckMode                 = API_CODE_CapMovieFocusCheckMode,
	XT5_API_CODE_SetMovieFocusCheckMode                 = API_CODE_SetMovieFocusCheckMode,
	XT5_API_CODE_GetMovieFocusCheckMode                 = API_CODE_GetMovieFocusCheckMode,
	XT5_API_CODE_CapMovieFocusCheckLock                 = API_CODE_CapMovieFocusCheckLock,
	XT5_API_CODE_SetMovieFocusCheckLock                 = API_CODE_SetMovieFocusCheckLock,
	XT5_API_CODE_GetMovieFocusCheckLock                 = API_CODE_GetMovieFocusCheckLock,
	XT5_API_CODE_GetMovieRecordingTime                  = API_CODE_GetMovieRecordingTime,
	XT5_API_CODE_GetMovieRemainingTime                  = API_CODE_GetMovieRemainingTime,
	XT5_API_CODE_GetTimeCode                            = API_CODE_GetTimeCode,
	XT5_API_CODE_CapTimeCodeDisplay                     = API_CODE_CapTimeCodeDisplay,
	XT5_API_CODE_SetTimeCodeDisplay                     = API_CODE_SetTimeCodeDisplay,
	XT5_API_CODE_GetTimeCodeDisplay                     = API_CODE_GetTimeCodeDisplay,
	XT5_API_CODE_CapTimeCodeStartSetting                = API_CODE_CapTimeCodeStartSetting,
	XT5_API_CODE_SetTimeCodeStartSetting                = API_CODE_SetTimeCodeStartSetting,
	XT5_API_CODE_CapTimeCodeCountUp                     = API_CODE_CapTimeCodeCountUp,
	XT5_API_CODE_SetTimeCodeCountUp                     = API_CODE_SetTimeCodeCountUp,
	XT5_API_CODE_GetTimeCodeCountUp                     = API_CODE_GetTimeCodeCountUp,
	XT5_API_CODE_CapTimeCodeDropFrame                   = API_CODE_CapTimeCodeDropFrame,
	XT5_API_CODE_SetTimeCodeDropFrame                   = API_CODE_SetTimeCodeDropFrame,
	XT5_API_CODE_GetTimeCodeDropFrame                   = API_CODE_GetTimeCodeDropFrame,
	XT5_API_CODE_CapTimeCodeHDMIOutput                  = API_CODE_CapTimeCodeHDMIOutput,
	XT5_API_CODE_SetTimeCodeHDMIOutput                  = API_CODE_SetTimeCodeHDMIOutput,
	XT5_API_CODE_GetTimeCodeHDMIOutput                  = API_CODE_GetTimeCodeHDMIOutput,
	XT5_API_CODE_GetTimeCodeCurrentValue                = API_CODE_GetTimeCodeCurrentValue,
	XT5_API_CODE_CapATOMOSAirGluConnection              = API_CODE_CapATOMOSAirGluConnection,
	XT5_API_CODE_SetATOMOSAirGluConnection              = API_CODE_SetATOMOSAirGluConnection,
	XT5_API_CODE_GetATOMOSAirGluConnection              = API_CODE_GetATOMOSAirGluConnection,
	XT5_API_CODE_GetTimeCodeStatus                      = API_CODE_GetTimeCodeStatus,
	XT5_API_CODE_CapInternalMicLevel                    = API_CODE_CapInternalMicLevel,
	XT5_API_CODE_SetInternalMicLevel                    = API_CODE_SetInternalMicLevel,
	XT5_API_CODE_GetInternalMicLevel                    = API_CODE_GetInternalMicLevel,
	XT5_API_CODE_CapInternalMicLevelManual              = API_CODE_CapInternalMicLevelManual,
	XT5_API_CODE_SetInternalMicLevelManual              = API_CODE_SetInternalMicLevelManual,
	XT5_API_CODE_GetInternalMicLevelManual              = API_CODE_GetInternalMicLevelManual,
	XT5_API_CODE_CapExternalMicLevel                    = API_CODE_CapExternalMicLevel,
	XT5_API_CODE_SetExternalMicLevel                    = API_CODE_SetExternalMicLevel,
	XT5_API_CODE_GetExternalMicLevel                    = API_CODE_GetExternalMicLevel,
	XT5_API_CODE_CapExternalMicLevelManual              = API_CODE_CapExternalMicLevelManual,
	XT5_API_CODE_SetExternalMicLevelManual              = API_CODE_SetExternalMicLevelManual,
	XT5_API_CODE_GetExternalMicLevelManual              = API_CODE_GetExternalMicLevelManual,
	XT5_API_CODE_CapMicLevelLimiter                     = API_CODE_CapMicLevelLimiter,
	XT5_API_CODE_SetMicLevelLimiter                     = API_CODE_SetMicLevelLimiter,
	XT5_API_CODE_GetMicLevelLimiter                     = API_CODE_GetMicLevelLimiter,
	XT5_API_CODE_CapWindFilter                          = API_CODE_CapWindFilter,
	XT5_API_CODE_SetWindFilter                          = API_CODE_SetWindFilter,
	XT5_API_CODE_GetWindFilter                          = API_CODE_GetWindFilter,
	XT5_API_CODE_CapLowCutFilter                        = API_CODE_CapLowCutFilter,
	XT5_API_CODE_SetLowCutFilter                        = API_CODE_SetLowCutFilter,
	XT5_API_CODE_GetLowCutFilter                        = API_CODE_GetLowCutFilter,
	XT5_API_CODE_CapHeadPhonesVolume                    = API_CODE_CapHeadPhonesVolume,
	XT5_API_CODE_SetHeadPhonesVolume                    = API_CODE_SetHeadPhonesVolume,
	XT5_API_CODE_GetHeadPhonesVolume                    = API_CODE_GetHeadPhonesVolume,
	XT5_API_CODE_CapXLRAdapterMicSource                 = API_CODE_CapXLRAdapterMicSource,
	XT5_API_CODE_SetXLRAdapterMicSource                 = API_CODE_SetXLRAdapterMicSource,
	XT5_API_CODE_GetXLRAdapterMicSource                 = API_CODE_GetXLRAdapterMicSource,
	XT5_API_CODE_CapXLRAdapterMoniteringSource          = API_CODE_CapXLRAdapterMoniteringSource,
	XT5_API_CODE_SetXLRAdapterMoniteringSource          = API_CODE_SetXLRAdapterMoniteringSource,
	XT5_API_CODE_GetXLRAdapterMoniteringSource          = API_CODE_GetXLRAdapterMoniteringSource,
	XT5_API_CODE_CapXLRAdapterHDMIOutputSource          = API_CODE_CapXLRAdapterHDMIOutputSource,
	XT5_API_CODE_SetXLRAdapterHDMIOutputSource          = API_CODE_SetXLRAdapterHDMIOutputSource,
	XT5_API_CODE_GetXLRAdapterHDMIOutputSource          = API_CODE_GetXLRAdapterHDMIOutputSource,
	XT5_API_CODE_GetMicLevelIndicator                   = API_CODE_GetMicLevelIndicator,
	XT5_API_CODE_GetMovieTransparentFrameInfo           = API_CODE_GetMovieTransparentFrameInfo,
	XT5_API_CODE_CapShortMovieSecond                    = API_CODE_CapShortMovieSecond,
	XT5_API_CODE_SetShortMovieSecond                    = API_CODE_SetShortMovieSecond,
	XT5_API_CODE_GetShortMovieSecond                    = API_CODE_GetShortMovieSecond,
	XT5_API_CODE_CapMoviePortraitEnhancer               = API_CODE_CapMoviePortraitEnhancer,
	XT5_API_CODE_SetMoviePortraitEnhancer               = API_CODE_SetMoviePortraitEnhancer,
	XT5_API_CODE_GetMoviePortraitEnhancer               = API_CODE_GetMoviePortraitEnhancer,
	XT5_API_CODE_CapDirectionalMic                      = API_CODE_CapDirectionalMic,
	XT5_API_CODE_SetDirectionalMic                      = API_CODE_SetDirectionalMic,
	XT5_API_CODE_GetDirectionalMic                      = API_CODE_GetDirectionalMic,
	XT5_API_CODE_CapEnvSoundsNR                         = API_CODE_CapEnvSoundsNR,
	XT5_API_CODE_SetEnvSoundsNR                         = API_CODE_SetEnvSoundsNR,
	XT5_API_CODE_GetEnvSoundsNR                         = API_CODE_GetEnvSoundsNR,
	XT5_API_CODE_GetBodyTemperatureWarning              = API_CODE_GetBodyTemperatureWarning,
	XT5_API_CODE_GetPlayBackMediaInfo                   = API_CODE_GetPlayBackMediaInfo,
	XT5_API_CODE_GetPlayBackObjectInfo                  = API_CODE_GetPlayBackObjectInfo,
	XT5_API_CODE_GetPlayBackThumb                       = API_CODE_GetPlayBackThumb,
	XT5_API_CODE_GetPlayBackPartialObject               = API_CODE_GetPlayBackPartialObject,
	XT5_API_CODE_CapMovieRecVolume                      = API_CODE_CapMovieRecVolume,
	XT5_API_CODE_SetMovieRecVolume                      = API_CODE_SetMovieRecVolume,
	XT5_API_CODE_GetMovieRecVolume                      = API_CODE_GetMovieRecVolume,
};

// Movie Af Mode
enum{
	XT5_MOVIE_AF_MULTI                     = SDK_MOVIE_AF_MULTI,
	XT5_MOVIE_AF_AREA                      = SDK_MOVIE_AF_AREA,
	XT5_MOVIE_AF_WIDETRACKING              = SDK_MOVIE_AF_WIDETRACKING,
};

// Short Movie Second
enum{
	XT5_SHORT_MOVIE_SECOND_OFF             = SDK_SHORT_MOVIE_SECOND_OFF,
	XT5_SHORT_MOVIE_SECOND_15S             = SDK_SHORT_MOVIE_SECOND_15S,
	XT5_SHORT_MOVIE_SECOND_30S             = SDK_SHORT_MOVIE_SECOND_30S,
	XT5_SHORT_MOVIE_SECOND_60S             = SDK_SHORT_MOVIE_SECOND_60S,
};

// Movie Resolution
// High Speed Rec Resolution
enum{
	XT5_MOVIE_RESOLUTION_6P2K_3_2          = SDK_MOVIE_RESOLUTION_6P2K_3_2,
	XT5_MOVIE_RESOLUTION_8K_16_9           = SDK_MOVIE_RESOLUTION_8K_16_9,
	XT5_MOVIE_RESOLUTION_6K_16_9           = SDK_MOVIE_RESOLUTION_6K_16_9,
	XT5_MOVIE_RESOLUTION_4KHQ_16_9         = SDK_MOVIE_RESOLUTION_4KHQ_16_9,
	XT5_MOVIE_RESOLUTION_4K_16_9           = SDK_MOVIE_RESOLUTION_4K_16_9,
	XT5_MOVIE_RESOLUTION_DCIHQ_17_9        = SDK_MOVIE_RESOLUTION_DCIHQ_17_9,
	XT5_MOVIE_RESOLUTION_DCI_17_9          = SDK_MOVIE_RESOLUTION_DCI_17_9,
	XT5_MOVIE_RESOLUTION_FULLHD_16_9       = SDK_MOVIE_RESOLUTION_FULLHD_16_9,
	XT5_MOVIE_RESOLUTION_FULLHD_17_9       = SDK_MOVIE_RESOLUTION_FULLHD_17_9,
	XT5_MOVIE_RESOLUTION_5K_17_9           = SDK_MOVIE_RESOLUTION_5K_17_9,
	XT5_MOVIE_RESOLUTION_DCI_8K_17_9       = SDK_MOVIE_RESOLUTION_DCI_8K_17_9,
	XT5_MOVIE_RESOLUTION_CINESCO_2P35_1    = SDK_MOVIE_RESOLUTION_CINESCO_2P35_1,
	XT5_MOVIE_RESOLUTION_OPENGATE_3_2      = SDK_MOVIE_RESOLUTION_OPENGATE_3_2,
	XT5_MOVIE_RESOLUTION_35MM_16_9         = SDK_MOVIE_RESOLUTION_35MM_16_9,
	XT5_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1,
	XT5_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1,
	XT5_MOVIE_RESOLUTION_FULLFRAME_3_2     = SDK_MOVIE_RESOLUTION_FULLFRAME_3_2,
	XT5_MOVIE_RESOLUTION_FULLHD_LP_16_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_16_9,
	XT5_MOVIE_RESOLUTION_FULLHD_LP_17_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_17_9,
	XT5_MOVIE_RESOLUTION_4K_LP_16_9        = SDK_MOVIE_RESOLUTION_4K_LP_16_9,
	XT5_MOVIE_RESOLUTION_FULLHD_9_16       = SDK_MOVIE_RESOLUTION_FULLHD_9_16,
};

// Movie FrameRate
// High Speed Rec PlayBack FrameRate
enum{
	XT5_MOVIE_FRAMERATE_59_94P             = SDK_MOVIE_FRAMERATE_59_94P,
	XT5_MOVIE_FRAMERATE_50P                = SDK_MOVIE_FRAMERATE_50P,
	XT5_MOVIE_FRAMERATE_29_97P             = SDK_MOVIE_FRAMERATE_29_97P,
	XT5_MOVIE_FRAMERATE_25P                = SDK_MOVIE_FRAMERATE_25P,
	XT5_MOVIE_FRAMERATE_24P                = SDK_MOVIE_FRAMERATE_24P,
	XT5_MOVIE_FRAMERATE_23_98P             = SDK_MOVIE_FRAMERATE_23_98P,
};

// Movie BitRate
enum{
	XT5_MOVIE_BITRATE_720MBPS              = SDK_MOVIE_BITRATE_720MBPS,
	XT5_MOVIE_BITRATE_400MBPS              = SDK_MOVIE_BITRATE_400MBPS,
	XT5_MOVIE_BITRATE_360MBPS              = SDK_MOVIE_BITRATE_360MBPS,
	XT5_MOVIE_BITRATE_200MBPS              = SDK_MOVIE_BITRATE_200MBPS,
	XT5_MOVIE_BITRATE_100MBPS              = SDK_MOVIE_BITRATE_100MBPS,
	XT5_MOVIE_BITRATE_50MBPS               = SDK_MOVIE_BITRATE_50MBPS,
	XT5_MOVIE_BITRATE_25MBPS               = SDK_MOVIE_BITRATE_25MBPS,
	XT5_MOVIE_BITRATE_8MBPS                = SDK_MOVIE_BITRATE_8MBPS,
};

// Movie FileFormat
enum{
	XT5_MOVIE_FORMAT_H264_ALL_I_MOV        = SDK_MOVIE_FORMAT_H264_ALL_I_MOV,
	XT5_MOVIE_FORMAT_H264_LONGGOP_I_MOV    = SDK_MOVIE_FORMAT_H264_LONGGOP_I_MOV,
	XT5_MOVIE_FORMAT_H264_LONGGOP_MP4      = SDK_MOVIE_FORMAT_H264_LONGGOP_MP4,
	XT5_MOVIE_FORMAT_H265_4_2_0_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_0_ALL_I,
	XT5_MOVIE_FORMAT_H265_4_2_0_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_0_LONGGOP,
	XT5_MOVIE_FORMAT_H265_4_2_2_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_2_ALL_I,
	XT5_MOVIE_FORMAT_H265_4_2_2_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_2_LONGGOP,
	XT5_MOVIE_FORMAT_PRORESHQ              = SDK_MOVIE_FORMAT_PRORESHQ,
	XT5_MOVIE_FORMAT_PRORES                = SDK_MOVIE_FORMAT_PRORES,
	XT5_MOVIE_FORMAT_PRORESLT              = SDK_MOVIE_FORMAT_PRORESLT,
};

// High Speed RecMode
enum{
	XT5_HIGHSPEEDREC_OFF                   = SDK_HIGHSPEEDREC_OFF,
	XT5_HIGHSPEEDREC_ON                    = SDK_HIGHSPEEDREC_ON,
	XT5_HIGHSPEEDREC_ON_HDMI_ONLY          = SDK_HIGHSPEEDREC_ON_HDMI_ONLY,
};

// High Speed Rec FrameRate
enum{
	XT5_HIGHSPEEDREC_FRAMERATE_240P        = SDK_HIGHSPEEDREC_FRAMERATE_240P,
	XT5_HIGHSPEEDREC_FRAMERATE_200P        = SDK_HIGHSPEEDREC_FRAMERATE_200P,
	XT5_HIGHSPEEDREC_FRAMERATE_120P        = SDK_HIGHSPEEDREC_FRAMERATE_120P,
	XT5_HIGHSPEEDREC_FRAMERATE_100P        = SDK_HIGHSPEEDREC_FRAMERATE_100P,
};

// Movie Media Record
enum{
	XT5_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2,
	XT5_MOVIE_MEDIARECORD_SLOT2                  = SDK_MOVIE_MEDIARECORD_SLOT2,
	XT5_MOVIE_MEDIARECORD_SLOT1                  = SDK_MOVIE_MEDIARECORD_SLOT1,
	XT5_MOVIE_MEDIARECORD_BACKUP                 = SDK_MOVIE_MEDIARECORD_BACKUP,
	XT5_MOVIE_MEDIARECORD_SSD                    = SDK_MOVIE_MEDIARECORD_SSD,
	XT5_MOVIE_MEDIARECORD_SSD_CF                 = SDK_MOVIE_MEDIARECORD_SSD_CF,
	XT5_MOVIE_MEDIARECORD_OFF                    = SDK_MOVIE_MEDIARECORD_OFF,
	XT5_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF       = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF,
	XT5_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1,
};

// Movie Media Record ProRes
enum{
	XT5_MOVIE_MEDIARECORD_PRORES_OFF       = SDK_MOVIE_MEDIARECORD_PRORES_OFF,
	XT5_MOVIE_MEDIARECORD_PRORES_H264      = SDK_MOVIE_MEDIARECORD_PRORES_H264,
	XT5_MOVIE_MEDIARECORD_PRORES_PROXY     = SDK_MOVIE_MEDIARECORD_PRORES_PROXY,
};

// Movie HDMIOutput RAW
enum{
	XT5_MOVIE_HDMI_OUTPUT_RAW_OFF          = SDK_MOVIE_HDMI_OUTPUT_RAW_OFF,
	XT5_MOVIE_HDMI_OUTPUT_RAW_ATOMOS       = SDK_MOVIE_HDMI_OUTPUT_RAW_ATOMOS,
	XT5_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC   = SDK_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC,
};

// Movie HDMI Output RAW Resolution
enum{
	XT5_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K,
	XT5_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K,
	XT5_MOVIE_HDMI_OUTPUT_RESOLUTION_8K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_8K,
	XT5_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K,
	XT5_MOVIE_HDMI_OUTPUT_RESOLUTION_4K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4K,
	XT5_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K= SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K,
};

// Movie HDMI Output RAW FrameRate
enum{
	XT5_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P,
	XT5_MOVIE_HDMI_OUTPUT_FRAMERATE_50P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_50P,
	XT5_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P,
	XT5_MOVIE_HDMI_OUTPUT_FRAMERATE_25P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_25P,
	XT5_MOVIE_HDMI_OUTPUT_FRAMERATE_24P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_24P,
	XT5_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P,
};

// Flog Recording
enum{
	XT5_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM,
	XT5_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG,
	XT5_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2 = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2,
	XT5_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG,
	XT5_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2  = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2,
	XT5_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM,
	XT5_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM  = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM,
	XT5_MOVIERECORD_MEDIA_HLG_HDMI_HLG     = SDK_MOVIERECORD_MEDIA_HLG_HDMI_HLG,
};

// MovieIsMode
enum{
	XT5_MOVIE_IS_MODE_OFF                  = SDK_MOVIE_IS_MODE_OFF,
	XT5_MOVIE_IS_MODE_ON                   = SDK_MOVIE_IS_MODE_ON,
	XT5_MOVIE_IS_MODE_IBIS_OIS             = SDK_MOVIE_IS_MODE_IBIS_OIS,
	XT5_MOVIE_IS_MODE_IBIS_OIS_DIS         = SDK_MOVIE_IS_MODE_IBIS_OIS_DIS,
	XT5_MOVIE_IS_MODE_OIS                  = SDK_MOVIE_IS_MODE_OIS,
	XT5_MOVIE_IS_MODE_OIS_DIS              = SDK_MOVIE_IS_MODE_OIS_DIS,
};

// Movie Zebra Setting
enum{
	XT5_MOVIE_ZEBRA_SETTING_OFF            = SDK_MOVIE_ZEBRA_SETTING_OFF,
	XT5_MOVIE_ZEBRA_SETTING_RIGHT          = SDK_MOVIE_ZEBRA_SETTING_RIGHT,
	XT5_MOVIE_ZEBRA_SETTING_LEFT           = SDK_MOVIE_ZEBRA_SETTING_LEFT,
};

// Movie Tally Light
enum{
	XT5_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON      = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON,
	XT5_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK   = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK,
	XT5_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON       = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON,
	XT5_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF      = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF,
	XT5_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK,
	XT5_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF   = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF,
	XT5_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF     = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF,
};

// Media Eject Warning
enum{
	XT5_MEDIA_EJECT_WARNING_SLOT1          = SDK_MEDIA_EJECT_WARNING_SLOT1,
	XT5_MEDIA_EJECT_WARNING_SLOT2          = SDK_MEDIA_EJECT_WARNING_SLOT2,
	XT5_MEDIA_EJECT_WARNING_SLOT3          = SDK_MEDIA_EJECT_WARNING_SLOT3,
};

// Movie Data Level Setting
enum{
	XT5_MOVIE_DATA_LEVEL_SETTING_FULL      = SDK_MOVIE_DATA_LEVEL_SETTING_FULL,
	XT5_MOVIE_DATA_LEVEL_SETTING_VIDEO     = SDK_MOVIE_DATA_LEVEL_SETTING_VIDEO,
};

// Movie ImageFormat
enum{
	XT5_MOVIE_IMAGEFORMAT_GF               = SDK_MOVIE_IMAGEFORMAT_GF,
	XT5_MOVIE_IMAGEFORMAT_35MM             = SDK_MOVIE_IMAGEFORMAT_35MM,
	XT5_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM  = SDK_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM,
	XT5_MOVIE_IMAGEFORMAT_PREMISTA         = SDK_MOVIE_IMAGEFORMAT_PREMISTA,
};

// WaveForm VectorScope
enum{
	XT5_WAVEFORM_VECTORSCOPE_OFF           = SDK_WAVEFORM_VECTORSCOPE_OFF,
	XT5_WAVEFORM_VECTORSCOPE_WAVEFORM      = SDK_WAVEFORM_VECTORSCOPE_WAVEFORM,
	XT5_WAVEFORM_VECTORSCOPE_PARADE        = SDK_WAVEFORM_VECTORSCOPE_PARADE,
	XT5_WAVEFORM_VECTORSCOPE_VECTORSCOPE   = SDK_WAVEFORM_VECTORSCOPE_VECTORSCOPE,
};

// WaveForm Setting
enum{
	XT5_WAVEFORM_SETTING_PATTERN1          = SDK_WAVEFORM_SETTING_PATTERN1,
	XT5_WAVEFORM_SETTING_PATTERN2          = SDK_WAVEFORM_SETTING_PATTERN2,
	XT5_WAVEFORM_SETTING_PATTERN3          = SDK_WAVEFORM_SETTING_PATTERN3,
	XT5_WAVEFORM_SETTING_PATTERN4          = SDK_WAVEFORM_SETTING_PATTERN4,
};

// VectorScope Setting
enum{
	XT5_VECTORSCORE_SETTING_PATTERN1       = SDK_VECTORSCORE_SETTING_PATTERN1,
	XT5_VECTORSCORE_SETTING_PATTERN2       = SDK_VECTORSCORE_SETTING_PATTERN2,
	XT5_VECTORSCORE_SETTING_PATTERN3       = SDK_VECTORSCORE_SETTING_PATTERN3,
	XT5_VECTORSCORE_SETTING_PATTERN4       = SDK_VECTORSCORE_SETTING_PATTERN4,
};

// Parade Setting Display
enum{
	XT5_PARADE_SETTING_DISPLAY_PATTERN1    = SDK_PARADE_SETTING_DISPLAY_PATTERN1,
	XT5_PARADE_SETTING_DISPLAY_PATTERN2    = SDK_PARADE_SETTING_DISPLAY_PATTERN2,
	XT5_PARADE_SETTING_DISPLAY_PATTERN3    = SDK_PARADE_SETTING_DISPLAY_PATTERN3,
	XT5_PARADE_SETTING_DISPLAY_PATTERN4    = SDK_PARADE_SETTING_DISPLAY_PATTERN4,
};

// Parade Setting Color
enum{
	XT5_PARADE_SETTING_COLOR_RGB           = SDK_PARADE_SETTING_COLOR_RGB,
	XT5_PARADE_SETTING_COLOR_WHITE         = SDK_PARADE_SETTING_COLOR_WHITE,
};

// Movie FocusMeter(display)
enum{
	XT5_FOCUSMETER_DISPLAY_OFF             = SDK_FOCUSMETER_DISPLAY_OFF,
	XT5_FOCUSMETER_DISPLAY_ON              = SDK_FOCUSMETER_DISPLAY_ON,
};

// Movie FocusMeter(color)
enum{
	XT5_FOCUSMETER_COLOR_WHITE             = SDK_FOCUSMETER_COLOR_WHITE,
	XT5_FOCUSMETER_COLOR_GREEN             = SDK_FOCUSMETER_COLOR_GREEN,
};

// TimeCode Start Setting
enum{
	XT5_TIMECODE_START_SETTING_MANUAL      = SDK_TIMECODE_START_SETTING_MANUAL,
	XT5_TIMECODE_START_SETTING_CURRENT     = SDK_TIMECODE_START_SETTING_CURRENT,
	XT5_TIMECODE_START_SETTING_RESET       = SDK_TIMECODE_START_SETTING_RESET,
};

// TimeCode CountUp
enum{
	XT5_TIMECODE_COUNTUP_RECRUN            = SDK_TIMECODE_COUNTUP_RECRUN,
	XT5_TIMECODE_COUNTUP_FREERUN           = SDK_TIMECODE_COUNTUP_FREERUN,
};

// TimeCode Status
enum{
	XT5_TIMECODE_STATUS_USE_DSC_ALONE      = SDK_TIMECODE_STATUS_USE_DSC_ALONE,
	XT5_TIMECODE_STATUS_SYNCING            = SDK_TIMECODE_STATUS_SYNCING,
	XT5_TIMECODE_STATUS_DISCONNECTED       = SDK_TIMECODE_STATUS_DISCONNECTED,
	XT5_TIMECODE_STATUS_NOT_SYNCED         = SDK_TIMECODE_STATUS_NOT_SYNCED,
	XT5_TIMECODE_STATUS_FRAMERATE_MISMATCH = SDK_TIMECODE_STATUS_FRAMERATE_MISMATCH,
};

// Internal Mic Level
// External Mic Level
enum{
	XT5_MIC_LEVEL_OFF                      = SDK_MIC_LEVEL_OFF,
	XT5_MIC_LEVEL_MANUAL                   = SDK_MIC_LEVEL_MANUAL,
	XT5_MIC_LEVEL_AUTO                     = SDK_MIC_LEVEL_AUTO,
};

// HeadPhones Volume
enum{
	XT5_HEADPHONES_VOLUME_0                = SDK_HEADPHONES_VOLUME_0,
	XT5_HEADPHONES_VOLUME_1                = SDK_HEADPHONES_VOLUME_1,
	XT5_HEADPHONES_VOLUME_2                = SDK_HEADPHONES_VOLUME_2,
	XT5_HEADPHONES_VOLUME_3                = SDK_HEADPHONES_VOLUME_3,
	XT5_HEADPHONES_VOLUME_4                = SDK_HEADPHONES_VOLUME_4,
	XT5_HEADPHONES_VOLUME_5                = SDK_HEADPHONES_VOLUME_5,
	XT5_HEADPHONES_VOLUME_6                = SDK_HEADPHONES_VOLUME_6,
	XT5_HEADPHONES_VOLUME_7                = SDK_HEADPHONES_VOLUME_7,
	XT5_HEADPHONES_VOLUME_8                = SDK_HEADPHONES_VOLUME_8,
	XT5_HEADPHONES_VOLUME_9                = SDK_HEADPHONES_VOLUME_9,
	XT5_HEADPHONES_VOLUME_10               = SDK_HEADPHONES_VOLUME_10,
};

// XLR Adapter Mic Source
enum{
	XT5_XLRADAPTER_MIC_SOURCE_4CH          = SDK_XLRADAPTER_MIC_SOURCE_4CH,
	XT5_XLRADAPTER_MIC_SOURCE_2CH          = SDK_XLRADAPTER_MIC_SOURCE_2CH,
};

// XLR Adapter Monitering Source
enum{
	XT5_XLRADAPTER_MONITER_SOURCE_XLR      = SDK_XLRADAPTER_MONITER_SOURCE_XLR,
	XT5_XLRADAPTER_MONITER_SOURCE_CAMERA   = SDK_XLRADAPTER_MONITER_SOURCE_CAMERA,
};

// XLR Adapter HDMI Output Source
enum{
	XT5_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR   = SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR,
	XT5_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA= SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA,
};

// Directional Mic
enum{
	XT5_DIRECTIONAL_MIC_AUTO               = SDK_DIRECTIONAL_MIC_AUTO,
	XT5_DIRECTIONAL_MIC_SURROUND           = SDK_DIRECTIONAL_MIC_SURROUND,
	XT5_DIRECTIONAL_MIC_FRONT              = SDK_DIRECTIONAL_MIC_FRONT,
	XT5_DIRECTIONAL_MIC_TRACKING           = SDK_DIRECTIONAL_MIC_TRACKING,
	XT5_DIRECTIONAL_MIC_BACK               = SDK_DIRECTIONAL_MIC_BACK,
	XT5_DIRECTIONAL_MIC_FRONT_BACK         = SDK_DIRECTIONAL_MIC_FRONT_BACK,
};

// Body Temperature Warning
enum{
	XT5_BODY_TEMPERATURE_WARNING_NONE      = SDK_BODY_TEMPERATURE_WARNING_NONE,
	XT5_BODY_TEMPERATURE_WARNING_YELLOW    = SDK_BODY_TEMPERATURE_WARNING_YELLOW,
	XT5_BODY_TEMPERATURE_WARNING_RED       = SDK_BODY_TEMPERATURE_WARNING_RED,
};

// Movie Rec Volume
enum{
	XT5_MOVIE_REC_VOLUME_OFF               = SDK_MOVIE_REC_VOLUME_OFF,
	XT5_MOVIE_REC_VOLUME_1                 = SDK_MOVIE_REC_VOLUME_1,
	XT5_MOVIE_REC_VOLUME_2                 = SDK_MOVIE_REC_VOLUME_2,
	XT5_MOVIE_REC_VOLUME_3                 = SDK_MOVIE_REC_VOLUME_3,
};
