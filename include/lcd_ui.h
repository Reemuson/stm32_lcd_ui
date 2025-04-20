/**
 * @file        lcd_ui.h
 * @brief       Public interface for LCD UI library.
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

#ifndef LCD_UI_H
#define LCD_UI_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct lcd_ui_context lcd_ui_context_t;
	typedef struct lcd_ui_widget lcd_ui_widget_t;

	/**
	 * @brief Widget text alignment
	 */
	typedef enum
	{
		LCD_UI_ALIGN_LEFT = 0,
		LCD_UI_ALIGN_CENTER,
		LCD_UI_ALIGN_RIGHT
	} lcd_ui_align_t;

	/**
	 * @brief Widget callback structure
	 */
	typedef void (*lcd_ui_touch_callback_t)(
	    lcd_ui_context_t *ctx,
	    lcd_ui_widget_t *widget,
	    uint16_t touch_x,
	    uint16_t touch_y,
	    void *user_data);

	/**
	 * @brief Widget types
	 */
	typedef enum
	{
		LCD_UI_WIDGET_BUTTON,
		LCD_UI_WIDGET_SLIDER,
		LCD_UI_WIDGET_PROGRESS_BAR,
		LCD_UI_WIDGET_LABEL,
	} lcd_ui_widget_type_t;

	/**
	 * @brief Represents a single UI widget in the system.
	 */
	struct lcd_ui_widget
	{
		uint16_t x;
		uint16_t y;
		uint16_t width;
		uint16_t height;
		lcd_ui_widget_type_t type;

		lcd_ui_touch_callback_t on_touch;
		void *user_data;

		const char *label_text;

		uint8_t progress_percent;
		uint32_t slider_value;

		uint32_t background_color;
		uint32_t text_color;

		lcd_ui_align_t text_align;

		void (*slider_update_callback)(lcd_ui_context_t *ctx,
					       lcd_ui_widget_t *widget,
					       uint32_t new_value);
	};

	typedef struct
	{
		void (*init)(void);
		void (*set_backlight)(uint8_t level);
		void (*draw_pixel)(uint16_t x, uint16_t y, uint32_t colour);
		void (*draw_rect)(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t colour);
		void (*draw_text)(uint16_t x, uint16_t y, const char *text, uint32_t text_colour, uint32_t background_colour, lcd_ui_align_t align);
		void (*clear)(uint32_t colour);
		void (*get_screen_size)(uint16_t *w, uint16_t *h);
		uint16_t (*get_font_width)(void);
		uint16_t (*get_font_height)(void);
	} lcd_ui_driver_t;

	struct lcd_ui_context
	{
		const lcd_ui_driver_t *driver;
		lcd_ui_widget_t **widgets;
		uint8_t widget_capacity;
		uint8_t widget_count;
		uint16_t screen_width;
		uint16_t screen_height;

		lcd_ui_widget_t *active_widget;
		uint8_t touch_active;
	};

	void lcd_ui_init(lcd_ui_context_t *ctx,
			 const lcd_ui_driver_t *driver,
			 lcd_ui_widget_t **widget_buffer,
			 uint8_t capacity);

	void lcd_ui_reset_screen(lcd_ui_context_t *ctx, uint32_t colour);

	void lcd_ui_add_widget(lcd_ui_context_t *ctx,
			       const lcd_ui_widget_t *widget);

	void lcd_ui_clear_widgets(lcd_ui_context_t *ctx);

	void lcd_ui_render(const lcd_ui_context_t *ctx);

	void lcd_ui_handle_touch(lcd_ui_context_t *ctx,
				 uint16_t x, uint16_t y,
				 uint8_t is_pressed);

	void lcd_ui_redraw_widget(const lcd_ui_context_t *context,
				  const lcd_ui_widget_t *widget);

	uint16_t lcd_ui_get_screen_width(const lcd_ui_context_t *ctx);

	uint16_t lcd_ui_get_screen_height(const lcd_ui_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif // LCD_UI_H
