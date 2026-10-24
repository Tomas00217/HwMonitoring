#include <Arduino.h>

#include "HardwareSnapshot.h"
#include "HardwareMonitorDisplay.h"
#include "SerialDataReceiver.h"

namespace
{
    constexpr unsigned long ConnectionTimeoutMs = 3000;
    constexpr unsigned long DisplayRefreshMs = 250;

    HardwareSnapshot snapshot;

    HardwareMonitorDisplay display;
    SerialDataReceiver receiver;

    bool hasReceivedData = false;
    unsigned long lastReceivedAt = 0;
    unsigned long lastDisplayUpdateAt = 0;
}

void setup()
{
    Serial.begin(115200);

    if (!display.begin())
    {
        // Display initialization failed.
        while (true)
        {
            delay(1000);
        }
    }

    display.render(snapshot, false);
}

void loop()
{
    if (receiver.poll(snapshot))
    {
        hasReceivedData = true;
        lastReceivedAt = millis();
    }

    const unsigned long now = millis();

    const bool connected = hasReceivedData && now - lastReceivedAt < ConnectionTimeoutMs;

    if (now - lastDisplayUpdateAt >= DisplayRefreshMs)
    {
        display.render(snapshot, connected);
        lastDisplayUpdateAt = now;
    }
}