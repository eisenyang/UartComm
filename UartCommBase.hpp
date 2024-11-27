#pragma once

#include "UartCommTypes.h"

namespace UartCommLib {
    class UartCommBase {
    public:
        UartCommBase &onReceive(std::function<void(const PacketBase &)> callback) {
            _onReceive = callback;
            return *this;
        }

        void set(const uint16_t topic, const uint8_t *data = nullptr, uint8_t size = 0) {
            writePacket(METHOD_SET, topic, data, size);
        }

        void setNoAck(const uint16_t topic, const uint8_t *data = nullptr, uint8_t size = 0) {
            writePacket(METHOD_SET_NO_ACK, topic, data, size);
        }

        void unset(const uint16_t topic) {
            writePacket(METHOD_UNSET, topic, nullptr, 0);
        }

        void unsetNoAck(const uint16_t topic) {
            writePacket(METHOD_UNSET_NO_ACK, topic, nullptr, 0);
        }

        void get(const uint16_t topic, const uint8_t *data = nullptr, uint8_t size = 0) {
            writePacket(METHOD_GET, topic, data, size);
        }

        void getNoAck(const uint16_t topic, const uint8_t *data = nullptr, uint8_t size = 0) {
            writePacket(METHOD_GET_NO_ACK, topic, data, size);
        }

        void response(const uint16_t topic, const uint8_t *data = nullptr, uint8_t size = 0) {
            writePacket(METHOD_RESPONSE, topic, data, size);
        }

        void responseNoAck(const uint16_t topic, const uint8_t *data = nullptr, uint8_t size = 0) {
            writePacket(UartCommLib::Method::METHOD_RESPONSE_NO_ACK, topic, data, size);
        }

    public:// pure virtual functions
        virtual int available() = 0;
        virtual void setTimeout(unsigned long timeout) = 0;
        virtual uint8_t read() = 0;
        virtual size_t readBytes(uint8_t *data, size_t size) = 0;
        virtual size_t write(const uint8_t *data, uint8_t size) = 0;
        virtual void flush() = 0;
        virtual uint8_t peek() = 0;
        virtual void writePacket(const Method method, const uint16_t topic, const uint8_t *data, uint8_t size) = 0;
        virtual void loop() = 0;

    protected:
        OnReceiveFn _onReceive;
    };

}// namespace UartCommLib