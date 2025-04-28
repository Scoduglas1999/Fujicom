; Inno Setup Script for ASCOM Fujicom Camera Driver
; Assumes this script is located in the root 'Fujicom' repository folder.

; --- Defines ---
; Use '.' for the source root since the script is in the root.
#define SourceRoot "." 

; !!! Updated target framework moniker for .NET Framework 4.7.2 !!!
#define TargetFramework "net472" 
#define LibRawWrapperOutputPath SourceRoot + "\LibRawWrapper\bin\Release\" + TargetFramework

; Path to the main Fuji driver output (assuming Release configuration)
#define FujiOutputPath SourceRoot + "\Fuji\bin\Release" 

; Path to installer resources (copied from ASCOM SDK)
#define InstallerResourcesPath SourceRoot + "\InstallerResources" 

; --- Setup Section ---
[Setup]
; Use the same unique AppID generated for your driver
AppID={{92e40f6e-9299-4666-95d1-75c962b70abb}
AppName=ASCOM Fujicom Camera Driver
AppVerName=ASCOM Fujicom Camera Driver 1.0
AppVersion=1.0
AppPublisher=Sean Douglas <scdouglas1999@gmail.com>
AppPublisherURL=mailto:scdouglas1999@gmail.com
AppSupportURL=https://ascomtalk.groups.io/g/Help ; Link to your support forum/page
AppUpdatesURL=https://ascom-standards.org/ ; Link to driver download page if available
VersionInfoVersion=1.0.0
; Minimum Windows version (Win 7 SP1). You might increase this if needed.
MinVersion=6.1.7601 
; Installs to C:\Program Files (x86)\Common Files\ASCOM\Camera\ASCOM Fujicom Camera Driver
DefaultDirName="{cf}\ASCOM\Camera\{#AppName}" 
DisableDirPage=yes
DisableProgramGroupPage=yes
; Place the compiled setup file in the SourceRoot (Fujicom folder)
OutputDir="{#SourceRoot}"
OutputBaseFilename="Fujicom Setup v{#AppVersion}" ; Include version in filename
Compression=lzma
SolidCompression=yes
; Use relative paths for wizard image and license (after copying them)
WizardImageFile="{#InstallerResourcesPath}\WizardImage.bmp"
LicenseFile="{#InstallerResourcesPath}\CreativeCommons.txt" 
; Uninstaller data folder - ensure this matches the structure derived from AppName
UninstallFilesDir="{cf}\ASCOM\Uninstall\Camera\{#AppName}"

; --- Languages Section ---
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

; --- Dirs Section ---
[Dirs]
; Create the directory for uninstaller data if it doesn't exist
Name: "{cf}\ASCOM\Uninstall\Camera\{#AppName}"

; --- Files Section ---
; List all files needed by the driver, using relative paths from SourceRoot.
[Files]
; Main driver executable - MUST exist at FujiOutputPath relative to script
Source: "{#FujiOutputPath}\ASCOM.ScdouglasFujifilm.exe"; DestDir: "{app}"; Flags: ignoreversion

; LibRawWrapper DLL - MUST exist at LibRawWrapperOutputPath relative to script
; *** Verify that LibRawWrapper.dll is actually in \bin\Release\net472\ ***
Source: "{#LibRawWrapperOutputPath}\LibRawWrapper.dll"; DestDir: "{app}"; Flags: ignoreversion

; Native libraw DLL (Assuming x64 and copied to Fuji output dir by build process) 
; !!! Verify this location and filename are correct !!!
Source: "{#FujiOutputPath}\libraw.dll"; DestDir: "{app}"; Flags: ignoreversion

; Include any other DLLs required by LibRawWrapper or libraw.dll itself
; For example, if it needs specific C++ runtime DLLs and you are not relying on the user having the VC++ Redist installed:
; Source: "{#FujiOutputPath}\msvcp140.dll"; DestDir: "{app}"; Flags: ignoreversion
; Source: "{#FujiOutputPath}\vcruntime140.dll"; DestDir: "{app}"; Flags: ignoreversion
; (Uncomment and adjust above lines only if necessary and licensed for redistribution)

; ReadMe file (Assuming it's in the Fuji project folder)
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
Filename: "{app}\ASCOM.ScdouglasFujifilm.exe"; Parameters: "/unregister"; Flags: runhidden waituntilterminated

; --- Code Section ---
; Standard ASCOM Platform version check and Uninstall Previous Version logic
; No changes needed here from the generated script unless you have specific needs.
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
