///////////////////////////////////////////////////
//  X-T5.h
//     Version 1.0.0.0
//
//     Copyright (C) 2022 FUJIFILM Corporation.
//

#ifndef _XAPIOPT_H
#include "XAPIOpt.h"
#define _XAPIOPT_H
#endif


//
// API_PARAM for X-T5
//
enum {
	XT5_API_PARAM_CapHighFrequencyFlickerlessMode = 2,
	XT5_API_PARAM_SetHighFrequencyFlickerlessMode = 1,
	XT5_API_PARAM_GetHighFrequencyFlickerlessMode = 1,
	XT5_API_PARAM_CapImageSize                = 2,
	XT5_API_PARAM_SetImageSize                = 1,
	XT5_API_PARAM_GetImageSize                = 1,
	XT5_API_PARAM_CapSharpness                = 2,
	XT5_API_PARAM_SetSharpness                = 1,
	XT5_API_PARAM_GetSharpness                = 1,
	XT5_API_PARAM_CapColorMode                = 2,
	XT5_API_PARAM_SetColorMode                = 1,
	XT5_API_PARAM_GetColorMode                = 1,
	XT5_API_PARAM_CapFilmSimulationMode       = 2,
	XT5_API_PARAM_SetFilmSimulationMode       = 1,
	XT5_API_PARAM_GetFilmSimulationMode       = 1,
	XT5_API_PARAM_CapColorSpace               = 2,
	XT5_API_PARAM_SetColorSpace               = 1,
	XT5_API_PARAM_GetColorSpace               = 1,
	XT5_API_PARAM_CapImageQuality             = 2,
	XT5_API_PARAM_SetImageQuality             = 1,
	XT5_API_PARAM_GetImageQuality             = 1,
	XT5_API_PARAM_CapRAWCompression           = 2,
	XT5_API_PARAM_SetRAWCompression           = 1,
	XT5_API_PARAM_GetRAWCompression           = 1,
	XT5_API_PARAM_CapGrainEffect              = 2,
	XT5_API_PARAM_SetGrainEffect              = 1,
	XT5_API_PARAM_GetGrainEffect              = 1,
	XT5_API_PARAM_CapShadowing                = 2,
	XT5_API_PARAM_SetShadowing                = 1,
	XT5_API_PARAM_GetShadowing                = 1,
	XT5_API_PARAM_CapNoiseReduction           = 2,
	XT5_API_PARAM_SetNoiseReduction           = 1,
	XT5_API_PARAM_GetNoiseReduction           = 1,
	XT5_API_PARAM_CapFaceDetectionMode        = 2,
	XT5_API_PARAM_SetFaceDetectionMode        = 1,
	XT5_API_PARAM_GetFaceDetectionMode        = 1,
	XT5_API_PARAM_CapEyeAFMode                = 2,
	XT5_API_PARAM_SetEyeAFMode                = 1,
	XT5_API_PARAM_GetEyeAFMode                = 1,
	XT5_API_PARAM_SetMacroMode                = -1,
	XT5_API_PARAM_GetMacroMode                = -1,
	XT5_API_PARAM_CapFocusLimiter             = -1,
	XT5_API_PARAM_SetFocusLimiter             = -1,
	XT5_API_PARAM_GetFocusLimiter             = -1,
	XT5_API_PARAM_CapHighLightTone            = 2,
	XT5_API_PARAM_SetHighLightTone            = 1,
	XT5_API_PARAM_GetHighLightTone            = 1,
	XT5_API_PARAM_CapShadowTone               = 2,
	XT5_API_PARAM_SetShadowTone               = 1,
	XT5_API_PARAM_GetShadowTone               = 1,
	XT5_API_PARAM_CapLongExposureNR           = 2,
	XT5_API_PARAM_SetLongExposureNR           = 1,
	XT5_API_PARAM_GetLongExposureNR           = 1,
	XT5_API_PARAM_CapFullTimeManualFocus      = 2,
	XT5_API_PARAM_SetFullTimeManualFocus      = 1,
	XT5_API_PARAM_GetFullTimeManualFocus      = 1,
	XT5_API_PARAM_SetFocusMode                = 1,
	XT5_API_PARAM_GetFocusMode                = 1,
	XT5_API_PARAM_CapAFMode                   = 3,
	XT5_API_PARAM_SetAFMode                   = 2,
	XT5_API_PARAM_GetAFMode                   = 2,
	XT5_API_PARAM_CapFocusPoints              = 2,
	XT5_API_PARAM_SetFocusPoints              = 1,
	XT5_API_PARAM_GetFocusPoints              = 1,
	XT5_API_PARAM_CapFocusArea                = 3,
	XT5_API_PARAM_SetFocusArea                = 2,
	XT5_API_PARAM_GetFocusArea                = 2,
	XT5_API_PARAM_CapFocusMode                = 2,
	XT5_API_PARAM_CapAFStatus                 = 2,
	XT5_API_PARAM_GetAFStatus                 = 1,
	XT5_API_PARAM_CapShutterPriorityMode      = 3,
	XT5_API_PARAM_SetShutterPriorityMode      = 2,
	XT5_API_PARAM_GetShutterPriorityMode      = 2,
	XT5_API_PARAM_CapInstantAFMode            = 2,
	XT5_API_PARAM_SetInstantAFMode            = 1,
	XT5_API_PARAM_GetInstantAFMode            = 1,
	XT5_API_PARAM_CapPreAFMode                = 2,
	XT5_API_PARAM_SetPreAFMode                = 1,
	XT5_API_PARAM_GetPreAFMode                = 1,
	XT5_API_PARAM_CapAFIlluminator            = 2,
	XT5_API_PARAM_SetAFIlluminator            = 1,
	XT5_API_PARAM_GetAFIlluminator            = 1,
	XT5_API_PARAM_CapLensISSwitch             = 2,
	XT5_API_PARAM_SetLensISSwitch             = 1,
	XT5_API_PARAM_GetLensISSwitch             = 1,
	XT5_API_PARAM_CapISMode                   = 2,
	XT5_API_PARAM_SetISMode                   = 1,
	XT5_API_PARAM_GetISMode                   = 1,
	XT5_API_PARAM_CapLMOMode                  = 2,
	XT5_API_PARAM_SetLMOMode                  = 1,
	XT5_API_PARAM_GetLMOMode                  = 1,
	XT5_API_PARAM_SetWhiteBalanceMode         = -1,
	XT5_API_PARAM_GetWhiteBalanceMode         = -1,
	XT5_API_PARAM_CapWhiteBalanceTune         = 5,
	XT5_API_PARAM_SetWhiteBalanceTune         = 3,
	XT5_API_PARAM_GetWhiteBalanceTune         = 3,
	XT5_API_PARAM_CapCaptureDelay             = 2,
	XT5_API_PARAM_SetCaptureDelay             = 1,
	XT5_API_PARAM_GetCaptureDelay             = 1,
	XT5_API_PARAM_CapFocusPos                 = 2,
	XT5_API_PARAM_SetFocusPos                 = 1,
	XT5_API_PARAM_GetFocusPos                 = 1,
	XT5_API_PARAM_CapMFAssistMode             = 2,
	XT5_API_PARAM_SetMFAssistMode             = 1,
	XT5_API_PARAM_GetMFAssistMode             = 1,
	XT5_API_PARAM_CapFocusCheckMode           = 2,
	XT5_API_PARAM_SetFocusCheckMode           = 1,
	XT5_API_PARAM_GetFocusCheckMode           = 1,
	XT5_API_PARAM_CapInterlockAEAFArea        = 2,
	XT5_API_PARAM_SetInterlockAEAFArea        = 1,
	XT5_API_PARAM_GetInterlockAEAFArea        = 1,
	XT5_API_PARAM_StartLiveView               = 0,
	XT5_API_PARAM_StopLiveView                = 0,
	XT5_API_PARAM_CapLiveViewImageQuality     = 2,
	XT5_API_PARAM_SetLiveViewImageQuality     = 1,
	XT5_API_PARAM_GetLiveViewImageQuality     = 1,
	XT5_API_PARAM_CapLiveViewImageSize        = 2,
	XT5_API_PARAM_SetLiveViewImageSize        = 1,
	XT5_API_PARAM_GetLiveViewImageSize        = 1,
	XT5_API_PARAM_CapThroughImageZoom         = 2,
	XT5_API_PARAM_SetThroughImageZoom         = 1,
	XT5_API_PARAM_GetThroughImageZoom         = 1,
	XT5_API_PARAM_SetDateTime                 = 6,
	XT5_API_PARAM_GetDateTime                 = 6,
	XT5_API_PARAM_CapDateTimeDispFormat       = 2,
	XT5_API_PARAM_SetDateTimeDispFormat       = 1,
	XT5_API_PARAM_GetDateTimeDispFormat       = 1,
	XT5_API_PARAM_CapWorldClock               = 2,
	XT5_API_PARAM_SetWorldClock               = 1,
	XT5_API_PARAM_GetWorldClock               = 1,
	XT5_API_PARAM_CapTimeDifference           = 4,
	XT5_API_PARAM_SetTimeDifference           = 2,
	XT5_API_PARAM_GetTimeDifference           = 2,
	XT5_API_PARAM_CapResetSetting             = 2,
	XT5_API_PARAM_ResetSetting                = 1,
	XT5_API_PARAM_SetSilentMode               = -1,
	XT5_API_PARAM_GetSilentMode               = -1,
	XT5_API_PARAM_SetBeep                     = -1,
	XT5_API_PARAM_GetBeep                     = -1,
	XT5_API_PARAM_SetFunctionLock             = 1,
	XT5_API_PARAM_GetFunctionLock             = 1,
	XT5_API_PARAM_CapFunctionLock             = 2,
	XT5_API_PARAM_SetFunctionLockCategory     = 3,
	XT5_API_PARAM_GetFunctionLockCategory     = 3,
	XT5_API_PARAM_CapFunctionLockCategory     = 6,
	XT5_API_PARAM_SetComment                  = 1,
	XT5_API_PARAM_GetComment                  = 1,
	XT5_API_PARAM_SetCopyright                = 2,
	XT5_API_PARAM_GetCopyright                = 2,
	XT5_API_PARAM_SetArtist                   = 1,
	XT5_API_PARAM_GetArtist                   = 1,
	XT5_API_PARAM_SetFilenamePrefix           = 2,
	XT5_API_PARAM_GetFilenamePrefix           = 2,
	XT5_API_PARAM_SetFoldernameSuffix         = 3,
	XT5_API_PARAM_GetFoldernameSuffix         = 3,
	XT5_API_PARAM_GetFoldernameList           = 4,
	XT5_API_PARAM_CheckBatteryInfo            = 8,
	XT5_API_PARAM_CapFrameNumberSequence      = 2,
	XT5_API_PARAM_SetFrameNumberSequence      = 1,
	XT5_API_PARAM_GetFrameNumberSequence      = 1,
	XT5_API_PARAM_SetUSBMode                  = -1,
	XT5_API_PARAM_GetUSBMode                  = -1,
	XT5_API_PARAM_CapFormatMemoryCard         = 2,
	XT5_API_PARAM_FormatMemoryCard            = 1,
	XT5_API_PARAM_SetMediaRecord              = 1,
	XT5_API_PARAM_GetMediaRecord              = 1,
	XT5_API_PARAM_GetMediaCapacity            = 5,
	XT5_API_PARAM_GetMediaStatus              = 2,
	XT5_API_PARAM_GetShutterCount             = 3,
	XT5_API_PARAM_GetShutterCountEx           = 3,
	XT5_API_PARAM_SetSensorCleanTiming        = -1,
	XT5_API_PARAM_GetSensorCleanTiming        = -1,
	XT5_API_PARAM_SetPreviewTime              = -1,
	XT5_API_PARAM_GetPreviewTime              = -1,
	XT5_API_PARAM_SetEVFDispAutoRotate        = -1,
	XT5_API_PARAM_GetEVFDispAutoRotate        = -1,
	XT5_API_PARAM_SetDispMMode                = 1,
	XT5_API_PARAM_GetDispMMode                = 1,
	XT5_API_PARAM_CapExposurePreview          = 2,
	XT5_API_PARAM_SetExposurePreview          = 1,
	XT5_API_PARAM_GetExposurePreview          = 1,
	XT5_API_PARAM_SetDispBrightness           = -1,
	XT5_API_PARAM_GetDispBrightness           = -1,
	XT5_API_PARAM_CapFrameGuideMode           = 2,
	XT5_API_PARAM_SetFrameGuideMode           = 1,
	XT5_API_PARAM_GetFrameGuideMode           = 1,
	XT5_API_PARAM_SetFrameGuideGridInfo       = 2,
	XT5_API_PARAM_GetFrameGuideGridInfo       = 2,
	XT5_API_PARAM_SetAutoImageRotation        = -1,
	XT5_API_PARAM_GetAutoImageRotation        = -1,
	XT5_API_PARAM_CapFocusScaleUnit           = 2,
	XT5_API_PARAM_SetFocusScaleUnit           = 1,
	XT5_API_PARAM_GetFocusScaleUnit           = 1,
	XT5_API_PARAM_CapCustomDispInfo           = 4,
	XT5_API_PARAM_SetCustomDispInfo           = 2,
	XT5_API_PARAM_GetCustomDispInfo           = 2,
	XT5_API_PARAM_CapViewMode                 = 3,
	XT5_API_PARAM_SetViewMode                 = 2,
	XT5_API_PARAM_GetViewMode                 = 2,
	XT5_API_PARAM_SetDispInfoMode             = -1,
	XT5_API_PARAM_GetDispInfoMode             = -1,
	XT5_API_PARAM_SetDispChroma               = -1,
	XT5_API_PARAM_GetDispChroma               = -1,
	XT5_API_PARAM_CapCustomAutoPowerOff       = 2,
	XT5_API_PARAM_SetCustomAutoPowerOff       = 1,
	XT5_API_PARAM_GetCustomAutoPowerOff       = 1,
	XT5_API_PARAM_CapCustomStudioPowerSave    = -1,
	XT5_API_PARAM_SetCustomStudioPowerSave    = -1,
	XT5_API_PARAM_GetCustomStudioPowerSave    = -1,
	XT5_API_PARAM_SetFunctionButton           = -1,
	XT5_API_PARAM_GetFunctionButton           = -1,
	XT5_API_PARAM_SetISODialHn                = -1,
	XT5_API_PARAM_GetISODialHn                = -1,
	XT5_API_PARAM_CapLockButtonMode           = 2,
	XT5_API_PARAM_SetLockButtonMode           = 1,
	XT5_API_PARAM_GetLockButtonMode           = 1,
	XT5_API_PARAM_SetAFLockMode               = -1,
	XT5_API_PARAM_GetAFLockMode               = -1,
	XT5_API_PARAM_SetMicJackMode              = -1,
	XT5_API_PARAM_GetMicJackMode              = -1,
	XT5_API_PARAM_SetAeAfLockKeyAssign        = -1,
	XT5_API_PARAM_GetAeAfLockKeyAssign        = -1,
	XT5_API_PARAM_SetCrossKeyAssign           = -1,
	XT5_API_PARAM_GetCrossKeyAssign           = -1,
	XT5_API_PARAM_CapWideDynamicRange         = 2,
	XT5_API_PARAM_SetWideDynamicRange         = 1,
	XT5_API_PARAM_GetWideDynamicRange         = 1,
	XT5_API_PARAM_CapClarityMode              = 2,
	XT5_API_PARAM_SetClarityMode              = 1,
	XT5_API_PARAM_GetClarityMode              = 1,
	XT5_API_PARAM_SetBlackImageTone           = -1,
	XT5_API_PARAM_GetBlackImageTone           = -1,
	XT5_API_PARAM_GetTNumber                  = 1,
	XT5_API_PARAM_CapCropMode                 = 2,
	XT5_API_PARAM_SetCropMode                 = 1,
	XT5_API_PARAM_GetCropMode                 = 2,
	XT5_API_PARAM_GetCropAreaFrameInfo        = 1,
	XT5_API_PARAM_CapRAWOutputDepth           = -1,
	XT5_API_PARAM_SetRAWOutputDepth           = -1,
	XT5_API_PARAM_GetRAWOutputDepth           = -1,
	XT5_API_PARAM_CapSmoothSkinEffect         = 2,
	XT5_API_PARAM_SetSmoothSkinEffect         = 1,
	XT5_API_PARAM_GetSmoothSkinEffect         = 1,
	XT5_API_PARAM_GetDetectedFaceFrame        = 2,
	XT5_API_PARAM_SetDetectedFaceFrame        = 1,
    XT5_API_PARAM_SetCustomWBArea             = 2,
    XT5_API_PARAM_GetCustomWBArea             = 2,
	XT5_API_PARAM_CapColorChromeBlue          = 2,
	XT5_API_PARAM_SetColorChromeBlue          = 1,
	XT5_API_PARAM_GetColorChromeBlue          = 1,
	XT5_API_PARAM_CapMonochromaticColor       = 4,
	XT5_API_PARAM_SetMonochromaticColor       = 2,
	XT5_API_PARAM_GetMonochromaticColor       = 2,
	XT5_API_PARAM_CapFocusLimiterPos          = 4,
	XT5_API_PARAM_SetFocusLimiterPos          = 2,
	XT5_API_PARAM_GetFocusLimiterIndicator    = 1,
	XT5_API_PARAM_GetFocusLimiterRange        = 2,
	XT5_API_PARAM_CapFocusLimiterMode         = 2,
	XT5_API_PARAM_SetFocusLimiterMode         = 1,
	XT5_API_PARAM_GetFocusLimiterMode         = 1,
	XT5_API_PARAM_GetCommandDialStatus        = 4,
	XT5_API_PARAM_CapPerformanceSettings      = 2,
	XT5_API_PARAM_SetPerformanceSettings      = 1,
	XT5_API_PARAM_GetPerformanceSettings      = 1,
	XT5_API_PARAM_CapMicLineSetting           = 2,
	XT5_API_PARAM_SetMicLineSetting           = 1,
	XT5_API_PARAM_GetMicLineSetting           = 1,
	XT5_API_PARAM_CapCustomSettingAutoUpdate  = 2,
	XT5_API_PARAM_SetCustomSettingAutoUpdate  = 1,
	XT5_API_PARAM_GetCustomSettingAutoUpdate  = 1,
    XT5_API_PARAM_CapPixelShiftSettings       = 2,
    XT5_API_PARAM_SetPixelShiftSettings       = 1,
    XT5_API_PARAM_GetPixelShiftSettings       = 1,
    XT5_API_PARAM_CapImageFormat              = 2,
    XT5_API_PARAM_SetImageFormat              = 1,
    XT5_API_PARAM_GetImageFormat              = 1,
	XT5_API_PARAM_CapSubjectDetectionMode     = 2,
	XT5_API_PARAM_SetSubjectDetectionMode     = 1,
	XT5_API_PARAM_GetSubjectDetectionMode     = 1,
	XT5_API_PARAM_CapCropZoom                 = 2,
	XT5_API_PARAM_SetCropZoom                 = 1,
	XT5_API_PARAM_GetCropZoom                 = 1,
	XT5_API_PARAM_CapZoomOperation            = 4,
	XT5_API_PARAM_SetZoomOperation            = 3,
	XT5_API_PARAM_CapFocusOperation           = 4,
	XT5_API_PARAM_SetFocusOperation           = 3,
	XT5_API_PARAM_CapZoomSpeed                = 2,
	XT5_API_PARAM_SetZoomSpeed                = 1,
	XT5_API_PARAM_GetZoomSpeed                = 1,
	XT5_API_PARAM_CapFocusSpeed               = 2,
	XT5_API_PARAM_SetFocusSpeed               = 1,
	XT5_API_PARAM_GetFocusSpeed               = 1,
	XT5_API_PARAM_CapLiveViewStatus           = 2,
	XT5_API_PARAM_GetLiveViewStatus           = 1,
	XT5_API_PARAM_CapLiveViewMode             = 2,
	XT5_API_PARAM_SetLiveViewMode             = 1,
	XT5_API_PARAM_GetLiveViewMode             = 1,
	XT5_API_PARAM_CapLiveViewImageRatio       = 2,
	XT5_API_PARAM_SetLiveViewImageRatio       = 1,
	XT5_API_PARAM_GetLiveViewImageRatio       = 1,
	XT5_API_PARAM_CapSummerTime               = 2,
	XT5_API_PARAM_SetSummerTime               = 1,
	XT5_API_PARAM_GetSummerTime               = 1,
	XT5_API_PARAM_CapFanSetting               = 2,
	XT5_API_PARAM_SetFanSetting               = 1,
	XT5_API_PARAM_GetFanSetting               = 1,
	XT5_API_PARAM_CapElectronicLevelSetting   = 2,
	XT5_API_PARAM_SetElectronicLevelSetting   = 1,
	XT5_API_PARAM_GetElectronicLevelSetting   = 1,
	XT5_API_PARAM_CapApertureUnit             = 2,
	XT5_API_PARAM_SetApertureUnit             = 1,
	XT5_API_PARAM_GetApertureUnit             = 1,
	XT5_API_PARAM_CapUSBPowerSupplyCommunication = 2,
	XT5_API_PARAM_SetUSBPowerSupplyCommunication = 1,
	XT5_API_PARAM_GetUSBPowerSupplyCommunication = 1,
	XT5_API_PARAM_CapAutoPowerOffSetting      = 2,
	XT5_API_PARAM_SetAutoPowerOffSetting      = 1,
	XT5_API_PARAM_GetAutoPowerOffSetting      = 1,
};

