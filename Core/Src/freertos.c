/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "spi.h"
#include "usart.h"
#include "rtc.h"
#include "tim.h"
#include "adc.h"
#include "unistd.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRUE 1
#define FALSE 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

typedef struct uart_buf_t {
    uint8_t rec_loaded;
    uint8_t send_loaded;
    uint8_t send_buf[25];
    uint8_t rec_buf[25];
    uint8_t rec_size;
} uart_buf_t;

typedef struct display_buf_t {
    uint8_t L1_new_data;
    uint8_t L1_buf[10];
    uint8_t L2_new_data;
    uint8_t L2_buf[10];
    uint8_t L3_new_data;
    uint8_t L3_buf[10];
    uint8_t L4_new_data;
    uint8_t L4_buf[10];
} display_buf_t;

uart_buf_t *uart_buf = &(uart_buf_t) {
    .rec_loaded = FALSE,
    .send_loaded = FALSE,
    .send_buf = {0},
    .rec_buf = {0},
    .rec_size = 0
};

display_buf_t *disp_buf = &(display_buf_t) {
    .L1_new_data = FALSE,
    .L1_buf = {0},
    .L2_new_data = FALSE,
    .L2_buf = {0},
    .L3_new_data = FALSE,
    .L3_buf = {0},
    .L4_new_data = FALSE,
    .L4_buf = {0}
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for digitalClock */
osThreadId_t digitalClockHandle;
const osThreadAttr_t digitalClock_attributes = {
  .name = "digitalClock",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for backlight */
osThreadId_t backlightHandle;
const osThreadAttr_t backlight_attributes = {
  .name = "backlight",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for display */
osThreadId_t displayHandle;
const osThreadAttr_t display_attributes = {
  .name = "display",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for uart */
osThreadId_t uartHandle;
const osThreadAttr_t uart_attributes = {
  .name = "uart",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void digitalClockEntry(void *argument);
void backlightEntry(void *argument);
void displayEntry(void *argument);
void uartEntry(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of digitalClock */
  digitalClockHandle = osThreadNew(digitalClockEntry, NULL, &digitalClock_attributes);

  /* creation of backlight */
  backlightHandle = osThreadNew(backlightEntry, NULL, &backlight_attributes);

  /* creation of display */
  displayHandle = osThreadNew(displayEntry, NULL, &display_attributes);

  /* creation of uart */
  uartHandle = osThreadNew(uartEntry, NULL, &uart_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    while(1) {
        osDelay(1);
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_digitalClockEntry */
/**
* @brief Function implementing the digitalClock thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_digitalClockEntry */
void digitalClockEntry(void *argument)
{
  /* USER CODE BEGIN digitalClockEntry */
    TickType_t xPeriod = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime;
    uint8_t user_prompt[] = "set time as HH:MM:SS\r\n";
    uint8_t current_time[8]; //HH:MM:SS

    taskENTER_CRITICAL();
    memcpy(uart_buf->send_buf, user_prompt, strlen((char*)user_prompt));
    taskEXIT_CRITICAL();

    uart_buf->send_loaded = TRUE;
    uart_buf->rec_size = 8;

    while(uart_buf->rec_loaded == FALSE) {
	vTaskDelay(pdMS_TO_TICKS(10));
    }

    rtc_set_time(uart_buf->rec_buf);

    xLastWakeTime = xTaskGetTickCount(); //10k
    while(1) {
	rtc_get_time(current_time);
	uart_transmit(current_time, 8);
	HAL_Delay(1);
	uart_transmit("\r\n", 2);
	memcpy(disp_buf->L1_buf, current_time, strlen((char*)current_time));
	disp_buf->L1_new_data = TRUE;
	vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
  /* USER CODE END digitalClockEntry */
}

/* USER CODE BEGIN Header_backlightEntry */
/**
* @brief Function implementing the backlight thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_backlightEntry */
void backlightEntry(void *argument)
{
  /* USER CODE BEGIN backlightEntry */
    TickType_t xPeriod = pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime;
    uint16_t voltage;

    display_set_backlight(RED);
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);

    xLastWakeTime = xTaskGetTickCount();

    while(1) {
	voltage = adc_read_pot();
	tim_set_duty_cycle(voltage);
	vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
  /* USER CODE END backlightEntry */
}

/* USER CODE BEGIN Header_displayEntry */
/**
* @brief Function implementing the display thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_displayEntry */
void displayEntry(void *argument)
{
  /* USER CODE BEGIN displayEntry */
    TickType_t xPeriod = pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime;

    display_init();
    xLastWakeTime = xTaskGetTickCount();
    while(1) {
	if (disp_buf->L1_new_data == TRUE) {
	    display_write_line(disp_buf->L1_buf, strlen((char*)disp_buf->L1_buf), 1);
	    disp_buf->L1_new_data = FALSE;
	}
	if (disp_buf->L2_new_data == TRUE) {
	    display_write_line(disp_buf->L2_buf, strlen((char*)disp_buf->L2_buf), 2);
	    disp_buf->L2_new_data = FALSE;
	}
	if (disp_buf->L3_new_data == TRUE) {
	    display_write_line(disp_buf->L3_buf, strlen((char*)disp_buf->L3_buf), 3);
	    disp_buf->L3_new_data = FALSE;
	}
	if (disp_buf->L4_new_data == TRUE) {
	    display_write_line(disp_buf->L4_buf, strlen((char*)disp_buf->L4_buf), 4);
	    disp_buf->L4_new_data = FALSE;
	}
	vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
  /* USER CODE END displayEntry */
}

/* USER CODE BEGIN Header_uartEntry */
/**
* @brief This thread will look for .
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uartEntry */
void uartEntry(void *argument)
{
  /* USER CODE BEGIN uartEntry */
    TickType_t xLastWakeTime;
    TickType_t xPeriod = pdMS_TO_TICKS(100);

    while (uart_buf->send_loaded == FALSE) {
	vTaskDelay(pdMS_TO_TICKS(10));
    }
    taskENTER_CRITICAL();
    uart_transmit(uart_buf->send_buf, strlen((char*)uart_buf->send_buf));
    uart_buf->send_loaded = FALSE;
    memset(uart_buf->send_buf, 0x30, 25);

    uart_receive(uart_buf->rec_buf, uart_buf->rec_size);
    uart_buf->rec_loaded = TRUE;
    uart_buf->rec_size = 0;
    taskEXIT_CRITICAL();

    xLastWakeTime = xTaskGetTickCount();
    while(1) {
       vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
  /* USER CODE END uartEntry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
