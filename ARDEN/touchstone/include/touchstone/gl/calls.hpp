// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_CALLS_HPP
#define TOUCHSTONE_GL_CALLS_HPP

#include "touchstone/gl/types.hpp"
#include "touchstone/trace/call.hpp"

namespace touchstone
{
	class CallStream;
	class Dispatch;

	class GLCall : public Call
	{
	public:
		virtual void execute(Dispatch& gl) = 0;
	};

	template <std::uint32_t Token, typename DerivedType>
	class TGLCall : public GLCall
	{
	public:
		static const std::uint32_t token = Token;

		TGLCall(std::size_t call_num = 0);

		std::uint32_t get_token() const override;
		std::size_t get_call_num() const override;

		Call* clone() const override;

	protected:
		static const std::uint8_t* get_index_offset(std::size_t index, const std::uint8_t* data);
		static std::size_t get_index_size(std::size_t index, const std::uint8_t* data);

	private:
		std::size_t call_num;
	};

#include "touchstone/gl/calls.hpp.inl"
}

template <std::uint32_t Token, typename DerivedType>
const std::uint32_t touchstone::TGLCall<Token, DerivedType>::token;

template <std::uint32_t Token, typename DerivedType>
touchstone::TGLCall<Token, DerivedType>::TGLCall(std::size_t call_num)
{
	this->call_num = call_num;
}

template <std::uint32_t Token, typename DerivedType>
std::uint32_t touchstone::TGLCall<Token, DerivedType>::get_token() const
{
	return this->token;
}

template <std::uint32_t Token, typename DerivedType>
std::size_t touchstone::TGLCall<Token, DerivedType>::get_call_num() const
{
	return this->call_num;
}

template <std::uint32_t Token, typename DerivedType>
touchstone::Call* touchstone::TGLCall<Token, DerivedType>::clone() const
{
	auto result = new DerivedType(*(const DerivedType*)this);
	result->call_num = this->call_num;

	return result;
}

template <std::uint32_t Token, typename DerivedType>
const std::uint8_t* touchstone::TGLCall<Token, DerivedType>::get_index_offset(
	std::size_t index, const std::uint8_t* data)
{
	std::size_t current_offset = 0;
	std::size_t next_offset = 0;
	std::size_t i = 0;
	do
	{
		current_offset += next_offset;

		auto length = *((const std::uint32_t*)(data + current_offset));
		current_offset += 4;
		next_offset = length;

		++i;
	} while (i < index);

	return data + current_offset;
}

template <std::uint32_t Token, typename DerivedType>
std::size_t touchstone::TGLCall<Token, DerivedType>::get_index_size(
	std::size_t index, const std::uint8_t* data)
{
	std::size_t current_offset = 0;
	std::size_t last_length = 0;
	std::size_t i = 0;
	do
	{
		auto length = *((const std::uint32_t*)(data + current_offset));
		current_offset += 4 + length;

		last_length = length;
		++i;
	} while (i < index);

	return last_length;
}

#endif
