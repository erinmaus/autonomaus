// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_CLIENT_INPUT_QUEUE_HPP
#define CELIA_CLIENT_INPUT_QUEUE_HPP

#include <thread>
#include <allegro5/allegro.h>

namespace celia
{
	class NetworkStream;

	class InputQueue
	{
	public:
		InputQueue(int frame_width, int frame_height);
		~InputQueue();

	private:
		void run();

		std::thread input_thread;

		bool is_running = true;
		int mouse_offset_x, mouse_offset_y;
	};
}

#endif
