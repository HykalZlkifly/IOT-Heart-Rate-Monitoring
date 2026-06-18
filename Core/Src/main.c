/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "max30102_for_stm32_hal.h"
#include "stm32f4xx_hal_pwr_ex.h"
#include "stm32f4xx_hal_rcc_ex.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
max30102_t max30102; // Global instance for the MAX30102 sensor

/* WIFHOTSPOT & THINGSPEAK CONFIGURATION CREDENTIALS */
#define WIFI_SSID       "UTEMSlayer_2.4GHz"
#define WIFI_PASSWORD   "BERR@DT649"
#define TS_HOST         "api.thingspeak.com"
#define TS_PORT         "80"
#define TS_API_KEY      "S7WE19GNUYCMD2VD"
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t ESP_Send_Command(char* command, char* expected_reply, uint32_t timeout);
void ESP_Init_WiFi(char* ssid, char* password);
void ESP_Send_To_ThingSpeak(int bpm);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* MASSIVE 24x22 PIXEL BEATING HEART */
void Draw_Large_Heart(uint8_t x, uint8_t y) {
    for(int i=4; i<=9; i++) {
        ssd1306_DrawPixel(x+i, y, 1);    ssd1306_DrawPixel(x+i, y+1, 1);
        ssd1306_DrawPixel(x+i+10, y, 1); ssd1306_DrawPixel(x+i+10, y+1, 1);
    }
    for(int i=2; i<=11; i++) {
        ssd1306_DrawPixel(x+i, y+2, 1);  ssd1306_DrawPixel(x+i, y+3, 1);
        ssd1306_DrawPixel(x+i+10, y+2, 1); ssd1306_DrawPixel(x+i+10, y+3, 1);
    }
    for(int j=4; j<=9; j++) {
        for(int i=1; i<=22; i++) { ssd1306_DrawPixel(x+i, y+j, 1); }
    }
    for(int i=2;  i<=21; i++) { ssd1306_DrawPixel(x+i, y+10, 1); ssd1306_DrawPixel(x+i, y+11, 1); }
    for(int i=4;  i<=19; i++) { ssd1306_DrawPixel(x+i, y+12, 1); ssd1306_DrawPixel(x+i, y+13, 1); }
    for(int i=6;  i<=17; i++) { ssd1306_DrawPixel(x+i, y+14, 1); ssd1306_DrawPixel(x+i, y+15, 1); }
    for(int i=8;  i<=15; i++) { ssd1306_DrawPixel(x+i, y+16, 1); ssd1306_DrawPixel(x+i, y+17, 1); }
    for(int i=10; i<=13; i++) { ssd1306_DrawPixel(x+i, y+18, 1); ssd1306_DrawPixel(x+i, y+19, 1); }
    ssd1306_DrawPixel(x+11, y+20, 1); ssd1306_DrawPixel(x+12, y+20, 1);
    ssd1306_DrawPixel(x+11, y+21, 1); ssd1306_DrawPixel(x+12, y+21, 1);
}

