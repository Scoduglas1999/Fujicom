///////////////////////////////////////////////////
//  GFX100SII_MOV.h
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
typedef  SDK_MOVIE_AFC_CUSTOM     GFX100SII_SDK_MOVIE_AFC_CUSTOM, *PGFX100SII_SDK_MOVIE_AFC_CUSTOM;

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
typedef  SDK_MICLEVEL_INDICATOR     GFX100SII_SDK_MICLEVEL_INDICATOR, *PGFX100SII_SDK_MICLEVEL_INDICATOR;

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
typedef  SDK_PlayBackObjectInformation     GFX100SII_SDK_PlayBackObjectInformation, *PGFX100SII_SDK_PlayBackObjectInformation;

// Focus Map Data
//typedef	struct _SDK_FocusMapData {
//	long lDistance;
//  long lColorR;
//	long lColorG;
//	long lColorB;
//	long lAlpha;
//} SDK_FocusMapData;
typedef  SDK_FocusMapData     GFX100SII_SDK_FocusMapData, *PGFX100SII_SDK_FocusMapData;

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
typedef  SDK_TrackingAfFrameInfo     GFX100SII_SDK_TrackingAfFrameInfo, *PGFX100SII_SDK_TrackingAfFrameInfo;

// Histogram Data
//typedef struct SDK_HistogramData {
//	long lLuminance;
//	long lColorR;
//	long lColorG;
//	long lColorB;
//} SDK_HistogramData;
typedef  SDK_HistogramData     GFX100SII_SDK_HistogramData, *PGFX100SII_SDK_HistogramData;


