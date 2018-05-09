// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/deserializers/vertexFetcherDeserializer.hpp"

void mantorok::DATDeserializer<kompvter::VertexFetcher>::deserialize(
	kompvter::VertexFetcher& fetcher, DATReader& reader)
{
	if (!reader.match_section("kompvter.VertexFetcher::root"))
	{
		return;
	}

	std::int32_t num_components;
	std::int32_t format;
	std::int32_t normalized;
	std::uint32_t stride;
	std::uint32_t offset;
	std::unique_ptr<std::uint8_t[]> buffer_data;
	std::size_t buffer_data_size;

	reader.read<std::int32_t>(num_components);
	reader.read<std::int32_t>(format);
	reader.read<std::int32_t>(normalized);
	reader.read<std::uint32_t>(stride);
	reader.read<std::uint32_t>(offset);
	reader.read(buffer_data, buffer_data_size);

	kompvter::Buffer buffer;
	buffer.allocate(buffer_data_size);
	buffer.update(0, buffer_data.get(), buffer_data_size);
	fetcher = kompvter::VertexFetcher(
		buffer,
		num_components,
		format, normalized,
		stride, offset);

	reader.leave_section();
}
