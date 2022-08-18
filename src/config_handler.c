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

int app_config_data_init(void)
{
    memset(&g_app_param, 0, sizeof(app_param_t));
}

int app_config_data_get(void)
{
    const char app_cfg_obj_name[] = "app_conf";

    JSON_Value  *root_val = NULL;
    JSON_Object *app_cfg_obj = NULL;
    JSON_Value  *val = NULL;

    char *p;

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

    val = json_object_get_value(app_cfg_obj, "iot_ip_addr");
    if(json_value_get_type(val) == JSONString)
    {
        
        //trace_debugln("%s", val->value.string);
        p = (char *)json_value_get_string(val);
        memcpy(p, g_app_param.iot_server_ip, strlen(p));
        trace_debugln("%s obj have obj %s : %s", app_cfg_obj_name, "iot_ip_addr", g_app_param.iot_server_ip);
    }
    else
    {
        trace_errorln("%s obj have no obj %s", app_cfg_obj_name, "iot_ip_addr");
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
