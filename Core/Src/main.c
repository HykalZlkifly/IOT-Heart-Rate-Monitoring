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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
max30102_t max30102; // Global instance for the MAX30102 sensor
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE BEGIN 2 */
  // 1. Initialize display console
  ssd1306_Init();
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Optimizing Bus...", Font_7x10, White);
  ssd1306_UpdateScreen();
  HAL_Delay(500);

  // 2. Hardware sanity diagnostic
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

  // 3. Initialize the MAX30102 configuration registers
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

  // 4. Update UI layout
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Place Finger", Font_11x18, White);
  ssd1306_UpdateScreen();

  char display_buffer[32];
  uint32_t last_display_update = 0;
  uint32_t last_led_blink = 0;
  uint32_t last_fifo_read = 0;

  /* NEW CALCULATION TRACKING VARIABLES */
  uint32_t last_ir = 0;
  uint32_t last_beat_time = 0;
  int raw_bpm = 0;
  int display_bpm = 0;
  int is_beating = 0;
  uint32_t beat_hold_start = 0;

  // Moving Average Filter Arrays
  int bpm_history[4] = {0, 0, 0, 0};
  int history_index = 0;
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

  __HAL_RCC_PWR_CLK_ENABLE();
  MODIFY_REG(PWR->CR, PWR_CR_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);

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
  __HAL_RCC_I2C1_CLK_ENABLE();

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

  HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
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
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
