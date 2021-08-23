#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#include "vedirect.h"

#define vedirect_kBufferSize (256)

static int fd = -1;
static struct termios original_termios;

unsigned char asc2bin(unsigned char ascii) {
  if (ascii < '0')
    return 0;
  unsigned char val = ascii - '0';
  if (val > 9)
    val -= 7;
  if (val > 15)
    return 0;
  return val;
}

static int enframe() {}

static int deframe(unsigned char *frame) {
  unsigned char *ptr = frame + 1;
  unsigned char csum = asc2bin(*ptr++);
  while (*ptr != '\n') {
    csum += asc2bin(*ptr++) << 4;
    if (*ptr == '\n')
      break;
    csum += asc2bin(*ptr++);
  }

  printf("Bytes : %d  Checksum : %2.2X  String %s", frameSize, csum, frame);
  // if (csum == 0x55) {
  //   *ptr = '\0';
  //   memcpy(data, buffer, bytesReceived);
  //   return bytesReceived;
  // }
}

void debug(char data) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long int usecs = tv.tv_sec * 1000000L + tv.tv_usec;
  if (isalnum(data)) {
    printf("%ld\t%2.2X\t%c\n", usecs, data, data);
  } else {
    printf("%ld\t%2.2X\n", usecs, data);
  }
}

int vedirect_open(char *port) {
  // open the serial port
  int rc = -1;
  struct termios termios;

  fd = open(port, O_RDWR | O_NOCTTY); // | O_NONBLOCK);
  if (fd < 0) {
    fprintf(stderr, "TTY Device \"%s\" could not be opened\n", port);
    return vedirect_kError;
  }

  if (!isatty(fd)) {
    fprintf(stderr, "TTY Device \"%s\" is not a tty\n", port);
    return vedirect_kError;
  }
  // initialize terminal modes
  rc = tcgetattr(fd, &termios);
  if (rc < 0) {
    fprintf(stderr, "TTY Device tcgetattr() failed\n");
    return vedirect_kError;
  }
  original_termios = termios;
  termios.c_iflag = IGNBRK;
  termios.c_oflag = 0;
  termios.c_lflag = 0;
  termios.c_cflag = (CS8 | CREAD | CLOCAL);
  termios.c_cc[VMIN] = 0;
  termios.c_cc[VTIME] = 1;
  if (cfsetospeed(&termios, B19200) < 0) {
    fprintf(stderr, "TTY Device cfsetospeed() failed\n");
    return vedirect_kError;
  }
  if (cfsetispeed(&termios, B19200) < 0) {
    fprintf(stderr, "TTY Device cfsetispeed() failed\n");
    return vedirect_kError;
  }
  rc = tcsetattr(fd, TCSANOW, &termios);
  if (rc < 0) {
    fprintf(stderr, "TTY Device tcsetattr() failed\n");
    return vedirect_kError;
  }
  return vedirect_kOK;
}

int vedirect_read(unsigned char *data, int maxBytes) {
  static unsigned char buffer[vedirect_kBufferSize];
  static unsigned char *bufferPointer = NULL;

  if (fd < 0) {
    bufferPointer = NULL;
    return vedirect_kError;
  }

  while (1) {
    unsigned char rxByte;
    if (read(fd, &rxByte, 1) == 0) {
      // no more rx data to process
      return vedirect_kOK;
    } else {
      if (rxByte == ':') {
        bufferPointer = buffer;
      }
      if (bufferPointer != NULL) {
        int bytesReceived = bufferPointer - buffer + 1;
        if (bytesReceived < vedirect_kBufferSize) {
          *bufferPointer++ = rxByte;
          if (rxByte == '\n') {
            memcpy(data, buffer, bytesReceived);
            *bufferPointer++ = '\0';
            deframe(buffer);
            bufferPointer = NULL;
            return bytesReceived;
          }
        }
      }
    }
  }
}

int vedirect_write(unsigned char *data) {
  return write(fd, (uint8_t *)data, strnlen(data, vedirect_kBufferSize));
}

void vedirect_close(void) {
  close(fd);
  fd = -1;
}

