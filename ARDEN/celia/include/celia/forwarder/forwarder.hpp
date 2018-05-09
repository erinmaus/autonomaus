// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_FORWARDER_FORWARDER_HPP
#define CELIA_FORWARDER_FORWARDER_HPP

#include <cstdint>
#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "celia/common/networkStream.hpp"

namespace celia
{
	class Buffer;
	class InputQueue;

	class Forwarder
	{
	public:
		static void forward(const std::string& function, Buffer& buffer);
		static void forward_with_reply(
			const std::string& function, Buffer& buffer);

	private:
		static void connect();

		struct Instance
		{
			Instance();
			~Instance() = default;

			boost::asio::io_service io_service;
			boost::asio::ip::tcp::socket socket;
			NetworkStream stream;
			std::unique_ptr<InputQueue> input_queue;
		};

		static std::unique_ptr<Instance> instance;
	};
}

#endif
