// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "plato/tools/tool.hpp"
#include "plato/tools/toolbox.hpp"

plato::Toolbox::Toolbox(
	ToolFactory& tool_factory,
	const thoreau::Map& map,
	twoflower::Brochure& brochure,
	mapp::Atlas& atlas)
{
	this->tool_factory = &tool_factory;
	this->map = &map;
	this->brochure = &brochure;
	this->atlas = &atlas;
}

plato::Toolbox::~Toolbox()
{
	for (auto tool: this->tools)
	{
		delete tool;
	}
}

plato::ToolFactory& plato::Toolbox::get_factory() const
{
	return *this->tool_factory;
}

int plato::Toolbox::get_current_layer() const
{
	return this->current_layer;
}

void plato::Toolbox::set_current_layer(int value)
{
	this->current_layer = value;
}

const thoreau::TileCoordinate& plato::Toolbox::get_mouse_tile_coordinate() const
{
	return this->current_mouse_tile_coordinate;
}

void plato::Toolbox::set_mouse_tile_coordinate(const thoreau::TileCoordinate& value)
{
	this->current_mouse_tile_coordinate = value;
}

glm::vec2 plato::Toolbox::get_mouse_position() const
{
	return this->mouse_position;
}

const thoreau::Map& plato::Toolbox::get_map() const
{
	return *this->map;
}

const twoflower::Brochure& plato::Toolbox::get_brochure() const
{
	return *this->brochure;
}

const mapp::Atlas& plato::Toolbox::get_atlas() const
{
	return *this->atlas;
}

void plato::Toolbox::use(Tool* tool)
{
	this->tools.insert(tool);
	if (this->current_tool != nullptr && tool != this->current_tool)
	{
		this->current_tool->on_cancel();
	}
	this->current_tool = tool;
}

void plato::Toolbox::stop(Tool* tool)
{
	this->tools.erase(tool);
	if (this->current_tool == tool)
	{
		this->current_tool = nullptr;
	}

	delete tool;
}

bool plato::Toolbox::active() const
{
	return this->current_tool != nullptr;
}

void plato::Toolbox::on_mouse_move(const principle::MouseEvent& event)
{
	if (this->current_tool != nullptr)
	{
		this->current_tool->on_mouse_move(event);
	}
}

void plato::Toolbox::on_mouse_button_down(const principle::MouseEvent& event)
{
	if (this->current_tool != nullptr)
	{
		this->current_tool->on_mouse_button_down(event);
	}
}

void plato::Toolbox::on_mouse_button_up(const principle::MouseEvent& event)
{
	if (this->current_tool != nullptr)
	{
		this->current_tool->on_mouse_button_up(event);
	}
}

void plato::Toolbox::on_key_down(const principle::KeyboardEvent& event)
{
	if (this->current_tool != nullptr)
	{
		this->current_tool->on_key_down(event);
	}
}

void plato::Toolbox::on_key_up(const principle::KeyboardEvent& event)
{
	if (this->current_tool != nullptr)
	{
		this->current_tool->on_key_up(event);
	}
}

void plato::Toolbox::draw(ToolRenderer& renderer)
{
	if (this->current_tool != nullptr)
	{
		this->current_tool->draw(renderer);
	}
}
