// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/world/query/actorTileSpacePointCloudMatchExecutor.hpp"
#include "chomsky/world/query/actorMaterialSetMatchExecutor.hpp"
#include "glooper/npc/npcManager.hpp"
#include "thoreau/thoreau.hpp"

glooper::NPCManager::NPCManager(
	ResourceManager& resource_manager,
	NPCCabinet& cabinet,
	const Minimap& minimap,
	const chomsky::AnimatedWorldUpdateBlock& animated_block)
{
	this->resource_manager = &resource_manager;
	this->cabinet = &cabinet;
	this->minimap = &minimap;
	this->animated_block = &animated_block;
	this->resource_manager->add_listener(this);

	on_reload_resources();
}

glooper::NPCManager::~NPCManager()
{
	this->resource_manager->remove_listener(this);
}

glooper::NPCProfile glooper::NPCManager::add_profile(const std::string& name)
{
	if(!has_profile(name))
	{
		assert(this->queries.count(name) == 0);
		this->queries.emplace(name, NPCQuery(name, *this->resource_manager));
		this->profiles.emplace(name, NPCProfile(name, *this));
		this->queries_dirty = true;
	}

	return this->profiles[name];
}

void glooper::NPCManager::remove_profile(const std::string& name)
{
	if (has_profile(name))
	{
		assert(this->queries.count(name) != 0);
		this->queries.erase(name);
		this->profiles.erase(name);

		for (auto i = this->model_set_matches.begin(); i != this->model_set_matches.end(); /* Nop. */)
		{
			auto& match = i->second;
			if (match.first == name)
			{
				i = this->model_set_matches.erase(i);
			}
			else
			{
				++i;
			}
		}

		std::set<NPC::ID> npcs;
		for (auto& npc: *this->cabinet)
		{
			if (npc.second.get_profile_name() == name)
			{
				npcs.insert(npc.first);
			}
		}

		for (auto npc: npcs)
		{
			this->cabinet->remove(npc);
		}
	}
}

void glooper::NPCManager::remove_profile(const NPCProfile& profile)
{
	if (profile.scope && profile.scope->manager == this)
	{
		remove_profile(profile.scope->name);
	}
}

bool glooper::NPCManager::has_profile(const std::string& name) const
{
	return this->profiles.count(name) != 0;
}

bool glooper::NPCManager::has_profile(const NPCProfile& profile) const
{
	if (profile.scope && profile.scope->manager == this)
	{
		assert(has_profile(profile.scope->name));
		return true;
	}

	return false;
}

void glooper::NPCManager::on_reload_resources()
{
	this->queries_dirty = true;

	this->world_distance_threshold = this->resource_manager->get_config_float(
		"npc", "world_distance_threshold", 2);
	this->minimap_distance_threshold = this->resource_manager->get_config_float(
		"npc", "minimap_distance_threshold", 4);

	boost::filesystem::path base = this->resource_manager->get_config_string(
		"npc", "queries", "queries/npcs");
	base.append("internal");
	this->resource_manager->for_each_directory(
		base.string(),
		false,
		[this](auto& directory)
		{
			boost::filesystem::path path(directory);

			std::string query = "internal.";
			query += path.stem().string();

			this->internal_queries.emplace(query, NPCQuery(query, *this->resource_manager));
		});
}

void glooper::NPCManager::on_draw_enabled(float timestamp)
{
	this->draw_enabled = true;

	if (this->previous_draw == HUGE_VALF)
	{
		this->previous_draw = timestamp;
	}
	else
	{
		this->previous_draw = this->current_draw;
	}
	this->current_draw = timestamp;
}

void glooper::NPCManager::on_draw_disabled(float timestamp)
{
	this->draw_enabled = false;
	update();
}

void glooper::NPCManager::on_draw_model(const kompvter::ModelDrawEvent& event)
{
	if (this->draw_enabled)
	{
		this->view_matrix = event.get_view_matrix();
		this->projection_matrix = event.get_projection_matrix();
	}
}

