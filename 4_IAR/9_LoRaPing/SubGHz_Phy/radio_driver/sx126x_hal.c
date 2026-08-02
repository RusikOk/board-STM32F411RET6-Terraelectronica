/**
 * @file      sx126x_hal.c
 *
 * @brief     Implements the SX126x radio HAL functions
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2022. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sx126x_hal.h"
//#include "sx126x_hal_context.h"
#include "sx126x_hal.h"
//#include "smtc_hal_mcu_spi.h"
//#include "smtc_hal_mcu_gpio.h"
//#include "smtc_hal_mcu.h"

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/**
 * @brief Wait until radio busy pin returns to 0
 */
void sx126x_hal_wait_on_busy( const void* radio );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

sx126x_hal_status_t sx126x_hal_reset( const void* context )
{
	HAL_GPIO_WritePin(SX_NRST_GPIO_Port, SX_NRST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SX_NRST_GPIO_Port, SX_NRST_Pin, GPIO_PIN_SET);
	return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context )
{
	HAL_GPIO_WritePin(SX_NSS_GPIO_Port, SX_NSS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(SX_NSS_GPIO_Port, SX_NSS_Pin, GPIO_PIN_SET);
	return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length, const uint8_t* data, const uint16_t data_length )
{
	sx126x_hal_wait_on_busy(context);
	HAL_GPIO_WritePin(SX_NSS_GPIO_Port, SX_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, command, command_length, 10);
	HAL_SPI_Transmit(&hspi2, data, data_length, 10);
	HAL_GPIO_WritePin(SX_NSS_GPIO_Port, SX_NSS_Pin, GPIO_PIN_SET);
	return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length, uint8_t* data, const uint16_t data_length )
{
	sx126x_hal_wait_on_busy(context);
	HAL_GPIO_WritePin(SX_NSS_GPIO_Port, SX_NSS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, command, command_length, 10);
	HAL_SPI_Receive(&hspi2, data, data_length, 10);
	HAL_GPIO_WritePin(SX_NSS_GPIO_Port, SX_NSS_Pin, GPIO_PIN_SET);
	return SX126X_HAL_STATUS_OK;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

void sx126x_hal_wait_on_busy( const void* radio )
{
	GPIO_PinState gpio_state;
	do
	{
		gpio_state = HAL_GPIO_ReadPin(SX_BUSY_GPIO_Port, SX_BUSY_Pin);
	} while(GPIO_PIN_SET == gpio_state);
}

/* --- EOF ------------------------------------------------------------------ */
