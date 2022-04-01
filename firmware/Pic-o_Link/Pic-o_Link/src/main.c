/*
 * @Author: Wanderer
 * @Date: 2022-03-07 20:55:31
 * @LastEditTime: 2022-03-27 14:59:13
 * @LastEditors: Please set LastEditors
 * @Description: 主函数
 * @FilePath: \UART\src\main.c
 */
#include "headfile.h"

static const char *TAG = "app_main";
SemaphoreHandle_t xSemaphore_udpSend = NULL; // udp发送信号量

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //基础驱动初始化
    rgbInit();
    uartInit();
    //配置初始化
    configInit();
    //启动WiFi连接
    ESP_LOGI(TAG, "start working");
    wifi_init_sta();
    //启动udp发送任务
    xSemaphore_udpSend = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(udp_client_task, "udp_client", 20000, NULL, 5, NULL, 1); //将udp通信任务交给1核
    //根据选择协议启动相应接收任务
    ESP_LOGI(TAG, "protocol:%d", picoConfig.protocol);
    if (picoConfig.protocol == PROTOCOL_UART)
    {
        xTaskCreatePinnedToCore(uart_recv_task, "uart_recv", 20000, NULL, 4, NULL, 0); //将串口接收任务交给0核
    }
    else if (picoConfig.protocol == PROTOCOL_SPI)
    {
        xTaskCreatePinnedToCore(spi_recv_task, "spi_recv", 20000, NULL, 4, NULL, 0); //将spi接收任务交给0核
    }
}
