// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "plato/tools/selectTool.hpp"
#include "plato/tools/toolbox.hpp"
#include "plato/tools/toolRenderer.hpp"

plato::SelectTool::SelectTool(
	Toolbox& toolbox,
	const std::function<void(mapp::Location location)>& on_click) :
		Tool(toolbox)
{
	this->on_click = on_click;
}

void plato::SelectTool::on_mouse_button_down(const principle::MouseEvent& event)
{
	if (event.get_button() == 1)
	{
		mapp::Location hover;
		if (find(get_toolbox().get_atlas().root(), hover))
		{
			this->on_click(hover);
		}

		get_toolbox().stop(this);
	}
}

void plato::SelectTool::draw(ToolRenderer& renderer)
{
	mapp::Location hover;
	if (find(get_toolbox().get_atlas().root(), hover))
	{
		if (hover.shape.get_num_points() != 0)
		{
			renderer.draw_map_shape(hover.shape, glm::vec4(1.0f, 1.0f, 0.25f, 0.2f), ToolRenderer::NO_OUTLINE);
		}

		auto mouse_position = get_toolbox().get_mouse_position();
		renderer.draw_text(
			mouse_position + glm::vec2(1, 1),
			get_toolbox().get_atlas().address(hover),
			glm::vec4(0, 0, 0, 1));
		renderer.draw_text(
			mouse_position,
			get_toolbox().get_atlas().address(hover),
			glm::vec4(1));
	}

	Tool::draw(renderer);
}

bool plato::SelectTool::find(const mapp::Location& parent, mapp::Location& result)
{
	if (get_toolbox().get_mouse_tile_coordinate().x < parent.min.x ||
		get_toolbox().get_mouse_tile_coordinate().x > parent.max.x ||
		get_toolbox().get_mouse_tile_coordinate().y < parent.min.y ||
		get_toolbox().get_mouse_tile_coordinate().y > parent.max.y)
	{
		return false;
	}

	bool match = false;
	if (parent.shape.get_num_points() == 0)
	{
		result = parent;
		match = true;
	}

	auto children = get_toolbox().get_atlas().children(parent);
	for (auto& child: children)
	{
		if (find(child, result))
		{
			match = true;
			break;
		}
	}

	if (!match)
	{
		result = parent;
	}

	return true;
}
