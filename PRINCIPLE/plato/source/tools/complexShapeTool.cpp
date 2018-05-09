// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <allegro5/allegro.h>
#include "plato/tools/complexShapeTool.hpp"
#include "plato/tools/toolbox.hpp"
#include "plato/tools/toolRenderer.hpp"

plato::ComplexShapeTool::ComplexShapeTool(
	Toolbox& toolbox,
	const std::function<void(thoreau::Shape)>& callback) :
		Tool(toolbox)
{
	this->callback = callback;
}

void plato::ComplexShapeTool::on_mouse_button_down(
	const principle::MouseEvent& event)
{
	if (event.get_button() != 1)
	{
		return;
	}

	auto current_tile = get_toolbox().get_mouse_tile_coordinate();
	if (this->shapes.empty())
	{
		thoreau::Shape shape(get_toolbox().get_current_layer());
		shape.add_point(current_tile);
		this->shapes.push(shape);
	}
	else
	{
		thoreau::Shape shape = this->shapes.top();
		shape.add_point(current_tile);
		this->shapes.push(shape);
	}
}

void plato::ComplexShapeTool::draw(ToolRenderer& renderer)
{
	renderer.draw_circle(get_toolbox().get_mouse_tile_coordinate(), 2.0f, glm::vec4(0, 1, 0, 1));

	if (!this->shapes.empty())
	{
		auto& top = this->shapes.top();
		for (std::size_t i = 0; i < top.get_num_points() - 1; ++i)
		{
			auto a = top.get_point(i);
			auto b = top.get_point(i + 1);
			renderer.draw_line(a, b, glm::vec4(1));
		}

		for (std::size_t i = 0; i < top.get_num_points(); ++i)
		{
			auto point = top.get_point(i);
			renderer.draw_circle(point, 1.5f, glm::vec4(1), false);
		}

		if (top.get_num_points() > 0)
		{
			auto last_point = top.get_point(top.get_num_points() - 1);
			renderer.draw_line(last_point, get_toolbox().get_mouse_tile_coordinate(), glm::vec4(0, 1, 0, 0.5f), 0.5f);
		}
	}

	Tool::draw(renderer);
}

void plato::ComplexShapeTool::on_key_down(const principle::KeyboardEvent& event)
{
	if (event.get_key_code() == ALLEGRO_KEY_BACKSPACE)
	{
		if (this->shapes.empty())
		{
			get_toolbox().stop(this);
		}
		else
		{
			this->shapes.pop();
		}
	}
	else if (event.get_key_code() == ALLEGRO_KEY_ENTER)
	{
		if (this->shapes.empty())
		{
			thoreau::Shape empty;
			this->callback(empty);
		}
		else
		{
			this->callback(this->shapes.top());
		}

		get_toolbox().stop(this);
	}
}
