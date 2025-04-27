///////////////////////////////////////////////////
//  GFX50R.h
//     Version 1.0.0.0
//
//     Copyright (C) 2018 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for GFX50R
//
enum {
	GFX50R_API_PARAM_SetImageSize                = 1,
	GFX50R_API_PARAM_GetImageSize                = 1,
	GFX50R_API_PARAM_SetSharpness                = 1,
	GFX50R_API_PARAM_GetSharpness                = 1,
	GFX50R_API_PARAM_SetColorMode                = 1,
	GFX50R_API_PARAM_GetColorMode                = 1,
	GFX50R_API_PARAM_SetFilmSimulationMode       = 1,
	GFX50R_API_PARAM_GetFilmSimulationMode       = 1,
	GFX50R_API_PARAM_SetColorSpace               = 1,
	GFX50R_API_PARAM_GetColorSpace               = 1,
	GFX50R_API_PARAM_SetImageQuality             = 1,
	GFX50R_API_PARAM_GetImageQuality             = 1,
	GFX50R_API_PARAM_SetRAWCompression           = 1,
	GFX50R_API_PARAM_GetRAWCompression           = 1,
	GFX50R_API_PARAM_SetGrainEffect              = 1,
	GFX50R_API_PARAM_GetGrainEffect              = 1,
	GFX50R_API_PARAM_SetShadowing                = 1,
	GFX50R_API_PARAM_GetShadowing                = 1,
	GFX50R_API_PARAM_SetNoiseReduction           = 1,
	GFX50R_API_PARAM_GetNoiseReduction           = 1,
	GFX50R_API_PARAM_SetFaceDetectionMode        = 1,
	GFX50R_API_PARAM_GetFaceDetectionMode        = 1,
	GFX50R_API_PARAM_SetEyeAFMode                = 1,
	GFX50R_API_PARAM_GetEyeAFMode                = 1,
	GFX50R_API_PARAM_SetMacroMode                = -1,
	GFX50R_API_PARAM_GetMacroMode                = -1,
	GFX50R_API_PARAM_CapFocusLimiter             = 2,
	GFX50R_API_PARAM_SetFocusLimiter             = 1,
	GFX50R_API_PARAM_GetFocusLimiter             = 1,
	GFX50R_API_PARAM_SetHighLightTone            = 1,
	GFX50R_API_PARAM_GetHighLightTone            = 1,
	GFX50R_API_PARAM_SetShadowTone               = 1,
	GFX50R_API_PARAM_GetShadowTone               = 1,
	GFX50R_API_PARAM_SetLongExposureNR           = 1,
	GFX50R_API_PARAM_GetLongExposureNR           = 1,
	GFX50R_API_PARAM_SetFullTimeManualFocus      = 1,
	GFX50R_API_PARAM_GetFullTimeManualFocus      = 1,
	GFX50R_API_PARAM_SetFocusMode                = 1,
	GFX50R_API_PARAM_GetFocusMode                = 1,
	GFX50R_API_PARAM_SetAFMode                   = 2,
	GFX50R_API_PARAM_GetAFMode                   = 2,
	GFX50R_API_PARAM_SetFocusPoints              = 1,
	GFX50R_API_PARAM_GetFocusPoints              = 1,
	GFX50R_API_PARAM_SetFocusArea                = 2,
	GFX50R_API_PARAM_GetFocusArea                = 2,
	GFX50R_API_PARAM_SetShutterPriorityMode      = 2,
	GFX50R_API_PARAM_GetShutterPriorityMode      = 2,
	GFX50R_API_PARAM_SetInstantAFMode            = 1,
	GFX50R_API_PARAM_GetInstantAFMode            = 1,
	GFX50R_API_PARAM_SetPreAFMode                = 1,
	GFX50R_API_PARAM_GetPreAFMode                = 1,
	GFX50R_API_PARAM_SetAFIlluminator            = 1,
	GFX50R_API_PARAM_GetAFIlluminator            = 1,
	GFX50R_API_PARAM_CapLensISSwitch             = 2,
	GFX50R_API_PARAM_SetLensISSwitch             = 1,
	GFX50R_API_PARAM_GetLensISSwitch             = 1,
	GFX50R_API_PARAM_SetISMode                   = 1,
	GFX50R_API_PARAM_GetISMode                   = 1,
	GFX50R_API_PARAM_SetLMOMode                  = 1,
	GFX50R_API_PARAM_GetLMOMode                  = 1,
	GFX50R_API_PARAM_SetWhiteBalanceMode         = -1,
	GFX50R_API_PARAM_GetWhiteBalanceMode         = -1,
	GFX50R_API_PARAM_SetWhiteBalanceTune         = 3,
	GFX50R_API_PARAM_GetWhiteBalanceTune         = 3,
	GFX50R_API_PARAM_SetCaptureDelay             = 1,
	GFX50R_API_PARAM_GetCaptureDelay             = 1,
	GFX50R_API_PARAM_CapFocusPos                 = 2,
	GFX50R_API_PARAM_SetFocusPos                 = 1,
	GFX50R_API_PARAM_GetFocusPos                 = 1,
	GFX50R_API_PARAM_SetMFAssistMode             = 1,
	GFX50R_API_PARAM_GetMFAssistMode             = 1,
	GFX50R_API_PARAM_SetFocusCheckMode           = 1,
	GFX50R_API_PARAM_GetFocusCheckMode           = 1,
	GFX50R_API_PARAM_SetInterlockAEAFArea        = 1,
	GFX50R_API_PARAM_GetInterlockAEAFArea        = 1,
	GFX50R_API_PARAM_StartLiveView               = 0,
	GFX50R_API_PARAM_StopLiveView                = 0,
	GFX50R_API_PARAM_SetLiveViewImageQuality     = 1,
	GFX50R_API_PARAM_GetLiveViewImageQuality     = 1,
	GFX50R_API_PARAM_SetLiveViewImageSize        = 1,
	GFX50R_API_PARAM_GetLiveViewImageSize        = 1,
	GFX50R_API_PARAM_SetThroughImageZoom         = 1,
	GFX50R_API_PARAM_GetThroughImageZoom         = 1,
	GFX50R_API_PARAM_SetDateTime                 = 6,
	GFX50R_API_PARAM_GetDateTime                 = 6,
	GFX50R_API_PARAM_SetDateTimeDispFormat       = -1,
	GFX50R_API_PARAM_GetDateTimeDispFormat       = -1,
	GFX50R_API_PARAM_SetWorldClock               = -1,
	GFX50R_API_PARAM_GetWorldClock               = -1,
	GFX50R_API_PARAM_SetTimeDifference           = -1,
	GFX50R_API_PARAM_GetTimeDifference           = -1,
	GFX50R_API_PARAM_ResetSetting                = 1,
	GFX50R_API_PARAM_SetSilentMode               = -1,
	GFX50R_API_PARAM_GetSilentMode               = -1,
	GFX50R_API_PARAM_SetBeep                     = -1,
	GFX50R_API_PARAM_GetBeep                     = -1,
	GFX50R_API_PARAM_SetFunctionLock             = 1,
	GFX50R_API_PARAM_GetFunctionLock             = 1,
	GFX50R_API_PARAM_SetFunctionLockCategory     = 2,
	GFX50R_API_PARAM_GetFunctionLockCategory     = 2,
	GFX50R_API_PARAM_SetComment                  = 1,
	GFX50R_API_PARAM_GetComment                  = 1,
	GFX50R_API_PARAM_SetCopyright                = 2,
	GFX50R_API_PARAM_GetCopyright                = 2,
	GFX50R_API_PARAM_SetFilenamePrefix           = 2,
	GFX50R_API_PARAM_GetFilenamePrefix           = 2,
	GFX50R_API_PARAM_SetFoldernameSuffix         = 3,
	GFX50R_API_PARAM_GetFoldernameSuffix         = 3,
	GFX50R_API_PARAM_GetFoldernameList           = 4,
	GFX50R_API_PARAM_CheckBatteryInfo            = 6,
	GFX50R_API_PARAM_SetFrameNumberSequence      = -1,
	GFX50R_API_PARAM_GetFrameNumberSequence      = -1,
	GFX50R_API_PARAM_SetUSBMode                  = -1,
	GFX50R_API_PARAM_GetUSBMode                  = -1,
	GFX50R_API_PARAM_FormatMemoryCard            = 1,
	GFX50R_API_PARAM_SetMediaRecord              = 1,
	GFX50R_API_PARAM_GetMediaRecord              = 1,
	GFX50R_API_PARAM_GetMediaCapacity            = 5,
	GFX50R_API_PARAM_GetMediaStatus              = 2,
	GFX50R_API_PARAM_GetShutterCount             = 3,
	GFX50R_API_PARAM_GetShutterCountEx           = 3,
	GFX50R_API_PARAM_SetSensorCleanTiming        = -1,
	GFX50R_API_PARAM_GetSensorCleanTiming        = -1,
	GFX50R_API_PARAM_SetPreviewTime              = -1,
	GFX50R_API_PARAM_GetPreviewTime              = -1,
	GFX50R_API_PARAM_SetEVFDispAutoRotate        = -1,
	GFX50R_API_PARAM_GetEVFDispAutoRotate        = -1,
	GFX50R_API_PARAM_SetDispMMode                = 1,
	GFX50R_API_PARAM_GetDispMMode                = 1,
	GFX50R_API_PARAM_SetExposurePreview          = 1,
	GFX50R_API_PARAM_GetExposurePreview          = 1,
	GFX50R_API_PARAM_SetDispBrightness           = -1,
	GFX50R_API_PARAM_GetDispBrightness           = -1,
	GFX50R_API_PARAM_SetFrameGuideMode           = 1,
	GFX50R_API_PARAM_GetFrameGuideMode           = 1,
	GFX50R_API_PARAM_SetFrameGuideGridInfo       = 2,
	GFX50R_API_PARAM_GetFrameGuideGridInfo       = 2,
	GFX50R_API_PARAM_SetAutoImageRotation        = -1,
	GFX50R_API_PARAM_GetAutoImageRotation        = -1,
	GFX50R_API_PARAM_SetFocusScaleUnit           = -1,
	GFX50R_API_PARAM_GetFocusScaleUnit           = -1,
	GFX50R_API_PARAM_SetCustomDispInfo           = -1,
	GFX50R_API_PARAM_GetCustomDispInfo           = -1,
	GFX50R_API_PARAM_SetViewMode                 = 2,
	GFX50R_API_PARAM_GetViewMode                 = 2,
	GFX50R_API_PARAM_SetDispInfoMode             = -1,
	GFX50R_API_PARAM_GetDispInfoMode             = -1,
	GFX50R_API_PARAM_SetDispChroma               = -1,
	GFX50R_API_PARAM_GetDispChroma               = -1,
	GFX50R_API_PARAM_SetCustomAutoPowerOff       = 1,
	GFX50R_API_PARAM_GetCustomAutoPowerOff       = 1,
	GFX50R_API_PARAM_SetCustomStudioPowerSave    = 1,
	GFX50R_API_PARAM_GetCustomStudioPowerSave    = 1,
	GFX50R_API_PARAM_SetFunctionButton           = -1,
	GFX50R_API_PARAM_GetFunctionButton           = -1,
	GFX50R_API_PARAM_SetISODialHn                = -1,
	GFX50R_API_PARAM_GetISODialHn                = -1,
	GFX50R_API_PARAM_SetLockButtonMode           = 1,
	GFX50R_API_PARAM_GetLockButtonMode           = 1,
	GFX50R_API_PARAM_SetAFLockMode               = -1,
	GFX50R_API_PARAM_GetAFLockMode               = -1,
	GFX50R_API_PARAM_SetMicJackMode              = -1,
	GFX50R_API_PARAM_GetMicJackMode              = -1,
	GFX50R_API_PARAM_SetAeAfLockKeyAssign        = -1,
	GFX50R_API_PARAM_GetAeAfLockKeyAssign        = -1,
	GFX50R_API_PARAM_SetCrossKeyAssign           = -1,
	GFX50R_API_PARAM_GetCrossKeyAssign           = -1,
	GFX50R_API_PARAM_SetWideDynamicRange         = 1,
	GFX50R_API_PARAM_GetWideDynamicRange         = 1,
	GFX50R_API_PARAM_SetBlackImageTone           = 1,
	GFX50R_API_PARAM_GetBlackImageTone           = 1,
	GFX50R_API_PARAM_GetTNumber                  = -1,
	GFX50R_API_PARAM_SetCropMode                 = 1,
	GFX50R_API_PARAM_GetCropMode                 = 2,
	GFX50R_API_PARAM_GetCropAreaFrameInfo        = -1,
	GFX50R_API_PARAM_SetSmoothSkinEffect         = 1,
	GFX50R_API_PARAM_GetSmoothSkinEffect         = 1,
	GFX50R_API_PARAM_GetCommandDialStatus        = 4,
};

