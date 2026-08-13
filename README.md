# ASCOM Driver for Fujifilm X/GFX Cameras

[![ASCOM CameraV3](https://img.shields.io/badge/ASCOM-CameraV3-blue)](https://ascom-standards.org/)
[![Build and test](https://github.com/Scdouglas1999/Fujicom/actions/workflows/build.yml/badge.svg)](https://github.com/Scdouglas1999/Fujicom/actions/workflows/build.yml)
[![Support on Patreon](https://img.shields.io/badge/Support%20on-Patreon-f96854?style=for-the-badge&logo=patreon&logoColor=white)](https://www.patreon.com/cw/SeanDouglas)

## Overview

This project provides an **ASCOM Camera driver** for controlling select Fujifilm X-Series and GFX-System cameras from popular astronomical imaging software like NINA, Sequence Generator Pro (SGP), KStars/Ekos, and others compatible with the ASCOM Platform.

The primary goal is to enable the use of these excellent Fujifilm cameras for astrophotography by providing essential controls such as exposure, ISO settings, and RAW image data retrieval directly within your preferred imaging suite.

This driver utilizes the official **Fujifilm X SDK** for camera communication and **LibRaw** for decoding the RAW Bayer data, ensuring reliable operation and accurate data extraction.

## Support Development

<p align="center">
  <a href="https://www.patreon.com/cw/SeanDouglas"><img src="https://img.shields.io/badge/Support%20Fujicom%20on-Patreon-f96854?style=for-the-badge&logo=patreon&logoColor=white" alt="Support Fujicom on Patreon"></a>
</p>

Fujicom is free to use. If it helps make your Fujifilm camera useful in an astronomy workflow, you can optionally [support ongoing development on Patreon](https://www.patreon.com/cw/SeanDouglas).

Support goes toward the practical maintenance work behind the driver: camera compatibility testing, SDK updates, installer upkeep, documentation, and fixes for the awkward hardware-specific issues that only show up on real imaging rigs. There are no paid-only driver features; support is appreciated, never required.

## Features

* **ASCOM ICameraV3 Compliance:** Implements the standard ASCOM interface for broad compatibility.
* **Timed, T-mode, and Bulb Exposures:** Uses the shutter speeds reported by each body, including extended timed exposures through 60 minutes, with bulb fallback where needed.
* **Abort and Stop Support:** Cancels an active capture and drains stale frames so a cancelled exposure cannot be returned as the next sequence image.
* **ISO Control:** Discovers fixed ISO values from the camera while excluding Auto ISO modes.
* **Reliable RAW Download:** Polls until a RAW frame is ready, accepts rotated RAW format codes, skips JPEG/HEIF frames, crops Fuji optical-black columns, and decodes RAF data through LibRaw.
* **X-Trans Compatibility:** Converts X-Trans captures to a synthetic RGGB compatibility image after LibRaw demosaicing. This works with standard ASCOM clients but is not a replacement for the native RAF in calibration-sensitive workflows. GFX Bayer data remains native.
* **Shared Camera Sessions:** Multiple ASCOM clients share one SDK session; the camera closes only when the final client disconnects.
* **Dynamic Camera Configuration:** Validated model configurations are matched against Fuji SDK product names instead of relying on an exact filename match.
* **Automated Regression Checks:** CI compiles the C# driver, tests camera/configuration logic, and verifies P/Invoke entry points and constants against the bundled SDK.

## Supported Cameras

Camera support is limited by the official Fujifilm X SDK. The following models are currently supported by the SDK and targeted by this driver:

**GFX System:**

* GFX 50S
* GFX 50R
* GFX 50S II
* GFX 100
* GFX 100 II
* GFX 100RF
* GFX 100S
* GFX 100S II

**X Series:**

* X-H2
* X-H2S
* X-M5
* X-Pro3
* X-S10
* X-S20
* X-T2
* X-T3
* X-T4
* X-T5

**Important:** Please ensure your camera has the latest firmware installed from the official Fujifilm website: [Fujifilm Firmware Downloads](https://fujifilm-x.com/support/download/firmware/cameras/)

*Note: Adding support for cameras not listed here requires updates to the official Fujifilm SDK or significant reverse-engineering efforts.*

## Installation (Recommended)

1.  **Install .NET Framework 4.7.2 or newer:** This is required for the driver and is included on current Windows 10/11 systems. See [Microsoft's .NET Framework installation guide](https://learn.microsoft.com/en-us/dotnet/framework/install/guide-for-developers) if needed.
2.  **Install ASCOM Platform:** If you haven't already, download and install the latest ASCOM Platform from [ascom-standards.org](https://ascom-standards.org/). This driver **will not function** without it.
3.  **Download Driver Installer:** Go to the [Releases](https://github.com/Scdouglas1999/Fujicom/releases) page and download the newest `Fujicom.Setup.*.exe`.
4.  **Run Installer:** Run the downloaded `.exe` file and follow the installation prompts. It will register the driver with the ASCOM Platform.
5.  **Connect Camera:** Connect your supported Fujifilm camera to your computer via USB and set the camera's connection mode (usually under `CONNECTION SETTING` > `PC CONNECTION MODE` or similar) to `USB TETHER SHOOTING AUTO` or `USB AUTO`. Consult your camera manual for the exact menu path.
6.  **Select in Software:** Open your ASCOM-compatible software (e.g., NINA) and select "Scdouglas Fujifilm Camera" (or similar name) from the camera dropdown list.

## Usage Notes

### Camera Setup (IMPORTANT!)

* **Image Quality:** RAW is recommended. RAW+JPEG is accepted; the driver discards the companion JPEG and waits for RAW.
* **RAW Compression:** Uncompressed is recommended. The driver requests this setting on connection when the body supports software control.
* **Non-PASM Dial Cameras (X-T#, X-Pro#):**
    * For cameras with physical dials for Shutter Speed, Aperture, and ISO, you **must** set the camera to full manual control *before connecting* for reliable operation, especially for BULB exposures.
    * Set the **Shutter Speed dial** to **T (Time)** so the SDK can select timed and bulb shutter codes. If a body rejects software control, the trace log reports this explicitly.
    * Set the **Aperture ring** on the lens to manual control (usually by moving it off the 'A' setting).
    * Set the **ISO dial** to a specific value (e.g., base ISO), not 'A'.
    * Set the **Focus Mode Selector** to **M (Manual Focus)**.
* **PASM Dial Cameras (GFX Series, X-S#, X-H#):**
    * Set the physical exposure mode dial to **Manual (M)**. The driver reports the current mode for diagnostics but does not change it remotely, because several bodies reject remote mode changes with a combination error.
    * Set focus mode to **M (Manual Focus)** on the body and verify RAW image quality in the camera menu.

### ASCOM Driver Settings

After installing, you can access the driver's settings via your imaging software's ASCOM Chooser dialog when selecting the camera.

* **Trace Logger:** Enable this checkbox to generate detailed logs, which are helpful for troubleshooting. Logs are typically saved in `Documents\ASCOM\Logs`.

## Building from Source (For Developers)

If you prefer to build the driver yourself:

**Prerequisites:**

* ASCOM Platform (Installed)
* Visual Studio 2022 (or later) with ".NET desktop development" (C#) and "Desktop development with C++" (for C++/CLI wrapper) workloads installed.
* Fujifilm X SDK: You must obtain this directly from Fujifilm. Place the necessary DLLs (e.g., `XAPI.dll`, `FTLPTP.dll`, `FF*.dll`) and header files (`XAPI.h`, `XAPIOpt.h`, model-specific headers) where the projects can find them (often requires adding include/library paths in project settings or placing DLLs in the output directory).
* LibRaw DLL: Ensure `libraw.dll` (the native C++ version) is accessible to the C++/CLI wrapper project at runtime, typically by placing it in the final build output directory alongside the driver `.exe` and wrapper `.dll`.

**Steps:**

1.  Clone or download the repository.
2.  Open the `*.sln` solution file in Visual Studio 2022.
3.  In the Visual Studio toolbar, set the Solution Configuration to `Debug` (or `Release`) and the Solution Platform to `x64`. **The driver must be built as x64.**
4.  Ensure both projects (`Fuji` - the C# driver, and `Fujifilm.LibRawWrapper` - the C++/CLI wrapper, names may vary slightly) are loaded in the Solution Explorer.
5.  **(Optional but Recommended)** Right-click the Solution in Solution Explorer and select "Clean Solution". Wait for it to finish.
6.  Right-click the Solution again and select "Build Solution". Wait for it to finish. Check the Output window for any errors.
7.  If the build is successful, open a Command Prompt **as Administrator**.
8.  Navigate (`cd`) to the build output directory (e.g., `bin\x64\Debug` or `bin\x64\Release` relative to the C# driver project's folder).
9.  Register the driver with ASCOM using the command (replace the `.exe` name if yours is different):
    ```bash
    ASCOM.ScdouglasFujifilm.Camera.exe /register
    ```
10. The driver should now be available in ASCOM applications. To unregister later, use:
    ```bash
    ASCOM.ScdouglasFujifilm.Camera.exe /unregister
    ```

Pure managed driver logic can also be checked on any OS with .NET 8:

```bash
dotnet run --project tests/Fujicom.Core.Tests/Fujicom.Core.Tests.csproj --configuration Release
python3 build/verify-sdk-interop.py
```

The second command validates native entry points and high-risk constants against the bundled Fujifilm SDK DLL and headers.

## Contributing

Contributions are welcome! Please feel free to submit Pull Requests or report Issues via the GitHub repository.

## License

This project is licensed under the MIT License - see [LICENSE](LICENSE).
