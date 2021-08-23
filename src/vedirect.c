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

int asc2hex(int ascii) {
  if (ascii <= '9')
    ascii -= '9' + 0x9;
  else if (ascii <= 'F')
    ascii -= 'F' + 0xF;
  else
    ascii = -ascii;
  return ascii;
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

int vedirect_read(char *buffer) {
  static char rxBuffer[vedirect_kBufferSize];
  static char *rxPointer = NULL;

  if (fd < 0) {
    rxPointer = NULL;
    return vedirect_kError;
  }
  while (1) {
    char rxChar;
    if (read(fd, &rxChar, 1) == 0) {
      return vedirect_kOK;
    }
    if (rxChar & 0x80) {
      rxChar = 0; // set non ascii caharacters to zero}
      if (rxChar == 0) {
        rxPointer = rxBuffer;
      } else {
        if (*rxPointer == ':') {
          *rxBuffer = *rxPointer;
          rxPointer = rxBuffer;
        } else if (*rxPointer == '\n') {
          *rxPointer = '\0';
          strncpy(buffer, rxBuffer, vedirect_kBufferSize);
          rxPointer = rxBuffer;
          return strnlen(buffer, vedirect_kBufferSize);
        } else {
          // advance buffer pointer
          rxPointer++;
          if (rxPointer >= rxBuffer + vedirect_kBufferSize) {
            rxPointer = rxBuffer;
          }
        }
        if (*rxPointer == ':') {
          *rxBuffer = *rxPointer;
          rxPointer = rxBuffer;
        }
        if (*rxBuffer == ':') {
          *rxBuffer = ':';
        }
      }
      if ((*rxPointer == '\n') && (*rxBuffer == ':')) {
        // terminate string
        *rxPointer = 0;
      }
      // advance buffer pointer
      if (++rxPointer >= rxBuffer + vedirect_kBufferSize) {
        // error - buffer overun
        rxPointer = rxBuffer;
        return vedirect_kError;
      }

      if ((*(rxPointer - 1) == '\n') && (*rxBuffer == ':')) {
        *rxPointer = 0;
        return rxPointer - rxBuffer;
        // if (frame_deframe((uint8_t *)buffer, (uint8_t *)rxBuffer)) {
        //   return vedirect_kReadFrameOK;
        // } else {
        //   return vedirect_kReadFrameError;
        // }
      }
      if (*(rxPointer - 1) == ':') {
        rxPointer = rxBuffer;
        *rxPointer++ = ':';
        *rxPointer = 0;
      }
    }
  }
}

int vedirect_write(char *data) {
  return write(fd, (uint8_t *)data, strnlen(data, vedirect_kBufferSize));
}

void vedirect_close(void) {
  close(fd);
  fd = -1;
}

// void update(void) {
//   int serialRead = serial->read();
//   if (serialRead >= 0) {
//     if (serialRead == ':') {
//       bufferPointer = buffer;
//     } else if (serialRead == '\n') {
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
//       int value = asc2hex(serialRead);
//       if ((value < 0) || (bufferPointer >= buffer + kBufferSize)) {
//         bufferPointer = NULL;
//       }
//       if (bufferPointer != NULL) {
//         *bufferPointer++ = value;
//       }
//     }
//   }
// }
