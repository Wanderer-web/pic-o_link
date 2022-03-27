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

uint16 trans_ms = 0;

int main(void)
{
    DisableGlobalIRQ();
    board_init(); //��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���

    //�˴���д�û�����(���磺�����ʼ�������)
    seekfree_wireless_init();
    mt9v03x_csi_init();
    ips114_init();
    pit_init();

    systick_delay_ms(1000);

    //���ж������
    EnableGlobalIRQ(0);
    while (1)
    {
        //�˴���д��Ҫѭ��ִ�еĴ���
        if (mt9v03x_csi_finish_flag)
        {
            pit_start(PIT_CH1);
            seekfree_wireless_send_buff(mt9v03x_csi_image[0], 90 * 60);
            trans_ms = (uint16)(pit_get_ms(PIT_CH1));
            pit_close(PIT_CH1);
            mt9v03x_csi_finish_flag = 0;
            ips114_showuint16(0, 0, (uint16)trans_ms);
        }
    }
}
