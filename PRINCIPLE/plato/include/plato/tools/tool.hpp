// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_TOOL_HPP
#define PLATO_TOOLS_TOOL_HPP

#include <glm/glm.hpp>
#include "plato/tools/toolbox.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/deps/imgui/imgui.h"
#include "thoreau/tileCoordinate.hpp"

namespace plato
{
	class Toolbox;
	class ToolRenderer;

	class Tool
	{
	public:
		Tool(Toolbox& toolbox);
		virtual ~Tool() = default;

		virtual void on_mouse_move(const principle::MouseEvent& event);
		virtual void on_mouse_button_down(const principle::MouseEvent& event);
		virtual void on_mouse_button_up(const principle::MouseEvent& event);
		virtual void on_key_down(const principle::KeyboardEvent& event);
		virtual void on_key_up(const principle::KeyboardEvent& event);
		virtual void on_cancel();

		virtual void draw(ToolRenderer& renderer);

	protected:
		Toolbox& get_toolbox() const;

	private:
		Toolbox* toolbox;
	};
}

#endif
