#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fcntl.h>

#include "parson.h"
#include "trace.h"

#include "boards.h"
#include "app_files.h"
#include "app_data.h"
#include "config_handler.h"

int g_config_file_fd = -1;

app_param_t g_app_param;

void app_config_data_init(void)
{
    memset(&g_app_param, 0, sizeof(app_param_t));
}

int app_config_data_get(void)
{
    const char app_cfg_obj_name[] = "app_conf";
    const char iot_ip_add_obj_name[] = "iot_ip_addr";
    const char iot_port_obj_name[] = "iot_port";
    const char iot_protocol_ver_obj_name[] = "iot_protocol_version";

    JSON_Value  *root_val = NULL;
    JSON_Object *app_cfg_obj = NULL;
    JSON_Value  *val = NULL;

    char  *p = NULL;
    size_t      len = 0;

    // 从配置文件中，获取配置参数
    g_config_file_fd = open(APP_CONFIG_FILE, O_RDWR|O_CREAT, 0666);
    if(g_config_file_fd < 0)
    {
        trace_error("Open %s file error\n\r", APP_CONFIG_FILE);
        close(g_config_file_fd);
        return -1;
    }
    else
    {
        trace_debugln("%s config file opened", APP_CONFIG_FILE);
    }

    /* Try to parse gw_transmit_service_cfg.json file*/
    root_val = json_parse_file_with_comments(APP_CONFIG_FILE);
    if (root_val == NULL) 
    {
        trace_errorln("%s is not a valid JSON file", APP_CONFIG_FILE);
        close(g_config_file_fd);
        return -1;
    }

/**
 * @brief 解析 app_conf 配置结构
 */
#if 1

    /* point to the app_conf object */
    app_cfg_obj = json_object_get_object(json_value_get_object(root_val), app_cfg_obj_name);
    if (app_cfg_obj == NULL)
    {
        trace_error("%s contain a JSON object named %s", APP_CONFIG_FILE, app_cfg_obj_name);
        close(g_config_file_fd);
        return -1;
    }

    /* iot_ip_addr */
    val = json_object_get_value(app_cfg_obj, iot_ip_add_obj_name);
    if(json_value_get_type(val) == JSONString)
    {
        p = (char *)json_value_get_string(val);
        memcpy(g_app_param.iot_server_ip, p, strlen(p));
        trace_debugln("%s - %s : %s", app_cfg_obj_name, iot_ip_add_obj_name, g_app_param.iot_server_ip);
    }
    else
    {
        trace_errorln("%s have no obj : %s !", app_cfg_obj_name, iot_ip_add_obj_name);
    }

    /* iot_port */
    val = json_object_get_value(app_cfg_obj, iot_port_obj_name);
    if(json_value_get_type(val) == JSONString)
    {
        p = (char *)json_value_get_string(val);
        memcpy(g_app_param.iot_server_port, p, strlen(p));
        trace_debugln("%s - %s : %s", app_cfg_obj_name, iot_port_obj_name, g_app_param.iot_server_port);
    }
    else
    {
        trace_errorln("%s have no obj : %s !", app_cfg_obj_name, iot_port_obj_name);
    }

    /* iot_protocol_version */
    val = json_object_get_value(app_cfg_obj, iot_protocol_ver_obj_name);
    if(json_value_get_type(val) == JSONNumber)
    {
        g_app_param.iot_protocol_version = (unsigned)json_value_get_number(val);
        trace_debugln("%s - %s : %d", app_cfg_obj_name, iot_protocol_ver_obj_name, g_app_param.iot_protocol_version);
    }
    else
    {
        trace_errorln("%s have no obj : %s !", app_cfg_obj_name, iot_protocol_ver_obj_name);
    }


#endif


    close(g_config_file_fd);
    trace_debugln("%s config file close", APP_CONFIG_FILE);

    return 0;
}

int app_config_data_storage(void)
{

    return 0;
}
