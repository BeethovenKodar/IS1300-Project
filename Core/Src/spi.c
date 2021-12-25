/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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

/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

SPI_HandleTypeDef hspi2;

/* SPI2 init function */
void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* SPI2 clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PC3     ------> SPI2_MOSI
    PB10     ------> SPI2_SCK
    PB12     ------> SPI2_NSS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_MspInit 1 */

  /* USER CODE END SPI2_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();

    /**SPI2 GPIO Configuration
    PC3     ------> SPI2_MOSI
    PB10     ------> SPI2_SCK
    PB12     ------> SPI2_NSS
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_12);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint8_t instr[3] = {0x0F, 0, 0};

const uint8_t DDRAM_L[4] = {0x80, 0xA0, 0xC0, 0xE0};
const uint8_t RED = 0;
const uint8_t WHITE = 1;
const uint8_t GREEN = 2;
const uint8_t RS0_RW0 = 0x1;
const uint8_t RS1_RW0 = 0x5;
const uint16_t CLEAR_DISPLAY = 0x01;



uint8_t init_seq[11] = {
    0x3A, //0x1F0A03	#function set: 	   8 bit data length (RE = 1, REV = 0)
    0x09, //0x1F0900	#ext function set: 4 line display
    0x06, //0x1f0600	#entry mode set:   bottom view
    0x1E, //0x1f0E01	#bias setting:	   BS1 = 1
    0x39, //0x1f0903	#function set:	   8 bit data length (RE = 0, IS = 1)
    0x1B, //0x1f0B01	#internal osc:	   BS0 = 1 -> Bias = 1/6
    0x6E, //0x1f0E06	#follower control: devider on and set value
    0x56, //0x1f0605	#power control:    booster on and set contrast
    0x7A, //0x1f0A07	#contrast set:	   set contrast (DB3-DB0 = C3-C0)
    0x38, //0x1f0803	#functions set:	   8 bit data length (RE = 0, IS = 0)
    0x0C, //0x1f0E00	#display on:	   display on, cursor on, blink on
};


void SPI_Error() {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

/**
  * @brief Set the R/W bit and RS bit in the start byte.
  * @param nibble: the sequence to set.
  * @note  nibble in format of "0|RS|R/W|1" (MSB first representation).
  * Placed at bits 4-7 in the startbyte.
  * @retval None
  */
void set_startbyte(uint8_t nibble) {
    instr[0] = ((instr[0] & 0x0F) | (nibble << 4));
}

/**
  * @brief Set the data byte to send, split into two separate bytes
  * to follow convention.
  * @param byte: the byte to set up transmission for
  * @note LSB first ordering in the instruction array instr[].
  * @retval None
  */
void set_byte(uint8_t byte) {
    instr[1] = (byte & 0x0F);
    instr[2] = ((byte & 0xF0) >> 4);
}

/**
 * @brief This functions sets the backlight color of the display.
 * @param color: color mapped to an integer used in the switch statement below.
 */
void display_set_backlight(uint8_t color) {
    switch(color) {
	case 0:
	  HAL_GPIO_WritePin(Red_Backlight_GPIO_Port, Red_Backlight_Pin, GPIO_PIN_SET);
	  break;
	case 1:
	  HAL_GPIO_WritePin(White_Backlight_GPIO_Port, White_Backlight_Pin, GPIO_PIN_SET);
	  break;
	case 2:
	  HAL_GPIO_WritePin(Green_Backlight_GPIO_Port, Green_Backlight_Pin, GPIO_PIN_SET);
	  break;
    }
}

/**
 * @brief Before using the display it should be reset by toggling the
 * Disp_Reset pin: high->low->high with sufficient delays in between.
 */
void display_hw_reset() {
    HAL_Delay(5);
    HAL_GPIO_WritePin(Disp_Reset_GPIO_Port, Disp_Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(Disp_Reset_GPIO_Port, Disp_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

/**
 * @brief sends the current data in the instructions array instr[] over SPI.
 */
void send() {
    if (HAL_SPI_Transmit(&hspi2, (uint8_t*)instr, 3, 50) != HAL_OK) {
	SPI_Error();
    }
}

/**
 * @brief Sets all segments of the display to 20H, i.e. invisible character.
 */
void display_clear() {
    set_startbyte(RS0_RW0);
    set_byte(CLEAR_DISPLAY);
    send();
}

/**
 * @brief Initialization routine to set up the running conditions for the display.
 * @note Sequence needed is predefined. Hardware reset needed first.
 */
void display_init() {

    display_hw_reset();

    display_set_backlight(WHITE);

    set_startbyte(RS0_RW0);
    for (int i = 0; i < 11; i++) {
	set_byte(init_seq[i]);
	send();
	HAL_Delay(1);
    }

    display_clear();
}

void display_set_line(uint8_t line) {
    set_startbyte(RS0_RW0);
    set_byte(DDRAM_L[line - 1]);
    send();
}

void display_write(uint8_t c) {
    set_startbyte(RS1_RW0);
    set_byte(c);
    send();
}

void display_write_line(uint8_t *buf, uint8_t len, uint8_t line) {
    display_set_line(line);
    uint8_t i = 0;
    while (i < len) {
	display_write(buf[i++]);
	HAL_Delay(1);
    }
    while (i < 10) {
	display_write((uint8_t) 0x20);	//set remaining segments to blank
	i++;
    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