//
// API_CODE for X-T5
//
enum {
	XT5_API_CODE_CapHighFrequencyFlickerlessMode = API_CODE_CapHighFrequencyFlickerlessMode,
	XT5_API_CODE_SetHighFrequencyFlickerlessMode = API_CODE_SetHighFrequencyFlickerlessMode,
	XT5_API_CODE_GetHighFrequencyFlickerlessMode = API_CODE_GetHighFrequencyFlickerlessMode,
	XT5_API_CODE_CapImageSize                = API_CODE_CapImageSize,
	XT5_API_CODE_SetImageSize                = API_CODE_SetImageSize,
	XT5_API_CODE_GetImageSize                = API_CODE_GetImageSize,
	XT5_API_CODE_CapSharpness                = API_CODE_CapSharpness,
	XT5_API_CODE_SetSharpness                = API_CODE_SetSharpness,
	XT5_API_CODE_GetSharpness                = API_CODE_GetSharpness,
	XT5_API_CODE_CapColorMode                = API_CODE_CapColorMode,
	XT5_API_CODE_SetColorMode                = API_CODE_SetColorMode,
	XT5_API_CODE_GetColorMode                = API_CODE_GetColorMode,
	XT5_API_CODE_CapFilmSimulationMode       = API_CODE_CapFilmSimulationMode,
	XT5_API_CODE_SetFilmSimulationMode       = API_CODE_SetFilmSimulationMode,
	XT5_API_CODE_GetFilmSimulationMode       = API_CODE_GetFilmSimulationMode,
	XT5_API_CODE_CapColorSpace               = API_CODE_CapColorSpace,
	XT5_API_CODE_SetColorSpace               = API_CODE_SetColorSpace,
	XT5_API_CODE_GetColorSpace               = API_CODE_GetColorSpace,
	XT5_API_CODE_CapImageQuality             = API_CODE_CapImageQuality,
	XT5_API_CODE_SetImageQuality             = API_CODE_SetImageQuality,
	XT5_API_CODE_GetImageQuality             = API_CODE_GetImageQuality,
	XT5_API_CODE_CapRAWCompression           = API_CODE_CapRAWCompression,
	XT5_API_CODE_SetRAWCompression           = API_CODE_SetRAWCompression,
	XT5_API_CODE_GetRAWCompression           = API_CODE_GetRAWCompression,
	XT5_API_CODE_CapGrainEffect              = API_CODE_CapGrainEffect,
	XT5_API_CODE_SetGrainEffect              = API_CODE_SetGrainEffect,
	XT5_API_CODE_GetGrainEffect              = API_CODE_GetGrainEffect,
	XT5_API_CODE_CapShadowing                = API_CODE_CapShadowing,
	XT5_API_CODE_SetShadowing                = API_CODE_SetShadowing,
	XT5_API_CODE_GetShadowing                = API_CODE_GetShadowing,
	XT5_API_CODE_CapNoiseReduction           = API_CODE_CapNoiseReduction,
	XT5_API_CODE_SetNoiseReduction           = API_CODE_SetNoiseReduction,
	XT5_API_CODE_GetNoiseReduction           = API_CODE_GetNoiseReduction,
	XT5_API_CODE_CapFaceDetectionMode        = API_CODE_CapFaceDetectionMode,
	XT5_API_CODE_SetFaceDetectionMode        = API_CODE_SetFaceDetectionMode,
	XT5_API_CODE_GetFaceDetectionMode        = API_CODE_GetFaceDetectionMode,
	XT5_API_CODE_CapEyeAFMode                = API_CODE_CapEyeAFMode,
	XT5_API_CODE_SetEyeAFMode                = API_CODE_SetEyeAFMode,
	XT5_API_CODE_GetEyeAFMode                = API_CODE_GetEyeAFMode,
	XT5_API_CODE_SetMacroMode                = API_CODE_SetMacroMode,
	XT5_API_CODE_GetMacroMode                = API_CODE_GetMacroMode,
	XT5_API_CODE_CapHighLightTone            = API_CODE_CapHighLightTone,
	XT5_API_CODE_SetHighLightTone            = API_CODE_SetHighLightTone,
	XT5_API_CODE_GetHighLightTone            = API_CODE_GetHighLightTone,
	XT5_API_CODE_CapShadowTone               = API_CODE_CapShadowTone,
	XT5_API_CODE_SetShadowTone               = API_CODE_SetShadowTone,
	XT5_API_CODE_GetShadowTone               = API_CODE_GetShadowTone,
	XT5_API_CODE_CapLongExposureNR           = API_CODE_CapLongExposureNR,
	XT5_API_CODE_SetLongExposureNR           = API_CODE_SetLongExposureNR,
	XT5_API_CODE_GetLongExposureNR           = API_CODE_GetLongExposureNR,
	XT5_API_CODE_CapFullTimeManualFocus      = API_CODE_CapFullTimeManualFocus,
	XT5_API_CODE_SetFullTimeManualFocus      = API_CODE_SetFullTimeManualFocus,
	XT5_API_CODE_GetFullTimeManualFocus      = API_CODE_GetFullTimeManualFocus,
	XT5_API_CODE_SetFocusMode                = API_CODE_SetFocusMode,
	XT5_API_CODE_GetFocusMode                = API_CODE_GetFocusMode,
	XT5_API_CODE_CapAFMode                   = API_CODE_CapAFMode,
	XT5_API_CODE_SetAFMode                   = API_CODE_SetAFMode,
	XT5_API_CODE_GetAFMode                   = API_CODE_GetAFMode,
	XT5_API_CODE_CapFocusPoints              = API_CODE_CapFocusPoints,
	XT5_API_CODE_SetFocusPoints              = API_CODE_SetFocusPoints,
	XT5_API_CODE_GetFocusPoints              = API_CODE_GetFocusPoints,
	XT5_API_CODE_CapFocusArea                = API_CODE_CapFocusArea,
	XT5_API_CODE_SetFocusArea                = API_CODE_SetFocusArea,
	XT5_API_CODE_GetFocusArea                = API_CODE_GetFocusArea,
	XT5_API_CODE_CapFocusMode                = API_CODE_CapFocusMode,
	XT5_API_CODE_CapAFStatus                 = API_CODE_CapAFStatus,
	XT5_API_CODE_GetAFStatus                 = API_CODE_GetAFStatus,
	XT5_API_CODE_CapShutterPriorityMode      = API_CODE_CapShutterPriorityMode,
	XT5_API_CODE_SetShutterPriorityMode      = API_CODE_SetShutterPriorityMode,
	XT5_API_CODE_GetShutterPriorityMode      = API_CODE_GetShutterPriorityMode,
	XT5_API_CODE_CapInstantAFMode            = API_CODE_CapInstantAFMode,
	XT5_API_CODE_SetInstantAFMode            = API_CODE_SetInstantAFMode,
	XT5_API_CODE_GetInstantAFMode            = API_CODE_GetInstantAFMode,
	XT5_API_CODE_CapPreAFMode                = API_CODE_CapPreAFMode,
	XT5_API_CODE_SetPreAFMode                = API_CODE_SetPreAFMode,
	XT5_API_CODE_GetPreAFMode                = API_CODE_GetPreAFMode,
	XT5_API_CODE_CapAFIlluminator            = API_CODE_CapAFIlluminator,
	XT5_API_CODE_SetAFIlluminator            = API_CODE_SetAFIlluminator,
	XT5_API_CODE_GetAFIlluminator            = API_CODE_GetAFIlluminator,
	XT5_API_CODE_CapLensISSwitch             = API_CODE_CapLensISSwitch,
	XT5_API_CODE_SetLensISSwitch             = API_CODE_SetLensISSwitch,
	XT5_API_CODE_GetLensISSwitch             = API_CODE_GetLensISSwitch,
	XT5_API_CODE_CapISMode                   = API_CODE_CapISMode,
	XT5_API_CODE_SetISMode                   = API_CODE_SetISMode,
	XT5_API_CODE_GetISMode                   = API_CODE_GetISMode,
	XT5_API_CODE_CapLMOMode                  = API_CODE_CapLMOMode,
	XT5_API_CODE_SetLMOMode                  = API_CODE_SetLMOMode,
	XT5_API_CODE_GetLMOMode                  = API_CODE_GetLMOMode,
	XT5_API_CODE_SetWhiteBalanceMode         = API_CODE_SetWhiteBalanceMode,
	XT5_API_CODE_GetWhiteBalanceMode         = API_CODE_GetWhiteBalanceMode,
	XT5_API_CODE_CapWhiteBalanceTune         = API_CODE_CapWhiteBalanceTune,
	XT5_API_CODE_SetWhiteBalanceTune         = API_CODE_SetWhiteBalanceTune,
	XT5_API_CODE_GetWhiteBalanceTune         = API_CODE_GetWhiteBalanceTune,
	XT5_API_CODE_CapCaptureDelay             = API_CODE_CapCaptureDelay,
	XT5_API_CODE_SetCaptureDelay             = API_CODE_SetCaptureDelay,
	XT5_API_CODE_GetCaptureDelay             = API_CODE_GetCaptureDelay,
	XT5_API_CODE_CapFocusPos                 = API_CODE_CapFocusPos,
	XT5_API_CODE_SetFocusPos                 = API_CODE_SetFocusPos,
	XT5_API_CODE_GetFocusPos                 = API_CODE_GetFocusPos,
	XT5_API_CODE_CapMFAssistMode             = API_CODE_CapMFAssistMode,
	XT5_API_CODE_SetMFAssistMode             = API_CODE_SetMFAssistMode,
	XT5_API_CODE_GetMFAssistMode             = API_CODE_GetMFAssistMode,
	XT5_API_CODE_CapFocusCheckMode           = API_CODE_CapFocusCheckMode,
	XT5_API_CODE_SetFocusCheckMode           = API_CODE_SetFocusCheckMode,
	XT5_API_CODE_GetFocusCheckMode           = API_CODE_GetFocusCheckMode,
	XT5_API_CODE_CapInterlockAEAFArea        = API_CODE_CapInterlockAEAFArea,
	XT5_API_CODE_SetInterlockAEAFArea        = API_CODE_SetInterlockAEAFArea,
	XT5_API_CODE_GetInterlockAEAFArea        = API_CODE_GetInterlockAEAFArea,
	XT5_API_CODE_StartLiveView               = API_CODE_StartLiveView,
	XT5_API_CODE_StopLiveView                = API_CODE_StopLiveView,
	XT5_API_CODE_CapLiveViewImageQuality     = API_CODE_CapLiveViewImageQuality,
	XT5_API_CODE_SetLiveViewImageQuality     = API_CODE_SetLiveViewImageQuality,
	XT5_API_CODE_GetLiveViewImageQuality     = API_CODE_GetLiveViewImageQuality,
	XT5_API_CODE_CapLiveViewImageSize        = API_CODE_CapLiveViewImageSize,
	XT5_API_CODE_SetLiveViewImageSize        = API_CODE_SetLiveViewImageSize,
	XT5_API_CODE_GetLiveViewImageSize        = API_CODE_GetLiveViewImageSize,
	XT5_API_CODE_CapThroughImageZoom         = API_CODE_CapThroughImageZoom,
	XT5_API_CODE_SetThroughImageZoom         = API_CODE_SetThroughImageZoom,
	XT5_API_CODE_GetThroughImageZoom         = API_CODE_GetThroughImageZoom,
	XT5_API_CODE_SetDateTime                 = API_CODE_SetDateTime,
	XT5_API_CODE_GetDateTime                 = API_CODE_GetDateTime,
	XT5_API_CODE_CapDateTimeDispFormat       = API_CODE_CapDateTimeDispFormat,
	XT5_API_CODE_SetDateTimeDispFormat       = API_CODE_SetDateTimeDispFormat,
	XT5_API_CODE_GetDateTimeDispFormat       = API_CODE_GetDateTimeDispFormat,
	XT5_API_CODE_CapWorldClock               = API_CODE_CapWorldClock,
	XT5_API_CODE_SetWorldClock               = API_CODE_SetWorldClock,
	XT5_API_CODE_GetWorldClock               = API_CODE_GetWorldClock,
	XT5_API_CODE_CapTimeDifference           = API_CODE_CapTimeDifference,
	XT5_API_CODE_SetTimeDifference           = API_CODE_SetTimeDifference,
	XT5_API_CODE_GetTimeDifference           = API_CODE_GetTimeDifference,
	XT5_API_CODE_CapResetSetting             = API_CODE_CapResetSetting,
	XT5_API_CODE_ResetSetting                = API_CODE_ResetSetting,
	XT5_API_CODE_SetSilentMode               = API_CODE_SetSilentMode,
	XT5_API_CODE_GetSilentMode               = API_CODE_GetSilentMode,
	XT5_API_CODE_SetBeep                     = API_CODE_SetBeep,
	XT5_API_CODE_GetBeep                     = API_CODE_GetBeep,
	XT5_API_CODE_CapFunctionLock             = API_CODE_CapFunctionLock,
	XT5_API_CODE_SetFunctionLock             = API_CODE_SetFunctionLock,
	XT5_API_CODE_GetFunctionLock             = API_CODE_GetFunctionLock,
	XT5_API_CODE_CapFunctionLockCategory     = API_CODE_CapFunctionLockCategory,
	XT5_API_CODE_SetFunctionLockCategory     = API_CODE_SetFunctionLockCategory,
	XT5_API_CODE_GetFunctionLockCategory     = API_CODE_GetFunctionLockCategory,
	XT5_API_CODE_SetComment                  = API_CODE_SetComment,
	XT5_API_CODE_GetComment                  = API_CODE_GetComment,
	XT5_API_CODE_SetCopyright                = API_CODE_SetCopyright,
	XT5_API_CODE_GetCopyright                = API_CODE_GetCopyright,
	XT5_API_CODE_SetArtist                   = API_CODE_SetArtist,
	XT5_API_CODE_GetArtist                   = API_CODE_GetArtist,
	XT5_API_CODE_SetFilenamePrefix           = API_CODE_SetFilenamePrefix,
	XT5_API_CODE_GetFilenamePrefix           = API_CODE_GetFilenamePrefix,
	XT5_API_CODE_SetFoldernameSuffix         = API_CODE_SetFoldernameSuffix,
	XT5_API_CODE_GetFoldernameSuffix         = API_CODE_GetFoldernameSuffix,
	XT5_API_CODE_GetFoldernameList           = API_CODE_GetFoldernameList,
	XT5_API_CODE_CheckBatteryInfo            = API_CODE_CheckBatteryInfo,
	XT5_API_CODE_CapFrameNumberSequence      = API_CODE_CapFrameNumberSequence,
	XT5_API_CODE_SetFrameNumberSequence      = API_CODE_SetFrameNumberSequence,
	XT5_API_CODE_GetFrameNumberSequence      = API_CODE_GetFrameNumberSequence,
	XT5_API_CODE_SetUSBMode                  = API_CODE_SetUSBMode,
	XT5_API_CODE_GetUSBMode                  = API_CODE_GetUSBMode,
	XT5_API_CODE_CapFormatMemoryCard         = API_CODE_CapFormatMemoryCard,
	XT5_API_CODE_FormatMemoryCard            = API_CODE_FormatMemoryCard,
	XT5_API_CODE_SetMediaRecord              = API_CODE_SDK_SetMediaRecord,
	XT5_API_CODE_GetMediaRecord              = API_CODE_SDK_GetMediaRecord,
	XT5_API_CODE_GetMediaCapacity            = API_CODE_GetMediaCapacity,
	XT5_API_CODE_GetMediaStatus              = API_CODE_GetMediaStatus,
	XT5_API_CODE_GetShutterCount             = API_CODE_GetShutterCount,
	XT5_API_CODE_GetShutterCountEx           = API_CODE_GetShutterCountEx,
	XT5_API_CODE_SetSensorCleanTiming        = API_CODE_SetSensorCleanTiming,
	XT5_API_CODE_GetSensorCleanTiming        = API_CODE_GetSensorCleanTiming,
	XT5_API_CODE_SetPreviewTime              = API_CODE_SetPreviewTime,
	XT5_API_CODE_GetPreviewTime              = API_CODE_GetPreviewTime,
	XT5_API_CODE_SetEVFDispAutoRotate        = API_CODE_SetEVFDispAutoRotate,
	XT5_API_CODE_GetEVFDispAutoRotate        = API_CODE_GetEVFDispAutoRotate,
	XT5_API_CODE_SetDispMMode                = API_CODE_SetExposurePreview,
	XT5_API_CODE_GetDispMMode                = API_CODE_GetExposurePreview,
	XT5_API_CODE_CapExposurePreview          = API_CODE_CapExposurePreview,
	XT5_API_CODE_SetExposurePreview          = API_CODE_SetExposurePreview,
	XT5_API_CODE_GetExposurePreview          = API_CODE_GetExposurePreview,
	XT5_API_CODE_SetDispBrightness           = API_CODE_SetDispBrightness,
	XT5_API_CODE_GetDispBrightness           = API_CODE_GetDispBrightness,
	XT5_API_CODE_CapFrameGuideMode           = API_CODE_CapFrameGuideMode,
	XT5_API_CODE_SetFrameGuideMode           = API_CODE_SetFrameGuideMode,
	XT5_API_CODE_GetFrameGuideMode           = API_CODE_GetFrameGuideMode,
	XT5_API_CODE_SetFrameGuideGridInfo       = API_CODE_SetFrameGuideGridInfo,
	XT5_API_CODE_GetFrameGuideGridInfo       = API_CODE_GetFrameGuideGridInfo,
	XT5_API_CODE_SetAutoImageRotation        = API_CODE_SetAutoImageRotation,
	XT5_API_CODE_GetAutoImageRotation        = API_CODE_GetAutoImageRotation,
	XT5_API_CODE_CapFocusScaleUnit           = API_CODE_CapFocusScaleUnit,
	XT5_API_CODE_SetFocusScaleUnit           = API_CODE_SetFocusScaleUnit,
	XT5_API_CODE_GetFocusScaleUnit           = API_CODE_GetFocusScaleUnit,
	XT5_API_CODE_CapCustomDispInfo           = API_CODE_CapCustomDispInfo,
	XT5_API_CODE_SetCustomDispInfo           = API_CODE_SetCustomDispInfo,
	XT5_API_CODE_GetCustomDispInfo           = API_CODE_GetCustomDispInfo,
	XT5_API_CODE_CapViewMode                 = API_CODE_CapViewMode,
	XT5_API_CODE_SetViewMode                 = API_CODE_SetViewMode,
	XT5_API_CODE_GetViewMode                 = API_CODE_GetViewMode,
	XT5_API_CODE_SetDispInfoMode             = API_CODE_SetDispInfoMode,
	XT5_API_CODE_GetDispInfoMode             = API_CODE_GetDispInfoMode,
	XT5_API_CODE_SetDispChroma               = API_CODE_SetDispChroma,
	XT5_API_CODE_GetDispChroma               = API_CODE_GetDispChroma,
	XT5_API_CODE_CapCustomAutoPowerOff       = API_CODE_CapCustomAutoPowerOff,
	XT5_API_CODE_SetCustomAutoPowerOff       = API_CODE_SetCustomAutoPowerOff,
	XT5_API_CODE_GetCustomAutoPowerOff       = API_CODE_GetCustomAutoPowerOff,
	XT5_API_CODE_CapCustomStudioPowerSave    = API_CODE_CapCustomStudioPowerSave,
	XT5_API_CODE_SetCustomStudioPowerSave    = API_CODE_SetCustomStudioPowerSave,
	XT5_API_CODE_GetCustomStudioPowerSave    = API_CODE_GetCustomStudioPowerSave,
	XT5_API_CODE_SetFunctionButton           = API_CODE_SetFunctionButton,
	XT5_API_CODE_GetFunctionButton           = API_CODE_GetFunctionButton,
	XT5_API_CODE_SetISODialHn                = API_CODE_SetISODialHn,
	XT5_API_CODE_GetISODialHn                = API_CODE_GetISODialHn,
	XT5_API_CODE_CapLockButtonMode           = API_CODE_CapLockButtonMode,
	XT5_API_CODE_SetLockButtonMode           = API_CODE_SetLockButtonMode,
	XT5_API_CODE_GetLockButtonMode           = API_CODE_GetLockButtonMode,
	XT5_API_CODE_SetAFLockMode               = API_CODE_SetAFLockMode,
	XT5_API_CODE_GetAFLockMode               = API_CODE_GetAFLockMode,
	XT5_API_CODE_SetMicJackMode              = API_CODE_SetMicJackMode,
	XT5_API_CODE_GetMicJackMode              = API_CODE_GetMicJackMode,
	XT5_API_CODE_SetAeAfLockKeyAssign        = API_CODE_SetAeAfLockKeyAssign,
	XT5_API_CODE_GetAeAfLockKeyAssign        = API_CODE_GetAeAfLockKeyAssign,
	XT5_API_CODE_SetCrossKeyAssign           = API_CODE_SetCrossKeyAssign,
	XT5_API_CODE_GetCrossKeyAssign           = API_CODE_GetCrossKeyAssign,
	XT5_API_CODE_CapWideDynamicRange         = API_CODE_CapWideDynamicRange,
	XT5_API_CODE_SetWideDynamicRange         = API_CODE_SetWideDynamicRange,
	XT5_API_CODE_GetWideDynamicRange         = API_CODE_GetWideDynamicRange,
	XT5_API_CODE_CapClarityMode              = API_CODE_CapClarityMode,
	XT5_API_CODE_SetClarityMode              = API_CODE_SetClarityMode,
	XT5_API_CODE_GetClarityMode              = API_CODE_GetClarityMode,
	XT5_API_CODE_SetBlackImageTone           = API_CODE_SetBlackImageTone,
	XT5_API_CODE_GetBlackImageTone           = API_CODE_GetBlackImageTone,
	XT5_API_CODE_GetTNumber                  = API_CODE_GetTNumber,
	XT5_API_CODE_CapCropMode                 = API_CODE_CapCropMode,
	XT5_API_CODE_SetCropMode                 = API_CODE_SetCropMode,
	XT5_API_CODE_GetCropMode                 = API_CODE_GetCropMode,
	XT5_API_CODE_GetCropAreaFrameInfo        = API_CODE_GetCropAreaFrameInfo,
	XT5_API_CODE_CapRAWOutputDepth           = API_CODE_CapRAWOutputDepth,
	XT5_API_CODE_SetRAWOutputDepth           = API_CODE_SetRAWOutputDepth,
	XT5_API_CODE_GetRAWOutputDepth           = API_CODE_GetRAWOutputDepth,
	XT5_API_CODE_CapSmoothSkinEffect         = API_CODE_CapSmoothSkinEffect,
	XT5_API_CODE_SetSmoothSkinEffect         = API_CODE_SetSmoothSkinEffect,
	XT5_API_CODE_GetSmoothSkinEffect         = API_CODE_GetSmoothSkinEffect,
	XT5_API_CODE_GetDetectedFaceFrame        = API_CODE_GetDetectedFaceFrame,
	XT5_API_CODE_SetDetectedFaceFrame        = API_CODE_SetDetectedFaceFrame,
	XT5_API_CODE_SetCustomWBArea             = API_CODE_SetCustomWBArea,
	XT5_API_CODE_GetCustomWBArea             = API_CODE_GetCustomWBArea,
	XT5_API_CODE_CapColorChromeBlue          = API_CODE_CapColorChromeBlue,
	XT5_API_CODE_SetColorChromeBlue          = API_CODE_SetColorChromeBlue,
	XT5_API_CODE_GetColorChromeBlue          = API_CODE_GetColorChromeBlue,
	XT5_API_CODE_CapMonochromaticColor       = API_CODE_CapMonochromaticColor,
	XT5_API_CODE_SetMonochromaticColor       = API_CODE_SetMonochromaticColor,
	XT5_API_CODE_GetMonochromaticColor       = API_CODE_GetMonochromaticColor,
	XT5_API_CODE_CapFocusLimiterPos          = API_CODE_CapFocusLimiterPos,
	XT5_API_CODE_SetFocusLimiterPos          = API_CODE_SetFocusLimiterPos,
	XT5_API_CODE_GetFocusLimiterIndicator    = API_CODE_GetFocusLimiterIndicator,
	XT5_API_CODE_GetFocusLimiterRange        = API_CODE_GetFocusLimiterRange,
	XT5_API_CODE_CapFocusLimiterMode         = API_CODE_CapFocusLimiterMode,
	XT5_API_CODE_SetFocusLimiterMode         = API_CODE_SetFocusLimiterMode,
	XT5_API_CODE_GetFocusLimiterMode         = API_CODE_GetFocusLimiterMode,
	XT5_API_CODE_GetCommandDialStatus        = API_CODE_GetCommandDialStatus,
	XT5_API_CODE_CapPerformanceSettings      = API_CODE_CapPerformanceSettings,
	XT5_API_CODE_SetPerformanceSettings      = API_CODE_SetPerformanceSettings,
	XT5_API_CODE_GetPerformanceSettings      = API_CODE_GetPerformanceSettings,
	XT5_API_CODE_CapMicLineSetting           = API_CODE_CapMicLineSetting,
	XT5_API_CODE_SetMicLineSetting           = API_CODE_SetMicLineSetting,
	XT5_API_CODE_GetMicLineSetting           = API_CODE_GetMicLineSetting,
	XT5_API_CODE_CapCustomSettingAutoUpdate  = API_CODE_CapCustomSettingAutoUpdate,
	XT5_API_CODE_SetCustomSettingAutoUpdate  = API_CODE_SetCustomSettingAutoUpdate,
	XT5_API_CODE_GetCustomSettingAutoUpdate  = API_CODE_GetCustomSettingAutoUpdate,
    XT5_API_CODE_CapPixelShiftSettings       = API_CODE_CapPixelShiftSettings,
    XT5_API_CODE_SetPixelShiftSettings       = API_CODE_SetPixelShiftSettings,
    XT5_API_CODE_GetPixelShiftSettings       = API_CODE_GetPixelShiftSettings,
    XT5_API_CODE_CapImageFormat              = API_CODE_CapImageFormat,
    XT5_API_CODE_SetImageFormat              = API_CODE_SetImageFormat,
    XT5_API_CODE_GetImageFormat              = API_CODE_GetImageFormat,
	XT5_API_CODE_CapSubjectDetectionMode     = API_CODE_CapSubjectDetectionMode,
	XT5_API_CODE_SetSubjectDetectionMode     = API_CODE_SetSubjectDetectionMode,
	XT5_API_CODE_GetSubjectDetectionMode     = API_CODE_GetSubjectDetectionMode,
	XT5_API_CODE_CapCropZoom                 = API_CODE_CapCropZoom,
	XT5_API_CODE_SetCropZoom                 = API_CODE_SetCropZoom,
	XT5_API_CODE_GetCropZoom                 = API_CODE_GetCropZoom,
	XT5_API_CODE_CapZoomOperation            = API_CODE_CapZoomOperation,
	XT5_API_CODE_SetZoomOperation            = API_CODE_SetZoomOperation,
	XT5_API_CODE_CapFocusOperation           = API_CODE_CapFocusOperation,
	XT5_API_CODE_SetFocusOperation           = API_CODE_SetFocusOperation,
	XT5_API_CODE_CapZoomSpeed                = API_CODE_CapZoomSpeed,
	XT5_API_CODE_SetZoomSpeed                = API_CODE_SetZoomSpeed,
	XT5_API_CODE_GetZoomSpeed                = API_CODE_GetZoomSpeed,
	XT5_API_CODE_CapFocusSpeed               = API_CODE_CapFocusSpeed,
	XT5_API_CODE_SetFocusSpeed               = API_CODE_SetFocusSpeed,
	XT5_API_CODE_GetFocusSpeed               = API_CODE_GetFocusSpeed,
	XT5_API_CODE_CapLiveViewStatus           = API_CODE_CapLiveViewStatus,
	XT5_API_CODE_GetLiveViewStatus           = API_CODE_GetLiveViewStatus,
	XT5_API_CODE_CapLiveViewMode             = API_CODE_CapLiveViewMode,
	XT5_API_CODE_SetLiveViewMode             = API_CODE_SetLiveViewMode,
	XT5_API_CODE_GetLiveViewMode             = API_CODE_GetLiveViewMode,
	XT5_API_CODE_CapLiveViewImageRatio       = API_CODE_CapLiveViewImageRatio,
	XT5_API_CODE_SetLiveViewImageRatio       = API_CODE_SetLiveViewImageRatio,
	XT5_API_CODE_GetLiveViewImageRatio       = API_CODE_GetLiveViewImageRatio,
	XT5_API_CODE_CapSummerTime               = API_CODE_CapSummerTime,
	XT5_API_CODE_SetSummerTime               = API_CODE_SetSummerTime,
	XT5_API_CODE_GetSummerTime               = API_CODE_GetSummerTime,
	XT5_API_CODE_CapFanSetting               = API_CODE_CapFanSetting,
	XT5_API_CODE_SetFanSetting               = API_CODE_SetFanSetting,
	XT5_API_CODE_GetFanSetting               = API_CODE_GetFanSetting,
	XT5_API_CODE_CapElectronicLevelSetting   = API_CODE_CapElectronicLevelSetting,
	XT5_API_CODE_SetElectronicLevelSetting   = API_CODE_SetElectronicLevelSetting,
	XT5_API_CODE_GetElectronicLevelSetting   = API_CODE_GetElectronicLevelSetting,
	XT5_API_CODE_CapApertureUnit             = API_CODE_CapApertureUnit,
	XT5_API_CODE_SetApertureUnit             = API_CODE_SetApertureUnit,
	XT5_API_CODE_GetApertureUnit             = API_CODE_GetApertureUnit,
	XT5_API_CODE_CapUSBPowerSupplyCommunication = API_CODE_CapUSBPowerSupplyCommunication,
	XT5_API_CODE_SetUSBPowerSupplyCommunication = API_CODE_SetUSBPowerSupplyCommunication,
	XT5_API_CODE_GetUSBPowerSupplyCommunication = API_CODE_GetUSBPowerSupplyCommunication,
	XT5_API_CODE_CapAutoPowerOffSetting      = API_CODE_CapAutoPowerOffSetting,
	XT5_API_CODE_SetAutoPowerOffSetting      = API_CODE_SetAutoPowerOffSetting,
	XT5_API_CODE_GetAutoPowerOffSetting      = API_CODE_GetAutoPowerOffSetting,
};

