#include "LCD_Display.hpp"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2.h"
#include "ScoreBoard.hpp"

static uint8_t u8g2_esp8266_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
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
        return 0; // Message not handled
    }
    return 1;
}

static uint8_t u8g2_esp8266_i2c_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
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

LCD_Display::LCD_Display(gpio_num_t data, gpio_num_t clock) : _dataPin(data), _clockPin(clock)
{
    // Setup U8g2
    // u8g2_Setup_ssd1306_i2c_128x64_noname_f
    // SSD1306 = Display's chip
    // 128x64  = Resolution
    // noname  = Generic driver
    // f       = Full buffer (faster, use 1024 bytes of RAM)
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &_u8g2,
        U8G2_R0,                       // Rotation(R0, R1, R2, R3)
        u8g2_esp8266_i2c_byte_cb,      // Hardware I2C callback
        u8g2_esp8266_gpio_and_delay_cb // GPIO and delay callback
    );

    u8g2_InitDisplay(&_u8g2);     // send init sequence to the display, display is in sleep mode after this
    u8g2_SetPowerSave(&_u8g2, 0); // wake up display
}

LCD_Display::~LCD_Display()
{
    if (_taskHandle != nullptr)
    {
        vTaskDelete(_taskHandle);
    }
}

void LCD_Display::start()
{
    xTaskCreate(taskTrampoline, "DisplayTask", 1000, this, 2, &_taskHandle);
}

void LCD_Display::taskTrampoline(void *arg)
{
    LCD_Display *self = static_cast<LCD_Display *>(arg);

    self->taskLoop();

    vTaskDelete(NULL);
}

void LCD_Display::taskLoop()
{
    const char *label1 = "TEAM1:";
    const char *label2 = "TEAM2:";
    char buffer[5];
    GameState currentGame = {0, 0, 0, 0};

    u8g2_SetFont(&_u8g2, u8g2_font_ncenB08_tr);
    int widthLabel1 = u8g2_GetStrWidth(&_u8g2, label1);
    int widthLabel2 = u8g2_GetStrWidth(&_u8g2, label2);

    int xLabel1 = 5;
    int xLabel2 = 128 - widthLabel2 - 5;

    while (true)
    {
        currentGame = ScoreBoard::getInstance().getGameState();
        u8g2_ClearBuffer(&_u8g2);

        u8g2_DrawFrame(&_u8g2, 0, 0, 128, 64);

        u8g2_DrawStr(&_u8g2, xLabel1, 12, label1);
        u8g2_DrawStr(&_u8g2, xLabel2, 12, label2);

        snprintf(buffer, sizeof(buffer), "%02d", currentGame.teamOneScore);
        int widthBuffer = u8g2_GetStrWidth(&_u8g2, buffer);
        int xScore1 = xLabel1 + (widthLabel1 / 2) - (widthBuffer / 2);
        u8g2_DrawStr(&_u8g2, xScore1, 30, buffer);

        snprintf(buffer, sizeof(buffer), "%02d", currentGame.teamTwoScore);
        widthBuffer = u8g2_GetStrWidth(&_u8g2, buffer);
        int xScore2 = xLabel2 + (widthLabel2 / 2) - (widthBuffer / 2);
        u8g2_DrawStr(&_u8g2, xScore2, 30, buffer);

        u8g2_SendBuffer(&_u8g2);

        UBaseType_t stackMinimoLibero = uxTaskGetStackHighWaterMark(NULL);

        printf("[LCD Task] Minimo Stack Libero: %d bytes\n", (int)stackMinimoLibero);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void LCD_Display::showText(uint16_t x, uint16_t y, char *text)
{
    u8g2_ClearBuffer(&_u8g2);
    u8g2_DrawFrame(&_u8g2, 0, 0, 128, 64);

    u8g2_SetFont(&_u8g2, u8g2_font_ncenB08_tr);
    // u8g2_DrawStr(&u8g2, 5, 20, text);
    u8g2_DrawStr(&_u8g2, x, y, text);

    u8g2_SendBuffer(&_u8g2);
}
