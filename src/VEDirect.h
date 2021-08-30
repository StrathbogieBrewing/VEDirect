#ifndef VEDIRECT_H_
#define VEDIRECT_H_

#include "Arduino.h"
#define VEDirect_kBufferSize (32)

typedef enum {
  VEDirect_kBatterySense = 0x2002,
} VEDirect_id_t;

typedef void (*receiveCallback)(VEDirect_id_t id, int32_t value);

class VEDirect {
public:
  VEDirect(Serial &serial, receiveCallback receive);
  void begin();
  void update();
  bool send(VEDirect_id_t id, int32_t value);
private:
  uint8_t rxBuffer[VEDirect_kBufferSize];
  uint8_t rxIndex;
};

#endif //  VEDIRECT_H_
