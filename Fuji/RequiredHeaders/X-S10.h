///////////////////////////////////////////////////
//  X-S10.h
//     Version 1.0.0.0
//
//     Copyright (C) 2021 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-S10
//
enum {
	XS10_API_PARAM_SetImageSize                = 1,
	XS10_API_PARAM_GetImageSize                = 1,
	XS10_API_PARAM_SetSharpness                = 1,
	XS10_API_PARAM_GetSharpness                = 1,
	XS10_API_PARAM_SetColorMode                = 1,
	XS10_API_PARAM_GetColorMode                = 1,
	XS10_API_PARAM_SetFilmSimulationMode       = 1,
	XS10_API_PARAM_GetFilmSimulationMode       = 1,
	XS10_API_PARAM_SetColorSpace               = 1,
	XS10_API_PARAM_GetColorSpace               = 1,
	XS10_API_PARAM_SetImageQuality             = 1,
	XS10_API_PARAM_GetImageQuality             = 1,
	XS10_API_PARAM_SetRAWCompression           = 1,
	XS10_API_PARAM_GetRAWCompression           = 1,
	XS10_API_PARAM_SetGrainEffect              = 1,
	XS10_API_PARAM_GetGrainEffect              = 1,
	XS10_API_PARAM_SetShadowing                = 1,
	XS10_API_PARAM_GetShadowing                = 1,
	XS10_API_PARAM_SetNoiseReduction           = 1,
	XS10_API_PARAM_GetNoiseReduction           = 1,
	XS10_API_PARAM_SetFaceDetectionMode        = 1,
	XS10_API_PARAM_GetFaceDetectionMode        = 1,
	XS10_API_PARAM_SetEyeAFMode                = 1,
	XS10_API_PARAM_GetEyeAFMode                = 1,
	XS10_API_PARAM_SetMacroMode                = -1,
	XS10_API_PARAM_GetMacroMode                = -1,
	XS10_API_PARAM_CapFocusLimiter             = 2,
	XS10_API_PARAM_SetFocusLimiter             = 1,
	XS10_API_PARAM_GetFocusLimiter             = 1,
	XS10_API_PARAM_SetHighLightTone            = 1,
	XS10_API_PARAM_GetHighLightTone            = 1,
	XS10_API_PARAM_SetShadowTone               = 1,
	XS10_API_PARAM_GetShadowTone               = 1,
	XS10_API_PARAM_SetLongExposureNR           = 1,
	XS10_API_PARAM_GetLongExposureNR           = 1,
	XS10_API_PARAM_SetFullTimeManualFocus      = 1,
	XS10_API_PARAM_GetFullTimeManualFocus      = 1,
	XS10_API_PARAM_SetFocusMode                = 1,
	XS10_API_PARAM_GetFocusMode                = 1,
	XS10_API_PARAM_SetAFMode                   = 2,
	XS10_API_PARAM_GetAFMode                   = 2,
	XS10_API_PARAM_SetFocusPoints              = 1,
	XS10_API_PARAM_GetFocusPoints              = 1,
	XS10_API_PARAM_SetFocusArea                = 2,
	XS10_API_PARAM_GetFocusArea                = 2,
	XS10_API_PARAM_CapFocusMode                = 2,
	XS10_API_PARAM_GetAFStatus                 = 1,
	XS10_API_PARAM_SetShutterPriorityMode      = 2,
	XS10_API_PARAM_GetShutterPriorityMode      = 2,
	XS10_API_PARAM_SetInstantAFMode            = 1,
	XS10_API_PARAM_GetInstantAFMode            = 1,
	XS10_API_PARAM_SetPreAFMode                = 1,
	XS10_API_PARAM_GetPreAFMode                = 1,
	XS10_API_PARAM_SetAFIlluminator            = 1,
	XS10_API_PARAM_GetAFIlluminator            = 1,
	XS10_API_PARAM_CapLensISSwitch             = 2,
	XS10_API_PARAM_SetLensISSwitch             = 1,
	XS10_API_PARAM_GetLensISSwitch             = 1,
	XS10_API_PARAM_SetISMode                   = 1,
	XS10_API_PARAM_GetISMode                   = 1,
	XS10_API_PARAM_SetLMOMode                  = 1,
	XS10_API_PARAM_GetLMOMode                  = 1,
	XS10_API_PARAM_SetWhiteBalanceMode         = -1,
	XS10_API_PARAM_GetWhiteBalanceMode         = -1,
	XS10_API_PARAM_SetWhiteBalanceTune         = 3,
	XS10_API_PARAM_GetWhiteBalanceTune         = 3,
	XS10_API_PARAM_SetCaptureDelay             = 1,
	XS10_API_PARAM_GetCaptureDelay             = 1,
	XS10_API_PARAM_CapFocusPos                 = 2,
	XS10_API_PARAM_SetFocusPos                 = 1,
	XS10_API_PARAM_GetFocusPos                 = 1,
	XS10_API_PARAM_SetMFAssistMode             = 1,
	XS10_API_PARAM_GetMFAssistMode             = 1,
	XS10_API_PARAM_SetFocusCheckMode           = 1,
	XS10_API_PARAM_GetFocusCheckMode           = 1,
	XS10_API_PARAM_SetInterlockAEAFArea        = 1,
	XS10_API_PARAM_GetInterlockAEAFArea        = 1,
	XS10_API_PARAM_StartLiveView               = 0,
	XS10_API_PARAM_StopLiveView                = 0,
	XS10_API_PARAM_SetLiveViewImageQuality     = 1,
	XS10_API_PARAM_GetLiveViewImageQuality     = 1,
	XS10_API_PARAM_SetLiveViewImageSize        = 1,
	XS10_API_PARAM_GetLiveViewImageSize        = 1,
	XS10_API_PARAM_SetThroughImageZoom         = 1,
	XS10_API_PARAM_GetThroughImageZoom         = 1,
	XS10_API_PARAM_SetDateTime                 = 6,
	XS10_API_PARAM_GetDateTime                 = 6,
	XS10_API_PARAM_SetDateTimeDispFormat       = -1,
	XS10_API_PARAM_GetDateTimeDispFormat       = -1,
	XS10_API_PARAM_SetWorldClock               = -1,
	XS10_API_PARAM_GetWorldClock               = -1,
	XS10_API_PARAM_SetTimeDifference           = -1,
	XS10_API_PARAM_GetTimeDifference           = -1,
	XS10_API_PARAM_ResetSetting                = 1,
	XS10_API_PARAM_SetSilentMode               = -1,
	XS10_API_PARAM_GetSilentMode               = -1,
	XS10_API_PARAM_SetBeep                     = -1,
	XS10_API_PARAM_GetBeep                     = -1,
	XS10_API_PARAM_SetFunctionLock             = 1,
	XS10_API_PARAM_GetFunctionLock             = 1,
	XS10_API_PARAM_SetFunctionLockCategory     = 3,
	XS10_API_PARAM_GetFunctionLockCategory     = 3,
	XS10_API_PARAM_SetComment                  = 1,
	XS10_API_PARAM_GetComment                  = 1,
	XS10_API_PARAM_SetCopyright                = 2,
	XS10_API_PARAM_GetCopyright                = 2,
	XS10_API_PARAM_SetFilenamePrefix           = 2,
	XS10_API_PARAM_GetFilenamePrefix           = 2,
	XS10_API_PARAM_SetFoldernameSuffix         = 3,
	XS10_API_PARAM_GetFoldernameSuffix         = 3,
	XS10_API_PARAM_GetFoldernameList           = 4,
	XS10_API_PARAM_CheckBatteryInfo            = 6,
	XS10_API_PARAM_SetFrameNumberSequence      = -1,
	XS10_API_PARAM_GetFrameNumberSequence      = -1,
	XS10_API_PARAM_SetUSBMode                  = -1,
	XS10_API_PARAM_GetUSBMode                  = -1,
	XS10_API_PARAM_FormatMemoryCard            = 1,
	XS10_API_PARAM_SetMediaRecord              = 1,
	XS10_API_PARAM_GetMediaRecord              = 1,
	XS10_API_PARAM_GetMediaCapacity            = 5,
	XS10_API_PARAM_GetMediaStatus              = 2,
	XS10_API_PARAM_GetShutterCount             = 3,
	XS10_API_PARAM_GetShutterCountEx           = -1,
	XS10_API_PARAM_SetSensorCleanTiming        = -1,
	XS10_API_PARAM_GetSensorCleanTiming        = -1,
	XS10_API_PARAM_SetPreviewTime              = -1,
	XS10_API_PARAM_GetPreviewTime              = -1,
	XS10_API_PARAM_SetEVFDispAutoRotate        = -1,
	XS10_API_PARAM_GetEVFDispAutoRotate        = -1,
	XS10_API_PARAM_SetDispMMode                = 1,
	XS10_API_PARAM_GetDispMMode                = 1,
	XS10_API_PARAM_SetExposurePreview          = 1,
	XS10_API_PARAM_GetExposurePreview          = 1,
	XS10_API_PARAM_SetDispBrightness           = -1,
	XS10_API_PARAM_GetDispBrightness           = -1,
	XS10_API_PARAM_SetFrameGuideMode           = 1,
	XS10_API_PARAM_GetFrameGuideMode           = 1,
	XS10_API_PARAM_SetFrameGuideGridInfo       = 2,
	XS10_API_PARAM_GetFrameGuideGridInfo       = 2,
	XS10_API_PARAM_SetAutoImageRotation        = -1,
	XS10_API_PARAM_GetAutoImageRotation        = -1,
	XS10_API_PARAM_SetFocusScaleUnit           = -1,
	XS10_API_PARAM_GetFocusScaleUnit           = -1,
	XS10_API_PARAM_SetCustomDispInfo           = -1,
	XS10_API_PARAM_GetCustomDispInfo           = -1,
	XS10_API_PARAM_SetViewMode                 = 2,
	XS10_API_PARAM_GetViewMode                 = 2,
	XS10_API_PARAM_SetDispInfoMode             = -1,
	XS10_API_PARAM_GetDispInfoMode             = -1,
	XS10_API_PARAM_SetDispChroma               = -1,
	XS10_API_PARAM_GetDispChroma               = -1,
	XS10_API_PARAM_SetCustomAutoPowerOff       = 1,
	XS10_API_PARAM_GetCustomAutoPowerOff       = 1,
	XS10_API_PARAM_SetCustomStudioPowerSave    = 1,
	XS10_API_PARAM_GetCustomStudioPowerSave    = 1,
	XS10_API_PARAM_SetFunctionButton           = -1,
	XS10_API_PARAM_GetFunctionButton           = -1,
	XS10_API_PARAM_SetISODialHn                = -1,
	XS10_API_PARAM_GetISODialHn                = -1,
	XS10_API_PARAM_SetLockButtonMode           = 1,
	XS10_API_PARAM_GetLockButtonMode           = 1,
	XS10_API_PARAM_SetAFLockMode               = -1,
	XS10_API_PARAM_GetAFLockMode               = -1,
	XS10_API_PARAM_SetMicJackMode              = -1,
	XS10_API_PARAM_GetMicJackMode              = -1,
	XS10_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XS10_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XS10_API_PARAM_SetCrossKeyAssign           = -1,
	XS10_API_PARAM_GetCrossKeyAssign           = -1,
	XS10_API_PARAM_SetWideDynamicRange         = 1,
	XS10_API_PARAM_GetWideDynamicRange         = 1,
	XS10_API_PARAM_SetClarityMode              = 1,
	XS10_API_PARAM_GetClarityMode              = 1,
	XS10_API_PARAM_SetBlackImageTone           = 1,
	XS10_API_PARAM_GetBlackImageTone           = 1,
	XS10_API_PARAM_GetTNumber                  = -1,
	XS10_API_PARAM_SetCropMode                 = 1,
	XS10_API_PARAM_GetCropMode                 = 2,
	XS10_API_PARAM_GetCropAreaFrameInfo        = -1,
	XS10_API_PARAM_SetRAWOutputDepth           = -1,
	XS10_API_PARAM_GetRAWOutputDepth           = -1,
	XS10_API_PARAM_SetSmoothSkinEffect         = 1,
	XS10_API_PARAM_GetSmoothSkinEffect         = 1,
	XS10_API_PARAM_GetDetectedFaceFrame        = 2,
	XS10_API_PARAM_SetDetectedFaceFrame        = 1,
    XS10_API_PARAM_SetCustomWBArea             = 2,
    XS10_API_PARAM_GetCustomWBArea             = 2,
	XS10_API_PARAM_SetColorChromeBlue          = 1,
	XS10_API_PARAM_GetColorChromeBlue          = 1,
	XS10_API_PARAM_SetMonochromaticColor       = 2,
	XS10_API_PARAM_GetMonochromaticColor       = 2,
	XS10_API_PARAM_SetFocusLimiterPos          = 2,
	XS10_API_PARAM_GetFocusLimiterIndicator    = 1,
	XS10_API_PARAM_GetFocusLimiterRange        = 2,
	XS10_API_PARAM_SetFocusLimiterMode         = 1,
	XS10_API_PARAM_GetFocusLimiterMode         = 1,
	XS10_API_PARAM_GetCommandDialStatus        = 4,
	XS10_API_PARAM_SetPerformanceSettings      = 1,
	XS10_API_PARAM_GetPerformanceSettings      = 1,
	XS10_API_PARAM_SetMicLineSetting           = 1,
	XS10_API_PARAM_GetMicLineSetting           = 1,
	XS10_API_PARAM_CapCustomSettingAutoUpdate  = 2,
	XS10_API_PARAM_SetCustomSettingAutoUpdate  = 1,
	XS10_API_PARAM_GetCustomSettingAutoUpdate  = 1,
};

