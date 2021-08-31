#ifndef UDPRECEIVE_H
#define UDPRECEIVE_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

int udpReceive_open(int udp_port);
int udpReceive_read(unsigned char *data, int bytes);
void udpReceive_close(void);

#if defined(__cplusplus)
}
#endif

#endif // UDPRECEIVE_H
