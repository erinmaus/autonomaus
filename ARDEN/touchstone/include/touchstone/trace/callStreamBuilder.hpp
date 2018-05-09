// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_CALL_STREAM_BUILDER_HPP
#define TOUCHSTONE_TRACE_CALL_STREAM_BUILDER_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <vector>

namespace touchstone
{
	class CallStreamBuilder;

	namespace detail
	{
		template <typename Type>
		struct CallStreamResultWriter
		{
			static void write(CallStreamBuilder& build, const Type& value);
		};
	}

	class CallStreamBuilder
	{
	public:
		static const std::size_t INITIAL_SIZE = 16 * 1024 * 1024;

		CallStreamBuilder();
		~CallStreamBuilder() = default;

		void append(
			std::size_t size, std::size_t alignment,
			const std::uint8_t* bytes);
		void reset();

		template <typename Call>
		void begin();

		template <typename Call>
		void end();

		template <typename Type>
		void parameter(const typename Type::type& value);

		void data(std::size_t num_bytes, const void* value);

		template <typename Type>
		void data(
			const std::vector<std::size_t>& counts, const Type* const* values);

		template <typename Type>
		void result(const Type& result);

		const std::uint8_t* get_data() const;
		std::size_t get_size() const;
		std::size_t get_offset(std::size_t alignment) const;

	private:
		void resize(std::size_t minimum_size);

		typedef std::unique_ptr<std::uint8_t[]> BlockPointer;
		BlockPointer block;
		std::size_t block_size;
		std::size_t block_offset;
	};
}

template <typename Call>
inline void touchstone::CallStreamBuilder::begin()
{
	append(
		sizeof(std::uint32_t), alignof(std::max_align_t),
		(const std::uint8_t*)&Call::token);
}

template <typename Call>
inline void touchstone::CallStreamBuilder::end()
{
	append(
		sizeof(std::uint32_t), alignof(std::uint32_t),
		(const std::uint8_t*)&Call::token);
}

template <typename Type>
inline void touchstone::CallStreamBuilder::parameter(const typename  Type::type& value)
{
	append(
		Type::size, Type::alignment,
		(const std::uint8_t*)&value);
}

inline void touchstone::CallStreamBuilder::data(std::size_t num_bytes, const void* value)
{
	if (value == nullptr)
	{
		return;
	}

	assert(num_bytes <= std::numeric_limits<std::uint32_t>::max());
	std::uint32_t length = (std::uint32_t)num_bytes;
	append(
		sizeof(std::uint32_t), alignof(std::uint32_t),
		(const std::uint8_t*)&length);
	if (num_bytes > 0)
	{
		append(
			num_bytes, alignof(std::uint8_t),
			(const std::uint8_t*)value);
	}
}

template <typename Type>
inline void touchstone::CallStreamBuilder::data(
	const std::vector<std::size_t>& counts, const Type* const* values)
{
	if (values == nullptr)
	{
		return;
	}

	std::uint32_t num_values = counts.size();
	append(
		sizeof(std::uint32_t), alignof(std::uint32_t),
		(const std::uint8_t*)&num_values);

	for (auto count: counts)
	{
		data(count, *values);
		++values;
	}
}

template <typename Type>
inline void touchstone::CallStreamBuilder::result(const Type& result)
{
	detail::CallStreamResultWriter<Type>::write(*this, result);
}

template <typename Type>
inline void touchstone::detail::CallStreamResultWriter<Type>::write(
	CallStreamBuilder& builder, const Type& value)
{
	builder.append(
		sizeof(Type), alignof(Type),
		(const std::uint8_t*)&value);
}

#endif