bool glooper::NPCManager::world_distance_below_threshold(float value) const
{
	float scaled_threshold = this->world_distance_threshold * get_draw_delta();
	scaled_threshold *= thoreau::TILE_SIZE;

	return value <= thoreau::TILE_SIZE;
}

bool glooper::NPCManager::minimap_distance_below_threshold(float value) const
{
	float scaled_threshold = this->minimap_distance_threshold;
	scaled_threshold *= glm::length(this->minimap->get_scale());

	return value <= scaled_threshold;
}

float glooper::NPCManager::get_draw_delta() const
{
	return this->current_draw - this->previous_draw;
}

void glooper::NPCManager::update()
{
	std::set<std::string> unused_profiles;
	for (auto& i: this->profiles)
	{
		if (i.second.scope.use_count() == 1)
		{
			unused_profiles.insert(i.first);
		}
	}

	for (auto& profile: unused_profiles)
	{
		remove_profile(profile);
	}

	update_actors();
	update_positions();
	update_animations();

	this->previous_delta = get_draw_delta();
}

static float find_min_distance_from_feature(
	const glooper::Minimap& minimap,
	const glm::vec2& position,
	const std::string& feature)
{
	float min_distance = HUGE_VALF;
	auto begin = minimap.features_begin(feature);
	auto end = minimap.features_end(feature);
	for (auto i = begin; i != end; ++i)
	{
		auto difference = position - i->second;
		auto distance = glm::length(difference);

		if (distance < min_distance)
		{
			min_distance = distance;
		}
	}

	return min_distance;
}

static float find_min_distance_from_center(
	const glooper::Minimap& minimap,
	const glm::vec2& position)
{
	auto center = minimap.get_center();
	auto difference = position - center;
	return glm::length(difference);
}

void glooper::NPCManager::update_actors()
{
	prepare_actors_update();

	if (this->minimap->get_is_visible())
	{
		for (auto& i: *this->animated_block->get_actor_draw_state())
		{
			auto& draw = i.second;
			auto world_position = draw.get_world_position();
			auto minimap_position = minimap->world_to_minimap(world_position);
			float min_player_distance = find_min_distance_from_feature(*this->minimap, minimap_position, "player");
			float min_npc_distance = find_min_distance_from_feature(*this->minimap, minimap_position, "npc");
			float min_center_distance = find_min_distance_from_center(*this->minimap, minimap_position);

			if (min_npc_distance < min_player_distance && min_npc_distance < min_center_distance)
			{
				update_actor(i.first, i.second);
			}
		}
	}
	else
	{
		for (auto& i: *this->animated_block->get_actor_draw_state())
		{
			update_actor(i.first, i.second);
		}
	}

	for (auto& i: this->unvisited_model_sets)
	{
		this->model_set_matches.erase(i);
	}
}

void glooper::NPCManager::prepare_actors_update()
{
	for (auto& i: this->model_set_matches)
	{
		this->unvisited_model_sets.emplace(i.first);
	}

	if (this->queries_dirty)
	{
		this->checked_model_sets.clear();
	}
}

