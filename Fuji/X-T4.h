///////////////////////////////////////////////////
//  X-T4.h
//     Version 1.0.0.0
//
//     Copyright (C) 2020 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-T4
//
enum {
	XT4_API_PARAM_SetImageSize                = 1,
	XT4_API_PARAM_GetImageSize                = 1,
	XT4_API_PARAM_SetSharpness                = 1,
	XT4_API_PARAM_GetSharpness                = 1,
	XT4_API_PARAM_SetColorMode                = 1,
	XT4_API_PARAM_GetColorMode                = 1,
	XT4_API_PARAM_SetFilmSimulationMode       = 1,
	XT4_API_PARAM_GetFilmSimulationMode       = 1,
	XT4_API_PARAM_SetColorSpace               = 1,
	XT4_API_PARAM_GetColorSpace               = 1,
	XT4_API_PARAM_SetImageQuality             = 1,
	XT4_API_PARAM_GetImageQuality             = 1,
	XT4_API_PARAM_SetRAWCompression           = 1,
	XT4_API_PARAM_GetRAWCompression           = 1,
	XT4_API_PARAM_SetGrainEffect              = 1,
	XT4_API_PARAM_GetGrainEffect              = 1,
	XT4_API_PARAM_SetShadowing                = 1,
	XT4_API_PARAM_GetShadowing                = 1,
	XT4_API_PARAM_SetNoiseReduction           = 1,
	XT4_API_PARAM_GetNoiseReduction           = 1,
	XT4_API_PARAM_SetFaceDetectionMode        = 1,
	XT4_API_PARAM_GetFaceDetectionMode        = 1,
	XT4_API_PARAM_SetEyeAFMode                = 1,
	XT4_API_PARAM_GetEyeAFMode                = 1,
	XT4_API_PARAM_SetMacroMode                = -1,
	XT4_API_PARAM_GetMacroMode                = -1,
	XT4_API_PARAM_CapFocusLimiter             = 2,
	XT4_API_PARAM_SetFocusLimiter             = 1,
	XT4_API_PARAM_GetFocusLimiter             = 1,
	XT4_API_PARAM_SetHighLightTone            = 1,
	XT4_API_PARAM_GetHighLightTone            = 1,
	XT4_API_PARAM_SetShadowTone               = 1,
	XT4_API_PARAM_GetShadowTone               = 1,
	XT4_API_PARAM_SetLongExposureNR           = 1,
	XT4_API_PARAM_GetLongExposureNR           = 1,
	XT4_API_PARAM_SetFullTimeManualFocus      = 1,
	XT4_API_PARAM_GetFullTimeManualFocus      = 1,
	XT4_API_PARAM_SetFocusMode                = 1,
	XT4_API_PARAM_GetFocusMode                = 1,
	XT4_API_PARAM_SetAFMode                   = 2,
	XT4_API_PARAM_GetAFMode                   = 2,
	XT4_API_PARAM_SetFocusPoints              = 1,
	XT4_API_PARAM_GetFocusPoints              = 1,
	XT4_API_PARAM_SetFocusArea                = 2,
	XT4_API_PARAM_GetFocusArea                = 2,
	XT4_API_PARAM_CapFocusMode                = 2,
	XT4_API_PARAM_GetAFStatus                 = 1,
	XT4_API_PARAM_SetShutterPriorityMode      = 2,
	XT4_API_PARAM_GetShutterPriorityMode      = 2,
	XT4_API_PARAM_SetInstantAFMode            = 1,
	XT4_API_PARAM_GetInstantAFMode            = 1,
	XT4_API_PARAM_SetPreAFMode                = 1,
	XT4_API_PARAM_GetPreAFMode                = 1,
	XT4_API_PARAM_SetAFIlluminator            = 1,
	XT4_API_PARAM_GetAFIlluminator            = 1,
	XT4_API_PARAM_CapLensISSwitch             = 2,
	XT4_API_PARAM_SetLensISSwitch             = 1,
	XT4_API_PARAM_GetLensISSwitch             = 1,
	XT4_API_PARAM_SetISMode                   = 1,
	XT4_API_PARAM_GetISMode                   = 1,
	XT4_API_PARAM_SetLMOMode                  = 1,
	XT4_API_PARAM_GetLMOMode                  = 1,
	XT4_API_PARAM_SetWhiteBalanceMode         = -1,
	XT4_API_PARAM_GetWhiteBalanceMode         = -1,
	XT4_API_PARAM_SetWhiteBalanceTune         = 3,
	XT4_API_PARAM_GetWhiteBalanceTune         = 3,
	XT4_API_PARAM_SetCaptureDelay             = 1,
	XT4_API_PARAM_GetCaptureDelay             = 1,
	XT4_API_PARAM_CapFocusPos                 = 2,
	XT4_API_PARAM_SetFocusPos                 = 1,
	XT4_API_PARAM_GetFocusPos                 = 1,
	XT4_API_PARAM_SetMFAssistMode             = 1,
	XT4_API_PARAM_GetMFAssistMode             = 1,
	XT4_API_PARAM_SetFocusCheckMode           = 1,
	XT4_API_PARAM_GetFocusCheckMode           = 1,
	XT4_API_PARAM_SetInterlockAEAFArea        = 1,
	XT4_API_PARAM_GetInterlockAEAFArea        = 1,
	XT4_API_PARAM_StartLiveView               = 0,
	XT4_API_PARAM_StopLiveView                = 0,
	XT4_API_PARAM_SetLiveViewImageQuality     = 1,
	XT4_API_PARAM_GetLiveViewImageQuality     = 1,
	XT4_API_PARAM_SetLiveViewImageSize        = 1,
	XT4_API_PARAM_GetLiveViewImageSize        = 1,
	XT4_API_PARAM_SetThroughImageZoom         = 1,
	XT4_API_PARAM_GetThroughImageZoom         = 1,
	XT4_API_PARAM_SetDateTime                 = 6,
	XT4_API_PARAM_GetDateTime                 = 6,
	XT4_API_PARAM_SetDateTimeDispFormat       = -1,
	XT4_API_PARAM_GetDateTimeDispFormat       = -1,
	XT4_API_PARAM_SetWorldClock               = -1,
	XT4_API_PARAM_GetWorldClock               = -1,
	XT4_API_PARAM_SetTimeDifference           = -1,
	XT4_API_PARAM_GetTimeDifference           = -1,
	XT4_API_PARAM_ResetSetting                = 1,
	XT4_API_PARAM_SetSilentMode               = -1,
	XT4_API_PARAM_GetSilentMode               = -1,
	XT4_API_PARAM_SetBeep                     = -1,
	XT4_API_PARAM_GetBeep                     = -1,
	XT4_API_PARAM_SetFunctionLock             = 1,
	XT4_API_PARAM_GetFunctionLock             = 1,
	XT4_API_PARAM_SetFunctionLockCategory     = 2,
	XT4_API_PARAM_GetFunctionLockCategory     = 2,
	XT4_API_PARAM_SetComment                  = 1,
	XT4_API_PARAM_GetComment                  = 1,
	XT4_API_PARAM_SetCopyright                = 2,
	XT4_API_PARAM_GetCopyright                = 2,
	XT4_API_PARAM_SetFilenamePrefix           = 2,
	XT4_API_PARAM_GetFilenamePrefix           = 2,
	XT4_API_PARAM_SetFoldernameSuffix         = 3,
	XT4_API_PARAM_GetFoldernameSuffix         = 3,
	XT4_API_PARAM_GetFoldernameList           = 4,
	XT4_API_PARAM_CheckBatteryInfo            = 6,
	XT4_API_PARAM_SetFrameNumberSequence      = -1,
	XT4_API_PARAM_GetFrameNumberSequence      = -1,
	XT4_API_PARAM_SetUSBMode                  = -1,
	XT4_API_PARAM_GetUSBMode                  = -1,
	XT4_API_PARAM_FormatMemoryCard            = 1,
	XT4_API_PARAM_SetMediaRecord              = 1,
	XT4_API_PARAM_GetMediaRecord              = 1,
	XT4_API_PARAM_GetMediaCapacity            = 5,
	XT4_API_PARAM_GetMediaStatus              = 2,
	XT4_API_PARAM_GetShutterCount             = 3,
	XT4_API_PARAM_GetShutterCountEx           = -1,
	XT4_API_PARAM_SetSensorCleanTiming        = -1,
	XT4_API_PARAM_GetSensorCleanTiming        = -1,
	XT4_API_PARAM_SetPreviewTime              = -1,
	XT4_API_PARAM_GetPreviewTime              = -1,
	XT4_API_PARAM_SetEVFDispAutoRotate        = -1,
	XT4_API_PARAM_GetEVFDispAutoRotate        = -1,
	XT4_API_PARAM_SetDispMMode                = 1,
	XT4_API_PARAM_GetDispMMode                = 1,
	XT4_API_PARAM_SetExposurePreview          = 1,
	XT4_API_PARAM_GetExposurePreview          = 1,
	XT4_API_PARAM_SetDispBrightness           = -1,
	XT4_API_PARAM_GetDispBrightness           = -1,
	XT4_API_PARAM_SetFrameGuideMode           = 1,
	XT4_API_PARAM_GetFrameGuideMode           = 1,
	XT4_API_PARAM_SetFrameGuideGridInfo       = 2,
	XT4_API_PARAM_GetFrameGuideGridInfo       = 2,
	XT4_API_PARAM_SetAutoImageRotation        = -1,
	XT4_API_PARAM_GetAutoImageRotation        = -1,
	XT4_API_PARAM_SetFocusScaleUnit           = -1,
	XT4_API_PARAM_GetFocusScaleUnit           = -1,
	XT4_API_PARAM_SetCustomDispInfo           = -1,
	XT4_API_PARAM_GetCustomDispInfo           = -1,
	XT4_API_PARAM_SetViewMode                 = 2,
	XT4_API_PARAM_GetViewMode                 = 2,
	XT4_API_PARAM_SetDispInfoMode             = -1,
	XT4_API_PARAM_GetDispInfoMode             = -1,
	XT4_API_PARAM_SetDispChroma               = -1,
	XT4_API_PARAM_GetDispChroma               = -1,
	XT4_API_PARAM_SetCustomAutoPowerOff       = 1,
	XT4_API_PARAM_GetCustomAutoPowerOff       = 1,
	XT4_API_PARAM_SetCustomStudioPowerSave    = 1,
	XT4_API_PARAM_GetCustomStudioPowerSave    = 1,
	XT4_API_PARAM_SetFunctionButton           = -1,
	XT4_API_PARAM_GetFunctionButton           = -1,
	XT4_API_PARAM_SetISODialHn                = -1,
	XT4_API_PARAM_GetISODialHn                = -1,
	XT4_API_PARAM_SetLockButtonMode           = 1,
	XT4_API_PARAM_GetLockButtonMode           = 1,
	XT4_API_PARAM_SetAFLockMode               = -1,
	XT4_API_PARAM_GetAFLockMode               = -1,
	XT4_API_PARAM_SetMicJackMode              = -1,
	XT4_API_PARAM_GetMicJackMode              = -1,
	XT4_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XT4_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XT4_API_PARAM_SetCrossKeyAssign           = -1,
	XT4_API_PARAM_GetCrossKeyAssign           = -1,
	XT4_API_PARAM_SetWideDynamicRange         = 1,
	XT4_API_PARAM_GetWideDynamicRange         = 1,
	XT4_API_PARAM_SetClarityMode              = 1,
	XT4_API_PARAM_GetClarityMode              = 1,
	XT4_API_PARAM_SetBlackImageTone           = 1,
	XT4_API_PARAM_GetBlackImageTone           = 1,
	XT4_API_PARAM_GetTNumber                  = -1,
	XT4_API_PARAM_SetCropMode                 = 1,
	XT4_API_PARAM_GetCropMode                 = 2,
	XT4_API_PARAM_GetCropAreaFrameInfo        = -1,
	XT4_API_PARAM_SetRAWOutputDepth           = -1,
	XT4_API_PARAM_GetRAWOutputDepth           = -1,
	XT4_API_PARAM_SetSmoothSkinEffect         = 1,
	XT4_API_PARAM_GetSmoothSkinEffect         = 1,
	XT4_API_PARAM_GetDetectedFaceFrame        = 2,
	XT4_API_PARAM_SetDetectedFaceFrame        = 1,
    XT4_API_PARAM_SetCustomWBArea             = 2,
    XT4_API_PARAM_GetCustomWBArea             = 2,
	XT4_API_PARAM_SetColorChromeBlue          = 1,
	XT4_API_PARAM_GetColorChromeBlue          = 1,
	XT4_API_PARAM_SetMonochromaticColor       = 2,
	XT4_API_PARAM_GetMonochromaticColor       = 2,
	XT4_API_PARAM_SetFocusLimiterPos          = 2,
	XT4_API_PARAM_GetFocusLimiterIndicator    = 1,
	XT4_API_PARAM_GetFocusLimiterRange        = 2,
	XT4_API_PARAM_SetFocusLimiterMode         = 1,
	XT4_API_PARAM_GetFocusLimiterMode         = 1,
	XT4_API_PARAM_GetCommandDialStatus        = 4,
	XT4_API_PARAM_SetPerformanceSettings      = 1,
	XT4_API_PARAM_GetPerformanceSettings      = 1,
	XT4_API_PARAM_SetMicLineSetting           = 1,
	XT4_API_PARAM_GetMicLineSetting           = 1,
};