//
// API_PARAM for GFX100SII
//
enum {
	GFX100SII_API_PARAM_CapMovieHighFrequencyFlickerlessMode = 2,
	GFX100SII_API_PARAM_SetMovieHighFrequencyFlickerlessMode = 1,
	GFX100SII_API_PARAM_GetMovieHighFrequencyFlickerlessMode = 1,
	GFX100SII_API_PARAM_CapMovieAFMode                       = 2,
	GFX100SII_API_PARAM_SetMovieAFMode                       = 1,
	GFX100SII_API_PARAM_GetMovieAFMode                       = 1,
	GFX100SII_API_PARAM_CapMovieFocusArea                    = 3,
	GFX100SII_API_PARAM_SetMovieFocusArea                    = 2,
	GFX100SII_API_PARAM_GetMovieFocusArea                    = 2,
	GFX100SII_API_PARAM_GetTrackingAfFrameInfo               = 1,
	GFX100SII_API_PARAM_CapMovieResolution                   = 2,
	GFX100SII_API_PARAM_SetMovieResolution                   = 1,
	GFX100SII_API_PARAM_GetMovieResolution                   = 1,
	GFX100SII_API_PARAM_CapMovieFrameRate                    = 2,
	GFX100SII_API_PARAM_SetMovieFrameRate                    = 1,
	GFX100SII_API_PARAM_GetMovieFrameRate                    = 1,
	GFX100SII_API_PARAM_CapMovieBitRate                      = 2,
	GFX100SII_API_PARAM_SetMovieBitRate                      = 1,
	GFX100SII_API_PARAM_GetMovieBitRate                      = 1,
	GFX100SII_API_PARAM_CapMovieFileFormat                   = 2,
	GFX100SII_API_PARAM_SetMovieFileFormat                   = 1,
	GFX100SII_API_PARAM_GetMovieFileFormat                   = 1,
	GFX100SII_API_PARAM_CapHighSpeedRecMode                  = 2,
	GFX100SII_API_PARAM_SetHighSpeedRecMode                  = 1,
	GFX100SII_API_PARAM_GetHighSpeedRecMode                  = 1,
	GFX100SII_API_PARAM_CapHighSpeedRecResolution            = 2,
	GFX100SII_API_PARAM_SetHighSpeedRecResolution            = 1,
	GFX100SII_API_PARAM_GetHighSpeedRecResolution            = 1,
	GFX100SII_API_PARAM_CapHighSpeedRecFrameRate             = 2,
	GFX100SII_API_PARAM_SetHighSpeedRecFrameRate             = 1,
	GFX100SII_API_PARAM_GetHighSpeedRecFrameRate             = 1,
	GFX100SII_API_PARAM_CapHighSpeedRecPlayBackFrameRate     = 2,
	GFX100SII_API_PARAM_SetHighSpeedRecPlayBackFrameRate     = 1,
	GFX100SII_API_PARAM_GetHighSpeedRecPlayBackFrameRate     = 1,
	GFX100SII_API_PARAM_CapMovieMediaRecord                  = 2,
	GFX100SII_API_PARAM_SetMovieMediaRecord                  = 1,
	GFX100SII_API_PARAM_GetMovieMediaRecord                  = 1,
	GFX100SII_API_PARAM_CapMovieMediaRecordProRes            = 2,
	GFX100SII_API_PARAM_SetMovieMediaRecordProRes            = 1,
	GFX100SII_API_PARAM_GetMovieMediaRecordProRes            = 1,
	GFX100SII_API_PARAM_CapMovieHDMIOutputRAW                = 2,
	GFX100SII_API_PARAM_SetMovieHDMIOutputRAW                = 1,
	GFX100SII_API_PARAM_GetMovieHDMIOutputRAW                = 1,
	GFX100SII_API_PARAM_CapMovieHDMIOutputRAWResolution      = 2,
	GFX100SII_API_PARAM_SetMovieHDMIOutputRAWResolution      = 1,
	GFX100SII_API_PARAM_GetMovieHDMIOutputRAWResolution      = 1,
	GFX100SII_API_PARAM_CapMovieHDMIOutputRAWFrameRate       = 2,
	GFX100SII_API_PARAM_SetMovieHDMIOutputRAWFrameRate       = 1,
	GFX100SII_API_PARAM_GetMovieHDMIOutputRAWFrameRate       = 1,
	GFX100SII_API_PARAM_CapFlogRecording                     = 2,
	GFX100SII_API_PARAM_SetFlogRecording                     = 1,
	GFX100SII_API_PARAM_GetFlogRecording                     = 1,
	GFX100SII_API_PARAM_CapMovieCropMagnification            = 2,
	GFX100SII_API_PARAM_SetMovieCropMagnification            = 1,
	GFX100SII_API_PARAM_GetMovieCropMagnification            = 1,
	GFX100SII_API_PARAM_CapMovieHDMIOutputInfoDisplay        = 2,
	GFX100SII_API_PARAM_SetMovieHDMIOutputInfoDisplay        = 1,
	GFX100SII_API_PARAM_GetMovieHDMIOutputInfoDisplay        = 1,
	GFX100SII_API_PARAM_CapMovieHDMIRecControl               = 2,
	GFX100SII_API_PARAM_SetMovieHDMIRecControl               = 1,
	GFX100SII_API_PARAM_GetMovieHDMIRecControl               = 1,
	GFX100SII_API_PARAM_CapMovieIsMode                       = 2,
	GFX100SII_API_PARAM_SetMovieIsMode                       = 1,
	GFX100SII_API_PARAM_GetMovieIsMode                       = 1,
	GFX100SII_API_PARAM_CapMovieIsModeBoost                  = 2,
	GFX100SII_API_PARAM_SetMovieIsModeBoost                  = 1,
	GFX100SII_API_PARAM_GetMovieIsModeBoost                  = 1,
	GFX100SII_API_PARAM_CapMovieZebraSetting                 = 2,
	GFX100SII_API_PARAM_SetMovieZebraSetting                 = 1,
	GFX100SII_API_PARAM_GetMovieZebraSetting                 = 1,
	GFX100SII_API_PARAM_CapMovieZebraLevel                   = 2,
	GFX100SII_API_PARAM_SetMovieZebraLevel                   = 1,
	GFX100SII_API_PARAM_GetMovieZebraLevel                   = 1,
	GFX100SII_API_PARAM_CapMovieOptimizedControl             = 2,
	GFX100SII_API_PARAM_SetMovieOptimizedControl             = 1,
	GFX100SII_API_PARAM_GetMovieOptimizedControl             = 1,
	GFX100SII_API_PARAM_CapMovieTallyLight                   = 2,
	GFX100SII_API_PARAM_SetMovieTallyLight                   = 1,
	GFX100SII_API_PARAM_GetMovieTallyLight                   = 1,
	GFX100SII_API_PARAM_GetMediaEjectWarning                 = 1,
	GFX100SII_API_PARAM_CapMovieDataLevelSetting             = 2,
	GFX100SII_API_PARAM_SetMovieDataLevelSetting             = 1,
	GFX100SII_API_PARAM_GetMovieDataLevelSetting             = 1,
	GFX100SII_API_PARAM_GetMovieCropMagnificationValue       = 1,
	GFX100SII_API_PARAM_CapMovieFullTimeManual               = 2,
	GFX100SII_API_PARAM_SetMovieFullTimeManual               = 1,
	GFX100SII_API_PARAM_GetMovieFullTimeManual               = 1,
	GFX100SII_API_PARAM_CapMovieDigitalZoom                  = 2,
	GFX100SII_API_PARAM_SetMovieDigitalZoom                  = 1,
	GFX100SII_API_PARAM_GetMovieDigitalZoom                  = 1,
	GFX100SII_API_PARAM_GetMovieDigitalZoomRange             = 3,
	GFX100SII_API_PARAM_CapMovieImageFormat                  = 2,
	GFX100SII_API_PARAM_SetMovieImageFormat                  = 1,
	GFX100SII_API_PARAM_GetMovieImageFormat                  = 1,
	GFX100SII_API_PARAM_GetHistogramData                     = 2,
	GFX100SII_API_PARAM_GetFocusMapData                      = 2,
	GFX100SII_API_PARAM_CapWaveFormVectorScope               = 2,
	GFX100SII_API_PARAM_SetWaveFormVectorScope               = 1,
	GFX100SII_API_PARAM_GetWaveFormVectorScope               = 1,
	GFX100SII_API_PARAM_GetWaveFormData                      = 2,
	GFX100SII_API_PARAM_GetVectorScopeData                   = 2,
	GFX100SII_API_PARAM_GetParadeData                        = 2,
	GFX100SII_API_PARAM_CapRecFrameIndicator                 = 2,
	GFX100SII_API_PARAM_SetRecFrameIndicator                 = 1,
	GFX100SII_API_PARAM_GetRecFrameIndicator                 = 1,
	GFX100SII_API_PARAM_CapMovieCaptureDelay                 = 2,
	GFX100SII_API_PARAM_SetMovieCaptureDelay                 = 1,
	GFX100SII_API_PARAM_GetMovieCaptureDelay                 = 1,
	GFX100SII_API_PARAM_CapAnamorphicDesqueezeDisplay        = 2,
	GFX100SII_API_PARAM_SetAnamorphicDesqueezeDisplay        = 1,
	GFX100SII_API_PARAM_GetAnamorphicDesqueezeDisplay        = 1,
	GFX100SII_API_PARAM_CapAnamorphicMagnification           = 2,
	GFX100SII_API_PARAM_SetAnamorphicMagnification           = 1,
	GFX100SII_API_PARAM_GetAnamorphicMagnification           = 1,
	GFX100SII_API_PARAM_CapWaveFormSetting                   = 2,
	GFX100SII_API_PARAM_SetWaveFormSetting                   = 1,
	GFX100SII_API_PARAM_GetWaveFormSetting                   = 1,
	GFX100SII_API_PARAM_CapVectorScopeSetting                = 2,
	GFX100SII_API_PARAM_SetVectorScopeSetting                = 1,
	GFX100SII_API_PARAM_GetVectorScopeSetting                = 1,
	GFX100SII_API_PARAM_CapParadeSettingDisplay              = 2,
	GFX100SII_API_PARAM_SetParadeSettingDisplay              = 1,
	GFX100SII_API_PARAM_GetParadeSettingDisplay              = 1,
	GFX100SII_API_PARAM_CapParadeSettingColor                = 2,
	GFX100SII_API_PARAM_SetParadeSettingColor                = 1,
	GFX100SII_API_PARAM_GetParadeSettingColor                = 1,
	GFX100SII_API_PARAM_CapFlogDRangePriority                = 2,
	GFX100SII_API_PARAM_SetFlogDRangePriority                = 1,
	GFX100SII_API_PARAM_GetFlogDRangePriority                = 1,
	GFX100SII_API_PARAM_CapMovieWhiteBalanceTune             = 5,
	GFX100SII_API_PARAM_SetMovieWhiteBalanceTune             = 3,
	GFX100SII_API_PARAM_GetMovieWhiteBalanceTune             = 3,
	GFX100SII_API_PARAM_CapMovieFilmSimulationMode           = 2,
	GFX100SII_API_PARAM_SetMovieFilmSimulationMode           = 1,
	GFX100SII_API_PARAM_GetMovieFilmSimulationMode           = 1,
	GFX100SII_API_PARAM_CapMovieMonochromaticColor           = 4,
	GFX100SII_API_PARAM_SetMovieMonochromaticColor           = 2,
	GFX100SII_API_PARAM_GetMovieMonochromaticColor           = 2,
	GFX100SII_API_PARAM_CapMovieHighLightTone                = 2,
	GFX100SII_API_PARAM_SetMovieHighLightTone                = 1,
	GFX100SII_API_PARAM_GetMovieHighLightTone                = 1,
	GFX100SII_API_PARAM_CapMovieShadowTone                   = 2,
	GFX100SII_API_PARAM_SetMovieShadowTone                   = 1,
	GFX100SII_API_PARAM_GetMovieShadowTone                   = 1,
	GFX100SII_API_PARAM_CapMovieSharpness                    = 2,
	GFX100SII_API_PARAM_SetMovieSharpness                    = 1,
	GFX100SII_API_PARAM_GetMovieSharpness                    = 1,
	GFX100SII_API_PARAM_CapMovieColorMode                    = 2,
	GFX100SII_API_PARAM_SetMovieColorMode                    = 1,
	GFX100SII_API_PARAM_GetMovieColorMode                    = 1,
	GFX100SII_API_PARAM_CapMovieNoiseReduction               = 2,
	GFX100SII_API_PARAM_SetMovieNoiseReduction               = 1,
	GFX100SII_API_PARAM_GetMovieNoiseReduction               = 1,
	GFX100SII_API_PARAM_CapInterFrameNR                      = 2,
	GFX100SII_API_PARAM_SetInterFrameNR                      = 1,
	GFX100SII_API_PARAM_GetInterFrameNR                      = 1,
	GFX100SII_API_PARAM_CapMoviePeripheralLightCorrection    = 2,
	GFX100SII_API_PARAM_SetMoviePeripheralLightCorrection    = 1,
	GFX100SII_API_PARAM_GetMoviePeripheralLightCorrection    = 1,
	GFX100SII_API_PARAM_CapMovieFaceDetectionMode            = 2,
	GFX100SII_API_PARAM_SetMovieFaceDetectionMode            = 1,
	GFX100SII_API_PARAM_GetMovieFaceDetectionMode            = 1,
	GFX100SII_API_PARAM_CapMovieEyeAFMode                    = 2,
	GFX100SII_API_PARAM_SetMovieEyeAFMode                    = 1,
	GFX100SII_API_PARAM_GetMovieEyeAFMode                    = 1,
	GFX100SII_API_PARAM_CapMovieSubjectDetectionMode         = 2,
	GFX100SII_API_PARAM_SetMovieSubjectDetectionMode         = 1,
	GFX100SII_API_PARAM_GetMovieSubjectDetectionMode         = 1,
	GFX100SII_API_PARAM_CapMovieAFCCustom                    = 2,
	GFX100SII_API_PARAM_SetMovieAFCCustom                    = 1,
	GFX100SII_API_PARAM_GetMovieAFCCustom                    = 1,
	GFX100SII_API_PARAM_CapMovieMFAssistMode                 = 2,
	GFX100SII_API_PARAM_SetMovieMFAssistMode                 = 1,
	GFX100SII_API_PARAM_GetMovieMFAssistMode                 = 1,
	GFX100SII_API_PARAM_GetMovieFocusMeter                   = 3,
	GFX100SII_API_PARAM_CapMovieFocusCheckMode               = 2,
	GFX100SII_API_PARAM_SetMovieFocusCheckMode               = 1,
	GFX100SII_API_PARAM_GetMovieFocusCheckMode               = 1,
	GFX100SII_API_PARAM_CapMovieFocusCheckLock               = 2,
	GFX100SII_API_PARAM_SetMovieFocusCheckLock               = 1,
	GFX100SII_API_PARAM_GetMovieFocusCheckLock               = 1,
	GFX100SII_API_PARAM_GetMovieRecordingTime                = 3,
	GFX100SII_API_PARAM_GetMovieRemainingTime                = 3,
	GFX100SII_API_PARAM_GetTimeCode                          = 4,
	GFX100SII_API_PARAM_CapTimeCodeDisplay                   = 2,
	GFX100SII_API_PARAM_SetTimeCodeDisplay                   = 1,
	GFX100SII_API_PARAM_GetTimeCodeDisplay                   = 1,
	GFX100SII_API_PARAM_CapTimeCodeStartSetting              = 2,
	GFX100SII_API_PARAM_SetTimeCodeStartSetting              = 5,
	GFX100SII_API_PARAM_CapTimeCodeCountUp                   = 2,
	GFX100SII_API_PARAM_SetTimeCodeCountUp                   = 1,
	GFX100SII_API_PARAM_GetTimeCodeCountUp                   = 1,
	GFX100SII_API_PARAM_CapTimeCodeDropFrame                 = 2,
	GFX100SII_API_PARAM_SetTimeCodeDropFrame                 = 1,
	GFX100SII_API_PARAM_GetTimeCodeDropFrame                 = 1,
	GFX100SII_API_PARAM_CapTimeCodeHDMIOutput                = 2,
	GFX100SII_API_PARAM_SetTimeCodeHDMIOutput                = 1,
	GFX100SII_API_PARAM_GetTimeCodeHDMIOutput                = 1,
	GFX100SII_API_PARAM_GetTimeCodeCurrentValue              = 4,
	GFX100SII_API_PARAM_CapATOMOSAirGluConnection            = 2,
	GFX100SII_API_PARAM_SetATOMOSAirGluConnection            = 1,
	GFX100SII_API_PARAM_GetATOMOSAirGluConnection            = 1,
	GFX100SII_API_PARAM_GetTimeCodeStatus                    = 1,
	GFX100SII_API_PARAM_CapInternalMicLevel                  = 2,
	GFX100SII_API_PARAM_SetInternalMicLevel                  = 1,
	GFX100SII_API_PARAM_GetInternalMicLevel                  = 1,
	GFX100SII_API_PARAM_CapInternalMicLevelManual            = 2,
	GFX100SII_API_PARAM_SetInternalMicLevelManual            = 1,
	GFX100SII_API_PARAM_GetInternalMicLevelManual            = 1,
	GFX100SII_API_PARAM_CapExternalMicLevel                  = 2,
	GFX100SII_API_PARAM_SetExternalMicLevel                  = 1,
	GFX100SII_API_PARAM_GetExternalMicLevel                  = 1,
	GFX100SII_API_PARAM_CapExternalMicLevelManual            = 2,
	GFX100SII_API_PARAM_SetExternalMicLevelManual            = 1,
	GFX100SII_API_PARAM_GetExternalMicLevelManual            = 1,
	GFX100SII_API_PARAM_CapMicLevelLimiter                   = 2,
	GFX100SII_API_PARAM_SetMicLevelLimiter                   = 1,
	GFX100SII_API_PARAM_GetMicLevelLimiter                   = 1,
	GFX100SII_API_PARAM_CapWindFilter                        = 2,
	GFX100SII_API_PARAM_SetWindFilter                        = 1,
	GFX100SII_API_PARAM_GetWindFilter                        = 1,
	GFX100SII_API_PARAM_CapLowCutFilter                      = 2,
	GFX100SII_API_PARAM_SetLowCutFilter                      = 1,
	GFX100SII_API_PARAM_GetLowCutFilter                      = 1,
	GFX100SII_API_PARAM_CapHeadPhonesVolume                  = 2,
	GFX100SII_API_PARAM_SetHeadPhonesVolume                  = 1,
	GFX100SII_API_PARAM_GetHeadPhonesVolume                  = 1,
	GFX100SII_API_PARAM_CapXLRAdapterMicSource               = 2,
	GFX100SII_API_PARAM_SetXLRAdapterMicSource               = 1,
	GFX100SII_API_PARAM_GetXLRAdapterMicSource               = 1,
	GFX100SII_API_PARAM_CapXLRAdapterMoniteringSource        = 2,
	GFX100SII_API_PARAM_SetXLRAdapterMoniteringSource        = 1,
	GFX100SII_API_PARAM_GetXLRAdapterMoniteringSource        = 1,
	GFX100SII_API_PARAM_CapXLRAdapterHDMIOutputSource        = 2,
	GFX100SII_API_PARAM_SetXLRAdapterHDMIOutputSource        = 1,
	GFX100SII_API_PARAM_GetXLRAdapterHDMIOutputSource        = 1,
	GFX100SII_API_PARAM_GetMicLevelIndicator                 = 1,
	GFX100SII_API_PARAM_GetMovieTransparentFrameInfo         = -1,
	GFX100SII_API_PARAM_CapShortMovieSecond                  = -1,
	GFX100SII_API_PARAM_SetShortMovieSecond                  = -1,
	GFX100SII_API_PARAM_GetShortMovieSecond                  = -1,
	GFX100SII_API_PARAM_CapMoviePortraitEnhancer             = -1,
	GFX100SII_API_PARAM_SetMoviePortraitEnhancer             = -1,
	GFX100SII_API_PARAM_GetMoviePortraitEnhancer             = -1,
	GFX100SII_API_PARAM_CapDirectionalMic                    = 2,
	GFX100SII_API_PARAM_SetDirectionalMic                    = 1,
	GFX100SII_API_PARAM_GetDirectionalMic                    = 1,
	GFX100SII_API_PARAM_CapEnvSoundsNR                       = -1,
	GFX100SII_API_PARAM_SetEnvSoundsNR                       = -1,
	GFX100SII_API_PARAM_GetEnvSoundsNR                       = -1,
	GFX100SII_API_PARAM_GetBodyTemperatureWarning            = 1,
	GFX100SII_API_PARAM_GetPlayBackMediaInfo                 = -1,
	GFX100SII_API_PARAM_GetPlayBackObjectInfo                = -1,
	GFX100SII_API_PARAM_GetPlayBackThumb                     = -1,
	GFX100SII_API_PARAM_GetPlayBackPartialObject             = -1,
	GFX100SII_API_PARAM_CapMovieRecVolume                    = 2,
	GFX100SII_API_PARAM_SetMovieRecVolume                    = 1,
	GFX100SII_API_PARAM_GetMovieRecVolume                    = 1,
};

