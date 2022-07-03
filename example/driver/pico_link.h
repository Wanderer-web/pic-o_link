#ifndef _PICO_LINK_H__
#define _PICO_LINK_H__

//数据类型声明
typedef unsigned char pico_uint8;
typedef unsigned short int pico_uint16;
typedef unsigned long int pico_uint32;

void picoLinkInit_Uart(void);
void picoLinkSendByte_Uart(pico_uint8 data);
void picoLinkSendBytes_Uart(pico_uint8 *data, pico_uint32 length);

void picoLinkInit_Spi(void);

void ipcSendImage_Uart(pico_uint8 *pImage, pico_uint32 imageH, pico_uint32 imageW);
void ipcSendImage_Spi(pico_uint8 *pImage, pico_uint32 imageH, pico_uint32 imageW);

#endif