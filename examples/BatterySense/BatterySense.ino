#include "VEDirect.h"

void mpptCallback(VEDirect_id_t id, int32_t value);

VEDirect mppt(Serial1, mpptCallback);

void setup() {
  Serial.begin(19200);
  mppt.begin();
  // mppt.ping();
}

void loop() {
  mppt.update();
  if(millis() % 1000 == 0){
    mppt.get(VEDirect_kPanelVoltage);
  }
}

void mpptCallback(VEDirect_id_t id, int32_t value){
  Serial.print(id);
  Serial.print(F(" : "));
  Serial.println(value);
  mppt.ping();
}