//
// API_CODE for GFX100SII
//
enum {
	GFX100SII_API_CODE_CapMovieHighFrequencyFlickerlessMode   = API_CODE_CapMovieHighFrequencyFlickerlessMode,
	GFX100SII_API_CODE_SetMovieHighFrequencyFlickerlessMode   = API_CODE_SetMovieHighFrequencyFlickerlessMode,
	GFX100SII_API_CODE_GetMovieHighFrequencyFlickerlessMode   = API_CODE_GetMovieHighFrequencyFlickerlessMode,
	GFX100SII_API_CODE_CapMovieAFMode                         = API_CODE_CapMovieAFMode,
	GFX100SII_API_CODE_SetMovieAFMode                         = API_CODE_SetMovieAFMode,
	GFX100SII_API_CODE_GetMovieAFMode                         = API_CODE_GetMovieAFMode,
	GFX100SII_API_CODE_CapMovieFocusArea                      = API_CODE_CapMovieFocusArea,
	GFX100SII_API_CODE_SetMovieFocusArea                      = API_CODE_SetMovieFocusArea,
	GFX100SII_API_CODE_GetMovieFocusArea                      = API_CODE_GetMovieFocusArea,
	GFX100SII_API_CODE_GetTrackingAfFrameInfo                 = API_CODE_GetTrackingAfFrameInfo,
	GFX100SII_API_CODE_CapMovieResolution                     = API_CODE_CapMovieResolution,
	GFX100SII_API_CODE_SetMovieResolution                     = API_CODE_SetMovieResolution,
	GFX100SII_API_CODE_GetMovieResolution                     = API_CODE_GetMovieResolution,
	GFX100SII_API_CODE_CapMovieFrameRate                      = API_CODE_CapMovieFrameRate,
	GFX100SII_API_CODE_SetMovieFrameRate                      = API_CODE_SetMovieFrameRate,
	GFX100SII_API_CODE_GetMovieFrameRate                      = API_CODE_GetMovieFrameRate,
	GFX100SII_API_CODE_CapMovieBitRate                        = API_CODE_CapMovieBitRate,
	GFX100SII_API_CODE_SetMovieBitRate                        = API_CODE_SetMovieBitRate,
	GFX100SII_API_CODE_GetMovieBitRate                        = API_CODE_GetMovieBitRate,
	GFX100SII_API_CODE_CapMovieFileFormat                     = API_CODE_CapMovieFileFormat,
	GFX100SII_API_CODE_SetMovieFileFormat                     = API_CODE_SetMovieFileFormat,
	GFX100SII_API_CODE_GetMovieFileFormat                     = API_CODE_GetMovieFileFormat,
	GFX100SII_API_CODE_CapHighSpeedRecMode                    = API_CODE_CapHighSpeedRecMode,
	GFX100SII_API_CODE_SetHighSpeedRecMode                    = API_CODE_SetHighSpeedRecMode,
	GFX100SII_API_CODE_GetHighSpeedRecMode                    = API_CODE_GetHighSpeedRecMode,
	GFX100SII_API_CODE_CapHighSpeedRecResolution              = API_CODE_CapHighSpeedRecResolution,
	GFX100SII_API_CODE_SetHighSpeedRecResolution              = API_CODE_SetHighSpeedRecResolution,
	GFX100SII_API_CODE_GetHighSpeedRecResolution              = API_CODE_GetHighSpeedRecResolution,
	GFX100SII_API_CODE_CapHighSpeedRecFrameRate               = API_CODE_CapHighSpeedRecFrameRate,
	GFX100SII_API_CODE_SetHighSpeedRecFrameRate               = API_CODE_SetHighSpeedRecFrameRate,
	GFX100SII_API_CODE_GetHighSpeedRecFrameRate               = API_CODE_GetHighSpeedRecFrameRate,
	GFX100SII_API_CODE_CapHighSpeedRecPlayBackFrameRate       = API_CODE_CapHighSpeedRecPlayBackFrameRate,
	GFX100SII_API_CODE_SetHighSpeedRecPlayBackFrameRate       = API_CODE_SetHighSpeedRecPlayBackFrameRate,
	GFX100SII_API_CODE_GetHighSpeedRecPlayBackFrameRate       = API_CODE_GetHighSpeedRecPlayBackFrameRate,
	GFX100SII_API_CODE_CapMovieMediaRecord                    = API_CODE_CapMovieMediaRecord,
	GFX100SII_API_CODE_SetMovieMediaRecord                    = API_CODE_SetMovieMediaRecord,
	GFX100SII_API_CODE_GetMovieMediaRecord                    = API_CODE_GetMovieMediaRecord,
	GFX100SII_API_CODE_CapMovieMediaRecordProRes              = API_CODE_CapMovieMediaRecordProRes,
	GFX100SII_API_CODE_SetMovieMediaRecordProRes              = API_CODE_SetMovieMediaRecordProRes,
	GFX100SII_API_CODE_GetMovieMediaRecordProRes              = API_CODE_GetMovieMediaRecordProRes,
	GFX100SII_API_CODE_CapMovieHDMIOutputRAW                  = API_CODE_CapMovieHDMIOutputRAW,
	GFX100SII_API_CODE_SetMovieHDMIOutputRAW                  = API_CODE_SetMovieHDMIOutputRAW,
	GFX100SII_API_CODE_GetMovieHDMIOutputRAW                  = API_CODE_GetMovieHDMIOutputRAW,
	GFX100SII_API_CODE_CapMovieHDMIOutputRAWResolution        = API_CODE_CapMovieHDMIOutputRAWResolution,
	GFX100SII_API_CODE_SetMovieHDMIOutputRAWResolution        = API_CODE_SetMovieHDMIOutputRAWResolution,
	GFX100SII_API_CODE_GetMovieHDMIOutputRAWResolution        = API_CODE_GetMovieHDMIOutputRAWResolution,
	GFX100SII_API_CODE_CapMovieHDMIOutputRAWFrameRate         = API_CODE_CapMovieHDMIOutputRAWFrameRate,
	GFX100SII_API_CODE_SetMovieHDMIOutputRAWFrameRate         = API_CODE_SetMovieHDMIOutputRAWFrameRate,
	GFX100SII_API_CODE_GetMovieHDMIOutputRAWFrameRate         = API_CODE_GetMovieHDMIOutputRAWFrameRate,
	GFX100SII_API_CODE_CapFlogRecording                       = API_CODE_CapFlogRecording,
	GFX100SII_API_CODE_SetFlogRecording                       = API_CODE_SetFlogRecording,
	GFX100SII_API_CODE_GetFlogRecording                       = API_CODE_GetFlogRecording,
	GFX100SII_API_CODE_CapMovieCropMagnification              = API_CODE_CapMovieCropMagnification,
	GFX100SII_API_CODE_SetMovieCropMagnification              = API_CODE_SetMovieCropMagnification,
	GFX100SII_API_CODE_GetMovieCropMagnification              = API_CODE_GetMovieCropMagnification,
	GFX100SII_API_CODE_CapMovieHDMIOutputInfoDisplay          = API_CODE_CapMovieHDMIOutputInfoDisplay,
	GFX100SII_API_CODE_SetMovieHDMIOutputInfoDisplay          = API_CODE_SetMovieHDMIOutputInfoDisplay,
	GFX100SII_API_CODE_GetMovieHDMIOutputInfoDisplay          = API_CODE_GetMovieHDMIOutputInfoDisplay,
	GFX100SII_API_CODE_CapMovieHDMIRecControl                 = API_CODE_CapMovieHDMIRecControl,
	GFX100SII_API_CODE_SetMovieHDMIRecControl                 = API_CODE_SetMovieHDMIRecControl,
	GFX100SII_API_CODE_GetMovieHDMIRecControl                 = API_CODE_GetMovieHDMIRecControl,
	GFX100SII_API_CODE_CapMovieIsMode                         = API_CODE_CapMovieIsMode,
	GFX100SII_API_CODE_SetMovieIsMode                         = API_CODE_SetMovieIsMode,
	GFX100SII_API_CODE_GetMovieIsMode                         = API_CODE_GetMovieIsMode,
	GFX100SII_API_CODE_CapMovieIsModeBoost                    = API_CODE_CapMovieIsModeBoost,
	GFX100SII_API_CODE_SetMovieIsModeBoost                    = API_CODE_SetMovieIsModeBoost,
	GFX100SII_API_CODE_GetMovieIsModeBoost                    = API_CODE_GetMovieIsModeBoost,
	GFX100SII_API_CODE_CapMovieZebraSetting                   = API_CODE_CapMovieZebraSetting,
	GFX100SII_API_CODE_SetMovieZebraSetting                   = API_CODE_SetMovieZebraSetting,
	GFX100SII_API_CODE_GetMovieZebraSetting                   = API_CODE_GetMovieZebraSetting,
	GFX100SII_API_CODE_CapMovieZebraLevel                     = API_CODE_CapMovieZebraLevel,
	GFX100SII_API_CODE_SetMovieZebraLevel                     = API_CODE_SetMovieZebraLevel,
	GFX100SII_API_CODE_GetMovieZebraLevel                     = API_CODE_GetMovieZebraLevel,
	GFX100SII_API_CODE_CapMovieOptimizedControl               = API_CODE_CapMovieOptimizedControl,
	GFX100SII_API_CODE_SetMovieOptimizedControl               = API_CODE_SetMovieOptimizedControl,
	GFX100SII_API_CODE_GetMovieOptimizedControl               = API_CODE_GetMovieOptimizedControl,
	GFX100SII_API_CODE_CapMovieTallyLight                     = API_CODE_CapMovieTallyLight,
	GFX100SII_API_CODE_SetMovieTallyLight                     = API_CODE_SetMovieTallyLight,
	GFX100SII_API_CODE_GetMovieTallyLight                     = API_CODE_GetMovieTallyLight,
	GFX100SII_API_CODE_GetMediaEjectWarning                   = API_CODE_GetMediaEjectWarning,
	GFX100SII_API_CODE_CapMovieDataLevelSetting               = API_CODE_CapMovieDataLevelSetting,
	GFX100SII_API_CODE_SetMovieDataLevelSetting               = API_CODE_SetMovieDataLevelSetting,
	GFX100SII_API_CODE_GetMovieDataLevelSetting               = API_CODE_GetMovieDataLevelSetting,
	GFX100SII_API_CODE_GetMovieCropMagnificationValue         = API_CODE_GetMovieCropMagnificationValue,
	GFX100SII_API_CODE_CapMovieFullTimeManual                 = API_CODE_CapMovieFullTimeManual,
	GFX100SII_API_CODE_SetMovieFullTimeManual                 = API_CODE_SetMovieFullTimeManual,
	GFX100SII_API_CODE_GetMovieFullTimeManual                 = API_CODE_GetMovieFullTimeManual,
	GFX100SII_API_CODE_CapMovieDigitalZoom                    = API_CODE_CapMovieDigitalZoom,
	GFX100SII_API_CODE_SetMovieDigitalZoom                    = API_CODE_SetMovieDigitalZoom,
	GFX100SII_API_CODE_GetMovieDigitalZoom                    = API_CODE_GetMovieDigitalZoom,
	GFX100SII_API_CODE_GetMovieDigitalZoomRange               = API_CODE_GetMovieDigitalZoomRange,
	GFX100SII_API_CODE_CapMovieImageFormat                    = API_CODE_CapMovieImageFormat,
	GFX100SII_API_CODE_SetMovieImageFormat                    = API_CODE_SetMovieImageFormat,
	GFX100SII_API_CODE_GetMovieImageFormat                    = API_CODE_GetMovieImageFormat,
	GFX100SII_API_CODE_GetHistogramData                       = API_CODE_GetHistogramData,
	GFX100SII_API_CODE_GetFocusMapData                        = API_CODE_GetFocusMapData,
	GFX100SII_API_CODE_CapWaveFormVectorScope                 = API_CODE_CapWaveFormVectorScope,
	GFX100SII_API_CODE_SetWaveFormVectorScope                 = API_CODE_SetWaveFormVectorScope,
	GFX100SII_API_CODE_GetWaveFormVectorScope                 = API_CODE_GetWaveFormVectorScope,
	GFX100SII_API_CODE_GetWaveFormData                        = API_CODE_GetWaveFormData,
	GFX100SII_API_CODE_GetVectorScopeData                     = API_CODE_GetVectorScopeData,
	GFX100SII_API_CODE_GetParadeData                          = API_CODE_GetParadeData,
	GFX100SII_API_CODE_CapRecFrameIndicator                   = API_CODE_CapRecFrameIndicator,
	GFX100SII_API_CODE_SetRecFrameIndicator                   = API_CODE_SetRecFrameIndicator,
	GFX100SII_API_CODE_GetRecFrameIndicator                   = API_CODE_GetRecFrameIndicator,
	GFX100SII_API_CODE_CapMovieCaptureDelay                   = API_CODE_CapMovieCaptureDelay,
	GFX100SII_API_CODE_SetMovieCaptureDelay                   = API_CODE_SetMovieCaptureDelay,
	GFX100SII_API_CODE_GetMovieCaptureDelay                   = API_CODE_GetMovieCaptureDelay,
	GFX100SII_API_CODE_CapAnamorphicDesqueezeDisplay          = API_CODE_CapAnamorphicDesqueezeDisplay,
	GFX100SII_API_CODE_SetAnamorphicDesqueezeDisplay          = API_CODE_SetAnamorphicDesqueezeDisplay,
	GFX100SII_API_CODE_GetAnamorphicDesqueezeDisplay          = API_CODE_GetAnamorphicDesqueezeDisplay,
	GFX100SII_API_CODE_CapAnamorphicMagnification             = API_CODE_CapAnamorphicMagnification,
	GFX100SII_API_CODE_SetAnamorphicMagnification             = API_CODE_SetAnamorphicMagnification,
	GFX100SII_API_CODE_GetAnamorphicMagnification             = API_CODE_GetAnamorphicMagnification,
	GFX100SII_API_CODE_CapWaveFormSetting                     = API_CODE_CapWaveFormSetting,
	GFX100SII_API_CODE_SetWaveFormSetting                     = API_CODE_SetWaveFormSetting,
	GFX100SII_API_CODE_GetWaveFormSetting                     = API_CODE_GetWaveFormSetting,
	GFX100SII_API_CODE_CapVectorScopeSetting                  = API_CODE_CapVectorScopeSetting,
	GFX100SII_API_CODE_SetVectorScopeSetting                  = API_CODE_SetVectorScopeSetting,
	GFX100SII_API_CODE_GetVectorScopeSetting                  = API_CODE_GetVectorScopeSetting,
	GFX100SII_API_CODE_CapParadeSettingDisplay                = API_CODE_CapParadeSettingDisplay,
	GFX100SII_API_CODE_SetParadeSettingDisplay                = API_CODE_SetParadeSettingDisplay,
	GFX100SII_API_CODE_GetParadeSettingDisplay                = API_CODE_GetParadeSettingDisplay,
	GFX100SII_API_CODE_CapParadeSettingColor                  = API_CODE_CapParadeSettingColor,
	GFX100SII_API_CODE_SetParadeSettingColor                  = API_CODE_SetParadeSettingColor,
	GFX100SII_API_CODE_GetParadeSettingColor                  = API_CODE_GetParadeSettingColor,
	GFX100SII_API_CODE_CapFlogDRangePriority                  = API_CODE_CapFlogDRangePriority,
	GFX100SII_API_CODE_SetFlogDRangePriority                  = API_CODE_SetFlogDRangePriority,
	GFX100SII_API_CODE_GetFlogDRangePriority                  = API_CODE_GetFlogDRangePriority,
	GFX100SII_API_CODE_CapMovieWhiteBalanceTune               = API_CODE_CapMovieWhiteBalanceTune,
	GFX100SII_API_CODE_SetMovieWhiteBalanceTune               = API_CODE_SetMovieWhiteBalanceTune,
	GFX100SII_API_CODE_GetMovieWhiteBalanceTune               = API_CODE_GetMovieWhiteBalanceTune,
	GFX100SII_API_CODE_CapMovieFilmSimulationMode             = API_CODE_CapMovieFilmSimulationMode,
	GFX100SII_API_CODE_SetMovieFilmSimulationMode             = API_CODE_SetMovieFilmSimulationMode,
	GFX100SII_API_CODE_GetMovieFilmSimulationMode             = API_CODE_GetMovieFilmSimulationMode,
	GFX100SII_API_CODE_CapMovieMonochromaticColor             = API_CODE_CapMovieMonochromaticColor,
	GFX100SII_API_CODE_SetMovieMonochromaticColor             = API_CODE_SetMovieMonochromaticColor,
	GFX100SII_API_CODE_GetMovieMonochromaticColor             = API_CODE_GetMovieMonochromaticColor,
	GFX100SII_API_CODE_CapMovieHighLightTone                  = API_CODE_CapMovieHighLightTone,
	GFX100SII_API_CODE_SetMovieHighLightTone                  = API_CODE_SetMovieHighLightTone,
	GFX100SII_API_CODE_GetMovieHighLightTone                  = API_CODE_GetMovieHighLightTone,
	GFX100SII_API_CODE_CapMovieShadowTone                     = API_CODE_CapMovieShadowTone,
	GFX100SII_API_CODE_SetMovieShadowTone                     = API_CODE_SetMovieShadowTone,
	GFX100SII_API_CODE_GetMovieShadowTone                     = API_CODE_GetMovieShadowTone,
	GFX100SII_API_CODE_CapMovieSharpness                      = API_CODE_CapMovieSharpness,
	GFX100SII_API_CODE_SetMovieSharpness                      = API_CODE_SetMovieSharpness,
	GFX100SII_API_CODE_GetMovieSharpness                      = API_CODE_GetMovieSharpness,
	GFX100SII_API_CODE_CapMovieColorMode                      = API_CODE_CapMovieColorMode,
	GFX100SII_API_CODE_SetMovieColorMode                      = API_CODE_SetMovieColorMode,
	GFX100SII_API_CODE_GetMovieColorMode                      = API_CODE_GetMovieColorMode,
	GFX100SII_API_CODE_CapMovieNoiseReduction                 = API_CODE_CapMovieNoiseReduction,
	GFX100SII_API_CODE_SetMovieNoiseReduction                 = API_CODE_SetMovieNoiseReduction,
	GFX100SII_API_CODE_GetMovieNoiseReduction                 = API_CODE_GetMovieNoiseReduction,
	GFX100SII_API_CODE_CapInterFrameNR                        = API_CODE_CapInterFrameNR,
	GFX100SII_API_CODE_SetInterFrameNR                        = API_CODE_SetInterFrameNR,
	GFX100SII_API_CODE_GetInterFrameNR                        = API_CODE_GetInterFrameNR,
	GFX100SII_API_CODE_CapMoviePeripheralLightCorrection      = API_CODE_CapMoviePeripheralLightCorrection,
	GFX100SII_API_CODE_SetMoviePeripheralLightCorrection      = API_CODE_SetMoviePeripheralLightCorrection,
	GFX100SII_API_CODE_GetMoviePeripheralLightCorrection      = API_CODE_GetMoviePeripheralLightCorrection,
	GFX100SII_API_CODE_CapMovieFaceDetectionMode              = API_CODE_CapMovieFaceDetectionMode,
	GFX100SII_API_CODE_SetMovieFaceDetectionMode              = API_CODE_SetMovieFaceDetectionMode,
	GFX100SII_API_CODE_GetMovieFaceDetectionMode              = API_CODE_GetMovieFaceDetectionMode,
	GFX100SII_API_CODE_CapMovieEyeAFMode                      = API_CODE_CapMovieEyeAFMode,
	GFX100SII_API_CODE_SetMovieEyeAFMode                      = API_CODE_SetMovieEyeAFMode,
	GFX100SII_API_CODE_GetMovieEyeAFMode                      = API_CODE_GetMovieEyeAFMode,
	GFX100SII_API_CODE_CapMovieSubjectDetectionMode           = API_CODE_CapMovieSubjectDetectionMode,
	GFX100SII_API_CODE_SetMovieSubjectDetectionMode           = API_CODE_SetMovieSubjectDetectionMode,
	GFX100SII_API_CODE_GetMovieSubjectDetectionMode           = API_CODE_GetMovieSubjectDetectionMode,
	GFX100SII_API_CODE_CapMovieAFCCustom                      = API_CODE_CapMovieAFCCustom,
	GFX100SII_API_CODE_SetMovieAFCCustom                      = API_CODE_SetMovieAFCCustom,
	GFX100SII_API_CODE_GetMovieAFCCustom                      = API_CODE_GetMovieAFCCustom,
	GFX100SII_API_CODE_CapMovieMFAssistMode                   = API_CODE_CapMovieMFAssistMode,
	GFX100SII_API_CODE_SetMovieMFAssistMode                   = API_CODE_SetMovieMFAssistMode,
	GFX100SII_API_CODE_GetMovieMFAssistMode                   = API_CODE_GetMovieMFAssistMode,
	GFX100SII_API_CODE_GetMovieFocusMeter                     = API_CODE_GetMovieFocusMeter,
	GFX100SII_API_CODE_CapMovieFocusCheckMode                 = API_CODE_CapMovieFocusCheckMode,
	GFX100SII_API_CODE_SetMovieFocusCheckMode                 = API_CODE_SetMovieFocusCheckMode,
	GFX100SII_API_CODE_GetMovieFocusCheckMode                 = API_CODE_GetMovieFocusCheckMode,
	GFX100SII_API_CODE_CapMovieFocusCheckLock                 = API_CODE_CapMovieFocusCheckLock,
	GFX100SII_API_CODE_SetMovieFocusCheckLock                 = API_CODE_SetMovieFocusCheckLock,
	GFX100SII_API_CODE_GetMovieFocusCheckLock                 = API_CODE_GetMovieFocusCheckLock,
	GFX100SII_API_CODE_GetMovieRecordingTime                  = API_CODE_GetMovieRecordingTime,
	GFX100SII_API_CODE_GetMovieRemainingTime                  = API_CODE_GetMovieRemainingTime,
	GFX100SII_API_CODE_GetTimeCode                            = API_CODE_GetTimeCode,
	GFX100SII_API_CODE_CapTimeCodeDisplay                     = API_CODE_CapTimeCodeDisplay,
	GFX100SII_API_CODE_SetTimeCodeDisplay                     = API_CODE_SetTimeCodeDisplay,
	GFX100SII_API_CODE_GetTimeCodeDisplay                     = API_CODE_GetTimeCodeDisplay,
	GFX100SII_API_CODE_CapTimeCodeStartSetting                = API_CODE_CapTimeCodeStartSetting,
	GFX100SII_API_CODE_SetTimeCodeStartSetting                = API_CODE_SetTimeCodeStartSetting,
	GFX100SII_API_CODE_CapTimeCodeCountUp                     = API_CODE_CapTimeCodeCountUp,
	GFX100SII_API_CODE_SetTimeCodeCountUp                     = API_CODE_SetTimeCodeCountUp,
	GFX100SII_API_CODE_GetTimeCodeCountUp                     = API_CODE_GetTimeCodeCountUp,
	GFX100SII_API_CODE_CapTimeCodeDropFrame                   = API_CODE_CapTimeCodeDropFrame,
	GFX100SII_API_CODE_SetTimeCodeDropFrame                   = API_CODE_SetTimeCodeDropFrame,
	GFX100SII_API_CODE_GetTimeCodeDropFrame                   = API_CODE_GetTimeCodeDropFrame,
	GFX100SII_API_CODE_CapTimeCodeHDMIOutput                  = API_CODE_CapTimeCodeHDMIOutput,
	GFX100SII_API_CODE_SetTimeCodeHDMIOutput                  = API_CODE_SetTimeCodeHDMIOutput,
	GFX100SII_API_CODE_GetTimeCodeHDMIOutput                  = API_CODE_GetTimeCodeHDMIOutput,
	GFX100SII_API_CODE_GetTimeCodeCurrentValue                = API_CODE_GetTimeCodeCurrentValue,
	GFX100SII_API_CODE_CapATOMOSAirGluConnection              = API_CODE_CapATOMOSAirGluConnection,
	GFX100SII_API_CODE_SetATOMOSAirGluConnection              = API_CODE_SetATOMOSAirGluConnection,
	GFX100SII_API_CODE_GetATOMOSAirGluConnection              = API_CODE_GetATOMOSAirGluConnection,
	GFX100SII_API_CODE_GetTimeCodeStatus                      = API_CODE_GetTimeCodeStatus,
	GFX100SII_API_CODE_CapInternalMicLevel                    = API_CODE_CapInternalMicLevel,
	GFX100SII_API_CODE_SetInternalMicLevel                    = API_CODE_SetInternalMicLevel,
	GFX100SII_API_CODE_GetInternalMicLevel                    = API_CODE_GetInternalMicLevel,
	GFX100SII_API_CODE_CapInternalMicLevelManual              = API_CODE_CapInternalMicLevelManual,
	GFX100SII_API_CODE_SetInternalMicLevelManual              = API_CODE_SetInternalMicLevelManual,
	GFX100SII_API_CODE_GetInternalMicLevelManual              = API_CODE_GetInternalMicLevelManual,
	GFX100SII_API_CODE_CapExternalMicLevel                    = API_CODE_CapExternalMicLevel,
	GFX100SII_API_CODE_SetExternalMicLevel                    = API_CODE_SetExternalMicLevel,
	GFX100SII_API_CODE_GetExternalMicLevel                    = API_CODE_GetExternalMicLevel,
	GFX100SII_API_CODE_CapExternalMicLevelManual              = API_CODE_CapExternalMicLevelManual,
	GFX100SII_API_CODE_SetExternalMicLevelManual              = API_CODE_SetExternalMicLevelManual,
	GFX100SII_API_CODE_GetExternalMicLevelManual              = API_CODE_GetExternalMicLevelManual,
	GFX100SII_API_CODE_CapMicLevelLimiter                     = API_CODE_CapMicLevelLimiter,
	GFX100SII_API_CODE_SetMicLevelLimiter                     = API_CODE_SetMicLevelLimiter,
	GFX100SII_API_CODE_GetMicLevelLimiter                     = API_CODE_GetMicLevelLimiter,
	GFX100SII_API_CODE_CapWindFilter                          = API_CODE_CapWindFilter,
	GFX100SII_API_CODE_SetWindFilter                          = API_CODE_SetWindFilter,
	GFX100SII_API_CODE_GetWindFilter                          = API_CODE_GetWindFilter,
	GFX100SII_API_CODE_CapLowCutFilter                        = API_CODE_CapLowCutFilter,
	GFX100SII_API_CODE_SetLowCutFilter                        = API_CODE_SetLowCutFilter,
	GFX100SII_API_CODE_GetLowCutFilter                        = API_CODE_GetLowCutFilter,
	GFX100SII_API_CODE_CapHeadPhonesVolume                    = API_CODE_CapHeadPhonesVolume,
	GFX100SII_API_CODE_SetHeadPhonesVolume                    = API_CODE_SetHeadPhonesVolume,
	GFX100SII_API_CODE_GetHeadPhonesVolume                    = API_CODE_GetHeadPhonesVolume,
	GFX100SII_API_CODE_CapXLRAdapterMicSource                 = API_CODE_CapXLRAdapterMicSource,
	GFX100SII_API_CODE_SetXLRAdapterMicSource                 = API_CODE_SetXLRAdapterMicSource,
	GFX100SII_API_CODE_GetXLRAdapterMicSource                 = API_CODE_GetXLRAdapterMicSource,
	GFX100SII_API_CODE_CapXLRAdapterMoniteringSource          = API_CODE_CapXLRAdapterMoniteringSource,
	GFX100SII_API_CODE_SetXLRAdapterMoniteringSource          = API_CODE_SetXLRAdapterMoniteringSource,
	GFX100SII_API_CODE_GetXLRAdapterMoniteringSource          = API_CODE_GetXLRAdapterMoniteringSource,
	GFX100SII_API_CODE_CapXLRAdapterHDMIOutputSource          = API_CODE_CapXLRAdapterHDMIOutputSource,
	GFX100SII_API_CODE_SetXLRAdapterHDMIOutputSource          = API_CODE_SetXLRAdapterHDMIOutputSource,
	GFX100SII_API_CODE_GetXLRAdapterHDMIOutputSource          = API_CODE_GetXLRAdapterHDMIOutputSource,
	GFX100SII_API_CODE_GetMicLevelIndicator                   = API_CODE_GetMicLevelIndicator,
	GFX100SII_API_CODE_GetMovieTransparentFrameInfo           = API_CODE_GetMovieTransparentFrameInfo,
	GFX100SII_API_CODE_CapShortMovieSecond                    = API_CODE_CapShortMovieSecond,
	GFX100SII_API_CODE_SetShortMovieSecond                    = API_CODE_SetShortMovieSecond,
	GFX100SII_API_CODE_GetShortMovieSecond                    = API_CODE_GetShortMovieSecond,
	GFX100SII_API_CODE_CapMoviePortraitEnhancer               = API_CODE_CapMoviePortraitEnhancer,
	GFX100SII_API_CODE_SetMoviePortraitEnhancer               = API_CODE_SetMoviePortraitEnhancer,
	GFX100SII_API_CODE_GetMoviePortraitEnhancer               = API_CODE_GetMoviePortraitEnhancer,
	GFX100SII_API_CODE_CapDirectionalMic                      = API_CODE_CapDirectionalMic,
	GFX100SII_API_CODE_SetDirectionalMic                      = API_CODE_SetDirectionalMic,
	GFX100SII_API_CODE_GetDirectionalMic                      = API_CODE_GetDirectionalMic,
	GFX100SII_API_CODE_CapEnvSoundsNR                         = API_CODE_CapEnvSoundsNR,
	GFX100SII_API_CODE_SetEnvSoundsNR                         = API_CODE_SetEnvSoundsNR,
	GFX100SII_API_CODE_GetEnvSoundsNR                         = API_CODE_GetEnvSoundsNR,
	GFX100SII_API_CODE_GetBodyTemperatureWarning              = API_CODE_GetBodyTemperatureWarning,
	GFX100SII_API_CODE_GetPlayBackMediaInfo                   = API_CODE_GetPlayBackMediaInfo,
	GFX100SII_API_CODE_GetPlayBackObjectInfo                  = API_CODE_GetPlayBackObjectInfo,
	GFX100SII_API_CODE_GetPlayBackThumb                       = API_CODE_GetPlayBackThumb,
	GFX100SII_API_CODE_GetPlayBackPartialObject               = API_CODE_GetPlayBackPartialObject,
	GFX100SII_API_CODE_CapMovieRecVolume                      = API_CODE_CapMovieRecVolume,
	GFX100SII_API_CODE_SetMovieRecVolume                      = API_CODE_SetMovieRecVolume,
	GFX100SII_API_CODE_GetMovieRecVolume                      = API_CODE_GetMovieRecVolume,
};

