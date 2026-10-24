#include "HardwareMonitorDisplay.h"

#include <Wire.h>
#include <algorithm>
#include <cstdio>

HardwareMonitorDisplay::HardwareMonitorDisplay(): display_(ScreenWidth, ScreenHeight, &Wire, -1)
{
}

bool HardwareMonitorDisplay::begin()
{
    Wire.begin(SdaPin, SclPin);

    if (!display_.begin(SSD1306_SWITCHCAPVCC, ScreenAddress))
    {
        return false;
    }

    display_.clearDisplay();
    display_.setTextColor(SSD1306_WHITE);
    display_.display();

    return true;
}

void HardwareMonitorDisplay::render(const HardwareSnapshot& snapshot, bool connected)
{
    display_.clearDisplay();
    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);

    display_.setCursor(0, 0);
    display_.print("PC MONITOR");

    display_.setCursor(83, 0);
    display_.print(connected ? "ONLINE" : "OFFLINE");

    display_.drawLine(0, 10, ScreenWidth - 1, 10, SSD1306_WHITE);

    if (connected)
    {
        drawMetric(14, "CPU", snapshot.cpuTemperature, snapshot.cpuLoad);
        drawUsageBar(25, snapshot.cpuLoad);

        drawMetric(34, "GPU", snapshot.gpuTemperature, snapshot.gpuLoad);
        drawUsageBar(45, snapshot.gpuLoad);
    }
    else
    {
        display_.setCursor(0, 20);
        display_.print("Waiting for PC data..");

        display_.drawLine(0, 55, ScreenWidth - 1, 55, SSD1306_WHITE);
        display_.setCursor(0, 57);
        display_.print("CHECK PC / COM3");
    }

    display_.display();
}

void HardwareMonitorDisplay::drawMetric(int y, const char* label, float temperature, float load)
{
    char text[32];

    snprintf(
        text,
        sizeof(text),
        "%s %4.1fC %5.1f%%",
        label,
        temperature,
        load);

    display_.setCursor(0, y);
    display_.print(text);
}

void HardwareMonitorDisplay::drawUsageBar(int y, float load)
{
    constexpr int BarWidth = ScreenWidth;
    constexpr int BarHeight = 4;

    const float clampedLoad = std::clamp(load, 0.0F, 100.0F);
    const int fillWidth = static_cast<int>(BarWidth * clampedLoad / 100.0F);

    display_.drawRect(0, y, BarWidth, BarHeight, SSD1306_WHITE);

    if (fillWidth > 2)
    {
        display_.fillRect(
            1, y + 1,
            fillWidth - 2, BarHeight - 2,
            SSD1306_WHITE);
    }
}