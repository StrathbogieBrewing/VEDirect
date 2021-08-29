#ifndef VED_H
#define VED_H

#ifdef __cplusplus
extern "C" {
#endif
#define ved_kBufferSize (32)

typedef struct {
  int size;
  unsigned char data[ved_kBufferSize];
} ved_t;

int ved_enframe(ved_t *vedata);
int ved_deframe(ved_t *vedata, char inByte);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* VED_H_ */
