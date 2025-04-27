///////////////////////////////////////////////////
//  X-H2_MOV.h
//     Version 1.0.0.0
//
//     Copyright (C) 2021 FUJIFILM Corporation.
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
typedef  SDK_MOVIE_AFC_CUSTOM     XH2_SDK_MOVIE_AFC_CUSTOM, *PXH2_SDK_MOVIE_AFC_CUSTOM;

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
typedef  SDK_MICLEVEL_INDICATOR     XH2_SDK_MICLEVEL_INDICATOR, *PXH2_SDK_MICLEVEL_INDICATOR;

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
typedef  SDK_PlayBackObjectInformation     XH2_SDK_PlayBackObjectInformation, *PXH2_SDK_PlayBackObjectInformation;

// Focus Map Data
//typedef	struct _SDK_FocusMapData {
//	long lDistance;
//  long lColorR;
//	long lColorG;
//	long lColorB;
//	long lAlpha;
//} SDK_FocusMapData;
typedef  SDK_FocusMapData     XH2_SDK_FocusMapData, *PXH2_SDK_FocusMapData;

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
typedef  SDK_TrackingAfFrameInfo     XH2_SDK_TrackingAfFrameInfo, *PXH2_SDK_TrackingAfFrameInfo;

// Histogram Data
//typedef struct SDK_HistogramData {
//	long lLuminance;
//	long lColorR;
//	long lColorG;
//	long lColorB;
//} SDK_HistogramData;
typedef  SDK_HistogramData     XH2_SDK_HistogramData, *PXH2_SDK_HistogramData;


