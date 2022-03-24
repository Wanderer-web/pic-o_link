/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.28
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 ********************************************************************************************************************/

//整套推荐IO查看Projecct文件夹下的TXT文本

//打开新的工程或者工程移动了位置务必执行以下操作
//第一步 关闭上面所有打开的文件
//第二步 project  clean  等待下方进度条走完

//下载代码前请根据自己使用的下载器在工程里设置下载器为自己所使用的

#include "headfile.h"

#define SPI_NUM SPI_4
#define SPI_SCK_PIN SPI4_SCK_C23   //接模块SPC
#define SPI_MOSI_PIN SPI4_MOSI_C22 //接模块SDI
#define SPI_MISO_PIN SPI4_MISO_C21 //接模块SDO
#define SPI_CS_PIN SPI4_CS0_C20    //接模块CS

uint16 trans_ms = 0;

int main(void)
{
    DisableGlobalIRQ();
    board_init(); //务必保留，本函数用于初始化MPU 时钟 调试串口

    //此处编写用户代码(例如：外设初始化代码等)
    spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_CS_PIN, 0, 7 * 1000 * 1000); //硬件SPI初始化，波特率5M
    mt9v03x_csi_init();
    ips114_init();
    systick_delay_ms(1000);
    pit_init();
    NVIC_SetPriority(PIT_IRQn, 10);
    pit_interrupt_ms(PIT_CH0, 30);
    //总中断最后开启
    EnableGlobalIRQ(0);
    while (1)
    {
        if (mt9v03x_csi_finish_flag)
        {
            ips114_showuint16(0, 0, (uint16)trans_ms);
            mt9v03x_csi_finish_flag = 0;
        }
    }
}
