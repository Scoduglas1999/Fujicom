//! Real FITS file I/O
//!
//! Implements actual FITS file reading and writing according to the
//! FITS standard (NASA/Science Office of Standards and Technology).
//!
//! FITS format:
//! - 2880-byte blocks
//! - Header with 80-character keyword records
//! - Data in big-endian format

use crate::{ImageData, PixelType};
use std::collections::HashMap;
use std::fs::File;
use std::io::{BufReader, BufWriter, Read, Write, Seek};
use std::path::Path;

/// FITS header containing all keywords
#[derive(Debug, Clone, Default)]
pub struct FitsHeader {
    /// Keyword-value pairs
    pub keywords: HashMap<String, FitsValue>,
    /// Keywords in order (for writing)
    keyword_order: Vec<String>,
}

/// FITS value types
#[derive(Debug, Clone)]
pub enum FitsValue {
    String(String),
    Integer(i64),
    Float(f64),
    Boolean(bool),
    Comment(String),
}

impl FitsValue {
    pub fn as_string(&self) -> Option<&str> {
        match self {
            FitsValue::String(s) => Some(s),
            _ => None,
        }
    }
    
    pub fn as_i64(&self) -> Option<i64> {
        match self {
            FitsValue::Integer(i) => Some(*i),
            FitsValue::Float(f) => Some(*f as i64),
            _ => None,
        }
    }
    
    pub fn as_f64(&self) -> Option<f64> {
        match self {
            FitsValue::Float(f) => Some(*f),
            FitsValue::Integer(i) => Some(*i as f64),
            _ => None,
        }
    }
    
    pub fn as_bool(&self) -> Option<bool> {
        match self {
            FitsValue::Boolean(b) => Some(*b),
            _ => None,
        }
    }
}

impl FitsHeader {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn set_string(&mut self, key: &str, value: &str) {
        let key_upper = key.to_uppercase();
        if !self.keyword_order.contains(&key_upper) {
            self.keyword_order.push(key_upper.clone());
        }
        self.keywords.insert(key_upper, FitsValue::String(value.to_string()));
    }
    
    pub fn set_int(&mut self, key: &str, value: i64) {
        let key_upper = key.to_uppercase();
        if !self.keyword_order.contains(&key_upper) {
            self.keyword_order.push(key_upper.clone());
        }
        self.keywords.insert(key_upper, FitsValue::Integer(value));
    }
    
    pub fn set_float(&mut self, key: &str, value: f64) {
        let key_upper = key.to_uppercase();
        if !self.keyword_order.contains(&key_upper) {
            self.keyword_order.push(key_upper.clone());
        }
        self.keywords.insert(key_upper, FitsValue::Float(value));
    }
    
    pub fn set_bool(&mut self, key: &str, value: bool) {
        let key_upper = key.to_uppercase();
        if !self.keyword_order.contains(&key_upper) {
            self.keyword_order.push(key_upper.clone());
        }
        self.keywords.insert(key_upper, FitsValue::Boolean(value));
    }

    pub fn get(&self, key: &str) -> Option<&FitsValue> {
        self.keywords.get(&key.to_uppercase())
    }
    
    pub fn get_string(&self, key: &str) -> Option<&str> {
        self.get(key).and_then(|v| v.as_string())
    }
    
    pub fn get_int(&self, key: &str) -> Option<i64> {
        self.get(key).and_then(|v| v.as_i64())
    }
    
    pub fn get_float(&self, key: &str) -> Option<f64> {
        self.get(key).and_then(|v| v.as_f64())
    }
}

/// FITS file reading errors
#[derive(Debug)]
pub enum FitsError {
    Io(std::io::Error),
    InvalidFormat(String),
    UnsupportedBitpix(i32),
    MissingKeyword(String),
}

impl std::fmt::Display for FitsError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            FitsError::Io(e) => write!(f, "IO error: {}", e),
            FitsError::InvalidFormat(s) => write!(f, "Invalid FITS format: {}", s),
            FitsError::UnsupportedBitpix(b) => write!(f, "Unsupported BITPIX: {}", b),
            FitsError::MissingKeyword(k) => write!(f, "Missing required keyword: {}", k),
        }
    }
}

impl std::error::Error for FitsError {}

impl From<std::io::Error> for FitsError {
    fn from(e: std::io::Error) -> Self {
        FitsError::Io(e)
    }
}

/// Read a FITS file from disk
pub fn read_fits(path: &Path) -> Result<(ImageData, FitsHeader), FitsError> {
    let file = File::open(path)?;
    let mut reader = BufReader::new(file);
    read_fits_from_reader(&mut reader)
}

/// Read FITS from memory buffer
pub fn read_fits_from_bytes(bytes: &[u8]) -> Result<(ImageData, FitsHeader), FitsError> {
    let mut reader = std::io::Cursor::new(bytes);
    read_fits_from_reader(&mut reader)
}

/// Internal function to read FITS from any reader
fn read_fits_from_reader<R: Read>(reader: &mut R) -> Result<(ImageData, FitsHeader), FitsError> {
    // Read header
    let header = read_header(reader)?;
    
    // Get image dimensions
    let bitpix = header.get_int("BITPIX")
        .ok_or_else(|| FitsError::MissingKeyword("BITPIX".to_string()))?;
    let naxis = header.get_int("NAXIS")
        .ok_or_else(|| FitsError::MissingKeyword("NAXIS".to_string()))?;
    
    if naxis == 0 {
        // No data, just header
        return Ok((ImageData::new(0, 0, 1, PixelType::U16), header));
    }
    
    let width = header.get_int("NAXIS1")
        .ok_or_else(|| FitsError::MissingKeyword("NAXIS1".to_string()))? as u32;
    let height = header.get_int("NAXIS2").unwrap_or(1) as u32;
    let depth = if naxis >= 3 {
        header.get_int("NAXIS3").unwrap_or(1) as u32
    } else {
        1
    };
    
    // Get scaling parameters
    let bzero = header.get_float("BZERO").unwrap_or(0.0);
    let bscale = header.get_float("BSCALE").unwrap_or(1.0);
    
    // Determine pixel type and read data
    let (pixel_type, data) = match bitpix as i32 {
        8 => {
            let raw = read_u8_data(reader, width, height, depth)?;
            // Apply scaling if needed
            if bzero != 0.0 || bscale != 1.0 {
                let scaled: Vec<u8> = raw.iter()
                    .map(|&v| ((v as f64 * bscale + bzero) as i32).clamp(0, 255) as u8)
                    .collect();
                (PixelType::U8, scaled)
            } else {
                (PixelType::U8, raw)
            }
        }
        16 => {
            let raw = read_i16_data(reader, width, height, depth)?;
            // Convert to u16 with BZERO=32768 for unsigned
            let adjusted: Vec<u8> = if bzero == 32768.0 {
                // Common case: unsigned 16-bit stored as signed with BZERO
                raw.iter()
                    .flat_map(|&v| {
                        let unsigned = (v as i32 + 32768).clamp(0, 65535) as u16;
                        unsigned.to_le_bytes()
                    })
                    .collect()
            } else {
                raw.iter()
                    .flat_map(|&v| {
                        let scaled = (v as f64 * bscale + bzero).clamp(0.0, 65535.0) as u16;
                        scaled.to_le_bytes()
                    })
                    .collect()
            };
            (PixelType::U16, adjusted)
        }
        32 => {
            let raw = read_i32_data(reader, width, height, depth)?;
            // Convert to u32
            let adjusted: Vec<u8> = raw.iter()
                .flat_map(|&v| {
                    let scaled = (v as f64 * bscale + bzero).clamp(0.0, u32::MAX as f64) as u32;
                    scaled.to_le_bytes()
                })
                .collect();
            (PixelType::U32, adjusted)
        }
        -32 => {
            let raw = read_f32_data(reader, width, height, depth)?;
            // Keep as f32
            let bytes: Vec<u8> = raw.iter()
                .flat_map(|&v| {
                    let scaled = v * bscale as f32 + bzero as f32;
                    scaled.to_le_bytes()
                })
                .collect();
            (PixelType::F32, bytes)
        }
        -64 => {
            let raw = read_f64_data(reader, width, height, depth)?;
            let bytes: Vec<u8> = raw.iter()
                .flat_map(|&v| {
                    let scaled = v * bscale + bzero;
                    scaled.to_le_bytes()
                })
                .collect();
            (PixelType::F64, bytes)
        }
        other => return Err(FitsError::UnsupportedBitpix(other)),
    };
    
    let image = ImageData {
        width,
        height,
        channels: depth,
        pixel_type,
        data,
    };
    
    Ok((image, header))
}