//
// API_PARAM for X-H2
//
enum {
	XH2_API_PARAM_CapMovieHighFrequencyFlickerlessMode = 2,
	XH2_API_PARAM_SetMovieHighFrequencyFlickerlessMode = 1,
	XH2_API_PARAM_GetMovieHighFrequencyFlickerlessMode = 1,
	XH2_API_PARAM_CapMovieAFMode                       = 2,
	XH2_API_PARAM_SetMovieAFMode                       = 1,
	XH2_API_PARAM_GetMovieAFMode                       = 1,
	XH2_API_PARAM_CapMovieFocusArea                    = 3,
	XH2_API_PARAM_SetMovieFocusArea                    = 2,
	XH2_API_PARAM_GetMovieFocusArea                    = 2,
	XH2_API_PARAM_GetTrackingAfFrameInfo               = 1,
	XH2_API_PARAM_CapMovieResolution                   = 2,
	XH2_API_PARAM_SetMovieResolution                   = 1,
	XH2_API_PARAM_GetMovieResolution                   = 1,
	XH2_API_PARAM_CapMovieFrameRate                    = 2,
	XH2_API_PARAM_SetMovieFrameRate                    = 1,
	XH2_API_PARAM_GetMovieFrameRate                    = 1,
	XH2_API_PARAM_CapMovieBitRate                      = 2,
	XH2_API_PARAM_SetMovieBitRate                      = 1,
	XH2_API_PARAM_GetMovieBitRate                      = 1,
	XH2_API_PARAM_CapMovieFileFormat                   = 2,
	XH2_API_PARAM_SetMovieFileFormat                   = 1,
	XH2_API_PARAM_GetMovieFileFormat                   = 1,
	XH2_API_PARAM_CapHighSpeedRecMode                  = 2,
	XH2_API_PARAM_SetHighSpeedRecMode                  = 1,
	XH2_API_PARAM_GetHighSpeedRecMode                  = 1,
	XH2_API_PARAM_CapHighSpeedRecResolution            = 2,
	XH2_API_PARAM_SetHighSpeedRecResolution            = 1,
	XH2_API_PARAM_GetHighSpeedRecResolution            = 1,
	XH2_API_PARAM_CapHighSpeedRecFrameRate             = 2,
	XH2_API_PARAM_SetHighSpeedRecFrameRate             = 1,
	XH2_API_PARAM_GetHighSpeedRecFrameRate             = 1,
	XH2_API_PARAM_CapHighSpeedRecPlayBackFrameRate     = 2,
	XH2_API_PARAM_SetHighSpeedRecPlayBackFrameRate     = 1,
	XH2_API_PARAM_GetHighSpeedRecPlayBackFrameRate     = 1,
	XH2_API_PARAM_CapMovieMediaRecord                  = 2,
	XH2_API_PARAM_SetMovieMediaRecord                  = 1,
	XH2_API_PARAM_GetMovieMediaRecord                  = 1,
	XH2_API_PARAM_CapMovieMediaRecordProRes            = 2,
	XH2_API_PARAM_SetMovieMediaRecordProRes            = 1,
	XH2_API_PARAM_GetMovieMediaRecordProRes            = 1,
	XH2_API_PARAM_CapMovieHDMIOutputRAW                = 2,
	XH2_API_PARAM_SetMovieHDMIOutputRAW                = 1,
	XH2_API_PARAM_GetMovieHDMIOutputRAW                = 1,
	XH2_API_PARAM_CapMovieHDMIOutputRAWResolution      = 2,
	XH2_API_PARAM_SetMovieHDMIOutputRAWResolution      = 1,
	XH2_API_PARAM_GetMovieHDMIOutputRAWResolution      = 1,
	XH2_API_PARAM_CapMovieHDMIOutputRAWFrameRate       = 2,
	XH2_API_PARAM_SetMovieHDMIOutputRAWFrameRate       = 1,
	XH2_API_PARAM_GetMovieHDMIOutputRAWFrameRate       = 1,
	XH2_API_PARAM_CapFlogRecording                     = 2,
	XH2_API_PARAM_SetFlogRecording                     = 1,
	XH2_API_PARAM_GetFlogRecording                     = 1,
	XH2_API_PARAM_CapMovieCropMagnification            = 2,
	XH2_API_PARAM_SetMovieCropMagnification            = 1,
	XH2_API_PARAM_GetMovieCropMagnification            = 1,
	XH2_API_PARAM_CapMovieHDMIOutputInfoDisplay        = 2,
	XH2_API_PARAM_SetMovieHDMIOutputInfoDisplay        = 1,
	XH2_API_PARAM_GetMovieHDMIOutputInfoDisplay        = 1,
	XH2_API_PARAM_CapMovieHDMIRecControl               = 2,
	XH2_API_PARAM_SetMovieHDMIRecControl               = 1,
	XH2_API_PARAM_GetMovieHDMIRecControl               = 1,
	XH2_API_PARAM_CapMovieIsMode                       = 2,
	XH2_API_PARAM_SetMovieIsMode                       = 1,
	XH2_API_PARAM_GetMovieIsMode                       = 1,
	XH2_API_PARAM_CapMovieIsModeBoost                  = 2,
	XH2_API_PARAM_SetMovieIsModeBoost                  = 1,
	XH2_API_PARAM_GetMovieIsModeBoost                  = 1,
	XH2_API_PARAM_CapMovieZebraSetting                 = 2,
	XH2_API_PARAM_SetMovieZebraSetting                 = 1,
	XH2_API_PARAM_GetMovieZebraSetting                 = 1,
	XH2_API_PARAM_CapMovieZebraLevel                   = 2,
	XH2_API_PARAM_SetMovieZebraLevel                   = 1,
	XH2_API_PARAM_GetMovieZebraLevel                   = 1,
	XH2_API_PARAM_CapMovieOptimizedControl             = 2,
	XH2_API_PARAM_SetMovieOptimizedControl             = 1,
	XH2_API_PARAM_GetMovieOptimizedControl             = 1,
	XH2_API_PARAM_CapMovieTallyLight                   = 2,
	XH2_API_PARAM_SetMovieTallyLight                   = 1,
	XH2_API_PARAM_GetMovieTallyLight                   = 1,
	XH2_API_PARAM_GetMediaEjectWarning                 = 1,
	XH2_API_PARAM_CapMovieDataLevelSetting             = 2,
	XH2_API_PARAM_SetMovieDataLevelSetting             = 1,
	XH2_API_PARAM_GetMovieDataLevelSetting             = 1,
	XH2_API_PARAM_GetMovieCropMagnificationValue       = 1,
	XH2_API_PARAM_CapMovieFullTimeManual               = 2,
	XH2_API_PARAM_SetMovieFullTimeManual               = 1,
	XH2_API_PARAM_GetMovieFullTimeManual               = 1,
	XH2_API_PARAM_CapMovieDigitalZoom                  = 2,
	XH2_API_PARAM_SetMovieDigitalZoom                  = 1,
	XH2_API_PARAM_GetMovieDigitalZoom                  = 1,
	XH2_API_PARAM_GetMovieDigitalZoomRange             = 3,
	XH2_API_PARAM_CapMovieImageFormat                  = 2,
	XH2_API_PARAM_SetMovieImageFormat                  = 1,
	XH2_API_PARAM_GetMovieImageFormat                  = 1,
	XH2_API_PARAM_GetHistogramData                     = 2,
	XH2_API_PARAM_GetFocusMapData                      = 2,
	XH2_API_PARAM_CapWaveFormVectorScope               = 2,
	XH2_API_PARAM_SetWaveFormVectorScope               = 1,
	XH2_API_PARAM_GetWaveFormVectorScope               = 1,
	XH2_API_PARAM_GetWaveFormData                      = 2,
	XH2_API_PARAM_GetVectorScopeData                   = 2,
	XH2_API_PARAM_GetParadeData                        = 2,
	XH2_API_PARAM_CapRecFrameIndicator                 = 2,
	XH2_API_PARAM_SetRecFrameIndicator                 = 1,
	XH2_API_PARAM_GetRecFrameIndicator                 = 1,
	XH2_API_PARAM_CapMovieCaptureDelay                 = 2,
	XH2_API_PARAM_SetMovieCaptureDelay                 = 1,
	XH2_API_PARAM_GetMovieCaptureDelay                 = 1,
	XH2_API_PARAM_CapAnamorphicDesqueezeDisplay        = 2,
	XH2_API_PARAM_SetAnamorphicDesqueezeDisplay        = 1,
	XH2_API_PARAM_GetAnamorphicDesqueezeDisplay        = 1,
	XH2_API_PARAM_CapAnamorphicMagnification           = 2,
	XH2_API_PARAM_SetAnamorphicMagnification           = 1,
	XH2_API_PARAM_GetAnamorphicMagnification           = 1,
	XH2_API_PARAM_CapWaveFormSetting                   = 2,
	XH2_API_PARAM_SetWaveFormSetting                   = 1,
	XH2_API_PARAM_GetWaveFormSetting                   = 1,
	XH2_API_PARAM_CapVectorScopeSetting                = 2,
	XH2_API_PARAM_SetVectorScopeSetting                = 1,
	XH2_API_PARAM_GetVectorScopeSetting                = 1,
	XH2_API_PARAM_CapParadeSettingDisplay              = 2,
	XH2_API_PARAM_SetParadeSettingDisplay              = 1,
	XH2_API_PARAM_GetParadeSettingDisplay              = 1,
	XH2_API_PARAM_CapParadeSettingColor                = 2,
	XH2_API_PARAM_SetParadeSettingColor                = 1,
	XH2_API_PARAM_GetParadeSettingColor                = 1,
	XH2_API_PARAM_CapFlogDRangePriority                = 2,
	XH2_API_PARAM_SetFlogDRangePriority                = 1,
	XH2_API_PARAM_GetFlogDRangePriority                = 1,
	XH2_API_PARAM_CapMovieWhiteBalanceTune             = 5,
	XH2_API_PARAM_SetMovieWhiteBalanceTune             = 3,
	XH2_API_PARAM_GetMovieWhiteBalanceTune             = 3,
	XH2_API_PARAM_CapMovieFilmSimulationMode           = 2,
	XH2_API_PARAM_SetMovieFilmSimulationMode           = 1,
	XH2_API_PARAM_GetMovieFilmSimulationMode           = 1,
	XH2_API_PARAM_CapMovieMonochromaticColor           = 4,
	XH2_API_PARAM_SetMovieMonochromaticColor           = 2,
	XH2_API_PARAM_GetMovieMonochromaticColor           = 2,
	XH2_API_PARAM_CapMovieHighLightTone                = 2,
	XH2_API_PARAM_SetMovieHighLightTone                = 1,
	XH2_API_PARAM_GetMovieHighLightTone                = 1,
	XH2_API_PARAM_CapMovieShadowTone                   = 2,
	XH2_API_PARAM_SetMovieShadowTone                   = 1,
	XH2_API_PARAM_GetMovieShadowTone                   = 1,
	XH2_API_PARAM_CapMovieSharpness                    = 2,
	XH2_API_PARAM_SetMovieSharpness                    = 1,
	XH2_API_PARAM_GetMovieSharpness                    = 1,
	XH2_API_PARAM_CapMovieColorMode                    = 2,
	XH2_API_PARAM_SetMovieColorMode                    = 1,
	XH2_API_PARAM_GetMovieColorMode                    = 1,
	XH2_API_PARAM_CapMovieNoiseReduction               = 2,
	XH2_API_PARAM_SetMovieNoiseReduction               = 1,
	XH2_API_PARAM_GetMovieNoiseReduction               = 1,
	XH2_API_PARAM_CapInterFrameNR                      = 2,
	XH2_API_PARAM_SetInterFrameNR                      = 1,
	XH2_API_PARAM_GetInterFrameNR                      = 1,
	XH2_API_PARAM_CapMoviePeripheralLightCorrection    = 2,
	XH2_API_PARAM_SetMoviePeripheralLightCorrection    = 1,
	XH2_API_PARAM_GetMoviePeripheralLightCorrection    = 1,
	XH2_API_PARAM_CapMovieFaceDetectionMode            = 2,
	XH2_API_PARAM_SetMovieFaceDetectionMode            = 1,
	XH2_API_PARAM_GetMovieFaceDetectionMode            = 1,
	XH2_API_PARAM_CapMovieEyeAFMode                    = 2,
	XH2_API_PARAM_SetMovieEyeAFMode                    = 1,
	XH2_API_PARAM_GetMovieEyeAFMode                    = 1,
	XH2_API_PARAM_CapMovieSubjectDetectionMode         = 2,
	XH2_API_PARAM_SetMovieSubjectDetectionMode         = 1,
	XH2_API_PARAM_GetMovieSubjectDetectionMode         = 1,
	XH2_API_PARAM_CapMovieAFCCustom                    = 2,
	XH2_API_PARAM_SetMovieAFCCustom                    = 1,
	XH2_API_PARAM_GetMovieAFCCustom                    = 1,
	XH2_API_PARAM_CapMovieMFAssistMode                 = 2,
	XH2_API_PARAM_SetMovieMFAssistMode                 = 1,
	XH2_API_PARAM_GetMovieMFAssistMode                 = 1,
	XH2_API_PARAM_GetMovieFocusMeter                   = 3,
	XH2_API_PARAM_CapMovieFocusCheckMode               = 2,
	XH2_API_PARAM_SetMovieFocusCheckMode               = 1,
	XH2_API_PARAM_GetMovieFocusCheckMode               = 1,
	XH2_API_PARAM_CapMovieFocusCheckLock               = 2,
	XH2_API_PARAM_SetMovieFocusCheckLock               = 1,
	XH2_API_PARAM_GetMovieFocusCheckLock               = 1,
	XH2_API_PARAM_GetMovieRecordingTime                = 3,
	XH2_API_PARAM_GetMovieRemainingTime                = 3,
	XH2_API_PARAM_GetTimeCode                          = 4,
	XH2_API_PARAM_CapTimeCodeDisplay                   = 2,
	XH2_API_PARAM_SetTimeCodeDisplay                   = 1,
	XH2_API_PARAM_GetTimeCodeDisplay                   = 1,
	XH2_API_PARAM_CapTimeCodeStartSetting              = 2,
	XH2_API_PARAM_SetTimeCodeStartSetting              = 5,
	XH2_API_PARAM_CapTimeCodeCountUp                   = 2,
	XH2_API_PARAM_SetTimeCodeCountUp                   = 1,
	XH2_API_PARAM_GetTimeCodeCountUp                   = 1,
	XH2_API_PARAM_CapTimeCodeDropFrame                 = 2,
	XH2_API_PARAM_SetTimeCodeDropFrame                 = 1,
	XH2_API_PARAM_GetTimeCodeDropFrame                 = 1,
	XH2_API_PARAM_CapTimeCodeHDMIOutput                = 2,
	XH2_API_PARAM_SetTimeCodeHDMIOutput                = 1,
	XH2_API_PARAM_GetTimeCodeHDMIOutput                = 1,
	XH2_API_PARAM_GetTimeCodeCurrentValue              = 4,
	XH2_API_PARAM_CapATOMOSAirGluConnection            = 2,
	XH2_API_PARAM_SetATOMOSAirGluConnection            = 1,
	XH2_API_PARAM_GetATOMOSAirGluConnection            = 1,
	XH2_API_PARAM_GetTimeCodeStatus                    = 1,
	XH2_API_PARAM_CapInternalMicLevel                  = 2,
	XH2_API_PARAM_SetInternalMicLevel                  = 1,
	XH2_API_PARAM_GetInternalMicLevel                  = 1,
	XH2_API_PARAM_CapInternalMicLevelManual            = 2,
	XH2_API_PARAM_SetInternalMicLevelManual            = 1,
	XH2_API_PARAM_GetInternalMicLevelManual            = 1,
	XH2_API_PARAM_CapExternalMicLevel                  = 2,
	XH2_API_PARAM_SetExternalMicLevel                  = 1,
	XH2_API_PARAM_GetExternalMicLevel                  = 1,
	XH2_API_PARAM_CapExternalMicLevelManual            = 2,
	XH2_API_PARAM_SetExternalMicLevelManual            = 1,
	XH2_API_PARAM_GetExternalMicLevelManual            = 1,
	XH2_API_PARAM_CapMicLevelLimiter                   = 2,
	XH2_API_PARAM_SetMicLevelLimiter                   = 1,
	XH2_API_PARAM_GetMicLevelLimiter                   = 1,
	XH2_API_PARAM_CapWindFilter                        = 2,
	XH2_API_PARAM_SetWindFilter                        = 1,
	XH2_API_PARAM_GetWindFilter                        = 1,
	XH2_API_PARAM_CapLowCutFilter                      = 2,
	XH2_API_PARAM_SetLowCutFilter                      = 1,
	XH2_API_PARAM_GetLowCutFilter                      = 1,
	XH2_API_PARAM_CapHeadPhonesVolume                  = 2,
	XH2_API_PARAM_SetHeadPhonesVolume                  = 1,
	XH2_API_PARAM_GetHeadPhonesVolume                  = 1,
	XH2_API_PARAM_CapXLRAdapterMicSource               = 2,
	XH2_API_PARAM_SetXLRAdapterMicSource               = 1,
	XH2_API_PARAM_GetXLRAdapterMicSource               = 1,
	XH2_API_PARAM_CapXLRAdapterMoniteringSource        = 2,
	XH2_API_PARAM_SetXLRAdapterMoniteringSource        = 1,
	XH2_API_PARAM_GetXLRAdapterMoniteringSource        = 1,
	XH2_API_PARAM_CapXLRAdapterHDMIOutputSource        = 2,
	XH2_API_PARAM_SetXLRAdapterHDMIOutputSource        = 1,
	XH2_API_PARAM_GetXLRAdapterHDMIOutputSource        = 1,
	XH2_API_PARAM_GetMicLevelIndicator                 = 1,
	XH2_API_PARAM_GetMovieTransparentFrameInfo         = -1,
	XH2_API_PARAM_CapShortMovieSecond                  = -1,
	XH2_API_PARAM_SetShortMovieSecond                  = -1,
	XH2_API_PARAM_GetShortMovieSecond                  = -1,
	XH2_API_PARAM_CapMoviePortraitEnhancer             = -1,
	XH2_API_PARAM_SetMoviePortraitEnhancer             = -1,
	XH2_API_PARAM_GetMoviePortraitEnhancer             = -1,
	XH2_API_PARAM_CapDirectionalMic                    = 2,
	XH2_API_PARAM_SetDirectionalMic                    = 1,
	XH2_API_PARAM_GetDirectionalMic                    = 1,
	XH2_API_PARAM_CapEnvSoundsNR                       = -1,
	XH2_API_PARAM_SetEnvSoundsNR                       = -1,
	XH2_API_PARAM_GetEnvSoundsNR                       = -1,
	XH2_API_PARAM_GetBodyTemperatureWarning            = 1,
	XH2_API_PARAM_GetPlayBackMediaInfo                 = -1,
	XH2_API_PARAM_GetPlayBackObjectInfo                = -1,
	XH2_API_PARAM_GetPlayBackThumb                     = -1,
	XH2_API_PARAM_GetPlayBackPartialObject             = -1,
	XH2_API_PARAM_CapMovieRecVolume                    = 2,
	XH2_API_PARAM_SetMovieRecVolume                    = 1,
	XH2_API_PARAM_GetMovieRecVolume                    = 1,
};

