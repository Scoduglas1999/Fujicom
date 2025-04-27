///////////////////////////////////////////////////
//  X-Pro3.h
//     Version 1.0.0.0
//
//     Copyright (C) 2019 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-Pro3
//
enum {
	XPRO3_API_PARAM_SetImageSize                = 1,
	XPRO3_API_PARAM_GetImageSize                = 1,
	XPRO3_API_PARAM_SetSharpness                = 1,
	XPRO3_API_PARAM_GetSharpness                = 1,
	XPRO3_API_PARAM_SetColorMode                = 1,
	XPRO3_API_PARAM_GetColorMode                = 1,
	XPRO3_API_PARAM_SetFilmSimulationMode       = 1,
	XPRO3_API_PARAM_GetFilmSimulationMode       = 1,
	XPRO3_API_PARAM_SetColorSpace               = 1,
	XPRO3_API_PARAM_GetColorSpace               = 1,
	XPRO3_API_PARAM_SetImageQuality             = 1,
	XPRO3_API_PARAM_GetImageQuality             = 1,
	XPRO3_API_PARAM_SetRAWCompression           = 1,
	XPRO3_API_PARAM_GetRAWCompression           = 1,
	XPRO3_API_PARAM_SetGrainEffect              = 1,
	XPRO3_API_PARAM_GetGrainEffect              = 1,
	XPRO3_API_PARAM_SetShadowing                = 1,
	XPRO3_API_PARAM_GetShadowing                = 1,
	XPRO3_API_PARAM_SetNoiseReduction           = 1,
	XPRO3_API_PARAM_GetNoiseReduction           = 1,
	XPRO3_API_PARAM_SetFaceDetectionMode        = 1,
	XPRO3_API_PARAM_GetFaceDetectionMode        = 1,
	XPRO3_API_PARAM_SetEyeAFMode                = 1,
	XPRO3_API_PARAM_GetEyeAFMode                = 1,
	XPRO3_API_PARAM_SetMacroMode                = -1,
	XPRO3_API_PARAM_GetMacroMode                = -1,
	XPRO3_API_PARAM_CapFocusLimiter             = 2,
	XPRO3_API_PARAM_SetFocusLimiter             = 1,
	XPRO3_API_PARAM_GetFocusLimiter             = 1,
	XPRO3_API_PARAM_SetHighLightTone            = 1,
	XPRO3_API_PARAM_GetHighLightTone            = 1,
	XPRO3_API_PARAM_SetShadowTone               = 1,
	XPRO3_API_PARAM_GetShadowTone               = 1,
	XPRO3_API_PARAM_SetLongExposureNR           = 1,
	XPRO3_API_PARAM_GetLongExposureNR           = 1,
	XPRO3_API_PARAM_SetFullTimeManualFocus      = 1,
	XPRO3_API_PARAM_GetFullTimeManualFocus      = 1,
	XPRO3_API_PARAM_SetFocusMode                = 1,
	XPRO3_API_PARAM_GetFocusMode                = 1,
	XPRO3_API_PARAM_SetAFMode                   = 2,
	XPRO3_API_PARAM_GetAFMode                   = 2,
	XPRO3_API_PARAM_SetFocusPoints              = 1,
	XPRO3_API_PARAM_GetFocusPoints              = 1,
	XPRO3_API_PARAM_SetFocusArea                = 2,
	XPRO3_API_PARAM_GetFocusArea                = 2,
	XPRO3_API_PARAM_SetShutterPriorityMode      = 2,
	XPRO3_API_PARAM_GetShutterPriorityMode      = 2,
	XPRO3_API_PARAM_SetInstantAFMode            = 1,
	XPRO3_API_PARAM_GetInstantAFMode            = 1,
	XPRO3_API_PARAM_SetPreAFMode                = 1,
	XPRO3_API_PARAM_GetPreAFMode                = 1,
	XPRO3_API_PARAM_SetAFIlluminator            = 1,
	XPRO3_API_PARAM_GetAFIlluminator            = 1,
	XPRO3_API_PARAM_CapLensISSwitch             = 2,
	XPRO3_API_PARAM_SetLensISSwitch             = 1,
	XPRO3_API_PARAM_GetLensISSwitch             = 1,
	XPRO3_API_PARAM_SetISMode                   = 1,
	XPRO3_API_PARAM_GetISMode                   = 1,
	XPRO3_API_PARAM_SetLMOMode                  = 1,
	XPRO3_API_PARAM_GetLMOMode                  = 1,
	XPRO3_API_PARAM_SetWhiteBalanceMode         = -1,
	XPRO3_API_PARAM_GetWhiteBalanceMode         = -1,
	XPRO3_API_PARAM_SetWhiteBalanceTune         = 3,
	XPRO3_API_PARAM_GetWhiteBalanceTune         = 3,
	XPRO3_API_PARAM_SetCaptureDelay             = 1,
	XPRO3_API_PARAM_GetCaptureDelay             = 1,
	XPRO3_API_PARAM_CapFocusPos                 = 2,
	XPRO3_API_PARAM_SetFocusPos                 = 1,
	XPRO3_API_PARAM_GetFocusPos                 = 1,
	XPRO3_API_PARAM_SetMFAssistMode             = 1,
	XPRO3_API_PARAM_GetMFAssistMode             = 1,
	XPRO3_API_PARAM_SetFocusCheckMode           = 1,
	XPRO3_API_PARAM_GetFocusCheckMode           = 1,
	XPRO3_API_PARAM_SetInterlockAEAFArea        = 1,
	XPRO3_API_PARAM_GetInterlockAEAFArea        = 1,
	XPRO3_API_PARAM_StartLiveView               = 0,
	XPRO3_API_PARAM_StopLiveView                = 0,
	XPRO3_API_PARAM_SetLiveViewImageQuality     = 1,
	XPRO3_API_PARAM_GetLiveViewImageQuality     = 1,
	XPRO3_API_PARAM_SetLiveViewImageSize        = 1,
	XPRO3_API_PARAM_GetLiveViewImageSize        = 1,
	XPRO3_API_PARAM_SetThroughImageZoom         = 1,
	XPRO3_API_PARAM_GetThroughImageZoom         = 1,
	XPRO3_API_PARAM_SetDateTime                 = 6,
	XPRO3_API_PARAM_GetDateTime                 = 6,
	XPRO3_API_PARAM_SetDateTimeDispFormat       = -1,
	XPRO3_API_PARAM_GetDateTimeDispFormat       = -1,
	XPRO3_API_PARAM_SetWorldClock               = -1,
	XPRO3_API_PARAM_GetWorldClock               = -1,
	XPRO3_API_PARAM_SetTimeDifference           = -1,
	XPRO3_API_PARAM_GetTimeDifference           = -1,
	XPRO3_API_PARAM_ResetSetting                = 1,
	XPRO3_API_PARAM_SetSilentMode               = -1,
	XPRO3_API_PARAM_GetSilentMode               = -1,
	XPRO3_API_PARAM_SetBeep                     = -1,
	XPRO3_API_PARAM_GetBeep                     = -1,
	XPRO3_API_PARAM_SetFunctionLock             = 1,
	XPRO3_API_PARAM_GetFunctionLock             = 1,
	XPRO3_API_PARAM_SetFunctionLockCategory     = 2,
	XPRO3_API_PARAM_GetFunctionLockCategory     = 2,
	XPRO3_API_PARAM_SetComment                  = 1,
	XPRO3_API_PARAM_GetComment                  = 1,
	XPRO3_API_PARAM_SetCopyright                = 2,
	XPRO3_API_PARAM_GetCopyright                = 2,
	XPRO3_API_PARAM_SetFilenamePrefix           = 2,
	XPRO3_API_PARAM_GetFilenamePrefix           = 2,
	XPRO3_API_PARAM_SetFoldernameSuffix         = 3,
	XPRO3_API_PARAM_GetFoldernameSuffix         = 3,
	XPRO3_API_PARAM_GetFoldernameList           = 4,
	XPRO3_API_PARAM_CheckBatteryInfo            = 6,
	XPRO3_API_PARAM_SetFrameNumberSequence      = -1,
	XPRO3_API_PARAM_GetFrameNumberSequence      = -1,
	XPRO3_API_PARAM_SetUSBMode                  = -1,
	XPRO3_API_PARAM_GetUSBMode                  = -1,
	XPRO3_API_PARAM_FormatMemoryCard            = 1,
	XPRO3_API_PARAM_SetMediaRecord              = 1,
	XPRO3_API_PARAM_GetMediaRecord              = 1,
	XPRO3_API_PARAM_GetMediaCapacity            = 5,
	XPRO3_API_PARAM_GetMediaStatus              = 2,
	XPRO3_API_PARAM_GetShutterCount             = 3,
	XPRO3_API_PARAM_GetShutterCountEx           = -1,
	XPRO3_API_PARAM_SetSensorCleanTiming        = -1,
	XPRO3_API_PARAM_GetSensorCleanTiming        = -1,
	XPRO3_API_PARAM_SetPreviewTime              = -1,
	XPRO3_API_PARAM_GetPreviewTime              = -1,
	XPRO3_API_PARAM_SetEVFDispAutoRotate        = -1,
	XPRO3_API_PARAM_GetEVFDispAutoRotate        = -1,
	XPRO3_API_PARAM_SetDispMMode                = 1,
	XPRO3_API_PARAM_GetDispMMode                = 1,
	XPRO3_API_PARAM_SetExposurePreview          = 1,
	XPRO3_API_PARAM_GetExposurePreview          = 1,
	XPRO3_API_PARAM_SetDispBrightness           = -1,
	XPRO3_API_PARAM_GetDispBrightness           = -1,
	XPRO3_API_PARAM_SetFrameGuideMode           = 1,
	XPRO3_API_PARAM_GetFrameGuideMode           = 1,
	XPRO3_API_PARAM_SetFrameGuideGridInfo       = 2,
	XPRO3_API_PARAM_GetFrameGuideGridInfo       = 2,
	XPRO3_API_PARAM_SetAutoImageRotation        = -1,
	XPRO3_API_PARAM_GetAutoImageRotation        = -1,
	XPRO3_API_PARAM_SetFocusScaleUnit           = -1,
	XPRO3_API_PARAM_GetFocusScaleUnit           = -1,
	XPRO3_API_PARAM_SetCustomDispInfo           = -1,
	XPRO3_API_PARAM_GetCustomDispInfo           = -1,
	XPRO3_API_PARAM_SetViewMode                 = 2,
	XPRO3_API_PARAM_GetViewMode                 = 2,
	XPRO3_API_PARAM_SetDispInfoMode             = -1,
	XPRO3_API_PARAM_GetDispInfoMode             = -1,
	XPRO3_API_PARAM_SetDispChroma               = -1,
	XPRO3_API_PARAM_GetDispChroma               = -1,
	XPRO3_API_PARAM_SetCustomAutoPowerOff       = 1,
	XPRO3_API_PARAM_GetCustomAutoPowerOff       = 1,
	XPRO3_API_PARAM_SetCustomStudioPowerSave    = 1,
	XPRO3_API_PARAM_GetCustomStudioPowerSave    = 1,
	XPRO3_API_PARAM_SetFunctionButton           = -1,
	XPRO3_API_PARAM_GetFunctionButton           = -1,
	XPRO3_API_PARAM_SetISODialHn                = -1,
	XPRO3_API_PARAM_GetISODialHn                = -1,
	XPRO3_API_PARAM_SetLockButtonMode           = 1,
	XPRO3_API_PARAM_GetLockButtonMode           = 1,
	XPRO3_API_PARAM_SetAFLockMode               = -1,
	XPRO3_API_PARAM_GetAFLockMode               = -1,
	XPRO3_API_PARAM_SetMicJackMode              = -1,
	XPRO3_API_PARAM_GetMicJackMode              = -1,
	XPRO3_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XPRO3_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XPRO3_API_PARAM_SetCrossKeyAssign           = -1,
	XPRO3_API_PARAM_GetCrossKeyAssign           = -1,
	XPRO3_API_PARAM_SetWideDynamicRange         = 1,
	XPRO3_API_PARAM_GetWideDynamicRange         = 1,
	XPRO3_API_PARAM_SetClarityMode              = 1,
	XPRO3_API_PARAM_GetClarityMode              = 1,
	XPRO3_API_PARAM_SetBlackImageTone           = 1,
	XPRO3_API_PARAM_GetBlackImageTone           = 1,
	XPRO3_API_PARAM_GetTNumber                  = -1,
	XPRO3_API_PARAM_SetCropMode                 = 1,
	XPRO3_API_PARAM_GetCropMode                 = 2,
	XPRO3_API_PARAM_GetCropAreaFrameInfo        = -1,
	XPRO3_API_PARAM_SetRAWOutputDepth           = -1,
	XPRO3_API_PARAM_GetRAWOutputDepth           = -1,
	XPRO3_API_PARAM_SetSmoothSkinEffect         = 1,
	XPRO3_API_PARAM_GetSmoothSkinEffect         = 1,
	XPRO3_API_PARAM_GetDetectedFaceFrame        = 2,
	XPRO3_API_PARAM_SetDetectedFaceFrame        = 1,
    XPRO3_API_PARAM_SetCustomWBArea             = 2,
    XPRO3_API_PARAM_GetCustomWBArea             = 2,
	XPRO3_API_PARAM_SetColorChromeBlue          = 1,
	XPRO3_API_PARAM_GetColorChromeBlue          = 1,
	XPRO3_API_PARAM_SetMonochromaticColor       = 2,
	XPRO3_API_PARAM_GetMonochromaticColor       = 2,
	XPRO3_API_PARAM_SetFocusLimiterPos          = 2,
	XPRO3_API_PARAM_GetFocusLimiterIndicator    = 1,
	XPRO3_API_PARAM_GetFocusLimiterRange        = 2,
	XPRO3_API_PARAM_SetFocusLimiterMode         = 1,
	XPRO3_API_PARAM_GetFocusLimiterMode         = 1,
};