// Movie Af Mode
enum{
	GFX100SII_MOVIE_AF_MULTI                     = SDK_MOVIE_AF_MULTI,
	GFX100SII_MOVIE_AF_AREA                      = SDK_MOVIE_AF_AREA,
	GFX100SII_MOVIE_AF_WIDETRACKING              = SDK_MOVIE_AF_WIDETRACKING,
};

// Short Movie Second
enum{
	GFX100SII_SHORT_MOVIE_SECOND_OFF             = SDK_SHORT_MOVIE_SECOND_OFF,
	GFX100SII_SHORT_MOVIE_SECOND_15S             = SDK_SHORT_MOVIE_SECOND_15S,
	GFX100SII_SHORT_MOVIE_SECOND_30S             = SDK_SHORT_MOVIE_SECOND_30S,
	GFX100SII_SHORT_MOVIE_SECOND_60S             = SDK_SHORT_MOVIE_SECOND_60S,
};

// Movie Resolution
// High Speed Rec Resolution
enum{
	GFX100SII_MOVIE_RESOLUTION_6P2K_3_2          = SDK_MOVIE_RESOLUTION_6P2K_3_2,
	GFX100SII_MOVIE_RESOLUTION_8K_16_9           = SDK_MOVIE_RESOLUTION_8K_16_9,
	GFX100SII_MOVIE_RESOLUTION_6K_16_9           = SDK_MOVIE_RESOLUTION_6K_16_9,
	GFX100SII_MOVIE_RESOLUTION_4KHQ_16_9         = SDK_MOVIE_RESOLUTION_4KHQ_16_9,
	GFX100SII_MOVIE_RESOLUTION_4K_16_9           = SDK_MOVIE_RESOLUTION_4K_16_9,
	GFX100SII_MOVIE_RESOLUTION_DCIHQ_17_9        = SDK_MOVIE_RESOLUTION_DCIHQ_17_9,
	GFX100SII_MOVIE_RESOLUTION_DCI_17_9          = SDK_MOVIE_RESOLUTION_DCI_17_9,
	GFX100SII_MOVIE_RESOLUTION_FULLHD_16_9       = SDK_MOVIE_RESOLUTION_FULLHD_16_9,
	GFX100SII_MOVIE_RESOLUTION_FULLHD_17_9       = SDK_MOVIE_RESOLUTION_FULLHD_17_9,
	GFX100SII_MOVIE_RESOLUTION_5K_17_9           = SDK_MOVIE_RESOLUTION_5K_17_9,
	GFX100SII_MOVIE_RESOLUTION_DCI_8K_17_9       = SDK_MOVIE_RESOLUTION_DCI_8K_17_9,
	GFX100SII_MOVIE_RESOLUTION_CINESCO_2P35_1    = SDK_MOVIE_RESOLUTION_CINESCO_2P35_1,
	GFX100SII_MOVIE_RESOLUTION_OPENGATE_3_2      = SDK_MOVIE_RESOLUTION_OPENGATE_3_2,
	GFX100SII_MOVIE_RESOLUTION_35MM_16_9         = SDK_MOVIE_RESOLUTION_35MM_16_9,
	GFX100SII_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_2P76_1,
	GFX100SII_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1 = SDK_MOVIE_RESOLUTION_ANAMORPHIC_1P38_1,
	GFX100SII_MOVIE_RESOLUTION_FULLFRAME_3_2     = SDK_MOVIE_RESOLUTION_FULLFRAME_3_2,
	GFX100SII_MOVIE_RESOLUTION_FULLHD_LP_16_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_16_9,
	GFX100SII_MOVIE_RESOLUTION_FULLHD_LP_17_9    = SDK_MOVIE_RESOLUTION_FULLHD_LP_17_9,
	GFX100SII_MOVIE_RESOLUTION_4K_LP_16_9        = SDK_MOVIE_RESOLUTION_4K_LP_16_9,
	GFX100SII_MOVIE_RESOLUTION_FULLHD_9_16       = SDK_MOVIE_RESOLUTION_FULLHD_9_16,
};

