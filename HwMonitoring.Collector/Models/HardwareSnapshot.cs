namespace HwMonitor.Collector.Models;

public sealed record HardwareSnapshot(float? CpuTemperature, float? CpuLoad, float? GpuTemperature, float? GpuLoad);