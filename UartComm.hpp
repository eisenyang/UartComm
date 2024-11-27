#pragma once

#include "UartCommBase.hpp"

namespace UartCommLib {

    template<typename SerialType, typename PacketType, uint8_t MaxPayloadSize>
    class UartCommClass : public UartCommBase {
    public:
        explicit UartCommClass(SerialType &serial) : _serial(serial) {
            _packet = new PacketType(this, MaxPayloadSize);
        }

        ~UartCommClass() {
            delete _packet;
        }

        void loop() override {
            if (_packet->receive()) _onReceive(*_packet);
            delay(1);
            _packet->transmit();
        }

    protected:
        void setTimeout(unsigned long timeout) override {
            _serial.setTimeout(timeout);
        }

        size_t write(const uint8_t *data, uint8_t size) override {
            return _serial.write(data, size);
        }

        size_t readBytes(uint8_t *data, size_t size) override {
            return _serial.readBytes(data, size);
        }

        int available() override {
            return _serial.available();
        }

        uint8_t read() override {
            return _serial.read();
        }

        uint8_t peek() override {
            return _serial.peek();
        }

        void flush() override {
            _serial.flush();
        }

        void writePacket(const Method method, const uint16_t topic, const uint8_t *data, uint8_t size) override {
             _packet->write(method, topic, data, size);
        }

    private:
        SerialType &_serial;
        PacketType *_packet;
    };
}// namespace UartCommLib

template<typename SerialType, typename PacketType, uint8_t MaxPayloadSize>
using UartComm = UartCommLib::UartCommClass<SerialType, PacketType, MaxPayloadSize>;
using UartCommMethod = UartCommLib::Method;
using UartCommPacket = UartCommLib::PacketBase;