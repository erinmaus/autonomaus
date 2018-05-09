// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/map/minimap.hpp"
#include "thoreau/thoreau.hpp"

bool glooper::Minimap::get_is_visible() const
{
	return this->is_visible;
}

float glooper::Minimap::get_rotation() const
{
	return this->rotation;
}

const glm::vec2& glooper::Minimap::get_scale() const
{
	return this->scale;
}

const glm::vec2& glooper::Minimap::get_center() const
{
	return this->center;
}

const glm::vec2& glooper::Minimap::get_size() const
{
	return this->size;
}

const glm::vec2& glooper::Minimap::get_visible_size() const
{
	return this->visible_size;
}

const glm::vec2& glooper::Minimap::get_position() const
{
	return this->position;
}

glm::vec2 glooper::Minimap::minimap_to_subtile(glm::vec2 position) const
{
	glm::vec2 bottom_left = subtile_to_minimap(this->center - (this->size / 2.0f) / this->scale);
	position -= bottom_left;

	glm::vec2 result;
	result.x = position.x * std::cos(-this->rotation) - position.y * std::sin(-this->rotation);
	result.y = position.y * std::cos(-this->rotation) + position.x * std::sin(-this->rotation);

	result /= this->scale;
	result += this->center;
	result -= (this->size / 2.0f) / this->scale;

	return result;
}

glm::vec2 glooper::Minimap::subtile_to_minimap(glm::vec2 subtile_position) const
{
	subtile_position -= this->center;
	subtile_position *= this->scale;

	glm::vec2 result;
	result.x = subtile_position.x * std::cos(this->rotation) - subtile_position.y * std::sin(this->rotation);
	result.y = subtile_position.y * std::cos(this->rotation) + subtile_position.x * std::sin(this->rotation);

	result += (this->size / 2.0f) / this->scale;

	return result;
}

glm::vec2 glooper::Minimap::tile_to_minimap(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	auto tile = glm::vec2(tile_coordinate.x, tile_coordinate.y);
	return subtile_to_minimap(tile);
}

thoreau::TileCoordinate glooper::Minimap::minimap_to_tile(
	const glm::vec2& position) const
{
	auto tile = minimap_to_subtile(position);
	return thoreau::TileCoordinate(tile.x, tile.y);
}

bool glooper::Minimap::is_tile_visible(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	glm::vec2 tile = tile_to_minimap(tile_coordinate);
	return tile.x <= size.x && tile.y <= size.y;
}

glm::vec3 glooper::Minimap::minimap_to_world(const glm::vec2& position) const
{
	auto tile = minimap_to_subtile(position) + glm::vec2(0.5f);
	tile *= thoreau::TILE_SIZE;

	return glm::vec3(tile.x, 0.0f, tile.y);
}

glm::vec2 glooper::Minimap::world_to_minimap(const glm::vec3& world_position) const
{
	auto tile = glm::vec2(
		world_position.x / thoreau::TILE_SIZE,
		world_position.z / thoreau::TILE_SIZE);

	return subtile_to_minimap(tile);
}

thoreau::TileCoordinate glooper::Minimap::get_center_tile() const
{
	return thoreau::TileCoordinate(this->center.x, this->center.y);
}

thoreau::SectorCoordinate glooper::Minimap::get_current_sector() const
{
	return thoreau::sector_from_tile(get_center_tile());
}

void glooper::Minimap::extract(thoreau::Map& map, int layer) const
{
	thoreau::TileCoordinate current;
	if (this->working_map.get_first_tile(current))
	{
		do
		{
			auto translated_tile_coordinate = current;
			translated_tile_coordinate.layer = layer;

			auto& input_tile = this->working_map.get_tile(current);
			auto& tile = map.add_tile(translated_tile_coordinate);
			tile.append_flags(input_tile.get_flags());
		} while(this->working_map.get_next_tile(current, current));
	}
}

glooper::Minimap::const_features_iterator
glooper::Minimap::features_begin() const
{
	return this->features.begin();
}

glooper::Minimap::const_features_iterator
glooper::Minimap::features_begin(const std::string& feature) const
{
	return this->features.lower_bound(feature);
}

glooper::Minimap::const_features_iterator
glooper::Minimap::features_end() const
{
	return this->features.end();
}

glooper::Minimap::const_features_iterator
glooper::Minimap::features_end(const std::string& feature) const
{
	return this->features.upper_bound(feature);
}
