#ifndef VEDIRECT_H_
#define VEDIRECT_H_

#include "Arduino.h"
#include "ved.h"

// subset of register definitions from BlueSolar-HEX-protocol-MPPT.pdf
#define VEDirect_kBatterySense   (0x2002)
#define VEDirect_kPanelVoltage   (0xEDBB)
#define VEDirect_kChargeVoltage  (0xEDD5)
#define VEDirect_kPanelCurrent   (0xEDBD)
#define VEDirect_kChargeCurrent  (0xEDD7)
#define VEDirect_kPanelPower     (0xEDBC)

typedef void (*receiveCallback)(uint16_t id, int32_t value);

class VEDirect {
public:
  VEDirect(HardwareSerial &serial, receiveCallback receive);
  void begin();
  void update();
  size_t set(uint16_t id, int32_t value);
  size_t get(uint16_t id);
  size_t ping();
private:
  HardwareSerial &serialPort;
  receiveCallback rxCallback;
  ved_t rxBuffer;
};

#endif //  VEDIRECT_H_
