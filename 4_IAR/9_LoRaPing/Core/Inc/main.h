/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stdint.h"
#include "stdbool.h"
#include "SEGGER_RTT.h"	// библиотека для отладочного RTT вывода через интерфейс отладчика
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SPI_HandleTypeDef hspi2;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define LED1_ON()   	LED1_GPIO_Port->BSRR = (uint32_t)LED1_Pin
#define LED1_OFF()  	LED1_GPIO_Port->BSRR = (uint32_t)LED1_Pin << 16U;
#define LED2_ON()   	LED2_GPIO_Port->BSRR = (uint32_t)LED2_Pin
#define LED2_OFF()  	LED2_GPIO_Port->BSRR = (uint32_t)LED2_Pin << 16U;
#define LED3_ON()   	LED3_GPIO_Port->BSRR = (uint32_t)LED3_Pin
#define LED3_OFF()  	LED3_GPIO_Port->BSRR = (uint32_t)LED3_Pin << 16U;

										/* макросы для RTT */
#ifdef SEGGER_RTT_H
#define LOG_START()					SEGGER_RTT_WriteString(0, RTT_CTRL_CLEAR RTT_CTRL_RESET RTT_CTRL_TEXT_BRIGHT_GREEN); 		/* очистка терминала и установка стандартных параметров оформления */
#define LOG_FILE()					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW "\r\n\r\nFILE: " __FILE__ RTT_CTRL_TEXT_BRIGHT_GREEN); 	/* путь к текущему файлу */
#define LOGT(text)					SEGGER_RTT_WriteString(0, text); 										/* простой вывод текста */
#define LOGF(format, args...)			        SEGGER_RTT_printf(0, "\r\n%d " format, __LINE__, args); 							/* форматирваный вывод текста с номером строки в нулевой терминал */
#define LOG(text)					SEGGER_RTT_printf(0, "\r\n%d %s", __LINE__, text); 								/* вывод текста с номером строки в нулевой терминал */
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
//#define LOG_LCD(L1, L2)					SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_CYAN	"\r\n\r\n\t +------------------+"); SEGGER_RTT_printf(0,  "\r\n\t | %16.16s |", L1); SEGGER_RTT_printf(0, "\r\n\t | %16.16s |", L2); SEGGER_RTT_WriteString(0, "\r\n\t +------------------+\r\n" RTT_CTRL_TEXT_BRIGHT_GREEN);
#endif
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SX_MISO_Pin GPIO_PIN_2
#define SX_MISO_GPIO_Port GPIOC
#define SX_MOSI_Pin GPIO_PIN_3
#define SX_MOSI_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_5
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_6
#define LED3_GPIO_Port GPIOA
#define SD_POWER_Pin GPIO_PIN_0
#define SD_POWER_GPIO_Port GPIOB
#define SD_DETECT_Pin GPIO_PIN_1
#define SD_DETECT_GPIO_Port GPIOB
#define SX_RFSW_Pin GPIO_PIN_2
#define SX_RFSW_GPIO_Port GPIOB
#define SX_SCK_Pin GPIO_PIN_10
#define SX_SCK_GPIO_Port GPIOB
#define SX_NSS_Pin GPIO_PIN_12
#define SX_NSS_GPIO_Port GPIOB
#define SX_NRST_Pin GPIO_PIN_13
#define SX_NRST_GPIO_Port GPIOB
#define SX_BUSY_Pin GPIO_PIN_14
#define SX_BUSY_GPIO_Port GPIOB
#define SX_DIO1_Pin GPIO_PIN_15
#define SX_DIO1_GPIO_Port GPIOB
#define USB_DISCONNECT_Pin GPIO_PIN_5
#define USB_DISCONNECT_GPIO_Port GPIOB
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
