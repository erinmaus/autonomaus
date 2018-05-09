// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <string>
#include "thoreau/map.hpp"

bool thoreau::Map::has_tile(int x, int y) const
{
	return this->tiles.count(TileCoordinate(x, y)) != 0;
}

bool thoreau::Map::has_tile(int x, int y, int layer) const
{
	return has_tile(TileCoordinate(x, y, layer));
}

bool thoreau::Map::has_tile(const TileCoordinate& coordinate) const
{
	if (coordinate.layer < 0)
	{
		return false;
	}

	auto iter = this->tiles.find(TileCoordinate(coordinate.x, coordinate.y));
	if (iter == this->tiles.end())
	{
		return false;
	}

	auto offset = layer_to_offset(coordinate.layer);
	return offset < iter->second.size();
}

thoreau::Tile& thoreau::Map::insert_tile(int x, int y, int layer)
{
	return insert_tile(TileCoordinate(x, y, layer));
}

thoreau::Tile& thoreau::Map::insert_tile(const TileCoordinate& coordinate)
{
	auto& layer = add_layer(coordinate.x, coordinate.y);
	auto offset = layer_to_offset(coordinate.layer);

	if (offset >= layer.size())
	{
		layer.resize(offset + 1);
		layer[offset] = Tile();
	}
	else
	{
		layer.insert(layer.begin() + offset, Tile());
	}

	for (std::size_t i = 0; i < layer.size(); ++i)
	{
		touch_tile(coordinate.x, coordinate.y, i);
	}

	return layer[offset];
}

thoreau::Tile& thoreau::Map::add_tile(int x, int y, int layer)
{
	return add_tile(TileCoordinate(x, y, layer));
}

thoreau::Tile& thoreau::Map::add_tile(const TileCoordinate& coordinate)
{
	if (has_tile(coordinate))
	{
		return get_tile(coordinate);
	}

	return insert_tile(coordinate);
}

void thoreau::Map::remove_tile(int x, int y)
{
	auto tile_coordinate = TileCoordinate(x, y);
	auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
	auto& sector_tiles = this->tiles_by_sector[sector_coordinate];
	for (auto i = sector_tiles.begin(); i != sector_tiles.end(); /* Nop. */)
	{
		if (i->x == x && i->y == y)
		{
			i = sector_tiles.erase(i);
		}
		else
		{
			++i;
		}
	}

	this->tiles.erase(tile_coordinate);
}

void thoreau::Map::clear()
{
	this->tiles_by_sector.clear();
	this->tiles.clear();
}

void thoreau::Map::merge(const thoreau::Map& other)
{
	for (auto& i: other.tiles)
	{
		auto iter = this->tiles.find(i.first);
		if (iter == this->tiles.end())
		{
			this->tiles.insert(i);
			for (std::size_t j = 0; j < i.second.size(); ++j)
			{
				touch_tile(i.first.x, i.first.y, j);
			}
		}
		else
		{
			for (std::size_t j = 0; j < iter->second.size(); ++j)
			{
				touch_tile(iter->first.x, iter->first.y, j);

				auto& source_tile = i.second[j];
				auto& destination_tile = iter->second[j];
				destination_tile.append_flags(source_tile.get_flags());

				if (source_tile.get_height() != 0.0f)
				{
					destination_tile.set_height(source_tile.get_height());
				}

				if (source_tile.has_elevation())
				{
					destination_tile.set_elevation(
						source_tile.get_min_elevation(),
						source_tile.get_max_elevation());
				}
			}

			if (i.second.size() > iter->second.size())
			{
				for (auto j = iter->second.size(); j < i.second.size(); ++j)
				{
					iter->second.push_back(i.second[j]);
					touch_tile(i.first.x, i.first.y, j);
				}
			}
		}
	}
}

int thoreau::Map::get_num_layers(int x, int y) const
{
	if (has_tile(x, y))
	{
		return get_layer(x, y).size();
	}

	return 0;
}

int thoreau::Map::get_num_layers(const TileCoordinate& coordinate) const
{
	return get_num_layers(coordinate.x, coordinate.y);
}

bool thoreau::Map::try_get_tile_layer(
	int x, int y, float elevation, int& layer) const
{
	if (!has_tile(x, y))
	{
		return false;
	}

	auto& tile = get_layer(x, y);
	bool success = false;
	for (int i = 0; i < tile.size(); ++i)
	{
		if (tile[i].has_elevation() && elevation >= tile[i].get_min_elevation())
		{
			layer = i;
			success = true;
		}
	}

	return success;
}

bool thoreau::Map::try_get_tile_layer(
	const TileCoordinate& coordinate, float elevation, int& layer) const
{
	return try_get_tile_layer(coordinate.x, coordinate.y, elevation, layer);
}

