/**
 * @file        touch_ui_bsp_driver.c
 * @brief       BSP driver implementation for STM32H747I-DISCO LCD.
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

#include "touch_ui_driver.h"
#include "stm32h747i_discovery_ts.h"
#include "main.h"

static TS_State_t internal_ts_state; /* Track last read */
static TS_Init_t internal_ts_init;   /* Config used during init */

/**
 * @brief  Initializes the STM32H747I-Discovery touchscreen hardware.
 *         Sets orientation, accuracy, etc., as described in your note.
 */
static void initialize_touchscreen(void)
{
	internal_ts_init.Width = TS_MAX_WIDTH;
	internal_ts_init.Height = TS_MAX_HEIGHT;
	internal_ts_init.Orientation = TS_SWAP_XY | TS_SWAP_Y;
	internal_ts_init.Accuracy = 5U;

	if (BSP_TS_Init(0, &internal_ts_init) != BSP_ERROR_NONE)
	{
		Error_Handler();
	}

	/* By default, we do NOT enable BSP_TS_EnableIT(0). We let the user do that
	   via enable_touch_interrupt(true) if they want repeated triggers on a press. */
}

/**
 * @brief  Reads the current touch state from the hardware.
 * @param  x_position Pointer to store the X coordinate
 * @param  y_position Pointer to store the Y coordinate
 * @param  is_pressed Pointer to store press boolean
 * @param  timestamp_value Pointer to store a time value (e.g. from HAL_GetTick)
 * @return True if read is valid, false otherwise
 */
static bool read_touch_state(uint16_t *x_position,
			     uint16_t *y_position,
			     bool *is_pressed,
			     uint32_t *timestamp_value)
{
	if ((x_position == NULL) ||
	    (y_position == NULL) ||
	    (is_pressed == NULL) ||
	    (timestamp_value == NULL))
	{
		return false;
	}

	(void)BSP_TS_GetState(0, &internal_ts_state);

	/* If there's a touch: the driver sets TouchDetected != 0U */
	if (internal_ts_state.TouchDetected != 0U)
	{
		*x_position = (uint16_t)internal_ts_state.TouchX;
		*y_position = (uint16_t)internal_ts_state.TouchY;
		*is_pressed = true;
		/* Clear the detected flag if desired (but keep in mind that BSP might manage it) */
		internal_ts_state.TouchDetected = 0U;
	}
	else
	{
		/* If no new touch, we keep last X/Y */
		*x_position = (uint16_t)internal_ts_state.TouchX;
		*y_position = (uint16_t)internal_ts_state.TouchY;
		*is_pressed = false;
	}

	*timestamp_value = HAL_GetTick();

	return true; /* We “successfully read” no matter if pressed or not */
}

/**
 * @brief  Enables or disables the manual EXTI-based interrupt.
 *         If you prefer polling, you can no-op here.
 */
static void enable_touch_interrupt(bool enable_flag)
{
	if (enable_flag)
	{
		/* Possibly call BSP_TS_EnableIT(0) if you want repeated interrupts
		   while the finger remains pressed. But if you only want manual EXTI
		   once on press, you can literally do nothing here. */
	}
	else
	{
		/* Possibly disable your EXTI or do nothing again. */
	}
}

/* Define the driver struct for the board */
const touch_ui_driver_t touch_ui_bsp_driver = {
    .initialize = initialize_touchscreen,
    .read_touch_state = read_touch_state,
    .enable_interrupt = enable_touch_interrupt,
};