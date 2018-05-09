// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_TOOLBOX_HPP
#define PLATO_TOOLS_TOOLBOX_HPP

#include <glm/glm.hpp>
#include <set>
#include "mapp/atlas/atlas.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "thoreau/map.hpp"
#include "twoflower/brochure.hpp"

namespace plato
{
	class Tool;
	class ToolRenderer;
	class ToolFactory;

	class Toolbox
	{
	public:
		Toolbox(
			ToolFactory& tool_factory,
			const thoreau::Map& map,
			twoflower::Brochure& brochure,
			mapp::Atlas& atlas);
		~Toolbox();

		ToolFactory& get_factory() const;

		int get_current_layer() const;
		void set_current_layer(int value);

		const thoreau::TileCoordinate& get_mouse_tile_coordinate() const;
		void set_mouse_tile_coordinate(const thoreau::TileCoordinate& value);

		glm::vec2 get_mouse_position() const;

		const thoreau::Map& get_map() const;
		const twoflower::Brochure& get_brochure() const;
		const mapp::Atlas& get_atlas() const;

		void use(Tool* tool);
		void stop(Tool* tool);

		bool active() const;

		void on_mouse_move(const principle::MouseEvent& event);
		void on_mouse_button_down(const principle::MouseEvent& event);
		void on_mouse_button_up(const principle::MouseEvent& event);
		void on_key_down(const principle::KeyboardEvent& event);
		void on_key_up(const principle::KeyboardEvent& event);

		void draw(ToolRenderer& renderer);

	private:
		ToolFactory* tool_factory;
		const thoreau::Map* map;
		twoflower::Brochure* brochure;
		mapp::Atlas* atlas;

		int current_layer = 0;
		thoreau::TileCoordinate current_mouse_tile_coordinate;
		glm::vec2 mouse_position;

		glm::mat4 view_matrix, projection_matrix;

		std::set<Tool*> tools;
		Tool* current_tool = nullptr;
	};
}

#endif