//   if (rxByte == ':') {
//     bufferPointer = buffer;
//   } else if (rxByte == '\n') {
//     if (bufferPointer != NULL) {
//       int bytesReceived = bufferPointer - buffer + 1;
//       // if (bytesReceived & 0x01) { // valid frame has odd quantity
//       // test checksum
//       // unsigned char *ptr = buffer;
//       // unsigned char csum = *ptr++;
//       // while (ptr < bufferPointer) {
//       //   csum += (*ptr++) << 4;
//       //   csum += (*ptr++);
//       // }
//       // if (csum == 0x55) {
//       //   *ptr = '\0';
//       memcpy(data, buffer, bytesReceived);
//       return bytesReceived;
//       // }
//       // }
//     }
//     bufferPointer = NULL;
//   } else {
//     int value = asc2bin(rxByte);
//     if ((value < 0) || (bufferPointer >= buffer + vedirect_kBufferSize))
//     {
//       bufferPointer = NULL;
//     }
//     if (bufferPointer != NULL) {
//       *bufferPointer++ = value;
//     }
//   }

// if (rxChar & 0x80) {
//   rxChar = 0; // set non ascii caharacters to zero}
//   if (rxChar == 0) {
//     rxPointer = rxBuffer;
//   } else {
//     if (*rxPointer == ':') {
//       *rxBuffer = *rxPointer;
//       rxPointer = rxBuffer;
//     } else if (*rxPointer == '\n') {
//       *rxPointer = '\0';
//       strncpy(buffer, rxBuffer, vedirect_kBufferSize);
//       rxPointer = rxBuffer;
//       return strnlen(buffer, vedirect_kBufferSize);
//     } else {
//       // advance buffer pointer
//       rxPointer++;
//       if (rxPointer >= rxBuffer + vedirect_kBufferSize) {
//         rxPointer = rxBuffer;
//       }
//     }
//     if (*rxPointer == ':') {
//       *rxBuffer = *rxPointer;
//       rxPointer = rxBuffer;
//     }
//     if (*rxBuffer == ':') {
//       *rxBuffer = ':';
//     }
//   }
//   if ((*rxPointer == '\n') && (*rxBuffer == ':')) {
//     // terminate string
//     *rxPointer = 0;
//   }
//   // advance buffer pointer
//   if (++rxPointer >= rxBuffer + vedirect_kBufferSize) {
//     // error - buffer overun
//     rxPointer = rxBuffer;
//     return vedirect_kError;
//   }
//
//   if ((*(rxPointer - 1) == '\n') && (*rxBuffer == ':')) {
//     *rxPointer = 0;
//     return rxPointer - rxBuffer;
//     // if (frame_deframe((uint8_t *)buffer, (uint8_t *)rxBuffer)) {
//     //   return vedirect_kReadFrameOK;
//     // } else {
//     //   return vedirect_kReadFrameError;
//     // }
//   }
//   if (*(rxPointer - 1) == ':') {
//     rxPointer = rxBuffer;
//     *rxPointer++ = ':';
//     *rxPointer = 0;
//   }
// }

// void update(void) {
//   int rxByte = serial->read();
//   if (rxByte >= 0) {
//     if (rxByte == ':') {
//       bufferPointer = buffer;
//     } else if (rxByte == '\n') {
//       if (bufferPointer != NULL) {
//         int bytesReceived = bufferPointer - buffer + 1;
//         if (bytesReceived & 0x01) { // valid frame has odd quantity
//           // test checksum
//           uint8_t *ptr = buffer;
//           uint8_t csum = *ptr++;
//           while (ptr < bufferPointer) {
//             csum += (*ptr++) << 4;
//             csum += (*ptr++);
//           }
//           if (csum == 0x55) {
//           }
//         }
//       }
//       bufferPointer = NULL;
//     } else {
//       int value = asc2bin(rxByte);
//       if ((value < 0) || (bufferPointer >= buffer + kBufferSize)) {
//         bufferPointer = NULL;
//       }
//       if (bufferPointer != NULL) {
//         *bufferPointer++ = value;
//       }
//     }
//   }
// }
