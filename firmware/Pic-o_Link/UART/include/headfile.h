/*
 * @Author: Wanderer
 * @Date: 2022-03-12 17:08:30
 * @LastEditTime: 2022-03-17 19:16:58
 * @LastEditors: Please set LastEditors
 * @Description: 头文件集合
 * @FilePath: \UART\include\headfile.h
 */
#ifndef _HEADFILE_H
#define _HEADFILE_H

//官方驱动头文件
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <lwip/netdb.h>
#include "driver/gpio.h"
#include "driver/uart.h"

//用户头文件
#include "rgb.h"
#include "udp.h"
#include "wifi.h"

extern SemaphoreHandle_t xSemaphore_udpSend;
extern char *uartDataPtr;
extern int uartDataLength;

#endif
