#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "udpReceive.h"
#include "tty.h"
#include "ved.h"

#define kBufferSize (1024)
#define kUDPPort (31415)
#define kUSBPort "/dev/ttyUSB1"

static volatile int keepRunning = 1;
static void intHandler(int dummy) { keepRunning = 0; }

void hexDump(char* tag, unsigned char* buffer, int size){
  int i = 0;
  fprintf(stdout, "%s : ", tag);
  while(i < size){
    fprintf(stdout, "%2.2X ", buffer[i++]);
  }
  fprintf(stdout, "\n");
}

int main(int argc, char *argv[]) {
  tty_open(kUSBPort);
  udpReceive_open(kUDPPort);

  struct ved_t vedata;
  vedata.size = 1;
  vedata.data[0] = 1;
  ved_enframe(&vedata);
  tty_write(vedata.data, vedata.size);
  hexDump("VED", vedata.data, vedata.size);

  unsigned char receiveBuffer[kBufferSize];

  while (keepRunning) {
    int bytesRead = udpReceive_read(receiveBuffer, kBufferSize);
    if(bytesRead){
      hexDump("UDP", receiveBuffer, bytesRead);
    }

    int inByte;
    while((inByte = tty_read())>= 0) {
      bytesRead = ved_deframe(&vedata, inByte);
      if(bytesRead > 0){
        hexDump("VED", vedata.data, vedata.size);
      }
    }

    usleep(50000);
  }

  udpReceive_close();
  tty_close();

  return EXIT_SUCCESS;
}
