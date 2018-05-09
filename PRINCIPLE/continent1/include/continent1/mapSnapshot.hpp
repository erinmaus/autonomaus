// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT1_MAP_SNAPSHOT_HPP
#define CONTINENT1_MAP_SNAPSHOT_HPP

#include <functional>
#include <map>
#include <boost/container/flat_map.hpp>
#include "continent1/tileSnapshot.hpp"
#include "principle/scene/scene.hpp"
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace continent1
{
	class MapSnapshot
	{
	public:
		friend class mantorok::DATSerializer<MapSnapshot>;
		friend class mantorok::DATDeserializer<MapSnapshot>;

		MapSnapshot() = default;
		~MapSnapshot() = default;

		TileSnapshot& add(const thoreau::TileCoordinate& tile_coordinate);
		bool has(const thoreau::TileCoordinate& tile_coordinate) const;
		TileSnapshot& get(const thoreau::TileCoordinate& tile_coordinate);
		const TileSnapshot& get(const thoreau::TileCoordinate& tile_coordinate) const;

		std::size_t count() const;
		bool empty() const;

		void clear();

		void merge(const MapSnapshot& other);
		void merge_sector(
			const MapSnapshot& other,
			const thoreau::SectorCoordinate& sector);

	private:
		typedef std::map<thoreau::TileCoordinate, TileSnapshot> Tiles;
		Tiles tiles;

		typedef boost::container::flat_multimap<
				thoreau::SectorCoordinate,
				thoreau::TileCoordinate>
			Sectors;
		Sectors tiles_by_sector;

	public:
		typedef Tiles::const_iterator const_iterator;
		typedef Sectors::const_iterator const_sector_iterator;

		const_iterator begin() const;
		const_iterator end() const;

		const_sector_iterator sectors_begin(
			const thoreau::SectorCoordinate& sector_coordinate) const;
		const_sector_iterator sectors_end(
			const thoreau::SectorCoordinate& sector_coordinate) const;
	};
}

namespace mantorok
{
	template <>
	struct DATSerializer<continent1::MapSnapshot>
	{
		static void serialize(
			const continent1::MapSnapshot& map,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<continent1::MapSnapshot>
	{
		static void deserialize(
			continent1::MapSnapshot& map,
			DATReader& reader);
	};
}

#endif