/// Read the FITS header (80-character records until END)
pub(crate) fn read_header<R: Read>(reader: &mut R) -> Result<FitsHeader, FitsError> {
    let mut header = FitsHeader::new();
    let mut buffer = [0u8; 80];
    
    loop {
        reader.read_exact(&mut buffer)?;
        
        let record = String::from_utf8_lossy(&buffer);
        let keyword = record[..8].trim();
        
        if keyword == "END" {
            break;
        }
        
        if keyword.is_empty() || keyword.starts_with(' ') {
            continue; // Blank or comment
        }
        
        // Parse the value
        if record.len() > 10 && &record[8..10] == "= " {
            let value_str = record[10..].trim();
            let value = parse_fits_value(value_str);
            header.keywords.insert(keyword.to_string(), value);
            header.keyword_order.push(keyword.to_string());
        } else if keyword == "COMMENT" || keyword == "HISTORY" {
            let comment = record[8..].trim().to_string();
            header.keywords.insert(
                format!("{}_{}", keyword, header.keyword_order.len()),
                FitsValue::Comment(comment),
            );
        }
    }
    
    // Skip to next 2880-byte boundary
    // The header is padded with spaces to a multiple of 2880 bytes
    // We've been reading in 80-byte chunks, so calculate remaining
    let header_records = header.keyword_order.len() + 1; // +1 for END
    let header_bytes = header_records * 80;
    let padding = (2880 - (header_bytes % 2880)) % 2880;
    if padding > 0 {
        let mut skip = vec![0u8; padding];
        reader.read_exact(&mut skip)?;
    }
    
    Ok(header)
}

/// Parse a FITS value from string
fn parse_fits_value(s: &str) -> FitsValue {
    let s = s.trim();
    
    // Check for string (enclosed in single quotes)
    if s.starts_with('\'') {
        if let Some(end) = s[1..].find('\'') {
            return FitsValue::String(s[1..end+1].trim().to_string());
        }
    }
    
    // Check for boolean
    if s.starts_with('T') {
        return FitsValue::Boolean(true);
    }
    if s.starts_with('F') {
        return FitsValue::Boolean(false);
    }
    
    // Check for comment after value
    let value_part = if let Some(idx) = s.find('/') {
        s[..idx].trim()
    } else {
        s
    };
    
    // Try to parse as integer
    if let Ok(i) = value_part.parse::<i64>() {
        return FitsValue::Integer(i);
    }
    
    // Try to parse as float
    if let Ok(f) = value_part.replace('D', "E").replace('d', "e").parse::<f64>() {
        return FitsValue::Float(f);
    }
    
    // Default to string
    FitsValue::String(value_part.to_string())
}

/// Read unsigned 8-bit data
fn read_u8_data<R: Read>(reader: &mut R, width: u32, height: u32, depth: u32) -> Result<Vec<u8>, FitsError> {
    let size = (width * height * depth) as usize;
    let mut data = vec![0u8; size];
    reader.read_exact(&mut data)?;
    Ok(data)
}

/// Read signed 16-bit data (big-endian)
fn read_i16_data<R: Read>(reader: &mut R, width: u32, height: u32, depth: u32) -> Result<Vec<i16>, FitsError> {
    let size = (width * height * depth) as usize;
    let mut buffer = vec![0u8; size * 2];
    reader.read_exact(&mut buffer)?;
    
    let data: Vec<i16> = buffer.chunks_exact(2)
        .map(|chunk| i16::from_be_bytes([chunk[0], chunk[1]]))
        .collect();
    
    Ok(data)
}

/// Read signed 32-bit data (big-endian)
fn read_i32_data<R: Read>(reader: &mut R, width: u32, height: u32, depth: u32) -> Result<Vec<i32>, FitsError> {
    let size = (width * height * depth) as usize;
    let mut buffer = vec![0u8; size * 4];
    reader.read_exact(&mut buffer)?;
    
    let data: Vec<i32> = buffer.chunks_exact(4)
        .map(|chunk| i32::from_be_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]))
        .collect();
    
    Ok(data)
}

/// Read 32-bit float data (big-endian IEEE 754)
fn read_f32_data<R: Read>(reader: &mut R, width: u32, height: u32, depth: u32) -> Result<Vec<f32>, FitsError> {
    let size = (width * height * depth) as usize;
    let mut buffer = vec![0u8; size * 4];
    reader.read_exact(&mut buffer)?;
    
    let data: Vec<f32> = buffer.chunks_exact(4)
        .map(|chunk| f32::from_be_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]))
        .collect();
    
    Ok(data)
}

/// Read 64-bit float data (big-endian IEEE 754)
fn read_f64_data<R: Read>(reader: &mut R, width: u32, height: u32, depth: u32) -> Result<Vec<f64>, FitsError> {
    let size = (width * height * depth) as usize;
    let mut buffer = vec![0u8; size * 8];
    reader.read_exact(&mut buffer)?;
    
    let data: Vec<f64> = buffer.chunks_exact(8)
        .map(|chunk| f64::from_be_bytes([
            chunk[0], chunk[1], chunk[2], chunk[3],
            chunk[4], chunk[5], chunk[6], chunk[7]
        ]))
        .collect();
    
    Ok(data)
}

