#ifndef VED_H
#define VED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ved_kBufferSize (32)

typedef struct {
  uint8_t  size;
  uint8_t  data[ved_kBufferSize];
} ved_t;

uint8_t  ved_enframe(ved_t *vedata);
uint8_t  ved_deframe(ved_t *vedata, char inByte);

uint8_t ved_getCommand(ved_t *vedata);
uint16_t ved_getId(ved_t *vedata);
int32_t ved_getU8(ved_t *vedata);
int32_t ved_getU16(ved_t *vedata);
int32_t ved_getU32(ved_t *vedata);

void ved_setCommand(ved_t *vedata, uint8_t value);
void ved_setId(ved_t *vedata, uint16_t value);
void ved_setU8(ved_t *vedata, uint8_t value);
void ved_setU16(ved_t *vedata, uint16_t value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* VED_H_ */