void glooper::NPCManager::update_actor(
	const thoreau::WorldCoordinate& location,
	const chomsky::ActorDraw& draw)
{
	ModelSet model_set(draw.models_begin(), draw.models_end());
	this->unvisited_model_sets.erase(model_set);

	if (this->checked_model_sets.count(model_set) == 0 &&
		this->model_set_matches.count(model_set) == 0)
	{
		chomsky::ActorTileSpacePointCloudMatchExecutor model_executor(animated_block);
		chomsky::ActorMaterialSetMatchExecutor material_executor(
			animated_block,
			resource_manager->get_texture_fingerprint_manager());

		bool is_match = false;
		ModelSetMatch match;
		for (auto i: this->queries)
		{
			auto& query = i.second;

			std::string model_name;
			if (query.match(location, model_executor, model_name) ||
				query.match(location, material_executor, model_name))
			{
				is_match = true;
				match = std::make_pair(i.first, model_name);
				this->model_set_matches.emplace(model_set, match);
				break;
			}
		}

		if (is_match)
		{
			this->pending_profile_matches.emplace(location, match);
		}
		else if (!resource_manager->get_texture_fingerprint_manager().pending())
		{
			this->checked_model_sets.insert(model_set);
		}
	}
	else
	{
		auto iter = this->model_set_matches.find(model_set);
		if (iter != this->model_set_matches.end())
		{
			auto& match = iter->second;
			this->pending_profile_matches.emplace(location, match);
		}
		this->checked_model_sets.insert(model_set);
	}
}

void glooper::NPCManager::update_positions()
{
	prepare_positions_update();
	update_visible_positions();
	if (this->minimap->get_is_visible())
	{
		update_minimap_positions();
	}

	for (auto i: this->unvisited_npcs)
	{
		this->cabinet->remove(i);
	}
	this->unvisited_npcs.clear();

	collapse_minimap_positions();
}

void glooper::NPCManager::prepare_positions_update()
{
	for (auto& i: *this->cabinet)
	{
		this->unvisited_npcs.insert(i.first);
	}

	this->visited_npcs.clear();
}

void glooper::NPCManager::update_visible_positions()
{
	for (auto& i: this->pending_profile_matches)
	{
		auto& draw = this->animated_block->get_actor_draw_state()->get(i.first);
		auto& bounds = this->animated_block->get_actor_bounds_state()->get(i.first);

		auto pending_position = glm::vec3(i.first.x, 0.0f, i.first.y);
		float min_distance = HUGE_VALF;
		NPC::ID nearest = NPC::ID_NONE;
		for (auto j: this->unvisited_npcs)
		{
			if (!this->cabinet->has(j))
			{
				continue;
			}

			auto& npc = this->cabinet->get(j);
			auto current_position = npc.get_position();
			auto velocity = current_position - npc.get_previous_position();
			auto future_position = current_position + velocity;

			current_position.y = 0.0f;
			future_position.y = 0.0f;

			float current_distance = glm::length(current_position - pending_position);
			float future_distance = glm::length(future_position - pending_position);
			if (world_distance_below_threshold(current_distance) ||
				world_distance_below_threshold(future_distance) ||
				(!npc.get_is_on_screen() && current_distance < this->minimap_distance_threshold))
			{
				if (current_distance < min_distance ||
					future_distance < min_distance)
				{
					min_distance = std::min(current_distance, future_distance);
					nearest = j;
				}
			}
		}

		if (nearest == NPC::ID_NONE)
		{
			auto& npc = this->cabinet->add();
			npc.set_previous_position(draw.get_world_position());
			nearest = npc.get_id();
		}
		else
		{
			if (this->cabinet->get(nearest).get_profile_name() != i.second.first)
			{
				this->cabinet->remove(nearest);
				auto& npc = this->cabinet->add();
				npc.set_previous_position(draw.get_world_position());
				nearest = npc.get_id();
			}
		}

		auto& npc = this->cabinet->get(nearest);
		npc.set_world_coordinate(i.first);
		npc.set_profile_name(i.second.first);
		npc.set_profile_variant(i.second.second);
		npc.set_is_on_screen(true);

		npc.set_position(draw.get_world_position());
		npc.set_model_matrix(draw.get_model_matrix());

		npc.set_bounds(bounds);

		this->visited_npcs.insert(nearest);
		this->unvisited_npcs.erase(nearest);
	}
	this->pending_profile_matches.clear();
}

