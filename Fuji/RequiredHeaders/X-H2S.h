///////////////////////////////////////////////////
//  X-H2S.h
//     Version 1.0.0.0
//
//     Copyright (C) 2022 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-H2S
//
enum {
	XH2S_API_PARAM_CapHighFrequencyFlickerlessMode = 2,
	XH2S_API_PARAM_SetHighFrequencyFlickerlessMode = 1,
	XH2S_API_PARAM_GetHighFrequencyFlickerlessMode = 1,
	XH2S_API_PARAM_CapImageSize                = 2,
	XH2S_API_PARAM_SetImageSize                = 1,
	XH2S_API_PARAM_GetImageSize                = 1,
	XH2S_API_PARAM_CapSharpness                = 2,
	XH2S_API_PARAM_SetSharpness                = 1,
	XH2S_API_PARAM_GetSharpness                = 1,
	XH2S_API_PARAM_CapColorMode                = 2,
	XH2S_API_PARAM_SetColorMode                = 1,
	XH2S_API_PARAM_GetColorMode                = 1,
	XH2S_API_PARAM_CapFilmSimulationMode       = 2,
	XH2S_API_PARAM_SetFilmSimulationMode       = 1,
	XH2S_API_PARAM_GetFilmSimulationMode       = 1,
	XH2S_API_PARAM_CapColorSpace               = 2,
	XH2S_API_PARAM_SetColorSpace               = 1,
	XH2S_API_PARAM_GetColorSpace               = 1,
	XH2S_API_PARAM_CapImageQuality             = 2,
	XH2S_API_PARAM_SetImageQuality             = 1,
	XH2S_API_PARAM_GetImageQuality             = 1,
	XH2S_API_PARAM_CapRAWCompression           = 2,
	XH2S_API_PARAM_SetRAWCompression           = 1,
	XH2S_API_PARAM_GetRAWCompression           = 1,
	XH2S_API_PARAM_CapGrainEffect              = 2,
	XH2S_API_PARAM_SetGrainEffect              = 1,
	XH2S_API_PARAM_GetGrainEffect              = 1,
	XH2S_API_PARAM_CapShadowing                = 2,
	XH2S_API_PARAM_SetShadowing                = 1,
	XH2S_API_PARAM_GetShadowing                = 1,
	XH2S_API_PARAM_CapNoiseReduction           = 2,
	XH2S_API_PARAM_SetNoiseReduction           = 1,
	XH2S_API_PARAM_GetNoiseReduction           = 1,
	XH2S_API_PARAM_CapFaceDetectionMode        = 2,
	XH2S_API_PARAM_SetFaceDetectionMode        = 1,
	XH2S_API_PARAM_GetFaceDetectionMode        = 1,
	XH2S_API_PARAM_CapEyeAFMode                = 2,
	XH2S_API_PARAM_SetEyeAFMode                = 1,
	XH2S_API_PARAM_GetEyeAFMode                = 1,
	XH2S_API_PARAM_SetMacroMode                = -1,
	XH2S_API_PARAM_GetMacroMode                = -1,
	XH2S_API_PARAM_CapFocusLimiter             = -1,
	XH2S_API_PARAM_SetFocusLimiter             = -1,
	XH2S_API_PARAM_GetFocusLimiter             = -1,
	XH2S_API_PARAM_CapHighLightTone            = 2,
	XH2S_API_PARAM_SetHighLightTone            = 1,
	XH2S_API_PARAM_GetHighLightTone            = 1,
	XH2S_API_PARAM_CapShadowTone               = 2,
	XH2S_API_PARAM_SetShadowTone               = 1,
	XH2S_API_PARAM_GetShadowTone               = 1,
	XH2S_API_PARAM_CapLongExposureNR           = 2,
	XH2S_API_PARAM_SetLongExposureNR           = 1,
	XH2S_API_PARAM_GetLongExposureNR           = 1,
	XH2S_API_PARAM_CapFullTimeManualFocus      = 2,
	XH2S_API_PARAM_SetFullTimeManualFocus      = 1,
	XH2S_API_PARAM_GetFullTimeManualFocus      = 1,
	XH2S_API_PARAM_SetFocusMode                = 1,
	XH2S_API_PARAM_GetFocusMode                = 1,
	XH2S_API_PARAM_CapAFMode                   = 3,
	XH2S_API_PARAM_SetAFMode                   = 2,
	XH2S_API_PARAM_GetAFMode                   = 2,
	XH2S_API_PARAM_CapFocusPoints              = 2,
	XH2S_API_PARAM_SetFocusPoints              = 1,
	XH2S_API_PARAM_GetFocusPoints              = 1,
	XH2S_API_PARAM_CapFocusArea                = 3,
	XH2S_API_PARAM_SetFocusArea                = 2,
	XH2S_API_PARAM_GetFocusArea                = 2,
	XH2S_API_PARAM_CapFocusMode                = 2,
	XH2S_API_PARAM_CapAFStatus                 = 2,
	XH2S_API_PARAM_GetAFStatus                 = 1,
	XH2S_API_PARAM_CapShutterPriorityMode      = 3,
	XH2S_API_PARAM_SetShutterPriorityMode      = 2,
	XH2S_API_PARAM_GetShutterPriorityMode      = 2,
	XH2S_API_PARAM_CapInstantAFMode            = 2,
	XH2S_API_PARAM_SetInstantAFMode            = 1,
	XH2S_API_PARAM_GetInstantAFMode            = 1,
	XH2S_API_PARAM_CapPreAFMode                = 2,
	XH2S_API_PARAM_SetPreAFMode                = 1,
	XH2S_API_PARAM_GetPreAFMode                = 1,
	XH2S_API_PARAM_CapAFIlluminator            = 2,
	XH2S_API_PARAM_SetAFIlluminator            = 1,
	XH2S_API_PARAM_GetAFIlluminator            = 1,
	XH2S_API_PARAM_CapLensISSwitch             = 2,
	XH2S_API_PARAM_SetLensISSwitch             = 1,
	XH2S_API_PARAM_GetLensISSwitch             = 1,
	XH2S_API_PARAM_CapISMode                   = 2,
	XH2S_API_PARAM_SetISMode                   = 1,
	XH2S_API_PARAM_GetISMode                   = 1,
	XH2S_API_PARAM_CapLMOMode                  = 2,
	XH2S_API_PARAM_SetLMOMode                  = 1,
	XH2S_API_PARAM_GetLMOMode                  = 1,
	XH2S_API_PARAM_SetWhiteBalanceMode         = -1,
	XH2S_API_PARAM_GetWhiteBalanceMode         = -1,
	XH2S_API_PARAM_CapWhiteBalanceTune         = 5,
	XH2S_API_PARAM_SetWhiteBalanceTune         = 3,
	XH2S_API_PARAM_GetWhiteBalanceTune         = 3,
	XH2S_API_PARAM_CapCaptureDelay             = 2,
	XH2S_API_PARAM_SetCaptureDelay             = 1,
	XH2S_API_PARAM_GetCaptureDelay             = 1,
	XH2S_API_PARAM_CapFocusPos                 = 2,
	XH2S_API_PARAM_SetFocusPos                 = 1,
	XH2S_API_PARAM_GetFocusPos                 = 1,
	XH2S_API_PARAM_CapMFAssistMode             = 2,
	XH2S_API_PARAM_SetMFAssistMode             = 1,
	XH2S_API_PARAM_GetMFAssistMode             = 1,
	XH2S_API_PARAM_CapFocusCheckMode           = 2,
	XH2S_API_PARAM_SetFocusCheckMode           = 1,
	XH2S_API_PARAM_GetFocusCheckMode           = 1,
	XH2S_API_PARAM_CapInterlockAEAFArea        = 2,
	XH2S_API_PARAM_SetInterlockAEAFArea        = 1,
	XH2S_API_PARAM_GetInterlockAEAFArea        = 1,
	XH2S_API_PARAM_StartLiveView               = 0,
	XH2S_API_PARAM_StopLiveView                = 0,
	XH2S_API_PARAM_CapLiveViewImageQuality     = 2,
	XH2S_API_PARAM_SetLiveViewImageQuality     = 1,
	XH2S_API_PARAM_GetLiveViewImageQuality     = 1,
	XH2S_API_PARAM_CapLiveViewImageSize        = 2,
	XH2S_API_PARAM_SetLiveViewImageSize        = 1,
	XH2S_API_PARAM_GetLiveViewImageSize        = 1,
	XH2S_API_PARAM_CapThroughImageZoom         = 2,
	XH2S_API_PARAM_SetThroughImageZoom         = 1,
	XH2S_API_PARAM_GetThroughImageZoom         = 1,
	XH2S_API_PARAM_SetDateTime                 = 6,
	XH2S_API_PARAM_GetDateTime                 = 6,
	XH2S_API_PARAM_CapDateTimeDispFormat       = 2,
	XH2S_API_PARAM_SetDateTimeDispFormat       = 1,
	XH2S_API_PARAM_GetDateTimeDispFormat       = 1,
	XH2S_API_PARAM_CapWorldClock               = 2,
	XH2S_API_PARAM_SetWorldClock               = 1,
	XH2S_API_PARAM_GetWorldClock               = 1,
	XH2S_API_PARAM_CapTimeDifference           = 4,
	XH2S_API_PARAM_SetTimeDifference           = 2,
	XH2S_API_PARAM_GetTimeDifference           = 2,
	XH2S_API_PARAM_CapResetSetting             = 2,
	XH2S_API_PARAM_ResetSetting                = 1,
	XH2S_API_PARAM_SetSilentMode               = -1,
	XH2S_API_PARAM_GetSilentMode               = -1,
	XH2S_API_PARAM_SetBeep                     = -1,
	XH2S_API_PARAM_GetBeep                     = -1,
	XH2S_API_PARAM_SetFunctionLock             = 1,
	XH2S_API_PARAM_GetFunctionLock             = 1,
	XH2S_API_PARAM_CapFunctionLock             = 2,
	XH2S_API_PARAM_SetFunctionLockCategory     = 3,
	XH2S_API_PARAM_GetFunctionLockCategory     = 3,
	XH2S_API_PARAM_CapFunctionLockCategory     = 6,
	XH2S_API_PARAM_SetComment                  = 1,
	XH2S_API_PARAM_GetComment                  = 1,
	XH2S_API_PARAM_SetCopyright                = 2,
	XH2S_API_PARAM_GetCopyright                = 2,
	XH2S_API_PARAM_SetArtist                   = 1,
	XH2S_API_PARAM_GetArtist                   = 1,
	XH2S_API_PARAM_SetFilenamePrefix           = 2,
	XH2S_API_PARAM_GetFilenamePrefix           = 2,
	XH2S_API_PARAM_SetFoldernameSuffix         = 3,
	XH2S_API_PARAM_GetFoldernameSuffix         = 3,
	XH2S_API_PARAM_GetFoldernameList           = 4,
	XH2S_API_PARAM_CheckBatteryInfo            = 8,
	XH2S_API_PARAM_CapFrameNumberSequence      = 2,
	XH2S_API_PARAM_SetFrameNumberSequence      = 1,
	XH2S_API_PARAM_GetFrameNumberSequence      = 1,
	XH2S_API_PARAM_SetUSBMode                  = -1,
	XH2S_API_PARAM_GetUSBMode                  = -1,
	XH2S_API_PARAM_CapFormatMemoryCard         = 2,
	XH2S_API_PARAM_FormatMemoryCard            = 1,
	XH2S_API_PARAM_SetMediaRecord              = 1,
	XH2S_API_PARAM_GetMediaRecord              = 1,
	XH2S_API_PARAM_GetMediaCapacity            = 5,
	XH2S_API_PARAM_GetMediaStatus              = 2,
	XH2S_API_PARAM_GetShutterCount             = 3,
	XH2S_API_PARAM_GetShutterCountEx           = 3,
	XH2S_API_PARAM_SetSensorCleanTiming        = -1,
	XH2S_API_PARAM_GetSensorCleanTiming        = -1,
	XH2S_API_PARAM_SetPreviewTime              = -1,
	XH2S_API_PARAM_GetPreviewTime              = -1,
	XH2S_API_PARAM_SetEVFDispAutoRotate        = -1,
	XH2S_API_PARAM_GetEVFDispAutoRotate        = -1,
	XH2S_API_PARAM_SetDispMMode                = 1,
	XH2S_API_PARAM_GetDispMMode                = 1,
	XH2S_API_PARAM_CapExposurePreview          = 2,
	XH2S_API_PARAM_SetExposurePreview          = 1,
	XH2S_API_PARAM_GetExposurePreview          = 1,
	XH2S_API_PARAM_SetDispBrightness           = -1,
	XH2S_API_PARAM_GetDispBrightness           = -1,
	XH2S_API_PARAM_CapFrameGuideMode           = 2,
	XH2S_API_PARAM_SetFrameGuideMode           = 1,
	XH2S_API_PARAM_GetFrameGuideMode           = 1,
	XH2S_API_PARAM_SetFrameGuideGridInfo       = 2,
	XH2S_API_PARAM_GetFrameGuideGridInfo       = 2,
	XH2S_API_PARAM_SetAutoImageRotation        = -1,
	XH2S_API_PARAM_GetAutoImageRotation        = -1,
	XH2S_API_PARAM_CapFocusScaleUnit           = 2,
	XH2S_API_PARAM_SetFocusScaleUnit           = 1,
	XH2S_API_PARAM_GetFocusScaleUnit           = 1,
	XH2S_API_PARAM_CapCustomDispInfo           = 4,
	XH2S_API_PARAM_SetCustomDispInfo           = 2,
	XH2S_API_PARAM_GetCustomDispInfo           = 2,
	XH2S_API_PARAM_CapViewMode                 = 3,
	XH2S_API_PARAM_SetViewMode                 = 2,
	XH2S_API_PARAM_GetViewMode                 = 2,
	XH2S_API_PARAM_SetDispInfoMode             = -1,
	XH2S_API_PARAM_GetDispInfoMode             = -1,
	XH2S_API_PARAM_SetDispChroma               = -1,
	XH2S_API_PARAM_GetDispChroma               = -1,
	XH2S_API_PARAM_CapCustomAutoPowerOff       = 2,
	XH2S_API_PARAM_SetCustomAutoPowerOff       = 1,
	XH2S_API_PARAM_GetCustomAutoPowerOff       = 1,
	XH2S_API_PARAM_CapCustomStudioPowerSave    = -1,
	XH2S_API_PARAM_SetCustomStudioPowerSave    = -1,
	XH2S_API_PARAM_GetCustomStudioPowerSave    = -1,
	XH2S_API_PARAM_SetFunctionButton           = -1,
	XH2S_API_PARAM_GetFunctionButton           = -1,
	XH2S_API_PARAM_SetISODialHn                = -1,
	XH2S_API_PARAM_GetISODialHn                = -1,
	XH2S_API_PARAM_CapLockButtonMode           = 2,
	XH2S_API_PARAM_SetLockButtonMode           = 1,
	XH2S_API_PARAM_GetLockButtonMode           = 1,
	XH2S_API_PARAM_SetAFLockMode               = -1,
	XH2S_API_PARAM_GetAFLockMode               = -1,
	XH2S_API_PARAM_SetMicJackMode              = -1,
	XH2S_API_PARAM_GetMicJackMode              = -1,
	XH2S_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XH2S_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XH2S_API_PARAM_SetCrossKeyAssign           = -1,
	XH2S_API_PARAM_GetCrossKeyAssign           = -1,
	XH2S_API_PARAM_CapWideDynamicRange         = 2,
	XH2S_API_PARAM_SetWideDynamicRange         = 1,
	XH2S_API_PARAM_GetWideDynamicRange         = 1,
	XH2S_API_PARAM_CapClarityMode              = 2,
	XH2S_API_PARAM_SetClarityMode              = 1,
	XH2S_API_PARAM_GetClarityMode              = 1,
	XH2S_API_PARAM_SetBlackImageTone           = -1,
	XH2S_API_PARAM_GetBlackImageTone           = -1,
	XH2S_API_PARAM_GetTNumber                  = 1,
	XH2S_API_PARAM_CapCropMode                 = 2,
	XH2S_API_PARAM_SetCropMode                 = 1,
	XH2S_API_PARAM_GetCropMode                 = 2,
	XH2S_API_PARAM_GetCropAreaFrameInfo        = 1,
	XH2S_API_PARAM_CapRAWOutputDepth           = -1,
	XH2S_API_PARAM_SetRAWOutputDepth           = -1,
	XH2S_API_PARAM_GetRAWOutputDepth           = -1,
	XH2S_API_PARAM_CapSmoothSkinEffect         = -1,
	XH2S_API_PARAM_SetSmoothSkinEffect         = -1,
	XH2S_API_PARAM_GetSmoothSkinEffect         = -1,
	XH2S_API_PARAM_GetDetectedFaceFrame        = 2,
	XH2S_API_PARAM_SetDetectedFaceFrame        = 1,
    XH2S_API_PARAM_SetCustomWBArea             = 2,
    XH2S_API_PARAM_GetCustomWBArea             = 2,
	XH2S_API_PARAM_CapColorChromeBlue          = 2,
	XH2S_API_PARAM_SetColorChromeBlue          = 1,
	XH2S_API_PARAM_GetColorChromeBlue          = 1,
	XH2S_API_PARAM_CapMonochromaticColor       = 4,
	XH2S_API_PARAM_SetMonochromaticColor       = 2,
	XH2S_API_PARAM_GetMonochromaticColor       = 2,
	XH2S_API_PARAM_CapFocusLimiterPos          = 4,
	XH2S_API_PARAM_SetFocusLimiterPos          = 2,
	XH2S_API_PARAM_GetFocusLimiterIndicator    = 1,
	XH2S_API_PARAM_GetFocusLimiterRange        = 2,
	XH2S_API_PARAM_CapFocusLimiterMode         = 2,
	XH2S_API_PARAM_SetFocusLimiterMode         = 1,
	XH2S_API_PARAM_GetFocusLimiterMode         = 1,
	XH2S_API_PARAM_GetCommandDialStatus        = 4,
	XH2S_API_PARAM_CapPerformanceSettings      = 2,
	XH2S_API_PARAM_SetPerformanceSettings      = 1,
	XH2S_API_PARAM_GetPerformanceSettings      = 1,
	XH2S_API_PARAM_CapMicLineSetting           = 2,
	XH2S_API_PARAM_SetMicLineSetting           = 1,
	XH2S_API_PARAM_GetMicLineSetting           = 1,
	XH2S_API_PARAM_CapCustomSettingAutoUpdate  = 2,
	XH2S_API_PARAM_SetCustomSettingAutoUpdate  = 1,
	XH2S_API_PARAM_GetCustomSettingAutoUpdate  = 1,
    XH2S_API_PARAM_CapPixelShiftSettings       = -1,
    XH2S_API_PARAM_SetPixelShiftSettings       = -1,
    XH2S_API_PARAM_GetPixelShiftSettings       = -1,
    XH2S_API_PARAM_CapImageFormat              = 2,
    XH2S_API_PARAM_SetImageFormat              = 1,
    XH2S_API_PARAM_GetImageFormat              = 1,
	XH2S_API_PARAM_CapSubjectDetectionMode     = 2,
	XH2S_API_PARAM_SetSubjectDetectionMode     = 1,
	XH2S_API_PARAM_GetSubjectDetectionMode     = 1,
	XH2S_API_PARAM_CapCropZoom                 = -1,
	XH2S_API_PARAM_SetCropZoom                 = -1,
	XH2S_API_PARAM_GetCropZoom                 = -1,
	XH2S_API_PARAM_CapZoomOperation            = 4,
	XH2S_API_PARAM_SetZoomOperation            = 3,
	XH2S_API_PARAM_CapFocusOperation           = 4,
	XH2S_API_PARAM_SetFocusOperation           = 3,
	XH2S_API_PARAM_CapZoomSpeed                = 2,
	XH2S_API_PARAM_SetZoomSpeed                = 1,
	XH2S_API_PARAM_GetZoomSpeed                = 1,
	XH2S_API_PARAM_CapFocusSpeed               = 2,
	XH2S_API_PARAM_SetFocusSpeed               = 1,
	XH2S_API_PARAM_GetFocusSpeed               = 1,
	XH2S_API_PARAM_CapLiveViewStatus           = 2,
	XH2S_API_PARAM_GetLiveViewStatus           = 1,
	XH2S_API_PARAM_CapLiveViewMode             = 2,
	XH2S_API_PARAM_SetLiveViewMode             = 1,
	XH2S_API_PARAM_GetLiveViewMode             = 1,
	XH2S_API_PARAM_CapLiveViewImageRatio       = 2,
	XH2S_API_PARAM_SetLiveViewImageRatio       = 1,
	XH2S_API_PARAM_GetLiveViewImageRatio       = 1,
	XH2S_API_PARAM_CapSummerTime               = 2,
	XH2S_API_PARAM_SetSummerTime               = 1,
	XH2S_API_PARAM_GetSummerTime               = 1,
	XH2S_API_PARAM_CapFanSetting               = 2,
	XH2S_API_PARAM_SetFanSetting               = 1,
	XH2S_API_PARAM_GetFanSetting               = 1,
	XH2S_API_PARAM_CapElectronicLevelSetting   = 2,
	XH2S_API_PARAM_SetElectronicLevelSetting   = 1,
	XH2S_API_PARAM_GetElectronicLevelSetting   = 1,
	XH2S_API_PARAM_CapApertureUnit             = 2,
	XH2S_API_PARAM_SetApertureUnit             = 1,
	XH2S_API_PARAM_GetApertureUnit             = 1,
};

