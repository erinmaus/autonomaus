// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_PROP_DETAIL_PROP_QUERY_HPP
#define GLOOPER_PROP_DETAIL_PROP_QUERY_HPP

#include <cstdint>
#include <string>
#include <boost/filesystem.hpp>
#include <glm/glm.hpp>
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"
#include "chomsky/world/query/materialSetMatch.hpp"
#include "chomsky/world/query/staticTileSpacePointCloudMatchExecutor.hpp"
#include "chomsky/world/query/staticMaterialSetMatchExecutor.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "glooper/resources/resourceManager.hpp"

namespace glooper
{
	class PropQuery : public ResourceListener
	{
	public:
		PropQuery(const std::string& name, ResourceManager& resource_manager);
		~PropQuery();

		const std::string& get_name() const;

		bool match(
			const thoreau::TileCoordinate& location,
			const chomsky::StaticTileSpacePointCloudMatchExecutor& executor) const;

		bool match(
			const thoreau::TileCoordinate& location,
			const chomsky::StaticMaterialSetMatchExecutor& executor) const;

		void on_reload_resources() override;

	private:
		boost::filesystem::path get_root_path() const;
		void load_models(const boost::filesystem::path& root);
		void load_materials(const boost::filesystem::path& root);

		std::string name;
		ResourceManager* resource_manager;

		std::map<std::string, chomsky::TileSpacePointCloudMatch> model_matches;
		std::map<std::string, chomsky::MaterialSetMatch> material_matches;
	};
}

#endif
