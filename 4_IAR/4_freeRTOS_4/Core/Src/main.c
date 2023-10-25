/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdio.h>	        // ф-ии ввода вывода
#include <string.h> 	        // для работы со строками
#include <math.h>	        // для работы с математическими функциями
#include "SEGGER_RTT.h"	        // библиотека для отладочного RTT вывода через интерфейс отладчика
#include "oled_ssd1306.h"       // драйвер для дисплея SSD1306
#include "gfx_bw.h"             // графическая библиотека для дисплея
#include "fonts.h"              // библиотека преобразования шрифтов в битмап
#include "picture.h"            // для вывода изображений из массивов на дисплей

#include "queue.h"              // работа с очередями freeRTOS
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
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c2_tx;

RTC_HandleTypeDef hrtc;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTaskLed500 */
osThreadId_t myTaskLed500Handle;
const osThreadAttr_t myTaskLed500_attributes = {
  .name = "myTaskLed500",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for myTaskLed250 */
osThreadId_t myTaskLed250Handle;
const osThreadAttr_t myTaskLed250_attributes = {
  .name = "myTaskLed250",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for myTaskLed100 */
osThreadId_t myTaskLed100Handle;
const osThreadAttr_t myTaskLed100_attributes = {
  .name = "myTaskLed100",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for myTaskDisplay */
osThreadId_t myTaskDisplayHandle;
const osThreadAttr_t myTaskDisplay_attributes = {
  .name = "myTaskDisplay",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for myQueueBtn */
osMessageQueueId_t myQueueBtnHandle;
const osMessageQueueAttr_t myQueueBtn_attributes = {
  .name = "myQueueBtn"
};
/* Definitions for myTimerMinutes */
osTimerId_t myTimerMinutesHandle;
const osTimerAttr_t myTimerMinutes_attributes = {
  .name = "myTimerMinutes"
};
/* Definitions for myTimerDisplaySaver */
osTimerId_t myTimerDisplaySaverHandle;
const osTimerAttr_t myTimerDisplaySaver_attributes = {
  .name = "myTimerDisplaySaver"
};
/* Definitions for myTimerBtnScan */
osTimerId_t myTimerBtnScanHandle;
const osTimerAttr_t myTimerBtnScan_attributes = {
  .name = "myTimerBtnScan"
};
/* USER CODE BEGIN PV */
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

char vTaskListBuf[1024]; // буфер для диспетчера задач
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_I2C2_Init(void);
void StartDefaultTask(void *argument);
void StartTaskLed500(void *argument);
void StartTaskLed250(void *argument);
void StartTaskLed100(void *argument);
void StartTaskDisplay(void *argument);
void tmrMinutes(void *argument);
void tmrDisplaySaver(void *argument);
void tmrBtnScan(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void initOled() // инициализация дисплея SSD1306
{
        // инициализация OLED дисплея 
        SSD1306_I2cInit(&hi2c2);
        GFX_SetFont(font_7x5);
        GFX_SetFontSize(1); 
        //SSD1306_StartScrollLeftUp(0, 7, SCROLL_EVERY_256_FRAMES, 1);
        //SSD1306_StartScrollLeft(0, 7, SCROLL_EVERY_4_FRAMES);
        //SSD1306_StartBlinking(1); // красивая плавная моргалка яркостью. но уже заебала
        //SSD1306_StartFadeOut(1);
        //SSD1306_ZoomIn(1);
        //SSD1306_InvertColors(1);
        //SSD1306_RotateDisplay(0);
        SSD1306_Clear(BLACK);
        SSD1306_Bitmap((uint8_t*)picture);
        /*
        // прогрессбар
        GFX_DrawRoundRectangle(32, 16, 60, 10, 5, WHITE);
        GFX_DrawFillRoundRectangle(32, 16, 30, 10, 5, WHITE);
        // простая надпись
        GFX_DrawString(19,0, "(c) RusikOk", WHITE, BLACK);
        SSD1306_Display();
        */
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
  MX_DMA_Init();
  MX_RTC_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */
        /* ************************************************************************ */
	/* инициализация 						  	    */
	/* ************************************************************************ */
        LOG_START();
        LOGT("\r\n\r\n");
        LOGT(" __________________________________________________\r\n");
        LOGT("(  )                                            (  )\r\n");
        LOGT(" )(   ____  __   __  ____  __  ___ __ _ __  ___  )(\r\n");
        LOGT("(  ) (  _ \\(  ) (  )(  __)(  )(  /(  ) (  )(  / (  )\r\n");
        LOGT(" )(   )   / )(   )(  )_ (  )(  )(  )(   )(  )(   )(\r\n");
        LOGT("(  ) (_)\\_)(__)_(__)(____)(__)(__\\(__)_(__)(__\\ (  )\r\n");
        LOGT(" )(                                              )(\r\n");
        LOGT("(__)____________________________________________(__)\r\n\r\n");
        LOGT("      Compiled version: " __TIME__ " " __DATE__ "\r\n\r\n");

        initOled(); // инициализация OLED дисплея SSD1306
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of myTimerMinutes */
  myTimerMinutesHandle = osTimerNew(tmrMinutes, osTimerPeriodic, NULL, &myTimerMinutes_attributes);

  /* creation of myTimerDisplaySaver */
  myTimerDisplaySaverHandle = osTimerNew(tmrDisplaySaver, osTimerPeriodic, NULL, &myTimerDisplaySaver_attributes);

  /* creation of myTimerBtnScan */
  myTimerBtnScanHandle = osTimerNew(tmrBtnScan, osTimerPeriodic, NULL, &myTimerBtnScan_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  // выставляем периодичность работы таймеров и запускаем их (на самом деле таймеры запустяться после старта шедуллера)
  osTimerStart(myTimerMinutesHandle, 60 * 1000 / portTICK_RATE_MS); // выполнение раз в минуту
  osTimerStart(myTimerDisplaySaverHandle, displaySaverINTERVAL); // выполнение раз в X минут
  osTimerStart(myTimerBtnScanHandle, 10); // выполнение каждые 10 тиков ОС
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueueBtn */
  myQueueBtnHandle = osMessageQueueNew (8, sizeof(uint8_t), &myQueueBtn_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTaskLed500 */
  myTaskLed500Handle = osThreadNew(StartTaskLed500, NULL, &myTaskLed500_attributes);

  /* creation of myTaskLed250 */
  myTaskLed250Handle = osThreadNew(StartTaskLed250, NULL, &myTaskLed250_attributes);

  /* creation of myTaskLed100 */
  myTaskLed100Handle = osThreadNew(StartTaskLed100, NULL, &myTaskLed100_attributes);

  /* creation of myTaskDisplay */
  myTaskDisplayHandle = osThreadNew(StartTaskDisplay, NULL, &myTaskDisplay_attributes);

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED4_Pin */
  GPIO_InitStruct.Pin = LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN1_Pin BTN2_Pin */
  GPIO_InitStruct.Pin = BTN1_Pin|BTN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskLed500 */
/**
* @brief Function implementing the myTaskLed500 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLed500 */
void StartTaskLed500(void *argument)
{
  /* USER CODE BEGIN StartTaskLed500 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(500);
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
  }
  /* USER CODE END StartTaskLed500 */
}

/* USER CODE BEGIN Header_StartTaskLed250 */
/**
* @brief Function implementing the myTaskLed250 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLed250 */
void StartTaskLed250(void *argument)
{
  /* USER CODE BEGIN StartTaskLed250 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(250);
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
  }
  /* USER CODE END StartTaskLed250 */
}

/* USER CODE BEGIN Header_StartTaskLed100 */
/**
* @brief Function implementing the myTaskLed100 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLed100 */
void StartTaskLed100(void *argument)
{
  /* USER CODE BEGIN StartTaskLed100 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
    HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
  }
  /* USER CODE END StartTaskLed100 */
}

/* USER CODE BEGIN Header_StartTaskDisplay */
/**
* @brief Function implementing the myTaskDisplay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskDisplay */
void StartTaskDisplay(void *argument)
{
  /* USER CODE BEGIN StartTaskDisplay */
        uint8_t btnCounter = 0;
        /* Infinite loop */
        for(;;)
        {                
                uint8_t tick; // для чтения из очереди данных
                //osMessageQueueGet(myQueueBtnHandle,
                xQueueReceive(myQueueBtnHandle, &tick, portMAX_DELAY); // блокируем поток до появления данных в очереди
                if(tick < 100) // определяем короткое нажатие на кнопку
                        btnCounter++;
                if(tick > 100) // определяем длинное нажатие на кнопку
                        btnCounter++;              
                                
                // селектор вывода данных на дисплей
                switch(btnCounter)
                {
                case 0:      
                case 1:
                        SSD1306_Bitmap((uint8_t*)gerb);
                        break;
                case 2:
                        SSD1306_Bitmap((uint8_t*)telka);
                        break;
                case 3:
                        SSD1306_Bitmap((uint8_t*)my2);
                        break;
                case 4:
                        SSD1306_Bitmap((uint8_t*)Wait);
                        break;
                case 5:
                        // простая надпись
                        SSD1306_Clear(BLACK);
                        GFX_DrawString(0, 0, "(c) RusikOk", WHITE, BLACK);
                        SSD1306_Display();
                        break;
                case 6:
                        // вывод данных от диспетчера ОС
                        LOG(vTaskListBuf);
                        SSD1306_Clear(BLACK);
                        GFX_DrawString(0, 0, vTaskListBuf, WHITE, BLACK);
                        GFX_DrawString(0, 8, vTaskListBuf+26, WHITE, BLACK);
                        GFX_DrawString(0, 16, vTaskListBuf+53, WHITE, BLACK);
                        GFX_DrawString(0, 24, vTaskListBuf+80, WHITE, BLACK);
                        GFX_DrawString(0, 32, vTaskListBuf+106, WHITE, BLACK);
                        GFX_DrawString(0, 40, vTaskListBuf+133, WHITE, BLACK);
                        GFX_DrawString(0, 48, vTaskListBuf+160, WHITE, BLACK);
                        SSD1306_Display();
                        break;
                case 7:
                        // простая надпись
                        SSD1306_Clear(BLACK);
                        GFX_DrawString(0, 8, "CPU usage: ", WHITE, BLACK);
                        //osGetCPUUsage
                        
                        SSD1306_Display();
                        
                        // сбрасываем счетчик нажатий кнопки для новой итерации перебора пунктов меню
                        btnCounter = 0;   
                }
        }
  /* USER CODE END StartTaskDisplay */
}

/* tmrMinutes function */
void tmrMinutes(void *argument)
{
  /* USER CODE BEGIN tmrMinutes */
        /* ******************************************************************** */
	/* определяем текущее дату/время					*/
  	/* ******************************************************************** */
        if(HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
                Error_Handler();
        if(HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
                Error_Handler();
        
        /* ******************************************************************** */
	/* выполняем некоторые периодические действия				*/
	/* ******************************************************************** */
	/*if(((sTime.Hours % 1) == 0) && ((sTime.Minutes % 1) == 0)) // раз 5 минут
		sendData = true; // устанавливаем флаг отправки данных на сервер
	if((sTime.Hours == 4) && (sTime.Minutes == 45))
                devStat(); // отправляем на сервер данные статистики наработки прибора
	if(((sTime.Hours % 1) == 0) && ((sTime.Minutes % 5) == 0)) // каждые 5 минут
	  	if(mode == MODE_WIFI)
	  		csq = esp_csq(ssid); // запрашиваем у модуля уровень сигнала*/
  /* USER CODE END tmrMinutes */
}

/* tmrDisplaySaver function */
void tmrDisplaySaver(void *argument)
{
  /* USER CODE BEGIN tmrDisplaySaver */
        static uint8_t y = 0;
        osTimerStart(myTimerDisplaySaverHandle, 1000 / portTICK_RATE_MS); // раз уж попали сюда, то меняем интервал таймера для наглядности анимации хранителя экрана. раз в секунду
        SSD1306_Clear(BLACK); // очистка экрана
        SSD1306_DrawPixel(SSD1306_LCDWIDTH - 1, y++, WHITE); // формируем точку
        SSD1306_Display(); // передаем буфер на дисплей
        if(y > SSD1306_LCDHEIGHT - 2)
                y = 0;
  /* USER CODE END tmrDisplaySaver */
}

/* tmrBtnScan function */
void tmrBtnScan(void *argument)
{
  /* USER CODE BEGIN tmrBtnScan */
        static uint8_t tick = 0; // счетчик тиков при нажатой кнопке
        if(BTN1_PRESSED()) // ждем нажатия на кнопку
        {
                tick++; // считаем сколько тактов кнопка будет нажата
        }
        else if(tick > 0)
        {
                osTimerStart(myTimerDisplaySaverHandle, displaySaverINTERVAL); // сбрасываем таймер, xTimerReset не завезли(
                //xQueueSend(myQueueBtnHandle, &tick, 0); // отправляем в очередь время нажатия кнопки
                osMessageQueuePut(myQueueBtnHandle, &tick, NULL, 0); // отправляем в очередь время нажатия кнопки
                tick = 0; // сбрасываем счетчик
        }
  /* USER CODE END tmrBtnScan */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM11) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
        LOG_HALERROR();
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
