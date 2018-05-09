// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_COMPLEX_SHAPE_TOOL_HPP
#define PLATO_TOOLS_COMPLEX_SHAPE_TOOL_HPP

#include <functional>
#include <glm/glm.hpp>
#include <stack>
#include "plato/tools/tool.hpp"
#include "thoreau/shape.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace plato
{
	class ComplexShapeTool : public Tool
	{
	public:
		ComplexShapeTool(
			Toolbox& toolbox,
			const std::function<void(thoreau::Shape)>& callback);

		void on_mouse_button_down(const principle::MouseEvent& event) override;
		void on_key_down(const principle::KeyboardEvent& event) override;

		void draw(ToolRenderer& renderer) override;

	private:
		std::stack<thoreau::Shape> shapes;
		std::function<void(thoreau::Shape)> callback;
	};
}

#endif
