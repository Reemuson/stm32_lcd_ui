/**
 * @file        lcd_ui.c
 * @brief       Implementation of a simple UI library for STM32 LCD displays.
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

#include "lcd_ui.h"
#include "lcd_ui_colours.h"
#include <string.h>

void lcd_ui_init(lcd_ui_context_t *ctx,
		 const lcd_ui_driver_t *driver,
		 lcd_ui_widget_t **widget_buffer,
		 uint8_t capacity)
{
	if (!ctx || !driver || !widget_buffer)
		return;

	ctx->driver = driver;
	ctx->widgets = widget_buffer;
	ctx->widget_capacity = capacity;
	ctx->widget_count = 0;

	ctx->active_widget = NULL;
	ctx->touch_active = 0;

	driver->init();
	driver->get_screen_size(&ctx->screen_width, &ctx->screen_height);
}

void lcd_ui_reset_screen(lcd_ui_context_t *ctx, uint32_t colour)
{
	if (!ctx || !ctx->driver)
		return;
	ctx->driver->clear(colour);
	lcd_ui_clear_widgets(ctx);
}

void lcd_ui_clear_widgets(lcd_ui_context_t *ctx)
{
	if (!ctx)
		return;

	ctx->widget_count = 0;

	for (uint8_t i = 0; i < ctx->widget_capacity; ++i)
	{
		ctx->widgets[i] = NULL;
	}
}
void lcd_ui_add_widget(lcd_ui_context_t *ctx,
		       const lcd_ui_widget_t *widget)
{
	if (!ctx || !widget)
		return;

	if (ctx->widget_count >= ctx->widget_capacity)
		return;

	ctx->widgets[ctx->widget_count++] = (lcd_ui_widget_t *)widget;

	if (widget->text_align > LCD_UI_ALIGN_RIGHT)
	{
		ctx->widgets[ctx->widget_count]->text_align = LCD_UI_ALIGN_LEFT;
	}
}

/**
 * @brief Internal utility to render a single widget.
 *        Used by both full render and selective redraw.
 * @note AI-aided via Supermaven Copilot — reviewed and adapted.
 */
static void draw_widget(const lcd_ui_context_t *context,
			const lcd_ui_widget_t *widget)
{
	if (!context || !context->driver || !widget)
		return;

	switch (widget->type)
	{
	case LCD_UI_WIDGET_BUTTON:
	{
		context->driver->draw_rect(widget->x,
					   widget->y,
					   widget->width,
					   widget->height,
					   widget->background_color);

		if (widget->label_text != NULL)
		{
			uint16_t font_w = context->driver->get_font_width();
			uint16_t font_h = context->driver->get_font_height();

			uint16_t text_len = (uint16_t)strlen(widget->label_text);
			uint16_t text_width = text_len * font_w;

			uint16_t text_x;
			switch (widget->text_align)
			{
			case LCD_UI_ALIGN_CENTER:
				text_x = widget->x + (widget->width - text_width) / 2U;
				break;
			case LCD_UI_ALIGN_RIGHT:
				text_x = widget->x + widget->width - text_width;
				break;
			case LCD_UI_ALIGN_LEFT:
			default:
				text_x = widget->x;
				break;
			}

			uint16_t text_y = widget->y + (widget->height - font_h) / 2U;

			context->driver->draw_text(text_x,
						   text_y,
						   widget->label_text,
						   widget->text_color,
						   widget->background_color,
						   LCD_UI_ALIGN_LEFT); // force manual alignment
		}
		break;
	}

	case LCD_UI_WIDGET_LABEL:
		if (widget->label_text != NULL)
		{
			context->driver->draw_text(widget->x,
						   widget->y,
						   widget->label_text,
						   widget->text_color,
						   widget->background_color,
						   widget->text_align);
		}
		break;

	case LCD_UI_WIDGET_PROGRESS_BAR:
	{
		context->driver->draw_rect(widget->x,
					   widget->y,
					   widget->width,
					   widget->height,
					   widget->background_color);

		uint16_t fill_width =
		    (uint16_t)((widget->progress_percent * widget->width) / 100U);

		context->driver->draw_rect(widget->x,
					   widget->y,
					   fill_width,
					   widget->height,
					   widget->text_color);
		break;
	}

	case LCD_UI_WIDGET_SLIDER:
	{
		const uint16_t knob_size = widget->height; // square knob
		const uint16_t track_height = widget->height / 3U;
		const uint16_t track_y = widget->y + (widget->height - track_height) / 2U;

		/* Clear the entire slider widget area first */
		context->driver->draw_rect(widget->x,
					   widget->y,
					   widget->width,
					   widget->height,
					   widget->background_color);

		/* Draw the slider track using text_color */
		context->driver->draw_rect(widget->x,
					   track_y,
					   widget->width,
					   track_height,
					   widget->text_color); // Track color

		/* Compute knob position */
		uint16_t usable_width = widget->width - knob_size;
		uint16_t knob_x = widget->x + (widget->slider_value * usable_width) / 100U;

		/* Clamp knob_x to ensure knob stays within widget bounds */
		if (knob_x + knob_size > widget->x + widget->width)
		{
			knob_x = widget->x + widget->width - knob_size;
		}

		/* Compute knob color (lighter version of text_color) */
		uint32_t knob_color = lighten_colour(widget->text_color, 40U);

		/* Draw the knob (square) */
		context->driver->draw_rect(knob_x,
					   widget->y,
					   knob_size,
					   knob_size,
					   knob_color); // Knob

		break;
	}

	default:
		break;
	}
}

