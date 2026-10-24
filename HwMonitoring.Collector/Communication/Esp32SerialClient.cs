using System.Globalization;
using System.IO.Ports;
using HwMonitor.Collector.Models;

namespace HwMonitor.Collector.Communication;

public sealed class Esp32SerialClient : IDisposable
{
    private readonly string _portName;
    private readonly int _baudRate;
    private SerialPort? _serialPort;

    public Esp32SerialClient(string portName, int baudRate)
    {
        _portName = portName;
        _baudRate = baudRate;
        TryConnect();
    }

    public async Task WaitForDeviceAsync(CancellationToken cancellationToken = default)
    {
        // Opening the serial port can reset the ESP32.
        await Task.Delay(TimeSpan.FromSeconds(2), cancellationToken);
    }

    public bool Send(HardwareSnapshot snapshot)
    {
        if (!TryConnect())
        {
            return false;
        }

        if (snapshot.CpuTemperature is not float cpuTemp ||
            snapshot.CpuLoad is not float cpuLoad ||
            snapshot.GpuTemperature is not float gpuTemp ||
            snapshot.GpuLoad is not float gpuLoad)
        {
            return false;
        }

        string message = string.Join(",",
            $"CPU_TEMP:{Format(cpuTemp)}",
            $"CPU_LOAD:{Format(cpuLoad)}",
            $"GPU_TEMP:{Format(gpuTemp)}",
            $"GPU_LOAD:{Format(gpuLoad)}");

        try 
        {
            _serialPort!.WriteLine(message);
            return true;
        }
        catch (Exception ex) when (
            ex is IOException or
            InvalidOperationException or
            UnauthorizedAccessException)
        {
            Disconnect();
            return false;
        }
    }

    private bool TryConnect()
    {
        if (_serialPort?.IsOpen is true)
        {
            return true;
        }

        Disconnect();

        try
        {
            var port = new SerialPort(_portName, _baudRate)
            {
                NewLine = "\n",
                WriteTimeout = 1000
            };

            port.Open();
            _serialPort = port;

            // Opening the port may reset the ESP32.
            Thread.Sleep(2000);

            return true;
        }
        catch (Exception ex) when (
            ex is IOException or
            InvalidOperationException or
            UnauthorizedAccessException)
        {
            Disconnect();
            return false;
        }
    }

    private void Disconnect()
    {
        _serialPort?.Dispose();
        _serialPort = null;
    }

    private static string Format(float value) => value.ToString("F1", CultureInfo.InvariantCulture);

    public void Dispose()
    {
        Disconnect();
    }
}