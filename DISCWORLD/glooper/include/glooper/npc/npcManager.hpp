// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_NPC_NPC_MANAGER_HPP
#define GLOOPER_NPC_NPC_MANAGER_HPP

#include <map>
#include <set>
#include <glm/glm.hpp>
#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/listeners/frameListener.hpp"
#include "kompvter/listeners/modelListener.hpp"
#include "glooper/map/minimap.hpp"
#include "glooper/npc/npcCabinet.hpp"
#include "glooper/npc/npcProfile.hpp"
#include "glooper/npc/detail/npcQuery.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "glooper/resources/resourceManager.hpp"

namespace glooper
{
	class NPCManager :
		public ResourceListener,
		public kompvter::FrameListener,
		public kompvter::ModelListener
	{
	public:
		NPCManager(
			ResourceManager& resource_manager,
			NPCCabinet& cabinet,
			const Minimap& minimap,
			const chomsky::AnimatedWorldUpdateBlock& animated_block);
		NPCManager(const NPCManager& other) = delete;
		~NPCManager();

		NPCProfile add_profile(const std::string& name);
		void remove_profile(const std::string& name);
		void remove_profile(const NPCProfile& profile);
		bool has_profile(const std::string& name) const;
		bool has_profile(const NPCProfile& profile) const;

		void on_reload_resources() override;
		void on_draw_enabled(float timestamp) override;
		void on_draw_disabled(float timestamp) override;
		void on_draw_model(const kompvter::ModelDrawEvent& event) override;

		NPCManager& operator =(const NPCManager& other) = delete;

	private:
		void update();

		ResourceManager* resource_manager;
		NPCCabinet* cabinet;
		const Minimap* minimap;
		const chomsky::AnimatedWorldUpdateBlock* animated_block;

		float world_distance_threshold;
		float minimap_distance_threshold;
		bool world_distance_below_threshold(float value) const;
		bool minimap_distance_below_threshold(float value) const;

		bool draw_enabled = false;
		float previous_delta = 0.0f;
		float previous_draw = HUGE_VALF;
		float current_draw;
		float get_draw_delta() const;

		glm::mat4 view_matrix, projection_matrix;

		std::map<std::string, NPCProfile> profiles;
		std::map<std::string, NPCQuery> queries;
		std::map<std::string, NPCQuery> internal_queries;
		bool queries_dirty = false;


		typedef std::set<const kvre::Model*> ModelSet;
		typedef std::pair<std::string /* profile */, std::string /* variant */> ModelSetMatch;
		std::map<ModelSet, ModelSetMatch> model_set_matches;
		std::set<ModelSet> checked_model_sets;
		std::set<ModelSet> unvisited_model_sets;
		void update_actors();
		void prepare_actors_update();
		void update_actor(
			const thoreau::WorldCoordinate& location,
			const chomsky::ActorDraw& draw);

		std::map<thoreau::WorldCoordinate, ModelSetMatch> pending_profile_matches;
		std::set<NPC::ID> unvisited_npcs;
		std::set<NPC::ID> visited_npcs;
		void update_positions();
		void prepare_positions_update();
		void update_visible_positions();
		void update_minimap_positions();
		void collapse_minimap_positions();

		void update_animations();
	};
}

#endif
