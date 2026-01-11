#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2_esp8266_hal.h"
#include "driver/i2c.h"

uint8_t u8g2_esp8266_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        // HARDWARE I2C MODE:
        // Do NOT configure SDA or SCL pins here.
        // They are already configured by i2c_driver_install() in main.c.
        break;

    case U8X8_MSG_DELAY_MILLI:
        // U8g2 asks for a delay in milliseconds
        vTaskDelay(arg_int / portTICK_PERIOD_MS);
        break;

    case U8X8_MSG_DELAY_10MICRO:
        // Using ets_delay_us (blocking) for short precise delays
        ets_delay_us(arg_int * 10);
        break;

    case U8X8_MSG_DELAY_100NANO:
        // Usually not needed for I2C, but good to have
        ets_delay_us(1);
        break;

    case U8X8_MSG_GPIO_I2C_CLOCK:
    case U8X8_MSG_GPIO_I2C_DATA:
        break;

    default:
        return 0; // Messaggio non gestito
    }
    return 1;
}

uint8_t u8g2_esp8266_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static i2c_cmd_handle_t i2c_handler;
    uint8_t *data;

    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            i2c_master_write_byte(i2c_handler, *data, true);
            data++;
            arg_int--;
        }
        break;

    case U8X8_MSG_BYTE_INIT:
        // i2c_init(u8x8);
        break;
    case U8X8_MSG_BYTE_SET_DC:
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        i2c_handler = i2c_cmd_link_create();
        i2c_master_start(i2c_handler);
        i2c_master_write_byte(i2c_handler, u8x8_GetI2CAddress(u8x8) | I2C_MASTER_WRITE, true);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        i2c_master_stop(i2c_handler);
        i2c_master_cmd_begin(I2C_NUM_0, i2c_handler, 1000 / portTICK_RATE_MS);
        i2c_cmd_link_delete(i2c_handler);
        break;
    default:
        return 0;
    }
    return 1;
}