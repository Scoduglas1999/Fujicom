///////////////////////////////////////////////////
//  X-S20.h
//     Version 1.0.0.0
//
//     Copyright (C) 2022 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-S20
//
enum {
	XS20_API_PARAM_CapHighFrequencyFlickerlessMode = 2,
	XS20_API_PARAM_SetHighFrequencyFlickerlessMode = 1,
	XS20_API_PARAM_GetHighFrequencyFlickerlessMode = 1,
	XS20_API_PARAM_CapImageSize                = 2,
	XS20_API_PARAM_SetImageSize                = 1,
	XS20_API_PARAM_GetImageSize                = 1,
	XS20_API_PARAM_CapSharpness                = 2,
	XS20_API_PARAM_SetSharpness                = 1,
	XS20_API_PARAM_GetSharpness                = 1,
	XS20_API_PARAM_CapColorMode                = 2,
	XS20_API_PARAM_SetColorMode                = 1,
	XS20_API_PARAM_GetColorMode                = 1,
	XS20_API_PARAM_CapFilmSimulationMode       = 2,
	XS20_API_PARAM_SetFilmSimulationMode       = 1,
	XS20_API_PARAM_GetFilmSimulationMode       = 1,
	XS20_API_PARAM_CapColorSpace               = 2,
	XS20_API_PARAM_SetColorSpace               = 1,
	XS20_API_PARAM_GetColorSpace               = 1,
	XS20_API_PARAM_CapImageQuality             = 2,
	XS20_API_PARAM_SetImageQuality             = 1,
	XS20_API_PARAM_GetImageQuality             = 1,
	XS20_API_PARAM_CapRAWCompression           = 2,
	XS20_API_PARAM_SetRAWCompression           = 1,
	XS20_API_PARAM_GetRAWCompression           = 1,
	XS20_API_PARAM_CapGrainEffect              = 2,
	XS20_API_PARAM_SetGrainEffect              = 1,
	XS20_API_PARAM_GetGrainEffect              = 1,
	XS20_API_PARAM_CapShadowing                = 2,
	XS20_API_PARAM_SetShadowing                = 1,
	XS20_API_PARAM_GetShadowing                = 1,
	XS20_API_PARAM_CapNoiseReduction           = 2,
	XS20_API_PARAM_SetNoiseReduction           = 1,
	XS20_API_PARAM_GetNoiseReduction           = 1,
	XS20_API_PARAM_CapFaceDetectionMode        = 2,
	XS20_API_PARAM_SetFaceDetectionMode        = 1,
	XS20_API_PARAM_GetFaceDetectionMode        = 1,
	XS20_API_PARAM_CapEyeAFMode                = 2,
	XS20_API_PARAM_SetEyeAFMode                = 1,
	XS20_API_PARAM_GetEyeAFMode                = 1,
	XS20_API_PARAM_SetMacroMode                = -1,
	XS20_API_PARAM_GetMacroMode                = -1,
	XS20_API_PARAM_CapFocusLimiter             = -1,
	XS20_API_PARAM_SetFocusLimiter             = -1,
	XS20_API_PARAM_GetFocusLimiter             = -1,
	XS20_API_PARAM_CapHighLightTone            = 2,
	XS20_API_PARAM_SetHighLightTone            = 1,
	XS20_API_PARAM_GetHighLightTone            = 1,
	XS20_API_PARAM_CapShadowTone               = 2,
	XS20_API_PARAM_SetShadowTone               = 1,
	XS20_API_PARAM_GetShadowTone               = 1,
	XS20_API_PARAM_CapLongExposureNR           = 2,
	XS20_API_PARAM_SetLongExposureNR           = 1,
	XS20_API_PARAM_GetLongExposureNR           = 1,
	XS20_API_PARAM_CapFullTimeManualFocus      = 2,
	XS20_API_PARAM_SetFullTimeManualFocus      = 1,
	XS20_API_PARAM_GetFullTimeManualFocus      = 1,
	XS20_API_PARAM_SetFocusMode                = 1,
	XS20_API_PARAM_GetFocusMode                = 1,
	XS20_API_PARAM_CapAFMode                   = 3,
	XS20_API_PARAM_SetAFMode                   = 2,
	XS20_API_PARAM_GetAFMode                   = 2,
	XS20_API_PARAM_CapFocusPoints              = 2,
	XS20_API_PARAM_SetFocusPoints              = 1,
	XS20_API_PARAM_GetFocusPoints              = 1,
	XS20_API_PARAM_CapFocusArea                = 3,
	XS20_API_PARAM_SetFocusArea                = 2,
	XS20_API_PARAM_GetFocusArea                = 2,
	XS20_API_PARAM_CapFocusMode                = 2,
	XS20_API_PARAM_CapAFStatus                 = 2,
	XS20_API_PARAM_GetAFStatus                 = 1,
	XS20_API_PARAM_CapShutterPriorityMode      = 3,
	XS20_API_PARAM_SetShutterPriorityMode      = 2,
	XS20_API_PARAM_GetShutterPriorityMode      = 2,
	XS20_API_PARAM_CapInstantAFMode            = 2,
	XS20_API_PARAM_SetInstantAFMode            = 1,
	XS20_API_PARAM_GetInstantAFMode            = 1,
	XS20_API_PARAM_CapPreAFMode                = 2,
	XS20_API_PARAM_SetPreAFMode                = 1,
	XS20_API_PARAM_GetPreAFMode                = 1,
	XS20_API_PARAM_CapAFIlluminator            = 2,
	XS20_API_PARAM_SetAFIlluminator            = 1,
	XS20_API_PARAM_GetAFIlluminator            = 1,
	XS20_API_PARAM_CapLensISSwitch             = 2,
	XS20_API_PARAM_SetLensISSwitch             = 1,
	XS20_API_PARAM_GetLensISSwitch             = 1,
	XS20_API_PARAM_CapISMode                   = 2,
	XS20_API_PARAM_SetISMode                   = 1,
	XS20_API_PARAM_GetISMode                   = 1,
	XS20_API_PARAM_CapLMOMode                  = 2,
	XS20_API_PARAM_SetLMOMode                  = 1,
	XS20_API_PARAM_GetLMOMode                  = 1,
	XS20_API_PARAM_SetWhiteBalanceMode         = -1,
	XS20_API_PARAM_GetWhiteBalanceMode         = -1,
	XS20_API_PARAM_CapWhiteBalanceTune         = 5,
	XS20_API_PARAM_SetWhiteBalanceTune         = 3,
	XS20_API_PARAM_GetWhiteBalanceTune         = 3,
	XS20_API_PARAM_CapCaptureDelay             = 2,
	XS20_API_PARAM_SetCaptureDelay             = 1,
	XS20_API_PARAM_GetCaptureDelay             = 1,
	XS20_API_PARAM_CapFocusPos                 = 2,
	XS20_API_PARAM_SetFocusPos                 = 1,
	XS20_API_PARAM_GetFocusPos                 = 1,
	XS20_API_PARAM_CapMFAssistMode             = 2,
	XS20_API_PARAM_SetMFAssistMode             = 1,
	XS20_API_PARAM_GetMFAssistMode             = 1,
	XS20_API_PARAM_CapFocusCheckMode           = 2,
	XS20_API_PARAM_SetFocusCheckMode           = 1,
	XS20_API_PARAM_GetFocusCheckMode           = 1,
	XS20_API_PARAM_CapInterlockAEAFArea        = 2,
	XS20_API_PARAM_SetInterlockAEAFArea        = 1,
	XS20_API_PARAM_GetInterlockAEAFArea        = 1,
	XS20_API_PARAM_StartLiveView               = 0,
	XS20_API_PARAM_StopLiveView                = 0,
	XS20_API_PARAM_CapLiveViewImageQuality     = 2,
	XS20_API_PARAM_SetLiveViewImageQuality     = 1,
	XS20_API_PARAM_GetLiveViewImageQuality     = 1,
	XS20_API_PARAM_CapLiveViewImageSize        = 2,
	XS20_API_PARAM_SetLiveViewImageSize        = 1,
	XS20_API_PARAM_GetLiveViewImageSize        = 1,
	XS20_API_PARAM_CapThroughImageZoom         = 2,
	XS20_API_PARAM_SetThroughImageZoom         = 1,
	XS20_API_PARAM_GetThroughImageZoom         = 1,
	XS20_API_PARAM_SetDateTime                 = 6,
	XS20_API_PARAM_GetDateTime                 = 6,
	XS20_API_PARAM_CapDateTimeDispFormat       = 2,
	XS20_API_PARAM_SetDateTimeDispFormat       = 1,
	XS20_API_PARAM_GetDateTimeDispFormat       = 1,
	XS20_API_PARAM_CapWorldClock               = 2,
	XS20_API_PARAM_SetWorldClock               = 1,
	XS20_API_PARAM_GetWorldClock               = 1,
	XS20_API_PARAM_CapTimeDifference           = 4,
	XS20_API_PARAM_SetTimeDifference           = 2,
	XS20_API_PARAM_GetTimeDifference           = 2,
	XS20_API_PARAM_CapResetSetting             = 2,
	XS20_API_PARAM_ResetSetting                = 1,
	XS20_API_PARAM_SetSilentMode               = -1,
	XS20_API_PARAM_GetSilentMode               = -1,
	XS20_API_PARAM_SetBeep                     = -1,
	XS20_API_PARAM_GetBeep                     = -1,
	XS20_API_PARAM_SetFunctionLock             = 1,
	XS20_API_PARAM_GetFunctionLock             = 1,
	XS20_API_PARAM_CapFunctionLock             = 2,
	XS20_API_PARAM_SetFunctionLockCategory     = 3,
	XS20_API_PARAM_GetFunctionLockCategory     = 3,
	XS20_API_PARAM_CapFunctionLockCategory     = 6,
	XS20_API_PARAM_SetComment                  = 1,
	XS20_API_PARAM_GetComment                  = 1,
	XS20_API_PARAM_SetCopyright                = 2,
	XS20_API_PARAM_GetCopyright                = 2,
	XS20_API_PARAM_SetArtist                   = 1,
	XS20_API_PARAM_GetArtist                   = 1,
	XS20_API_PARAM_SetFilenamePrefix           = 2,
	XS20_API_PARAM_GetFilenamePrefix           = 2,
	XS20_API_PARAM_SetFoldernameSuffix         = 3,
	XS20_API_PARAM_GetFoldernameSuffix         = 3,
	XS20_API_PARAM_GetFoldernameList           = 4,
	XS20_API_PARAM_CheckBatteryInfo            = 8,
	XS20_API_PARAM_CapFrameNumberSequence      = 2,
	XS20_API_PARAM_SetFrameNumberSequence      = 1,
	XS20_API_PARAM_GetFrameNumberSequence      = 1,
	XS20_API_PARAM_SetUSBMode                  = -1,
	XS20_API_PARAM_GetUSBMode                  = -1,
	XS20_API_PARAM_CapFormatMemoryCard         = 2,
	XS20_API_PARAM_FormatMemoryCard            = 1,
	XS20_API_PARAM_SetMediaRecord              = 1,
	XS20_API_PARAM_GetMediaRecord              = 1,
	XS20_API_PARAM_GetMediaCapacity            = 5,
	XS20_API_PARAM_GetMediaStatus              = 2,
	XS20_API_PARAM_GetShutterCount             = 3,
	XS20_API_PARAM_GetShutterCountEx           = 3,
	XS20_API_PARAM_SetSensorCleanTiming        = -1,
	XS20_API_PARAM_GetSensorCleanTiming        = -1,
	XS20_API_PARAM_SetPreviewTime              = -1,
	XS20_API_PARAM_GetPreviewTime              = -1,
	XS20_API_PARAM_SetEVFDispAutoRotate        = -1,
	XS20_API_PARAM_GetEVFDispAutoRotate        = -1,
	XS20_API_PARAM_SetDispMMode                = 1,
	XS20_API_PARAM_GetDispMMode                = 1,
	XS20_API_PARAM_CapExposurePreview          = 2,
	XS20_API_PARAM_SetExposurePreview          = 1,
	XS20_API_PARAM_GetExposurePreview          = 1,
	XS20_API_PARAM_SetDispBrightness           = -1,
	XS20_API_PARAM_GetDispBrightness           = -1,
	XS20_API_PARAM_CapFrameGuideMode           = 2,
	XS20_API_PARAM_SetFrameGuideMode           = 1,
	XS20_API_PARAM_GetFrameGuideMode           = 1,
	XS20_API_PARAM_SetFrameGuideGridInfo       = 2,
	XS20_API_PARAM_GetFrameGuideGridInfo       = 2,
	XS20_API_PARAM_SetAutoImageRotation        = -1,
	XS20_API_PARAM_GetAutoImageRotation        = -1,
	XS20_API_PARAM_CapFocusScaleUnit           = 2,
	XS20_API_PARAM_SetFocusScaleUnit           = 1,
	XS20_API_PARAM_GetFocusScaleUnit           = 1,
	XS20_API_PARAM_CapCustomDispInfo           = 4,
	XS20_API_PARAM_SetCustomDispInfo           = 2,
	XS20_API_PARAM_GetCustomDispInfo           = 2,
	XS20_API_PARAM_CapViewMode                 = 3,
	XS20_API_PARAM_SetViewMode                 = 2,
	XS20_API_PARAM_GetViewMode                 = 2,
	XS20_API_PARAM_SetDispInfoMode             = -1,
	XS20_API_PARAM_GetDispInfoMode             = -1,
	XS20_API_PARAM_SetDispChroma               = -1,
	XS20_API_PARAM_GetDispChroma               = -1,
	XS20_API_PARAM_CapCustomAutoPowerOff       = 2,
	XS20_API_PARAM_SetCustomAutoPowerOff       = 1,
	XS20_API_PARAM_GetCustomAutoPowerOff       = 1,
	XS20_API_PARAM_CapCustomStudioPowerSave    = -1,
	XS20_API_PARAM_SetCustomStudioPowerSave    = -1,
	XS20_API_PARAM_GetCustomStudioPowerSave    = -1,
	XS20_API_PARAM_SetFunctionButton           = -1,
	XS20_API_PARAM_GetFunctionButton           = -1,
	XS20_API_PARAM_SetISODialHn                = -1,
	XS20_API_PARAM_GetISODialHn                = -1,
	XS20_API_PARAM_CapLockButtonMode           = 2,
	XS20_API_PARAM_SetLockButtonMode           = 1,
	XS20_API_PARAM_GetLockButtonMode           = 1,
	XS20_API_PARAM_SetAFLockMode               = -1,
	XS20_API_PARAM_GetAFLockMode               = -1,
	XS20_API_PARAM_SetMicJackMode              = -1,
	XS20_API_PARAM_GetMicJackMode              = -1,
	XS20_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XS20_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XS20_API_PARAM_SetCrossKeyAssign           = -1,
	XS20_API_PARAM_GetCrossKeyAssign           = -1,
	XS20_API_PARAM_CapWideDynamicRange         = 2,
	XS20_API_PARAM_SetWideDynamicRange         = 1,
	XS20_API_PARAM_GetWideDynamicRange         = 1,
	XS20_API_PARAM_CapClarityMode              = 2,
	XS20_API_PARAM_SetClarityMode              = 1,
	XS20_API_PARAM_GetClarityMode              = 1,
	XS20_API_PARAM_SetBlackImageTone           = -1,
	XS20_API_PARAM_GetBlackImageTone           = -1,
	XS20_API_PARAM_GetTNumber                  = 1,
	XS20_API_PARAM_CapCropMode                 = 2,
	XS20_API_PARAM_SetCropMode                 = 1,
	XS20_API_PARAM_GetCropMode                 = 2,
	XS20_API_PARAM_GetCropAreaFrameInfo        = 1,
	XS20_API_PARAM_CapRAWOutputDepth           = -1,
	XS20_API_PARAM_SetRAWOutputDepth           = -1,
	XS20_API_PARAM_GetRAWOutputDepth           = -1,
	XS20_API_PARAM_CapSmoothSkinEffect         = -1,
	XS20_API_PARAM_SetSmoothSkinEffect         = -1,
	XS20_API_PARAM_GetSmoothSkinEffect         = -1,
	XS20_API_PARAM_GetDetectedFaceFrame        = 2,
	XS20_API_PARAM_SetDetectedFaceFrame        = 1,
    XS20_API_PARAM_SetCustomWBArea             = 2,
    XS20_API_PARAM_GetCustomWBArea             = 2,
	XS20_API_PARAM_CapColorChromeBlue          = 2,
	XS20_API_PARAM_SetColorChromeBlue          = 1,
	XS20_API_PARAM_GetColorChromeBlue          = 1,
	XS20_API_PARAM_CapMonochromaticColor       = 4,
	XS20_API_PARAM_SetMonochromaticColor       = 2,
	XS20_API_PARAM_GetMonochromaticColor       = 2,
	XS20_API_PARAM_CapFocusLimiterPos          = 4,
	XS20_API_PARAM_SetFocusLimiterPos          = 2,
	XS20_API_PARAM_GetFocusLimiterIndicator    = 1,
	XS20_API_PARAM_GetFocusLimiterRange        = 2,
	XS20_API_PARAM_CapFocusLimiterMode         = 2,
	XS20_API_PARAM_SetFocusLimiterMode         = 1,
	XS20_API_PARAM_GetFocusLimiterMode         = 1,
	XS20_API_PARAM_GetCommandDialStatus        = 4,
	XS20_API_PARAM_CapPerformanceSettings      = 2,
	XS20_API_PARAM_SetPerformanceSettings      = 1,
	XS20_API_PARAM_GetPerformanceSettings      = 1,
	XS20_API_PARAM_CapMicLineSetting           = 2,
	XS20_API_PARAM_SetMicLineSetting           = 1,
	XS20_API_PARAM_GetMicLineSetting           = 1,
	XS20_API_PARAM_CapCustomSettingAutoUpdate  = 2,
	XS20_API_PARAM_SetCustomSettingAutoUpdate  = 1,
	XS20_API_PARAM_GetCustomSettingAutoUpdate  = 1,
    XS20_API_PARAM_CapPixelShiftSettings       = -1,
    XS20_API_PARAM_SetPixelShiftSettings       = -1,
    XS20_API_PARAM_GetPixelShiftSettings       = -1,
    XS20_API_PARAM_CapImageFormat              = 2,
    XS20_API_PARAM_SetImageFormat              = 1,
    XS20_API_PARAM_GetImageFormat              = 1,
	XS20_API_PARAM_CapSubjectDetectionMode     = 2,
	XS20_API_PARAM_SetSubjectDetectionMode     = 1,
	XS20_API_PARAM_GetSubjectDetectionMode     = 1,
	XS20_API_PARAM_CapCropZoom                 = -1,
	XS20_API_PARAM_SetCropZoom                 = -1,
	XS20_API_PARAM_GetCropZoom                 = -1,
	XS20_API_PARAM_CapZoomOperation            = 4,
	XS20_API_PARAM_SetZoomOperation            = 3,
	XS20_API_PARAM_CapFocusOperation           = 4,
	XS20_API_PARAM_SetFocusOperation           = 3,
	XS20_API_PARAM_CapZoomSpeed                = 2,
	XS20_API_PARAM_SetZoomSpeed                = 1,
	XS20_API_PARAM_GetZoomSpeed                = 1,
	XS20_API_PARAM_CapFocusSpeed               = 2,
	XS20_API_PARAM_SetFocusSpeed               = 1,
	XS20_API_PARAM_GetFocusSpeed               = 1,
	XS20_API_PARAM_CapLiveViewStatus           = 2,
	XS20_API_PARAM_GetLiveViewStatus           = 1,
	XS20_API_PARAM_CapLiveViewMode             = 2,
	XS20_API_PARAM_SetLiveViewMode             = 1,
	XS20_API_PARAM_GetLiveViewMode             = 1,
	XS20_API_PARAM_CapLiveViewImageRatio       = 2,
	XS20_API_PARAM_SetLiveViewImageRatio       = 1,
	XS20_API_PARAM_GetLiveViewImageRatio       = 1,
	XS20_API_PARAM_CapSummerTime               = 2,
	XS20_API_PARAM_SetSummerTime               = 1,
	XS20_API_PARAM_GetSummerTime               = 1,
	XS20_API_PARAM_CapFanSetting               = 2,
	XS20_API_PARAM_SetFanSetting               = 1,
	XS20_API_PARAM_GetFanSetting               = 1,
	XS20_API_PARAM_CapElectronicLevelSetting   = 2,
	XS20_API_PARAM_SetElectronicLevelSetting   = 1,
	XS20_API_PARAM_GetElectronicLevelSetting   = 1,
	XS20_API_PARAM_CapApertureUnit             = 2,
	XS20_API_PARAM_SetApertureUnit             = 1,
	XS20_API_PARAM_GetApertureUnit             = 1,
	XS20_API_PARAM_CapUSBPowerSupplyCommunication = 2,
	XS20_API_PARAM_SetUSBPowerSupplyCommunication = 1,
	XS20_API_PARAM_GetUSBPowerSupplyCommunication = 1,
	XS20_API_PARAM_CapAutoPowerOffSetting      = 2,
	XS20_API_PARAM_SetAutoPowerOffSetting      = 1,
	XS20_API_PARAM_GetAutoPowerOffSetting      = 1,
};

