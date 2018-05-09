// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/prop/propManager.hpp"

glooper::PropManager::PropManager(
	ResourceManager& resource_manager,
	Props& props)
{
	this->resource_manager = &resource_manager;
	this->props = &props;
}

glooper::PropManager::~PropManager()
{
	this->resource_manager->remove_listener(this);
}

glooper::PropDefinition glooper::PropManager::add_definition(const std::string& name)
{
	if(!has_definition(name))
	{
		assert(this->queries.count(name) == 0);
		this->queries.emplace(name, PropQuery(name, *this->resource_manager));
		this->definitions.emplace(name, PropDefinition(name, *this));
		this->is_dirty = true;
	}

	return this->definitions[name];
}

void glooper::PropManager::remove_definition(const std::string& name)
{
	if (has_definition(name))
	{
		assert(this->queries.count(name) != 0);
		this->queries.erase(name);
		this->definitions.erase(name);

		this->is_dirty = true;
	}
}

void glooper::PropManager::remove_definition(const PropDefinition& definition)
{
	if (definition.scope && definition.scope->manager == this)
	{
		remove_definition(definition.scope->name);
	}
}

bool glooper::PropManager::has_definition(const std::string& name) const
{
	return this->definitions.count(name) != 0;
}

bool glooper::PropManager::has_definition(const PropDefinition& definition) const
{
	if (definition.scope && definition.scope->manager == this)
	{
		assert(has_definition(definition.scope->name));
		return true;
	}

	return false;
}

void glooper::PropManager::on_reload_resources()
{
	this->is_dirty = true;
}

void glooper::PropManager::update(
	const chomsky::StaticWorldUpdateBlock::DirtyTileState& dirty_tile_state,
	const chomsky::StaticWorldUpdateBlock& static_block)
{
	execute_queries(dirty_tile_state, static_block);
	prune_definitions();
}

void glooper::PropManager::execute_queries(
	const chomsky::StaticWorldUpdateBlock::DirtyTileState& dirty_tile_state,
	const chomsky::StaticWorldUpdateBlock& static_block)
{
	std::set<thoreau::TileCoordinate> unvisited_tiles;
	if (this->is_dirty)
	{
		for (auto& i: *static_block.get_materials_state())
		{
			unvisited_tiles.insert(i.first);
		}
	}
	else
	{
		for (auto& i: dirty_tile_state)
		{
			unvisited_tiles.insert(i.first);
		}
	}

	typedef std::pair<thoreau::TileCoordinate, std::string> TilePropPair;
	std::set<TilePropPair> unvisited_props;
	for (auto& i: *this->props)
	{
		unvisited_props.emplace(i.get_tile_coordinate(), i.get_name());
		unvisited_tiles.insert(i.get_tile_coordinate());
	}

	chomsky::StaticTileSpacePointCloudMatchExecutor point_executor(&static_block);
	chomsky::StaticMaterialSetMatchExecutor material_executor(
		&static_block,
		this->resource_manager->get_texture_fingerprint_manager());
	for (auto tile_coordinate: unvisited_tiles)
	{
		for (auto& i: this->queries)
		{
			auto& query = i.second;
			if (query.match(tile_coordinate, point_executor) ||
				query.match(tile_coordinate, material_executor))
			{
				this->props->add(query.get_name(), tile_coordinate);
				unvisited_props.erase(std::make_pair(tile_coordinate, query.get_name()));
			}
		}
	}

	for (auto& i: unvisited_props)
	{
		this->props->remove(i.second, i.first);
	}

	this->is_dirty = false;
}

void glooper::PropManager::prune_definitions()
{
	std::set<std::string> unused_definitions;
	for (auto& i: this->definitions)
	{
		if (i.second.scope.use_count() == 1)
		{
			unused_definitions.insert(i.first);
		}
	}

	for (auto& definition: unused_definitions)
	{
		remove_definition(definition);
	}
}
