#ifndef APP_DATA_H__
#define APP_DATA_H__

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t    hw_version;
    uint32_t    fw_version;

    char        iot_server_ip[32];
    char        iot_server_port[8];
    uint8_t     iot_protocol_version;   //与iot平台的通信协议类型

    char        user_name[32];
    char        user_addr[64];
    char        user_phone[16];
    uint8_t     sn[8];

    bool        deploy_state;
    uint32_t    report_interval;

    uint32_t    up_freq;
    uint32_t    down_freq;
    uint8_t     sf;
    uint8_t     bandwidth;
    uint8_t     tx_power;

} app_param_t;


extern app_param_t g_app_param;

#endif
