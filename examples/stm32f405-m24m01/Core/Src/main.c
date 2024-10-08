/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include <string.h>
#include "m24cxx.h"
#include "lfs.h"
#include "littlefs.h"
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

/* USER CODE BEGIN PV */
M24CXX_HandleTypeDef m24cxx;

uint8_t buf[256];
uint8_t do_action = 0;

lfs_file_t file;
lfs_dir_t dir;
struct lfs_info info;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Send printf to uart1
int _write(int fd, char *ptr, int len) {
    HAL_StatusTypeDef hstatus;

    if (fd == 1 || fd == 2) {
        hstatus = HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, HAL_MAX_DELAY);
        if (hstatus == HAL_OK)
            return len;
        else
            return -1;
    }
    return -1;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == BTN_Pin) // If the button
    {
        do_action = 1;
    }
}

void dump_buf(uint8_t *buf, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        if (i % 16 == 0) {
            printf("0x%08lx: ", i);
        }
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0)
            printf("\n");
    }
}

void fill_buffer(uint8_t *buf, uint32_t size, uint8_t type) {
    switch (type) {
    case 0:
        memset(buf, 0x00, size);
        break;
    case 1:
        memset(buf, 0xff, size);
        break;
    case 2:
        memset(buf, 0xaa, size);
        break;
    case 3:
        for (uint32_t i = 0; i < size; ++i) {
            buf[i] = (uint8_t) i;
        }
        break;
    }
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    DBG("\n\n\n--------\nStarting");

    // Wait a few ms to get ready
    HAL_Delay(10);

    DBG("Scanning I2C bus:");
    // Go through all possible i2c addresses
    for (uint8_t i = 0; i < 128; i++) {

        if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t) (i << 1), 3, 100) == HAL_OK) {
            // We got an ack
            printf("%2x ", i);
        } else {
            printf("-- ");
        }

        if (i > 0 && (i + 1) % 16 == 0)
            printf("\n");

    }

    printf("\n");

    if (m24cxx_init(&m24cxx, &hi2c1, 0x50) != M24CXX_Ok) {
        DBG("M24CXX Failed to initialize");
        Error_Handler();
    }

    //m24cxx_erase_all(&m24cxx);

    m24cxx_littlefs_init(&m24cxx);

    // read current count
    uint32_t boot_count = 0;

    lfs_file_open(&littlefs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&littlefs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&littlefs, &file);
    lfs_file_write(&littlefs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&littlefs, &file);

    uint32_t start_uptime = 0;

    lfs_file_open(&littlefs, &file, "uptime", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&littlefs, &file, &start_uptime, sizeof(start_uptime));
    lfs_file_close(&littlefs, &file);

    DBG("Boot count = %lu start uptime = %lu", boot_count, start_uptime);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    uint32_t now = 0, last_blink = 0, last_update = 0;

    while (1) {
        now = HAL_GetTick();

        if (now - last_blink >= 500) {
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            last_blink = now;
        }

        if (now - last_update >= 10000) {

            uint32_t total_uptime = start_uptime + now;

            DBG("Total uptime = %lu", total_uptime);

            uint32_t start = HAL_GetTick();
            lfs_file_open(&littlefs, &file, "uptime", LFS_O_RDWR);
            lfs_file_rewind(&littlefs, &file);
            lfs_file_write(&littlefs, &file, &total_uptime, sizeof(total_uptime));
            lfs_file_close(&littlefs, &file);
            DBG("File update took %lu ms", HAL_GetTick() - start);

            last_update = now;
        }

        if (do_action) {
            do_action = 0;
            DBG("Do action!");

            if (m24cxx_read(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            dump_buf(buf, sizeof(buf));

            fill_buffer(buf, sizeof(buf), 0);

            if (m24cxx_write(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            if (m24cxx_read(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            dump_buf(buf, sizeof(buf));

            fill_buffer(buf, sizeof(buf), 1);

            if (m24cxx_write(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            if (m24cxx_read(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            dump_buf(buf, sizeof(buf));

            fill_buffer(buf, sizeof(buf), 2);

            if (m24cxx_write(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            if (m24cxx_read(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            dump_buf(buf, sizeof(buf));

            fill_buffer(buf, sizeof(buf), 3);

            if (m24cxx_write(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            if (m24cxx_read(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            dump_buf(buf, sizeof(buf));

            if (m24cxx_erase_all(&m24cxx) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            if (m24cxx_read(&m24cxx, 0x0, (uint8_t*) &buf, sizeof(buf)) != M24CXX_Ok) {
                DBG("Returned err");
                Error_Handler();
            }

            dump_buf(buf, sizeof(buf));

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
  huart1.Init.BaudRate = 921600;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BTN_Pin */
  GPIO_InitStruct.Pin = BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
