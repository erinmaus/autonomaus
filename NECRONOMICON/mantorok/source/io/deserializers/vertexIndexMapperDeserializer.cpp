// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/deserializers/vertexIndexMapperDeserializer.hpp"

void mantorok::DATDeserializer<kompvter::VertexIndexMapper>::deserialize(
	kompvter::VertexIndexMapper& index_mapper, DATReader& reader)
{
	if (!reader.match_section("kompvter.VertexIndexMapper::root"))
	{
		return;
	}

	std::int32_t format;
	std::unique_ptr<std::uint8_t[]> buffer_data;
	std::size_t buffer_data_size;

	reader.read<std::int32_t>(format);
	reader.read(buffer_data, buffer_data_size);

	kompvter::Buffer buffer;
	buffer.allocate(buffer_data_size);
	buffer.update(0, buffer_data.get(), buffer_data_size);
	index_mapper = kompvter::VertexIndexMapper(buffer, format);

	reader.leave_section();
}
