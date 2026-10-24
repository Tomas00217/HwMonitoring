using HwMonitor.Collector.Models;
using LibreHardwareMonitor.Hardware;

namespace HwMonitor.Collector.Hardware;

public sealed class HardwareMonitorService : IDisposable
{
    const string CpuTemperatureSensorName = "Core (Tctl/Tdie)";
    const string CpuLoadSensorName = "CPU Total";   
    const string GpuTemperatureSensorName = "GPU Core";
    const string GpuLoadSensorName = "GPU Core";

    private readonly Computer _computer;
    private readonly UpdateVisitor _updateVisitor = new();

    public HardwareMonitorService()
    {
        _computer = new Computer
        {
            IsCpuEnabled = true,
            IsGpuEnabled = true
        };

        _computer.Open();
    }

    public HardwareSnapshot ReadSnapshot()
    {
        _computer.Accept(_updateVisitor);

        var cpu = _computer.Hardware.FirstOrDefault(hw => hw.HardwareType == HardwareType.Cpu);
        var gpu = _computer.Hardware.FirstOrDefault(hw => hw.HardwareType == HardwareType.GpuAmd &&  hw.Name.Contains("RX 9070 XT", StringComparison.OrdinalIgnoreCase));

        return new HardwareSnapshot(
            CpuTemperature: GetSensorValue(cpu, SensorType.Temperature, CpuTemperatureSensorName),
            CpuLoad: GetSensorValue(cpu, SensorType.Load, CpuLoadSensorName),
            GpuTemperature: GetSensorValue(gpu, SensorType.Temperature, GpuTemperatureSensorName),
            GpuLoad: GetSensorValue(gpu, SensorType.Load, GpuLoadSensorName)
            );
    }

    private static float? GetSensorValue(
        IHardware? hardware,
        SensorType sensorType,
        string sensorName)
    {
        if (hardware is null)
        {
            return null;
        }

        ISensor? sensor = hardware.Sensors.FirstOrDefault(s =>
            s.SensorType == sensorType &&
            s.Name.Equals(
                sensorName,
                StringComparison.OrdinalIgnoreCase));

        return sensor?.Value is float value && float.IsFinite(value)
            ? value
            : null;
    }

    public void Dispose()
    {
        _computer.Close();
    }
}