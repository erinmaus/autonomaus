// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef RV_INTERNAL_H
#define RV_INTERNAL_H

#include <stdio.h>
#include <X11/Xlib.h>

struct rv_window
{
	// X11 display.
	Display* display;

	// X11 window instance.
	Window window;
};

struct rv_input_event_state
{
	Display* display;
	rv_window* target_window;
	int current_delay;
};

#define RV_MAX_HOTKEYS 8

struct rv_hotkey_queue
{
	rv_window* target_window;

	int hotkey_count;
	int hotkeys[RV_MAX_HOTKEYS];
};

#endif
