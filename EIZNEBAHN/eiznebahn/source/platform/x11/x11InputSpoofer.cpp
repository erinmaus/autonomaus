// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifdef EIZNEBAHN_BUILD_X11

#include <cassert>
#include <string>
#include <X11/Xatom.h>
#include "eiznebahn/platform/x11/x11InputSpoofer.hpp"

eiznebahn::X11InputSpoofer::X11InputSpoofer()
{
	this->display = XOpenDisplay(nullptr);
	{
		Window unused_root, unused_child;
		int unused_window_x, unused_window_y;
		unsigned int mask;
		XQueryPointer(
			this->display, DefaultRootWindow(this->display),
			&unused_root, &unused_child,
			&current_mouse_x, &current_mouse_y,
			&unused_window_x, &unused_window_y,
			&mask);

		this->current_button_mask = mask & BUTTON_MASK;
		this->current_key_mask = mask & KEY_MASK;
	}
	XkbSetDetectableAutoRepeat(display, True, nullptr);

	this->time_prop_atom = XInternAtom(this->display, "EIZNEBAHN_TIME_PROP", False);

	XSetWindowAttributes attributes = {};
	attributes.event_mask = PropertyChangeMask;
	this->dummy_window = XCreateSimpleWindow(
		this->display,
		XRootWindow(this->display, XDefaultScreen(this->display)),
		0, 0, 1, 1, 0, 0, 0);
	XChangeWindowAttributes(this->display, this->dummy_window, CWEventMask, &attributes);
}

eiznebahn::X11InputSpoofer::~X11InputSpoofer()
{
	XDestroyWindow(this->display, this->dummy_window);
	XCloseDisplay(this->display);
}

void eiznebahn::X11InputSpoofer::set_target_window(Window window)
{
	std::lock_guard<std::mutex> lock(this->mutex);
	this->target_window = window;
	if (window != None)
	{
		XEvent event;
		event.xfocus.type = FocusIn;
		event.xfocus.window = window;
		event.xfocus.mode = NotifyNormal;
		event.xfocus.detail = NotifyNonlinear;

		XSendEvent(
			this->display, this->target_window,
			True, FocusChangeMask, &event);
		XFlush(this->display);
	}
}

void eiznebahn::X11InputSpoofer::mouse_move(int x, int y)
{
	assert(this->target_window != None);
	std::lock_guard<std::mutex> lock(this->mutex);

	XEvent event;
	if (!window_to_root(x, y, event.xmotion.x_root, event.xmotion.y_root))
	{
		return;
	}

	event.xmotion.type = MotionNotify;
	event.xmotion.window = this->target_window;
	event.xmotion.root = get_root_window();
	event.xmotion.subwindow = None;
	event.xmotion.time = get_current_time();
	event.xmotion.x = x;
	event.xmotion.y = y;
	event.xmotion.state = this->current_key_mask | this->current_button_mask;
	event.xmotion.is_hint = 0;
	event.xmotion.same_screen = True;

	this->current_mouse_x = event.xmotion.x_root;
	this->current_mouse_y = event.xmotion.y_root;

	++this->num_pending_events;
	XSendEvent(
		this->display, this->target_window,
		True, PointerMotionMask, &event);
	XFlush(this->display);
	this->input_state.set_mouse_position(x, y);
}

void eiznebahn::X11InputSpoofer::mouse_scroll_down()
{
	mouse_button_press(MouseButton::button_down);
	mouse_button_release(MouseButton::button_down);
	this->input_state.mouse_scroll_down();
}

void eiznebahn::X11InputSpoofer::mouse_scroll_up()
{
	mouse_button_press(MouseButton::button_up);
	mouse_button_release(MouseButton::button_up);
	this->input_state.mouse_scroll_up();
}

void eiznebahn::X11InputSpoofer::mouse_button_press(int button)
{
	assert(this->target_window != None);
	std::lock_guard<std::mutex> lock(this->mutex);

	assert(button >= MouseButton::button_min);
	assert(button < MouseButton::button_max);

	XEvent event;
	if (!root_to_window(
		this->current_mouse_x, this->current_mouse_y,
		event.xbutton.x, event.xbutton.y))
	{
		return;
	}

	event.xbutton.type = ButtonPress;
	event.xbutton.window = this->target_window;
	event.xbutton.root = get_root_window();
	event.xbutton.subwindow = None;
	event.xbutton.time = get_current_time();
	event.xbutton.x_root = this->current_mouse_x;
	event.xbutton.y_root = this->current_mouse_y;
	event.xbutton.state = this->current_key_mask | this->current_button_mask;
	event.xbutton.button = MOUSE_MAP[button];
	event.xbutton.same_screen = True;

	++this->num_pending_events;
	XSendEvent(
		this->display, this->target_window,
		True, ButtonPressMask, &event);
	XFlush(this->display);

	this->current_button_mask |= MOUSE_MAP_MASK[button];
	this->input_state.set_mouse_button_pressed(button);
}