//
// API_CODE for X-H2
//
enum {
	XH2_API_CODE_CapMovieHighFrequencyFlickerlessMode   = API_CODE_CapMovieHighFrequencyFlickerlessMode,
	XH2_API_CODE_SetMovieHighFrequencyFlickerlessMode   = API_CODE_SetMovieHighFrequencyFlickerlessMode,
	XH2_API_CODE_GetMovieHighFrequencyFlickerlessMode   = API_CODE_GetMovieHighFrequencyFlickerlessMode,
	XH2_API_CODE_CapMovieAFMode                         = API_CODE_CapMovieAFMode,
	XH2_API_CODE_SetMovieAFMode                         = API_CODE_SetMovieAFMode,
	XH2_API_CODE_GetMovieAFMode                         = API_CODE_GetMovieAFMode,
	XH2_API_CODE_CapMovieFocusArea                      = API_CODE_CapMovieFocusArea,
	XH2_API_CODE_SetMovieFocusArea                      = API_CODE_SetMovieFocusArea,
	XH2_API_CODE_GetMovieFocusArea                      = API_CODE_GetMovieFocusArea,
	XH2_API_CODE_GetTrackingAfFrameInfo                 = API_CODE_GetTrackingAfFrameInfo,
	XH2_API_CODE_CapMovieResolution                     = API_CODE_CapMovieResolution,
	XH2_API_CODE_SetMovieResolution                     = API_CODE_SetMovieResolution,
	XH2_API_CODE_GetMovieResolution                     = API_CODE_GetMovieResolution,
	XH2_API_CODE_CapMovieFrameRate                      = API_CODE_CapMovieFrameRate,
	XH2_API_CODE_SetMovieFrameRate                      = API_CODE_SetMovieFrameRate,
	XH2_API_CODE_GetMovieFrameRate                      = API_CODE_GetMovieFrameRate,
	XH2_API_CODE_CapMovieBitRate                        = API_CODE_CapMovieBitRate,
	XH2_API_CODE_SetMovieBitRate                        = API_CODE_SetMovieBitRate,
	XH2_API_CODE_GetMovieBitRate                        = API_CODE_GetMovieBitRate,
	XH2_API_CODE_CapMovieFileFormat                     = API_CODE_CapMovieFileFormat,
	XH2_API_CODE_SetMovieFileFormat                     = API_CODE_SetMovieFileFormat,
	XH2_API_CODE_GetMovieFileFormat                     = API_CODE_GetMovieFileFormat,
	XH2_API_CODE_CapHighSpeedRecMode                    = API_CODE_CapHighSpeedRecMode,
	XH2_API_CODE_SetHighSpeedRecMode                    = API_CODE_SetHighSpeedRecMode,
	XH2_API_CODE_GetHighSpeedRecMode                    = API_CODE_GetHighSpeedRecMode,
	XH2_API_CODE_CapHighSpeedRecResolution              = API_CODE_CapHighSpeedRecResolution,
	XH2_API_CODE_SetHighSpeedRecResolution              = API_CODE_SetHighSpeedRecResolution,
	XH2_API_CODE_GetHighSpeedRecResolution              = API_CODE_GetHighSpeedRecResolution,
	XH2_API_CODE_CapHighSpeedRecFrameRate               = API_CODE_CapHighSpeedRecFrameRate,
	XH2_API_CODE_SetHighSpeedRecFrameRate               = API_CODE_SetHighSpeedRecFrameRate,
	XH2_API_CODE_GetHighSpeedRecFrameRate               = API_CODE_GetHighSpeedRecFrameRate,
	XH2_API_CODE_CapHighSpeedRecPlayBackFrameRate       = API_CODE_CapHighSpeedRecPlayBackFrameRate,
	XH2_API_CODE_SetHighSpeedRecPlayBackFrameRate       = API_CODE_SetHighSpeedRecPlayBackFrameRate,
	XH2_API_CODE_GetHighSpeedRecPlayBackFrameRate       = API_CODE_GetHighSpeedRecPlayBackFrameRate,
	XH2_API_CODE_CapMovieMediaRecord                    = API_CODE_CapMovieMediaRecord,
	XH2_API_CODE_SetMovieMediaRecord                    = API_CODE_SetMovieMediaRecord,
	XH2_API_CODE_GetMovieMediaRecord                    = API_CODE_GetMovieMediaRecord,
	XH2_API_CODE_CapMovieMediaRecordProRes              = API_CODE_CapMovieMediaRecordProRes,
	XH2_API_CODE_SetMovieMediaRecordProRes              = API_CODE_SetMovieMediaRecordProRes,
	XH2_API_CODE_GetMovieMediaRecordProRes              = API_CODE_GetMovieMediaRecordProRes,
	XH2_API_CODE_CapMovieHDMIOutputRAW                  = API_CODE_CapMovieHDMIOutputRAW,
	XH2_API_CODE_SetMovieHDMIOutputRAW                  = API_CODE_SetMovieHDMIOutputRAW,
	XH2_API_CODE_GetMovieHDMIOutputRAW                  = API_CODE_GetMovieHDMIOutputRAW,
	XH2_API_CODE_CapMovieHDMIOutputRAWResolution        = API_CODE_CapMovieHDMIOutputRAWResolution,
	XH2_API_CODE_SetMovieHDMIOutputRAWResolution        = API_CODE_SetMovieHDMIOutputRAWResolution,
	XH2_API_CODE_GetMovieHDMIOutputRAWResolution        = API_CODE_GetMovieHDMIOutputRAWResolution,
	XH2_API_CODE_CapMovieHDMIOutputRAWFrameRate         = API_CODE_CapMovieHDMIOutputRAWFrameRate,
	XH2_API_CODE_SetMovieHDMIOutputRAWFrameRate         = API_CODE_SetMovieHDMIOutputRAWFrameRate,
	XH2_API_CODE_GetMovieHDMIOutputRAWFrameRate         = API_CODE_GetMovieHDMIOutputRAWFrameRate,
	XH2_API_CODE_CapFlogRecording                       = API_CODE_CapFlogRecording,
	XH2_API_CODE_SetFlogRecording                       = API_CODE_SetFlogRecording,
	XH2_API_CODE_GetFlogRecording                       = API_CODE_GetFlogRecording,
	XH2_API_CODE_CapMovieCropMagnification              = API_CODE_CapMovieCropMagnification,
	XH2_API_CODE_SetMovieCropMagnification              = API_CODE_SetMovieCropMagnification,
	XH2_API_CODE_GetMovieCropMagnification              = API_CODE_GetMovieCropMagnification,
	XH2_API_CODE_CapMovieHDMIOutputInfoDisplay          = API_CODE_CapMovieHDMIOutputInfoDisplay,
	XH2_API_CODE_SetMovieHDMIOutputInfoDisplay          = API_CODE_SetMovieHDMIOutputInfoDisplay,
	XH2_API_CODE_GetMovieHDMIOutputInfoDisplay          = API_CODE_GetMovieHDMIOutputInfoDisplay,
	XH2_API_CODE_CapMovieHDMIRecControl                 = API_CODE_CapMovieHDMIRecControl,
	XH2_API_CODE_SetMovieHDMIRecControl                 = API_CODE_SetMovieHDMIRecControl,
	XH2_API_CODE_GetMovieHDMIRecControl                 = API_CODE_GetMovieHDMIRecControl,
	XH2_API_CODE_CapMovieIsMode                         = API_CODE_CapMovieIsMode,
	XH2_API_CODE_SetMovieIsMode                         = API_CODE_SetMovieIsMode,
	XH2_API_CODE_GetMovieIsMode                         = API_CODE_GetMovieIsMode,
	XH2_API_CODE_CapMovieIsModeBoost                    = API_CODE_CapMovieIsModeBoost,
	XH2_API_CODE_SetMovieIsModeBoost                    = API_CODE_SetMovieIsModeBoost,
	XH2_API_CODE_GetMovieIsModeBoost                    = API_CODE_GetMovieIsModeBoost,
	XH2_API_CODE_CapMovieZebraSetting                   = API_CODE_CapMovieZebraSetting,
	XH2_API_CODE_SetMovieZebraSetting                   = API_CODE_SetMovieZebraSetting,
	XH2_API_CODE_GetMovieZebraSetting                   = API_CODE_GetMovieZebraSetting,
	XH2_API_CODE_CapMovieZebraLevel                     = API_CODE_CapMovieZebraLevel,
	XH2_API_CODE_SetMovieZebraLevel                     = API_CODE_SetMovieZebraLevel,
	XH2_API_CODE_GetMovieZebraLevel                     = API_CODE_GetMovieZebraLevel,
	XH2_API_CODE_CapMovieOptimizedControl               = API_CODE_CapMovieOptimizedControl,
	XH2_API_CODE_SetMovieOptimizedControl               = API_CODE_SetMovieOptimizedControl,
	XH2_API_CODE_GetMovieOptimizedControl               = API_CODE_GetMovieOptimizedControl,
	XH2_API_CODE_CapMovieTallyLight                     = API_CODE_CapMovieTallyLight,
	XH2_API_CODE_SetMovieTallyLight                     = API_CODE_SetMovieTallyLight,
	XH2_API_CODE_GetMovieTallyLight                     = API_CODE_GetMovieTallyLight,
	XH2_API_CODE_GetMediaEjectWarning                   = API_CODE_GetMediaEjectWarning,
	XH2_API_CODE_CapMovieDataLevelSetting               = API_CODE_CapMovieDataLevelSetting,
	XH2_API_CODE_SetMovieDataLevelSetting               = API_CODE_SetMovieDataLevelSetting,
	XH2_API_CODE_GetMovieDataLevelSetting               = API_CODE_GetMovieDataLevelSetting,
	XH2_API_CODE_GetMovieCropMagnificationValue         = API_CODE_GetMovieCropMagnificationValue,
	XH2_API_CODE_CapMovieFullTimeManual                 = API_CODE_CapMovieFullTimeManual,
	XH2_API_CODE_SetMovieFullTimeManual                 = API_CODE_SetMovieFullTimeManual,
	XH2_API_CODE_GetMovieFullTimeManual                 = API_CODE_GetMovieFullTimeManual,
	XH2_API_CODE_CapMovieDigitalZoom                    = API_CODE_CapMovieDigitalZoom,
	XH2_API_CODE_SetMovieDigitalZoom                    = API_CODE_SetMovieDigitalZoom,
	XH2_API_CODE_GetMovieDigitalZoom                    = API_CODE_GetMovieDigitalZoom,
	XH2_API_CODE_GetMovieDigitalZoomRange               = API_CODE_GetMovieDigitalZoomRange,
	XH2_API_CODE_CapMovieImageFormat                    = API_CODE_CapMovieImageFormat,
	XH2_API_CODE_SetMovieImageFormat                    = API_CODE_SetMovieImageFormat,
	XH2_API_CODE_GetMovieImageFormat                    = API_CODE_GetMovieImageFormat,
	XH2_API_CODE_GetHistogramData                       = API_CODE_GetHistogramData,
	XH2_API_CODE_GetFocusMapData                        = API_CODE_GetFocusMapData,
	XH2_API_CODE_CapWaveFormVectorScope                 = API_CODE_CapWaveFormVectorScope,
	XH2_API_CODE_SetWaveFormVectorScope                 = API_CODE_SetWaveFormVectorScope,
	XH2_API_CODE_GetWaveFormVectorScope                 = API_CODE_GetWaveFormVectorScope,
	XH2_API_CODE_GetWaveFormData                        = API_CODE_GetWaveFormData,
	XH2_API_CODE_GetVectorScopeData                     = API_CODE_GetVectorScopeData,
	XH2_API_CODE_GetParadeData                          = API_CODE_GetParadeData,
	XH2_API_CODE_CapRecFrameIndicator                   = API_CODE_CapRecFrameIndicator,
	XH2_API_CODE_SetRecFrameIndicator                   = API_CODE_SetRecFrameIndicator,
	XH2_API_CODE_GetRecFrameIndicator                   = API_CODE_GetRecFrameIndicator,
	XH2_API_CODE_CapMovieCaptureDelay                   = API_CODE_CapMovieCaptureDelay,
	XH2_API_CODE_SetMovieCaptureDelay                   = API_CODE_SetMovieCaptureDelay,
	XH2_API_CODE_GetMovieCaptureDelay                   = API_CODE_GetMovieCaptureDelay,
	XH2_API_CODE_CapAnamorphicDesqueezeDisplay          = API_CODE_CapAnamorphicDesqueezeDisplay,
	XH2_API_CODE_SetAnamorphicDesqueezeDisplay          = API_CODE_SetAnamorphicDesqueezeDisplay,
	XH2_API_CODE_GetAnamorphicDesqueezeDisplay          = API_CODE_GetAnamorphicDesqueezeDisplay,
	XH2_API_CODE_CapAnamorphicMagnification             = API_CODE_CapAnamorphicMagnification,
	XH2_API_CODE_SetAnamorphicMagnification             = API_CODE_SetAnamorphicMagnification,
	XH2_API_CODE_GetAnamorphicMagnification             = API_CODE_GetAnamorphicMagnification,
	XH2_API_CODE_CapWaveFormSetting                     = API_CODE_CapWaveFormSetting,
	XH2_API_CODE_SetWaveFormSetting                     = API_CODE_SetWaveFormSetting,
	XH2_API_CODE_GetWaveFormSetting                     = API_CODE_GetWaveFormSetting,
	XH2_API_CODE_CapVectorScopeSetting                  = API_CODE_CapVectorScopeSetting,
	XH2_API_CODE_SetVectorScopeSetting                  = API_CODE_SetVectorScopeSetting,
	XH2_API_CODE_GetVectorScopeSetting                  = API_CODE_GetVectorScopeSetting,
	XH2_API_CODE_CapParadeSettingDisplay                = API_CODE_CapParadeSettingDisplay,
	XH2_API_CODE_SetParadeSettingDisplay                = API_CODE_SetParadeSettingDisplay,
	XH2_API_CODE_GetParadeSettingDisplay                = API_CODE_GetParadeSettingDisplay,
	XH2_API_CODE_CapParadeSettingColor                  = API_CODE_CapParadeSettingColor,
	XH2_API_CODE_SetParadeSettingColor                  = API_CODE_SetParadeSettingColor,
	XH2_API_CODE_GetParadeSettingColor                  = API_CODE_GetParadeSettingColor,
	XH2_API_CODE_CapFlogDRangePriority                  = API_CODE_CapFlogDRangePriority,
	XH2_API_CODE_SetFlogDRangePriority                  = API_CODE_SetFlogDRangePriority,
	XH2_API_CODE_GetFlogDRangePriority                  = API_CODE_GetFlogDRangePriority,
	XH2_API_CODE_CapMovieWhiteBalanceTune               = API_CODE_CapMovieWhiteBalanceTune,
	XH2_API_CODE_SetMovieWhiteBalanceTune               = API_CODE_SetMovieWhiteBalanceTune,
	XH2_API_CODE_GetMovieWhiteBalanceTune               = API_CODE_GetMovieWhiteBalanceTune,
	XH2_API_CODE_CapMovieFilmSimulationMode             = API_CODE_CapMovieFilmSimulationMode,
	XH2_API_CODE_SetMovieFilmSimulationMode             = API_CODE_SetMovieFilmSimulationMode,
	XH2_API_CODE_GetMovieFilmSimulationMode             = API_CODE_GetMovieFilmSimulationMode,
	XH2_API_CODE_CapMovieMonochromaticColor             = API_CODE_CapMovieMonochromaticColor,
	XH2_API_CODE_SetMovieMonochromaticColor             = API_CODE_SetMovieMonochromaticColor,
	XH2_API_CODE_GetMovieMonochromaticColor             = API_CODE_GetMovieMonochromaticColor,
	XH2_API_CODE_CapMovieHighLightTone                  = API_CODE_CapMovieHighLightTone,
	XH2_API_CODE_SetMovieHighLightTone                  = API_CODE_SetMovieHighLightTone,
	XH2_API_CODE_GetMovieHighLightTone                  = API_CODE_GetMovieHighLightTone,
	XH2_API_CODE_CapMovieShadowTone                     = API_CODE_CapMovieShadowTone,
	XH2_API_CODE_SetMovieShadowTone                     = API_CODE_SetMovieShadowTone,
	XH2_API_CODE_GetMovieShadowTone                     = API_CODE_GetMovieShadowTone,
	XH2_API_CODE_CapMovieSharpness                      = API_CODE_CapMovieSharpness,
	XH2_API_CODE_SetMovieSharpness                      = API_CODE_SetMovieSharpness,
	XH2_API_CODE_GetMovieSharpness                      = API_CODE_GetMovieSharpness,
	XH2_API_CODE_CapMovieColorMode                      = API_CODE_CapMovieColorMode,
	XH2_API_CODE_SetMovieColorMode                      = API_CODE_SetMovieColorMode,
	XH2_API_CODE_GetMovieColorMode                      = API_CODE_GetMovieColorMode,
	XH2_API_CODE_CapMovieNoiseReduction                 = API_CODE_CapMovieNoiseReduction,
	XH2_API_CODE_SetMovieNoiseReduction                 = API_CODE_SetMovieNoiseReduction,
	XH2_API_CODE_GetMovieNoiseReduction                 = API_CODE_GetMovieNoiseReduction,
	XH2_API_CODE_CapInterFrameNR                        = API_CODE_CapInterFrameNR,
	XH2_API_CODE_SetInterFrameNR                        = API_CODE_SetInterFrameNR,
	XH2_API_CODE_GetInterFrameNR                        = API_CODE_GetInterFrameNR,
	XH2_API_CODE_CapMoviePeripheralLightCorrection      = API_CODE_CapMoviePeripheralLightCorrection,
	XH2_API_CODE_SetMoviePeripheralLightCorrection      = API_CODE_SetMoviePeripheralLightCorrection,
	XH2_API_CODE_GetMoviePeripheralLightCorrection      = API_CODE_GetMoviePeripheralLightCorrection,
	XH2_API_CODE_CapMovieFaceDetectionMode              = API_CODE_CapMovieFaceDetectionMode,
	XH2_API_CODE_SetMovieFaceDetectionMode              = API_CODE_SetMovieFaceDetectionMode,
	XH2_API_CODE_GetMovieFaceDetectionMode              = API_CODE_GetMovieFaceDetectionMode,
	XH2_API_CODE_CapMovieEyeAFMode                      = API_CODE_CapMovieEyeAFMode,
	XH2_API_CODE_SetMovieEyeAFMode                      = API_CODE_SetMovieEyeAFMode,
	XH2_API_CODE_GetMovieEyeAFMode                      = API_CODE_GetMovieEyeAFMode,
	XH2_API_CODE_CapMovieSubjectDetectionMode           = API_CODE_CapMovieSubjectDetectionMode,
	XH2_API_CODE_SetMovieSubjectDetectionMode           = API_CODE_SetMovieSubjectDetectionMode,
	XH2_API_CODE_GetMovieSubjectDetectionMode           = API_CODE_GetMovieSubjectDetectionMode,
	XH2_API_CODE_CapMovieAFCCustom                      = API_CODE_CapMovieAFCCustom,
	XH2_API_CODE_SetMovieAFCCustom                      = API_CODE_SetMovieAFCCustom,
	XH2_API_CODE_GetMovieAFCCustom                      = API_CODE_GetMovieAFCCustom,
	XH2_API_CODE_CapMovieMFAssistMode                   = API_CODE_CapMovieMFAssistMode,
	XH2_API_CODE_SetMovieMFAssistMode                   = API_CODE_SetMovieMFAssistMode,
	XH2_API_CODE_GetMovieMFAssistMode                   = API_CODE_GetMovieMFAssistMode,
	XH2_API_CODE_GetMovieFocusMeter                     = API_CODE_GetMovieFocusMeter,
	XH2_API_CODE_CapMovieFocusCheckMode                 = API_CODE_CapMovieFocusCheckMode,
	XH2_API_CODE_SetMovieFocusCheckMode                 = API_CODE_SetMovieFocusCheckMode,
	XH2_API_CODE_GetMovieFocusCheckMode                 = API_CODE_GetMovieFocusCheckMode,
	XH2_API_CODE_CapMovieFocusCheckLock                 = API_CODE_CapMovieFocusCheckLock,
	XH2_API_CODE_SetMovieFocusCheckLock                 = API_CODE_SetMovieFocusCheckLock,
	XH2_API_CODE_GetMovieFocusCheckLock                 = API_CODE_GetMovieFocusCheckLock,
	XH2_API_CODE_GetMovieRecordingTime                  = API_CODE_GetMovieRecordingTime,
	XH2_API_CODE_GetMovieRemainingTime                  = API_CODE_GetMovieRemainingTime,
	XH2_API_CODE_GetTimeCode                            = API_CODE_GetTimeCode,
	XH2_API_CODE_CapTimeCodeDisplay                     = API_CODE_CapTimeCodeDisplay,
	XH2_API_CODE_SetTimeCodeDisplay                     = API_CODE_SetTimeCodeDisplay,
	XH2_API_CODE_GetTimeCodeDisplay                     = API_CODE_GetTimeCodeDisplay,
	XH2_API_CODE_CapTimeCodeStartSetting                = API_CODE_CapTimeCodeStartSetting,
	XH2_API_CODE_SetTimeCodeStartSetting                = API_CODE_SetTimeCodeStartSetting,
	XH2_API_CODE_CapTimeCodeCountUp                     = API_CODE_CapTimeCodeCountUp,
	XH2_API_CODE_SetTimeCodeCountUp                     = API_CODE_SetTimeCodeCountUp,
	XH2_API_CODE_GetTimeCodeCountUp                     = API_CODE_GetTimeCodeCountUp,
	XH2_API_CODE_CapTimeCodeDropFrame                   = API_CODE_CapTimeCodeDropFrame,
	XH2_API_CODE_SetTimeCodeDropFrame                   = API_CODE_SetTimeCodeDropFrame,
	XH2_API_CODE_GetTimeCodeDropFrame                   = API_CODE_GetTimeCodeDropFrame,
	XH2_API_CODE_CapTimeCodeHDMIOutput                  = API_CODE_CapTimeCodeHDMIOutput,
	XH2_API_CODE_SetTimeCodeHDMIOutput                  = API_CODE_SetTimeCodeHDMIOutput,
	XH2_API_CODE_GetTimeCodeHDMIOutput                  = API_CODE_GetTimeCodeHDMIOutput,
	XH2_API_CODE_GetTimeCodeCurrentValue                = API_CODE_GetTimeCodeCurrentValue,
	XH2_API_CODE_CapATOMOSAirGluConnection              = API_CODE_CapATOMOSAirGluConnection,
	XH2_API_CODE_SetATOMOSAirGluConnection              = API_CODE_SetATOMOSAirGluConnection,
	XH2_API_CODE_GetATOMOSAirGluConnection              = API_CODE_GetATOMOSAirGluConnection,
	XH2_API_CODE_GetTimeCodeStatus                      = API_CODE_GetTimeCodeStatus,
	XH2_API_CODE_CapInternalMicLevel                    = API_CODE_CapInternalMicLevel,
	XH2_API_CODE_SetInternalMicLevel                    = API_CODE_SetInternalMicLevel,
	XH2_API_CODE_GetInternalMicLevel                    = API_CODE_GetInternalMicLevel,
	XH2_API_CODE_CapInternalMicLevelManual              = API_CODE_CapInternalMicLevelManual,
	XH2_API_CODE_SetInternalMicLevelManual              = API_CODE_SetInternalMicLevelManual,
	XH2_API_CODE_GetInternalMicLevelManual              = API_CODE_GetInternalMicLevelManual,
	XH2_API_CODE_CapExternalMicLevel                    = API_CODE_CapExternalMicLevel,
	XH2_API_CODE_SetExternalMicLevel                    = API_CODE_SetExternalMicLevel,
	XH2_API_CODE_GetExternalMicLevel                    = API_CODE_GetExternalMicLevel,
	XH2_API_CODE_CapExternalMicLevelManual              = API_CODE_CapExternalMicLevelManual,
	XH2_API_CODE_SetExternalMicLevelManual              = API_CODE_SetExternalMicLevelManual,
	XH2_API_CODE_GetExternalMicLevelManual              = API_CODE_GetExternalMicLevelManual,
	XH2_API_CODE_CapMicLevelLimiter                     = API_CODE_CapMicLevelLimiter,
	XH2_API_CODE_SetMicLevelLimiter                     = API_CODE_SetMicLevelLimiter,
	XH2_API_CODE_GetMicLevelLimiter                     = API_CODE_GetMicLevelLimiter,
	XH2_API_CODE_CapWindFilter                          = API_CODE_CapWindFilter,
	XH2_API_CODE_SetWindFilter                          = API_CODE_SetWindFilter,
	XH2_API_CODE_GetWindFilter                          = API_CODE_GetWindFilter,
	XH2_API_CODE_CapLowCutFilter                        = API_CODE_CapLowCutFilter,
	XH2_API_CODE_SetLowCutFilter                        = API_CODE_SetLowCutFilter,
	XH2_API_CODE_GetLowCutFilter                        = API_CODE_GetLowCutFilter,
	XH2_API_CODE_CapHeadPhonesVolume                    = API_CODE_CapHeadPhonesVolume,
	XH2_API_CODE_SetHeadPhonesVolume                    = API_CODE_SetHeadPhonesVolume,
	XH2_API_CODE_GetHeadPhonesVolume                    = API_CODE_GetHeadPhonesVolume,
	XH2_API_CODE_CapXLRAdapterMicSource                 = API_CODE_CapXLRAdapterMicSource,
	XH2_API_CODE_SetXLRAdapterMicSource                 = API_CODE_SetXLRAdapterMicSource,
	XH2_API_CODE_GetXLRAdapterMicSource                 = API_CODE_GetXLRAdapterMicSource,
	XH2_API_CODE_CapXLRAdapterMoniteringSource          = API_CODE_CapXLRAdapterMoniteringSource,
	XH2_API_CODE_SetXLRAdapterMoniteringSource          = API_CODE_SetXLRAdapterMoniteringSource,
	XH2_API_CODE_GetXLRAdapterMoniteringSource          = API_CODE_GetXLRAdapterMoniteringSource,
	XH2_API_CODE_CapXLRAdapterHDMIOutputSource          = API_CODE_CapXLRAdapterHDMIOutputSource,
	XH2_API_CODE_SetXLRAdapterHDMIOutputSource          = API_CODE_SetXLRAdapterHDMIOutputSource,
	XH2_API_CODE_GetXLRAdapterHDMIOutputSource          = API_CODE_GetXLRAdapterHDMIOutputSource,
	XH2_API_CODE_GetMicLevelIndicator                   = API_CODE_GetMicLevelIndicator,
	XH2_API_CODE_GetMovieTransparentFrameInfo           = API_CODE_GetMovieTransparentFrameInfo,
	XH2_API_CODE_CapShortMovieSecond                    = API_CODE_CapShortMovieSecond,
	XH2_API_CODE_SetShortMovieSecond                    = API_CODE_SetShortMovieSecond,
	XH2_API_CODE_GetShortMovieSecond                    = API_CODE_GetShortMovieSecond,
	XH2_API_CODE_CapMoviePortraitEnhancer               = API_CODE_CapMoviePortraitEnhancer,
	XH2_API_CODE_SetMoviePortraitEnhancer               = API_CODE_SetMoviePortraitEnhancer,
	XH2_API_CODE_GetMoviePortraitEnhancer               = API_CODE_GetMoviePortraitEnhancer,
	XH2_API_CODE_CapDirectionalMic                      = API_CODE_CapDirectionalMic,
	XH2_API_CODE_SetDirectionalMic                      = API_CODE_SetDirectionalMic,
	XH2_API_CODE_GetDirectionalMic                      = API_CODE_GetDirectionalMic,
	XH2_API_CODE_CapEnvSoundsNR                         = API_CODE_CapEnvSoundsNR,
	XH2_API_CODE_SetEnvSoundsNR                         = API_CODE_SetEnvSoundsNR,
	XH2_API_CODE_GetEnvSoundsNR                         = API_CODE_GetEnvSoundsNR,
	XH2_API_CODE_GetBodyTemperatureWarning              = API_CODE_GetBodyTemperatureWarning,
	XH2_API_CODE_GetPlayBackMediaInfo                   = API_CODE_GetPlayBackMediaInfo,
	XH2_API_CODE_GetPlayBackObjectInfo                  = API_CODE_GetPlayBackObjectInfo,
	XH2_API_CODE_GetPlayBackThumb                       = API_CODE_GetPlayBackThumb,
	XH2_API_CODE_GetPlayBackPartialObject               = API_CODE_GetPlayBackPartialObject,
	XH2_API_CODE_CapMovieRecVolume                      = API_CODE_CapMovieRecVolume,
	XH2_API_CODE_SetMovieRecVolume                      = API_CODE_SetMovieRecVolume,
	XH2_API_CODE_GetMovieRecVolume                      = API_CODE_GetMovieRecVolume,
};

