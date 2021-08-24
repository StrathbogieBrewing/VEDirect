#ifndef VED_H
#define VED_H

#ifdef __cplusplus
extern "C" {
#endif
#define ved_kBufferSize (64)

struct ved_t {
  int size;
  unsigned char data[ved_kBufferSize];
};

int ved_enframe(struct ved_t *vedata);
int ved_deframe(struct ved_t *vedata, char inByte);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* VED_H_ */