void eiznebahn::X11InputSpoofer::mouse_button_release(int button)
{
	assert(this->target_window != None);
	std::lock_guard<std::mutex> lock(this->mutex);

	assert(button >= MouseButton::button_min);
	assert(button < MouseButton::button_max);

	XEvent event;
	if (!root_to_window(
		this->current_mouse_x, this->current_mouse_y,
		event.xbutton.x, event.xbutton.y))
	{
		return;
	}

	event.xbutton.type = ButtonRelease;
	event.xbutton.window = this->target_window;
	event.xbutton.root = get_root_window();
	event.xbutton.subwindow = None;
	event.xbutton.time = get_current_time();
	event.xbutton.x_root = this->current_mouse_x;
	event.xbutton.y_root = this->current_mouse_y;
	event.xbutton.state = this->current_key_mask | this->current_button_mask;
	event.xbutton.button = MOUSE_MAP[button];
	event.xbutton.same_screen = True;
	++this->num_pending_events;
	XSendEvent(
		this->display, this->target_window,
		True, ButtonReleaseMask, &event);
	XFlush(this->display);

	this->current_button_mask &= ~MOUSE_MAP_MASK[button];
	this->input_state.set_mouse_button_released(button);
}

void eiznebahn::X11InputSpoofer::key_press(int key_code)
{
	assert(this->target_window != None);
	std::lock_guard<std::mutex> lock(this->mutex);

	assert(key_code >= 0);
	assert(key_code < KeySequence::key_code_max);

	KeySym key_sym;
	if (!(this->current_key_mask & ShiftMask))
	{
		KeySym lower_key_sym, upper_key_sym;
		XConvertCase(KEY_MAP[key_code], &lower_key_sym, &upper_key_sym);
		key_sym = lower_key_sym;
	}
	else
	{
		key_sym = KEY_MAP[key_code];
	}

	XEvent event;
	event.xkey.type = KeyPress;
	event.xkey.window = this->target_window;
	event.xkey.root = get_root_window();
	event.xkey.subwindow = None;
	event.xkey.time = get_current_time();
	root_to_window(this->current_mouse_x, this->current_mouse_y, event.xkey.x, event.xkey.y);
	event.xkey.x_root = this->current_mouse_x;
	event.xkey.y_root = this->current_mouse_y;
	event.xkey.state = this->current_key_mask | this->current_button_mask;
	event.xkey.keycode = XKeysymToKeycode(this->display, key_sym);
	event.xkey.same_screen = true;
	++this->num_pending_events;
	XSendEvent(
		this->display, this->target_window,
		True, KeyPressMask, &event);
	XFlush(this->display);

	this->current_key_mask |= KEY_MAP_MASK[key_code];
	this->input_state.set_key_pressed(key_code);
}

void eiznebahn::X11InputSpoofer::key_release(int key_code)
{
	assert(this->target_window != None);
	std::lock_guard<std::mutex> lock(this->mutex);

	assert(key_code >= 0);
	assert(key_code < KeySequence::key_code_max);

	KeySym key_sym;
	if (!(this->current_key_mask & ShiftMask))
	{
		KeySym lower_key_sym, upper_key_sym;
		XConvertCase(KEY_MAP[key_code], &lower_key_sym, &upper_key_sym);
		key_sym = lower_key_sym;
	}
	else
	{
		key_sym = KEY_MAP[key_code];
	}

	XEvent event;
	event.xkey.type = KeyRelease;
	event.xkey.window = this->target_window;
	event.xkey.root = get_root_window();
	event.xkey.subwindow = None;
	event.xkey.time = get_current_time();
	root_to_window(this->current_mouse_x, this->current_mouse_y, event.xkey.x, event.xkey.y);
	event.xkey.x_root = this->current_mouse_x;
	event.xkey.y_root = this->current_mouse_y;
	event.xkey.state = this->current_key_mask | this->current_button_mask;
	event.xkey.keycode = XKeysymToKeycode(this->display, key_sym);
	event.xkey.same_screen = true;
	++this->num_pending_events;
	XSendEvent(
		this->display, this->target_window,
		True, KeyReleaseMask, &event);
	XFlush(this->display);

	this->current_key_mask &= ~KEY_MAP_MASK[key_code];
	this->input_state.set_key_released(key_code);
}

void eiznebahn::X11InputSpoofer::poll()
{
	this->real_input_state.set_text_input(std::string());
}