// Movie Af Mode
enum{
	XH2_MOVIE_AF_MULTI                     = SDK_MOVIE_AF_MULTI,
	XH2_MOVIE_AF_AREA                      = SDK_MOVIE_AF_AREA,
	XH2_MOVIE_AF_WIDETRACKING              = SDK_MOVIE_AF_WIDETRACKING,
};

// Short Movie Second
enum{
	XH2_SHORT_MOVIE_SECOND_OFF             = SDK_SHORT_MOVIE_SECOND_OFF,
	XH2_SHORT_MOVIE_SECOND_15S             = SDK_SHORT_MOVIE_SECOND_15S,
	XH2_SHORT_MOVIE_SECOND_30S             = SDK_SHORT_MOVIE_SECOND_30S,
	XH2_SHORT_MOVIE_SECOND_60S             = SDK_SHORT_MOVIE_SECOND_60S,
};

// Movie Resolution
// High Speed Rec Resolution
enum{
	XH2_MOVIE_RESOLUTION_6P2K_3_2          = SDK_MOVIE_RESOLUTION_6P2K_3_2,
	XH2_MOVIE_RESOLUTION_8K_16_9           = SDK_MOVIE_RESOLUTION_8K_16_9,
	XH2_MOVIE_RESOLUTION_6K_16_9           = SDK_MOVIE_RESOLUTION_6K_16_9,
	XH2_MOVIE_RESOLUTION_4KHQ_16_9         = SDK_MOVIE_RESOLUTION_4KHQ_16_9,
	XH2_MOVIE_RESOLUTION_4K_16_9           = SDK_MOVIE_RESOLUTION_4K_16_9,
	XH2_MOVIE_RESOLUTION_DCIHQ_17_9        = SDK_MOVIE_RESOLUTION_DCIHQ_17_9,
	XH2_MOVIE_RESOLUTION_DCI_17_9          = SDK_MOVIE_RESOLUTION_DCI_17_9,
	XH2_MOVIE_RESOLUTION_FULLHD_16_9       = SDK_MOVIE_RESOLUTION_FULLHD_16_9,
	XH2_MOVIE_RESOLUTION_FULLHD_17_9       = SDK_MOVIE_RESOLUTION_FULLHD_17_9,
	XH2_MOVIE_RESOLUTION_5K_17_9           = SDK_MOVIE_RESOLUTION_5K_17_9,
	XH2_MOVIE_RESOLUTION_DCI_8K_17_9       = SDK_MOVIE_RESOLUTION_DCI_8K_17_9,
	XH2_MOVIE_RESOLUTION_CINESCO_2P35_1    = SDK_MOVIE_RESOLUTION_CINESCO_2P35_1,
	XH2_MOVIE_RESOLUTION_OPENGATE_3_2      = SDK_MOVIE_RESOLUTION_OPENGATE_3_2,
	XH2_MOVIE_RESOLUTION_35MM_16_9         = SDK_MOVIE_RESOLUTION_35MM_16_9,
	XH2_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1,
	XH2_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1,
	XH2_MOVIE_RESOLUTION_FULLFRAME_3_2     = SDK_MOVIE_RESOLUTION_FULLFRAME_3_2,
	XH2_MOVIE_RESOLUTION_FULLHD_LP_16_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_16_9,
	XH2_MOVIE_RESOLUTION_FULLHD_LP_17_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_17_9,
	XH2_MOVIE_RESOLUTION_4K_LP_16_9        = SDK_MOVIE_RESOLUTION_4K_LP_16_9,
	XH2_MOVIE_RESOLUTION_FULLHD_9_16       = SDK_MOVIE_RESOLUTION_FULLHD_9_16,
};

