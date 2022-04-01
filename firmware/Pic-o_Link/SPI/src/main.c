/*
 * @Author: Wanderer
 * @Date: 2022-03-07 20:55:31
 * @LastEditTime: 2022-03-27 16:24:58
 * @LastEditors: Please set LastEditors
 * @Description: 主函数
 * @FilePath: \UART\src\main.c
 */
#include "headfile.h"

#define GPIO_HANDSHAKE 2
#define GPIO_MOSI 13
#define GPIO_MISO 12
#define GPIO_SCLK 14
#define GPIO_CS 15

#define RCV_HOST HSPI_HOST

const char *TAG = "app_main";
static const int RX_BUF_SIZE = 10000;
SemaphoreHandle_t xSemaphore_udpSend = NULL; // udp发送信号量
char *spiDataPtr = NULL;                     //指向接收缓冲区
int spiDataLength = 0;                       //实际接收的数据长度

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
        .mode = 3,
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
    spiInit();
    wifi_init_sta();

    xSemaphore_udpSend = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(udp_client_task, "udp_client", 20000, NULL, 5, NULL, 1); //将udp通信任务交给1核，数据接收任务交给0核

    WORD_ALIGNED_ATTR char *recvbuf; //四字节对齐
    recvbuf = (char *)malloc(RX_BUF_SIZE);
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    spiDataPtr = recvbuf;

    ESP_LOGE(TAG, "core:%d", xPortGetCoreID());
    while (1)
    {
        // Set up a transaction of 6000 bytes to send/receive
        t.length = 6000 * 8;
        t.tx_buffer = NULL;
        t.rx_buffer = recvbuf;
        ESP_ERROR_CHECK(spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY));
        // spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
        // received data from the master
        setRgbLevel(0, 0, 0);               // rgb闪烁指示进入通信状态
        spiDataLength = t.trans_len / 8;    //获取接收数据长度
        xSemaphoreGive(xSemaphore_udpSend); //释放信号量，通知udp模块完成spi数据接收，进行数据发送
    }
}
