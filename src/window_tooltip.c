/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <memory.h>
#include "addresses.h"
#include "strings.h"
#include "widget.h"
#include "window.h"

enum {
	WIDX_BACKGROUND
};

static rct_widget window_tooltip_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 31, 0x0FFFFFFFF, STR_NONE },
	{ WIDGETS_END },
};

static void window_tooltip_emptysub() { }
static void window_tooltip_onclose();
static void window_tooltip_update();
static void window_tooltip_paint();

static void* window_tooltip_events[] = {
	window_tooltip_onclose,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_update,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_emptysub,
	window_tooltip_paint,
	window_tooltip_emptysub
};

void window_tooltip_reset(int x, int y)
{
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_X, uint8) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_CURSOR_Y, uint8) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint8) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, uint8) = 255;
	RCT2_GLOBAL(RCT2_ADDRESS_INPUT_STATE, uint8) = 1;
	RCT2_GLOBAL(0x009DE518, uint32) &= ~(1 << 4);
}

/**
 * 
 *  rct2: 0x006EA10D
 */
void window_tooltip_open(rct_window *widgetWindow, int widgetIndex, int x, int y)
{
	rct_window *w;
	rct_widget *widget;
	int width, height;
	
	if (widgetWindow == NULL || widgetIndex == -1)
		return;

	widget = &widgetWindow->widgets[widgetIndex];
	RCT2_CALLPROC_X(widgetWindow->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)widgetWindow, 0, 0);
	if (widget->tooltip == 0xFFFF)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = widgetWindow->classification;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_NUMBER, rct_windownumber) = widgetWindow->number;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WIDGET_INDEX, uint16) = widgetIndex;

	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = widgetIndex;
	esi = (int)widgetWindow;
	RCT2_CALLFUNC_X(widgetWindow->event_handlers[WE_TOOLTIP], &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	if ((eax & 0xFFFF) == 0xFFFF)
		return;

	w = window_find_by_id(WC_ERROR, 0);
	if (w != NULL)
		return;

	RCT2_GLOBAL(0x0142006C, sint32) = -1;

	format_string((char*)0x0141ED68, widget->tooltip, (void*)0x013CE952);
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;

	esi = 0x0141ED68;
	RCT2_CALLFUNC_X(0x006C23B1, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	ecx &= 0xFFFF;
	if (ecx > 196)
		ecx = 196;

	esi = 0x0141ED68;
	edi = ecx + 1;
	RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_FONT_SPRITE_BASE, uint16) = 224;
	RCT2_CALLFUNC_X(0x006C21E2, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	ecx &= 0xFFFF;
	edi &= 0xFFFF;

	RCT2_GLOBAL(0x01420044, sint16) = edi;
	width = ecx + 3;
	height = ((edi + 1) * 10) + 4;
	window_tooltip_widgets[WIDX_BACKGROUND].right = width;
	window_tooltip_widgets[WIDX_BACKGROUND].bottom = height;

	memcpy((void*)0x0141FE44, (void*)0x0141ED68, 512);
	
	x = clamp(0, x - (width / 2), RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, uint16) - width);
	y = clamp(22, y + 26, RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, uint16) - height - 40);

	w = window_create(x, y, width, height, (uint32*)window_tooltip_events, WC_TOOLTIP, WF_TRANSPARENT | WF_STICK_TO_FRONT);
	w->widgets = window_tooltip_widgets;

	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, uint16) = 0;
}

/**
 * 
 *  rct2: 0x006E98C6
 */
void window_tooltip_close()
{
	window_close_by_id(WC_TOOLTIP, 0);
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_TIMEOUT, uint16) = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_WINDOW_CLASS, rct_windowclass) = 255;
	RCT2_GLOBAL(0x0142006C, sint32) = -1;
	RCT2_GLOBAL(0x009DE51E, uint8) = 0;
}

/**
 * 
 *  rct2: 0x006EA578
 */
static void window_tooltip_onclose()
{
	RCT2_GLOBAL(0x009BC3B0, uint8) = 0;
}

/**
 * 
 *  rct2: 0x006EA580
 */
static void window_tooltip_update()
{
	if (RCT2_GLOBAL(0x009DE51E, uint8) == 0)
		RCT2_GLOBAL(RCT2_ADDRESS_TOOLTIP_NOT_SHOWN_TICKS, uint16) = 0;
}

/**
 * 
 *  rct2: 0x006EA41D
 */
static void window_tooltip_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	__asm mov w, esi
	__asm mov dpi, edi

	int left = w->x;
	int top = w->y;
	int right = w->x + w->width - 1;
	int bottom = w->y + w->height - 1;

	// Background
	gfx_fill_rect(dpi, left + 1, top + 1, right - 1, bottom - 1, 0x0200002D);
	gfx_fill_rect(dpi, left + 1, top + 1, right - 1, bottom - 1, 0x02000084);

	// Sides
	gfx_fill_rect(dpi, left  + 0, top    + 2, left  + 0, bottom - 2, 0x0200002F);
	gfx_fill_rect(dpi, right + 0, top    + 2, right + 0, bottom - 2, 0x0200002F);
	gfx_fill_rect(dpi, left  + 2, bottom + 0, right - 2, bottom + 0, 0x0200002F);
	gfx_fill_rect(dpi, left  + 2, top    + 0, right - 2, top    + 0, 0x0200002F);

	// Corners
	gfx_draw_pixel(dpi, left  + 1, top    + 1, 0x0200002F);
	gfx_draw_pixel(dpi, right - 1, top    + 1, 0x0200002F);
	gfx_draw_pixel(dpi, left  + 1, bottom - 1, 0x0200002F);
	gfx_draw_pixel(dpi, right - 1, bottom - 1, 0x0200002F);
	
	// Text
	RCT2_CALLPROC_X(0x006C1DB7, 0, 0, w->x + ((w->width + 1) / 2) - 1, w->y + 1, 0x0141FE44, (int)dpi, RCT2_GLOBAL(0x01420044, uint16));
}