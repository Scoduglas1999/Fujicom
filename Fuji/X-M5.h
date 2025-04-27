///////////////////////////////////////////////////
//  X-M5.h
//     Version 1.0.0.0
//
//     Copyright (C) 2024 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-M5
//
enum {
	XM5_API_PARAM_CapHighFrequencyFlickerlessMode = 2,
	XM5_API_PARAM_SetHighFrequencyFlickerlessMode = 1,
	XM5_API_PARAM_GetHighFrequencyFlickerlessMode = 1,
	XM5_API_PARAM_CapImageSize                = 2,
	XM5_API_PARAM_SetImageSize                = 1,
	XM5_API_PARAM_GetImageSize                = 1,
	XM5_API_PARAM_CapSharpness                = 2,
	XM5_API_PARAM_SetSharpness                = 1,
	XM5_API_PARAM_GetSharpness                = 1,
	XM5_API_PARAM_CapColorMode                = 2,
	XM5_API_PARAM_SetColorMode                = 1,
	XM5_API_PARAM_GetColorMode                = 1,
	XM5_API_PARAM_CapFilmSimulationMode       = 2,
	XM5_API_PARAM_SetFilmSimulationMode       = 1,
	XM5_API_PARAM_GetFilmSimulationMode       = 1,
	XM5_API_PARAM_CapColorSpace               = 2,
	XM5_API_PARAM_SetColorSpace               = 1,
	XM5_API_PARAM_GetColorSpace               = 1,
	XM5_API_PARAM_CapImageQuality             = 2,
	XM5_API_PARAM_SetImageQuality             = 1,
	XM5_API_PARAM_GetImageQuality             = 1,
	XM5_API_PARAM_CapRAWCompression           = 2,
	XM5_API_PARAM_SetRAWCompression           = 1,
	XM5_API_PARAM_GetRAWCompression           = 1,
	XM5_API_PARAM_CapGrainEffect              = 2,
	XM5_API_PARAM_SetGrainEffect              = 1,
	XM5_API_PARAM_GetGrainEffect              = 1,
	XM5_API_PARAM_CapShadowing                = 2,
	XM5_API_PARAM_SetShadowing                = 1,
	XM5_API_PARAM_GetShadowing                = 1,
	XM5_API_PARAM_CapNoiseReduction           = 2,
	XM5_API_PARAM_SetNoiseReduction           = 1,
	XM5_API_PARAM_GetNoiseReduction           = 1,
	XM5_API_PARAM_CapFaceDetectionMode        = 2,
	XM5_API_PARAM_SetFaceDetectionMode        = 1,
	XM5_API_PARAM_GetFaceDetectionMode        = 1,
	XM5_API_PARAM_CapEyeAFMode                = 2,
	XM5_API_PARAM_SetEyeAFMode                = 1,
	XM5_API_PARAM_GetEyeAFMode                = 1,
	XM5_API_PARAM_SetMacroMode                = -1,
	XM5_API_PARAM_GetMacroMode                = -1,
	XM5_API_PARAM_CapFocusLimiter             = -1,
	XM5_API_PARAM_SetFocusLimiter             = -1,
	XM5_API_PARAM_GetFocusLimiter             = -1,
	XM5_API_PARAM_CapHighLightTone            = 2,
	XM5_API_PARAM_SetHighLightTone            = 1,
	XM5_API_PARAM_GetHighLightTone            = 1,
	XM5_API_PARAM_CapShadowTone               = 2,
	XM5_API_PARAM_SetShadowTone               = 1,
	XM5_API_PARAM_GetShadowTone               = 1,
	XM5_API_PARAM_CapLongExposureNR           = 2,
	XM5_API_PARAM_SetLongExposureNR           = 1,
	XM5_API_PARAM_GetLongExposureNR           = 1,
	XM5_API_PARAM_CapFullTimeManualFocus      = 2,
	XM5_API_PARAM_SetFullTimeManualFocus      = 1,
	XM5_API_PARAM_GetFullTimeManualFocus      = 1,
	XM5_API_PARAM_SetFocusMode                = 1,
	XM5_API_PARAM_GetFocusMode                = 1,
	XM5_API_PARAM_CapAFMode                   = 3,
	XM5_API_PARAM_SetAFMode                   = 2,
	XM5_API_PARAM_GetAFMode                   = 2,
	XM5_API_PARAM_CapFocusPoints              = 2,
	XM5_API_PARAM_SetFocusPoints              = 1,
	XM5_API_PARAM_GetFocusPoints              = 1,
	XM5_API_PARAM_CapFocusArea                = 3,
	XM5_API_PARAM_SetFocusArea                = 2,
	XM5_API_PARAM_GetFocusArea                = 2,
	XM5_API_PARAM_CapFocusMode                = 2,
	XM5_API_PARAM_CapAFStatus                 = 2,
	XM5_API_PARAM_GetAFStatus                 = 1,
	XM5_API_PARAM_CapShutterPriorityMode      = 3,
	XM5_API_PARAM_SetShutterPriorityMode      = 2,
	XM5_API_PARAM_GetShutterPriorityMode      = 2,
	XM5_API_PARAM_CapInstantAFMode            = 2,
	XM5_API_PARAM_SetInstantAFMode            = 1,
	XM5_API_PARAM_GetInstantAFMode            = 1,
	XM5_API_PARAM_CapPreAFMode                = 2,
	XM5_API_PARAM_SetPreAFMode                = 1,
	XM5_API_PARAM_GetPreAFMode                = 1,
	XM5_API_PARAM_CapAFIlluminator            = 2,
	XM5_API_PARAM_SetAFIlluminator            = 1,
	XM5_API_PARAM_GetAFIlluminator            = 1,
	XM5_API_PARAM_CapLensISSwitch             = 2,
	XM5_API_PARAM_SetLensISSwitch             = 1,
	XM5_API_PARAM_GetLensISSwitch             = 1,
	XM5_API_PARAM_CapISMode                   = 2,
	XM5_API_PARAM_SetISMode                   = 1,
	XM5_API_PARAM_GetISMode                   = 1,
	XM5_API_PARAM_CapLMOMode                  = 2,
	XM5_API_PARAM_SetLMOMode                  = 1,
	XM5_API_PARAM_GetLMOMode                  = 1,
	XM5_API_PARAM_SetWhiteBalanceMode         = -1,
	XM5_API_PARAM_GetWhiteBalanceMode         = -1,
	XM5_API_PARAM_CapWhiteBalanceTune         = 5,
	XM5_API_PARAM_SetWhiteBalanceTune         = 3,
	XM5_API_PARAM_GetWhiteBalanceTune         = 3,
	XM5_API_PARAM_CapCaptureDelay             = 2,
	XM5_API_PARAM_SetCaptureDelay             = 1,
	XM5_API_PARAM_GetCaptureDelay             = 1,
	XM5_API_PARAM_CapFocusPos                 = 2,
	XM5_API_PARAM_SetFocusPos                 = 1,
	XM5_API_PARAM_GetFocusPos                 = 1,
	XM5_API_PARAM_CapMFAssistMode             = 2,
	XM5_API_PARAM_SetMFAssistMode             = 1,
	XM5_API_PARAM_GetMFAssistMode             = 1,
	XM5_API_PARAM_CapFocusCheckMode           = 2,
	XM5_API_PARAM_SetFocusCheckMode           = 1,
	XM5_API_PARAM_GetFocusCheckMode           = 1,
	XM5_API_PARAM_CapInterlockAEAFArea        = 2,
	XM5_API_PARAM_SetInterlockAEAFArea        = 1,
	XM5_API_PARAM_GetInterlockAEAFArea        = 1,
	XM5_API_PARAM_StartLiveView               = 0,
	XM5_API_PARAM_StopLiveView                = 0,
	XM5_API_PARAM_CapLiveViewImageQuality     = 2,
	XM5_API_PARAM_SetLiveViewImageQuality     = 1,
	XM5_API_PARAM_GetLiveViewImageQuality     = 1,
	XM5_API_PARAM_CapLiveViewImageSize        = 2,
	XM5_API_PARAM_SetLiveViewImageSize        = 1,
	XM5_API_PARAM_GetLiveViewImageSize        = 1,
	XM5_API_PARAM_CapThroughImageZoom         = 2,
	XM5_API_PARAM_SetThroughImageZoom         = 1,
	XM5_API_PARAM_GetThroughImageZoom         = 1,
	XM5_API_PARAM_SetDateTime                 = 6,
	XM5_API_PARAM_GetDateTime                 = 6,
	XM5_API_PARAM_CapDateTimeDispFormat       = 2,
	XM5_API_PARAM_SetDateTimeDispFormat       = 1,
	XM5_API_PARAM_GetDateTimeDispFormat       = 1,
	XM5_API_PARAM_CapWorldClock               = 2,
	XM5_API_PARAM_SetWorldClock               = 1,
	XM5_API_PARAM_GetWorldClock               = 1,
	XM5_API_PARAM_CapTimeDifference           = 4,
	XM5_API_PARAM_SetTimeDifference           = 2,
	XM5_API_PARAM_GetTimeDifference           = 2,
	XM5_API_PARAM_CapResetSetting             = 2,
	XM5_API_PARAM_ResetSetting                = 1,
	XM5_API_PARAM_SetSilentMode               = -1,
	XM5_API_PARAM_GetSilentMode               = -1,
	XM5_API_PARAM_SetBeep                     = -1,
	XM5_API_PARAM_GetBeep                     = -1,
	XM5_API_PARAM_SetFunctionLock             = 1,
	XM5_API_PARAM_GetFunctionLock             = 1,
	XM5_API_PARAM_CapFunctionLock             = 2,
	XM5_API_PARAM_SetFunctionLockCategory     = 3,
	XM5_API_PARAM_GetFunctionLockCategory     = 3,
	XM5_API_PARAM_CapFunctionLockCategory     = 6,
	XM5_API_PARAM_SetComment                  = 1,
	XM5_API_PARAM_GetComment                  = 1,
	XM5_API_PARAM_SetCopyright                = 2,
	XM5_API_PARAM_GetCopyright                = 2,
	XM5_API_PARAM_SetArtist                   = 1,
	XM5_API_PARAM_GetArtist                   = 1,
	XM5_API_PARAM_SetFilenamePrefix           = 2,
	XM5_API_PARAM_GetFilenamePrefix           = 2,
	XM5_API_PARAM_SetFoldernameSuffix         = 3,
	XM5_API_PARAM_GetFoldernameSuffix         = 3,
	XM5_API_PARAM_GetFoldernameList           = 4,
	XM5_API_PARAM_CheckBatteryInfo            = 8,
	XM5_API_PARAM_CapFrameNumberSequence      = 2,
	XM5_API_PARAM_SetFrameNumberSequence      = 1,
	XM5_API_PARAM_GetFrameNumberSequence      = 1,
	XM5_API_PARAM_SetUSBMode                  = -1,
	XM5_API_PARAM_GetUSBMode                  = -1,
	XM5_API_PARAM_CapFormatMemoryCard         = 2,
	XM5_API_PARAM_FormatMemoryCard            = 1,
	XM5_API_PARAM_SetMediaRecord              = 1,
	XM5_API_PARAM_GetMediaRecord              = 1,
	XM5_API_PARAM_GetMediaCapacity            = 5,
	XM5_API_PARAM_GetMediaStatus              = 2,
	XM5_API_PARAM_GetShutterCount             = 3,
	XM5_API_PARAM_GetShutterCountEx           = 3,
	XM5_API_PARAM_SetSensorCleanTiming        = -1,
	XM5_API_PARAM_GetSensorCleanTiming        = -1,
	XM5_API_PARAM_SetPreviewTime              = -1,
	XM5_API_PARAM_GetPreviewTime              = -1,
	XM5_API_PARAM_SetEVFDispAutoRotate        = -1,
	XM5_API_PARAM_GetEVFDispAutoRotate        = -1,
	XM5_API_PARAM_SetDispMMode                = 1,
	XM5_API_PARAM_GetDispMMode                = 1,
	XM5_API_PARAM_CapExposurePreview          = 2,
	XM5_API_PARAM_SetExposurePreview          = 1,
	XM5_API_PARAM_GetExposurePreview          = 1,
	XM5_API_PARAM_SetDispBrightness           = -1,
	XM5_API_PARAM_GetDispBrightness           = -1,
	XM5_API_PARAM_CapFrameGuideMode           = 2,
	XM5_API_PARAM_SetFrameGuideMode           = 1,
	XM5_API_PARAM_GetFrameGuideMode           = 1,
	XM5_API_PARAM_SetFrameGuideGridInfo       = 2,
	XM5_API_PARAM_GetFrameGuideGridInfo       = 2,
	XM5_API_PARAM_SetAutoImageRotation        = -1,
	XM5_API_PARAM_GetAutoImageRotation        = -1,
	XM5_API_PARAM_CapFocusScaleUnit           = 2,
	XM5_API_PARAM_SetFocusScaleUnit           = 1,
	XM5_API_PARAM_GetFocusScaleUnit           = 1,
	XM5_API_PARAM_CapCustomDispInfo           = 4,
	XM5_API_PARAM_SetCustomDispInfo           = 2,
	XM5_API_PARAM_GetCustomDispInfo           = 2,
	XM5_API_PARAM_CapViewMode                 = 3,
	XM5_API_PARAM_SetViewMode                 = 2,
	XM5_API_PARAM_GetViewMode                 = 2,
	XM5_API_PARAM_SetDispInfoMode             = -1,
	XM5_API_PARAM_GetDispInfoMode             = -1,
	XM5_API_PARAM_SetDispChroma               = -1,
	XM5_API_PARAM_GetDispChroma               = -1,
	XM5_API_PARAM_CapCustomAutoPowerOff       = 2,
	XM5_API_PARAM_SetCustomAutoPowerOff       = 1,
	XM5_API_PARAM_GetCustomAutoPowerOff       = 1,
	XM5_API_PARAM_CapCustomStudioPowerSave    = -1,
	XM5_API_PARAM_SetCustomStudioPowerSave    = -1,
	XM5_API_PARAM_GetCustomStudioPowerSave    = -1,
	XM5_API_PARAM_SetFunctionButton           = -1,
	XM5_API_PARAM_GetFunctionButton           = -1,
	XM5_API_PARAM_SetISODialHn                = -1,
	XM5_API_PARAM_GetISODialHn                = -1,
	XM5_API_PARAM_CapLockButtonMode           = 2,
	XM5_API_PARAM_SetLockButtonMode           = 1,
	XM5_API_PARAM_GetLockButtonMode           = 1,
	XM5_API_PARAM_SetAFLockMode               = -1,
	XM5_API_PARAM_GetAFLockMode               = -1,
	XM5_API_PARAM_SetMicJackMode              = -1,
	XM5_API_PARAM_GetMicJackMode              = -1,
	XM5_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XM5_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XM5_API_PARAM_SetCrossKeyAssign           = -1,
	XM5_API_PARAM_GetCrossKeyAssign           = -1,
	XM5_API_PARAM_CapWideDynamicRange         = 2,
	XM5_API_PARAM_SetWideDynamicRange         = 1,
	XM5_API_PARAM_GetWideDynamicRange         = 1,
	XM5_API_PARAM_CapClarityMode              = 2,
	XM5_API_PARAM_SetClarityMode              = 1,
	XM5_API_PARAM_GetClarityMode              = 1,
	XM5_API_PARAM_SetBlackImageTone           = -1,
	XM5_API_PARAM_GetBlackImageTone           = -1,
	XM5_API_PARAM_GetTNumber                  = 1,
	XM5_API_PARAM_CapCropMode                 = 2,
	XM5_API_PARAM_SetCropMode                 = 1,
	XM5_API_PARAM_GetCropMode                 = 2,
	XM5_API_PARAM_GetCropAreaFrameInfo        = 1,
	XM5_API_PARAM_CapRAWOutputDepth           = -1,
	XM5_API_PARAM_SetRAWOutputDepth           = -1,
	XM5_API_PARAM_GetRAWOutputDepth           = -1,
	XM5_API_PARAM_CapSmoothSkinEffect         = -1,
	XM5_API_PARAM_SetSmoothSkinEffect         = -1,
	XM5_API_PARAM_GetSmoothSkinEffect         = -1,
	XM5_API_PARAM_GetDetectedFaceFrame        = 2,
	XM5_API_PARAM_SetDetectedFaceFrame        = 1,
    XM5_API_PARAM_SetCustomWBArea             = 2,
    XM5_API_PARAM_GetCustomWBArea             = 2,
	XM5_API_PARAM_CapColorChromeBlue          = 2,
	XM5_API_PARAM_SetColorChromeBlue          = 1,
	XM5_API_PARAM_GetColorChromeBlue          = 1,
	XM5_API_PARAM_CapMonochromaticColor       = 4,
	XM5_API_PARAM_SetMonochromaticColor       = 2,
	XM5_API_PARAM_GetMonochromaticColor       = 2,
	XM5_API_PARAM_CapFocusLimiterPos          = 4,
	XM5_API_PARAM_SetFocusLimiterPos          = 2,
	XM5_API_PARAM_GetFocusLimiterIndicator    = 1,
	XM5_API_PARAM_GetFocusLimiterRange        = 2,
	XM5_API_PARAM_CapFocusLimiterMode         = 2,
	XM5_API_PARAM_SetFocusLimiterMode         = 1,
	XM5_API_PARAM_GetFocusLimiterMode         = 1,
	XM5_API_PARAM_GetCommandDialStatus        = 4,
	XM5_API_PARAM_CapPerformanceSettings      = 2,
	XM5_API_PARAM_SetPerformanceSettings      = 1,
	XM5_API_PARAM_GetPerformanceSettings      = 1,
	XM5_API_PARAM_CapMicLineSetting           = 2,
	XM5_API_PARAM_SetMicLineSetting           = 1,
	XM5_API_PARAM_GetMicLineSetting           = 1,
	XM5_API_PARAM_CapCustomSettingAutoUpdate  = 2,
	XM5_API_PARAM_SetCustomSettingAutoUpdate  = 1,
	XM5_API_PARAM_GetCustomSettingAutoUpdate  = 1,
    XM5_API_PARAM_CapPixelShiftSettings       = -1,
    XM5_API_PARAM_SetPixelShiftSettings       = -1,
    XM5_API_PARAM_GetPixelShiftSettings       = -1,
    XM5_API_PARAM_CapImageFormat              = 2,
    XM5_API_PARAM_SetImageFormat              = 1,
    XM5_API_PARAM_GetImageFormat              = 1,
	XM5_API_PARAM_CapSubjectDetectionMode     = 2,
	XM5_API_PARAM_SetSubjectDetectionMode     = 1,
	XM5_API_PARAM_GetSubjectDetectionMode     = 1,
	XM5_API_PARAM_CapCropZoom                 = -1,
	XM5_API_PARAM_SetCropZoom                 = -1,
	XM5_API_PARAM_GetCropZoom                 = -1,
	XM5_API_PARAM_CapZoomOperation            = 4,
	XM5_API_PARAM_SetZoomOperation            = 3,
	XM5_API_PARAM_CapFocusOperation           = 4,
	XM5_API_PARAM_SetFocusOperation           = 3,
	XM5_API_PARAM_CapZoomSpeed                = 2,
	XM5_API_PARAM_SetZoomSpeed                = 1,
	XM5_API_PARAM_GetZoomSpeed                = 1,
	XM5_API_PARAM_CapFocusSpeed               = 2,
	XM5_API_PARAM_SetFocusSpeed               = 1,
	XM5_API_PARAM_GetFocusSpeed               = 1,
	XM5_API_PARAM_CapLiveViewStatus           = 2,
	XM5_API_PARAM_GetLiveViewStatus           = 1,
	XM5_API_PARAM_CapLiveViewMode             = 2,
	XM5_API_PARAM_SetLiveViewMode             = 1,
	XM5_API_PARAM_GetLiveViewMode             = 1,
	XM5_API_PARAM_CapLiveViewImageRatio       = 2,
	XM5_API_PARAM_SetLiveViewImageRatio       = 1,
	XM5_API_PARAM_GetLiveViewImageRatio       = 1,
	XM5_API_PARAM_CapSummerTime               = 2,
	XM5_API_PARAM_SetSummerTime               = 1,
	XM5_API_PARAM_GetSummerTime               = 1,
	XM5_API_PARAM_CapFanSetting               = 2,
	XM5_API_PARAM_SetFanSetting               = 1,
	XM5_API_PARAM_GetFanSetting               = 1,
	XM5_API_PARAM_CapElectronicLevelSetting   = 2,
	XM5_API_PARAM_SetElectronicLevelSetting   = 1,
	XM5_API_PARAM_GetElectronicLevelSetting   = 1,
	XM5_API_PARAM_CapApertureUnit             = 2,
	XM5_API_PARAM_SetApertureUnit             = 1,
	XM5_API_PARAM_GetApertureUnit             = 1,
	XM5_API_PARAM_CapUSBPowerSupplyCommunication = 2,
	XM5_API_PARAM_SetUSBPowerSupplyCommunication = 1,
	XM5_API_PARAM_GetUSBPowerSupplyCommunication = 1,
	XM5_API_PARAM_CapAutoPowerOffSetting      = 2,
	XM5_API_PARAM_SetAutoPowerOffSetting      = 1,
	XM5_API_PARAM_GetAutoPowerOffSetting      = 1,
	XM5_API_PARAM_GetTiltShiftLensStatus      = -1,
	XM5_API_PARAM_CapAFZoneCustom             = 2,
	XM5_API_PARAM_SetAFZoneCustom             = 2,
	XM5_API_PARAM_GetAFZoneCustom             = 2,
	XM5_API_PARAM_CapPortraitEnhancer         = 2,
	XM5_API_PARAM_SetPortraitEnhancer         = 1,
	XM5_API_PARAM_GetPortraitEnhancer         = 1,
};