//
// API_CODE for X-Pro3
//
enum {
	XPRO3_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XPRO3_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XPRO3_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XPRO3_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XPRO3_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XPRO3_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XPRO3_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XPRO3_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XPRO3_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XPRO3_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XPRO3_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XPRO3_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XPRO3_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XPRO3_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XPRO3_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XPRO3_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XPRO3_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XPRO3_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XPRO3_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XPRO3_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XPRO3_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XPRO3_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XPRO3_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XPRO3_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XPRO3_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XPRO3_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XPRO3_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XPRO3_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XPRO3_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XPRO3_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XPRO3_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XPRO3_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XPRO3_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XPRO3_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XPRO3_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XPRO3_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XPRO3_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XPRO3_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XPRO3_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XPRO3_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XPRO3_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XPRO3_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XPRO3_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XPRO3_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XPRO3_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XPRO3_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XPRO3_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XPRO3_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XPRO3_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XPRO3_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XPRO3_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XPRO3_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XPRO3_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XPRO3_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XPRO3_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XPRO3_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XPRO3_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XPRO3_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XPRO3_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XPRO3_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XPRO3_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XPRO3_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XPRO3_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XPRO3_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XPRO3_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XPRO3_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XPRO3_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XPRO3_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XPRO3_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XPRO3_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XPRO3_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XPRO3_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XPRO3_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XPRO3_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XPRO3_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XPRO3_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XPRO3_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XPRO3_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XPRO3_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XPRO3_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XPRO3_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XPRO3_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XPRO3_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XPRO3_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XPRO3_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XPRO3_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XPRO3_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XPRO3_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XPRO3_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XPRO3_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XPRO3_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XPRO3_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XPRO3_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XPRO3_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XPRO3_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XPRO3_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XPRO3_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XPRO3_API_CODE_SetComment                  = API_CODE_SetComment,
	XPRO3_API_CODE_GetComment                  = API_CODE_GetComment,
	XPRO3_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XPRO3_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XPRO3_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XPRO3_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XPRO3_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XPRO3_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XPRO3_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XPRO3_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XPRO3_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XPRO3_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XPRO3_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XPRO3_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XPRO3_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XPRO3_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XPRO3_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XPRO3_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XPRO3_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XPRO3_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XPRO3_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XPRO3_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XPRO3_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XPRO3_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XPRO3_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XPRO3_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XPRO3_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XPRO3_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XPRO3_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XPRO3_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XPRO3_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XPRO3_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XPRO3_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XPRO3_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XPRO3_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XPRO3_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XPRO3_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XPRO3_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XPRO3_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XPRO3_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XPRO3_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XPRO3_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XPRO3_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XPRO3_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XPRO3_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XPRO3_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XPRO3_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XPRO3_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XPRO3_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XPRO3_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XPRO3_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XPRO3_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XPRO3_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XPRO3_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XPRO3_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XPRO3_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XPRO3_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XPRO3_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XPRO3_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XPRO3_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XPRO3_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XPRO3_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XPRO3_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XPRO3_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XPRO3_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XPRO3_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XPRO3_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XPRO3_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XPRO3_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XPRO3_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XPRO3_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XPRO3_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XPRO3_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XPRO3_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XPRO3_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XPRO3_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XPRO3_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XPRO3_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XPRO3_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XPRO3_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XPRO3_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XPRO3_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XPRO3_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XPRO3_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XPRO3_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XPRO3_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XPRO3_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XPRO3_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XPRO3_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XPRO3_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XPRO3_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XPRO3_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XPRO3_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XPRO3_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XPRO3_FocusArea, *PXPRO3_FocusArea;


// XSDK_ISOAuto - ISO Auto Setting
// typedef struct {
//    long    defaultISO;         // DEFAULT SENSITIVITY 
//                                // 200/250/320/400/500/640/800/1000/1250/1600/2000/2500/3200/4000/5000/6400
//    long    maxISO;             // MAX. SENSITIVITY
//                                // 400/800/1600/3200/6400
//    long    minShutterSpeed;    // MIN. SHUTTER SPEED
//                                // XSDK_SHUTTER_1_8    (1/8)
//                                // XSDK_SHUTTER_1_15   (1/15)
//                                // XSDK_SHUTTER_1_30   (1/30)
//                                // XSDK_SHUTTER_1_40   (1/40)
//                                // XSDK_SHUTTER_1_60   (1/60)
//                                // XSDK_SHUTTER_1_80   (1/80)
//                                // XSDK_SHUTTER_1_100  (1/100)
//                                // XSDK_SHUTTER_1_125  (1/125)
//                                // XSDK_SHUTTER_1_160  (1/160)
//                                // XSDK_SHUTTER_1_250  (1/250)
//                                // XSDK_SHUTTER_1_320  (1/320)
//                                // XSDK_SHUTTER_1_400  (1/400)
//                                // XSDK_SHUTTER_1_500  (1/500)
//    char    pName[32];          // NAME OF SETTINGS
// } SDK_ISOAuto;
typedef  SDK_ISOAuto     XPRO3_ISOAuto, *PXPRO3_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XPRO3_FrameGuideGridInfo, *PXPRO3_FrameGuideGridInfo;


//typedef struct _SDK_FOCUS_POS_CAP {
//    long lSizeFocusPosCap;
//    long lStructVer;
//    long lFocusPlsINF;
//    long lFocusPlsMOD;
//    long lFocusOverSearchPlsINF;
//    long lFocusOverSearchPlsMOD;
//    long lFocusPlsFCSDepthCap;
//    long lMinDriveStepMFDriveEndThresh;
//} SDK_FOCUS_POS_CAP, *PSDK_FOCUS_POS_CAP;
typedef  SDK_FOCUS_POS_CAP     SDK_XPRO3_FOCUS_POS_CAP, *PSDK_XPRO3_FOCUS_POS_CAP;
typedef  SDK_XPRO3_FOCUS_POS_CAP     XPRO3_FOCUS_POS_CAP, *PXPRO3_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XPRO3_FOLDER_INFO, *PXPRO3_FOLDER_INFO;


// Crop Area Frame Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lLength_H;
//    long    lLength_V;
//    long    lColorR;
//    long    lColorG;
//    long    lColorB;
//    long    lAlpha;
//} SDK_CropAreaFrameInfo;
typedef  SDK_CropAreaFrameInfo     XPRO3_CropAreaFrameInfo, *PXPRO3_CropAreaFrameInfo;


// Face Frame Information
//typedef struct{
//    long    lID;
//    long    lTime;
//    long    lX;
//    long    lY;
//    long    lLength_H;
//    long    lLength_V;
//    long    lColorR;
//    long    lColorG;
//    long    lColorB;
//    long    lAlpha;
//    long    lType;
//    long    lLikeness;
//    long    lDisp;
//    long    lSelected;
//} SDK_FaceFrameInfo;
typedef  SDK_FaceFrameInfo     XPRO3_FaceFrameInfo, *PXPRO3_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XPRO3_CustomWBArea, *PXPRO3_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XPRO3_FOCUS_LIMITER_INDICATOR, *PXPRO3_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XPRO3_FOCUS_LIMITER, *PXPRO3_FOCUS_LIMITER;


// Image Size
enum{
	XPRO3_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XPRO3_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XPRO3_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XPRO3_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XPRO3_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XPRO3_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XPRO3_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XPRO3_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XPRO3_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XPRO3_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XPRO3_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XPRO3_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XPRO3_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XPRO3_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XPRO3_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XPRO3_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XPRO3_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XPRO3_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XPRO3_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XPRO3_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XPRO3_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XPRO3_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XPRO3_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XPRO3_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XPRO3_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XPRO3_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XPRO3_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XPRO3_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// RAW Image Quality
enum{
	XPRO3_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XPRO3_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Image Quality
enum{
	XPRO3_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XPRO3_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XPRO3_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XPRO3_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XPRO3_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XPRO3_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XPRO3_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XPRO3_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XPRO3_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XPRO3_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XPRO3_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XPRO3_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XPRO3_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XPRO3_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XPRO3_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XPRO3_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XPRO3_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XPRO3_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XPRO3_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XPRO3_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XPRO3_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XPRO3_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XPRO3_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XPRO3_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XPRO3_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XPRO3_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XPRO3_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
};

// Color Space
enum{
	XPRO3_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XPRO3_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XPRO3_WB_AUTO            = SDK_WB_AUTO,  
	XPRO3_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XPRO3_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XPRO3_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XPRO3_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XPRO3_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XPRO3_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XPRO3_WB_SHADE           = SDK_WB_SHADE,
	XPRO3_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XPRO3_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XPRO3_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XPRO3_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XPRO3_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XPRO3_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XPRO3_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XPRO3_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XPRO3_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XPRO3_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XPRO3_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XPRO3_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XPRO3_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XPRO3_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XPRO3_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XPRO3_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XPRO3_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XPRO3_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XPRO3_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XPRO3_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XPRO3_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XPRO3_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XPRO3_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XPRO3_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XPRO3_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XPRO3_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XPRO3_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XPRO3_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XPRO3_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XPRO3_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XPRO3_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XPRO3_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XPRO3_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XPRO3_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XPRO3_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XPRO3_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XPRO3_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XPRO3_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XPRO3_WB_R_SHIFT_STEP    = 1,
	XPRO3_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XPRO3_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XPRO3_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XPRO3_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XPRO3_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XPRO3_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XPRO3_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XPRO3_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XPRO3_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XPRO3_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XPRO3_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XPRO3_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XPRO3_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XPRO3_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XPRO3_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XPRO3_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XPRO3_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XPRO3_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XPRO3_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XPRO3_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XPRO3_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XPRO3_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XPRO3_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XPRO3_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
};

// COLOR
enum{
	XPRO3_COLOR_HIGH          = SDK_COLOR_HIGH,
	XPRO3_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XPRO3_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XPRO3_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XPRO3_COLOR_LOW           = SDK_COLOR_LOW,
	XPRO3_COLOR_P4            = SDK_COLOR_P4,
	XPRO3_COLOR_P3            = SDK_COLOR_P3,
	XPRO3_COLOR_P2            = SDK_COLOR_P2,
	XPRO3_COLOR_P1            = SDK_COLOR_P1,
	XPRO3_COLOR_0             = SDK_COLOR_0,
	XPRO3_COLOR_M1            = SDK_COLOR_M1,
	XPRO3_COLOR_M2            = SDK_COLOR_M2,
	XPRO3_COLOR_M3            = SDK_COLOR_M3,
	XPRO3_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XPRO3_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XPRO3_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XPRO3_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XPRO3_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XPRO3_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XPRO3_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XPRO3_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XPRO3_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XPRO3_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XPRO3_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XPRO3_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XPRO3_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XPRO3_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XPRO3_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XPRO3_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XPRO3_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XPRO3_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XPRO3_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XPRO3_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XPRO3_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XPRO3_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XPRO3_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XPRO3_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XPRO3_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XPRO3_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XPRO3_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XPRO3_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XPRO3_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XPRO3_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XPRO3_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XPRO3_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XPRO3_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XPRO3_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XPRO3_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XPRO3_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XPRO3_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XPRO3_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XPRO3_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XPRO3_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XPRO3_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XPRO3_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XPRO3_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XPRO3_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XPRO3_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XPRO3_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XPRO3_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XPRO3_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XPRO3_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XPRO3_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XPRO3_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XPRO3_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XPRO3_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XPRO3_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XPRO3_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XPRO3_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XPRO3_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XPRO3_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XPRO3_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XPRO3_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XPRO3_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XPRO3_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XPRO3_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XPRO3_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XPRO3_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XPRO3_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XPRO3_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XPRO3_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XPRO3_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XPRO3_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XPRO3_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XPRO3_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XPRO3_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XPRO3_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XPRO3_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XPRO3_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XPRO3_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XPRO3_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XPRO3_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XPRO3_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XPRO3_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XPRO3_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XPRO3_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XPRO3_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XPRO3_SHARPNESS_0               = SDK_SHARPNESS_0,
	XPRO3_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XPRO3_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XPRO3_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XPRO3_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XPRO3_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XPRO3_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XPRO3_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XPRO3_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XPRO3_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XPRO3_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XPRO3_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XPRO3_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XPRO3_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XPRO3_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XPRO3_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XPRO3_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XPRO3_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XPRO3_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XPRO3_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XPRO3_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XPRO3_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XPRO3_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XPRO3_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XPRO3_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XPRO3_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XPRO3_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XPRO3_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XPRO3_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XPRO3_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XPRO3_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XPRO3_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XPRO3_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XPRO3_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XPRO3_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XPRO3_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XPRO3_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XPRO3_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XPRO3_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XPRO3_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XPRO3_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XPRO3_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XPRO3_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XPRO3_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XPRO3_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XPRO3_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XPRO3_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XPRO3_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XPRO3_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XPRO3_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XPRO3_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XPRO3_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
};


// Grain Effect
enum{
	XPRO3_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XPRO3_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XPRO3_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XPRO3_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XPRO3_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XPRO3_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XPRO3_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XPRO3_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XPRO3_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XPRO3_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XPRO3_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XPRO3_CLARITY_P5		= SDK_CLARITY_P5,
	XPRO3_CLARITY_P4		= SDK_CLARITY_P4,
	XPRO3_CLARITY_P3		= SDK_CLARITY_P3,
	XPRO3_CLARITY_P2		= SDK_CLARITY_P2,
	XPRO3_CLARITY_P1		= SDK_CLARITY_P1,
	XPRO3_CLARITY_0			= SDK_CLARITY_0 ,
	XPRO3_CLARITY_M1		= SDK_CLARITY_M1,
	XPRO3_CLARITY_M2		= SDK_CLARITY_M2,
	XPRO3_CLARITY_M3		= SDK_CLARITY_M3,
	XPRO3_CLARITY_M4		= SDK_CLARITY_M4,
	XPRO3_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XPRO3_SHADOWING_0             = SDK_SHADOWING_0,
	XPRO3_SHADOWING_P1            = SDK_SHADOWING_P1,
	XPRO3_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XPRO3_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XPRO3_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XPRO3_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XPRO3_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XPRO3_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XPRO3_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XPRO3_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XPRO3_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XPRO3_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XPRO3_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XPRO3_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XPRO3_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XPRO3_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XPRO3_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XPRO3_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XPRO3_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XPRO3_FOCUS_AFS        = SDK_FOCUS_AFS,
	XPRO3_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XPRO3_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XPRO3_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XPRO3_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XPRO3_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XPRO3_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XPRO3_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XPRO3_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XPRO3_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Focus Points
enum{
	XPRO3_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XPRO3_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XPRO3_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XPRO3_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XPRO3_AF_ALL     = SDK_AF_ALL,
	XPRO3_AF_MULTI   = SDK_AF_MULTI,
	XPRO3_AF_AREA    = SDK_AF_AREA,
	XPRO3_AF_SINGLE  = XPRO3_AF_AREA,
	XPRO3_AF_ZONE    = SDK_AF_ZONE,
	XPRO3_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// Eye AF Mode
enum{
	XPRO3_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XPRO3_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XPRO3_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XPRO3_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XPRO3_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XPRO3_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XPRO3_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XPRO3_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XPRO3_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XPRO3_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XPRO3_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XPRO3_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XPRO3_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XPRO3_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XPRO3_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XPRO3_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XPRO3_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XPRO3_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XPRO3_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XPRO3_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XPRO3_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XPRO3_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
};

// FOCUS AREA
enum{
	XPRO3_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XPRO3_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XPRO3_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XPRO3_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XPRO3_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XPRO3_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XPRO3_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XPRO3_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XPRO3_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XPRO3_MACRO_MODE           = SDK_MACRO_MODE,
	XPRO3_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XPRO3_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XPRO3_DRIVE_MODE_CL   = 0x1000,
	XPRO3_DRIVE_MODE_CH   = 0x10F0,
	XPRO3_DRIVE_MODE_BKT  = 0x4000
};

// USB MODE
enum{
	XPRO3_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XPRO3_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XPRO3_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XPRO3_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XPRO3_BEEP_HIGH        = SDK_BEEP_HIGH,
	XPRO3_BEEP_MID         = SDK_BEEP_MID,
	XPRO3_BEEP_LOW         = SDK_BEEP_LOW,
	XPRO3_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XPRO3_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XPRO3_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XPRO3_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XPRO3_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XPRO3_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XPRO3_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XPRO3_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XPRO3_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XPRO3_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XPRO3_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XPRO3_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XPRO3_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XPRO3_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XPRO3_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XPRO3_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XPRO3_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XPRO3_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XPRO3_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XPRO3_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XPRO3_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XPRO3_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XPRO3_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XPRO3_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XPRO3_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XPRO3_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XPRO3_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XPRO3_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XPRO3_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XPRO3_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XPRO3_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XPRO3_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XPRO3_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XPRO3_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XPRO3_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XPRO3_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XPRO3_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XPRO3_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XPRO3_FRAMEGUIDE_GRID_HD      = 0x0003,
	XPRO3_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XPRO3_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XPRO3_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XPRO3_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XPRO3_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XPRO3_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XPRO3_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XPRO3_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XPRO3_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XPRO3_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XPRO3_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XPRO3_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XPRO3_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XPRO3_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XPRO3_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XPRO3_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XPRO3_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XPRO3_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XPRO3_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XPRO3_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XPRO3_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XPRO3_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XPRO3_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XPRO3_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XPRO3_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XPRO3_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XPRO3_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XPRO3_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XPRO3_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XPRO3_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XPRO3_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XPRO3_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XPRO3_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XPRO3_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XPRO3_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XPRO3_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XPRO3_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XPRO3_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XPRO3_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XPRO3_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XPRO3_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XPRO3_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XPRO3_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XPRO3_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XPRO3_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XPRO3_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XPRO3_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XPRO3_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XPRO3_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XPRO3_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XPRO3_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XPRO3_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XPRO3_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XPRO3_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XPRO3_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XPRO3_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XPRO3_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XPRO3_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XPRO3_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XPRO3_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XPRO3_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XPRO3_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XPRO3_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XPRO3_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XPRO3_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XPRO3_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XPRO3_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XPRO3_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XPRO3_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XPRO3_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XPRO3_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XPRO3_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XPRO3_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XPRO3_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XPRO3_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XPRO3_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XPRO3_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XPRO3_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XPRO3_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XPRO3_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XPRO3_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XPRO3_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XPRO3_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XPRO3_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XPRO3_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XPRO3_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XPRO3_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XPRO3_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XPRO3_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XPRO3_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XPRO3_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XPRO3_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XPRO3_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XPRO3_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XPRO3_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XPRO3_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XPRO3_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XPRO3_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XPRO3_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XPRO3_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XPRO3_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XPRO3_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XPRO3_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XPRO3_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XPRO3_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XPRO3_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XPRO3_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XPRO3_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XPRO3_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XPRO3_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XPRO3_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XPRO3_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XPRO3_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XPRO3_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XPRO3_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XPRO3_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XPRO3_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XPRO3_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XPRO3_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XPRO3_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XPRO3_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XPRO3_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XPRO3_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XPRO3_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XPRO3_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XPRO3_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XPRO3_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XPRO3_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XPRO3_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XPRO3_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XPRO3_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XPRO3_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XPRO3_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XPRO3_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XPRO3_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XPRO3_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XPRO3_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XPRO3_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XPRO3_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XPRO3_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XPRO3_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XPRO3_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XPRO3_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XPRO3_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XPRO3_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XPRO3_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XPRO3_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XPRO3_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XPRO3_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XPRO3_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XPRO3_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XPRO3_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XPRO3_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XPRO3_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XPRO3_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XPRO3_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XPRO3_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XPRO3_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XPRO3_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XPRO3_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XPRO3_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XPRO3_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XPRO3_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XPRO3_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XPRO3_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XPRO3_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XPRO3_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XPRO3_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
};

enum{
	XPRO3_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XPRO3_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XPRO3_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XPRO3_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XPRO3_ON  = SDK_ON,
	XPRO3_OFF = SDK_OFF,
};

enum{
	XPRO3_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XPRO3_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XPRO3_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XPRO3_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XPRO3_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XPRO3_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XPRO3_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XPRO3_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XPRO3_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XPRO3_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XPRO3_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XPRO3_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XPRO3_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XPRO3_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XPRO3_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XPRO3_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XPRO3_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XPRO3_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XPRO3_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XPRO3_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XPRO3_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XPRO3_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XPRO3_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XPRO3_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XPRO3_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XPRO3_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XPRO3_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XPRO3_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XPRO3_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XPRO3_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XPRO3_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XPRO3_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XPRO3_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XPRO3_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XPRO3_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XPRO3_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XPRO3_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XPRO3_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XPRO3_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XPRO3_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XPRO3_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XPRO3_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XPRO3_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XPRO3_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XPRO3_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XPRO3_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XPRO3_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XPRO3_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XPRO3_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XPRO3_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XPRO3_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XPRO3_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XPRO3_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XPRO3_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XPRO3_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XPRO3_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};
