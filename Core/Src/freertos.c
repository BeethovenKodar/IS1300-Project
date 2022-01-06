/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           freertos.c
  * @brief          Thread implementations for the main program.
  ******************************************************************************
  * @author	    Ludvig Larsson
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

/* middleman between uart and tasks */
typedef struct uart_buf_t {
    uint8_t rec_loaded;
    uint8_t send_loaded;
    uint8_t send_buf[25];
    uint8_t rec_buf[25];
    uint8_t rec_size;
} uart_buf_t;

/* middleman between display and tasks */
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

/* only uart buffer instance */
uart_buf_t *uart_buf = &(uart_buf_t) {
    .rec_loaded = FALSE,
    .send_loaded = FALSE,
    .send_buf = {0},
    .rec_buf = {0},
    .rec_size = 0
};

/* only display buffer instance */
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
/* Definitions for uartTxMutex */
osMutexId_t uartTxMutexHandle;
const osMutexAttr_t uartTxMutex_attributes = {
  .name = "uartTxMutex"
};
/* Definitions for dispL1mutex */
osMutexId_t dispL1mutexHandle;
const osMutexAttr_t dispL1mutex_attributes = {
  .name = "dispL1mutex"
};
/* Definitions for uartRxMutex */
osMutexId_t uartRxMutexHandle;
const osMutexAttr_t uartRxMutex_attributes = {
  .name = "uartRxMutex"
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
  /* Create the mutex(es) */
  /* creation of uartTxMutex */
  uartTxMutexHandle = osMutexNew(&uartTxMutex_attributes);

  /* creation of dispL1mutex */
  dispL1mutexHandle = osMutexNew(&dispL1mutex_attributes);

  /* creation of uartRxMutex */
  uartRxMutexHandle = osMutexNew(&uartRxMutex_attributes);

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
  * @brief  Dummy thread (not removable), only executes when
  * all other threads are idle.
  * @param  argument[in]: not used.
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
* @brief This thread is responsible for the time keeping for the RTC clock. It
* places data in both the uart and display buffer to handle the task to update
* the display each second with the RTC time.
* @param argument[in]: not used.
*/
/* USER CODE END Header_digitalClockEntry */
void digitalClockEntry(void *argument)
{
  /* USER CODE BEGIN digitalClockEntry */
    TickType_t xPeriod = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime;
    uint8_t user_prompt[] = "set time as HH:MM:SS\r\n";
    uint8_t time_rec[] = "\r\nClock started!\r\n";
    uint8_t current_time[8]; //HH:MM:SS

    osMutexWait(uartTxMutexHandle, osWaitForever); //wait for send lock
    taskENTER_CRITICAL();
    memcpy(uart_buf->send_buf, user_prompt, strlen((char*)user_prompt));
    taskEXIT_CRITICAL();
    uart_buf->send_loaded = TRUE;
    osMutexRelease(uartTxMutexHandle); //release send lock

    osMutexWait(uartRxMutexHandle, osWaitForever); //wait for receive lock
    uart_buf->rec_size = 8;

    while(uart_buf->rec_loaded == FALSE) {
	vTaskDelay(pdMS_TO_TICKS(10));
    }
    rtc_set_time(uart_buf->rec_buf);
    vTaskDelay(pdMS_TO_TICKS(1000)); //re-schedule to let clock tick once
    taskENTER_CRITICAL();
    memset(uart_buf->rec_buf, 0, 25); //reset buffer
    taskEXIT_CRITICAL();
    uart_buf->rec_loaded = FALSE;
    uart_buf->rec_size = 0;
    osMutexRelease(uartRxMutexHandle); //release receive lock

    osMutexWait(uartTxMutexHandle, osWaitForever); //wait for send lock
    taskENTER_CRITICAL();
    memcpy(uart_buf->send_buf, time_rec, strlen((char*)time_rec));
    taskEXIT_CRITICAL();
    uart_buf->send_loaded = TRUE;
    osMutexRelease(uartTxMutexHandle); //release send lock

    xLastWakeTime = xTaskGetTickCount();
    while(1) {
	rtc_get_time(current_time);
	osMutexWait(dispL1mutexHandle, osWaitForever); //wait for disp L1 lock
	taskENTER_CRITICAL();
	memcpy(disp_buf->L1_buf, current_time, strlen((char*)current_time));
	taskEXIT_CRITICAL();
	disp_buf->L1_new_data = TRUE;
	osMutexRelease(dispL1mutexHandle); //release disp L1 lock
	vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
  /* USER CODE END digitalClockEntry */
}

/* USER CODE BEGIN Header_backlightEntry */
/**
* @brief This thread checks continously the potentiometer value and
* adjusts the red display backlight accordingly.
* @param argument[in]: not used.
*/
/* USER CODE END Header_backlightEntry */
void backlightEntry(void *argument)
{
  /* USER CODE BEGIN backlightEntry */
    TickType_t xPeriod = pdMS_TO_TICKS(100);
    TickType_t xLastWakeTime;
    uint16_t voltage;

//    display_set_backlight(WHITE);
//    display_set_backlight(GREEN);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2); //red backlight
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

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
* @brief This thread will look for new data in each display buffer
* and update the display if current segments are outdated.
* @param argument[in]: not used.
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
* @brief This thread will check the uart buffer (transmit and receive) to eventually
* start transmitting or receiving for a reqesting thread.
* @param argument[in]: not used.
*/
/* USER CODE END Header_uartEntry */
void uartEntry(void *argument)
{
  /* USER CODE BEGIN uartEntry */
    TickType_t xLastWakeTime;
    TickType_t xPeriod = pdMS_TO_TICKS(100);

    xLastWakeTime = xTaskGetTickCount();
    while(1) {
	if (uart_buf->send_loaded == TRUE) { //a thread wants to send data
	    taskENTER_CRITICAL();
	    uart_transmit(uart_buf->send_buf, strlen((char*)uart_buf->send_buf));
	    memset(uart_buf->send_buf, 0x0, 25); //reset buffer
	    taskEXIT_CRITICAL();
	    uart_buf->send_loaded = FALSE;
	}

	if (uart_buf->rec_size != 0) { //a thread has requested data
	    taskENTER_CRITICAL();
	    uart_receive(uart_buf->rec_buf, uart_buf->rec_size);
	    taskEXIT_CRITICAL();
	    uart_buf->rec_loaded = TRUE;
	    uart_buf->rec_size = 0;
	}
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
  /* USER CODE END uartEntry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
