// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_TOOL_FACTORY_HPP
#define PLATO_TOOLS_TOOL_FACTORY_HPP

#include <glm/glm.hpp>
#include "principle/deps/imgui/imgui.h"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/shape.hpp"

namespace plato
{
	class ToolFactory
	{
	public:
		ToolFactory() = default;
		virtual ~ToolFactory() = default;

		virtual void add_new_location() = 0;
		virtual void add_new_child_location() = 0;
		virtual void add_new_child_location(const mapp::Location& parent) = 0;
		virtual void add_new_waypoint(
			const mapp::Location& parent,
			const thoreau::TileCoordinate& tile_coordinate) = 0;
		virtual void add_new_link(const mapp::Location& from) = 0;
		virtual void find_location(
			const std::function<void(mapp::Location)>& callback) = 0;
		virtual void find_resource(
			const std::string& type,
			const std::function<void(twoflower::Resource)>& callback) = 0;

		virtual void view(const twoflower::Resource& resource) = 0;
		virtual void view(const twoflower::Action& action) = 0;
		virtual void edit(const twoflower::Resource& resource) = 0;
		virtual void edit(const twoflower::Action& action) = 0;
	};
}

#endif
