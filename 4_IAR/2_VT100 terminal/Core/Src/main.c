/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define KEY_ENTER	13
#define KEY_RIGHT	68
#define KEY_LEFT	67
#define KEY_DOWN	66
#define KEY_UP		65
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};
RTC_AlarmTypeDef sAlarm = {0};
uint8_t uartBuf[1]; // буффер для получения символа от UART по прерыванию
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void showMainMemu(); // главный экран
void showSetDateMemu(); // меню установки даты
void showSetTimeMemu(); // меню установки времени
void showSetAlarmTimeONMemu(); // меню установки будильника на включение светодиода
void showSetAlarmTimeOFFMemu(); // меню установки будильника на выключение светодиода
void showParseStringMemu(); // меню парсинга
void showSetPWMMemu(); // меню установки будильника на включение светодиода
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int MyLowLevelPutchar(int x) // отправка байта в UART через printf и пр.
{
  	HAL_UART_Transmit(&huart1, (uint8_t *)&x, 1, 0xFFFF);
	return(x);
}

int MyLowLevelGetchar() // получение байта из UART в блокировочном режиме через scanf и пр.
{
  	uint8_t x;
	do
	{
	  	HAL_UART_Receive(&huart1, (uint8_t *)&x, 1, 0xFFFF);
	} 
	while(x == 0);
	return((int)x);
}

#define CURSOR_START	23
#define MENU_ITEM		6

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) // калбек по приему от UART
{
	static uint8_t cursorPos = CURSOR_START; // текущая позиция курсора на главном экране
	
	if(huart == &huart1)
	{
		// обрабатываем нажатия клавиш
		switch(uartBuf[0])
		{
		case KEY_UP:
			{
				if(cursorPos > CURSOR_START) 
					cursorPos--;
			} break;
		case KEY_DOWN:
			{
				if(cursorPos < CURSOR_START + MENU_ITEM - 1)
					cursorPos++;
			} break;
		case KEY_ENTER:
			{
				// запуск выбранной процедуры из основного меню
				switch(cursorPos - CURSOR_START)
				{
					case 0: showSetDateMemu(); break; // меню установки даты
					case 1: showSetTimeMemu(); break; // меню установки времени
					case 2: showSetAlarmTimeONMemu(); break; // меню установки будильника на включение светодиода
					case 3: showSetAlarmTimeOFFMemu(); break; // меню установки будильника на выключение светодиода
					case 4: showParseStringMemu(); break; // меню парсинга
					case 5: showSetPWMMemu(); break; // меню установки будильника на включение светодиода	
				}
				
				showMainMemu(); // заново формируем главный экран
			} break;
		}

		if(uartBuf[0] != 0x1B) // двигаем курсор только при получении кода клавиши, пропустив начало управляющей последовательности 0x1B
		{
			printf("\x1B[%u;15H      ", cursorPos - 1); // сначала подчистим артефакты
			printf("\x1B[%u;15H      ", cursorPos + 1);
			printf("\x1B[%u;15H >>>>>", cursorPos); // вывод курсора главного меню
		}
		
		HAL_UART_Receive_IT(&huart1, (uint8_t*)uartBuf, 1); // опять разрешаем прерывание по приему от UART, вызов колбека после получения каждого байта
	}
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) // калбек WakeUp таймера
{
	// обновляем значение времени в консоли каждую секунду
	
	// прочитаем DT из RTC
	HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);
	
	// выводим дату/время
	printf("\x1B[s"); // сохраняем текущую позицию курсора, чтобы ничего ненарушить в основной программе
	printf("\x1B[14;50H"); // позиция курсора для даты
	printf("Date: 20%02u:%02u:%02u", sDate.Year, sDate.Month, sDate.Date);
	printf("\x1B[16;50H"); // позиция курсора для вывода времени
	printf("Time:   %02u:%02u:%02u", sTime.Hours, sTime.Minutes, sTime.Seconds);
	printf("\x1B[u"); // восстанавливаем позицию курсора
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) // калбек от будильника(ов)
{
	printf("alarm!!!!!!!!!!!!!! \r\n");	
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
}

