// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_CALL_STREAM_HPP
#define TOUCHSTONE_TRACE_CALL_STREAM_HPP

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>

namespace touchstone
{
	namespace detail
	{
		template <typename Type>
		struct CallStreamResultReader;
	}

	class CallStream
	{
	public:
		// A call is at least a begin and end pair. A begin and end are simply
		// the call token. Therefore, the minimimum call size is the size of
		// two tokens.
		static const std::size_t MIN_CALL_SIZE = sizeof(std::uint32_t) * 2;

		// Reserved token representing a null (non-existent, empty, whatever)
		// call.
		static const std::uint32_t TOKEN_NONE = 0;

		class Slice
		{
		public:
			friend CallStream;

			template <typename T>
			friend class detail::CallStreamResultReader;

			Slice(const Slice& other) = default;
			~Slice() = default;

			template <typename Type>
			typename Type::type parameter();

			template <typename Type>
			Type result();

			std::size_t data_size();
			const std::uint8_t* data(std::size_t size);
			std::size_t data_array_count();
			const std::uint8_t* data_array(std::size_t length);

			operator bool();

			Slice& operator =(const Slice& other) = default;

		private:
			Slice(const CallStream* stream, std::size_t start_offset, std::uint32_t call_token);

			void advance(std::size_t count, std::size_t alignment);
			const std::uint8_t* get_data(std::size_t alignment) const;

			const CallStream* stream;
			std::size_t current_offset;
			std::uint32_t call_token;
		};

		CallStream(const std::uint8_t* frame_data, std::size_t frame_data_size);
		CallStream(const CallStream& other) = default;
		~CallStream() = default;

		const std::uint8_t* get_frame_data() const;
		std::size_t get_frame_data_size() const;

		std::size_t get_current_call_offset() const;
		std::uint32_t get_current_token() const;

		void seek(std::size_t offset);
		std::size_t tell() const;
		bool is_end_of_stream() const;

		Slice begin(std::uint32_t call_token);
		std::size_t end(Slice& slice);

		void reset();

		CallStream& operator =(const CallStream& other) = default;

	private:
		const std::uint8_t* frame_data;
		std::size_t frame_data_size;

		std::size_t offset;
	};

	namespace detail
	{
		template <typename Type>
		struct CallStreamResultReader
		{
			static Type read(CallStream::Slice& slice);
		};
	}
}

template <typename Type>
typename Type::type touchstone::CallStream::Slice::parameter()
{
	union
	{
		std::uint8_t bytes[Type::size];
		typename Type::type value;
	} result;

	std::memcpy(
		result.bytes, get_data(Type::alignment),
		Type::size);
	advance(Type::size, Type::alignment);

	return result.value;
}

template <typename Type>
Type touchstone::CallStream::Slice::result()
{
	return detail::CallStreamResultReader<Type>::read(*this);
}

template <typename Type>
inline Type touchstone::detail::CallStreamResultReader<Type>::read(
	CallStream::Slice& slice)
{
	union
	{
		std::uint8_t bytes[sizeof(Type)];
		Type value;
	} result;

	std::memcpy(
		result.bytes, slice.get_data(alignof(Type)),
		sizeof(Type));
	slice.advance(sizeof(Type), alignof(Type));

	return result.value;
}

#endif
