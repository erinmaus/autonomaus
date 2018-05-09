// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_WORLD_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_WORLD_CORTEX_HPP

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "autonomaus/state/memory/cortex.hpp"
#include "chomsky/gui/guiQuery.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace autonomaus
{
	class WorldCortex : public Cortex
	{
	public:
		WorldCortex(Autonomaus& autonomaus);
		~WorldCortex() = default;

		Result find_object(const std::string& name);
		Result find_npc(const std::string& name);

		std::vector<thoreau::TileCoordinate> get_nearby_objects(const std::string& name, bool visible = false) const;
		std::vector<int> get_nearby_npcs(const std::string& name, bool visible = true) const;
		std::vector<thoreau::TileCoordinate> get_nearby_items(const std::string& name) const;

		std::vector<std::string> get_npc_animations(int id) const;

		Result poke_nearby_object(
			const thoreau::TileCoordinate& tile_coordinate,
			const std::string& name,
			const std::string& action,
			const glm::vec3& offset = glm::vec3(0.0f));
		Result poke_nearby_npc(
			int id,
			const std::string& action);
		Result take_nearby_item(
			const thoreau::TileCoordinate& tile_coordinate,
			const std::string& item);

		Result walk_here(
			const thoreau::TileCoordinate& tile_coordinate,
			bool left_click_only);

		Result hover(const thoreau::TileCoordinate& tile_coordinate);

		Result open_door(const thoreau::TileCoordinate& tile_coordinate);
		Result peek_door(const thoreau::TileCoordinate& tile_coordinate);

		Result ascend(const thoreau::TileCoordinate& tile_coordinate);
		Result descend(const thoreau::TileCoordinate& tile_coordinate);

		enum
		{
			click_none = 0,
			click_walk,
			click_poke
		};
		int get_last_click_type() const;
		int get_last_click_frame() const;
		float get_last_click_timestamp() const;

		void update() override;

	private:
		std::vector<thoreau::TileCoordinate> get_nearby_visible_objects(
			const std::string& name) const;
		std::vector<thoreau::TileCoordinate> get_nearby_invisible_objects(
			const std::string& name) const;

		chomsky::GUIQuery click_query;
		int last_click = click_none;
		int last_click_frame = 0;
		float last_click_timestamp = 0.0f;

		struct Object
		{
			std::string name;
			float last_seen = 0.0f;
			bool visible = false;
		};
		typedef std::unordered_map<std::string, Object> Objects;
		std::unordered_map<thoreau::TileCoordinate, Objects> objects_by_tile;

		YAML::Node object_meta;
		YAML::Node npc_meta;
		void load_meta();

		std::vector<std::string> build_actions(
			const YAML::Node& meta,
			const std::string& name,
			const std::string& action);

		struct MoveMouseAction;
		struct HoverAction;
		struct WalkHereAction;
		struct PokePositionAction;
		struct PokeObjectAction;
		struct PokeNPCAction;
		struct TakeItemAction;
		struct OpenDoorAction;
	};
}

#endif
