# STM32 Biometric Pulse Monitor with IoT 

A highly responsive, real-time pulse monitoring system built on the **STM32F4 platform** using the **MAX30102 Pulse Oximeter** sensor and an **SSD1306 I2C OLED display**. 

This firmware is engineered using an asynchronous, non-blocking polling architecture to ensure real-time data ingestion while driving a smooth, aesthetic heart-rate animation layout on a shared I2C bus.

---

## 🚀 Features

* **Real-Time Data Ingestion:** Pure non-blocking `HAL_GetTick()` scheduling eliminates lag and data dropping caused by traditional `HAL_Delay()` routines.
* **Anti-Lag Safety Filter:** Actively monitors the MAX30102 hardware FIFO pointer stack to flush backlogged or stale data frames instantly upon finger detection.
* **Aesthetic UI Display:** Features an animated, dual-state beating heart (Massive 24x22 pixel active frame vs. 16x15 pixel recoil frame) synchronized with live peak-detection indicators.
* **Moving Average Filter:** Smooths pulse variations using a rolling 4-beat mathematical window to eliminate rogue spikes and display reliable BPM values.

---

## 🛠️ Hardware Components

* **Microcontroller:** STM32F4 Series (ARM Cortex-M4)
* **Sensor:** MAX30102 High-Sensitivity Pulse Oximeter & Heart-Rate Sensor
* **Display:** 128x64 Pixel SSD1306 OLED Screen (I2C Mode)
* **Future Extension:** Wi-Fi Module (ESP8266 / ESP32 via UART AT Commands or SPI)

### I2C Pin Configuration (Shared Bus)
| STM32 Peripheral | Hardware Pin | Target Device |
| :--- | :--- | :--- |
| **I2C1_SCL** | PB8 | SSD1306 SCL / MAX30102 SCL |
| **I2C1_SDA** | PB9 | SSD1306 SDA / MAX30102 SDA |
| **USART2_TX** | PA2 | ST-Link Virtual COM Port (115200 Baud) |
| **USART2_RX** | PA3 | ST-Link Virtual COM Port (115200 Baud) |

---

## 📂 Repository Structure

To clone and compile this project, you only need the core software configurations and driver architectures included in this layout:

```text
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── max30102_for_stm32_hal.h
│   │   ├── ssd1306.h
│   │   └── ssd1306_fonts.h
│   └── Src/
│       ├── main.c              <-- Core execution engine & UI Loop
│       ├── max30102_for_stm32_hal.c
│       ├── ssd1306.c
│       └── ssd1306_fonts.c
├── .cproject                   <-- IDE Project configurations
├── .project                    <-- IDE Project configurations
├── *.ioc                       <-- STM32CubeMX Graphic Initialization File
└── README.md
