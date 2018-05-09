// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstring>
#include "chomsky/io/resources/silhouette.hpp"
#include "chomsky/resources/silhouette.hpp"

void mantorok::DATSerializer<chomsky::Silhouette>::serialize(
	const chomsky::Silhouette& silhouette,
	DATWriter& writer)
{
	writer.begin_section("chomsky.Silhouette::root", DATWriter::flag_clean);
	writer.write(silhouette.get_width(), "width");
	writer.write(silhouette.get_height(), "height");
	writer.write(silhouette.get_depth(), "depth");
	auto num_fragments = silhouette.width * silhouette.height * silhouette.depth;
	writer.write(
		(const std::uint8_t*)silhouette.fragments.get(),
		sizeof(chomsky::Silhouette::Fragment) * num_fragments,
		"fragments");
	writer.end_section();
}


void mantorok::DATDeserializer<chomsky::Silhouette>::deserialize(
	chomsky::Silhouette& silhouette,
	DATReader& reader)
{
	if (reader.match_section("chomsky.Silhouette::root"))
	{
		int width, height, depth;
		reader.read(width);
		reader.read(height);
		reader.read(depth);

		assert(width >= 0);
		assert(height >= 0);
		assert(depth >= 0);

		silhouette.width = width;
		silhouette.height = height;
		silhouette.depth = depth;

		auto num_fragments = width * height * depth;
		std::unique_ptr<std::uint8_t[]> fragments;
		std::size_t size;
		reader.read(fragments, size);
		assert(size == num_fragments * sizeof(chomsky::Silhouette::Fragment));

		silhouette.fragments = std::make_unique<chomsky::Silhouette::Fragment[]>(num_fragments);
		std::memcpy(
			silhouette.fragments.get(),
			fragments.get(),
			num_fragments * sizeof(chomsky::Silhouette::Fragment));

		reader.leave_section();
	}
}
