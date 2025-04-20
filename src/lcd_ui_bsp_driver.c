/**
 * @file        lcd_ui_bsp_driver.c
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

#include "lcd_ui.h"
#include "stm32h747i_discovery_lcd.h" // STM32 board specific LCD header
#include "stm32_lcd.h"                // STM32 LCD driver header

static void driver_init(void)
{
	(void)BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
	UTIL_LCD_SetFuncDriver(&LCD_Driver);
	UTIL_LCD_SetLayer(0);
	UTIL_LCD_SetFont(&Font24);
	UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
}

static void driver_set_backlight(uint8_t level)
{
	// Implement later
	(void)level;
}

static void driver_draw_pixel(uint16_t x, uint16_t y, uint32_t colour)
{
	UTIL_LCD_DrawLine((uint32_t)x,
			  (uint32_t)y,
			  (uint32_t)x,
			  (uint32_t)y,
			  colour);
}

static void driver_draw_rect(uint16_t x,
			     uint16_t y,
			     uint16_t w,
			     uint16_t h,
			     uint32_t colour)
{
	UTIL_LCD_SetTextColor(colour);
	UTIL_LCD_FillRect(x, y, w, h, colour);
}

static void driver_draw_text(uint16_t x,
			     uint16_t y,
			     const char *text,
			     uint32_t colour,
			     uint32_t background_colour,
			     lcd_ui_align_t align)
{
	uint32_t stm_align = LEFT_MODE;
	if (align == LCD_UI_ALIGN_CENTER)
		stm_align = CENTER_MODE;
	else if (align == LCD_UI_ALIGN_RIGHT)
		stm_align = RIGHT_MODE;

	UTIL_LCD_SetTextColor(colour);
	UTIL_LCD_SetBackColor(background_colour);
	UTIL_LCD_DisplayStringAt(x, y, (uint8_t *)text, stm_align);
}

static void driver_clear(uint32_t colour)
{
	UTIL_LCD_Clear(colour);
}

static void driver_get_screen_size(uint16_t *w, uint16_t *h)
{
	uint32_t x = 0, y = 0;
	(void)BSP_LCD_GetXSize(0, &x);
	(void)BSP_LCD_GetYSize(0, &y);
	*w = (uint16_t)x;
	*h = (uint16_t)y;
}

static uint16_t driver_get_font_width(void)
{
	const sFONT *font = UTIL_LCD_GetFont();
	return font->Width;
}

static uint16_t driver_get_font_height(void)
{
	const sFONT *font = UTIL_LCD_GetFont();
	return font->Height;
}

/* Define the driver struct for the board */
const lcd_ui_driver_t lcd_ui_bsp_driver = {
    .init = driver_init,
    .set_backlight = driver_set_backlight,
    .draw_pixel = driver_draw_pixel,
    .draw_rect = driver_draw_rect,
    .draw_text = driver_draw_text,
    .clear = driver_clear,
    .get_screen_size = driver_get_screen_size,
    .get_font_width = driver_get_font_width,
    .get_font_height = driver_get_font_height,
};