//
// API_CODE for GFX50R
//
enum {
	GFX50R_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	GFX50R_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	GFX50R_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	GFX50R_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	GFX50R_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	GFX50R_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	GFX50R_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	GFX50R_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	GFX50R_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	GFX50R_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	GFX50R_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	GFX50R_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	GFX50R_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	GFX50R_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	GFX50R_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	GFX50R_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	GFX50R_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	GFX50R_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	GFX50R_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	GFX50R_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	GFX50R_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	GFX50R_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	GFX50R_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	GFX50R_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	GFX50R_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	GFX50R_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	GFX50R_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	GFX50R_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	GFX50R_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	GFX50R_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	GFX50R_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	GFX50R_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	GFX50R_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	GFX50R_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	GFX50R_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	GFX50R_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	GFX50R_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	GFX50R_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	GFX50R_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	GFX50R_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	GFX50R_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	GFX50R_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	GFX50R_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	GFX50R_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	GFX50R_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	GFX50R_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	GFX50R_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	GFX50R_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	GFX50R_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	GFX50R_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	GFX50R_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	GFX50R_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	GFX50R_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	GFX50R_API_CODE_SetISMode                   = API_CODE_SetISMode,
	GFX50R_API_CODE_GetISMode                   = API_CODE_GetISMode,
	GFX50R_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	GFX50R_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	GFX50R_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	GFX50R_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	GFX50R_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	GFX50R_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	GFX50R_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	GFX50R_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	GFX50R_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	GFX50R_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	GFX50R_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	GFX50R_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	GFX50R_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	GFX50R_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	GFX50R_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	GFX50R_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	GFX50R_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	GFX50R_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	GFX50R_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	GFX50R_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	GFX50R_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	GFX50R_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	GFX50R_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	GFX50R_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	GFX50R_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	GFX50R_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	GFX50R_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	GFX50R_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	GFX50R_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	GFX50R_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	GFX50R_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	GFX50R_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	GFX50R_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	GFX50R_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	GFX50R_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	GFX50R_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	GFX50R_API_CODE_SetBeep                     = API_CODE_SetBeep,
	GFX50R_API_CODE_GetBeep                     = API_CODE_GetBeep,
	GFX50R_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	GFX50R_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	GFX50R_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	GFX50R_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	GFX50R_API_CODE_SetComment                  = API_CODE_SetComment,
	GFX50R_API_CODE_GetComment                  = API_CODE_GetComment,
	GFX50R_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	GFX50R_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	GFX50R_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	GFX50R_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	GFX50R_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	GFX50R_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	GFX50R_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	GFX50R_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	GFX50R_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	GFX50R_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	GFX50R_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	GFX50R_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	GFX50R_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	GFX50R_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	GFX50R_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	GFX50R_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	GFX50R_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	GFX50R_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	GFX50R_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	GFX50R_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	GFX50R_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	GFX50R_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	GFX50R_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	GFX50R_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	GFX50R_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	GFX50R_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	GFX50R_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	GFX50R_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	GFX50R_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	GFX50R_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	GFX50R_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	GFX50R_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	GFX50R_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	GFX50R_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	GFX50R_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	GFX50R_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	GFX50R_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	GFX50R_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	GFX50R_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	GFX50R_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	GFX50R_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	GFX50R_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	GFX50R_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	GFX50R_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	GFX50R_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	GFX50R_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	GFX50R_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	GFX50R_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	GFX50R_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	GFX50R_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	GFX50R_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	GFX50R_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	GFX50R_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	GFX50R_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	GFX50R_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	GFX50R_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	GFX50R_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	GFX50R_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	GFX50R_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	GFX50R_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	GFX50R_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	GFX50R_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	GFX50R_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	GFX50R_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	GFX50R_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	GFX50R_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	GFX50R_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	GFX50R_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	GFX50R_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	GFX50R_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	GFX50R_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	GFX50R_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	GFX50R_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	GFX50R_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	GFX50R_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	GFX50R_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     GFX50R_FocusArea, *PGFX50R_FocusArea;


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
typedef  SDK_ISOAuto     GFX50R_ISOAuto, *PGFX50R_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     GFX50R_FrameGuideGridInfo, *PGFX50R_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_GFX50R_FOCUS_POS_CAP, *PSDK_GFX50R_FOCUS_POS_CAP;
typedef  SDK_GFX50R_FOCUS_POS_CAP     GFX50R_FOCUS_POS_CAP, *PGFX50R_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     GFX50R_FOLDER_INFO, *PGFX50R_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     GFX50R_CropAreaFrameInfo, *PGFX50R_CropAreaFrameInfo;


// Image Size
enum{
	GFX50R_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	GFX50R_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	GFX50R_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	GFX50R_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	GFX50R_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	GFX50R_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	GFX50R_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	GFX50R_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	GFX50R_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	GFX50R_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	GFX50R_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	GFX50R_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	GFX50R_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	GFX50R_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	GFX50R_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	GFX50R_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	GFX50R_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
};

// Still Image Quality
enum{
	GFX50R_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	GFX50R_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	GFX50R_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	GFX50R_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	GFX50R_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	GFX50R_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	GFX50R_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// LiveView Image Quality
enum{
	GFX50R_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	GFX50R_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	GFX50R_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	GFX50R_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	GFX50R_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	GFX50R_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	GFX50R_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	GFX50R_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	GFX50R_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	GFX50R_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	GFX50R_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	GFX50R_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	GFX50R_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	GFX50R_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	GFX50R_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	GFX50R_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	GFX50R_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	GFX50R_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	GFX50R_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	GFX50R_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	GFX50R_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	GFX50R_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	GFX50R_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	GFX50R_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	GFX50R_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
};

// Color Space
enum{
	GFX50R_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	GFX50R_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	GFX50R_WB_AUTO            = SDK_WB_AUTO,  
	GFX50R_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	GFX50R_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	GFX50R_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	GFX50R_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	GFX50R_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	GFX50R_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	GFX50R_WB_SHADE           = SDK_WB_SHADE,
	GFX50R_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	GFX50R_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	GFX50R_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	GFX50R_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	GFX50R_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	GFX50R_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	GFX50R_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	GFX50R_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	GFX50R_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	GFX50R_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	GFX50R_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	GFX50R_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	GFX50R_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	GFX50R_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	GFX50R_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	GFX50R_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	GFX50R_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	GFX50R_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	GFX50R_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	GFX50R_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	GFX50R_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	GFX50R_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	GFX50R_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	GFX50R_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	GFX50R_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	GFX50R_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	GFX50R_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	GFX50R_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	GFX50R_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	GFX50R_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	GFX50R_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	GFX50R_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	GFX50R_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	GFX50R_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	GFX50R_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	GFX50R_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	GFX50R_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	GFX50R_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	GFX50R_WB_R_SHIFT_STEP    = 1,
	GFX50R_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	GFX50R_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	GFX50R_WB_B_SHIFT_STEP    = 1,
};

// FILM SIMULATION
enum{
	GFX50R_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	GFX50R_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	GFX50R_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	GFX50R_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	GFX50R_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	GFX50R_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	GFX50R_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	GFX50R_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	GFX50R_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	GFX50R_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	GFX50R_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	GFX50R_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	GFX50R_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	GFX50R_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	GFX50R_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	GFX50R_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	GFX50R_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	GFX50R_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	GFX50R_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
};

// COLOR
enum{
	GFX50R_COLOR_HIGH          = SDK_COLOR_HIGH,
	GFX50R_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	GFX50R_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	GFX50R_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	GFX50R_COLOR_LOW           = SDK_COLOR_LOW,
	GFX50R_COLOR_P4            = SDK_COLOR_P4,
	GFX50R_COLOR_P3            = SDK_COLOR_P3,
	GFX50R_COLOR_P2            = SDK_COLOR_P2,
	GFX50R_COLOR_P1            = SDK_COLOR_P1,
	GFX50R_COLOR_0             = SDK_COLOR_0,
	GFX50R_COLOR_M1            = SDK_COLOR_M1,
	GFX50R_COLOR_M2            = SDK_COLOR_M2,
	GFX50R_COLOR_M3            = SDK_COLOR_M3,
	GFX50R_COLOR_M4            = SDK_COLOR_M4,
};

// SHARPNESS
enum{
	GFX50R_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	GFX50R_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	GFX50R_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	GFX50R_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	GFX50R_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	GFX50R_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	GFX50R_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	GFX50R_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	GFX50R_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	GFX50R_SHARPNESS_0               = SDK_SHARPNESS_0,
	GFX50R_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	GFX50R_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	GFX50R_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	GFX50R_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	GFX50R_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	GFX50R_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	GFX50R_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	GFX50R_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	GFX50R_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	GFX50R_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	GFX50R_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	GFX50R_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	GFX50R_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	GFX50R_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	GFX50R_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	GFX50R_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	GFX50R_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	GFX50R_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	GFX50R_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	GFX50R_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	GFX50R_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	GFX50R_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	GFX50R_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	GFX50R_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	GFX50R_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	GFX50R_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	GFX50R_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	GFX50R_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	GFX50R_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	GFX50R_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	GFX50R_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	GFX50R_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	GFX50R_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	GFX50R_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	GFX50R_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	GFX50R_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	GFX50R_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	GFX50R_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	GFX50R_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	GFX50R_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	GFX50R_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	GFX50R_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	GFX50R_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	GFX50R_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	GFX50R_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	GFX50R_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	GFX50R_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	GFX50R_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	GFX50R_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	GFX50R_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	GFX50R_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
};

// Grain Effect
enum{
	GFX50R_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	GFX50R_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	GFX50R_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	GFX50R_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	GFX50R_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
};

// Shadowing
enum{
	GFX50R_SHADOWING_0             = SDK_SHADOWING_0,
	GFX50R_SHADOWING_P1            = SDK_SHADOWING_P1,
	GFX50R_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// Smooth Skin Effect
enum{
	GFX50R_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	GFX50R_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	GFX50R_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	GFX50R_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	GFX50R_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	GFX50R_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	GFX50R_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	GFX50R_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	GFX50R_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	GFX50R_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	GFX50R_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	GFX50R_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	GFX50R_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	GFX50R_FOCUS_AFS        = SDK_FOCUS_AFS,
	GFX50R_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter
enum{
	GFX50R_FOCUS_LIMITER_OFF       = SDK_FOCUS_LIMITER_OFF,
	GFX50R_FOCUS_LIMITER_FULL      = SDK_FOCUS_LIMITER_FULL,
	GFX50R_FOCUS_LIMITER_MOD_MID   = SDK_FOCUS_LIMITER_MOD_MID,
	GFX50R_FOCUS_LIMITER_MID_INF   = SDK_FOCUS_LIMITER_MID_INF,
};

// Focus Points
enum{
	GFX50R_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	GFX50R_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	GFX50R_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	GFX50R_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	GFX50R_AF_ALL     = SDK_AF_ALL,
	GFX50R_AF_MULTI   = SDK_AF_MULTI,
	GFX50R_AF_AREA    = SDK_AF_AREA,
	GFX50R_AF_SINGLE  = GFX50R_AF_AREA,
	GFX50R_AF_ZONE    = SDK_AF_ZONE,
	GFX50R_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// Eye AF Mode
enum{
	GFX50R_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	GFX50R_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	GFX50R_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	GFX50R_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// MF Assist Mode
enum{
	GFX50R_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	GFX50R_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	GFX50R_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	GFX50R_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	GFX50R_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	GFX50R_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	GFX50R_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	GFX50R_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	GFX50R_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	GFX50R_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	GFX50R_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	GFX50R_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
};

// FOCUS AREA
enum{
	GFX50R_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	GFX50R_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	GFX50R_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	GFX50R_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	GFX50R_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	GFX50R_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	GFX50R_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	GFX50R_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	GFX50R_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	GFX50R_MACRO_MODE           = SDK_MACRO_MODE,
	GFX50R_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	GFX50R_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	GFX50R_DRIVE_MODE_CL   = 0x1000,
	GFX50R_DRIVE_MODE_CH   = 0x10F0,
	GFX50R_DRIVE_MODE_BKT  = 0x4000
};

// USB MODE
enum{
	GFX50R_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	GFX50R_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	GFX50R_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	GFX50R_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	GFX50R_BEEP_HIGH        = SDK_BEEP_HIGH,
	GFX50R_BEEP_MID         = SDK_BEEP_MID,
	GFX50R_BEEP_LOW         = SDK_BEEP_LOW,
	GFX50R_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	GFX50R_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	GFX50R_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	GFX50R_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	GFX50R_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	GFX50R_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	GFX50R_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	GFX50R_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	GFX50R_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	GFX50R_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
};

// LCD DISP MODE
enum{
	GFX50R_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	GFX50R_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	GFX50R_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	GFX50R_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	GFX50R_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	GFX50R_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	GFX50R_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	GFX50R_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	GFX50R_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	GFX50R_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	GFX50R_DATE_FORMAT_YMD      = SDK_DATE_FORMAT_YMD,
	GFX50R_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	GFX50R_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	GFX50R_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	GFX50R_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	GFX50R_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	GFX50R_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	GFX50R_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	GFX50R_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	GFX50R_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	GFX50R_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	GFX50R_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	GFX50R_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	GFX50R_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	GFX50R_FRAMEGUIDE_GRID_HD      = 0x0003,
	GFX50R_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	GFX50R_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	GFX50R_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	GFX50R_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	GFX50R_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	GFX50R_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	GFX50R_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	GFX50R_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	GFX50R_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	GFX50R_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	GFX50R_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	GFX50R_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	GFX50R_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	GFX50R_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	GFX50R_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	GFX50R_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	GFX50R_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	GFX50R_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	GFX50R_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	GFX50R_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	GFX50R_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	GFX50R_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	GFX50R_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	GFX50R_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	GFX50R_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	GFX50R_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	GFX50R_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	GFX50R_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	GFX50R_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	GFX50R_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	GFX50R_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	GFX50R_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	GFX50R_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	GFX50R_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	GFX50R_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	GFX50R_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	GFX50R_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	GFX50R_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	GFX50R_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	GFX50R_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	GFX50R_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	GFX50R_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	GFX50R_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	GFX50R_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	GFX50R_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	GFX50R_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	GFX50R_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	GFX50R_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	GFX50R_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	GFX50R_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	GFX50R_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	GFX50R_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	GFX50R_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	GFX50R_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	GFX50R_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	GFX50R_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	GFX50R_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	GFX50R_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	GFX50R_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	GFX50R_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	GFX50R_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	GFX50R_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	GFX50R_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	GFX50R_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	GFX50R_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	GFX50R_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	GFX50R_FUNCTION_WB                  = SDK_FUNCTION_WB,
	GFX50R_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	GFX50R_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	GFX50R_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	GFX50R_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	GFX50R_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	GFX50R_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	GFX50R_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	GFX50R_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	GFX50R_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	GFX50R_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	GFX50R_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	GFX50R_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	GFX50R_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	GFX50R_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	GFX50R_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	GFX50R_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	GFX50R_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	GFX50R_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	GFX50R_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	GFX50R_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	GFX50R_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	GFX50R_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	GFX50R_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	GFX50R_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	GFX50R_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	GFX50R_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	GFX50R_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	GFX50R_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	GFX50R_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	GFX50R_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	GFX50R_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	GFX50R_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	GFX50R_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	GFX50R_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	GFX50R_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	GFX50R_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	GFX50R_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	GFX50R_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	GFX50R_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	GFX50R_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	GFX50R_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	GFX50R_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	GFX50R_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	GFX50R_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	GFX50R_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	GFX50R_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	GFX50R_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	GFX50R_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	GFX50R_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	GFX50R_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	GFX50R_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	GFX50R_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	GFX50R_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	GFX50R_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	GFX50R_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	GFX50R_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	GFX50R_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	GFX50R_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	GFX50R_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	GFX50R_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	GFX50R_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	GFX50R_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	GFX50R_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	GFX50R_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	GFX50R_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	GFX50R_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	GFX50R_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	GFX50R_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	GFX50R_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	GFX50R_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	GFX50R_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	GFX50R_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	GFX50R_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	GFX50R_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	GFX50R_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	GFX50R_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	GFX50R_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	GFX50R_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	GFX50R_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	GFX50R_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	GFX50R_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	GFX50R_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	GFX50R_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	GFX50R_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	GFX50R_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	GFX50R_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	GFX50R_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	GFX50R_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	GFX50R_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	GFX50R_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FN3   = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FN4   = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FN5   = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	GFX50R_FUNCTIONLOCK_CATEGORY2_FN10  = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
};

enum{
	GFX50R_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	GFX50R_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	GFX50R_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
};

enum{
	GFX50R_ON  = SDK_ON,
	GFX50R_OFF = SDK_OFF,
};

enum{
	GFX50R_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	GFX50R_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	GFX50R_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	GFX50R_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	GFX50R_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	GFX50R_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	GFX50R_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	GFX50R_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	GFX50R_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	GFX50R_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	GFX50R_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	GFX50R_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	GFX50R_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	GFX50R_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	GFX50R_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	GFX50R_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	GFX50R_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	GFX50R_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	GFX50R_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	GFX50R_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	GFX50R_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	GFX50R_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	GFX50R_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	GFX50R_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	GFX50R_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	GFX50R_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	GFX50R_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	GFX50R_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	GFX50R_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	GFX50R_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	GFX50R_NEW_FOLDER                   = SDK_NEW_FOLDER,
	GFX50R_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	GFX50R_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	GFX50R_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	GFX50R_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	GFX50R_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	GFX50R_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	GFX50R_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	GFX50R_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	GFX50R_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	GFX50R_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	GFX50R_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	GFX50R_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	GFX50R_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	GFX50R_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	GFX50R_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	GFX50R_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	GFX50R_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	GFX50R_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	GFX50R_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	GFX50R_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	GFX50R_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	GFX50R_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	GFX50R_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	GFX50R_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};
