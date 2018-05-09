// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_RESOURCES_TILE_SPACE_POINT_CLOUD_HPP
#define CHOMSKY_IO_RESOURCES_TILE_SPACE_POINT_CLOUD_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class TileSpacePointCloud;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::TileSpacePointCloud>
	{
		static void serialize(
			const chomsky::TileSpacePointCloud& point_cloud,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::TileSpacePointCloud>
	{
		static void deserialize(
			chomsky::TileSpacePointCloud& point_cloud,
			DATReader& reader);
	};
}

#endif
