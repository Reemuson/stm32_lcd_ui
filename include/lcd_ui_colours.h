/**
 * @file lcd_ui_colours.h
 * @brief Provides ARGB colour creation and shade/light/dark utility functions.
 * @author Ryan Hicks
 */

#ifndef LCD_UI_COLOURS_H
#define LCD_UI_COLOURS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

	/**
	 * @brief Holds RGBA channels in separate bytes.
	 */
	typedef struct argb_colour_t
	{
		/**
		 * @brief Alpha channel, 0..255.
		 */
		uint8_t alpha_value;

		/**
		 * @brief Red channel, 0..255.
		 */
		uint8_t red_value;

		/**
		 * @brief Green channel, 0..255.
		 */
		uint8_t green_value;

		/**
		 * @brief Blue channel, 0..255.
		 */
		uint8_t blue_value;
	} argb_colour_t;

	/**
	 * @brief Creates a 32-bit ARGB colour (Alpha, Red, Green, Blue).
	 *        Bits 31..24 = alpha, 23..16 = red, 15..8 = green, 7..0 = blue.
	 *
	 * @param alpha_value 0..255 alpha
	 * @param red_value   0..255 red
	 * @param green_value 0..255 green
	 * @param blue_value  0..255 blue
	 * @return 32-bit ARGB colour
	 */
	static inline uint32_t make_argb_colour(uint8_t alpha_value,
						uint8_t red_value,
						uint8_t green_value,
						uint8_t blue_value)
	{
		/* Compose four bytes into one 32-bit value. */
		uint32_t colour_argb = 0U;
		colour_argb |= ((uint32_t)alpha_value << 24);
		colour_argb |= ((uint32_t)red_value << 16);
		colour_argb |= ((uint32_t)green_value << 8);
		colour_argb |= (uint32_t)blue_value;
		return colour_argb;
	}

	/**
	 * @brief Extracts an argb_colour_t from a 32-bit ARGB value.
	 *
	 * @param argb_value 32-bit ARGB colour
	 * @return Decomposed argb_colour_t
	 */
	static inline argb_colour_t decompose_argb_colour(uint32_t argb_value)
	{
		argb_colour_t colour_struct;
		colour_struct.alpha_value = (uint8_t)((argb_value >> 24) & 0xFFU);
		colour_struct.red_value = (uint8_t)((argb_value >> 16) & 0xFFU);
		colour_struct.green_value = (uint8_t)((argb_value >> 8) & 0xFFU);
		colour_struct.blue_value = (uint8_t)(argb_value & 0xFFU);
		return colour_struct;
	}

	/**
	 * @brief Scales the red, green, and blue channels by a factor,
	 *        leaving alpha unchanged.
	 *
	 * @param argb_value    32-bit ARGB colour
	 * @param scale_factor  <1.0 => darker, >1.0 => lighter, =1.0 => no change
	 * @return Scaled 32-bit ARGB colour
	 */
	static inline uint32_t scale_colour_by_factor(uint32_t argb_value,
						      float scale_factor)
	{
		argb_colour_t colour_struct;
		int scaled_red;
		int scaled_green;
		int scaled_blue;
		uint32_t result;

		colour_struct = decompose_argb_colour(argb_value);

		scaled_red = (int)((float)colour_struct.red_value * scale_factor);
		if (scaled_red < 0)
		{
			scaled_red = 0;
		}
		else if (scaled_red > 255)
		{
			scaled_red = 255;
		}

		scaled_green = (int)((float)colour_struct.green_value * scale_factor);
		if (scaled_green < 0)
		{
			scaled_green = 0;
		}
		else if (scaled_green > 255)
		{
			scaled_green = 255;
		}

		scaled_blue = (int)((float)colour_struct.blue_value * scale_factor);
		if (scaled_blue < 0)
		{
			scaled_blue = 0;
		}
		else if (scaled_blue > 255)
		{
			scaled_blue = 255;
		}

		result = make_argb_colour(colour_struct.alpha_value,
					  (uint8_t)scaled_red,
					  (uint8_t)scaled_green,
					  (uint8_t)scaled_blue);
		return result;
	}

	/**
	 * @brief Similar to LaTeX '!XX' syntax. Percentage <100 => darker,
	 *        >100 => lighter, ==100 => unchanged.
	 *
	 * @param argb_value        32-bit ARGB colour
	 * @param percentage_value  0..200 typical
	 * @return Adjusted 32-bit ARGB colour
	 */
	static inline uint32_t scale_colour_by_percentage(uint32_t argb_value,
							  uint8_t percentage_value)
	{
		float factor;
		factor = (float)percentage_value / 100.0f;
		return scale_colour_by_factor(argb_value, factor);
	}

	/**
	 * @brief Returns a darker version of the given colour by the specified amount.
	 *
	 * @param argb_value   32-bit ARGB colour
	 * @param amount_value 0..100 -> how many percent to reduce from 100
	 * @return Darkened colour
	 */
	static inline uint32_t darken_colour(uint32_t argb_value,
					     uint8_t amount_value)
	{
		uint8_t target_percentage;
		if (amount_value > 100U)
		{
			amount_value = 100U;
		}
		target_percentage = (uint8_t)(100U - amount_value);
		return scale_colour_by_percentage(argb_value, target_percentage);
	}

	/**
	 * @brief Returns a lighter version of the given colour by the specified amount.
	 *
	 * @param argb_value   32-bit ARGB colour
	 * @param amount_value 0..100 -> how many percent to add above 100
	 * @return Lightened colour
	 */
	static inline uint32_t lighten_colour(uint32_t argb_value,
					      uint8_t amount_value)
	{
		uint16_t total;
		total = (uint16_t)(100U + amount_value);
		if (total > 200U)
		{
			total = 200U;
		}
		return scale_colour_by_percentage(argb_value, (uint8_t)total);
	}

	/*
	 * Below are standard named colours as static constants.
	 * They are fully opaque with alpha=255.
	 *
	 * No macros for numeric values to avoid debug trouble.
	 */

	/**
	 * @brief Standard opaque white colour.
	 */
	static const uint32_t colour_white =
	    (uint32_t)0xFFFFFFFFU;

	/**
	 * @brief Standard opaque black colour.
	 */
	static const uint32_t colour_black =
	    (uint32_t)0xFF000000U;

	/**
	 * @brief Standard opaque red colour.
	 */
	static const uint32_t colour_red =
	    (uint32_t)0xFFFF0000U;

	/**
	 * @brief Standard opaque green colour.
	 */
	static const uint32_t colour_green =
	    (uint32_t)0xFF00FF00U;

	/**
	 * @brief Standard opaque blue colour.
	 */
	static const uint32_t colour_blue =
	    (uint32_t)0xFF0000FFU;

	/**
	 * @brief Standard opaque gray colour at 50% intensity.
	 */
	static const uint32_t colour_gray =
	    (uint32_t)0xFF808080U;

	/**
	 * @brief Standard opaque yellow colour.
	 */
	static const uint32_t colour_yellow =
	    (uint32_t)0xFFFFFF00U;

	/**
	 * @brief Standard opaque cyan colour.
	 */
	static const uint32_t colour_cyan =
	    (uint32_t)0xFF00FFFFU;

	/**
	 * @brief Standard opaque magenta colour.
	 */
	static const uint32_t colour_magenta =
	    (uint32_t)0xFFFF00FFU;

#ifdef __cplusplus
}
#endif

#endif /* LCD_UI_COLOURS_H */
