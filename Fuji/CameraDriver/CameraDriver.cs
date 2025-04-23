// ASCOM Camera driver for ScdouglasFujifilm
//
// Description:	 ASCOM driver for Fujifilm GFX/X series cameras using the Fujifilm X SDK. Controls basic camera functions like exposure and image download.
//
// Implements:	ASCOM Camera interface version: 3
// Author:		S. Douglas <your@email.here> // Replace with your actual details
//

using ASCOM;
using ASCOM.DeviceInterface;
using ASCOM.LocalServer;
using ASCOM.Utilities;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ASCOM.ScdouglasFujifilm.Camera
{
    //
    // This code is mostly a presentation layer for the functionality in the CameraHardware class. You should not need to change the contents of this file very much, if at all.
    // Most customisation will be in the CameraHardware class, which is shared by all instances of the driver, and which must handle all aspects of communicating with your device.
    //
    // Your driver's DeviceID is ASCOM.ScdouglasFujifilm.Camera
    //
    // The COM Guid attribute sets the CLSID for ASCOM.ScdouglasFujifilm.Camera
    // The COM ClassInterface/None attribute prevents an empty interface called _ScdouglasFujifilm from being created and used as the [default] interface
    //

    /// <summary>
    /// ASCOM Camera Driver for ScdouglasFujifilm.
    /// </summary>
    [ComVisible(true)]
    [Guid("b8ca541e-754e-4215-962e-2f2bd50bcaad")] // Keep this GUID unless you need to re-register explicitly
    [ProgId("ASCOM.ScdouglasFujifilm.Camera")]
    [ServedClassName("Fujifilm Camera (Scdouglas)")] // Driver description that appears in the Chooser
    [ClassInterface(ClassInterfaceType.None)]
    public class Camera : ReferenceCountedObjectBase, ICameraV3, IDisposable
    {
        internal static string DriverProgId; // ASCOM DeviceID (COM ProgID) for this driver, the value is retrieved from the ServedClassName attribute in the class initialiser.
        internal static string DriverDescription; // The value is retrieved from the ServedClassName attribute in the class initialiser.

        // connectedState holds the connection state from this driver instance's perspective, as opposed to the local server's perspective, which may be different because of other client connections.
        internal bool connectedState; // The connected state from this driver's perspective)
        internal TraceLogger tl; // Trace logger object to hold diagnostic information just for this instance of the driver, as opposed to the local server's log, which includes activity from all driver instances.
        private bool disposedValue;

        #region Initialisation and Dispose

        /// <summary>
        /// Initializes a new instance of the <see cref="ScdouglasFujifilm"/> class. Must be public to successfully register for COM.
        /// </summary>
        public Camera()
        {
            try
            {
                // Pull the ProgID from the ProgID class attribute.
                Attribute attr = Attribute.GetCustomAttribute(this.GetType(), typeof(ProgIdAttribute));
                DriverProgId = ((ProgIdAttribute)attr)?.Value ?? "PROGID NOT SET!";  // Get the driver ProgIDfrom the ProgID attribute.

                // Pull the display name from the ServedClassName class attribute.
                attr = Attribute.GetCustomAttribute(this.GetType(), typeof(ServedClassNameAttribute));
                DriverDescription = ((ServedClassNameAttribute)attr)?.DisplayName ?? "DISPLAY NAME NOT SET!";  // Get the driver description that displays in the ASCOM Chooser from the ServedClassName attribute.

                // LOGGING CONFIGURATION
                // By default all driver logging will appear in Hardware log file
                // If you would like each instance of the driver to have its own log file as well, uncomment the lines below

                tl = new TraceLogger("", "ScdouglasFujifilm.Driver"); // Remove the leading ASCOM. from the ProgId because this will be added back by TraceLogger.
                SetTraceState(); // Read profile to see if logging is enabled

                // Initialise the hardware layer (this ensures CameraHardware's static constructor runs if needed)
                CameraHardware.InitialiseHardware();

                LogMessage("Camera", "Starting driver initialisation");
                LogMessage("Camera", $"ProgID: {DriverProgId}, Description: {DriverDescription}");

                connectedState = false; // Initialise connected to false for this instance


                LogMessage("Camera", "Completed initialisation");
            }
            catch (Exception ex)
            {
                LogMessage("Camera", $"Initialisation exception: {ex}");
                // Avoid showing UI in constructor if possible, rely on logging.
                // MessageBox.Show($"{ex.Message}", "Exception creating ASCOM.ScdouglasFujifilm.Camera", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        /// <summary>
        /// Class destructor called automatically by the .NET runtime when the object is finalised in order to release resources that are NOT managed by the .NET runtime.
        /// </summary>
        ~Camera()
        {
            Dispose(false);
        }

        /// <summary>
        /// Deterministically dispose of any managed and unmanaged resources used in this instance of the driver.
        /// </summary>
        public void Dispose()
        {
            Dispose(disposing: true);
            // Do not call GC.SuppressFinalize(this); here - it breaks ReferenceCountedObjectBase COM counting.
        }

        /// <summary>
        /// Dispose of managed and unmanaged resources.
        /// </summary>
        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // Dispose managed state (managed objects).
                    try
                    {
                        // Clean up the trace logger object for this instance
                        if (!(tl is null))
                        {
                            tl.Enabled = false;
                            tl.Dispose();
                            tl = null;
                        }
                        // IMPORTANT: Do NOT dispose CameraHardware here. It's static and shared.
                        // The local server calls CameraHardware.Dispose() when it shuts down.
                    }
                    catch (Exception ex)
                    {
                        // Log exception during disposal if possible
                        try { LogMessage("Dispose", $"Exception disposing managed resources: {ex.Message}"); } catch { }
                    }
                }

                // TODO: Release unmanaged resources (e.g., file handles) if this specific instance
                // created any directly (unlikely for this presentation layer).

                disposedValue = true;
            }
        }

        #endregion

        // PUBLIC COM INTERFACE ICameraV3 IMPLEMENTATION

        #region Common properties and methods.

        /// <summary>
        /// Displays the Setup Dialogue form.
        /// If the user clicks the OK button to dismiss the form, then
        /// the new settings are saved, otherwise the old values are reloaded.
        /// THIS IS THE ONLY PLACE WHERE SHOWING USER INTERFACE IS ALLOWED!
        /// </summary>
        public void SetupDialog()
        {
            try
            {
                // Only show the dialog if not connected. Settings affecting connection cannot be changed while connected.
                if (connectedState)
                {
                    MessageBox.Show("Already connected. Please disconnect before changing settings.", "Setup", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    return;
                }

                LogMessage("SetupDialog", $"Calling SetupDialog.");
                // Delegate to the hardware class method which loads/shows the form
                CameraHardware.SetupDialog();
                LogMessage("SetupDialog", $"Completed.");
                // Refresh trace state in case it was changed in the dialog
                SetTraceState();
            }
            catch (Exception ex)
            {
                LogMessage("SetupDialog", $"Threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Returns the list of custom action names supported by this driver.</summary>
        public ArrayList SupportedActions
        {
            get
            {
                try
                {
                    // Check if the hardware is connected
                    CheckConnected($"SupportedActions");
                    // Delegate the call to the hardware class
                    ArrayList actions = CameraHardware.SupportedActions;
                    LogMessage("SupportedActions", $"Returning {actions.Count} actions.");
                    return actions;
                }
                catch (Exception ex)
                {
                    LogMessage("SupportedActions", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Invokes the specified device-specific custom action.</summary>
        public string Action(string actionName, string actionParameters)
        {
            try
            {
                CheckConnected($"Action {actionName} - {actionParameters}");
                LogMessage("", $"Calling Action: {actionName} with parameters: {actionParameters}");
                // Delegate the call to the hardware class
                string actionResponse = CameraHardware.Action(actionName, actionParameters);
                LogMessage("Action", $"Completed.");
                return actionResponse;
            }
            catch (Exception ex)
            {
                LogMessage("Action", $"Threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Transmits an arbitrary string command to the device and does not wait for a response.</summary>
        public void CommandBlind(string command, bool raw)
        {
            try
            {
                CheckConnected($"CommandBlind: {command}, Raw: {raw}");
                LogMessage("CommandBlind", $"Calling method - Command: {command}, Raw: {raw}");
                // Delegate the call to the hardware class
                CameraHardware.CommandBlind(command, raw);
                LogMessage("CommandBlind", $"Completed.");
            }
            catch (Exception ex)
            {
                LogMessage("CommandBlind", $"Command: {command}, Raw: {raw} threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Transmits an arbitrary string command to the device and waits for a boolean response.</summary>
        public bool CommandBool(string command, bool raw)
        {
            try
            {
                CheckConnected($"CommandBool: {command}, Raw: {raw}");
                LogMessage("CommandBool", $"Calling method - Command: {command}, Raw: {raw}");
                // Delegate the call to the hardware class
                bool commandBoolResponse = CameraHardware.CommandBool(command, raw);
                LogMessage("CommandBool", $"Returning: {commandBoolResponse}.");
                return commandBoolResponse;
            }
            catch (Exception ex)
            {
                LogMessage("CommandBool", $"Command: {command}, Raw: {raw} threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Transmits an arbitrary string command to the device and waits for a string response.</summary>
        public string CommandString(string command, bool raw)
        {
            try
            {
                CheckConnected($"CommandString: {command}, Raw: {raw}");
                LogMessage("CommandString", $"Calling method - Command: {command}, Raw: {raw}");
                // Delegate the call to the hardware class
                string commandStringResponse = CameraHardware.CommandString(command, raw);
                LogMessage("CommandString", $"Returning: {commandStringResponse}.");
                return commandStringResponse;
            }
            catch (Exception ex)
            {
                LogMessage("CommandString", $"Command: {command}, Raw: {raw} threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Sets or Gets the connected state of the camera for this instance.</summary>
        public bool Connected
        {
            get
            {
                // Returns the connection state for this specific driver instance.
                LogMessage("Connected Get", connectedState.ToString());
                return connectedState;
            }
            set
            {
                LogMessage("Connected Set", value.ToString());
                if (value == connectedState)
                {
                    LogMessage("Connected Set", "No change required.");
                    return; // No change
                }

                try
                {
                    if (value) // Connect this instance
                    {
                        // Check if the hardware layer is already connected (by another instance perhaps)
                        if (!CameraHardware.Connected)
                        {
                            LogMessage("Connected Set", "Hardware layer not connected, attempting hardware connect...");
                            CameraHardware.Connected = true; // This will perform the actual SDK connection
                            LogMessage("Connected Set", "Hardware layer connect successful.");
                        }
                        else
                        {
                            LogMessage("Connected Set", "Hardware layer already connected by another instance.");
                        }
                        connectedState = true; // Mark this instance as connected
                        LogMessage("Connected Set", "Instance connected.");
                    }
                    else // Disconnect this instance
                    {
                        connectedState = false; // Mark this instance as disconnected
                        LogMessage("Connected Set", "Instance disconnected.");
                        // IMPORTANT: Do NOT disconnect the hardware (CameraHardware.Connected = false) here!
                        // Another client instance might still be connected. The hardware connection
                        // is managed globally by the CameraHardware class and the local server.
                        // It will be disconnected when the last client disconnects or the server shuts down.
                    }
                }
                catch (Exception ex)
                {
                    LogMessage("Connected Set", $"Exception: {ex.Message}");
                    connectedState = false; // Ensure state reflects failure
                    // Re-throw the exception to the client
                    throw;
                }
            }
        }

        /// <summary>Returns a description of the device, such as manufacturer and model number.</summary>
        public string Description
        {
            get
            {
                // Get from the hardware class, which might read it from the device or profile
                try
                {
                    string description = CameraHardware.Description;
                    LogMessage("Description Get", description);
                    return description;
                }
                catch (Exception ex)
                {
                    LogMessage("Description Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Descriptive and version information about this ASCOM driver.</summary>
        public string DriverInfo
        {
            get
            {
                // This is about the ASCOM driver itself, not the hardware.
                string driverInfo = CameraHardware.DriverInfo; // Get from hardware class for consistency
                LogMessage("DriverInfo Get", driverInfo);
                return driverInfo;
            }
        }

        /// <summary>A string containing only the major and minor version of the driver.</summary>
        public string DriverVersion
        {
            get
            {
                // Get from the hardware class
                string driverVersion = CameraHardware.DriverVersion;
                LogMessage("DriverVersion Get", driverVersion);
                return driverVersion;
            }
        }

        /// <summary>The interface version number that this device supports.</summary>
        public short InterfaceVersion
        {
            get
            {
                short interfaceVersion = CameraHardware.InterfaceVersion;
                LogMessage("InterfaceVersion Get", interfaceVersion.ToString());
                return interfaceVersion;
            }
        }

        /// <summary>The short name of the driver, for display purposes.</summary>
        public string Name
        {
            get
            {
                // Get from the hardware class
                string name = CameraHardware.Name;
                LogMessage("Name Get", name);
                return name;
            }
        }

        #endregion

        #region ICameraV3 Implementation

        /// <summary>Aborts the current exposure, if any, and returns the camera to Idle state.</summary>
        public void AbortExposure()
        {
            try
            {
                CheckConnected("AbortExposure");
                LogMessage("AbortExposure", $"Calling method.");
                CameraHardware.AbortExposure(); // Delegate to hardware class
                LogMessage("AbortExposure", $"Completed.");
            }
            catch (Exception ex)
            {
                LogMessage("AbortExposure", $"Threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Returns the X offset of the Bayer matrix, as defined in SensorType.</summary>
        public short BayerOffsetX
        {
            get
            {
                try
                {
                    CheckConnected("BayerOffsetX Get");
                    short bayerOffsetX = CameraHardware.BayerOffsetX; // Delegate to hardware class
                    LogMessage("BayerOffsetX Get", bayerOffsetX.ToString());
                    return bayerOffsetX;
                }
                catch (Exception ex)
                {
                    LogMessage("BayerOffsetX Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the Y offset of the Bayer matrix, as defined in SensorType.</summary>
        public short BayerOffsetY
        {
            get
            {
                try
                {
                    CheckConnected("BayerOffsetY Get");
                    short bayerOffsetY = CameraHardware.BayerOffsetY; // Delegate to hardware class
                    LogMessage("BayerOffsetY Get", bayerOffsetY.ToString());
                    return bayerOffsetY;
                }
                catch (Exception ex)
                {
                    LogMessage("BayerOffsetY Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the binning factor for the X axis.</summary>
        public short BinX
        {
            get
            {
                try
                {
                    CheckConnected("BinX Get");
                    short binX = CameraHardware.BinX; // Delegate to hardware class
                    LogMessage("BinX Get", binX.ToString());
                    return binX;
                }
                catch (Exception ex)
                {
                    LogMessage("BinX Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("BinX Set");
                    LogMessage("BinX Set", value.ToString());
                    CameraHardware.BinX = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("BinX Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the binning factor for the Y axis.</summary>
        public short BinY
        {
            get
            {
                try
                {
                    CheckConnected("BinY Get");
                    short binY = CameraHardware.BinY; // Delegate to hardware class
                    LogMessage("BinY Get", binY.ToString());
                    return binY;
                }
                catch (Exception ex)
                {
                    LogMessage("BinY Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("BinY Set");
                    LogMessage("BinY Set", value.ToString());
                    CameraHardware.BinY = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("BinY Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the current CCD temperature in degrees Celsius.</summary>
        public double CCDTemperature
        {
            get
            {
                try
                {
                    CheckConnected("CCDTemperature Get");
                    double ccdTemperature = CameraHardware.CCDTemperature; // Delegate to hardware class
                    LogMessage("CCDTemperature Get", ccdTemperature.ToString("F2")); // Format for display
                    return ccdTemperature;
                }
                catch (Exception ex)
                {
                    LogMessage("CCDTemperature Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the current camera operational state.</summary>
        public CameraStates CameraState
        {
            get
            {
                try
                {
                    // No CheckConnected here, state should be readable even if connection attempt failed mid-operation
                    CameraStates cameraState = CameraHardware.CameraState; // Delegate to hardware class
                    LogMessage("CameraState Get", cameraState.ToString());
                    return cameraState;
                }
                catch (Exception ex)
                {
                    LogMessage("CameraState Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the width of the CCD camera chip in unbinned pixels.</summary>
        public int CameraXSize
        {
            get
            {
                try
                {
                    CheckConnected("CameraXSize Get");
                    int cameraXSize = CameraHardware.CameraXSize; // Delegate to hardware class
                    LogMessage("CameraXSize Get", cameraXSize.ToString());
                    return cameraXSize;
                }
                catch (Exception ex)
                {
                    LogMessage("CameraXSize Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the height of the CCD camera chip in unbinned pixels.</summary>
        public int CameraYSize
        {
            get
            {
                try
                {
                    CheckConnected("CameraYSize Get");
                    int cameraYSize = CameraHardware.CameraYSize; // Delegate to hardware class
                    LogMessage("CameraYSize Get", cameraYSize.ToString());
                    return cameraYSize;
                }
                catch (Exception ex)
                {
                    LogMessage("CameraYSize Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera can abort exposures.</summary>
        public bool CanAbortExposure
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canAbortExposure = CameraHardware.CanAbortExposure; // Delegate to hardware class
                    LogMessage("CanAbortExposure Get", canAbortExposure.ToString());
                    return canAbortExposure;
                }
                catch (Exception ex)
                {
                    LogMessage("CanAbortExposure Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera supports asymmetric binning.</summary>
        public bool CanAsymmetricBin
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canAsymmetricBin = CameraHardware.CanAsymmetricBin; // Delegate to hardware class
                    LogMessage("CanAsymmetricBin Get", canAsymmetricBin.ToString());
                    return canAsymmetricBin;
                }
                catch (Exception ex)
                {
                    LogMessage("CanAsymmetricBin Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera has a fast readout mode.</summary>
        public bool CanFastReadout
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canFastReadout = CameraHardware.FastReadout; // Delegate to hardware class property
                    LogMessage("CanFastReadout Get", canFastReadout.ToString());
                    return canFastReadout;
                }
                catch (Exception ex)
                {
                    LogMessage("CanFastReadout Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera's cooler power setting can be read.</summary>
        public bool CanGetCoolerPower
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canGetCoolerPower = CameraHardware.CanGetCoolerPower; // Delegate to hardware class
                    LogMessage("CanGetCoolerPower Get", canGetCoolerPower.ToString());
                    return canGetCoolerPower;
                }
                catch (Exception ex)
                {
                    LogMessage("CanGetCoolerPower Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera supports pulse guiding.</summary>
        public bool CanPulseGuide
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canPulseGuide = CameraHardware.CanPulseGuide; // Delegate to hardware class
                    LogMessage("CanPulseGuide Get", canPulseGuide.ToString());
                    return canPulseGuide;
                }
                catch (Exception ex)
                {
                    LogMessage("CanPulseGuide Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera supports setting the CCD temperature.</summary>
        public bool CanSetCCDTemperature
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canSetCCDTemperature = CameraHardware.CanSetCCDTemperature; // Delegate to hardware class
                    LogMessage("CanSetCCDTemperature Get", canSetCCDTemperature.ToString());
                    return canSetCCDTemperature;
                }
                catch (Exception ex)
                {
                    LogMessage("CanSetCCDTemperature Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera can stop an exposure that is in progress.</summary>
        public bool CanStopExposure
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool canStopExposure = CameraHardware.CanStopExposure; // Delegate to hardware class
                    LogMessage("CanStopExposure Get", canStopExposure.ToString());
                    return canStopExposure;
                }
                catch (Exception ex)
                {
                    LogMessage("CanStopExposure Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the camera cooler on/off state.</summary>
        public bool CoolerOn
        {
            get
            {
                try
                {
                    CheckConnected("CoolerOn Get");
                    bool coolerOn = CameraHardware.CoolerOn; // Delegate to hardware class
                    LogMessage("CoolerOn Get", coolerOn.ToString());
                    return coolerOn;
                }
                catch (Exception ex)
                {
                    LogMessage("CoolerOn Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("CoolerOn Set");
                    LogMessage("CoolerOn Set", value.ToString());
                    CameraHardware.CoolerOn = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("CoolerOn Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the present cooler power level, in percent.</summary>
        public double CoolerPower
        {
            get
            {
                try
                {
                    CheckConnected("CoolerPower Get");
                    double coolerPower = CameraHardware.CoolerPower; // Delegate to hardware class
                    LogMessage("CoolerPower Get", coolerPower.ToString("F1"));
                    return coolerPower;
                }
                catch (Exception ex)
                {
                    LogMessage("CoolerPower Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the gain of the camera in photoelectrons per A/D unit.</summary>
        public double ElectronsPerADU
        {
            get
            {
                try
                {
                    CheckConnected("ElectronsPerADU Get");
                    double electronsPerAdu = CameraHardware.ElectronsPerADU; // Delegate to hardware class
                    LogMessage("ElectronsPerADU Get", electronsPerAdu.ToString("F3"));
                    return electronsPerAdu;
                }
                catch (Exception ex)
                {
                    LogMessage("ElectronsPerADU Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the maximum exposure time supported by StartExposure.</summary>
        public double ExposureMax
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    double exposureMax = CameraHardware.ExposureMax; // Delegate to hardware class
                    LogMessage("ExposureMax Get", exposureMax.ToString());
                    return exposureMax;
                }
                catch (Exception ex)
                {
                    LogMessage("ExposureMax Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the minimum exposure time supported by StartExposure.</summary>
        public double ExposureMin
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    double exposureMin = CameraHardware.ExposureMin; // Delegate to hardware class
                    LogMessage("ExposureMin Get", exposureMin.ToString());
                    return exposureMin;
                }
                catch (Exception ex)
                {
                    LogMessage("ExposureMin Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the smallest increment in exposure time supported by StartExposure.</summary>
        public double ExposureResolution
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    double exposureResolution = CameraHardware.ExposureResolution; // Delegate to hardware class
                    LogMessage("ExposureResolution Get", exposureResolution.ToString());
                    return exposureResolution;
                }
                catch (Exception ex)
                {
                    LogMessage("ExposureResolution Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets Fast Readout Mode.</summary>
        public bool FastReadout
        {
            get
            {
                try
                {
                    CheckConnected("FastReadout Get");
                    bool fastReadout = CameraHardware.FastReadout; // Delegate to hardware class
                    LogMessage("FastReadout Get", fastReadout.ToString());
                    return fastReadout;
                }
                catch (Exception ex)
                {
                    LogMessage("FastReadout Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("FastReadout Set");
                    LogMessage("FastReadout Set", value.ToString());
                    CameraHardware.FastReadout = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("FastReadout Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Reports the full well capacity of the camera in electrons.</summary>
        public double FullWellCapacity
        {
            get
            {
                try
                {
                    CheckConnected("FullWellCapacity Get");
                    double fullWellCapacity = CameraHardware.FullWellCapacity; // Delegate to hardware class
                    LogMessage("FullWellCapacity Get", fullWellCapacity.ToString("F0"));
                    return fullWellCapacity;
                }
                catch (Exception ex)
                {
                    LogMessage("FullWellCapacity Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the camera gain (or gain index).</summary>
        public short Gain
        {
            get
            {
                try
                {
                    CheckConnected("Gain Get");
                    short gain = CameraHardware.Gain; // Delegate to hardware class
                    LogMessage("Gain Get", gain.ToString());
                    return gain;
                }
                catch (Exception ex)
                {
                    LogMessage("Gain Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("Gain Set");
                    LogMessage("Gain Set", value.ToString());
                    CameraHardware.Gain = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("Gain Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the maximum Gain value supported by the camera.</summary>
        public short GainMax
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    short gainMax = CameraHardware.GainMax; // Delegate to hardware class
                    LogMessage("GainMax Get", gainMax.ToString());
                    return gainMax;
                }
                catch (Exception ex)
                {
                    LogMessage("GainMax Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the minimum Gain value supported by the camera.</summary>
        public short GainMin
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    short gainMin = CameraHardware.GainMin; // Delegate to hardware class
                    LogMessage("GainMin Get", gainMin.ToString());
                    return gainMin;
                }
                catch (Exception ex)
                {
                    LogMessage("GainMin Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the list of supported gain names or values.</summary>
        public ArrayList Gains
        {
            get
            {
                try
                {
                    CheckConnected("Gains Get");
                    ArrayList gains = CameraHardware.Gains; // Delegate to hardware class
                    LogMessage("Gains Get", $"Returning {gains.Count} gain values.");
                    return gains;
                }
                catch (Exception ex)
                {
                    LogMessage("Gains Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera has a mechanical shutter.</summary>
        public bool HasShutter
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    bool hasShutter = CameraHardware.HasShutter; // Delegate to hardware class
                    LogMessage("HasShutter Get", hasShutter.ToString());
                    return hasShutter;
                }
                catch (Exception ex)
                {
                    LogMessage("HasShutter Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the current heat sink temperature (ambient) in degrees Celsius.</summary>
        public double HeatSinkTemperature
        {
             get
            {
                try
                {
                    CheckConnected("HeatSinkTemperature Get");
                    double heatSinkTemperature = CameraHardware.HeatSinkTemperature; // Delegate to hardware class
                    LogMessage("HeatSinkTemperature Get", heatSinkTemperature.ToString("F2"));
                    return heatSinkTemperature;
                }
                catch (Exception ex)
                {
                    LogMessage("HeatSinkTemperature Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns a safearray of integers of size NumX * NumY containing pixel values from the last exposure.</summary>
        public object ImageArray
        {
            get
            {
                try
                {
                    CheckConnected("ImageArray Get");
                    LogMessage("ImageArray", $"Retrieving image array from hardware...");
                    object imageArray = CameraHardware.ImageArray; // Delegate to hardware class
                    LogMessage("ImageArray", $"Image array retrieved.");
                    return imageArray;
                }
                catch (Exception ex)
                {
                    LogMessage("ImageArray Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns a safearray of Variants of size NumX * NumY containing pixel values from the last exposure.</summary>
        public object ImageArrayVariant
        {
             get
            {
                try
                {
                    CheckConnected("ImageArrayVariant Get");
                    LogMessage("ImageArrayVariant", $"Retrieving image array variant from hardware...");
                     object imageArrayVariant = CameraHardware.ImageArrayVariant; // Delegate to hardware class
                    LogMessage("ImageArrayVariant", $"Image array variant retrieved.");
                    return imageArrayVariant;
                }
                catch (Exception ex)
                {
                    LogMessage("ImageArrayVariant Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if an image is ready to be downloaded.</summary>
        public bool ImageReady
        {
            get
            {
                try
                {
                    // CheckConnected("ImageReady Get"); // Don't check connected, should be readable
                    bool imageReady = CameraHardware.ImageReady; // Delegate to hardware class
                    LogMessage("ImageReady Get", imageReady.ToString());
                    return imageReady;
                }
                catch (Exception ex)
                {
                    LogMessage("ImageReady Get", $"Threw an exception: \r\n{ex}");
                    // Decide if this should throw or return false on error
                    // Returning false might be safer for polling loops
                    return false;
                    // throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns true if the camera is currently pulse guiding.</summary>
        public bool IsPulseGuiding
        {
            get
            {
                try
                {
                    CheckConnected("IsPulseGuiding Get");
                    bool isPulseGuiding = CameraHardware.IsPulseGuiding; // Delegate to hardware class
                    LogMessage("IsPulseGuiding Get", isPulseGuiding.ToString());
                    return isPulseGuiding;
                }
                catch (Exception ex)
                {
                    LogMessage("IsPulseGuiding Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Reports the actual exposure duration in seconds.</summary>
        public double LastExposureDuration
        {
            get
            {
                try
                {
                    CheckConnected("LastExposureDuration Get");
                    double lastExposureDuration = CameraHardware.LastExposureDuration; // Delegate to hardware class
                    LogMessage("LastExposureDuration Get", lastExposureDuration.ToString());
                    return lastExposureDuration;
                }
                catch (Exception ex)
                {
                    LogMessage("LastExposureDuration Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Reports the start time of the last exposure in FITS format (CCYY-MM-DDThh:mm:ss.sss...).</summary>
        public string LastExposureStartTime
        {
            get
            {
                try
                {
                    CheckConnected("LastExposureStartTime Get");
                    string lastExposureStartTime = CameraHardware.LastExposureStartTime; // Delegate to hardware class
                    LogMessage("LastExposureStartTime Get", lastExposureStartTime);
                    return lastExposureStartTime;
                }
                catch (Exception ex)
                {
                    LogMessage("LastExposureStartTime Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Reports the maximum ADU value the camera can produce.</summary>
        public int MaxADU
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    int maxAdu = CameraHardware.MaxADU; // Delegate to hardware class
                    LogMessage("MaxADU Get", maxAdu.ToString());
                    return maxAdu;
                }
                catch (Exception ex)
                {
                    LogMessage("MaxADU Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the maximum allowed binning for the X camera axis.</summary>
        public short MaxBinX
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    short maxBinX = CameraHardware.MaxBinX; // Delegate to hardware class
                    LogMessage("MaxBinX Get", maxBinX.ToString());
                    return maxBinX;
                }
                catch (Exception ex)
                {
                    LogMessage("MaxBinX Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the maximum allowed binning for the Y camera axis.</summary>
        public short MaxBinY
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    short maxBinY = CameraHardware.MaxBinY; // Delegate to hardware class
                    LogMessage("MaxBinY Get", maxBinY.ToString());
                    return maxBinY;
                }
                catch (Exception ex)
                {
                    LogMessage("MaxBinY Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the subframe width.</summary>
        public int NumX
        {
            get
            {
                try
                {
                    CheckConnected("NumX Get");
                    int numX = CameraHardware.NumX; // Delegate to hardware class
                    LogMessage("NumX Get", numX.ToString());
                    return numX;
                }
                catch (Exception ex)
                {
                    LogMessage("NumX Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                 try
                {
                    CheckConnected("NumX Set");
                    LogMessage("NumX Set", value.ToString());
                    CameraHardware.NumX = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("NumX Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the subframe height.</summary>
        public int NumY
        {
            get
            {
                try
                {
                    CheckConnected("NumY Get");
                    int numY = CameraHardware.NumY; // Delegate to hardware class
                    LogMessage("NumY Get", numY.ToString());
                    return numY;
                }
                catch (Exception ex)
                {
                    LogMessage("NumY Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("NumY Set");
                    LogMessage("NumY Set", value.ToString());
                    CameraHardware.NumY = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("NumY Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the percentage of the current operation that is complete.</summary>
        public short PercentCompleted
        {
            get
            {
                try
                {
                    // No CheckConnected here, should be readable even if connection attempt failed mid-operation
                    short percentCompleted = CameraHardware.PercentCompleted; // Delegate to hardware class
                    LogMessage("PercentCompleted Get", percentCompleted.ToString());
                    return percentCompleted;
                }
                catch (Exception ex)
                {
                    LogMessage("PercentCompleted Get", $"Threw an exception: \r\n{ex}");
                    // Decide if this should throw or return 0 on error
                    return 0;
                    // throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the width of the CCD chip pixels in microns.</summary>
        public double PixelSizeX
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    double pixelSizeX = CameraHardware.PixelSizeX; // Delegate to hardware class
                    LogMessage("PixelSizeX Get", pixelSizeX.ToString());
                    return pixelSizeX;
                }
                catch (Exception ex)
                {
                    LogMessage("PixelSizeX Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the height of the CCD chip pixels in microns.</summary>
        public double PixelSizeY
        {
            get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    double pixelSizeY = CameraHardware.PixelSizeY; // Delegate to hardware class
                    LogMessage("PixelSizeY Get", pixelSizeY.ToString());
                    return pixelSizeY;
                }
                catch (Exception ex)
                {
                    LogMessage("PixelSizeY Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Activates the Camera's mount control system to instruct the mount to move.</summary>
        public void PulseGuide(GuideDirections direction, int duration)
        {
             try
            {
                CheckConnected("PulseGuide");
                LogMessage("PulseGuide", $"Direction: {direction}, Duration: {duration}");
                CameraHardware.PulseGuide(direction, duration); // Delegate to hardware class
                LogMessage("PulseGuide", $"Completed.");
            }
            catch (Exception ex)
            {
                LogMessage("PulseGuide", $"Threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Gets or sets the camera's readout mode index.</summary>
        public short ReadoutMode
        {
            get
            {
                try
                {
                    CheckConnected("ReadoutMode Get");
                    short readoutMode = CameraHardware.ReadoutMode; // Delegate to hardware class
                    LogMessage("ReadoutMode Get", readoutMode.ToString());
                    return readoutMode;
                }
                catch (Exception ex)
                {
                    LogMessage("ReadoutMode Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("ReadoutMode Set");
                    LogMessage("ReadoutMode Set", value.ToString());
                    CameraHardware.ReadoutMode = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("ReadoutMode Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the list of available readout modes.</summary>
        public ArrayList ReadoutModes
        {
             get
            {
                try
                {
                    CheckConnected("ReadoutModes Get");
                    ArrayList readoutModes = CameraHardware.ReadoutModes; // Delegate to hardware class
                    LogMessage("ReadoutModes Get", $"Returning {readoutModes.Count} modes.");
                    return readoutModes;
                }
                catch (Exception ex)
                {
                    LogMessage("ReadoutModes Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the type of colour information returned by the camera sensor.</summary>
        public SensorType SensorType
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    SensorType sensorType = CameraHardware.SensorType; // Delegate to hardware class
                    LogMessage("SensorType Get", sensorType.ToString());
                    return sensorType;
                }
                catch (Exception ex)
                {
                    LogMessage("SensorType Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Returns the name of the camera sensor.</summary>
        public string SensorName
        {
             get
            {
                try
                {
                    // No CheckConnected needed for capability properties
                    string sensorName = CameraHardware.SensorName; // Delegate to hardware class
                    LogMessage("SensorName Get", sensorName);
                    return sensorName;
                }
                catch (Exception ex)
                {
                    LogMessage("SensorName Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the camera cooler setpoint in degrees Celsius.</summary>
        public double SetCCDTemperature
        {
            get
            {
                try
                {
                    CheckConnected("SetCCDTemperature Get");
                    double setCcdTemperature = CameraHardware.SetCCDTemperature; // Delegate to hardware class
                    LogMessage("SetCCDTemperature Get", setCcdTemperature.ToString("F2"));
                    return setCcdTemperature;
                }
                catch (Exception ex)
                {
                    LogMessage("SetCCDTemperature Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("SetCCDTemperature Set");
                    LogMessage("SetCCDTemperature Set", value.ToString("F2"));
                    CameraHardware.SetCCDTemperature = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("SetCCDTemperature Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Starts an exposure.</summary>
        public void StartExposure(double duration, bool light)
        {
            try
            {
                CheckConnected("StartExposure");
                LogMessage("StartExposure", $"Duration: {duration}, Light: {light}");
                CameraHardware.StartExposure(duration, light); // Delegate to hardware class
                LogMessage("StartExposure", $"Exposure initiated in hardware class.");
            }
            catch (Exception ex)
            {
                LogMessage("StartExposure", $"Threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        /// <summary>Gets or sets the subframe start position for the X axis.</summary>
        public int StartX
        {
            get
            {
                try
                {
                    CheckConnected("StartX Get");
                    int startX = CameraHardware.StartX; // Delegate to hardware class
                    LogMessage("StartX Get", startX.ToString());
                    return startX;
                }
                catch (Exception ex)
                {
                    LogMessage("StartX Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("StartX Set");
                    LogMessage("StartX Set", value.ToString());
                    CameraHardware.StartX = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("StartX Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Gets or sets the subframe start position for the Y axis.</summary>
        public int StartY
        {
            get
            {
                 try
                {
                    CheckConnected("StartY Get");
                    int startY = CameraHardware.StartY; // Delegate to hardware class
                    LogMessage("StartY Get", startY.ToString());
                    return startY;
                }
                catch (Exception ex)
                {
                    LogMessage("StartY Get", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
            set
            {
                try
                {
                    CheckConnected("StartY Set");
                    LogMessage("StartY Set", value.ToString());
                    CameraHardware.StartY = value; // Delegate to hardware class
                }
                catch (Exception ex)
                {
                    LogMessage("StartY Set", $"Threw an exception: \r\n{ex}");
                    throw; // Re-throw ASCOM exceptions
                }
            }
        }

        /// <summary>Stops the current exposure, if any.</summary>
        public void StopExposure()
        {
            try
            {
                CheckConnected("StopExposure");
                LogMessage("StopExposure", $"Calling method.");
                CameraHardware.StopExposure(); // Delegate to hardware class
                LogMessage("StopExposure", $"Completed.");
            }
            catch (Exception ex)
            {
                LogMessage("StopExposure", $"Threw an exception: \r\n{ex}");
                throw; // Re-throw ASCOM exceptions
            }
        }

        // Note: Offset, OffsetMin, OffsetMax, Offsets are not typically supported by DSLRs/MILCs via SDK
        public int Offset { get => throw new PropertyNotImplementedException("Offset", false); set => throw new PropertyNotImplementedException("Offset", true); }
        public int OffsetMax => throw new PropertyNotImplementedException("OffsetMax", false);
        public int OffsetMin => throw new PropertyNotImplementedException("OffsetMin", false);
        public ArrayList Offsets => throw new PropertyNotImplementedException("Offsets", false);

        // Note: SubExposureDuration is not typically supported by DSLRs/MILCs
        public double SubExposureDuration { get => throw new PropertyNotImplementedException("SubExposureDuration", false); set => throw new PropertyNotImplementedException("SubExposureDuration", true); }


        #endregion

        #region Private Members

        /// <summary>
        /// Checks if the driver instance is connected to the hardware.
        /// Throws a NotConnectedException if not connected.
        /// </summary>
        /// <param name="message">The message to include in the exception if not connected.</param>
        private void CheckConnected(string message)
        {
            if (!connectedState)
            {
                throw new NotConnectedException($"{DriverDescription} ({DriverProgId}) is not connected: {message}");
            }
            // Additionally, check if the hardware layer itself is still connected
            if (!CameraHardware.Connected)
            {
                 // If the hardware got disconnected unexpectedly, update this instance's state
                 connectedState = false;
                 throw new NotConnectedException($"{DriverDescription} ({DriverProgId}) hardware layer disconnected unexpectedly: {message}");
            }
        }

        /// <summary>
        /// Log helper function that writes to the driver or local server loggers as required.
        /// </summary>
        /// <param name="identifier">Identifier such as method name.</param>
        /// <param name="message">Message to be logged.</param>
        private void LogMessage(string identifier, string message)
        {
            // Log to instance-specific logger if enabled
            tl?.LogMessageCrLf(identifier, message);
            // Also log to the shared hardware logger
            CameraHardware.LogMessage(identifier, message);
        }

        /// <summary>
        /// Reads the trace state from the driver's Profile and enables/disables the instance trace log accordingly.
        /// </summary>
        private void SetTraceState()
        {
            if (tl == null) return; // Guard against null logger
            try
            {
                using (Profile driverProfile = new Profile())
                {
                    driverProfile.DeviceType = "Camera";
                    tl.Enabled = Convert.ToBoolean(driverProfile.GetValue(DriverProgId, CameraHardware.traceStateProfileName, string.Empty, CameraHardware.traceStateDefault));
                }
            }
            catch (Exception ex)
            {
                 // Log error reading profile if possible, but don't crash
                 try { LogMessage("SetTraceState", $"Error reading profile: {ex.Message}"); } catch {}
                 // Keep previous trace state or default to disabled? Defaulting to disabled might be safer.
                 tl.Enabled = false;
            }
        }

        #endregion
    }
}
