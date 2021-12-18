/*
 * uartcom.c
 *
 *  Created on: Dec 18, 2021
 *      Author: ludvigsmacbook
 */


/* private defines */
#define BUFFERSIZE 22

/* private variables */
static ITStatus UartDoneT = NULL;
static ITStatus UartDoneR = NULL;
static uint8_t Buffer[] = "Hello World interrupt!";

/**
  * @brief UART transmission initializer
  * @param buffer: the buffer to use
  * @note Reporting that the tranmission over UART is complete.
  * @retval None
  */
void uart_transmit(uint8_t *buffer) {
    /* uart in reception process */
    UartDoneT = RESET;
    if (HAL_UART_Transmit_IT(&huart5, (uint8_t*)Buffer, BUFFERSIZE) != HAL_OK) {
	Error_Handler();
    }
}

/**
  * @brief UART reception initializer
  * @param buffer: the buffer to use
  * @note Starting the interrupt handler for receiving.
  * @retval None
  */
void uart_receive(uint8_t *buffer) {
    /* uart in reception process */
    UartDoneR = RESET;
    if (HAL_UART_Receive_IT(&huart5, (uint8_t*)Buffer, BUFFERSIZE) != HAL_OK) {
	Error_Handler();
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
    UartReady = SET;
}
