/*
 * test.c
 *
 *  Created on: Dec 28, 2021
 *      Author: ludvigsmacbook
 */

/* includes */
#include "test.h"
#include "spi.h"
#include "tim.h"
#include "adc.h"


/**
 * @brief Testing that the PWM manipulation works.
 * @note Potentiometer value ranges between 0 - 4067.
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
 * @note PWM for red backlight is tested separatly.
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
 * line in case fewer segments are needed. Also resets a line.
 * Visual rest result.
 */
void test_display() {
    display_init();

    test_PWM();

    test_backlight();

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
    display_write_line((uint8_t*)"   DONE", 7, 2);
    HAL_Delay(1000);
    display_write_line((uint8_t*)"", 0, 2);
}

void main_test() {
    test_display();
}