const eiznebahn::InputState& eiznebahn::X11InputSpoofer::get_input_state() const
{
	return this->input_state;
}

const eiznebahn::InputState& eiznebahn::X11InputSpoofer::get_real_input_state() const
{
	return this->real_input_state;
}

void eiznebahn::X11InputSpoofer::disable_real_events()
{
	std::lock_guard<std::mutex> lock(this->mutex);

	this->real_events_enabled = false;
}

void eiznebahn::X11InputSpoofer::enable_real_events()
{
	std::lock_guard<std::mutex> lock(this->mutex);

	this->real_events_enabled = true;
}

bool eiznebahn::X11InputSpoofer::get_real_events_enabled() const
{
	std::lock_guard<std::mutex> lock(this->mutex);

	return this->real_events_enabled;
}

int eiznebahn::X11InputSpoofer::get_num_pending_events() const
{
	return this->num_pending_events < 0 ? 0 : this->num_pending_events;
}

void eiznebahn::X11InputSpoofer::acknowledge_pending_event()
{
	--this->num_pending_events;
	if (this->num_pending_events < 0)
	{
		this->num_pending_events = 0;
	}
}

void eiznebahn::X11InputSpoofer::register_mouse_callback(const MouseEventCallback& callback)
{
	std::lock_guard<std::mutex> lock(this->mutex);

	this->mouse_callback = std::make_unique<MouseEventCallback>(callback);
}

void eiznebahn::X11InputSpoofer::register_keyboard_callback(const KeyboardEventCallback& callback)
{
	std::lock_guard<std::mutex> lock(this->mutex);

	this->keyboard_callback = std::make_unique<KeyboardEventCallback>(callback);
}

void eiznebahn::X11InputSpoofer::make_current()
{
	current = this;
}

eiznebahn::X11InputSpoofer* eiznebahn::X11InputSpoofer::get_current()
{
	return current;
}

bool eiznebahn::X11InputSpoofer::process_event(const XEvent& event)
{
	if (event.xany.send_event)
	{
		return false;
	}

	std::lock_guard<std::mutex> lock(this->mutex);

	bool is_keyboard_event = false;
	bool is_mouse_event = false;
	int event_type;
	int key_code;
	switch (event.type)
	{
		case KeyPress:
			{
				if(try_marshal_key_code_from_x11(event.xkey.keycode, key_code))
				{
					this->real_input_state.set_key_pressed(key_code);
				}

				int key_sym = XkbKeycodeToKeysym(this->display, event.xkey.keycode, 0, 0);
				if (key_sym != NoSymbol)
				{
					char buffer[32] = {};
					XLookupString(const_cast<XKeyEvent*>(&event.xkey), buffer, sizeof(buffer), nullptr, nullptr);

					auto current_input = this->real_input_state.get_text_input();
					for (int i = 0; i < sizeof(buffer); ++i)
					{
						if (buffer[i] >= ' ' && buffer[i] <= '~')
						{
							current_input += buffer[i];
						}
					}

					this->real_input_state.set_text_input(current_input);
				}

				is_keyboard_event = true;
				event_type = event_key_press;
			}
			break;
		case KeyRelease:
			{
				if(try_marshal_key_code_from_x11(event.xkey.keycode, key_code))
				{
					this->real_input_state.set_key_released(key_code);
				}

				is_keyboard_event = true;
				event_type = event_key_release;
			}
			break;
		case MotionNotify:
			this->real_input_state.set_mouse_position(event.xmotion.x, event.xmotion.y);
			if (this->real_events_enabled)
			{
				this->input_state.set_mouse_position(event.xmotion.x, event.xmotion.y);
			}

			is_mouse_event = true;
			event_type = event_mouse_move;
			break;
		case ButtonPress:
			{
				int button;
				if(try_marshal_mouse_button_from_x11(event.xbutton.button, button))
				{
					if (button == MouseButton::button_up)
					{
						this->real_input_state.mouse_scroll_up();
						event_type = event_mouse_scroll_up;
					}
					else if (button == MouseButton::button_down)
					{
						this->real_input_state.mouse_scroll_down();
						event_type = event_mouse_scroll_down;
					}
					else
					{
						this->real_input_state.set_mouse_button_pressed(button);
						event_type = event_mouse_button_press;
					}
				}

				is_mouse_event = true;
			}
			break;
		case ButtonRelease:
			{
				int button;
				if(try_marshal_mouse_button_from_x11(event.xbutton.button, button))
				{
					if (button != MouseButton::button_up &&
						button != MouseButton::button_down)
					{
						this->real_input_state.set_mouse_button_released(button);
					}
				}

				is_mouse_event = true;
				event_type = event_mouse_button_release;
			}
			break;
	}

	if (is_keyboard_event && this->keyboard_callback)
	{
		return (*this->keyboard_callback.get())(event_type, key_code);
	}
	else if (is_mouse_event && this->mouse_callback)
	{
		return (*this->mouse_callback.get())(event_type, event.xmotion.x, event.xmotion.y);
	}

	return false;
}

