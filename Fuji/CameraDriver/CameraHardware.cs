// ASCOM Camera hardware class for ScdouglasFujifilm
// Author: S. Douglas <your@email.here>
// Description: Interfaces with the Fujifilm X SDK to control Fujifilm cameras.
// Implements: ASCOM Camera interface version: 3

using ASCOM;
using ASCOM.Astrometry.AstroUtils;
using ASCOM.DeviceInterface;
using ASCOM.Utilities;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices; // Needed for MemoryMarshal, GCHandle, Marshal
using System.Threading;
using System.Windows.Forms;

// Using direct P/Invoke via the Libraw class with corrected namespace
using ASCOM.LocalServer.NativeLibRaw; // Corrected namespace

namespace ASCOM.ScdouglasFujifilm.Camera
{
    /// <summary>
    /// Wraps the Fujifilm X SDK C-style DLL functions using P/Invoke.
    /// </summary>
    internal static class FujifilmSdkWrapper
    {
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

        // Release Modes (From XAPI.H & XAPIOpt.h)
        public const int XSDK_RELEASE_SHOOT = 0x0100; // Just shoot (S2?)
        public const int XSDK_RELEASE_N_S1OFF = 0x0004; // Option flag
        public const int XSDK_RELEASE_SHOOT_S1OFF = (XSDK_RELEASE_SHOOT | XSDK_RELEASE_N_S1OFF); // 0x0104 = 260
        public const int SDK_RELEASE_MODE_S1ONLY = 0x0001; // S1 Press Only (Focus/AE Lock)
        public const int SDK_RELEASE_MODE_S2ONLY = 0x0002; // S2 Press Only (Release without S1)
        public const int SDK_RELEASE_MODE_S1S2 = 0x0003;   // S1 + S2 Press

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

        // LibRaw P/Invokes are now in the separate Libraw class

        #region Helper Methods

        internal delegate int CapFunctionDelegate(IntPtr hCamera, out int count, IntPtr buffer);
        internal delegate int CapFunctionBulbDelegate(IntPtr hCamera, out int count, IntPtr buffer, out int bulbCapable);

