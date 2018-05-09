// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <allegro5/allegro.h>
#include "celia/client/inputQueue.hpp"
#include "celia/common/buffer.hpp"
#include "celia/common/networkStream.hpp"

celia::InputQueue::InputQueue(int frame_width, int frame_height) :
	input_thread([this] { this->run(); }),
	mouse_offset_x(-frame_width),
	mouse_offset_y(-frame_height)
{
	// Nada.
}

celia::InputQueue::~InputQueue()
{
	this->is_running = false;
	this->input_thread.join();
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

		stream.connect(address, std::atoi(port));
	}

	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	while (this->is_running)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		const char* function = nullptr;
		celia::Buffer result;
		switch (event.type)
		{
			case ALLEGRO_EVENT_KEY_DOWN:
				function = "key_down";
				result.push<int>(event.keyboard.keycode);
				break;
			case ALLEGRO_EVENT_KEY_UP:
				function = "key_up";
				result.push<int>(event.keyboard.keycode);
				break;
			case ALLEGRO_EVENT_MOUSE_AXES:
				function = "mouse_move";
				result.push<int>(event.mouse.x + mouse_offset_x);
				result.push<int>(event.mouse.y + mouse_offset_y);
				result.push<int>(event.mouse.dz);
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
				function = "mouse_down";
				switch (event.mouse.button)
				{
					case 2:
						result.push<int>(3);
					case 3:
						result.push<int>(2);
					default:
						result.push<int>(1);
				}
				break;
			case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
				function = "mouse_up";
				switch (event.mouse.button)
				{
					case 2:
						result.push<int>(3);
					case 3:
						result.push<int>(2);
					default:
						result.push<int>(1);
				}
				break;
		}

		if (function != nullptr)
		{
			stream.send(result, function, NetworkStream::flag_none);
		}
	}
}
