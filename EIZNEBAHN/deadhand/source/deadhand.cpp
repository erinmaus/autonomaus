// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <chrono>
#include <X11/XKBlib.h>
#include "deadhand/deadhand.hpp"

deadhand::Deadhand::Deadhand(touchstone::ImmediateDispatch& dispatch) :
	ganymede::Plugin(dispatch),
	display(XOpenDisplay(nullptr)),
	work_thread([this] { this->work(); })
{
	// Nothing.
}

deadhand::Deadhand::~Deadhand()
{
	this->is_running = false;
	{
		XEvent event;
		event.xclient.type = ClientMessage;
		event.xclient.window = None;
		event.xclient.message_type = XA_CARDINAL;
		event.xclient.format = 0;

		XSendEvent(
			this->display, this->window,
			True, 0, &event);
		XFlush(this->display);
		work_thread.join();
	}
	XCloseDisplay(this->display);
}

void deadhand::Deadhand::after_switch_context(bool valid, WindowHandle handle)
{
	XSetWindowAttributes attributes = {};
	attributes.event_mask = KeyPressMask
		| KeyReleaseMask
		| PointerMotionMask
		| ButtonPressMask
		| ButtonReleaseMask;
	XChangeWindowAttributes(this->display, handle, CWEventMask, &attributes);
}

void deadhand::Deadhand::work()
{
	XkbSetDetectableAutoRepeat(this->display, True, nullptr);

	XEvent event;
	while (this->is_running)
	{
		if (XPending(this->display))
		{
			XNextEvent(this->display, &event);
		}
	}
}

extern "C"
ganymede::Plugin* ganymede_create_plugin(
	touchstone::ImmediateDispatch* dispatch)
{
	return new deadhand::Deadhand(*dispatch);
}