//
// API_CODE for X-T4
//
enum {
	XT4_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XT4_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XT4_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XT4_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XT4_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XT4_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XT4_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XT4_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XT4_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XT4_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XT4_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XT4_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XT4_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XT4_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XT4_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XT4_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XT4_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XT4_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XT4_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XT4_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XT4_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XT4_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XT4_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XT4_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XT4_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XT4_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XT4_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XT4_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XT4_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XT4_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XT4_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XT4_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XT4_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XT4_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XT4_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XT4_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XT4_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XT4_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XT4_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XT4_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XT4_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XT4_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XT4_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XT4_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XT4_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XT4_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XT4_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XT4_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XT4_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XT4_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XT4_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XT4_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XT4_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XT4_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XT4_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XT4_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XT4_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XT4_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XT4_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XT4_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XT4_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XT4_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XT4_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XT4_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XT4_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XT4_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XT4_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XT4_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XT4_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XT4_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XT4_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XT4_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XT4_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XT4_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XT4_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XT4_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XT4_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XT4_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XT4_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XT4_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XT4_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XT4_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XT4_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XT4_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XT4_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XT4_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XT4_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XT4_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XT4_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XT4_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XT4_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XT4_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XT4_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XT4_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XT4_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XT4_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XT4_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XT4_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XT4_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XT4_API_CODE_SetComment                  = API_CODE_SetComment,
	XT4_API_CODE_GetComment                  = API_CODE_GetComment,
	XT4_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XT4_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XT4_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XT4_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XT4_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XT4_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XT4_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XT4_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XT4_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XT4_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XT4_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XT4_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XT4_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XT4_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XT4_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XT4_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XT4_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XT4_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XT4_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XT4_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XT4_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XT4_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XT4_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XT4_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XT4_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XT4_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XT4_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XT4_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XT4_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XT4_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XT4_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XT4_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XT4_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XT4_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XT4_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XT4_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XT4_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XT4_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XT4_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XT4_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XT4_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XT4_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XT4_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XT4_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XT4_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XT4_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XT4_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XT4_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XT4_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XT4_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XT4_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XT4_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XT4_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XT4_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XT4_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XT4_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XT4_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XT4_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XT4_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XT4_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XT4_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XT4_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XT4_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XT4_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XT4_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XT4_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XT4_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XT4_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XT4_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XT4_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XT4_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XT4_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XT4_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XT4_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XT4_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XT4_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XT4_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XT4_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XT4_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XT4_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XT4_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XT4_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XT4_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XT4_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XT4_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XT4_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XT4_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XT4_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XT4_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XT4_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XT4_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XT4_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
	XT4_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
	XT4_API_CODE_SetPerformanceSettings      = API_CODE_SetPerformanceSettings,
	XT4_API_CODE_GetPerformanceSettings      = API_CODE_GetPerformanceSettings,
	XT4_API_CODE_SetMicLineSetting           = API_CODE_SetMicLineSetting,
	XT4_API_CODE_GetMicLineSetting           = API_CODE_GetMicLineSetting,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XT4_FocusArea, *PXT4_FocusArea;


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
typedef  SDK_ISOAuto     XT4_ISOAuto, *PXT4_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XT4_FrameGuideGridInfo, *PXT4_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XT4_FOCUS_POS_CAP, *PSDK_XT4_FOCUS_POS_CAP;
typedef  SDK_XT4_FOCUS_POS_CAP     XT4_FOCUS_POS_CAP, *PXT4_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XT4_FOLDER_INFO, *PXT4_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XT4_CropAreaFrameInfo, *PXT4_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XT4_FaceFrameInfo, *PXT4_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XT4_CustomWBArea, *PXT4_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XT4_FOCUS_LIMITER_INDICATOR, *PXT4_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XT4_FOCUS_LIMITER, *PXT4_FOCUS_LIMITER;


// Image Size
enum{
	XT4_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XT4_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XT4_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XT4_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XT4_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XT4_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XT4_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XT4_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XT4_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XT4_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XT4_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XT4_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XT4_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XT4_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XT4_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XT4_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XT4_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XT4_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XT4_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XT4_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XT4_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XT4_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XT4_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XT4_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XT4_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XT4_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XT4_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XT4_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// RAW Image Quality
enum{
	XT4_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XT4_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Image Quality
enum{
	XT4_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XT4_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XT4_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XT4_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XT4_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XT4_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XT4_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XT4_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XT4_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XT4_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XT4_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XT4_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XT4_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XT4_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XT4_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XT4_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XT4_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XT4_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XT4_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XT4_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XT4_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XT4_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XT4_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XT4_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XT4_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XT4_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XT4_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
};

// Color Space
enum{
	XT4_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XT4_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XT4_WB_AUTO            = SDK_WB_AUTO,  
	XT4_WB_AUTO_WHITE_PRIORITY = SDK_WB_AUTO_WHITE_PRIORITY,
	XT4_WB_AUTO_AMBIENCE_PRIORITY = SDK_WB_AUTO_AMBIENCE_PRIORITY,
	XT4_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XT4_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XT4_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XT4_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XT4_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XT4_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XT4_WB_SHADE           = SDK_WB_SHADE,
	XT4_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XT4_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XT4_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XT4_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XT4_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XT4_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XT4_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XT4_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XT4_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XT4_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XT4_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XT4_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XT4_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XT4_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XT4_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XT4_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XT4_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XT4_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XT4_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XT4_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XT4_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XT4_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XT4_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XT4_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XT4_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XT4_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XT4_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XT4_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XT4_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XT4_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XT4_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XT4_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XT4_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XT4_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XT4_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XT4_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XT4_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XT4_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XT4_WB_R_SHIFT_STEP    = 1,
	XT4_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XT4_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XT4_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XT4_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XT4_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XT4_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XT4_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XT4_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XT4_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XT4_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XT4_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XT4_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XT4_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XT4_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XT4_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XT4_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XT4_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XT4_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XT4_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XT4_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XT4_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XT4_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XT4_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XT4_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
	XT4_FILMSIMULATION_BLEACH_BYPASS   = SDK_FILMSIMULATION_BLEACH_BYPASS,
};

// COLOR
enum{
	XT4_COLOR_HIGH          = SDK_COLOR_HIGH,
	XT4_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XT4_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XT4_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XT4_COLOR_LOW           = SDK_COLOR_LOW,
	XT4_COLOR_P4            = SDK_COLOR_P4,
	XT4_COLOR_P3            = SDK_COLOR_P3,
	XT4_COLOR_P2            = SDK_COLOR_P2,
	XT4_COLOR_P1            = SDK_COLOR_P1,
	XT4_COLOR_0             = SDK_COLOR_0,
	XT4_COLOR_M1            = SDK_COLOR_M1,
	XT4_COLOR_M2            = SDK_COLOR_M2,
	XT4_COLOR_M3            = SDK_COLOR_M3,
	XT4_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XT4_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XT4_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XT4_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XT4_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XT4_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XT4_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XT4_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XT4_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XT4_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XT4_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XT4_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XT4_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XT4_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XT4_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XT4_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XT4_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XT4_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XT4_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XT4_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XT4_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XT4_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XT4_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XT4_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XT4_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XT4_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XT4_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XT4_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XT4_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XT4_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XT4_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XT4_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XT4_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XT4_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XT4_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XT4_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XT4_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XT4_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XT4_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XT4_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XT4_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XT4_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XT4_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XT4_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XT4_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XT4_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XT4_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XT4_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XT4_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XT4_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XT4_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XT4_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XT4_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XT4_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XT4_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XT4_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XT4_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XT4_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XT4_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XT4_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XT4_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XT4_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XT4_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XT4_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XT4_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XT4_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XT4_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XT4_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XT4_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XT4_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XT4_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XT4_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XT4_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XT4_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XT4_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XT4_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XT4_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XT4_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XT4_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XT4_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XT4_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XT4_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XT4_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XT4_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XT4_SHARPNESS_0               = SDK_SHARPNESS_0,
	XT4_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XT4_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XT4_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XT4_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XT4_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XT4_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XT4_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XT4_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XT4_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XT4_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XT4_HIGHLIGHT_TONE_P3_5       = SDK_HIGHLIGHT_TONE_P3_5,
	XT4_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XT4_HIGHLIGHT_TONE_P2_5       = SDK_HIGHLIGHT_TONE_P2_5,
	XT4_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XT4_HIGHLIGHT_TONE_P1_5       = SDK_HIGHLIGHT_TONE_P1_5,
	XT4_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XT4_HIGHLIGHT_TONE_P0_5       = SDK_HIGHLIGHT_TONE_P0_5,
	XT4_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XT4_HIGHLIGHT_TONE_M0_5       = SDK_HIGHLIGHT_TONE_M0_5,
	XT4_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XT4_HIGHLIGHT_TONE_M1_5       = SDK_HIGHLIGHT_TONE_M1_5,
	XT4_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XT4_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XT4_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XT4_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XT4_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XT4_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XT4_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XT4_SHADOW_TONE_P3_5          = SDK_SHADOW_TONE_P3_5,
	XT4_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XT4_SHADOW_TONE_P2_5          = SDK_SHADOW_TONE_P2_5,
	XT4_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XT4_SHADOW_TONE_P1_5          = SDK_SHADOW_TONE_P1_5,
	XT4_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XT4_SHADOW_TONE_P0_5          = SDK_SHADOW_TONE_P0_5,
	XT4_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XT4_SHADOW_TONE_M0_5          = SDK_SHADOW_TONE_M0_5,
	XT4_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XT4_SHADOW_TONE_M1_5          = SDK_SHADOW_TONE_M1_5,
	XT4_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XT4_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XT4_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XT4_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XT4_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XT4_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XT4_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XT4_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XT4_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XT4_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XT4_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XT4_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XT4_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XT4_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XT4_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XT4_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XT4_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XT4_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XT4_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XT4_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XT4_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XT4_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XT4_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XT4_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
	XT4_RAW_COMPRESSION_LOSSY		 = SDK_RAW_COMPRESSION_LOSSY,
};

// Grain Effect
enum{
	XT4_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XT4_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XT4_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XT4_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XT4_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XT4_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XT4_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XT4_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XT4_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XT4_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XT4_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XT4_CLARITY_P5		= SDK_CLARITY_P5,
	XT4_CLARITY_P4		= SDK_CLARITY_P4,
	XT4_CLARITY_P3		= SDK_CLARITY_P3,
	XT4_CLARITY_P2		= SDK_CLARITY_P2,
	XT4_CLARITY_P1		= SDK_CLARITY_P1,
	XT4_CLARITY_0			= SDK_CLARITY_0 ,
	XT4_CLARITY_M1		= SDK_CLARITY_M1,
	XT4_CLARITY_M2		= SDK_CLARITY_M2,
	XT4_CLARITY_M3		= SDK_CLARITY_M3,
	XT4_CLARITY_M4		= SDK_CLARITY_M4,
	XT4_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XT4_SHADOWING_0             = SDK_SHADOWING_0,
	XT4_SHADOWING_P1            = SDK_SHADOWING_P1,
	XT4_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XT4_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XT4_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XT4_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XT4_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XT4_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XT4_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XT4_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XT4_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XT4_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XT4_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XT4_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XT4_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XT4_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XT4_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XT4_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XT4_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XT4_FOCUS_AFS        = SDK_FOCUS_AFS,
	XT4_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XT4_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XT4_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XT4_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XT4_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XT4_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XT4_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XT4_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XT4_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Focus Points
enum{
	XT4_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XT4_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XT4_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XT4_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XT4_AF_ALL     = SDK_AF_ALL,
	XT4_AF_MULTI   = SDK_AF_MULTI,
	XT4_AF_AREA    = SDK_AF_AREA,
	XT4_AF_SINGLE  = XT4_AF_AREA,
	XT4_AF_ZONE    = SDK_AF_ZONE,
	XT4_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XT4_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XT4_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XT4_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XT4_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XT4_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XT4_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XT4_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XT4_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XT4_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XT4_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XT4_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XT4_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XT4_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XT4_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XT4_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XT4_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XT4_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XT4_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XT4_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XT4_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XT4_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XT4_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XT4_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XT4_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XT4_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XT4_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
};

// FOCUS AREA
enum{
	XT4_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XT4_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XT4_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XT4_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XT4_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XT4_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XT4_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XT4_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XT4_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XT4_MACRO_MODE           = SDK_MACRO_MODE,
	XT4_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XT4_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XT4_DRIVE_MODE_CL   = 0x1000,
	XT4_DRIVE_MODE_CH   = 0x10F0,
	XT4_DRIVE_MODE_BKT  = 0x4000,
	XT4_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE,
};

// USB MODE
enum{
	XT4_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XT4_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XT4_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XT4_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XT4_BEEP_HIGH        = SDK_BEEP_HIGH,
	XT4_BEEP_MID         = SDK_BEEP_MID,
	XT4_BEEP_LOW         = SDK_BEEP_LOW,
	XT4_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XT4_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XT4_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XT4_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XT4_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XT4_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XT4_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XT4_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XT4_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XT4_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XT4_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XT4_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XT4_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XT4_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XT4_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XT4_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XT4_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XT4_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XT4_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XT4_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XT4_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XT4_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XT4_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XT4_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XT4_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XT4_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XT4_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XT4_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT4_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT4_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XT4_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT4_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT4_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XT4_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT4_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT4_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XT4_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XT4_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XT4_FRAMEGUIDE_GRID_HD      = 0x0003,
	XT4_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XT4_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XT4_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XT4_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XT4_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XT4_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XT4_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XT4_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XT4_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XT4_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XT4_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XT4_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XT4_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XT4_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XT4_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XT4_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XT4_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XT4_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XT4_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XT4_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XT4_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XT4_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XT4_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XT4_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XT4_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XT4_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XT4_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XT4_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XT4_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XT4_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XT4_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XT4_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XT4_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XT4_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XT4_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XT4_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XT4_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XT4_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XT4_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XT4_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XT4_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XT4_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XT4_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XT4_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XT4_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XT4_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XT4_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XT4_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XT4_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XT4_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XT4_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XT4_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XT4_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XT4_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XT4_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XT4_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XT4_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XT4_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XT4_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XT4_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XT4_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XT4_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XT4_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XT4_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XT4_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XT4_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XT4_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XT4_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XT4_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XT4_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XT4_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XT4_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XT4_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XT4_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XT4_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XT4_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XT4_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XT4_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XT4_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XT4_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XT4_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XT4_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XT4_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XT4_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XT4_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XT4_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XT4_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XT4_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XT4_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XT4_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XT4_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XT4_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XT4_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XT4_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XT4_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XT4_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XT4_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XT4_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XT4_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XT4_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XT4_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XT4_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XT4_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XT4_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XT4_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XT4_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XT4_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XT4_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XT4_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XT4_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XT4_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XT4_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XT4_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XT4_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XT4_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XT4_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XT4_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XT4_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XT4_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XT4_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XT4_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XT4_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XT4_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XT4_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XT4_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XT4_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XT4_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XT4_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XT4_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XT4_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XT4_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XT4_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XT4_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XT4_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XT4_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XT4_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XT4_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XT4_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XT4_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XT4_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XT4_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XT4_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XT4_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XT4_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XT4_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XT4_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XT4_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XT4_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XT4_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XT4_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XT4_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XT4_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XT4_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XT4_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XT4_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XT4_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XT4_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XT4_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XT4_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XT4_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XT4_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XT4_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XT4_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XT4_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER  = SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER,
	XT4_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XT4_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XT4_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XT4_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XT4_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XT4_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XT4_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XT4_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XT4_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XT4_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XT4_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XT4_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XT4_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XT4_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XT4_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XT4_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XT4_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XT4_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XT4_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XT4_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XT4_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XT4_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XT4_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XT4_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	XT4_MEDIASTATUS_ACCESSING             = SDK_MEDIASTATUS_ACCESSING,
};

enum{
	XT4_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XT4_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XT4_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XT4_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XT4_ON  = SDK_ON,
	XT4_OFF = SDK_OFF,
};

enum{
	XT4_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XT4_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XT4_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XT4_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XT4_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XT4_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XT4_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XT4_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XT4_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XT4_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XT4_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XT4_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XT4_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XT4_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XT4_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XT4_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XT4_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XT4_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XT4_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XT4_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XT4_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XT4_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XT4_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XT4_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XT4_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XT4_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XT4_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XT4_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XT4_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XT4_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XT4_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XT4_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XT4_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XT4_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XT4_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XT4_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XT4_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XT4_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XT4_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XT4_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XT4_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XT4_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XT4_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XT4_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XT4_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XT4_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XT4_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XT4_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XT4_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XT4_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XT4_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XT4_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XT4_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XT4_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XT4_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XT4_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};

// Micline Setting
enum{
	XT4_MICLINE_SETTING_MIC          = SDK_MICLINE_SETTING_MIC,
	XT4_MICLINE_SETTING_LINE         = SDK_MICLINE_SETTING_LINE,
};

// Shuttercount Type
enum{
	XT4_SHUTTERCOUNT_TYPE_FRONTCURTAIN = SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN,
	XT4_SHUTTERCOUNT_TYPE_REARCURTAIN  = SDK_SHUTTERCOUNT_TYPE_REARCURTAIN,
	XT4_SHUTTERCOUNT_TYPE_TOTAL        = SDK_SHUTTERCOUNT_TYPE_TOTAL,
};

// Performance
enum{
	XT4_PERFORMANCE_NORMAL                    = SDK_PERFORMANCE_NORMAL,
	XT4_PERFORMANCE_ECONOMY                   = SDK_PERFORMANCE_ECONOMY,
	XT4_PERFORMANCE_BOOST_LOWLIGHT            = SDK_PERFORMANCE_BOOST_LOWLIGHT,
	XT4_PERFORMANCE_BOOST_RESOLUTION_PRIORITY = SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY,
	XT4_PERFORMANCE_BOOST_FRAMERATE_PRIORITY  = SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY,
};
