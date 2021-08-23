#include <stdio.h>
#include <stdlib.h>

#include "udpReceive.h"

#define kBufferSize (1024)
#define kUDPPort (31415)
#define kUSBPort "/dev/ttyUSB1"

static volatile int keepRunning = 1;
static void intHandler(int dummy) { keepRunning = 0; }

void hexDump(unsigned char* buffer, int size){
  int i = 0;
  fprintf(stdout, "HEX : ");
  while(i < size){
    fprintf(stdout, "%2.2x ", buffer[i++]);
  }
  fprintf(stdout, "\n");
}

int main(int argc, char *argv[]) {
  vedirect_open(char *port);
  udpReceive_open(kUDPPort);

  unsigned char receiveBuffer[kBufferSize];
  while (keepRunning) {
    int bytesRead = udpReceive_read(receiveBuffer, kBufferSize);
    if(bytesRead){
      hexDump(receiveBuffer, bytesRead);
    }



  }

  udpReceive_close();
  vedirect_close();

  return EXIT_SUCCESS;
}
