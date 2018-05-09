// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "rv.h"
#include "rv_internal.h"

static rv_window* rv_internal_create_window()
{
	rv_window* window = calloc(1, sizeof(rv_window));

	if (window)
	{
		window->display = XOpenDisplay(NULL);

		if (!window->display)
		{
			free(window);

			return NULL;
		}
	}

	return window;
}

static bool rv_internal_has_wm_state_property(Display* display, Window window)
{
	Atom wm_state_atom = XInternAtom(display, "WM_STATE", False);
	Atom unused_type;
	int unused_format;
	unsigned long num_items;
	unsigned long unused_bytes_after;
	unsigned char* unused_prop;

	int status = XGetWindowProperty(
		display,
		window,
		wm_state_atom,
		0,
		~0L,
		False,
		AnyPropertyType,
		&unused_type,
		&unused_format,
		&num_items,
		&unused_bytes_after,
		&unused_prop);

	if (status != Success)
	{
		return false;
	}

	return num_items > 0;
}

static Window rv_internal_find_client(
	Display* display,
	Window target_window)
{
	Window root_window, parent_window;
	Window* children_windows;
	unsigned int num_children;

	Window client_window = None;
	bool is_client = rv_internal_has_wm_state_property(display, target_window);
	if (!is_client)
	{
		XQueryTree(
			display,
			target_window,
			&root_window,
			&parent_window,
			&children_windows,
			&num_children);

		for (unsigned int i = 0; i < num_children; ++i)
		{
			 Window child_window = rv_internal_find_client(display, children_windows[i]);

			 if (child_window != None)
			 {
			 	client_window = child_window;

			 	break;
			 }
		}

		XFree(children_windows);
	}
	else
	{
		client_window = target_window;
	}

	return client_window;
}

rv_window* rv_select_window()
{
	rv_window* window = rv_internal_create_window();

	if (!window)
	{
		return NULL;
	}

	Window root_window = None, target_window = None;
	Cursor grab_cursor = None;

	root_window = XRootWindow(window->display, XDefaultScreen(window->display));
	if (!root_window)
	{
		goto failure;
	}

	grab_cursor = XCreateFontCursor(window->display, XC_crosshair);
	if (!grab_cursor)
	{
		goto failure;
	}

	int grab_status = XGrabPointer(
		window->display,
		root_window,
		False,
		ButtonPressMask,
		GrabModeSync,
		GrabModeAsync,
		root_window,
		grab_cursor,
		CurrentTime);
	if (grab_status != GrabSuccess)
	{
		goto failure;
	}

	while (target_window == None)
	{
		XEvent event;
		XAllowEvents(window->display, SyncPointer, CurrentTime);
		XWindowEvent(
			window->display,
			root_window,
			ButtonPressMask,
			&event);

		if (event.type == ButtonPress)
		{
			if (event.xbutton.subwindow == None)
			{
				target_window = root_window;
			}
			else
			{
				target_window = event.xbutton.subwindow;
			}

			break;
		}
	}

	XUngrabPointer(window->display, CurrentTime);
	XFreeCursor(window->display, grab_cursor);
	grab_cursor = None;

	window->window = rv_internal_find_client(window->display, target_window);
	if (window->window == None)
	{
		goto failure;
	}

	return window;

failure:
	if (grab_cursor != None)
	{
		XFreeCursor(window->display, grab_cursor);
	}

	rv_destroy_window(window);

	return NULL;
}

rv_window* rv_window_from_handle(size_t handle)
{
	rv_window* window = rv_internal_create_window();
	window->window = handle;

	return window;
}

void rv_destroy_window(rv_window* window)
{
	XCloseDisplay(window->display);

	free(window);
}

int rv_get_window_x(rv_window* window)
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		window->display,
		window->window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	if (window->window != root_window)
	{
		int t_x, t_y;
		Window unused_window;
		XTranslateCoordinates(
			window->display,
			window->window,
			root_window,
			x, y,
			&t_x, &t_y,
			&unused_window);

		x = t_x;
		y = t_y;
	}

	return x;
}

int rv_get_window_y(rv_window* window)
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		window->display,
		window->window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	if (window->window != root_window)
	{
		int t_x, t_y;
		Window unused_window;
		XTranslateCoordinates(
			window->display,
			window->window,
			root_window,
			x, y,
			&t_x, &t_y,
			&unused_window);

		x = t_x;
		y = t_y;
	}

	return y;
}

int rv_get_window_width(rv_window* window)
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		window->display,
		window->window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	return width;
}

int rv_get_window_height(rv_window* window)
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		window->display,
		window->window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	return height;
}

void rv_resize_window(rv_window* window, int width, int height)
{
	XResizeWindow(window->display, window->window, (unsigned)width, (unsigned)height);
	XSync(window->display, False);
}

static int rv_internal_get_pixel_offset_from_mask(unsigned long mask)
{
	switch (mask)
	{
		case 0x000000FF:
			return 0;

		case 0x0000FF00:
			return 1;

		case 0x00FF0000:
			return 2;

		case 0xFF000000:
			return 3;
	}

	assert(false);
	return 0;
}
