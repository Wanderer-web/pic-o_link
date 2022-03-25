/*
 * @Author: Wanderer
 * @Date: 2022-03-12 17:05:45
 * @LastEditTime: 2022-03-24 20:39:16
 * @LastEditors: Please set LastEditors
 * @Description:
 * @FilePath: \UART\include\udp.h
 */
#ifndef _UDP_H__
#define _UDP_H__

#include "headfile.h"

extern char *udpSendDataPtr;
extern int udpSendDataLength;

void udp_client_task(void *pvParameters);

#endif
