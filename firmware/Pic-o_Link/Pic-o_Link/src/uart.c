/*
 * @Author: your name
 * @Date: 2022-03-24 19:02:12
 * @LastEditTime: 2022-03-25 20:32:27
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Pic-o_Link\src\uart.c
 */
#include "uart.h"

#define TXD_PIN (1)
#define RXD_PIN (3)
#define RTS_PIN (13)

#define RX_BUF_SIZE (20000)
static const char *TAG = "uart_recv";

char *uartRecvDataPtr = NULL;

/**
 * @description: 串口初始化
 * @param {*}
 * @return {*}
 */
void uartInit(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200, //波特率
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //开启硬件流控
        .source_clk = UART_SCLK_APB,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_0, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

/**
 * @description: UART接收任务
 * @param {void} *pvParameters
 * @return {*}
 */
void uart_recv_task(void *pvParameters)
{
    const uart_config_t uart_config = {
        .baud_rate = picoConfig.uartSpeed, //波特率
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS, //开启硬件流控
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, TXD_PIN, RXD_PIN, RTS_PIN, UART_PIN_NO_CHANGE);
    uartRecvDataPtr = (char *)malloc(RX_BUF_SIZE); //创建串口接收缓冲区
    int uartRecvDataLength = 0;
    udpSendDataPtr = uartRecvDataPtr;
    ESP_LOGI(TAG, "core:%d", xPortGetCoreID());
    while (1)
    {
        // Read data from the UART
        uartRecvDataLength = uart_read_bytes(UART_NUM_0, uartRecvDataPtr, picoConfig.uartRxBuffer, 20 / portTICK_PERIOD_MS); //串口轮询最长等待20ms，低于这个值系统可能出现错误
        if (uartRecvDataLength > 0)
        {
            setRgbLevel(0, 0, 0);                   // rgb闪烁指示进入通信状态
            udpSendDataLength = uartRecvDataLength; //获取串口数据长度
            xSemaphoreGive(xSemaphore_udpSend);     //释放信号量，通知udp模块完成串口数据接收，进行数据发送
            uart_flush(UART_NUM_0);
        }
    }
}