thoreau::Tile& thoreau::Map::get_tile(int x, int y, int layer)
{
	return get_tile(TileCoordinate(x, y, layer));
}

thoreau::Tile& thoreau::Map::get_tile(const TileCoordinate& coordinate)
{
	assert(has_tile(coordinate));
	auto& layer = get_layer(coordinate.x, coordinate.y);
	return layer[layer_to_offset(coordinate.layer)];
}

const thoreau::Tile& thoreau::Map::get_tile(int x, int y, int layer) const
{
	return get_tile(TileCoordinate(x, y, layer));
}

const thoreau::Tile& thoreau::Map::get_tile(const TileCoordinate& coordinate) const
{
	assert(has_tile(coordinate));
	auto& layer = get_layer(coordinate.x, coordinate.y);
	return layer[layer_to_offset(coordinate.layer)];
}

bool thoreau::Map::get_first_tile(TileCoordinate& result) const
{
	auto iter = this->tiles.begin();
	if (iter == this->tiles.end())
	{
		return false;
	}

	result = iter->first;
	return true;
}

bool thoreau::Map::get_next_tile(
	const TileCoordinate& current, TileCoordinate& result) const
{
	auto iter = this->tiles.find(TileCoordinate(current.x, current.y));
	if (iter == this->tiles.end())
	{
		return false;
	}

	auto offset = layer_to_offset(current.layer + 1);
	if (offset < iter->second.size())
	{
		result = current;
		++result.layer;

		return true;
	}

	++iter;
	if (iter == this->tiles.end())
	{
		return false;
	}

	assert(iter->second.size() > 0);
	result = iter->first;
	result.layer = 0;
	return true;
}

void thoreau::Map::touch_tile(int x, int y, int layer)
{
	auto tile_coordinate = TileCoordinate(x, y, layer);
	auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
	sector_coordinate.layer = 0;

	this->tiles_by_sector[sector_coordinate].insert(tile_coordinate);
}

thoreau::Map::LayerCollection& thoreau::Map::add_layer(int x, int y)
{
	auto coordinate = TileCoordinate(x, y);
	return this->tiles[coordinate];
}

thoreau::Map::LayerCollection& thoreau::Map::get_layer(int x, int y)
{
	auto iter = this->tiles.find(TileCoordinate(x, y));
	assert(iter != this->tiles.end());
	return iter->second;
}

const thoreau::Map::LayerCollection& thoreau::Map::get_layer(int x, int y) const
{
	auto iter = this->tiles.find(TileCoordinate(x, y));
	assert(iter != this->tiles.end());
	return iter->second;
}

std::size_t thoreau::Map::layer_to_offset(int layer)
{
	assert(layer >= 0);
	return (std::size_t)layer;
}

bool thoreau::Map::can_move(
	const TileCoordinate& tile_coordinate,
	int offset_x, int offset_y, int offset_layer) const
{
	if (tile_coordinate.layer + offset_layer < TileCoordinate::MIN_LAYER)
	{
		return false;
	}

	auto to = TileCoordinate(
		tile_coordinate.x + offset_x,
		tile_coordinate.y + offset_y,
		tile_coordinate.layer + offset_layer);
	return can_move(tile_coordinate, to);
}

bool thoreau::Map::can_move(
	int from_x, int from_y, int from_layer,
	int to_x, int to_y, int to_layer) const
{
	auto from = thoreau::TileCoordinate(from_x, from_y, from_layer);
	auto to = thoreau::TileCoordinate(to_x, to_y, to_layer);

	return can_move(from, to);
}

