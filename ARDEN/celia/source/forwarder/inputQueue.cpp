// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <allegro5/allegro.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include "celia/common/buffer.hpp"
#include "celia/common/networkStream.hpp"
#include "celia/forwarder/inputQueue.hpp"

static void get_window_position(Display* display, Window window, int& x, int& y)
{
	Window root_window;
	unsigned int width, height;
	unsigned int border, depth;

	XGetGeometry(
		display,
		window,
		&root_window,
		&x,
		&y,
		&width,
		&height,
		&border,
		&depth);

	if (window != root_window)
	{
		int t_x, t_y;
		Window unused_window;
		XTranslateCoordinates(
			display,
			window,
			root_window,
			x, y,
			&t_x, &t_y,
			&unused_window);

		x = t_x;
		y = t_y;
	}
}

static int map_key(int allegro_key)
{
	struct KeyMap { int allegro; int x11; };
	static const KeyMap map[] =
	{
		{ ALLEGRO_KEY_A, XK_A },
		{ ALLEGRO_KEY_B, XK_B },
		{ ALLEGRO_KEY_C, XK_C },
		{ ALLEGRO_KEY_D, XK_D },
		{ ALLEGRO_KEY_E, XK_E },
		{ ALLEGRO_KEY_F, XK_F },
		{ ALLEGRO_KEY_G, XK_G },
		{ ALLEGRO_KEY_H, XK_H },
		{ ALLEGRO_KEY_I, XK_I },
		{ ALLEGRO_KEY_J, XK_J },
		{ ALLEGRO_KEY_K, XK_K },
		{ ALLEGRO_KEY_L, XK_L },
		{ ALLEGRO_KEY_M, XK_M },
		{ ALLEGRO_KEY_N, XK_N },
		{ ALLEGRO_KEY_O, XK_O },
		{ ALLEGRO_KEY_P, XK_P },
		{ ALLEGRO_KEY_Q, XK_Q },
		{ ALLEGRO_KEY_R, XK_R },
		{ ALLEGRO_KEY_S, XK_S },
		{ ALLEGRO_KEY_T, XK_T },
		{ ALLEGRO_KEY_U, XK_U },
		{ ALLEGRO_KEY_V, XK_V },
		{ ALLEGRO_KEY_W, XK_W },
		{ ALLEGRO_KEY_X, XK_X },
		{ ALLEGRO_KEY_Y, XK_Y },
		{ ALLEGRO_KEY_Z, XK_Z },
		{ ALLEGRO_KEY_0, XK_0 },
		{ ALLEGRO_KEY_1, XK_1 },
		{ ALLEGRO_KEY_2, XK_2 },
		{ ALLEGRO_KEY_3, XK_3 },
		{ ALLEGRO_KEY_4, XK_4 },
		{ ALLEGRO_KEY_5, XK_5 },
		{ ALLEGRO_KEY_6, XK_6 },
		{ ALLEGRO_KEY_7, XK_7 },
		{ ALLEGRO_KEY_8, XK_8 },
		{ ALLEGRO_KEY_9, XK_9 },
		{ ALLEGRO_KEY_PAD_0, XK_0 },
		{ ALLEGRO_KEY_PAD_1, XK_1 },
		{ ALLEGRO_KEY_PAD_2, XK_2 },
		{ ALLEGRO_KEY_PAD_3, XK_3 },
		{ ALLEGRO_KEY_PAD_4, XK_4 },
		{ ALLEGRO_KEY_PAD_5, XK_5 },
		{ ALLEGRO_KEY_PAD_6, XK_6 },
		{ ALLEGRO_KEY_PAD_7, XK_7 },
		{ ALLEGRO_KEY_PAD_8, XK_8 },
		{ ALLEGRO_KEY_PAD_9, XK_9 },
		{ ALLEGRO_KEY_F1, XK_F1 },
		{ ALLEGRO_KEY_F2, XK_F2 },
		{ ALLEGRO_KEY_F3, XK_F3 },
		{ ALLEGRO_KEY_F4, XK_F4 },
		{ ALLEGRO_KEY_F5, XK_F5 },
		{ ALLEGRO_KEY_F6, XK_F6 },
		{ ALLEGRO_KEY_F7, XK_F7 },
		{ ALLEGRO_KEY_F8, XK_F8 },
		{ ALLEGRO_KEY_F9, XK_F9 },
		{ ALLEGRO_KEY_F10, XK_F10 },
		{ ALLEGRO_KEY_F11, XK_F11 },
		{ ALLEGRO_KEY_F12, XK_F12 },
		{ ALLEGRO_KEY_ESCAPE, XK_Escape },
		{ ALLEGRO_KEY_TILDE, XK_grave },
		{ ALLEGRO_KEY_MINUS, XK_minus },
		{ ALLEGRO_KEY_EQUALS, XK_equal },
		{ ALLEGRO_KEY_BACKSPACE, XK_BackSpace },
		{ ALLEGRO_KEY_TAB, XK_Tab },
		{ ALLEGRO_KEY_OPENBRACE, XK_braceleft },
		{ ALLEGRO_KEY_CLOSEBRACE, XK_braceright },
		{ ALLEGRO_KEY_ENTER, XK_Return },
		{ ALLEGRO_KEY_SEMICOLON, XK_semicolon },
		{ ALLEGRO_KEY_QUOTE, XK_quoteleft },
		{ ALLEGRO_KEY_BACKSLASH, XK_backslash },
		{ ALLEGRO_KEY_COMMA, XK_comma },
		{ ALLEGRO_KEY_FULLSTOP, XK_period },
		{ ALLEGRO_KEY_SLASH, XK_slash },
		{ ALLEGRO_KEY_SPACE, XK_space },
		{ ALLEGRO_KEY_INSERT, XK_Insert },
		{ ALLEGRO_KEY_DELETE, XK_Delete },
		{ ALLEGRO_KEY_HOME, XK_Home },
		{ ALLEGRO_KEY_END, XK_End },
		{ ALLEGRO_KEY_PGUP, XK_Page_Up },
		{ ALLEGRO_KEY_PGDN, XK_Page_Down },
		{ ALLEGRO_KEY_LEFT, XK_Left },
		{ ALLEGRO_KEY_RIGHT, XK_Right },
		{ ALLEGRO_KEY_UP, XK_Up },
		{ ALLEGRO_KEY_DOWN, XK_Down },
		{ ALLEGRO_KEY_PAD_SLASH, XK_slash },
		{ ALLEGRO_KEY_PAD_ASTERISK, XK_asterisk },
		{ ALLEGRO_KEY_PAD_MINUS, XK_minus },
		{ ALLEGRO_KEY_PAD_PLUS, XK_plus },
		{ ALLEGRO_KEY_PAD_DELETE, XK_Delete },
		{ ALLEGRO_KEY_PAD_ENTER, XK_Return },
		{ ALLEGRO_KEY_PAUSE, XK_Pause },
		{ ALLEGRO_KEY_AT, XK_at },
		{ ALLEGRO_KEY_PAD_EQUALS, XK_equal },
		{ ALLEGRO_KEY_LSHIFT, XK_Shift_L },
		{ ALLEGRO_KEY_RSHIFT, XK_Shift_R },
		{ ALLEGRO_KEY_LCTRL, XK_Control_L },
		{ ALLEGRO_KEY_RCTRL, XK_Control_R },
		{ ALLEGRO_KEY_ALT, XK_Alt_L },
		{ ALLEGRO_KEY_ALTGR, XK_Alt_R },
		{ ALLEGRO_KEY_LWIN, XK_Super_L },
		{ ALLEGRO_KEY_RWIN, XK_Super_R },
		{ ALLEGRO_KEY_SCROLLLOCK, XK_Scroll_Lock },
		{ ALLEGRO_KEY_NUMLOCK, XK_Num_Lock },
		{ ALLEGRO_KEY_CAPSLOCK, XK_Caps_Lock },
		{ -1, -1 }
	};

	for (auto key = map; key->x11 != -1; ++key)
	{
		if (key->allegro == allegro_key)
		{
			return key->x11;
		}
	}

	return -1;
}

