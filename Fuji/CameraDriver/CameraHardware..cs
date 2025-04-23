// ASCOM Camera hardware class for ScdouglasFujifilm
// Author: S. Douglas <your@email.here>
// Description: Interfaces with the Fujifilm X SDK to control Fujifilm cameras.
// Implements: ASCOM Camera interface version: 3
// Fix: Added explicit XSDK_RELEASE_S1ON call before XSDK_RELEASE_BULBS2_ON
//      when starting Bulb exposures, based on user example and common SDK patterns.
//      Ensured plShotOpt is allocated for all Release calls.
//      Uses XSDK_RELEASE_N_BULBS2OFF (0x0008) to stop Bulb.
//      Preserved the exact code structure provided by the user.

using ASCOM;
using ASCOM.Astrometry.AstroUtils;
using ASCOM.DeviceInterface;
using ASCOM.Utilities;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq; // Added for Max() extension method
using System.Runtime.InteropServices; // Needed for GCHandle, Marshal
using System.Threading;
using System.Windows.Forms;

// Remove direct using for NativeLibRaw as it's handled by the wrapper now
// using ASCOM.LocalServer.NativeLibRaw;

// Add using for the C++/CLI Wrapper namespace (adjust if you used a different namespace)
using Fujifilm.LibRawWrapper;

namespace ASCOM.ScdouglasFujifilm.Camera
{
    #region Configuration Classes
    // Configuration classes (SdkConstantConfig, ShutterSpeedMapping, CameraConfig)
    // remain exactly as in the uploaded CameraHardware..cs file.
    public class SdkConstantConfig
    {
        public int ModeManual { get; set; }
        public int FocusModeManual { get; set; }
        public int ImageQualityRaw { get; set; }
        public int ImageQualityRawFine { get; set; }
        public int ImageQualityRawNormal { get; set; }
        public int ImageQualityRawSuperfine { get; set; }
    }
    public class ShutterSpeedMapping
    {
        public int SdkCode { get; set; }
        public double Duration { get; set; }
    }
    public class CameraConfig
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
    #endregion

    /// <summary>
    /// Wraps the Fujifilm X SDK C-style DLL functions using P/Invoke.
    /// </summary>
    internal static class FujifilmSdkWrapper
    {
        // --- SDK Wrapper code remains the same as user provided file ---
        private const string SdkDllName = "XAPI.dll"; // Core Fuji SDK DLL

        #region SDK Structures (Matching XAPI.H)

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct XSDK_ImageInformation
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string strInternalName;
            public int lFormat;
            public int lDataSize;
            public int lImagePixHeight;
            public int lImagePixWidth;
            public int lImageBitDepth;
            public int lPreviewSize;
            public IntPtr hCamera; // XSDK_HANDLE
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct XSDK_DeviceInformation
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strVendor;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strManufacturer;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strProduct;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strFirmware;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strDeviceType;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strSerialNo;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string strFramework;
            public byte bDeviceId;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string strDeviceName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
            public string strYNo;
        }

        #endregion

        #region SDK Constants (Matching XAPI.H & XAPIOpt.h)

        // Results
        public const int XSDK_COMPLETE = 0;
        public const int XSDK_ERROR = -1;

        // Error Codes (Ensure these match XAPI.H hex values)
        public const int XSDK_ERRCODE_NOERR = 0x0000;
        public const int XSDK_ERRCODE_SEQUENCE = 0x1001;
        public const int XSDK_ERRCODE_PARAM = 0x1002; // Corrected
        public const int XSDK_ERRCODE_INVALID_CAMERA = 0x1003;
        public const int XSDK_ERRCODE_LOADLIB = 0x1004;
        public const int XSDK_ERRCODE_UNSUPPORTED = 0x1005;
        public const int XSDK_ERRCODE_BUSY = 0x1006;
        public const int XSDK_ERRCODE_AF_TIMEOUT = 0x1007;
        public const int XSDK_ERRCODE_SHOOT_ERROR = 0x1008;
        public const int XSDK_ERRCODE_FRAME_FULL = 0x1009;
        public const int XSDK_ERRCODE_STANDBY = 0x1010;
        public const int XSDK_ERRCODE_NODRIVER = 0x1011;
        public const int XSDK_ERRCODE_NO_MODEL_MODULE = 0x1012;
        public const int XSDK_ERRCODE_API_NOTFOUND = 0x1013;
        public const int XSDK_ERRCODE_API_MISMATCH = 0x1014;
        public const int XSDK_ERRCODE_INVALID_USBMODE = 0x1015;
        public const int XSDK_ERRCODE_FORCEMODE_BUSY = 0x1016;
        public const int XSDK_ERRCODE_RUNNING_OTHER_FUNCTION = 0x1017;
        public const int XSDK_ERRCODE_COMMUNICATION = 0x2001;
        public const int XSDK_ERRCODE_TIMEOUT = 0x2002;
        public const int XSDK_ERRCODE_COMBINATION = 0x2003;
        public const int XSDK_ERRCODE_WRITEERROR = 0x2004;
        public const int XSDK_ERRCODE_CARDFULL = 0x2005;
        public const int XSDK_ERRCODE_HARDWARE = 0x3001;
        public const int XSDK_ERRCODE_INTERNAL = 0x9001;
        public const int XSDK_ERRCODE_MEMFULL = 0x9002;
        public const int XSDK_ERRCODE_UNKNOWN = 0x9100;

        // Priority Modes
        public const int XSDK_PRIORITY_CAMERA = 0x0001;
        public const int XSDK_PRIORITY_PC = 0x0002;

        // Interfaces
        public const int XSDK_DSC_IF_USB = 1;

        // Exposure Modes (From XAPI.H & GFX100S.h)
        public const int GFX100S_MODE_M = 0x0001; // Manual Exposure Mode

        // Focus Modes (From XAPI.H & GFX100S.h)
        public const int GFX100S_FOCUSMODE_MANUAL = 0x0001; // Manual Focus Mode

        // Image Quality / Format (Examples - Ensure these match GFX100S.h if used)
        public const int GFX100S_IMAGEQUALITY_RAW = 0x0001; // Assume Format 1 is RAW
        public const int GFX100S_IMAGEQUALITY_FINE = 0x0002;
        public const int GFX100S_IMAGEQUALITY_NORMAL = 0x0003;
        public const int GFX100S_IMAGEQUALITY_RAW_FINE = 0x0102; // Matches XAPI.h 0x0004
        public const int GFX100S_IMAGEQUALITY_RAW_NORMAL = 0x0103; // Matches XAPI.h 0x0005
        public const int GFX100S_IMAGEQUALITY_SUPERFINE = 0x0004; // Matches XAPI.h 0x0006
        public const int GFX100S_IMAGEQUALITY_RAW_SUPERFINE = 0x0104; // Matches XAPI.h 0x0007

        // Release Modes (From XAPI.h & XAPIOpt.h)
        public const int XSDK_RELEASE_SHOOT = 0x0100; // Just shoot (S2?)
        public const int XSDK_RELEASE_S1ON = 0x0200;   // S1 Press Only (Added from XAPI.h)
        public const int XSDK_RELEASE_N_S1OFF = 0x0004; // Option flag
        public const int XSDK_RELEASE_SHOOT_S1OFF = (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_S1OFF); // 0x0104 = 260
        public const int SDK_RELEASE_MODE_S1ONLY = 0x0001; // S1 Press Only (Focus/AE Lock) - This is likely the same intent as XSDK_RELEASE_S1ON
        public const int SDK_RELEASE_MODE_S2ONLY = 0x0002; // S2 Press Only (Release without S1)
        public const int SDK_RELEASE_MODE_S1S2 = 0x0003;   // S1 + S2 Press

        // Bulb Release Modes (From XAPI.h)
        public const int XSDK_RELEASE_BULBS2_ON = 0x0500;  // Correct value from XAPI.h
        public const int XSDK_RELEASE_N_BULBS2OFF = 0x0008; // Correct value from XAPI.h

        // Shutter Speed
        public const int XSDK_SHUTTER_BULB = -1;

        #endregion

        #region Fujifilm SDK P/Invoke Signatures (Matching XAPI.H)

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_Init")]
        public static extern int XSDK_Init(IntPtr hLib);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_Exit")]
        public static extern int XSDK_Exit();

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_Detect")]
        public static extern int XSDK_Detect(int lInterface, IntPtr pInterface, IntPtr pDeviceName, out int plCount);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_OpenEx")]
        public static extern int XSDK_OpenEx([MarshalAs(UnmanagedType.LPStr)] string pDevice, out IntPtr phCamera, out int plCameraMode, IntPtr pOption);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_Close")]
        public static extern int XSDK_Close(IntPtr hCamera);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetErrorNumber")]
        public static extern int XSDK_GetErrorNumber(IntPtr hCamera, out int plAPICode, out int plERRCode);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetDeviceInfoEx")]
        public static extern int XSDK_GetDeviceInfoEx(IntPtr hCamera, out XSDK_DeviceInformation pDevInfo, out int plNumAPICode, IntPtr plAPICode);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_SetPriorityMode")]
        public static extern int XSDK_SetPriorityMode(IntPtr hCamera, int lPriorityMode);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetPriorityMode")]
        public static extern int XSDK_GetPriorityMode(IntPtr hCamera, out int plPriorityMode);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_SetMode")]
        public static extern int XSDK_SetMode(IntPtr hCamera, int lMode);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetMode")]
        public static extern int XSDK_GetMode(IntPtr hCamera, out int plMode);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_CapSensitivity")]
        public static extern int XSDK_CapSensitivity(IntPtr hCamera, int lDR, out int plNumSensitivity, IntPtr plSensitivity);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_SetSensitivity")]
        public static extern int XSDK_SetSensitivity(IntPtr hCamera, int lSensitivity);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetSensitivity")]
        public static extern int XSDK_GetSensitivity(IntPtr hCamera, out int plSensitivity);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_CapShutterSpeed")]
        public static extern int XSDK_CapShutterSpeed(IntPtr hCamera, out int plNumShutterSpeed, IntPtr plShutterSpeed, out int plBulbCapable);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_SetShutterSpeed")]
        public static extern int XSDK_SetShutterSpeed(IntPtr hCamera, int lShutterSpeed, int lBulb);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetShutterSpeed")]
        public static extern int XSDK_GetShutterSpeed(IntPtr hCamera, out int plShutterSpeed, out int plBulb);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_Release")]
        public static extern int XSDK_Release(IntPtr hCamera, int lReleaseMode, IntPtr plShotOpt, out int pStatus);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_ReadImageInfo")]
        public static extern int XSDK_ReadImageInfo(IntPtr hCamera, out XSDK_ImageInformation pImgInfo);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_ReadImage")]
        public static extern int XSDK_ReadImage(IntPtr hCamera, IntPtr pData, uint ulDataSize);

