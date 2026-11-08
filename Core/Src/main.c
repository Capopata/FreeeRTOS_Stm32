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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Sector/Page 63 cua STM32F103 (1KB cuoi cung trong 64KB Flash: 0x0800FC00 - 0x0800FFFF) */
#define FLASH_USER_PAGE_ADDR   ((uint32_t)0x0800FC00)
#define RX_BUFFER_SIZE         256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* Definitions for commMessagefrom */
osThreadId_t commMessagefromHandle;
const osThreadAttr_t commMessagefrom_attributes = {
  .name = "commMessagefrom",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_data_len = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
HAL_StatusTypeDef Flash_ErasePage(uint32_t pageAddress);
HAL_StatusTypeDef Flash_Write(uint32_t address, const uint8_t *data, uint32_t length);
void Flash_Read(uint32_t address, uint8_t *buffer, uint32_t length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  /* Bat ngat USART1 va bat dau nhan du lieu che do Idle Line */
  HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of commMessagefrom */
  commMessagefromHandle = osThreadNew(StartDefaultTask, NULL, &commMessagefrom_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

/* Callback ngat nhan du lieu tu USART1 (ket thuc khi gap Idle Line hoac du so byte) */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart->Instance == USART1)
  {
    rx_data_len = Size;
    /* Danh thuc task FreeRTOS commMessagefrom */
    osThreadFlagsSet(commMessagefromHandle, 0x01);
  }
}

/**
  * @brief  Xoa 1 Page (Sector) Flash (STM32F103 Page = 1KB = 1024 bytes)
  */
HAL_StatusTypeDef Flash_ErasePage(uint32_t pageAddress)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t PageError = 0;

  HAL_FLASH_Unlock();

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = pageAddress;
  EraseInitStruct.NbPages     = 1;

  HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

  HAL_FLASH_Lock();
  return status;
}

/**
  * @brief  Ghi mang byte vao Flash (ghi theo Word 32-bit)
  */
HAL_StatusTypeDef Flash_Write(uint32_t address, const uint8_t *data, uint32_t length)
{
  HAL_StatusTypeDef status = HAL_OK;
  HAL_FLASH_Unlock();

  for (uint32_t i = 0; i < length; i += 4)
  {
    uint32_t word = 0xFFFFFFFF;
    uint32_t chunk = (length - i < 4) ? (length - i) : 4;
    memcpy(&word, data + i, chunk);

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, word);
    if (status != HAL_OK)
    {
      break;
    }
  }

  HAL_FLASH_Lock();
  return status;
}

/**
  * @brief  Doc du lieu tu Flash (bo nho Flash duoc map truc tiep vao dia chi 0x08000000)
  */
void Flash_Read(uint32_t address, uint8_t *buffer, uint32_t length)
{
  memcpy(buffer, (const void *)address, length);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the commMessagefrom thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  uint8_t readback_buf[RX_BUFFER_SIZE + 1];

  printf("\r\n=======================================================\r\n");
  printf("  STM32F103 Flash Write/Read via Serial Ready!\r\n");
  printf("  Target Flash Page: 0x%08lX (1024 bytes)\r\n", (unsigned long)FLASH_USER_PAGE_ADDR);
  printf("  -> Hay gui bat ky chuoi ky tu nao qua Serial Terminal...\r\n");
  printf("=======================================================\r\n");

  /* Infinite loop */
  for (;;)
  {
    /* Cho co 0x01 tu ngat UART (o trang thai Block, khong ton CPU) */
    uint32_t flags = osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
    if (flags & 0x01)
    {
      uint16_t len = rx_data_len;
      if (len > 0)
      {
        printf("\r\n[1. SERIAL] Da nhan %u bytes: \"%.*s\"\r\n", len, len, rx_buffer);

        /* 1. Xoa Flash Page truoc khi ghi */
        printf("[2. FLASH] Dang xoa Page tai dia chi 0x%08lX...\r\n", (unsigned long)FLASH_USER_PAGE_ADDR);
        HAL_StatusTypeDef status = Flash_ErasePage(FLASH_USER_PAGE_ADDR);
        if (status != HAL_OK)
        {
          printf("[ERROR] Xoa Flash that bai! Ma loi: %d\r\n", (int)status);
        }
        else
        {
          /* 2. Ghi du lieu vao Flash */
          printf("[3. FLASH] Dang ghi %u bytes vao Flash...\r\n", len);
          status = Flash_Write(FLASH_USER_PAGE_ADDR, rx_buffer, len);
          if (status != HAL_OK)
          {
            printf("[ERROR] Ghi Flash that bai! Ma loi: %d\r\n", (int)status);
          }
          else
          {
            printf("[4. FLASH] Ghi vao Flash thanh cong!\r\n");

            /* 3. Doc lai du lieu tu Flash */
            memset(readback_buf, 0, sizeof(readback_buf));
            Flash_Read(FLASH_USER_PAGE_ADDR, readback_buf, len);

            /* 4. Gui thong bao va noi dung doc duoc len Serial */
            printf("\r\n================== [KET QUA DOC FLASH] ==================\r\n");
            printf(">> Dia chi Flash    : 0x%08lX\r\n", (unsigned long)FLASH_USER_PAGE_ADDR);
            printf(">> Du lieu doc lai  : \"%s\"\r\n", (char *)readback_buf);
            if (memcmp(rx_buffer, readback_buf, len) == 0)
            {
              printf(">> TRANG THAI       : HOAN TOAN TRUNG KHOP 100%%!\r\n");
            }
            else
            {
              printf(">> CANH BAO         : Du lieu doc khong khop voi ban tin nhan!\r\n");
            }
            printf("=========================================================\r\n\r\n");
          }
        }
      }

      /* Xoa bo dem va tiep tuc lang nghe du lieu tiep theo tu Serial */
      memset(rx_buffer, 0, sizeof(rx_buffer));
      HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
    }
  }
  /* USER CODE END 5 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
