#pragma once

#include <Adafruit_SSD1306.h>
#include "HardwareSnapshot.h"

class HardwareMonitorDisplay
{
public:
    HardwareMonitorDisplay();

    bool begin();
    void render(const HardwareSnapshot& snapshot, bool connected);

private:
    static constexpr int ScreenWidth = 128;
    static constexpr int ScreenHeight = 64;

    static constexpr int SdaPin = 21;
    static constexpr int SclPin = 22;
    static constexpr uint8_t ScreenAddress = 0x3C;

    Adafruit_SSD1306 display_;

    void drawMetric(
        int y,
        const char* label,
        float temperature,
        float load);

    void drawUsageBar(int y, float load);
};