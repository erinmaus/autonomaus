// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "continent1/tileSnapshot.hpp"
#include "thoreau/thoreau.hpp"

continent1::TileSnapshot::TileSnapshot(
	const thoreau::TileCoordinate& tile_coordinate)
{
	this->tile_coordinate = tile_coordinate;
	this->bounds.min.x = (tile_coordinate.x) * thoreau::TILE_SIZE;
	this->bounds.min.y = 0.0f;
	this->bounds.min.z = (tile_coordinate.y) * thoreau::TILE_SIZE;
	this->bounds.max.x = (tile_coordinate.x + 1) * thoreau::TILE_SIZE;
	this->bounds.max.y = 0.0f;
	this->bounds.max.z = (tile_coordinate.y + 1) * thoreau::TILE_SIZE;
}

const thoreau::TileCoordinate& continent1::TileSnapshot::get_tile_coordinate() const
{
	return this->tile_coordinate;
}

bool continent1::TileSnapshot::add_model(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper,
	const glm::mat4& model_matrix,
	int tag)
{
	auto snapshot = DrawSnapshot::extract_model(
		this->bounds, model, index_mapper, model_matrix);
	if (snapshot.get_vertices().get_data_size() == 0)
	{
		return false;
	}

	snapshot.set_tag(tag);
	this->draws.push_back(snapshot);
	return true;
}

bool continent1::TileSnapshot::add_terrain(
	const kvre::Terrain* terrain,
	const glm::mat4& model_matrix,
	int tag)
{
	auto snapshot = DrawSnapshot::extract_terrain(
		this->bounds, terrain, model_matrix);
	if (snapshot.get_vertices().get_data_size() == 0)
	{
		return false;
	}

	snapshot.set_tag(tag);
	this->draws.push_back(snapshot);
	return true;
}

bool continent1::TileSnapshot::add_water(
	const kvre::Terrain* water,
	const glm::mat4& model_matrix,
	int tag)
{
	auto snapshot = DrawSnapshot::extract_water(
		this->bounds, water, model_matrix);
	if (snapshot.get_vertices().get_data_size() == 0)
	{
		return false;
	}

	snapshot.set_tag(tag);
	this->draws.push_back(snapshot);
	return true;
}

std::size_t continent1::TileSnapshot::count() const
{
	return this->draws.size();
}

bool continent1::TileSnapshot::empty() const
{
	return this->draws.empty();
}

const continent1::DrawSnapshot& continent1::TileSnapshot::get(
	std::size_t index) const
{
	return this->draws.at(index);
}

continent1::TileSnapshot::const_iterator continent1::TileSnapshot::begin() const
{
	return this->draws.begin();
}

continent1::TileSnapshot::const_iterator continent1::TileSnapshot::end() const
{
	return this->draws.end();
}