//
// API_CODE for X-H2S
//
enum {
	XH2S_API_CODE_CapHighFrequencyFlickerlessMode = API_CODE_CapHighFrequencyFlickerlessMode,
	XH2S_API_CODE_SetHighFrequencyFlickerlessMode = API_CODE_SetHighFrequencyFlickerlessMode,
	XH2S_API_CODE_GetHighFrequencyFlickerlessMode = API_CODE_GetHighFrequencyFlickerlessMode,
	XH2S_API_CODE_CapImageSize                = API_CODE_CapImageSize,
	XH2S_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XH2S_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XH2S_API_CODE_CapSharpness                = API_CODE_CapSharpness,
	XH2S_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XH2S_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XH2S_API_CODE_CapColorMode                = API_CODE_CapColorMode,
	XH2S_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XH2S_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XH2S_API_CODE_CapFilmSimulationMode       = API_CODE_CapFilmSimulationMode,
	XH2S_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XH2S_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XH2S_API_CODE_CapColorSpace               = API_CODE_CapColorSpace,
	XH2S_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XH2S_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XH2S_API_CODE_CapImageQuality             = API_CODE_CapImageQuality,
	XH2S_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XH2S_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XH2S_API_CODE_CapRAWCompression           = API_CODE_CapRAWCompression,
	XH2S_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XH2S_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XH2S_API_CODE_CapGrainEffect              = API_CODE_CapGrainEffect,
	XH2S_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XH2S_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XH2S_API_CODE_CapShadowing                = API_CODE_CapShadowing,
	XH2S_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XH2S_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XH2S_API_CODE_CapNoiseReduction           = API_CODE_CapNoiseReduction,
	XH2S_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XH2S_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XH2S_API_CODE_CapFaceDetectionMode        = API_CODE_CapFaceDetectionMode,
	XH2S_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XH2S_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XH2S_API_CODE_CapEyeAFMode                = API_CODE_CapEyeAFMode,
	XH2S_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XH2S_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XH2S_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XH2S_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XH2S_API_CODE_CapHighLightTone            = API_CODE_CapHighLightTone,
	XH2S_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XH2S_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XH2S_API_CODE_CapShadowTone               = API_CODE_CapShadowTone,
	XH2S_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XH2S_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XH2S_API_CODE_CapLongExposureNR           = API_CODE_CapLongExposureNR,
	XH2S_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XH2S_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XH2S_API_CODE_CapFullTimeManualFocus      = API_CODE_CapFullTimeManualFocus,
	XH2S_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XH2S_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XH2S_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XH2S_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XH2S_API_CODE_CapAFMode                   = API_CODE_CapAFMode,
	XH2S_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XH2S_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XH2S_API_CODE_CapFocusPoints              = API_CODE_CapFocusPoints,
	XH2S_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XH2S_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XH2S_API_CODE_CapFocusArea                = API_CODE_CapFocusArea,
	XH2S_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XH2S_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XH2S_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XH2S_API_CODE_CapAFStatus                 = API_CODE_CapAFStatus,
	XH2S_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XH2S_API_CODE_CapShutterPriorityMode      = API_CODE_CapShutterPriorityMode,
	XH2S_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XH2S_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XH2S_API_CODE_CapInstantAFMode            = API_CODE_CapInstantAFMode,
	XH2S_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XH2S_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XH2S_API_CODE_CapPreAFMode                = API_CODE_CapPreAFMode,
	XH2S_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XH2S_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XH2S_API_CODE_CapAFIlluminator            = API_CODE_CapAFIlluminator,
	XH2S_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XH2S_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XH2S_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XH2S_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XH2S_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XH2S_API_CODE_CapISMode                   = API_CODE_CapISMode,
	XH2S_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XH2S_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XH2S_API_CODE_CapLMOMode                  = API_CODE_CapLMOMode,
	XH2S_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XH2S_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XH2S_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XH2S_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XH2S_API_CODE_CapWhiteBalanceTune         = API_CODE_CapWhiteBalanceTune,
	XH2S_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XH2S_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XH2S_API_CODE_CapCaptureDelay             = API_CODE_CapCaptureDelay,
	XH2S_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XH2S_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XH2S_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XH2S_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XH2S_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XH2S_API_CODE_CapMFAssistMode             = API_CODE_CapMFAssistMode,
	XH2S_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XH2S_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XH2S_API_CODE_CapFocusCheckMode           = API_CODE_CapFocusCheckMode,
	XH2S_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XH2S_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XH2S_API_CODE_CapInterlockAEAFArea        = API_CODE_CapInterlockAEAFArea,
	XH2S_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XH2S_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XH2S_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XH2S_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XH2S_API_CODE_CapLiveViewImageQuality     = API_CODE_CapLiveViewImageQuality,
	XH2S_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XH2S_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XH2S_API_CODE_CapLiveViewImageSize        = API_CODE_CapLiveViewImageSize,
	XH2S_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XH2S_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XH2S_API_CODE_CapThroughImageZoom         = API_CODE_CapThroughImageZoom,
	XH2S_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XH2S_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XH2S_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XH2S_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XH2S_API_CODE_CapDateTimeDispFormat       = API_CODE_CapDateTimeDispFormat,
	XH2S_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XH2S_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XH2S_API_CODE_CapWorldClock               = API_CODE_CapWorldClock,
	XH2S_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XH2S_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XH2S_API_CODE_CapTimeDifference           = API_CODE_CapTimeDifference,
	XH2S_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XH2S_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XH2S_API_CODE_CapResetSetting             = API_CODE_CapResetSetting,
	XH2S_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XH2S_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XH2S_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XH2S_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XH2S_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XH2S_API_CODE_CapFunctionLock             = API_CODE_CapFunctionLock,
	XH2S_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XH2S_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XH2S_API_CODE_CapFunctionLockCategory     = API_CODE_CapFunctionLockCategory,
	XH2S_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XH2S_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XH2S_API_CODE_SetComment                  = API_CODE_SetComment,
	XH2S_API_CODE_GetComment                  = API_CODE_GetComment,
	XH2S_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XH2S_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XH2S_API_CODE_SetArtist                   = API_CODE_SetArtist,
	XH2S_API_CODE_GetArtist                   = API_CODE_GetArtist,
	XH2S_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XH2S_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XH2S_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XH2S_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XH2S_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XH2S_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XH2S_API_CODE_CapFrameNumberSequence      = API_CODE_CapFrameNumberSequence,
	XH2S_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XH2S_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XH2S_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XH2S_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XH2S_API_CODE_CapFormatMemoryCard         = API_CODE_CapFormatMemoryCard,
	XH2S_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XH2S_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XH2S_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XH2S_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XH2S_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XH2S_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XH2S_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XH2S_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XH2S_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XH2S_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XH2S_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XH2S_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XH2S_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XH2S_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XH2S_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XH2S_API_CODE_CapExposurePreview          = API_CODE_CapExposurePreview,
	XH2S_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XH2S_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XH2S_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XH2S_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XH2S_API_CODE_CapFrameGuideMode           = API_CODE_CapFrameGuideMode,
	XH2S_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XH2S_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XH2S_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XH2S_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XH2S_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XH2S_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XH2S_API_CODE_CapFocusScaleUnit           = API_CODE_CapFocusScaleUnit,
	XH2S_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XH2S_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XH2S_API_CODE_CapCustomDispInfo           = API_CODE_CapCustomDispInfo,
	XH2S_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XH2S_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XH2S_API_CODE_CapViewMode                 = API_CODE_CapViewMode,
	XH2S_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XH2S_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XH2S_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XH2S_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XH2S_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XH2S_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XH2S_API_CODE_CapCustomAutoPowerOff       = API_CODE_CapCustomAutoPowerOff,
	XH2S_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XH2S_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XH2S_API_CODE_CapCustomStudioPowerSave    = API_CODE_CapCustomStudioPowerSave,
	XH2S_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XH2S_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XH2S_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XH2S_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XH2S_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XH2S_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XH2S_API_CODE_CapLockButtonMode           = API_CODE_CapLockButtonMode,
	XH2S_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XH2S_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XH2S_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XH2S_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XH2S_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XH2S_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XH2S_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XH2S_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XH2S_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XH2S_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XH2S_API_CODE_CapWideDynamicRange         = API_CODE_CapWideDynamicRange,
	XH2S_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XH2S_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XH2S_API_CODE_CapClarityMode              = API_CODE_CapClarityMode,
	XH2S_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XH2S_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XH2S_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XH2S_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XH2S_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XH2S_API_CODE_CapCropMode                 = API_CODE_CapCropMode,
	XH2S_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XH2S_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XH2S_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XH2S_API_CODE_CapRAWOutputDepth           = API_CODE_CapRAWOutputDepth,
	XH2S_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XH2S_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XH2S_API_CODE_CapSmoothSkinEffect         = API_CODE_CapSmoothSkinEffect,
	XH2S_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XH2S_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XH2S_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XH2S_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XH2S_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XH2S_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XH2S_API_CODE_CapColorChromeBlue          = API_CODE_CapColorChromeBlue,
	XH2S_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XH2S_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XH2S_API_CODE_CapMonochromaticColor       = API_CODE_CapMonochromaticColor,
	XH2S_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XH2S_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XH2S_API_CODE_CapFocusLimiterPos          = API_CODE_CapFocusLimiterPos,
	XH2S_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XH2S_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XH2S_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XH2S_API_CODE_CapFocusLimiterMode         = API_CODE_CapFocusLimiterMode,
	XH2S_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XH2S_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
	XH2S_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
	XH2S_API_CODE_CapPerformanceSettings      = API_CODE_CapPerformanceSettings,
	XH2S_API_CODE_SetPerformanceSettings      = API_CODE_SetPerformanceSettings,
	XH2S_API_CODE_GetPerformanceSettings      = API_CODE_GetPerformanceSettings,
	XH2S_API_CODE_CapMicLineSetting           = API_CODE_CapMicLineSetting,
	XH2S_API_CODE_SetMicLineSetting           = API_CODE_SetMicLineSetting,
	XH2S_API_CODE_GetMicLineSetting           = API_CODE_GetMicLineSetting,
	XH2S_API_CODE_CapCustomSettingAutoUpdate  = API_CODE_CapCustomSettingAutoUpdate,
	XH2S_API_CODE_SetCustomSettingAutoUpdate  = API_CODE_SetCustomSettingAutoUpdate,
	XH2S_API_CODE_GetCustomSettingAutoUpdate  = API_CODE_GetCustomSettingAutoUpdate,
    XH2S_API_CODE_CapPixelShiftSettings       = API_CODE_CapPixelShiftSettings,
    XH2S_API_CODE_SetPixelShiftSettings       = API_CODE_SetPixelShiftSettings,
    XH2S_API_CODE_GetPixelShiftSettings       = API_CODE_GetPixelShiftSettings,
    XH2S_API_CODE_CapImageFormat              = API_CODE_CapImageFormat,
    XH2S_API_CODE_SetImageFormat              = API_CODE_SetImageFormat,
    XH2S_API_CODE_GetImageFormat              = API_CODE_GetImageFormat,
	XH2S_API_CODE_CapSubjectDetectionMode     = API_CODE_CapSubjectDetectionMode,
	XH2S_API_CODE_SetSubjectDetectionMode     = API_CODE_SetSubjectDetectionMode,
	XH2S_API_CODE_GetSubjectDetectionMode     = API_CODE_GetSubjectDetectionMode,
	XH2S_API_CODE_CapCropZoom                 = API_CODE_CapCropZoom,
	XH2S_API_CODE_SetCropZoom                 = API_CODE_SetCropZoom,
	XH2S_API_CODE_GetCropZoom                 = API_CODE_GetCropZoom,
	XH2S_API_CODE_CapZoomOperation            = API_CODE_CapZoomOperation,
	XH2S_API_CODE_SetZoomOperation            = API_CODE_SetZoomOperation,
	XH2S_API_CODE_CapFocusOperation           = API_CODE_CapFocusOperation,
	XH2S_API_CODE_SetFocusOperation           = API_CODE_SetFocusOperation,
	XH2S_API_CODE_CapZoomSpeed                = API_CODE_CapZoomSpeed,
	XH2S_API_CODE_SetZoomSpeed                = API_CODE_SetZoomSpeed,
	XH2S_API_CODE_GetZoomSpeed                = API_CODE_GetZoomSpeed,
	XH2S_API_CODE_CapFocusSpeed               = API_CODE_CapFocusSpeed,
	XH2S_API_CODE_SetFocusSpeed               = API_CODE_SetFocusSpeed,
	XH2S_API_CODE_GetFocusSpeed               = API_CODE_GetFocusSpeed,
	XH2S_API_CODE_CapLiveViewStatus           = API_CODE_CapLiveViewStatus,
	XH2S_API_CODE_GetLiveViewStatus           = API_CODE_GetLiveViewStatus,
	XH2S_API_CODE_CapLiveViewMode             = API_CODE_CapLiveViewMode,
	XH2S_API_CODE_SetLiveViewMode             = API_CODE_SetLiveViewMode,
	XH2S_API_CODE_GetLiveViewMode             = API_CODE_GetLiveViewMode,
	XH2S_API_CODE_CapLiveViewImageRatio       = API_CODE_CapLiveViewImageRatio,
	XH2S_API_CODE_SetLiveViewImageRatio       = API_CODE_SetLiveViewImageRatio,
	XH2S_API_CODE_GetLiveViewImageRatio       = API_CODE_GetLiveViewImageRatio,
	XH2S_API_CODE_CapSummerTime               = API_CODE_CapSummerTime,
	XH2S_API_CODE_SetSummerTime               = API_CODE_SetSummerTime,
	XH2S_API_CODE_GetSummerTime               = API_CODE_GetSummerTime,
	XH2S_API_CODE_CapFanSetting               = API_CODE_CapFanSetting,
	XH2S_API_CODE_SetFanSetting               = API_CODE_SetFanSetting,
	XH2S_API_CODE_GetFanSetting               = API_CODE_GetFanSetting,
	XH2S_API_CODE_CapElectronicLevelSetting   = API_CODE_CapElectronicLevelSetting,
	XH2S_API_CODE_SetElectronicLevelSetting   = API_CODE_SetElectronicLevelSetting,
	XH2S_API_CODE_GetElectronicLevelSetting   = API_CODE_GetElectronicLevelSetting,
	XH2S_API_CODE_CapApertureUnit             = API_CODE_CapApertureUnit,
	XH2S_API_CODE_SetApertureUnit             = API_CODE_SetApertureUnit,
	XH2S_API_CODE_GetApertureUnit             = API_CODE_GetApertureUnit,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XH2S_FocusArea, *PXH2S_FocusArea;


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
typedef  SDK_ISOAuto     XH2S_ISOAuto, *PXH2S_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XH2S_FrameGuideGridInfo, *PXH2S_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XH2S_FOCUS_POS_CAP, *PSDK_XH2S_FOCUS_POS_CAP;
typedef  SDK_XH2S_FOCUS_POS_CAP     XH2S_FOCUS_POS_CAP, *PXH2S_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XH2S_FOLDER_INFO, *PXH2S_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XH2S_CropAreaFrameInfo, *PXH2S_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XH2S_FaceFrameInfo, *PXH2S_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XH2S_CustomWBArea, *PXH2S_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XH2S_FOCUS_LIMITER_INDICATOR, *PXH2S_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XH2S_FOCUS_LIMITER, *PXH2S_FOCUS_LIMITER;


// Image Size
enum{
	XH2S_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XH2S_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XH2S_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XH2S_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XH2S_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XH2S_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XH2S_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XH2S_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XH2S_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XH2S_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XH2S_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XH2S_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XH2S_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XH2S_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XH2S_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XH2S_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XH2S_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XH2S_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XH2S_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XH2S_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XH2S_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XH2S_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XH2S_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XH2S_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XH2S_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XH2S_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XH2S_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XH2S_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// Image Format
enum{
	XH2S_IMAGEFORMAT_JPEG         = SDK_IMAGEFORMAT_JPEG,
	XH2S_IMAGEFORMAT_HEIF         = SDK_IMAGEFORMAT_HEIF,
};

// RAW Image Quality
enum{
	XH2S_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XH2S_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Mode
enum{
	XH2S_LIVEVIEW_MODE1           = SDK_LIVEVIEW_MODE1,
	XH2S_LIVEVIEW_MODE2           = SDK_LIVEVIEW_MODE2,
};

// LiveView Ratio
enum{
	XH2S_LIVEVIEW_RATIO_FIXED     = SDK_LIVEVIEW_RATIO_FIXED,
	XH2S_LIVEVIEW_RATIO_VARIABLE  = SDK_LIVEVIEW_RATIO_VARIABLE,
};

// LiveView Image Quality
enum{
	XH2S_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XH2S_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XH2S_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XH2S_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XH2S_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XH2S_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XH2S_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XH2S_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XH2S_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XH2S_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XH2S_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XH2S_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XH2S_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XH2S_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XH2S_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XH2S_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XH2S_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XH2S_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XH2S_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XH2S_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XH2S_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XH2S_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XH2S_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XH2S_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XH2S_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XH2S_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XH2S_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
	XH2S_THROUGH_ZOOM_120    = SDK_THROUGH_ZOOM_120,
};

// Color Space
enum{
	XH2S_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XH2S_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XH2S_WB_AUTO            = SDK_WB_AUTO,  
	XH2S_WB_AUTO_WHITE_PRIORITY    = SDK_WB_AUTO_WHITE_PRIORITY,
	XH2S_WB_AUTO_AMBIENCE_PRIORITY = SDK_WB_AUTO_AMBIENCE_PRIORITY,
	XH2S_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XH2S_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XH2S_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XH2S_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XH2S_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XH2S_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XH2S_WB_SHADE           = SDK_WB_SHADE,
	XH2S_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XH2S_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XH2S_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XH2S_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XH2S_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XH2S_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XH2S_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XH2S_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XH2S_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XH2S_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XH2S_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XH2S_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XH2S_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XH2S_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XH2S_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XH2S_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XH2S_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XH2S_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XH2S_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XH2S_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XH2S_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XH2S_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XH2S_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XH2S_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XH2S_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XH2S_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XH2S_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XH2S_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XH2S_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XH2S_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XH2S_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XH2S_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XH2S_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XH2S_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XH2S_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XH2S_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XH2S_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XH2S_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XH2S_WB_R_SHIFT_STEP    = 1,
	XH2S_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XH2S_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XH2S_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XH2S_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XH2S_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XH2S_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XH2S_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XH2S_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XH2S_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XH2S_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XH2S_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XH2S_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XH2S_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XH2S_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XH2S_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XH2S_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XH2S_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XH2S_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XH2S_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XH2S_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XH2S_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XH2S_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XH2S_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XH2S_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
	XH2S_FILMSIMULATION_BLEACH_BYPASS   = SDK_FILMSIMULATION_BLEACH_BYPASS,
	XH2S_FILMSIMULATION_NOSTALGICNEG    = SDK_FILMSIMULATION_NOSTALGICNEG,
	XH2S_FILMSIMULATION_REALA_ACE       = SDK_FILMSIMULATION_REALA_ACE,
};

// COLOR
enum{
	XH2S_COLOR_HIGH          = SDK_COLOR_HIGH,
	XH2S_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XH2S_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XH2S_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XH2S_COLOR_LOW           = SDK_COLOR_LOW,
	XH2S_COLOR_P4            = SDK_COLOR_P4,
	XH2S_COLOR_P3            = SDK_COLOR_P3,
	XH2S_COLOR_P2            = SDK_COLOR_P2,
	XH2S_COLOR_P1            = SDK_COLOR_P1,
	XH2S_COLOR_0             = SDK_COLOR_0,
	XH2S_COLOR_M1            = SDK_COLOR_M1,
	XH2S_COLOR_M2            = SDK_COLOR_M2,
	XH2S_COLOR_M3            = SDK_COLOR_M3,
	XH2S_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XH2S_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XH2S_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XH2S_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XH2S_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XH2S_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XH2S_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XH2S_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XH2S_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XH2S_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XH2S_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XH2S_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XH2S_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XH2S_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XH2S_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XH2S_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XH2S_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XH2S_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XH2S_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XH2S_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XH2S_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XH2S_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XH2S_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XH2S_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XH2S_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XH2S_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XH2S_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XH2S_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XH2S_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XH2S_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XH2S_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XH2S_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XH2S_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XH2S_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XH2S_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XH2S_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XH2S_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XH2S_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XH2S_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XH2S_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XH2S_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XH2S_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XH2S_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XH2S_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XH2S_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XH2S_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XH2S_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XH2S_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XH2S_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XH2S_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XH2S_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XH2S_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XH2S_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XH2S_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XH2S_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XH2S_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XH2S_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XH2S_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XH2S_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XH2S_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XH2S_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XH2S_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XH2S_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XH2S_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XH2S_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XH2S_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XH2S_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XH2S_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XH2S_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XH2S_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XH2S_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XH2S_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XH2S_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XH2S_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XH2S_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XH2S_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XH2S_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XH2S_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XH2S_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XH2S_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XH2S_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XH2S_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XH2S_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XH2S_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XH2S_SHARPNESS_0               = SDK_SHARPNESS_0,
	XH2S_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XH2S_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XH2S_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XH2S_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XH2S_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XH2S_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XH2S_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XH2S_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XH2S_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XH2S_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XH2S_HIGHLIGHT_TONE_P3_5       = SDK_HIGHLIGHT_TONE_P3_5,
	XH2S_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XH2S_HIGHLIGHT_TONE_P2_5       = SDK_HIGHLIGHT_TONE_P2_5,
	XH2S_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XH2S_HIGHLIGHT_TONE_P1_5       = SDK_HIGHLIGHT_TONE_P1_5,
	XH2S_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XH2S_HIGHLIGHT_TONE_P0_5       = SDK_HIGHLIGHT_TONE_P0_5,
	XH2S_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XH2S_HIGHLIGHT_TONE_M0_5       = SDK_HIGHLIGHT_TONE_M0_5,
	XH2S_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XH2S_HIGHLIGHT_TONE_M1_5       = SDK_HIGHLIGHT_TONE_M1_5,
	XH2S_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XH2S_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XH2S_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XH2S_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XH2S_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XH2S_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XH2S_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XH2S_SHADOW_TONE_P3_5          = SDK_SHADOW_TONE_P3_5,
	XH2S_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XH2S_SHADOW_TONE_P2_5          = SDK_SHADOW_TONE_P2_5,
	XH2S_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XH2S_SHADOW_TONE_P1_5          = SDK_SHADOW_TONE_P1_5,
	XH2S_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XH2S_SHADOW_TONE_P0_5          = SDK_SHADOW_TONE_P0_5,
	XH2S_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XH2S_SHADOW_TONE_M0_5          = SDK_SHADOW_TONE_M0_5,
	XH2S_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XH2S_SHADOW_TONE_M1_5          = SDK_SHADOW_TONE_M1_5,
	XH2S_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XH2S_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XH2S_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XH2S_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XH2S_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XH2S_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XH2S_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XH2S_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XH2S_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XH2S_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XH2S_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XH2S_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XH2S_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XH2S_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XH2S_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XH2S_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XH2S_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XH2S_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XH2S_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XH2S_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XH2S_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XH2S_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XH2S_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XH2S_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
	XH2S_RAW_COMPRESSION_LOSSY		 = SDK_RAW_COMPRESSION_LOSSY,
};

// Grain Effect
enum{
	XH2S_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XH2S_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XH2S_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XH2S_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XH2S_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XH2S_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XH2S_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XH2S_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XH2S_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XH2S_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XH2S_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XH2S_CLARITY_P5		= SDK_CLARITY_P5,
	XH2S_CLARITY_P4		= SDK_CLARITY_P4,
	XH2S_CLARITY_P3		= SDK_CLARITY_P3,
	XH2S_CLARITY_P2		= SDK_CLARITY_P2,
	XH2S_CLARITY_P1		= SDK_CLARITY_P1,
	XH2S_CLARITY_0			= SDK_CLARITY_0 ,
	XH2S_CLARITY_M1		= SDK_CLARITY_M1,
	XH2S_CLARITY_M2		= SDK_CLARITY_M2,
	XH2S_CLARITY_M3		= SDK_CLARITY_M3,
	XH2S_CLARITY_M4		= SDK_CLARITY_M4,
	XH2S_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XH2S_SHADOWING_0             = SDK_SHADOWING_0,
	XH2S_SHADOWING_P1            = SDK_SHADOWING_P1,
	XH2S_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XH2S_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XH2S_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XH2S_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XH2S_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XH2S_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XH2S_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XH2S_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XH2S_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XH2S_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XH2S_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XH2S_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XH2S_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XH2S_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XH2S_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XH2S_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XH2S_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XH2S_FOCUS_AFS        = SDK_FOCUS_AFS,
	XH2S_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XH2S_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XH2S_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XH2S_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XH2S_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XH2S_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XH2S_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XH2S_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XH2S_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Focus Points
enum{
	XH2S_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XH2S_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XH2S_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XH2S_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XH2S_AF_ALL     = SDK_AF_ALL,
	XH2S_AF_MULTI   = SDK_AF_MULTI,
	XH2S_AF_AREA    = SDK_AF_AREA,
	XH2S_AF_SINGLE  = XH2S_AF_AREA,
	XH2S_AF_ZONE    = SDK_AF_ZONE,
	XH2S_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XH2S_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XH2S_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XH2S_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XH2S_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XH2S_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XH2S_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XH2S_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XH2S_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XH2S_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XH2S_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XH2S_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XH2S_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XH2S_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XH2S_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XH2S_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XH2S_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XH2S_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XH2S_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XH2S_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XH2S_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XH2S_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XH2S_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XH2S_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XH2S_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XH2S_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XH2S_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
	XH2S_MF_ASSIST_FOCUSMETER               = SDK_MF_ASSIST_FOCUSMETER,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_RED_L    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_L,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_RED_H    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_H,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L,
	XH2S_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H,
	XH2S_MF_ASSIST_FOCUSMAP_BW              = SDK_MF_ASSIST_FOCUSMAP_BW,
	XH2S_MF_ASSIST_FOCUSMAP_COLOR           = SDK_MF_ASSIST_FOCUSMAP_COLOR,
};

// FOCUS AREA
enum{
	XH2S_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XH2S_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XH2S_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XH2S_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XH2S_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XH2S_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XH2S_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XH2S_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XH2S_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XH2S_MACRO_MODE           = SDK_MACRO_MODE,
	XH2S_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XH2S_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XH2S_DRIVE_MODE_CL   = 0x1000,
	XH2S_DRIVE_MODE_CH   = 0x10F0,
	XH2S_DRIVE_MODE_BKT  = 0x4000,
	XH2S_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE,
};

// USB MODE
enum{
	XH2S_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XH2S_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XH2S_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XH2S_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XH2S_BEEP_HIGH        = SDK_BEEP_HIGH,
	XH2S_BEEP_MID         = SDK_BEEP_MID,
	XH2S_BEEP_LOW         = SDK_BEEP_LOW,
	XH2S_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XH2S_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XH2S_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XH2S_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XH2S_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XH2S_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XH2S_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XH2S_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XH2S_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XH2S_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XH2S_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XH2S_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XH2S_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XH2S_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XH2S_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XH2S_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XH2S_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XH2S_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XH2S_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XH2S_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XH2S_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XH2S_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XH2S_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XH2S_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XH2S_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XH2S_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XH2S_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XH2S_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XH2S_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XH2S_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XH2S_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XH2S_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XH2S_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XH2S_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XH2S_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XH2S_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XH2S_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XH2S_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XH2S_FRAMEGUIDE_GRID_HD      = 0x0003,
	XH2S_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XH2S_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XH2S_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XH2S_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XH2S_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XH2S_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XH2S_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XH2S_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XH2S_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XH2S_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XH2S_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XH2S_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XH2S_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XH2S_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XH2S_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XH2S_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XH2S_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XH2S_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XH2S_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XH2S_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XH2S_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XH2S_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XH2S_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XH2S_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XH2S_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XH2S_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XH2S_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XH2S_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XH2S_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XH2S_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XH2S_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XH2S_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XH2S_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XH2S_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XH2S_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XH2S_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XH2S_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XH2S_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XH2S_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XH2S_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XH2S_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XH2S_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XH2S_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XH2S_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XH2S_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XH2S_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XH2S_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XH2S_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XH2S_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XH2S_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XH2S_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XH2S_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XH2S_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XH2S_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XH2S_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XH2S_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XH2S_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XH2S_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XH2S_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XH2S_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XH2S_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XH2S_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XH2S_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XH2S_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XH2S_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XH2S_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XH2S_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XH2S_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XH2S_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XH2S_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XH2S_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XH2S_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XH2S_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XH2S_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XH2S_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XH2S_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XH2S_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XH2S_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XH2S_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XH2S_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XH2S_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XH2S_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XH2S_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XH2S_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XH2S_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XH2S_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XH2S_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XH2S_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XH2S_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XH2S_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XH2S_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XH2S_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XH2S_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XH2S_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XH2S_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XH2S_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XH2S_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XH2S_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XH2S_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XH2S_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XH2S_CUSTOMDISPINFO_LIVEVIEWHIGHT                 = SDK_CUSTOMDISPINFO_LIVEVIEWHIGHT,
	XH2S_CUSTOMDISPINFO_EXPOSUREBIASDIGIT             = SDK_CUSTOMDISPINFO_EXPOSUREBIASDIGIT,
	XH2S_CUSTOMDISPINFO_TOUCHSCREENMODE               = SDK_CUSTOMDISPINFO_TOUCHSCREENMODE,
	XH2S_CUSTOMDISPINFO_BOOSTMODE                     = SDK_CUSTOMDISPINFO_BOOSTMODE,
	XH2S_CUSTOMDISPINFO_IMAGETRANSFERORDER            = SDK_CUSTOMDISPINFO_IMAGETRANSFERORDER,
	XH2S_CUSTOMDISPINFO_MICLEVEL                      = SDK_CUSTOMDISPINFO_MICLEVEL,
	XH2S_CUSTOMDISPINFO_GUIDANCEMESSAGE               = SDK_CUSTOMDISPINFO_GUIDANCEMESSAGE,
	XH2S_CUSTOMDISPINFO_FRAMEOUTLINE                  = SDK_CUSTOMDISPINFO_FRAMEOUTLINE,
	XH2S_CUSTOMDISPINFO_35MMFORMAT                    = SDK_CUSTOMDISPINFO_35MMFORMAT,
	XH2S_CUSTOMDISPINFO_COOLINGFANSETTING             = SDK_CUSTOMDISPINFO_COOLINGFANSETTING,
	XH2S_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XH2S_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XH2S_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XH2S_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XH2S_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XH2S_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XH2S_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XH2S_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XH2S_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XH2S_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XH2S_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XH2S_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XH2S_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XH2S_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XH2S_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XH2S_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XH2S_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XH2S_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XH2S_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XH2S_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XH2S_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XH2S_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XH2S_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XH2S_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XH2S_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XH2S_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XH2S_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XH2S_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XH2S_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XH2S_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XH2S_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XH2S_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XH2S_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XH2S_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XH2S_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XH2S_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XH2S_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XH2S_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XH2S_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XH2S_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XH2S_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XH2S_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XH2S_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XH2S_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XH2S_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XH2S_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XH2S_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XH2S_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XH2S_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XH2S_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XH2S_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XH2S_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XH2S_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XH2S_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER  = SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER,
	XH2S_FUNCTIONLOCK_CATEGORY2_MODEDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_MODEDIAL,
	XH2S_FUNCTIONLOCK_CATEGORY2_FDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FDIAL,
	XH2S_FUNCTIONLOCK_CATEGORY2_FN_DIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FN_DIAL,
	XH2S_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT		= SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT,
	XH2S_FUNCTIONLOCK_CATEGORY3_ISOBUTTON            = SDK_FUNCTIONLOCK_CATEGORY3_ISOBUTTON,
	XH2S_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE      = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE,
	XH2S_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE         = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE,
	XH2S_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU      = SDK_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU,
	XH2S_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE         = SDK_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE,
	XH2S_FUNCTIONLOCK_CATEGORY3_WBBUTTON             = SDK_FUNCTIONLOCK_CATEGORY3_WBBUTTON,
	XH2S_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING     = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING,
	XH2S_FUNCTIONLOCK_CATEGORY3_BLUETOOTH            = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTH,
	XH2S_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT        = SDK_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT,
	XH2S_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING = SDK_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING,
	XH2S_FUNCTIONLOCK_CATEGORY3_FM1                  = SDK_FUNCTIONLOCK_CATEGORY3_FM1,
	XH2S_FUNCTIONLOCK_CATEGORY3_FM2                  = SDK_FUNCTIONLOCK_CATEGORY3_FM2,
	XH2S_FUNCTIONLOCK_CATEGORY3_FM3                  = SDK_FUNCTIONLOCK_CATEGORY3_FM3,
	XH2S_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION = SDK_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION,
	XH2S_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XH2S_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XH2S_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XH2S_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XH2S_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XH2S_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XH2S_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XH2S_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XH2S_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XH2S_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XH2S_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XH2S_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XH2S_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XH2S_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XH2S_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XH2S_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XH2S_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XH2S_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XH2S_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XH2S_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XH2S_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XH2S_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XH2S_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XH2S_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	XH2S_MEDIASTATUS_ACCESSING             = SDK_MEDIASTATUS_ACCESSING,
	XH2S_MEDIASTATUS_INCOMPATIBLE          = SDK_MEDIASTATUS_INCOMPATIBLE,
};

enum{
	XH2S_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XH2S_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XH2S_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XH2S_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XH2S_ON  = SDK_ON,
	XH2S_OFF = SDK_OFF,
};

enum{
	XH2S_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XH2S_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XH2S_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XH2S_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XH2S_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XH2S_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XH2S_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XH2S_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XH2S_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XH2S_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XH2S_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XH2S_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XH2S_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XH2S_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XH2S_ITEM_RESET_MOVIEMENU         = SDK_ITEM_RESET_MOVIEMENU,
	XH2S_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XH2S_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XH2S_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XH2S_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XH2S_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XH2S_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XH2S_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XH2S_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XH2S_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XH2S_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XH2S_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XH2S_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XH2S_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XH2S_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XH2S_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XH2S_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XH2S_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XH2S_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XH2S_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XH2S_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XH2S_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XH2S_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XH2S_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XH2S_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XH2S_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XH2S_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XH2S_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XH2S_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XH2S_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XH2S_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XH2S_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XH2S_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XH2S_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XH2S_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XH2S_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XH2S_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XH2S_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XH2S_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XH2S_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XH2S_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XH2S_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XH2S_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};

// Micline Setting
enum{
	XH2S_MICLINE_SETTING_MIC          = SDK_MICLINE_SETTING_MIC,
	XH2S_MICLINE_SETTING_LINE         = SDK_MICLINE_SETTING_LINE,
};

// Shuttercount Type
enum{
	XH2S_SHUTTERCOUNT_TYPE_FRONTCURTAIN = SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN,
	XH2S_SHUTTERCOUNT_TYPE_REARCURTAIN  = SDK_SHUTTERCOUNT_TYPE_REARCURTAIN,
	XH2S_SHUTTERCOUNT_TYPE_TOTAL        = SDK_SHUTTERCOUNT_TYPE_TOTAL,
};

// Performance
enum{
	XH2S_PERFORMANCE_NORMAL                    = SDK_PERFORMANCE_NORMAL,
	XH2S_PERFORMANCE_ECONOMY                   = SDK_PERFORMANCE_ECONOMY,
	XH2S_PERFORMANCE_BOOST_LOWLIGHT            = SDK_PERFORMANCE_BOOST_LOWLIGHT,
	XH2S_PERFORMANCE_BOOST_RESOLUTION_PRIORITY = SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY,
	XH2S_PERFORMANCE_BOOST_FRAMERATE_PRIORITY  = SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY,
	XH2S_PERFORMANCE_BOOST_AFPRIORITY_NORMAL   = SDK_PERFORMANCE_BOOST_AFPRIORITY_NORMAL,
	XH2S_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION= SDK_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION,
};

// PixelShift Settings
enum{
	XH2S_PIXELSHIFT_INTERVAL_SHORTEST = SDK_PIXELSHIFT_INTERVAL_SHORTEST,
	XH2S_PIXELSHIFT_INTERVAL_1S       = SDK_PIXELSHIFT_INTERVAL_1S,
	XH2S_PIXELSHIFT_INTERVAL_2S       = SDK_PIXELSHIFT_INTERVAL_2S,
	XH2S_PIXELSHIFT_INTERVAL_5S       = SDK_PIXELSHIFT_INTERVAL_5S,
	XH2S_PIXELSHIFT_INTERVAL_15S      = SDK_PIXELSHIFT_INTERVAL_15S,
};

// SubjectDetectionMode
enum{
	XH2S_SUBJECT_DETECTION_OFF        = SDK_SUBJECT_DETECTION_OFF,
	XH2S_SUBJECT_DETECTION_ANIMAL     = SDK_SUBJECT_DETECTION_ANIMAL,
	XH2S_SUBJECT_DETECTION_BIRD       = SDK_SUBJECT_DETECTION_BIRD,
	XH2S_SUBJECT_DETECTION_CAR        = SDK_SUBJECT_DETECTION_CAR,
	XH2S_SUBJECT_DETECTION_BIKE       = SDK_SUBJECT_DETECTION_BIKE,
	XH2S_SUBJECT_DETECTION_AIRPLANE   = SDK_SUBJECT_DETECTION_AIRPLANE,
	XH2S_SUBJECT_DETECTION_TRAIN      = SDK_SUBJECT_DETECTION_TRAIN,
};

// FanSetting
enum{
	XH2S_FAN_SETTING_OFF              = SDK_FAN_SETTING_OFF,
	XH2S_FAN_SETTING_WEAK             = SDK_FAN_SETTING_WEAK,
	XH2S_FAN_SETTING_STRONG           = SDK_FAN_SETTING_STRONG,
	XH2S_FAN_SETTING_AUTO1            = SDK_FAN_SETTING_AUTO1,
	XH2S_FAN_SETTING_AUTO2            = SDK_FAN_SETTING_AUTO2,
};

// ElectronicLevelSetting
enum{
	XH2S_ELECTRONIC_LEVEL_SETTING_OFF = SDK_ELECTRONIC_LEVEL_SETTING_OFF,
	XH2S_ELECTRONIC_LEVEL_SETTING_2D  = SDK_ELECTRONIC_LEVEL_SETTING_2D,
	XH2S_ELECTRONIC_LEVEL_SETTING_3D  = SDK_ELECTRONIC_LEVEL_SETTING_3D,
};

// ApertureUnit
enum{
	XH2S_APERTURE_UNIT_TNUMBER        = SDK_APERTURE_UNIT_TNUMBER,
	XH2S_APERTURE_UNIT_FNUMBER        = SDK_APERTURE_UNIT_FNUMBER,
};