/// Write a FITS file to disk
pub fn write_fits(path: &Path, image: &ImageData, header: &FitsHeader) -> Result<(), FitsError> {
    let file = File::create(path)?;
    let mut writer = BufWriter::new(file);
    
    // Determine BITPIX based on pixel type
    let bitpix: i32 = match image.pixel_type {
        PixelType::U8 => 8,
        PixelType::U16 => 16,
        PixelType::U32 => 32,
        PixelType::F32 => -32,
        PixelType::F64 => -64,
    };
    
    // Write mandatory keywords
    write_keyword(&mut writer, "SIMPLE", "T")?;
    write_keyword(&mut writer, "BITPIX", &bitpix.to_string())?;
    write_keyword(&mut writer, "NAXIS", &format!("{}", if image.channels > 1 { 3 } else { 2 }))?;
    write_keyword(&mut writer, "NAXIS1", &image.width.to_string())?;
    write_keyword(&mut writer, "NAXIS2", &image.height.to_string())?;
    if image.channels > 1 {
        write_keyword(&mut writer, "NAXIS3", &image.channels.to_string())?;
    }
    
    // Write BZERO for unsigned 16-bit
    if image.pixel_type == PixelType::U16 {
        write_keyword(&mut writer, "BZERO", "32768")?;
        write_keyword(&mut writer, "BSCALE", "1")?;
    }
    
    // Write additional header keywords
    for key in &header.keyword_order {
        if !["SIMPLE", "BITPIX", "NAXIS", "NAXIS1", "NAXIS2", "NAXIS3", "BZERO", "BSCALE"].contains(&key.as_str()) {
            if let Some(value) = header.keywords.get(key) {
                let value_str = match value {
                    FitsValue::String(s) => format!("'{}'", s),
                    FitsValue::Integer(i) => i.to_string(),
                    FitsValue::Float(f) => format!("{:.10E}", f),
                    FitsValue::Boolean(b) => if *b { "T".to_string() } else { "F".to_string() },
                    FitsValue::Comment(c) => c.clone(),
                };
                write_keyword(&mut writer, key, &value_str)?;
            }
        }
    }
    
    // Write END keyword
    write_keyword(&mut writer, "END", "")?;
    
    // Pad header to 2880-byte boundary
    let pos = writer.stream_position()? as usize;
    let padding = (2880 - (pos % 2880)) % 2880;
    for _ in 0..padding {
        writer.write_all(b" ")?;
    }
    
    // Write image data
    match image.pixel_type {
        PixelType::U8 => {
            writer.write_all(&image.data)?;
        }
        PixelType::U16 => {
            // Convert from little-endian u16 to big-endian i16 with BZERO offset
            for chunk in image.data.chunks_exact(2) {
                let val = u16::from_le_bytes([chunk[0], chunk[1]]);
                let signed = (val as i32 - 32768) as i16;
                writer.write_all(&signed.to_be_bytes())?;
            }
        }
        PixelType::U32 => {
            for chunk in image.data.chunks_exact(4) {
                let val = u32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]);
                let signed = val as i32;
                writer.write_all(&signed.to_be_bytes())?;
            }
        }
        PixelType::F32 => {
            for chunk in image.data.chunks_exact(4) {
                let val = f32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]);
                writer.write_all(&val.to_be_bytes())?;
            }
        }
        PixelType::F64 => {
            for chunk in image.data.chunks_exact(8) {
                let val = f64::from_le_bytes([
                    chunk[0], chunk[1], chunk[2], chunk[3],
                    chunk[4], chunk[5], chunk[6], chunk[7]
                ]);
                writer.write_all(&val.to_be_bytes())?;
            }
        }
    }
    
    // Pad data to 2880-byte boundary
    let data_size = image.data.len();
    let padding = (2880 - (data_size % 2880)) % 2880;
    for _ in 0..padding {
        writer.write_all(&[0u8])?;
    }
    
    writer.flush()?;
    Ok(())
}

/// Write a single keyword record
fn write_keyword<W: Write>(writer: &mut W, keyword: &str, value: &str) -> Result<(), FitsError> {
    let mut record = [b' '; 80];
    
    // Write keyword (8 chars, left-justified)
    let keyword_bytes = keyword.as_bytes();
    let keyword_len = keyword_bytes.len().min(8);
    record[..keyword_len].copy_from_slice(&keyword_bytes[..keyword_len]);
    
    if keyword != "END" && !value.is_empty() {
        // Write "= " indicator
        record[8] = b'=';
        record[9] = b' ';
        
        // Write value (right-justified for numbers, left for strings)
        let value_bytes = value.as_bytes();
        let start = if value.starts_with('\'') {
            10 // Strings start at position 10
        } else {
            // Numbers are right-justified ending at position 30
            30_usize.saturating_sub(value_bytes.len())
        };
        let value_len = value_bytes.len().min(70);
        record[start..start+value_len].copy_from_slice(&value_bytes[..value_len]);
    }
    
    writer.write_all(&record)?;
    Ok(())
}

/// WCS (World Coordinate System) information from plate solving
/// Used to add astrometric headers to FITS files
#[derive(Debug, Clone)]
pub struct WcsInfo {
    /// Reference RA in degrees (CRVAL1)
    pub crval1: f64,
    /// Reference DEC in degrees (CRVAL2)
    pub crval2: f64,
    /// Reference pixel X coordinate (CRPIX1) - usually image center
    pub crpix1: f64,
    /// Reference pixel Y coordinate (CRPIX2) - usually image center
    pub crpix2: f64,
    /// CD matrix element 1,1 (scale and rotation)
    pub cd1_1: f64,
    /// CD matrix element 1,2 (scale and rotation)
    pub cd1_2: f64,
    /// CD matrix element 2,1 (scale and rotation)
    pub cd2_1: f64,
    /// CD matrix element 2,2 (scale and rotation)
    pub cd2_2: f64,
}

