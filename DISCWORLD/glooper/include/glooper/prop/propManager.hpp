// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_PROP_PROP_MANAGER_HPP
#define GLOOPER_PROP_PROP_MANAGER_HPP

#include <string>
#include <map>
#include "glooper/prop/props.hpp"
#include "glooper/prop/propDefinition.hpp"
#include "glooper/prop/detail/propQuery.hpp"

namespace glooper
{
	class PropManager : public ResourceListener
	{
	public:
		PropManager(
			ResourceManager& resource_manager,
			Props& props);
		PropManager(const PropManager& other) = delete;
		~PropManager();

		PropDefinition add_definition(const std::string& name);
		void remove_definition(const std::string& name);
		void remove_definition(const PropDefinition& definition);
		bool has_definition(const std::string& name) const;
		bool has_definition(const PropDefinition& definition) const;

		void on_reload_resources() override;

		void update(
			const chomsky::StaticWorldUpdateBlock::DirtyTileState& dirty_tile_state,
			const chomsky::StaticWorldUpdateBlock& static_block);

		PropManager& operator =(const PropManager& other) = delete;

	private:
		ResourceManager* resource_manager;
		Props* props;

		bool is_dirty = false;
		std::map<std::string, PropDefinition> definitions;
		std::map<std::string, PropQuery> queries;
		void execute_queries(
			const chomsky::StaticWorldUpdateBlock::DirtyTileState& dirty_tile_state,
			const chomsky::StaticWorldUpdateBlock& static_block);
		void prune_definitions();
	};
}

#endif
