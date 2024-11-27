#pragma once

#include "BasicPacketHelpers.hpp"
#include "UartCommBase.hpp"
#include "UartCommHelpers.hpp"
#include <vector>

namespace UartCommLib {

    struct BasicPacket : PacketBase {
        static constexpr uint16_t TRANSMIT_INTERVAL = 20;
        static constexpr uint32_t ACK_TIMEOUT = 50;
        static constexpr uint8_t TRANSMIT_MAX_RETRY_COUNT = 3;

        typedef enum {
            TRANSMIT_STATE_IDLE = 0,
            TRANSMIT_STATE_START = 1,
            TRANSMIT_STATE_WAIT_ACK = 2,
            TRANSMIT_STATE_TIMEOUT = 3,
            TRANSMIT_STATE_RETRY = 4,
            TRANSMIT_STATE_FAILED = 5,
        } TransmitState;

        BasicPacket(UartCommBase *uartComm, uint8_t maxPayloadSize)
            : _uartComm(uartComm), _packetSize(maxPayloadSize + 5) {
            _readBuffer.resize(_packetSize);
            _writeBuffer.resize(_packetSize);
            _uartComm->setTimeout(100);
        }

        void write(const Method method, const uint16_t topic, const uint8_t *payload, uint8_t payloadSize) override {
            _writeToBuffer(method, topic, payload, payloadSize);
        }

        bool receive() override {
            return _receive();
        }

        bool transmit() override {
            if (millis() - _lastTransmitTime < TRANSMIT_INTERVAL) return false;
            _lastTransmitTime = millis();
            if (_isOkToAck) return _ack();
            switch (_transmitState) {
                case TRANSMIT_STATE_START:
                    return _transmit();
                case TRANSMIT_STATE_WAIT_ACK:
                    return _checkAck();
                case TRANSMIT_STATE_TIMEOUT:
                    if (++_transmitRetryCount >= TRANSMIT_MAX_RETRY_COUNT) {
                        _transmitState = TRANSMIT_STATE_FAILED;
                        Serial.printf("PacketBasic::transmit failed after %d retries\n", _transmitRetryCount);
                        _transmitRetryCount = 0;
                        return false;
                    }
                    _transmitState = TRANSMIT_STATE_RETRY;
                    Serial.printf("PacketBasic::transmit retry %d\n", _transmitRetryCount);
                    return _transmit();
                default:
                    return false;
            }
        }

    private:
        void _writeToBuffer(const Method method, const uint16_t topic, const uint8_t *payload, uint8_t payloadSize) {
            _writeBuffer[0] = PACKET_HEADER;
            _writeBuffer[1] = topic & 0xFF;
            _writeBuffer[2] = (method & 0xF) << 4 | (topic >> 8);
            _writeBuffer[3] = payloadSize;
            if (payload && payloadSize > 0) {
                memcpy(_writeBuffer.data() + 4, payload, payloadSize);
            }
            _writeBuffer[_packetSize - 1] = Helpers::calculateChecksum(_writeBuffer.data(), _packetSize - 1);
            _transmitState = TRANSMIT_STATE_START;
        }

        bool _receive() {
            if (_uartComm->available() < _packetSize || _uartComm->read() != PACKET_HEADER) return false;
            _readBuffer[0] = PACKET_HEADER;
            _uartComm->readBytes(_readBuffer.data() + 1, _packetSize - 1);
            if (!_validatePacket()) return false;
            _parsePacket();
            _isOkToAck = BasicPacketHelpers::isNeedAck(method);
            Helpers::printBuffer(_readBuffer.data(), _packetSize, "PacketBasic::receive success");
            return true;
        }

        bool _validatePacket() {
            return Helpers::calculateChecksum(_readBuffer.data(), _packetSize - 1) == _readBuffer[_packetSize - 1];
        }

        void _parsePacket() {
            uint16_t rawTopic = (_readBuffer[2] << 8) | _readBuffer[1];
            topic = rawTopic & 0x0FFF;
            method = static_cast<Method>((rawTopic >> 12) & 0x0F);
            payloadSize = _readBuffer[3];
            payload = _readBuffer.data() + 4;
        }

        bool _transmit() {
            auto ret = _uartComm->write(_writeBuffer.data(), _packetSize) == _packetSize;
            _uartComm->flush();
            Helpers::printBuffer(_writeBuffer.data(), _packetSize, "PacketBasic::transmit");
            if (BasicPacketHelpers::isNeedAck(_writeBuffer.data(), _packetSize)) {
                _transmitState = TRANSMIT_STATE_WAIT_ACK;
                _checkAckStartTime = millis();
                Serial.println("PacketBasic::transmit need ack");
            } else {
                _transmitState = TRANSMIT_STATE_IDLE;
                Serial.println("PacketBasic::transmit no need ack");
            }
            return ret;
        }

        bool _ack() {
            auto data = PACKET_ACK;
            auto ret = _uartComm->write(&data, 1) == 1;
            _uartComm->flush();
            _isOkToAck = false;
            Serial.printf("PacketBasic::ack send: %02X\n", data);
            return ret;
        }

        bool _clearAckFlag() {
            Serial.println("PacketBasic::clearAckFlag");
            return false;
        }

        bool _checkAck() {
            if (_uartComm->available() < 1) return false;
            if (_transmitState == TRANSMIT_STATE_WAIT_ACK && _uartComm->peek() == PACKET_ACK) {
                _uartComm->read();
                _transmitState = TRANSMIT_STATE_IDLE;
                _transmitRetryCount = 0;
                Serial.println("PacketBasic::checkAck ok");
                return true;
            }
            if (_transmitState == TRANSMIT_STATE_WAIT_ACK && millis() - _checkAckStartTime > ACK_TIMEOUT) {
                _transmitState = TRANSMIT_STATE_TIMEOUT;
                Serial.println("PacketBasic::checkAck timeout");
                return false;
            }
            return false;
        }

    private:
        UartCommBase *_uartComm;
        std::vector<uint8_t> _readBuffer, _writeBuffer;
        uint8_t _packetSize;
        bool _isOkToAck = false;
        uint32_t _checkAckStartTime = 0;
        TransmitState _transmitState = TRANSMIT_STATE_IDLE;
        uint32_t _lastTransmitTime = 0;
        uint8_t _transmitRetryCount = 0;
    };
}// namespace UartCommLib

using BasicPacket = UartCommLib::BasicPacket;