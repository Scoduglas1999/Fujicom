///////////////////////////////////////////////////
//  X-T3.h
//     Version 1.0.0.0
//
//     Copyright (C) 2018 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-T3
//
enum {
	XT3_API_PARAM_SetImageSize                = 1,
	XT3_API_PARAM_GetImageSize                = 1,
	XT3_API_PARAM_SetSharpness                = 1,
	XT3_API_PARAM_GetSharpness                = 1,
	XT3_API_PARAM_SetColorMode                = 1,
	XT3_API_PARAM_GetColorMode                = 1,
	XT3_API_PARAM_SetFilmSimulationMode       = 1,
	XT3_API_PARAM_GetFilmSimulationMode       = 1,
	XT3_API_PARAM_SetColorSpace               = 1,
	XT3_API_PARAM_GetColorSpace               = 1,
	XT3_API_PARAM_SetImageQuality             = 1,
	XT3_API_PARAM_GetImageQuality             = 1,
	XT3_API_PARAM_SetRAWCompression           = 1,
	XT3_API_PARAM_GetRAWCompression           = 1,
	XT3_API_PARAM_SetGrainEffect              = 1,
	XT3_API_PARAM_GetGrainEffect              = 1,
	XT3_API_PARAM_SetShadowing                = 1,
	XT3_API_PARAM_GetShadowing                = 1,
	XT3_API_PARAM_SetNoiseReduction           = 1,
	XT3_API_PARAM_GetNoiseReduction           = 1,
	XT3_API_PARAM_SetFaceDetectionMode        = 1,
	XT3_API_PARAM_GetFaceDetectionMode        = 1,
	XT3_API_PARAM_SetEyeAFMode                = 1,
	XT3_API_PARAM_GetEyeAFMode                = 1,
	XT3_API_PARAM_SetMacroMode                = -1,
	XT3_API_PARAM_GetMacroMode                = -1,
	XT3_API_PARAM_CapFocusLimiter             = 2,
	XT3_API_PARAM_SetFocusLimiter             = 1,
	XT3_API_PARAM_GetFocusLimiter             = 1,
	XT3_API_PARAM_SetHighLightTone            = 1,
	XT3_API_PARAM_GetHighLightTone            = 1,
	XT3_API_PARAM_SetShadowTone               = 1,
	XT3_API_PARAM_GetShadowTone               = 1,
	XT3_API_PARAM_SetLongExposureNR           = 1,
	XT3_API_PARAM_GetLongExposureNR           = 1,
	XT3_API_PARAM_SetFullTimeManualFocus      = 1,
	XT3_API_PARAM_GetFullTimeManualFocus      = 1,
	XT3_API_PARAM_SetFocusMode                = 1,
	XT3_API_PARAM_GetFocusMode                = 1,
	XT3_API_PARAM_SetAFMode                   = 2,
	XT3_API_PARAM_GetAFMode                   = 2,
	XT3_API_PARAM_SetFocusPoints              = 1,
	XT3_API_PARAM_GetFocusPoints              = 1,
	XT3_API_PARAM_SetFocusArea                = 2,
	XT3_API_PARAM_GetFocusArea                = 2,
	XT3_API_PARAM_CapFocusMode                = 2,
	XT3_API_PARAM_GetAFStatus                 = 1,
	XT3_API_PARAM_SetShutterPriorityMode      = 2,
	XT3_API_PARAM_GetShutterPriorityMode      = 2,
	XT3_API_PARAM_SetInstantAFMode            = 1,
	XT3_API_PARAM_GetInstantAFMode            = 1,
	XT3_API_PARAM_SetPreAFMode                = 1,
	XT3_API_PARAM_GetPreAFMode                = 1,
	XT3_API_PARAM_SetAFIlluminator            = 1,
	XT3_API_PARAM_GetAFIlluminator            = 1,
	XT3_API_PARAM_CapLensISSwitch             = 2,
	XT3_API_PARAM_SetLensISSwitch             = 1,
	XT3_API_PARAM_GetLensISSwitch             = 1,
	XT3_API_PARAM_SetISMode                   = 1,
	XT3_API_PARAM_GetISMode                   = 1,
	XT3_API_PARAM_SetLMOMode                  = 1,
	XT3_API_PARAM_GetLMOMode                  = 1,
	XT3_API_PARAM_SetWhiteBalanceMode         = -1,
	XT3_API_PARAM_GetWhiteBalanceMode         = -1,
	XT3_API_PARAM_SetWhiteBalanceTune         = 3,
	XT3_API_PARAM_GetWhiteBalanceTune         = 3,
	XT3_API_PARAM_SetCaptureDelay             = 1,
	XT3_API_PARAM_GetCaptureDelay             = 1,
	XT3_API_PARAM_CapFocusPos                 = 2,
	XT3_API_PARAM_SetFocusPos                 = 1,
	XT3_API_PARAM_GetFocusPos                 = 1,
	XT3_API_PARAM_SetMFAssistMode             = 1,
	XT3_API_PARAM_GetMFAssistMode             = 1,
	XT3_API_PARAM_SetFocusCheckMode           = 1,
	XT3_API_PARAM_GetFocusCheckMode           = 1,
	XT3_API_PARAM_SetInterlockAEAFArea        = 1,
	XT3_API_PARAM_GetInterlockAEAFArea        = 1,
	XT3_API_PARAM_StartLiveView               = 0,
	XT3_API_PARAM_StopLiveView                = 0,
	XT3_API_PARAM_SetLiveViewImageQuality     = 1,
	XT3_API_PARAM_GetLiveViewImageQuality     = 1,
	XT3_API_PARAM_SetLiveViewImageSize        = 1,
	XT3_API_PARAM_GetLiveViewImageSize        = 1,
	XT3_API_PARAM_SetThroughImageZoom         = 1,
	XT3_API_PARAM_GetThroughImageZoom         = 1,
	XT3_API_PARAM_SetDateTime                 = 6,
	XT3_API_PARAM_GetDateTime                 = 6,
	XT3_API_PARAM_SetDateTimeDispFormat       = -1,
	XT3_API_PARAM_GetDateTimeDispFormat       = -1,
	XT3_API_PARAM_SetWorldClock               = -1,
	XT3_API_PARAM_GetWorldClock               = -1,
	XT3_API_PARAM_SetTimeDifference           = -1,
	XT3_API_PARAM_GetTimeDifference           = -1,
	XT3_API_PARAM_ResetSetting                = 1,
	XT3_API_PARAM_SetSilentMode               = -1,
	XT3_API_PARAM_GetSilentMode               = -1,
	XT3_API_PARAM_SetBeep                     = -1,
	XT3_API_PARAM_GetBeep                     = -1,
	XT3_API_PARAM_SetFunctionLock             = 1,
	XT3_API_PARAM_GetFunctionLock             = 1,
	XT3_API_PARAM_SetFunctionLockCategory     = 2,
	XT3_API_PARAM_GetFunctionLockCategory     = 2,
	XT3_API_PARAM_SetComment                  = 1,
	XT3_API_PARAM_GetComment                  = 1,
	XT3_API_PARAM_SetCopyright                = 2,
	XT3_API_PARAM_GetCopyright                = 2,
	XT3_API_PARAM_SetFilenamePrefix           = 2,
	XT3_API_PARAM_GetFilenamePrefix           = 2,
	XT3_API_PARAM_SetFoldernameSuffix         = 3,
	XT3_API_PARAM_GetFoldernameSuffix         = 3,
	XT3_API_PARAM_GetFoldernameList           = 4,
	XT3_API_PARAM_CheckBatteryInfo            = 6,
	XT3_API_PARAM_SetFrameNumberSequence      = -1,
	XT3_API_PARAM_GetFrameNumberSequence      = -1,
	XT3_API_PARAM_SetUSBMode                  = -1,
	XT3_API_PARAM_GetUSBMode                  = -1,
	XT3_API_PARAM_FormatMemoryCard            = 1,
	XT3_API_PARAM_SetMediaRecord              = 1,
	XT3_API_PARAM_GetMediaRecord              = 1,
	XT3_API_PARAM_GetMediaCapacity            = 5,
	XT3_API_PARAM_GetMediaStatus              = 2,
	XT3_API_PARAM_GetShutterCount             = 3,
	XT3_API_PARAM_SetSensorCleanTiming        = -1,
	XT3_API_PARAM_GetSensorCleanTiming        = -1,
	XT3_API_PARAM_SetPreviewTime              = -1,
	XT3_API_PARAM_GetPreviewTime              = -1,
	XT3_API_PARAM_SetEVFDispAutoRotate        = -1,
	XT3_API_PARAM_GetEVFDispAutoRotate        = -1,
	XT3_API_PARAM_SetDispMMode                = 1,
	XT3_API_PARAM_GetDispMMode                = 1,
	XT3_API_PARAM_SetExposurePreview          = 1,
	XT3_API_PARAM_GetExposurePreview          = 1,
	XT3_API_PARAM_SetDispBrightness           = -1,
	XT3_API_PARAM_GetDispBrightness           = -1,
	XT3_API_PARAM_SetFrameGuideMode           = 1,
	XT3_API_PARAM_GetFrameGuideMode           = 1,
	XT3_API_PARAM_SetFrameGuideGridInfo       = 2,
	XT3_API_PARAM_GetFrameGuideGridInfo       = 2,
	XT3_API_PARAM_SetAutoImageRotation        = -1,
	XT3_API_PARAM_GetAutoImageRotation        = -1,
	XT3_API_PARAM_SetFocusScaleUnit           = -1,
	XT3_API_PARAM_GetFocusScaleUnit           = -1,
	XT3_API_PARAM_SetCustomDispInfo           = -1,
	XT3_API_PARAM_GetCustomDispInfo           = -1,
	XT3_API_PARAM_SetViewMode                 = 2,
	XT3_API_PARAM_GetViewMode                 = 2,
	XT3_API_PARAM_SetDispInfoMode             = -1,
	XT3_API_PARAM_GetDispInfoMode             = -1,
	XT3_API_PARAM_SetDispChroma               = -1,
	XT3_API_PARAM_GetDispChroma               = -1,
	XT3_API_PARAM_SetCustomAutoPowerOff       = 1,
	XT3_API_PARAM_GetCustomAutoPowerOff       = 1,
	XT3_API_PARAM_SetFunctionButton           = -1,
	XT3_API_PARAM_GetFunctionButton           = -1,
	XT3_API_PARAM_SetISODialHn                = -1,
	XT3_API_PARAM_GetISODialHn                = -1,
	XT3_API_PARAM_SetLockButtonMode           = 1,
	XT3_API_PARAM_GetLockButtonMode           = 1,
	XT3_API_PARAM_SetAFLockMode               = -1,
	XT3_API_PARAM_GetAFLockMode               = -1,
	XT3_API_PARAM_SetMicJackMode              = -1,
	XT3_API_PARAM_GetMicJackMode              = -1,
	XT3_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XT3_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XT3_API_PARAM_SetCrossKeyAssign           = -1,
	XT3_API_PARAM_GetCrossKeyAssign           = -1,
	XT3_API_PARAM_SetWideDynamicRange         = 1,
	XT3_API_PARAM_GetWideDynamicRange         = 1,
	XT3_API_PARAM_SetBlackImageTone           = 1,
	XT3_API_PARAM_GetBlackImageTone           = 1,
	XT3_API_PARAM_GetTNumber                  = 1,
	XT3_API_PARAM_SetCropMode                 = 1,
	XT3_API_PARAM_GetCropMode                 = 2,
	XT3_API_PARAM_GetCropAreaFrameInfo        = 1,
	XT3_API_PARAM_GetDetectedFaceFrame        = 2,
	XT3_API_PARAM_SetDetectedFaceFrame        = 1,
	XT3_API_PARAM_SetFocusLimiterPos          = 2,
	XT3_API_PARAM_GetFocusLimiterIndicator    = 1,
	XT3_API_PARAM_GetFocusLimiterRange        = 2,
	XT3_API_PARAM_SetFocusLimiterMode         = 1,
	XT3_API_PARAM_GetFocusLimiterMode         = 1,
};

