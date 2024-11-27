# UartCommLib

## 项目简介
该项目是一个用于在 Arduino 上处理串口通信的 C++ 库，提供了基本的数据包处理和通信功能。
项目包含多个头文件，分别定义了数据包的基本结构、通信协议和辅助功能。

## 文件结构
- `BasicPacket.hpp`：定义了基本的数据包结构。
- `BasicPacketHelpers.hpp`：提供了数据包处理的辅助函数。
- `UartComm.hpp`：类模板，定义了串口通信的主要功能，适用于 HardwareSerial 或 SoftwareSerial。
- `UartCommBase.hpp`：定义了串口通信的基础类。
- `UartCommHelpers.hpp`：提供了串口通信的辅助函数。
- `UartCommTypes.h`：定义了串口通信中使用的类型。

