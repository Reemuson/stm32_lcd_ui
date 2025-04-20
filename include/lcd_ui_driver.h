/**
 * @file        lcd_ui_driver.h
 * @brief       Public interface linking the UI library to the board driver.
 *
 * @author      Ryan Hicks <c3361231@uon.edu.au>
 *              School of Engineering (Electrical and Computer Engineering)
 *              University of Newcastle
 *
 * @date        2025-04-11
 * @version     1.0.0
 *
 * @copyright   Copyright (c) 2025 Ryan
 * @license     SPDX-License-Identifier: MIT
 *
 * @note        Redistribution and use permitted with attribution.
 */

#ifndef LCD_UI_DRIVER_H
#define LCD_UI_DRIVER_H

#include "lcd_ui.h"

#ifdef __cplusplus
extern "C"
{
#endif

        /**
         * @brief BSP-specific driver instance for STM32H747I-DISCO LCD.
         */
        extern const lcd_ui_driver_t lcd_ui_bsp_driver;

#ifdef __cplusplus
}
#endif

#endif // LCD_UI_DRIVER_H