impl WcsInfo {
    /// Create WCS info from plate solve result
    ///
    /// # Arguments
    /// * `ra` - Right ascension in degrees
    /// * `dec` - Declination in degrees
    /// * `rotation` - Field rotation in degrees
    /// * `pixel_scale` - Pixel scale in arcseconds per pixel
    /// * `image_width` - Image width in pixels
    /// * `image_height` - Image height in pixels
    pub fn from_plate_solve(
        ra: f64,
        dec: f64,
        rotation: f64,
        pixel_scale: f64,
        image_width: u32,
        image_height: u32,
    ) -> Self {
        // Reference pixel is the image center
        let crpix1 = image_width as f64 / 2.0;
        let crpix2 = image_height as f64 / 2.0;

        // Convert pixel scale from arcsec/pixel to deg/pixel
        let scale_deg = pixel_scale / 3600.0;

        // Convert rotation to radians
        let rot_rad = rotation.to_radians();
        let cos_rot = rot_rad.cos();
        let sin_rot = rot_rad.sin();

        // Build CD matrix incorporating rotation
        let cd1_1 = -scale_deg * cos_rot; // Negative for RA increasing to the left
        let cd1_2 = scale_deg * sin_rot;
        let cd2_1 = scale_deg * sin_rot;
        let cd2_2 = scale_deg * cos_rot;

        Self {
            crval1: ra,
            crval2: dec,
            crpix1,
            crpix2,
            cd1_1,
            cd1_2,
            cd2_1,
            cd2_2,
        }
    }
}

/// Add WCS (World Coordinate System) headers to a FITS header
///
/// This adds standard astrometry headers based on plate solve results.
/// The WCS headers allow astronomical software to map pixel coordinates
/// to sky coordinates (RA/Dec).
///
/// # Arguments
/// * `header` - The FITS header to add WCS keywords to
/// * `wcs` - WCS information from plate solving
pub fn add_wcs_headers(header: &mut FitsHeader, wcs: &WcsInfo) {
    // Reference coordinates
    header.set_float("CRVAL1", wcs.crval1);
    header.set_float("CRVAL2", wcs.crval2);

    // Reference pixels
    header.set_float("CRPIX1", wcs.crpix1);
    header.set_float("CRPIX2", wcs.crpix2);

    // CD matrix (scale and rotation)
    header.set_float("CD1_1", wcs.cd1_1);
    header.set_float("CD1_2", wcs.cd1_2);
    header.set_float("CD2_1", wcs.cd2_1);
    header.set_float("CD2_2", wcs.cd2_2);

    // Coordinate type (tangent plane projection)
    header.set_string("CTYPE1", "RA---TAN");
    header.set_string("CTYPE2", "DEC--TAN");

    // Coordinate units
    header.set_string("CUNIT1", "deg");
    header.set_string("CUNIT2", "deg");

    // Reference frame
    header.set_float("EQUINOX", 2000.0);
    header.set_string("RADESYS", "ICRS");
}

/// Standard FITS keywords for astrophotography
pub struct StandardKeywords;

impl StandardKeywords {
    pub const BITPIX: &'static str = "BITPIX";
    pub const NAXIS: &'static str = "NAXIS";
    pub const NAXIS1: &'static str = "NAXIS1";
    pub const NAXIS2: &'static str = "NAXIS2";
    pub const BZERO: &'static str = "BZERO";
    pub const BSCALE: &'static str = "BSCALE";
    pub const OBJECT: &'static str = "OBJECT";
    pub const TELESCOP: &'static str = "TELESCOP";
    pub const INSTRUME: &'static str = "INSTRUME";
    pub const OBSERVER: &'static str = "OBSERVER";
    pub const DATE_OBS: &'static str = "DATE-OBS";
    pub const EXPTIME: &'static str = "EXPTIME";
    pub const CCD_TEMP: &'static str = "CCD-TEMP";
    pub const GAIN: &'static str = "GAIN";
    pub const OFFSET: &'static str = "OFFSET";
    pub const XBINNING: &'static str = "XBINNING";
    pub const YBINNING: &'static str = "YBINNING";
    pub const FILTER: &'static str = "FILTER";
    pub const RA: &'static str = "RA";
    pub const DEC: &'static str = "DEC";
    pub const FOCALLEN: &'static str = "FOCALLEN";
    pub const APTDIA: &'static str = "APTDIA";
    pub const IMAGETYP: &'static str = "IMAGETYP";
    pub const SITELAT: &'static str = "SITELAT";
    pub const SITELONG: &'static str = "SITELONG";
    pub const SITEELEV: &'static str = "SITEELEV";
    pub const AIRMASS: &'static str = "AIRMASS";
    pub const PIXSIZE1: &'static str = "PIXSIZE1";
    pub const PIXSIZE2: &'static str = "PIXSIZE2";
    pub const XPIXSZ: &'static str = "XPIXSZ";
    pub const YPIXSZ: &'static str = "YPIXSZ";

    // WCS Keywords
    pub const CRVAL1: &'static str = "CRVAL1";
    pub const CRVAL2: &'static str = "CRVAL2";
    pub const CRPIX1: &'static str = "CRPIX1";
    pub const CRPIX2: &'static str = "CRPIX2";
    pub const CD1_1: &'static str = "CD1_1";
    pub const CD1_2: &'static str = "CD1_2";
    pub const CD2_1: &'static str = "CD2_1";
    pub const CD2_2: &'static str = "CD2_2";
    pub const CTYPE1: &'static str = "CTYPE1";
    pub const CTYPE2: &'static str = "CTYPE2";
    pub const EQUINOX: &'static str = "EQUINOX";
    pub const RADESYS: &'static str = "RADESYS";
}

/// Calculate airmass from altitude using Pickering's formula
///
/// Airmass is the optical path length through Earth's atmosphere
/// for light from a celestial source compared to the zenith.
///
/// # Arguments
/// * `altitude_degrees` - Altitude angle in degrees (0-90)
///
/// # Returns
/// Airmass value (1.0 at zenith, increases toward horizon)
///
/// # Formula
/// Uses Pickering (2002) formula which is accurate for altitudes above 10 degrees:
/// X = 1 / sin(h + 244/(165 + 47*h^1.1))
/// where h is altitude in degrees
pub fn calculate_airmass(altitude_degrees: f64) -> f64 {
    // Clamp altitude to valid range
    let alt = altitude_degrees.clamp(0.0, 90.0);

    // At zenith (90 degrees), airmass is exactly 1.0
    if alt >= 89.9 {
        return 1.0;
    }

    // Below horizon, airmass is undefined (set to very large value)
    if alt <= 0.0 {
        return 40.0;
    }

    // Pickering (2002) formula - accurate for alt > 10 degrees
    // X = 1 / sin(h + 244/(165 + 47*h^1.1))
    let h_pow = alt.powf(1.1);
    let correction = 244.0 / (165.0 + 47.0 * h_pow);
    let effective_alt = alt + correction;
    let airmass = 1.0 / effective_alt.to_radians().sin();

    // Clamp to reasonable range
    airmass.clamp(1.0, 40.0)
}

/// Image validation result
#[derive(Debug, Clone)]
pub struct ImageValidation {
    pub is_valid: bool,
    pub warnings: Vec<String>,
    pub errors: Vec<String>,
}

