// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <vector>
#include "chomsky/io/resources/tileSpacePointCloud.hpp"
#include "chomsky/resources/tileSpacePointCloud.hpp"

void mantorok::DATSerializer<chomsky::TileSpacePointCloud>::serialize(
	const chomsky::TileSpacePointCloud& point_cloud,
	DATWriter& writer)
{
	writer.begin_section("chomsky.TileSpacePointCloud::root", DATWriter::flag_clean);

	std::vector<chomsky::TileSpacePointCloud::PackedPoint> points;
	points.reserve(point_cloud.count());
	for (auto& point: point_cloud.points)
	{
		points.push_back(point);
	}

	writer.write(
		(const std::uint8_t*)&points[0],
		sizeof(chomsky::TileSpacePointCloud::PackedPoint) * points.size(),
		"points");

	writer.end_section();
}


void mantorok::DATDeserializer<chomsky::TileSpacePointCloud>::deserialize(
	chomsky::TileSpacePointCloud& point_cloud,
	DATReader& reader)
{
	if (reader.match_section("chomsky.TileSpacePointCloud::root"))
	{
		std::unique_ptr<std::uint8_t[]> points_data;
		std::size_t points_data_size;

		reader.read(points_data, points_data_size);
		reader.leave_section();

		if (points_data_size % sizeof(chomsky::TileSpacePointCloud::PackedPoint))
		{
			return;
		}

		point_cloud = chomsky::TileSpacePointCloud();
		auto points = (const chomsky::TileSpacePointCloud::PackedPoint*)points_data.get();
		auto num_points = points_data_size / sizeof(chomsky::TileSpacePointCloud::PackedPoint);
		point_cloud.points.reserve(num_points);
		for (std::size_t i = 0; i < num_points; ++i)
		{
			point_cloud.points.insert(points[i]);
		}
	}
}