// Movie FrameRate
// High Speed Rec PlayBack FrameRate
enum{
	GFX100SII_MOVIE_FRAMERATE_59_94P             = SDK_MOVIE_FRAMERATE_59_94P,
	GFX100SII_MOVIE_FRAMERATE_50P                = SDK_MOVIE_FRAMERATE_50P,
	GFX100SII_MOVIE_FRAMERATE_29_97P             = SDK_MOVIE_FRAMERATE_29_97P,
	GFX100SII_MOVIE_FRAMERATE_25P                = SDK_MOVIE_FRAMERATE_25P,
	GFX100SII_MOVIE_FRAMERATE_24P                = SDK_MOVIE_FRAMERATE_24P,
	GFX100SII_MOVIE_FRAMERATE_23_98P             = SDK_MOVIE_FRAMERATE_23_98P,
};

// Movie BitRate
enum{
	GFX100SII_MOVIE_BITRATE_720MBPS              = SDK_MOVIE_BITRATE_720MBPS,
	GFX100SII_MOVIE_BITRATE_400MBPS              = SDK_MOVIE_BITRATE_400MBPS,
	GFX100SII_MOVIE_BITRATE_360MBPS              = SDK_MOVIE_BITRATE_360MBPS,
	GFX100SII_MOVIE_BITRATE_200MBPS              = SDK_MOVIE_BITRATE_200MBPS,
	GFX100SII_MOVIE_BITRATE_100MBPS              = SDK_MOVIE_BITRATE_100MBPS,
	GFX100SII_MOVIE_BITRATE_50MBPS               = SDK_MOVIE_BITRATE_50MBPS,
	GFX100SII_MOVIE_BITRATE_25MBPS               = SDK_MOVIE_BITRATE_25MBPS,
	GFX100SII_MOVIE_BITRATE_8MBPS                = SDK_MOVIE_BITRATE_8MBPS,
};

