using System.Collections.Generic;

namespace ASCOM.ScdouglasFujifilm.Camera.Core
{
    internal sealed class CameraConfig
    {
        public string ModelName { get; set; }
        public int CameraXSize { get; set; }
        public int CameraYSize { get; set; }
        public double PixelSizeX { get; set; }
        public double PixelSizeY { get; set; }
        public int MaxAdu { get; set; }
        public int DefaultMinSensitivity { get; set; }
        public int DefaultMaxSensitivity { get; set; }
        public double DefaultMinExposure { get; set; }
        public double DefaultMaxExposure { get; set; }
        public bool DefaultBulbCapable { get; set; }
        public SdkConstantConfig SdkConstants { get; set; }
        public List<ShutterSpeedMapping> ShutterSpeedMap { get; set; }
    }

    internal sealed class SdkConstantConfig
    {
        public int ModeManual { get; set; }
        public int FocusModeManual { get; set; }
        public int ImageQualityRaw { get; set; }
        public int ImageQualityRawFine { get; set; }
        public int ImageQualityRawNormal { get; set; }
        public int ImageQualityRawSuperfine { get; set; }
    }

    internal sealed class ShutterSpeedMapping
    {
        public int SdkCode { get; set; }
        public double Duration { get; set; }
    }
}
