; Inno Setup Script for ASCOM Fujicom Camera Driver
; Assumes this script is located in the root 'Fujicom' repository folder.

; --- Defines ---
; Use '.' for the source root since the script is in the root.
#define SourceRoot "." 

; *** All build output goes to this single directory ***
#define BuildOutputPath SourceRoot + "\Fuji\bin\Release" 

; Path to installer resources (copied from ASCOM SDK)
#define InstallerResourcesPath SourceRoot + "\InstallerResources"

; Define the App Name here to avoid repetition and potential typos
#define MyAppName "ASCOM Fujicom Camera Driver"
; Define the App Version here for consistency
#define MyAppVersion "1.0"
; Define the full Version Info string (major.minor.build)
#define MyAppVersionInfo "1.0.0" 

; --- Setup Section ---
[Setup]
; Use the same unique AppID generated for your driver
AppID={{92e40f6e-9299-4666-95d1-75c962b70abb}
AppName={#MyAppName}
AppVerName={#MyAppName} {#MyAppVersion} 
AppVersion={#MyAppVersion} 
AppPublisher=Sean Douglas <scdouglas1999@gmail.com>
AppPublisherURL=mailto:scdouglas1999@gmail.com
AppSupportURL=https://ascomtalk.groups.io/g/Help ; Link to your support forum/page
AppUpdatesURL=https://ascom-standards.org/ ; Link to driver download page if available
VersionInfoVersion={#MyAppVersionInfo} 
MinVersion=6.1sp1 
DefaultDirName="{commoncf}\ASCOM\Camera\{#MyAppName}" 
DisableDirPage=yes
DisableProgramGroupPage=yes
OutputDir="{#SourceRoot}"
OutputBaseFilename="Fujicom Setup v{#MyAppVersion}" 
Compression=lzma
SolidCompression=yes
WizardImageFile="{#InstallerResourcesPath}\WizardImage.bmp"
LicenseFile="{#InstallerResourcesPath}\CreativeCommons.txt" 
UninstallFilesDir="{commoncf}\ASCOM\Uninstall\Camera\{#MyAppName}"

; --- Languages Section ---
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

; --- Dirs Section ---
[Dirs]
Name: "{commoncf}\ASCOM\Uninstall\Camera\{#MyAppName}"

; --- Files Section ---
; List all files needed by the driver, using the single build output path.
; Ensure all these files exist in the folder defined by BuildOutputPath before compiling!
[Files]
; Main driver executable and its config file
Source: "{#BuildOutputPath}\ASCOM.ScdouglasFujifilm.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildOutputPath}\ASCOM.ScdouglasFujifilm.exe.config"; DestDir: "{app}"; Flags: ignoreversion

; LibRaw Wrapper and native DLL
Source: "{#BuildOutputPath}\LibRawWrapper.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildOutputPath}\libraw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildOutputPath}\Sdcb.LibRaw.dll"; DestDir: "{app}"; Flags: ignoreversion ;

; Fujifilm Specific DLLs (Verify redistribution rights)
Source: "{#BuildOutputPath}\XAPI.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FTLPTP.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FTLPTPIP.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0000API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0001API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0002API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0003API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0004API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0005API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0006API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0007API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0008API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0009API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0010API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0011API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0012API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0013API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0014API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0015API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0016API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0017API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0018API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0019API.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\FF0020API.dll"; DestDir: "{app}"; Flags: ignoreversion 

; Common Libraries (Often included via NuGet)
Source: "{#BuildOutputPath}\Newtonsoft.Json.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\System.Buffers.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\System.Memory.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\System.Runtime.CompilerServices.Unsafe.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\System.Text.Encodings.Web.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\System.Text.Json.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\System.Threading.Tasks.Extensions.dll"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\Microsoft.Bcl.AsyncInterfaces.dll"; DestDir: "{app}"; Flags: ignoreversion 
; *** ADDED MISSING DEPENDENCY ***
Source: "{#BuildOutputPath}\System.Numerics.Vectors.dll"; DestDir: "{app}"; Flags: ignoreversion 

; JSON Configuration/Data Files (Assuming these are needed at runtime)
Source: "{#BuildOutputPath}\GFX50R.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX50S.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100S.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-H2.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-H2S.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-M5.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-Pro3.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-S20.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T2.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T3.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T4.json"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T5.json"; DestDir: "{app}"; Flags: ignoreversion 

; Header Files (Included based on user request, but likely NOT needed for runtime)
Source: "{#BuildOutputPath}\XAPI.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\XAPIOpt.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\XAPIOpt_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX50R.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX50S.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX50SII.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100II.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100II_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100S.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100SII.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\GFX100SII_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-H2.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-H2_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-H2S.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-H2S_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-M5.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-M5_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-Pro3.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-S10.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-S20.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-S20_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T3.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T4.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T5.h"; DestDir: "{app}"; Flags: ignoreversion 
Source: "{#BuildOutputPath}\X-T5_MOV.h"; DestDir: "{app}"; Flags: ignoreversion 

; ReadMe file (Assuming it's in the Fuji project folder, NOT the build output)
Source: "{#SourceRoot}\Fuji\ReadMe.htm"; DestDir: "{app}"; Flags: isreadme

; Installer Resources needed for the setup UI itself (copy these from ASCOM SDK to InstallerResources folder first)
; These are used by the installer at runtime, not copied to the final {app} folder
Source: "{#InstallerResourcesPath}\WizardImage.bmp"; DestDir: "{tmp}"; Flags: dontcopy ignoreversion nocompression
Source: "{#InstallerResourcesPath}\CreativeCommons.txt"; DestDir: "{tmp}"; Flags: dontcopy ignoreversion nocompression

; --- Run Section ---
; Register the ASCOM local server driver during installation
[Run]
Filename: "{app}\ASCOM.ScdouglasFujifilm.exe"; Parameters: "/register"; Flags: runhidden waituntilterminated

; --- Uninstall Run Section ---
; Unregister the ASCOM local server driver during uninstallation
[UninstallRun]
Filename: "{app}\ASCOM.ScdouglasFujifilm.exe"; Parameters: "/unregister"; Flags: runhidden waituntilterminated; RunOnceId: "UnregisterFujicomDriver"

; --- Code Section ---
; Standard ASCOM Platform version check and Uninstall Previous Version logic
[Code]
const
    REQUIRED_PLATFORM_VERSION = 6.2;    // Set this to the minimum required ASCOM Platform version

// Function to return the ASCOM Platform's version number as a double.
function PlatformVersion(): Double;
var
    PlatVerString : String;
begin
    Result := 0.0;  // Initialise the return value in case we can't read the registry
    try
      if RegQueryStringValue(HKEY_LOCAL_MACHINE_32, 'Software\ASCOM','PlatformVersion', PlatVerString) then 
      begin // Successfully read the value from the registry
          Result := StrToFloat(PlatVerString); // Create a double from the X.Y Platform version string
      end;
    except           
      ShowExceptionMessage;
      Result:= -1.0; // Indicate in the return value that an exception was generated
    end;
end;

// Before the installer UI appears, verify that the required ASCOM Platform version is installed.
function InitializeSetup(): Boolean;
var
    PlatformVersionNumber : double;
 begin
    Result := FALSE;  // Assume failure
    PlatformVersionNumber := PlatformVersion(); // Get the installed Platform version as a double
    If PlatformVersionNumber >= REQUIRED_PLATFORM_VERSION then // Check whether we have the minimum required Platform or newer
       Result := TRUE
    else
       if PlatformVersionNumber = 0.0 then
          MsgBox('No ASCOM Platform is installed. Please install Platform ' + Format('%3.1f', [REQUIRED_PLATFORM_VERSION]) + ' or later from https://www.ascom-standards.org', mbCriticalError, MB_OK)
       else 
          MsgBox('ASCOM Platform ' + Format('%3.1f', [REQUIRED_PLATFORM_VERSION]) + ' or later is required, but Platform '+ Format('%3.1f', [PlatformVersionNumber]) + ' is installed. Please install the latest Platform before continuing; you will find it at https://www.ascom-standards.org', mbCriticalError, MB_OK);
end;

// Code to enable the installer to uninstall previous versions of itself when a new version is installed
procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  UninstallExe: String;
  UninstallRegistry: String;
begin
  if (CurStep = ssInstall) then // Install step has started
    begin
      // Create the correct registry location name, which is based on the AppId
      UninstallRegistry := ExpandConstant('Software\Microsoft\Windows\CurrentVersion\Uninstall\{#SetupSetting("AppId")}' + '_is1');
      // Check whether an entry exists
      if RegQueryStringValue(HKLM, UninstallRegistry, 'UninstallString', UninstallExe) then
        begin // Entry exists and previous version is installed so run its uninstaller quietly after informing the user
          MsgBox('Setup will now remove the previous version.', mbInformation, MB_OK);
          // Execute the old uninstaller silently and wait for it to finish
          Exec(RemoveQuotes(UninstallExe), ' /SILENT', '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
          sleep(1000);     // Give enough time for the install screen to be repainted before continuing
        end
  end;
end;
