// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/serializers/vertexIndexMapperSerializer.hpp"

void mantorok::DATSerializer<kompvter::VertexIndexMapper>::serialize(
	const kompvter::VertexIndexMapper& index_mapper, DATWriter& writer)
{
	writer.begin_section("kompvter.VertexIndexMapper::root");

	writer.write<std::int32_t>(index_mapper.get_format(), "format");

	auto& buffer = index_mapper.get_buffer();
	writer.write(buffer.get_data(), buffer.get_data_size(), "data");

	writer.end_section();
}
