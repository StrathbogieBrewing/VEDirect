#ifndef TTY_H_
#define TTY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define tty_kOK (true)
#define tty_kError (false)

bool tty_open(char *port);
int tty_read(void);
int tty_write(unsigned char *txBuffer, int bytesToSend);
void tty_close(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* TTY_H_ */
