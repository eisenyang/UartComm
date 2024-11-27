#pragma once

#include <Arduino.h>

namespace UartCommLib {
    struct Helpers {
        static uint8_t calculateChecksum(const uint8_t *data, uint8_t size) {
            uint8_t sum = 0;
            for (uint8_t i = 0; i < size; ++i) {
                sum += data[i];
            }
            return ~sum;
        }

        static void printBuffer(const uint8_t *data, uint8_t size, const char *prefix = "") {
            Serial.print(prefix);
            if (prefix) Serial.print(" ");
            for (uint8_t i = 0; i < size; ++i) {
                Serial.printf("%02X ", data[i]);
            }
            Serial.println();
        }
    };
}// namespace UartCommLib
