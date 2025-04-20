/**
 * @file        touch_ui_driver.h
 * @brief       Public interface linking the touch library to the board driver.
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

#ifndef TOUCH_UI_DRIVER_H
#define TOUCH_UI_DRIVER_H

#include "touch_ui.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

	/**
	 * @brief Extern a board-specific driver instance, e.g.:
	 *        extern const touch_ui_driver_t my_board_touch_driver;
	 */
	extern const touch_ui_driver_t touch_ui_bsp_driver;

#ifdef __cplusplus
}
#endif

#endif /* TOUCH_UI_DRIVER_H */
