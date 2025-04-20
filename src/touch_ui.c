/**
 * @file        touch_ui.c
 * @brief       Implementation of a simple touchscreen library for STM32 LCD displays.
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

#include "touch_ui.h"

void touch_ui_init(touch_ui_context_t *context, const touch_ui_driver_t *driver)
{
	if (!context || !driver)
		return;

	driver->initialize();
	context->driver = driver;

	context->internal_state.last_press_state = false;
	context->internal_state.last_x_position = 0U;
	context->internal_state.last_y_position = 0U;
	context->internal_state.last_press_timestamp_value = 0U;
}

touch_ui_event_t touch_ui_process_input(touch_ui_context_t *context,
					uint16_t x_position,
					uint16_t y_position,
					bool is_pressed,
					uint32_t timestamp_value)
{
	touch_ui_event_t event_result;
	bool was_pressed;
	bool press_changed;

	event_result.event_type = touch_ui_event_none;
	event_result.x_position = x_position;
	event_result.y_position = y_position;
	event_result.timestamp_value = timestamp_value;

	if (context == 0)
	{
		return event_result;
	}

	was_pressed = context->internal_state.last_press_state;
	press_changed = (was_pressed != is_pressed);

	/* If press state changed from up to down => pressed event. */
	if ((!was_pressed) && (is_pressed))
	{
		event_result.event_type = touch_ui_event_pressed;
		context->internal_state.last_x_position = x_position;
		context->internal_state.last_y_position = y_position;
		context->internal_state.last_press_timestamp_value = timestamp_value;
	}
	/* If press state changed from down to up => released event. */
	else if ((was_pressed) && (!is_pressed))
	{
		event_result.event_type = touch_ui_event_released;
	}

	/* If press is still down, but x/y changed => moved event. */
	else if (is_pressed && !press_changed)
	{
		if ((x_position != context->internal_state.last_x_position) ||
		    (y_position != context->internal_state.last_y_position))
		{
			event_result.event_type = touch_ui_event_moved;
		}
	}

	/*
	 * Additional logic for hold/double-tap can go here.
	 * For instance, if is_pressed == true for > 500ms => event_type = touch_ui_event_held.
	 */

	/* Store new state in the context. */
	context->internal_state.last_press_state = is_pressed;
	if (is_pressed)
	{
		context->internal_state.last_x_position = x_position;
		context->internal_state.last_y_position = y_position;
		/* Possibly update last_press_timestamp_value if needed for hold. */
	}

	return event_result;
}

bool touch_ui_read_state(touch_ui_context_t *context,
			 uint16_t *x,
			 uint16_t *y,
			 bool *pressed,
			 uint32_t *timestamp)
{
	if (!context || !context->driver || !context->driver->read_touch_state)
		return false;

	return context->driver->read_touch_state(x, y, pressed, timestamp);
}