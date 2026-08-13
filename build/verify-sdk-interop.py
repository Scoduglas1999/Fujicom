#!/usr/bin/env python3
"""Verify Fujicom's highest-risk P/Invoke declarations against the bundled Fuji SDK."""

from __future__ import annotations

import pathlib
import re
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
WRAPPER = ROOT / "Fuji" / "CameraDriver" / "CameraHardware.cs"
XAPI_HEADER = ROOT / "Fuji" / "RequiredHeaders" / "XAPI.h"
XAPIOPT_HEADER = ROOT / "Fuji" / "RequiredHeaders" / "XAPIOpt.h"
SDK_DLL = ROOT / "Fuji" / "RequiredDLL" / "XAPI.dll"
LIBRAW_DLL = ROOT / "Fuji" / "RequiredDLL" / "libraw.dll"


def pe_exports(path: pathlib.Path) -> set[str]:
    data = path.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe : pe + 4] != b"PE\0\0":
        raise ValueError(f"{path} is not a PE image")
    section_count = struct.unpack_from("<H", data, pe + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe + 20)[0]
    magic = struct.unpack_from("<H", data, pe + 24)[0]
    export_directory = pe + 24 + (112 if magic == 0x20B else 96)
    export_rva = struct.unpack_from("<I", data, export_directory)[0]
    sections = []
    table = pe + 24 + optional_size
    for index in range(section_count):
        offset = table + 40 * index
        virtual_size = struct.unpack_from("<I", data, offset + 8)[0]
        virtual_address = struct.unpack_from("<I", data, offset + 12)[0]
        raw_size = struct.unpack_from("<I", data, offset + 16)[0]
        raw = struct.unpack_from("<I", data, offset + 20)[0]
        sections.append((virtual_address, max(virtual_size, raw_size), raw))

    def file_offset(rva: int) -> int:
        for virtual_address, size, raw in sections:
            if virtual_address <= rva < virtual_address + size:
                return raw + rva - virtual_address
        raise ValueError(f"RVA {rva:#x} is not mapped")

    base = file_offset(export_rva)
    count = struct.unpack_from("<I", data, base + 24)[0]
    names_rva = struct.unpack_from("<I", data, base + 32)[0]
    names = file_offset(names_rva)
    result = set()
    for index in range(count):
        offset = file_offset(struct.unpack_from("<I", data, names + 4 * index)[0])
        result.add(data[offset : data.index(b"\0", offset)].decode())
    return result


def constant(text: str, name: str) -> int:
    match = re.search(rf"(?:#define|const int)\s+{re.escape(name)}\s*(?:=)?\s*(0x[0-9a-fA-F]+|-?\d+)", text)
    if not match:
        raise ValueError(f"Could not find {name}")
    return int(match.group(1), 0)


def main() -> int:
    wrapper = WRAPPER.read_text(encoding="utf-8-sig")
    xapi = XAPI_HEADER.read_bytes().decode("latin-1")
    xapiopt = XAPIOPT_HEADER.read_bytes().decode("latin-1")
    failures = []

    exports = pe_exports(SDK_DLL)
    entry_points = set(re.findall(r'EntryPoint\s*=\s*"([^"]+)"', wrapper))
    for entry_point in sorted(entry_points - exports):
        failures.append(f"DllImport entry point is not exported by XAPI.dll: {entry_point}")

    libraw_exports = pe_exports(LIBRAW_DLL)
    required_libraw = {
        "libraw_init", "libraw_open_buffer", "libraw_unpack", "libraw_dcraw_process",
        "libraw_dcraw_make_mem_image", "libraw_dcraw_clear_mem", "libraw_close"
    }
    for entry_point in sorted(required_libraw - libraw_exports):
        failures.append(f"Required entry point is not exported by libraw.dll: {entry_point}")

    if not re.search(
        r"XSDK_CapSensitivity\s*\(IntPtr\s+hCamera,\s*ref\s+int\s+plNumSensitivity,\s*IntPtr\s+plSensitivity\)",
        wrapper,
    ):
        failures.append("XSDK_CapSensitivity must have exactly the three SDK parameters")

    for name, header in (
        ("XSDK_RELEASE_CANCEL", xapi),
        ("SDK_RAW_COMPRESSION_OFF", xapiopt),
        ("SDK_RAW_COMPRESSION_LOSSLESS", xapiopt),
        ("SDK_RAW_COMPRESSION_LOSSY", xapiopt),
    ):
        expected = constant(header, name)
        actual = constant(wrapper, name)
        if actual != expected:
            failures.append(f"{name}: wrapper={actual:#x}, header={expected:#x}")

    print(f"Checked {len(entry_points)} XAPI exports, {len(required_libraw)} LibRaw exports, and 5 signature/constant contracts.")
    if failures:
        for failure in failures:
            print("FAIL " + failure)
        return 1
    print("All Fujifilm SDK interop checks passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
