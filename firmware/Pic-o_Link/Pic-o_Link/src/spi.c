/*
 * @Author: your name
 * @Date: 2022-03-24 19:02:19
 * @LastEditTime: 2022-03-24 19:46:52
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \Pic-o_Link\src\spi.c
 */
#include "spi.h"

#define GPIO_MOSI 13
#define GPIO_MISO 12
#define GPIO_SCLK 14
#define GPIO_CS 15

#define RCV_HOST HSPI_HOST

#define RX_BUF_SIZE 20000
static const char *TAG = "spi_recv";
WORD_ALIGNED_ATTR char *spiRecvDataPtr = NULL; //四字节对齐

/**
 * @description: SPI初始化
 * @param {*}
 * @return {*}
 */
void spiInit(void)
{
    esp_err_t ret;

    // Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 10000,
    };

    // Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 3,
        .flags = 0,
    };

    // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    // Initialize SPI slave interface
    ret = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO); // SPI DMA传输
    assert(ret == ESP_OK);
}

/**
 * @description: spi接收任务
 * @param {void} *pvParameters
 * @return {*}
 */
void spi_recv_task(void *pvParameters)
{
    spiInit();
    spiRecvDataPtr = (char *)malloc(RX_BUF_SIZE);
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    udpSendDataPtr = spiRecvDataPtr;
    ESP_LOGI(TAG, "core:%d", xPortGetCoreID());
    while (1)
    {
        // Set up a transaction of 10000 bytes to send/receive
        t.length = 6000 * 8;
        t.tx_buffer = NULL;
        t.rx_buffer = spiRecvDataPtr;
        ESP_ERROR_CHECK(spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY));
        // spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
        // received data from the master
        setRgbLevel(0, 0, 0);                // rgb闪烁指示进入通信状态
        udpSendDataLength = t.trans_len / 8; //获取接收数据长度
        xSemaphoreGive(xSemaphore_udpSend);  //释放信号量，通知udp模块完成spi数据接收，进行数据发送
    }
}