// FocusArea - Focus Area
// typedef struct {
//    long    h;      // HORIZONTAL POSITION (at the camera orientation of absolute 0 degree) 3(left) to +3(right)
//    long    v;      // VERTICAL POSITION (at the camera orientation of absolute 0 degree) -3(bottom) to +3(top)
//    long    size;   // AREA SIZE 1(small) to 5(large)
// } SDK_FocusArea
typedef  SDK_FocusArea     XT5_FocusArea, *PXT5_FocusArea;


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
typedef  SDK_ISOAuto     XT5_ISOAuto, *PXT5_ISOAuto;


// FrameGuideGridInfo - Frame Guideline Grid information
// typedef struct  {
//    double  gridH[5];       // HORIZONTAL LINE POSITION in %, 0.1(top) to 100.0(bottom), 0.0=suppressed
//    double  gridV[5];       // VERTICAL LINE POSITION in %, 0.1(left) to 100.0(right), 0.0=suppressed
//    double  lineWidth;      // LINE WIDTH in %, 0.0 to 25.0
//    long    lineColorIndex; // LINE COLOR, 0:BLACK, 1:BLUE, 2:GREEN, 3:CYAN, 4:RED, 5:VIOLET, 6:YELLOW, 7:WHITE
//    long    lineAlpha;      // TRANSPARENCY in %, 0(solid), 10, 20, 30, 40, 50, 60, 70, 80, 90, 100(transparent)
// } SDK_FrameGuideGridInfo;
typedef  SDK_FrameGuideGridInfo     XT5_FrameGuideGridInfo, *PXT5_FrameGuideGridInfo;


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
typedef  SDK_FOCUS_POS_CAP     SDK_XT5_FOCUS_POS_CAP, *PSDK_XT5_FOCUS_POS_CAP;
typedef  SDK_XT5_FOCUS_POS_CAP     XT5_FOCUS_POS_CAP, *PXT5_FOCUS_POS_CAP;


//typedef struct _SDK_FOLDER_INFO {
//    char pFoldernameSuffix[6];
//    long lFolderNumber;
//    long lMaxFrameNumber;
//    long lStatus;
//} SDK_FOLDER_INFO, *PSDK_FOLDER_INFO;
typedef  SDK_FOLDER_INFO     XT5_FOLDER_INFO, *PXT5_FOLDER_INFO;


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
typedef  SDK_CropAreaFrameInfo     XT5_CropAreaFrameInfo, *PXT5_CropAreaFrameInfo;


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
typedef  SDK_FaceFrameInfo     XT5_FaceFrameInfo, *PXT5_FaceFrameInfo;


// Custom White Balance Information
//typedef struct{
//    long    lX;
//    long    lY;
//    long    lSize;
//    long    lMode;
//} SDK_CustomWBArea;
typedef  SDK_CustomWBArea     XT5_CustomWBArea, *PXT5_CustomWBArea;

// Focus Limiter
//typedef struct _SDK_FOCUS_LIMITER_INDICATOR {
//	long	lCurrent;
//	long	lDOF_Near;
//	long	lDOF_Far;
//	long	lPos_A;
//	long	lPos_B;
//	long	lStatus;
//} SDK_FOCUS_LIMITER_INDICATOR;
typedef  SDK_FOCUS_LIMITER_INDICATOR     XT5_FOCUS_LIMITER_INDICATOR, *PXT5_FOCUS_LIMITER_INDICATOR;

// Focus Limiter Range
//typedef struct _SDK_FOCUS_LIMITER {
//	long	lPos_A;
//	long	lPos_B;
//} SDK_FOCUS_LIMITER;
typedef  SDK_FOCUS_LIMITER     XT5_FOCUS_LIMITER, *PXT5_FOCUS_LIMITER;


