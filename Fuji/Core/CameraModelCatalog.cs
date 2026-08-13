using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;

namespace ASCOM.ScdouglasFujifilm.Camera.Core
{
    internal static class CameraModelCatalog
    {
        private static readonly JsonSerializerOptions SerializerOptions = new JsonSerializerOptions
        {
            PropertyNameCaseInsensitive = true,
            ReadCommentHandling = JsonCommentHandling.Skip,
            AllowTrailingCommas = true
        };

        internal static CameraConfig Load(string assemblyDirectory, string productName, Action<string> log)
        {
            if (string.IsNullOrWhiteSpace(assemblyDirectory))
                throw new ArgumentException("The driver directory is not available.", nameof(assemblyDirectory));
            if (string.IsNullOrWhiteSpace(productName))
                throw new ArgumentException("The camera did not report a product name.", nameof(productName));

            string nested = Path.Combine(assemblyDirectory, "CameraConfigs");
            string directory = Directory.Exists(nested) ? nested : assemblyDirectory;
            if (!Directory.Exists(directory))
                throw new DirectoryNotFoundException("Camera configuration directory not found: " + directory);

            var configs = new List<CameraConfig>();
            foreach (string file in Directory.GetFiles(directory, "*.json", SearchOption.TopDirectoryOnly))
            {
                try
                {
                    string json = File.ReadAllText(file);
                    CameraConfig config = JsonSerializer.Deserialize<CameraConfig>(json, SerializerOptions);
                    if (IsValid(config)) configs.Add(config);
                    else log?.Invoke("Ignoring invalid camera configuration: " + file);
                }
                catch (Exception ex)
                {
                    log?.Invoke("Ignoring camera configuration '" + file + "': " + ex.Message);
                }
            }

            string normalizedProduct = Normalize(productName);
            CameraConfig match = configs
                .Select(config => new { Config = config, Key = Normalize(config.ModelName) })
                .Where(candidate => candidate.Key.Length > 0 &&
                    normalizedProduct.EndsWith(candidate.Key, StringComparison.OrdinalIgnoreCase))
                .OrderByDescending(candidate => candidate.Key.Length)
                .Select(candidate => candidate.Config)
                .FirstOrDefault();

            if (match == null)
                throw new InvalidOperationException("No camera configuration matches SDK product '" + productName + "'.");
            return match;
        }

        internal static string Normalize(string value)
        {
            return new string((value ?? string.Empty).Where(char.IsLetterOrDigit).ToArray());
        }

        internal static bool IsValid(CameraConfig config)
        {
            return config != null && !string.IsNullOrWhiteSpace(config.ModelName) &&
                config.CameraXSize > 0 && config.CameraYSize > 0 &&
                config.PixelSizeX > 0 && config.PixelSizeY > 0 &&
                config.DefaultMinSensitivity > 0 &&
                config.DefaultMaxSensitivity >= config.DefaultMinSensitivity &&
                config.DefaultMinExposure > 0 &&
                config.DefaultMaxExposure >= config.DefaultMinExposure &&
                config.SdkConstants != null;
        }
    }
}
