#pragma once

#include <Arduino.h>
#include "HardwareSnapshot.h"

class SerialDataReceiver
{
public:
    bool poll(HardwareSnapshot& snapshot);

private:
    static constexpr size_t BufferSize = 160;

    char buffer_[BufferSize]{};
    size_t length_ = 0;

    bool processLine(HardwareSnapshot& snapshot);
};