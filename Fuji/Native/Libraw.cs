using System;
using System.Runtime.InteropServices;
using System.Security;

namespace ASCOM.LocalServer.NativeLibRaw // Ensure this namespace matches your project
{
    /// <summary>
    /// Provides direct P/Invoke wrappers for essential native LibRaw C API functions.
    /// Assumes 64-bit libraw.dll is accessible at runtime.
    /// Structures defined based on libraw.h and libraw_types.h.
    /// </summary>
    [SuppressUnmanagedCodeSecurity]
    public static class Libraw
    {
        private const string LibRawDllName = "libraw.dll"; // Or the exact name/path of your libraw DLL

        #region P/Invoke Signatures

        [DllImport(LibRawDllName, EntryPoint = "libraw_init", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr libraw_init(uint flags);

        [DllImport(LibRawDllName, EntryPoint = "libraw_close", CallingConvention = CallingConvention.Cdecl)]
        public static extern void libraw_close(IntPtr libraw_data);

        [DllImport(LibRawDllName, EntryPoint = "libraw_recycle", CallingConvention = CallingConvention.Cdecl)]
        public static extern void libraw_recycle(IntPtr libraw_data);

        [DllImport(LibRawDllName, EntryPoint = "libraw_open_buffer", CallingConvention = CallingConvention.Cdecl)]
        public static extern int libraw_open_buffer(IntPtr libraw_data, IntPtr buffer, int size); // Using int for size_t for compatibility, check if 64-bit size needed

        [DllImport(LibRawDllName, EntryPoint = "libraw_unpack", CallingConvention = CallingConvention.Cdecl)]
        public static extern int libraw_unpack(IntPtr libraw_data);

        [DllImport(LibRawDllName, EntryPoint = "libraw_get_raw_height", CallingConvention = CallingConvention.Cdecl)]
        public static extern int libraw_get_raw_height(IntPtr libraw_data);

        [DllImport(LibRawDllName, EntryPoint = "libraw_get_raw_width", CallingConvention = CallingConvention.Cdecl)]
        public static extern int libraw_get_raw_width(IntPtr libraw_data);

        [DllImport(LibRawDllName, EntryPoint = "libraw_strerror", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr libraw_strerror(int errorcode); // Returns const char*

        // Add other P/Invoke definitions from libraw.h if needed

        #endregion

        #region LibRaw Enumerations (Add enums used by structures if needed)

        // Example: Add enums like LibRaw_thumbnail_formats, LibRaw_colorspace, etc.
        // based on libraw_const.h if they are used in the structures below.
        // public enum LibRaw_thumbnail_formats { ... }

        #endregion

        #region LibRaw Structure Definitions (Based on libraw.h / libraw_types.h)

        // Note: Using LayoutKind.Sequential. If issues persist, consider explicit Pack values (e.g., Pack = 1 or Pack = 4)
        // if the C compiler used non-default packing, but Sequential is usually correct for well-defined C APIs.

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct libraw_gps_info_t
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
            public float[] latitude; // deg,min,sec
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
            public float[] longitude; // deg,min,sec
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
            public float[] gpstimestamp; // hour,min,sec
            public float altitude;
            public byte altref; // Altitude reference (0=above sea level, 1=below)
            public byte latref; // Latitude ref ('N' or 'S')
            public byte longref; // Longitude ref ('E' or 'W')
            public byte gpsstatus; // GPS status ('A'=active, 'V'=void)
            public byte gpsparsed; // Indicates if GPS data was successfully parsed
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct libraw_imgother_t
        {
            public float iso_speed;
            public float shutter;
            public float aperture;
            public float focal_len;
            public long timestamp; // time_t (usually 64-bit on modern systems)
            public uint shot_order;
            // Correctly marshal the array of structures
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public libraw_gps_info_t[] gpsdata;
            // This field holds the *parsed* GPS data (a single instance)
            public libraw_gps_info_t parsed_gps;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 512)]
            public string desc;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string artist;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public float[] analogbalance;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_thumbnail_t
        {
            public int tformat; // LibRaw_thumbnail_formats enum
            public ushort twidth;
            public ushort theight;
            public uint tlength;
            public int tcolors;
            public IntPtr thumb; // Pointer to thumb data (unsigned char *)
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_internal_data_t
        {
            // These are pointers to internal buffers, IntPtr is appropriate
            public IntPtr internal_image; // ushort (*internal_image)[4];
            public IntPtr meta_data;      // void *meta_data; (or specific struct if known)
            public uint meta_length;
            // Add other internal fields if needed, based on libraw_internal.h if required for size/layout
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_rawdata_t
        {
            // Pointers to raw image data buffers
            public IntPtr raw_image;        // ushort* - This is the target for Bayer data
            public IntPtr color4_image;     // ushort (*color4_image)[4];
            public IntPtr color3_image;     // ushort (*color3_image)[3];
            public IntPtr float_image;      // float *
            public IntPtr float3_image;     // float (*float3_image)[3];
            public IntPtr float4_image;     // float (*float4_image)[4];
            // Pointers to Phase One specific black level data
            public IntPtr ph1_cblack;       // short (*ph1_cblack)[2];
            public IntPtr ph1_rblack;       // short (*ph1_rblack)[2];
            // Contains pointers to internal structures/buffers
            public libraw_internal_data_t internal_data;
            // Note: iparams is part of libraw_data_t, not nested here in libraw.h
        }


        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_raw_inset_crop_t
        {
            public ushort cleft;
            public ushort ctop;
            public ushort cwidth;
            public ushort cheight;
            // public ushort aspect; // LibRawImageAspects enum - Add enum if needed
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_image_sizes_t
        {
            public ushort raw_height;
            public ushort raw_width;
            public ushort height;
            public ushort width;
            public ushort top_margin;
            public ushort left_margin;
            public ushort iheight; // Output image height
            public ushort iwidth;  // Output image width
            public uint raw_pitch; // Bytes per row in raw data buffer
            public double pixel_aspect;
            public int flip; // Image orientation enum LibRaw_flip - Add enum if needed
            // Mask defines image area within raw area
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8 * 4)] // Assuming int[8][4] -> 32 ints
            public int[] mask; // Example: Might need adjustment based on actual C definition (e.g., if it's short)
            public libraw_raw_inset_crop_t raw_inset_crop; // NEW: Added based on libraw.h v0.20+
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct libraw_iparams_t
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 4)]
            public string guard; // Should be "LibRaw"
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string make;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string model;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string software;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string normalized_make;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string normalized_model;
            public uint maker_index; // LibRaw_cameramaker_index enum
            public uint raw_count; // Number of raw images in file (usually 1)
            public uint dng_version;
            public uint is_foveon;
            public int colors; // Number of colors (usually 3 or 4)
            public uint filters; // Bitmask describing CFA pattern
            // Fuji X-Trans Pattern
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6 * 6)]
            public byte[] xtrans; // char[6][6] -> byte[36]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6 * 6)]
            public byte[] xtrans_abs; // char[6][6] -> byte[36]
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 5)]
            public string cdesc; // Color description e.g. "RGBG"
            public uint xmplen; // Length of XMP data block
            public IntPtr xmpdata; // Pointer to XMP data block (char*)
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct libraw_lensinfo_t
        {
            public float MinFocal;
            public float MaxFocal;
            public float MaxAp; // Max aperture at MinFocal
            public float MinAp; // Min aperture at MaxFocal
            public float MaxAp4MaxFocal; // Max aperture at MaxFocal
            public float MinAp4MinFocal; // Min aperture at MinFocal (added in later LibRaw versions)
            public float EXIF_MaxAp; // Max aperture reported by EXIF
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string LensMake;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string Lens;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string LensSerial;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string InternalLensSerial; // (added later)
            public ushort FocalLengthIn35mmFormat;
            // Many more fields exist in newer libraw.h versions (LensID, CameraFormat, LensFormat, etc.)
            // Add them if needed, ensuring correct type and order.
            // For simplicity, stopping here, but ADD MORE FIELDS IF YOUR libraw.h HAS THEM.
        }