        internal static int[] GetIntArrayFromSdk(IntPtr hCamera, CapFunctionDelegate capFunc)
        {
            int count;
            LogMessageStatic("GetIntArrayFromSdk", $"Calling CapFunc (GetCount)...");
            int result = capFunc(hCamera, out count, IntPtr.Zero);
            LogMessageStatic("GetIntArrayFromSdk", $"CapFunc returned count={count}, result={result}");
            CheckSdkError(hCamera, result, $"Capability Function (GetCount)");
            if (count <= 0) return new int[0];

            IntPtr ptr = Marshal.AllocHGlobal(sizeof(int) * count);
            try
            {
                LogMessageStatic("GetIntArrayFromSdk", $"Calling CapFunc (GetData)...");
                result = capFunc(hCamera, out count, ptr);
                LogMessageStatic("GetIntArrayFromSdk", $"CapFunc returned result={result}");
                CheckSdkError(hCamera, result, $"Capability Function (GetData)");
                int[] array = new int[count];
                Marshal.Copy(ptr, array, 0, count);
                return array;
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }

        internal static int[] GetIntArrayFromSdkShutterSpeed(IntPtr hCamera, out int bulbCapable)
        {
            int count;
            LogMessageStatic("GetIntArrayFromSdkShutterSpeed", $"Calling XSDK_CapShutterSpeed (GetCount)...");
            int result = XSDK_CapShutterSpeed(hCamera, out count, IntPtr.Zero, out bulbCapable);
            LogMessageStatic("GetIntArrayFromSdkShutterSpeed", $"XSDK_CapShutterSpeed returned count={count}, bulbCapable={bulbCapable}, result={result}");
            CheckSdkError(hCamera, result, $"XSDK_CapShutterSpeed (GetCount)");
            if (count <= 0) return new int[0];

            IntPtr ptr = Marshal.AllocHGlobal(sizeof(int) * count);
            try
            {
                LogMessageStatic("GetIntArrayFromSdkShutterSpeed", $"Calling XSDK_CapShutterSpeed (GetData)...");
                result = XSDK_CapShutterSpeed(hCamera, out count, ptr, out bulbCapable);
                LogMessageStatic("GetIntArrayFromSdkShutterSpeed", $"XSDK_CapShutterSpeed returned result={result}");
                CheckSdkError(hCamera, result, $"XSDK_CapShutterSpeed (GetData)");
                int[] array = new int[count];
                Marshal.Copy(ptr, array, 0, count);
                return array;
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }

        internal static int[] GetIntArrayFromSdkSensitivity(IntPtr hCamera, int lDR)
        {
            int count;
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
                        // If hCamera is null (e.g., during Init/Detect), use sdkResult directly
                        errCode = sdkResult;
                    }
                }
                catch { errCode = sdkResult; } // Fallback if GetErrorNumber fails

                // Convert error code to string name
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
            // Use Debug.WriteLine for simplicity if TraceLogger isn't available here
            Debug.WriteLine($"[{DateTime.Now:HH:mm:ss.fff}] {identifier}: {message}");
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
        // ... (Standard ASCOM fields) ...
        internal const string traceStateProfileName = "Trace Level";
        internal const string traceStateDefault = "true";
        internal const string cameraNameProfileName = "Camera Name";
        internal const string cameraNameDefault = "";

        // Driver state
        private static string DriverProgId = "";
        private static string DriverDescription = "";
        internal static string cameraName;
        private static bool connectedState;
        private static bool sdkInitialized = false;
        private static IntPtr hCamera = IntPtr.Zero; // Fuji SDK camera handle
        private static IntPtr libraw_handle = IntPtr.Zero; // LibRaw context handle
        private static object hardwareLock = new object();
        private static bool runOnce = false;

        // ASCOM Utilities
        internal static Util utilities;
        internal static AstroUtils astroUtilities;
        internal static TraceLogger tl;

        // Camera Info & Capabilities
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

        // Exposure related state
        private static DateTime exposureStartTime;
        private static double lastExposureDuration;
        private static bool imageReady = false;
        private static System.Threading.Timer exposureTimer;
        private static object exposureLock = new object();

        // Cached capabilities
        private static List<int> supportedSensitivities = new List<int>();
        private static int minSensitivity = 100;
        private static int maxSensitivity = 12800;
        private static Dictionary<int, double> sdkShutterSpeedToDuration = new Dictionary<int, double>();
        private static Dictionary<double, int> durationToSdkShutterSpeed = new Dictionary<double, int>();
        private static List<int> supportedShutterSpeeds = new List<int>();
        private static double minExposure = 0.0001;
        private static double maxExposure = 3600.0;
        private static bool bulbCapable = true;

        // Image buffer
        private static object lastImageArray = null;

        #endregion

        #region Initialisation and Dispose

        static CameraHardware()
        {
            try
            {
                // Initialize TraceLogger first
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
                        ReadProfile(); // Read profile after setting ProgId
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
                        libraw_handle = IntPtr.Zero; // Ensure LibRaw handle is also reset
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
                // Ensure disconnected (this will call Libraw.libraw_close)
                if (Connected) { try { Connected = false; } catch (Exception ex) { LogMessage("Dispose", $"Exception during disconnect in Dispose: {ex.Message}"); } }

                // Ensure Fuji SDK is exited if it was initialized
                if (sdkInitialized) { try { FujifilmSdkWrapper.XSDK_Exit(); sdkInitialized = false; LogMessage("Dispose", "Fuji SDK Exited."); } catch (Exception ex) { LogMessage("Dispose", $"Exception during XSDK_Exit: {ex.Message}"); } }

                // Dispose managed resources
                utilities?.Dispose(); utilities = null;
                astroUtilities?.Dispose(); astroUtilities = null;
                exposureTimer?.Dispose(); exposureTimer = null;
                if (tl != null) { tl.Enabled = false; tl.Dispose(); tl = null; }
                LogMessage("Dispose", $"CameraHardware disposal complete.");
            }
        }

        #endregion

        #region ASCOM Common Properties and Methods

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

        /// <summary>Sets or Gets the connected state of the hardware</summary>
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
                        libraw_handle = IntPtr.Zero; // Reset LibRaw handle on connect attempt
                        sensorName = "Unknown";
                        IntPtr apiCodeBufferPtr = IntPtr.Zero;
                        try
                        {
                            // --- Initialize Fuji SDK ---
                            LogMessage("Connected Set", "Step 1: Initializing Fuji SDK (if needed)...");
                            if (!sdkInitialized)
                            {
                                int initResult = FujifilmSdkWrapper.XSDK_Init(IntPtr.Zero);
                                FujifilmSdkWrapper.CheckSdkError(IntPtr.Zero, initResult, "XSDK_Init");
                                sdkInitialized = true;
                                LogMessage("Connected Set", "Fuji SDK Initialized.");
                            }
                            else { LogMessage("Connected Set", "Fuji SDK already initialized."); }

                            // --- Initialize LibRaw ---
                            LogMessage("Connected Set", "Step 1.5: Initializing LibRaw...");
                            libraw_handle = Libraw.libraw_init(0); // Flags = 0
                            if (libraw_handle == IntPtr.Zero)
                            {
                                throw new DriverException("libraw_init failed. Ensure native libraw.dll is deployed correctly.");
                            }
                            LogMessage("Connected Set", "LibRaw Initialized.");

                            // --- Detect and Open Fuji Camera ---
                            LogMessage("Connected Set", "Step 2: Detecting cameras...");
                            // ... (XSDK_Detect logic as before) ...
                            int cameraCount;
                            int detectResult = FujifilmSdkWrapper.XSDK_Detect(FujifilmSdkWrapper.XSDK_DSC_IF_USB, IntPtr.Zero, IntPtr.Zero, out cameraCount);
                            FujifilmSdkWrapper.CheckSdkError(IntPtr.Zero, detectResult, "XSDK_Detect");
                            LogMessage("Connected Set", $"Detected {cameraCount} camera(s).");
                            if (cameraCount <= 0) throw new ASCOM.NotConnectedException("No Fujifilm cameras detected via USB.");

                            string deviceId = "ENUM:0"; // TODO: Implement camera selection
                            LogMessage("Connected Set", $"Step 3: Opening camera session for '{deviceId}'...");
                            int openResult = FujifilmSdkWrapper.XSDK_OpenEx(deviceId, out hCamera, out int cameraMode, IntPtr.Zero);
                            FujifilmSdkWrapper.CheckSdkError(IntPtr.Zero, openResult, $"XSDK_OpenEx ({deviceId})");
                            LogMessage("Connected Set", $"Camera session opened. Handle: {hCamera}, Mode: {cameraMode}");
                            if (hCamera == IntPtr.Zero) throw new ASCOM.DriverException("Failed to open camera session (handle is null).");

                            // --- Configure Fuji Camera ---
                            LogMessage("Connected Set", "Step 4: Setting PC Priority Mode...");
                            // ... (SetPriorityMode, SetMode(Manual), GetMode verification as before) ...
                            int priorityResult = FujifilmSdkWrapper.XSDK_SetPriorityMode(hCamera, FujifilmSdkWrapper.XSDK_PRIORITY_PC);
                            FujifilmSdkWrapper.CheckSdkError(hCamera, priorityResult, "XSDK_SetPriorityMode");
                            LogMessage("Connected Set", "PC Priority Mode set.");

                            LogMessage("Connected Set", "Step 4.5: Setting Exposure Mode to Manual (M)...");
                            int modeResult = FujifilmSdkWrapper.XSDK_SetMode(hCamera, FujifilmSdkWrapper.GFX100S_MODE_M);
                            FujifilmSdkWrapper.CheckSdkError(hCamera, modeResult, "XSDK_SetMode(Manual)");
                            LogMessage("Connected Set", "Exposure Mode set to Manual (M).");
                            // (Optional GetMode verification here)

                            // --- Get Device Info ---
                            LogMessage("Connected Set", "Step 5: Getting device info...");
                            // ... (GetDeviceInfoEx logic as before) ...
                            try
                            {
                                FujifilmSdkWrapper.XSDK_DeviceInformation deviceInfo;
                                int numApiCodes = 0;
                                int infoResult;
                                infoResult = FujifilmSdkWrapper.XSDK_GetDeviceInfoEx(hCamera, out deviceInfo, out numApiCodes, IntPtr.Zero); // Call 1
                                FujifilmSdkWrapper.CheckSdkError(hCamera, infoResult, "XSDK_GetDeviceInfoEx (GetCount)");
                                if (numApiCodes < 0) numApiCodes = 0;
                                int bufferSize = numApiCodes * sizeof(int);
                                if (bufferSize > 0) apiCodeBufferPtr = Marshal.AllocHGlobal(bufferSize); else apiCodeBufferPtr = IntPtr.Zero;
                                infoResult = FujifilmSdkWrapper.XSDK_GetDeviceInfoEx(hCamera, out deviceInfo, out numApiCodes, apiCodeBufferPtr); // Call 2
                                FujifilmSdkWrapper.CheckSdkError(hCamera, infoResult, "XSDK_GetDeviceInfoEx (GetData)");
                                sensorName = deviceInfo.strProduct ?? "Unknown Model";
                                LogMessage("Connected Set", $"Retrieved Product Name: {sensorName}");
                            }
                            finally { if (apiCodeBufferPtr != IntPtr.Zero) Marshal.FreeHGlobal(apiCodeBufferPtr); }


                            // --- Set Connected State and Cache Capabilities ---
                            connectedState = true; // Set state true *before* caching
                            LogMessage("Connected Set", $"State before CacheCameraCapabilities: connectedState={connectedState}, hCamera={hCamera}, libraw_handle={libraw_handle}");
                            LogMessage("Connected Set", "Step 6: Caching camera capabilities...");
                            CacheCameraCapabilities();
                            LogMessage("Connected Set", "Capabilities cached.");
                            LogMessage("Connected Set", "Hardware Connected Successfully.");
                        }
                        catch (Exception ex)
                        {
                            LogMessage("Connected Set", $"HARDWARE CONNECTION FAILED: {ex.Message}\n{ex.StackTrace}");
                            // Cleanup on failure
                            if (hCamera != IntPtr.Zero) { try { FujifilmSdkWrapper.XSDK_Close(hCamera); } catch { } hCamera = IntPtr.Zero; }
                            if (libraw_handle != IntPtr.Zero) { try { Libraw.libraw_close(libraw_handle); } catch { } libraw_handle = IntPtr.Zero; } // Close LibRaw on failure too
                            connectedState = false;
                            sensorName = "Unknown";
                            throw; // Rethrow exception to ASCOM client
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
                            finally { hCamera = IntPtr.Zero; }
                        }
                        else { LogMessage("Connected Set", "Fuji SDK Already disconnected (no handle)."); }

                        // Close LibRaw context if it was initialized
                        if (libraw_handle != IntPtr.Zero)
                        {
                            try
                            {
                                LogMessage("Connected Set", $"Closing LibRaw handle {libraw_handle}...");
                                Libraw.libraw_close(libraw_handle);
                                LogMessage("Connected Set", "LibRaw context closed.");
                            }
                            catch (Exception ex) { LogMessage("Connected Set", $"Exception during libraw_close: {ex.Message}"); }
                            finally { libraw_handle = IntPtr.Zero; }
                        }
                        else { LogMessage("Connected Set", "LibRaw already closed (no handle)."); }

                        connectedState = false;
                        sensorName = "Unknown";
                        LogMessage("Connected Set", "Hardware Disconnected.");
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

                lock (hardwareLock)
                {
                    LogMessage("Gain Set", $"Calling XSDK_SetSensitivity(hCamera={hCamera}, value={value})...");
                    int result = FujifilmSdkWrapper.XSDK_SetSensitivity(hCamera, value);
                    LogMessage("Gain Set", $"XSDK_SetSensitivity returned {result}");
                    FujifilmSdkWrapper.CheckSdkError(hCamera, result, "XSDK_SetSensitivity");
                    LogMessage("Gain Set", $"SDK Sensitivity set to: {value}");
                }
            }
        }

