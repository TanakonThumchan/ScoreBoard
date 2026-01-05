#ifndef WPROGRAM_H
#define WPROGRAM_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ESP8266 RTOS SDK specific includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h" 
#include "rom/ets_sys.h" // For ets_delay_us

#define ESP8266

// --- Arduino Data Types ---
typedef uint8_t byte;
typedef bool boolean;

// --- Constants Mapping ---
#define LOW  0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1

// Interrupt Modes (Mapped to ESP GPIO interrupt types)
#define RISING  GPIO_INTR_POSEDGE
#define FALLING GPIO_INTR_NEGEDGE
#define CHANGE  GPIO_INTR_ANYEDGE

#ifndef RECEIVE_ATTR
#define RECEIVE_ATTR IRAM_ATTR
#endif

// --- Shim Functions ---

// Get microseconds since boot
static inline unsigned long micros() {
    return (unsigned long)esp_timer_get_time();
}

// Delay in milliseconds (Non-blocking for RTOS)
static inline void delay(unsigned long ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

// Delay in microseconds (Blocking, precise)
static inline void delayMicroseconds(unsigned int us) {
    ets_delay_us(us);
}

// Read GPIO state
static inline int digitalRead(int pin) {
    return gpio_get_level((gpio_num_t)pin);
}

// Write GPIO state
static inline void digitalWrite(int pin, int val) {
    gpio_set_level((gpio_num_t)pin, val);
}

// Configure GPIO mode
static inline void pinMode(int pin, int mode) {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = (mode == OUTPUT) ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
}

// --- Interrupt Handling ---

/**
 * @brief Simulates Arduino attachInterrupt.
 * * Note: You MUST call gpio_install_isr_service(0) in your app_main() 
 * before using this function, otherwise it will fail or crash.
 * * @param pin The GPIO number
 * @param handler The callback function (ISR)
 * @param mode The trigger mode (RISING, FALLING, CHANGE)
 */
static inline void attachInterrupt(int pin, void (*handler)(void), int mode) {
    static bool isr_service_installed = false;
    if (!isr_service_installed) {
        gpio_install_isr_service(0);
        isr_service_installed = true;
    }

    // Set the interrupt trigger type
    gpio_set_intr_type((gpio_num_t)pin, (gpio_int_type_t)mode);    

    // Register the handler. 
    // We cast the handler because ESP SDK expects void(*)(void*) but Arduino uses void(*)(void).
    // Since RCSwitch handlers don't use arguments, this is safe here.
    gpio_isr_handler_add((gpio_num_t)pin, (gpio_isr_t)handler, NULL);
}

static inline void detachInterrupt(int pin) {
    gpio_set_intr_type((gpio_num_t)pin, GPIO_INTR_DISABLE);
    gpio_isr_handler_remove((gpio_num_t)pin);
}

#endif // WPROGRAM_H