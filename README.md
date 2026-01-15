# RF Remote Controlled Scoreboard (ESP8266) - (Ongoing)

A digital scoreboard powered by **ESP8266** and **FreeRTOS**, designed to be controlled via a 433MHz RF remote. This project drives WS2812B LED strips to display scores and utilizes an IO expander for physical inputs, operating completely offline (No Wi-Fi) to maximize real-time performance.

## 🚀 Features

* **Real-Time OS:** Built on Espressif ESP8266_RTOS_SDK (FreeRTOS).
* **RF Control:** Decodes 433MHz signals (RX470/WL102) for wireless score updates.
* **High Performance:** CPU clocked at 160MHz to handle RF interrupts and LED timing.
* **Graphics:** Uses the **U8g2** library with a custom C++ wrapper and Hardware Abstraction Layer (HAL).
* **Expanded I/O:** Integrates PCF8575 via I2C for additional physical buttons.
* **WiFi-Free:** WiFi radio is disabled to save RAM and ensure deterministic timing for the RF receiver.

## 🛠 Hardware Required

* **MCU:** ESP8266 (NodeMCU v2/v3 or ESP-12E).
* **Display:** WS2812B Addressable LED Strip (7-Segment arrangement).
* **RF Module:** 433MHz Receiver (RX470-4 or WL102-341).
* **IO Expander:** PCF8575 (I2C).
* **Power:** 5V Power Supply (Amperage depends on LED count).
* **Misc:** Logic Level Shifter (3.3V to 5V) for the LED data line.

## 🔌 Pinout Configuration

| Component | Pin Name | ESP8266 GPIO |
| :--- | :--- | :--- |
| **I2C SCL** | D6 | GPIO 12 |
| **I2C SDA** | D5 | GPIO 14 |
| **RF Data** | D7 | GPIO 13 |
| **LED Data** | RX | GPIO 3 |

## 💻 Software & Build

### Prerequisites
* **ESP8266_RTOS_SDK** (v3.4 or newer).
* **Xtensa Toolchain** (gcc/g++).
* **Make** or CMake.

### Project Structure
* `main/`: Core application logic (C++).
* `components/u8g2/`: Graphics library (cloned and stripped of .git).
* `u8g2_esp8266_hal`: Custom HAL to bridge U8g2 with ESP8266 GPIOs.

### Build Instructions

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/TanakonThumchan/ESP8266-RF-Remote-Scoreboard.git
    cd ScoreBoard
    ```

2.  **Configure the project:**
    ```bash
    make menuconfig
    ```
    * *Serial Flasher Config:* Set Flash Size to **4MB** and Mode to **DIO**.
    * *Component config:* Set CPU Frequency to **160 MHz**.

3.  **Build and Flash:**
    ```bash
    make flash monitor
    ```

## License & Credits

This project is open source and available under the **MIT License** (see the [LICENSE](LICENSE) file for details).

### Third-Party Components
This project makes use of the following open-source libraries and components:

* **ESP8266 RTOS SDK** by [Espressif Systems](https://github.com/espressif/ESP8266_RTOS_SDK)
    * Official development framework for ESP8266.
    * License: [Apache License 2.0](https://github.com/espressif/ESP8266_RTOS_SDK/blob/master/LICENSE)

* **U8g2** by [olikraus](https://github.com/olikraus/u8g2)
    * Universal 8-bit Graphics Library for embedded systems.
    * License: [2-Clause BSD License](https://github.com/olikraus/u8g2/blob/master/LICENSE)

* **rc-switch** by [sui77](https://github.com/sui77/rc-switch)
    * Library for operating low cost 433MHz/315MHz remote control mains switches.
    * License: LGPL-2.1 (Check component header for specific details).
    * **Note:** Adapted for ESP8266 RTOS SDK usage via a custom `WProgram.h` compatibility shim (no changes to core library logic).

* **WS2812 I2S Driver** by [esp8266-setup](https://github.com/esp8266-setup/ws2812_i2s)
    * I2S based driver for WS2812B leds on ESP8266.
    * License: [MIT License](https://github.com/esp8266-setup/ws2812_i2s/blob/master/LICENSE-MIT.txt) / [BSD License](https://github.com/esp8266-setup/ws2812_i2s/blob/master/LICENSE-BSD.txt) (Dual licensed based on ESP Open RTOS derivation).