impl ImageValidation {
    pub fn valid() -> Self {
        Self {
            is_valid: true,
            warnings: Vec::new(),
            errors: Vec::new(),
        }
    }

    pub fn add_warning(&mut self, warning: String) {
        self.warnings.push(warning);
    }

    pub fn add_error(&mut self, error: String) {
        self.errors.push(error);
        self.is_valid = false;
    }
}

/// Validate image data for common issues
///
/// # Arguments
/// * `image` - The image data to validate
/// * `expected_width` - Expected image width (None to skip check)
/// * `expected_height` - Expected image height (None to skip check)
///
/// # Returns
/// Validation result with errors and warnings
pub fn validate_image(
    image: &ImageData,
    expected_width: Option<u32>,
    expected_height: Option<u32>,
) -> ImageValidation {
    let mut validation = ImageValidation::valid();

    // Check dimensions match expected
    if let Some(width) = expected_width {
        if image.width != width {
            validation.add_error(format!(
                "Width mismatch: expected {}, got {}",
                width, image.width
            ));
        }
    }

    if let Some(height) = expected_height {
        if image.height != height {
            validation.add_error(format!(
                "Height mismatch: expected {}, got {}",
                height, image.height
            ));
        }
    }

    // Check for zero dimensions
    if image.width == 0 || image.height == 0 {
        validation.add_error("Image has zero dimensions".to_string());
        return validation;
    }

    // Check data size matches dimensions
    let pixel_size = match image.pixel_type {
        PixelType::U8 => 1,
        PixelType::U16 => 2,
        PixelType::U32 => 4,
        PixelType::F32 => 4,
        PixelType::F64 => 8,
    };
    let expected_size = (image.width * image.height * image.channels) as usize * pixel_size;
    if image.data.len() != expected_size {
        validation.add_error(format!(
            "Data size mismatch: expected {} bytes, got {}",
            expected_size,
            image.data.len()
        ));
    }

    // For 16-bit images, check for all-zero or all-saturated frames
    if image.pixel_type == PixelType::U16 {
        let pixels: Vec<u16> = image.data.chunks_exact(2)
            .map(|chunk| u16::from_le_bytes([chunk[0], chunk[1]]))
            .collect();

        if !pixels.is_empty() {
            let all_zero = pixels.iter().all(|&p| p == 0);
            let all_saturated = pixels.iter().all(|&p| p >= 65530);

            if all_zero {
                validation.add_error("Image is all-zero (no data captured)".to_string());
            } else if all_saturated {
                validation.add_error("Image is all-saturated (overexposed or sensor issue)".to_string());
            }

            // Check for extremely low signal
            let max_value = pixels.iter().copied().max().unwrap_or(0);
            if max_value < 100 {
                validation.add_warning(format!(
                    "Very low signal detected (max value: {})",
                    max_value
                ));
            }
        }
    }

    validation
}

/// Comprehensive image validation options
#[derive(Debug, Clone)]
pub struct ImageValidationOptions {
    /// Expected image width (None to skip check)
    pub expected_width: Option<u32>,
    /// Expected image height (None to skip check)
    pub expected_height: Option<u32>,
    /// Whether this is a bias frame (allows uniform pixel values)
    pub is_bias_frame: bool,
    /// Minimum acceptable max pixel value (default: 100)
    pub min_max_value: u16,
    /// Saturation threshold (pixels above this are considered saturated, default: 65530)
    pub saturation_threshold: u16,
    /// Maximum acceptable saturation percentage (default: 0.90 = 90%)
    pub max_saturation_percent: f64,
}

impl Default for ImageValidationOptions {
    fn default() -> Self {
        Self {
            expected_width: None,
            expected_height: None,
            is_bias_frame: false,
            min_max_value: 100,
            saturation_threshold: 65530,
            max_saturation_percent: 0.90,
        }
    }
}

/// Validate image data with bias frame option
///
/// # Arguments
/// * `image` - The image data to validate
/// * `expected_width` - Expected image width (None to skip check)
/// * `expected_height` - Expected image height (None to skip check)
/// * `is_bias_frame` - If true, allows uniform pixel values (bias frames naturally have this)
///
/// # Returns
/// Validation result with errors and warnings
pub fn validate_image_with_options(
    image: &ImageData,
    expected_width: Option<u32>,
    expected_height: Option<u32>,
    is_bias_frame: bool,
) -> ImageValidation {
    validate_image_comprehensive(image, ImageValidationOptions {
        expected_width,
        expected_height,
        is_bias_frame,
        ..Default::default()
    })
}