// Movie FrameRate
// High Speed Rec PlayBack FrameRate
enum{
	XH2_MOVIE_FRAMERATE_59_94P             = SDK_MOVIE_FRAMERATE_59_94P,
	XH2_MOVIE_FRAMERATE_50P                = SDK_MOVIE_FRAMERATE_50P,
	XH2_MOVIE_FRAMERATE_29_97P             = SDK_MOVIE_FRAMERATE_29_97P,
	XH2_MOVIE_FRAMERATE_25P                = SDK_MOVIE_FRAMERATE_25P,
	XH2_MOVIE_FRAMERATE_24P                = SDK_MOVIE_FRAMERATE_24P,
	XH2_MOVIE_FRAMERATE_23_98P             = SDK_MOVIE_FRAMERATE_23_98P,
};

// Movie BitRate
enum{
	XH2_MOVIE_BITRATE_720MBPS              = SDK_MOVIE_BITRATE_720MBPS,
	XH2_MOVIE_BITRATE_400MBPS              = SDK_MOVIE_BITRATE_400MBPS,
	XH2_MOVIE_BITRATE_360MBPS              = SDK_MOVIE_BITRATE_360MBPS,
	XH2_MOVIE_BITRATE_200MBPS              = SDK_MOVIE_BITRATE_200MBPS,
	XH2_MOVIE_BITRATE_100MBPS              = SDK_MOVIE_BITRATE_100MBPS,
	XH2_MOVIE_BITRATE_50MBPS               = SDK_MOVIE_BITRATE_50MBPS,
	XH2_MOVIE_BITRATE_25MBPS               = SDK_MOVIE_BITRATE_25MBPS,
	XH2_MOVIE_BITRATE_8MBPS                = SDK_MOVIE_BITRATE_8MBPS,
};

