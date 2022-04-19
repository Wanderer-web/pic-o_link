/*
 * @Author: Wanderer
 * @Date: 2022-03-24 19:02:35
 * @LastEditTime: 2022-03-24 19:23:57
 * @LastEditors: Please set LastEditors
 * @Description:
 * @FilePath: \Pic-o_Link\include\uart.h
 */
#ifndef _UART_H__
#define _UART_H__

#include "headfile.h"

void uartInit(void);
void uart_recv_task(void *pvParameters);

#endif