        [DllImport(SdkDllName, CallingConvention = CallingConvention.Cdecl, EntryPoint = "XSDK_GetBufferCapacity")]
        public static extern int XSDK_GetBufferCapacity(IntPtr hCamera, out int plShootFrameNum, out int plTotalFrameNum);

        #endregion

        #region Helper Methods
        // --- Helper methods remain the same as user provided file ---
        internal delegate int CapFunctionDelegate(IntPtr hCamera, out int count, IntPtr buffer);
        internal delegate int CapFunctionBulbDelegate(IntPtr hCamera, out int count, IntPtr buffer, out int bulbCapable);

        internal static int[] GetIntArrayFromSdk(IntPtr hCamera, CapFunctionDelegate capFunc)
        {
            int count = 0;
            // First call to get the count
            int result = capFunc(hCamera, out count, IntPtr.Zero);
            CheckSdkError(hCamera, result, "GetIntArrayFromSdk (GetCount)");
            if (count <= 0) return new int[0];

            IntPtr buffer = IntPtr.Zero;
            try
            {
                buffer = Marshal.AllocHGlobal(count * sizeof(int));
                // Second call to get the data
                result = capFunc(hCamera, out count, buffer); // Count might change, but buffer size is fixed
                CheckSdkError(hCamera, result, "GetIntArrayFromSdk (GetData)");

                int[] managedArray = new int[count];
                Marshal.Copy(buffer, managedArray, 0, count);
                return managedArray;
            }
            finally { if (buffer != IntPtr.Zero) Marshal.FreeHGlobal(buffer); }
        }

        internal static int[] GetIntArrayFromSdkShutterSpeed(IntPtr hCamera, out int bulbCapable)
        {
            int count = 0;
            bulbCapable = 0; // Default to false
                             // First call to get the count and bulb capability
            int result = FujifilmSdkWrapper.XSDK_CapShutterSpeed(hCamera, out count, IntPtr.Zero, out bulbCapable);
            CheckSdkError(hCamera, result, "GetIntArrayFromSdkShutterSpeed (GetCount)");
            if (count <= 0) return new int[0];

            IntPtr buffer = IntPtr.Zero;
            try
            {
                buffer = Marshal.AllocHGlobal(count * sizeof(int));
                // Second call to get the data (bulbCapable is already retrieved)
                result = FujifilmSdkWrapper.XSDK_CapShutterSpeed(hCamera, out count, buffer, out _); // Ignore bulbCapable on second call
                CheckSdkError(hCamera, result, "GetIntArrayFromSdkShutterSpeed (GetData)");

                int[] managedArray = new int[count];
                Marshal.Copy(buffer, managedArray, 0, count);
                return managedArray;
            }
            finally { if (buffer != IntPtr.Zero) Marshal.FreeHGlobal(buffer); }
        }

        // Modified: Returns empty array on failure instead of throwing
        internal static int[] GetIntArrayFromSdkSensitivity(IntPtr hCamera, int lDR)
        {
            int count = 0;
            LogMessageStatic("GetIntArrayFromSdkSensitivity", $"Calling XSDK_CapSensitivity(lDR={lDR}, GetCount)...");
            int result = XSDK_CapSensitivity(hCamera, lDR, out count, IntPtr.Zero);
            LogMessageStatic("GetIntArrayFromSdkSensitivity", $"XSDK_CapSensitivity returned count={count}, result={result}");
            CheckSdkError(hCamera, result, $"XSDK_CapSensitivity (lDR={lDR}, GetCount)");
            if (count <= 0) return new int[0];

            IntPtr ptr = Marshal.AllocHGlobal(sizeof(int) * count);
            try
            {
                LogMessageStatic("GetIntArrayFromSdkSensitivity", $"Calling XSDK_CapSensitivity(lDR={lDR}, GetData)...");
                result = XSDK_CapSensitivity(hCamera, lDR, out count, ptr);
                LogMessageStatic("GetIntArrayFromSdkSensitivity", $"XSDK_CapSensitivity returned result={result}");
                CheckSdkError(hCamera, result, $"XSDK_CapSensitivity (lDR={lDR}, GetData)");
                int[] array = new int[count];
                Marshal.Copy(ptr, array, 0, count);
                return array;
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }

        #endregion

        #region Error Handling Helper
        // --- Error handling remains the same as user provided file ---
        public static void CheckSdkError(IntPtr hCamera, int sdkResult, string operation)
        {
            if (sdkResult != XSDK_COMPLETE)
            {
                int apiCode = 0, errCode = 0;
                try
                {
                    if (hCamera != IntPtr.Zero)
                    {
                        XSDK_GetErrorNumber(hCamera, out apiCode, out errCode);
                    }
                    else
                    {
                        switch (sdkResult)
                        {
                            case XSDK_ERRCODE_COMMUNICATION:
                            case XSDK_ERRCODE_TIMEOUT:
                            case XSDK_ERRCODE_PARAM:
                                errCode = sdkResult; break;
                            default: errCode = XSDK_ERRCODE_UNKNOWN; break;
                        }
                    }
                }
                catch { errCode = sdkResult; } // Fallback

                // Convert error code to string name using if-else if (C# 7.3 compatible)
                string errCodeName;
                if (errCode == XSDK_ERRCODE_NOERR) errCodeName = "NOERR";
                else if (errCode == XSDK_ERRCODE_SEQUENCE) errCodeName = "SEQUENCE";
                else if (errCode == XSDK_ERRCODE_PARAM) errCodeName = "PARAM";
                else if (errCode == XSDK_ERRCODE_INVALID_CAMERA) errCodeName = "INVALID_CAMERA";
                else if (errCode == XSDK_ERRCODE_LOADLIB) errCodeName = "LOADLIB";
                else if (errCode == XSDK_ERRCODE_UNSUPPORTED) errCodeName = "UNSUPPORTED";
                else if (errCode == XSDK_ERRCODE_BUSY) errCodeName = "BUSY";
                else if (errCode == XSDK_ERRCODE_AF_TIMEOUT) errCodeName = "AF_TIMEOUT";
                else if (errCode == XSDK_ERRCODE_SHOOT_ERROR) errCodeName = "SHOOT_ERROR";
                else if (errCode == XSDK_ERRCODE_FRAME_FULL) errCodeName = "FRAME_FULL";
                else if (errCode == XSDK_ERRCODE_STANDBY) errCodeName = "STANDBY";
                else if (errCode == XSDK_ERRCODE_NODRIVER) errCodeName = "NODRIVER";
                else if (errCode == XSDK_ERRCODE_NO_MODEL_MODULE) errCodeName = "NO_MODEL_MODULE";
                else if (errCode == XSDK_ERRCODE_API_NOTFOUND) errCodeName = "API_NOTFOUND";
                else if (errCode == XSDK_ERRCODE_API_MISMATCH) errCodeName = "API_MISMATCH";
                else if (errCode == XSDK_ERRCODE_INVALID_USBMODE) errCodeName = "INVALID_USBMODE";
                else if (errCode == XSDK_ERRCODE_FORCEMODE_BUSY) errCodeName = "FORCEMODE_BUSY";
                else if (errCode == XSDK_ERRCODE_RUNNING_OTHER_FUNCTION) errCodeName = "RUNNING_OTHER_FUNCTION";
                else if (errCode == XSDK_ERRCODE_COMMUNICATION) errCodeName = "COMMUNICATION";
                else if (errCode == XSDK_ERRCODE_TIMEOUT) errCodeName = "TIMEOUT";
                else if (errCode == XSDK_ERRCODE_COMBINATION) errCodeName = "COMBINATION";
                else if (errCode == XSDK_ERRCODE_WRITEERROR) errCodeName = "WRITEERROR";
                else if (errCode == XSDK_ERRCODE_CARDFULL) errCodeName = "CARDFULL";
                else if (errCode == XSDK_ERRCODE_HARDWARE) errCodeName = "HARDWARE";
                else if (errCode == XSDK_ERRCODE_INTERNAL) errCodeName = "INTERNAL";
                else if (errCode == XSDK_ERRCODE_MEMFULL) errCodeName = "MEMFULL";
                else errCodeName = $"UNKNOWN ({errCode:X})"; // Show hex if unknown

                string errorMessage = $"Fujifilm SDK Error during '{operation}'. SDK Result: {sdkResult}, Last API Code: {apiCode:X}, Last Error Code: {errCode} ({errCodeName})";
                LogMessageStatic("CheckSdkError", errorMessage);

                // Throw appropriate ASCOM exception based on the error code
                if (errCode == XSDK_ERRCODE_BUSY) throw new ASCOM.InvalidOperationException($"{errorMessage} (Camera Busy)");
                else if (errCode == XSDK_ERRCODE_COMMUNICATION || errCode == XSDK_ERRCODE_TIMEOUT) throw new ASCOM.NotConnectedException($"{errorMessage} (Communication Error/Timeout)");
                else if (errCode == XSDK_ERRCODE_UNSUPPORTED) throw new ASCOM.MethodNotImplementedException($"{errorMessage} (Unsupported Operation)");
                else if (errCode == XSDK_ERRCODE_PARAM) throw new ASCOM.InvalidValueException($"{errorMessage} (Invalid Parameter)");
                // Add more specific exceptions if needed
                else throw new ASCOM.DriverException(errorMessage); // General driver exception for others
            }
        }


        private static void LogMessageStatic(string identifier, string message)
        {
            Debug.WriteLine($"[{DateTime.Now:HH:mm:ss.fff}] {identifier}: {message}");
            // Try to log to ASCOM trace logger if CameraHardware is available
            try { CameraHardware.LogMessage(identifier, message); } catch { }
        }

        #endregion
    }

