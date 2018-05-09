// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DAT_WRITER_HPP
#define MANTOROK_IO_DAT_WRITER_HPP

#include <stack>
#include <string>
#include <iostream>
#include "mantorok/io/fields.hpp"

namespace mantorok
{
	class DATWriter
	{
	public:
		explicit DATWriter(std::ostream& stream);
		~DATWriter();

		enum
		{
			flag_none = 0x0,
			flag_clean = flag_none,
			flag_c_violation = 0x1,
			flag_detail_root = 0x2,
			flag_detail_field = 0x4,
			flag_detail_end = 0xf0000000
		};

		void begin_section(
			const std::string& name,
			int flags = flag_c_violation);
		void end_section();

		template <typename Type>
		void write(
			const Type& data,
			const std::string& comment = "");

		void write(
			const std::uint8_t* data,
			std::size_t size,
			const std::string& comment = "");

	private:
		std::ostream& stream;

		struct Section
		{
			std::uint32_t size_field_offset = 0;
			std::uint32_t next_field_offset = 0;
			std::uint32_t data_offset = 0;
			std::uint32_t next = 0;
			int flags;
		};
		std::stack<Section> sections;
		Section current;
	};
}

template <typename Type>
void mantorok::DATWriter::write(
	const Type& data,
	const std::string& comment)
{
	begin_section(comment, flag_detail_field);

	this->stream.write(
		(const char*)&mantorok::detail::FieldToken<Type>::value,
		sizeof(std::int32_t));
	mantorok::detail::FieldWriterReader<Type>::write(data, this->stream);

	end_section();
}

#endif
