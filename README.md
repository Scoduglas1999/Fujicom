# ASCOM Driver for Fujifilm X/GFX Cameras

[![ASCOM Conform](https://img.shields.io/badge/ASCOM-Conform%20CameraV3-blue)](https://ascom-standards.org/)
## Overview

This project provides an **ASCOM Camera driver** for controlling select Fujifilm X-Series and GFX-System cameras from popular astronomical imaging software like NINA, Sequence Generator Pro (SGP), KStars/Ekos, and others compatible with the ASCOM Platform.

The primary goal is to enable the use of these excellent Fujifilm cameras for astrophotography by providing essential controls such as exposure, ISO settings, and RAW image data retrieval directly within your preferred imaging suite.

This driver utilizes the official **Fujifilm X SDK** for camera communication and **LibRaw** for decoding the RAW Bayer data, ensuring reliable operation and accurate data extraction.

## Features

* **ASCOM ICameraV3 Compliance:** Implements the standard ASCOM interface for broad compatibility.
* **Exposure Control:** Start exposures, including long BULB exposures.
* **ISO Control:** Get and set camera ISO sensitivity.
* **RAW Image Download:** Retrieves the raw Bayer data from the camera's .RAF file using LibRaw, providing it as an `int[,]` array via the `ImageArray` property for direct use by imaging software.
* **Dynamic Camera Configuration:** Loads camera-specific parameters (sensor size, pixel size, supported modes) from model-specific JSON files at connection time.
* **Save Copy to SD Card:** Optional feature (configurable in ASCOM setup) to save a native `.RAF` file directly to the camera's SD card simultaneously with the data transfer to the computer. Useful for backups or separate processing.
* **ASCOM Profile Settings:** Standard ASCOM setup dialog for persistent settings like Trace Logging and the "Save Copy to SD Card" option.

## Supported Cameras

Camera support is limited by the official Fujifilm X SDK. The following models are currently supported by the SDK and targeted by this driver:

**GFX System:**

* GFX 50S
* GFX 50R
* GFX 50S II
* GFX 100
* GFX 100 II
* GFX 100S
* GFX 100S II

**X Series:**

* X-H2
* X-H2S
* X-M5
* X-Pro3
* X-S10 (Requires Firmware v2.00+)
* X-S20
* X-T3
* X-T4
* X-T5

**Important:** Please ensure your camera has the latest firmware installed from the official Fujifilm website: [Fujifilm Firmware Downloads](https://fujifilm-x.com/support/download/firmware/cameras/)

*Note: Adding support for cameras not listed here requires updates to the official Fujifilm SDK or significant reverse-engineering efforts.*

## Installation (Recommended)

1.  **Install ASCOM Platform:** If you haven't already, download and install the latest ASCOM Platform from [ascom-standards.org](https://ascom-standards.org/). This driver **will not function** without it.
2.  **Download Driver Installer:** Go to the [Releases](https://github.com/Scoduglas1999/Fujicom/releases) page of this repository and download the latest `.exe` installer file (e.g., `Fujicom.Setup.V2.0.exe`).
3.  **Run Installer:** Run the downloaded `.exe` file and follow the installation prompts. It will register the driver with the ASCOM Platform.
4.  **Connect Camera:** Connect your supported Fujifilm camera to your computer via USB and set the camera's connection mode (usually under `CONNECTION SETTING` > `PC CONNECTION MODE` or similar) to `USB TETHER SHOOTING AUTO` or `USB AUTO`. Consult your camera manual for the exact menu path.
5.  **Select in Software:** Open your ASCOM-compatible software (e.g., NINA) and select "Scdouglas Fujifilm Camera" (or similar name) from the camera dropdown list.

## Usage Notes

### Camera Setup (IMPORTANT!)

* **Image Quality:** Set your camera's **Image Quality** setting to **RAW** only. Do *not* use RAW+JPEG modes, as the driver is designed to fetch and process only the RAW data.
* **RAW Compression:** Set the **RAW Recording** or **RAW Compression** setting to **Uncompressed**. Lossless or Lossy compressed RAW files may not decode correctly via LibRaw in this driver.
* **Non-PASM Dial Cameras (X-T#, X-Pro#):**
    * For cameras with physical dials for Shutter Speed, Aperture, and ISO, you **must** set the camera to full manual control *before connecting* for reliable operation, especially for BULB exposures.
    * Set the **Shutter Speed dial** to **'B' (Bulb)**. The camera display should show "BULB".
    * Set the **Aperture ring** on the lens to manual control (usually by moving it off the 'A' setting).
    * Set the **ISO dial** to a specific value (e.g., base ISO), not 'A'.
    * Set the **Focus Mode Selector** to **M (Manual Focus)**.
* **PASM Dial Cameras (GFX Series, X-S#, X-H#):**
    * The driver will attempt to set the camera to **Manual (M)** exposure mode and **Manual Focus (MF)** mode automatically upon connection.
    * You still need to manually ensure **Image Quality** is **RAW** and **RAW Compression** is **Uncompressed** in the camera menu.

### ASCOM Driver Settings

After installing, you can access the driver's settings via your imaging software's ASCOM Chooser dialog when selecting the camera.

* **Trace Logger:** Enable this checkbox to generate detailed logs, which are helpful for troubleshooting. Logs are typically saved in `Documents\ASCOM\Logs`.
* **Save Copy to SD Card:** Check this box if you want the camera to save a standard `.RAF` file to the installed SD card *in addition* to transferring the raw Bayer data to the computer. Leave unchecked to only transfer data to the computer (default and recommended for most sequences to save shutter actuations if not needed).

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

## Contributing

Contributions are welcome! Please feel free to submit Pull Requests or report Issues via the GitHub repository.

## License

This project is licensed under the [Your License Name] License - see the LICENSE.md file for details.
