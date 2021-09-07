#ifndef REG_H
#define REG_H

#include "fmt.h"

#ifdef __cplusplus
extern "C" {
#endif



#define REG_UNIT ((uint32_t)FMT_UNIT << 16)  // display at least one digit
#define REG_MILL ((uint32_t)FMT_MILL << 16)  // make fixed point with 3 decimal places
#define REG_CENT ((uint32_t)FMT_CENT << 16)  // make fixed point with 2 decimal places
#define REG_DECI ((uint32_t)FMT_DECI << 16)  // make fixed point with 1 decimal places
#define REG_2DGT ((uint32_t)FMT_2DGT << 16)  // display at least two digit
#define REG_TIME ((uint32_t)FMT_TIME << 16)  // display as a time
#define REG_BOOL ((uint32_t)FMT_BOOL << 16)  // display as on / off
#define REG_NULL ((uint32_t)FMT_NULL << 16)
#define REG_UINT ((uint32_t)FMT_UINT << 16)
#define REG_FORMAT(M) ((M & 0x00FF0000) >> 16)

#define REG_BYTE 0x00000000
#define REG_WORD 0x10000000
#define REG_SIZE(M) (M & 0xF0000000)

#define REG_MSG_ID(M) (M & 0x0000FFFF)

typedef struct {
  const char *name;
  const uint32_t sig;
} reg_name_t;

#define REG_BATTERYSENSE (0x2002 | REG_CENT | REG_WORD)

#define REG_PANELPOWER ( 0xEDBC | REG_CENT | REG_WORD)
#define REG_PANELVOLTAGE ( 0xEDBB | REG_CENTI | REG_WORD)
#define REG_CHARGECURRENT (0xEDD7| REG_DECI | REG_WORD)


#define REG_NAMES   {"Ppv", REG_PANELPOWER},  \
                    {"Vpv", REG_PANELVOLTAGE},  \

#ifdef __cplusplus
} // extern "C"
#endif

#endif // REG_H
