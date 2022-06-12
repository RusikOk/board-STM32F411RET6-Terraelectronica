/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"	        // библиотека для отладочного RTT вывода через интерфейс отладчика
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
										/* настройки проэкта */
#define displaySaverINTERVAL                            10 * 60 * 1000 / portTICK_RATE_MS /* выполнение раз в 5 минут */
        
										/* мои определения и макросы */
#define true                                            1
#define false						0
        
										/* настройка голых портов ввода/вывода */
#define LED1_ON()   				        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define LED1_OFF()  				        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_TOG()  				        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)
#define LED2_ON()   				        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define LED2_OFF()  				        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_TOG()  				        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin)
#define LED3_ON()   				        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)
#define LED3_OFF()  				        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_TOG()  				        HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin)
#define LED4_ON()   				        HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET)
#define LED4_OFF()  				        HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET)
#define LED4_TOG()  				        HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin)
        
#define BTN1_PRESSED()                                  !HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin)
#define BTN2_PRESSED()                                  !HAL_GPIO_ReadPin(BTN2_GPIO_Port, BTN2_Pin)

										/* макросы для RTT */
#ifdef SEGGER_RTT_H
#define LOG_START()					SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR RTT_CTRL_RESET RTT_CTRL_TEXT_BRIGHT_GREEN); 		/* очистка терминала и установка стандартных параметров оформления */
#define LOG_FILE()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW "\r\n\r\nFILE: " __FILE__ RTT_CTRL_TEXT_BRIGHT_GREEN); 	/* путь к текущему файлу */
#define LOGT(text)					SEGGER_RTT_WriteString(0, text); 															/* простой вывод текста */
#define LOGF(format, args...)			        SEGGER_RTT_printf(0, "\r\n%d " format, __LINE__, args); 											/* форматирваный вывод текста с номером строки в нулевой терминал */
#define LOG(text)					SEGGER_RTT_printf(0, "\r\n%d %s", __LINE__, text); 											/* вывод текста с номером строки в нулевой терминал */
#define LOG0(text)					SEGGER_RTT_printf(0, "\r\n%d %s", __LINE__, text);
#define LOG1(text)					SEGGER_RTT_printf(1, "\r\n%d %s", __LINE__, text);	 
#define LOG2(text)					SEGGER_RTT_printf(2, "\r\n%d %s", __LINE__, text);
#define LOG3(text)					SEGGER_RTT_printf(3, "\r\n%d %s", __LINE__, text);
#define LOG4(text)					SEGGER_RTT_printf(4, "\r\n%d %s", __LINE__, text);
#define LOG5(text)					SEGGER_RTT_printf(5, "\r\n%d %s", __LINE__, text);
#define LOG_NMI()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |       NMI        |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_PVD()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |       PVD        |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_HARDFAULT()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |    HARD FAULT    |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_MEMMANAGE()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |    MEM MANAGE    |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_BUSFAULT()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |    BUS  FAULT    |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_USAGEFAULT()				SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |    USAGE FAULT   |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_HALERROR()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |    HAL  ERROR    |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#define LOG_POWEROFF()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED	"\r\n\r\n\t +------------------+\r\n\t |    POWER  OFF    |\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#endif
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_6
#define LED3_GPIO_Port GPIOA
#define LED4_Pin GPIO_PIN_0
#define LED4_GPIO_Port GPIOB
#define BTN1_Pin GPIO_PIN_6
#define BTN1_GPIO_Port GPIOB
#define BTN2_Pin GPIO_PIN_7
#define BTN2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
