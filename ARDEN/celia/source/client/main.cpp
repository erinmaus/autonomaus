// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <cstdlib>
#define ALLEGRO_UNSTABLE
#include <allegro5/allegro.h>
#include "celia/client/client.hpp"

int main()
{
	const char* address = std::getenv("ARDEN_CELIA_CLIENT_ADDRESS");
	if (address == nullptr)
	{
		address = "127.0.0.1";
	}

	const char* port = std::getenv("ARDEN_CELIA_CLIENT_PORT");
	if (port == nullptr)
	{
		port = "18625";
	}

	const char* frame_width = std::getenv("ARDEN_CELIA_CLIENT_FRAME_WIDTH");
	if (frame_width == nullptr)
	{
		frame_width = "0";
	}

	const char* frame_height = std::getenv("ARDEN_CELIA_CLIENT_FRAME_HEIGHT");
	if (frame_height == nullptr)
	{
		frame_height = "0";
	}

	if (!al_init())
	{
		std::fprintf(stderr, "couldn't initialize Allegro\n");
		std::abort();
	}

	al_install_keyboard();
	al_install_mouse();

	celia::Client client(address, std::atoi(port));
	client.set_frame_dimensions(std::atoi(frame_width), std::atoi(frame_height));
	client.run();
}
