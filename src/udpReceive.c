#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udpReceive.h"

static int udp_socket = -1;
static struct sockaddr_in si_me;

int udpReceive_open(int udp_port) {
  // check socket doesnt already exist
  if (udp_socket != -1) {
    fprintf(stderr, "UDP Socket already open\n");
    return -1;
  }

  // create a non blocking socket
  udp_socket = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
  if (udp_socket == -1) {
    fprintf(stderr, "UDP Socket could not be opened\n");
    return -1;
  }

  int broadcast = 1;
  int err = setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast,
                       sizeof(broadcast));
  if (err == -1) {
    fprintf(stderr, "UDP Socket could not be set for broadcast\n");
    return -1;
  }

  int reuse = 1;
  err = setsockopt(udp_socket, SOL_SOCKET, SO_REUSEPORT, &reuse,
                       sizeof(reuse));
  if (err == -1) {
    fprintf(stderr, "UDP Socket could not be set for port reuse\n");
    return -1;
  }

  // bind socket to udp port
  memset((char *)&si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(udp_port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(udp_socket, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
    fprintf(stderr, "UDP Socket could not be bound to port\n");
    close(udp_socket);
    udp_socket = -1;
    return -1;
  }
}

int udpReceive_read(unsigned char *data, int maxBytes) {
  struct sockaddr_in si_other = {0};
  int slen = sizeof(si_other);
  int recv_len = 0;

  if (udp_socket == -1) {
    fprintf(stderr, "UDP Socket not open\n");
    return -1;
  }

  if ((recv_len = recvfrom(udp_socket, data, maxBytes, 0,
                           (struct sockaddr *)&si_other, &slen)) == -1) {
    if ((errno != EAGAIN) && (errno != EWOULDBLOCK)) {
      fprintf(stderr, "UDP Socket read failed\n");
      return -1;
    }
    recv_len = 0;
  }

  return recv_len;
}


void udpReceive_close(void) {
  // check socket exists before closing
  if (udp_socket != -1) {
    close(udp_socket);
  }
}
