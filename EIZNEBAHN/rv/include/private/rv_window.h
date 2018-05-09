// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef RV_PRIVATE_WINDOW_H
#define RV_PRIVATE_WINDOW_H

#include <stddef.h>

#ifndef RV_INCLUDE_API
	#error "don't include private headers; use 'rv.h' instead"
#endif

struct rv_image;

// Opaque object representing a UI window.
typedef struct rv_window rv_window;

// Interactively selects the next window.
//
// The user is notified by some means to click on a window. The window clicked
// on is returned by this method, if possible.
//
// Returns a valid window object on success; otherwise, returns NULL on failure.
rv_window* rv_select_window();

// Creates a window from a handle.
//
// Returns a valid window object on success; otherwise, returns NULL on failure.
rv_window* rv_window_from_handle(size_t handle);

// Frees a window object.
void rv_destroy_window(rv_window* window);

// Methods to obtain the absolute position of the client portion of the window,
// relative to the upper-left corner of the screen, which is considered (0, 0).
int rv_get_window_x(rv_window* window);
int rv_get_window_y(rv_window* window);
int rv_get_window_width(rv_window* window);
int rv_get_window_height(rv_window* window);

// Resizes the window.
//
// This does not take into account the window frame. As such, the actual window
// dimensions on the display will be larger to accomdate.
void rv_resize_window(rv_window* window, int width, int height);

#endif