// Movie FileFormat
enum{
	XH2_MOVIE_FORMAT_H264_ALL_I_MOV        = SDK_MOVIE_FORMAT_H264_ALL_I_MOV,
	XH2_MOVIE_FORMAT_H264_LONGGOP_I_MOV    = SDK_MOVIE_FORMAT_H264_LONGGOP_I_MOV,
	XH2_MOVIE_FORMAT_H264_LONGGOP_MP4      = SDK_MOVIE_FORMAT_H264_LONGGOP_MP4,
	XH2_MOVIE_FORMAT_H265_4_2_0_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_0_ALL_I,
	XH2_MOVIE_FORMAT_H265_4_2_0_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_0_LONGGOP,
	XH2_MOVIE_FORMAT_H265_4_2_2_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_2_ALL_I,
	XH2_MOVIE_FORMAT_H265_4_2_2_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_2_LONGGOP,
	XH2_MOVIE_FORMAT_PRORESHQ              = SDK_MOVIE_FORMAT_PRORESHQ,
	XH2_MOVIE_FORMAT_PRORES                = SDK_MOVIE_FORMAT_PRORES,
	XH2_MOVIE_FORMAT_PRORESLT              = SDK_MOVIE_FORMAT_PRORESLT,
};

// High Speed RecMode
enum{
	XH2_HIGHSPEEDREC_OFF                   = SDK_HIGHSPEEDREC_OFF,
	XH2_HIGHSPEEDREC_ON                    = SDK_HIGHSPEEDREC_ON,
	XH2_HIGHSPEEDREC_ON_HDMI_ONLY          = SDK_HIGHSPEEDREC_ON_HDMI_ONLY,
};

