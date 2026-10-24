using HwMonitor.Collector.Communication;
using HwMonitor.Collector.Hardware;
using HwMonitor.Collector.Models;

const string serialPortName = "COM3";
const int baudRate = 115200;
TimeSpan refreshInterval = TimeSpan.FromSeconds(1);

using var esp32 = new Esp32SerialClient(serialPortName, baudRate);

using var monitor = new HardwareMonitorService();


try
{
    while (true)
    {
        HardwareSnapshot snapshot = monitor.ReadSnapshot();

        Console.Clear();
        Console.WriteLine("HW Monitor");
        Console.WriteLine("==========");
        Console.WriteLine($"Updated: {DateTime.Now:HH:mm:ss}");
        Console.WriteLine("Press Ctrl+C to exit");
        Console.WriteLine();

        PrintReading("CPU Temperature", snapshot.CpuTemperature, "°C");
        PrintReading("CPU Load", snapshot.CpuLoad, "%");
        PrintReading("GPU Temperature", snapshot.GpuTemperature, "°C");
        PrintReading("GPU Load", snapshot.GpuLoad, "%");

        bool sent = esp32.Send(snapshot);

        Console.WriteLine();
        Console.WriteLine($"ESP32: {(sent ? "Data sent" : "Disconnected — retrying...")}");

        await Task.Delay(refreshInterval);
    }
}
finally
{
}

static void PrintReading(string name, float? value, string unit)
{
    string formatted = value.HasValue
        ? $"{value.Value,6:F1} {unit}"
        : "   N/A";

    Console.WriteLine($"{name,-20} {formatted}");
}