void lcd_ui_render(const lcd_ui_context_t *ctx)
{
	if (!ctx || !ctx->driver)
		return;

	for (uint8_t i = 0; i < ctx->widget_count; ++i)
	{
		draw_widget(ctx, ctx->widgets[i]);
	}
}

void lcd_ui_redraw_widget(const lcd_ui_context_t *context,
			  const lcd_ui_widget_t *widget)
{
	draw_widget(context, widget);
}

static void default_slider_touch_handler(lcd_ui_context_t *ctx,
					 lcd_ui_widget_t *widget,
					 uint16_t x, uint16_t y,
					 void *user_data)
{
	if (!ctx || !widget)
		return;

	const uint16_t knob_size = widget->height;
	const uint16_t knob_half = knob_size / 2U;

	const uint16_t min_x = widget->x + knob_half;
	const uint16_t max_x = widget->x + widget->width - knob_half;
	const uint16_t range_x = max_x - min_x;

	uint16_t clamped_x = (x < min_x) ? min_x : (x > max_x) ? max_x
							       : x;

	uint16_t relative_x = clamped_x - min_x;

	/* Calculate the new slider value */
	uint32_t new_slider_value = (relative_x * 100U) / range_x;

	widget->slider_value = new_slider_value;

	if (widget->slider_update_callback)
	{
		widget->slider_update_callback(ctx, widget, new_slider_value);
	}

	/* Update the progress bar if linked */
	lcd_ui_widget_t *linked_progress = (lcd_ui_widget_t *)user_data;
	if (linked_progress)
	{
		linked_progress->progress_percent = 100U - widget->slider_value;
		lcd_ui_redraw_widget(ctx, linked_progress);
	}

	lcd_ui_redraw_widget(ctx, widget);
}

void lcd_ui_handle_touch(lcd_ui_context_t *ctx,
			 uint16_t x, uint16_t y,
			 uint8_t is_pressed)
{
	if (!ctx)
		return;

	if (is_pressed)
	{
		if (!ctx->touch_active)
		{
			/* New press: find the widget under the touch */
			ctx->touch_active = 1;
			ctx->active_widget = NULL;

			for (uint8_t i = 0; i < ctx->widget_count; ++i)
			{
				lcd_ui_widget_t *w = ctx->widgets[i];

				uint16_t margin = 0U;

				switch (w->type)
				{
				case LCD_UI_WIDGET_BUTTON:
					margin = 6U; // Slight padding for easier touch
					break;

				case LCD_UI_WIDGET_SLIDER:
					margin = w->height / 5U;
					break;

				default:
					margin = 2U;
					break;
				}

				uint16_t x0 = (w->x > margin) ? (w->x - margin) : 0U;
				uint16_t y0 = (w->y > margin) ? (w->y - margin) : 0U;
				uint16_t x1 = w->x + w->width + margin;
				uint16_t y1 = w->y + w->height + margin;

				if ((x >= x0) && (x < x1) &&
				    (y >= y0) && (y < y1))
				{
					ctx->active_widget = w;
					break;
				}
			}
		}

		/* Continue sending to active widget while pressed */
		if (ctx->active_widget)
		{
			if (ctx->active_widget->type == LCD_UI_WIDGET_SLIDER)
			{
				if (ctx->active_widget->on_touch)
				{
					ctx->active_widget->on_touch(ctx, ctx->active_widget, x, y,
								     ctx->active_widget->user_data);
				}
				else
				{
					default_slider_touch_handler(ctx, ctx->active_widget, x, y,
								     ctx->active_widget->user_data);
				}
			}
		}
	}
	else
	{
		/* On release: trigger button if it was the active widget */
		if (ctx->active_widget &&
		    ctx->active_widget->type == LCD_UI_WIDGET_BUTTON &&
		    ctx->active_widget->on_touch)
		{
			ctx->active_widget->on_touch(ctx,
						     ctx->active_widget,
						     x, y,
						     ctx->active_widget->user_data);
		}

		ctx->touch_active = 0;
		ctx->active_widget = NULL;
	}
}

/**
 * @brief Get the screen width in pixels.
 * @param ctx Pointer to initialized lcd_ui_context_t
 * @return Screen width
 */
uint16_t lcd_ui_get_screen_width(const lcd_ui_context_t *ctx)
{
	if (!ctx)
		return 0U;
	return ctx->screen_width;
}

/**
 * @brief Get the screen height in pixels.
 * @param ctx Pointer to initialized lcd_ui_context_t
 * @return Screen height
 */
uint16_t lcd_ui_get_screen_height(const lcd_ui_context_t *ctx)
{
	if (!ctx)
		return 0U;
	return ctx->screen_height;
}