    /// <summary>
    /// ASCOM Camera hardware class for ScdouglasFujifilm.
    /// Static class containing the shared hardware control logic.
    /// </summary>
    [HardwareClass()]
    internal static class CameraHardware
    {
        #region Constants and Fields
        // --- Fields remain the same as user provided file ---
        internal const string traceStateProfileName = "Trace Level";
        internal const string traceStateDefault = "true";
        internal const string cameraNameProfileName = "Camera Name";
        internal const string cameraNameDefault = "";
        private static string DriverProgId = "";
        private static string DriverDescription = "";
        internal static string cameraName = cameraNameDefault;
        private static bool connectedState;
        private static bool sdkInitialized = false;
        private static IntPtr hCamera = IntPtr.Zero;
        private static object hardwareLock = new object();
        private static bool runOnce = false;
        private static CameraConfig currentConfig = null; // Keep this null initially
        internal static Util utilities;
        internal static AstroUtils astroUtilities;
        internal static TraceLogger tl;
        private static CameraStates cameraState = CameraStates.cameraIdle;
        private static int cameraXSize = 11648;
        private static int cameraYSize = 8736;
        private static double pixelSizeX = 3.76;
        private static double pixelSizeY = 3.76;
        private static int maxAdu = 65535;
        private static bool canAbortExposure = false;
        private static bool canStopExposure = false;
        private static bool canPulseGuide = false;
        private static bool hasShutter = true;
        private static string sensorName = "Unknown";
        private static DateTime exposureStartTime;
        private static double lastExposureDuration;
        private static bool imageReady = false;
        private static System.Threading.Timer exposureTimer;
        private static readonly object exposureLock = new object();
        private static List<int> supportedSensitivities = new List<int>();
        private static int minSensitivity = 100;
        private static int maxSensitivity = 12800;
        private static Dictionary<int, double> sdkShutterSpeedToDuration = new Dictionary<int, double>();
        private static Dictionary<double, int> durationToSdkShutterSpeed = new Dictionary<double, int>();
        private static List<int> supportedShutterSpeeds = new List<int>();
        private static double minExposure = 0.0001;
        private static double maxExposure = 3600.0;
        private static bool bulbCapable = true;
        private static object lastImageArray = null;
        #endregion

        #region Initialisation and Dispose
        // --- Init and Dispose remain the same as user provided file ---
        static CameraHardware()
        {
            try
            {
                tl = new TraceLogger("", "ScdouglasFujifilm.Hardware");
                LogMessage("CameraHardware", $"Static initialiser created TraceLogger.");
            }
            catch (Exception ex) { Debug.WriteLine($"Static Initialisation Exception creating TraceLogger: {ex}"); }
        }
        internal static void InitialiseHardware()
        {
            lock (hardwareLock)
            {
                if (string.IsNullOrEmpty(DriverProgId))
                {
                    try
                    {
                        DriverProgId = Camera.DriverProgId;
                        DriverDescription = Camera.DriverDescription;
                        ReadProfile();
                        LogMessage("InitialiseHardware", $"ProgID set: {DriverProgId}. Profile read. Trace State: {tl?.Enabled}");
                    }
                    catch (Exception ex) { LogMessage("InitialiseHardware", $"Exception setting ProgID/reading profile: {ex.Message}"); }
                }

                if (!runOnce)
                {
                    LogMessage("InitialiseHardware", $"Starting one-off initialisation.");
                    try
                    {
                        utilities = new Util();
                        astroUtilities = new AstroUtils();
                        connectedState = false;
                        hCamera = IntPtr.Zero;
                        sdkInitialized = false;
                        LogMessage("InitialiseHardware", "One-off initialisation complete.");
                        runOnce = true;
                    }
                    catch (Exception ex) { LogMessage("InitialiseHardware", $"One-off Initialisation Exception: {ex}"); }
                }
                else { LogMessage("InitialiseHardware", "Skipping one-off initialisation (already run)."); }
            }
        }
        public static void Dispose()
        {
            lock (hardwareLock)
            {
                LogMessage("Dispose", $"Disposing CameraHardware resources.");
                if (Connected) { try { Connected = false; } catch (Exception ex) { LogMessage("Dispose", $"Exception during disconnect in Dispose: {ex.Message}"); } }
                if (sdkInitialized) { try { FujifilmSdkWrapper.XSDK_Exit(); sdkInitialized = false; } catch (Exception ex) { LogMessage("Dispose", $"Exception during XSDK_Exit: {ex.Message}"); } }
                utilities?.Dispose(); utilities = null;
                astroUtilities?.Dispose(); astroUtilities = null;
                exposureTimer?.Dispose(); exposureTimer = null;
                if (tl != null) { tl.Enabled = false; tl.Dispose(); tl = null; }
                LogMessage("Dispose", $"CameraHardware disposal complete.");
            }
        }
        #endregion

        #region ASCOM Common Properties and Methods
        // --- Common properties remain the same as user provided file ---
        public static void SetupDialog()
        {
            if (IsConnected) { MessageBox.Show("Already connected, settings cannot be changed.", "ScdouglasFujifilm Setup", MessageBoxButtons.OK, MessageBoxIcon.Information); return; }
            using (SetupDialogForm F = new SetupDialogForm(tl)) { if (F.ShowDialog() == DialogResult.OK) { WriteProfile(); } ReadProfile(); }
        }
        public static ArrayList SupportedActions => new ArrayList();
        public static string Action(string actionName, string actionParameters) { LogMessage("Action", $"Action {actionName} not implemented."); throw new ActionNotImplementedException($"Action {actionName} is not implemented by this driver"); }
        public static void CommandBlind(string command, bool raw) { CheckConnected("CommandBlind"); throw new MethodNotImplementedException($"CommandBlind - Command:{command}, Raw: {raw}"); }
        public static bool CommandBool(string command, bool raw) { CheckConnected("CommandBool"); throw new MethodNotImplementedException($"CommandBool - Command:{command}, Raw: {raw}"); }
        public static string CommandString(string command, bool raw) { CheckConnected("CommandString"); throw new MethodNotImplementedException($"CommandString - Command:{command}, Raw: {raw}"); }