//
// API_CODE for X-T3
//
enum {
	XT3_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XT3_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XT3_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XT3_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XT3_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XT3_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XT3_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XT3_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XT3_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XT3_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XT3_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XT3_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XT3_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XT3_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XT3_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XT3_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XT3_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XT3_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XT3_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XT3_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XT3_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XT3_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XT3_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XT3_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XT3_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XT3_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XT3_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XT3_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XT3_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XT3_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XT3_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XT3_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XT3_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XT3_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XT3_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XT3_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XT3_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XT3_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XT3_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XT3_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XT3_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XT3_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XT3_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XT3_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XT3_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XT3_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XT3_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XT3_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XT3_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XT3_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XT3_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XT3_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XT3_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XT3_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XT3_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XT3_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XT3_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XT3_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XT3_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XT3_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XT3_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XT3_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XT3_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XT3_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XT3_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XT3_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XT3_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XT3_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XT3_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XT3_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XT3_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XT3_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XT3_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XT3_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XT3_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XT3_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XT3_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XT3_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XT3_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XT3_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XT3_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XT3_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XT3_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XT3_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XT3_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XT3_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XT3_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XT3_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XT3_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XT3_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XT3_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XT3_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XT3_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XT3_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XT3_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XT3_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XT3_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XT3_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XT3_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XT3_API_CODE_SetComment                  = API_CODE_SetComment,
	XT3_API_CODE_GetComment                  = API_CODE_GetComment,
	XT3_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XT3_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XT3_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XT3_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XT3_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XT3_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XT3_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XT3_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XT3_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XT3_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XT3_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XT3_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XT3_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XT3_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XT3_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XT3_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XT3_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XT3_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XT3_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XT3_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XT3_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XT3_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XT3_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XT3_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XT3_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XT3_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XT3_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XT3_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XT3_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XT3_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XT3_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XT3_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XT3_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XT3_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XT3_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XT3_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XT3_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XT3_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XT3_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XT3_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XT3_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XT3_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XT3_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XT3_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XT3_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XT3_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XT3_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XT3_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XT3_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XT3_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XT3_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XT3_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XT3_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XT3_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XT3_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XT3_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XT3_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XT3_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XT3_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XT3_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XT3_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XT3_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XT3_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XT3_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XT3_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XT3_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XT3_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XT3_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XT3_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XT3_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XT3_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XT3_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XT3_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XT3_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XT3_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XT3_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XT3_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XT3_FocusArea, *PXT3_FocusArea;


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
typedef  SDK_ISOAuto     XT3_ISOAuto, *PXT3_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XT3_FrameGuideGridInfo, *PXT3_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XT3_FOCUS_POS_CAP, *PSDK_XT3_FOCUS_POS_CAP;
typedef  SDK_XT3_FOCUS_POS_CAP     XT3_FOCUS_POS_CAP, *PXT3_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XT3_FOLDER_INFO, *PXT3_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XT3_CropAreaFrameInfo, *PXT3_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XT3_FaceFrameInfo, *PXT3_FaceFrameInfo;


// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XT3_FOCUS_LIMITER_INDICATOR, *PXT3_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XT3_FOCUS_LIMITER, *PXT3_FOCUS_LIMITER;


// Image Size
enum{
	XT3_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XT3_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XT3_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XT3_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XT3_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XT3_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XT3_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XT3_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XT3_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
};

// Still Image Quality
enum{
	XT3_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XT3_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XT3_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XT3_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XT3_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
};

// LiveView Image Quality
enum{
	XT3_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XT3_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XT3_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XT3_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XT3_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XT3_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XT3_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XT3_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XT3_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XT3_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XT3_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XT3_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XT3_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XT3_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XT3_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XT3_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XT3_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XT3_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
};

// Color Space
enum{
	XT3_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XT3_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XT3_WB_AUTO            = SDK_WB_AUTO,  
	XT3_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XT3_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XT3_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XT3_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XT3_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XT3_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XT3_WB_SHADE           = SDK_WB_SHADE,
	XT3_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XT3_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XT3_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XT3_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XT3_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XT3_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XT3_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XT3_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XT3_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XT3_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XT3_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XT3_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XT3_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XT3_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XT3_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XT3_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XT3_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XT3_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XT3_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XT3_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XT3_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XT3_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XT3_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XT3_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XT3_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XT3_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XT3_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XT3_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XT3_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XT3_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XT3_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XT3_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XT3_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XT3_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XT3_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XT3_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XT3_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XT3_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XT3_WB_R_SHIFT_STEP    = 1,
	XT3_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XT3_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XT3_WB_B_SHIFT_STEP    = 1,
};

// FILM SIMULATION
enum{
	XT3_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XT3_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XT3_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XT3_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XT3_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XT3_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XT3_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XT3_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XT3_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XT3_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XT3_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XT3_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XT3_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XT3_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XT3_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XT3_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XT3_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XT3_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
};

// COLOR
enum{
	XT3_COLOR_HIGH          = SDK_COLOR_HIGH,
	XT3_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XT3_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XT3_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XT3_COLOR_LOW           = SDK_COLOR_LOW,
	XT3_COLOR_P4            = SDK_COLOR_P4,
	XT3_COLOR_P3            = SDK_COLOR_P3,
	XT3_COLOR_P2            = SDK_COLOR_P2,
	XT3_COLOR_P1            = SDK_COLOR_P1,
	XT3_COLOR_0             = SDK_COLOR_0,
	XT3_COLOR_M1            = SDK_COLOR_M1,
	XT3_COLOR_M2            = SDK_COLOR_M2,
	XT3_COLOR_M3            = SDK_COLOR_M3,
	XT3_COLOR_M4            = SDK_COLOR_M4,
};

// SHARPNESS
enum{
	XT3_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XT3_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XT3_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XT3_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XT3_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XT3_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XT3_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XT3_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XT3_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XT3_SHARPNESS_0               = SDK_SHARPNESS_0,
	XT3_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XT3_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XT3_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XT3_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XT3_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XT3_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XT3_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XT3_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XT3_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XT3_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XT3_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XT3_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XT3_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XT3_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XT3_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XT3_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XT3_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XT3_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XT3_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XT3_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XT3_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XT3_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XT3_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XT3_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XT3_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XT3_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XT3_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XT3_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XT3_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XT3_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XT3_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XT3_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XT3_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XT3_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XT3_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XT3_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XT3_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XT3_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XT3_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XT3_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XT3_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XT3_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XT3_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XT3_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XT3_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XT3_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XT3_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XT3_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XT3_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XT3_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XT3_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
};

// Grain Effect
enum{
	XT3_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XT3_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XT3_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XT3_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XT3_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
};

// Shadowing
enum{
	XT3_SHADOWING_0             = SDK_SHADOWING_0,
	XT3_SHADOWING_P1            = SDK_SHADOWING_P1,
	XT3_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// SELF TIMER
enum{
	XT3_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XT3_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XT3_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XT3_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XT3_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XT3_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XT3_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XT3_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XT3_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XT3_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XT3_FOCUS_AFS        = SDK_FOCUS_AFS,
	XT3_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter
enum{
	XT3_FOCUS_LIMITER_OFF       = SDK_FOCUS_LIMITER_OFF,
	XT3_FOCUS_LIMITER_FULL      = SDK_FOCUS_LIMITER_FULL,
	XT3_FOCUS_LIMITER_MOD_MID   = SDK_FOCUS_LIMITER_MOD_MID,
	XT3_FOCUS_LIMITER_MID_INF   = SDK_FOCUS_LIMITER_MID_INF,
	XT3_FOCUS_LIMITER_1         = SDK_FOCUS_LIMITER_1,
	XT3_FOCUS_LIMITER_2         = SDK_FOCUS_LIMITER_2,
	XT3_FOCUS_LIMITER_3         = SDK_FOCUS_LIMITER_3,
};

// Focus Limiter Pos
enum{
	XT3_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XT3_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XT3_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XT3_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Points
enum{
	XT3_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XT3_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XT3_AF_ALL     = SDK_AF_ALL,
	XT3_AF_MULTI   = SDK_AF_MULTI,
	XT3_AF_AREA    = SDK_AF_AREA,
	XT3_AF_SINGLE  = XT3_AF_AREA,
	XT3_AF_ZONE    = SDK_AF_ZONE,
	XT3_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XT3_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XT3_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XT3_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XT3_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XT3_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XT3_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XT3_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XT3_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XT3_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XT3_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XT3_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XT3_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XT3_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XT3_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XT3_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XT3_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XT3_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XT3_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XT3_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XT3_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XT3_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XT3_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XT3_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XT3_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XT3_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XT3_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
};

// FOCUS AREA
enum{
	XT3_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XT3_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XT3_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XT3_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XT3_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XT3_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XT3_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XT3_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XT3_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XT3_MACRO_MODE           = SDK_MACRO_MODE,
	XT3_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XT3_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XT3_DRIVE_MODE_CL   = 0x1000,
	XT3_DRIVE_MODE_CH   = 0x10F0,
	XT3_DRIVE_MODE_BKT  = 0x4000,
	XT3_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE
};

// USB MODE
enum{
	XT3_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XT3_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XT3_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XT3_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XT3_BEEP_HIGH        = SDK_BEEP_HIGH,
	XT3_BEEP_MID         = SDK_BEEP_MID,
	XT3_BEEP_LOW         = SDK_BEEP_LOW,
	XT3_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XT3_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XT3_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XT3_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XT3_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XT3_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XT3_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XT3_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XT3_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XT3_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
};

// LCD DISP MODE
enum{
	XT3_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XT3_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XT3_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XT3_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XT3_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XT3_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XT3_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XT3_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XT3_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XT3_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XT3_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XT3_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XT3_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XT3_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT3_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT3_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XT3_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT3_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT3_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XT3_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT3_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT3_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XT3_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XT3_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XT3_FRAMEGUIDE_GRID_HD      = 0x0003,
	XT3_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XT3_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XT3_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XT3_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XT3_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XT3_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XT3_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XT3_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XT3_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XT3_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XT3_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XT3_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XT3_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XT3_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XT3_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XT3_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
};

enum{
	XT3_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XT3_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XT3_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XT3_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XT3_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XT3_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XT3_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XT3_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XT3_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XT3_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XT3_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XT3_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XT3_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XT3_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XT3_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XT3_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XT3_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XT3_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XT3_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XT3_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XT3_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XT3_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XT3_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XT3_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XT3_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XT3_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XT3_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XT3_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XT3_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XT3_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XT3_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XT3_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XT3_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XT3_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XT3_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XT3_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XT3_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XT3_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XT3_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XT3_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XT3_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XT3_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XT3_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XT3_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XT3_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XT3_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XT3_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XT3_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XT3_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XT3_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XT3_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XT3_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XT3_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XT3_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XT3_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XT3_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XT3_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XT3_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XT3_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XT3_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XT3_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XT3_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XT3_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XT3_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XT3_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XT3_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XT3_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XT3_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XT3_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XT3_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XT3_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XT3_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XT3_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XT3_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XT3_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XT3_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XT3_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XT3_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XT3_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XT3_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XT3_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XT3_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XT3_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XT3_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XT3_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XT3_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XT3_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XT3_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XT3_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XT3_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XT3_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XT3_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XT3_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XT3_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XT3_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XT3_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XT3_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XT3_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XT3_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XT3_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XT3_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XT3_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XT3_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XT3_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XT3_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XT3_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XT3_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XT3_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XT3_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XT3_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XT3_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XT3_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XT3_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XT3_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XT3_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XT3_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XT3_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XT3_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XT3_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XT3_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XT3_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XT3_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XT3_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XT3_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XT3_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XT3_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XT3_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XT3_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XT3_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XT3_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XT3_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XT3_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XT3_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XT3_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XT3_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XT3_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XT3_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XT3_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XT3_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XT3_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XT3_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XT3_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XT3_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XT3_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XT3_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XT3_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XT3_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XT3_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XT3_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XT3_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XT3_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XT3_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XT3_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XT3_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XT3_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XT3_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XT3_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XT3_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XT3_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XT3_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
};

enum{
	XT3_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XT3_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XT3_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XT3_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XT3_ON  = SDK_ON,
	XT3_OFF = SDK_OFF,
};

enum{
	XT3_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XT3_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XT3_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XT3_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XT3_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XT3_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XT3_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XT3_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XT3_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XT3_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XT3_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XT3_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XT3_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XT3_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XT3_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XT3_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XT3_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XT3_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XT3_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XT3_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XT3_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XT3_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XT3_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XT3_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XT3_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XT3_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XT3_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XT3_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XT3_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XT3_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XT3_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XT3_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XT3_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XT3_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XT3_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XT3_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XT3_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XT3_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XT3_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XT3_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XT3_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XT3_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XT3_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XT3_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XT3_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XT3_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XT3_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XT3_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XT3_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XT3_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XT3_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XT3_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XT3_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XT3_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XT3_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};
