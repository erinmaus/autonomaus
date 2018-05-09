// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "celia/common/buffer.hpp"
#include "celia/common/networkStream.hpp"
#include "celia/forwarder/forwarder.hpp"
#include "celia/forwarder/inputQueue.hpp"

void celia::Forwarder::forward(const std::string& function, Buffer& buffer)
{
	if (!instance)
	{
		connect();
	}

	instance->stream.send(buffer, function, NetworkStream::flag_none);
}

void celia::Forwarder::forward_with_reply(
	const std::string& function, Buffer& buffer)
{
	if (!instance)
	{
		instance = std::make_unique<Instance>();
		connect();
	}

	instance->stream.send(buffer, function, NetworkStream::flag_reply);

	std::string reply_function;
	int flags;
	instance->stream.receive(buffer, reply_function, flags);

	if (reply_function != function)
	{
		std::fprintf(stderr, "expected %s reply, got %s\n", function.c_str(), reply_function.c_str());
		std::abort();
	}

	if (flags & NetworkStream::flag_reply)
	{
		std::fprintf(stderr, "%s reply also expects reply\n", function.c_str());
		std::abort();
	}
}

void celia::Forwarder::connect()
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

	instance->stream.connect(address, std::atoi(port));
	instance->input_queue = std::make_unique<InputQueue>();
}

std::unique_ptr<celia::Forwarder::Instance> celia::Forwarder::instance;

celia::Forwarder::Instance::Instance()
	: socket(this->io_service)
{
	// Nada.
}