Window celia::InputQueue::window = 0;

celia::InputQueue::InputQueue()
	: input_thread([this] { this->run(); })
{
	// Nada.
}

celia::InputQueue::~InputQueue()
{
	this->is_running = false;
	this->input_thread.join();
}

void celia::InputQueue::set_window(Window value)
{
	window = value;
}

void celia::InputQueue::run()
{
	NetworkStream stream;
	{
		const char* address = std::getenv("ARDEN_CELIA_INPUT_CLIENT_ADDRESS");
		if (address == nullptr)
		{
			return;
		}

		const char* port = std::getenv("ARDEN_CELIA_INPUT_CLIENT_PORT");
		if (port == nullptr)
		{
			port = "18626";
		};

		stream.accept(address, std::atoi(port));
	}

	auto display = XOpenDisplay(nullptr);
	while (this->is_running)
	{
		Buffer buffer;
		std::string function;
		int flags;

		stream.receive(buffer, function, flags);

		if (window == 0)
		{
			continue;
		}

		if (function == "key_down")
		{
			auto key = map_key(buffer.pop<int>());
			if (key == -1)
			{
				continue;
			}

			XTestFakeKeyEvent(
				display,
				XKeysymToKeycode(display, key), True,
				CurrentTime);
		}
		else if (function == "key_up")
		{
			auto key = map_key(buffer.pop<int>());
			if (key == -1)
			{
				continue;
			}

			XTestFakeKeyEvent(
				display,
				XKeysymToKeycode(display, key), False,
				CurrentTime);
		}
		else if (function == "mouse_move")
		{
			auto x = buffer.pop<int>();
			auto y = buffer.pop<int>();
			auto z = buffer.pop<int>();
			int window_x, window_y;
			get_window_position(display, window, window_x, window_y);

			XTestFakeMotionEvent(
				display,
				XDefaultScreen(display),
				x + window_x, y + window_y,
				CurrentTime);
			if (z < 0)
			{
				XTestFakeButtonEvent(
					display,
					5,
					True,
					CurrentTime);
				XTestFakeButtonEvent(
					display,
					5,
					False,
					CurrentTime);
			}
			else if (z > 0)
			{
				XTestFakeButtonEvent(
					display,
					4,
					True,
					CurrentTime);
				XTestFakeButtonEvent(
					display,
					4,
					False,
					CurrentTime);
			}
		}
		else if (function == "mouse_down")
		{
			auto button = buffer.pop<int>();
			XTestFakeButtonEvent(
				display,
				button,
				True,
				CurrentTime);
		}
		else if (function == "mouse_up")
		{
			auto button = buffer.pop<int>();
			XTestFakeButtonEvent(
				display,
				button,
				False,
				CurrentTime);
		}
		XSync(display, False);
	}
}