// Movie FileFormat
enum{
	GFX100SII_MOVIE_FORMAT_H264_ALL_I_MOV        = SDK_MOVIE_FORMAT_H264_ALL_I_MOV,
	GFX100SII_MOVIE_FORMAT_H264_LONGGOP_I_MOV    = SDK_MOVIE_FORMAT_H264_LONGGOP_I_MOV,
	GFX100SII_MOVIE_FORMAT_H264_LONGGOP_MP4      = SDK_MOVIE_FORMAT_H264_LONGGOP_MP4,
	GFX100SII_MOVIE_FORMAT_H265_4_2_0_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_0_ALL_I,
	GFX100SII_MOVIE_FORMAT_H265_4_2_0_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_0_LONGGOP,
	GFX100SII_MOVIE_FORMAT_H265_4_2_2_ALL_I      = SDK_MOVIE_FORMAT_H265_4_2_2_ALL_I,
	GFX100SII_MOVIE_FORMAT_H265_4_2_2_LONGGOP    = SDK_MOVIE_FORMAT_H265_4_2_2_LONGGOP,
	GFX100SII_MOVIE_FORMAT_PRORESHQ              = SDK_MOVIE_FORMAT_PRORESHQ,
	GFX100SII_MOVIE_FORMAT_PRORES                = SDK_MOVIE_FORMAT_PRORES,
	GFX100SII_MOVIE_FORMAT_PRORESLT              = SDK_MOVIE_FORMAT_PRORESLT,
};