// Image Size
enum{
	XT5_IMAGESIZE_S_3_2       = SDK_IMAGESIZE_S_3_2,
	XT5_IMAGESIZE_S_16_9      = SDK_IMAGESIZE_S_16_9,
	XT5_IMAGESIZE_S_1_1       = SDK_IMAGESIZE_S_1_1,
	XT5_IMAGESIZE_M_3_2       = SDK_IMAGESIZE_M_3_2,
	XT5_IMAGESIZE_M_16_9      = SDK_IMAGESIZE_M_16_9,
	XT5_IMAGESIZE_M_1_1       = SDK_IMAGESIZE_M_1_1,
	XT5_IMAGESIZE_L_3_2       = SDK_IMAGESIZE_L_3_2,
	XT5_IMAGESIZE_L_16_9      = SDK_IMAGESIZE_L_16_9,
	XT5_IMAGESIZE_L_1_1       = SDK_IMAGESIZE_L_1_1,
	XT5_IMAGESIZE_S_4_3       = SDK_IMAGESIZE_S_4_3,
	XT5_IMAGESIZE_S_65_24     = SDK_IMAGESIZE_S_65_24,
	XT5_IMAGESIZE_S_5_4       = SDK_IMAGESIZE_S_5_4,
	XT5_IMAGESIZE_S_7_6       = SDK_IMAGESIZE_S_7_6,
	XT5_IMAGESIZE_L_4_3       = SDK_IMAGESIZE_L_4_3,
	XT5_IMAGESIZE_L_65_24     = SDK_IMAGESIZE_L_65_24,
	XT5_IMAGESIZE_L_5_4       = SDK_IMAGESIZE_L_5_4,
	XT5_IMAGESIZE_L_7_6       = SDK_IMAGESIZE_L_7_6,
	XT5_IMAGESIZE_M_4_3       = SDK_IMAGESIZE_M_4_3,
	XT5_IMAGESIZE_M_65_24     = SDK_IMAGESIZE_M_65_24,
	XT5_IMAGESIZE_M_5_4       = SDK_IMAGESIZE_M_5_4,
	XT5_IMAGESIZE_M_7_6       = SDK_IMAGESIZE_M_7_6,
};

// Still Image Quality
enum{
	XT5_IMAGEQUALITY_RAW          = SDK_IMAGEQUALITY_RAW,
	XT5_IMAGEQUALITY_FINE         = SDK_IMAGEQUALITY_FINE,
	XT5_IMAGEQUALITY_NORMAL       = SDK_IMAGEQUALITY_NORMAL,
	XT5_IMAGEQUALITY_RAW_FINE     = SDK_IMAGEQUALITY_RAW_FINE,
	XT5_IMAGEQUALITY_RAW_NORMAL   = SDK_IMAGEQUALITY_RAW_NORMAL,
	XT5_IMAGEQUALITY_SUPERFINE    = SDK_IMAGEQUALITY_SUPERFINE,
	XT5_IMAGEQUALITY_RAW_SUPERFINE   = SDK_IMAGEQUALITY_RAW_SUPERFINE,
};

// Image Format
enum{
	XT5_IMAGEFORMAT_JPEG         = SDK_IMAGEFORMAT_JPEG,
	XT5_IMAGEFORMAT_HEIF         = SDK_IMAGEFORMAT_HEIF,
};

// RAW Image Quality
enum{
	XT5_RAWOUTPUTDEPTH_14BIT     = SDK_RAWOUTPUTDEPTH_14BIT,
	XT5_RAWOUTPUTDEPTH_16BIT     = SDK_RAWOUTPUTDEPTH_16BIT,
};

// LiveView Mode
enum{
	XT5_LIVEVIEW_MODE1           = SDK_LIVEVIEW_MODE1,
	XT5_LIVEVIEW_MODE2           = SDK_LIVEVIEW_MODE2,
};

// LiveView Ratio
enum{
	XT5_LIVEVIEW_RATIO_FIXED     = SDK_LIVEVIEW_RATIO_FIXED,
	XT5_LIVEVIEW_RATIO_VARIABLE  = SDK_LIVEVIEW_RATIO_VARIABLE,
};

// LiveView Image Quality
enum{
	XT5_LIVEVIEW_QUALITY_FINE    = SDK_LIVE_QUALITY_FINE,
	XT5_LIVEVIEW_QUALITY_NORMAL  = SDK_LIVE_QUALITY_NORMAL,
	XT5_LIVEVIEW_QUALITY_BASIC   = SDK_LIVE_QUALITY_BASIC,
	XT5_LIVE_QUALITY_FINE        = SDK_LIVE_QUALITY_FINE,
	XT5_LIVE_QUALITY_NORMAL      = SDK_LIVE_QUALITY_NORMAL,
	XT5_LIVE_QUALITY_BASIC       = SDK_LIVE_QUALITY_BASIC,
};

// LiveView Image Size
enum{
	XT5_LIVEVIEW_SIZE_L     = SDK_LIVE_SIZE_L,
	XT5_LIVEVIEW_SIZE_M     = SDK_LIVE_SIZE_M,
	XT5_LIVEVIEW_SIZE_S     = SDK_LIVE_SIZE_S,
	XT5_LIVE_SIZE_L         = SDK_LIVE_SIZE_L,
	XT5_LIVE_SIZE_M         = SDK_LIVE_SIZE_M,
	XT5_LIVE_SIZE_S         = SDK_LIVE_SIZE_S,
	XT5_LIVE_SIZE_1024      = SDK_LIVE_SIZE_1024,
	XT5_LIVE_SIZE_640       = SDK_LIVE_SIZE_640,
	XT5_LIVE_SIZE_320       = SDK_LIVE_SIZE_320,
};

// Through Image Zoom
enum{
	XT5_THROUGH_ZOOM_10     = SDK_THROUGH_ZOOM_10,
	XT5_THROUGH_ZOOM_25     = SDK_THROUGH_ZOOM_25,
	XT5_THROUGH_ZOOM_60     = SDK_THROUGH_ZOOM_60,
	XT5_THROUGH_ZOOM_40     = SDK_THROUGH_ZOOM_40,
	XT5_THROUGH_ZOOM_80     = SDK_THROUGH_ZOOM_80,
	XT5_THROUGH_ZOOM_160    = SDK_THROUGH_ZOOM_160,
	XT5_THROUGH_ZOOM_20     = SDK_THROUGH_ZOOM_20,
	XT5_THROUGH_ZOOM_33     = SDK_THROUGH_ZOOM_33,
	XT5_THROUGH_ZOOM_66     = SDK_THROUGH_ZOOM_66,
	XT5_THROUGH_ZOOM_131    = SDK_THROUGH_ZOOM_131,
	XT5_THROUGH_ZOOM_240    = SDK_THROUGH_ZOOM_240,
	XT5_THROUGH_ZOOM_197    = SDK_THROUGH_ZOOM_197,
	XT5_THROUGH_ZOOM_120    = SDK_THROUGH_ZOOM_120,
};

// Color Space
enum{
	XT5_COLORSPACE_sRGB        = SDK_COLORSPACE_sRGB,
	XT5_COLORSPACE_AdobeRGB    = SDK_COLORSPACE_AdobeRGB,
};

// White Balance
enum{
	XT5_WB_AUTO            = SDK_WB_AUTO,  
	XT5_WB_AUTO_WHITE_PRIORITY    = SDK_WB_AUTO_WHITE_PRIORITY,
	XT5_WB_AUTO_AMBIENCE_PRIORITY = SDK_WB_AUTO_AMBIENCE_PRIORITY,
	XT5_WB_DAYLIGHT        = SDK_WB_DAYLIGHT,
	XT5_WB_INCANDESCENT    = SDK_WB_INCANDESCENT,
	XT5_WB_UNDER_WATER     = SDK_WB_UNDER_WATER,
	XT5_WB_FLUORESCENT1    = SDK_WB_FLUORESCENT1,
	XT5_WB_FLUORESCENT2    = SDK_WB_FLUORESCENT2,
	XT5_WB_FLUORESCENT3    = SDK_WB_FLUORESCENT3,
	XT5_WB_SHADE           = SDK_WB_SHADE,
	XT5_WB_COLORTEMP       = SDK_WB_COLORTEMP,
	XT5_WB_CUSTOM1         = SDK_WB_CUSTOM1,
	XT5_WB_CUSTOM2         = SDK_WB_CUSTOM2,
	XT5_WB_CUSTOM3         = SDK_WB_CUSTOM3,
};

// Color Temperature
enum {
	XT5_WB_COLORTEMP_2500        = SDK_WB_COLORTEMP_2500,
	XT5_WB_COLORTEMP_2550        = SDK_WB_COLORTEMP_2550,
	XT5_WB_COLORTEMP_2650        = SDK_WB_COLORTEMP_2650,
	XT5_WB_COLORTEMP_2700        = SDK_WB_COLORTEMP_2700,
	XT5_WB_COLORTEMP_2800        = SDK_WB_COLORTEMP_2800,
	XT5_WB_COLORTEMP_2850        = SDK_WB_COLORTEMP_2850,
	XT5_WB_COLORTEMP_2950        = SDK_WB_COLORTEMP_2950,
	XT5_WB_COLORTEMP_3000        = SDK_WB_COLORTEMP_3000,
	XT5_WB_COLORTEMP_3100        = SDK_WB_COLORTEMP_3100,
	XT5_WB_COLORTEMP_3200        = SDK_WB_COLORTEMP_3200,
	XT5_WB_COLORTEMP_3300        = SDK_WB_COLORTEMP_3300,
	XT5_WB_COLORTEMP_3400        = SDK_WB_COLORTEMP_3400,
	XT5_WB_COLORTEMP_3600        = SDK_WB_COLORTEMP_3600,
	XT5_WB_COLORTEMP_3700        = SDK_WB_COLORTEMP_3700,
	XT5_WB_COLORTEMP_3800        = SDK_WB_COLORTEMP_3800,
	XT5_WB_COLORTEMP_4000        = SDK_WB_COLORTEMP_4000,
	XT5_WB_COLORTEMP_4200        = SDK_WB_COLORTEMP_4200,
	XT5_WB_COLORTEMP_4300        = SDK_WB_COLORTEMP_4300,
	XT5_WB_COLORTEMP_4500        = SDK_WB_COLORTEMP_4500,
	XT5_WB_COLORTEMP_4800        = SDK_WB_COLORTEMP_4800,
	XT5_WB_COLORTEMP_5000        = SDK_WB_COLORTEMP_5000,
	XT5_WB_COLORTEMP_5300        = SDK_WB_COLORTEMP_5300,
	XT5_WB_COLORTEMP_5600        = SDK_WB_COLORTEMP_5600,
	XT5_WB_COLORTEMP_5900        = SDK_WB_COLORTEMP_5900,
	XT5_WB_COLORTEMP_6300        = SDK_WB_COLORTEMP_6300,
	XT5_WB_COLORTEMP_6700        = SDK_WB_COLORTEMP_6700,
	XT5_WB_COLORTEMP_7100        = SDK_WB_COLORTEMP_7100,
	XT5_WB_COLORTEMP_7700        = SDK_WB_COLORTEMP_7700,
	XT5_WB_COLORTEMP_8300        = SDK_WB_COLORTEMP_8300,
	XT5_WB_COLORTEMP_9100        = SDK_WB_COLORTEMP_9100,
	XT5_WB_COLORTEMP_10000       = SDK_WB_COLORTEMP_10000,
	XT5_WB_COLORTEMP_CURRENT     = SDK_WB_COLORTEMP_CURRENT
};

// WB Shift
enum {
	XT5_WB_R_SHIFT_MIN     = SDK_WB_R_SHIFT_MIN,
	XT5_WB_R_SHIFT_MAX     = SDK_WB_R_SHIFT_MAX,
	XT5_WB_R_SHIFT_STEP    = 1,
	XT5_WB_B_SHIFT_MIN     = SDK_WB_B_SHIFT_MIN,
	XT5_WB_B_SHIFT_MAX     = SDK_WB_B_SHIFT_MAX,
	XT5_WB_B_SHIFT_STEP    = 1,
};

// Custom White balance Information
enum {
	XT5_CUSTOM_WB_MODE_LIVEVIEW = SDK_CUSTOM_WB_MODE_LIVEVIEW,
	XT5_CUSTOM_WB_MODE_PLAY     = SDK_CUSTOM_WB_MODE_PLAY,
};

// FILM SIMULATION
enum{
	XT5_FILMSIMULATION_PROVIA          = SDK_FILMSIMULATION_PROVIA,
	XT5_FILMSIMULATION_STD             = SDK_FILMSIMULATION_STD,
	XT5_FILMSIMULATION_VELVIA          = SDK_FILMSIMULATION_VELVIA,
	XT5_FILMSIMULATION_ASTIA           = SDK_FILMSIMULATION_ASTIA,
	XT5_FILMSIMULATION_CLASSICCHROME   = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XT5_FILMSIMULATION_NEGHI           = SDK_FILMSIMULATION_NEGHI,
	XT5_FILMSIMULATION_NEGSTD          = SDK_FILMSIMULATION_NEGSTD,
	XT5_FILMSIMULATION_MONOCHRO        = SDK_FILMSIMULATION_MONOCHRO,
	XT5_FILMSIMULATION_MONOCHRO_Y      = SDK_FILMSIMULATION_MONOCHRO_Y,
	XT5_FILMSIMULATION_MONOCHRO_R      = SDK_FILMSIMULATION_MONOCHRO_R,
	XT5_FILMSIMULATION_MONOCHRO_G      = SDK_FILMSIMULATION_MONOCHRO_G,
	XT5_FILMSIMULATION_SEPIA           = SDK_FILMSIMULATION_SEPIA,
	XT5_FILMSIMULATION_CLASSIC_CHROME  = SDK_FILMSIMULATION_CLASSIC_CHROME,
	XT5_FILMSIMULATION_ACROS           = SDK_FILMSIMULATION_ACROS,
	XT5_FILMSIMULATION_ACROS_Y         = SDK_FILMSIMULATION_ACROS_Y,
	XT5_FILMSIMULATION_ACROS_R         = SDK_FILMSIMULATION_ACROS_R,
	XT5_FILMSIMULATION_ACROS_G         = SDK_FILMSIMULATION_ACROS_G,
	XT5_FILMSIMULATION_ETERNA          = SDK_FILMSIMULATION_ETERNA,
	XT5_FILMSIMULATION_CLASSICNEG      = SDK_FILMSIMULATION_CLASSICNEG,
	XT5_FILMSIMULATION_BLEACH_BYPASS   = SDK_FILMSIMULATION_BLEACH_BYPASS,
	XT5_FILMSIMULATION_NOSTALGICNEG    = SDK_FILMSIMULATION_NOSTALGICNEG,
	XT5_FILMSIMULATION_REALA_ACE       = SDK_FILMSIMULATION_REALA_ACE,
};

// COLOR
enum{
	XT5_COLOR_HIGH          = SDK_COLOR_HIGH,
	XT5_COLOR_MEDIUM_HIGH   = SDK_COLOR_MEDIUM_HIGH,
	XT5_COLOR_STANDARD      = SDK_COLOR_STANDARD,
	XT5_COLOR_MEDIUM_LOW    = SDK_COLOR_MEDIUM_LOW,
	XT5_COLOR_LOW           = SDK_COLOR_LOW,
	XT5_COLOR_P4            = SDK_COLOR_P4,
	XT5_COLOR_P3            = SDK_COLOR_P3,
	XT5_COLOR_P2            = SDK_COLOR_P2,
	XT5_COLOR_P1            = SDK_COLOR_P1,
	XT5_COLOR_0             = SDK_COLOR_0,
	XT5_COLOR_M1            = SDK_COLOR_M1,
	XT5_COLOR_M2            = SDK_COLOR_M2,
	XT5_COLOR_M3            = SDK_COLOR_M3,
	XT5_COLOR_M4            = SDK_COLOR_M4,
};

// Monochromatic Color WC
enum{ 
	XT5_MONOCHROMATICCOLOR_WC_P180	= SDK_MONOCHROMATICCOLOR_WC_P180,
	XT5_MONOCHROMATICCOLOR_WC_P170	= SDK_MONOCHROMATICCOLOR_WC_P170,
	XT5_MONOCHROMATICCOLOR_WC_P160	= SDK_MONOCHROMATICCOLOR_WC_P160,
	XT5_MONOCHROMATICCOLOR_WC_P150	= SDK_MONOCHROMATICCOLOR_WC_P150,
	XT5_MONOCHROMATICCOLOR_WC_P140	= SDK_MONOCHROMATICCOLOR_WC_P140,
	XT5_MONOCHROMATICCOLOR_WC_P130	= SDK_MONOCHROMATICCOLOR_WC_P130,
	XT5_MONOCHROMATICCOLOR_WC_P120	= SDK_MONOCHROMATICCOLOR_WC_P120,
	XT5_MONOCHROMATICCOLOR_WC_P110	= SDK_MONOCHROMATICCOLOR_WC_P110,
	XT5_MONOCHROMATICCOLOR_WC_P100	= SDK_MONOCHROMATICCOLOR_WC_P100,
	XT5_MONOCHROMATICCOLOR_WC_P90		= SDK_MONOCHROMATICCOLOR_WC_P90,
	XT5_MONOCHROMATICCOLOR_WC_P80		= SDK_MONOCHROMATICCOLOR_WC_P80,
	XT5_MONOCHROMATICCOLOR_WC_P70		= SDK_MONOCHROMATICCOLOR_WC_P70,
	XT5_MONOCHROMATICCOLOR_WC_P60		= SDK_MONOCHROMATICCOLOR_WC_P60,
	XT5_MONOCHROMATICCOLOR_WC_P50		= SDK_MONOCHROMATICCOLOR_WC_P50,
	XT5_MONOCHROMATICCOLOR_WC_P40		= SDK_MONOCHROMATICCOLOR_WC_P40,
	XT5_MONOCHROMATICCOLOR_WC_P30		= SDK_MONOCHROMATICCOLOR_WC_P30,
	XT5_MONOCHROMATICCOLOR_WC_P20		= SDK_MONOCHROMATICCOLOR_WC_P20,
	XT5_MONOCHROMATICCOLOR_WC_P10		= SDK_MONOCHROMATICCOLOR_WC_P10,
	XT5_MONOCHROMATICCOLOR_WC_0		= SDK_MONOCHROMATICCOLOR_WC_0,
	XT5_MONOCHROMATICCOLOR_WC_M10		= SDK_MONOCHROMATICCOLOR_WC_M10,
	XT5_MONOCHROMATICCOLOR_WC_M20		= SDK_MONOCHROMATICCOLOR_WC_M20,
	XT5_MONOCHROMATICCOLOR_WC_M30		= SDK_MONOCHROMATICCOLOR_WC_M30,
	XT5_MONOCHROMATICCOLOR_WC_M40		= SDK_MONOCHROMATICCOLOR_WC_M40,
	XT5_MONOCHROMATICCOLOR_WC_M50		= SDK_MONOCHROMATICCOLOR_WC_M50,
	XT5_MONOCHROMATICCOLOR_WC_M60		= SDK_MONOCHROMATICCOLOR_WC_M60,
	XT5_MONOCHROMATICCOLOR_WC_M70		= SDK_MONOCHROMATICCOLOR_WC_M70,
	XT5_MONOCHROMATICCOLOR_WC_M80		= SDK_MONOCHROMATICCOLOR_WC_M80,
	XT5_MONOCHROMATICCOLOR_WC_M90		= SDK_MONOCHROMATICCOLOR_WC_M90,
	XT5_MONOCHROMATICCOLOR_WC_M100	= SDK_MONOCHROMATICCOLOR_WC_M100,
	XT5_MONOCHROMATICCOLOR_WC_M110	= SDK_MONOCHROMATICCOLOR_WC_M110,
	XT5_MONOCHROMATICCOLOR_WC_M120	= SDK_MONOCHROMATICCOLOR_WC_M120,
	XT5_MONOCHROMATICCOLOR_WC_M130	= SDK_MONOCHROMATICCOLOR_WC_M130,
	XT5_MONOCHROMATICCOLOR_WC_M140	= SDK_MONOCHROMATICCOLOR_WC_M140,
	XT5_MONOCHROMATICCOLOR_WC_M150	= SDK_MONOCHROMATICCOLOR_WC_M150,
	XT5_MONOCHROMATICCOLOR_WC_M160	= SDK_MONOCHROMATICCOLOR_WC_M160,
	XT5_MONOCHROMATICCOLOR_WC_M170	= SDK_MONOCHROMATICCOLOR_WC_M170,
	XT5_MONOCHROMATICCOLOR_WC_M180	= SDK_MONOCHROMATICCOLOR_WC_M180,
};