/* COMPRESSED 16x15 PIXEL RECOIL HEART */
void Draw_Small_Heart(uint8_t x, uint8_t y) {
    x += 4; y += 4;
    for(int i=2; i<=5; i++) { ssd1306_DrawPixel(x+i, y, 1);   ssd1306_DrawPixel(x+i+7, y, 1); }
    for(int i=1; i<=6; i++) { ssd1306_DrawPixel(x+i, y+1, 1); ssd1306_DrawPixel(x+i+7, y+1, 1); }
    for(int i=0; i<=14; i++) {
        ssd1306_DrawPixel(x+i, y+2, 1); ssd1306_DrawPixel(x+i, y+3, 1);
        ssd1306_DrawPixel(x+i, y+4, 1); ssd1306_DrawPixel(x+i, y+5, 1);
    }
    for(int i=1;  i<=13; i++) { ssd1306_DrawPixel(x+i, y+6, 1);  ssd1306_DrawPixel(x+i, y+7, 1); }
    for(int i=3;  i<=11; i++) { ssd1306_DrawPixel(x+i, y+8, 1);  ssd1306_DrawPixel(x+i, y+9, 1); }
    for(int i=5;  i<=9; i++)  { ssd1306_DrawPixel(x+i, y+10, 1); ssd1306_DrawPixel(x+i, y+11, 1); }
    for(int i=6;  i<=8; i++)  { ssd1306_DrawPixel(x+i, y+12, 1); ssd1306_DrawPixel(x+i, y+13, 1); }
    ssd1306_DrawPixel(x+7, y+14, 1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  // 1. Initialize display hardware
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Optimizing Bus...", Font_7x10, White);
  ssd1306_UpdateScreen();
  HAL_Delay(500);

  // 2. Network handshake presentation Layer
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Connecting WiFi...", Font_7x10, White);
  ssd1306_UpdateScreen();

  // Establish connection with the configured hotspot profile
  ESP_Init_WiFi(WIFI_SSID, WIFI_PASSWORD);

  HAL_Delay(5000);
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("WiFi Ready!", Font_7x10, White);
  ssd1306_UpdateScreen();


  // 3. Hardware sanity diagnostic for I2C Sensor Bus
  if (HAL_I2C_IsDeviceReady(&hi2c1, (0x57 << 1), 5, 1000) == HAL_OK)
  {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("MAX30102: ONLINE", Font_7x10, White);
      ssd1306_SetCursor(0, 15);
      ssd1306_WriteString("Realtime Mode...", Font_7x10, White);
      ssd1306_UpdateScreen();
      HAL_Delay(800);
  }
  else
  {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("BUS ERROR", Font_7x10, White);
      ssd1306_UpdateScreen();
      while(1);
  }

  // 4. Initialize the MAX30102 configuration registers
  max30102_init(&max30102, &hi2c1);
  max30102_reset(&max30102);
  HAL_Delay(100);

  // Force clean active SpO2 mode
  uint8_t mode_config = 0x03;
  max30102_write(&max30102, MAX30102_MODE_CONFIG, &mode_config, 1);
  HAL_Delay(10);

  // Set highly responsive current level (0x24 = ~7.2mA)
  uint8_t led_current = 0x24;
  max30102_write(&max30102, MAX30102_LED_IR_PA1, &led_current, 1);
  max30102_write(&max30102, MAX30102_LED_RED_PA2, &led_current, 1);

  // Fast-sampling SpO2 configuration (18-bit ADC, 100 samples per second)
  uint8_t spo2_config = 0x27;
  max30102_write(&max30102, MAX30102_SPO2_CONFIG, &spo2_config, 1);

  max30102_clear_fifo(&max30102);

  // 5. Update UI layout to ready state
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Place Finger", Font_11x18, White);
  ssd1306_UpdateScreen();

  char display_buffer[32];
  uint32_t last_display_update = 0;
  uint32_t last_led_blink = 0;
  uint32_t last_fifo_read = 0;

  /* METRIC CALCULATION & FILTER VARIANCE VARIABLES */
  uint32_t last_ir = 0;
  uint32_t last_beat_time = 0;
  int raw_bpm = 0;
  int display_bpm = 0;
  int is_beating = 0;
  uint32_t beat_hold_start = 0;

  // Moving Average Filter Arrays
  int bpm_history[4] = {0, 0, 0, 0};
  int history_index = 0;

  // Cloud transmission asynchronous update tracking timer
  uint32_t last_thingspeak_upload = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      // FAST RADAR POLLING: Poll the sensor every 10ms instead of running completely uncontrolled
      if (HAL_GetTick() - last_fifo_read > 10)
      {
          last_fifo_read = HAL_GetTick();

          // ANTI-LAG SAFETY FILTER:
          uint8_t wr_ptr = 0, rd_ptr = 0;
          max30102_read(&max30102, MAX30102_FIFO_WR_PTR, &wr_ptr, 1);
          max30102_read(&max30102, MAX30102_FIFO_RD_PTR, &rd_ptr, 1);

          int8_t accumulated = (int8_t)wr_ptr - (int8_t)rd_ptr;
          if (accumulated < 0) accumulated += 32;

          if (accumulated > 16)
          {
              max30102_clear_fifo(&max30102); // Wipe old backlogged cache instantly
          }

          max30102_read_fifo(&max30102);
          uint32_t current_ir = max30102._ir_samples[0];

          // Turn off heartbeat animation frame after 140ms hold time
          if (is_beating && (HAL_GetTick() - beat_hold_start > 140)) {
              is_beating = 0;
          }

          // Calculate Heart Beats and BPM via Peak-Dipping logic
          if (current_ir > 35000) {
              if ((last_ir > current_ir) && (HAL_GetTick() - last_beat_time > 450)) {
                  uint32_t time_diff = HAL_GetTick() - last_beat_time;
                  raw_bpm = 60000 / time_diff;

                  if (raw_bpm >= 40 && raw_bpm <= 180) {
                      is_beating = 1;
                      beat_hold_start = HAL_GetTick();

                      bpm_history[history_index] = raw_bpm;
                      history_index = (history_index + 1) % 4;

                      int sum = 0;
                      int valid_beats = 0;
                      for(int i = 0; i < 4; i++) {
                          if(bpm_history[i] > 0) {
                              sum += bpm_history[i];
                              valid_beats++;
                          }
                      }
                      if(valid_beats > 0) {
                          display_bpm = sum / valid_beats;
                      }
                  }
                  last_beat_time = HAL_GetTick();
              }
          } else {
              // Clear context elements when finger gets detached
              display_bpm = 0;
              is_beating = 0;
              for(int i=0; i<4; i++) bpm_history[i] = 0;
          }
          last_ir = current_ir;
      }

      // HIGH REFRESH SCREEN ENGINE: Process layout updates every 150ms instead of 250ms
      if (HAL_GetTick() - last_display_update > 150)
      {
          last_display_update = HAL_GetTick();

          ssd1306_Fill(Black);

          // Render Header Title Banner UI
          ssd1306_SetCursor(4, 2);
          ssd1306_WriteString("PULSE MONITOR", Font_7x10, White);
          for(uint8_t i = 0; i < 128; i++) {
              ssd1306_DrawPixel(i, 13, 1);
          }

          if (display_bpm > 0) {
              // Render active real-time BPM values
              sprintf(display_buffer, "%d", display_bpm);
              ssd1306_SetCursor(8, 30);
              ssd1306_WriteString(display_buffer, Font_11x18, White);

              ssd1306_SetCursor(48, 36);
              ssd1306_WriteString("BPM", Font_7x10, White);

              // Render Animated Heart graphic state machine
              if (is_beating) {
                  Draw_Large_Heart(88, 24);
              } else {
                  Draw_Small_Heart(88, 24);
              }
          } else {
              // Standard Idle prompt state
              ssd1306_SetCursor(10, 34);
              ssd1306_WriteString("PLACE FINGER...", Font_7x10, White);
          }

          ssd1306_UpdateScreen();
      }

      // Board Status heartbeat LED
      if (HAL_GetTick() - last_led_blink > 500)
      {
          last_led_blink = HAL_GetTick();
          HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      }

      // NON-BLOCKING CLOUD ENGINE: Push data to ThingSpeak every 20 seconds (bypasses the 15s limit safely)
      if (display_bpm > 0 && (HAL_GetTick() - last_thingspeak_upload > 20000))
      {
          last_thingspeak_upload = HAL_GetTick();

          // Temporary tiny HUD notification update to show active cloud sync without blocking
          ssd1306_SetCursor(4, 54);
          ssd1306_WriteString("TS SYNC...", Font_7x10, White);
          ssd1306_UpdateScreen();

          ESP_Send_To_ThingSpeak(display_bpm);
      }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE BEGIN USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Enable USART1 Clock for ESP-01 Wi-Fi Module */
  __HAL_RCC_USART1_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 (TX) and PA10 (RX) for ESP-01 Module */
  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 (SCL) and PB9 (SDA) for I2C1 (OLED & MAX30102) */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
uint8_t ESP_Send_Command(char* command, char* expected_reply, uint32_t timeout) {
    uint8_t reply_buffer[200];
    memset(reply_buffer, 0, sizeof(reply_buffer));

    // Clear any residual rx noise before sending
    uint32_t tmpreg = 0x00U;
    tmpreg = huart1.Instance->SR;
    tmpreg = huart1.Instance->DR;
    (void)tmpreg;

    HAL_UART_Transmit(&huart1, (uint8_t*)command, strlen(command), 1000);

    // Check response (Using simple polling for setup phase)
    uint32_t tickstart = HAL_GetTick();
    uint16_t idx = 0;
    while ((HAL_GetTick() - tickstart) < timeout) {
        uint8_t ch;
        if (HAL_UART_Receive(&huart1, &ch, 1, 10) == HAL_OK) {
            if (idx < sizeof(reply_buffer) - 1) {
                reply_buffer[idx++] = ch;
            }
            if (strstr((char*)reply_buffer, expected_reply) != NULL) {
                // Optional: Mirror response to ST-Link (USART2) for easy debugging
                HAL_UART_Transmit(&huart2, reply_buffer, idx, 1000);
                return 1; // Success
            }
        }
    }
    // Print failure message to terminal for debug tracking
    char err_msg[50];
    sprintf(err_msg, "\r\nTimeout waiting for: %s\r\n", expected_reply);
    HAL_UART_Transmit(&huart2, (uint8_t*)err_msg, strlen(err_msg), 1000);
    return 0; // Fail
}

void ESP_Init_WiFi(char* ssid, char* password) {
    char wifi_cmd[128];

    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n--- Initializing ESP-01 Wi-Fi ---\r\n", 37, 1000);

    ESP_Send_Command("AT\r\n", "OK", 2000);
    ESP_Send_Command("AT+CWMODE=1\r\n", "OK", 2000);

    sprintf(wifi_cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);

    // 🌟 CHANGE "WIFI CONNECTED" TO "WIFI GOT IP" 🌟
    // This stops the STM32 from jumping out of setup until the router assigns an IP!
    ESP_Send_Command(wifi_cmd, "WIFI GOT IP", 15000);
}

void ESP_Send_To_ThingSpeak(int bpm) {
    char post_body[64];
    char http_request[300];
    char cip_start[64];
    char cip_send[32];

    // 1. Format the form-urlencoded payload body
    snprintf(post_body, sizeof(post_body), "api_key=%s&field1=%d", TS_API_KEY, bpm);
    int body_len = strlen(post_body);

    // 2. Build standard ThingSpeak HTTP POST data structure
    snprintf(http_request, sizeof(http_request),
             "POST /update HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "Content-Type: application/x-www-form-urlencoded\r\n"
             "Content-Length: %d\r\n\r\n"
             "%s\r\n",
             TS_HOST, body_len, post_body);
    int http_len = strlen(http_request);

    // 3. Command pipeline sequence execution over TCP
    snprintf(cip_start, sizeof(cip_start), "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", TS_HOST, TS_PORT);
    if (ESP_Send_Command(cip_start, "CONNECT", 4000)) {
        snprintf(cip_send, sizeof(cip_send), "AT+CIPSEND=%d\r\n", http_len);
        if (ESP_Send_Command(cip_send, ">", 2000)) {
            HAL_UART_Transmit(&huart1, (uint8_t*)http_request, http_len, 2000);
            HAL_Delay(500); // Buffer relief window
        }
        ESP_Send_Command("AT+CIPCLOSE\r\n", "OK", 2000);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