bool thoreau::Map::can_move(
	const TileCoordinate& from,
	const TileCoordinate& to) const
{
	if (!has_tile(from) || !has_tile(to))
	{
		return false;
	}

	int offset_x = to.x - from.x;
	int offset_y = to.y - from.y;
	int offset_layer = to.layer - from.layer;

	auto& from_tile = get_tile(from);
	auto& to_tile = get_tile(to);

	if (offset_layer != 0)
	{
		if (offset_x != 0 && offset_y != 0)
		{
			return false;
		}

		if (std::abs(offset_layer) > 1)
		{
			return false;
		}

		int explicit_flag;
		if (offset_layer < 0)
		{
			explicit_flag = Tile::flag_portal_down;
		}
		else if (offset_layer > 0)
		{
			explicit_flag = Tile::flag_portal_up;
		}
		else
		{
			// This should never happen.
			assert(false);
			explicit_flag = Tile::flag_portal;
		}

		static const int EXPLICIT_PORTAL_FLAGS = Tile::flag_portal_up | Tile::flag_portal_down;
		bool is_from_explicit = from_tile.get_flags() & EXPLICIT_PORTAL_FLAGS;
		bool is_to_explicit = to_tile.get_flags() & EXPLICIT_PORTAL_FLAGS;

		if (is_from_explicit)
		{
			if (!(from_tile.get_flags() & explicit_flag))
			{
				return false;
			}
		}
		else if (is_to_explicit || !(from_tile.is_portal() && to_tile.is_portal()))
		{
			return false;
		}
	}

	if (offset_x == 0 && offset_y == 0)
	{
		return true;
	}

	if (std::abs(offset_x) > 1 || std::abs(offset_y) > 1)
	{
		return false;
	}

	if (std::abs(offset_x) == 1 && std::abs(offset_y) == 1)
	{
		bool to_x = can_move(from.x, from.y, from.layer, from.x + offset_x, from.y, from.layer);
		bool to_y = can_move(from.x, from.y, from.layer, from.x, from.y + offset_y, from.layer);
		bool from_x = can_move(from.x + offset_x, from.y, from.layer, from.x + offset_x, from.y + offset_y, from.layer);
		bool from_y = can_move(from.x, from.y + offset_y, from.layer, from.x + offset_x, from.y + offset_y, from.layer);

		return (to_x && from_x) && (to_y && from_y);
	}

	if (to_tile.is_impassable())
	{
		return false;
	}

	int from_flags = from_tile.get_flags();
	int to_flags = to_tile.get_flags();
	if (offset_x < 0 &&
		(from_flags & thoreau::Tile::flag_wall_left ||
		to_flags & thoreau::Tile::flag_wall_right))
	{
		return false;
	}

	if (offset_x > 0 &&
		(from_flags & thoreau::Tile::flag_wall_right ||
		to_flags & thoreau::Tile::flag_wall_left))
	{
		return false;
	}

	if (offset_y > 0 &&
		(from_flags & thoreau::Tile::flag_wall_top ||
		to_flags & thoreau::Tile::flag_wall_bottom))
	{
		return false;
	}

	if (offset_y < 0 &&
		(from_flags & thoreau::Tile::flag_wall_bottom ||
		to_flags & thoreau::Tile::flag_wall_top))
	{
		return false;
	}

	return true;
}

void thoreau::Map::save(std::ostream& stream) const
{
	static const std::string SEP = " ";
	for (auto& i: this->tiles)
	{
		auto coordinate = i.first;

		int layer = 0;
		for (auto& tile: i.second)
		{
			stream << coordinate.x << SEP
				<< coordinate.y << SEP
				<< layer++ << SEP
				<< std::noshowpoint
				<< tile.get_min_elevation() << SEP
				<< tile.get_max_elevation() << SEP
				<< tile.get_height() << SEP
				<< std::showpoint
				<< std::hex << tile.get_flags() << std::dec
				<< std::endl;
		}
	}
}

std::size_t thoreau::Map::load(std::istream& stream)
{
	std::size_t count = 0;
	while (stream)
	{
		int x, y, layer;
		std::string min_elevation, max_elevation;
		std::string height;
		unsigned int flags;
		stream >> x >> y >> layer
			>> min_elevation >> max_elevation >> height
			>> std::hex >> flags >> std::dec;

		if (stream)
		{
			auto& tile = add_tile(x, y, layer);
			tile.set_flags(flags);

			try
			{
				tile.set_elevation(std::stod(min_elevation), std::stod(max_elevation));
				tile.set_height(std::stod(height));
			}
			catch (...)
			{
				stream.setstate(std::ios::failbit);
				return count;
			}

			++count;
		}
	}

	return count;
}

thoreau::Map::const_sector_iterator thoreau::Map::sector_begin(
	const thoreau::SectorCoordinate& sector_coordinate) const
{
	auto sector_coordinate_without_layer = sector_coordinate;
	sector_coordinate_without_layer.layer = 0;

	auto iter = this->tiles_by_sector.find(sector_coordinate_without_layer);
	if (iter == this->tiles_by_sector.end())
	{
		return this->empty_tile_set.begin();
	}

	return iter->second.begin();
}

thoreau::Map::const_sector_iterator thoreau::Map::sector_end(
	const thoreau::SectorCoordinate& sector_coordinate) const
{
	auto sector_coordinate_without_layer = sector_coordinate;
	sector_coordinate_without_layer.layer = 0;

	auto iter = this->tiles_by_sector.find(sector_coordinate_without_layer);
	if (iter == this->tiles_by_sector.end())
	{
		return this->empty_tile_set.end();
	}

	return iter->second.end();
}
