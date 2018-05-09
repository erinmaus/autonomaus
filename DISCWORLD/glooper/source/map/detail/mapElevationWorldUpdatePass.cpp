// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/map/detail/mapElevationWorldUpdatePass.hpp"

glooper::MapElevationWorldUpdatePass::MapElevationWorldUpdatePass(
	const TileElevationState* terrain_tile_elevation_state,
	const TileElevationState* water_tile_elevation_state,
	const thoreau::Map* map,
	TileElevationState* tile_elevation_state,
	RuntimeSectorFlagState* runtime_sector_flag_state,
	thoreau::Map* runtime_map)
{
	this->terrain_tile_elevation_state = terrain_tile_elevation_state;
	this->water_tile_elevation_state = water_tile_elevation_state;
	this->map = map;
	this->tile_elevation_state = tile_elevation_state;
	this->runtime_sector_flag_state = runtime_sector_flag_state;
	this->runtime_map = runtime_map;
}

void glooper::MapElevationWorldUpdatePass::stop()
{
	if (!this->is_dirty)
	{
		return;
	}

	//this->runtime_map->clear();
	this->tile_elevation_state->clear();
	for (auto& i: *this->terrain_tile_elevation_state)
	{
		if (this->map->has_tile(i.first.x, i.first.y))
		{
			auto& extents = this->tile_elevation_state->get(i.first);
			for (int j = 0; j < this->map->get_num_layers(i.first); ++j)
			{
				if (extents.has(j))
				{
					continue;
				}

				auto& tile = this->map->get_tile(i.first.x, i.first.y, j);
				if (tile.has_elevation())
				{
					float min_elevation_y = tile.get_min_elevation();
					float max_elevation_y = tile.get_max_elevation();
					extents.test(j, min_elevation_y);
					extents.test(j, max_elevation_y);

					this->runtime_map->add_tile(i.first.x, i.first.y, j) = tile;
				}
			}

			continue;
		}

		for (auto extent: i.second)
		{
			float min_y = std::get<chomsky::TileElevationExtents::MIN_Y>(extent);
			float max_y = std::get<chomsky::TileElevationExtents::MAX_Y>(extent);
			auto tile_coordinate = i.first;
			tile_coordinate.layer = std::get<chomsky::TileElevationExtents::TAG>(extent);

			float min_elevation_y = -HUGE_VALF, max_elevation_y = HUGE_VALF;
			if (this->map->has_tile(tile_coordinate))
			{
				auto& tile = this->map->get_tile(tile_coordinate);
				if (tile.has_elevation())
				{
					min_elevation_y = tile.get_min_elevation();
					max_elevation_y = tile.get_max_elevation();
				}

				this->runtime_map->add_tile(tile_coordinate) = tile;
			}

			if (min_elevation_y == -HUGE_VALF || max_elevation_y == HUGE_VALF)
			{
				auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
				this->runtime_sector_flag_state->add(sector_coordinate) = true;

				auto& tile = this->runtime_map->add_tile(tile_coordinate);
				tile.set_elevation(min_y, max_y);

				min_elevation_y = min_y;
				max_elevation_y = max_y;

				if (this->water_tile_elevation_state->has(i.first))
				{
					auto& water_extent = this->water_tile_elevation_state->get(i.first);
					float water_max_elevation_y = water_extent.get_max(tile_coordinate.layer);
					if (water_max_elevation_y != HUGE_VALF && water_max_elevation_y >= max_elevation_y)
					{
						tile.append_flags(thoreau::Tile::flag_water);
						max_elevation_y = water_max_elevation_y;
						tile.set_elevation(min_elevation_y, water_max_elevation_y);
					}
				}
			}

			auto terrain_tile_coordinate = thoreau::TileCoordinate(tile_coordinate.x, tile_coordinate.y);
			auto& result = this->tile_elevation_state->add(terrain_tile_coordinate);
			result.test(tile_coordinate.layer, min_elevation_y);
			result.test(tile_coordinate.layer, max_elevation_y);
		}
	}

	this->is_dirty = false;
}

void glooper::MapElevationWorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		return;
	}

	this->tags.insert(tag);
	this->is_dirty = true;
}

void glooper::MapElevationWorldUpdatePass::invalidate_tag(int tag)
{
	this->tags.erase(tag);
}