//
// API_CODE for X-S10
//
enum {
	XS10_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XS10_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XS10_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XS10_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XS10_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XS10_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XS10_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XS10_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XS10_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XS10_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XS10_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XS10_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XS10_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XS10_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XS10_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XS10_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XS10_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XS10_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XS10_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XS10_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XS10_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XS10_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XS10_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XS10_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XS10_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XS10_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XS10_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XS10_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XS10_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XS10_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XS10_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XS10_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XS10_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XS10_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XS10_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XS10_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XS10_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XS10_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XS10_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XS10_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XS10_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XS10_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XS10_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XS10_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XS10_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XS10_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XS10_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XS10_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XS10_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XS10_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XS10_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XS10_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XS10_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XS10_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XS10_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XS10_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XS10_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XS10_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XS10_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XS10_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XS10_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XS10_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XS10_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XS10_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XS10_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XS10_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XS10_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XS10_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XS10_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XS10_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XS10_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XS10_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XS10_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XS10_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XS10_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XS10_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XS10_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XS10_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XS10_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XS10_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XS10_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XS10_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XS10_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XS10_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XS10_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XS10_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XS10_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XS10_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XS10_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XS10_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XS10_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XS10_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XS10_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XS10_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XS10_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XS10_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XS10_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XS10_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XS10_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XS10_API_CODE_SetComment                  = API_CODE_SetComment,
	XS10_API_CODE_GetComment                  = API_CODE_GetComment,
	XS10_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XS10_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XS10_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XS10_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XS10_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XS10_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XS10_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XS10_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XS10_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XS10_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XS10_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XS10_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XS10_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XS10_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XS10_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XS10_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XS10_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XS10_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XS10_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XS10_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XS10_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XS10_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XS10_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XS10_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XS10_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XS10_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XS10_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XS10_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XS10_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XS10_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XS10_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XS10_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XS10_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XS10_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XS10_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XS10_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XS10_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XS10_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XS10_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XS10_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XS10_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XS10_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XS10_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XS10_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XS10_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XS10_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XS10_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XS10_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XS10_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XS10_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XS10_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XS10_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XS10_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XS10_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XS10_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XS10_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XS10_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XS10_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XS10_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XS10_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XS10_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XS10_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XS10_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XS10_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XS10_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XS10_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XS10_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XS10_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XS10_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XS10_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XS10_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XS10_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XS10_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XS10_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XS10_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XS10_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XS10_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XS10_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XS10_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XS10_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XS10_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XS10_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XS10_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XS10_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XS10_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XS10_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XS10_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XS10_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XS10_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XS10_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XS10_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XS10_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
	XS10_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
	XS10_API_CODE_SetPerformanceSettings      = API_CODE_SetPerformanceSettings,
	XS10_API_CODE_GetPerformanceSettings      = API_CODE_GetPerformanceSettings,
	XS10_API_CODE_SetMicLineSetting           = API_CODE_SetMicLineSetting,
	XS10_API_CODE_GetMicLineSetting           = API_CODE_GetMicLineSetting,
	XS10_API_CODE_CapCustomSettingAutoUpdate  = API_CODE_CapCustomSettingAutoUpdate,
	XS10_API_CODE_SetCustomSettingAutoUpdate  = API_CODE_SetCustomSettingAutoUpdate,
	XS10_API_CODE_GetCustomSettingAutoUpdate  = API_CODE_GetCustomSettingAutoUpdate,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XS10_FocusArea, *PXS10_FocusArea;


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
typedef  SDK_ISOAuto     XS10_ISOAuto, *PXS10_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XS10_FrameGuideGridInfo, *PXS10_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XS10_FOCUS_POS_CAP, *PSDK_XS10_FOCUS_POS_CAP;
typedef  SDK_XS10_FOCUS_POS_CAP     XS10_FOCUS_POS_CAP, *PXS10_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XS10_FOLDER_INFO, *PXS10_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XS10_CropAreaFrameInfo, *PXS10_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XS10_FaceFrameInfo, *PXS10_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XS10_CustomWBArea, *PXS10_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XS10_FOCUS_LIMITER_INDICATOR, *PXS10_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XS10_FOCUS_LIMITER, *PXS10_FOCUS_LIMITER;


// Image Size
enum{
	XS10_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XS10_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XS10_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XS10_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XS10_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XS10_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XS10_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XS10_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XS10_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XS10_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XS10_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XS10_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XS10_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XS10_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XS10_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XS10_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XS10_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XS10_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XS10_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XS10_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XS10_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XS10_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XS10_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XS10_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XS10_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XS10_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XS10_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XS10_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// RAW Image Quality
enum{
	XS10_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XS10_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Image Quality
enum{
	XS10_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XS10_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XS10_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XS10_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XS10_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XS10_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XS10_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XS10_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XS10_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XS10_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XS10_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XS10_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XS10_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XS10_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XS10_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XS10_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XS10_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XS10_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XS10_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XS10_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XS10_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XS10_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XS10_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XS10_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XS10_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XS10_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XS10_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
};

// Color Space
enum{
	XS10_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XS10_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XS10_WB_AUTO            = SDK_WB_AUTO,  
	XS10_WB_AUTO_WHITE_PRIORITY = SDK_WB_AUTO_WHITE_PRIORITY,
	XS10_WB_AUTO_AMBIENCE_PRIORITY = SDK_WB_AUTO_AMBIENCE_PRIORITY,
	XS10_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XS10_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XS10_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XS10_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XS10_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XS10_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XS10_WB_SHADE           = SDK_WB_SHADE,
	XS10_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XS10_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XS10_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XS10_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XS10_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XS10_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XS10_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XS10_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XS10_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XS10_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XS10_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XS10_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XS10_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XS10_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XS10_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XS10_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XS10_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XS10_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XS10_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XS10_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XS10_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XS10_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XS10_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XS10_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XS10_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XS10_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XS10_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XS10_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XS10_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XS10_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XS10_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XS10_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XS10_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XS10_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XS10_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XS10_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XS10_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XS10_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XS10_WB_R_SHIFT_STEP    = 1,
	XS10_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XS10_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XS10_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XS10_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XS10_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XS10_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XS10_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XS10_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XS10_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XS10_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XS10_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XS10_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XS10_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XS10_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XS10_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XS10_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XS10_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XS10_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XS10_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XS10_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XS10_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XS10_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XS10_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XS10_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
	XS10_FILMSIMULATION_BLEACH_BYPASS   = SDK_FILMSIMULATION_BLEACH_BYPASS,
	XS10_FILMSIMULATION_AUTO            = SDK_FILMSIMULATION_AUTO,
};

// COLOR
enum{
	XS10_COLOR_HIGH          = SDK_COLOR_HIGH,
	XS10_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XS10_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XS10_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XS10_COLOR_LOW           = SDK_COLOR_LOW,
	XS10_COLOR_P4            = SDK_COLOR_P4,
	XS10_COLOR_P3            = SDK_COLOR_P3,
	XS10_COLOR_P2            = SDK_COLOR_P2,
	XS10_COLOR_P1            = SDK_COLOR_P1,
	XS10_COLOR_0             = SDK_COLOR_0,
	XS10_COLOR_M1            = SDK_COLOR_M1,
	XS10_COLOR_M2            = SDK_COLOR_M2,
	XS10_COLOR_M3            = SDK_COLOR_M3,
	XS10_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XS10_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XS10_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XS10_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XS10_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XS10_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XS10_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XS10_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XS10_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XS10_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XS10_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XS10_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XS10_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XS10_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XS10_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XS10_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XS10_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XS10_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XS10_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XS10_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XS10_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XS10_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XS10_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XS10_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XS10_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XS10_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XS10_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XS10_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XS10_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XS10_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XS10_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XS10_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XS10_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XS10_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XS10_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XS10_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XS10_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XS10_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XS10_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XS10_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XS10_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XS10_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XS10_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XS10_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XS10_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XS10_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XS10_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XS10_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XS10_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XS10_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XS10_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XS10_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XS10_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XS10_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XS10_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XS10_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XS10_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XS10_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XS10_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XS10_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XS10_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XS10_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XS10_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XS10_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XS10_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XS10_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XS10_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XS10_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XS10_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XS10_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XS10_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XS10_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XS10_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XS10_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XS10_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XS10_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XS10_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XS10_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XS10_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XS10_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XS10_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XS10_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XS10_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XS10_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XS10_SHARPNESS_0               = SDK_SHARPNESS_0,
	XS10_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XS10_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XS10_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XS10_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XS10_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XS10_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XS10_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XS10_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XS10_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XS10_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XS10_HIGHLIGHT_TONE_P3_5       = SDK_HIGHLIGHT_TONE_P3_5,
	XS10_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XS10_HIGHLIGHT_TONE_P2_5       = SDK_HIGHLIGHT_TONE_P2_5,
	XS10_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XS10_HIGHLIGHT_TONE_P1_5       = SDK_HIGHLIGHT_TONE_P1_5,
	XS10_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XS10_HIGHLIGHT_TONE_P0_5       = SDK_HIGHLIGHT_TONE_P0_5,
	XS10_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XS10_HIGHLIGHT_TONE_M0_5       = SDK_HIGHLIGHT_TONE_M0_5,
	XS10_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XS10_HIGHLIGHT_TONE_M1_5       = SDK_HIGHLIGHT_TONE_M1_5,
	XS10_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XS10_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XS10_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XS10_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XS10_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XS10_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XS10_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XS10_SHADOW_TONE_P3_5          = SDK_SHADOW_TONE_P3_5,
	XS10_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XS10_SHADOW_TONE_P2_5          = SDK_SHADOW_TONE_P2_5,
	XS10_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XS10_SHADOW_TONE_P1_5          = SDK_SHADOW_TONE_P1_5,
	XS10_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XS10_SHADOW_TONE_P0_5          = SDK_SHADOW_TONE_P0_5,
	XS10_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XS10_SHADOW_TONE_M0_5          = SDK_SHADOW_TONE_M0_5,
	XS10_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XS10_SHADOW_TONE_M1_5          = SDK_SHADOW_TONE_M1_5,
	XS10_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XS10_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XS10_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XS10_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XS10_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XS10_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XS10_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XS10_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XS10_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XS10_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XS10_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XS10_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XS10_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XS10_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XS10_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XS10_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XS10_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XS10_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XS10_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XS10_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XS10_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XS10_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XS10_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XS10_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
	XS10_RAW_COMPRESSION_LOSSY		 = SDK_RAW_COMPRESSION_LOSSY,
};

// Grain Effect
enum{
	XS10_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XS10_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XS10_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XS10_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XS10_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XS10_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XS10_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XS10_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XS10_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XS10_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XS10_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XS10_CLARITY_P5		= SDK_CLARITY_P5,
	XS10_CLARITY_P4		= SDK_CLARITY_P4,
	XS10_CLARITY_P3		= SDK_CLARITY_P3,
	XS10_CLARITY_P2		= SDK_CLARITY_P2,
	XS10_CLARITY_P1		= SDK_CLARITY_P1,
	XS10_CLARITY_0			= SDK_CLARITY_0 ,
	XS10_CLARITY_M1		= SDK_CLARITY_M1,
	XS10_CLARITY_M2		= SDK_CLARITY_M2,
	XS10_CLARITY_M3		= SDK_CLARITY_M3,
	XS10_CLARITY_M4		= SDK_CLARITY_M4,
	XS10_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XS10_SHADOWING_0             = SDK_SHADOWING_0,
	XS10_SHADOWING_P1            = SDK_SHADOWING_P1,
	XS10_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XS10_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XS10_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XS10_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XS10_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XS10_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XS10_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XS10_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XS10_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XS10_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XS10_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XS10_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XS10_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XS10_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XS10_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XS10_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XS10_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XS10_FOCUS_AFS        = SDK_FOCUS_AFS,
	XS10_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XS10_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XS10_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XS10_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XS10_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XS10_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XS10_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XS10_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XS10_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Focus Points
enum{
	XS10_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XS10_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XS10_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XS10_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XS10_AF_ALL     = SDK_AF_ALL,
	XS10_AF_MULTI   = SDK_AF_MULTI,
	XS10_AF_AREA    = SDK_AF_AREA,
	XS10_AF_SINGLE  = XS10_AF_AREA,
	XS10_AF_ZONE    = SDK_AF_ZONE,
	XS10_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XS10_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XS10_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XS10_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XS10_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XS10_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XS10_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XS10_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XS10_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XS10_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XS10_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XS10_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XS10_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XS10_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XS10_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XS10_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XS10_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XS10_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XS10_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XS10_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XS10_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XS10_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XS10_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XS10_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XS10_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XS10_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XS10_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
};

// FOCUS AREA
enum{
	XS10_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XS10_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XS10_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XS10_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XS10_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XS10_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XS10_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XS10_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XS10_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XS10_MACRO_MODE           = SDK_MACRO_MODE,
	XS10_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XS10_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XS10_DRIVE_MODE_CL   = 0x1000,
	XS10_DRIVE_MODE_CH   = 0x10F0,
	XS10_DRIVE_MODE_BKT  = 0x4000,
	XS10_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE,
};

// USB MODE
enum{
	XS10_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XS10_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XS10_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XS10_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XS10_BEEP_HIGH        = SDK_BEEP_HIGH,
	XS10_BEEP_MID         = SDK_BEEP_MID,
	XS10_BEEP_LOW         = SDK_BEEP_LOW,
	XS10_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XS10_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XS10_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XS10_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XS10_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XS10_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XS10_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XS10_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XS10_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XS10_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XS10_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XS10_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XS10_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XS10_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XS10_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XS10_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XS10_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XS10_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XS10_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XS10_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XS10_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XS10_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XS10_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XS10_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XS10_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XS10_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XS10_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XS10_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XS10_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XS10_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XS10_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XS10_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XS10_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XS10_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XS10_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XS10_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XS10_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XS10_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XS10_FRAMEGUIDE_GRID_HD      = 0x0003,
	XS10_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XS10_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XS10_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XS10_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XS10_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XS10_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XS10_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XS10_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XS10_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XS10_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XS10_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XS10_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XS10_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XS10_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XS10_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XS10_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XS10_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XS10_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XS10_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XS10_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XS10_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XS10_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XS10_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XS10_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XS10_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XS10_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XS10_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XS10_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XS10_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XS10_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XS10_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XS10_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XS10_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XS10_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XS10_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XS10_IS_MODE_CONTINUOUS_MOTION    = SDK_IS_MODE_CONTINUOUS_MOTION,
	XS10_IS_MODE_SHOOT_MOTION         = SDK_IS_MODE_SHOOT_MOTION,
	XS10_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XS10_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XS10_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XS10_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XS10_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XS10_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XS10_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XS10_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XS10_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XS10_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XS10_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XS10_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XS10_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XS10_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XS10_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XS10_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XS10_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XS10_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XS10_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XS10_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XS10_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XS10_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XS10_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XS10_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XS10_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XS10_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XS10_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XS10_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XS10_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XS10_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XS10_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XS10_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XS10_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XS10_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XS10_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XS10_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XS10_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XS10_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XS10_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XS10_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XS10_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XS10_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XS10_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XS10_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XS10_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XS10_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XS10_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XS10_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XS10_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XS10_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XS10_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XS10_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XS10_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XS10_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XS10_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XS10_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XS10_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XS10_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XS10_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XS10_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XS10_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XS10_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XS10_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XS10_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XS10_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XS10_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XS10_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XS10_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XS10_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XS10_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XS10_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XS10_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XS10_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XS10_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XS10_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XS10_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XS10_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XS10_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XS10_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XS10_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XS10_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XS10_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XS10_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XS10_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XS10_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XS10_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XS10_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XS10_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XS10_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XS10_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XS10_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XS10_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XS10_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XS10_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XS10_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XS10_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XS10_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XS10_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XS10_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XS10_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XS10_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XS10_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XS10_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XS10_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XS10_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XS10_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XS10_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XS10_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XS10_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XS10_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XS10_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XS10_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XS10_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XS10_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XS10_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XS10_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XS10_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XS10_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XS10_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XS10_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XS10_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XS10_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XS10_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XS10_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XS10_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XS10_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XS10_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XS10_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XS10_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER  = SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER,
	XS10_FUNCTIONLOCK_CATEGORY2_MODEDIAL             = SDK_FUNCTIONLOCK_CATEGORY2_MODEDIAL,
	XS10_FUNCTIONLOCK_CATEGORY2_FDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_FDIAL,
	XS10_FUNCTIONLOCK_CATEGORY2_FN_DIAL              = SDK_FUNCTIONLOCK_CATEGORY2_FN_DIAL,
	XS10_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT        = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT,
	XS10_FUNCTIONLOCK_CATEGORY3_ISOBUTTON            = SDK_FUNCTIONLOCK_CATEGORY3_ISOBUTTON,
	XS10_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE      = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE,
	XS10_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE         = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE,
	XS10_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU      = SDK_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU,
	XS10_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE         = SDK_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE,
	XS10_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XS10_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XS10_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XS10_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XS10_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XS10_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XS10_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XS10_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XS10_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XS10_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XS10_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XS10_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XS10_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XS10_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XS10_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XS10_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XS10_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XS10_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XS10_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XS10_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XS10_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XS10_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XS10_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XS10_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	XS10_MEDIASTATUS_ACCESSING             = SDK_MEDIASTATUS_ACCESSING,
};

enum{
	XS10_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XS10_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XS10_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XS10_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XS10_ON  = SDK_ON,
	XS10_OFF = SDK_OFF,
};

enum{
	XS10_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XS10_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XS10_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XS10_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XS10_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XS10_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XS10_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XS10_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XS10_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XS10_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XS10_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XS10_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XS10_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XS10_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XS10_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XS10_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XS10_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XS10_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XS10_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XS10_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XS10_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XS10_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XS10_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XS10_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XS10_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XS10_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XS10_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XS10_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XS10_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XS10_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XS10_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XS10_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XS10_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XS10_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XS10_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XS10_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XS10_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XS10_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XS10_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XS10_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XS10_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XS10_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XS10_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XS10_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XS10_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XS10_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XS10_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XS10_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XS10_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XS10_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XS10_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XS10_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XS10_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XS10_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XS10_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XS10_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};

// Micline Setting
enum{
	XS10_MICLINE_SETTING_MIC          = SDK_MICLINE_SETTING_MIC,
	XS10_MICLINE_SETTING_LINE         = SDK_MICLINE_SETTING_LINE,
};

// Shuttercount Type
enum{
	XS10_SHUTTERCOUNT_TYPE_FRONTCURTAIN = SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN,
	XS10_SHUTTERCOUNT_TYPE_REARCURTAIN  = SDK_SHUTTERCOUNT_TYPE_REARCURTAIN,
	XS10_SHUTTERCOUNT_TYPE_TOTAL        = SDK_SHUTTERCOUNT_TYPE_TOTAL,
};

// Performance
enum{
	XS10_PERFORMANCE_NORMAL                    = SDK_PERFORMANCE_NORMAL,
	XS10_PERFORMANCE_ECONOMY                   = SDK_PERFORMANCE_ECONOMY,
	XS10_PERFORMANCE_BOOST_LOWLIGHT            = SDK_PERFORMANCE_BOOST_LOWLIGHT,
	XS10_PERFORMANCE_BOOST_RESOLUTION_PRIORITY = SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY,
	XS10_PERFORMANCE_BOOST_FRAMERATE_PRIORITY  = SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY,
};