//
// API_CODE for X-S20
//
enum {
	XS20_API_CODE_CapHighFrequencyFlickerlessMode = API_CODE_CapHighFrequencyFlickerlessMode,
	XS20_API_CODE_SetHighFrequencyFlickerlessMode = API_CODE_SetHighFrequencyFlickerlessMode,
	XS20_API_CODE_GetHighFrequencyFlickerlessMode = API_CODE_GetHighFrequencyFlickerlessMode,
	XS20_API_CODE_CapImageSize                = API_CODE_CapImageSize,
	XS20_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XS20_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XS20_API_CODE_CapSharpness                = API_CODE_CapSharpness,
	XS20_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XS20_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XS20_API_CODE_CapColorMode                = API_CODE_CapColorMode,
	XS20_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XS20_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XS20_API_CODE_CapFilmSimulationMode       = API_CODE_CapFilmSimulationMode,
	XS20_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XS20_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XS20_API_CODE_CapColorSpace               = API_CODE_CapColorSpace,
	XS20_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XS20_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XS20_API_CODE_CapImageQuality             = API_CODE_CapImageQuality,
	XS20_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XS20_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XS20_API_CODE_CapRAWCompression           = API_CODE_CapRAWCompression,
	XS20_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XS20_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XS20_API_CODE_CapGrainEffect              = API_CODE_CapGrainEffect,
	XS20_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XS20_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XS20_API_CODE_CapShadowing                = API_CODE_CapShadowing,
	XS20_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XS20_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XS20_API_CODE_CapNoiseReduction           = API_CODE_CapNoiseReduction,
	XS20_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XS20_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XS20_API_CODE_CapFaceDetectionMode        = API_CODE_CapFaceDetectionMode,
	XS20_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XS20_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XS20_API_CODE_CapEyeAFMode                = API_CODE_CapEyeAFMode,
	XS20_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XS20_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XS20_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XS20_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XS20_API_CODE_CapHighLightTone            = API_CODE_CapHighLightTone,
	XS20_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XS20_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XS20_API_CODE_CapShadowTone               = API_CODE_CapShadowTone,
	XS20_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XS20_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XS20_API_CODE_CapLongExposureNR           = API_CODE_CapLongExposureNR,
	XS20_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XS20_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XS20_API_CODE_CapFullTimeManualFocus      = API_CODE_CapFullTimeManualFocus,
	XS20_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XS20_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XS20_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XS20_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XS20_API_CODE_CapAFMode                   = API_CODE_CapAFMode,
	XS20_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XS20_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XS20_API_CODE_CapFocusPoints              = API_CODE_CapFocusPoints,
	XS20_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XS20_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XS20_API_CODE_CapFocusArea                = API_CODE_CapFocusArea,
	XS20_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XS20_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XS20_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XS20_API_CODE_CapAFStatus                 = API_CODE_CapAFStatus,
	XS20_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XS20_API_CODE_CapShutterPriorityMode      = API_CODE_CapShutterPriorityMode,
	XS20_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XS20_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XS20_API_CODE_CapInstantAFMode            = API_CODE_CapInstantAFMode,
	XS20_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XS20_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XS20_API_CODE_CapPreAFMode                = API_CODE_CapPreAFMode,
	XS20_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XS20_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XS20_API_CODE_CapAFIlluminator            = API_CODE_CapAFIlluminator,
	XS20_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XS20_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XS20_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XS20_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XS20_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XS20_API_CODE_CapISMode                   = API_CODE_CapISMode,
	XS20_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XS20_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XS20_API_CODE_CapLMOMode                  = API_CODE_CapLMOMode,
	XS20_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XS20_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XS20_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XS20_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XS20_API_CODE_CapWhiteBalanceTune         = API_CODE_CapWhiteBalanceTune,
	XS20_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XS20_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XS20_API_CODE_CapCaptureDelay             = API_CODE_CapCaptureDelay,
	XS20_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XS20_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XS20_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XS20_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XS20_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XS20_API_CODE_CapMFAssistMode             = API_CODE_CapMFAssistMode,
	XS20_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XS20_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XS20_API_CODE_CapFocusCheckMode           = API_CODE_CapFocusCheckMode,
	XS20_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XS20_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XS20_API_CODE_CapInterlockAEAFArea        = API_CODE_CapInterlockAEAFArea,
	XS20_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XS20_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XS20_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XS20_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XS20_API_CODE_CapLiveViewImageQuality     = API_CODE_CapLiveViewImageQuality,
	XS20_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XS20_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XS20_API_CODE_CapLiveViewImageSize        = API_CODE_CapLiveViewImageSize,
	XS20_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XS20_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XS20_API_CODE_CapThroughImageZoom         = API_CODE_CapThroughImageZoom,
	XS20_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XS20_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XS20_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XS20_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XS20_API_CODE_CapDateTimeDispFormat       = API_CODE_CapDateTimeDispFormat,
	XS20_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XS20_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XS20_API_CODE_CapWorldClock               = API_CODE_CapWorldClock,
	XS20_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XS20_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XS20_API_CODE_CapTimeDifference           = API_CODE_CapTimeDifference,
	XS20_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XS20_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XS20_API_CODE_CapResetSetting             = API_CODE_CapResetSetting,
	XS20_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XS20_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XS20_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XS20_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XS20_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XS20_API_CODE_CapFunctionLock             = API_CODE_CapFunctionLock,
	XS20_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XS20_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XS20_API_CODE_CapFunctionLockCategory     = API_CODE_CapFunctionLockCategory,
	XS20_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XS20_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XS20_API_CODE_SetComment                  = API_CODE_SetComment,
	XS20_API_CODE_GetComment                  = API_CODE_GetComment,
	XS20_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XS20_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XS20_API_CODE_SetArtist                   = API_CODE_SetArtist,
	XS20_API_CODE_GetArtist                   = API_CODE_GetArtist,
	XS20_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XS20_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XS20_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XS20_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XS20_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XS20_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XS20_API_CODE_CapFrameNumberSequence      = API_CODE_CapFrameNumberSequence,
	XS20_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XS20_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XS20_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XS20_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XS20_API_CODE_CapFormatMemoryCard         = API_CODE_CapFormatMemoryCard,
	XS20_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XS20_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XS20_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XS20_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XS20_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XS20_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XS20_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XS20_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XS20_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XS20_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XS20_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XS20_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XS20_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XS20_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XS20_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XS20_API_CODE_CapExposurePreview          = API_CODE_CapExposurePreview,
	XS20_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XS20_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XS20_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XS20_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XS20_API_CODE_CapFrameGuideMode           = API_CODE_CapFrameGuideMode,
	XS20_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XS20_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XS20_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XS20_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XS20_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XS20_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XS20_API_CODE_CapFocusScaleUnit           = API_CODE_CapFocusScaleUnit,
	XS20_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XS20_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XS20_API_CODE_CapCustomDispInfo           = API_CODE_CapCustomDispInfo,
	XS20_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XS20_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XS20_API_CODE_CapViewMode                 = API_CODE_CapViewMode,
	XS20_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XS20_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XS20_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XS20_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XS20_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XS20_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XS20_API_CODE_CapCustomAutoPowerOff       = API_CODE_CapCustomAutoPowerOff,
	XS20_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XS20_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XS20_API_CODE_CapCustomStudioPowerSave    = API_CODE_CapCustomStudioPowerSave,
	XS20_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XS20_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XS20_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XS20_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XS20_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XS20_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XS20_API_CODE_CapLockButtonMode           = API_CODE_CapLockButtonMode,
	XS20_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XS20_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XS20_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XS20_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XS20_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XS20_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XS20_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XS20_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XS20_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XS20_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XS20_API_CODE_CapWideDynamicRange         = API_CODE_CapWideDynamicRange,
	XS20_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XS20_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XS20_API_CODE_CapClarityMode              = API_CODE_CapClarityMode,
	XS20_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XS20_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XS20_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XS20_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XS20_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XS20_API_CODE_CapCropMode                 = API_CODE_CapCropMode,
	XS20_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XS20_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XS20_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XS20_API_CODE_CapRAWOutputDepth           = API_CODE_CapRAWOutputDepth,
	XS20_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XS20_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XS20_API_CODE_CapSmoothSkinEffect         = API_CODE_CapSmoothSkinEffect,
	XS20_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XS20_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XS20_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XS20_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XS20_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XS20_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XS20_API_CODE_CapColorChromeBlue          = API_CODE_CapColorChromeBlue,
	XS20_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XS20_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XS20_API_CODE_CapMonochromaticColor       = API_CODE_CapMonochromaticColor,
	XS20_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XS20_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XS20_API_CODE_CapFocusLimiterPos          = API_CODE_CapFocusLimiterPos,
	XS20_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XS20_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XS20_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XS20_API_CODE_CapFocusLimiterMode         = API_CODE_CapFocusLimiterMode,
	XS20_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XS20_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
	XS20_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
	XS20_API_CODE_CapPerformanceSettings      = API_CODE_CapPerformanceSettings,
	XS20_API_CODE_SetPerformanceSettings      = API_CODE_SetPerformanceSettings,
	XS20_API_CODE_GetPerformanceSettings      = API_CODE_GetPerformanceSettings,
	XS20_API_CODE_CapMicLineSetting           = API_CODE_CapMicLineSetting,
	XS20_API_CODE_SetMicLineSetting           = API_CODE_SetMicLineSetting,
	XS20_API_CODE_GetMicLineSetting           = API_CODE_GetMicLineSetting,
	XS20_API_CODE_CapCustomSettingAutoUpdate  = API_CODE_CapCustomSettingAutoUpdate,
	XS20_API_CODE_SetCustomSettingAutoUpdate  = API_CODE_SetCustomSettingAutoUpdate,
	XS20_API_CODE_GetCustomSettingAutoUpdate  = API_CODE_GetCustomSettingAutoUpdate,
    XS20_API_CODE_CapPixelShiftSettings       = API_CODE_CapPixelShiftSettings,
    XS20_API_CODE_SetPixelShiftSettings       = API_CODE_SetPixelShiftSettings,
    XS20_API_CODE_GetPixelShiftSettings       = API_CODE_GetPixelShiftSettings,
    XS20_API_CODE_CapImageFormat              = API_CODE_CapImageFormat,
    XS20_API_CODE_SetImageFormat              = API_CODE_SetImageFormat,
    XS20_API_CODE_GetImageFormat              = API_CODE_GetImageFormat,
	XS20_API_CODE_CapSubjectDetectionMode     = API_CODE_CapSubjectDetectionMode,
	XS20_API_CODE_SetSubjectDetectionMode     = API_CODE_SetSubjectDetectionMode,
	XS20_API_CODE_GetSubjectDetectionMode     = API_CODE_GetSubjectDetectionMode,
	XS20_API_CODE_CapCropZoom                 = API_CODE_CapCropZoom,
	XS20_API_CODE_SetCropZoom                 = API_CODE_SetCropZoom,
	XS20_API_CODE_GetCropZoom                 = API_CODE_GetCropZoom,
	XS20_API_CODE_CapZoomOperation            = API_CODE_CapZoomOperation,
	XS20_API_CODE_SetZoomOperation            = API_CODE_SetZoomOperation,
	XS20_API_CODE_CapFocusOperation           = API_CODE_CapFocusOperation,
	XS20_API_CODE_SetFocusOperation           = API_CODE_SetFocusOperation,
	XS20_API_CODE_CapZoomSpeed                = API_CODE_CapZoomSpeed,
	XS20_API_CODE_SetZoomSpeed                = API_CODE_SetZoomSpeed,
	XS20_API_CODE_GetZoomSpeed                = API_CODE_GetZoomSpeed,
	XS20_API_CODE_CapFocusSpeed               = API_CODE_CapFocusSpeed,
	XS20_API_CODE_SetFocusSpeed               = API_CODE_SetFocusSpeed,
	XS20_API_CODE_GetFocusSpeed               = API_CODE_GetFocusSpeed,
	XS20_API_CODE_CapLiveViewStatus           = API_CODE_CapLiveViewStatus,
	XS20_API_CODE_GetLiveViewStatus           = API_CODE_GetLiveViewStatus,
	XS20_API_CODE_CapLiveViewMode             = API_CODE_CapLiveViewMode,
	XS20_API_CODE_SetLiveViewMode             = API_CODE_SetLiveViewMode,
	XS20_API_CODE_GetLiveViewMode             = API_CODE_GetLiveViewMode,
	XS20_API_CODE_CapLiveViewImageRatio       = API_CODE_CapLiveViewImageRatio,
	XS20_API_CODE_SetLiveViewImageRatio       = API_CODE_SetLiveViewImageRatio,
	XS20_API_CODE_GetLiveViewImageRatio       = API_CODE_GetLiveViewImageRatio,
	XS20_API_CODE_CapSummerTime               = API_CODE_CapSummerTime,
	XS20_API_CODE_SetSummerTime               = API_CODE_SetSummerTime,
	XS20_API_CODE_GetSummerTime               = API_CODE_GetSummerTime,
	XS20_API_CODE_CapFanSetting               = API_CODE_CapFanSetting,
	XS20_API_CODE_SetFanSetting               = API_CODE_SetFanSetting,
	XS20_API_CODE_GetFanSetting               = API_CODE_GetFanSetting,
	XS20_API_CODE_CapElectronicLevelSetting   = API_CODE_CapElectronicLevelSetting,
	XS20_API_CODE_SetElectronicLevelSetting   = API_CODE_SetElectronicLevelSetting,
	XS20_API_CODE_GetElectronicLevelSetting   = API_CODE_GetElectronicLevelSetting,
	XS20_API_CODE_CapApertureUnit             = API_CODE_CapApertureUnit,
	XS20_API_CODE_SetApertureUnit             = API_CODE_SetApertureUnit,
	XS20_API_CODE_GetApertureUnit             = API_CODE_GetApertureUnit,
	XS20_API_CODE_CapUSBPowerSupplyCommunication = API_CODE_CapUSBPowerSupplyCommunication,
	XS20_API_CODE_SetUSBPowerSupplyCommunication = API_CODE_SetUSBPowerSupplyCommunication,
	XS20_API_CODE_GetUSBPowerSupplyCommunication = API_CODE_GetUSBPowerSupplyCommunication,
	XS20_API_CODE_CapAutoPowerOffSetting      = API_CODE_CapAutoPowerOffSetting,
	XS20_API_CODE_SetAutoPowerOffSetting      = API_CODE_SetAutoPowerOffSetting,
	XS20_API_CODE_GetAutoPowerOffSetting      = API_CODE_GetAutoPowerOffSetting,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XS20_FocusArea, *PXS20_FocusArea;


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
typedef  SDK_ISOAuto     XS20_ISOAuto, *PXS20_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XS20_FrameGuideGridInfo, *PXS20_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XS20_FOCUS_POS_CAP, *PSDK_XS20_FOCUS_POS_CAP;
typedef  SDK_XS20_FOCUS_POS_CAP     XS20_FOCUS_POS_CAP, *PXS20_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XS20_FOLDER_INFO, *PXS20_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XS20_CropAreaFrameInfo, *PXS20_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XS20_FaceFrameInfo, *PXS20_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XS20_CustomWBArea, *PXS20_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XS20_FOCUS_LIMITER_INDICATOR, *PXS20_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XS20_FOCUS_LIMITER, *PXS20_FOCUS_LIMITER;


// Image Size
enum{
	XS20_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XS20_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XS20_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XS20_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XS20_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XS20_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XS20_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XS20_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XS20_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XS20_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XS20_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XS20_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XS20_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XS20_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XS20_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XS20_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XS20_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XS20_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XS20_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XS20_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XS20_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XS20_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XS20_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XS20_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XS20_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XS20_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XS20_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XS20_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// Image Format
enum{
	XS20_IMAGEFORMAT_JPEG         = SDK_IMAGEFORMAT_JPEG,
	XS20_IMAGEFORMAT_HEIF         = SDK_IMAGEFORMAT_HEIF,
};

// RAW Image Quality
enum{
	XS20_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XS20_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Mode
enum{
	XS20_LIVEVIEW_MODE1           = SDK_LIVEVIEW_MODE1,
	XS20_LIVEVIEW_MODE2           = SDK_LIVEVIEW_MODE2,
};

// LiveView Ratio
enum{
	XS20_LIVEVIEW_RATIO_FIXED     = SDK_LIVEVIEW_RATIO_FIXED,
	XS20_LIVEVIEW_RATIO_VARIABLE  = SDK_LIVEVIEW_RATIO_VARIABLE,
};

// LiveView Image Quality
enum{
	XS20_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XS20_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XS20_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XS20_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XS20_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XS20_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XS20_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XS20_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XS20_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XS20_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XS20_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XS20_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XS20_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XS20_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XS20_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XS20_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XS20_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XS20_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XS20_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XS20_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XS20_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XS20_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XS20_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XS20_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XS20_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XS20_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XS20_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
	XS20_THROUGH_ZOOM_120    = SDK_THROUGH_ZOOM_120,
};

// Color Space
enum{
	XS20_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XS20_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XS20_WB_AUTO            = SDK_WB_AUTO,  
	XS20_WB_AUTO_WHITE_PRIORITY    = SDK_WB_AUTO_WHITE_PRIORITY,
	XS20_WB_AUTO_AMBIENCE_PRIORITY = SDK_WB_AUTO_AMBIENCE_PRIORITY,
	XS20_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XS20_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XS20_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XS20_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XS20_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XS20_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XS20_WB_SHADE           = SDK_WB_SHADE,
	XS20_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XS20_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XS20_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XS20_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XS20_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XS20_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XS20_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XS20_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XS20_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XS20_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XS20_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XS20_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XS20_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XS20_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XS20_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XS20_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XS20_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XS20_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XS20_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XS20_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XS20_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XS20_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XS20_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XS20_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XS20_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XS20_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XS20_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XS20_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XS20_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XS20_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XS20_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XS20_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XS20_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XS20_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XS20_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XS20_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XS20_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XS20_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XS20_WB_R_SHIFT_STEP    = 1,
	XS20_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XS20_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XS20_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XS20_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XS20_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XS20_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XS20_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XS20_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XS20_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XS20_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XS20_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XS20_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XS20_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XS20_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XS20_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XS20_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XS20_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XS20_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XS20_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XS20_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XS20_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XS20_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XS20_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XS20_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
	XS20_FILMSIMULATION_BLEACH_BYPASS   = SDK_FILMSIMULATION_BLEACH_BYPASS,
	XS20_FILMSIMULATION_NOSTALGICNEG    = SDK_FILMSIMULATION_NOSTALGICNEG,
	XS20_FILMSIMULATION_REALA_ACE       = SDK_FILMSIMULATION_REALA_ACE,
};

// COLOR
enum{
	XS20_COLOR_HIGH          = SDK_COLOR_HIGH,
	XS20_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XS20_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XS20_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XS20_COLOR_LOW           = SDK_COLOR_LOW,
	XS20_COLOR_P4            = SDK_COLOR_P4,
	XS20_COLOR_P3            = SDK_COLOR_P3,
	XS20_COLOR_P2            = SDK_COLOR_P2,
	XS20_COLOR_P1            = SDK_COLOR_P1,
	XS20_COLOR_0             = SDK_COLOR_0,
	XS20_COLOR_M1            = SDK_COLOR_M1,
	XS20_COLOR_M2            = SDK_COLOR_M2,
	XS20_COLOR_M3            = SDK_COLOR_M3,
	XS20_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XS20_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XS20_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XS20_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XS20_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XS20_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XS20_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XS20_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XS20_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XS20_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XS20_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XS20_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XS20_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XS20_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XS20_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XS20_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XS20_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XS20_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XS20_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XS20_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XS20_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XS20_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XS20_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XS20_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XS20_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XS20_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XS20_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XS20_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XS20_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XS20_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XS20_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XS20_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XS20_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XS20_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XS20_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XS20_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XS20_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XS20_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XS20_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XS20_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XS20_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XS20_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XS20_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XS20_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XS20_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XS20_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XS20_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XS20_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XS20_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XS20_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XS20_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XS20_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XS20_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XS20_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XS20_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XS20_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XS20_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XS20_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XS20_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XS20_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XS20_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XS20_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XS20_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XS20_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XS20_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XS20_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XS20_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XS20_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XS20_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XS20_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XS20_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XS20_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XS20_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XS20_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XS20_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XS20_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XS20_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XS20_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XS20_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XS20_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XS20_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XS20_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XS20_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XS20_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XS20_SHARPNESS_0               = SDK_SHARPNESS_0,
	XS20_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XS20_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XS20_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XS20_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XS20_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XS20_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XS20_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XS20_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XS20_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XS20_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XS20_HIGHLIGHT_TONE_P3_5       = SDK_HIGHLIGHT_TONE_P3_5,
	XS20_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XS20_HIGHLIGHT_TONE_P2_5       = SDK_HIGHLIGHT_TONE_P2_5,
	XS20_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XS20_HIGHLIGHT_TONE_P1_5       = SDK_HIGHLIGHT_TONE_P1_5,
	XS20_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XS20_HIGHLIGHT_TONE_P0_5       = SDK_HIGHLIGHT_TONE_P0_5,
	XS20_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XS20_HIGHLIGHT_TONE_M0_5       = SDK_HIGHLIGHT_TONE_M0_5,
	XS20_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XS20_HIGHLIGHT_TONE_M1_5       = SDK_HIGHLIGHT_TONE_M1_5,
	XS20_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XS20_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XS20_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XS20_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XS20_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XS20_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XS20_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XS20_SHADOW_TONE_P3_5          = SDK_SHADOW_TONE_P3_5,
	XS20_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XS20_SHADOW_TONE_P2_5          = SDK_SHADOW_TONE_P2_5,
	XS20_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XS20_SHADOW_TONE_P1_5          = SDK_SHADOW_TONE_P1_5,
	XS20_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XS20_SHADOW_TONE_P0_5          = SDK_SHADOW_TONE_P0_5,
	XS20_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XS20_SHADOW_TONE_M0_5          = SDK_SHADOW_TONE_M0_5,
	XS20_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XS20_SHADOW_TONE_M1_5          = SDK_SHADOW_TONE_M1_5,
	XS20_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XS20_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XS20_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XS20_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XS20_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XS20_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XS20_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XS20_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XS20_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XS20_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XS20_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XS20_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XS20_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XS20_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XS20_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XS20_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XS20_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XS20_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XS20_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XS20_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XS20_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XS20_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XS20_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XS20_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
	XS20_RAW_COMPRESSION_LOSSY		 = SDK_RAW_COMPRESSION_LOSSY,
};

// Grain Effect
enum{
	XS20_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XS20_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XS20_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XS20_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XS20_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XS20_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XS20_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XS20_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XS20_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XS20_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XS20_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XS20_CLARITY_P5		= SDK_CLARITY_P5,
	XS20_CLARITY_P4		= SDK_CLARITY_P4,
	XS20_CLARITY_P3		= SDK_CLARITY_P3,
	XS20_CLARITY_P2		= SDK_CLARITY_P2,
	XS20_CLARITY_P1		= SDK_CLARITY_P1,
	XS20_CLARITY_0			= SDK_CLARITY_0 ,
	XS20_CLARITY_M1		= SDK_CLARITY_M1,
	XS20_CLARITY_M2		= SDK_CLARITY_M2,
	XS20_CLARITY_M3		= SDK_CLARITY_M3,
	XS20_CLARITY_M4		= SDK_CLARITY_M4,
	XS20_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XS20_SHADOWING_0             = SDK_SHADOWING_0,
	XS20_SHADOWING_P1            = SDK_SHADOWING_P1,
	XS20_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XS20_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XS20_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XS20_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XS20_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XS20_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XS20_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XS20_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XS20_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XS20_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XS20_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XS20_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XS20_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XS20_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XS20_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XS20_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XS20_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XS20_FOCUS_AFS        = SDK_FOCUS_AFS,
	XS20_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XS20_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XS20_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XS20_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XS20_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XS20_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XS20_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XS20_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XS20_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Focus Points
enum{
	XS20_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XS20_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XS20_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XS20_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XS20_AF_ALL     = SDK_AF_ALL,
	XS20_AF_MULTI   = SDK_AF_MULTI,
	XS20_AF_AREA    = SDK_AF_AREA,
	XS20_AF_SINGLE  = XS20_AF_AREA,
	XS20_AF_ZONE    = SDK_AF_ZONE,
	XS20_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XS20_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XS20_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XS20_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XS20_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XS20_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XS20_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XS20_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XS20_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XS20_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XS20_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XS20_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XS20_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XS20_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XS20_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XS20_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XS20_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XS20_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XS20_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XS20_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XS20_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XS20_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XS20_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XS20_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XS20_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XS20_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XS20_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
	XS20_MF_ASSIST_FOCUSMETER               = SDK_MF_ASSIST_FOCUSMETER,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_RED_L    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_L,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_RED_H    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_H,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L,
	XS20_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H,
	XS20_MF_ASSIST_FOCUSMAP_BW              = SDK_MF_ASSIST_FOCUSMAP_BW,
	XS20_MF_ASSIST_FOCUSMAP_COLOR           = SDK_MF_ASSIST_FOCUSMAP_COLOR,
};

// FOCUS AREA
enum{
	XS20_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XS20_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XS20_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XS20_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XS20_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XS20_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XS20_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XS20_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XS20_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XS20_MACRO_MODE           = SDK_MACRO_MODE,
	XS20_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XS20_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XS20_DRIVE_MODE_CL   = 0x1000,
	XS20_DRIVE_MODE_CH   = 0x10F0,
	XS20_DRIVE_MODE_BKT  = 0x4000,
	XS20_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE,
};

// USB MODE
enum{
	XS20_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XS20_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XS20_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XS20_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XS20_BEEP_HIGH        = SDK_BEEP_HIGH,
	XS20_BEEP_MID         = SDK_BEEP_MID,
	XS20_BEEP_LOW         = SDK_BEEP_LOW,
	XS20_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XS20_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XS20_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XS20_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XS20_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XS20_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XS20_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XS20_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XS20_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XS20_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XS20_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XS20_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XS20_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XS20_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XS20_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XS20_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XS20_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XS20_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XS20_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XS20_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XS20_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XS20_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XS20_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XS20_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XS20_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XS20_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XS20_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XS20_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XS20_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XS20_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XS20_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XS20_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XS20_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XS20_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XS20_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XS20_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XS20_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XS20_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XS20_FRAMEGUIDE_GRID_HD      = 0x0003,
	XS20_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XS20_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XS20_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XS20_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XS20_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XS20_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XS20_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XS20_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XS20_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XS20_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XS20_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XS20_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XS20_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XS20_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XS20_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XS20_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XS20_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XS20_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XS20_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XS20_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XS20_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XS20_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XS20_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XS20_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XS20_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XS20_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XS20_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XS20_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XS20_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XS20_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XS20_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XS20_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XS20_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XS20_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XS20_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XS20_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XS20_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XS20_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XS20_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XS20_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XS20_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XS20_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XS20_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XS20_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XS20_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XS20_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XS20_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XS20_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XS20_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XS20_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XS20_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XS20_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XS20_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XS20_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XS20_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XS20_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XS20_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XS20_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XS20_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XS20_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XS20_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XS20_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XS20_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XS20_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XS20_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XS20_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XS20_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XS20_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XS20_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XS20_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XS20_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XS20_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XS20_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XS20_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XS20_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XS20_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XS20_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XS20_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XS20_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XS20_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XS20_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XS20_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XS20_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XS20_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XS20_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XS20_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XS20_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XS20_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XS20_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XS20_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XS20_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XS20_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XS20_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XS20_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XS20_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XS20_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XS20_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XS20_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XS20_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XS20_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XS20_CUSTOMDISPINFO_LIVEVIEWHIGHT                 = SDK_CUSTOMDISPINFO_LIVEVIEWHIGHT,
	XS20_CUSTOMDISPINFO_EXPOSUREBIASDIGIT             = SDK_CUSTOMDISPINFO_EXPOSUREBIASDIGIT,
	XS20_CUSTOMDISPINFO_TOUCHSCREENMODE               = SDK_CUSTOMDISPINFO_TOUCHSCREENMODE,
	XS20_CUSTOMDISPINFO_BOOSTMODE                     = SDK_CUSTOMDISPINFO_BOOSTMODE,
	XS20_CUSTOMDISPINFO_IMAGETRANSFERORDER            = SDK_CUSTOMDISPINFO_IMAGETRANSFERORDER,
	XS20_CUSTOMDISPINFO_MICLEVEL                      = SDK_CUSTOMDISPINFO_MICLEVEL,
	XS20_CUSTOMDISPINFO_GUIDANCEMESSAGE               = SDK_CUSTOMDISPINFO_GUIDANCEMESSAGE,
	XS20_CUSTOMDISPINFO_FRAMEOUTLINE                  = SDK_CUSTOMDISPINFO_FRAMEOUTLINE,
	XS20_CUSTOMDISPINFO_35MMFORMAT                    = SDK_CUSTOMDISPINFO_35MMFORMAT,
	XS20_CUSTOMDISPINFO_COOLINGFANSETTING             = SDK_CUSTOMDISPINFO_COOLINGFANSETTING,
	XS20_CUSTOMDISPINFO_DIGITALTELECONV               = SDK_CUSTOMDISPINFO_DIGITALTELECONV,
	XS20_CUSTOMDISPINFO_DIGITALZOOM                   = SDK_CUSTOMDISPINFO_DIGITALZOOM,
	XS20_CUSTOMDISPINFO_FOCUSINDICATOR                = SDK_CUSTOMDISPINFO_FOCUSINDICATOR,
	XS20_CUSTOMDISPINFO_NOCARDWARNING                 = SDK_CUSTOMDISPINFO_NOCARDWARNING,
	XS20_CUSTOMDISPINFO_DATETIME                      = SDK_CUSTOMDISPINFO_DATETIME,
	XS20_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XS20_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XS20_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XS20_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XS20_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XS20_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XS20_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XS20_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XS20_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XS20_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XS20_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XS20_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XS20_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XS20_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XS20_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XS20_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XS20_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XS20_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XS20_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XS20_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XS20_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XS20_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XS20_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XS20_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XS20_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XS20_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XS20_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XS20_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XS20_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XS20_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XS20_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XS20_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XS20_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XS20_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XS20_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XS20_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XS20_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XS20_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XS20_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XS20_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XS20_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XS20_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XS20_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XS20_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XS20_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XS20_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XS20_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XS20_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XS20_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XS20_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XS20_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XS20_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XS20_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XS20_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XS20_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XS20_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XS20_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XS20_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XS20_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XS20_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XS20_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XS20_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XS20_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XS20_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER  = SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER,
	XS20_FUNCTIONLOCK_CATEGORY2_MODEDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_MODEDIAL,
	XS20_FUNCTIONLOCK_CATEGORY2_FDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FDIAL,
	XS20_FUNCTIONLOCK_CATEGORY2_FN_DIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FN_DIAL,
	XS20_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT		= SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT,
	XS20_FUNCTIONLOCK_CATEGORY3_ISOBUTTON            = SDK_FUNCTIONLOCK_CATEGORY3_ISOBUTTON,
	XS20_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE      = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE,
	XS20_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE         = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE,
	XS20_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU      = SDK_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU,
	XS20_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE         = SDK_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE,
	XS20_FUNCTIONLOCK_CATEGORY3_WBBUTTON             = SDK_FUNCTIONLOCK_CATEGORY3_WBBUTTON,
	XS20_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING     = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING,
	XS20_FUNCTIONLOCK_CATEGORY3_BLUETOOTH            = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTH,
	XS20_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT        = SDK_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT,
	XS20_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING = SDK_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING,
	XS20_FUNCTIONLOCK_CATEGORY3_FM1                  = SDK_FUNCTIONLOCK_CATEGORY3_FM1,
	XS20_FUNCTIONLOCK_CATEGORY3_FM2                  = SDK_FUNCTIONLOCK_CATEGORY3_FM2,
	XS20_FUNCTIONLOCK_CATEGORY3_FM3                  = SDK_FUNCTIONLOCK_CATEGORY3_FM3,
	XS20_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION = SDK_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION,
	XS20_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP      = SDK_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP,
	XS20_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XS20_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XS20_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XS20_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XS20_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XS20_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XS20_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XS20_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XS20_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XS20_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XS20_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XS20_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XS20_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XS20_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XS20_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XS20_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XS20_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XS20_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XS20_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XS20_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XS20_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XS20_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XS20_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XS20_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	XS20_MEDIASTATUS_ACCESSING             = SDK_MEDIASTATUS_ACCESSING,
	XS20_MEDIASTATUS_INCOMPATIBLE          = SDK_MEDIASTATUS_INCOMPATIBLE,
};

enum{
	XS20_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XS20_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XS20_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XS20_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XS20_ON  = SDK_ON,
	XS20_OFF = SDK_OFF,
};

enum{
	XS20_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XS20_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XS20_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XS20_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XS20_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XS20_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XS20_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XS20_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XS20_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XS20_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XS20_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XS20_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XS20_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XS20_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XS20_ITEM_RESET_MOVIEMENU         = SDK_ITEM_RESET_MOVIEMENU,
	XS20_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XS20_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XS20_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XS20_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XS20_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XS20_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XS20_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XS20_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XS20_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XS20_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XS20_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XS20_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XS20_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XS20_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XS20_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XS20_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XS20_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XS20_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XS20_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XS20_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XS20_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XS20_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XS20_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XS20_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XS20_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XS20_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XS20_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XS20_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XS20_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XS20_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XS20_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XS20_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XS20_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XS20_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XS20_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XS20_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XS20_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XS20_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XS20_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XS20_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XS20_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XS20_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};

// Micline Setting
enum{
	XS20_MICLINE_SETTING_MIC          = SDK_MICLINE_SETTING_MIC,
	XS20_MICLINE_SETTING_LINE         = SDK_MICLINE_SETTING_LINE,
};

// Shuttercount Type
enum{
	XS20_SHUTTERCOUNT_TYPE_FRONTCURTAIN = SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN,
	XS20_SHUTTERCOUNT_TYPE_REARCURTAIN  = SDK_SHUTTERCOUNT_TYPE_REARCURTAIN,
	XS20_SHUTTERCOUNT_TYPE_TOTAL        = SDK_SHUTTERCOUNT_TYPE_TOTAL,
};

// Performance
enum{
	XS20_PERFORMANCE_NORMAL                    = SDK_PERFORMANCE_NORMAL,
	XS20_PERFORMANCE_ECONOMY                   = SDK_PERFORMANCE_ECONOMY,
	XS20_PERFORMANCE_BOOST_LOWLIGHT            = SDK_PERFORMANCE_BOOST_LOWLIGHT,
	XS20_PERFORMANCE_BOOST_RESOLUTION_PRIORITY = SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY,
	XS20_PERFORMANCE_BOOST_FRAMERATE_PRIORITY  = SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY,
	XS20_PERFORMANCE_BOOST_AFPRIORITY_NORMAL   = SDK_PERFORMANCE_BOOST_AFPRIORITY_NORMAL,
	XS20_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION= SDK_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION,
};

// PixelShift Settings
enum{
	XS20_PIXELSHIFT_INTERVAL_SHORTEST = SDK_PIXELSHIFT_INTERVAL_SHORTEST,
	XS20_PIXELSHIFT_INTERVAL_1S       = SDK_PIXELSHIFT_INTERVAL_1S,
	XS20_PIXELSHIFT_INTERVAL_2S       = SDK_PIXELSHIFT_INTERVAL_2S,
	XS20_PIXELSHIFT_INTERVAL_5S       = SDK_PIXELSHIFT_INTERVAL_5S,
	XS20_PIXELSHIFT_INTERVAL_15S      = SDK_PIXELSHIFT_INTERVAL_15S,
};

// SubjectDetectionMode
enum{
	XS20_SUBJECT_DETECTION_OFF        = SDK_SUBJECT_DETECTION_OFF,
	XS20_SUBJECT_DETECTION_ANIMAL     = SDK_SUBJECT_DETECTION_ANIMAL,
	XS20_SUBJECT_DETECTION_BIRD       = SDK_SUBJECT_DETECTION_BIRD,
	XS20_SUBJECT_DETECTION_CAR        = SDK_SUBJECT_DETECTION_CAR,
	XS20_SUBJECT_DETECTION_BIKE       = SDK_SUBJECT_DETECTION_BIKE,
	XS20_SUBJECT_DETECTION_AIRPLANE   = SDK_SUBJECT_DETECTION_AIRPLANE,
	XS20_SUBJECT_DETECTION_TRAIN      = SDK_SUBJECT_DETECTION_TRAIN,
	XS20_SUBJECT_DETECTION_ALL        = SDK_SUBJECT_DETECTION_ALL,
};

// FanSetting
enum{
	XS20_FAN_SETTING_OFF              = SDK_FAN_SETTING_OFF,
	XS20_FAN_SETTING_WEAK             = SDK_FAN_SETTING_WEAK,
	XS20_FAN_SETTING_STRONG           = SDK_FAN_SETTING_STRONG,
	XS20_FAN_SETTING_AUTO1            = SDK_FAN_SETTING_AUTO1,
	XS20_FAN_SETTING_AUTO2            = SDK_FAN_SETTING_AUTO2,
};

// ElectronicLevelSetting
enum{
	XS20_ELECTRONIC_LEVEL_SETTING_OFF = SDK_ELECTRONIC_LEVEL_SETTING_OFF,
	XS20_ELECTRONIC_LEVEL_SETTING_2D  = SDK_ELECTRONIC_LEVEL_SETTING_2D,
	XS20_ELECTRONIC_LEVEL_SETTING_3D  = SDK_ELECTRONIC_LEVEL_SETTING_3D,
};

// ApertureUnit
enum{
	XS20_APERTURE_UNIT_TNUMBER        = SDK_APERTURE_UNIT_TNUMBER,
	XS20_APERTURE_UNIT_FNUMBER        = SDK_APERTURE_UNIT_FNUMBER,
};

// USBPowerSupplyCommunication
enum{
	XS20_USB_POWER_SUPPLY_COMMUNICATION_AUTO  = SDK_USB_POWER_SUPPLY_COMMUNICATION_AUTO,
	XS20_USB_POWER_SUPPLY_COMMUNICATION_ON    = SDK_USB_POWER_SUPPLY_COMMUNICATION_ON,
	XS20_USB_POWER_SUPPLY_COMMUNICATION_OFF   = SDK_USB_POWER_SUPPLY_COMMUNICATION_OFF,
};

// AutoPowerOffSetting
enum{
	XS20_AUTOPOWEROFF_OFF             = SDK_AUTOPOWEROFF_OFF,
	XS20_AUTOPOWEROFF_15SEC           = SDK_AUTOPOWEROFF_15SEC,
	XS20_AUTOPOWEROFF_30SEC           = SDK_AUTOPOWEROFF_30SEC,
	XS20_AUTOPOWEROFF_1MIN            = SDK_AUTOPOWEROFF_1MIN,
	XS20_AUTOPOWEROFF_2MIN            = SDK_AUTOPOWEROFF_2MIN,
	XS20_AUTOPOWEROFF_5MIN            = SDK_AUTOPOWEROFF_5MIN,
};