//
// API_CODE for X-M5
//
enum {
	XM5_API_CODE_CapHighFrequencyFlickerlessMode = API_CODE_CapHighFrequencyFlickerlessMode,
	XM5_API_CODE_SetHighFrequencyFlickerlessMode = API_CODE_SetHighFrequencyFlickerlessMode,
	XM5_API_CODE_GetHighFrequencyFlickerlessMode = API_CODE_GetHighFrequencyFlickerlessMode,
	XM5_API_CODE_CapImageSize                = API_CODE_CapImageSize,
	XM5_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XM5_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XM5_API_CODE_CapSharpness                = API_CODE_CapSharpness,
	XM5_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XM5_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XM5_API_CODE_CapColorMode                = API_CODE_CapColorMode,
	XM5_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XM5_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XM5_API_CODE_CapFilmSimulationMode       = API_CODE_CapFilmSimulationMode,
	XM5_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XM5_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XM5_API_CODE_CapColorSpace               = API_CODE_CapColorSpace,
	XM5_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XM5_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XM5_API_CODE_CapImageQuality             = API_CODE_CapImageQuality,
	XM5_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XM5_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XM5_API_CODE_CapRAWCompression           = API_CODE_CapRAWCompression,
	XM5_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XM5_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XM5_API_CODE_CapGrainEffect              = API_CODE_CapGrainEffect,
	XM5_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XM5_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XM5_API_CODE_CapShadowing                = API_CODE_CapShadowing,
	XM5_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XM5_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XM5_API_CODE_CapNoiseReduction           = API_CODE_CapNoiseReduction,
	XM5_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XM5_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XM5_API_CODE_CapFaceDetectionMode        = API_CODE_CapFaceDetectionMode,
	XM5_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XM5_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XM5_API_CODE_CapEyeAFMode                = API_CODE_CapEyeAFMode,
	XM5_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XM5_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XM5_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XM5_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XM5_API_CODE_CapHighLightTone            = API_CODE_CapHighLightTone,
	XM5_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XM5_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XM5_API_CODE_CapShadowTone               = API_CODE_CapShadowTone,
	XM5_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XM5_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XM5_API_CODE_CapLongExposureNR           = API_CODE_CapLongExposureNR,
	XM5_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XM5_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XM5_API_CODE_CapFullTimeManualFocus      = API_CODE_CapFullTimeManualFocus,
	XM5_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XM5_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XM5_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XM5_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XM5_API_CODE_CapAFMode                   = API_CODE_CapAFMode,
	XM5_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XM5_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XM5_API_CODE_CapFocusPoints              = API_CODE_CapFocusPoints,
	XM5_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XM5_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XM5_API_CODE_CapFocusArea                = API_CODE_CapFocusArea,
	XM5_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XM5_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XM5_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XM5_API_CODE_CapAFStatus                 = API_CODE_CapAFStatus,
	XM5_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XM5_API_CODE_CapShutterPriorityMode      = API_CODE_CapShutterPriorityMode,
	XM5_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XM5_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XM5_API_CODE_CapInstantAFMode            = API_CODE_CapInstantAFMode,
	XM5_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XM5_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XM5_API_CODE_CapPreAFMode                = API_CODE_CapPreAFMode,
	XM5_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XM5_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XM5_API_CODE_CapAFIlluminator            = API_CODE_CapAFIlluminator,
	XM5_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XM5_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XM5_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XM5_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XM5_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XM5_API_CODE_CapISMode                   = API_CODE_CapISMode,
	XM5_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XM5_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XM5_API_CODE_CapLMOMode                  = API_CODE_CapLMOMode,
	XM5_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XM5_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XM5_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XM5_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XM5_API_CODE_CapWhiteBalanceTune         = API_CODE_CapWhiteBalanceTune,
	XM5_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XM5_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XM5_API_CODE_CapCaptureDelay             = API_CODE_CapCaptureDelay,
	XM5_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XM5_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XM5_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XM5_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XM5_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XM5_API_CODE_CapMFAssistMode             = API_CODE_CapMFAssistMode,
	XM5_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XM5_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XM5_API_CODE_CapFocusCheckMode           = API_CODE_CapFocusCheckMode,
	XM5_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XM5_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XM5_API_CODE_CapInterlockAEAFArea        = API_CODE_CapInterlockAEAFArea,
	XM5_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XM5_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XM5_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XM5_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XM5_API_CODE_CapLiveViewImageQuality     = API_CODE_CapLiveViewImageQuality,
	XM5_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XM5_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XM5_API_CODE_CapLiveViewImageSize        = API_CODE_CapLiveViewImageSize,
	XM5_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XM5_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XM5_API_CODE_CapThroughImageZoom         = API_CODE_CapThroughImageZoom,
	XM5_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XM5_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XM5_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XM5_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XM5_API_CODE_CapDateTimeDispFormat       = API_CODE_CapDateTimeDispFormat,
	XM5_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XM5_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XM5_API_CODE_CapWorldClock               = API_CODE_CapWorldClock,
	XM5_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XM5_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XM5_API_CODE_CapTimeDifference           = API_CODE_CapTimeDifference,
	XM5_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XM5_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XM5_API_CODE_CapResetSetting             = API_CODE_CapResetSetting,
	XM5_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XM5_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XM5_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XM5_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XM5_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XM5_API_CODE_CapFunctionLock             = API_CODE_CapFunctionLock,
	XM5_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XM5_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XM5_API_CODE_CapFunctionLockCategory     = API_CODE_CapFunctionLockCategory,
	XM5_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XM5_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XM5_API_CODE_SetComment                  = API_CODE_SetComment,
	XM5_API_CODE_GetComment                  = API_CODE_GetComment,
	XM5_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XM5_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XM5_API_CODE_SetArtist                   = API_CODE_SetArtist,
	XM5_API_CODE_GetArtist                   = API_CODE_GetArtist,
	XM5_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XM5_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XM5_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XM5_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XM5_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XM5_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XM5_API_CODE_CapFrameNumberSequence      = API_CODE_CapFrameNumberSequence,
	XM5_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XM5_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XM5_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XM5_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XM5_API_CODE_CapFormatMemoryCard         = API_CODE_CapFormatMemoryCard,
	XM5_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XM5_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XM5_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XM5_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XM5_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XM5_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XM5_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XM5_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XM5_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XM5_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XM5_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XM5_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XM5_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XM5_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XM5_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XM5_API_CODE_CapExposurePreview          = API_CODE_CapExposurePreview,
	XM5_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XM5_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XM5_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XM5_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XM5_API_CODE_CapFrameGuideMode           = API_CODE_CapFrameGuideMode,
	XM5_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XM5_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XM5_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XM5_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XM5_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XM5_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XM5_API_CODE_CapFocusScaleUnit           = API_CODE_CapFocusScaleUnit,
	XM5_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XM5_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XM5_API_CODE_CapCustomDispInfo           = API_CODE_CapCustomDispInfo,
	XM5_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XM5_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XM5_API_CODE_CapViewMode                 = API_CODE_CapViewMode,
	XM5_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XM5_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XM5_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XM5_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XM5_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XM5_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XM5_API_CODE_CapCustomAutoPowerOff       = API_CODE_CapCustomAutoPowerOff,
	XM5_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XM5_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XM5_API_CODE_CapCustomStudioPowerSave    = API_CODE_CapCustomStudioPowerSave,
	XM5_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XM5_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XM5_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XM5_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XM5_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XM5_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XM5_API_CODE_CapLockButtonMode           = API_CODE_CapLockButtonMode,
	XM5_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XM5_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XM5_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XM5_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XM5_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XM5_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XM5_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XM5_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XM5_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XM5_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XM5_API_CODE_CapWideDynamicRange         = API_CODE_CapWideDynamicRange,
	XM5_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XM5_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XM5_API_CODE_CapClarityMode              = API_CODE_CapClarityMode,
	XM5_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XM5_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XM5_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XM5_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XM5_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XM5_API_CODE_CapCropMode                 = API_CODE_CapCropMode,
	XM5_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XM5_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XM5_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XM5_API_CODE_CapRAWOutputDepth           = API_CODE_CapRAWOutputDepth,
	XM5_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XM5_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XM5_API_CODE_CapSmoothSkinEffect         = API_CODE_CapSmoothSkinEffect,
	XM5_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XM5_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XM5_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XM5_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XM5_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XM5_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XM5_API_CODE_CapColorChromeBlue          = API_CODE_CapColorChromeBlue,
	XM5_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XM5_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XM5_API_CODE_CapMonochromaticColor       = API_CODE_CapMonochromaticColor,
	XM5_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XM5_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XM5_API_CODE_CapFocusLimiterPos          = API_CODE_CapFocusLimiterPos,
	XM5_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XM5_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XM5_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XM5_API_CODE_CapFocusLimiterMode         = API_CODE_CapFocusLimiterMode,
	XM5_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XM5_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
	XM5_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
	XM5_API_CODE_CapPerformanceSettings      = API_CODE_CapPerformanceSettings,
	XM5_API_CODE_SetPerformanceSettings      = API_CODE_SetPerformanceSettings,
	XM5_API_CODE_GetPerformanceSettings      = API_CODE_GetPerformanceSettings,
	XM5_API_CODE_CapMicLineSetting           = API_CODE_CapMicLineSetting,
	XM5_API_CODE_SetMicLineSetting           = API_CODE_SetMicLineSetting,
	XM5_API_CODE_GetMicLineSetting           = API_CODE_GetMicLineSetting,
	XM5_API_CODE_CapCustomSettingAutoUpdate  = API_CODE_CapCustomSettingAutoUpdate,
	XM5_API_CODE_SetCustomSettingAutoUpdate  = API_CODE_SetCustomSettingAutoUpdate,
	XM5_API_CODE_GetCustomSettingAutoUpdate  = API_CODE_GetCustomSettingAutoUpdate,
    XM5_API_CODE_CapPixelShiftSettings       = API_CODE_CapPixelShiftSettings,
    XM5_API_CODE_SetPixelShiftSettings       = API_CODE_SetPixelShiftSettings,
    XM5_API_CODE_GetPixelShiftSettings       = API_CODE_GetPixelShiftSettings,
    XM5_API_CODE_CapImageFormat              = API_CODE_CapImageFormat,
    XM5_API_CODE_SetImageFormat              = API_CODE_SetImageFormat,
    XM5_API_CODE_GetImageFormat              = API_CODE_GetImageFormat,
	XM5_API_CODE_CapSubjectDetectionMode     = API_CODE_CapSubjectDetectionMode,
	XM5_API_CODE_SetSubjectDetectionMode     = API_CODE_SetSubjectDetectionMode,
	XM5_API_CODE_GetSubjectDetectionMode     = API_CODE_GetSubjectDetectionMode,
	XM5_API_CODE_CapCropZoom                 = API_CODE_CapCropZoom,
	XM5_API_CODE_SetCropZoom                 = API_CODE_SetCropZoom,
	XM5_API_CODE_GetCropZoom                 = API_CODE_GetCropZoom,
	XM5_API_CODE_CapZoomOperation            = API_CODE_CapZoomOperation,
	XM5_API_CODE_SetZoomOperation            = API_CODE_SetZoomOperation,
	XM5_API_CODE_CapFocusOperation           = API_CODE_CapFocusOperation,
	XM5_API_CODE_SetFocusOperation           = API_CODE_SetFocusOperation,
	XM5_API_CODE_CapZoomSpeed                = API_CODE_CapZoomSpeed,
	XM5_API_CODE_SetZoomSpeed                = API_CODE_SetZoomSpeed,
	XM5_API_CODE_GetZoomSpeed                = API_CODE_GetZoomSpeed,
	XM5_API_CODE_CapFocusSpeed               = API_CODE_CapFocusSpeed,
	XM5_API_CODE_SetFocusSpeed               = API_CODE_SetFocusSpeed,
	XM5_API_CODE_GetFocusSpeed               = API_CODE_GetFocusSpeed,
	XM5_API_CODE_CapLiveViewStatus           = API_CODE_CapLiveViewStatus,
	XM5_API_CODE_GetLiveViewStatus           = API_CODE_GetLiveViewStatus,
	XM5_API_CODE_CapLiveViewMode             = API_CODE_CapLiveViewMode,
	XM5_API_CODE_SetLiveViewMode             = API_CODE_SetLiveViewMode,
	XM5_API_CODE_GetLiveViewMode             = API_CODE_GetLiveViewMode,
	XM5_API_CODE_CapLiveViewImageRatio       = API_CODE_CapLiveViewImageRatio,
	XM5_API_CODE_SetLiveViewImageRatio       = API_CODE_SetLiveViewImageRatio,
	XM5_API_CODE_GetLiveViewImageRatio       = API_CODE_GetLiveViewImageRatio,
	XM5_API_CODE_CapSummerTime               = API_CODE_CapSummerTime,
	XM5_API_CODE_SetSummerTime               = API_CODE_SetSummerTime,
	XM5_API_CODE_GetSummerTime               = API_CODE_GetSummerTime,
	XM5_API_CODE_CapFanSetting               = API_CODE_CapFanSetting,
	XM5_API_CODE_SetFanSetting               = API_CODE_SetFanSetting,
	XM5_API_CODE_GetFanSetting               = API_CODE_GetFanSetting,
	XM5_API_CODE_CapElectronicLevelSetting   = API_CODE_CapElectronicLevelSetting,
	XM5_API_CODE_SetElectronicLevelSetting   = API_CODE_SetElectronicLevelSetting,
	XM5_API_CODE_GetElectronicLevelSetting   = API_CODE_GetElectronicLevelSetting,
	XM5_API_CODE_CapApertureUnit             = API_CODE_CapApertureUnit,
	XM5_API_CODE_SetApertureUnit             = API_CODE_SetApertureUnit,
	XM5_API_CODE_GetApertureUnit             = API_CODE_GetApertureUnit,
	XM5_API_CODE_CapUSBPowerSupplyCommunication = API_CODE_CapUSBPowerSupplyCommunication,
	XM5_API_CODE_SetUSBPowerSupplyCommunication = API_CODE_SetUSBPowerSupplyCommunication,
	XM5_API_CODE_GetUSBPowerSupplyCommunication = API_CODE_GetUSBPowerSupplyCommunication,
	XM5_API_CODE_CapAutoPowerOffSetting      = API_CODE_CapAutoPowerOffSetting,
	XM5_API_CODE_SetAutoPowerOffSetting      = API_CODE_SetAutoPowerOffSetting,
	XM5_API_CODE_GetAutoPowerOffSetting      = API_CODE_GetAutoPowerOffSetting,
	XM5_API_CODE_GetTiltShiftLensStatus      = API_CODE_GetTiltShiftLensStatus,
	XM5_API_CODE_CapAFZoneCustom             = API_CODE_CapAFZoneCustom,
	XM5_API_CODE_SetAFZoneCustom             = API_CODE_SetAFZoneCustom,
	XM5_API_CODE_GetAFZoneCustom             = API_CODE_GetAFZoneCustom,
	XM5_API_CODE_CapPortraitEnhancer         = API_CODE_CapPortraitEnhancer,
	XM5_API_CODE_SetPortraitEnhancer         = API_CODE_SetPortraitEnhancer,
	XM5_API_CODE_GetPortraitEnhancer         = API_CODE_GetPortraitEnhancer,

};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XM5_FocusArea, *PXM5_FocusArea;


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
typedef  SDK_ISOAuto     XM5_ISOAuto, *PXM5_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XM5_FrameGuideGridInfo, *PXM5_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XM5_FOCUS_POS_CAP, *PSDK_XM5_FOCUS_POS_CAP;
typedef  SDK_XM5_FOCUS_POS_CAP     XM5_FOCUS_POS_CAP, *PXM5_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XM5_FOLDER_INFO, *PXM5_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XM5_CropAreaFrameInfo, *PXM5_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XM5_FaceFrameInfo, *PXM5_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XM5_CustomWBArea, *PXM5_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XM5_FOCUS_LIMITER_INDICATOR, *PXM5_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XM5_FOCUS_LIMITER, *PXM5_FOCUS_LIMITER;

