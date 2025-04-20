/**
 * @file        touch_ui.h
 * @brief       Public interface for LCD touch library.
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

#ifndef TOUCH_UI_H
#define TOUCH_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

	/**
	 * @brief Board- or hardware-specific driver interface for one-finger touch input.
	 */
	typedef struct touch_ui_driver_t
	{
		/**
		 * @brief Initializes the underlying touchscreen hardware.
		 *        Possibly sets up pins, I2C, EXTI or timers.
		 */
		void (*initialize)(void);

		/**
		 * @brief Reads the current state of the touchscreen.
		 * @param x_position      Output: X coordinate if pressed
		 * @param y_position      Output: Y coordinate if pressed
		 * @param is_pressed      Output: True if finger is detected
		 * @param timestamp_value Output: Current time in ms or ticks
		 * @return                True if read succeeded, false if error/no data
		 */
		bool (*read_touch_state)(uint16_t *x_position,
					 uint16_t *y_position,
					 bool *is_pressed,
					 uint32_t *timestamp_value);

		/**
		 * @brief Optionally enables or disables an interrupt or other callback mechanism.
		 *        If the user wants polling only, this can be a no-op.
		 */
		void (*enable_interrupt)(bool enable_flag);

	} touch_ui_driver_t;

	/**
	 * @brief Enumerates types of touch events that this library can detect.
	 */
	typedef enum touch_ui_event_type_t
	{
		touch_ui_event_none = 0,
		touch_ui_event_pressed,
		touch_ui_event_released,
		touch_ui_event_moved,
		touch_ui_event_held,
		touch_ui_event_double_tap,
		/* Additional future gestures can go here. */
	} touch_ui_event_type_t;

	/**
	 * @brief Holds data about a single touch event,
	 *        as interpreted by the library.
	 */
	typedef struct touch_ui_event_t
	{
		/** @brief Type of event (press, release, move, etc.). */
		touch_ui_event_type_t event_type;

		/** @brief X screen coordinate in pixels. */
		uint16_t x_position;

		/** @brief Y screen coordinate in pixels. */
		uint16_t y_position;

		/** @brief A timestamp in milliseconds or system ticks, as provided. */
		uint32_t timestamp_value;
	} touch_ui_event_t;

	/**
	 * @brief Internal state for single-touch tracking.
	 */
	typedef struct touch_ui_state_t
	{
		/** @brief Last known press state: false = up, true = down. */
		bool last_press_state;

		/** @brief Last known X coordinate while pressed. */
		uint16_t last_x_position;

		/** @brief Last known Y coordinate while pressed. */
		uint16_t last_y_position;

		/** @brief Timestamp of last press event (for hold/double-tap logic). */
		uint32_t last_press_timestamp_value;

		/* Additional fields for expansions: hold duration, double-tap spacing, etc. */
	} touch_ui_state_t;

	/**
	 * @brief Main context for the touch UI library.
	 *        The user must allocate this and pass it to initialization.
	 */
	typedef struct touch_ui_context_t
	{
		/** @brief Internal single-touch tracking state. */
		touch_ui_state_t internal_state;

		const touch_ui_driver_t *driver;

		/** @brief Future expansions: multi-touch state, gesture buffers, etc. */
	} touch_ui_context_t;

	/**
	 * @brief Initializes the touch UI context.
	 * @param context Pointer to an allocated touch_ui_context_t structure.
	 * @param driver  Pointer to a driver structure.
	 */
	void touch_ui_init(touch_ui_context_t *context, const touch_ui_driver_t *driver);

	/**
	 * @brief Processes raw touch input (x, y, pressed) to produce a higher-level event.
	 *        This function does single-touch logic: if pressed state changes from up->down,
	 *        it returns a 'pressed' event, etc.
	 *
	 * @param context          Pointer to the initialized touch UI context.
	 * @param x_position       The raw X coordinate from the driver.
	 * @param y_position       The raw Y coordinate from the driver.
	 * @param is_pressed       True if finger is down, false if lifted.
	 * @param timestamp_value  A millisecond or tick count for event timing.
	 * @return                 A single touch_ui_event_t describing the new event, or
	 *                         type=touch_ui_event_none if nothing changed.
	 */
	touch_ui_event_t touch_ui_process_input(touch_ui_context_t *context,
						uint16_t x_position,
						uint16_t y_position,
						bool is_pressed,
						uint32_t timestamp_value);

	/**
	 * @brief	Reads the current raw touch state from the driver.
	 *
	 * @param	context Pointer to initialized touch context.
	 * @param	x Pointer to store X coordinate.
	 * @param	y Pointer to store Y coordinate.
	 * @param	pressed Pointer to store touch status.
	 * @param	timestamp Pointer to store timestamp in ms.
	 *
	 * @return	true if successful, false otherwise.
	 */
	bool touch_ui_read_state(touch_ui_context_t *context,
				 uint16_t *x,
				 uint16_t *y,
				 bool *pressed,
				 uint32_t *timestamp);

#ifdef __cplusplus
}
#endif

#endif /* TOUCH_UI_H */
