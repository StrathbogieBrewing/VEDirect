#include "AceBMS.h"
#include "AceBus.h"
#include "VEDirect.h"
#include "VEDirect_sig.h"

#define kRxInterruptPin (19)
AceBus aceBus(Serial1, kRxInterruptPin);

void mppt2Callback(uint16_t id, int32_t value);
void mppt3Callback(uint16_t id, int32_t value);
VEDirect mppt2(Serial2, mppt2Callback);
VEDirect mppt3(Serial3, mppt3Callback);

uint16_t panelVoltage2 = 0;
uint16_t chargeCurrent2 = 0;
uint16_t panelVoltage3 = 0;
uint16_t chargeCurrent3 = 0;

void hexDump(char *tag, uint8_t *buffer, int size) {
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

  // hexDump("TST", (uint8_t*)&testFrame, tinframe_kFrameSize);
  // Serial.println(tinframe_checkFrame(&testFrame));
}

void loop() {
  mppt2.update();
  mppt3.update();

  int status = aceBus.update();
  if (status == AceBus_kReadDataReady) {
    tinframe_t rxFrame;
    aceBus.read(&rxFrame);
    // hexDump("RX ", (unsigned char*)&rxFrame, tinframe_kFrameSize);
    msg_t *msg = (msg_t *)(rxFrame.data);
    int16_t value;
    if (sig_decode(msg, ACEBMS_VBAT, &value) != FMT_NULL) {
      uint32_t senseVoltage = (value + 5) / 10 - 5;
      mppt2.set(VEDirect_kBatterySense, senseVoltage);
      mppt3.set(VEDirect_kBatterySense, senseVoltage);
    }
    if (sig_decode(msg, ACEBMS_RQST, &value) != FMT_NULL) {
      uint8_t frameSequence = value;
      if(frameSequence == (SIG_MSG_ID(VEDIRECT_SIG_STATUS) & 0xFF)){
        tinframe_t txFrame;
        msg_t *txMsg = (msg_t *)txFrame.data;
        sig_encode(txMsg, VEDIRECT_SIG_VPV2, panelVoltage2);
        sig_encode(txMsg, VEDIRECT_SIG_ICH2, chargeCurrent2);
        sig_encode(txMsg, VEDIRECT_SIG_VPV3, panelVoltage3);
        sig_encode(txMsg, VEDIRECT_SIG_ICH3, chargeCurrent3);
        aceBus.write(&txFrame);
      }
    }
  }
}

void mppt2Callback(uint16_t id, int32_t value) {
  if (id == VEDirect_kPanelVoltage){
    panelVoltage2 = value;
    Serial.print(F("Vp2 : "));
    Serial.println(value * 0.01);
  }
  if (id == VEDirect_kChargeCurrent){
    chargeCurrent2 = value;
    Serial.print(F("Ic2 : "));
    Serial.println(value * 0.1);
  }
}

void mppt3Callback(uint16_t id, int32_t value) {
  if (id == VEDirect_kPanelVoltage){
    panelVoltage3 = value;
    Serial.print(F("Vp3 : "));
    Serial.println(value * 0.01);
  }
  if (id == VEDirect_kChargeCurrent){
    chargeCurrent3 = value;
    Serial.print(F("Ic3 : "));
    Serial.println(value * 0.1);
  }
}
