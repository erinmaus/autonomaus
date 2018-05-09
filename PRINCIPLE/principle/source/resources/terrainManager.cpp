// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/resources/terrainManager.hpp"

bool principle::TerrainManager::has(int tag)
{
	return this->tags.count(tag) != 0;
}

const kvre::Terrain* principle::TerrainManager::get(int tag)
{
	auto iter = this->tags.find(tag);
	if (iter != this->tags.end())
	{
		auto terrain = terrains.find(iter->second);
		assert(terrain != this->terrains.end());
		return &terrain->second;
	}

	return nullptr;
}

void principle::TerrainManager::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	auto iter = this->terrains.find(terrain);
	if (iter == this->terrains.end())
	{
		this->terrains.emplace(terrain, *terrain);
	}

	this->tags.emplace(tag, terrain);
}

void principle::TerrainManager::invalidate_tag(int tag)
{
	this->tags.erase(tag);
}

void principle::TerrainManager::invalidate_terrain(const kvre::Terrain* terrain)
{
	this->terrains.erase(terrain);
}