        public static bool Connected
        {
            get { lock (hardwareLock) { LogMessage("Connected Get", IsConnected.ToString()); return IsConnected; } }
            set
            {
                lock (hardwareLock)
                {
                    if (value == IsConnected) { LogMessage("Connected Set", $"Already in state: {value}"); return; }

                    if (value) // Connect
                    {
                        LogMessage("Connected Set", "Attempting to connect hardware...");
                        hCamera = IntPtr.Zero;
                        sensorName = "Unknown";
                        IntPtr apiCodeBufferPtr = IntPtr.Zero; // <<<< Pointer for API code buffer
                        try
                        {
                            LogMessage("Connected Set", "Step 1: Initializing SDK (if needed)...");
                            if (!sdkInitialized)
                            {
                                int initResult = FujifilmSdkWrapper.XSDK_Init(IntPtr.Zero);
                                FujifilmSdkWrapper.CheckSdkError(IntPtr.Zero, initResult, "XSDK_Init");
                                sdkInitialized = true;
                                LogMessage("Connected Set", "SDK Initialized.");
                            }
                            else { LogMessage("Connected Set", "SDK already initialized."); }

                            LogMessage("Connected Set", "Step 2: Detecting cameras...");
                            int cameraCount;
                            int detectResult = FujifilmSdkWrapper.XSDK_Detect(FujifilmSdkWrapper.XSDK_DSC_IF_USB, IntPtr.Zero, IntPtr.Zero, out cameraCount);
                            FujifilmSdkWrapper.CheckSdkError(IntPtr.Zero, detectResult, "XSDK_Detect");
                            LogMessage("Connected Set", $"Detected {cameraCount} camera(s).");
                            if (cameraCount <= 0) throw new ASCOM.NotConnectedException("No Fujifilm cameras detected via USB.");

                            string deviceId = "ENUM:0"; // TODO: Implement camera selection based on 'cameraName' profile setting.
                            LogMessage("Connected Set", $"Step 3: Opening camera session for '{deviceId}'...");
                            int openResult = FujifilmSdkWrapper.XSDK_OpenEx(deviceId, out hCamera, out int cameraMode, IntPtr.Zero);
                            FujifilmSdkWrapper.CheckSdkError(IntPtr.Zero, openResult, $"XSDK_OpenEx ({deviceId})");
                            LogMessage("Connected Set", $"Camera session opened. Handle: {hCamera}, Mode: {cameraMode}");
                            if (hCamera == IntPtr.Zero) throw new ASCOM.DriverException("Failed to open camera session (handle is null).");

                            LogMessage("Connected Set", "Step 4: Setting PC Priority Mode...");
                            int priorityResult = FujifilmSdkWrapper.XSDK_SetPriorityMode(hCamera, FujifilmSdkWrapper.XSDK_PRIORITY_PC);
                            FujifilmSdkWrapper.CheckSdkError(hCamera, priorityResult, "XSDK_SetPriorityMode");
                            LogMessage("Connected Set", "PC Priority Mode set.");

                            // --- Set Exposure Mode to Manual ---
                            LogMessage("Connected Set", "Step 4.5: Setting Exposure Mode to Manual (M)...");
                            int modeResult = FujifilmSdkWrapper.XSDK_SetMode(hCamera, FujifilmSdkWrapper.GFX100S_MODE_M);
                            // *** Make this fatal if it fails ***
                            FujifilmSdkWrapper.CheckSdkError(hCamera, modeResult, "XSDK_SetMode(Manual)"); // Ensure error check is active
                            LogMessage("Connected Set", "Exposure Mode set to Manual (M).");

                            // --- Verify Mode ---
                            int currentMode;
                            int getModeResult = FujifilmSdkWrapper.XSDK_GetMode(hCamera, out currentMode);
                            if (getModeResult == FujifilmSdkWrapper.XSDK_COMPLETE)
                            {
                                LogMessage("Connected Set", $"Verified camera exposure mode is now: {currentMode} (Expected {FujifilmSdkWrapper.GFX100S_MODE_M})");
                                if (currentMode != FujifilmSdkWrapper.GFX100S_MODE_M)
                                {
                                    // If SetMode succeeded but GetMode returns something else, something is weird.
                                    LogMessage("Connected Set", $"CRITICAL WARNING: SetMode succeeded but GetMode returned unexpected mode {currentMode}!");
                                    // Optional: throw an exception here if Mode M is absolutely essential
                                    // throw new DriverException($"Failed to confirm Manual (M) mode after setting. Current mode: {currentMode}");
                                }
                            }
                            else
                            {
                                LogMessage("Connected Set", $"Warning: XSDK_GetMode failed with result {getModeResult} after setting exposure mode.");
                            }
                            // --- End Verify Mode ---
                            // --- End Set Exposure Mode ---

                            // --- Removed attempt to Set Focus Mode via SetProp ---
                            LogMessage("Connected Set", "Step 4.6: Skipping Focus Mode set (requires reliable SDK method or manual camera setting).");
                            // --- End Removed Code ---


                            // --- Get Device Info ---
                            LogMessage("Connected Set", "Step 5: Getting device info...");
                            try
                            {
                                FujifilmSdkWrapper.XSDK_DeviceInformation deviceInfo;
                                int numApiCodes = 0;
                                int infoResult;

                                // --- Call 1: Get the number of API codes ---
                                LogMessage("Connected Set", $"Calling XSDK_GetDeviceInfoEx (GetCount - Handle: {hCamera})...");
                                infoResult = FujifilmSdkWrapper.XSDK_GetDeviceInfoEx(hCamera, out deviceInfo, out numApiCodes, IntPtr.Zero);
                                LogMessage("Connected Set", $"XSDK_GetDeviceInfoEx (GetCount) returned {infoResult}, numApiCodes={numApiCodes}");
                                FujifilmSdkWrapper.CheckSdkError(hCamera, infoResult, "XSDK_GetDeviceInfoEx (GetCount)");

                                if (numApiCodes < 0) numApiCodes = 0;

                                // --- Allocate buffer for API codes ---
                                int bufferSize = numApiCodes * sizeof(int);
                                if (bufferSize > 0)
                                {
                                    apiCodeBufferPtr = Marshal.AllocHGlobal(bufferSize);
                                    LogMessage("Connected Set", $"Allocated {bufferSize} bytes for {numApiCodes} API codes at {apiCodeBufferPtr}.");
                                }
                                else
                                {
                                    apiCodeBufferPtr = IntPtr.Zero;
                                    LogMessage("Connected Set", "No API codes reported, buffer not allocated.");
                                }

                                // --- Call 2: Get the info struct AND the API codes list ---
                                LogMessage("Connected Set", $"Calling XSDK_GetDeviceInfoEx (GetData - Handle: {hCamera}, Buffer: {apiCodeBufferPtr})...");
                                infoResult = FujifilmSdkWrapper.XSDK_GetDeviceInfoEx(hCamera, out deviceInfo, out numApiCodes, apiCodeBufferPtr);
                                LogMessage("Connected Set", $"XSDK_GetDeviceInfoEx (GetData) returned {infoResult}");
                                FujifilmSdkWrapper.CheckSdkError(hCamera, infoResult, "XSDK_GetDeviceInfoEx (GetData)");

                                // Optional: Read the API codes
                                if (apiCodeBufferPtr != IntPtr.Zero && numApiCodes > 0)
                                {
                                    int[] apiCodes = new int[numApiCodes];
                                    Marshal.Copy(apiCodeBufferPtr, apiCodes, 0, numApiCodes);
                                    LogMessage("Connected Set", $"Retrieved {numApiCodes} API Codes (Example: {apiCodes[0]})"); // Log first code as example
                                }

                                sensorName = deviceInfo.strProduct ?? "Unknown Model";
                                LogMessage("Connected Set", $"Retrieved Product Name: {sensorName}");
                                LogMessage("Connected Set", $"Serial: {deviceInfo.strSerialNo}, Firmware: {deviceInfo.strFirmware}");
                            }
                            catch (Exception infoEx)
                            {
                                LogMessage("Connected Set", $"Error getting device info: {infoEx.Message}");
                                sensorName = "Fujifilm Camera (Info Error)";
                                throw;
                            }
                            finally
                            {
                                if (apiCodeBufferPtr != IntPtr.Zero)
                                {
                                    Marshal.FreeHGlobal(apiCodeBufferPtr);
                                    LogMessage("Connected Set", $"Freed API code buffer at {apiCodeBufferPtr}.");
                                    apiCodeBufferPtr = IntPtr.Zero;
                                }
                            }
                            // --- End Get Device Info ---

                            // --- Set connected state TRUE *before* caching capabilities ---
                            connectedState = true;
                            LogMessage("Connected Set", $"State before CacheCameraCapabilities: connectedState={connectedState}, hCamera={hCamera}");
                            // --- End Change ---

                            LogMessage("Connected Set", "Step 6: Caching camera capabilities...");
                            CacheCameraCapabilities(); // This should now run correctly
                            LogMessage("Connected Set", "Capabilities cached.");

                            // connectedState = true; // MOVED EARLIER
                            LogMessage("Connected Set", "Hardware Connected Successfully.");
                        }
                        catch (Exception ex)
                        {
                            LogMessage("Connected Set", $"HARDWARE CONNECTION FAILED: {ex.Message}\n{ex.StackTrace}");
                            if (hCamera != IntPtr.Zero) { try { FujifilmSdkWrapper.XSDK_Close(hCamera); } catch { } hCamera = IntPtr.Zero; }
                            if (apiCodeBufferPtr != IntPtr.Zero) { try { Marshal.FreeHGlobal(apiCodeBufferPtr); } catch { } }
                            connectedState = false; // Ensure state is false on error
                            sensorName = "Unknown";
                            throw;
                        }
                    }
                    else // Disconnect
                    {
                        LogMessage("Connected Set", "Disconnecting hardware...");
                        if (hCamera != IntPtr.Zero)
                        {
                            try
                            {
                                LogMessage("Connected Set", $"Closing camera handle {hCamera}...");
                                int closeResult = FujifilmSdkWrapper.XSDK_Close(hCamera);
                                LogMessage("Connected Set", $"XSDK_Close returned {closeResult}");
                                LogMessage("Connected Set", "Camera session closed.");
                            }
                            catch (Exception ex) { LogMessage("Connected Set", $"Exception during XSDK_Close: {ex.Message}"); }
                            finally { hCamera = IntPtr.Zero; connectedState = false; sensorName = "Unknown"; LogMessage("Connected Set", "Hardware Disconnected."); }
                        }
                        else { LogMessage("Connected Set", "Already disconnected (no handle)."); connectedState = false; sensorName = "Unknown"; }
                    }
                }
            }
        }


        public static string Description => DriverDescription;
        public static string DriverInfo => $"Fujifilm ASCOM Driver. Version: {DriverVersion}";
        public static string DriverVersion => System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(2);
        public static short InterfaceVersion => 3;
        public static string Name => "Fujifilm Camera (ASCOM)";

        #endregion

        #region ASCOM Camera Specific Properties and Methods

        // ... (Properties like AbortExposure, BayerOffsetX/Y, BinX/Y, CCDTemperature, CameraState etc. remain largely unchanged) ...
        public static void AbortExposure()
        {
            lock (exposureLock)
            {
                LogMessage("AbortExposure", $"Request received. Current state: {cameraState}");
                if (cameraState == CameraStates.cameraExposing)
                {
                    LogMessage("AbortExposure", "Abort not currently supported by SDK/driver.");
                    exposureTimer?.Dispose(); exposureTimer = null;
                    cameraState = CameraStates.cameraIdle;
                    imageReady = false;
                    throw new MethodNotImplementedException("AbortExposure is not implemented.");
                }
                else { LogMessage("AbortExposure", "No exposure in progress to abort."); }
            }
        }

        public static short BayerOffsetX => 0;
        public static short BayerOffsetY => 0;
        public static short BinX { get => 1; set { if (value != 1) throw new InvalidValueException("BinX", value.ToString(), "1"); } }
        public static short BinY { get => 1; set { if (value != 1) throw new InvalidValueException("BinY", value.ToString(), "1"); } }
        public static double CCDTemperature => throw new PropertyNotImplementedException("CCDTemperature", false);

        public static CameraStates CameraState
        {
            get { lock (exposureLock) { LogMessage("CameraState Get", cameraState.ToString()); return cameraState; } }
        }

        public static int CameraXSize => cameraXSize;
        public static int CameraYSize => cameraYSize;
        public static bool CanAbortExposure => canAbortExposure;
        public static bool CanAsymmetricBin => false;
        public static bool CanGetCoolerPower => false;
        public static bool CanPulseGuide => canPulseGuide;
        public static bool CanSetCCDTemperature => false;
        public static bool CanStopExposure => canStopExposure;
        public static bool CoolerOn { get => false; set => throw new PropertyNotImplementedException("CoolerOn", true); }
        public static double CoolerPower => 0.0;
        public static double ElectronsPerADU => throw new PropertyNotImplementedException("ElectronsPerADU", false);
        public static double ExposureMax => maxExposure;
        public static double ExposureMin => minExposure;
        public static double ExposureResolution => -1;
        public static bool FastReadout { get => false; set => throw new PropertyNotImplementedException("FastReadout", true); }
        public static double FullWellCapacity => throw new PropertyNotImplementedException("FullWellCapacity", false);

        public static short Gain
        {
            get
            {
                CheckConnected("Gain Get");
                lock (hardwareLock)
                {
                    int sdkSensitivity;
                    LogMessage("Gain Get", $"Calling XSDK_GetSensitivity(hCamera={hCamera})...");
                    int result = FujifilmSdkWrapper.XSDK_GetSensitivity(hCamera, out sdkSensitivity);
                    LogMessage("Gain Get", $"XSDK_GetSensitivity returned {result}, sensitivity={sdkSensitivity}");
                    FujifilmSdkWrapper.CheckSdkError(hCamera, result, "XSDK_GetSensitivity");
                    LogMessage("Gain Get", $"SDK Sensitivity: {sdkSensitivity}");
                    return (short)Math.Max(GainMin, Math.Min(GainMax, sdkSensitivity));
                }
            }
            set
            {
                CheckConnected("Gain Set");
                if (value < GainMin || value > GainMax) throw new InvalidValueException("Gain", value.ToString(), $"Range {GainMin} to {GainMax}");
                // Allow setting gain even if not in the *cached* list, the SDK might still accept it.
                // if (!supportedSensitivities.Contains(value)) { LogMessage("Gain Set", $"Warning: Requested ISO {value} is not in the list of explicitly supported values from CapSensitivity."); }

                lock (hardwareLock)
                {
                    LogMessage("Gain Set", $"Calling XSDK_SetSensitivity(hCamera={hCamera}, value={value})...");
                    int result = FujifilmSdkWrapper.XSDK_SetSensitivity(hCamera, value);
                    LogMessage("Gain Set", $"XSDK_SetSensitivity returned {result}");
                    FujifilmSdkWrapper.CheckSdkError(hCamera, result, "XSDK_SetSensitivity"); // This might throw if SDK fails
                    LogMessage("Gain Set", $"SDK Sensitivity set to: {value}");
                }
            }
        }

        public static short GainMax => (short)maxSensitivity;
        public static short GainMin => (short)minSensitivity;
        public static ArrayList Gains
        {
            // Return empty list if capabilities couldn't be read to avoid errors
            get { ArrayList list = new ArrayList(); foreach (int iso in supportedSensitivities) list.Add(iso.ToString()); return list; }
        }
        public static bool HasShutter => hasShutter;
        public static double HeatSinkTemperature => throw new PropertyNotImplementedException("HeatSinkTemperature", false);

