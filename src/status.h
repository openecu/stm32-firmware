#ifndef _STATUS_H
#define _STATUS_H
    
#include <stdint.h>

#define STATUS_FLAGS1_CRANK     0x00000001
#define STATUS_FLAGS1_RUN       0x00000002
#define STATUS_FLAGS1_STROKE    0x00000004

#define STATUS_FLAGS2_FAN           0x00000001
#define STATUS_FLAGS2_VVT           0x00000002
#define STATUS_FLAGS2_FUEL_PUMP     0x00000004
#define STATUS_FLAGS2_MAIN_RELAY    0x00000008
#define STATUS_FLAGS2_IGN_SW        0x00010000
#define STATUS_FLAGS2_START_SW      0x00020000

#define STATUS_FLAGS3_ACCEL_ENRICH  0x00000001

typedef struct status_s
{
    uint32_t flags1;
    uint32_t flags2;
    uint32_t flags3;
    uint32_t mtime;
    uint32_t utime;
    uint16_t load;
    uint16_t acce;
    uint16_t wue;
    uint16_t ase;
    uint16_t enrich;
    uint16_t afr;
    uint16_t injpw;
} status_t;

#endif
