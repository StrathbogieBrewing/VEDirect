#include <string.h>
// #include <stdio.h>

#include "ved.h"

// static void hexDump(char* tag, unsigned char* buffer, int size){
//   int i = 0;
//   fprintf(stdout, "%s : ", tag);
//   while(i < size){
//     fprintf(stdout, "%2.2X ", buffer[i++]);
//   }
//   fprintf(stdout, "\n");
// }

static unsigned char bin2hex(unsigned char bin) {
  bin &= 0x0F;
  if (bin < 10)
    return bin + '0';
  return bin + 'A' - 10;
}

int ved_enframe(ved_t *vedata) {
  unsigned char *input = vedata->data;
  unsigned char buffer[ved_kBufferSize];
  unsigned char *output = buffer;
  unsigned char csum = 0x00;
  unsigned char src = *input++;
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

static unsigned char hex2bin(unsigned char hex) {
  if (hex < '0')
    return 0;
  unsigned char val = hex - '0';
  if (val > 9) {
    val -= 7;
    if ((val > 15) || (val < 10))
      return 0;
  }
  return val;
}

int ved_deframe(ved_t *vedata, char inByte) {
  if (inByte == ':') {
    vedata->size = 0; // reset data buffer
  }
  if (vedata->size < ved_kBufferSize - 2) {
    vedata->data[vedata->size++] = inByte;
    if (inByte == '\n') {

      // vedata->data[vedata->size] = '\0';
      // printf("RX  : %s", vedata->data);
      // hexDump("RXD", vedata->data, vedata->size);

      unsigned char *input = vedata->data;
      unsigned char *output = vedata->data;
      unsigned char csum = 0x00;
      while ((*input != '\n') && (*input != '\0')) {
        unsigned char byte = hex2bin(*input++) << 4;
        byte += hex2bin(*input++);
        csum += byte;
        *output++ = byte;
      }
      if (csum == 0x55) {
        vedata->size = output - vedata->data - 1;
        return vedata->size;
      }
    }
  }
  return 0;
}
