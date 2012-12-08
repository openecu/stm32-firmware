#ifndef _CONFIG_H
#define _CONFIG_H

typedef struct
{
    uint8_t fan_temp;
    uint8_t fan_temp_hyst;
} config_t;

extern config_t config;

#endif
