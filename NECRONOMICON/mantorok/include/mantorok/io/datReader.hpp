// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DAT_READER_HPP
#define MANTOROK_IO_DAT_READER_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <stack>
#include <iostream>
#include "mantorok/io/fields.hpp"

namespace mantorok
{
	class DATReader
	{
	public:
		struct Section
		{
			std::size_t size;
			std::string name;
			bool clean;
		};

		struct Field
		{
			std::size_t size;
			std::string comment;
			int token;
		};

		enum
		{
			type_none = 0,
			type_section,
			type_field,
			type_end,
			type_max
		};

		explicit DATReader(std::istream& stream);
		~DATReader() = default;

		int peek_type();
		bool try_peek_section(Section& section);
		bool try_peek_field(Field& field);

		bool match_section(const std::string& name);
		bool match_section(const std::string& name, Section& section);

		bool enter_section();
		bool leave_section();

		bool next();

		template <typename Type>
		bool read(Type& data);

		bool read(std::unique_ptr<std::uint8_t[]>& data, std::size_t& data_size);

	private:
		bool read();

		std::istream& stream;

		struct SectionHeader
		{
			int flags = 0;
			std::uint32_t header_offset = 0;
			std::uint32_t data_offset = 0;
			std::uint32_t size = 0;
			std::string name;
			int token = 0;
		};
		std::stack<SectionHeader> sections;
		SectionHeader current;
	};
}

template <typename Type>
bool mantorok::DATReader::read(Type& data)
{
	if (peek_type() != type_field ||
		this->current.token != detail::FieldToken<Type>::value)
	{
		return false;
	}

	detail::FieldWriterReader<Type>::read(data, this->stream);
	next();

	return static_cast<bool>(this->stream);
}

#endif
