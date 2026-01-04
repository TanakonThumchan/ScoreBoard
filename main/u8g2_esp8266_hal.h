#ifndef U8G2_ESP8266_HAL_H
#define U8G2_ESP8266_HAL_H

#include "u8g2.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

// Definisci qui i tuoi PIN per il display I2C
#define I2C_SCL_IO           GPIO_NUM_12   
#define I2C_SDA_IO           GPIO_NUM_14   

#ifdef __cplusplus
extern "C" {
#endif

// Questa è la funzione callback che gestisce GPIO e ritardi
uint8_t u8g2_esp8266_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

// Questa è la funzione callback per mandare i byte via I2C Hardware (opzionale, ma veloce)
uint8_t u8g2_esp8266_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#ifdef __cplusplus
}
#endif

#endif // U8G2_ESP8266_HAL_H