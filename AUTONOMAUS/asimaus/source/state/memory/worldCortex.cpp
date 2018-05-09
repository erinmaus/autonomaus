// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <cstdlib>
#include <boost/regex.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/input/gestures/worldViewportMouseMoveGesture.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/guiProvider.hpp"
#include "autonomaus/state/lootProvider.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/propsProvider.hpp"
#include "autonomaus/state/npcProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/memory/viewCortex.hpp"
#include "autonomaus/state/memory/worldCortex.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvlc/collision/scene.hpp"
#include "kvlc/collision/query.hpp"
#include "thoreau/map.hpp"
#include "thoreau/thoreau.hpp"

struct autonomaus::WorldCortex::MoveMouseAction : public Action
{
	MoveMouseAction(
		WorldCortex* cortex);
	~MoveMouseAction();

	Result get_result() override;
	void update() override;

	virtual void cancel();

	virtual glm::vec3 get_world_position() = 0;
	virtual bool is_point_stale(const glm::vec2& point);

	virtual Result::Status on_start() { return Result::Status::success; }
	virtual Result::Status on_success() = 0;
	virtual Result::Status on_failure() { return Result::Status::failure; }

	bool is_point_in_bounds(
		const glm::vec2& point,
		const kvlc::Bounds& bounds,
		const glm::mat4& model_matrix = glm::mat4(1.0f));

	WorldCortex* cortex;

private:
	bool try_get_screen_space_point(glm::vec2& result);

	enum
	{
		state_start,
		state_move,
		state_failure,
		state_wait,
		state_success,
		state_done
	};
	int state = state_start;

	// This requires finer control & timing than MotionSequence can provide.
	std::unique_ptr<Gesture> gesture;
	void play_gesture();

	glm::vec2 screen_space_point = glm::vec2(0.0f);
	glm::vec2 target_screen_space_point = glm::vec2(0.0f);
	glm::vec2 previous_screen_space_point = glm::vec2(0.0f);

	int pending_frame;

	Result::StatusReference status;
	Result final_result;
};

struct autonomaus::WorldCortex::HoverAction : public MoveMouseAction
{
	HoverAction(
		WorldCortex* cortex,
		const thoreau::TileCoordinate& tile_coordinate);
	~HoverAction() = default;

	glm::vec3 get_world_position() override;
	Result::Status on_success() override { return Result::Status::success; }

	thoreau::TileCoordinate tile_coordinate;

private:
	glm::vec3 world_position = glm::vec3(0.0f);
	void compute_world_position();
};

struct autonomaus::WorldCortex::WalkHereAction : public MoveMouseAction
{
	WalkHereAction(
		WorldCortex* cortex,
		const thoreau::TileCoordinate& tile_coordinate,
		bool left_click_only);
	~WalkHereAction();

	void cancel() override;
	glm::vec3 get_world_position() override;
	Result::Status on_success() override;

	thoreau::TileCoordinate tile_coordinate;
	bool left_click_only;

private:
	enum
	{
		state_start,
		state_click,
		state_failure,
		state_success,
		state_done
	};
	int state = state_start;

	Result current_result;

	glm::vec3 world_position = glm::vec3(0.0f);
	void compute_world_position();
};

struct autonomaus::WorldCortex::PokePositionAction : public MoveMouseAction
{
	PokePositionAction(
		WorldCortex* cortex,
		const std::vector<std::string>& options);
	~PokePositionAction();

	void cancel() override;
	Result::Status on_success() override;

	std::vector<std::string> options;
	bool hidden = false;

private:
	enum
	{
		state_start,
		state_wait,
		state_select,
		state_click,
		state_after_click,
		state_failure,
		state_success
	};
	int state = state_start;

	float target_time = 0.0f;
	int click_frame = 0;

	Result current_result;
};

struct autonomaus::WorldCortex::PokeObjectAction : public PokePositionAction
{
public:
	PokeObjectAction(
		WorldCortex* cortex,
		const std::vector<std::string>& options,
		const thoreau::TileCoordinate& tile_coordinate,
		const glm::vec3& spray_offset,
		int spray_count,
		float spray_radius);

	glm::vec3 get_world_position() override;
	Result::Status on_start() override;

	thoreau::TileCoordinate tile_coordinate;
	glm::vec3 spray_offset;
	int spray_count;
	float spray_radius;

private:
	enum
	{
		state_start,
		state_failure,
		state_success
	};
	int state = state_start;

	std::future<kvlc::Query> query;
	void perform_query();

	glm::vec3 world_position;
};

struct autonomaus::WorldCortex::PokeNPCAction : public PokePositionAction
{
	PokeNPCAction(
		WorldCortex* cortex,
		const std::vector<std::string>& options,
		int id,
		bool hidden);

	glm::vec3 get_world_position() override;
	bool is_point_stale(const glm::vec2& point) override;

	int id;

private:
	glm::vec3 world_position;
};

struct autonomaus::WorldCortex::TakeItemAction : public PokePositionAction
{
	TakeItemAction(
		WorldCortex* cortex,
		const thoreau::TileCoordinate& tile_coordinate,
		const std::string& item);
	~TakeItemAction() = default;

	glm::vec3 get_world_position() override;

	thoreau::TileCoordinate tile_coordinate;

private:
	glm::vec3 world_position = glm::vec3(0.0f);
	void compute_world_position();
};

struct autonomaus::WorldCortex::OpenDoorAction : public Action
{
	OpenDoorAction(
		WorldCortex* cortex,
		const thoreau::TileCoordinate& tile_coordinate,
		bool peek_only = false);
	~OpenDoorAction();

	Result get_result() override;
	void update() override;

	void cancel();

	WorldCortex* cortex;

	thoreau::TileCoordinate tile_coordinate;
	bool peek_only;

private:
	enum
	{
		state_start,
		state_open,
		state_failure,
		state_success,
		state_done
	};
	int state = state_start;
	Result current_result;

	float offset_x;
	float offset_y;

	std::future<kvlc::Query> query;
	void perform_query();

	Result::StatusReference status;
	Result final_result;
};

