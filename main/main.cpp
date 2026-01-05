#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "u8g2.h"
#include "u8g2_esp8266_hal.h"
#include "ws2812_i2s.h"
#include "RCSwitch.h"

#define PIXEL_COUNT (7 * 3 * 4)
u8g2_t u8g2;
RCSwitch rf_receiver = RCSwitch();

extern "C"
{
    void app_main(void);
}

void display_task(void *pvParameters)
{
    // Setup U8g2
    // IMPORTANTE: Scegli il driver giusto per il tuo display!
    // u8g2_Setup_ssd1306_i2c_128x64_noname_f
    // SSD1306 = Chip del display
    // 128x64  = Risoluzione
    // noname  = Driver generico
    // f       = Full buffer (più veloce, usa 1024 byte di RAM)
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2,
        U8G2_R0,                       // Rotazione (R0, R1, R2, R3)
        u8x8_byte_sw_i2c,              // Usa il bit-banging software incluso in U8g2
        u8g2_esp8266_gpio_and_delay_cb // Usa la nostra funzione callback definita sopra
    );

    u8g2_InitDisplay(&u8g2);     // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    while (1)
    {
        u8g2_ClearBuffer(&u8g2);

        u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);

        u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr);
        u8g2_DrawStr(&u8g2, 5, 20, "GPIO 14 & 12");
        printf("GPIO 14 & 12\n");

        u8g2_SetFont(&u8g2, u8g2_font_6x10_tr);
        u8g2_DrawStr(&u8g2, 5, 45, "Funziona VERO!");
        printf("Funziona!\n");

        u8g2_SendBuffer(&u8g2);

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void led_task(void *pvParameters)
{
    ws2812_i2s_init(PIXEL_COUNT);

    // Crea un array di pixel (struttura definita dalla lib)
    ws2812_pixel_t pixels[PIXEL_COUNT];

    while (1)
    {
        // Imposta tutto rosso
        for (int i = 0; i < PIXEL_COUNT; i++)
        {
            pixels[i].red = 255;
            pixels[i].green = 0;
            pixels[i].blue = 0;
        }

        // Manda i dati (Non bloccante grazie al DMA!)
        ws2812_i2s_update(pixels);
        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

void app_main()
{
    printf("Hello world!\n");

    xTaskCreate(led_task, "LedTask", 2048, NULL, 2, NULL);

    xTaskCreate(display_task, "DisplayTask", 2048, NULL, 2, NULL);
}