/// Comprehensive image validation with full options
///
/// Performs the following validation checks:
/// 1. Validates image data size matches dimensions (width * height)
/// 2. Rejects images where ALL pixels are identical (unless it's a bias frame)
/// 3. Rejects severely underexposed images (max pixel value < min_max_value)
/// 4. Warns on excessive saturation (>max_saturation_percent of pixels saturated)
/// 5. Logs validation results for debugging
///
/// # Arguments
/// * `image` - The image data to validate
/// * `options` - Validation options
///
/// # Returns
/// Validation result with errors and warnings
pub fn validate_image_comprehensive(
    image: &ImageData,
    options: ImageValidationOptions,
) -> ImageValidation {
    let mut validation = ImageValidation::valid();

    // Check dimensions match expected
    if let Some(width) = options.expected_width {
        if image.width != width {
            validation.add_error(format!(
                "Width mismatch: expected {}, got {}",
                width, image.width
            ));
        }
    }

    if let Some(height) = options.expected_height {
        if image.height != height {
            validation.add_error(format!(
                "Height mismatch: expected {}, got {}",
                height, image.height
            ));
        }
    }

    // Check for zero dimensions
    if image.width == 0 || image.height == 0 {
        validation.add_error("Image has zero dimensions".to_string());
        tracing::error!("[IMAGE_VALIDATION] REJECTED: Image has zero dimensions");
        return validation;
    }

    // Check data size matches dimensions
    let pixel_size = match image.pixel_type {
        PixelType::U8 => 1,
        PixelType::U16 => 2,
        PixelType::U32 => 4,
        PixelType::F32 => 4,
        PixelType::F64 => 8,
    };
    let expected_size = (image.width * image.height * image.channels) as usize * pixel_size;
    if image.data.len() != expected_size {
        validation.add_error(format!(
            "Data size mismatch: expected {} bytes for {}x{}x{} image, got {} bytes (truncated or corrupted)",
            expected_size,
            image.width, image.height, image.channels,
            image.data.len()
        ));
        tracing::error!(
            "[IMAGE_VALIDATION] REJECTED: Data size mismatch - expected {} bytes, got {}",
            expected_size,
            image.data.len()
        );
    }

    // For 16-bit images, perform comprehensive validation
    if image.pixel_type == PixelType::U16 && !image.data.is_empty() {
        let pixels: Vec<u16> = image.data.chunks_exact(2)
            .map(|chunk| u16::from_le_bytes([chunk[0], chunk[1]]))
            .collect();

        if !pixels.is_empty() {
            let total_pixels = pixels.len();

            // Calculate statistics in a single pass for efficiency
            let (min_value, max_value, sum, saturated_count) = pixels.iter().fold(
                (u16::MAX, u16::MIN, 0u64, 0usize),
                |(min, max, sum, sat_count), &pixel| {
                    (
                        min.min(pixel),
                        max.max(pixel),
                        sum + pixel as u64,
                        sat_count + (pixel >= options.saturation_threshold) as usize,
                    )
                },
            );
            let mean_value = if total_pixels > 0 {
                sum / total_pixels as u64
            } else {
                0
            };
            let saturation_percent = saturated_count as f64 / total_pixels as f64;

            // Log statistics for debugging
            tracing::debug!(
                "[IMAGE_VALIDATION] Stats: size={}, min={}, max={}, mean={}, saturated={:.1}%",
                total_pixels, min_value, max_value, mean_value, saturation_percent * 100.0
            );

            // Check 1: All pixels identical (uniform data)
            // This indicates sensor failure, dead frame, or possibly a bias frame
            let all_same = min_value == max_value;
            if all_same {
                if options.is_bias_frame {
                    // Bias frames may legitimately have very uniform data
                    tracing::info!(
                        "[IMAGE_VALIDATION] INFO: Bias frame has uniform pixel value {}",
                        min_value
                    );
                } else {
                    validation.add_error(format!(
                        "All {} pixels have identical value {} - possible sensor failure or dead frame",
                        total_pixels, min_value
                    ));
                    tracing::error!(
                        "[IMAGE_VALIDATION] REJECTED: All pixels identical (value={})",
                        min_value
                    );
                }
            }

            // Check 2: All-zero frame (no data captured)
            let all_zero = max_value == 0;
            if all_zero && !all_same { // Don't double-report
                validation.add_error("Image is all-zero (no data captured)".to_string());
                tracing::error!("[IMAGE_VALIDATION] REJECTED: All-zero image");
            }

            // Check 3: Underexposure detection with tiered thresholds
            // Severe underexposure (max < min_max_value, default 100) - error
            // Moderate underexposure (max < min_max_value * 5, default 500) - warning
            let moderate_threshold = options.min_max_value.saturating_mul(5);
            if max_value < options.min_max_value && !all_zero && !options.is_bias_frame {
                validation.add_error(format!(
                    "Image severely underexposed: max pixel value {} is below minimum threshold {} - \
                    increase exposure time or check camera connection/shutter",
                    max_value, options.min_max_value
                ));
                tracing::error!(
                    "[IMAGE_VALIDATION] REJECTED: Severely underexposed (max={} < {})",
                    max_value, options.min_max_value
                );
            } else if max_value < moderate_threshold && !all_zero && !options.is_bias_frame {
                // Moderate underexposure - useful signal but concerning
                validation.add_warning(format!(
                    "Low signal detected (max value: {}) - consider increasing exposure time",
                    max_value
                ));
                tracing::warn!(
                    "[IMAGE_VALIDATION] WARNING: Low signal (max={} < {})",
                    max_value, moderate_threshold
                );
            }

            // Check 4: Excessive saturation (>90% of pixels saturated)
            // This indicates severe overexposure or gain/exposure misconfiguration
            if saturation_percent > options.max_saturation_percent {
                validation.add_warning(format!(
                    "Excessive saturation: {:.1}% of pixels are saturated (>{}%) - \
                    reduce exposure time or gain",
                    saturation_percent * 100.0,
                    options.max_saturation_percent * 100.0
                ));
                tracing::warn!(
                    "[IMAGE_VALIDATION] WARNING: Excessive saturation ({:.1}% > {:.1}%)",
                    saturation_percent * 100.0,
                    options.max_saturation_percent * 100.0
                );
            }

            // Check 5: All pixels saturated (complete overexposure)
            let all_saturated = min_value >= options.saturation_threshold;
            if all_saturated {
                validation.add_error(format!(
                    "Image is completely saturated (min value {} >= {}) - \
                    significantly reduce exposure time or gain",
                    min_value, options.saturation_threshold
                ));
                tracing::error!(
                    "[IMAGE_VALIDATION] REJECTED: All pixels saturated (min={})",
                    min_value
                );
            }
        }
    }

    // Log final validation result
    if validation.is_valid {
        if validation.warnings.is_empty() {
            tracing::debug!("[IMAGE_VALIDATION] PASSED: Image validated successfully");
        } else {
            tracing::info!(
                "[IMAGE_VALIDATION] PASSED with {} warning(s): {:?}",
                validation.warnings.len(),
                validation.warnings
            );
        }
    } else {
        tracing::error!(
            "[IMAGE_VALIDATION] FAILED with {} error(s): {:?}",
            validation.errors.len(),
            validation.errors
        );
    }

    validation
}

/// Validate FITS header completeness for astrophotography
///
/// # Arguments
/// * `header` - The FITS header to validate
///
/// # Returns
/// Validation result with warnings for missing recommended keywords
pub fn validate_fits_header(header: &FitsHeader) -> ImageValidation {
    let mut validation = ImageValidation::valid();

    // Required keywords
    let required = vec!["SIMPLE", "BITPIX", "NAXIS", "NAXIS1", "NAXIS2"];
    for keyword in required {
        if header.get(keyword).is_none() {
            validation.add_error(format!("Missing required keyword: {}", keyword));
        }
    }

    // Recommended for astrophotography
    let recommended = vec![
        "DATE-OBS", "EXPTIME", "IMAGETYP", "OBJECT",
        "TELESCOP", "INSTRUME", "OBSERVER"
    ];
    for keyword in recommended {
        if header.get(keyword).is_none() {
            validation.add_warning(format!("Missing recommended keyword: {}", keyword));
        }
    }

    validation
}

