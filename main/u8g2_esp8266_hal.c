#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2_esp8266_hal.h"

// Funzione Callback per gestire Ritardi e controllo PIN
uint8_t u8g2_esp8266_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        // Inizializza i GPIO. 
        // Nota: Se usi I2C Software, U8g2 gestisce i pin, ma noi li configuriamo come output per sicurezza.
        {
            gpio_config_t io_conf;
            io_conf.intr_type = GPIO_INTR_DISABLE;
            io_conf.mode = GPIO_MODE_OUTPUT;
            io_conf.pin_bit_mask = (1ULL << I2C_SCL_IO) | (1ULL << I2C_SDA_IO);
            io_conf.pull_down_en = 0;
            io_conf.pull_up_en = 1; // Pullup necessario per I2C
            gpio_config(&io_conf);
        }
        break;

    case U8X8_MSG_DELAY_MILLI:
        // Ritardo in millisecondi. Usiamo vTaskDelay per non bloccare RTOS.
        // vTaskDelay aspetta tick, quindi convertiamo.
        vTaskDelay(arg_int / portTICK_PERIOD_MS);
        break;

    case U8X8_MSG_DELAY_10MICRO:
        // Ritardo microsecondi (bloccante, ma brevissimo)
        ets_delay_us(arg_int * 10);
        break;

    case U8X8_MSG_DELAY_100NANO:
        // Ritardo nanosecondi (bloccante)
        ets_delay_us(1); // Minimo ritardo possibile
        break;

    case U8X8_MSG_GPIO_I2C_CLOCK:
        // Imposta il pin Clock (SCL)
        // arg_int contiene il livello (0 o 1)
        gpio_set_level(I2C_SCL_IO, arg_int);
        break;

    case U8X8_MSG_GPIO_I2C_DATA:
        // Imposta il pin Data (SDA)
        gpio_set_level(I2C_SDA_IO, arg_int);
        break;

    default:
        return 0; // Messaggio non gestito
    }
    return 1;
}