// High Speed Rec FrameRate
enum{
	XH2_HIGHSPEEDREC_FRAMERATE_240P        = SDK_HIGHSPEEDREC_FRAMERATE_240P,
	XH2_HIGHSPEEDREC_FRAMERATE_200P        = SDK_HIGHSPEEDREC_FRAMERATE_200P,
	XH2_HIGHSPEEDREC_FRAMERATE_120P        = SDK_HIGHSPEEDREC_FRAMERATE_120P,
	XH2_HIGHSPEEDREC_FRAMERATE_100P        = SDK_HIGHSPEEDREC_FRAMERATE_100P,
};

// Movie Media Record
enum{
	XH2_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2,
	XH2_MOVIE_MEDIARECORD_SLOT2                  = SDK_MOVIE_MEDIARECORD_SLOT2,
	XH2_MOVIE_MEDIARECORD_SLOT1                  = SDK_MOVIE_MEDIARECORD_SLOT1,
	XH2_MOVIE_MEDIARECORD_BACKUP                 = SDK_MOVIE_MEDIARECORD_BACKUP,
	XH2_MOVIE_MEDIARECORD_SSD                    = SDK_MOVIE_MEDIARECORD_SSD,
	XH2_MOVIE_MEDIARECORD_SSD_CF                 = SDK_MOVIE_MEDIARECORD_SSD_CF,
	XH2_MOVIE_MEDIARECORD_OFF                    = SDK_MOVIE_MEDIARECORD_OFF,
	XH2_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF       = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF,
	XH2_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1,
};

// Movie Media Record ProRes
enum{
	XH2_MOVIE_MEDIARECORD_PRORES_OFF       = SDK_MOVIE_MEDIARECORD_PRORES_OFF,
	XH2_MOVIE_MEDIARECORD_PRORES_H264      = SDK_MOVIE_MEDIARECORD_PRORES_H264,
	XH2_MOVIE_MEDIARECORD_PRORES_PROXY     = SDK_MOVIE_MEDIARECORD_PRORES_PROXY,
};

// Movie HDMIOutput RAW
enum{
	XH2_MOVIE_HDMI_OUTPUT_RAW_OFF          = SDK_MOVIE_HDMI_OUTPUT_RAW_OFF,
	XH2_MOVIE_HDMI_OUTPUT_RAW_ATOMOS       = SDK_MOVIE_HDMI_OUTPUT_RAW_ATOMOS,
	XH2_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC   = SDK_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC,
};

// Movie HDMI Output RAW Resolution
enum{
	XH2_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K,
	XH2_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K,
	XH2_MOVIE_HDMI_OUTPUT_RESOLUTION_8K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_8K,
	XH2_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K,
	XH2_MOVIE_HDMI_OUTPUT_RESOLUTION_4K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4K,
	XH2_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K= SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K,
};

// Movie HDMI Output RAW FrameRate
enum{
	XH2_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P,
	XH2_MOVIE_HDMI_OUTPUT_FRAMERATE_50P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_50P,
	XH2_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P,
	XH2_MOVIE_HDMI_OUTPUT_FRAMERATE_25P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_25P,
	XH2_MOVIE_HDMI_OUTPUT_FRAMERATE_24P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_24P,
	XH2_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P,
};

// Flog Recording
enum{
	XH2_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM,
	XH2_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG,
	XH2_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2 = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2,
	XH2_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG,
	XH2_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2  = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2,
	XH2_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM,
	XH2_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM  = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM,
	XH2_MOVIERECORD_MEDIA_HLG_HDMI_HLG     = SDK_MOVIERECORD_MEDIA_HLG_HDMI_HLG,
};

// MovieIsMode
enum{
	XH2_MOVIE_IS_MODE_OFF                  = SDK_MOVIE_IS_MODE_OFF,
	XH2_MOVIE_IS_MODE_ON                   = SDK_MOVIE_IS_MODE_ON,
	XH2_MOVIE_IS_MODE_IBIS_OIS             = SDK_MOVIE_IS_MODE_IBIS_OIS,
	XH2_MOVIE_IS_MODE_IBIS_OIS_DIS         = SDK_MOVIE_IS_MODE_IBIS_OIS_DIS,
	XH2_MOVIE_IS_MODE_OIS                  = SDK_MOVIE_IS_MODE_OIS,
	XH2_MOVIE_IS_MODE_OIS_DIS              = SDK_MOVIE_IS_MODE_OIS_DIS,
};

// Movie Zebra Setting
enum{
	XH2_MOVIE_ZEBRA_SETTING_OFF            = SDK_MOVIE_ZEBRA_SETTING_OFF,
	XH2_MOVIE_ZEBRA_SETTING_RIGHT          = SDK_MOVIE_ZEBRA_SETTING_RIGHT,
	XH2_MOVIE_ZEBRA_SETTING_LEFT           = SDK_MOVIE_ZEBRA_SETTING_LEFT,
};

// Movie Tally Light
enum{
	XH2_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON      = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON,
	XH2_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK   = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK,
	XH2_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON       = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON,
	XH2_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF      = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF,
	XH2_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK,
	XH2_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF   = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF,
	XH2_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF     = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF,
};

// Media Eject Warning
enum{
	XH2_MEDIA_EJECT_WARNING_SLOT1          = SDK_MEDIA_EJECT_WARNING_SLOT1,
	XH2_MEDIA_EJECT_WARNING_SLOT2          = SDK_MEDIA_EJECT_WARNING_SLOT2,
	XH2_MEDIA_EJECT_WARNING_SLOT3          = SDK_MEDIA_EJECT_WARNING_SLOT3,
};

// Movie Data Level Setting
enum{
	XH2_MOVIE_DATA_LEVEL_SETTING_FULL      = SDK_MOVIE_DATA_LEVEL_SETTING_FULL,
	XH2_MOVIE_DATA_LEVEL_SETTING_VIDEO     = SDK_MOVIE_DATA_LEVEL_SETTING_VIDEO,
};

