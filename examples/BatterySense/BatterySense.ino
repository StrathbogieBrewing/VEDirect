#include "VEDirect.h"

void mppt(uint16_t id, int32_t value);
VEDirect mppt(Serial1, mpptCallback);

uint16_t panelVoltage = 0;
uint16_t chargeCurrent = 0;

void setup() {
  Serial.begin(19200);
  mppt.begin();
}

void loop() {
  mppt.update();
  if(millis() % 1000 == 0) {
    mppt.ping();
    delay_ms(1);
  }
}

void mpptCallback(uint16_t id, int32_t value) {
  if (id == VEDirect_kPanelVoltage){
    panelVoltage = value;
    Serial.print(F("Vpv : "));
    Serial.println(value * 0.01);
  }
  if (id == VEDirect_kChargeCurrent){
    chargeCurrent = value;
    Serial.print(F("Ich : "));
    Serial.println(value * 0.1);
  }
}