// High Speed RecMode
enum{
	GFX100SII_HIGHSPEEDREC_OFF                   = SDK_HIGHSPEEDREC_OFF,
	GFX100SII_HIGHSPEEDREC_ON                    = SDK_HIGHSPEEDREC_ON,
	GFX100SII_HIGHSPEEDREC_ON_HDMI_ONLY          = SDK_HIGHSPEEDREC_ON_HDMI_ONLY,
};

// High Speed Rec FrameRate
enum{
	GFX100SII_HIGHSPEEDREC_FRAMERATE_240P        = SDK_HIGHSPEEDREC_FRAMERATE_240P,
	GFX100SII_HIGHSPEEDREC_FRAMERATE_200P        = SDK_HIGHSPEEDREC_FRAMERATE_200P,
	GFX100SII_HIGHSPEEDREC_FRAMERATE_120P        = SDK_HIGHSPEEDREC_FRAMERATE_120P,
	GFX100SII_HIGHSPEEDREC_FRAMERATE_100P        = SDK_HIGHSPEEDREC_FRAMERATE_100P,
};

// Movie Media Record
enum{
	GFX100SII_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT1_SLOT2,
	GFX100SII_MOVIE_MEDIARECORD_SLOT2                  = SDK_MOVIE_MEDIARECORD_SLOT2,
	GFX100SII_MOVIE_MEDIARECORD_SLOT1                  = SDK_MOVIE_MEDIARECORD_SLOT1,
	GFX100SII_MOVIE_MEDIARECORD_BACKUP                 = SDK_MOVIE_MEDIARECORD_BACKUP,
	GFX100SII_MOVIE_MEDIARECORD_SSD                    = SDK_MOVIE_MEDIARECORD_SSD,
	GFX100SII_MOVIE_MEDIARECORD_SSD_CF                 = SDK_MOVIE_MEDIARECORD_SSD_CF,
	GFX100SII_MOVIE_MEDIARECORD_OFF                    = SDK_MOVIE_MEDIARECORD_OFF,
	GFX100SII_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF       = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SD_CF,
	GFX100SII_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1 = SDK_MOVIE_MEDIARECORD_SEQUENTIAL_SLOT2_SLOT1,
};

// Movie Media Record ProRes
enum{
	GFX100SII_MOVIE_MEDIARECORD_PRORES_OFF       = SDK_MOVIE_MEDIARECORD_PRORES_OFF,
	GFX100SII_MOVIE_MEDIARECORD_PRORES_H264      = SDK_MOVIE_MEDIARECORD_PRORES_H264,
	GFX100SII_MOVIE_MEDIARECORD_PRORES_PROXY     = SDK_MOVIE_MEDIARECORD_PRORES_PROXY,
};

// Movie HDMIOutput RAW
enum{
	GFX100SII_MOVIE_HDMI_OUTPUT_RAW_OFF          = SDK_MOVIE_HDMI_OUTPUT_RAW_OFF,
	GFX100SII_MOVIE_HDMI_OUTPUT_RAW_ATOMOS       = SDK_MOVIE_HDMI_OUTPUT_RAW_ATOMOS,
	GFX100SII_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC   = SDK_MOVIE_HDMI_OUTPUT_RAW_BLACKMAGIC,
};

// Movie HDMI Output RAW Resolution
enum{
	GFX100SII_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4P8K,
	GFX100SII_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_6P2K,
	GFX100SII_MOVIE_HDMI_OUTPUT_RESOLUTION_8K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_8K,
	GFX100SII_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K  = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_5P2K,
	GFX100SII_MOVIE_HDMI_OUTPUT_RESOLUTION_4K    = SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_4K,
	GFX100SII_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K= SDK_MOVIE_HDMI_OUTPUT_RESOLUTION_DCI_8K,
};

// Movie HDMI Output RAW FrameRate
enum{
	GFX100SII_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_59_94P,
	GFX100SII_MOVIE_HDMI_OUTPUT_FRAMERATE_50P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_50P,
	GFX100SII_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_29_97P,
	GFX100SII_MOVIE_HDMI_OUTPUT_FRAMERATE_25P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_25P,
	GFX100SII_MOVIE_HDMI_OUTPUT_FRAMERATE_24P    = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_24P,
	GFX100SII_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P = SDK_MOVIE_HDMI_OUTPUT_FRAMERATE_23_98P,
};

// Flog Recording
enum{
	GFX100SII_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FSIM,
	GFX100SII_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FLOG,
	GFX100SII_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2 = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FLOG2,
	GFX100SII_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG   = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG,
	GFX100SII_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2  = SDK_MOVIERECORD_MEDIA_FSIM_HDMI_FLOG2,
	GFX100SII_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM   = SDK_MOVIERECORD_MEDIA_FLOG_HDMI_FSIM,
	GFX100SII_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM  = SDK_MOVIERECORD_MEDIA_FLOG2_HDMI_FSIM,
	GFX100SII_MOVIERECORD_MEDIA_HLG_HDMI_HLG     = SDK_MOVIERECORD_MEDIA_HLG_HDMI_HLG,
};

// MovieIsMode
enum{
	GFX100SII_MOVIE_IS_MODE_OFF                  = SDK_MOVIE_IS_MODE_OFF,
	GFX100SII_MOVIE_IS_MODE_ON                   = SDK_MOVIE_IS_MODE_ON,
	GFX100SII_MOVIE_IS_MODE_IBIS_OIS             = SDK_MOVIE_IS_MODE_IBIS_OIS,
	GFX100SII_MOVIE_IS_MODE_IBIS_OIS_DIS         = SDK_MOVIE_IS_MODE_IBIS_OIS_DIS,
	GFX100SII_MOVIE_IS_MODE_OIS                  = SDK_MOVIE_IS_MODE_OIS,
	GFX100SII_MOVIE_IS_MODE_OIS_DIS              = SDK_MOVIE_IS_MODE_OIS_DIS,
};

// Movie Zebra Setting
enum{
	GFX100SII_MOVIE_ZEBRA_SETTING_OFF            = SDK_MOVIE_ZEBRA_SETTING_OFF,
	GFX100SII_MOVIE_ZEBRA_SETTING_RIGHT          = SDK_MOVIE_ZEBRA_SETTING_RIGHT,
	GFX100SII_MOVIE_ZEBRA_SETTING_LEFT           = SDK_MOVIE_ZEBRA_SETTING_LEFT,
};

// Movie Tally Light
enum{
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON      = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_ON,
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK   = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_BLINK,
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON       = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_ON,
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF      = SDK_MOVIE_TALLYLIGHT_FRONT_ON_REAR_OFF,
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_BLINK,
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF   = SDK_MOVIE_TALLYLIGHT_FRONT_BLINK_REAR_OFF,
	GFX100SII_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF     = SDK_MOVIE_TALLYLIGHT_FRONT_OFF_REAR_OFF,
};

// Media Eject Warning
enum{
	GFX100SII_MEDIA_EJECT_WARNING_SLOT1          = SDK_MEDIA_EJECT_WARNING_SLOT1,
	GFX100SII_MEDIA_EJECT_WARNING_SLOT2          = SDK_MEDIA_EJECT_WARNING_SLOT2,
	GFX100SII_MEDIA_EJECT_WARNING_SLOT3          = SDK_MEDIA_EJECT_WARNING_SLOT3,
};

// Movie Data Level Setting
enum{
	GFX100SII_MOVIE_DATA_LEVEL_SETTING_FULL      = SDK_MOVIE_DATA_LEVEL_SETTING_FULL,
	GFX100SII_MOVIE_DATA_LEVEL_SETTING_VIDEO     = SDK_MOVIE_DATA_LEVEL_SETTING_VIDEO,
};

