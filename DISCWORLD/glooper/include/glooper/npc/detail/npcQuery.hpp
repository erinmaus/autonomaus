// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_NPC_DETAIL_NPC_QUERY_HPP
#define GLOOPER_NPC_DETAIL_NPC_QUERY_HPP

#include <cstdint>
#include <string>
#include <boost/filesystem.hpp>
#include <glm/glm.hpp>
#include "chomsky/world/actorAnimation.hpp"
#include "chomsky/world/actorDraw.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"
#include "chomsky/world/query/materialSetMatch.hpp"
#include "chomsky/world/query/actorTileSpacePointCloudMatchExecutor.hpp"
#include "chomsky/world/query/actorMaterialSetMatchExecutor.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "glooper/resources/resourceManager.hpp"

namespace glooper
{
	class NPCQuery : 
		public ResourceListener
	{
	public:
		NPCQuery(const std::string& name, ResourceManager& resource_manager);
		~NPCQuery();

		bool match(
			const thoreau::WorldCoordinate& location,
			const chomsky::ActorTileSpacePointCloudMatchExecutor& executor,
			std::string& result) const;

		bool match(
			const thoreau::WorldCoordinate& location,
			const chomsky::ActorMaterialSetMatchExecutor& executor,
			std::string& result) const;

		bool match(
			const chomsky::ActorDraw& draw,
			std::vector<std::string>& results) const;

		bool match(
			const thoreau::WorldCoordinate& location,
			const chomsky::ActorTileSpacePointCloudMatchExecutor& executor,
			std::vector<std::string>& results) const;

		void on_reload_resources() override;

	private:
		boost::filesystem::path get_root_path() const;
		void load_models(const boost::filesystem::path& root);
		void load_materials(const boost::filesystem::path& root);
		void load_animations(const boost::filesystem::path& root);
		void load_effect(const std::string& name, const boost::filesystem::path& root);
		void load_animation(const std::string& name, const boost::filesystem::path& root);

		std::string name;
		ResourceManager* resource_manager;

		std::map<std::string, chomsky::TileSpacePointCloudMatch> model_matches;
		std::map<std::string, chomsky::MaterialSetMatch> material_matches;
		std::multimap<std::string, chomsky::TileSpacePointCloudMatch> effect_matches;
		std::multimap<std::string, chomsky::ActorAnimation> animation_matches;
	};
}

#endif
