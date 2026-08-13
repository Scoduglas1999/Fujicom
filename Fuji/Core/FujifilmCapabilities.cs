using System;
using System.Collections.Generic;
using System.Linq;

namespace ASCOM.ScdouglasFujifilm.Camera.Core
{
    internal static class FujifilmCapabilities
    {
        internal const int BulbCode = -1;
        internal const int RawImageFormat = 1;
        private static readonly IReadOnlyDictionary<int, double> UniversalShutterMap = CreateUniversalShutterMap();

        internal static IList<int> FixedSensitivities(IEnumerable<int> reported)
        {
            if (reported == null) throw new ArgumentNullException(nameof(reported));
            // ICameraV3 exposes Gain as Int16, so expanded ISO values above 32767
            // cannot be represented safely through the ASCOM interface.
            return reported.Where(value => value > 0 && value <= short.MaxValue)
                .Distinct()
                .OrderBy(value => value)
                .ToList();
        }

        internal static bool IsRawImageFormat(int format)
        {
            // Bits 0x0f00 are orientation. The low byte is the actual image format.
            return (format & 0xff) == RawImageFormat;
        }

        internal static bool ResolveBulbCapability(bool sdkReported, bool configured)
        {
            // The SDK flag has returned false on bodies that subsequently completed bulb captures.
            return sdkReported || configured;
        }

        internal static IDictionary<int, double> BuildShutterMap(
            IEnumerable<int> supportedCodes,
            IEnumerable<ShutterSpeedMapping> modelMappings,
            Action<int> unknownCode)
        {
            var overrides = (modelMappings ?? Enumerable.Empty<ShutterSpeedMapping>())
                .Where(mapping => mapping != null && mapping.Duration > 0)
                .GroupBy(mapping => mapping.SdkCode)
                .ToDictionary(group => group.Key, group => group.Last().Duration);
            var result = new Dictionary<int, double>();

            foreach (int code in (supportedCodes ?? Enumerable.Empty<int>()).Distinct())
            {
                if (code == BulbCode) continue;
                double duration;
                if (overrides.TryGetValue(code, out duration) || UniversalShutterMap.TryGetValue(code, out duration))
                    result[code] = duration;
                else
                    unknownCode?.Invoke(code);
            }
            return result;
        }

        internal static int SelectShutterCode(IDictionary<int, double> map, double requestedSeconds, bool bulbCapable)
        {
            if (requestedSeconds <= 0) throw new ArgumentOutOfRangeException(nameof(requestedSeconds));
            var timed = (map ?? new Dictionary<int, double>())
                .Where(pair => pair.Key != BulbCode && pair.Value > 0)
                .ToArray();
            if (timed.Length == 0)
            {
                if (bulbCapable) return BulbCode;
                throw new InvalidOperationException("The camera reported no usable shutter speeds or bulb support.");
            }

            double maxTimed = timed.Max(pair => pair.Value);
            if (requestedSeconds > maxTimed)
            {
                if (bulbCapable) return BulbCode;
                throw new InvalidOperationException(
                    "The requested exposure exceeds the longest timed shutter speed and bulb mode is unavailable.");
            }

            return timed.OrderBy(pair => Math.Abs(pair.Value - requestedSeconds))
                .ThenBy(pair => pair.Value > requestedSeconds ? 1 : 0)
                .First().Key;
        }

        internal static double TimedMaximum(IDictionary<int, double> map, double fallback)
        {
            var durations = (map ?? new Dictionary<int, double>()).Values.Where(value => value > 0).ToArray();
            return durations.Length == 0 ? fallback : durations.Max();
        }

        internal static short PercentComplete(DateTime startedUtc, double requestedSeconds, bool downloading, bool ready)
        {
            if (ready) return 100;
            if (downloading) return 99;
            if (requestedSeconds <= 0 || startedUtc == DateTime.MinValue) return 0;
            double elapsed = (DateTime.UtcNow - startedUtc).TotalSeconds;
            return (short)Math.Max(0, Math.Min(98, Math.Floor(elapsed / requestedSeconds * 98.0)));
        }

