// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_CLIENT_CLIENT_HPP
#define CELIA_CLIENT_CLIENT_HPP

#include <cstdint>
#include <string>
#include <memory>
#include <map>
#include <allegro5/allegro.h>
#include <boost/asio.hpp>
#include "celia/common/networkStream.hpp"

namespace celia
{
	class Buffer;

	class Client
	{
	public:
		Client(const std::string& address, std::uint16_t port);
		~Client();

		void run();

		void resize(int width, int height);
		void reply(const std::string& function, const Buffer& buffer);

		void set_frame_dimensions(int width, int height);

	private:
		void call_function(const std::string& method, Buffer& buffer);

		typedef void (* Function)(Buffer& buffer, Client& client);
		std::map<std::string, Function> functions;

		boost::asio::io_service io_service;
		NetworkStream stream;

		ALLEGRO_DISPLAY* display = nullptr;
		bool is_running = true;
		bool reply_sent = false;

		int frame_width = 0, frame_height = 0;
	};
}

#endif
