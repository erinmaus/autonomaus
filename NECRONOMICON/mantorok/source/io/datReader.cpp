// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/datWriter.hpp"
#include "mantorok/io/datReader.hpp"
#include "mantorok/io/fields.hpp"

mantorok::DATReader::DATReader(std::istream& stream) :
	stream(stream)
{
	read();
	enter_section();
}

int mantorok::DATReader::peek_type()
{
	if (!this->stream || current.flags & DATWriter::flag_detail_end)
	{
		return type_end;
	}
	else if (current.flags & DATWriter::flag_detail_field)
	{
		return type_field;
	}
	else
	{
		return type_section;
	}
}

bool mantorok::DATReader::try_peek_section(Section& section)
{
	if (peek_type() != type_section)
	{
		return false;
	}

	section.name = this->current.name;
	section.size = this->current.size;
	section.clean = !(this->current.flags & DATWriter::flag_c_violation);

	return true;
}

bool mantorok::DATReader::try_peek_field(Field& field)
{
	if (peek_type() != type_field)
	{
		return false;
	}

	field.comment = this->current.name;
	field.size = this->current.size;
	field.token = this->current.token;

	return true;
}

bool mantorok::DATReader::match_section(const std::string& name)
{
	Section result;
	return match_section(name, result);
}

bool mantorok::DATReader::match_section(const std::string& name, Section& section)
{
	if (!try_peek_section(section) || section.name != name)
	{
		return false;
	}

	return enter_section();
}

bool mantorok::DATReader::next()
{
	if (peek_type() != type_end)
	{
		this->stream.seekg(this->current.data_offset + this->current.size);
		read();
	}

	return peek_type() != type_end;
}

bool mantorok::DATReader::enter_section()
{
	if (peek_type() != type_section)
	{
		return false;
	}

	this->sections.push(this->current);
	this->stream.seekg(this->current.data_offset);

	return read();
}

bool mantorok::DATReader::leave_section()
{
	if (this->sections.empty())
	{
		return false;
	}

	this->stream.seekg(this->current.data_offset + this->current.size);

	this->current = sections.top();
	sections.pop();

	return read();
}

bool mantorok::DATReader::read(
	std::unique_ptr<std::uint8_t[]>& data,
	std::size_t& data_size)
{
	if (peek_type() != type_field ||
		this->current.token != detail::FieldToken<std::uint8_t*>::value)
	{
		return false;
	}

	detail::FieldWriterReader<std::size_t>::read(data_size, this->stream);
	if (!this->stream)
	{
		return false;
	}

	data = std::make_unique<std::uint8_t[]>(data_size);
	this->stream.read((char*)data.get(), data_size);
	if (!this->stream)
	{
		return false;
	}

	next();

	return true;
}

bool mantorok::DATReader::read()
{
	SectionHeader header;
	header.header_offset = this->stream.tellg();

	std::int32_t qualified_flags;
	this->stream.read((char*)&qualified_flags, sizeof(std::int32_t));
	header.flags = qualified_flags;

	if (!(qualified_flags & DATWriter::flag_detail_end))
	{
		std::uint32_t qualified_size;
		this->stream.read((char*)&qualified_size, sizeof(std::uint32_t));
		header.size = qualified_size;

		std::uint32_t qualified_name_size;
		this->stream.read((char*)&qualified_name_size, sizeof(std::uint32_t));
		if (!this->stream)
		{
			return false;
		}

		header.name.resize(qualified_name_size);
		this->stream.read(&header.name[0], qualified_name_size);
		if (!this->stream)
		{
			return false;
		}

		header.data_offset = this->stream.tellg();

		if (header.flags & DATWriter::flag_detail_field)
		{
			std::int32_t qualified_token;
			this->stream.read((char*)&qualified_token, sizeof(std::int32_t));
			if (!this->stream)
			{
				return false;
			}

			header.token = qualified_token;
		}
	}
	else
	{
		header.data_offset = this->stream.tellg();
	}

	this->current = header;
	return true;
}
