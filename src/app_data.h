#ifndef APP_DATA_H__
#define APP_DATA_H__

#include <stdint.h>

typedef struct
{
    uint32_t    hw_version;
    uint32_t    fw_version;

    uint32_t    report_interval;
    uint8_t     sn[8];

    uint32_t    up_freq;
    uint32_t    down_freq;
    uint8_t     sf;
    uint8_t     bandwidth;
    uint8_t     tx_power;

    uint8_t     deploy_state;

    char        iot_server_ip[32];
    char        iot_server_port[8];

    char        user_name[32];
    uint8_t     protocol_version;
} app_param_t;


extern app_param_t g_app_param;

#endif
