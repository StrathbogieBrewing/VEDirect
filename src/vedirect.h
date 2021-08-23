#ifndef VEDIRECT_H_
#define VEDIRECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define vedirect_kError (-1)
#define vedirect_kOK (0)

int vedirect_open(char *port);

int vedirect_read(char *data);
int vedirect_write(char *data);

void vedirect_close(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* VEDIRECT_H_ */
