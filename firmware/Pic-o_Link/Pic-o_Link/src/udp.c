/*
 * @Author: Wanderer
 * @Date: 2022-03-12 17:05:21
 * @LastEditTime: 2022-03-24 20:44:41
 * @LastEditors: Please set LastEditors
 * @Description: udp模块
 * @FilePath: \UART\src\udp.c
 */

#include "udp.h"

static const char *TAG = "udp client";

char *udpSendDataPtr = NULL;
int udpSendDataLength = 0;

/**
 * @description:  udp client任务
 * @param {void} *pvParameters
 * @return {*}
 */
void udp_client_task(void *pvParameters)
{
    int addr_family = 0;
    int ip_protocol = 0;
    while (1)
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(picoConfig.hostIP);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(picoConfig.port);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, sending to %s:%d", picoConfig.hostIP, picoConfig.port);
        ESP_LOGI(TAG, "core:%d", xPortGetCoreID());
        setRgbLevel(1, 1, 0);
        while (1)
        {
            if (xSemaphoreTake(xSemaphore_udpSend, 1) == pdTRUE) //等待0核接收完串口数据
            {
                int err = sendto(sock, udpSendDataPtr, udpSendDataLength, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                setRgbLevel(1, 1, 0);
                if (err < 0)
                {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
            }
        }
        if (sock != -1)
        {
            setRgbLevel(1, 0, 1);
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    setRgbLevel(0, 1, 1);
    vTaskDelete(NULL);
}