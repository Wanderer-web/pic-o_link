#include "pico_link.h"

#define DEVICE_TC264 //目标芯片名称

#if (defined DEVICE_TC264) || (defined DEVICE_RT1064)

#include "zf_uart.h"
#include "zf_spi.h"
#include "zf_gpio.h"
#include "SEEKFREE_WIRELESS.h"

#define UART_PORT (WIRELESS_UART) //串口端口默认配置和逐飞无线串口接口相同
#define UART_TXD (WIRELESS_UART_TX)
#define UART_RXD (WIRELESS_UART_RX)
#define UART_CTS (RTS_PIN)

#define uartWriteBuff uart_putbuff

#elif (defined DEVICE_MM32) // MM32请使用最新的逐飞库

#include "zf_driver_uart.h"
#include "zf_driver_spi.h"
#include "zf_driver_gpio.h"
#include "zf_device_wireless_uart.h"

#define UART_PORT (WRIELESS_UART_INDEX) //串口端口默认配置和逐飞无线串口接口相同
#define UART_TXD (WRIELESS_UART_RX_PIN)
#define UART_RXD (WRIELESS_UART_TX_PIN)
#define UART_CTS (WRIELESS_UART_RTS_PIN)

#define uartWriteBuff uart_write_buffer

#endif

#define HAS_CTS_PIN (1) //是否接了流控引脚

#define UART_BAUD (3000000)     // UART波特率
#define SPI_BAUD (10000000)     // SPI波特率
#define IPC_BUFFER_SIZE (25000) //致用上位机发送缓冲区大小，建议略大于图像字节数，太大会占用过多内存
//下列参数根据单片机型号自行定义
#define SPI_NUM (0)
#define SPI_SCK_PIN (0)  //接模块CLK
#define SPI_MOSI_PIN (0) //接模块MOSI
#define SPI_MISO_PIN (0) //接模块MISO
#define SPI_CS_PIN (0)   //接模块CS
static pico_uint8 ipcSendBuffer[IPC_BUFFER_SIZE];

/**
 * @description: Pico Link 串口初始化
 * @return {*}
 */
void picoLinkInit_Uart(void)
{
#if HAS_CTS_PIN
#if (defined DEVICE_TC264)
    gpio_init(UART_CTS, GPI, 0, PULLDOWN); //初始化流控引脚
#elif (defined DEVICE_RT1064)
    gpio_init(UART_CTS, GPI, 0, PULLDOWN_100K); //初始化流控引脚
#elif (defined DEVICE_MM32)
    gpio_init(UART_CTS, GPI, 0, GPI_PULL_DOWN);
#endif
#endif
    uart_init(UART_PORT, UART_BAUD, UART_TXD, UART_RXD); //初始化串口
}

/**
 * @description: Pico Link 串口发送多个字节（硬件协议端）
 * @param {pico_uint8} *pBuff 数据指针
 * @param {pico_uint32} len 数据长度
 * @return {*}
 */
static inline void BaseSendBuff_Uart(pico_uint8 *pBuff, pico_uint32 len)
{
#if HAS_CTS_PIN
#define UART_FIFO_LENGTH (128) //接收端UART FIFO长度
    while (len > UART_FIFO_LENGTH)
    {
        while (gpio_get(UART_CTS))
            ; //如果CTS为低电平，则继续发送数据
        uartWriteBuff(UART_PORT, pBuff, UART_FIFO_LENGTH);
        pBuff += UART_FIFO_LENGTH; //地址偏移
        len -= UART_FIFO_LENGTH;   //数量
    }
    while (gpio_get(UART_CTS))
        ;                                 //如果CTS为低电平，则继续发送数据
    uartWriteBuff(UART_PORT, pBuff, len); //发送最后的数据
#else
    uartWriteBuff(UART_PORT, pBuff, len);
#endif
}

/**
 * @description: Pico Link 串口发送单个字节
 * @param {pico_uint8} data 字节数据
 * @return {*}
 */
void picoLinkSendByte_Uart(pico_uint8 data)
{
    BaseSendBuff_Uart(&data, 1);
}

/**
 * @description:Pico Link 串口发送多个字节
 * @param {pico_uint8} *data 数据指针
 * @param {pico_uint32} length 数据长度
 * @return {*}
 */
void picoLinkSendBytes_Uart(pico_uint8 *data, pico_uint32 length)
{
    BaseSendBuff_Uart(data, length); //发送数据
}

/**
 * @description: Pico Link 串口发送图像到致用上位机
 * @param {pico_uint8} *pImage 图像指针
 * @param {pico_uint32} imageH 图像高度
 * @param {pico_uint32} imageW 图像宽度
 * @return {*}
 */
void ipcSendImage_Uart(pico_uint8 *pImage, pico_uint32 imageH, pico_uint32 imageW)
{
    BaseSendBuff_Uart((pico_uint8 *)"CSU", 3);  //发送帧头
    BaseSendBuff_Uart(pImage, imageH * imageW); //发送图像数据
    BaseSendBuff_Uart((pico_uint8 *)"USC", 3);  //发送帧尾
}

/**
 * @description:Pico Link SPI初始化
 * @return {*}
 */
void picoLinkInit_Spi(void)
{
#if (defined DEVICE_TC264) || (defined DEVICE_RT1064)
    spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_CS_PIN, 3, SPI_BAUD); // SPI初始化
#elif (defined DEVICE_MM32)
    spi_init(SPI_NUM, 3, SPI_BAUD, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_CS_PIN); // SPI初始化
#endif
    memset(ipcSendBuffer, 0, IPC_BUFFER_SIZE);
}

/**
 * @description: Pico Link SPI发送图像到致用上位机
 * @param {pico_uint8} *pImage 图像指针
 * @param {pico_uint32} imageH 图像高度
 * @param {pico_uint32} imageW 图像宽度
 * @return {*}
 */
void ipcSendImage_Spi(pico_uint8 *pImage, pico_uint32 imageH, pico_uint32 imageW)
{
    pico_uint8 i;
    pico_uint8 overLength = 4 - (6 + imageH * imageW) % 4;
    memcpy((ipcSendBuffer + 0), "CSU", 3);
    memcpy((ipcSendBuffer + 3), pImage, imageH * imageW);
    memcpy((ipcSendBuffer + 3 + imageH * imageW), "USC", 3);
    for (i = 0; i < overLength; i++)
    {
        ipcSendBuffer[imageH * imageW + 6 + i] = (pico_uint8)'*';
    }
#if (defined DEVICE_TC264) || (defined DEVICE_RT1064)
    spi_mosi(SPI_NUM, SPI_CS_PIN, ipcSendBuffer, NULL, (6 + imageH * imageW + overLength), 1);
#elif (defined DEVICE_MM32)
    spi_mosi(SPI_NUM, ipcSendBuffer, NULL, (6 + imageH * imageW + overLength));
#endif
}