// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_SELECT_TOOL_HPP
#define PLATO_TOOLS_SELECT_TOOL_HPP

#include <functional>
#include <glm/glm.hpp>
#include "plato/tools/tool.hpp"
#include "mapp/atlas/location.hpp"

namespace plato
{
	class SelectTool : public Tool
	{
	public:
		SelectTool(
			Toolbox& toolbox,
			const std::function<void(mapp::Location)>& on_click);

		void on_mouse_button_down(const principle::MouseEvent& event) override;

		void draw(ToolRenderer& renderer) override;

	private:
		bool find(const mapp::Location& parent, mapp::Location& result);

		std::function<void(mapp::Location)> on_click;
	};
}

#endif
