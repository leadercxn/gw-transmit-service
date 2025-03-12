# 详细程序设计

[TOC]

## 1. 文件

### 1.1 配置文件
* cloud-lora 应用程序(工程生成可执行文件)：  `~/bin/gw_transmit_service`
    + `gw_transmit_service`运行依赖
        1. 消息队列文件
            - ` ~/bin/msg/host-cloud-msg  `
            - ` ~/bin/msg/cloud-host-msg  `
            - ` ~/bin/msg/host-lora-msg  `
            - ` ~/bin/msg/lora-host-msg  `
            - ` ~/bin/msg/ble-host-msg  `
            - ` ~/bin/msg/host-ble-msg  `
* app 应用配置  ` ~/bin/config/gw_transmit_service_cfg.json `
* 日志  ` ~/log/yy_mm_dd.log `
* 设备管理  ` ~/dev_manage/dev_manage.list `
* 

### 1.2 文件内容
* 


## 2. 操作
### 2.1 文件内容
* 编译
    ```bash
        $ make
    ```

* 执行
    ```bash
        ./bin/gw_transmit_service
    ```


## 3. 坑
* 

## n. 参考源码
### n.1 mqtt参考源码
* [MQTT-C](git@github.com:LiamBindle/MQTT-C.git)

## 



