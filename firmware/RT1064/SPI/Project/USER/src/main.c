/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2019,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ��һȺ��179029047(����)  ��Ⱥ��244861897
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		main
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		IAR 8.3 or MDK 5.28
 * @Target core		NXP RT1064DVL6A
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 ********************************************************************************************************************/

//�����Ƽ�IO�鿴Projecct�ļ����µ�TXT�ı�

//���µĹ��̻��߹����ƶ���λ�����ִ�����²���
//��һ�� �ر��������д򿪵��ļ�
//�ڶ��� project  clean  �ȴ��·�����������

//���ش���ǰ������Լ�ʹ�õ��������ڹ���������������Ϊ�Լ���ʹ�õ�

#include "headfile.h"

#define SPI_NUM SPI_4
#define SPI_SCK_PIN SPI4_SCK_C23   //��ģ��SPC
#define SPI_MOSI_PIN SPI4_MOSI_C22 //��ģ��SDI
#define SPI_MISO_PIN SPI4_MISO_C21 //��ģ��SDO
#define SPI_CS_PIN SPI4_CS0_C20    //��ģ��CS

uint16 trans_ms = 0;

int main(void)
{
    DisableGlobalIRQ();
    board_init(); //��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���

    //�˴���д�û�����(���磺�����ʼ�������)
    spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_CS_PIN, 0, 7 * 1000 * 1000); //Ӳ��SPI��ʼ����������5M
    mt9v03x_csi_init();
    ips114_init();
    systick_delay_ms(1000);
    pit_init();
    NVIC_SetPriority(PIT_IRQn, 10);
    pit_interrupt_ms(PIT_CH0, 30);
    //���ж������
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
