#pragma once
#include <Arduino.h>

namespace UartCommLib {

    typedef enum : uint8_t {
        METHOD_NONE = 0,
        METHOD_SET = 1,
        METHOD_UNSET = 2,
        METHOD_GET = 3,
        METHOD_RESPONSE = 4,
        METHOD_SET_NO_ACK = 1 | 0x8,
        METHOD_UNSET_NO_ACK = 2 | 0x8,
        METHOD_GET_NO_ACK = 3 | 0x8,
        METHOD_RESPONSE_NO_ACK = 4 | 0x8,
    } Method;

    struct PacketBase {
        PacketBase() = default;
        static constexpr uint8_t PACKET_HEADER = 0xFF;
        static constexpr uint8_t PACKET_ACK = 0xAA;

        Method method = METHOD_NONE;
        uint16_t topic = 0;
        uint8_t *payload = nullptr;
        uint8_t payloadSize = 0;

        virtual void write(const Method method, const uint16_t topic, const uint8_t *payload, uint8_t payloadSize) = 0;
        virtual bool receive() = 0;
        virtual bool transmit() = 0;
    };

    using OnReceiveFn = std::function<void(const PacketBase &)>;
}// namespace UartCommLib