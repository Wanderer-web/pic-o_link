/*
 * @Author: Wanderer
 * @Date: 2022-03-07 20:55:31
 * @LastEditTime: 2022-03-24 16:21:33
 * @LastEditors: Please set LastEditors
 * @Description: 主函数
 * @FilePath: \UART\src\main.c
 */
#include "headfile.h"

#define TXD_PIN (1)
#define RXD_PIN (3)
#define RTS_PIN (13)

const char *TAG = "app_main";
static const int RX_BUF_SIZE = 20000;
SemaphoreHandle_t xSemaphore_udpSend = NULL; // udp发送信号量
char *uartDataPtr = NULL;                    //指向接收缓冲区
int len = 0;
int uartDataLength = 0; //实际接收的数据长度

/**
 * @description: 串口初始化
 * @param {*}
 * @return {*}
 */
void uartInit(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 3000000, //波特率
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS, //开启硬件流控
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, RTS_PIN, UART_PIN_NO_CHANGE);
}

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

    rgbInit();
    uartInit();
    wifi_init_sta();

    xSemaphore_udpSend = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(udp_client_task, "udp_client", 20000, NULL, 5, NULL, 1); //将udp通信任务交给1核，串口接收任务交给0核

    uartDataPtr = (char *)malloc(RX_BUF_SIZE); //创建串口接收缓冲区，大小20000字节

    ESP_LOGE(TAG, "core:%d", xPortGetCoreID());
    while (1)
    {
        // Read data from the UART
        len = uart_read_bytes(UART_NUM_0, uartDataPtr, RX_BUF_SIZE - 1, 20 / portTICK_PERIOD_MS); //串口轮询最长等待20ms，低于这个值系统可能出现错误

        if (len > 0)
        {
            setRgbLevel(0, 0, 0);               // rgb闪烁指示进入通信状态
            uartDataLength = len;               //获取串口数据长度
            xSemaphoreGive(xSemaphore_udpSend); //释放信号量，通知udp模块完成串口数据接收，进行数据发送
        }
    }
}
