/*
 * @Author: Wanderer
 * @Date: 2022-03-12 16:53:22
 * @LastEditTime: 2022-03-12 17:15:58
 * @LastEditors: Please set LastEditors
 * @Description: rgb灯驱动
 * @FilePath: \UART\src\rgb.c
 */

#include "rgb.h"

#define RGB_G_PIN 5  //绿色通道
#define RGB_B_PIN 18 //蓝色通道
#define RGB_R_PIN 23 //红色通道

#define GPIO_OUTPUT_PIN_SEL ((1ULL << RGB_G_PIN) | (1ULL << RGB_B_PIN) | (1ULL << RGB_R_PIN))

/**
 * @description: rgb灯控制引脚初始化
 * @param {*}
 * @return {*}
 */
void rgbInit(void)
{
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_set_level(RGB_G_PIN, 1);
    gpio_set_level(RGB_B_PIN, 1);
    gpio_set_level(RGB_R_PIN, 1);
}

/**
 * @description: 设定rgb灯引脚电平
 * @param {uint8_t} rLevel 0打开，1关闭
 * @param {uint8_t} gLevel 0打开，1关闭
 * @param {uint8_t} bLevel 0打开，1关闭
 * @return {*}
 */
void setRgbLevel(uint8_t rLevel, uint8_t gLevel, uint8_t bLevel)
{
    gpio_set_level(RGB_R_PIN, rLevel);
    gpio_set_level(RGB_G_PIN, gLevel);
    gpio_set_level(RGB_B_PIN, bLevel);
}
