// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_COMMON_NETWORK_STREAM_HPP
#define CELIA_COMMON_NETWORK_STREAM_HPP

#include <iostream>
#include <string>
#include <boost/asio.hpp>

namespace celia
{
	class Buffer;

	class NetworkStream
	{
	public:
		enum
		{
			flag_none = 0x0,
			flag_reply = 0x1
		};

		NetworkStream();
		~NetworkStream() = default;

		void connect(const std::string& address, std::uint16_t port);
		void accept(const std::string& address, std::uint16_t port);

		void send(const Buffer& buffer, const std::string& function, int flags);
		void receive(Buffer& buffer, std::string& function, int& flags);

	private:
		void send(const Buffer& buffer);
		void receive(std::size_t size, Buffer& buffer);

		boost::asio::io_service io_service; 
		boost::asio::ip::tcp::socket socket;
	};
}

#endif
