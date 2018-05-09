// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <glm/glm.hpp>
#include <algorithm>
#include "kvncer/render/gl.hpp"
#include "principle/common/view.hpp"
#include "principle/common/viewManager.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/graphics/display.hpp"

const int principle::ViewManager::MAX_VIEWS;
const int principle::ViewManager::DIVIDER_SIZE;

principle::ViewManager::ViewManager(Display* display, const View* parent)
{
	this->display = display;
	this->parent = parent;

	this->views.fill(nullptr);
}

float principle::ViewManager::get_vertical_divider() const
{
	return this->vertical_divider;
}

void principle::ViewManager::set_vertical_divider(float value)
{
	float length = DIVIDER_SIZE / this->get_size().x;
	this->vertical_divider = glm::clamp(value, length, 1.0f - length);
	perform_layout();
}

void principle::ViewManager::enable_vertical_divider(float value)
{
	if (!get_is_vertical_divider_enabled())
	{
		this->vertical_divider_enabled = true;
		set_vertical_divider(value);
	}
}

void principle::ViewManager::disable_vertical_divider()
{
	this->vertical_divider_enabled = false;
	this->vertical_divider = 0.0f;
	perform_layout();
}

bool principle::ViewManager::get_is_vertical_divider_enabled() const
{
	return this->vertical_divider_enabled;
}

float principle::ViewManager::get_horizontal_divider() const
{
	return this->horizontal_divider;
}

void principle::ViewManager::set_horizontal_divider(float value)
{
	float length = DIVIDER_SIZE / this->get_size().y;
	this->horizontal_divider = glm::clamp(value, length, 1.0f - length);
	perform_layout();
}

void principle::ViewManager::enable_horizontal_divider(float value)
{
	if (!get_is_horizontal_divider_enabled())
	{
		this->horizontal_divider_enabled = true;
		set_horizontal_divider(value);
	}
}

void principle::ViewManager::disable_horizontal_divider()
{
	this->horizontal_divider_enabled = false;
	this->horizontal_divider = 0.0f;
	perform_layout();
}

bool principle::ViewManager::get_is_horizontal_divider_enabled() const
{
	return this->horizontal_divider_enabled;
}

glm::vec2 principle::ViewManager::get_position() const
{
	if (this->parent == nullptr)
	{
		return glm::vec2(0.0f, 0.0f);
	}
	else
	{
		return View::get_position();
	}
}

void principle::ViewManager::set_position(const glm::vec2& value)
{
	if (this->parent != nullptr)
	{
		View::set_position(value);
	}
}

glm::vec2 principle::ViewManager::get_size() const
{
	if (this->parent == nullptr)
	{
		return glm::vec2(
			this->display->get_width(),
			this->display->get_height());
	}
	else
	{
		return View::get_size();
	}
}

void principle::ViewManager::set_size(const glm::vec2& value)
{
	if (this->parent != nullptr)
	{
		View::set_size(value);
	}
}

void principle::ViewManager::perform_layout()
{
	auto position = get_position();
	auto size = get_size();

	auto anchor = glm::vec2(this->vertical_divider, this->horizontal_divider);
	size -= anchor * (float)DIVIDER_SIZE;

	if (get_is_vertical_divider_enabled() && get_is_vertical_divider_enabled())
	{
		layout_view(
			0,
			position + glm::vec2(0.0f, size.y * anchor.y + DIVIDER_SIZE),
			glm::vec2(anchor.x * size.x, (1.0f - anchor.y) * size.y));
		layout_view(
			1,
			position + size * anchor + glm::vec2(DIVIDER_SIZE),
			glm::vec2((1.0f - anchor.x) * size.x, (1.0f - anchor.y) * size.y));
		layout_view(
			2,
			position,
			anchor * size);
		layout_view(
			3,
			position + glm::vec2(anchor.x * size.x + DIVIDER_SIZE, 0.0f),
			glm::vec2((1.0f - anchor.x) * size.x, anchor.y * size.y));
	}
	else if (get_is_vertical_divider_enabled())
	{
		layout_view(
			0,
			position,
			anchor * size);
		layout_view(
			1,
			position + glm::vec2(anchor.x * size.x + DIVIDER_SIZE, 0.0f),
			glm::vec2((1.0f - anchor.x) * size.x, size.y));
	}
	else if (get_is_horizontal_divider_enabled())
	{
		layout_view(
			0,
			position + anchor * size + glm::vec2(0, DIVIDER_SIZE),
			glm::vec2(size.x, (1.0f - anchor.y) * size.y));
		layout_view(
			1,
			position,
			glm::vec2(size.x, anchor.y * size.y));
	}
	else
	{
		layout_view(0, position, size);
	}
}

