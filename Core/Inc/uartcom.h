/*
 * uartcom.h
 *
 *  Created on: Dec 18, 2021
 *      Author: ludvigsmacbook
 */

#ifndef INC_UARTCOM_H_
  #define INC_UARTCOM_H_
#endif /* INC_UARTCOM_H_ */


/* function prototypes */
void uart_transmit(uint8_t *buffer);
void uart_receive(uint8_t *buffer);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);