// Monochromatic Color Red Green
enum{ 
	XT5_MONOCHROMATICCOLOR_RG_P180	= SDK_MONOCHROMATICCOLOR_RG_P180,
	XT5_MONOCHROMATICCOLOR_RG_P170	= SDK_MONOCHROMATICCOLOR_RG_P170,
	XT5_MONOCHROMATICCOLOR_RG_P160	= SDK_MONOCHROMATICCOLOR_RG_P160,
	XT5_MONOCHROMATICCOLOR_RG_P150	= SDK_MONOCHROMATICCOLOR_RG_P150,
	XT5_MONOCHROMATICCOLOR_RG_P140	= SDK_MONOCHROMATICCOLOR_RG_P140,
	XT5_MONOCHROMATICCOLOR_RG_P130	= SDK_MONOCHROMATICCOLOR_RG_P130,
	XT5_MONOCHROMATICCOLOR_RG_P120	= SDK_MONOCHROMATICCOLOR_RG_P120,
	XT5_MONOCHROMATICCOLOR_RG_P110	= SDK_MONOCHROMATICCOLOR_RG_P110,
	XT5_MONOCHROMATICCOLOR_RG_P100	= SDK_MONOCHROMATICCOLOR_RG_P100,
	XT5_MONOCHROMATICCOLOR_RG_P90		= SDK_MONOCHROMATICCOLOR_RG_P90,
	XT5_MONOCHROMATICCOLOR_RG_P80		= SDK_MONOCHROMATICCOLOR_RG_P80,
	XT5_MONOCHROMATICCOLOR_RG_P70		= SDK_MONOCHROMATICCOLOR_RG_P70,
	XT5_MONOCHROMATICCOLOR_RG_P60		= SDK_MONOCHROMATICCOLOR_RG_P60,
	XT5_MONOCHROMATICCOLOR_RG_P50		= SDK_MONOCHROMATICCOLOR_RG_P50,
	XT5_MONOCHROMATICCOLOR_RG_P40		= SDK_MONOCHROMATICCOLOR_RG_P40,
	XT5_MONOCHROMATICCOLOR_RG_P30		= SDK_MONOCHROMATICCOLOR_RG_P30,
	XT5_MONOCHROMATICCOLOR_RG_P20		= SDK_MONOCHROMATICCOLOR_RG_P20,
	XT5_MONOCHROMATICCOLOR_RG_P10		= SDK_MONOCHROMATICCOLOR_RG_P10,
	XT5_MONOCHROMATICCOLOR_RG_0		= SDK_MONOCHROMATICCOLOR_RG_0,
	XT5_MONOCHROMATICCOLOR_RG_M10		= SDK_MONOCHROMATICCOLOR_RG_M10,
	XT5_MONOCHROMATICCOLOR_RG_M20		= SDK_MONOCHROMATICCOLOR_RG_M20,
	XT5_MONOCHROMATICCOLOR_RG_M30		= SDK_MONOCHROMATICCOLOR_RG_M30,
	XT5_MONOCHROMATICCOLOR_RG_M40		= SDK_MONOCHROMATICCOLOR_RG_M40,
	XT5_MONOCHROMATICCOLOR_RG_M50		= SDK_MONOCHROMATICCOLOR_RG_M50,
	XT5_MONOCHROMATICCOLOR_RG_M60		= SDK_MONOCHROMATICCOLOR_RG_M60,
	XT5_MONOCHROMATICCOLOR_RG_M70		= SDK_MONOCHROMATICCOLOR_RG_M70,
	XT5_MONOCHROMATICCOLOR_RG_M80		= SDK_MONOCHROMATICCOLOR_RG_M80,
	XT5_MONOCHROMATICCOLOR_RG_M90		= SDK_MONOCHROMATICCOLOR_RG_M90,
	XT5_MONOCHROMATICCOLOR_RG_M100	= SDK_MONOCHROMATICCOLOR_RG_M100,
	XT5_MONOCHROMATICCOLOR_RG_M110	= SDK_MONOCHROMATICCOLOR_RG_M110,
	XT5_MONOCHROMATICCOLOR_RG_M120	= SDK_MONOCHROMATICCOLOR_RG_M120,
	XT5_MONOCHROMATICCOLOR_RG_M130	= SDK_MONOCHROMATICCOLOR_RG_M130,
	XT5_MONOCHROMATICCOLOR_RG_M140	= SDK_MONOCHROMATICCOLOR_RG_M140,
	XT5_MONOCHROMATICCOLOR_RG_M150	= SDK_MONOCHROMATICCOLOR_RG_M150,
	XT5_MONOCHROMATICCOLOR_RG_M160	= SDK_MONOCHROMATICCOLOR_RG_M160,
	XT5_MONOCHROMATICCOLOR_RG_M170	= SDK_MONOCHROMATICCOLOR_RG_M170,
	XT5_MONOCHROMATICCOLOR_RG_M180	= SDK_MONOCHROMATICCOLOR_RG_M180,
};

// SHARPNESS
enum{
	XT5_SHARPNESSTYPE_HARD        = SDK_SHARPNESSTYPE_HARD,
	XT5_SHARPNESSTYPE_MEDIUM_HARD = SDK_SHARPNESSTYPE_MEDIUM_HARD,
	XT5_SHARPNESSTYPE_STANDARD    = SDK_SHARPNESSTYPE_STANDARD,
	XT5_SHARPNESSTYPE_MEDIUM_SOFT = SDK_SHARPNESSTYPE_MEDIUM_SOFT,
	XT5_SHARPNESSTYPE_SOFT        = SDK_SHARPNESSTYPE_SOFT,
	XT5_SHARPNESS_P4              = SDK_SHARPNESS_P4,
	XT5_SHARPNESS_P3              = SDK_SHARPNESS_P3,
	XT5_SHARPNESS_P2              = SDK_SHARPNESS_P2,
	XT5_SHARPNESS_P1              = SDK_SHARPNESS_P1,
	XT5_SHARPNESS_0               = SDK_SHARPNESS_0,
	XT5_SHARPNESS_M1              = SDK_SHARPNESS_M1,
	XT5_SHARPNESS_M2              = SDK_SHARPNESS_M2,
	XT5_SHARPNESS_M3              = SDK_SHARPNESS_M3,
	XT5_SHARPNESS_M4              = SDK_SHARPNESS_M4,
};

// HIGHLIGHT TONE
enum{
	XT5_HIGHLIGHT_TONE_HARD       = SDK_HIGHLIGHT_TONE_HARD,
	XT5_HIGHLIGHT_TONE_MEDIUM_HARD= SDK_HIGHLIGHT_TONE_MEDIUM_HARD,
	XT5_HIGHLIGHT_TONE_STANDARD   = SDK_HIGHLIGHT_TONE_STANDARD,
	XT5_HIGHLIGHT_TONE_MEDIUM_SOFT= SDK_HIGHLIGHT_TONE_MEDIUM_SOFT,
	XT5_HIGHLIGHT_TONE_SOFT       = SDK_HIGHLIGHT_TONE_SOFT,
	XT5_HIGHLIGHT_TONE_P4         = SDK_HIGHLIGHT_TONE_P4,
	XT5_HIGHLIGHT_TONE_P3_5       = SDK_HIGHLIGHT_TONE_P3_5,
	XT5_HIGHLIGHT_TONE_P3         = SDK_HIGHLIGHT_TONE_P3,
	XT5_HIGHLIGHT_TONE_P2_5       = SDK_HIGHLIGHT_TONE_P2_5,
	XT5_HIGHLIGHT_TONE_P2         = SDK_HIGHLIGHT_TONE_P2,
	XT5_HIGHLIGHT_TONE_P1_5       = SDK_HIGHLIGHT_TONE_P1_5,
	XT5_HIGHLIGHT_TONE_P1         = SDK_HIGHLIGHT_TONE_P1,
	XT5_HIGHLIGHT_TONE_P0_5       = SDK_HIGHLIGHT_TONE_P0_5,
	XT5_HIGHLIGHT_TONE_0          = SDK_HIGHLIGHT_TONE_0,
	XT5_HIGHLIGHT_TONE_M0_5       = SDK_HIGHLIGHT_TONE_M0_5,
	XT5_HIGHLIGHT_TONE_M1         = SDK_HIGHLIGHT_TONE_M1,
	XT5_HIGHLIGHT_TONE_M1_5       = SDK_HIGHLIGHT_TONE_M1_5,
	XT5_HIGHLIGHT_TONE_M2         = SDK_HIGHLIGHT_TONE_M2,
};

// SHADOW TONE
enum{
	XT5_SHADOW_TONE_HARD          = SDK_SHADOW_TONE_HARD,
	XT5_SHADOW_TONE_MEDIUM_HARD   = SDK_SHADOW_TONE_MEDIUM_HARD,
	XT5_SHADOW_TONE_STANDARD      = SDK_SHADOW_TONE_STANDARD,
	XT5_SHADOW_TONE_MEDIUM_SOFT   = SDK_SHADOW_TONE_MEDIUM_SOFT,
	XT5_SHADOW_TONE_SOFT          = SDK_SHADOW_TONE_SOFT,
	XT5_SHADOW_TONE_P4            = SDK_SHADOW_TONE_P4,
	XT5_SHADOW_TONE_P3_5          = SDK_SHADOW_TONE_P3_5,
	XT5_SHADOW_TONE_P3            = SDK_SHADOW_TONE_P3,
	XT5_SHADOW_TONE_P2_5          = SDK_SHADOW_TONE_P2_5,
	XT5_SHADOW_TONE_P2            = SDK_SHADOW_TONE_P2,
	XT5_SHADOW_TONE_P1_5          = SDK_SHADOW_TONE_P1_5,
	XT5_SHADOW_TONE_P1            = SDK_SHADOW_TONE_P1,
	XT5_SHADOW_TONE_P0_5          = SDK_SHADOW_TONE_P0_5,
	XT5_SHADOW_TONE_0             = SDK_SHADOW_TONE_0,
	XT5_SHADOW_TONE_M0_5          = SDK_SHADOW_TONE_M0_5,
	XT5_SHADOW_TONE_M1            = SDK_SHADOW_TONE_M1,
	XT5_SHADOW_TONE_M1_5          = SDK_SHADOW_TONE_M1_5,
	XT5_SHADOW_TONE_M2            = SDK_SHADOW_TONE_M2,
};

// NOISE REDUCTION
enum{
	XT5_NOISEREDUCTION_HIGH         = SDK_NOISEREDUCTION_HIGH,
	XT5_NOISEREDUCTION_MEDIUM_HIGH  = SDK_NOISEREDUCTION_MEDIUM_HIGH,
	XT5_NOISEREDUCTION_STANDARD     = SDK_NOISEREDUCTION_STANDARD,
	XT5_NOISEREDUCTION_MEDIUM_LOW   = SDK_NOISEREDUCTION_MEDIUM_LOW,
	XT5_NOISEREDUCTION_LOW          = SDK_NOISEREDUCTION_LOW,
	XT5_NOISEREDUCTION_P4           = SDK_NOISEREDUCTION_P4,
	XT5_NOISEREDUCTION_P3           = SDK_NOISEREDUCTION_P3,
	XT5_NOISEREDUCTION_P2           = SDK_NOISEREDUCTION_P2,
	XT5_NOISEREDUCTION_P1           = SDK_NOISEREDUCTION_P1,
	XT5_NOISEREDUCTION_0            = SDK_NOISEREDUCTION_0,
	XT5_NOISEREDUCTION_M1           = SDK_NOISEREDUCTION_M1,
	XT5_NOISEREDUCTION_M2           = SDK_NOISEREDUCTION_M2,
	XT5_NOISEREDUCTION_M3           = SDK_NOISEREDUCTION_M3,
	XT5_NOISEREDUCTION_M4           = SDK_NOISEREDUCTION_M4,
};

// CUSTOM SETTING
enum{
	XT5_CUSTOM_SETTING_CUSTOM1       = SDK_CUSTOM_SETTING_CUSTOM1,
	XT5_CUSTOM_SETTING_CUSTOM2       = SDK_CUSTOM_SETTING_CUSTOM2,
	XT5_CUSTOM_SETTING_CUSTOM3       = SDK_CUSTOM_SETTING_CUSTOM3,
	XT5_CUSTOM_SETTING_CUSTOM4       = SDK_CUSTOM_SETTING_CUSTOM4,
	XT5_CUSTOM_SETTING_CUSTOM5       = SDK_CUSTOM_SETTING_CUSTOM5,
	XT5_CUSTOM_SETTING_CUSTOM6       = SDK_CUSTOM_SETTING_CUSTOM6,
	XT5_CUSTOM_SETTING_CUSTOM7       = SDK_CUSTOM_SETTING_CUSTOM7,
};

// RAW Compression
enum{
	XT5_RAW_COMPRESSION_OFF          = SDK_RAW_COMPRESSION_OFF,
	XT5_RAW_COMPRESSION_LOSSLESS     = SDK_RAW_COMPRESSION_LOSSLESS,
	XT5_RAW_COMPRESSION_LOSSY		 = SDK_RAW_COMPRESSION_LOSSY,
};

// Grain Effect
enum{
	XT5_GRAIN_EFFECT_OFF             = SDK_GRAIN_EFFECT_OFF,
	XT5_GRAIN_EFFECT_WEAK            = SDK_GRAIN_EFFECT_WEAK,
	XT5_GRAIN_EFFECT_P1              = SDK_GRAIN_EFFECT_P1,
	XT5_GRAIN_EFFECT_STRONG          = SDK_GRAIN_EFFECT_STRONG,
	XT5_GRAIN_EFFECT_P2              = SDK_GRAIN_EFFECT_P2,
	XT5_GRAIN_EFFECT_OFF_SMALL       = SDK_GRAIN_EFFECT_OFF_SMALL,
	XT5_GRAIN_EFFECT_WEAK_SMALL      = SDK_GRAIN_EFFECT_WEAK_SMALL,
	XT5_GRAIN_EFFECT_STRONG_SMALL    = SDK_GRAIN_EFFECT_STRONG_SMALL,
	XT5_GRAIN_EFFECT_OFF_LARGE       = SDK_GRAIN_EFFECT_OFF_LARGE,
	XT5_GRAIN_EFFECT_WEAK_LARGE      = SDK_GRAIN_EFFECT_WEAK_LARGE,
	XT5_GRAIN_EFFECT_STRONG_LARGE    = SDK_GRAIN_EFFECT_STRONG_LARGE,
};

// Clarity Mode
enum{
	XT5_CLARITY_P5		= SDK_CLARITY_P5,
	XT5_CLARITY_P4		= SDK_CLARITY_P4,
	XT5_CLARITY_P3		= SDK_CLARITY_P3,
	XT5_CLARITY_P2		= SDK_CLARITY_P2,
	XT5_CLARITY_P1		= SDK_CLARITY_P1,
	XT5_CLARITY_0			= SDK_CLARITY_0 ,
	XT5_CLARITY_M1		= SDK_CLARITY_M1,
	XT5_CLARITY_M2		= SDK_CLARITY_M2,
	XT5_CLARITY_M3		= SDK_CLARITY_M3,
	XT5_CLARITY_M4		= SDK_CLARITY_M4,
	XT5_CLARITY_M5		= SDK_CLARITY_M5,
};

// Shadowing
enum{
	XT5_SHADOWING_0             = SDK_SHADOWING_0,
	XT5_SHADOWING_P1            = SDK_SHADOWING_P1,
	XT5_SHADOWING_P2            = SDK_SHADOWING_P2,
};

// ColorChrome Blue
enum{
	XT5_COLORCHROME_BLUE_0      = SDK_COLORCHROME_BLUE_0,
	XT5_COLORCHROME_BLUE_P1     = SDK_COLORCHROME_BLUE_P1,
	XT5_COLORCHROME_BLUE_P2     = SDK_COLORCHROME_BLUE_P2,
};

// Smooth Skin Effect
enum{
	XT5_SMOOTHSKIN_EFFECT_OFF        = SDK_SMOOTHSKIN_EFFECT_OFF,
	XT5_SMOOTHSKIN_EFFECT_P1         = SDK_SMOOTHSKIN_EFFECT_P1,
	XT5_SMOOTHSKIN_EFFECT_P2         = SDK_SMOOTHSKIN_EFFECT_P2,
};

// SELF TIMER
enum{
	XT5_CAPTUREDELAY_10     = SDK_CAPTUREDELAY_10,
	XT5_CAPTUREDELAY_2      = SDK_CAPTUREDELAY_2,
	XT5_CAPTUREDELAY_OFF    = SDK_CAPTUREDELAY_OFF,
	XT5_SELFTIMER_10        = SDK_CAPTUREDELAY_10,
	XT5_SELFTIMER_2         = SDK_CAPTUREDELAY_2,
	XT5_SELFTIMER_OFF       = SDK_CAPTUREDELAY_OFF,
};