        // Makernotes are highly vendor-specific and complex.
        // This is a minimal placeholder structure. Accessing specific notes
        // usually requires dedicated parsing logic beyond basic marshalling.
        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_makernotes_t
        {
            public uint canon_ev; // Example field - add others as needed
            // ... many other potential fields ...
            // It's often better to keep this opaque or use specific parsing functions if available.
            // Using a fixed size might be necessary if its position affects subsequent fields.
            // For now, keep it minimal or even just an IntPtr if size is unknown/variable.
            // Let's assume it's complex and use IntPtr for the main struct for now.
            // If specific fields are needed, they must be defined accurately.
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct libraw_shootinginfo_t
        {
            public short DriveMode;
            public short FocusMode;
            public short MeteringMode;
            public short AFPoint;
            public short ExposureMode;
            public short ExposureProgram; // (Added later)
            public short ImageStabilization;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string BodySerial;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string InternalBodySerial; // (Added later)
            // Add more fields based on your libraw_types.h (e.g., FlashEC, FlashMode, etc.)
        }

        // Color data is extremely complex, involving profiles, matrices, curves.
        // Full marshalling is difficult. This defines the structure shell.
        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_colordata_t
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 0x10000)] // ushort[65536]
            public ushort[] curve; // Camera specific curve
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public uint[] cblack; // Black level (per channel or combined)
            public uint black; // Overall black level
            public uint data_maximum; // Maximum data value before scaling
            public uint maximum; // Maximum value after scaling
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public float[] linear_max; // Per-channel linear max (added later)

            // Corrected C# marshalling for fixed-size array:
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)] // 4 * 4 = 16
            public float[] kblack; // Black level pattern (added later)

            // White balance info
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public float[] cam_mul; // Camera multipliers (as shot)
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public float[] pre_mul; // Preset multipliers (daylight)
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3 * 4)] // float[3][4]
            public float[] cam_xyz; // Camera to XYZ matrix (D65)
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3 * 4)] // float[3][4]
            public float[] rgb_cam; // Camera to sRGB matrix

            // Color profiles
            public IntPtr profile; // Pointer to embedded ICC profile (void*)
            public uint profile_length; // Length of profile data

            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8 * 8 * 4)] // uint[8][8][4] Black level pattern
            public uint[] black_stat;

            public IntPtr dng_color0; // Pointer to dng_color struct (or similar)
            public IntPtr dng_color1; // Pointer to dng_color struct
            public IntPtr phase_one_data; // Pointer to phase_one_data struct
            // Add more fields based on your libraw_types.h (e.g., flash_used, canon_ev, etc.)
            // Pointers (profile, dng_color*, phase_one_data) point to complex data.
        }


        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_output_params_t
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public uint[] greybox; // Crop box [top, left, height, width]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public uint[] cropbox; // Crop box [top, left, height, width]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public double[] aber; // Chromatic aberration correction [red_mul, blue_mul, ?, ?]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
            public double[] gamm; // Gamma curve [power, slope, toe_slope, toe_offset, ?, ?]
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public float[] user_mul; // User white balance multipliers
            public uint shot_select; // Select image number for multi-image RAWs
            public float bright; // Brightness adjustment (default 1.0)
            public float threshold; // Noise threshold for wavelet denoising
            public int half_size; // Output half size image
            public int four_color_rgb; // Use four-color interpolation
            public int highlight; // Highlight recovery mode (0-9)
            public int use_auto_wb; // Auto white balance calculation
            public int use_camera_wb; // Use camera white balance if available
            public int use_camera_matrix; // Use camera color matrix (0=off, 1= D65, 3=any)
            public int output_color; // Output colorspace (LibRaw_colorspace enum)
            public IntPtr output_profile; // Path to output ICC profile (char*)
            public IntPtr camera_profile; // Path to input camera ICC profile (char*)
            public IntPtr bad_pixels;     // Path to bad pixel map file (char*)
            public IntPtr dark_frame;     // Path to dark frame file (char*)
            public int output_bps; // Output bits per sample (8 or 16)
            public int output_tiff; // Output TIFF format (0=PPM, 1=TIFF)
            public int user_flip; // User specified flip (LibRaw_flip enum)
            public int user_qual; // Demosaic algorithm quality (LibRaw_output_flags enum)
            public int user_black; // User specified black level
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
            public int[] user_cblack; // User specified per-channel black level
            public int user_sat; // User specified saturation level
            public int med_passes; // Median filter passes
            public float auto_bright_thr; // Auto brightness threshold
            public float adjust_maximum_thr; // Auto-adjust maximum threshold
            public int no_auto_bright; // Disable auto brightness adjustment
            public int use_fuji_rotate; // Rotate Fuji images automatically
            public int green_matching; // Perform green channel matching
            // DCB interpolation parameters
            public int dcb_iterations;
            public int dcb_enhance_fl;
            // FBDD noise reduction parameter
            public int fbdd_noiserd; // Demosaic: 0-off, 1-light, 2-full
            // Exposure correction parameters
            public int exp_correc; // Exposure correction: 0-off, 1-linear shift
            public float exp_shift; // Exposure shift value (in stops)
            public float exp_preser; // Exposure preservation factor (0.0-1.0)
            // External library usage flags
            public int use_rawspeed; // Use RawSpeed library if available
            public int use_dngsdk; // Use Adobe DNG SDK if available
            // Other flags
            public int no_auto_scale;
            public int no_interpolation;
            public uint raw_processing_options; // LibRaw_processing_options bitmask
            public uint max_raw_memory_mb; // Memory limit for RawSpeed
            public int sony_arw2_posterization_thr; // Threshold for Sony ARW2 posterization fix
            public float coolscan_nef_gamma; // Gamma for Coolscan NEF
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 5)]
            public byte[] p4shot_order; // Order for Pixel Shift files (char[5])
            public IntPtr custom_camera_strings; // Custom camera strings (char**) - Complex marshalling needed if used
        }

        /// <summary>
        /// Mirrors the native libraw_data_t structure based on libraw.h.
        /// Order and definition of nested structures are critical for correct marshalling.
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct libraw_data_t
        {
            // Pointer to processed image data (ushort (*image)[4]) - Populated after dcraw_process or similar
            public IntPtr image;
            // Image dimension and cropping information
            public libraw_image_sizes_t sizes;
            // Basic image parameters (make, model, ISO, etc.)
            public libraw_iparams_t idata;
            // Lens information - Replaced IntPtr with struct definition
            public libraw_lensinfo_t lens;
            // Makernotes - Replaced IntPtr placeholder. NOTE: Actual content is complex/vendor-specific.
            // Using IntPtr here is safer unless specific fields are needed and known.
            // public libraw_makernotes_t makernotes; // Keep as struct if size/layout known
            public IntPtr makernotes; // Using IntPtr as a safer default for complex makernotes
            // Shooting information - Replaced IntPtr with struct definition
            public libraw_shootinginfo_t shootinginfo;
            // Parameters controlling LibRaw processing
            public libraw_output_params_t oparams;
            // Progress flags reported during processing
            public uint progress_flags;
            // Warnings generated during processing
            public uint process_warnings;
            // Color data (profiles, matrices, WB) - Replaced IntPtr with struct definition shell.
            // NOTE: Contains pointers to complex data, full marshalling is hard.
            public libraw_colordata_t color;
            // Other image metadata (timestamp, GPS, description)
            public libraw_imgother_t other;
            // Thumbnail data
            public libraw_thumbnail_t thumbnail;
            // Raw data pointers and internal buffer info - THIS MUST BE AT THE CORRECT OFFSET
            public libraw_rawdata_t rawdata;
            // Pointer to parent LibRaw object (void*) - Used internally by LibRaw++ class
            public IntPtr parent_class;
        }

        #endregion
    }
}