        public static object ImageArray
        {
            get
            {
                CheckConnected("ImageArray Get");
                lock (exposureLock)
                {
                    if (!imageReady) { LogMessage("ImageArray Get", "Error: Image not ready."); throw new InvalidOperationException("Image not ready. Check ImageReady first."); }
                    if (lastImageArray == null) { LogMessage("ImageArray Get", "Error: ImageReady was true but image data is null. Attempting download again..."); DownloadImageData(); }
                    if (lastImageArray == null) { LogMessage("ImageArray Get", "Error: DownloadImageData failed to produce image data."); cameraState = CameraStates.cameraError; throw new DriverException("Failed to retrieve image data after download attempt."); }

                    LogMessage("ImageArray Get", "Returning image array.");
                    object imageToReturn = lastImageArray;
                    lastImageArray = null;
                    imageReady = false;
                    if (cameraState != CameraStates.cameraError) cameraState = CameraStates.cameraIdle;
                    return imageToReturn;
                }
            }
        }
        public static object ImageArrayVariant => ImageArray;
        public static bool ImageReady
        {
            get { lock (exposureLock) { LogMessage("ImageReady Get", imageReady.ToString()); return imageReady; } }
        }
        public static bool IsPulseGuiding => false;
        public static double LastExposureDuration => lastExposureDuration;
        public static string LastExposureStartTime => exposureStartTime.ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ss.fff");
        public static int MaxADU => maxAdu;
        public static short MaxBinX => 1;
        public static short MaxBinY => 1;
        public static int NumX { get => CameraXSize; set => CheckSubframe("NumX", value, CameraXSize); }
        public static int NumY { get => CameraYSize; set => CheckSubframe("NumY", value, CameraYSize); }
        public static int StartX { get => 0; set => CheckSubframe("StartX", value, 0); }
        public static int StartY { get => 0; set => CheckSubframe("StartY", value, 0); }
        public static short PercentCompleted
        {
            get
            {
                lock (exposureLock)
                {
                    if (cameraState == CameraStates.cameraExposing) return 50;
                    if (cameraState == CameraStates.cameraDownload) return 90;
                    if (cameraState == CameraStates.cameraIdle && imageReady) return 100;
                    return 0;
                }
            }
        }
        public static double PixelSizeX => pixelSizeX;
        public static double PixelSizeY => pixelSizeY;
        public static void PulseGuide(GuideDirections direction, int duration) => throw new MethodNotImplementedException("PulseGuide");
        public static short ReadoutMode { get => 0; set { if (value != 0) throw new InvalidValueException("ReadoutMode", "0", "Only ReadoutMode 0 is supported."); } }
        public static ArrayList ReadoutModes => new ArrayList { "Normal" };
        // Updated SensorType based on user finding for NINA compatibility
        public static SensorType SensorType => SensorType.RGGB;

        public static string SensorName => sensorName;

        public static double SetCCDTemperature { get => throw new PropertyNotImplementedException("SetCCDTemperature", false); set => throw new PropertyNotImplementedException("SetCCDTemperature", true); }

        // *** MODIFIED StartExposure ***
        public static void StartExposure(double duration, bool light)
        {
            CheckConnected("StartExposure");
            lock (exposureLock)
            {
                if (cameraState != CameraStates.cameraIdle) throw new InvalidOperationException($"Camera not idle. State: {cameraState}");
                LogMessage("StartExposure", $"Request: Duration={duration}s, Light Frame={light}");

                // Check against cached min/max exposure values
                if (duration < minExposure) { LogMessage("StartExposure", $"Requested duration {duration}s is less than minimum {minExposure}s."); throw new InvalidValueException("StartExposure Duration", duration.ToString(), $"Minimum exposure is {minExposure}"); }
                // Check against max *non-bulb* exposure if bulb isn't supported OR if duration doesn't require bulb
                if (!bulbCapable && duration > maxExposure) { LogMessage("StartExposure", $"Requested duration {duration}s exceeds max coded exposure {maxExposure}s and Bulb is not supported."); throw new InvalidValueException("StartExposure Duration", duration.ToString(), $"Bulb mode not supported, maximum exposure is {maxExposure}"); }
                // Note: If bulbCapable is true, we allow durations > maxExposure

                IntPtr shotOptPtr = IntPtr.Zero; // Pointer for the long* parameter
                long shotOptValue = 0;          // Value for the long* parameter (0 = default/no options)
                bool shotOptAllocated = false;  // Track if we allocated memory

                try
                {
                    // Log Current Camera State (same as your file)
                    try
                    {
                        int currentMode = -1, currentShut = -1, currentBulb = -1, currentIso = -1;
                        int getResult = FujifilmSdkWrapper.XSDK_GetMode(hCamera, out currentMode);
                        if (getResult == FujifilmSdkWrapper.XSDK_COMPLETE) LogMessage("StartExposure", $"GetMode OK: Mode={currentMode}");
                        else LogMessage("StartExposure", $"GetMode FAILED: Result={getResult}");

                        getResult = FujifilmSdkWrapper.XSDK_GetShutterSpeed(hCamera, out currentShut, out currentBulb);
                        if (getResult == FujifilmSdkWrapper.XSDK_COMPLETE) LogMessage("StartExposure", $"GetShutterSpeed OK: Shutter={currentShut}, Bulb={currentBulb}");
                        else LogMessage("StartExposure", $"GetShutterSpeed FAILED: Result={getResult}");

                        getResult = FujifilmSdkWrapper.XSDK_GetSensitivity(hCamera, out currentIso);
                        if (getResult == FujifilmSdkWrapper.XSDK_COMPLETE) LogMessage("StartExposure", $"GetSensitivity OK: ISO={currentIso}");
                        else LogMessage("StartExposure", $"GetSensitivity FAILED: Result={getResult}");

                        LogMessage("StartExposure", $"State Before Exposure: Mode={currentMode}, Shutter={currentShut}, Bulb={currentBulb}, ISO={currentIso}");
                    }
                    catch (Exception stateEx) { LogMessage("StartExposure", $"Warning: Could not get full camera state before exposure: {stateEx.Message}"); }

                    // --- Set Shutter Speed ---
                    LogMessage("StartExposure", "Converting duration to SDK code...");
                    int sdkShutterSpeed = DurationToSdkShutterSpeed(duration); // Gets -1 if bulb needed
                    int isBulb = (sdkShutterSpeed == FujifilmSdkWrapper.XSDK_SHUTTER_BULB) ? 1 : 0;

                    if (isBulb == 1 && !bulbCapable)
                    {
                        LogMessage("StartExposure", $"Error: Bulb exposure requested ({duration}s) but camera config/capabilities indicate Bulb is not supported.");
                        throw new InvalidValueException("StartExposure Duration", duration.ToString(), "Internal Error: Bulb exposure requested but camera config indicates Bulb is not supported.");
                    }

                    LogMessage("StartExposure", $"Setting SDK Shutter Speed Code: {sdkShutterSpeed}, Bulb Flag: {isBulb}");
                    int setResult = FujifilmSdkWrapper.XSDK_SetShutterSpeed(hCamera, sdkShutterSpeed, isBulb);
                    FujifilmSdkWrapper.CheckSdkError(hCamera, setResult, "XSDK_SetShutterSpeed");

                    // *** Add small delay after setting shutter speed, especially before Bulb start ***
                    if (isBulb == 1)
                    {
                        LogMessage("StartExposure", "Adding short delay after setting Bulb mode...");
                        System.Threading.Thread.Sleep(100); // 100ms delay
                    }

                    // --- Allocate shotOptPtr for all cases ---
                    shotOptPtr = Marshal.AllocHGlobal(sizeof(long));
                    Marshal.WriteInt64(shotOptPtr, shotOptValue); // Write 0L to the allocated memory
                    shotOptAllocated = true; // Mark as allocated
                    LogMessage("StartExposure", $"Allocated and initialized plShotOpt (long*) at {shotOptPtr} with value {shotOptValue}");

                    // --- Trigger Exposure Start ---
                    int releaseModeStart;
                    int releaseResult;
                    int releaseStatus;

                    if (isBulb == 1)
                    {
                        // *** Sequence for Bulb: S1ON -> BULBS2_ON ***

                        // 1. Press Halfway (S1ON)
                        releaseModeStart = FujifilmSdkWrapper.XSDK_RELEASE_S1ON; // Use 0x0200
                        LogMessage("StartExposure", $"Triggering S1ON via XSDK_Release (Mode: 0x{releaseModeStart:X}, Options Ptr: {shotOptPtr})...");
                        releaseResult = FujifilmSdkWrapper.XSDK_Release(hCamera, releaseModeStart, shotOptPtr, out releaseStatus);
                        LogMessage("StartExposure", $"XSDK_Release (S1ON) returned {releaseResult}, status={releaseStatus}");
                        try
                        {
                            FujifilmSdkWrapper.CheckSdkError(hCamera, releaseResult, "XSDK_Release (S1ON)");
                        }
                        catch (Exception s1Ex)
                        {
                            LogMessage("StartExposure", $"Error during S1ON: {s1Ex.Message}. Aborting exposure start.");
                            throw; // Re-throw S1ON error, cannot proceed
                        }
                        LogMessage("StartExposure", $"SDK Release command (S1ON) sent successfully.");

                        // 2. Start Bulb (BULBS2_ON)
                        releaseModeStart = FujifilmSdkWrapper.XSDK_RELEASE_BULBS2_ON; // Use 0x0500
                        LogMessage("StartExposure", $"Triggering BULBS2_ON via XSDK_Release (Mode: 0x{releaseModeStart:X}, Options Ptr: {shotOptPtr})...");
                        releaseResult = FujifilmSdkWrapper.XSDK_Release(hCamera, releaseModeStart, shotOptPtr, out releaseStatus);
                        LogMessage("StartExposure", $"XSDK_Release (BULBS2_ON) returned {releaseResult}, status={releaseStatus}");
                        try
                        {
                            FujifilmSdkWrapper.CheckSdkError(hCamera, releaseResult, "XSDK_Release (BULBS2_ON - Start)");
                        }
                        catch (Exception bulbStartEx)
                        {
                            LogMessage("StartExposure", $"Error during BULBS2_ON: {bulbStartEx.Message}. Attempting to release S1...");
                            // Attempt to clean up by releasing S1
                            try
                            {
                                FujifilmSdkWrapper.XSDK_Release(hCamera, FujifilmSdkWrapper.XSDK_RELEASE_N_S1OFF, IntPtr.Zero, out _);
                            }
                            catch { /* Ignore errors during cleanup */ }
                            throw; // Re-throw the original BULBS2_ON error
                        }
                        LogMessage("StartExposure", $"SDK Release command (BULBS2_ON - Start) sent successfully.");
                    }
                    else
                    {
                        // *** Sequence for Timed: SHOOT_S1OFF ***
                        releaseModeStart = FujifilmSdkWrapper.XSDK_RELEASE_SHOOT_S1OFF; // Use 0x0104
                        LogMessage("StartExposure", $"Triggering timed exposure START via XSDK_Release (Mode: 0x{releaseModeStart:X}, Options Ptr: {shotOptPtr})...");
                        releaseResult = FujifilmSdkWrapper.XSDK_Release(hCamera, releaseModeStart, shotOptPtr, out releaseStatus);
                        LogMessage("StartExposure", $"XSDK_Release (Timed Start) returned {releaseResult}, status={releaseStatus}");
                        FujifilmSdkWrapper.CheckSdkError(hCamera, releaseResult, "XSDK_Release (Timed Start)");
                        LogMessage("StartExposure", $"SDK Release command (Timed Start) sent successfully.");
                    }


                    // --- Update State and Start Timer ---
                    cameraState = CameraStates.cameraExposing;
                    exposureStartTime = DateTime.UtcNow;
                    lastExposureDuration = duration;
                    imageReady = false;
                    lastImageArray = null;

                    int exposureMillis = (int)(duration * 1000);
                    int bufferMillis = 2000; // Add buffer time for camera processing

                    exposureTimer?.Dispose();

                    // Use different timer callbacks based on bulb mode
                    if (isBulb == 1)
                    {
                        LogMessage("StartExposure", $"Starting BULB timer for {exposureMillis} ms (Callback: OnBulbExposureTimerElapsed).");
                        // Start timer for the exact duration for bulb, stop command will be sent in callback
                        exposureTimer = new System.Threading.Timer(OnBulbExposureTimerElapsed, null, exposureMillis, Timeout.Infinite);
                    }
                    else
                    {
                        LogMessage("StartExposure", $"Starting TIMED timer for {exposureMillis + bufferMillis} ms (Callback: OnExposureComplete).");
                        // Start timer with buffer for timed exposures, image check happens in callback
                        exposureTimer = new System.Threading.Timer(OnExposureComplete, null, exposureMillis + bufferMillis, Timeout.Infinite);
                    }
                    LogMessage("StartExposure", $"Exposure timing initiated.");

                }
                catch (Exception ex)
                {
                    LogMessage("StartExposure", $"StartExposure failed: {ex.Message}\n{ex.StackTrace}"); // Added stack trace
                    cameraState = CameraStates.cameraError;
                    throw;
                }
                finally
                {
                    // --- Free allocated shot options memory ---
                    if (shotOptAllocated && shotOptPtr != IntPtr.Zero) // Check flag
                    {
                        Marshal.FreeHGlobal(shotOptPtr);
                        LogMessage("StartExposure", $"Freed allocated plShotOpt memory at {shotOptPtr}");
                    }
                    // --- End Free ---
                }
            }
        }


