// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_FORWARDER_INPUT_QUEUE_HPP
#define CELIA_FORWARDER_INPUT_QUEUE_HPP

#include <thread>
#include <X11/Xlib.h>

namespace celia
{
	class InputQueue
	{
	public:
		InputQueue();
		~InputQueue();

		static void set_window(Window value);

	private:
		void run();

		std::thread input_thread;

		bool is_running = true;

		static Window window;
	};
}

#endif
