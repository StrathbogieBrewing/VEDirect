#include "VEDirect.h"
#include "AceBus.h"
#include "AceBMS.h"

#define kRxInterruptPin (19)

void mpptCallback(uint16_t id, int32_t value);

AceBus aceBus(Serial1, kRxInterruptPin);
VEDirect mppt2(Serial2, mpptCallback);
VEDirect mppt3(Serial3, mpptCallback);

void hexDump(char *tag, unsigned char *buffer, int size) {
  int i = 0;
  Serial.print(tag);
  Serial.print(" : ");
  while (i < size) {
    Serial.print(buffer[i] >> 4, HEX);
    Serial.print(buffer[i++] & 0x0F, HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void setup() {
  Serial.begin(19200);

  mppt2.begin();
  mppt3.begin();

  aceBus.begin();
}

void loop() {
  mppt2.update();
  mppt3.update();

  // if (millis() % 1000 == 0) {
  //   mppt1.get(VEDirect_kPanelVoltage);
  // }

  int status = aceBus.update();
  if(status == AceBus_kReadDataReady){
    tinframe_t rxFrame;
    aceBus.read(&rxFrame);
    // hexDump("RX ", (unsigned char*)&rxFrame, tinframe_kFrameSize);

    msg_t *msg = (msg_t *)(rxFrame.data);
    int16_t value;
    // fmt_t format = sig_decode(msg, ACEBMS_VBAT, &value);
    if (sig_decode(msg, ACEBMS_VBAT, &value) != FMT_NULL) {
      // Serial.println(value );
      // Serial.println((value + 5) / 10);
      uint32_t senseVoltage = (value + 5) / 10 - 1;
      // uint32_t senseVoltage = value / 10 + 1;
      mppt2.set(VEDirect_kBatterySense, senseVoltage);
      mppt3.set(VEDirect_kBatterySense, senseVoltage);
    }
  }
}

void mpptCallback(uint16_t id, int32_t value) {
  Serial.print(F("VED : 0x"));
  Serial.print(id, HEX);
  Serial.print(F(" = "));
  Serial.println(value);
  // mppt.ping();
}