// Movie ImageFormat
enum{
	XH2_MOVIE_IMAGEFORMAT_GF               = SDK_MOVIE_IMAGEFORMAT_GF,
	XH2_MOVIE_IMAGEFORMAT_35MM             = SDK_MOVIE_IMAGEFORMAT_35MM,
	XH2_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM  = SDK_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM,
	XH2_MOVIE_IMAGEFORMAT_PREMISTA         = SDK_MOVIE_IMAGEFORMAT_PREMISTA,
};

// WaveForm VectorScope
enum{
	XH2_WAVEFORM_VECTORSCOPE_OFF           = SDK_WAVEFORM_VECTORSCOPE_OFF,
	XH2_WAVEFORM_VECTORSCOPE_WAVEFORM      = SDK_WAVEFORM_VECTORSCOPE_WAVEFORM,
	XH2_WAVEFORM_VECTORSCOPE_PARADE        = SDK_WAVEFORM_VECTORSCOPE_PARADE,
	XH2_WAVEFORM_VECTORSCOPE_VECTORSCOPE   = SDK_WAVEFORM_VECTORSCOPE_VECTORSCOPE,
};

// WaveForm Setting
enum{
	XH2_WAVEFORM_SETTING_PATTERN1          = SDK_WAVEFORM_SETTING_PATTERN1,
	XH2_WAVEFORM_SETTING_PATTERN2          = SDK_WAVEFORM_SETTING_PATTERN2,
	XH2_WAVEFORM_SETTING_PATTERN3          = SDK_WAVEFORM_SETTING_PATTERN3,
	XH2_WAVEFORM_SETTING_PATTERN4          = SDK_WAVEFORM_SETTING_PATTERN4,
};

// VectorScope Setting
enum{
	XH2_VECTORSCORE_SETTING_PATTERN1       = SDK_VECTORSCORE_SETTING_PATTERN1,
	XH2_VECTORSCORE_SETTING_PATTERN2       = SDK_VECTORSCORE_SETTING_PATTERN2,
	XH2_VECTORSCORE_SETTING_PATTERN3       = SDK_VECTORSCORE_SETTING_PATTERN3,
	XH2_VECTORSCORE_SETTING_PATTERN4       = SDK_VECTORSCORE_SETTING_PATTERN4,
};

// Parade Setting Display
enum{
	XH2_PARADE_SETTING_DISPLAY_PATTERN1    = SDK_PARADE_SETTING_DISPLAY_PATTERN1,
	XH2_PARADE_SETTING_DISPLAY_PATTERN2    = SDK_PARADE_SETTING_DISPLAY_PATTERN2,
	XH2_PARADE_SETTING_DISPLAY_PATTERN3    = SDK_PARADE_SETTING_DISPLAY_PATTERN3,
	XH2_PARADE_SETTING_DISPLAY_PATTERN4    = SDK_PARADE_SETTING_DISPLAY_PATTERN4,
};

// Parade Setting Color
enum{
	XH2_PARADE_SETTING_COLOR_RGB           = SDK_PARADE_SETTING_COLOR_RGB,
	XH2_PARADE_SETTING_COLOR_WHITE         = SDK_PARADE_SETTING_COLOR_WHITE,
};

// Movie FocusMeter(display)
enum{
	XH2_FOCUSMETER_DISPLAY_OFF             = SDK_FOCUSMETER_DISPLAY_OFF,
	XH2_FOCUSMETER_DISPLAY_ON              = SDK_FOCUSMETER_DISPLAY_ON,
};

// Movie FocusMeter(color)
enum{
	XH2_FOCUSMETER_COLOR_WHITE             = SDK_FOCUSMETER_COLOR_WHITE,
	XH2_FOCUSMETER_COLOR_GREEN             = SDK_FOCUSMETER_COLOR_GREEN,
};

// TimeCode Start Setting
enum{
	XH2_TIMECODE_START_SETTING_MANUAL      = SDK_TIMECODE_START_SETTING_MANUAL,
	XH2_TIMECODE_START_SETTING_CURRENT     = SDK_TIMECODE_START_SETTING_CURRENT,
	XH2_TIMECODE_START_SETTING_RESET       = SDK_TIMECODE_START_SETTING_RESET,
};

// TimeCode CountUp
enum{
	XH2_TIMECODE_COUNTUP_RECRUN            = SDK_TIMECODE_COUNTUP_RECRUN,
	XH2_TIMECODE_COUNTUP_FREERUN           = SDK_TIMECODE_COUNTUP_FREERUN,
};

// TimeCode Status
enum{
	XH2_TIMECODE_STATUS_USE_DSC_ALONE      = SDK_TIMECODE_STATUS_USE_DSC_ALONE,
	XH2_TIMECODE_STATUS_SYNCING            = SDK_TIMECODE_STATUS_SYNCING,
	XH2_TIMECODE_STATUS_DISCONNECTED       = SDK_TIMECODE_STATUS_DISCONNECTED,
	XH2_TIMECODE_STATUS_NOT_SYNCED         = SDK_TIMECODE_STATUS_NOT_SYNCED,
	XH2_TIMECODE_STATUS_FRAMERATE_MISMATCH = SDK_TIMECODE_STATUS_FRAMERATE_MISMATCH,
};

// Internal Mic Level
// External Mic Level
enum{
	XH2_MIC_LEVEL_OFF                      = SDK_MIC_LEVEL_OFF,
	XH2_MIC_LEVEL_MANUAL                   = SDK_MIC_LEVEL_MANUAL,
	XH2_MIC_LEVEL_AUTO                     = SDK_MIC_LEVEL_AUTO,
};

// HeadPhones Volume
enum{
	XH2_HEADPHONES_VOLUME_0                = SDK_HEADPHONES_VOLUME_0,
	XH2_HEADPHONES_VOLUME_1                = SDK_HEADPHONES_VOLUME_1,
	XH2_HEADPHONES_VOLUME_2                = SDK_HEADPHONES_VOLUME_2,
	XH2_HEADPHONES_VOLUME_3                = SDK_HEADPHONES_VOLUME_3,
	XH2_HEADPHONES_VOLUME_4                = SDK_HEADPHONES_VOLUME_4,
	XH2_HEADPHONES_VOLUME_5                = SDK_HEADPHONES_VOLUME_5,
	XH2_HEADPHONES_VOLUME_6                = SDK_HEADPHONES_VOLUME_6,
	XH2_HEADPHONES_VOLUME_7                = SDK_HEADPHONES_VOLUME_7,
	XH2_HEADPHONES_VOLUME_8                = SDK_HEADPHONES_VOLUME_8,
	XH2_HEADPHONES_VOLUME_9                = SDK_HEADPHONES_VOLUME_9,
	XH2_HEADPHONES_VOLUME_10               = SDK_HEADPHONES_VOLUME_10,
};

// XLR Adapter Mic Source
enum{
	XH2_XLRADAPTER_MIC_SOURCE_4CH          = SDK_XLRADAPTER_MIC_SOURCE_4CH,
	XH2_XLRADAPTER_MIC_SOURCE_2CH          = SDK_XLRADAPTER_MIC_SOURCE_2CH,
};

// XLR Adapter Monitering Source
enum{
	XH2_XLRADAPTER_MONITER_SOURCE_XLR      = SDK_XLRADAPTER_MONITER_SOURCE_XLR,
	XH2_XLRADAPTER_MONITER_SOURCE_CAMERA   = SDK_XLRADAPTER_MONITER_SOURCE_CAMERA,
};

// XLR Adapter HDMI Output Source
enum{
	XH2_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR   = SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR,
	XH2_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA= SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA,
};

// Directional Mic
enum{
	XH2_DIRECTIONAL_MIC_AUTO               = SDK_DIRECTIONAL_MIC_AUTO,
	XH2_DIRECTIONAL_MIC_SURROUND           = SDK_DIRECTIONAL_MIC_SURROUND,
	XH2_DIRECTIONAL_MIC_FRONT              = SDK_DIRECTIONAL_MIC_FRONT,
	XH2_DIRECTIONAL_MIC_TRACKING           = SDK_DIRECTIONAL_MIC_TRACKING,
	XH2_DIRECTIONAL_MIC_BACK               = SDK_DIRECTIONAL_MIC_BACK,
	XH2_DIRECTIONAL_MIC_FRONT_BACK         = SDK_DIRECTIONAL_MIC_FRONT_BACK,
};

// Body Temperature Warning
enum{
	XH2_BODY_TEMPERATURE_WARNING_NONE      = SDK_BODY_TEMPERATURE_WARNING_NONE,
	XH2_BODY_TEMPERATURE_WARNING_YELLOW    = SDK_BODY_TEMPERATURE_WARNING_YELLOW,
	XH2_BODY_TEMPERATURE_WARNING_RED       = SDK_BODY_TEMPERATURE_WARNING_RED,
};

// Movie Rec Volume
enum{
	XH2_MOVIE_REC_VOLUME_OFF               = SDK_MOVIE_REC_VOLUME_OFF,
	XH2_MOVIE_REC_VOLUME_1                 = SDK_MOVIE_REC_VOLUME_1,
	XH2_MOVIE_REC_VOLUME_2                 = SDK_MOVIE_REC_VOLUME_2,
	XH2_MOVIE_REC_VOLUME_3                 = SDK_MOVIE_REC_VOLUME_3,
};
