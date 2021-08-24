#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "tty.h"

static int fd = -1;
static struct termios original_termios;

bool tty_open(char *port) {
  // open the serial port
  struct termios termios;

  fd = open(port, O_RDWR | O_NOCTTY); // | O_NONBLOCK);
  if (fd < 0) {
    fprintf(stderr, "TTY Device \"%s\" could not be opened\n", port);
    return tty_kError;
  }
  if (!isatty(fd)) {
    fprintf(stderr, "TTY Device \"%s\" is not a tty\n", port);
    return tty_kError;
  }
  // initialize terminal modes
  int rc = tcgetattr(fd, &termios);
  if (rc < 0) {
    fprintf(stderr, "TTY Device tcgetattr() failed\n");
    return tty_kError;
  }
  original_termios = termios;
  termios.c_iflag = IGNBRK;
  termios.c_oflag = 0;
  termios.c_lflag = 0;
  termios.c_cflag = (CS8 | CREAD | CLOCAL);
  termios.c_cc[VMIN] = 0;
  termios.c_cc[VTIME] = 0;
  if (cfsetospeed(&termios, B19200) < 0) {
    fprintf(stderr, "TTY Device cfsetospeed() failed\n");
    return tty_kError;
  }
  if (cfsetispeed(&termios, B19200) < 0) {
    fprintf(stderr, "TTY Device cfsetispeed() failed\n");
    return tty_kError;
  }
  rc = tcsetattr(fd, TCSANOW, &termios);
  if (rc < 0) {
    fprintf(stderr, "TTY Device tcsetattr() failed\n");
    return tty_kError;
  }
  return tty_kOK;
}

int tty_read(void) {
  unsigned char rxByte;
  ssize_t bytesRead = read(fd, &rxByte, 1);
  if (bytesRead == 1)
    return rxByte;
  else
    return -1;
}

int tty_write(unsigned char *txBuffer, int bytesToSend) {
  return write(fd, txBuffer, bytesToSend);
}

void tty_close(void) {
  if (fd != -1)
    close(fd);
  fd = -1;
}
