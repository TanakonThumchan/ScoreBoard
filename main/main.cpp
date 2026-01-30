#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "u8g2.h"
#include "ws2812_i2s.h"
#include "RCSwitch.h"
#include "LCD_Display.hpp"
#include "ScoreBoard.hpp"

#define LED_PER_SEGMENT 3
#define PIXEL_COUNT (7 * LED_PER_SEGMENT * 4)
#define RF_PIN GPIO_NUM_13

#define I2C_SCL_IO GPIO_NUM_12
#define I2C_SDA_IO GPIO_NUM_14

u8g2_t u8g2;
RCSwitch rf_receiver = RCSwitch();

void displayTest_task(void *pvParameters)
{
    LCD_Display *display = new LCD_Display(I2C_SDA_IO, I2C_SCL_IO);
    static int contatore = 0;
    char buffer[16];
    while (1)
    {
        snprintf(buffer, sizeof(buffer), "TESTING: %d", contatore++);
        display->showText(5, 15, buffer);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void led_task(void *pvParameters)
{
    ws2812_i2s_init(PIXEL_COUNT);

    ws2812_pixel_t pixels[PIXEL_COUNT];

    while (1)
    {
        // setPixel(pixels,);
        for (int i = 0; i < PIXEL_COUNT; i++)
        {
            pixels[i].red = 255;
            pixels[i].green = 0;
            pixels[i].blue = 0;
        }

        ws2812_i2s_update(pixels);
        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

void rf_task(void *pvParameters)
{
    unsigned long msg;
    rf_receiver.enableReceive(0);

    while (1)
    {
        if (rf_receiver.available() && rf_receiver.getReceivedBitlength() >= 8)
        {
            msg = rf_receiver.getReceivedValue();

            rf_receiver.resetAvailable();
        }
        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

static void i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;

    conf.clk_stretch_tick = 300;

    i2c_driver_install(I2C_NUM_0, conf.mode);
    i2c_param_config(I2C_NUM_0, &conf);
}

extern "C" void app_main()
{
    i2c_master_init();
    ScoreBoard::getInstance().init(); 

    // xTaskCreate(led_task, "LedTask", 2048, NULL, 2, NULL);
    // xTaskCreate(rf_task, "RFTask", 1024, NULL, 2, NULL);
    // xTaskCreate(display_task, "DisplayTask", 2048, NULL, 2, NULL);
    //xTaskCreate(displayTest_task, "DisplayTask", 2048, NULL, 2, NULL);

    LCD_Display *myDisplay = new LCD_Display(I2C_SDA_IO, I2C_SCL_IO);
    myDisplay->start();
}