        public static void StopExposure()
        {
            lock (exposureLock)
            {
                LogMessage("StopExposure", $"Request received. Current state: {cameraState}");
                if (cameraState == CameraStates.cameraExposing)
                {
                    LogMessage("StopExposure", "StopExposure not currently supported.");
                    exposureTimer?.Dispose(); exposureTimer = null;
                    cameraState = CameraStates.cameraIdle;
                    imageReady = false;
                    throw new MethodNotImplementedException("StopExposure");
                }
                else { LogMessage("StopExposure", "No exposure in progress to stop."); }
            }
        }

        #endregion

        #region Private Helper Methods

        private static void CheckConnected(string message) { if (!IsConnected) { throw new NotConnectedException($"{DriverDescription} ({DriverProgId}) is not connected: {message}"); } }
        private static void CheckSubframe(string property, int value, int expected) { if (value != expected) { LogMessage(property, $"Invalid value {value}. Only full frame ({expected}) is supported."); throw new InvalidValueException(property, value.ToString(), expected.ToString()); } }

        internal static void ReadProfile()
        {
            if (tl == null) { Debug.WriteLine("ReadProfile called before TraceLogger initialized!"); return; }
            try
            {
                // Correct Profile usage: Instantiate, set DeviceType, then use
                Profile driverProfile = new Profile();
                driverProfile.DeviceType = "Camera"; // Set DeviceType
                tl.Enabled = Convert.ToBoolean(driverProfile.GetValue(DriverProgId, traceStateProfileName, string.Empty, traceStateDefault));
                cameraName = driverProfile.GetValue(DriverProgId, cameraNameProfileName, string.Empty, cameraNameDefault);
                LogMessage("ReadProfile", $"Trace state: {tl.Enabled}, Camera Name: '{cameraName}'");
            }
            catch (Exception ex) { LogMessage("ReadProfile", $"Error reading profile: {ex.Message}"); tl.Enabled = Convert.ToBoolean(traceStateDefault); cameraName = cameraNameDefault; }
        }

        internal static void WriteProfile()
        {
            try
            {
                // Correct Profile usage: Instantiate, set DeviceType, then use
                Profile driverProfile = new Profile();
                driverProfile.DeviceType = "Camera"; // Set DeviceType
                if (tl != null) driverProfile.WriteValue(DriverProgId, traceStateProfileName, tl.Enabled.ToString());
                // driverProfile.WriteValue(DriverProgId, cameraNameProfileName, cameraName); // Uncomment when camera selection is added
                LogMessage("WriteProfile", $"Trace state saved: {tl?.Enabled}");
            }
            catch (Exception ex) { LogMessage("WriteProfile", $"Error writing profile: {ex.Message}"); }
        }


        internal static void LogMessage(string identifier, string message) { tl?.LogMessageCrLf(identifier, message); }
        internal static void LogMessage(string identifier, string format, params object[] args) { tl?.LogMessageCrLf(identifier, string.Format(format, args)); }
        private static bool IsConnected => connectedState && hCamera != IntPtr.Zero;

        // --- Reverted to Simplified CacheCameraCapabilities (skipping SDK calls) ---
        private static void CacheCameraCapabilities()
        {
            // --- Log state INSIDE CacheCameraCapabilities ---
            LogMessage("CacheCameraCapabilities", $"Entering CacheCameraCapabilities. State: connectedState={connectedState}, hCamera={hCamera}, IsConnected={IsConnected}");
            // --- End Log ---

            if (!IsConnected) // Check connection status
            {
                LogMessage("CacheCameraCapabilities", "Exiting CacheCameraCapabilities early because IsConnected is false.");
                return; // Exit if not connected
            }

            LogMessage("CacheCameraCapabilities", "Caching camera capabilities (Simplified Version - Skipping SDK calls)...");

            // --- Populate Shutter Map FIRST ---
            PopulateShutterSpeedMaps();
            // --- Log count immediately after population attempt ---
            LogMessage("CacheCameraCapabilities", $"After PopulateShutterSpeedMaps, durationToSdkShutterSpeed.Count = {durationToSdkShutterSpeed.Count}");

            // --- Use Defaults because SDK calls are skipped ---
            LogMessage("CacheCameraCapabilities", "Using default values for Sensitivity and ShutterSpeed capabilities.");

            supportedSensitivities.Clear(); // Ensure list is empty as we didn't query SDK
            minSensitivity = 100; // Default
            maxSensitivity = 12800; // Default

            supportedShutterSpeeds.Clear(); // Ensure list is empty
            bulbCapable = true; // Default to true for now to allow testing bulb if needed

            // Determine Min/Max Exposure from the populated map
            if (durationToSdkShutterSpeed.Count > 0)
            {
                List<double> durations = new List<double>(durationToSdkShutterSpeed.Keys);
                durations.Sort();
                minExposure = durations[0];
                maxExposure = durations[durations.Count - 1];
            }
            else
            { // Should not happen if PopulateShutterSpeedMaps ran
                LogMessage("CacheCameraCapabilities", "WARNING: durationToSdkShutterSpeed map is empty after population attempt! Using fallback exposure range.");
                minExposure = 1.0 / 8000.0; maxExposure = 60.0 * 60.0; // Fallback
            }

            // Log the values used (defaults + map-derived)
            LogMessage("CacheCameraCapabilities", $"Using capabilities: Min Sensitivity={minSensitivity}, Max Sensitivity={maxSensitivity}, Min Exposure={minExposure}s, Max Exposure={maxExposure}s, Bulb Capable={bulbCapable}");
            // --- End Defaults ---

            LogMessage("CacheCameraCapabilities", "Simplified capability caching finished.");
        }