// FOCUS MODE
enum{
	XT5_FOCUSMODE_MANUAL = SDK_FOCUS_MANUAL,
	XT5_FOCUSMODE_AFS    = SDK_FOCUS_AFS,
	XT5_FOCUSMODE_AFC    = SDK_FOCUS_AFC,
	XT5_FOCUS_MANUAL     = SDK_FOCUS_MANUAL,
	XT5_FOCUS_AFS        = SDK_FOCUS_AFS,
	XT5_FOCUS_AFC        = SDK_FOCUS_AFC
};

// Focus Limiter Pos
enum{
	XT5_FOCUS_LIMITER_POS_A    = SDK_FOCUS_LIMITER_POS_A,
	XT5_FOCUS_LIMITER_POS_B    = SDK_FOCUS_LIMITER_POS_B,
};

// Focus Limiter Status
enum{
	XT5_FOCUS_LIMITER_STATUS_VALID    = SDK_FOCUS_LIMITER_STATUS_VALID,
	XT5_FOCUS_LIMITER_STATUS_INVALID  = SDK_FOCUS_LIMITER_STATUS_INVALID,
};

// Focus Limiter Mode
enum{
	XT5_FOCUS_LIMITER_OFF        = SDK_FOCUS_LIMITER_OFF,
	XT5_FOCUS_LIMITER_1          = SDK_FOCUS_LIMITER_1,
	XT5_FOCUS_LIMITER_2          = SDK_FOCUS_LIMITER_2,
	XT5_FOCUS_LIMITER_3          = SDK_FOCUS_LIMITER_3,
};

// Crop Zoom
enum{
	XT5_CROP_ZOOM_OFF            = SDK_CROP_ZOOM_OFF,
	XT5_CROP_ZOOM_10             = SDK_CROP_ZOOM_10,
	XT5_CROP_ZOOM_11             = SDK_CROP_ZOOM_11,
	XT5_CROP_ZOOM_12             = SDK_CROP_ZOOM_12,
	XT5_CROP_ZOOM_13             = SDK_CROP_ZOOM_13,
	XT5_CROP_ZOOM_14             = SDK_CROP_ZOOM_14,
	XT5_CROP_ZOOM_15             = SDK_CROP_ZOOM_15,
	XT5_CROP_ZOOM_16             = SDK_CROP_ZOOM_16,
	XT5_CROP_ZOOM_17             = SDK_CROP_ZOOM_17,
	XT5_CROP_ZOOM_18             = SDK_CROP_ZOOM_18,
	XT5_CROP_ZOOM_19             = SDK_CROP_ZOOM_19,
	XT5_CROP_ZOOM_20             = SDK_CROP_ZOOM_20,
};

// Zoom Speed
enum{
	XT5_LENS_ZOOM_SPEED_1        = SDK_LENS_ZOOM_SPEED_1,
	XT5_LENS_ZOOM_SPEED_2        = SDK_LENS_ZOOM_SPEED_2,
	XT5_LENS_ZOOM_SPEED_3        = SDK_LENS_ZOOM_SPEED_3,
	XT5_LENS_ZOOM_SPEED_4        = SDK_LENS_ZOOM_SPEED_4,
	XT5_LENS_ZOOM_SPEED_5        = SDK_LENS_ZOOM_SPEED_5,
	XT5_LENS_ZOOM_SPEED_6        = SDK_LENS_ZOOM_SPEED_6,
	XT5_LENS_ZOOM_SPEED_7        = SDK_LENS_ZOOM_SPEED_7,
	XT5_LENS_ZOOM_SPEED_8        = SDK_LENS_ZOOM_SPEED_8,
};

// Zoom Operation
enum{
	XT5_ZOOM_OPERATION_START     = SDK_ZOOM_OPERATION_START,
	XT5_ZOOM_OPERATION_STOP      = SDK_ZOOM_OPERATION_STOP,
	XT5_ZOOM_DIRECTION_WIDE      = SDK_ZOOM_DIRECTION_WIDE,
	XT5_ZOOM_DIRECTION_TELE      = SDK_ZOOM_DIRECTION_TELE,
};

// Focus Speed
enum{
	XT5_LENS_FOCUS_SPEED_1       = SDK_LENS_FOCUS_SPEED_1,
	XT5_LENS_FOCUS_SPEED_2       = SDK_LENS_FOCUS_SPEED_2,
	XT5_LENS_FOCUS_SPEED_3       = SDK_LENS_FOCUS_SPEED_3,
	XT5_LENS_FOCUS_SPEED_4       = SDK_LENS_FOCUS_SPEED_4,
	XT5_LENS_FOCUS_SPEED_5       = SDK_LENS_FOCUS_SPEED_5,
	XT5_LENS_FOCUS_SPEED_6       = SDK_LENS_FOCUS_SPEED_6,
	XT5_LENS_FOCUS_SPEED_7       = SDK_LENS_FOCUS_SPEED_7,
	XT5_LENS_FOCUS_SPEED_8       = SDK_LENS_FOCUS_SPEED_8,
};

// Focus Operation
enum{
	XT5_FOCUS_OPERATION_START    = SDK_FOCUS_OPERATION_START,
	XT5_FOCUS_OPERATION_STOP     = SDK_FOCUS_OPERATION_STOP,
	XT5_FOCUS_DIRECTION_NEAR     = SDK_FOCUS_DIRECTION_NEAR,
	XT5_FOCUS_DIRECTION_FAR      = SDK_FOCUS_DIRECTION_FAR,
};

// Focus Points
enum{
	XT5_FOCUS_POINTS_13X7       = SDK_FOCUS_POINTS_13X7,
	XT5_FOCUS_POINTS_25X13      = SDK_FOCUS_POINTS_25X13,
	XT5_FOCUS_POINTS_13X9       = SDK_FOCUS_POINTS_13X9,
	XT5_FOCUS_POINTS_25X17      = SDK_FOCUS_POINTS_25X17,
};

// AF MODE
enum{
	XT5_AF_ALL     = SDK_AF_ALL,
	XT5_AF_MULTI   = SDK_AF_MULTI,
	XT5_AF_AREA    = SDK_AF_AREA,
	XT5_AF_SINGLE  = XT5_AF_AREA,
	XT5_AF_ZONE    = SDK_AF_ZONE,
	XT5_AF_WIDETRACKING = SDK_AF_WIDETRACKING,
};

// AF Status
enum{
	XT5_AF_STATUS_OPERATING		= SDK_AF_STATUS_OPERATING,
	XT5_AF_STATUS_SUCCESS		= SDK_AF_STATUS_SUCCESS,
	XT5_AF_STATUS_FAIL			= SDK_AF_STATUS_FAIL,
	XT5_AF_STATUS_NO_OPERATION	= SDK_AF_STATUS_NO_OPERATION,
};

// Eye AF Mode
enum{
	XT5_EYE_AF_OFF             = SDK_EYE_AF_OFF,
	XT5_EYE_AF_AUTO            = SDK_EYE_AF_AUTO,
	XT5_EYE_AF_RIGHT_PRIORITY  = SDK_EYE_AF_RIGHT_PRIORITY,
	XT5_EYE_AF_LEFT_PRIORITY   = SDK_EYE_AF_LEFT_PRIORITY,
};

// Face Frame Information
enum{
	XT5_FRAMEINFO_FACE         = SDK_FRAMEINFO_FACE,
	XT5_FRAMEINFO_EYE_RIGHT    = SDK_FRAMEINFO_EYE_RIGHT,
	XT5_FRAMEINFO_EYE_LEFT     = SDK_FRAMEINFO_EYE_LEFT,
	XT5_FACEFRAMEINFO_NON      = SDK_FACEFRAMEINFO_NON,
	XT5_FACEFRAMEINFO_AUTO     = SDK_FACEFRAMEINFO_AUTO,
	XT5_FACEFRAMEINFO_MANUAL   = SDK_FACEFRAMEINFO_MANUAL,
};

// MF Assist Mode
enum{
	XT5_MF_ASSIST_STANDARD      = SDK_MF_ASSIST_STANDARD,
	XT5_MF_ASSIST_SPLIT_BW      = SDK_MF_ASSIST_SPLIT_BW,
	XT5_MF_ASSIST_SPLIT_COLOR   = SDK_MF_ASSIST_SPLIT_COLOR,
	XT5_MF_ASSIST_PEAK_WHITE_L  = SDK_MF_ASSIST_PEAK_WHITE_L,
	XT5_MF_ASSIST_PEAK_WHITE_H  = SDK_MF_ASSIST_PEAK_WHITE_H,
	XT5_MF_ASSIST_PEAK_RED_L    = SDK_MF_ASSIST_PEAK_RED_L,
	XT5_MF_ASSIST_PEAK_RED_H    = SDK_MF_ASSIST_PEAK_RED_H,
	XT5_MF_ASSIST_PEAK_BLUE_L   = SDK_MF_ASSIST_PEAK_BLUE_L,
	XT5_MF_ASSIST_PEAK_BLUE_H   = SDK_MF_ASSIST_PEAK_BLUE_H,
	XT5_MF_ASSIST_PEAK_YELLOW_L = SDK_MF_ASSIST_PEAK_YELLOW_L,
	XT5_MF_ASSIST_PEAK_YELLOW_H = SDK_MF_ASSIST_PEAK_YELLOW_H,
	XT5_MF_ASSIST_MICROPRISM    = SDK_MF_ASSIST_MICROPRISM,
	XT5_MF_ASSIST_FOCUSMETER               = SDK_MF_ASSIST_FOCUSMETER,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_L,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H  = SDK_MF_ASSIST_FOCUSMETER_PEAK_WHITE_H,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_RED_L    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_L,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_RED_H    = SDK_MF_ASSIST_FOCUSMETER_PEAK_RED_H,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_L,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H   = SDK_MF_ASSIST_FOCUSMETER_PEAK_BLUE_H,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_L,
	XT5_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H = SDK_MF_ASSIST_FOCUSMETER_PEAK_YELLOW_H,
	XT5_MF_ASSIST_FOCUSMAP_BW              = SDK_MF_ASSIST_FOCUSMAP_BW,
	XT5_MF_ASSIST_FOCUSMAP_COLOR           = SDK_MF_ASSIST_FOCUSMAP_COLOR,
};

// FOCUS AREA
enum{
	XT5_FOCUSAREA_H_MIN        = SDK_FOCUSAREA_H_MIN,
	XT5_FOCUSAREA_H_MAX        = SDK_FOCUSAREA_H_MAX,
	XT5_FOCUSAREA_V_MIN        = SDK_FOCUSAREA_V_MIN,
	XT5_FOCUSAREA_V_MAX        = SDK_FOCUSAREA_V_MAX,
	XT5_FOCUSAREA_SIZE_MIN     = SDK_FOCUSAREA_SIZE_MIN,
	XT5_FOCUSAREA_SIZE_MAX     = SDK_FOCUSAREA_SIZE_MAX,
};

// FACE DETECTION
enum{
	XT5_FACE_DETECTION_ON       = SDK_FACE_DETECTION_ON,
	XT5_FACE_DETECTION_OFF      = SDK_FACE_DETECTION_OFF,
};

// MACRO
enum{
	XT5_MACRO_MODE_OFF       = SDK_MACRO_MODE_OFF,
	XT5_MACRO_MODE           = SDK_MACRO_MODE,
	XT5_MACRO_MODE_ON        = SDK_MACRO_MODE_ON
};

// DRIVE MODE
enum{
	XT5_DRIVE_MODE_S    = SDK_DRIVE_MODE_S,
	XT5_DRIVE_MODE_CL   = 0x1000,
	XT5_DRIVE_MODE_CH   = 0x10F0,
	XT5_DRIVE_MODE_BKT  = 0x4000,
	XT5_DRIVE_MODE_MOVIE= SDK_DRIVE_MODE_MOVIE,
};

// USB MODE
enum{
	XT5_USB_PCSHOOTAUTO    = SDK_USB_PCSHOOTAUTO,
	XT5_USB_PCSHOOT        = SDK_USB_PCSHOOT,
};

// FILE FRAME NUMBERING
enum{
	XT5_FRAMENUMBERSEQUENCE_ON     = SDK_FRAMENUMBERSEQUENCE_ON,
	XT5_FRAMENUMBERSEQUENCE_OFF    = SDK_FRAMENUMBERSEQUENCE_OFF,
};

// OPERATION SOUND
enum{
	XT5_BEEP_HIGH        = SDK_BEEP_HIGH,
	XT5_BEEP_MID         = SDK_BEEP_MID,
	XT5_BEEP_LOW         = SDK_BEEP_LOW,
	XT5_BEEP_OFF         = SDK_BEEP_OFF,
};

// PREVIEW TIME
enum{
	XT5_PREVIEWTIME_CONTINUOUS  = SDK_PREVIEWTIME_CONTINUOUS,
	XT5_PREVIEWTIME_1P5SEC      = SDK_PREVIEWTIME_1P5SEC,
	XT5_PREVIEWTIME_0P5SEC      = SDK_PREVIEWTIME_0P5SEC,
	XT5_PREVIEWTIME_OFF         = SDK_PREVIEWTIME_OFF,
};

// VIEW MODE
enum{
	XT5_VIEW_MODE_EYE         = SDK_VIEW_MODE_EYE,
	XT5_VIEW_MODE_EVF         = SDK_VIEW_MODE_EVF,
	XT5_VIEW_MODE_LCD         = SDK_VIEW_MODE_LCD,
	XT5_VIEW_MODE_EVF_EYE     = SDK_VIEW_MODE_EVF_EYE,
	XT5_VIEW_MODE_LCDPOSTVIEW = SDK_VIEW_MODE_LCDPOSTVIEW,
	XT5_VIEW_MODE_OVF         = SDK_VIEW_MODE_OVF,
	XT5_VIEW_MODE_ERF         = SDK_VIEW_MODE_ERF,
	XT5_VIEW_MODE_EYESENSOR_ON = SDK_VIEW_MODE_EYESENSOR_ON,
	XT5_VIEW_MODE_EYESENSOR_OFF= SDK_VIEW_MODE_EYESENSOR_OFF,
};

// LCD DISP MODE
enum{
	XT5_LCD_DISPINFO_MODE_INFO      = SDK_LCD_DISPINFO_MODE_INFO,
	XT5_LCD_DISPINFO_MODE_STD       = SDK_LCD_DISPINFO_MODE_STD,
	XT5_LCD_DISPINFO_MODE_OFF       = SDK_LCD_DISPINFO_MODE_OFF,
	XT5_LCD_DISPINFO_MODE_CUSTOM    = SDK_LCD_DISPINFO_MODE_CUSTOM,
	XT5_LCD_DISPINFO_MODE_DUAL      = SDK_LCD_DISPINFO_MODE_DUAL,
};

// EVF DISP MODE
enum{
	XT5_EVF_DISPINFO_MODE_FULL_CUSTOM     = SDK_EVF_DISPINFO_MODE_FULL_CUSTOM,
	XT5_EVF_DISPINFO_MODE_NORMAL_CUSTOM   = SDK_EVF_DISPINFO_MODE_NORMAL_CUSTOM,
	XT5_EVF_DISPINFO_MODE_DUAL            = SDK_EVF_DISPINFO_MODE_DUAL,
	XT5_EVF_DISPINFO_MODE_FULL_OFF        = SDK_EVF_DISPINFO_MODE_FULL_OFF,
	XT5_EVF_DISPINFO_MODE_NORMAL_OFF      = SDK_EVF_DISPINFO_MODE_NORMAL_OFF,
};



enum{
	XT5_DATE_FORMAT_YMD	     = SDK_DATE_FORMAT_YMD,
	XT5_DATE_FORMAT_DMY      = SDK_DATE_FORMAT_DMY,
	XT5_DATE_FORMAT_MDY      = SDK_DATE_FORMAT_MDY,
};

enum{
	XT5_DISP_MMODE_MANUAL = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT5_DISP_MMODE_AE     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT5_DISP_MMODE_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XT5_DISP_MMODE_EXPWB  = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT5_DISP_MMODE_WB     = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT5_DISP_MMODE_OFF    = SDK_EXPOSURE_PREVIEW_AE_AWB,
	XT5_EXPOSURE_PREVIEW_ME_MWB = SDK_EXPOSURE_PREVIEW_ME_MWB,
	XT5_EXPOSURE_PREVIEW_AE_MWB = SDK_EXPOSURE_PREVIEW_AE_MWB,
	XT5_EXPOSURE_PREVIEW_AE_AWB = SDK_EXPOSURE_PREVIEW_AE_AWB,
};

enum{
	XT5_FRAMEGUIDE_GRID_9       = SDK_FRAMEGUIDE_GRID_9,
	XT5_FRAMEGUIDE_GRID_24      = SDK_FRAMEGUIDE_GRID_24,
	XT5_FRAMEGUIDE_GRID_HD      = 0x0003,
	XT5_FRAMEGUIDE_GRID_CUSTOM  = 0x0004, 
};

enum{
	XT5_SCALEUNIT_M     = SDK_SCALEUNIT_M,
	XT5_SCALEUNIT_FT    = SDK_SCALEUNIT_FT,
};

enum {
	XT5_POWERCAPACITY_EMPTY   = SDK_POWERCAPACITY_EMPTY,
	XT5_POWERCAPACITY_END     = SDK_POWERCAPACITY_END,
	XT5_POWERCAPACITY_PREEND  = SDK_POWERCAPACITY_PREEND,
	XT5_POWERCAPACITY_HALF    = SDK_POWERCAPACITY_HALF,
	XT5_POWERCAPACITY_FULL    = SDK_POWERCAPACITY_FULL,
	XT5_POWERCAPACITY_HIGH    = SDK_POWERCAPACITY_HIGH,
	XT5_POWERCAPACITY_PREEND5 = SDK_POWERCAPACITY_PREEND5,
	XT5_POWERCAPACITY_20      = SDK_POWERCAPACITY_20,
	XT5_POWERCAPACITY_40      = SDK_POWERCAPACITY_40,
	XT5_POWERCAPACITY_60      = SDK_POWERCAPACITY_60,
	XT5_POWERCAPACITY_80      = SDK_POWERCAPACITY_80,
	XT5_POWERCAPACITY_100     = SDK_POWERCAPACITY_100,
	XT5_POWERCAPACITY_DC      = SDK_POWERCAPACITY_DC,
	XT5_POWERCAPACITY_DC_CHARGE      = SDK_POWERCAPACITY_DC_CHARGE,
};

