// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/io/world/query/tileSpacePointCloudMatch.hpp"
#include "chomsky/io/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"

void mantorok::DATSerializer<chomsky::TileSpacePointCloudMatch>::serialize(
	const chomsky::TileSpacePointCloudMatch& match,
	DATWriter& writer)
{
	writer.begin_section("chomsky.TileSpacePointCloudMatch::root", DATWriter::flag_clean);
	for (auto& i: match)
	{
		writer.begin_section("chomsky.TileSpacePointCloudMatch::tile", DATWriter::flag_clean);
		writer.write<std::int32_t>(i.first.x, "x");
		writer.write<std::int32_t>(i.first.layer, "y");
		writer.write<std::int32_t>(i.first.y, "z");
		DATSerializer<chomsky::TileSpacePointCloud>::serialize(i.second, writer);
		writer.end_section();

	}
	writer.end_section();
}

void mantorok::DATDeserializer<chomsky::TileSpacePointCloudMatch>::deserialize(
	chomsky::TileSpacePointCloudMatch& match,
	DATReader& reader)
{
	if (reader.match_section("chomsky.TileSpacePointCloudMatch::root"))
	{
		match = chomsky::TileSpacePointCloudMatch();

		DATReader::Section section;
		while (reader.try_peek_section(section) &&
			section.name == "chomsky.TileSpacePointCloudMatch::tile")
		{
			reader.enter_section();

			thoreau::TileCoordinate offset;
			reader.read<std::int32_t>(offset.x);
			reader.read<std::int32_t>(offset.layer);
			reader.read<std::int32_t>(offset.y);

			chomsky::TileSpacePointCloud point_cloud;
			DATDeserializer<chomsky::TileSpacePointCloud>::deserialize(
				point_cloud, reader);

			match.set(offset, point_cloud);
			reader.leave_section();
		}

		reader.leave_section();
	}
}
