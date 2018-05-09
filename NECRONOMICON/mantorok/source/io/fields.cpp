// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <limits>
#include "mantorok/io/fields.hpp"

const std::int32_t mantorok::detail::FieldToken<std::uint8_t>::value;
const std::int32_t mantorok::detail::FieldToken<std::int8_t>::value;
const std::int32_t mantorok::detail::FieldToken<std::uint16_t>::value;
const std::int32_t mantorok::detail::FieldToken<std::int16_t>::value;
const std::int32_t mantorok::detail::FieldToken<std::uint32_t>::value;
const std::int32_t mantorok::detail::FieldToken<std::int32_t>::value;
const std::int32_t mantorok::detail::FieldToken<float>::value;
const std::int32_t mantorok::detail::FieldToken<double>::value;
const std::int32_t mantorok::detail::FieldToken<std::size_t>::value;
const std::int32_t mantorok::detail::FieldToken<std::string>::value;
const std::int32_t mantorok::detail::FieldToken<std::uint8_t*>::value;

void mantorok::detail::FieldWriterReader<std::size_t>::write(
	std::size_t value, std::ostream& stream)
{
	assert(value <= std::numeric_limits<std::uint32_t>::max());
	std::uint32_t qualified_value = value;
	stream.write((const char*)&qualified_value, sizeof(std::uint32_t));
}

void mantorok::detail::FieldWriterReader<std::size_t>::read(
	std::size_t& value, std::istream& stream)
{
	std::uint32_t result;
	stream.read((char*)&result, sizeof(std::uint32_t));

	value = result;
}

void mantorok::detail::FieldWriterReader<std::string>::write(
	const std::string& value, std::ostream& stream)
{
	FieldWriterReader<std::size_t>::write(value.size(), stream);
	stream.write(value.data(), value.size());
}

void mantorok::detail::FieldWriterReader<std::string>::read(
	std::string& value, std::istream& stream)
{
	std::size_t length;
	FieldWriterReader<std::size_t>::read(length, stream);

	value.resize(length);
	stream.read(&value[0], length);
}