bool eiznebahn::X11InputSpoofer::window_to_root(
	int input_x, int input_y,
	int& output_x, int& output_y)
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		this->display,
		this->target_window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	if (this->target_window != root_window)
	{
		Window unused_window;
		XTranslateCoordinates(
			this->display,
			this->target_window,
			root_window,
			x, y,
			&output_x, &output_y,
			&unused_window);

		if (output_x < 0 || output_x > width || output_y < 0 || output_y > height)
		{
			return false;
		}
	}
	else
	{
		output_x = x;
		output_y = y;
	}

	return true;
}

bool eiznebahn::X11InputSpoofer::root_to_window(
	int input_x, int input_y,
	int& output_x, int& output_y)
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		this->display,
		this->target_window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	output_x = input_x - x;
	output_y = input_y - y;

	if (output_x < 0 || output_x > width || output_y < 0 || output_y > height)
	{
		return false;
	}

	return true;
}

Window eiznebahn::X11InputSpoofer::get_root_window()
{
	Window root_window;
	int x, y;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		this->display,
		this->target_window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	return root_window;
}

Bool eiznebahn::X11InputSpoofer::filter_timestamp_event(
	Display *display, XEvent* event, XPointer argument)
{
	X11InputSpoofer* spoofer = (X11InputSpoofer*)argument;
	if (event->type == PropertyNotify &&
		event->xproperty.window == spoofer->dummy_window &&
		event->xproperty.atom == spoofer->time_prop_atom)
	{
		return True;
	}

	return False;
}

Time eiznebahn::X11InputSpoofer::get_current_time()
{
	unsigned char value = 0;
	XChangeProperty(
		this->display, this->dummy_window,
		this->time_prop_atom, this->time_prop_atom, 8, PropModeReplace,
		&value, sizeof(unsigned char));

	XEvent event;
	XIfEvent(this->display, &event, filter_timestamp_event, (XPointer)this);
	return event.xproperty.time;
}

eiznebahn::X11InputSpoofer* eiznebahn::X11InputSpoofer::current = nullptr;
const int eiznebahn::X11InputSpoofer::BUTTON_MASK = Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask;
const int eiznebahn::X11InputSpoofer::KEY_MASK = ShiftMask | LockMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask;
const int eiznebahn::X11InputSpoofer::KEY_MAP[] =
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

	XK_F1,
	XK_F2,
	XK_F3,
	XK_F4,
	XK_F5,
	XK_F6,
	XK_F7,
	XK_F8,
	XK_F9,
	XK_F10,
	XK_F11,
	XK_F12,

	XK_Control_L,
	XK_Control_R,
	XK_Alt_L,
	XK_Alt_R,
	XK_Shift_L,
	XK_Shift_R
};

const int eiznebahn::X11InputSpoofer::KEY_MAP_MASK[] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

	ControlMask,
	ControlMask,
	Mod1Mask,
	Mod1Mask,
	ShiftMask,
	ShiftMask
};

const int eiznebahn::X11InputSpoofer::MOUSE_MAP[] =
{
	0,
	1,
	3,
	2,
	4,
	5
};

const int eiznebahn::X11InputSpoofer::MOUSE_MAP_MASK[] =
{
	0,
	Button1Mask,
	Button3Mask,
	Button2Mask,
	Button4Mask,
	Button5Mask
};

bool eiznebahn::X11InputSpoofer::try_marshal_key_code_from_x11(
	int x11_key_code, int& key_code) const
{
	KeySym key_sym = XkbKeycodeToKeysym(this->display, x11_key_code, 0, 0);
	if (key_sym == NoSymbol)
	{
		return false;
	}

	KeySym upper_key_sym, lower_key_sym;
	XConvertCase(key_sym, &lower_key_sym, &upper_key_sym);

	for (int i = 0; i < KeySequence::key_code_max; ++i)
	{
		if (KEY_MAP[i] == upper_key_sym)
		{
			key_code = i;
			return true;
		}
	}

	return false;
}

bool eiznebahn::X11InputSpoofer::try_marshal_mouse_button_from_x11(
	int x11_mouse_button, int& mouse_button) const
{
	for (int i = MouseButton::button_min; i < MouseButton::button_max; ++i)
	{
		if (MOUSE_MAP[i] == x11_mouse_button)
		{
			mouse_button = i;
			return true;
		}
	}

	return false;
}

#endif