        public static short GainMax => (short)maxSensitivity;
        public static short GainMin => (short)minSensitivity;
        public static ArrayList Gains
        {
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
                    if (lastImageArray == null) { LogMessage("ImageArray Get", "ImageReady is true but image data is null. Attempting download..."); DownloadImageData(); }
                    if (lastImageArray == null) { LogMessage("ImageArray Get", "Error: DownloadImageData failed to produce image data."); cameraState = CameraStates.cameraError; throw new DriverException("Failed to retrieve image data after download attempt."); }

                    LogMessage("ImageArray Get", "Returning image array.");
                    object imageToReturn = lastImageArray;
                    lastImageArray = null; // Clear buffer after retrieval
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
                    // Simple progress indication
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
        public static SensorType SensorType => SensorType.RGGB;
        public static string SensorName => sensorName;
        public static double SetCCDTemperature { get => throw new PropertyNotImplementedException("SetCCDTemperature", false); set => throw new PropertyNotImplementedException("SetCCDTemperature", true); }

        public static void StartExposure(double duration, bool light)
        {
            CheckConnected("StartExposure");
            lock (exposureLock)
            {
                if (cameraState != CameraStates.cameraIdle) throw new InvalidOperationException($"Camera not idle. State: {cameraState}");
                LogMessage("StartExposure", $"Request: Duration={duration}s, Light Frame={light}");
                if (duration < minExposure || duration > maxExposure) throw new InvalidValueException("StartExposure Duration", duration.ToString(), $"Range {minExposure} to {maxExposure}");

                IntPtr shotOptPtr = IntPtr.Zero;
                long shotOptValue = 0;

                try
                {
                    // ... (Log current camera state as before) ...

                    LogMessage("StartExposure", "Converting duration to SDK code...");
                    int sdkShutterSpeed = DurationToSdkShutterSpeed(duration);
                    int isBulb = (sdkShutterSpeed == FujifilmSdkWrapper.XSDK_SHUTTER_BULB) ? 1 : 0;
                    if (isBulb == 1 && !bulbCapable) throw new InvalidValueException("StartExposure Duration", duration.ToString(), "Bulb exposure requested but camera does not support Bulb via SDK.");

                    LogMessage("StartExposure", $"Setting SDK Shutter Speed Code: {sdkShutterSpeed}, Bulb Flag: {isBulb}");
                    int setResult = FujifilmSdkWrapper.XSDK_SetShutterSpeed(hCamera, sdkShutterSpeed, isBulb);
                    FujifilmSdkWrapper.CheckSdkError(hCamera, setResult, "XSDK_SetShutterSpeed");

                    shotOptPtr = Marshal.AllocHGlobal(sizeof(long));
                    Marshal.WriteInt64(shotOptPtr, shotOptValue);
                    LogMessage("StartExposure", $"Allocated and initialized plShotOpt (long*) at {shotOptPtr} with value {shotOptValue}");

                    int releaseMode = FujifilmSdkWrapper.XSDK_RELEASE_SHOOT_S1OFF;
                    LogMessage("StartExposure", $"Triggering exposure via XSDK_Release (Mode: {releaseMode}, Options Ptr: {shotOptPtr})...");
                    int releaseStatus;
                    int releaseResult = FujifilmSdkWrapper.XSDK_Release(hCamera, releaseMode, shotOptPtr, out releaseStatus);
                    LogMessage("StartExposure", $"XSDK_Release returned {releaseResult}, status={releaseStatus}");
                    FujifilmSdkWrapper.CheckSdkError(hCamera, releaseResult, "XSDK_Release");

                    LogMessage("StartExposure", $"SDK Release command sent.");

                    cameraState = CameraStates.cameraExposing;
                    exposureStartTime = DateTime.UtcNow;
                    lastExposureDuration = duration;
                    imageReady = false;
                    lastImageArray = null;

                    int exposureMillis = (int)(duration * 1000);
                    int bufferMillis = 2000;
                    exposureTimer?.Dispose();
                    exposureTimer = new System.Threading.Timer(OnExposureComplete, null, exposureMillis + bufferMillis, Timeout.Infinite);
                    LogMessage("StartExposure", $"Exposure started. Monitoring timer set for {exposureMillis + bufferMillis} ms.");
                }
                catch (Exception ex)
                {
                    LogMessage("StartExposure", $"StartExposure failed: {ex.Message}\n{ex.StackTrace}");
                    cameraState = CameraStates.cameraError;
                    throw;
                }
                finally
                {
                    if (shotOptPtr != IntPtr.Zero)
                    {
                        Marshal.FreeHGlobal(shotOptPtr);
                        LogMessage("StartExposure", $"Freed plShotOpt memory at {shotOptPtr}");
                    }
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
                Profile driverProfile = new Profile { DeviceType = "Camera" };
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
                Profile driverProfile = new Profile { DeviceType = "Camera" };
                if (tl != null) driverProfile.WriteValue(DriverProgId, traceStateProfileName, tl.Enabled.ToString());
                // driverProfile.WriteValue(DriverProgId, cameraNameProfileName, cameraName);
                LogMessage("WriteProfile", $"Trace state saved: {tl?.Enabled}");
            }
            catch (Exception ex) { LogMessage("WriteProfile", $"Error writing profile: {ex.Message}"); }
        }


        internal static void LogMessage(string identifier, string message) { tl?.LogMessageCrLf(identifier, message); }
        internal static void LogMessage(string identifier, string format, params object[] args) { tl?.LogMessageCrLf(identifier, string.Format(format, args)); }
        // Check LibRaw handle as well for IsConnected
        private static bool IsConnected => connectedState && hCamera != IntPtr.Zero && libraw_handle != IntPtr.Zero;

        private static void CacheCameraCapabilities()
        {
            LogMessage("CacheCameraCapabilities", $"Entering CacheCameraCapabilities. State: connectedState={connectedState}, hCamera={hCamera}, libraw_handle={libraw_handle}, IsConnected={IsConnected}");

            if (!IsConnected)
            {
                LogMessage("CacheCameraCapabilities", "Exiting CacheCameraCapabilities early because IsConnected is false.");
                return;
            }

            LogMessage("CacheCameraCapabilities", "Caching camera capabilities (Simplified Version - Skipping SDK calls)...");
            PopulateShutterSpeedMaps();
            LogMessage("CacheCameraCapabilities", $"After PopulateShutterSpeedMaps, durationToSdkShutterSpeed.Count = {durationToSdkShutterSpeed.Count}");

            // Using defaults as SDK calls were skipped in previous versions - keep this simple for now
            supportedSensitivities.Clear();
            minSensitivity = 100;
            maxSensitivity = 12800;
            supportedShutterSpeeds.Clear();
            bulbCapable = true;

            if (durationToSdkShutterSpeed.Count > 0)
            {
                List<double> durations = new List<double>(durationToSdkShutterSpeed.Keys);
                durations.Sort();
                minExposure = durations[0];
                maxExposure = durations[durations.Count - 1];
            }
            else
            {
                LogMessage("CacheCameraCapabilities", "WARNING: durationToSdkShutterSpeed map is empty! Using fallback exposure range.");
                minExposure = 1.0 / 8000.0; maxExposure = 60.0 * 60.0;
            }
            LogMessage("CacheCameraCapabilities", $"Using capabilities: Min Sensitivity={minSensitivity}, Max Sensitivity={maxSensitivity}, Min Exposure={minExposure}s, Max Exposure={maxExposure}s, Bulb Capable={bulbCapable}");
            LogMessage("CacheCameraCapabilities", "Simplified capability caching finished.");
        }


        private static void PopulateShutterSpeedMaps()
        {
            // Ensure maps are clear before populating
            sdkShutterSpeedToDuration.Clear();
            durationToSdkShutterSpeed.Clear();
            LogMessage("PopulateShutterSpeedMaps", "Populating shutter speed maps based on SDK PDF...");
            // --- MAPPINGS BASED ON SDK PDF pp. 91-95 ---
            // (Keep the AddSdkShutterMapping calls as before)
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
            LogMessage("DurationToSdkShutterSpeed", $"Attempting to map duration: {duration}s");
            if (durationToSdkShutterSpeed.Count == 0)
            {
                LogMessage("DurationToSdkShutterSpeed", "Error: durationToSdkShutterSpeed map is empty!");
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
                if (bulbCapable && duration > maxExposure)
                {
                    LogMessage("DurationToSdkShutterSpeed", $"Duration {duration}s > max non-bulb ({maxExposure}s), mapping to BULB (-1).");
                    return FujifilmSdkWrapper.XSDK_SHUTTER_BULB;
                }
                LogMessage("DurationToSdkShutterSpeed", $"Duration {duration}s is too far from nearest supported {closestDuration}s (Diff: {minDiff}, Tol: {tolerance}).");
                throw new InvalidValueException($"Requested duration {duration}s is not supported or too far from nearest value {closestDuration}s.");
            }
        }

        private static void OnExposureComplete(object state)
        {
            lock (exposureLock)
            {
                if (cameraState != CameraStates.cameraExposing) { LogMessage("OnExposureComplete", $"Timer fired but state is {cameraState}. Ignoring."); return; }
                LogMessage("OnExposureComplete", $"Timer fired. Checking for image availability.");
                try
                {
                    if (!IsConnected) { LogMessage("OnExposureComplete", "Disconnected during exposure wait."); cameraState = CameraStates.cameraError; return; }
                    FujifilmSdkWrapper.XSDK_ImageInformation imgInfo;
                    int result = FujifilmSdkWrapper.XSDK_ReadImageInfo(hCamera, out imgInfo);
                    if (result == FujifilmSdkWrapper.XSDK_COMPLETE && imgInfo.lDataSize > 0)
                    {
                        LogMessage("OnExposureComplete", $"Image detected in buffer via ReadImageInfo. Size: {imgInfo.lDataSize}, Format: {imgInfo.lFormat:X}");
                        imageReady = true;
                        cameraState = CameraStates.cameraIdle; // Ready for download
                    }
                    else
                    {
                        LogMessage("OnExposureComplete", $"No image data found via ReadImageInfo (Result: {result}). Client needs to poll ImageReady.");
                        // Keep polling, don't change state yet unless error
                        if (result != FujifilmSdkWrapper.XSDK_COMPLETE)
                        {
                            // Maybe log an error but don't necessarily set cameraState to error yet
                            // FujifilmSdkWrapper.CheckSdkError(hCamera, result, "XSDK_ReadImageInfo (Polling)");
                            LogMessage("OnExposureComplete", $"Polling ReadImageInfo failed (Result: {result}). Will retry on next ImageReady check.");
                            // Decide if this should be a fatal error or if polling should continue
                            // For now, assume polling continues. If it persists, ImageArray will fail.
                        }
                        cameraState = CameraStates.cameraIdle; // Or maybe keep exposing if timer fired early? Needs thought. Let's set idle.
                        imageReady = false;

                    }
                }
                catch (Exception ex) { LogMessage("OnExposureComplete", $"Error checking for image: {ex.Message}"); cameraState = CameraStates.cameraError; imageReady = false; }
            }
        }

        /// <summary>
        /// Downloads and extracts RAW Bayer data from the camera using direct LibRaw P/Invoke calls.
        /// Stores the result in lastImageArray as int[,].
        /// Assumes libraw_handle is valid (initialized during connect).
        /// </summary>
        private static void DownloadImageData()
        {
            LogMessage("DownloadImageData", "Starting RAW Bayer image download (Direct P/Invoke)...");
            cameraState = CameraStates.cameraDownload;
            lastImageArray = null;
            byte[] downloadBuffer = null;
            // LibRaw handle is now managed globally (libraw_handle field)

            // Temporary handle for recycle logic within this method's scope
            IntPtr current_libraw_handle = IntPtr.Zero;

            try
            {
                CheckConnected("DownloadImageData"); // Checks hCamera AND libraw_handle implicitly via IsConnected

                // Ensure global LibRaw handle is valid before proceeding
                if (libraw_handle == IntPtr.Zero)
                {
                    throw new DriverException("LibRaw handle is not initialized. Connect sequence failed?");
                }
                current_libraw_handle = libraw_handle; // Use the global handle

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

                // --- Process with LibRaw using direct P/Invoke ---
                LogMessage("DownloadImageData", $"Attempting to process {downloadBuffer.Length} bytes with LibRaw (Direct)...");
                Stopwatch procSw = Stopwatch.StartNew();

                // 1. Open Buffer (using existing global handle)
                GCHandle pinnedDownloadBuffer = GCHandle.Alloc(downloadBuffer, GCHandleType.Pinned);
                IntPtr downloadBufferPtr = IntPtr.Zero;
                try
                {
                    downloadBufferPtr = pinnedDownloadBuffer.AddrOfPinnedObject();
                    int openResult = Libraw.libraw_open_buffer(current_libraw_handle, downloadBufferPtr, downloadBuffer.Length);
                    if (openResult != 0)
                    {
                        string errorMsg = Marshal.PtrToStringAnsi(Libraw.libraw_strerror(openResult)) ?? "Unknown LibRaw Error";
                        throw new DriverException($"libraw_open_buffer failed: {errorMsg} (Code: {openResult})");
                    }
                    LogMessage("DownloadImageData", "libraw_open_buffer successful.");
                }
                finally
                {
                    if (pinnedDownloadBuffer.IsAllocated) pinnedDownloadBuffer.Free();
                }

                // 2. Unpack
                int unpackResult = Libraw.libraw_unpack(current_libraw_handle);
                if (unpackResult != 0)
                {
                    string errorMsg = Marshal.PtrToStringAnsi(Libraw.libraw_strerror(unpackResult)) ?? "Unknown LibRaw Error";
                    throw new DriverException($"libraw_unpack failed: {errorMsg} (Code: {unpackResult})");
                }
                LogMessage("DownloadImageData", "libraw_unpack successful.");

                // 3. Get Dimensions
                int rawWidth = Libraw.libraw_get_raw_width(current_libraw_handle);
                int rawHeight = Libraw.libraw_get_raw_height(current_libraw_handle);
                LogMessage("DownloadImageData", $"LibRaw Raw Dimensions: {rawWidth}x{rawHeight}");
                if (rawWidth <= 0 || rawHeight <= 0) throw new DriverException("LibRaw returned invalid raw dimensions.");

                // 4. Get Raw Data Pointer via Structure Marshalling
                // *** CORRECTED: Use generic overload Marshal.PtrToStructure<T> ***
                Libraw.libraw_data_t dataStruct = Marshal.PtrToStructure<Libraw.libraw_data_t>(current_libraw_handle);
                // Access the pointer
                IntPtr rawDataPtr = dataStruct.rawdata.raw_image;
                LogMessage("DownloadImageData", $"Got raw image pointer via structure access: {rawDataPtr}");

                if (rawDataPtr == IntPtr.Zero)
                {
                    throw new DriverException("LibRaw structure did not contain a valid raw image pointer after unpack.");
                }

                int pixelCount = rawWidth * rawHeight;
                int[,] bayerArray = new int[rawWidth, rawHeight];

                // 5. Copy Data using Unsafe Code
                LogMessage("DownloadImageData", "Attempting to copy raw data using unsafe pointer access...");
                try
                {
                    unsafe // Ensure unsafe context
                    {
                        ushort* pRaw = (ushort*)rawDataPtr.ToPointer();
                        if (pRaw == null) throw new NullReferenceException("Failed to get pointer from rawDataPtr.");

                        long copiedPixels = 0;
                        for (int y = 0; y < rawHeight; y++)
                        {
                            for (int x = 0; x < rawWidth; x++)
                            {
                                int sourceIndex = y * rawWidth + x;
                                // Basic bounds check (optional but safer)
                                // if (sourceIndex >= pixelCount) throw new IndexOutOfRangeException(...);
                                bayerArray[x, y] = pRaw[sourceIndex];
                                copiedPixels++;
                            }
                        }
                        LogMessage("DownloadImageData", $"Successfully copied {copiedPixels} raw Bayer data values.");
                    }
                }
                catch (Exception unsafeEx)
                {
                    LogMessage("DownloadImageData", $"Unsafe copy failed: {unsafeEx.Message}\n{unsafeEx.StackTrace}");
                    throw new DriverException("Failed to copy raw image data using unsafe pointer access.", unsafeEx);
                }

                lastImageArray = bayerArray; // Store the raw Bayer data
                procSw.Stop();
                LogMessage("DownloadImageData", $"LibRaw direct processing completed in {procSw.ElapsedMilliseconds} ms.");

            }
            catch (DllNotFoundException dllEx)
            {
                LogMessage("DownloadImageData", $"Native DLL Not Found: {dllEx.Message}. Ensure libraw.dll and dependencies are in the output directory.");
                lastImageArray = null;
                cameraState = CameraStates.cameraError;
                throw new DriverException($"Native DLL Not Found: {dllEx.Message}. Ensure libraw.dll and dependencies are correctly deployed.", dllEx);
            }
            // Keep EntryPointNotFoundException catch in case underlying structure access fails similarly
            catch (EntryPointNotFoundException epnfEx) { LogMessage("DownloadImageData", $"LibRaw EntryPointNotFoundException: {epnfEx.Message}. This likely means the libraw.dll version does NOT match the expected API OR structure access failed. Ensure native DLLs are correct."); lastImageArray = null; cameraState = CameraStates.cameraError; throw new DriverException($"LibRaw EntryPointNotFoundException: {epnfEx.Message}. Mismatched native libraw.dll version or structure access failed.", epnfEx); }
            catch (Exception ex)
            {
                LogMessage("DownloadImageData", $"Image download/processing failed: {ex.Message}\n{ex.StackTrace}");
                lastImageArray = null;
                cameraState = CameraStates.cameraError;
                throw; // Rethrow original exception
            }
            finally
            {
                // 6. Recycle LibRaw resources for this specific image load
                // Use the handle we know was valid for this operation
                if (current_libraw_handle != IntPtr.Zero)
                {
                    Libraw.libraw_recycle(current_libraw_handle);
                    LogMessage("DownloadImageData", "Called libraw_recycle.");
                }
                // Do NOT call libraw_close here, only on driver disconnect/dispose

                if (cameraState != CameraStates.cameraError) { cameraState = CameraStates.cameraIdle; }
                downloadBuffer = null; // Allow GC
                GC.Collect(); // Optional, consider if memory pressure is high
            }
        }


        #endregion
    }
}