void principle::ViewManager::update(float delta)
{
	for (auto view: *this)
	{
		if (view == nullptr)
		{
			continue;
		}

		view->update(delta);
	}
}

void principle::ViewManager::draw()
{
	if (get_is_vertical_divider_enabled() && get_is_vertical_divider_enabled())
	{
		draw_view(0);
		draw_view(1);
		draw_view(2);
		draw_view(3);
	}
	else if (get_is_vertical_divider_enabled() || get_is_horizontal_divider_enabled())
	{
		draw_view(0);
		draw_view(1);
	}
	else
	{
		draw_view(0);
	}
}

void principle::ViewManager::on_mouse_button_down(const MouseEvent& event)
{
	auto position = event.get_position();
	position.y = this->display->get_height() - position.y;

	auto anchor = glm::vec2(this->vertical_divider, this->horizontal_divider);
	anchor *= get_size();
	anchor += get_position();

	bool capture_input = false;
	if (event.get_button() == 2)
	{
		float border = DIVIDER_SIZE * 4;
		bool is_near_bottom = position.y >= get_position().y &&
			position.y <= get_position().y + border;
		bool is_near_top = position.y <= get_position().y + get_size().y &&
			position.y >= get_position().y + get_size().y - border;
		bool is_near_left = position.x >= get_position().x &&
			position.x <= get_position().x + border;
		bool is_near_right = position.x <= get_position().x + get_size().x &&
			position.x >= get_position().x + get_size().x - border;

		if (!get_is_vertical_divider_enabled() && (is_near_top || is_near_bottom))
		{
			enable_vertical_divider((position.x - get_position().x) / get_size().x);
			this->is_dragging_vertical_divider = true;
			this->vertical_divider_drag_button = event.get_button();
			capture_input = true;
		}

		if (!get_is_horizontal_divider_enabled() && (is_near_left || is_near_right))
		{
			enable_horizontal_divider((position.y - get_position().y) / get_size().y);
			this->is_dragging_horizontal_divider = true;
			this->horizontal_divider_drag_button = event.get_button();
			capture_input = true;
		}
	}
	else
	{
		if (std::fabs(position.x - anchor.x) <= DIVIDER_SIZE)
		{
			if (event.get_button() == 1)
			{
				this->is_dragging_vertical_divider = get_is_vertical_divider_enabled();
				this->vertical_divider_drag_button = event.get_button();
			}
			else if (event.get_button() == 3)
			{
				disable_vertical_divider();
			}
			capture_input = true;
		}

		if (std::fabs(position.y - anchor.y) <= DIVIDER_SIZE)
		{
			if (event.get_button() == 1)
			{
				this->is_dragging_horizontal_divider = get_is_horizontal_divider_enabled();
				this->horizontal_divider_drag_button = event.get_button();
			}
			else if (event.get_button() == 3)
			{
				disable_horizontal_divider();
			}
			capture_input = true;
		}
	}

	if (!capture_input)
	{
		auto view = this->views[this->active_view];
		if (view != nullptr && this->is_view_active)
		{
			++this->view_active_input_count;
			view->on_mouse_button_down(event);
		}
	}
}

