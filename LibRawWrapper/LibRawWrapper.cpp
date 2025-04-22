#include "pch.h" // Standard precompiled header for C++/CLI projects

// Ensure standard C headers are included AFTER pch.h if needed
#include <string.h> // For memcpy_s
#include <vcruntime_string.h> // Alternative for memcpy_s if string.h doesn't work alone

// Include marshal helper for pinning managed arrays
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h> // If needed for string conversions

// Include LibRaw headers for type definitions (structs, enums)
// We will declare the functions explicitly below using DllImport
#include "libraw_types.h"
#include "libraw_const.h"
// #include "libraw.h" // Can potentially remove this if types/consts are sufficient

// Include the header for the managed class definition
#include "LibRawWrapper.h"


// Use namespaces at the top level for clarity
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Fujifilm::LibRawWrapper; // Use the namespace defined in the header

// --- Explicit P/Invoke declarations for LibRaw C API ---
// This tells the C++/CLI compiler exactly where to find these C functions
// and ensures correct C linkage (__cdecl) is used.
[DllImport("libraw.dll", CallingConvention = CallingConvention::Cdecl, EntryPoint = "libraw_init")]
extern "C" libraw_data_t* libraw_init_pinvoke(unsigned int flags);

[DllImport("libraw.dll", CallingConvention = CallingConvention::Cdecl, EntryPoint = "libraw_open_buffer")]
extern "C" int libraw_open_buffer_pinvoke(libraw_data_t* lr, const void* buffer, size_t size);

[DllImport("libraw.dll", CallingConvention = CallingConvention::Cdecl, EntryPoint = "libraw_unpack")]
extern "C" int libraw_unpack_pinvoke(libraw_data_t* lr);

[DllImport("libraw.dll", CallingConvention = CallingConvention::Cdecl, EntryPoint = "libraw_close")]
extern "C" void libraw_close_pinvoke(libraw_data_t* lr);

// Declare others if needed, e.g.:
// [DllImport("libraw.dll", CallingConvention = CallingConvention::Cdecl, EntryPoint = "libraw_strerror")]
// extern "C" const char* libraw_strerror_pinvoke(int errorcode);


/// <summary>
/// Processes a raw image buffer using LibRaw and extracts the Bayer data.
/// </summary>
int RawProcessor::ProcessRawBuffer(
    // Explicitly qualify .NET types
    array<System::Byte>^ rawBuffer,
    [Out] array<System::UInt16, 2>^% bayerData,
    [Out] int% width,
    [Out] int% height)
{
    // Initialize output parameters
    bayerData = nullptr;
    width = 0;
    height = 0;

    // Check input buffer
    if (rawBuffer == nullptr || rawBuffer->Length == 0)
    {
        return LIBRAW_UNSPECIFIED_ERROR;
    }

    // LibRaw data structure pointer (native)
    libraw_data_t* lr = nullptr;
    // Use defined LibRaw success code
    int ret = LIBRAW_SUCCESS;

    // Pin the managed byte array to get a native pointer
    pin_ptr<System::Byte> pinnedRawBuffer = &rawBuffer[0];
    // Use const unsigned char* for buffer pointer if libraw function expects const
    const unsigned char* nativeBufferPtr = pinnedRawBuffer;
    size_t bufferSize = (size_t)rawBuffer->Length;

    try
    {
        // 1. Initialize LibRaw (using explicit P/Invoke)
        lr = libraw_init_pinvoke(0); // Flags = 0
        if (!lr)
        {
            return LIBRAW_UNSPECIFIED_ERROR; // Or LIBRAW_FATAL_ERROR
        }

        // 2. Open the buffer (using explicit P/Invoke)
        ret = libraw_open_buffer_pinvoke(lr, (const void*)nativeBufferPtr, bufferSize);
        if (ret != LIBRAW_SUCCESS)
        {
            libraw_close_pinvoke(lr); // Use P/Invoke version
            return ret;
        }

        // 3. Unpack the raw data (using explicit P/Invoke)
        ret = libraw_unpack_pinvoke(lr);
        if (ret != LIBRAW_SUCCESS)
        {
            libraw_close_pinvoke(lr); // Use P/Invoke version
            return ret;
        }

        // 4. Get dimensions (Direct struct access is still fine)
        width = lr->sizes.raw_width;
        height = lr->sizes.raw_height;

        if (width <= 0 || height <= 0)
        {
            ret = LIBRAW_DATA_ERROR;
            libraw_close_pinvoke(lr); // Use P/Invoke version
            return ret;
        }

        // 5. Get pointer to raw Bayer data (Direct struct access is still fine)
        ushort* raw_image_ptr = lr->rawdata.raw_image;
        if (!raw_image_ptr)
        {
            ret = LIBRAW_DATA_ERROR;
            libraw_close_pinvoke(lr); // Use P/Invoke version
            return ret;
        }

        // 6. Create the managed output array
        bayerData = gcnew array<System::UInt16, 2>(height, width);

        // 7. Copy data from native buffer to managed array
        pin_ptr<System::UInt16> pinnedBayerData = &bayerData[0, 0];
        ushort* dest_ptr = pinnedBayerData;
        size_t totalPixels = (size_t)width * height;
        size_t bytesToCopy = totalPixels * sizeof(ushort);

        // Use memcpy_s with explicit casts to void*
        errno_t memcpy_ret = memcpy_s((void*)dest_ptr, bytesToCopy, (const void*)raw_image_ptr, bytesToCopy);

        if (memcpy_ret != 0)
        {
            System::Diagnostics::Debug::WriteLine(L"memcpy_s failed with error code: " + memcpy_ret);
            ret = LIBRAW_UNSPECIFIED_ERROR;
            bayerData = nullptr;
            libraw_close_pinvoke(lr); // Use P/Invoke version
            return ret;
        }
    }
    catch (System::Exception^ ex) // Catch managed exceptions
    {
        System::Diagnostics::Debug::WriteLine(L"Managed exception in RawProcessor: " + ex->Message);
        ret = LIBRAW_UNSPECIFIED_ERROR;
        bayerData = nullptr;
        width = 0;
        height = 0;
        if (lr) {
            libraw_close_pinvoke(lr); // Use P/Invoke version
            lr = nullptr;
        }
    }
    finally // Ensure LibRaw handle is closed
    {
        if (lr)
        {
            // 8. Close LibRaw resources (using explicit P/Invoke)
            libraw_close_pinvoke(lr);
        }
    }

    return ret; // Return the LibRaw status code (0 on success)
}
