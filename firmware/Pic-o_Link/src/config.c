/*
 * @Author: Wanderer
 * @Date: 2022-03-24 18:05:03
 * @LastEditTime: 2022-03-26 10:58:31
 * @LastEditors: Please set LastEditors
 * @Description: 配置驱动
 * @FilePath: \UART\src\config.c
 */
#include "config.h"

#define CONFIG_PIN_1 (13)
#define CONFIG_PIN_2 (15)

#define GPIO_OUTPUT_PIN_SEL ((1ULL << CONFIG_PIN_1))
#define GPIO_INPUT_PIN_SEL ((1ULL << CONFIG_PIN_2))

picoConfig_t picoConfig = {
    .protocol = PROTOCOL_UART,
    .uartSpeed = 1500000,
    .uartRxBuffer = 5400,
    .SSID = "Best Match",
    .PWD = "88888888",
    .hostIP = "192.168.137.1",
    .port = 2333,
}; //默认初始化参数

static const char *TAG = "config";

/**
 * @description: 向Flash写入参数
 * @param {*}
 * @return {*}
 */
void writeFlashConfig(void)
{
    // ESP_LOGI(TAG, "write flash");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        // ESP_LOGI(TAG, "open done\n");
        ESP_ERROR_CHECK(nvs_set_u8(my_handle, "protocol", picoConfig.protocol));
        ESP_ERROR_CHECK(nvs_set_u32(my_handle, "uartSpeed", picoConfig.uartSpeed));
        ESP_ERROR_CHECK(nvs_set_u32(my_handle, "uartRxBuffer", picoConfig.uartRxBuffer));
        ESP_ERROR_CHECK(nvs_set_str(my_handle, "SSID", picoConfig.SSID));
        ESP_ERROR_CHECK(nvs_set_str(my_handle, "PWD", picoConfig.PWD));
        ESP_ERROR_CHECK(nvs_set_str(my_handle, "hostIP", picoConfig.hostIP));
        ESP_ERROR_CHECK(nvs_set_u16(my_handle, "port", picoConfig.port));
        uart_write_bytes(UART_NUM_0, "DONE", 5); //回报写进成功
        // ESP_LOGI(TAG, "write done\n");
    }
    // Close
    nvs_close(my_handle);
}

/**
 * @description: 读取Flash设置参数
 * @param {*}
 * @return {*}
 */
void readFlashConfig(void)
{
    ESP_LOGI(TAG, "read flash");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, "open done\n");
        err = nvs_get_u8(my_handle, "protocol", &picoConfig.protocol);
        if (err == ESP_OK)
        {
            ESP_ERROR_CHECK(nvs_get_u32(my_handle, "uartSpeed", &picoConfig.uartSpeed));
            ESP_ERROR_CHECK(nvs_get_u32(my_handle, "uartRxBuffer", &picoConfig.uartRxBuffer));
            size_t length = 32;
            ESP_ERROR_CHECK(nvs_get_str(my_handle, "SSID", picoConfig.SSID, &length));
            length = 64;
            ESP_ERROR_CHECK(nvs_get_str(my_handle, "PWD", picoConfig.PWD, &length));
            length = 16;
            ESP_ERROR_CHECK(nvs_get_str(my_handle, "hostIP", picoConfig.hostIP, &length));
            ESP_ERROR_CHECK(nvs_get_u16(my_handle, "port", &picoConfig.port));
        }
        else if (err == ESP_ERR_NVS_NOT_FOUND) //值没有初始化
        {
            ESP_LOGE(TAG, "The value is not initialized yet!\n");
            ESP_ERROR_CHECK(nvs_set_u8(my_handle, "protocol", picoConfig.protocol));
            ESP_ERROR_CHECK(nvs_set_u32(my_handle, "uartSpeed", picoConfig.uartSpeed));
            ESP_ERROR_CHECK(nvs_set_u32(my_handle, "uartRxBuffer", picoConfig.uartRxBuffer));
            ESP_ERROR_CHECK(nvs_set_str(my_handle, "SSID", picoConfig.SSID));
            ESP_ERROR_CHECK(nvs_set_str(my_handle, "PWD", picoConfig.PWD));
            ESP_ERROR_CHECK(nvs_set_str(my_handle, "hostIP", picoConfig.hostIP));
            ESP_ERROR_CHECK(nvs_set_u16(my_handle, "port", picoConfig.port));
            ESP_LOGI(TAG, "initial write done\n");
        }
        else
        {
            ESP_LOGE(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
        }
        ESP_LOGI(TAG, "read done\n");
    }
    // Close
    nvs_close(my_handle);
}

