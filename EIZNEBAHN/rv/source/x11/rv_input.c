// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#include <assert.h>
#include <sched.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include "rv.h"
#include "rv_internal.h"

static Window rv_internal_get_active_window(Display* display)
{
	Atom unused_type;
	int unused_format;
	unsigned long num_items;
	unsigned long unused_bytes_after;
	unsigned char* prop;

	int status = XGetWindowProperty(
		display,
		XRootWindow(display, XDefaultScreen(display)),
		XInternAtom(display, "_NET_ACTIVE_WINDOW", False),
		0,
		~0L,
		False,
		AnyPropertyType,
		&unused_type,
		&unused_format,
		&num_items,
		&unused_bytes_after,
		&prop);

	if (status != Success)
	{
		return None;
	}

	return *((Window*)prop);
}

static void rv_internal_activate_window(Display* display, Window target_window)
{
	Window current_active_window = rv_internal_get_active_window(display);

	if (current_active_window == target_window)
		return;

	XEvent event = { 0 };
	event.type = ClientMessage;
	event.xclient.display = display;
	event.xclient.window = target_window;
	event.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
	event.xclient.format = 32;
	event.xclient.data.l[0] = 2;
	event.xclient.data.l[1] = CurrentTime;

	XSendEvent(
		display,
		XRootWindow(display, XDefaultScreen(display)),
		False,
		SubstructureNotifyMask | SubstructureRedirectMask,
		&event);
	XSync(display, False);

	while (target_window != current_active_window
		&& rv_internal_get_active_window(display) != target_window)
	{
		sched_yield();
	}
}

rv_input_event_state* rv_create_input_event_state(rv_window* window)
{
	rv_input_event_state* event_state = calloc(1, sizeof(rv_input_event_state));

	if (event_state)
	{
		if (window == NULL)
		{
			event_state->display = XOpenDisplay(NULL);
		}
		else
		{
			event_state->display = window->display;
		}

		event_state->target_window = window;
	}

	return event_state;
}

void rv_destroy_input_event_state(rv_input_event_state* event_state)
{
	if (event_state->target_window == NULL)
	{
		XCloseDisplay(event_state->display);
	}

	free(event_state);
}

void rv_begin_event_sequence(rv_input_event_state* event_state)
{
	event_state->current_delay = 0;

	if (event_state->target_window != NULL)
	{
		rv_internal_activate_window(
			event_state->display,
			event_state->target_window->window);
	}
}

void rv_end_event_sequence(rv_input_event_state* event_state)
{
	XSync(event_state->display, False);
	if (event_state->current_delay != 0)
	{
		usleep(event_state->current_delay * 1000);
	}
}

static int rv_internal_key_map[] =
{
	XK_A,
	XK_B,
	XK_C,
	XK_D,
	XK_E,
	XK_F,
	XK_G,
	XK_H,
	XK_I,
	XK_J,
	XK_K,
	XK_L,
	XK_M,
	XK_N,
	XK_O,
	XK_P,
	XK_Q,
	XK_R,
	XK_S,
	XK_T,
	XK_U,
	XK_V,
	XK_W,
	XK_X,
	XK_Y,
	XK_Z,

	XK_0,
	XK_1,
	XK_2,
	XK_3,
	XK_4,
	XK_5,
	XK_6,
	XK_7,
	XK_8,
	XK_9,

	XK_Escape,
	XK_asciitilde,
	XK_minus,
	XK_equal,
	XK_BackSpace,
	XK_Tab,
	XK_bracketleft,
	XK_bracketright,
	XK_Return,
	XK_semicolon,
	XK_apostrophe,
	XK_backslash,
	XK_comma,
	XK_period,
	XK_slash,
	XK_space,
	XK_Insert,
	XK_Delete,
	XK_Home,
	XK_End,
	XK_Page_Up,
	XK_Page_Down,
	XK_Left,
	XK_Right,
	XK_Up,
	XK_Down,

	XK_Control_L,
	XK_Alt_L,
	XK_Shift_L,

	0
};

static bool rv_internal_unmap_key(int xk, int* output_key)
{
	for (int i = 0; i < rv_key_max; ++i)
	{
		if (rv_internal_key_map[i] == xk)
		{
			*output_key = i;

			return true;
		}
	}

	return false;
}

rv_hotkey_queue* rv_create_hotkey_queue(rv_window* window)
{
	rv_hotkey_queue* hotkey_queue = calloc(1, sizeof(rv_hotkey_queue));

	if (!hotkey_queue)
	{
		return NULL;
	}

	hotkey_queue->target_window = window;

	return hotkey_queue;
}

void rv_destroy_hotkey_queue(rv_hotkey_queue* hotkey_queue)
{
	for (int i = 0; i < hotkey_queue->hotkey_count; ++i)
	{
		Display* display = hotkey_queue->target_window->display;
		XUngrabKey(
			display,
			XKeysymToKeycode(display, rv_internal_key_map[hotkey_queue->hotkeys[i]]),
			AnyModifier,
			XRootWindow(display, XDefaultScreen(display)));
	}
}

