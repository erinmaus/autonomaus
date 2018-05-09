// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/serializers/vertexFetcherSerializer.hpp"

void mantorok::DATSerializer<kompvter::VertexFetcher>::serialize(
	const kompvter::VertexFetcher& fetcher, DATWriter& writer)
{
	writer.begin_section("kompvter.VertexFetcher::root");

	writer.write<std::int32_t>(fetcher.get_num_components(), "num_components");
	writer.write<std::int32_t>(fetcher.get_format(), "format");
	writer.write<std::int32_t>(fetcher.get_normalized(), "normalized");
	writer.write<std::uint32_t>(fetcher.get_stride(), "stride");
	writer.write<std::uint32_t>(fetcher.get_offset(), "offset");

	auto& buffer = fetcher.get_buffer();
	writer.write(buffer.get_data(), buffer.get_data_size(), "data");

	writer.end_section();
}