autonomaus::WorldCortex::WorldCortex(Autonomaus& autonomaus) :
	Cortex("Game::World", autonomaus),
	click_query("*.main.click")
{
	load_meta();

	on_poke(
		"find_object",
		[this](auto& message)
		{
			return this->find_object(message.get_string("name"));
		});

	on_poke(
		"find_npc",
		[this](auto& message)
		{
			return this->find_npc(message.get_string("name"));
		});

	on_pull(
		"nearby_objects",
		[this](auto& message)
		{
			bool visible = true;
			if (message.has("visible"))
			{
				visible = message.get_boolean("visible");
			}

			auto objects = this->get_nearby_objects(
				message.get_string("name"),
				visible);

			Message result;
			for (std::size_t i = 0; i < objects.size(); ++i)
			{
				result.set_integer("x", objects[i].x, i);
				result.set_integer("y", objects[i].y, i);
				result.set_integer("layer", objects[i].layer, i);
			}

			return result;
		});

	on_pull(
		"nearby_npcs",
		[this](auto& message)
		{
			bool visible;
			if (message.has("visible"))
			{
				visible = message.get_boolean("visible");
			}
			else
			{
				visible = false;
			}

			auto npcs = this->get_nearby_npcs(message.get_string("name"), visible);

			Message result;
			for (std::size_t i = 0; i < npcs.size(); ++i)
			{
				result.set_integer("id", npcs[i], i);
			}

			return result;
		});

	on_pull(
		"nearby_items",
		[this](auto& message)
		{
			auto items = this->get_nearby_items(message.get_string("name"));

			Message result;
			for (std::size_t i = 0; i < items.size(); ++i)
			{
				result.set_integer("x", items[i].x, i);
				result.set_integer("y", items[i].y, i);
				result.set_integer("layer", items[i].layer, i);
			}

			return result;
		});

	on_pull(
		"npc_animations",
		[this](auto& message)
		{
			auto animations = this->get_npc_animations(message.get_integer("id"));

			Message result;
			for (std::size_t i = 0; i < animations.size(); ++i)
			{
				result.set_string("result", animations[i], i);
			}

			return result;
		});

	on_poke(
		"poke_nearby_object",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->poke_nearby_object(
				tile_coordinate,
				message.get_string("name"),
				message.get_string("action"));
		});

	on_poke(
		"poke_nearby_npc",
		[this](auto& message)
		{
			return this->poke_nearby_npc(
				message.get_integer("id"),
				message.get_string("action"));
		});

	on_poke(
		"take_nearby_item",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->take_nearby_item(tile_coordinate, message.get_string("name"));
		});

	on_poke(
		"walk_here",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->walk_here(tile_coordinate, message.get_boolean("left_click_only"));
		});

	on_poke(
		"hover",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->hover(tile_coordinate);
		});

	on_poke(
		"open_door",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->open_door(tile_coordinate);
		});

	on_poke(
		"peek_door",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->peek_door(tile_coordinate);
		});

	on_poke(
		"ascend",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->ascend(tile_coordinate);
		});

	on_poke(
		"descend",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->descend(tile_coordinate);
		});

	on_pull(
		"last_click",
		[this](auto&)
		{
			Message result;
			result.set_integer("type", this->last_click);
			result.set_integer("frame", this->last_click_frame);
			result.set_single("timestamp", this->last_click_timestamp);
			return result;
		});
}

autonomaus::Cortex::Result
autonomaus::WorldCortex::find_object(const std::string& name)
{
	auto& props = get_autonomaus().get_props_state();
	auto& game_state = this->get_autonomaus().get_game_state();

	Result::StatusReference status = std::make_shared<Result::Status>(Result::Status::success);
	Result result(
		status,
		[this, name, &props, &game_state]
		{
			auto& game_state = this->get_autonomaus().get_game_state();
			PropsProvider::remove_definition(game_state, props, name);
		});

	PropsProvider::add_definition(game_state, props, name);
	return result;
}

autonomaus::Cortex::Result
autonomaus::WorldCortex::find_npc(const std::string& name)
{
	auto& npc = get_autonomaus().get_npc_state();
	auto& game_state = this->get_autonomaus().get_game_state();

	Result::StatusReference status = std::make_shared<Result::Status>(Result::Status::success);
	Result result(
		status,
		[this, name, &npc, &game_state]
		{
			auto& game_state = this->get_autonomaus().get_game_state();
			NPCProvider::remove_profile(game_state, npc, name);
		});

	NPCProvider::add_profile(game_state, npc, name);
	return result;
}

std::vector<thoreau::TileCoordinate>
autonomaus::WorldCortex::get_nearby_objects(
	const std::string& name,
	bool visible) const
{
	if (visible)
	{
		return get_nearby_visible_objects(name);
	}
	else
	{
		return get_nearby_invisible_objects(name);
	}
}