        private static IReadOnlyDictionary<int, double> CreateUniversalShutterMap()
        {
            return new Dictionary<int, double>
            {
                [5] = 1.0 / 180000.0, [6] = 1.0 / 160000.0, [7] = 1.0 / 128000.0,
                [9] = 1.0 / 102400.0, [12] = 1.0 / 80000.0, [15] = 1.0 / 64000.0,
                [19] = 1.0 / 51200.0, [24] = 1.0 / 40000.0, [30] = 1.0 / 32000.0,
                [38] = 1.0 / 25600.0, [43] = 1.0 / 24000.0, [48] = 1.0 / 20000.0,
                [61] = 1.0 / 16000.0, [76] = 1.0 / 12800.0, [86] = 1.0 / 12000.0,
                [96] = 1.0 / 10000.0, [122] = 1.0 / 8000.0, [153] = 1.0 / 6400.0,
                [172] = 1.0 / 6000.0, [193] = 1.0 / 5000.0, [244] = 1.0 / 4000.0,
                [307] = 1.0 / 3200.0, [345] = 1.0 / 3000.0, [387] = 1.0 / 2500.0,
                [488] = 1.0 / 2000.0, [615] = 1.0 / 1600.0, [690] = 1.0 / 1500.0,
                [775] = 1.0 / 1250.0, [976] = 1.0 / 1000.0, [1230] = 1.0 / 800.0,
                [1381] = 1.0 / 750.0, [1550] = 1.0 / 640.0, [1953] = 1.0 / 500.0,
                [2460] = 1.0 / 400.0, [2762] = 1.0 / 350.0, [3100] = 1.0 / 320.0,
                [3906] = 1.0 / 250.0, [4921] = 1.0 / 200.0, [5524] = 1.0 / 180.0,
                [6200] = 1.0 / 160.0, [7812] = 1.0 / 125.0, [9843] = 1.0 / 100.0,
                [11048] = 1.0 / 90.0, [12401] = 1.0 / 80.0, [15625] = 1.0 / 60.0,
                [19686] = 1.0 / 50.0, [22097] = 1.0 / 45.0, [24803] = 1.0 / 40.0,
                [31250] = 1.0 / 30.0, [39372] = 1.0 / 25.0, [49606] = 1.0 / 20.0,
                [62500] = 1.0 / 15.0, [78745] = 1.0 / 13.0, [99212] = 1.0 / 10.0,
                [125000] = 1.0 / 8.0, [157490] = 1.0 / 6.0, [198425] = 1.0 / 5.0,
                [250000] = 1.0 / 4.0, [314980] = 1.0 / 3.0, [396850] = 1.0 / 2.5,
                [500000] = 1.0 / 2.0, [629960] = 1.0 / 1.6, [707106] = 1.0 / 1.5,
                [793700] = 1.0 / 1.3, [1000000] = 1.0, [1259921] = 1.3,
                [1414213] = 1.5, [1587401] = 1.6, [2000000] = 2.0,
                [2519842] = 2.5, [3174802] = 3.0, [4000000] = 4.0,
                [5039684] = 5.0, [6349604] = 6.0, [8000000] = 8.0,
                [10079368] = 10.0, [12699208] = 13.0, [16000000] = 15.0,
                [20158736] = 20.0, [25398416] = 25.0, [32000000] = 30.0,
                [64000000] = 60.0,
                [44194] = 1.0 / 20.0, [88388] = 1.0 / 10.0, [176776] = 1.0 / 6.0,
                [353553] = 1.0 / 3.0, [2828427] = 3.0, [5656854] = 6.0,
                [11313708] = 10.0, [22627416] = 20.0,
                [35000000] = 35.0, [40000000] = 40.0, [40317473] = 40.0,
                [45000000] = 45.0, [50000000] = 50.0, [50796833] = 52.0,
                [55000000] = 55.0, [60000000] = 60.0, [80634947] = 80.0,
                [101593667] = 110.0, [128000000] = 140.0, [161269894] = 170.0,
                [203187334] = 210.0, [256000000] = 250.0, [322539788] = 320.0,
                [406374669] = 420.0, [512000000] = 500.0, [645079577] = 640.0,
                [812749338] = 850.0, [1024000000] = 1000.0, [1290159155] = 1300.0,
                [1625498677] = 1700.0, [2048000000] = 2000.0,
                [64000030] = 120.0, [64000060] = 240.0, [64000090] = 480.0,
                [64000120] = 900.0, [64000150] = 1800.0, [64000180] = 3600.0
            };
        }
    }
}
