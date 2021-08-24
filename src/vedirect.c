#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#include "tty.h"
#include "vedirect.h"

#define vedirect_kBufferSize (64)

static int fd = -1;
static struct termios original_termios;

unsigned char hex2bin(unsigned char ascii) {
  if (ascii < '0')
    return 0;
  unsigned char val = ascii - '0';
  if (val > 9) {
    val -= 7;
    if ((val > 15) || (val < 10))
      return 0;
  }
  return val;
}

unsigned char bin2hex(unsigned char bin) {
  bin &= 0x0F;
  if(bin < 10) return bin + '0';
  return bin + 'A' - 10;
}

static int enframe(unsigned char *frame, unsigned char *data, int bytes) {
  unsigned char *in = data;
  unsigned char *out = frame;
  unsigned char csum = 0x00;
  unsigned char src = *in++;
  *out++ = ':';
  *out++ = bin2hex(src);
  csum -= src;
  while(in < data + bytes){
    src = *in++;
    *out++ = bin2hex(src >> 4);
    *out++ = bin2hex(src);
    csum -= src;
  }
  csum += 0x55;
  *out++ = bin2hex(csum >> 4);
  *out++ = bin2hex(csum);
  *out++ = '\n';
  *out = '\0';  // no harm in terminating as a string
  return out - frame;
}

static int deframe(unsigned char *data, unsigned char *frame, int maxBytes) {
  unsigned char *in = frame;
  unsigned char *out = data;
  unsigned char csum = 0x00;

  while ((*in != '\n') && (*in != '\0')) {
    unsigned char byte = hex2bin(*in++) << 4;
    byte += hex2bin(*in++);
    csum += byte;
    *out++ = byte;
    if(out >= data + maxBytes) return 0;
  }
  if (csum == 0x55) {
    return out - data - 1; // dont include the csum as data
  }
  return 0;
}

bool vedirect_open(char *port) {
  return tty_open(port);  // open the serial port
}

int vedirect_read(unsigned char *data, int maxBytes) {
  static unsigned char buffer[vedirect_kBufferSize];
  static unsigned char *bufferPointer = NULL;

  while (1) {
    int rxByte = tty_read();
    if (rxByte < 0) {
      return 0; // no more rx data to process
    } else {
      if (rxByte == ':') {
        bufferPointer = buffer;
      }
      if (bufferPointer != NULL) {
        int bytesReceived = bufferPointer - buffer + 1;
        if (bytesReceived < vedirect_kBufferSize) {
          *bufferPointer++ = rxByte;
          if (rxByte == '\n') {
            *bufferPointer++ = '\0';
            // printf("RX : %s", buffer);
            int bytesDecoded = deframe(data, buffer, maxBytes);
            bufferPointer = NULL;
            return bytesDecoded;
          }
        }
      }
    }
  }
}

int vedirect_write(unsigned char *data, int bytes) {
  unsigned char buffer[bytes * 2 + 2];
  int bytesToSend = enframe(buffer, data, bytes);
  if(bytesToSend > 0){
    printf("TX: %s", buffer);
    return tty_write(buffer, bytesToSend);
  }
  return 0;
}

void vedirect_close(void) {
  close(fd);
  fd = -1;
}
