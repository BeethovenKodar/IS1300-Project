/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stdint.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart5;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_UART5_Init(void);

/* USER CODE BEGIN Prototypes */
/**
  * @brief UART transmission initializer
  * @param buffer: the buffer to use
  * @note Starting the interrupt handler for transmitting.
  * @retval None
  */
void uart_transmit(uint8_t buffer[], uint16_t size);

/**
  * @brief UART reception initializer
  * @param buffer: the buffer to use
  * @note Starting the interrupt handler for receiving.
  * @retval None
  */
void uart_receive(uint8_t buffer[], uint16_t size);

/**
  * @brief UART transfer competed callback
  * @param UartHandle: UART handle
  * @note Reporting that the tranmission over UART is complete.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);

/**
* @brief UART reception completed callback
* @param UartHandle: UART handle
* @note Reporting that the reception over UART is complete.
* @retval None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);

/**
  * @brief  This function is executed in case of error occurrence by own implementations over UART.
  * @retval None
  */
void Error_Uart(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
