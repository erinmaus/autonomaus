// This file is a part of EIZNABAHN.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef RV_PRIVATE_INPUT_H
#define RV_PRIVATE_INPUT_H

#include <stdbool.h>

#ifndef RV_INCLUDE_API
	#error "don't include private headers; use 'rv.h' instead"
#endif

struct rv_window;

// Opaque object representing global hotkeys.
typedef struct rv_hotkey_queue rv_hotkey_queue;

enum
{
	rv_hotkey_modifier_none = 0x0,
	rv_hotkey_modifier_ctrl = 0x1,
	rv_hotkey_modifier_alt = 0x2,
	rv_hotkey_modifier_shift = 0x4
};

// Creates an `rv_hotkey_queue` object compatible with the provided window.,
rv_hotkey_queue* rv_create_hotkey_queue(struct rv_window* window);

// Destroys a previously created hotkey queue.
void rv_destroy_hotkey_queue(rv_hotkey_queue* hotkey_queue);

// Registers a hotkey.
//
// Returns true on success; otherwise, returns false on failure. Failure can
// occur if another application has registered the hotkey, or if a maximum
// number of hotkeys has been registered, or some other reason.
bool rv_register_hotkey(rv_hotkey_queue* hotkey_queue, int key);

// Checks for the next hotkey in the queue.
//
// Returns true and sets `key` and `modifiers` if a hotkey was in the queue;
// otherwise, returns false and leaves `key` and `modifiers` unmodified.
bool rv_check_hotkey(rv_hotkey_queue* hotkey_queue, int* key, int* modifiers);

// Opaque object representing event state.
//
// Sending an event requires signaling an event sequence with
// `rv_begin_event_sequence' to hide platform-specific differences This object
// holds the necessary state to do so.
typedef struct rv_input_event_state rv_input_event_state;

// Creates an `rv_input_event_state` object compatible with the provided window.
//
// Events sent using this state will be sent to `window'.
//
// Returns an `rv_input_event_state' object on success; otherwise, returns NULL
// on failure. Failure can occur due to exhausted resources or because of
// platform-specific conditions.
rv_input_event_state* rv_create_input_event_state(struct rv_window* window);

// Destroys a previously created state object.
void rv_destroy_input_event_state(rv_input_event_state* event_state);

// Signals the beginning of an event sequence.
//
// Any events, such as key presses or mouse movements, can safely be sent until
// a corresponding call to `rv_end_event_sequence'.
//
// Ideally, as many events should be sent as possible. For example, for the key
// combination 'Shift + R', the key down and key up events for both 'Shift' and
// 'R' should be sent prior to calling `rv_end_event_sequence', including any
// necessary artificial delays.
void rv_begin_event_sequence(rv_input_event_state* event_state);

// Ends an event sequence.
//
// See `rv_begin_event_sequence' for more information.
void rv_end_event_sequence(rv_input_event_state* event_state);

// Simulates a physical key press.
void rv_send_key_down(rv_input_event_state* window, int key, int delay);

// Simulates a physical key release.
void rv_send_key_up(rv_input_event_state* window, int key, int delay);

// Gets the location of the mouse cursor.
int rv_get_mouse_x(rv_input_event_state* event_state);
int rv_get_mouse_y(rv_input_event_state* event_state);

// Gets the state of the mouse buttons.
unsigned int rv_get_mouse_buttons(rv_input_event_state* event_state);

// Simulates a physical mouse movement.
//
// `x' and `y' are absolute to the display.
void rv_send_mouse_move(rv_input_event_state* window, int x, int y, int delay);

// Simulates a physical mouse button press.
//
// The location of the press can be changed with `rv_send_mouse_move' prior.
void rv_send_mouse_button_down(rv_input_event_state* window, int button, int delay);

// Simulates a physical mouse button release.
//
// The location of the press can be changed with `rv_send_mouse_move' prior.
void rv_send_mouse_button_up(rv_input_event_state* window, int button, int delay);

#endif
