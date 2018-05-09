// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "plato/tools/previewTool.hpp"
#include "plato/tools/toolbox.hpp"
#include "plato/tools/toolRenderer.hpp"

plato::PreviewTool::PreviewTool(Toolbox& toolbox) :
	Tool(toolbox)
{
	// Nothing.
}

void plato::PreviewTool::draw(ToolRenderer& renderer)
{
	auto& atlas = get_toolbox().get_atlas();
	auto root = atlas.root();
	auto children = atlas.children(root);
	for (auto& child: children)
	{
		draw(renderer, child);
	}

	mapp::Location hover;
	if (atlas.find(get_toolbox().get_mouse_tile_coordinate(), hover))
	{
		if (hover.shape.get_num_points() != 0)
		{
			if (hover.shape.get_num_points() == 1)
			{
				renderer.draw_circle(hover.shape.get_point(0), 1.5f, glm::vec4(0.5f, 0.5f, 0.5f, 0.1f), ToolRenderer::NO_OUTLINE);
			}
			else
			{
				renderer.draw_map_shape(hover.shape, glm::vec4(0.5f, 0.5f, 0.5f, 0.1f), ToolRenderer::NO_OUTLINE);
			}
		}
	}

	Tool::draw(renderer);
}

void plato::PreviewTool::draw(ToolRenderer& renderer, const mapp::Location& location)
{
	if (location.shape.get_num_points() == 0)
	{
		renderer.draw_rectangle(location.min, location.max, glm::vec4(1));
	}
	else if (location.shape.get_num_points() == 1)
	{
		renderer.draw_circle(location.shape.get_point(0), 1.5f, glm::vec4(1));
	}
	else
	{
		renderer.draw_map_shape(location.shape, glm::vec4(1));
	}

	auto center = location.min;
	center.x += (location.max.x - location.min.x) / 2;
	center.y += (location.max.y - location.min.y) / 2;

	renderer.draw_text(renderer.tile_to_screen(center) + glm::vec2(1.0f, 1.0f), location.resource.get_name(), glm::vec4(0, 0, 0, 1));
	renderer.draw_text(renderer.tile_to_screen(center), location.resource.get_name(), glm::vec4(1));

	auto children = get_toolbox().get_atlas().children(location);
	for (auto& child: children)
	{
		draw(renderer, child);
	}
}