// AFZoneCustom
//typedef	struct _SDK_AFZoneCustom {
//	long	h;
//	long	v;
//} SDK_AFZoneCustom;
typedef  SDK_AFZoneCustom     XM5_AFZoneCustom, *PXM5_AFZoneCustom;

//typedef	struct _SDK_AFZoneCustomCapablity {
//	long				mode;
//	SDK_AFZoneCustom	min;
//	SDK_AFZoneCustom	max;
//} SDK_AFZoneCustomCapablity;
typedef  SDK_AFZoneCustomCapablity     XM5_AFZoneCustomCapablity, *PXM5_AFZoneCustomCapablity;

// Image Size
enum{
	XM5_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XM5_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XM5_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XM5_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XM5_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XM5_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XM5_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XM5_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XM5_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XM5_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XM5_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XM5_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XM5_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XM5_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XM5_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XM5_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XM5_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XM5_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XM5_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XM5_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XM5_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XM5_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XM5_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XM5_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XM5_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XM5_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XM5_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XM5_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// Image Format
enum{
	XM5_IMAGEFORMAT_JPEG         = SDK_IMAGEFORMAT_JPEG,
	XM5_IMAGEFORMAT_HEIF         = SDK_IMAGEFORMAT_HEIF,
};

// RAW Image Quality
enum{
	XM5_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XM5_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Mode
enum{
	XM5_LIVEVIEW_MODE1           = SDK_LIVEVIEW_MODE1,
	XM5_LIVEVIEW_MODE2           = SDK_LIVEVIEW_MODE2,
};

// LiveView Ratio
enum{
	XM5_LIVEVIEW_RATIO_FIXED     = SDK_LIVEVIEW_RATIO_FIXED,
	XM5_LIVEVIEW_RATIO_VARIABLE  = SDK_LIVEVIEW_RATIO_VARIABLE,
};

// LiveView Image Quality
enum{
	XM5_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XM5_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XM5_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XM5_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XM5_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XM5_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XM5_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XM5_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XM5_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XM5_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XM5_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XM5_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XM5_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XM5_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XM5_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XM5_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XM5_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XM5_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XM5_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XM5_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XM5_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XM5_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XM5_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XM5_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XM5_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XM5_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XM5_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
	XM5_THROUGH_ZOOM_120    = SDK_THROUGH_ZOOM_120,
};

// Color Space
enum{
	XM5_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XM5_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XM5_WB_AUTO            = SDK_WB_AUTO,  
	XM5_WB_AUTO_WHITE_PRIORITY    = SDK_WB_AUTO_WHITE_PRIORITY,
	XM5_WB_AUTO_AMBIENCE_PRIORITY = SDK_WB_AUTO_AMBIENCE_PRIORITY,
	XM5_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XM5_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XM5_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XM5_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XM5_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XM5_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XM5_WB_SHADE           = SDK_WB_SHADE,
	XM5_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XM5_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XM5_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XM5_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XM5_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XM5_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XM5_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XM5_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XM5_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XM5_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XM5_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XM5_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XM5_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XM5_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XM5_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XM5_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XM5_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XM5_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XM5_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XM5_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XM5_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XM5_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XM5_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XM5_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XM5_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XM5_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XM5_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XM5_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XM5_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XM5_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XM5_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XM5_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XM5_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XM5_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XM5_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XM5_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XM5_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XM5_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XM5_WB_R_SHIFT_STEP    = 1,
	XM5_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XM5_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XM5_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XM5_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XM5_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XM5_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XM5_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XM5_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XM5_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XM5_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XM5_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XM5_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XM5_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XM5_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XM5_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XM5_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XM5_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XM5_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XM5_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XM5_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XM5_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XM5_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XM5_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XM5_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
	XM5_FILMSIMULATION_BLEACH_BYPASS   = SDK_FILMSIMULATION_BLEACH_BYPASS,
	XM5_FILMSIMULATION_NOSTALGICNEG    = SDK_FILMSIMULATION_NOSTALGICNEG,
	XM5_FILMSIMULATION_REALA_ACE       = SDK_FILMSIMULATION_REALA_ACE,
	XM5_FILMSIMULATION_AUTO            = SDK_FILMSIMULATION_AUTO,
};

// COLOR
enum{
	XM5_COLOR_HIGH          = SDK_COLOR_HIGH,
	XM5_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XM5_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XM5_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XM5_COLOR_LOW           = SDK_COLOR_LOW,
	XM5_COLOR_P4            = SDK_COLOR_P4,
	XM5_COLOR_P3            = SDK_COLOR_P3,
	XM5_COLOR_P2            = SDK_COLOR_P2,
	XM5_COLOR_P1            = SDK_COLOR_P1,
	XM5_COLOR_0             = SDK_COLOR_0,
	XM5_COLOR_M1            = SDK_COLOR_M1,
	XM5_COLOR_M2            = SDK_COLOR_M2,
	XM5_COLOR_M3            = SDK_COLOR_M3,
	XM5_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XM5_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XM5_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XM5_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XM5_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XM5_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XM5_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XM5_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XM5_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XM5_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XM5_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XM5_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XM5_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XM5_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XM5_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XM5_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XM5_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XM5_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XM5_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XM5_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XM5_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XM5_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XM5_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XM5_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XM5_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XM5_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XM5_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XM5_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XM5_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XM5_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XM5_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XM5_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XM5_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XM5_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XM5_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XM5_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XM5_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XM5_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XM5_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XM5_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XM5_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XM5_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XM5_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XM5_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XM5_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XM5_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XM5_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XM5_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XM5_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XM5_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XM5_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XM5_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XM5_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XM5_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XM5_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XM5_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XM5_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XM5_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XM5_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XM5_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XM5_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XM5_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XM5_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XM5_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XM5_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XM5_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XM5_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XM5_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XM5_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XM5_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XM5_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XM5_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XM5_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XM5_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XM5_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XM5_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XM5_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XM5_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XM5_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XM5_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XM5_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XM5_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XM5_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XM5_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XM5_SHARPNESS_0               = SDK_SHARPNESS_0,
	XM5_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XM5_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XM5_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XM5_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XM5_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XM5_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XM5_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XM5_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XM5_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XM5_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XM5_HIGHLIGHT_TONE_P3_5       = SDK_HIGHLIGHT_TONE_P3_5,
	XM5_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XM5_HIGHLIGHT_TONE_P2_5       = SDK_HIGHLIGHT_TONE_P2_5,
	XM5_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XM5_HIGHLIGHT_TONE_P1_5       = SDK_HIGHLIGHT_TONE_P1_5,
	XM5_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XM5_HIGHLIGHT_TONE_P0_5       = SDK_HIGHLIGHT_TONE_P0_5,
	XM5_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XM5_HIGHLIGHT_TONE_M0_5       = SDK_HIGHLIGHT_TONE_M0_5,
	XM5_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XM5_HIGHLIGHT_TONE_M1_5       = SDK_HIGHLIGHT_TONE_M1_5,
	XM5_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XM5_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XM5_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XM5_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XM5_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XM5_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XM5_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XM5_SHADOW_TONE_P3_5          = SDK_SHADOW_TONE_P3_5,
	XM5_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XM5_SHADOW_TONE_P2_5          = SDK_SHADOW_TONE_P2_5,
	XM5_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XM5_SHADOW_TONE_P1_5          = SDK_SHADOW_TONE_P1_5,
	XM5_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XM5_SHADOW_TONE_P0_5          = SDK_SHADOW_TONE_P0_5,
	XM5_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XM5_SHADOW_TONE_M0_5          = SDK_SHADOW_TONE_M0_5,
	XM5_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XM5_SHADOW_TONE_M1_5          = SDK_SHADOW_TONE_M1_5,
	XM5_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XM5_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XM5_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XM5_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XM5_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XM5_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XM5_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XM5_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XM5_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XM5_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XM5_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XM5_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XM5_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XM5_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XM5_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XM5_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XM5_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XM5_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XM5_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XM5_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XM5_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XM5_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XM5_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XM5_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
	XM5_RAW_COMPRESSION_LOSSY		 = SDK_RAW_COMPRESSION_LOSSY,
};

// Grain Effect
enum{
	XM5_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XM5_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XM5_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XM5_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XM5_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XM5_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XM5_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XM5_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XM5_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XM5_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XM5_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XM5_CLARITY_P5		= SDK_CLARITY_P5,
	XM5_CLARITY_P4		= SDK_CLARITY_P4,
	XM5_CLARITY_P3		= SDK_CLARITY_P3,
	XM5_CLARITY_P2		= SDK_CLARITY_P2,
	XM5_CLARITY_P1		= SDK_CLARITY_P1,
	XM5_CLARITY_0			= SDK_CLARITY_0 ,
	XM5_CLARITY_M1		= SDK_CLARITY_M1,
	XM5_CLARITY_M2		= SDK_CLARITY_M2,
	XM5_CLARITY_M3		= SDK_CLARITY_M3,
	XM5_CLARITY_M4		= SDK_CLARITY_M4,
	XM5_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XM5_SHADOWING_0             = SDK_SHADOWING_0,
	XM5_SHADOWING_P1            = SDK_SHADOWING_P1,
	XM5_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XM5_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XM5_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XM5_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XM5_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XM5_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XM5_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XM5_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XM5_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XM5_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XM5_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XM5_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XM5_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XM5_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XM5_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XM5_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XM5_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XM5_FOCUS_AFS        = SDK_FOCUS_AFS,
	XM5_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XM5_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XM5_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XM5_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XM5_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XM5_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XM5_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XM5_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XM5_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Crop Zoom
enum{
	XM5_CROP_ZOOM_OFF            = SDK_CROP_ZOOM_OFF,
	XM5_CROP_ZOOM_10             = SDK_CROP_ZOOM_10,
	XM5_CROP_ZOOM_11             = SDK_CROP_ZOOM_11,
	XM5_CROP_ZOOM_12             = SDK_CROP_ZOOM_12,
	XM5_CROP_ZOOM_13             = SDK_CROP_ZOOM_13,
	XM5_CROP_ZOOM_14             = SDK_CROP_ZOOM_14,
	XM5_CROP_ZOOM_15             = SDK_CROP_ZOOM_15,
	XM5_CROP_ZOOM_16             = SDK_CROP_ZOOM_16,
	XM5_CROP_ZOOM_17             = SDK_CROP_ZOOM_17,
	XM5_CROP_ZOOM_18             = SDK_CROP_ZOOM_18,
	XM5_CROP_ZOOM_19             = SDK_CROP_ZOOM_19,
	XM5_CROP_ZOOM_20             = SDK_CROP_ZOOM_20,
};

// Zoom Speed
enum{
	XM5_LENS_ZOOM_SPEED_1        = SDK_LENS_ZOOM_SPEED_1,
	XM5_LENS_ZOOM_SPEED_2        = SDK_LENS_ZOOM_SPEED_2,
	XM5_LENS_ZOOM_SPEED_3        = SDK_LENS_ZOOM_SPEED_3,
	XM5_LENS_ZOOM_SPEED_4        = SDK_LENS_ZOOM_SPEED_4,
	XM5_LENS_ZOOM_SPEED_5        = SDK_LENS_ZOOM_SPEED_5,
	XM5_LENS_ZOOM_SPEED_6        = SDK_LENS_ZOOM_SPEED_6,
	XM5_LENS_ZOOM_SPEED_7        = SDK_LENS_ZOOM_SPEED_7,
	XM5_LENS_ZOOM_SPEED_8        = SDK_LENS_ZOOM_SPEED_8,
};

// Zoom Operation
enum{
	XM5_ZOOM_OPERATION_START     = SDK_ZOOM_OPERATION_START,
	XM5_ZOOM_OPERATION_STOP      = SDK_ZOOM_OPERATION_STOP,
	XM5_ZOOM_DIRECTION_WIDE      = SDK_ZOOM_DIRECTION_WIDE,
	XM5_ZOOM_DIRECTION_TELE      = SDK_ZOOM_DIRECTION_TELE,
};

// Focus Speed
enum{
	XM5_LENS_FOCUS_SPEED_1       = SDK_LENS_FOCUS_SPEED_1,
	XM5_LENS_FOCUS_SPEED_2       = SDK_LENS_FOCUS_SPEED_2,
	XM5_LENS_FOCUS_SPEED_3       = SDK_LENS_FOCUS_SPEED_3,
	XM5_LENS_FOCUS_SPEED_4       = SDK_LENS_FOCUS_SPEED_4,
	XM5_LENS_FOCUS_SPEED_5       = SDK_LENS_FOCUS_SPEED_5,
	XM5_LENS_FOCUS_SPEED_6       = SDK_LENS_FOCUS_SPEED_6,
	XM5_LENS_FOCUS_SPEED_7       = SDK_LENS_FOCUS_SPEED_7,
	XM5_LENS_FOCUS_SPEED_8       = SDK_LENS_FOCUS_SPEED_8,
};

// Focus Operation
enum{
	XM5_FOCUS_OPERATION_START    = SDK_FOCUS_OPERATION_START,
	XM5_FOCUS_OPERATION_STOP     = SDK_FOCUS_OPERATION_STOP,
	XM5_FOCUS_DIRECTION_NEAR     = SDK_FOCUS_DIRECTION_NEAR,
	XM5_FOCUS_DIRECTION_FAR      = SDK_FOCUS_DIRECTION_FAR,
};

// Focus Points
enum{
	XM5_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XM5_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XM5_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XM5_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XM5_AF_ALL     = SDK_AF_ALL,
	XM5_AF_MULTI   = SDK_AF_MULTI,
	XM5_AF_AREA    = SDK_AF_AREA,
	XM5_AF_SINGLE  = XM5_AF_AREA,
	XM5_AF_ZONE    = SDK_AF_ZONE,
	XM5_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XM5_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XM5_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XM5_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XM5_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XM5_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XM5_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XM5_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XM5_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XM5_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XM5_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XM5_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XM5_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XM5_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XM5_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XM5_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XM5_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XM5_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XM5_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XM5_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XM5_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XM5_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XM5_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XM5_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XM5_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XM5_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XM5_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
	XM5_MF_ASSIST_FOCUSMETER               = SDK_MF_ASSIST_FOCUSMETER,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_RED_L    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_L,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_RED_H    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_H,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L,
	XM5_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H,
	XM5_MF_ASSIST_FOCUSMAP_BW              = SDK_MF_ASSIST_FOCUSMAP_BW,
	XM5_MF_ASSIST_FOCUSMAP_COLOR           = SDK_MF_ASSIST_FOCUSMAP_COLOR,
};

// FOCUS AREA
enum{
	XM5_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XM5_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XM5_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XM5_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XM5_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XM5_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XM5_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XM5_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XM5_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XM5_MACRO_MODE           = SDK_MACRO_MODE,
	XM5_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XM5_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XM5_DRIVE_MODE_CL   = 0x1000,
	XM5_DRIVE_MODE_CH   = 0x10F0,
	XM5_DRIVE_MODE_BKT  = 0x4000,
	XM5_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE,
};

// USB MODE
enum{
	XM5_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XM5_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XM5_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XM5_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XM5_BEEP_HIGH        = SDK_BEEP_HIGH,
	XM5_BEEP_MID         = SDK_BEEP_MID,
	XM5_BEEP_LOW         = SDK_BEEP_LOW,
	XM5_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XM5_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XM5_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XM5_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XM5_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XM5_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XM5_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XM5_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XM5_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XM5_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XM5_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XM5_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XM5_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XM5_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XM5_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XM5_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XM5_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XM5_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XM5_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XM5_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XM5_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XM5_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XM5_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XM5_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XM5_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XM5_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XM5_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XM5_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XM5_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XM5_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XM5_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XM5_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XM5_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XM5_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XM5_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XM5_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XM5_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XM5_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XM5_FRAMEGUIDE_GRID_HD      = 0x0003,
	XM5_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XM5_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XM5_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XM5_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XM5_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XM5_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XM5_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XM5_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XM5_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XM5_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XM5_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XM5_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XM5_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XM5_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XM5_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XM5_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XM5_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XM5_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XM5_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XM5_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XM5_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XM5_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XM5_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XM5_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XM5_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XM5_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XM5_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XM5_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XM5_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XM5_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XM5_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XM5_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XM5_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XM5_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XM5_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XM5_IS_MODE_CONTINUOUS_MOTION    = SDK_IS_MODE_CONTINUOUS_MOTION,
	XM5_IS_MODE_SHOOT_MOTION         = SDK_IS_MODE_SHOOT_MOTION,
	XM5_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XM5_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XM5_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XM5_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XM5_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XM5_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XM5_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XM5_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XM5_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XM5_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XM5_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XM5_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XM5_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XM5_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XM5_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XM5_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XM5_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XM5_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XM5_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XM5_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XM5_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XM5_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XM5_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XM5_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XM5_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XM5_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XM5_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XM5_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XM5_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XM5_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XM5_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XM5_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XM5_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XM5_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XM5_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XM5_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XM5_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XM5_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XM5_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XM5_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XM5_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XM5_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XM5_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XM5_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XM5_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XM5_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XM5_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XM5_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XM5_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XM5_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XM5_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XM5_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XM5_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XM5_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XM5_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XM5_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XM5_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XM5_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XM5_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XM5_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XM5_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XM5_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XM5_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XM5_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XM5_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XM5_CUSTOMDISPINFO_LIVEVIEWHIGHT                 = SDK_CUSTOMDISPINFO_LIVEVIEWHIGHT,
	XM5_CUSTOMDISPINFO_EXPOSUREBIASDIGIT             = SDK_CUSTOMDISPINFO_EXPOSUREBIASDIGIT,
	XM5_CUSTOMDISPINFO_TOUCHSCREENMODE               = SDK_CUSTOMDISPINFO_TOUCHSCREENMODE,
	XM5_CUSTOMDISPINFO_BOOSTMODE                     = SDK_CUSTOMDISPINFO_BOOSTMODE,
	XM5_CUSTOMDISPINFO_IMAGETRANSFERORDER            = SDK_CUSTOMDISPINFO_IMAGETRANSFERORDER,
	XM5_CUSTOMDISPINFO_MICLEVEL                      = SDK_CUSTOMDISPINFO_MICLEVEL,
	XM5_CUSTOMDISPINFO_GUIDANCEMESSAGE               = SDK_CUSTOMDISPINFO_GUIDANCEMESSAGE,
	XM5_CUSTOMDISPINFO_FRAMEOUTLINE                  = SDK_CUSTOMDISPINFO_FRAMEOUTLINE,
	XM5_CUSTOMDISPINFO_35MMFORMAT                    = SDK_CUSTOMDISPINFO_35MMFORMAT,
	XM5_CUSTOMDISPINFO_COOLINGFANSETTING             = SDK_CUSTOMDISPINFO_COOLINGFANSETTING,
	XM5_CUSTOMDISPINFO_DIGITALTELECONV               = SDK_CUSTOMDISPINFO_DIGITALTELECONV,
	XM5_CUSTOMDISPINFO_DIGITALZOOM                   = SDK_CUSTOMDISPINFO_DIGITALZOOM,
	XM5_CUSTOMDISPINFO_FOCUSINDICATOR                = SDK_CUSTOMDISPINFO_FOCUSINDICATOR,
	XM5_CUSTOMDISPINFO_NOCARDWARNING                 = SDK_CUSTOMDISPINFO_NOCARDWARNING,
	XM5_CUSTOMDISPINFO_DATETIME                      = SDK_CUSTOMDISPINFO_DATETIME,
	XM5_CUSTOMDISPINFO_LENSSHIFT                     = SDK_CUSTOMDISPINFO_LENSSHIFT,
	XM5_CUSTOMDISPINFO_LENSTILT                      = SDK_CUSTOMDISPINFO_LENSTILT,
	XM5_CUSTOMDISPINFO_LENSREVOLVING                 = SDK_CUSTOMDISPINFO_LENSREVOLVING,
	XM5_CUSTOMDISPINFO_SSD                           = SDK_CUSTOMDISPINFO_SSD,
	XM5_CUSTOMDISPINFO_VLOGMODE                      = SDK_CUSTOMDISPINFO_VLOGMODE,
	XM5_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XM5_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XM5_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XM5_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XM5_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XM5_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XM5_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XM5_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XM5_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XM5_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XM5_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XM5_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XM5_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XM5_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XM5_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XM5_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XM5_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XM5_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XM5_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XM5_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XM5_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XM5_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XM5_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XM5_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XM5_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XM5_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XM5_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XM5_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XM5_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XM5_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XM5_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XM5_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XM5_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XM5_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XM5_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XM5_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XM5_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XM5_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XM5_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XM5_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XM5_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XM5_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XM5_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XM5_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XM5_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XM5_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XM5_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XM5_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XM5_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XM5_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XM5_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XM5_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XM5_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XM5_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XM5_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XM5_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XM5_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XM5_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XM5_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XM5_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XM5_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XM5_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XM5_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XM5_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER  = SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER,
	XM5_FUNCTIONLOCK_CATEGORY2_MODEDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_MODEDIAL,
	XM5_FUNCTIONLOCK_CATEGORY2_FDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FDIAL,
	XM5_FUNCTIONLOCK_CATEGORY2_FN_DIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FN_DIAL,
	XM5_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT		= SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT,
	XM5_FUNCTIONLOCK_CATEGORY3_ISOBUTTON            = SDK_FUNCTIONLOCK_CATEGORY3_ISOBUTTON,
	XM5_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE      = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE,
	XM5_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE         = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE,
	XM5_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU      = SDK_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU,
	XM5_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE         = SDK_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE,
	XM5_FUNCTIONLOCK_CATEGORY3_WBBUTTON             = SDK_FUNCTIONLOCK_CATEGORY3_WBBUTTON,
	XM5_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING     = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING,
	XM5_FUNCTIONLOCK_CATEGORY3_BLUETOOTH            = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTH,
	XM5_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT        = SDK_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT,
	XM5_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING = SDK_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING,
	XM5_FUNCTIONLOCK_CATEGORY3_FM1                  = SDK_FUNCTIONLOCK_CATEGORY3_FM1,
	XM5_FUNCTIONLOCK_CATEGORY3_FM2                  = SDK_FUNCTIONLOCK_CATEGORY3_FM2,
	XM5_FUNCTIONLOCK_CATEGORY3_FM3                  = SDK_FUNCTIONLOCK_CATEGORY3_FM3,
	XM5_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION = SDK_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION,
	XM5_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP      = SDK_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP,
	XM5_FUNCTIONLOCK_CATEGORY3_FN6                  = SDK_FUNCTIONLOCK_CATEGORY3_FN6,
	XM5_FUNCTIONLOCK_CATEGORY3_FSIM_DIAL            = SDK_FUNCTIONLOCK_CATEGORY3_FSIM_DIAL,
	XM5_FUNCTIONLOCK_CATEGORY3_FSIM_DIAL_SETTING    = SDK_FUNCTIONLOCK_CATEGORY3_FSIM_DIAL_SETTING,
	XM5_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XM5_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XM5_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XM5_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XM5_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XM5_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XM5_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XM5_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XM5_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XM5_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XM5_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XM5_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XM5_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XM5_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XM5_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XM5_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XM5_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XM5_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XM5_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XM5_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XM5_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XM5_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XM5_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XM5_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	XM5_MEDIASTATUS_ACCESSING             = SDK_MEDIASTATUS_ACCESSING,
	XM5_MEDIASTATUS_INCOMPATIBLE          = SDK_MEDIASTATUS_INCOMPATIBLE,
};

enum{
	XM5_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XM5_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XM5_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XM5_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XM5_ON  = SDK_ON,
	XM5_OFF = SDK_OFF,
};

enum{
	XM5_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XM5_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XM5_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XM5_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XM5_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XM5_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XM5_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XM5_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XM5_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XM5_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XM5_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XM5_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XM5_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XM5_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XM5_ITEM_RESET_MOVIEMENU         = SDK_ITEM_RESET_MOVIEMENU,
	XM5_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XM5_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XM5_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XM5_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XM5_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XM5_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XM5_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XM5_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XM5_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XM5_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XM5_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XM5_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XM5_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XM5_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XM5_ITEM_MEDIASLOT3              = SDK_ITEM_MEDIASLOT3,
	XM5_ITEM_HDMIOUTPUT              = SDK_ITEM_HDMIOUTPUT,
	XM5_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XM5_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XM5_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XM5_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XM5_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XM5_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XM5_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XM5_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XM5_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XM5_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XM5_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XM5_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XM5_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XM5_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XM5_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XM5_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XM5_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XM5_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XM5_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XM5_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XM5_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XM5_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XM5_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XM5_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XM5_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XM5_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XM5_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XM5_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};

// Micline Setting
enum{
	XM5_MICLINE_SETTING_MIC          = SDK_MICLINE_SETTING_MIC,
	XM5_MICLINE_SETTING_LINE         = SDK_MICLINE_SETTING_LINE,
};

// Shuttercount Type
enum{
	XM5_SHUTTERCOUNT_TYPE_FRONTCURTAIN = SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN,
	XM5_SHUTTERCOUNT_TYPE_REARCURTAIN  = SDK_SHUTTERCOUNT_TYPE_REARCURTAIN,
	XM5_SHUTTERCOUNT_TYPE_TOTAL        = SDK_SHUTTERCOUNT_TYPE_TOTAL,
};

// Performance
enum{
	XM5_PERFORMANCE_NORMAL                    = SDK_PERFORMANCE_NORMAL,
	XM5_PERFORMANCE_ECONOMY                   = SDK_PERFORMANCE_ECONOMY,
	XM5_PERFORMANCE_BOOST_LOWLIGHT            = SDK_PERFORMANCE_BOOST_LOWLIGHT,
	XM5_PERFORMANCE_BOOST_RESOLUTION_PRIORITY = SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY,
	XM5_PERFORMANCE_BOOST_FRAMERATE_PRIORITY  = SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY,
	XM5_PERFORMANCE_BOOST_AFPRIORITY_NORMAL   = SDK_PERFORMANCE_BOOST_AFPRIORITY_NORMAL,
	XM5_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION= SDK_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION,
};

// PixelShift Settings
enum{
	XM5_PIXELSHIFT_INTERVAL_SHORTEST = SDK_PIXELSHIFT_INTERVAL_SHORTEST,
	XM5_PIXELSHIFT_INTERVAL_1S       = SDK_PIXELSHIFT_INTERVAL_1S,
	XM5_PIXELSHIFT_INTERVAL_2S       = SDK_PIXELSHIFT_INTERVAL_2S,
	XM5_PIXELSHIFT_INTERVAL_5S       = SDK_PIXELSHIFT_INTERVAL_5S,
	XM5_PIXELSHIFT_INTERVAL_15S      = SDK_PIXELSHIFT_INTERVAL_15S,
};

// SubjectDetectionMode
enum{
	XM5_SUBJECT_DETECTION_OFF        = SDK_SUBJECT_DETECTION_OFF,
	XM5_SUBJECT_DETECTION_ANIMAL     = SDK_SUBJECT_DETECTION_ANIMAL,
	XM5_SUBJECT_DETECTION_BIRD       = SDK_SUBJECT_DETECTION_BIRD,
	XM5_SUBJECT_DETECTION_CAR        = SDK_SUBJECT_DETECTION_CAR,
	XM5_SUBJECT_DETECTION_BIKE       = SDK_SUBJECT_DETECTION_BIKE,
	XM5_SUBJECT_DETECTION_AIRPLANE   = SDK_SUBJECT_DETECTION_AIRPLANE,
	XM5_SUBJECT_DETECTION_TRAIN      = SDK_SUBJECT_DETECTION_TRAIN,
	XM5_SUBJECT_DETECTION_ALL        = SDK_SUBJECT_DETECTION_ALL,
};

// FanSetting
enum{
	XM5_FAN_SETTING_OFF              = SDK_FAN_SETTING_OFF,
	XM5_FAN_SETTING_WEAK             = SDK_FAN_SETTING_WEAK,
	XM5_FAN_SETTING_STRONG           = SDK_FAN_SETTING_STRONG,
	XM5_FAN_SETTING_AUTO1            = SDK_FAN_SETTING_AUTO1,
	XM5_FAN_SETTING_AUTO2            = SDK_FAN_SETTING_AUTO2,
};

// ElectronicLevelSetting
enum{
	XM5_ELECTRONIC_LEVEL_SETTING_OFF = SDK_ELECTRONIC_LEVEL_SETTING_OFF,
	XM5_ELECTRONIC_LEVEL_SETTING_2D  = SDK_ELECTRONIC_LEVEL_SETTING_2D,
	XM5_ELECTRONIC_LEVEL_SETTING_3D  = SDK_ELECTRONIC_LEVEL_SETTING_3D,
};

// ApertureUnit
enum{
	XM5_APERTURE_UNIT_TNUMBER        = SDK_APERTURE_UNIT_TNUMBER,
	XM5_APERTURE_UNIT_FNUMBER        = SDK_APERTURE_UNIT_FNUMBER,
};

// USBPowerSupplyCommunication
enum{
	XM5_USB_POWER_SUPPLY_COMMUNICATION_AUTO  = SDK_USB_POWER_SUPPLY_COMMUNICATION_AUTO,
	XM5_USB_POWER_SUPPLY_COMMUNICATION_ON    = SDK_USB_POWER_SUPPLY_COMMUNICATION_ON,
	XM5_USB_POWER_SUPPLY_COMMUNICATION_OFF   = SDK_USB_POWER_SUPPLY_COMMUNICATION_OFF,
};

// AutoPowerOffSetting
enum{
	XM5_AUTOPOWEROFF_OFF             = SDK_AUTOPOWEROFF_OFF,
	XM5_AUTOPOWEROFF_15SEC           = SDK_AUTOPOWEROFF_15SEC,
	XM5_AUTOPOWEROFF_30SEC           = SDK_AUTOPOWEROFF_30SEC,
	XM5_AUTOPOWEROFF_1MIN            = SDK_AUTOPOWEROFF_1MIN,
	XM5_AUTOPOWEROFF_2MIN            = SDK_AUTOPOWEROFF_2MIN,
	XM5_AUTOPOWEROFF_5MIN            = SDK_AUTOPOWEROFF_5MIN,
};

// AFZoneCustom
enum{
	XM5_AF_ZONECUSTOM1               = SDK_AF_ZONECUSTOM1,
	XM5_AF_ZONECUSTOM2               = SDK_AF_ZONECUSTOM2,
	XM5_AF_ZONECUSTOM3               = SDK_AF_ZONECUSTOM3,
};

// PortraitEnhancer
enum{
	XM5_PORTRAIT_ENHANCER_OFF        = SDK_PORTRAIT_ENHANCER_OFF,
	XM5_PORTRAIT_ENHANCER_SOFT       = SDK_PORTRAIT_ENHANCER_SOFT,
	XM5_PORTRAIT_ENHANCER_MEDIUM     = SDK_PORTRAIT_ENHANCER_MEDIUM,
	XM5_PORTRAIT_ENHANCER_HARD       = SDK_PORTRAIT_ENHANCER_HARD,
};
