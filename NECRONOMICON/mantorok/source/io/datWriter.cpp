// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <limits>
#include "mantorok/io/datWriter.hpp"

mantorok::DATWriter::DATWriter(std::ostream& stream) :
	stream(stream)
{
	begin_section("mantorok.DAT::root", flag_clean | flag_detail_root);
}

mantorok::DATWriter::~DATWriter()
{
	assert(this->sections.empty());
	assert(this->current.flags & flag_detail_root);

	this->sections.push(this->current);
	end_section();
}

void mantorok::DATWriter::begin_section(const std::string& name, int flags)
{
	Section section;
	section.flags = flags;

	std::int32_t qualified_flags = flags;
	this->stream.write((const char*)&qualified_flags, sizeof(std::int32_t));

	section.size_field_offset = this->stream.tellp();
	std::uint32_t qualified_section_size = 0;
	this->stream.write((const char*)&qualified_section_size, sizeof(std::uint32_t));

	assert(name.size() <= std::numeric_limits<std::uint32_t>::max());
	std::uint32_t qualified_name_size = name.size();
	this->stream.write((const char*)&qualified_name_size, sizeof(std::uint32_t));
	this->stream.write(name.data(), name.size());

	section.data_offset = this->stream.tellp();

	if (!(flags & flag_detail_root) && !(flags & flag_detail_end))
	{
		sections.push(this->current);
	}
	else
	{
		assert(!(flags & flag_detail_root) || this->sections.empty());
	}

	this->current = section;
}

void mantorok::DATWriter::end_section()
{
	assert(!(this->current.flags & flag_detail_field) || !this->sections.empty());

	if (!(this->current.flags & flag_detail_field))
	{
		std::int32_t qualified_flags = flag_detail_end;
		this->stream.write((const char*)&qualified_flags, sizeof(std::int32_t));
	}

	std::size_t end_offset = this->stream.tellp();
	std::size_t size = end_offset - this->current.data_offset;

	assert(size <= std::numeric_limits<std::uint32_t>::max());

	this->stream.seekp(current.size_field_offset);

	std::uint32_t qualified_section_size = size;
	this->stream.write((const char*)&qualified_section_size, sizeof(std::uint32_t));

	this->stream.seekp(end_offset);
	this->current = this->sections.top();
	this->sections.pop();
}

void mantorok::DATWriter::write(
	const std::uint8_t* data, std::size_t size, const std::string& comment)
{
	begin_section(comment, flag_detail_field);

	this->stream.write(
		(const char*)&mantorok::detail::FieldToken<std::uint8_t*>::value,
		sizeof(std::int32_t));
	detail::FieldWriterReader<std::size_t>::write(size, this->stream);
	this->stream.write((const char*)data, size);

	end_section();
}