enum{
	XT5_AFPRIORITY_RELEASE       = SDK_AFPRIORITY_RELEASE,
	XT5_AFPRIORITY_FOCUS         = SDK_AFPRIORITY_FOCUS,
	XT5_INSTANT_AF_MODE_AFS        = SDK_INSTANT_AF_MODE_AFS,
	XT5_INSTANT_AF_MODE_AFC        = SDK_INSTANT_AF_MODE_AFC,
	XT5_LOCKBUTTON_MODE_PRESSING     = SDK_LOCKBUTTON_MODE_PRESSING,
	XT5_LOCKBUTTON_MODE_SWITCH       = SDK_LOCKBUTTON_MODE_SWITCH,
	XT5_AFLOCK_MODE_AF              = SDK_AFLOCK_MODE_AF,
	XT5_AFLOCK_MODE_AEAF            = SDK_AFLOCK_MODE_AEAF,
	XT5_MICJACK_MODE_MIC             = SDK_MICJACK_MODE_MIC,
	XT5_MICJACK_MODE_REMOTE          = SDK_MICJACK_MODE_REMOTE,
	XT5_AEAFLKEY_AE_AF               = SDK_AEAFLKEY_AE_AF,
	XT5_AEAFLKEY_AF_AE               = SDK_AEAFLKEY_AF_AE,
	XT5_CROSSKEY_FOCUSAREA           = SDK_CROSSKEY_FOCUSAREA,
	XT5_CROSSKEY_FUNCTION            = SDK_CROSSKEY_FUNCTION,
	XT5_IS_MODE_CONTINUOUS           = SDK_IS_MODE_CONTINUOUS,
	XT5_IS_MODE_SHOOT                = SDK_IS_MODE_SHOOT,
	XT5_IS_MODE_OFF                  = SDK_IS_MODE_OFF,
	XT5_IS_MODE_S1_SHOOT             = SDK_IS_MODE_S1_SHOOT,
	XT5_TIMEDIFF_HOME                = SDK_TIMEDIFF_HOME,
	XT5_TIMEDIFF_LOCAL               = SDK_TIMEDIFF_LOCAL,
	XT5_LANGUAGE_JA                  = SDK_LANGUAGE_JA,
	XT5_LCDBRIGHTNESS_MIN           = SDK_LCDBRIGHTNESS_MIN,
	XT5_LCDBRIGHTNESS_MAX           = SDK_LCDBRIGHTNESS_MAX,
	XT5_EVFBRIGHTNESS_MIN           = SDK_EVFBRIGHTNESS_MIN,
	XT5_EVFBRIGHTNESS_MAX           = SDK_EVFBRIGHTNESS_MAX,
	XT5_COLORINDEX_BLACK             = SDK_COLORINDEX_BLACK,
	XT5_COLORINDEX_BLUE              = SDK_COLORINDEX_BLUE,
	XT5_COLORINDEX_GREEN             = SDK_COLORINDEX_GREEN,
	XT5_COLORINDEX_CYAN              = SDK_COLORINDEX_CYAN,
	XT5_COLORINDEX_RED               = SDK_COLORINDEX_RED,
	XT5_COLORINDEX_MAGENTA           = SDK_COLORINDEX_MAGENTA,
	XT5_COLORINDEX_YELLOW            = SDK_COLORINDEX_YELLOW,
	XT5_COLORINDEX_WHITE             = SDK_COLORINDEX_WHITE,
	XT5_MEDIAREC_RAWJPEG           = SDK_MEDIAREC_RAWJPEG,
	XT5_MEDIAREC_RAW               = SDK_MEDIAREC_RAW,
	XT5_MEDIAREC_JPEG              = SDK_MEDIAREC_JPEG,
	XT5_MEDIAREC_OFF               = SDK_MEDIAREC_OFF,
	XT5_SENSORCLEANING_NONE          = SDK_SENSORCLEANING_NONE,
	XT5_SENSORCLEANING_POWERON       = SDK_SENSORCLEANING_POWERON,
	XT5_SENSORCLEANING_POWEROFF      = SDK_SENSORCLEANING_POWEROFF,
	XT5_SENSORCLEANING_POWERONOFF    = SDK_SENSORCLEANING_POWERONOFF,
	XT5_FUNCTION_DRV                 = SDK_FUNCTION_DRV,
	XT5_FUNCTION_MACRO               = SDK_FUNCTION_MACRO,
	XT5_FUNCTION_DEPTHPREVIEW        = SDK_FUNCTION_DEPTHPREVIEW,
	XT5_FUNCTION_ISOAUTOSETTING      = SDK_FUNCTION_ISOAUTOSETTING,
	XT5_FUNCTION_SELFTIMER           = SDK_FUNCTION_SELFTIMER,
	XT5_FUNCTION_IMAGESIZE           = SDK_FUNCTION_IMAGESIZE,
	XT5_FUNCTION_IMAGEQUALITY        = SDK_FUNCTION_IMAGEQUALITY,
	XT5_FUNCTION_DRANGE              = SDK_FUNCTION_DRANGE,
	XT5_FUNCTION_FILMSIMULATION      = SDK_FUNCTION_FILMSIMULATION,
	XT5_FUNCTION_WB                  = SDK_FUNCTION_WB,
	XT5_FUNCTION_AFMODE              = SDK_FUNCTION_AFMODE,
	XT5_FUNCTION_FOCUSAREA           = SDK_FUNCTION_FOCUSAREA,
	XT5_FUNCTION_CUSTOMSETTING       = SDK_FUNCTION_CUSTOMSETTING,
	XT5_FUNCTION_FACEDETECT          = SDK_FUNCTION_FACEDETECT,
	XT5_FUNCTION_RAW                 = SDK_FUNCTION_RAW,
	XT5_FUNCTION_APERTURE            = SDK_FUNCTION_APERTURE,
	XT5_FUNCTION_WIRELESS            = SDK_FUNCTION_WIRELESS,
	XT5_FUNCTION_EXPOSURE_PREVIEW    = SDK_FUNCTION_EXPOSURE_PREVIEW,
	XT5_CUSTOMDISPINFO_FRAMEGUIDE                    = SDK_CUSTOMDISPINFO_FRAMEGUIDE,
	XT5_CUSTOMDISPINFO_ELECTRONLEVEL                 = SDK_CUSTOMDISPINFO_ELECTRONLEVEL,
	XT5_CUSTOMDISPINFO_AFDISTANCE                    = SDK_CUSTOMDISPINFO_AFDISTANCE,
	XT5_CUSTOMDISPINFO_MFDISTANCE                    = SDK_CUSTOMDISPINFO_MFDISTANCE,
	XT5_CUSTOMDISPINFO_HISTOGRAM                     = SDK_CUSTOMDISPINFO_HISTOGRAM,
	XT5_CUSTOMDISPINFO_EXPOSUREPARAM                 = SDK_CUSTOMDISPINFO_EXPOSUREPARAM,
	XT5_CUSTOMDISPINFO_EXPOSUREBIAS                  = SDK_CUSTOMDISPINFO_EXPOSUREBIAS,
	XT5_CUSTOMDISPINFO_PHOTOMETRY                    = SDK_CUSTOMDISPINFO_PHOTOMETRY,
	XT5_CUSTOMDISPINFO_FLASH                         = SDK_CUSTOMDISPINFO_FLASH,
	XT5_CUSTOMDISPINFO_WB                            = SDK_CUSTOMDISPINFO_WB,
	XT5_CUSTOMDISPINFO_FILMSIMULATION                = SDK_CUSTOMDISPINFO_FILMSIMULATION,
	XT5_CUSTOMDISPINFO_DRANGE                        = SDK_CUSTOMDISPINFO_DRANGE,
	XT5_CUSTOMDISPINFO_FRAMESREMAIN                  = SDK_CUSTOMDISPINFO_FRAMESREMAIN,
	XT5_CUSTOMDISPINFO_IMAGESIZEQUALITY              = SDK_CUSTOMDISPINFO_IMAGESIZEQUALITY,
	XT5_CUSTOMDISPINFO_BATTERY                       = SDK_CUSTOMDISPINFO_BATTERY,
	XT5_CUSTOMDISPINFO_FOCUSFRAME                    = SDK_CUSTOMDISPINFO_FOCUSFRAME,
	XT5_CUSTOMDISPINFO_SHOOTINGMODE                  = SDK_CUSTOMDISPINFO_SHOOTINGMODE,
	XT5_CUSTOMDISPINFO_INFORMATIONBACKGROUND         = SDK_CUSTOMDISPINFO_INFORMATIONBACKGROUND,
	XT5_CUSTOMDISPINFO_FOCUSMODE                     = SDK_CUSTOMDISPINFO_FOCUSMODE,
	XT5_CUSTOMDISPINFO_SHUTTERTYPE                   = SDK_CUSTOMDISPINFO_SHUTTERTYPE,
	XT5_CUSTOMDISPINFO_CONTINUOUSMODE                = SDK_CUSTOMDISPINFO_CONTINUOUSMODE,
	XT5_CUSTOMDISPINFO_DUALISMODE                    = SDK_CUSTOMDISPINFO_DUALISMODE,
	XT5_CUSTOMDISPINFO_MOVIEMODE                     = SDK_CUSTOMDISPINFO_MOVIEMODE,
	XT5_CUSTOMDISPINFO_BLURWARNING                   = SDK_CUSTOMDISPINFO_BLURWARNING,
	XT5_CUSTOMDISPINFO_LIVEVIEWHIGHT                 = SDK_CUSTOMDISPINFO_LIVEVIEWHIGHT,
	XT5_CUSTOMDISPINFO_EXPOSUREBIASDIGIT             = SDK_CUSTOMDISPINFO_EXPOSUREBIASDIGIT,
	XT5_CUSTOMDISPINFO_TOUCHSCREENMODE               = SDK_CUSTOMDISPINFO_TOUCHSCREENMODE,
	XT5_CUSTOMDISPINFO_BOOSTMODE                     = SDK_CUSTOMDISPINFO_BOOSTMODE,
	XT5_CUSTOMDISPINFO_IMAGETRANSFERORDER            = SDK_CUSTOMDISPINFO_IMAGETRANSFERORDER,
	XT5_CUSTOMDISPINFO_MICLEVEL                      = SDK_CUSTOMDISPINFO_MICLEVEL,
	XT5_CUSTOMDISPINFO_GUIDANCEMESSAGE               = SDK_CUSTOMDISPINFO_GUIDANCEMESSAGE,
	XT5_CUSTOMDISPINFO_FRAMEOUTLINE                  = SDK_CUSTOMDISPINFO_FRAMEOUTLINE,
	XT5_CUSTOMDISPINFO_35MMFORMAT                    = SDK_CUSTOMDISPINFO_35MMFORMAT,
	XT5_CUSTOMDISPINFO_COOLINGFANSETTING             = SDK_CUSTOMDISPINFO_COOLINGFANSETTING,
	XT5_CUSTOMDISPINFO_DIGITALTELECONV               = SDK_CUSTOMDISPINFO_DIGITALTELECONV,
	XT5_CUSTOMDISPINFO_DIGITALZOOM                   = SDK_CUSTOMDISPINFO_DIGITALZOOM,
	XT5_CUSTOMDISPINFO_FOCUSINDICATOR                = SDK_CUSTOMDISPINFO_FOCUSINDICATOR,
	XT5_CUSTOMDISPINFO_NOCARDWARNING                 = SDK_CUSTOMDISPINFO_NOCARDWARNING,
	XT5_CUSTOMDISPINFO_DATETIME                      = SDK_CUSTOMDISPINFO_DATETIME,
	XT5_FUNCTIONLOCK_UNLOCK                          = SDK_FUNCTIONLOCK_FREE,
	XT5_FUNCTIONLOCK_ALL                             = SDK_FUNCTIONLOCK_ALL,
	XT5_FUNCTIONLOCK_CATEGORY                        = SDK_FUNCTIONLOCK_CATEGORY,
	XT5_FUNCTIONLOCK_CATEGORY1_FOCUSMODE            = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSMODE,
	XT5_FUNCTIONLOCK_CATEGORY1_APERTURE             = SDK_FUNCTIONLOCK_CATEGORY1_APERTURE,
	XT5_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED         = SDK_FUNCTIONLOCK_CATEGORY1_SHUTTERSPEED,
	XT5_FUNCTIONLOCK_CATEGORY1_ISO                  = SDK_FUNCTIONLOCK_CATEGORY1_ISO,
	XT5_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS         = SDK_FUNCTIONLOCK_CATEGORY1_EXPOSUREBIAS,
	XT5_FUNCTIONLOCK_CATEGORY1_DRV                  = SDK_FUNCTIONLOCK_CATEGORY1_DRV,
	XT5_FUNCTIONLOCK_CATEGORY1_AEMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AEMODE,
	XT5_FUNCTIONLOCK_CATEGORY1_QBUTTON              = SDK_FUNCTIONLOCK_CATEGORY1_QBUTTON,
	XT5_FUNCTIONLOCK_CATEGORY1_ISSWITCH             = SDK_FUNCTIONLOCK_CATEGORY1_ISSWITCH,
	XT5_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT         = SDK_FUNCTIONLOCK_CATEGORY1_PROGRAMSHIFT,
	XT5_FUNCTIONLOCK_CATEGORY1_VIEWMODE             = SDK_FUNCTIONLOCK_CATEGORY1_VIEWMODE,
	XT5_FUNCTIONLOCK_CATEGORY1_DISPBACK             = SDK_FUNCTIONLOCK_CATEGORY1_DISPBACK,
	XT5_FUNCTIONLOCK_CATEGORY1_AELOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AELOCK,
	XT5_FUNCTIONLOCK_CATEGORY1_AFLOCK               = SDK_FUNCTIONLOCK_CATEGORY1_AFLOCK,
	XT5_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST          = SDK_FUNCTIONLOCK_CATEGORY1_FOCUSASSIST,
	XT5_FUNCTIONLOCK_CATEGORY1_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY1_MOVIEREC,
	XT5_FUNCTIONLOCK_CATEGORY1_UP                   = SDK_FUNCTIONLOCK_CATEGORY1_UP,
	XT5_FUNCTIONLOCK_CATEGORY1_RIGHT                = SDK_FUNCTIONLOCK_CATEGORY1_RIGHT,
	XT5_FUNCTIONLOCK_CATEGORY1_LEFT                 = SDK_FUNCTIONLOCK_CATEGORY1_LEFT,
	XT5_FUNCTIONLOCK_CATEGORY1_DOWN                 = SDK_FUNCTIONLOCK_CATEGORY1_DOWN,
	XT5_FUNCTIONLOCK_CATEGORY1_FN1                  = SDK_FUNCTIONLOCK_CATEGORY1_FN1,
	XT5_FUNCTIONLOCK_CATEGORY1_FN2                  = SDK_FUNCTIONLOCK_CATEGORY1_FN2,
	XT5_FUNCTIONLOCK_CATEGORY1_AFMODE               = SDK_FUNCTIONLOCK_CATEGORY1_AFMODE,
	XT5_FUNCTIONLOCK_CATEGORY1_FACEDETECT           = SDK_FUNCTIONLOCK_CATEGORY1_FACEDETECT,
	XT5_FUNCTIONLOCK_CATEGORY1_OTHERSHOOTINGMENU    = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XT5_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU         = SDK_FUNCTIONLOCK_CATEGORY1_SHOOTINGMENU,
	XT5_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT          = SDK_FUNCTIONLOCK_CATEGORY1_MEDIAFORMAT,
	XT5_FUNCTIONLOCK_CATEGORY1_ERASE                = SDK_FUNCTIONLOCK_CATEGORY1_ERASE,
	XT5_FUNCTIONLOCK_CATEGORY1_DATETIME             = SDK_FUNCTIONLOCK_CATEGORY1_DATETIME,
	XT5_FUNCTIONLOCK_CATEGORY1_RESET                = SDK_FUNCTIONLOCK_CATEGORY1_RESET,
	XT5_FUNCTIONLOCK_CATEGORY1_SILENTMODE           = SDK_FUNCTIONLOCK_CATEGORY1_SILENTMODE,
	XT5_FUNCTIONLOCK_CATEGORY1_SOUND                = SDK_FUNCTIONLOCK_CATEGORY1_SOUND,
	XT5_FUNCTIONLOCK_CATEGORY2_SCREENDISP           = SDK_FUNCTIONLOCK_CATEGORY2_SCREENDISP,
	XT5_FUNCTIONLOCK_CATEGORY2_MOVIEREC             = SDK_FUNCTIONLOCK_CATEGORY2_MOVIEREC,
	XT5_FUNCTIONLOCK_CATEGORY2_COLORSPACE           = SDK_FUNCTIONLOCK_CATEGORY2_COLORSPACE,
	XT5_FUNCTIONLOCK_CATEGORY2_OTHERSETUP           = SDK_FUNCTIONLOCK_CATEGORY2_OTHERSETUP,
	XT5_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE         = SDK_FUNCTIONLOCK_CATEGORY2_WHITEBALANCE,
	XT5_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION       = SDK_FUNCTIONLOCK_CATEGORY2_FILMSIMULATION,
	XT5_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK           = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSSTICK,
	XT5_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR   = SDK_FUNCTIONLOCK_CATEGORY2_FOCUSRANGESELECTOR,
	XT5_FUNCTIONLOCK_CATEGORY2_FN3                  = SDK_FUNCTIONLOCK_CATEGORY2_FN3,
	XT5_FUNCTIONLOCK_CATEGORY2_FN4                  = SDK_FUNCTIONLOCK_CATEGORY2_FN4,
	XT5_FUNCTIONLOCK_CATEGORY2_FN5                  = SDK_FUNCTIONLOCK_CATEGORY2_FN5,
	XT5_FUNCTIONLOCK_CATEGORY2_FN10                 = SDK_FUNCTIONLOCK_CATEGORY2_FN10,
	XT5_FUNCTIONLOCK_CATEGORY2_RDIAL                = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL,
	XT5_FUNCTIONLOCK_CATEGORY2_AFON                 = SDK_FUNCTIONLOCK_CATEGORY2_AFON,
	XT5_FUNCTIONLOCK_CATEGORY2_TOUCHMODE            = SDK_FUNCTIONLOCK_CATEGORY2_TOUCHMODE,
	XT5_FUNCTIONLOCK_CATEGORY2_TFN1                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN1,
	XT5_FUNCTIONLOCK_CATEGORY2_TFN2                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN2,
	XT5_FUNCTIONLOCK_CATEGORY2_TFN3                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN3,
	XT5_FUNCTIONLOCK_CATEGORY2_TFN4                 = SDK_FUNCTIONLOCK_CATEGORY2_TFN4,
	XT5_FUNCTIONLOCK_CATEGORY2_SUBDISP              = SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP,
	XT5_FUNCTIONLOCK_CATEGORY2_AELOCK_V             = SDK_FUNCTIONLOCK_CATEGORY2_AELOCK_V,
	XT5_FUNCTIONLOCK_CATEGORY2_AFON_V               = SDK_FUNCTIONLOCK_CATEGORY2_AFON_V,
	XT5_FUNCTIONLOCK_CATEGORY2_FN1_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN1_V,
	XT5_FUNCTIONLOCK_CATEGORY2_FN2_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN2_V,
	XT5_FUNCTIONLOCK_CATEGORY2_FN3_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN3_V,
	XT5_FUNCTIONLOCK_CATEGORY2_FN4_V                = SDK_FUNCTIONLOCK_CATEGORY2_FN4_V,
	XT5_FUNCTIONLOCK_CATEGORY2_RDIAL_V              = SDK_FUNCTIONLOCK_CATEGORY2_RDIAL_V,
	XT5_FUNCTIONLOCK_CATEGORY2_LEVER                = SDK_FUNCTIONLOCK_CATEGORY2_LEVER,
	XT5_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER  = SDK_FUNCTIONLOCK_CATEGORY2_IMAGESWITCHINGLEVER,
	XT5_FUNCTIONLOCK_CATEGORY2_MODEDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_MODEDIAL,
	XT5_FUNCTIONLOCK_CATEGORY2_FDIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FDIAL,
	XT5_FUNCTIONLOCK_CATEGORY2_FN_DIAL				= SDK_FUNCTIONLOCK_CATEGORY2_FN_DIAL,
	XT5_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT		= SDK_FUNCTIONLOCK_CATEGORY2_SUBDISP_LIGHT,
	XT5_FUNCTIONLOCK_CATEGORY3_ISOBUTTON            = SDK_FUNCTIONLOCK_CATEGORY3_ISOBUTTON,
	XT5_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE      = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_FOCUSMODE,
	XT5_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE         = SDK_FUNCTIONLOCK_CATEGORY3_MOVIE_AFMODE,
	XT5_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU      = SDK_FUNCTIONLOCK_CATEGORY3_OTHER_MOVIEMENU,
	XT5_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE         = SDK_FUNCTIONLOCK_CATEGORY3_EXPOSUREMODE,
	XT5_FUNCTIONLOCK_CATEGORY3_WBBUTTON             = SDK_FUNCTIONLOCK_CATEGORY3_WBBUTTON,
	XT5_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING     = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTHPAIRING,
	XT5_FUNCTIONLOCK_CATEGORY3_BLUETOOTH            = SDK_FUNCTIONLOCK_CATEGORY3_BLUETOOTH,
	XT5_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT        = SDK_FUNCTIONLOCK_CATEGORY3_SUBJECTDETECT,
	XT5_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING = SDK_FUNCTIONLOCK_CATEGORY3_OTHERCONNECTIONSETTING,
	XT5_FUNCTIONLOCK_CATEGORY3_FM1                  = SDK_FUNCTIONLOCK_CATEGORY3_FM1,
	XT5_FUNCTIONLOCK_CATEGORY3_FM2                  = SDK_FUNCTIONLOCK_CATEGORY3_FM2,
	XT5_FUNCTIONLOCK_CATEGORY3_FM3                  = SDK_FUNCTIONLOCK_CATEGORY3_FM3,
	XT5_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION = SDK_FUNCTIONLOCK_CATEGORY3_COMMUNICATIONSETSELECTION,
	XT5_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP      = SDK_FUNCTIONLOCK_CATEGORY3_INFORMATIONDISP,
	XT5_MEDIASIZE_1M                     = SDK_MEDIASIZE_1M,
	XT5_MEDIASIZE_2M                     = SDK_MEDIASIZE_2M,
	XT5_MEDIASIZE_4M                     = SDK_MEDIASIZE_4M,
	XT5_MEDIASIZE_8M                     = SDK_MEDIASIZE_8M,
	XT5_MEDIASIZE_16M                    = SDK_MEDIASIZE_16M,
	XT5_MEDIASIZE_32M                    = SDK_MEDIASIZE_32M,
	XT5_MEDIASIZE_64M                    = SDK_MEDIASIZE_64M,
	XT5_MEDIASIZE_128M                   = SDK_MEDIASIZE_128M,
	XT5_MEDIASIZE_256M                   = SDK_MEDIASIZE_256M,
	XT5_MEDIASIZE_512M                   = SDK_MEDIASIZE_512M,
	XT5_MEDIASIZE_1G                     = SDK_MEDIASIZE_1G,
	XT5_MEDIASIZE_2G                     = SDK_MEDIASIZE_2G,
	XT5_MEDIASIZE_4G                     = SDK_MEDIASIZE_4G,
	XT5_MEDIASIZE_8G                     = SDK_MEDIASIZE_8G,
	XT5_MEDIASIZE_16G                    = SDK_MEDIASIZE_16G,
	XT5_MEDIASIZE_32G                    = SDK_MEDIASIZE_32G,
	XT5_MEDIASIZE_32G_OVER               = SDK_MEDIASIZE_32G_OVER,
	XT5_MEDIASTATUS_OK                    = SDK_MEDIASTATUS_OK,
	XT5_MEDIASTATUS_WRITEPROTECTED        = SDK_MEDIASTATUS_WRITEPROTECTED,
	XT5_MEDIASTATUS_NOCARD                = SDK_MEDIASTATUS_NOCARD,
	XT5_MEDIASTATUS_UNFORMATTED           = SDK_MEDIASTATUS_UNFORMATTED,
	XT5_MEDIASTATUS_ERROR                 = SDK_MEDIASTATUS_ERROR,
	XT5_MEDIASTATUS_MAXNO                 = SDK_MEDIASTATUS_MAXNO,
	XT5_MEDIASTATUS_FULL                  = SDK_MEDIASTATUS_FULL,
	XT5_MEDIASTATUS_ACCESSING             = SDK_MEDIASTATUS_ACCESSING,
	XT5_MEDIASTATUS_INCOMPATIBLE          = SDK_MEDIASTATUS_INCOMPATIBLE,
};

