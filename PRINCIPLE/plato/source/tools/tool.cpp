// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "principle/graphics/guiManager.hpp"
#include "plato/tools/tool.hpp"

plato::Tool::Tool(Toolbox& toolbox)
{
	this->toolbox = &toolbox;
}

void plato::Tool::on_mouse_move(const principle::MouseEvent& event)
{
	// Nothing.
}

void plato::Tool::on_mouse_button_down(const principle::MouseEvent& event)
{
	// Nothing.
}

void plato::Tool::on_mouse_button_up(const principle::MouseEvent& event)
{
	// Nothing.
}

void plato::Tool::on_key_down(const principle::KeyboardEvent& event)
{
	// Nothing.
}

void plato::Tool::on_key_up(const principle::KeyboardEvent& event)
{
	// Nothing.
}

void plato::Tool::on_cancel()
{
	// Nothing.
}

void plato::Tool::draw(ToolRenderer& renderer)
{
	// Nothing.
}

plato::Toolbox& plato::Tool::get_toolbox() const
{
	return *this->toolbox;
}