bool rv_register_hotkey(rv_hotkey_queue* hotkey_queue, int key)
{
	assert(key >= 0);
	assert(key < rv_key_max);

	if (hotkey_queue->hotkey_count < RV_MAX_HOTKEYS)
	{
		Display* display = hotkey_queue->target_window->display;

		int status = XGrabKey(
			display,
			XKeysymToKeycode(display, rv_internal_key_map[key]),
			AnyModifier,
			XRootWindow(display, XDefaultScreen(display)),
			False,
			GrabModeAsync,
			GrabModeAsync);

		if (status != Success) 
		{
			return false;
		}

		hotkey_queue->hotkeys[hotkey_queue->hotkey_count] = key;
		++hotkey_queue->hotkey_count;

		return true;
	}
	else
	{
		return false;
	}
}

bool rv_check_hotkey(rv_hotkey_queue* hotkey_queue, int* key, int* modifiers)
{
	Display* display = hotkey_queue->target_window->display;

	XEvent event = { 0 };
	if (XCheckMaskEvent(display, KeyPressMask, &event))
	{
		*modifiers = rv_hotkey_modifier_none;

		if (event.xkey.state & ControlMask)
		{
			*modifiers |= rv_hotkey_modifier_ctrl;
		}

		if (event.xkey.state & Mod1Mask)
		{
			*modifiers |= rv_hotkey_modifier_alt;
		}

		if (event.xkey.state & ShiftMask)
		{
			*modifiers |= rv_hotkey_modifier_shift;
		}

		int xk = XkbKeycodeToKeysym(
			display, event.xkey.keycode, 0, event.xkey.state & ShiftMask);

		rv_internal_unmap_key(xk, key);

		return true;
	}

	return false;
}

void rv_send_key_down(rv_input_event_state* event_state, int key, int delay)
{
	assert(key >= 0);
	assert(key < rv_key_max);

	XTestFakeKeyEvent(
		event_state->display,
		XKeysymToKeycode(event_state->display, rv_internal_key_map[key]),
		True,
		delay);
	event_state->current_delay += delay;
}

void rv_send_key_up(rv_input_event_state* event_state, int key, int delay)
{
	assert(key >= 0);
	assert(key < rv_key_max);

	XTestFakeKeyEvent(
		event_state->display,
		XKeysymToKeycode(event_state->display, rv_internal_key_map[key]),
		False,
		delay);
	event_state->current_delay += delay;
}

int rv_get_mouse_x(rv_input_event_state* event_state)
{
	Window unused_root, unused_child;
	int root_x, root_y, unused_window_x, unused_window_y;
	unsigned int mask;

	XQueryPointer(
		event_state->display, DefaultRootWindow(event_state->display),
		&unused_root, &unused_child,
		&root_x, &root_y, &unused_window_x, &unused_window_y,
		&mask);

	return root_x;
}

int rv_get_mouse_y(rv_input_event_state* event_state)
{
	Window unused_root, unused_child;
	int root_x, root_y, unused_window_x, unused_window_y;
	unsigned int mask;

	XQueryPointer(
		event_state->display, DefaultRootWindow(event_state->display),
		&unused_root, &unused_child,
		&root_x, &root_y, &unused_window_x, &unused_window_y,
		&mask);

	return root_y;
}

unsigned int rv_get_mouse_buttons(rv_input_event_state* event_state)
{
	Window unused_root, unused_child;
	int root_x, root_y, unused_window_x, unused_window_y;
	unsigned int mask;

	XQueryPointer(
		event_state->display, DefaultRootWindow(event_state->display),
		&unused_root, &unused_child,
		&root_x, &root_y, &unused_window_x, &unused_window_y,
		&mask);

	unsigned int result = 0;
	if (mask & Button1Mask)
	{
		result |= 1 << 0;
	}
	if (mask & Button2Mask)
	{
		result |= 1 << 2;
	}
	if (mask & Button3Mask)
	{
		result |= 1 << 3;
	}

	return result;
}

void rv_send_mouse_move(
	rv_input_event_state* event_state, int x, int y, int delay)
{
	XTestFakeMotionEvent(
		event_state->display,
		XDefaultScreen(event_state->display),
		x,
		y,
		delay);
	event_state->current_delay += delay;
}

void rv_send_mouse_button_down(
	rv_input_event_state* event_state, int button, int delay)
{
	XTestFakeButtonEvent(
		event_state->display,
		button,
		True,
		delay);
	event_state->current_delay += delay;
}

void rv_send_mouse_button_up(
	rv_input_event_state* event_state, int button, int delay)
{
	XTestFakeButtonEvent(
		event_state->display,
		button,
		False,
		delay);
	event_state->current_delay += delay;
}
