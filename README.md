# 📟 lcd_ui + touch_ui

A lightweight and modular STM32H7 embedded UI library for creating simple touchscreen interfaces with buttons, labels, sliders, and progress bars.

---

## ✨ Features

- **Modular design** – drop-in `lcd_ui` for rendering, and `touch_ui` for touch input processing
- **Widget types**: `LABEL`, `BUTTON`, `SLIDER`, `PROGRESS_BAR`
- **Custom callback support** for button presses and slider movement
- **FreeRTOS friendly** (no dynamic memory, thread-safe usage)
- **Driver abstraction layer** to support multiple displays or touch ICs
- **Built-in colour and shading utilities** via `lcd_ui_colours.h`

---

## 📦 Files

- `lcd_ui.[c/h]` – core UI logic and widget rendering
- `touch_ui.[c/h]` – touchscreen state machine and input events
- `lcd_ui_driver.h` – abstract interface to your display driver
- `touch_ui_driver.h` – abstract interface to your touch driver
- `lcd_ui_colours.h` – colour constants and lighten/darken helpers

---

## 🚀 Getting Started

### 1. Provide Display + Touch Drivers

Implement your display/touch drivers:

```c
// lcd_ui_driver_t lcd_ui_bsp_driver;
lcd_ui_bsp_driver.draw_rect = ...;
lcd_ui_bsp_driver.draw_text = ...;
lcd_ui_bsp_driver.get_screen_size = ...;
```

```c
// touch_ui_driver_t touch_ui_bsp_driver;
touch_ui_bsp_driver.read_touch_state = ...;
```

### 2. Initialize UI System

```c
#include "lcd_ui.h"
#include "lcd_ui_driver.h"
#include "lcd_ui_colours.h"

#define MAX_WIDGETS 5

static lcd_ui_context_t ui_ctx;
static lcd_ui_widget_t *widget_buffer[MAX_WIDGETS];

void ui_init(void)
{
	lcd_ui_init(&ui_ctx, &lcd_ui_bsp_driver, widget_buffer, MAX_WIDGETS);
	lcd_ui_reset_screen(&ui_ctx, colour_black);

	static lcd_ui_widget_t label;
	label.x = 20;
	label.y = 40;
	label.width = 200;
	label.height = 30;
	label.type = LCD_UI_WIDGET_LABEL;
	label.label_text = "Hello World";
	label.text_color = colour_white;
	label.background_color = colour_black;
	label.text_align = LCD_UI_ALIGN_CENTER;

	lcd_ui_add_widget(&ui_ctx, &label);
	lcd_ui_render(&ui_ctx);
}
```

### 3. Feed Touch Input

In your main loop or touch task:

```c
#include "touch_ui.h"

static touch_ui_context_t touch_ctx;

void touch_task(void *arg)
{
	touch_ui_init(&touch_ctx, &touch_ui_bsp_driver);

	while (1)
	{
		uint16_t x, y;
		bool is_pressed;
		uint32_t time;

		if (touch_ui_read_state(&touch_ctx, &x, &y, &is_pressed, &time))
		{
			touch_ui_event_t evt = touch_ui_process_input(&touch_ctx, x, y, is_pressed, time);
			if (evt.event_type != touch_ui_event_none)
				lcd_ui_handle_touch(&ui_ctx, x, y, is_pressed);
		}
	}
}
```

---

## 🧱 Supported Widgets

| Widget Type     | Description                             | Touch Support |
|------------------|-----------------------------------------|----------------|
| `LABEL`         | Displays static or dynamic text         | ❌              |
| `BUTTON`        | Executes a callback on press            | ✅              |
| `SLIDER`        | Adjustable control (0–100)              | ✅              |
| `PROGRESS_BAR`  | Read-only progress (0–100)              | ❌              |

---

## 🎨 Colour Utilities

Use `colour_red`, `colour_black`, etc., or dynamically shade:

```c
uint32_t bg = darken_colour(colour_blue, 50);     // 50% darker
uint32_t fg = lighten_colour(colour_yellow, 20);  // 20% lighter
```

---

## 📄 License

This library is licensed under the **MIT License**. See `LICENSE.md`.

---

## 🧠 Author

**Ryan Hicks**  
📧 c3361231@uon.edu.au  
🏫 University of Newcastle – School of Electrical & Computer Engineering

> Built using Visual Studio Code with ❤️ and Supermaven Copilot suggestions.  
> Attribution required for reuse. MIT SPDX license applies.