        // Removed List<int> parameter as it wasn't used for population
        private static void PopulateShutterSpeedMaps()
        {
            // Ensure maps are clear before populating
            sdkShutterSpeedToDuration.Clear();
            durationToSdkShutterSpeed.Clear();
            LogMessage("PopulateShutterSpeedMaps", "Populating shutter speed maps based on SDK PDF...");
            // --- MAPPINGS BASED ON SDK PDF pp. 91-95 ---
            AddSdkShutterMapping(5, 1.0 / 180000.0); AddSdkShutterMapping(6, 1.0 / 160000.0); AddSdkShutterMapping(7, 1.0 / 128000.0);
            AddSdkShutterMapping(9, 1.0 / 102400.0); AddSdkShutterMapping(12, 1.0 / 80000.0); AddSdkShutterMapping(15, 1.0 / 64000.0);
            AddSdkShutterMapping(19, 1.0 / 51200.0); AddSdkShutterMapping(24, 1.0 / 40000.0); AddSdkShutterMapping(30, 1.0 / 32000.0);
            AddSdkShutterMapping(38, 1.0 / 25600.0); AddSdkShutterMapping(43, 1.0 / 24000.0); AddSdkShutterMapping(48, 1.0 / 20000.0);
            AddSdkShutterMapping(61, 1.0 / 16000.0); AddSdkShutterMapping(76, 1.0 / 12800.0); AddSdkShutterMapping(86, 1.0 / 12000.0);
            AddSdkShutterMapping(96, 1.0 / 10000.0); AddSdkShutterMapping(122, 1.0 / 8000.0); AddSdkShutterMapping(153, 1.0 / 6400.0);
            AddSdkShutterMapping(172, 1.0 / 6000.0); AddSdkShutterMapping(193, 1.0 / 5000.0); AddSdkShutterMapping(244, 1.0 / 4000.0);
            AddSdkShutterMapping(307, 1.0 / 3200.0); AddSdkShutterMapping(345, 1.0 / 3000.0); AddSdkShutterMapping(387, 1.0 / 2500.0);
            AddSdkShutterMapping(488, 1.0 / 2000.0); AddSdkShutterMapping(615, 1.0 / 1600.0); AddSdkShutterMapping(690, 1.0 / 1500.0);
            AddSdkShutterMapping(775, 1.0 / 1250.0); AddSdkShutterMapping(976, 1.0 / 1000.0); AddSdkShutterMapping(1230, 1.0 / 800.0);
            AddSdkShutterMapping(1381, 1.0 / 750.0); AddSdkShutterMapping(1550, 1.0 / 640.0); AddSdkShutterMapping(1953, 1.0 / 500.0);
            AddSdkShutterMapping(2460, 1.0 / 400.0); AddSdkShutterMapping(2762, 1.0 / 350.0); AddSdkShutterMapping(3100, 1.0 / 320.0);
            AddSdkShutterMapping(3906, 1.0 / 250.0); AddSdkShutterMapping(4921, 1.0 / 200.0); AddSdkShutterMapping(5524, 1.0 / 180.0);
            AddSdkShutterMapping(6200, 1.0 / 160.0); AddSdkShutterMapping(7812, 1.0 / 125.0); AddSdkShutterMapping(9843, 1.0 / 100.0);
            AddSdkShutterMapping(11048, 1.0 / 90.0); AddSdkShutterMapping(12401, 1.0 / 80.0); AddSdkShutterMapping(15625, 1.0 / 60.0);
            AddSdkShutterMapping(19686, 1.0 / 50.0); AddSdkShutterMapping(22097, 1.0 / 45.0); AddSdkShutterMapping(24803, 1.0 / 40.0);
            AddSdkShutterMapping(31250, 1.0 / 30.0); AddSdkShutterMapping(39372, 1.0 / 25.0); AddSdkShutterMapping(49606, 1.0 / 20.0);
            AddSdkShutterMapping(62500, 1.0 / 15.0); AddSdkShutterMapping(78745, 1.0 / 13.0); AddSdkShutterMapping(99212, 1.0 / 10.0);
            AddSdkShutterMapping(125000, 1.0 / 8.0); AddSdkShutterMapping(157490, 1.0 / 6.0); AddSdkShutterMapping(198425, 1.0 / 5.0);
            AddSdkShutterMapping(250000, 1.0 / 4.0); AddSdkShutterMapping(314980, 1.0 / 3.0); AddSdkShutterMapping(396850, 1.0 / 2.5);
            AddSdkShutterMapping(500000, 1.0 / 2.0); AddSdkShutterMapping(629960, 1.0 / 1.6); AddSdkShutterMapping(707106, 1.0 / 1.5);
            AddSdkShutterMapping(793700, 1.0 / 1.3); AddSdkShutterMapping(1000000, 1.0); AddSdkShutterMapping(1259921, 1.3);
            AddSdkShutterMapping(1414213, 1.5); AddSdkShutterMapping(1587401, 1.6); AddSdkShutterMapping(2000000, 2.0);
            AddSdkShutterMapping(2519842, 2.5); AddSdkShutterMapping(3174802, 3.0); AddSdkShutterMapping(4000000, 4.0);
            AddSdkShutterMapping(5039684, 5.0); AddSdkShutterMapping(6349604, 6.0); AddSdkShutterMapping(8000000, 8.0);
            AddSdkShutterMapping(10079368, 10.0); AddSdkShutterMapping(12699208, 13.0); AddSdkShutterMapping(16000000, 15.0);
            AddSdkShutterMapping(20158736, 20.0); AddSdkShutterMapping(25398416, 25.0); AddSdkShutterMapping(32000000, 30.0);
            AddSdkShutterMapping(64000000, 60.0);
            // Add other long exposures if needed and supported by CapShutterSpeed
            LogMessage("PopulateShutterSpeedMaps", $"Populated {sdkShutterSpeedToDuration.Count} shutter speed mappings.");
        }

        private static void AddSdkShutterMapping(int sdkValue, double duration)
        {
            if (sdkValue == 0 || sdkValue == -1) return;
            double tolerance = 1e-9;
            bool durationExists = false;
            foreach (var existingKey in durationToSdkShutterSpeed.Keys) { if (Math.Abs(existingKey - duration) < tolerance) { durationExists = true; break; } }
            if (!sdkShutterSpeedToDuration.ContainsKey(sdkValue))
            {
                sdkShutterSpeedToDuration.Add(sdkValue, duration);
                if (!durationExists) { durationToSdkShutterSpeed.Add(duration, sdkValue); }
                else { LogMessage("AddSdkShutterMapping", $"Warning: Duration {duration} already mapped. SDK value {sdkValue} added to forward map only."); }
            }
            else { LogMessage("AddSdkShutterMapping", $"Warning: SDK value {sdkValue} already mapped. Skipping."); }
        }

        private static int DurationToSdkShutterSpeed(double duration)
        {
            LogMessage("DurationToSdkShutterSpeed", $"Attempting to map duration: {duration}s"); // Added log
            if (durationToSdkShutterSpeed.Count == 0)
            {
                LogMessage("DurationToSdkShutterSpeed", "Error: durationToSdkShutterSpeed map is empty!"); // Added log
                throw new DriverException("Shutter speed capabilities not loaded or empty map.");
            }
            double minDiff = double.MaxValue;
            double closestDuration = -1.0;
            foreach (double supportedDuration in durationToSdkShutterSpeed.Keys)
            {
                double diff = Math.Abs(supportedDuration - duration);
                if (diff < minDiff) { minDiff = diff; closestDuration = supportedDuration; }
            }
            if (closestDuration < 0) { throw new DriverException("Could not find closest duration match."); }
            double tolerance = Math.Max(closestDuration * 0.001, 0.0001);
            if (minDiff <= tolerance)
            {
                int sdkVal = durationToSdkShutterSpeed[closestDuration];
                LogMessage("DurationToSdkShutterSpeed", $"Mapping duration {duration}s to closest supported {closestDuration}s (SDK: {sdkVal})");
                return sdkVal;
            }
            else
            {
                // Check bulb capability using the 'bulbCapable' field populated during CacheCameraCapabilities
                if (bulbCapable && duration > maxExposure) // Use maxExposure determined from the map
                {
                    LogMessage("DurationToSdkShutterSpeed", $"Duration {duration}s > max non-bulb ({maxExposure}s), mapping to BULB (-1).");
                    return FujifilmSdkWrapper.XSDK_SHUTTER_BULB;
                }
                LogMessage("DurationToSdkShutterSpeed", $"Duration {duration}s is too far from nearest supported {closestDuration}s (Diff: {minDiff}, Tol: {tolerance}).");
                throw new InvalidValueException($"Requested duration {duration}s is not supported or too far from nearest value {closestDuration}s.");
            }
        }

        // *** MODIFIED: Timer callback specifically for BULB exposures ***
        private static void OnBulbExposureTimerElapsed(object state)
        {
            lock (exposureLock)
            {
                if (cameraState != CameraStates.cameraExposing)
                {
                    LogMessage("OnBulbExposureTimerElapsed", $"Timer fired but state is {cameraState}. Ignoring.");
                    return;
                }
                LogMessage("OnBulbExposureTimerElapsed", $"BULB timer fired for {lastExposureDuration}s. Attempting to STOP exposure.");

                IntPtr shotOptPtr = IntPtr.Zero;
                long shotOptValue = 0;

                try
                {
                    if (!IsConnected)
                    {
                        LogMessage("OnBulbExposureTimerElapsed", "Disconnected during bulb exposure wait.");
                        cameraState = CameraStates.cameraError;
                        return;
                    }

                    // --- Send second Release command to STOP Bulb ---
                    shotOptPtr = Marshal.AllocHGlobal(sizeof(long));
                    Marshal.WriteInt64(shotOptPtr, shotOptValue);
                    LogMessage("OnBulbExposureTimerElapsed", $"Allocated plShotOpt for STOP at {shotOptPtr}");

                    // *** Use the defined BULBS2_OFF command ***
                    // *** Uses 0x0008 from XAPI.h ***
                    int releaseModeStop = FujifilmSdkWrapper.XSDK_RELEASE_N_BULBS2OFF;
                    LogMessage("OnBulbExposureTimerElapsed", $"Triggering exposure STOP via XSDK_Release (Mode: 0x{releaseModeStop:X}, Options Ptr: {shotOptPtr})...");
                    int releaseStatus;
                    int releaseResult = FujifilmSdkWrapper.XSDK_Release(hCamera, releaseModeStop, shotOptPtr, out releaseStatus);
                    LogMessage("OnBulbExposureTimerElapsed", $"XSDK_Release (STOP) returned {releaseResult}, status={releaseStatus}");

                    // Check for errors, but don't make it fatal if stopping fails, still check for image
                    if (releaseResult != FujifilmSdkWrapper.XSDK_COMPLETE)
                    {
                        LogMessage("OnBulbExposureTimerElapsed", $"WARNING: XSDK_Release (STOP) failed with result {releaseResult}. Image might not be available.");
                        // Optionally log the specific SDK error using CheckSdkError logic without throwing
                        // try { FujifilmSdkWrapper.CheckSdkError(hCamera, releaseResult, "XSDK_Release (Stop Bulb - Non-Fatal)"); } catch (Exception sdkEx) { LogMessage("OnBulbExposureTimerElapsed", $"SDK Error details on stop: {sdkEx.Message}");}
                    }
                    else
                    {
                        LogMessage("OnBulbExposureTimerElapsed", $"SDK Release command (STOP) sent successfully.");
                    }

                    // Add a small delay to allow camera to process the stop command and write buffer
                    LogMessage("OnBulbExposureTimerElapsed", "Adding short delay after stop command...");
                    System.Threading.Thread.Sleep(500); // 500ms delay, adjust if needed

                    // Now check for the image data using the helper method
                    CheckForImageData();

                }
                catch (Exception ex)
                {
                    LogMessage("OnBulbExposureTimerElapsed", $"Error stopping bulb exposure or checking image: {ex.Message}\n{ex.StackTrace}");
                    cameraState = CameraStates.cameraError;
                    imageReady = false;
                }
                finally
                {
                    // Free allocated shot options memory
                    if (shotOptPtr != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(shotOptPtr);
                        LogMessage("OnBulbExposureTimerElapsed", $"Freed plShotOpt memory at {shotOptPtr}");
                    }
                    // Ensure state moves away from exposing if not already error/idle
                    // CheckForImageData will set to Idle if successful, or Error if exception occurred there.
                    if (cameraState == CameraStates.cameraExposing)
                    {
                        LogMessage("OnBulbExposureTimerElapsed", "State still 'Exposing' after checks, setting to 'Idle'.");
                        cameraState = CameraStates.cameraIdle; // Move to idle if checks didn't change state
                    }
                }
            }
        }