void showMainMemu() // формируем главный экран в консоли
{
  	// http://microsin.net/adminstuff/xnix/ansivt100-terminal-control-escape-sequences.html			- управляющие последовательности
  	// https://pixelplus.ru/samostoyatelno/stati/vnutrennie-faktory/tablica-simvolov-unicode.html	- таблица символов UTF-8
  
  	// сброс всех настроек терминала, стирание экрана, установка курсора на 0 позицию, сброс всех атрибутов
	printf("\x1Bс\x1B[2J\x1B[0;0H\x1B[0m"); 
	
	// формируем рабочую область экрана
	printf("\r\n\r\n\r\n\x1B[1;32m");
	printf("\r\n\t\t ╔══╗ ╔═══╗╔══╗    ╔╗╔╗╔════╗ ╔╗╔══╗╔══╗");
	printf("\r\n\t\t ║╔╗╚╗║╔══╝║╔═╝    ║║║║╚═╗╔═╝╔╝║║╔╗║║╔╗║");
	printf("\r\n\t\t ║║╚╗║║╚══╗║║      ║║║║  ║║  ╚╗║║║║║║║║║");
	printf("\r\n\t\t ║║ ║║║╔══╝║║      ║╚╝║  ║║   ║║║║║║║║║║");
	printf("\r\n\t\t ║╚═╝║║╚══╗║╚═╗    ╚╗╔╝  ║║   ║║║╚╝║║╚╝║");
	printf("\r\n\t\t ╚═══╝╚═══╝╚══╝     ╚╝   ╚╝   ╚╝╚══╝╚══╝");
	printf("\r\n\r\n\x1B[0m");
	printf("\r\n\t╔═══════════════════════════════════════╦════════════════╗");
	printf("\r\n\t║\t\t\t\t\t║\t\t ║");
	printf("\r\n\t║\t\x1B[1;33m (c) 2021-02-09 RusikOk \x1B[0m \t╟────────────────╢");
	printf("\r\n\t║\t\t\t\t\t║\t\t ║");
	printf("\r\n\t╠═══════════════════════════════════════╬════════════════╣");
	printf("\r\n\t║\t\t\t\t\t║\t\t ║");
	printf("\r\n\t║\t\t MENU\t\t\t║     STATE      ║");
	printf("\r\n\t║\t\t\t\t\t║\t\t ║");
	printf("\r\n\t╠═══════════════════════════════════════╬════════════════╣");
	printf("\r\n\t║\t\t\t\t\t║\t\t ║");
	printf("\r\n\t║\t\t Set Date \t\t║\t\t ║");
	printf("\r\n\t║\t\t Set Time \t\t║\t\t ║");
	printf("\r\n\t║\t\t Set Alarm Time ON \t║\t\t ║");
	printf("\r\n\t║\t\t Set Alarm Time OFF \t║\t\t ║");
	printf("\r\n\t║\t\t Parse string \t\t║\t\t ║");
	printf("\r\n\t║\t\t Set PWM \t\t║\t\t ║");
	printf("\r\n\t║\t\t\t\t\t║\t\t ║");
	printf("\r\n\t╠═══════════════════════════════════════╩════════════════╣");
	printf("\r\n\t║ Для перехода по пунктам меню используйте клавиши \t ║");
	printf("\r\n\t║ управления курсором UP и DOWN. \t\t\t ║");
	printf("\r\n\t║ Use the UP and DOWN cursor keys to navigate through \t ║");
	printf("\r\n\t║ the menu items. \t\t\t\t\t ║");
	printf("\r\n\t╚════════════════════════════════════════════════════════╝");
	
	// выводим состояние переменных
	printf("\x1B[24;52H LED1: %s", GPIO_PIN_SET == HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin) ? "\x1B[41mON\x1B[0m" : "OFF"); // LED1
	printf("\x1B[27;52H PWM: %u%", htim3.Instance->CCR1 / (65535 / 100)); // PWM
}

void showSetDateMemu() // меню установки даты
{
	printf("showSetDateMemu()");
}

void showSetTimeMemu() // меню установки времени
{
	printf("showSetTimeMemu()");
}

void showSetAlarmTimeONMemu() // меню установки будильника на включение светодиода
{
	printf("showSetAlarmTimeONMemu()");
}

void showSetAlarmTimeOFFMemu() // меню установки будильника на выключение светодиода
{
	printf("showSetAlarmTimeOFFMemu()");
}

void showParseStringMemu() // меню парсинга
{
	printf("showParseStringMemu()");
}

void showSetPWMMemu() // меню установки будильника на включение светодиода
{
	printf("\x1B[50;0H");
	printf("введи процент скважности ШИМ: ");
	uint8_t pwm;
	scanf("%u", &pwm);
	
	htim3.Instance->CCR1 = 65535 / 100 * pwm; // задаем скважность PWM в процентах
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
  MX_RTC_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  	htim3.Instance->CCR1 = 1000; // задаем начальное значение скважности PWM	
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // запускаем Ш�?М
	
	HAL_UART_Receive_IT(&huart1, (uint8_t*)uartBuf, 1); // разрешаем прерывание по приему от UART, вызов колбека после получения каждого байта
	
	showMainMemu(); // формируем главный экран в консоли
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {

	  //HAL_Delay(1000);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
  RTC_AlarmTypeDef sAlarm = {0};

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
  
  	if(HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK) // запускаем прерывание от RTC раз в секунду
  	{
    	Error_Handler();
  	}
  
	if((*(__IO uint32_t *)RCC_BDCR_RTCEN_BB) == ENABLE) // если батарейка жива и часы идут
		return; // то пропустим инициализачию часов нулями
  
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
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm B
  */
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_B;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the WakeUp
  */
  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_ERROR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED2_ERROR_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_ERROR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
  HAL_GPIO_WritePin(LED2_ERROR_GPIO_Port, LED2_ERROR_Pin, GPIO_PIN_SET); // отдельный светодиод на индикацию ошибок HAL, hard fault...
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
