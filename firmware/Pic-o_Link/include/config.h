/*
 * @Author: Wanderer
 * @Date: 2022-03-24 18:05:10
 * @LastEditTime: 2022-03-25 11:37:05
 * @LastEditors: Please set LastEditors
 * @Description:
 * @FilePath: \UART\include\config.h
 */
#ifndef _CONFIG_H__
#define _CONFIG_H__

#include "headfile.h"

#define PROTOCOL_UART (0)
#define PROTOCOL_SPI (1)

typedef struct
{
    uint8_t protocol;
    uint32_t uartSpeed;
    uint32_t uartRxBuffer;
    char SSID[32]; /**< SSID of the AP. Null terminated string. */
    char PWD[64];  /**< Password of the AP. Null terminated string. */
    char hostIP[16];
    uint16_t port;
} picoConfig_t;

extern picoConfig_t picoConfig;

void configInit(void);

#endif