std::vector<thoreau::TileCoordinate>
autonomaus::WorldCortex::get_nearby_visible_objects(const std::string& name) const
{
	auto& props = *get_autonomaus().get_props_state();

	std::vector<thoreau::TileCoordinate> results;
	for (auto i = props.begin(name); i != props.end(name); ++i)
	{
		results.push_back(i->second);
	}

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view != nullptr)
	{
		std::sort(
			results.begin(), results.end(),
			[view](auto a, auto b)
			{
				auto p = view->get_target_tile_coordinate();
				int a_x = a.x - p.x;
				int a_y = a.y - p.y;
				int a_distance = a_x * a_x + a_y * a_y;
				int b_x = b.x - p.x;
				int b_y = b.y - p.y;
				int b_distance = b_x * b_x + b_y * b_y;
				return a_distance < b_distance;
			});

		for (auto i = results.begin(); i != results.end(); /* Nop. */)
		{
			if (view->get_target_tile_coordinate().layer != i->layer)
			{
				i = results.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	return results;
}

std::vector<thoreau::TileCoordinate>
autonomaus::WorldCortex::get_nearby_invisible_objects(const std::string& name) const
{
	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		return {};
	}

	std::vector<std::pair<thoreau::TileCoordinate, float>> pending_results;
	for (auto& tile: this->objects_by_tile)
	{
		if (!view->is_in_bounds(tile.first))
		{
			continue;
		}

		auto iter = tile.second.find(name);
		if (iter != tile.second.end() && !iter->second.visible)
		{
			pending_results.emplace_back(tile.first, iter->second.last_seen);
		}
	}

	std::sort(
		pending_results.begin(), pending_results.end(),
		[view](auto a, auto b)
		{
			return a.second < b.second;
		});

	std::vector<thoreau::TileCoordinate> results;
	for (auto& i: pending_results)
	{
		results.push_back(i.first);
	}

	return results;
}

std::vector<thoreau::TileCoordinate>
autonomaus::WorldCortex::get_nearby_items(const std::string& name) const
{
	auto& loot = *get_autonomaus().get_loot_state();
	auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();

	if (item_cortex == nullptr)
	{
		return {};
	}

	std::vector<thoreau::TileCoordinate> results;
	for (auto& i: loot)
	{
		if (item_cortex->has_name(i.second))
		{
			auto names = item_cortex->get_names(i.second);
			for (auto& n: names)
			{
				if (fuzzy_string_match(n, name))
				{
					results.push_back(i.first);
					break;
				}
			}
		}
	}

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view != nullptr)
	{
		std::sort(
			results.begin(), results.end(),
			[view](auto a, auto b)
			{
				auto p = view->get_target_tile_coordinate();
				int a_x = a.x - p.x;
				int a_y = a.y - p.y;
				int a_distance = a_x * a_x + a_y * a_y;
				int b_x = b.x - p.x;
				int b_y = b.y - p.y;
				int b_distance = b_x * b_x + b_y * b_y;
				return a_distance < b_distance;
			});

		for (auto i = results.begin(); i != results.end(); /* Nop. */)
		{
			if (view->get_target_tile_coordinate().layer != i->layer)
			{
				i = results.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	return results;
}

std::vector<int>
autonomaus::WorldCortex::get_nearby_npcs(const std::string& name, bool visible) const
{
	auto& npcs = *get_autonomaus().get_npc_state();

	std::vector<int> results;
	for (auto& i: npcs)
	{
		auto& npc = i.second;

		if (npc.get_profile_name() == name)
		{
			if (!visible || npc.get_is_on_screen())
			{
				results.push_back(npc.get_id());
			}
		}
	}

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view != nullptr)
	{
		std::sort(
			results.begin(), results.end(),
			[view, &npcs](auto a, auto b)
			{
				auto p_world = view->get_target_position();
				p_world.y = 0.0f;
				auto a_world = npcs.get(a).get_position() - p_world;
				a_world.y = 0.0f;
				auto b_world = npcs.get(b).get_position() - p_world;
				b_world.y = 0.0f;

				return glm::length2(a_world) < glm::length2(b_world);
			});
	}

	return results;
}

std::vector<std::string>
autonomaus::WorldCortex::get_npc_animations(int id) const
{
	std::vector<std::string> result;

	auto& npcs = *get_autonomaus().get_npc_state();
	if (npcs.has(id))
	{
		auto& npc = npcs.get(id);
		result.insert(
			result.begin(),
			npc.get_animations().begin(),
			npc.get_animations().end());
	}

	return result;
}

autonomaus::Cortex::Result autonomaus::WorldCortex::poke_nearby_object(
	const thoreau::TileCoordinate& tile_coordinate,
	const std::string& name,
	const std::string& action,
	const glm::vec3& offset)
{
	Log::info("(WorldCortex) Performing poke '{}' on object '{}' at tile ({}, {}; {})",
		action,
		name,
		tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);

	glm::vec3 spray_offset;
	float spray_radius;
	int spray_count;
	{
		static const auto DEFAULT_X = Config::get<float>("game", "WorldCortex", "DEFAULT_SPRAY_OFFSET_X");
		static const auto DEFAULT_Y = Config::get<float>("game", "WorldCortex", "DEFAULT_SPRAY_OFFSET_Y");
		static const auto DEFAULT_Z = Config::get<float>("game", "WorldCortex", "DEFAULT_SPRAY_OFFSET_Z");
		static const auto DEFAULT_COUNT = Config::get<int>("game", "WorldCortex", "DEFAULT_SPRAY_COUNT");
		static const auto DEFAULT_RADIUS = Config::get<int>("game", "WorldCortex", "DEFAULT_SPRAY_RADIUS");

		auto spray_node = this->object_meta[name]["spray"];
		if (this->object_meta[name]["spray"])
		{
			spray_offset.x = spray_node["offset-x"].as<float>(DEFAULT_X);
			spray_offset.y = spray_node["offset-y"].as<float>(DEFAULT_Y);
			spray_offset.z = spray_node["offset-z"].as<float>(DEFAULT_Z);
			spray_count = spray_node["count"].as<int>(DEFAULT_COUNT);
			spray_radius = spray_node["radius"].as<int>(DEFAULT_RADIUS);
		}
		else
		{
			spray_offset.x = DEFAULT_X;
			spray_offset.y = DEFAULT_Y;
			spray_offset.z = DEFAULT_Z;
			spray_count = DEFAULT_COUNT;
			spray_radius = DEFAULT_RADIUS;
		}
	}

	auto actions = build_actions(this->object_meta, name, action);

	return perform(new PokeObjectAction(
		this,
		actions,
		tile_coordinate,
		spray_offset + offset,
		spray_count,
		spray_radius));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::poke_nearby_npc(int id, const std::string& action)
{
	auto& npcs = *get_autonomaus().get_npc_state();
	if (!npcs.has(id))
	{
		Log::warn("(WorldCortex) NPC (id: {}) not found.", id);
		return Result::Status::failure;
	}

	auto& npc = npcs.get(id);
	Log::info("(WorldCortex) Performing poke '{}' on NPC '{}' (id: '{}')...",
		action,
		npc.get_profile_name(),
		id);

	bool hidden = false;
	if (this->npc_meta[npc.get_profile_name()])
	{
		auto meta = this->npc_meta[npc.get_profile_name()];
		if (meta["hidden"])
		{
			hidden = true;
		}
	}

	auto actions = build_actions(this->npc_meta, npc.get_profile_name(), action);
	return perform(new PokeNPCAction(this, actions, id, hidden));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::take_nearby_item(
	const thoreau::TileCoordinate& tile_coordinate,
	const std::string& item)
{
	Log::info(
		"(WorldCortex) Taking item '{}' from nearby tile ({}, {}; {})...",
		item,
		tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);

	return perform(new TakeItemAction(this, tile_coordinate, item));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::walk_here(
	const thoreau::TileCoordinate& tile_coordinate,
	bool left_click_only)
{
	Log::info(
		"(WorldCortex) Walking to nearby tile ({}, {}; {})...",
		tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view != nullptr)
	{
		if (view->get_target_tile_coordinate().layer != tile_coordinate.layer)
		{
			Log::warn("(WorldCortex) Player is on different layer ({})!", tile_coordinate.layer);
			return Result::Status::failure;
		}
	}

	return perform(new WalkHereAction(this, tile_coordinate, left_click_only));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::hover(const thoreau::TileCoordinate& tile_coordinate)
{
	Log::info(
		"(WorldCortex) Moving mouse to nearby tile ({}, {}; {})...",
		tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);

	return perform(new HoverAction(this, tile_coordinate));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::open_door(
	const thoreau::TileCoordinate& tile_coordinate)
{
	Log::info(
		"(WorldCortex) Opening door on nearby tile ({}, {}; {})...",
		tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view != nullptr)
	{
		if (view->get_target_tile_coordinate().layer != tile_coordinate.layer)
		{
			Log::warn("(WorldCortex) Player is on different layer ({})!", tile_coordinate.layer);
			return Result::Status::failure;
		}
	}

	return perform(new OpenDoorAction(this, tile_coordinate));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::peek_door(
	const thoreau::TileCoordinate& tile_coordinate)
{
	Log::info(
		"(WorldCortex) Opening door on nearby tile ({}, {}; {})...",
		tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);

	return perform(new OpenDoorAction(this, tile_coordinate, true));
}

autonomaus::Cortex::Result autonomaus::WorldCortex::ascend(
	const thoreau::TileCoordinate& tile_coordinate)
{
	return poke_nearby_object(tile_coordinate, "internal.portal_up", "use");
}

autonomaus::Cortex::Result autonomaus::WorldCortex::descend(
	const thoreau::TileCoordinate& tile_coordinate)
{
	return poke_nearby_object(tile_coordinate, "internal.portal_down", "use");
}

int autonomaus::WorldCortex::get_last_click_type() const
{
	return this->last_click;
}

int autonomaus::WorldCortex::get_last_click_frame() const
{
	return this->last_click_frame;
}

float autonomaus::WorldCortex::get_last_click_timestamp() const
{
	return this->last_click_timestamp;
}

void autonomaus::WorldCortex::update()
{
	Cortex::update();

	if (get_autonomaus().get_gui_state()->has("game"))
	{
		auto& gui_root = get_autonomaus().get_gui_state()->get("game");

		std::vector<chomsky::GUIContent> elements;
		if (this->click_query.collect(gui_root, elements) && !elements.empty())
		{
			auto& tags = elements[0].get_tags();
			if (tags.has("click-interact"))
			{
				this->last_click = click_poke;
			}
			else if (tags.has("click-walk"))
			{
				this->last_click = click_walk;
			}
			else
			{
				this->last_click = click_none;
			}

			auto& game_state = get_autonomaus().get_game_state();
			this->last_click_timestamp = game_state.get_last_update_timestamp();
			this->last_click_frame = game_state.get_last_update_frame();
		}
	}

	for (auto& tiles: this->objects_by_tile)
	{
		for (auto& object: tiles.second)
		{
			object.second.visible = false;
		}
	}

	auto& props = *get_autonomaus().get_props_state();
	for (auto& prop: props)
	{
		auto& object = this->objects_by_tile[prop.get_tile_coordinate()][prop.get_name()];
		object.name = prop.get_name();
		object.last_seen = get_current_time();
		object.visible = true;
	}
}

void autonomaus::WorldCortex::load_meta()
{
	boost::filesystem::path root(get_autonomaus().get_game_state().get_resource_manager().get_path());

	try
	{
		auto objects = root;
		objects.append("queries");
		objects.append("props");
		objects.append("meta.yml");

		this->object_meta = YAML::LoadFile(objects.string());
	}
	catch (const std::exception& e)
	{
		Log::critical("(WorldCortex) Couldn't load objects' meta: {}", e.what());
	}

	try
	{
		auto npcs = root;
		npcs.append("queries");
		npcs.append("npcs");
		npcs.append("meta.yml");

		this->npc_meta = YAML::LoadFile(npcs.string());
	}
	catch (const std::exception& e)
	{
		Log::critical("(WorldCortex) Couldn't load NPCs' meta: {}", e.what());
	}
}

std::vector<std::string> autonomaus::WorldCortex::build_actions(
	const YAML::Node& meta,
	const std::string& name,
	const std::string& action)
{
	std::vector<std::string> actions;

	if (meta[name] && meta[name]["actions"] && meta[name]["actions"][action])
	{
		auto actions_node = meta[name]["actions"][action];
		actions = actions_node.as<std::vector<std::string>>(std::vector<std::string>());
	}

	if (actions.empty() && meta[name])
	{
		auto names_node = meta[name]["names"];
		if (names_node)
		{
			for (auto name_node: names_node)
			{
				auto name = name_node.as<std::string>(std::string());
				if (!name.empty())
				{
					if (!action.empty() && action[0] == '%')
					{
						actions.push_back(action + ".*" + name);
					}
					else
					{
						auto sanitized_action = boost::regex_replace(
							action,
							boost::regex("[.^$|()\\[\\]{}*+?\\\\]"),
							"\\\\&",
							boost::match_default | boost::format_sed);

						actions.push_back(std::string("%") + sanitized_action + ".*" + name + ".*");
					}
				}
			}
		}
	}

	if (actions.empty())
	{
		actions.push_back(action);
	}

	return actions;
}

autonomaus::WorldCortex::MoveMouseAction::MoveMouseAction(
	WorldCortex* cortex) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
}

autonomaus::WorldCortex::MoveMouseAction::~MoveMouseAction()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->gesture && this->gesture->pending())
	{
		this->gesture->cancel();
	}

	this->final_result.terminate();
}

autonomaus::Cortex::Result autonomaus::WorldCortex::MoveMouseAction::get_result()
{
	return this->final_result;
}

void autonomaus::WorldCortex::MoveMouseAction::update()
{
	switch (this->state)
	{
		case state_start:
			{
				auto result = on_start();
				if (result == Result::Status::success)
				{
					try_get_screen_space_point(this->previous_screen_space_point);

					this->state = state_move;
					play_gesture();
				}
				else if (result == Result::Status::failure)
				{
					this->state = state_failure;
					break;
				}
			}
			break;
		case state_move:
			if (!this->gesture->pending())
			{
				this->state = state_wait;
				this->pending_frame = this->cortex->get_autonomaus().get_current_frame();
			}
			else if (is_point_stale(this->screen_space_point))
			{
				play_gesture();
			}
			break;
		case state_wait:
			{
				static const int THRESHOLD = Config::get<int>("game", "WorldCortex", "UPDATE_THRESHOLD");
				auto& game_state = this->cortex->get_autonomaus().get_game_state();
				int difference = game_state.get_last_update_frame() - this->pending_frame;
				if (difference >= THRESHOLD)
				{
					this->state = state_success;

					auto result = on_success();
					if (result != Result::Status::working)
					{
						*this->status = result;
						this->state = state_done;
					}
				}
				else if (is_point_stale(this->target_screen_space_point))
				{
					this->state = state_move;
					play_gesture();
				}
				break;
			}
			break;
		case state_failure:
			{
				auto result = on_failure();
				if (result != Result::Status::working)
				{
					*this->status = result;
					this->state = state_done;
				}
			}
			break;
		case state_success:
			{
				auto result = on_success();
				if (result != Result::Status::working)
				{
					*this->status = result;
					this->state = state_done;
				}
			}
			break;
		case state_done:
			// Nothing.
			break;
		default:
			Log::warn("(WorldCortex::MoveMouseAction) Unhandled state {}. Bad logic!", this->state);
			cancel();
			break;
	}

	try_get_screen_space_point(this->previous_screen_space_point);
}

void autonomaus::WorldCortex::MoveMouseAction::cancel()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->gesture && this->gesture->pending())
	{
		this->gesture->cancel();
	}

	this->final_result.terminate();
}

bool autonomaus::WorldCortex::MoveMouseAction::try_get_screen_space_point(
	glm::vec2& result)
{
	auto position = get_world_position();
	auto camera = Camera(this->cortex->get_autonomaus().get_draw_state());

	auto view = this->cortex->get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		if (camera.is_visible(position))
		{
			result = glm::vec2(camera.world_to_screen(position));
			return true;
		}
	}
	else
	{
		if (view->is_in_bounds(position))
		{
			result = glm::vec2(camera.world_to_screen(position));
			return true;
		}
	}

	return false;
}

bool autonomaus::WorldCortex::MoveMouseAction::is_point_stale(const glm::vec2& point)
{
	glm::vec2 current_point;
	if (!try_get_screen_space_point(current_point))
	{
		return true;
	}

	static const float DRIFT = Config::get<float>("game", "WorldCortex", "DRIFT");
	if (glm::distance(current_point, point) > DRIFT)
	{
		return true;
	}

	return false;
}

bool autonomaus::WorldCortex::MoveMouseAction::is_point_in_bounds(
	const glm::vec2& point,
	const kvlc::Bounds& bounds,
	const glm::mat4& model_matrix)
{
	glm::vec4 points[] = {
		glm::vec4(bounds.min.x, bounds.min.y, bounds.min.z, 1.0f),
		glm::vec4(bounds.max.x, bounds.min.y, bounds.min.z, 1.0f),
		glm::vec4(bounds.min.x, bounds.max.y, bounds.min.z, 1.0f),
		glm::vec4(bounds.min.x, bounds.min.y, bounds.max.z, 1.0f),
		glm::vec4(bounds.max.x, bounds.max.y, bounds.min.z, 1.0f),
		glm::vec4(bounds.max.x, bounds.min.y, bounds.max.z, 1.0f),
		glm::vec4(bounds.min.x, bounds.max.y, bounds.max.z, 1.0f),
		glm::vec4(bounds.max.x, bounds.max.y, bounds.max.z, 1.0f)
	};

	for (std::size_t i = 0; i < 8; ++i)
	{
		points[i] = model_matrix * points[i];
	}

	auto camera = Camera(this->cortex->get_autonomaus().get_draw_state());
	glm::vec2 projections[8];
	glm::vec2 center = glm::vec2(0.0f);
	for (std::size_t i = 0; i < 8; ++i)
	{
		projections[i] = glm::vec2(camera.world_to_screen(glm::vec3(points[i])));
		center += projections[i];
	}
	center /= 8.0f;

	std::sort(
		projections,
		projections + 8,
		[center](auto a, auto b)
		{
			// https://stackoverflow.com/a/6989383
			if (a.x - center.x >= 0 && b.x - center.x < 0)
			{
				return true;
			}
			else if (a.x - center.x < 0 && b.x - center.x >= 0)
			{
				return false;
			}
			else if (a.x - center.x == 0 && b.x - center.x == 0)
			{
				if (a.y - center.y >= 0 || b.y - center.y >= 0)
				{
					return a.y > b.y;
				}
				
				return b.y > a.y;
			}

			int det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
			if (det < 0)
			{
				return true;
			}
			else if (det > 0)
			{
				return false;
			}

			int d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
			int d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
			return d1 > d2;
		});

	int crosses = 0;
	for (std::size_t i = 0; i < 8; ++i)
	{
		auto a = projections[i];
		auto b = projections[(i + 1) % 8];

		if ((a.y <= point.y && b.y > point.y) ||
			(a.y > point.y && b.y <= point.y))
		{
			float v = (float)(point.y - a.y) / (float)(b.y - a.y);
			float u = a.x + v * (b.x - a.x);
			if (point.x < u)
			{
				++crosses;
			}
		}
	}

	return (crosses % 2) == 1;
}

void autonomaus::WorldCortex::MoveMouseAction::play_gesture()
{
	auto position = get_world_position();
	if (!try_get_screen_space_point(this->screen_space_point))
	{
		Log::warn(
			"(WorldCortex::MoveMouseAction) Position {:1f}, {:0f}, {:1f} is not visible.",
			position.x / thoreau::TILE_SIZE,
			position.y,
			position.z / thoreau::TILE_SIZE);
		this->state = state_failure;
		return;
	}

	this->target_screen_space_point = this->screen_space_point + (this->screen_space_point - this->previous_screen_space_point) * 4.0f;

	auto camera = Camera(this->cortex->get_autonomaus().get_draw_state());
	this->gesture = std::make_unique<WorldViewportMouseMoveGesture>(
		this->cortex->get_autonomaus().get_input_manager(),
		this->target_screen_space_point,
		camera);
	if (this->gesture->play() != Gesture::success)
	{
		Log::warn(
			"(WorldCortex::MoveMouseAction) Could not move mouse to {:1f}, {:0f}, {:1f}.",
			position.x / thoreau::TILE_SIZE,
			position.y,
			position.z / thoreau::TILE_SIZE);
		this->state = state_failure;
		return;
	}
}

autonomaus::WorldCortex::HoverAction::HoverAction(
	WorldCortex* cortex,
	const thoreau::TileCoordinate& tile_coordinate) :
		MoveMouseAction(cortex)
{
	this->tile_coordinate = tile_coordinate;
	compute_world_position();
}

glm::vec3 autonomaus::WorldCortex::HoverAction::get_world_position()
{
	return this->world_position;
}

void autonomaus::WorldCortex::HoverAction::compute_world_position()
{
	auto& map = this->cortex->get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(this->tile_coordinate))
	{
		Log::warn("(WorldCortex::HoverAction) Map doesn't have tile ({}, {}; {}).",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		return;
	}

	auto& tile = map.get_tile(this->tile_coordinate);
	if (!tile.has_elevation())
	{
		Log::warn("(WorldCortex::HoverAction) Tile ({}, {}; {}) doesn't have elevation data.",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		return;
	}

	this->world_position.x = (this->tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	this->world_position.y = RNG::nextf(tile.get_min_elevation(), tile.get_max_elevation());
	this->world_position.z = (this->tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;
}

autonomaus::WorldCortex::WalkHereAction::WalkHereAction(
	WorldCortex* cortex,
	const thoreau::TileCoordinate& tile_coordinate,
	bool left_click_only) :
		MoveMouseAction(cortex)
{
	this->tile_coordinate = tile_coordinate;
	this->left_click_only = left_click_only;

	compute_world_position();
}

autonomaus::WorldCortex::WalkHereAction::~WalkHereAction()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

void autonomaus::WorldCortex::WalkHereAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	MoveMouseAction::cancel();
}

glm::vec3 autonomaus::WorldCortex::WalkHereAction::get_world_position()
{
	return this->world_position;
}

autonomaus::Cortex::Result::Status autonomaus::WorldCortex::WalkHereAction::on_success()
{
	switch (this->state)
	{
		case state_start:
			{
				auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
				if (tool_tip == nullptr)
				{
					Log::warn("(WorldCortex::WalkHereAction) Tool tip cortex not valid. Bad logic!");
					this->state = state_failure;
					break;
				}

				auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
				if (right_click_menu == nullptr)
				{
					Log::warn("(WorldCortex::WalkHereAction) Right click menu cortex not valid. Bad logic!");
					this->state = state_failure;
					break;
				}

				if (tool_tip->is_visible())
				{
					if (this->left_click_only)
					{
						Log::info("(WorldCortex::WalkHereAction) Tool tip visible and flag 'left_click_only' set. Canceling...");
						this->state = state_failure;
					}
					else
					{
						this->current_result = right_click_menu->select("Walk here");
						this->state = state_click;
					}
				}
				else
				{
					MotionSequence sequence(*this->cortex);
					sequence.left_click();

					this->current_result = this->cortex->perform(sequence);
					this->state = state_click;
				}
			}
			break;
		case state_click:
			if (!this->current_result.pending())
			{
				return this->current_result.get();
			}
			break;
		case state_failure:
			return Result::Status::failure;
		default:
			Log::warn("(WorldCortex::WalkHereAction) Unhandled state {}. Bad logic!", this->state);
			return Result::Status::failure;
	}

	return Result::Status::working;
}

void autonomaus::WorldCortex::WalkHereAction::compute_world_position()
{
	auto& map = this->cortex->get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(this->tile_coordinate))
	{
		Log::warn("(WorldCortex::WalkHereAction) Map doesn't have tile ({}, {}; {}).",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_failure;
	}

	auto& tile = map.get_tile(this->tile_coordinate);

	if (!tile.has_elevation())
	{
		Log::warn("(WorldCortex::WalkHereAction) Tile ({}, {}; {}) doesn't have elevation data.",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_failure;
	}

	this->world_position.x = (this->tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	this->world_position.y = RNG::nextf(tile.get_min_elevation(), tile.get_max_elevation());
	this->world_position.z = (this->tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;
}

autonomaus::WorldCortex::PokePositionAction::PokePositionAction(
	WorldCortex* cortex,
	const std::vector<std::string>& options) :
		MoveMouseAction(cortex)
{
	this->options = options;
}

autonomaus::WorldCortex::PokePositionAction::~PokePositionAction()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

void autonomaus::WorldCortex::PokePositionAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	MoveMouseAction::cancel();
}

autonomaus::Cortex::Result::Status
autonomaus::WorldCortex::PokePositionAction::on_success()
{
	auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
	if (tool_tip == nullptr)
	{
		Log::warn("(WorldCortex::PokePositionAction) Tool tip cortex not valid. Bad logic!");
		this->state = state_failure;
	}

	switch (this->state)
	{
		case state_start:
			this->target_time = get_current_time() + Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
			this->state = state_wait;
			break;
		case state_wait:
			if (tool_tip->is_visible() || get_current_time() > this->target_time)
			{
				this->state = state_select;
			}
			break;
		case state_select:
			{
				if (!this->hidden)
				{
					if (!tool_tip->is_visible())
					{
						Log::warn("(WorldCortex::PokePositionAction) Tool tip not visible. Nothing under cursor.");
						this->state = state_failure;
						break;
					}

					if (tool_tip->match_any(this->options))
					{
						MotionSequence sequence(*this->cortex);
						sequence.left_click();

						this->current_result = this->cortex->perform(sequence);
						this->state = state_click;
						break;
					}
				}

				auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
				if (right_click_menu == nullptr)
				{
					Log::warn("(WorldCortex::PokePositionAction) Right click menu cortex not valid. Bad logic!");
					this->state = state_failure;
					break;
				}

				this->current_result = right_click_menu->select_any(this->options);
				this->state = state_click;
			}
			break;
		case state_click:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->click_frame = this->cortex->get_autonomaus().get_current_frame() + 1;
					this->state = state_after_click;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_after_click:
			if (this->cortex->get_autonomaus().get_game_state().get_last_update_frame() > this->click_frame)
			{
				if (this->cortex->get_last_click_frame() >= this->click_frame &&
					this->cortex->get_last_click_type() != click_poke)
				{
					Log::warn("(WorldCortex::PokePositionAction) Clicked, but failed to poke!");
					this->state = state_failure;
				}
				else
				{
					this->state = state_success;
				}
			}
			break;
		case state_failure:
			return Result::Status::failure;
		case state_success:
			return Result::Status::success;
		default:
			Log::warn("(WorldCortex::PokePositionAction) Unhandled state {}. Bad logic!", this->state);
			return Result::Status::failure;
	}

	return Result::Status::working;
}

autonomaus::WorldCortex::PokeObjectAction::PokeObjectAction(
		WorldCortex* cortex,
		const std::vector<std::string>& actions,
		const thoreau::TileCoordinate& tile_coordinate,
		const glm::vec3& spray_offset,
		int spray_count,
		float spray_radius) :
			PokePositionAction(cortex, actions)
{
	this->tile_coordinate = tile_coordinate;
	this->spray_offset = spray_offset;
	this->spray_count = spray_count;
	this->spray_radius = spray_radius;

	perform_query();
}

glm::vec3 autonomaus::WorldCortex::PokeObjectAction::get_world_position()
{
	return this->world_position;
}

autonomaus::Cortex::Result::Status
autonomaus::WorldCortex::PokeObjectAction::on_start()
{
	switch (this->state)
	{
		case state_start:
			{
				auto status = this->query.wait_for(std::chrono::seconds(0));
				if (status == std::future_status::ready)
				{
					auto result = this->query.get();
					auto& world = *this->cortex->get_autonomaus().get_world_state();

					std::vector<glm::vec3> candidates;
					for (auto& hit: result)
					{
						int draw_type = world.get_draw_type(hit.first);
						if (draw_type == World::draw_type_scenery ||
							draw_type == World::draw_type_prop)
						{
							candidates.push_back(hit.second);
						}
					}

					if (candidates.empty())
					{
						Log::warn("(WorldCortex::PokeObjectAction) No scenery/prop collisions on tile ({}, {}; {}).",
							this->tile_coordinate.x,
							this->tile_coordinate.y,
							this->tile_coordinate.layer);
					}
					else
					{
						Log::info("WorldCortex::PokeObjectAction) {} possible points.", candidates.size());
						this->world_position = candidates[RNG::next(candidates.size())];
					}

					this->state = state_success;
				}
			}
			break;
		case state_success:
			return Result::Status::success;
		case state_failure:
			return Result::Status::failure;
		default:
			Log::warn("(WorldCortex::PokeObjectAction) Unhandled state {}. Bad logic!", this->state);
			return Result::Status::failure;
	}

	return Result::Status::working;
}

void autonomaus::WorldCortex::PokeObjectAction::perform_query()
{
	auto& map = this->cortex->get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(this->tile_coordinate))
	{
		Log::warn("(WorldCortex::PokeObjectAction) Map doesn't have tile ({}, {}; {}).",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_failure;
		return;
	}

	auto& tile = map.get_tile(this->tile_coordinate);
	if (!tile.has_elevation())
	{
		Log::warn("(WorldCortex::PokeObjectAction) Tile ({}, {}; {}) doesn't have elevation data.",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_failure;
		return;
	}

	this->world_position.x = (this->tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	this->world_position.y = RNG::nextf(tile.get_min_elevation(), tile.get_max_elevation());
	this->world_position.z = (this->tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	auto& game_state = this->cortex->get_autonomaus().get_game_state();
	auto& world = this->cortex->get_autonomaus().get_world_state();
	this->query = std::move(WorldProvider::test_spray(
		game_state,
		world,
		this->world_position + this->spray_offset,
		spray_radius,
		spray_count));
}

autonomaus::WorldCortex::PokeNPCAction::PokeNPCAction(
	WorldCortex* cortex,
	const std::vector<std::string>& options,
	int id,
	bool hidden) :
		PokePositionAction(cortex, options)
{
	this->id = id;
	this->hidden = hidden;
}

glm::vec3 autonomaus::WorldCortex::PokeNPCAction::get_world_position()
{
	auto npcs = *this->cortex->get_autonomaus().get_npc_state();

	if (npcs.has(this->id))
	{
		auto& npc = npcs.get(this->id);
		if (npc.get_is_on_screen())
		{
			auto center = glm::vec4(npc.get_bounds().compute_center(), 1.0f);
			auto transformed_center = npc.get_model_matrix() * center;
			this->world_position = glm::vec3(transformed_center);
		}
		else
		{
			Log::warn("(WorldCortex::PokeNPCAction) NPC (id: {}) no longer visible.", this->id);
			cancel();
		}
	}
	else
	{
		Log::warn("(WorldCortex::PokeNPCAction) NPC (id: {}) no longer exists.", this->id);
		cancel();
	}

	return this->world_position;
}

bool autonomaus::WorldCortex::PokeNPCAction::is_point_stale(const glm::vec2& point)
{
	auto npcs = *this->cortex->get_autonomaus().get_npc_state();

	if (npcs.has(this->id))
	{
		auto& npc = npcs.get(this->id);
		if (npc.get_is_on_screen())
		{
			return !is_point_in_bounds(point, npc.get_bounds(), npc.get_model_matrix());
		}
		else
		{
			Log::warn("(WorldCortex::PokeNPCAction) NPC (id: {}) no longer visible.", this->id);
			cancel();
		}
	}
	else
	{
		Log::warn("(WorldCortex::PokeNPCAction) NPC (id: {}) no longer exists.", this->id);
		cancel();
	}

	// It'll terminate anyway.
	return false;
}

autonomaus::WorldCortex::TakeItemAction::TakeItemAction(
	WorldCortex* cortex,
	const thoreau::TileCoordinate& tile_coordinate,
	const std::string& item) :
		PokePositionAction(cortex, { std::string("%take.*") +  item })
{
	this->tile_coordinate = tile_coordinate;

	compute_world_position();
}

glm::vec3 autonomaus::WorldCortex::TakeItemAction::get_world_position()
{
	return this->world_position;
}

void autonomaus::WorldCortex::TakeItemAction::compute_world_position()
{
	auto& map = this->cortex->get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(this->tile_coordinate))
	{
		Log::warn("(WorldCortex::TakeItemAction) Map doesn't have tile ({}, {}; {}).",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		return;
	}

	auto& tile = map.get_tile(this->tile_coordinate);

	if (!tile.has_elevation())
	{
		Log::warn("(WorldCortex::TakeItemAction) Tile ({}, {}; {}) doesn't have elevation data.",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		return;
	}

	this->world_position.x = (this->tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	this->world_position.y = RNG::nextf(tile.get_min_elevation(), tile.get_max_elevation());
	this->world_position.z = (this->tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	// This handles tables.
	if (tile.is_impassable())
	{
		this->world_position.y += tile.get_height();
	}
}

autonomaus::WorldCortex::OpenDoorAction::OpenDoorAction(
	WorldCortex* cortex,
	const thoreau::TileCoordinate& tile_coordinate,
	bool peek_only) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->tile_coordinate = tile_coordinate;
	this->peek_only = peek_only;

	perform_query();
}

autonomaus::WorldCortex::OpenDoorAction::~OpenDoorAction()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	this->final_result.terminate();
}

autonomaus::Cortex::Result autonomaus::WorldCortex::OpenDoorAction::get_result()
{
	return this->final_result;
}

void autonomaus::WorldCortex::OpenDoorAction::update()
{
	switch (this->state)
	{
		case state_start:
			{
				auto status = this->query.wait_for(std::chrono::seconds(0));
				if (status == std::future_status::ready)
				{
					auto result = this->query.get();

					if (result.empty())
					{
						Log::info("(WorldCortex::OpenDoorAction) Door is open.");
						this->state = state_success;
						break;
					}
					else
					{
						Log::info("(WorldCortex::OpenDoorAction) Door is closed.");
						if (this->peek_only)
						{
							this->state = state_failure;
							break;
						}

						this->current_result = this->cortex->poke_nearby_object(
							this->tile_coordinate,
							"internal.door",
							"open",
							glm::vec3((this->offset_x - 0.5f) * thoreau::TILE_SIZE, 0.0f, (this->offset_y - 0.5f) * thoreau::TILE_SIZE));
						this->state = state_open;
					}
				}
			}
			break;
		case state_open:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_success;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_failure:
			cancel();
			this->state = state_done;
			break;
		case state_success:
			*this->status = Result::Status::success;
			this->state = state_done;
			break;
		case state_done:
			// Nothing.
		default:
			Log::warn("(WorldCortex::OpenDoorAction) Unhandled state {}. Bad logic!", this->state);
			cancel();
			break;
	}
}

void autonomaus::WorldCortex::OpenDoorAction::cancel()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

void autonomaus::WorldCortex::OpenDoorAction::perform_query()
{	auto& map = this->cortex->get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(this->tile_coordinate))
	{
		Log::warn("(WorldCortex::OpenDoorAction) Map doesn't have tile ({}, {}; {}).",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_failure;
		return;
	}

	auto& tile = map.get_tile(this->tile_coordinate);
	if (!tile.is_door())
	{
		Log::info("(WorldCortex::OpenDoorAction) Tile ({}, {}; {}) isn't a door.",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_success;
		return;
	}

	if (!tile.has_elevation())
	{
		Log::warn("(WorldCortex::OpenDoorAction) Tile ({}, {}; {}) doesn't have elevation data.",
			this->tile_coordinate.x,
			this->tile_coordinate.y,
			this->tile_coordinate.layer);
		this->state = state_failure;
		return;
	}

	if (tile.get_flags() & thoreau::Tile::flag_door_left)
	{
		this->offset_x = 0.0f;
	}
	else if (tile.get_flags() & thoreau::Tile::flag_door_right)
	{
		this->offset_x = 1.0f;
	}
	else
	{
		this->offset_x = 0.5f;
	}

	if (tile.get_flags() & thoreau::Tile::flag_door_bottom)
	{
		this->offset_y = 0.0f;
	}
	else if (tile.get_flags() & thoreau::Tile::flag_door_top)
	{
		this->offset_y = 1.0f;
	}
	else
	{
		this->offset_y = 0.5f;
	}

	static const float SIZE_X = Config::get<float>("game", "WorldCortex", "DOOR_OPEN_HALF_X");
	static const float SIZE_Y = Config::get<float>("game", "WorldCortex", "DOOR_OPEN_HALF_Y");
	static const float SIZE_Z = Config::get<float>("game", "WorldCortex", "DOOR_OPEN_HALF_Z");
	static const glm::vec3 SIZE = glm::vec3(SIZE_X, SIZE_Y, SIZE_Z);
	static const float OFFSET_Y = Config::get<float>("game", "WorldCortex", "DOOR_OPEN_OFFSET_Y");

	glm::vec3 world_position;
	world_position.x = (this->tile_coordinate.x + this->offset_x) * thoreau::TILE_SIZE;
	world_position.y = tile.get_max_elevation() + OFFSET_Y;
	world_position.z = (this->tile_coordinate.y + this->offset_y) * thoreau::TILE_SIZE;

	kvlc::Bounds bounds(world_position, world_position);
	bounds.min -= SIZE;
	bounds.max += SIZE;

	auto& game_state = this->cortex->get_autonomaus().get_game_state();
	auto& world = this->cortex->get_autonomaus().get_world_state();
	this->query = std::move(WorldProvider::test_collision(
		game_state,
		world,
		bounds));
}
