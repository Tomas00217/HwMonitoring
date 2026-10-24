#include "SerialDataReceiver.h"

#include <cmath>
#include <cstdlib>
#include <cstring>

bool SerialDataReceiver::poll(HardwareSnapshot& snapshot)
{
    while (Serial.available() > 0)
    {
        const char character = static_cast<char>(Serial.read());

        if (character == '\n')
        {
            buffer_[length_] = '\0';
            length_ = 0;

            if (processLine(snapshot))
            {
                return true;
            }
        }
        else if (character != '\r')
        {
            if (length_ < BufferSize - 1)
            {
                buffer_[length_++] = character;
            }
            else
            {
                // Discard an oversized line.
                length_ = 0;
            }
        }
    }

    return false;
}

bool SerialDataReceiver::processLine(HardwareSnapshot& snapshot)
{
    HardwareSnapshot candidate;
    unsigned int fieldsFound = 0;

    char* context = nullptr;
    char* token = strtok_r(buffer_, ",", &context);

    while (token != nullptr)
    {
        char* separator = strchr(token, ':');

        if (separator == nullptr)
        {
            return false;
        }

        *separator = '\0';

        const char* key = token;
        char* valueEnd = nullptr;
        const float value = strtof(separator + 1, &valueEnd);

        if (valueEnd == separator + 1 ||
            *valueEnd != '\0' ||
            !std::isfinite(value))
        {
            return false;
        }

        if (strcmp(key, "CPU_TEMP") == 0)
        {
            candidate.cpuTemperature = value;
            fieldsFound |= 1U << 0;
        }
        else if (strcmp(key, "CPU_LOAD") == 0)
        {
            candidate.cpuLoad = value;
            fieldsFound |= 1U << 1;
        }
        else if (strcmp(key, "GPU_TEMP") == 0)
        {
            candidate.gpuTemperature = value;
            fieldsFound |= 1U << 2;
        }
        else if (strcmp(key, "GPU_LOAD") == 0)
        {
            candidate.gpuLoad = value;
            fieldsFound |= 1U << 3;
        }

        token = strtok_r(nullptr, ",", &context);
    }

    // Only accept a complete snapshot.
    if (fieldsFound != 0b1111U)
    {
        return false;
    }

    snapshot = candidate;
    return true;
}