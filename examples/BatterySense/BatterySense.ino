#include "VEDirect.h"

void mpptCallback(uint16_t id, int32_t value);

VEDirect mppt(Serial1, mpptCallback);

void setup() {
  Serial.begin(19200);
  Serial3.begin(1200);
  mppt.begin();
}

void loop() {
  mppt.update();
  if (millis() % 1000 == 0) {
    mppt.get(VEDirect_kPanelVoltage);
  }

  if (Serial3.available()) {      // If anything comes in Serial3 (pins 0 & 1)
    Serial.write(Serial3.read()); // read it and send it out Serial (USB)
  }
}

void mpptCallback(uint16_t id, int32_t value) {
  Serial.print(id, HEX);
  Serial.print(F(" : "));
  Serial.println(value);
  mppt.ping();
}
