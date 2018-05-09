// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstddef>
#include "celia/common/buffer.hpp"
#include "celia/common/networkStream.hpp"

celia::NetworkStream::NetworkStream()
	: socket(this->io_service)
{
	// Nada.
}

void celia::NetworkStream::connect(const std::string& address, std::uint16_t port)
{
	boost::asio::ip::tcp::resolver resolver(this->io_service);
	auto endpoint = resolver.resolve({ address, std::to_string(port) });

	boost::system::error_code error;

	boost::asio::connect(this->socket, endpoint, error);

	if (error)
	{
		std::fprintf(stderr, "couldn't connect to %s:%u\n", address.c_str(), port);
		std::abort();
	}

	this->socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

void celia::NetworkStream::accept(const std::string& address, std::uint16_t port)
{
	auto a = boost::asio::ip::address::from_string(address);
	boost::asio::ip::tcp::endpoint endpoint(a, port);
	boost::asio::ip::tcp::acceptor acceptor(this->io_service, endpoint);
	acceptor.accept(this->socket);

	this->socket.set_option(boost::asio::ip::tcp::no_delay(true));
}

void celia::NetworkStream::send(
	const Buffer& buffer, const std::string& function, int flags)
{
	Buffer primary_header;
	primary_header.push<std::uint16_t>(function.size());
	primary_header.push<std::uint16_t>(flags);
	primary_header.push<std::uint32_t>(buffer.get_data_size());
	send(primary_header);

	Buffer secondary_header;
	secondary_header.push_data(function.c_str(), function.size());
	send(secondary_header);

	send(buffer);
}

void celia::NetworkStream::receive(
	Buffer& buffer, std::string& function, int& flags)
{
	Buffer primary_header;
	const std::size_t primary_header_size = 8; // uint16, uint16, uint32
	receive(primary_header_size, primary_header);

	const auto secondary_header_size = primary_header.pop<std::uint16_t>();
	flags = (int)primary_header.pop<std::uint16_t>();
	const auto buffer_size = primary_header.pop<std::uint32_t>();

	Buffer secondary_header;
	receive(secondary_header_size, secondary_header);
	function = std::string((char*)secondary_header.get_data(), secondary_header.get_data_size());

	receive(buffer_size, buffer);
}

void celia::NetworkStream::send(const Buffer& buffer)
{
	boost::system::error_code ignored_error;
	boost::asio::write(
		this->socket,
		boost::asio::buffer(buffer.get_data(), buffer.get_data_size()),
		ignored_error);
}

void celia::NetworkStream::receive(std::size_t size, Buffer& buffer)
{
	Buffer b;
	b.push_data((void*)nullptr, size);

	boost::system::error_code ignored_error;
	boost::asio::read(
		this->socket,
		boost::asio::buffer(b.get_data(), b.get_data_size()),
		ignored_error);

	buffer = std::move(b);
}
