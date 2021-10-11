#include <string.h>

#include "ved.h"

static uint8_t bin2hex(uint8_t bin) {
  bin &= 0x0F;
  if (bin < 10)
    return bin + '0';
  return bin + 'A' - 10;
}

uint8_t ved_enframe(ved_t *vedata) {
  uint8_t *input = vedata->data;
  uint8_t buffer[ved_kBufferSize];
  uint8_t *output = buffer;
  uint8_t csum = 0x00;
  uint8_t src = *input++;
  *output++ = ':';
  *output++ = bin2hex(src);
  csum -= src;
  while (input < vedata->data + vedata->size) {
    src = *input++;
    *output++ = bin2hex(src >> 4);
    *output++ = bin2hex(src);
    csum -= src;
  }
  csum += 0x55;
  *output++ = bin2hex(csum >> 4);
  *output++ = bin2hex(csum);
  *output++ = '\n';
  *output = '\0';

  vedata->size = output - buffer;
  memcpy(vedata->data, buffer, vedata->size);
  return vedata->size;
}

static uint8_t hex2bin(uint8_t hex) {
  if (hex < '0')
    return 0;
  uint8_t val = hex - '0';
  if (val > 9) {
    val -= 7;
    if ((val > 15) || (val < 10))
      return 0;
  }
  return val;
}

uint8_t ved_deframe(ved_t *vedata, char inByte) {
  if (inByte == ':') {
    vedata->size = 0; // reset data buffer
  }
  if (vedata->size < ved_kBufferSize - 2) {
    vedata->data[vedata->size++] = inByte;
    if (inByte == '\n') {
      vedata->data[vedata->size] = '\0';
      uint8_t *input = vedata->data;
      uint8_t *output = vedata->data;
      uint8_t csum = 0x00;
      while ((*input != '\n') && (*input != '\0')) {
        uint8_t byte = hex2bin(*input++) << 4;
        byte += hex2bin(*input++);
        csum += byte;
        *output++ = byte;
      }
      if (csum == 0x55) {
        vedata->size = output - vedata->data - 1;
        return vedata->size; // size not including terminating null
      }
    }
  }
  return 0;
}

uint8_t ved_getCommand(ved_t *vedata) { return vedata->data[0]; }

uint16_t ved_getId(ved_t *vedata) {
  return (((uint16_t)vedata->data[2]) << 8) + (uint16_t)vedata->data[1];
}

uint8_t ved_getFlags(ved_t *vedata) { return vedata->data[3]; }

int32_t ved_getU8(ved_t *vedata) {
  return (uint32_t)vedata->data[4];
}

int32_t ved_getU16(ved_t *vedata) {
  return (((uint32_t)vedata->data[5]) << 8) + (uint32_t)vedata->data[4];
}

int32_t ved_getU32(ved_t *vedata) {
  return (((uint32_t)vedata->data[7]) << 24) +
         (((uint32_t)vedata->data[6]) << 16) +
         (((uint32_t)vedata->data[5]) << 8) + (uint32_t)vedata->data[4];
}

void ved_setCommand(ved_t *vedata, uint8_t value){
  vedata->data[0] = value;
  vedata->size = 1;
}

void ved_setId(ved_t *vedata, uint16_t value){
  vedata->data[1] = value;
  vedata->data[2] = (value >> 8);
  vedata->data[3] = 0;
  vedata->size = 4;
}

void ved_setU8(ved_t *vedata, uint8_t value){
  vedata->data[4] = value;
  vedata->size = 5;
}

void ved_setU16(ved_t *vedata, uint16_t value){
  vedata->data[5] = (value >> 8);
  vedata->data[4] = value;
  vedata->size = 6;
}
