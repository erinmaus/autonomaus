// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_WORLD_QUERY_TILE_SPACE_POINT_CLOUD_MATCH_HPP
#define CHOMSKY_IO_WORLD_QUERY_TILE_SPACE_POINT_CLOUD_MATCH_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class TileSpacePointCloudMatch;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::TileSpacePointCloudMatch>
	{
		static void serialize(
			const chomsky::TileSpacePointCloudMatch& match,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::TileSpacePointCloudMatch>
	{
		static void deserialize(
			chomsky::TileSpacePointCloudMatch& match,
			DATReader& reader);
	};
}

#endif
