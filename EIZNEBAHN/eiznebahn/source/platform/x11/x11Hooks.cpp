// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifdef EIZNEBAHN_BUILD_X11

#include <dlfcn.h>
#include "eiznebahn/platform/x11/x11InputSpoofer.hpp"

static bool is_input_event(XEvent* event)
{
	return event->type == KeyPress ||
		event->type == KeyRelease ||
		event->type == MotionNotify ||
		event->type == ButtonPress ||
		event->type == ButtonRelease;
}

static bool eiznebahn_skip_events = false;
extern "C"
int XEventsQueued(Display* display, int mode)
{
	typedef int (* XEventsQueuedProc)(Display*, int);
	static auto original_XEventsQueued = (XEventsQueuedProc)dlsym(RTLD_NEXT, "XEventsQueued");

	auto spoofer = eiznebahn::X11InputSpoofer::get_current();
	if (spoofer != nullptr && !spoofer->get_real_events_enabled())
	{
		if (spoofer->get_num_pending_events() == 0)
		{
			eiznebahn_skip_events = true;
			while (original_XEventsQueued(display, mode))
			{
				XEvent event;
				XNextEvent(display, &event);
			}
			eiznebahn_skip_events = false;

			return 0;
		}

		return spoofer->get_num_pending_events();
	}
	else
	{
		return original_XEventsQueued(display, mode);
	}
}

extern "C"
int XNextEvent(Display* display, XEvent* event_return)
{
	typedef int (* XNextEventProc)(Display*, XEvent*);
	static auto original_XNextEvent = (XNextEventProc)dlsym(RTLD_NEXT, "XNextEvent");

	int result;
	while (true)
	{
		result = original_XNextEvent(display, event_return);
		auto spoofer = eiznebahn::X11InputSpoofer::get_current();

		if (spoofer == nullptr)
		{
			break;
		}

		bool suppress = spoofer->process_event(*event_return);

		if (eiznebahn_skip_events)
		{
			break;
		}

		if (suppress && !event_return->xany.send_event)
		{
			continue;
		}

		if (is_input_event(event_return))
		{
			if (event_return->xany.send_event)
			{
				spoofer->acknowledge_pending_event();
				break;
			}

			if (spoofer->get_real_events_enabled() &&
				!eiznebahn_skip_events)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	event_return->xany.send_event = False;
	return result;
}

extern "C"
int XPeekEvent(Display* display, XEvent* event_return)
{
	typedef int (* XNextEventProc)(Display*, XEvent*);
	static auto original_XNextEvent = (XNextEventProc)dlsym(RTLD_NEXT, "XNextEvent");
	typedef int (* XPeekEventProc)(Display*, XEvent*);
	static auto original_XPeekEvent = (XPeekEventProc)dlsym(RTLD_NEXT, "XPeekEvent");

	int result;
	while (true)
	{
		result = original_XPeekEvent(display, event_return);
		auto spoofer = eiznebahn::X11InputSpoofer::get_current();

		if (spoofer == nullptr)
		{
			break;
		}

		if (is_input_event(event_return))
		{
			if (event_return->xany.send_event)
			{
				break;
			}

			if (!spoofer->get_real_events_enabled())
			{
				original_XNextEvent(display, event_return);
			}
		}
	}

	event_return->xany.send_event = False;
	return result;
}

#endif