enum{
	XT5_CROPMODE_OFF     = SDK_CROPMODE_OFF,
	XT5_CROPMODE_35MM    = SDK_CROPMODE_35MM,
	XT5_CROPMODE_AUTO    = SDK_CROPMODE_AUTO,
	XT5_CROPMODE_SPORTSFINDER_125 = SDK_CROPMODE_SPORTSFINDER_125,
};

enum{
	XT5_ON  = SDK_ON,
	XT5_OFF = SDK_OFF,
};

enum{
	XT5_ITEM_DIRECTION_0             = SDK_ITEM_DIRECTION_0,
	XT5_ITEM_DIRECTION_90            = SDK_ITEM_DIRECTION_90,
	XT5_ITEM_DIRECTION_180           = SDK_ITEM_DIRECTION_180,
	XT5_ITEM_DIRECTION_270           = SDK_ITEM_DIRECTION_270,
	XT5_ITEM_ISODIAL_H1              = SDK_ITEM_ISODIAL_H1,
	XT5_ITEM_ISODIAL_H2              = SDK_ITEM_ISODIAL_H2,
	XT5_ITEM_VIEWMODE_SHOOT          = SDK_ITEM_VIEWMODE_SHOOT,
	XT5_ITEM_VIEWMODE_PLAYBACK       = SDK_ITEM_VIEWMODE_PLAYBACK,
	XT5_ITEM_DISPINFO_LCD            = SDK_ITEM_DISPINFO_LCD,
	XT5_ITEM_DISPINFO_EVF            = SDK_ITEM_DISPINFO_EVF,
	XT5_ITEM_AFPRIORITY_AFS          = SDK_ITEM_AFPRIORITY_AFS,
	XT5_ITEM_AFPRIORITY_AFC          = SDK_ITEM_AFPRIORITY_AFC,
	XT5_ITEM_RESET_SHOOTMENU         = SDK_ITEM_RESET_SHOOTMENU,
	XT5_ITEM_RESET_SETUP             = SDK_ITEM_RESET_SETUP,
	XT5_ITEM_RESET_MOVIEMENU         = SDK_ITEM_RESET_MOVIEMENU,
	XT5_ITEM_BRIGHTNESS_LCD          = SDK_ITEM_BRIGHTNESS_LCD,
	XT5_ITEM_BRIGHTNESS_EVF          = SDK_ITEM_BRIGHTNESS_EVF,
	XT5_ITEM_CHROMA_LCD              = SDK_ITEM_CHROMA_LCD,
	XT5_ITEM_CHROMA_EVF              = SDK_ITEM_CHROMA_EVF,
	XT5_ITEM_FUNCBUTTON_FN1          = SDK_ITEM_FUNCBUTTON_FN1,
	XT5_ITEM_FUNCBUTTON_FN2          = SDK_ITEM_FUNCBUTTON_FN2,
	XT5_ITEM_FUNCBUTTON_FN3          = SDK_ITEM_FUNCBUTTON_FN3,
	XT5_ITEM_FUNCBUTTON_FN4          = SDK_ITEM_FUNCBUTTON_FN4,
	XT5_ITEM_FUNCBUTTON_FN5          = SDK_ITEM_FUNCBUTTON_FN5,
	XT5_ITEM_FUNCBUTTON_FN6          = SDK_ITEM_FUNCBUTTON_FN6,
	XT5_ITEM_FILENAME_sRGB           = SDK_ITEM_FILENAME_sRGB,
	XT5_ITEM_FILENAME_AdobeRGB       = SDK_ITEM_FILENAME_AdobeRGB,
	XT5_ITEM_MEDIASLOT1              = SDK_ITEM_MEDIASLOT1,
	XT5_ITEM_MEDIASLOT2              = SDK_ITEM_MEDIASLOT2,
	XT5_ITEM_DIRECTION_CURRENT       = SDK_ITEM_DIRECTION_CURRENT,
	XT5_ITEM_FOLDERNAME_NOCATEGORY   = SDK_ITEM_FOLDERNAME_NOCATEGORY,
	XT5_NEW_FOLDER                   = SDK_NEW_FOLDER,
	XT5_FOLDERNUMBER_NIL             = SDK_FOLDERNUMBER_NIL,
};

enum{
	XT5_WIDEDYNAMICRANGE_0           = SDK_WIDEDYNAMICRANGE_0,
	XT5_WIDEDYNAMICRANGE_P1          = SDK_WIDEDYNAMICRANGE_P1,
	XT5_WIDEDYNAMICRANGE_P2          = SDK_WIDEDYNAMICRANGE_P2,
	XT5_WIDEDYNAMICRANGE_P3          = SDK_WIDEDYNAMICRANGE_P3,
	XT5_WIDEDYNAMICRANGE_AUTO        = SDK_WIDEDYNAMICRANGE_AUTO,
};

enum{
	XT5_BLACKIMAGETONE_P90           = SDK_BLACKIMAGETONE_P90,
	XT5_BLACKIMAGETONE_P80           = SDK_BLACKIMAGETONE_P80,
	XT5_BLACKIMAGETONE_P70           = SDK_BLACKIMAGETONE_P70,
	XT5_BLACKIMAGETONE_P60           = SDK_BLACKIMAGETONE_P60,
	XT5_BLACKIMAGETONE_P50           = SDK_BLACKIMAGETONE_P50,
	XT5_BLACKIMAGETONE_P40           = SDK_BLACKIMAGETONE_P40,
	XT5_BLACKIMAGETONE_P30           = SDK_BLACKIMAGETONE_P30,
	XT5_BLACKIMAGETONE_P20           = SDK_BLACKIMAGETONE_P20,
	XT5_BLACKIMAGETONE_P10           = SDK_BLACKIMAGETONE_P10,
	XT5_BLACKIMAGETONE_0             = SDK_BLACKIMAGETONE_0,
	XT5_BLACKIMAGETONE_M10           = SDK_BLACKIMAGETONE_M10,
	XT5_BLACKIMAGETONE_M20           = SDK_BLACKIMAGETONE_M20,
	XT5_BLACKIMAGETONE_M30           = SDK_BLACKIMAGETONE_M30,
	XT5_BLACKIMAGETONE_M40           = SDK_BLACKIMAGETONE_M40,
	XT5_BLACKIMAGETONE_M50           = SDK_BLACKIMAGETONE_M50,
	XT5_BLACKIMAGETONE_M60           = SDK_BLACKIMAGETONE_M60,
	XT5_BLACKIMAGETONE_M70           = SDK_BLACKIMAGETONE_M70,
	XT5_BLACKIMAGETONE_M80           = SDK_BLACKIMAGETONE_M80,
	XT5_BLACKIMAGETONE_M90           = SDK_BLACKIMAGETONE_M90,
};

// Micline Setting
enum{
	XT5_MICLINE_SETTING_MIC          = SDK_MICLINE_SETTING_MIC,
	XT5_MICLINE_SETTING_LINE         = SDK_MICLINE_SETTING_LINE,
};

// Shuttercount Type
enum{
	XT5_SHUTTERCOUNT_TYPE_FRONTCURTAIN = SDK_SHUTTERCOUNT_TYPE_FRONTCURTAIN,
	XT5_SHUTTERCOUNT_TYPE_REARCURTAIN  = SDK_SHUTTERCOUNT_TYPE_REARCURTAIN,
	XT5_SHUTTERCOUNT_TYPE_TOTAL        = SDK_SHUTTERCOUNT_TYPE_TOTAL,
};

// Performance
enum{
	XT5_PERFORMANCE_NORMAL                    = SDK_PERFORMANCE_NORMAL,
	XT5_PERFORMANCE_ECONOMY                   = SDK_PERFORMANCE_ECONOMY,
	XT5_PERFORMANCE_BOOST_LOWLIGHT            = SDK_PERFORMANCE_BOOST_LOWLIGHT,
	XT5_PERFORMANCE_BOOST_RESOLUTION_PRIORITY = SDK_PERFORMANCE_BOOST_RESOLUTION_PRIORITY,
	XT5_PERFORMANCE_BOOST_FRAMERATE_PRIORITY  = SDK_PERFORMANCE_BOOST_FRAMERATE_PRIORITY,
	XT5_PERFORMANCE_BOOST_AFPRIORITY_NORMAL   = SDK_PERFORMANCE_BOOST_AFPRIORITY_NORMAL,
	XT5_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION= SDK_PERFORMANCE_BOOST_AFTERIMAGE_REDUCTION,
};

// PixelShift Settings
enum{
	XT5_PIXELSHIFT_INTERVAL_SHORTEST = SDK_PIXELSHIFT_INTERVAL_SHORTEST,
	XT5_PIXELSHIFT_INTERVAL_1S       = SDK_PIXELSHIFT_INTERVAL_1S,
	XT5_PIXELSHIFT_INTERVAL_2S       = SDK_PIXELSHIFT_INTERVAL_2S,
	XT5_PIXELSHIFT_INTERVAL_5S       = SDK_PIXELSHIFT_INTERVAL_5S,
	XT5_PIXELSHIFT_INTERVAL_15S      = SDK_PIXELSHIFT_INTERVAL_15S,
};

// SubjectDetectionMode
enum{
	XT5_SUBJECT_DETECTION_OFF        = SDK_SUBJECT_DETECTION_OFF,
	XT5_SUBJECT_DETECTION_ANIMAL     = SDK_SUBJECT_DETECTION_ANIMAL,
	XT5_SUBJECT_DETECTION_BIRD       = SDK_SUBJECT_DETECTION_BIRD,
	XT5_SUBJECT_DETECTION_CAR        = SDK_SUBJECT_DETECTION_CAR,
	XT5_SUBJECT_DETECTION_BIKE       = SDK_SUBJECT_DETECTION_BIKE,
	XT5_SUBJECT_DETECTION_AIRPLANE   = SDK_SUBJECT_DETECTION_AIRPLANE,
	XT5_SUBJECT_DETECTION_TRAIN      = SDK_SUBJECT_DETECTION_TRAIN,
};

// FanSetting
enum{
	XT5_FAN_SETTING_OFF              = SDK_FAN_SETTING_OFF,
	XT5_FAN_SETTING_WEAK             = SDK_FAN_SETTING_WEAK,
	XT5_FAN_SETTING_STRONG           = SDK_FAN_SETTING_STRONG,
	XT5_FAN_SETTING_AUTO1            = SDK_FAN_SETTING_AUTO1,
	XT5_FAN_SETTING_AUTO2            = SDK_FAN_SETTING_AUTO2,
};

// ElectronicLevelSetting
enum{
	XT5_ELECTRONIC_LEVEL_SETTING_OFF = SDK_ELECTRONIC_LEVEL_SETTING_OFF,
	XT5_ELECTRONIC_LEVEL_SETTING_2D  = SDK_ELECTRONIC_LEVEL_SETTING_2D,
	XT5_ELECTRONIC_LEVEL_SETTING_3D  = SDK_ELECTRONIC_LEVEL_SETTING_3D,
};

// ApertureUnit
enum{
	XT5_APERTURE_UNIT_TNUMBER        = SDK_APERTURE_UNIT_TNUMBER,
	XT5_APERTURE_UNIT_FNUMBER        = SDK_APERTURE_UNIT_FNUMBER,
};

// USBPowerSupplyCommunication
enum{
	XT5_USB_POWER_SUPPLY_COMMUNICATION_AUTO = SDK_USB_POWER_SUPPLY_COMMUNICATION_AUTO,
	XT5_USB_POWER_SUPPLY_COMMUNICATION_ON   = SDK_USB_POWER_SUPPLY_COMMUNICATION_ON,
	XT5_USB_POWER_SUPPLY_COMMUNICATION_OFF  = SDK_USB_POWER_SUPPLY_COMMUNICATION_OFF,
};

// AutoPowerOffSetting
enum{
	XT5_AUTOPOWEROFF_OFF           = SDK_AUTOPOWEROFF_OFF,
	XT5_AUTOPOWEROFF_15SEC         = SDK_AUTOPOWEROFF_15SEC,
	XT5_AUTOPOWEROFF_30SEC         = SDK_AUTOPOWEROFF_30SEC,
	XT5_AUTOPOWEROFF_1MIN          = SDK_AUTOPOWEROFF_1MIN,
	XT5_AUTOPOWEROFF_2MIN          = SDK_AUTOPOWEROFF_2MIN,
	XT5_AUTOPOWEROFF_5MIN          = SDK_AUTOPOWEROFF_5MIN,
};