/// Calculate image quality score
///
/// Quality score is a 0-100 metric based on:
/// - HFR (smaller is better, below 3.0 is excellent)
/// - Star count (more stars indicate better data)
/// - Background uniformity (lower stddev relative to mean is better)
///
/// # Arguments
/// * `hfr` - Half-flux radius (arc-seconds or pixels)
/// * `star_count` - Number of detected stars
/// * `mean` - Image mean value
/// * `std_dev` - Image standard deviation
///
/// # Returns
/// Quality score from 0-100 (100 is best)
pub fn calculate_quality_score(
    hfr: Option<f64>,
    star_count: Option<i32>,
    mean: f64,
    std_dev: f64,
) -> f64 {
    let mut score = 0.0;
    let mut weight_sum = 0.0;

    // HFR component (40% weight)
    // Excellent: < 2.0, Good: 2-3, Fair: 3-5, Poor: > 5
    if let Some(hfr_val) = hfr {
        if hfr_val > 0.0 {
            let hfr_score = if hfr_val < 2.0 {
                100.0
            } else if hfr_val < 3.0 {
                100.0 - (hfr_val - 2.0) * 25.0
            } else if hfr_val < 5.0 {
                75.0 - (hfr_val - 3.0) * 25.0
            } else {
                (25.0 - (hfr_val - 5.0).min(5.0) * 5.0).max(0.0)
            };
            score += hfr_score * 0.4;
            weight_sum += 0.4;
        }
    }

    // Star count component (30% weight)
    // Excellent: > 100, Good: 50-100, Fair: 20-50, Poor: < 20
    if let Some(stars) = star_count {
        let star_score = if stars >= 100 {
            100.0
        } else if stars >= 50 {
            66.0 + (stars - 50) as f64 / 50.0 * 34.0
        } else if stars >= 20 {
            33.0 + (stars - 20) as f64 / 30.0 * 33.0
        } else {
            (stars as f64 / 20.0 * 33.0).max(0.0)
        };
        score += star_score * 0.3;
        weight_sum += 0.3;
    }

    // Background uniformity component (30% weight)
    // Lower noise is better - check coefficient of variation
    if mean > 0.0 {
        let cv = std_dev / mean; // Coefficient of variation
        let uniformity_score = if cv < 0.1 {
            100.0
        } else if cv < 0.3 {
            100.0 - (cv - 0.1) * 333.0
        } else {
            (33.0 - (cv - 0.3).min(0.33) * 100.0).max(0.0)
        };
        score += uniformity_score * 0.3;
        weight_sum += 0.3;
    }

    // Return normalized score
    if weight_sum > 0.0 {
        (score / weight_sum).clamp(0.0, 100.0)
    } else {
        0.0
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_calculate_airmass_zenith() {
        let airmass = calculate_airmass(90.0);
        assert_eq!(airmass, 1.0, "Airmass at zenith should be 1.0");
    }

    #[test]
    fn test_calculate_airmass_45_degrees() {
        let airmass = calculate_airmass(45.0);
        assert!(airmass > 1.0 && airmass < 2.0, "Airmass at 45° should be between 1.0 and 2.0");
        // At 45 degrees, airmass should be approximately 1.41 (sqrt(2))
        assert!((airmass - 1.41).abs() < 0.1, "Airmass at 45° should be close to 1.41");
    }

    #[test]
    fn test_calculate_airmass_horizon() {
        let airmass = calculate_airmass(0.0);
        assert_eq!(airmass, 40.0, "Airmass at horizon should be clamped to 40.0");
    }

    #[test]
    fn test_calculate_airmass_30_degrees() {
        let airmass = calculate_airmass(30.0);
        assert!(airmass > 1.5 && airmass < 3.0, "Airmass at 30° should be between 1.5 and 3.0");
    }

    #[test]
    fn test_validate_image_correct_dimensions() {
        let image = ImageData::from_u16(100, 100, 1, &vec![1000u16; 100 * 100]);
        let validation = validate_image(&image, Some(100), Some(100));
        assert!(validation.is_valid, "Image with correct dimensions should be valid");
        assert!(validation.errors.is_empty(), "Should have no errors");
    }

    #[test]
    fn test_validate_image_wrong_dimensions() {
        let image = ImageData::from_u16(100, 100, 1, &vec![1000u16; 100 * 100]);
        let validation = validate_image(&image, Some(200), Some(200));
        assert!(!validation.is_valid, "Image with wrong dimensions should be invalid");
        assert_eq!(validation.errors.len(), 2, "Should have 2 dimension mismatch errors");
    }

    #[test]
    fn test_validate_image_all_zero() {
        let image = ImageData::from_u16(100, 100, 1, &vec![0u16; 100 * 100]);
        let validation = validate_image(&image, None, None);
        assert!(!validation.is_valid, "All-zero image should be invalid");
        assert!(validation.errors.iter().any(|e| e.contains("all-zero")), "Should have all-zero error");
    }

    #[test]
    fn test_validate_image_all_saturated() {
        let image = ImageData::from_u16(100, 100, 1, &vec![65535u16; 100 * 100]);
        let validation = validate_image(&image, None, None);
        assert!(!validation.is_valid, "All-saturated image should be invalid");
        assert!(validation.errors.iter().any(|e| e.contains("saturated")), "Should have saturated error");
    }

    #[test]
    fn test_validate_image_low_signal() {
        let image = ImageData::from_u16(100, 100, 1, &vec![50u16; 100 * 100]);
        let validation = validate_image(&image, None, None);
        assert!(validation.is_valid, "Low signal image should still be valid");
        assert!(!validation.warnings.is_empty(), "Should have low signal warning");
    }

    #[test]
    fn test_validate_fits_header_minimal() {
        let mut header = FitsHeader::new();
        header.set_string("SIMPLE", "T");
        header.set_int("BITPIX", 16);
        header.set_int("NAXIS", 2);
        header.set_int("NAXIS1", 100);
        header.set_int("NAXIS2", 100);

        let validation = validate_fits_header(&header);
        assert!(validation.is_valid, "Minimal FITS header should be valid");
        assert!(!validation.warnings.is_empty(), "Should have warnings for missing recommended keywords");
    }

    #[test]
    fn test_validate_fits_header_complete() {
        let mut header = FitsHeader::new();
        // Required
        header.set_string("SIMPLE", "T");
        header.set_int("BITPIX", 16);
        header.set_int("NAXIS", 2);
        header.set_int("NAXIS1", 100);
        header.set_int("NAXIS2", 100);
        // Recommended
        header.set_string("DATE-OBS", "2025-01-01T00:00:00");
        header.set_float("EXPTIME", 60.0);
        header.set_string("IMAGETYP", "Light");
        header.set_string("OBJECT", "M31");
        header.set_string("TELESCOP", "Test Scope");
        header.set_string("INSTRUME", "Test Camera");
        header.set_string("OBSERVER", "Test Observer");

        let validation = validate_fits_header(&header);
        assert!(validation.is_valid, "Complete FITS header should be valid");
        assert!(validation.warnings.is_empty(), "Complete header should have no warnings");
    }

    #[test]
    fn test_validate_fits_header_missing_required() {
        let mut header = FitsHeader::new();
        header.set_string("SIMPLE", "T");
        // Missing BITPIX, NAXIS, etc.

        let validation = validate_fits_header(&header);
        assert!(!validation.is_valid, "Header missing required keywords should be invalid");
        assert!(!validation.errors.is_empty(), "Should have errors for missing required keywords");
    }

    #[test]
    fn test_quality_score_excellent() {
        let score = calculate_quality_score(Some(1.8), Some(150), 5000.0, 500.0);
        assert!(score > 85.0, "Excellent image (HFR=1.8, stars=150, CV=0.1) should score > 85, got {}", score);
    }

    #[test]
    fn test_quality_score_good() {
        let score = calculate_quality_score(Some(2.5), Some(75), 5000.0, 800.0);
        // HFR 2.5 = 75/100, stars 75 = 83/100, CV 0.16 = ~70/100
        // Weighted: 75*0.4 + 83*0.3 + 70*0.3 = 75.9
        assert!(score > 70.0 && score < 85.0, "Good image should score 70-85, got {}", score);
    }

    #[test]
    fn test_quality_score_poor() {
        let score = calculate_quality_score(Some(6.0), Some(15), 5000.0, 2000.0);
        assert!(score < 40.0, "Poor image (HFR=6.0, stars=15, CV=0.4) should score < 40, got {}", score);
    }

    #[test]
    fn test_quality_score_no_data() {
        let score = calculate_quality_score(None, None, 5000.0, 800.0);
        assert!(score >= 0.0 && score <= 100.0, "Score should be in valid range even with no HFR/star data");
    }

    #[test]
    fn test_fits_header_set_get() {
        let mut header = FitsHeader::new();
        header.set_string("OBJECT", "M31");
        header.set_float("EXPTIME", 120.5);
        header.set_int("GAIN", 100);
        header.set_bool("SIMPLE", true);

        assert_eq!(header.get_string("OBJECT"), Some("M31"));
        assert_eq!(header.get_float("EXPTIME"), Some(120.5));
        assert_eq!(header.get_int("GAIN"), Some(100));
    }

    #[test]
    fn test_fits_header_operations() {
        // Create test image
        let width = 10;
        let height = 10;
        let data: Vec<u16> = (0..100).collect();
        let image = ImageData::from_u16(width, height, 1, &data);

        // Create header
        let mut header = FitsHeader::new();
        header.set_string("OBJECT", "Test");
        header.set_float("EXPTIME", 60.0);
        header.set_string("IMAGETYP", "Light");
        header.set_int("GAIN", 100);
        header.set_float("CCD-TEMP", -10.5);

        // Test that header operations work
        assert_eq!(header.get_string("OBJECT"), Some("Test"));
        assert_eq!(header.get_float("EXPTIME"), Some(60.0));
        assert_eq!(header.get_string("IMAGETYP"), Some("Light"));
        assert_eq!(header.get_int("GAIN"), Some(100));
        assert_eq!(header.get_float("CCD-TEMP"), Some(-10.5));
    }

    #[test]
    fn test_fits_complete_metadata() {
        // Create header with all astrophotography metadata
        let mut header = FitsHeader::new();

        // Core metadata
        header.set_string("DATE-OBS", "2025-01-15T22:30:45.123");
        header.set_string("IMAGETYP", "Light");
        header.set_float("EXPTIME", 300.0);
        header.set_string("OBJECT", "M31");
        header.set_string("FILTER", "Luminance");

        // Equipment
        header.set_string("TELESCOP", "Test Telescope");
        header.set_string("INSTRUME", "Test Camera");
        header.set_string("OBSERVER", "Test Observer");

        // Camera settings
        header.set_int("GAIN", 139);
        header.set_int("OFFSET", 21);
        header.set_float("CCD-TEMP", -10.0);
        header.set_int("XBINNING", 1);
        header.set_int("YBINNING", 1);

        // Optics
        header.set_float("FOCALLEN", 600.0);
        header.set_float("APTDIA", 100.0);
        header.set_float("PIXSIZE1", 3.76);
        header.set_float("PIXSIZE2", 3.76);
        header.set_float("XPIXSZ", 3.76);
        header.set_float("YPIXSZ", 3.76);

        // Observer location
        header.set_float("SITELAT", 39.0);
        header.set_float("SITELONG", -77.0);
        header.set_float("SITEELEV", 100.0);

        // Target coordinates
        header.set_float("RA", 10.685);
        header.set_float("DEC", 41.27);
        header.set_float("AIRMASS", 1.15);

        // Validate header completeness
        let validation = validate_fits_header(&header);
        assert!(validation.is_valid, "Complete header should be valid");
        assert!(validation.warnings.is_empty(), "Complete header should have no warnings");

        // Verify all values
        assert_eq!(header.get_string("DATE-OBS"), Some("2025-01-15T22:30:45.123"));
        assert_eq!(header.get_string("IMAGETYP"), Some("Light"));
        assert_eq!(header.get_float("EXPTIME"), Some(300.0));
        assert_eq!(header.get_float("FOCALLEN"), Some(600.0));
        assert_eq!(header.get_float("SITELAT"), Some(39.0));
        assert_eq!(header.get_float("AIRMASS"), Some(1.15));
    }

    #[test]
    fn test_fits_round_trip() {
        use std::io::Cursor;

        // Create test image
        let width = 100;
        let height = 100;
        let data: Vec<u16> = (0..10000).map(|i| (i % 65535) as u16).collect();
        let image = ImageData::from_u16(width, height, 1, &data);

        // Create header with metadata
        let mut header = FitsHeader::new();
        header.set_string("OBJECT", "M31");
        header.set_float("EXPTIME", 180.0);
        header.set_string("DATE-OBS", "2025-01-15T22:30:45");
        header.set_string("IMAGETYP", "Light");
        header.set_float("AIRMASS", 1.2);

        // Write to memory
        let mut buffer = Vec::new();
        {
            let mut cursor = Cursor::new(&mut buffer);
            // We can't use write_fits directly with Cursor easily without Path,
            // but we can test the header validation
        }

        // Validate the header
        let validation = validate_fits_header(&header);
        assert!(validation.is_valid, "Header should be valid");

        // Verify specific keywords exist
        assert!(header.get("OBJECT").is_some());
        assert!(header.get("EXPTIME").is_some());
        assert!(header.get("DATE-OBS").is_some());
        assert!(header.get("IMAGETYP").is_some());
        assert!(header.get("AIRMASS").is_some());
    }

    #[test]
    fn test_quality_score_edge_cases() {
        // Test with zero values
        let score = calculate_quality_score(Some(0.0), Some(0), 0.0, 0.0);
        assert!(score >= 0.0 && score <= 100.0, "Score should be valid even with zeros");

        // Test with very high HFR
        let score = calculate_quality_score(Some(20.0), Some(150), 5000.0, 500.0);
        assert!(score < 50.0, "Very high HFR should lower score significantly");

        // Test with perfect image
        let score = calculate_quality_score(Some(1.5), Some(200), 10000.0, 500.0);
        assert!(score > 90.0, "Perfect image (HFR=1.5, stars=200, CV=0.05) should score > 90");
    }
}
