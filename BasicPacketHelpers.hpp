#pragma once

#include <Arduino.h>

namespace UartCommLib {
    struct BasicPacketHelpers {
        static bool isNeedAck(uint8_t *data, uint8_t size) {
            return (data[2] >> 7) == 0;
        }

        static bool isNeedAck(uint8_t method) {
            return (method >> 3) == 0;
        }
    };
}// namespace UartCommLib
