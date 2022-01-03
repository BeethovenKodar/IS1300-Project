/**
  ******************************************************************************
  * @file           test.c
  * @brief          Testing of all components of included in the main program.
  ******************************************************************************
  * @author	    Ludvig Larsson
  ******************************************************************************
  */

/* includes */
#include "test.h"
#include "spi.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include "rtc.h"
#include "string.h"

/**
 * @brief Testing that RTC time is correctly set and
 * counts correctly. Visual result shown over uart.
 * @note UART needs to be working beforehand.
 * @retval None.
 */
void test_rtc() {
    uint8_t timestr1[] = "23:59:58";
    uint8_t currtime[8];

    rtc_set_time(timestr1);
    uint8_t i = 0;
    while (i < 5) {
	HAL_Delay(999);
	rtc_get_time(currtime);
	uart_transmit(currtime, strlen((char*)currtime));
	HAL_Delay(1);
	uart_transmit((uint8_t*)"\r\n", 2);
	i++;
    }
}

/**
 * @brief Testing UART transmit and receive, also feedback
 * when sending chars. Visual test result.
 * @retval None.
 */
void test_uart() {
    uint8_t buf[] = "Hello testing testing\r\nEnter 10 chars:\r\n";
    uint8_t mesrec[] = "Message recieved: ";

    uint8_t rec[25];

    uart_transmit(buf, strlen((char*)buf));
    HAL_Delay(1);
    uart_receive(rec, 10);
    HAL_Delay(1);
    uart_transmit(mesrec, strlen((char*)mesrec));
    HAL_Delay(1);
    uart_transmit(rec, strlen((char*)rec));
}

/**
 * @brief Testing that the PWM duty cycle works. Visual test result.
 * @note Potentiometer value ranges between 0 - 4067.
 * @retval None.
 */
void test_PWM() {
    display_set_backlight(RED);
    uint16_t high_value = 4000;
    uint16_t med_value = 2000;
    uint16_t low_value = 100;
    tim_set_duty_cycle(high_value);
    HAL_Delay(1000);
    tim_set_duty_cycle(med_value);
    HAL_Delay(1000);
    tim_set_duty_cycle(low_value);
    HAL_Delay(1000);
    tim_set_duty_cycle(med_value);
    HAL_Delay(1000);
    tim_set_duty_cycle(high_value);
}

/**
 * @brief Ensure that backlight can take colors white and green.
 * Visual test result. Tests that multiple colors can be active
 * simultaneously.
 * @note PWM for red backlight is tested manually via the potentiometer.
 * @retval None.
 */
void test_backlight() {
    display_set_backlight(WHITE);
    HAL_Delay(1000);
    display_reset_backlight();
    display_set_backlight(GREEN);
    HAL_Delay(1000);
    display_reset_backlight();
    display_set_backlight(GREEN);
    display_set_backlight(WHITE);
    HAL_Delay(1000);
    display_reset_backlight();
}

/**
 * @brief Initializes the display and targets all 4 lines with content.
 * Tries to overwrite previous data to ensure no old data is left on a
 * line in case fewer segments are needed. Also resets lines.
 * Visual rest result.
 * @retval None.
 */
void test_display() {
    display_init();

    display_set_backlight(WHITE);
    uint8_t longstr[] = "hello12345";
    uint8_t shortstr[] = "hello";
    display_write_line(longstr, 10, 1);
    HAL_Delay(1);
    display_write_line(longstr, 10, 2);
    HAL_Delay(1);
    display_write_line(longstr, 10, 3);
    HAL_Delay(1);
    display_write_line(longstr, 10, 4);
    HAL_Delay(1000);
    display_write_line(shortstr, 5, 1);
    HAL_Delay(1);
    display_write_line(shortstr, 5, 2);
    HAL_Delay(1000);
    display_write_line((uint8_t*)"", 0, 1);
    HAL_Delay(1);
    display_write_line((uint8_t*)"", 0, 2);
    HAL_Delay(1);
    display_write_line((uint8_t*)"", 0, 3);
    HAL_Delay(1);
    display_write_line((uint8_t*)"", 0, 4);
    HAL_Delay(1000);

    test_backlight();

    test_PWM();
}

/**
 * @brief Entry point for all tests.
 * @retval None.
 */
void main_test() {
    test_uart();
    test_rtc();
    test_display();

    display_write_line((uint8_t*)"   DONE", 7, 2);
}

