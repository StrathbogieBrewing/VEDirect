#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tty.h"
#include "udpReceive.h"
#include "ved.h"

#include "AceBMS.h"

#define kBufferSize (1024)
#define kUDPPort (31415)
#define kUSBPort "/dev/ttyUSB1"

static volatile int keepRunning = 1;
static void intHandler(int dummy) { keepRunning = 0; }

void hexDump(char *tag, unsigned char *buffer, int size) {
  int i = 0;
  fprintf(stdout, "%s : ", tag);
  while (i < size) {
    fprintf(stdout, "%2.2X ", buffer[i++]);
  }
  fprintf(stdout, "\n");
}

int main(int argc, char *argv[]) {
  tty_open(kUSBPort);
  udpReceive_open(kUDPPort);

  ved_t vedata;
  vedata.size = 1;
  vedata.data[0] = 1;
  ved_enframe(&vedata);
  tty_write(vedata.data, vedata.size);
  hexDump("VED", vedata.data, vedata.size);

  unsigned char receiveBuffer[kBufferSize];

  while (keepRunning) {
    int bytesRead = udpReceive_read(receiveBuffer, kBufferSize);
    if (bytesRead == 12) {
      hexDump("UDP", receiveBuffer, bytesRead);

      msg_t *msg = (msg_t *)(&receiveBuffer[1]);
      int16_t value;
      fmt_t format = sig_decode(msg, ACEBMS_VBAT, &value);
      if (format != FMT_NULL) {
        // printf("BAT : %d\n", value);
        uint8_t batHi = (value / 10) >> 8;
        uint8_t batLo = (value / 10) & 0x00FF;
        ved_t batteryMessage = {.size = 6,
                                .data = {0x08, 0x02, 0x20, 0x00, batLo, batHi}};
        ved_enframe(&batteryMessage);
        tty_write(batteryMessage.data, batteryMessage.size);
        // printf("MSG : (%d) %s", batteryMessage.size, batteryMessage.data);
      }
    }

    int inByte;
    while ((inByte = tty_read()) >= 0) {
      bytesRead = ved_deframe(&vedata, inByte);
      if (bytesRead > 0) {
        hexDump("VED", vedata.data, vedata.size);
        vedata.size = 0;
      }
    }

    usleep(50000);
  }

  udpReceive_close();
  tty_close();

  return EXIT_SUCCESS;
}
