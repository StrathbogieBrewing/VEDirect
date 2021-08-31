#include "VEDirect.h"

#define VEDirect_kBaud (19200)

#define VEDirect_kPingCommand (0x01)
#define VEDirect_kSetCommand (0x08)
#define VEDirect_kGetCommand (0x07)
#define VEDirect_kAsyncCommand (0x0A)

VEDirect::VEDirect(HardwareSerial &serial, receiveCallback receive)
    : serialPort{serial}, rxCallback{receive} {};

void VEDirect::begin() {
  serialPort.begin(VEDirect_kBaud);
  rxBuffer.size = 0;
}

void VEDirect::update() {
  while (serialPort.available() > 0) {
    char rxData = serialPort.read();
    Serial.write(rxData);
    uint8_t rxCount = ved_deframe(&rxBuffer, rxData);
    if (rxCount > 0) {
      // Serial.println(rxCount);
      rxBuffer.size = 0;
      uint8_t command = ved_getCommand(&rxBuffer);
      if ((command = VEDirect_kGetCommand) ||
          (command = VEDirect_kSetCommand) ||
          (command = VEDirect_kAsyncCommand)) {
        uint16_t id = ved_getId(&rxBuffer);
        switch (id) {
        case VEDirect_kPanelPower:
          rxCallback(id, ved_getU32(&rxBuffer));
          break;
        case VEDirect_kPanelVoltage:
          rxCallback(id, ved_getU16(&rxBuffer));
          break;

        default:
          break;
        }
      }
    }
  }
}

size_t VEDirect::set(VEDirect_id_t id, int32_t value) {
  ved_t txBuffer;
  ved_setCommand(&txBuffer, VEDirect_kSetCommand);
  ved_setId(&txBuffer, id);
  switch (id) {
  case VEDirect_kBatterySense:
    ved_setU16(&txBuffer, value);
    ved_enframe(&txBuffer);
    return serialPort.write(txBuffer.data, txBuffer.size);
  default:
    break;
  }
  return 0;
}

size_t VEDirect::get(VEDirect_id_t id){
  ved_t txBuffer;
  ved_setCommand(&txBuffer, VEDirect_kSetCommand);
  ved_setId(&txBuffer, id);
  ved_enframe(&txBuffer);
  return serialPort.write(txBuffer.data, txBuffer.size);
}

size_t VEDirect::ping() {
  ved_t txBuffer;
  ved_setCommand(&txBuffer, VEDirect_kPingCommand);
  ved_enframe(&txBuffer);
  return serialPort.write(txBuffer.data, txBuffer.size);
}