/**
 * @description: 配置模式
 * @param {*}
 * @return {*}
 */
void configMode(void)
{
    setRgbLevel(0, 0, 1); //指示灯变黄
    ESP_LOGI(TAG, "enter config mode");
    char *dataPtr = (char *)malloc(100);
    memset(dataPtr, 0, 100);
    while (1)
    {
        uart_read_bytes(UART_NUM_0, dataPtr, 1, portMAX_DELAY); //一个字节一个字节地读
        if (dataPtr[0] == 0x41)                                 //设置通信协议
        {
            uart_read_bytes(UART_NUM_0, dataPtr + 1, 1, portMAX_DELAY);
            picoConfig.protocol = (uint8_t)(*(dataPtr + 1));
        }
        else if (dataPtr[0] == 0x42) //设置UART波特率
        {
            uart_read_bytes(UART_NUM_0, dataPtr + 1, 4, portMAX_DELAY);
            uint32_t n1 = (*(dataPtr + 1)) << 24;
            uint32_t n2 = (*(dataPtr + 2)) << 16;
            uint32_t n3 = (*(dataPtr + 3)) << 8;
            uint32_t n4 = *(dataPtr + 4);
            picoConfig.uartSpeed = n1 + n2 + n3 + n4;
        }
        else if (dataPtr[0] == 0x43) //设置UART接收缓冲大小
        {
            uart_read_bytes(UART_NUM_0, dataPtr + 1, 2, portMAX_DELAY);
            uint32_t n1 = (*(dataPtr + 1)) << 8;
            uint32_t n2 = *(dataPtr + 2);
            picoConfig.uartRxBuffer = n1 + n2;
        }
        else if (dataPtr[0] == 0x44) //设置WiFi账号
        {
            int len = uart_read_bytes(UART_NUM_0, dataPtr + 1, 32, 20 / portTICK_PERIOD_MS);
            memset(picoConfig.SSID, 0, 32);
            memcpy(picoConfig.SSID, dataPtr + 1, len);
        }
        else if (dataPtr[0] == 0x45) //设置WiFi密码
        {
            int len = uart_read_bytes(UART_NUM_0, dataPtr + 1, 64, 20 / portTICK_PERIOD_MS);
            memset(picoConfig.PWD, 0, 64);
            memcpy(picoConfig.PWD, dataPtr + 1, len);
        }
        else if (dataPtr[0] == 0x46) //设置udp server IP地址
        {
            int len = uart_read_bytes(UART_NUM_0, dataPtr + 1, 16, 20 / portTICK_PERIOD_MS);
            memset(picoConfig.hostIP, 0, 16);
            memcpy(picoConfig.hostIP, dataPtr + 1, len);
        }
        else if (dataPtr[0] == 0x47) //设置udp server 端口
        {
            uart_read_bytes(UART_NUM_0, dataPtr + 1, 2, portMAX_DELAY);
            uint16_t n1 = (*(dataPtr + 1)) << 8;
            uint16_t n2 = *(dataPtr + 2);
            picoConfig.port = n1 + n2;
        }
        else if (dataPtr[0] == 0x48) //查询参数
        {
            ESP_LOGI(TAG, "\n%d\n%d\n%d\n%s\n%s\n%s\n%d\n", picoConfig.protocol, picoConfig.uartSpeed, picoConfig.uartRxBuffer, picoConfig.SSID, picoConfig.PWD, picoConfig.hostIP, picoConfig.port);
        }
        else if (dataPtr[0] == 0x49) //写入参数
        {
            writeFlashConfig();
        }
        memset(dataPtr, 0, 100); //清空读下一条指令
        uart_flush(UART_NUM_0);
    }
}

/**
 * @description: 配置初始化
 * @param {*}
 * @return {*}
 */
void configInit(void)
{
    readFlashConfig(); //读取参数
    //输出引脚配置
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
    gpio_set_level(CONFIG_PIN_1, 0); //设置输出低电平

    //输入引脚配置
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    if (gpio_get_level(CONFIG_PIN_2) == 0)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        //引脚13、15如果短接则进入配置模式
        if (gpio_get_level(CONFIG_PIN_2) == 0)
        {
            configMode();
        }
    }
    //重置配置引脚，进入工作模式
    gpio_reset_pin(CONFIG_PIN_1);
    gpio_reset_pin(CONFIG_PIN_2);
}
