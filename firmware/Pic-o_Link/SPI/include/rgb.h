/*
 * @Author: Wanderer
 * @Date: 2022-03-12 16:53:40
 * @LastEditTime: 2022-03-16 19:52:16
 * @LastEditors: Please set LastEditors
 * @Description: 
 * @FilePath: \UART\include\rgb.h
 */
#ifndef _RGB_H__
#define _RGB_H__

#include "headfile.h"

void rgbInit(void);
void setRgbLevel(uint8_t rLevel, uint8_t gLevel, uint8_t bLevel);

#endif