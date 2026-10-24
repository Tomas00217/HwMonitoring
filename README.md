# HWMonitor

A DIY PC hardware monitor that displays CPU and GPU temperatures and utilization on an ESP32-powered OLED screen.

The project consists of two components:

- **HwMonitor.Collector** — a .NET console application that reads hardware sensor data on Windows using LibreHardwareMonitor and sends it over USB serial.
- **HwMonitor.Firmware** — ESP32 firmware that receives the serial data and displays it on a 128×64 SSD1306 OLED.

## Features

- CPU temperature and utilization monitoring
- GPU temperature and utilization monitoring
- Live updates over USB serial
- OLED status indicator when the PC data stream stops
- Separate components for sensor collection, serial communication, data parsing, and display rendering

## Architecture

```text
Windows PC
┌──────────────────────────┐
│ .NET Hardware Collector  │
│ LibreHardwareMonitor     │
└────────────┬─────────────┘
             │ USB serial
             │ 115200 baud
             ▼
┌──────────────────────────┐
│ ESP32                    │
│ SerialDataReceiver       │
│ HardwareMonitorDisplay   │
└────────────┬─────────────┘
             │ I²C
             ▼
       SSD1306 OLED
```

## Hardware

- ESP32 development board
- 0.96-inch SSD1306 I²C OLED display
- USB cable connecting the ESP32 to the Windows PC

### OLED wiring

| OLED | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SCL | GPIO 22 |
| SDA | GPIO 21 |

The firmware currently uses I²C address `0x3C`. Check your display module's specifications if it doesn't initialize.

## Requirements

### Windows collector

- Windows
- .NET 10
- LibreHardwareMonitor-compatible hardware sensors
- PawnIO may be required for CPU sensor access, depending on your system
- ESP32 connected over USB

### ESP32 firmware

- PlatformIO, for example through the VS Code extension
- A compatible ESP32 development board
- Adafruit SSD1306 and Adafruit GFX libraries, declared in `platformio.ini`

## Build and Run

### 1. Build the ESP32 firmware

1. Open the firmware project in PlatformIO.
2. Check `platformio.ini` and select the correct board environment.
3. Build and upload the firmware.
4. Close the PlatformIO Serial Monitor before running the collector.

### 2. Build and run the Windows collector

Open a terminal in the collector directory:

```powershell
dotnet restore
dotnet build
dotnet run
```

The collector currently uses `COM3` at `115200` baud. Update the configuration if your ESP32 uses a different serial port.

### 3. Publish a standalone executable

Optionally, publish the collector as a standalone Windows executable:

```powershell
dotnet publish -c Release -r win-x64 --self-contained true /p:PublishSingleFile=true
```

Windows Task Scheduler can be configured to launch the executable at logon. Automatic startup is not configured by the application itself.

## Serial Protocol

The collector sends a newline-terminated message per update at **115200 baud**.

Example:

```text
CPU_TEMP:42.5,CPU_LOAD:3.7,GPU_TEMP:55.0,GPU_LOAD:10.0
```

The firmware expects four fields:

| Field | Description |
|---|---|
| `CPU_TEMP` | CPU temperature in °C |
| `CPU_LOAD` | CPU utilization percentage |
| `GPU_TEMP` | GPU temperature in °C |
| `GPU_LOAD` | GPU utilization percentage |

Values use a dot as the decimal separator. The firmware only accepts a complete, valid snapshot.

## Project Structure

```text
HWMonitor/
├── .gitignore
├── README.md
├── HwMonitor.Collector/
│   ├── Program.cs
│   ├── Models/
│   ├── Hardware/
│   └── Communication/
└── HwMonitor.Firmware/
    ├── platformio.ini
    ├── include/
    └── src/
```

The exact filenames and folders may change as the project evolves.

## Troubleshooting

- **OLED remains offline:** Check the USB connection, COM port, baud rate, and whether the collector is running.
- **COM port cannot be opened:** Close PlatformIO's Serial Monitor and any other application using the ESP32.
- **OLED does not initialize:** Check power, ground, SDA/SCL wiring, and I²C address.
- **CPU readings are missing or zero:** 
    - Run the HwMonitoring.Collector with elevated privileges
    - Verify LibreHardwareMonitor's sensor access and whether [PawnIO](https://pawnio.eu/) is required on your system.
- **GPU readings are missing:** Verify that LibreHardwareMonitor detects your GPU and that the collector's GPU-name filter matches your device.

## Roadmap

- [ ] Add GPU hotspot, memory temperature, fan speed, and RAM usage
- [ ] Improve display layout and status handling
- [ ] Configure automatic startup on Windows