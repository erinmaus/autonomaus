// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/io/gui/itemIconSilhouette.hpp"
#include "chomsky/io/world/query/tileSpacePointCloudMatch.hpp"
#include "chomsky/io/resources/silhouette.hpp"

void mantorok::DATSerializer<chomsky::ItemIconSilhouette>::serialize(
	const chomsky::ItemIconSilhouette& silhouette,
	DATWriter& writer)
{
	writer.begin_section("chomsky.ItemIconSilhouette::root", DATWriter::flag_clean);
	DATSerializer<chomsky::Silhouette>::serialize(silhouette.get_weight(), writer);
	DATSerializer<chomsky::Silhouette>::serialize(silhouette.get_red(), writer);
	DATSerializer<chomsky::Silhouette>::serialize(silhouette.get_green(), writer);
	DATSerializer<chomsky::Silhouette>::serialize(silhouette.get_blue(), writer);
	DATSerializer<chomsky::TileSpacePointCloudMatch>::serialize(silhouette.get_point_cloud_match(), writer);
	writer.end_section();
}

void mantorok::DATDeserializer<chomsky::ItemIconSilhouette>::deserialize(
	chomsky::ItemIconSilhouette& silhouette,
	DATReader& reader)
{
	if (reader.match_section("chomsky.ItemIconSilhouette::root"))
	{
		DATDeserializer<chomsky::Silhouette>::deserialize(silhouette.weight, reader);
		DATDeserializer<chomsky::Silhouette>::deserialize(silhouette.red, reader);
		DATDeserializer<chomsky::Silhouette>::deserialize(silhouette.green, reader);
		DATDeserializer<chomsky::Silhouette>::deserialize(silhouette.blue, reader);
		DATDeserializer<chomsky::TileSpacePointCloudMatch>::deserialize(silhouette.point_cloud_match, reader);
		reader.leave_section();
	}
}
