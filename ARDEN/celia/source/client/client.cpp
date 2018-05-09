// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <dlfcn.h>
#include "celia/client/client.hpp"
#include "celia/client/inputQueue.hpp"
#include "celia/common/buffer.hpp"
#include "celia/common/networkStream.hpp"

celia::Client::Client(const std::string& address, std::uint16_t port)
{
	this->stream.accept(address, port);

	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);
	this->display = al_create_display(800, 600);
}

celia::Client::~Client()
{
	if (this->display != nullptr)
	{
		al_destroy_display(this->display);
	}
}

void celia::Client::run()
{
	InputQueue input_queue(this->frame_width, this->frame_height);

	this->is_running = true;
	while(this->is_running)
	{
		Buffer buffer;
		std::string function;
		int flags;

		this->stream.receive(buffer, function, flags);

		call_function(function, buffer);
		if ((flags & NetworkStream::flag_reply))
		{
			if (!this->reply_sent)
			{
				std::fprintf(stderr, "%s expected reply, none sent\n", function.c_str());
				this->is_running = false;
			}
		}
		else if (this->reply_sent)
		{
			std::fprintf(stderr, "%s didn't request reply, but reply sent", function.c_str());
			this->is_running = false;
		}
	}
}

void celia::Client::resize(int width, int height)
{
	al_resize_display(this->display, width, height);
}

void celia::Client::reply(const std::string& function, const Buffer& buffer)
{
	this->reply_sent = true;
	this->stream.send(buffer, function, NetworkStream::flag_none);
}

void celia::Client::set_frame_dimensions(int width, int height)
{
	this->frame_width = width;
	this->frame_height = height;
}

void celia::Client::call_function(const std::string& function, Buffer& buffer)
{
	Function f = nullptr;
	auto iter = this->functions.find(function);
	if (iter != this->functions.end())
	{
		f = iter->second;
	}
	else
	{
		std::string cx_function = std::string("cx") + function;
		f = (Function)dlsym(nullptr, cx_function.c_str());
		if (f == nullptr)
		{
			std::fprintf(stderr, "couldn't find %s\n", cx_function.c_str());
			this->is_running = false;
			return;
		}

		this->functions[function] = f;
	}

	this->reply_sent = false;
	f(buffer, *this);
}
