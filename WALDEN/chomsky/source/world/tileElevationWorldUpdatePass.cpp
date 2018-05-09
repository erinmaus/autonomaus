// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <set>
#include "chomsky/world/tileElevationWorldUpdatePass.hpp"
#include "thoreau/thoreau.hpp"

chomsky::TileElevationWorldUpdatePass::TileElevationWorldUpdatePass(
	TileElevationState* state)
{
	this->state = state;
}

void chomsky::TileElevationWorldUpdatePass::start()
{
	this->state->clear();
}

void chomsky::TileElevationWorldUpdatePass::stop()
{
	if (this->is_dirty)
	{
		collect_layers();
		this->is_dirty = false;
	}
}

void chomsky::TileElevationWorldUpdatePass::invalidate_tag(int tag)
{
	if (this->tags.find(tag) != this->tags.end())
	{
		for (auto i = this->working_state.begin(); i != this->working_state.end(); /* Nop. */)
		{
			i->second.remove_tag(tag);
			if (i->second.count() == 0)
			{
				i = this->working_state.remove(i);
			}
			else
			{
				++i;
			}
		}

		this->tags.erase(tag);
		this->sectors.erase(tag);
	}
}

chomsky::TileElevationWorldUpdatePass::TileElevationState*
chomsky::TileElevationWorldUpdatePass::get_output_state()
{
	return this->state;
}

chomsky::TileElevationWorldUpdatePass::TileElevationState&
chomsky::TileElevationWorldUpdatePass::get_working_state()
{
	return this->working_state;
}

bool chomsky::TileElevationWorldUpdatePass::has_tag(int tag) const
{
	return this->tags.count(tag) != 0;
}

void chomsky::TileElevationWorldUpdatePass::add_sector(
	int tag, const thoreau::SectorCoordinate& sector)
{
	if (this->tags.count(tag) == 0)
	{
		this->is_dirty = true;
	}

	this->tags.insert(tag);
	this->sectors[tag] = sector;
}

void chomsky::TileElevationWorldUpdatePass::remove_sector(
	int tag, const thoreau::SectorCoordinate& sector)
{
	this->tags.erase(tag);
	this->sectors.erase(tag);
}

void chomsky::TileElevationWorldUpdatePass::collect_layers()
{
	std::map<int, int> tag_hits;
	for (auto& tile: this->working_state)
	{
		std::multimap<int, int> layers;
		for (auto extent: tile.second)
		{
			float y = std::get<TileElevationExtents::MIN_Y>(extent);
			int tag = std::get<TileElevationExtents::TAG>(extent);
			layers.emplace(y, tag);
		}

		int i = 1;
		int previous_y = layers.begin()->first;
		for (auto& layer: layers)
		{
			if (layer.first != previous_y)
			{
				++i;
			}

			auto iter = tag_hits.find(layer.second);
			if (iter == tag_hits.end())
			{
				tag_hits[layer.second] = i;
			}
			else
			{
				iter->second = std::max(iter->second, i);
			}

			previous_y = layer.first;
		}
	}

	std::map<int /* tag */, std::set<int>> terrain_neighbors;
	for (auto i: this->tags)
	{
		for (auto j: this->tags)
		{
			if (i == j)
			{
				continue;
			}

			auto sector_i = this->sectors[i];
			auto sector_j = this->sectors[j];
			int offset_x = sector_i.x - sector_j.x;
			int offset_y = sector_i.y - sector_j.y;

			if (std::abs(offset_x) > 1 || std::abs(offset_y) > 1 ||
				std::abs(offset_x) == std::abs(offset_y))
			{
				continue;
			}

			if (offset_x != 0)
			{
				int i_x, j_x;
				if (offset_x < 0)
				{
					i_x = 0;
					j_x = thoreau::SECTOR_SIZE - 1;
				}
				else
				{
					i_x = thoreau::SECTOR_SIZE - 1;
					j_x = 0;
				}

				for (int y = 0; y < thoreau::SECTOR_SIZE; ++y)
				{
					auto i_coordinate = thoreau::tile_from_sector(sector_i);
					i_coordinate.x += i_x;
					i_coordinate.y += y;
					auto j_coordinate = thoreau::tile_from_sector(sector_j);
					j_coordinate.x += j_x;
					j_coordinate.y += y;

					if (!this->working_state.has(i_coordinate) ||
						!this->working_state.has(j_coordinate))
					{
						continue;
					}

					auto& i_extents = this->working_state.get(i_coordinate);
					auto& j_extents = this->working_state.get(j_coordinate);
					if (i_extents.has(i) && j_extents.has(j))
					{
						float min_i = i_extents.get_min(i);
						float max_i = i_extents.get_max(i);
						float min_j = j_extents.get_min(j);
						float max_j = j_extents.get_max(j);

						if (min_i == min_j || min_i == max_j ||
							max_i == min_j || max_i == min_j)
						{
							terrain_neighbors[i].insert(j);
							break;
						}
					}
				}
			}
			else if (offset_y != 0)
			{
				int i_y, j_y;
				if (offset_y < 0)
				{
					i_y = thoreau::SECTOR_SIZE - 1;
					j_y = 0;
				}
				else
				{
					i_y = 0;
					j_y = thoreau::SECTOR_SIZE - 1;
				}

				for (int x = 0; x < thoreau::SECTOR_SIZE; ++x)
				{
					auto i_coordinate = thoreau::tile_from_sector(sector_i);
					i_coordinate.x += x;
					i_coordinate.y += i_y;
					auto j_coordinate = thoreau::tile_from_sector(sector_j);
					j_coordinate.x += x;
					j_coordinate.y += j_y;

					if (!this->working_state.has(i_coordinate) ||
						!this->working_state.has(j_coordinate))
					{
						continue;
					}

					auto& i_extents = this->working_state.get(i_coordinate);
					auto& j_extents = this->working_state.get(j_coordinate);
					if (i_extents.has(i) && j_extents.has(j))
					{
						float min_i = i_extents.get_min(i);
						float max_i = i_extents.get_max(i);
						float min_j = j_extents.get_min(j);
						float max_j = j_extents.get_max(j);

						if (min_i == min_j || min_i == max_j ||
							max_i == min_j || max_i == min_j)
						{
							terrain_neighbors[i].insert(j);
							break;
						}
					}
				}
			}
		}
	}

	for (auto& i: terrain_neighbors)
	{
		auto current_tag = i.first;
		std::set<int> closed_set;
		std::set<int> open_set = i.second;

		while (!open_set.empty())
		{
			int top;
			{
				auto iter = open_set.begin();
				top = *iter;
				open_set.erase(top);
			}

			if (closed_set.count(top) == 0 && top != current_tag)
			{
				if (terrain_neighbors.count(top) != 0)
				{
					for (auto j: terrain_neighbors[top])
					{
						open_set.insert(j);
					}
				}

				closed_set.insert(top);
			}
		}

		for (auto dependency: closed_set)
		{
			tag_hits[current_tag] = std::max(tag_hits[dependency], tag_hits[current_tag]);
		}
	}

	for (auto& tile: this->working_state)
	{
		for (auto extent: tile.second)
		{
			int tag = std::get<TileElevationExtents::TAG>(extent);
			float min_y = std::get<TileElevationExtents::MIN_Y>(extent);
			float max_y = std::get<TileElevationExtents::MAX_Y>(extent);

			assert(tag_hits.count(tag) != 0);
			int layer = tag_hits[tag] - 1;

			auto& e = this->state->add(tile.first);
			e.test(layer, min_y);
			e.test(layer, max_y);
		}
	}
}
