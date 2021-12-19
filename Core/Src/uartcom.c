/*
 * uartcom.c
 *
 *  Created on: Dec 18, 2021
 *      Author: ludvigsmacbook
 */


/* includes */
#include "uartcom.h"

/* private defines */
#define BUFFERSIZE 22

/* private variables */
static ITStatus UartDoneT = SET;
static ITStatus UartDoneR = SET;

/**
  * @brief UART transmission initializer
  * @param buffer: the buffer to use
  * @note Starting the interrupt handler for transmitting.
  * @retval None
  */
void uart_transmit(uint8_t buffer[], uint16_t size) {
    if (UartDoneR == SET) {
	/* uart in reception process */
	UartDoneR = RESET;
	UartDoneT = RESET;
	if (HAL_UART_Transmit_IT(&huart5, (uint8_t*)buffer, size) != HAL_OK) {
	    Error_Uart();
	}
    }
}

/**
  * @brief UART reception initializer
  * @param buffer: the buffer to use
  * @note Starting the interrupt handler for receiving.
  * @retval None
  */
void uart_receive(uint8_t buffer[], uint16_t size) {
    if (UartDoneT == SET) {
	/* uart in reception process */
	UartDoneR = RESET;
	UartDoneT = RESET;
	if (HAL_UART_Receive_IT(&huart5, (uint8_t*)buffer, size) != HAL_OK) {
	    Error_Uart();
	}
    }
}

/**
  * @brief UART transfer competed callback
  * @param UartHandle: UART handle
  * @note Reporting that the tranmission over UART is complete.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
    UartDoneT = SET;
}

/**
* @brief UART reception completed callback
* @param UartHandle: UART handle
* @note Reporting that the reception over UART is complete.
* @retval None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
    UartDoneR = SET;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Uart(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add an own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    }
    /* USER CODE END Error_Handler_Debug */
}