void glooper::NPCManager::update_minimap_positions() 
{
	assert(this->minimap->get_is_visible());

	for (auto i = this->unvisited_npcs.begin(); i != this->unvisited_npcs.end(); /* Nil. */)
	{
		if (!this->cabinet->has(*i))
		{
			continue;
		}

		auto& npc = this->cabinet->get(*i);
		auto minimap_position = this->minimap->world_to_minimap(npc.get_position());

		int count = 0;
		glm::vec2 dot_position;
		float closest_dot = HUGE_VALF;
		auto begin = this->minimap->features_begin("npc");
		auto end = this->minimap->features_end("npc");
		for (auto j = begin; j != end; ++j)
		{
			auto difference = minimap_position - j->second;
			float distance = glm::length(difference);
			if (minimap_distance_below_threshold(distance))
			{
				if (distance < closest_dot)
				{
					dot_position = j->second;
					closest_dot = distance;
				}

				// XXX: We can break if count exceeds 1. But the others would
				// feel lonely...
				++count;
			}
		}

		if (count == 0)
		{
			++i;
		}
		else
		{
			auto& npc = this->cabinet->get(*i);
			auto position = this->minimap->minimap_to_world(dot_position);
			auto world_coordinate = thoreau::world_from_position(npc.get_position());
			world_coordinate.layer = npc.get_world_coordinate().layer;

			npc.set_position(position);
			npc.set_world_coordinate(world_coordinate);
			npc.set_is_on_screen(false);

			this->visited_npcs.insert(*i);
			i = this->unvisited_npcs.erase(i);
		}
	}
}

void glooper::NPCManager::collapse_minimap_positions()
{
	std::set<NPC::ID> pending;

	for (auto i = this->visited_npcs.begin(); i != this->visited_npcs.end(); ++i)
	{
		auto& source = this->cabinet->get(*i);
		if (source.get_is_on_screen())
		{
			continue;
		}

		if (pending.count(*i) != 0)
		{
			continue;
		}

		if (!source.maybe_on_screen(this->view_matrix, this->projection_matrix))
		{
			continue;
		}

		for (auto j = this->visited_npcs.begin(); j != this->visited_npcs.end(); ++j)
		{
			auto& destination = this->cabinet->get(*j);
			if (!destination.get_is_on_screen())
			{
				continue;
			}

			if (pending.count(*j) != 0)
			{
				continue;
			}

			if (source.get_profile_name() != destination.get_profile_name() ||
				source.get_profile_variant() != destination.get_profile_variant())
			{
				continue;
			}

			auto distance = destination.get_position() - source.get_position();
			distance /= thoreau::TILE_SIZE;
			if (glm::length(distance) <= this->world_distance_threshold)
			{
				if (*i < *j)
				{
					pending.insert(*j);
					this->cabinet->get(*i) = NPC(*i, destination);
				}
				else
				{
					pending.insert(*i);
					this->cabinet->get(*j) = NPC(*j, destination);
				}
			}
		}
	}

	for (auto i: pending)
	{
		this->cabinet->remove(i);
	}
}

void glooper::NPCManager::update_animations()
{
	chomsky::ActorTileSpacePointCloudMatchExecutor model_executor(animated_block);
	auto& actor_draws = *animated_block->get_actor_draw_state();

	for (auto& i: *this->cabinet)
	{
		auto& npc = this->cabinet->get(i.first);
		auto key = npc.get_world_coordinate();

		if (actor_draws.has(key))
		{
			std::set<std::string> animations;

			auto query = this->queries.find(npc.get_profile_name());
			if (query != this->queries.end())
			{
				std::vector<std::string> matches;
				query->second.match(actor_draws.get(key), matches);
				query->second.match(key, model_executor, matches);

				animations.insert(matches.begin(), matches.end());
			}

			for (auto& query: this->internal_queries)
			{
				std::vector<std::string> matches;
				query.second.match(key, model_executor, matches);

				animations.insert(matches.begin(), matches.end());
			}

			npc.set_animations(animations);
		}
	}
}
