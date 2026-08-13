# Fujicom 3.0.0

This release overhauls the ASCOM driver's capture lifecycle using the hardware lessons incorporated into the NINA Fujifilm Native Plugin.

## Highlights

- Corrected the `XSDK_CapSensitivity` native signature and now filters Auto ISO modes and expanded ISO values that exceed ASCOM's signed 16-bit `Gain` range.
- Corrected RAW compression constants and routed optional settings through the exported `XSDK_GetProp`/`XSDK_SetProp` functions.
- Added repeated RAW-ready polling for both timed and bulb exposures.
- Added working `AbortExposure` and `StopExposure` paths, including stale-buffer cleanup.
- Recognizes rotated RAW codes and safely skips non-RAW companion frames.
- Crops the Fuji/LibRaw optical-black columns so returned arrays match the advertised active image area.
- Added the extended timed/T-mode shutter catalog through 60 minutes.
- Added X-Trans-to-synthetic-RGGB compatibility conversion while preserving native GFX Bayer data. Synthetic X-Trans output is intended for ASCOM compatibility, not as a substitute for native RAF files in calibration-sensitive workflows.
- Reference-counts shared ASCOM client connections and closes the SDK session after the final disconnect.
- Matches and validates model configurations using normalized SDK product names.
- Added GFX100 II, GFX100RF, GFX100S II, GFX50S II, and X-S10 configurations.
- Corrected the inherited template version `6.6` to `3.0.0`.
- Added repeatable core tests, cross-platform C# compilation, configuration validation, and native interop verification in GitHub Actions.

## Validation boundary

The managed driver, configuration catalog, shutter/ISO logic, SDK exports, constants, and project structure are automatically checked. Final release validation still requires Windows, ASCOM Platform, the x64 C++/CLI wrapper, and a physical supported Fujifilm body; CI cannot simulate the proprietary camera hardware protocol.
