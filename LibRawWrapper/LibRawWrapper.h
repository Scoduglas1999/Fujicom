#pragma once

// Include necessary .NET types
#using <System.dll> // For IntPtr, Byte, UInt16, Array etc.

// Use namespaces for brevity
using namespace System;
using namespace System::Runtime::InteropServices; // For Marshal

// Define the managed wrapper class within a namespace
namespace Fujifilm
{
    namespace LibRawWrapper // Choose a suitable namespace
    {
        // Managed public class accessible from C#
        public ref class RawProcessor
        {
        public:
            /// <summary>
            /// Processes a raw image buffer using LibRaw and extracts the Bayer data.
            /// </summary>
            /// <param name="rawBuffer">Managed byte array containing the raw file data (e.g., from Fuji SDK).</param>
            /// <param name="bayerData">Output: 2D managed array (ushort) to receive the raw Bayer data.</param>
            /// <param name="width">Output: Width of the extracted Bayer image.</param>
            /// <param name="height">Output: Height of the extracted Bayer image.</param>
            /// <returns>LibRaw error code (0 for success, non-zero for failure).</returns>
            static int ProcessRawBuffer(
                array<Byte>^ rawBuffer,
                [Out] array<System::UInt16, 2>^% bayerData, // Pass managed array by reference for output
                [Out] int% width,
                [Out] int% height
            );
        };
    }
}
