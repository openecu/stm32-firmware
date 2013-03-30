#ifndef _STATUS_H
#define _STATUS_H
    
#include <stdint.h>

#define STATUS_FLAGS1_CRANK     0x00000001
#define STATUS_FLAGS1_RUN       0x00000002
#define STATUS_FLAGS1_STROKE    0x00000004

#define STATUS_FLAGS2_FAN   0x00000001
#define STATUS_FLAGS2_VVT   0x00000002

typedef struct status_s
{
    uint32_t flags1;
    uint32_t flags2;
} status_t;

#endif