void principle::ViewManager::on_mouse_button_up(const MouseEvent& event)
{
	if (this->is_dragging_vertical_divider &&
		this->vertical_divider_drag_button == event.get_button())
	{
		this->is_dragging_vertical_divider = false;
	}

	if (this->is_dragging_horizontal_divider &&
		this->horizontal_divider_drag_button == event.get_button())
	{
		this->is_dragging_horizontal_divider = false;
	}

	auto view = this->views[this->active_view];
	if (view != nullptr && this->is_view_active)
	{
		--this->view_active_input_count;
		view->on_mouse_button_up(event);
	}
}

void principle::ViewManager::on_mouse_move(const MouseEvent& event)
{
	auto display_size = glm::vec2(display->get_width(), display->get_height());
	auto difference = event.get_difference() / display_size;

	if (this->is_dragging_vertical_divider)
	{
		float value = this->vertical_divider + difference.x;
		set_vertical_divider(value);
	}

	if (this->is_dragging_horizontal_divider)
	{
		float value = this->horizontal_divider - difference.y;
		set_horizontal_divider(value);
	}

	glm::vec2 position = event.get_position();
	position.y = this->display->get_height() - position.y;
	set_active_view(position);
	auto view = this->views[this->active_view];
	if (view != nullptr && this->is_view_active)
	{
		view->on_mouse_move(event);
	}
}

void principle::ViewManager::on_mouse_scroll(const MouseEvent& event)
{
	auto view = this->views[this->active_view];
	if (view != nullptr && this->is_view_active)
	{
		view->on_mouse_scroll(event);
	}
}

void principle::ViewManager::on_key_down(const KeyboardEvent& event)
{
	auto view = this->views[this->active_view];
	if (view != nullptr && this->is_view_active)
	{
		++this->view_active_input_count;
		view->on_key_down(event);
	}
}

void principle::ViewManager::on_key_up(const KeyboardEvent& event)
{
	auto view = this->views[this->active_view];
	if (view != nullptr && this->is_view_active)
	{
		--this->view_active_input_count;
		view->on_key_up(event);
	}
}

bool principle::ViewManager::has_view(int index) const
{
	if (index < 0 || index >= MAX_VIEWS)
	{
		return false;
	}

	return this->views[index] != nullptr;
}

principle::View* principle::ViewManager::get_view(int index) const
{
	if (has_view(index))
	{
		return this->views[index];
	}

	return nullptr;
}

void principle::ViewManager::set_view(int index, View* view)
{
	assert(index >= 0 && index < MAX_VIEWS);
	this->views[index] = view;
	if (view != nullptr)
	{
		perform_layout();
	}
}

void principle::ViewManager::unset_view(int index)
{
	set_view(index, nullptr);
}

void principle::ViewManager::layout_view(
	int index, const glm::vec2& position, const glm::vec2& size)
{
	if (this->views[index] != nullptr)
	{
		this->views[index]->set_position(position);
		this->views[index]->set_size(size);
		this->views[index]->perform_layout();
	}
}

void principle::ViewManager::draw_view(int index)
{
	if (this->views[index] != nullptr)
	{
		this->views[index]->draw();
	}
}

void principle::ViewManager::set_active_view(const glm::vec2& focus_position)
{
	if (this->view_active_input_count != 0)
	{
		return;
	}

	for (int i = 0; i < MAX_VIEWS; ++i)
	{
		if (views[i] == nullptr)
		{
			continue;
		}

		auto bottom_left = views[i]->get_position();
		auto top_right = bottom_left + views[i]->get_size();
		if (focus_position.x >= bottom_left.x && focus_position.x <= top_right.x &&
			focus_position.y >= bottom_left.y && focus_position.y <= top_right.y)
		{
			this->active_view = i;
			this->is_view_active = true;
			return;
		}
	}

	this->is_view_active = false;
}

principle::ViewManager::const_iterator
principle::ViewManager::begin() const
{
	return this->views.begin();
}

principle::ViewManager::const_iterator
principle::ViewManager::end() const
{
	return this->views.end();
}