// Movie ImageFormat
enum{
	GFX100SII_MOVIE_IMAGEFORMAT_GF               = SDK_MOVIE_IMAGEFORMAT_GF,
	GFX100SII_MOVIE_IMAGEFORMAT_35MM             = SDK_MOVIE_IMAGEFORMAT_35MM,
	GFX100SII_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM  = SDK_MOVIE_IMAGEFORMAT_ANAMORPHIC_35MM,
	GFX100SII_MOVIE_IMAGEFORMAT_PREMISTA         = SDK_MOVIE_IMAGEFORMAT_PREMISTA,
};

// WaveForm VectorScope
enum{
	GFX100SII_WAVEFORM_VECTORSCOPE_OFF           = SDK_WAVEFORM_VECTORSCOPE_OFF,
	GFX100SII_WAVEFORM_VECTORSCOPE_WAVEFORM      = SDK_WAVEFORM_VECTORSCOPE_WAVEFORM,
	GFX100SII_WAVEFORM_VECTORSCOPE_PARADE        = SDK_WAVEFORM_VECTORSCOPE_PARADE,
	GFX100SII_WAVEFORM_VECTORSCOPE_VECTORSCOPE   = SDK_WAVEFORM_VECTORSCOPE_VECTORSCOPE,
};

// WaveForm Setting
enum{
	GFX100SII_WAVEFORM_SETTING_PATTERN1          = SDK_WAVEFORM_SETTING_PATTERN1,
	GFX100SII_WAVEFORM_SETTING_PATTERN2          = SDK_WAVEFORM_SETTING_PATTERN2,
	GFX100SII_WAVEFORM_SETTING_PATTERN3          = SDK_WAVEFORM_SETTING_PATTERN3,
	GFX100SII_WAVEFORM_SETTING_PATTERN4          = SDK_WAVEFORM_SETTING_PATTERN4,
};

// VectorScope Setting
enum{
	GFX100SII_VECTORSCORE_SETTING_PATTERN1       = SDK_VECTORSCORE_SETTING_PATTERN1,
	GFX100SII_VECTORSCORE_SETTING_PATTERN2       = SDK_VECTORSCORE_SETTING_PATTERN2,
	GFX100SII_VECTORSCORE_SETTING_PATTERN3       = SDK_VECTORSCORE_SETTING_PATTERN3,
	GFX100SII_VECTORSCORE_SETTING_PATTERN4       = SDK_VECTORSCORE_SETTING_PATTERN4,
};

// Parade Setting Display
enum{
	GFX100SII_PARADE_SETTING_DISPLAY_PATTERN1    = SDK_PARADE_SETTING_DISPLAY_PATTERN1,
	GFX100SII_PARADE_SETTING_DISPLAY_PATTERN2    = SDK_PARADE_SETTING_DISPLAY_PATTERN2,
	GFX100SII_PARADE_SETTING_DISPLAY_PATTERN3    = SDK_PARADE_SETTING_DISPLAY_PATTERN3,
	GFX100SII_PARADE_SETTING_DISPLAY_PATTERN4    = SDK_PARADE_SETTING_DISPLAY_PATTERN4,
};

// Parade Setting Color
enum{
	GFX100SII_PARADE_SETTING_COLOR_RGB           = SDK_PARADE_SETTING_COLOR_RGB,
	GFX100SII_PARADE_SETTING_COLOR_WHITE         = SDK_PARADE_SETTING_COLOR_WHITE,
};

// Movie FocusMeter(display)
enum{
	GFX100SII_FOCUSMETER_DISPLAY_OFF             = SDK_FOCUSMETER_DISPLAY_OFF,
	GFX100SII_FOCUSMETER_DISPLAY_ON              = SDK_FOCUSMETER_DISPLAY_ON,
};

// Movie FocusMeter(color)
enum{
	GFX100SII_FOCUSMETER_COLOR_WHITE             = SDK_FOCUSMETER_COLOR_WHITE,
	GFX100SII_FOCUSMETER_COLOR_GREEN             = SDK_FOCUSMETER_COLOR_GREEN,
};

// TimeCode Start Setting
enum{
	GFX100SII_TIMECODE_START_SETTING_MANUAL      = SDK_TIMECODE_START_SETTING_MANUAL,
	GFX100SII_TIMECODE_START_SETTING_CURRENT     = SDK_TIMECODE_START_SETTING_CURRENT,
	GFX100SII_TIMECODE_START_SETTING_RESET       = SDK_TIMECODE_START_SETTING_RESET,
};

// TimeCode CountUp
enum{
	GFX100SII_TIMECODE_COUNTUP_RECRUN            = SDK_TIMECODE_COUNTUP_RECRUN,
	GFX100SII_TIMECODE_COUNTUP_FREERUN           = SDK_TIMECODE_COUNTUP_FREERUN,
};

// TimeCode Status
enum{
	GFX100SII_TIMECODE_STATUS_USE_DSC_ALONE      = SDK_TIMECODE_STATUS_USE_DSC_ALONE,
	GFX100SII_TIMECODE_STATUS_SYNCING            = SDK_TIMECODE_STATUS_SYNCING,
	GFX100SII_TIMECODE_STATUS_DISCONNECTED       = SDK_TIMECODE_STATUS_DISCONNECTED,
	GFX100SII_TIMECODE_STATUS_NOT_SYNCED         = SDK_TIMECODE_STATUS_NOT_SYNCED,
	GFX100SII_TIMECODE_STATUS_FRAMERATE_MISMATCH = SDK_TIMECODE_STATUS_FRAMERATE_MISMATCH,
};

// Internal Mic Level
// External Mic Level
enum{
	GFX100SII_MIC_LEVEL_OFF                      = SDK_MIC_LEVEL_OFF,
	GFX100SII_MIC_LEVEL_MANUAL                   = SDK_MIC_LEVEL_MANUAL,
	GFX100SII_MIC_LEVEL_AUTO                     = SDK_MIC_LEVEL_AUTO,
};

// HeadPhones Volume
enum{
	GFX100SII_HEADPHONES_VOLUME_0                = SDK_HEADPHONES_VOLUME_0,
	GFX100SII_HEADPHONES_VOLUME_1                = SDK_HEADPHONES_VOLUME_1,
	GFX100SII_HEADPHONES_VOLUME_2                = SDK_HEADPHONES_VOLUME_2,
	GFX100SII_HEADPHONES_VOLUME_3                = SDK_HEADPHONES_VOLUME_3,
	GFX100SII_HEADPHONES_VOLUME_4                = SDK_HEADPHONES_VOLUME_4,
	GFX100SII_HEADPHONES_VOLUME_5                = SDK_HEADPHONES_VOLUME_5,
	GFX100SII_HEADPHONES_VOLUME_6                = SDK_HEADPHONES_VOLUME_6,
	GFX100SII_HEADPHONES_VOLUME_7                = SDK_HEADPHONES_VOLUME_7,
	GFX100SII_HEADPHONES_VOLUME_8                = SDK_HEADPHONES_VOLUME_8,
	GFX100SII_HEADPHONES_VOLUME_9                = SDK_HEADPHONES_VOLUME_9,
	GFX100SII_HEADPHONES_VOLUME_10               = SDK_HEADPHONES_VOLUME_10,
};

// XLR Adapter Mic Source
enum{
	GFX100SII_XLRADAPTER_MIC_SOURCE_4CH          = SDK_XLRADAPTER_MIC_SOURCE_4CH,
	GFX100SII_XLRADAPTER_MIC_SOURCE_2CH          = SDK_XLRADAPTER_MIC_SOURCE_2CH,
};

// XLR Adapter Monitering Source
enum{
	GFX100SII_XLRADAPTER_MONITER_SOURCE_XLR      = SDK_XLRADAPTER_MONITER_SOURCE_XLR,
	GFX100SII_XLRADAPTER_MONITER_SOURCE_CAMERA   = SDK_XLRADAPTER_MONITER_SOURCE_CAMERA,
};

// XLR Adapter HDMI Output Source
enum{
	GFX100SII_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR   = SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_XLR,
	GFX100SII_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA= SDK_XLRADAPTER_HDMIOUTPUT_SOURCE_CAMERA,
};

// Directional Mic
enum{
	GFX100SII_DIRECTIONAL_MIC_AUTO               = SDK_DIRECTIONAL_MIC_AUTO,
	GFX100SII_DIRECTIONAL_MIC_SURROUND           = SDK_DIRECTIONAL_MIC_SURROUND,
	GFX100SII_DIRECTIONAL_MIC_FRONT              = SDK_DIRECTIONAL_MIC_FRONT,
	GFX100SII_DIRECTIONAL_MIC_TRACKING           = SDK_DIRECTIONAL_MIC_TRACKING,
	GFX100SII_DIRECTIONAL_MIC_BACK               = SDK_DIRECTIONAL_MIC_BACK,
	GFX100SII_DIRECTIONAL_MIC_FRONT_BACK         = SDK_DIRECTIONAL_MIC_FRONT_BACK,
};

// Body Temperature Warning
enum{
	GFX100SII_BODY_TEMPERATURE_WARNING_NONE      = SDK_BODY_TEMPERATURE_WARNING_NONE,
	GFX100SII_BODY_TEMPERATURE_WARNING_YELLOW    = SDK_BODY_TEMPERATURE_WARNING_YELLOW,
	GFX100SII_BODY_TEMPERATURE_WARNING_RED       = SDK_BODY_TEMPERATURE_WARNING_RED,
};

// Movie Rec Volume
enum{
	GFX100SII_MOVIE_REC_VOLUME_OFF               = SDK_MOVIE_REC_VOLUME_OFF,
	GFX100SII_MOVIE_REC_VOLUME_1                 = SDK_MOVIE_REC_VOLUME_1,
	GFX100SII_MOVIE_REC_VOLUME_2                 = SDK_MOVIE_REC_VOLUME_2,
	GFX100SII_MOVIE_REC_VOLUME_3                 = SDK_MOVIE_REC_VOLUME_3,
};
