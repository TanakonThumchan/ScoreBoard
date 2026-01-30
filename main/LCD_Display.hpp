#include "u8g2.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class LCD_Display
{
private:
    TaskHandle_t _taskHandle = nullptr;
    u8g2_t _u8g2;
    gpio_num_t _dataPin;
    gpio_num_t _clockPin;

    static void taskTrampoline(void *arg);
    void taskLoop();

public:
    LCD_Display(gpio_num_t data, gpio_num_t clock);
    ~LCD_Display();
    void start();
    void showText(uint16_t x, uint16_t y, char *text);
};