        // *** MODIFIED: Original callback now only for TIMED exposures ***
        private static void OnExposureComplete(object state)
        {
            lock (exposureLock)
            {
                if (cameraState != CameraStates.cameraExposing)
                {
                    // This might happen if AbortExposure was called, or if it's a bulb exposure handled elsewhere
                    LogMessage("OnExposureComplete", $"Timer fired but state is {cameraState}. Ignoring.");
                    return;
                }
                LogMessage("OnExposureComplete", $"TIMED exposure timer fired for {lastExposureDuration}s. Checking for image availability.");
                CheckForImageData(); // Call the common image check logic
            }
        }

        // *** NEW: Helper method to check for image data ***
        private static void CheckForImageData()
        {
            // This logic was previously in OnExposureComplete
            lock (exposureLock) // Ensure lock is held
            {
                LogMessage("CheckForImageData", $"Checking image buffer.");
                try
                {
                    if (!IsConnected)
                    {
                        LogMessage("CheckForImageData", "Disconnected while checking for image.");
                        cameraState = CameraStates.cameraError;
                        imageReady = false;
                        return;
                    }
                    FujifilmSdkWrapper.XSDK_ImageInformation imgInfo;
                    int result = FujifilmSdkWrapper.XSDK_ReadImageInfo(hCamera, out imgInfo);
                    if (result == FujifilmSdkWrapper.XSDK_COMPLETE && imgInfo.lDataSize > 0)
                    {
                        LogMessage("CheckForImageData", $"Image detected in buffer via ReadImageInfo. Size: {imgInfo.lDataSize}, Format: {imgInfo.lFormat:X}");
                        imageReady = true;
                        cameraState = CameraStates.cameraIdle; // Set to Idle, ImageReady indicates download needed
                    }
                    else
                    {
                        // Log if no image found, but don't necessarily treat as error yet
                        LogMessage("CheckForImageData", $"No image data found via ReadImageInfo (Result: {result}). Client needs to poll ImageReady or retry download.");
                        // Keep state as Idle, ImageReady false. Client might retry Get ImageArray.
                        cameraState = CameraStates.cameraIdle;
                        imageReady = false;
                        // Optionally check for specific non-zero results from ReadImageInfo if they indicate errors vs just 'not ready'
                    }
                }
                catch (Exception ex)
                {
                    LogMessage("CheckForImageData", $"Error checking for image: {ex.Message}\n{ex.StackTrace}");
                    cameraState = CameraStates.cameraError;
                    imageReady = false;
                }
            }
        }


        /// <summary>
        /// Downloads RAW data using Fuji SDK and processes it using the C++/CLI LibRawWrapper.
        /// Stores the result in lastImageArray as int[,].
        /// </summary>
        private static void DownloadImageData()
        {
            LogMessage("DownloadImageData", "Starting RAW Bayer image download (Using C++/CLI Wrapper)...");
            cameraState = CameraStates.cameraDownload;
            lastImageArray = null;
            byte[] downloadBuffer = null;

            try
            {
                CheckConnected("DownloadImageData"); // Checks hCamera

                // --- Get Image Info from Fuji SDK ---
                FujifilmSdkWrapper.XSDK_ImageInformation imgInfo;
                LogMessage("DownloadImageData", "Calling XSDK_ReadImageInfo...");
                int result = FujifilmSdkWrapper.XSDK_ReadImageInfo(hCamera, out imgInfo);
                FujifilmSdkWrapper.CheckSdkError(hCamera, result, "XSDK_ReadImageInfo (DownloadImageData)");
                if (imgInfo.lDataSize <= 0) throw new DriverException($"XSDK_ReadImageInfo reported zero data size ({imgInfo.lDataSize}).");
                LogMessage("DownloadImageData", $"Expecting image: {imgInfo.lImagePixWidth}x{imgInfo.lImagePixHeight}, Size: {imgInfo.lDataSize}, Format: {imgInfo.lFormat:X}");

                // --- Check Format ---
                bool isRawFormat = (imgInfo.lFormat == FujifilmSdkWrapper.GFX100S_IMAGEQUALITY_RAW ||
                                    imgInfo.lFormat == FujifilmSdkWrapper.GFX100S_IMAGEQUALITY_RAW_FINE ||
                                    imgInfo.lFormat == FujifilmSdkWrapper.GFX100S_IMAGEQUALITY_RAW_NORMAL ||
                                    imgInfo.lFormat == FujifilmSdkWrapper.GFX100S_IMAGEQUALITY_RAW_SUPERFINE);

                if (!isRawFormat)
                {
                    throw new DriverException($"Unsupported image format {imgInfo.lFormat:X} for Bayer data retrieval. Ensure camera saves RAW.");
                }

                // --- Download using Fuji SDK ---
                downloadBuffer = new byte[imgInfo.lDataSize];
                GCHandle pinnedBuffer = GCHandle.Alloc(downloadBuffer, GCHandleType.Pinned);
                IntPtr bufferPtr = IntPtr.Zero;
                try
                {
                    bufferPtr = pinnedBuffer.AddrOfPinnedObject();
                    LogMessage("DownloadImageData", $"Calling XSDK_ReadImage for {imgInfo.lDataSize} bytes...");
                    Stopwatch sw = Stopwatch.StartNew();
                    result = FujifilmSdkWrapper.XSDK_ReadImage(hCamera, bufferPtr, (uint)imgInfo.lDataSize);
                    sw.Stop();
                    LogMessage("DownloadImageData", $"XSDK_ReadImage completed in {sw.ElapsedMilliseconds} ms. Result: {result}");
                    FujifilmSdkWrapper.CheckSdkError(hCamera, result, "XSDK_ReadImage");
                }
                finally
                {
                    if (pinnedBuffer.IsAllocated) pinnedBuffer.Free();
                }

                // --- Process with LibRaw using C++/CLI Wrapper ---
                LogMessage("DownloadImageData", $"Attempting to process {downloadBuffer.Length} bytes with LibRawWrapper...");
                Stopwatch procSw = Stopwatch.StartNew();

                ushort[,] bayerDataUShort = null; // Wrapper returns ushort[,]
                int width = 0;
                int height = 0;

                // Call the static method from the C++/CLI wrapper
                // Ensure the namespace Fujifilm.LibRawWrapper matches the C++/CLI project
                int wrapperResult = RawProcessor.ProcessRawBuffer(downloadBuffer, out bayerDataUShort, out width, out height);

                if (wrapperResult != 0) // LibRaw error codes are non-zero (LIBRAW_SUCCESS is 0)
                {
                    // TODO: Optionally get error string from libraw_strerror if needed (might require adding a P/Invoke for it back or in the wrapper)
                    LogMessage("DownloadImageData", $"LibRawWrapper.ProcessRawBuffer failed with LibRaw error code: {wrapperResult}");
                    throw new DriverException($"LibRaw processing failed via wrapper. LibRaw Error Code: {wrapperResult}");
                }

                if (bayerDataUShort == null || width <= 0 || height <= 0)
                {
                    LogMessage("DownloadImageData", "LibRawWrapper.ProcessRawBuffer returned success but output data/dimensions are invalid.");
                    throw new DriverException("LibRaw wrapper returned invalid data or dimensions despite success code.");
                }

                LogMessage("DownloadImageData", $"LibRawWrapper processed successfully. Dimensions: {width}x{height}");
                procSw.Stop();
                LogMessage("DownloadImageData", $"LibRawWrapper processing completed in {procSw.ElapsedMilliseconds} ms.");

                // --- Convert ushort[,] to int[,] for ASCOM ImageArray ---
                // ASCOM ImageArray standard is int[x,y] or int[,,]
                // C# arrays from GetLength are [rank0=rows=height, rank1=cols=width]
                if (width != bayerDataUShort.GetLength(1) || height != bayerDataUShort.GetLength(0))
                {
                    LogMessage("DownloadImageData", $"Dimension mismatch between reported ({width}x{height}) and array ({bayerDataUShort.GetLength(1)}x{bayerDataUShort.GetLength(0)}).");
                    // Adjust width/height or throw error? Let's trust the array dimensions.
                    height = bayerDataUShort.GetLength(0);
                    width = bayerDataUShort.GetLength(1);
                }

                int[,] bayerArrayInt = new int[width, height]; // ASCOM usually expects [width, height] or [X, Y]

                // Check dimensions match expected camera size (optional sanity check)
                if (width != cameraXSize || height != cameraYSize)
                {
                    LogMessage("DownloadImageData", $"WARNING: LibRaw dimensions ({width}x{height}) differ from expected ({cameraXSize}x{cameraYSize}). Using LibRaw dimensions.");
                }

                // Copy data, converting ushort to int
                // Assuming ASCOM ImageArray wants [X, Y] which means [width, height]
                for (int y = 0; y < height; y++) // Iterate rows (dimension 0 of C# array)
                {
                    for (int x = 0; x < width; x++) // Iterate columns (dimension 1 of C# array)
                    {
                        bayerArrayInt[x, y] = bayerDataUShort[y, x]; // Assign C#[row, col] to ASCOM[x, y]
                    }
                }
                LogMessage("DownloadImageData", $"Converted ushort[,] to int[,]");

                lastImageArray = bayerArrayInt; // Store the final int[,] array

            }
            catch (DllNotFoundException dllEx) // Catch errors finding the wrapper DLL or its dependencies (like libraw.dll)
            {
                LogMessage("DownloadImageData", $"DLL Not Found: {dllEx.Message}. Ensure LibRawWrapper.dll and libraw.dll are correctly deployed.");
                lastImageArray = null;
                cameraState = CameraStates.cameraError;
                throw new DriverException($"DLL Not Found: {dllEx.Message}. Ensure LibRawWrapper.dll and libraw.dll are correctly deployed.", dllEx);
            }
            catch (Exception ex)
            {
                LogMessage("DownloadImageData", $"Image download/processing failed: {ex.Message}\n{ex.StackTrace}");
                lastImageArray = null;
                cameraState = CameraStates.cameraError;
                throw; // Rethrow original exception
            }
            finally
            {
                // No LibRaw recycle/close needed here - handled by wrapper
                if (cameraState != CameraStates.cameraError) { cameraState = CameraStates.cameraIdle; }
                downloadBuffer = null; // Allow GC
            }
        }


        #endregion
    }
}
