// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/log.hpp"
#include "autonomaus/core/config.hpp"
#include "autonomaus/state/memory/guiCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/guiProvider.hpp"
#include "autonomaus/input/inputManager.hpp"

struct autonomaus::GUICortex::PokeAction : public Action
{
	PokeAction(
		GUICortex* cortex,
		const chomsky::GUIContent& content,
		const std::vector<std::string>& actions,
		bool ignore_tool_tip,
		const MotionSequence::Guard& guard);
	~PokeAction();

	Cortex::Result get_result() override;
	void update() override;

	void cancel();

	GUICortex* cortex;
	chomsky::GUIContent content;
	std::vector<std::string> actions;
	bool ignore_tool_tip;
	MotionSequence::Guard guard;

	enum
	{
		state_start,
		state_move,
		state_left_click,
		state_right_click,
		state_select,
		state_done
	};
	int state = state_start;

	Cortex::Result current_result;

	Cortex::Result::StatusReference status;
	Cortex::Result final_result;
};

autonomaus::GUICortex::GUICortex(
	const std::string& name,
	Autonomaus& autonomaus,
	const chomsky::GUIQuery& root,
	const std::string& gui) :
		Cortex(name, autonomaus)
{
	this->gui = gui;
	this->root_query = root;

	on_pull("is_visible", [this](auto)
	{
		Message message;
		message.set_boolean("result", this->is_visible());
		return message;
	});

	on_pull("is_ready", [this](auto)
	{
		Message message;
		message.set_boolean("result", this->is_ready());
		return message;
	});
}

autonomaus::GUICortex::~GUICortex()
{
	// Nothing.
}

bool autonomaus::GUICortex::is_visible() const
{
	return this->valid && !root_content.empty();
}

bool autonomaus::GUICortex::is_ready() const
{
	if (!get_autonomaus().get_gui_state()->has(this->gui))
	{
		return false;
	}

	if (get_autonomaus().get_gui_state()->get(this->gui).empty())
	{
		return false;
	}

	return true;
}

void autonomaus::GUICortex::update()
{
	Cortex::update();

	if (!get_autonomaus().get_gui_state()->has(this->gui))
	{
		this->valid = false;
		return;
	}

	auto& gui_root = get_autonomaus().get_gui_state()->get(this->gui);
	std::vector<chomsky::GUIContent> content;
	if (!this->root_query.collect(gui_root, content) || content.empty())
	{
		this->valid = false;
		return;
	}

	this->root_content = content[0];
	this->valid = true;
}

autonomaus::Cortex::Result autonomaus::GUICortex::poke(
	const chomsky::GUIContent& content,
	const std::string& action,
	bool ignore_tool_tip)
{
	return poke(content, action, ignore_tool_tip, [] { return true; });
}

autonomaus::Cortex::Result autonomaus::GUICortex::poke_any(
	const chomsky::GUIContent& content,
	const std::vector<std::string>& actions,
	bool ignore_tool_tip)
{
	return poke_any(content, actions, ignore_tool_tip, [] { return true; });
}

autonomaus::Cortex::Result autonomaus::GUICortex::poke(
	const chomsky::GUIContent& content,
	const std::string& action,
	bool ignore_tool_tip,
	const MotionSequence::Guard& guard)
{
	return poke_any(content, { action }, ignore_tool_tip, guard);
}

autonomaus::Cortex::Result autonomaus::GUICortex::poke_any(
	const chomsky::GUIContent& content,
	const std::vector<std::string>& actions,
	bool ignore_tool_tip,
	const MotionSequence::Guard& guard)
{
	return perform(new PokeAction(this, content, actions, ignore_tool_tip, guard));
}

glm::ivec2 autonomaus::GUICortex::get_mouse_position() const
{
	auto& input_state = get_autonomaus().get_input_manager().get_state();
	int mouse_x = input_state.get_mouse_x();
	int mouse_y = input_state.get_mouse_y();
	mouse_y = get_autonomaus().get_draw_state().get_gui_viewport().w - mouse_y;

	return glm::ivec2(mouse_x, mouse_y);
}

glm::ivec2 autonomaus::GUICortex::point_to_screen(const glm::ivec2& point) const
{
	glm::ivec2 screen = point;
	screen.y = get_autonomaus().get_draw_state().get_gui_viewport().w - point.y;

	return screen;
}

glm::ivec2 autonomaus::GUICortex::screen_to_point(const glm::ivec2& screen) const
{
	// We're just flipping the point. It's assumed 'screen' is in screen
	// co-ordinates. Why wouldn't it be?
	glm::ivec2 point = screen;
	point.y = get_autonomaus().get_draw_state().get_gui_viewport().w - screen.y;

	return point;
}

bool autonomaus::GUICortex::try_get_global_root(chomsky::GUIContent& content) const
{
	if (!get_autonomaus().get_gui_state()->has(this->gui))
	{
		return false;
	}

	content = get_autonomaus().get_gui_state()->get(this->gui);
	return true;
}

bool autonomaus::GUICortex::try_get_root(chomsky::GUIContent& result) const
{
	if (!is_visible())
	{
		return false;
	}

	result = this->root_content;
	return true;
}

bool autonomaus::GUICortex::try_get_root(
	const chomsky::GUIQuery& query,
	chomsky::GUIContent& result) const
{
	chomsky::GUIContent gui_root;
	if (!try_get_global_root(gui_root))
	{
		return false;
	}

	std::vector<chomsky::GUIContent> content;
	if (!query.collect(gui_root, content) || content.empty())
	{
		return false;
	}

	result = content[0];
	return true;
}

bool autonomaus::GUICortex::try_get_child(
	const chomsky::GUIQuery& query,
	chomsky::GUIContent& result) const
{
	if (!this->valid)
	{
		return false;
	}

	std::vector<chomsky::GUIContent> children;
	if (query.collect(this->root_content, children) && !children.empty())
	{
		result = children[0];
		return true;
	}

	return false;
}

bool autonomaus::GUICortex::try_get_content_under_mouse(
	chomsky::GUIContent& result) const
{
	return try_get_content_under_point(get_mouse_position(), result);
}

bool autonomaus::GUICortex::try_get_content_under_point(
	const glm::ivec2& point,
	chomsky::GUIContent& result) const
{
	if (!this->valid)
	{
		return false;
	}

	return do_try_get_content_under_point(this->root_content, point, result);
}

bool autonomaus::GUICortex::try_get_element_under_mouse(
	chomsky::GUIElement& result) const
{
	return try_get_element_under_point(get_mouse_position(), result);
}

bool autonomaus::GUICortex::try_get_element_under_point(
	const glm::ivec2& point,
	chomsky::GUIElement& result) const
{
	chomsky::GUIContent content;
	if (!try_get_content_under_point(point, content))
	{
		return false;
	}

	if (content.get_num_elements() == 0)
	{
		return false;
	}

	for (std::size_t i = content.get_num_elements(); i > 0; --i)
	{
		auto index = i - 1;
		auto& element = content.get_element(index);

		if (point.x > element.get_x() &&
			point.x < element.get_x() + element.get_width() &&
			point.y > element.get_y() &&
			point.y < element.get_y() + element.get_height())
		{
			result = element;
			return true;
		}
	}

	return false;
}

bool autonomaus::GUICortex::try_get_text(
	const chomsky::GUIContent& content,
	std::string& result) const
{
	auto text = content.get_text().to_string();
	if (text.empty())
	{
		return false;
	}

	result = text;
	return true;
}

bool autonomaus::GUICortex::try_get_data(
	const chomsky::GUIContent& content,
	const std::string& data,
	std::string& result) const
{
	auto& texture_dictionary = get_autonomaus().get_game_state().get_resource_manager().get_texture_dictionary();

	for (std::size_t i = 0; i < content.get_num_elements(); ++i)
	{
		auto& element = content.get_element(i);

		if (!element.get_is_textured())
		{
			continue;
		}

		auto texture_key = element.get_key();
		if (!texture_dictionary.has(texture_key))
		{
			continue;
		}

		auto& entry = texture_dictionary.get(texture_key);
		if (entry.has_data(data))
		{
			result = entry.get_data(data);
			return true;
		}
	}

	return false;
}

bool autonomaus::GUICortex::try_get_name(
	const chomsky::GUIContent& content,
	std::string& result) const
{
	auto& texture_dictionary = get_autonomaus().get_game_state().get_resource_manager().get_texture_dictionary();

	for (std::size_t i = 0; i < content.get_num_elements(); ++i)
	{
		auto& element = content.get_element(i);

		if (!element.get_is_textured())
		{
			continue;
		}

		auto texture_key = element.get_key();
		if (!texture_dictionary.has(texture_key))
		{
			continue;
		}

		auto& entry = texture_dictionary.get(texture_key);
		result = entry.get_value();
		return true;
	}

	return false;
}

bool autonomaus::GUICortex::try_get_item(
	const chomsky::GUIContent& content,
	Memory& result) const
{
	for (std::size_t i = 0; i < content.get_num_elements(); ++i)
	{
		auto& element = content.get_element(i);

		Memory item;
		if (try_get_item(element, item))
		{
			result = item;
			return true;
		}
	}

	return false;
}

bool autonomaus::GUICortex::try_get_item(
	const chomsky::GUIElement& element,
	Memory& memory) const
{
	auto& texture_dictionary = get_autonomaus().get_game_state().get_resource_manager().get_texture_dictionary();
	if (!element.get_is_textured())
	{
		return false;
	}

	auto texture_key = element.get_key();
	if (!texture_dictionary.has(texture_key))
	{
		return false;
	}

	auto& entry = texture_dictionary.get(texture_key);
	if (entry.get_value() != "item" || !entry.has_data("item-key"))
	{
		return false;
	}

	auto item_key = entry.get_data("item-key");
	memory->set_string("key", item_key);
	memory->set_integer("quantity", std::atoi(entry.get_data("item-quantity").c_str()));
	memory->set_string("runtime-key", texture_key);

	if (entry.has_tag("item-noted"))
	{
		memory->set_boolean("noted", true);
	}
	else
	{
		memory->set_boolean("noted", false);
	}

	return true;
}

bool autonomaus::GUICortex::try_get_vertical_scroll_bar_percent(
	const chomsky::GUIQuery& query,
	float& value) const
{
	chomsky::GUIContent content;
	if (try_get_child(query, content))
	{
		return try_get_vertical_scroll_bar_percent(content, value);
	}

	return false;
}

bool autonomaus::GUICortex::try_get_vertical_scroll_bar_percent(
	const chomsky::GUIContent& content,
	float& value) const
{
	chomsky::GUIContent scroll_down;
	chomsky::GUIContent scroll_up;
	chomsky::GUIContent scroll_slider;
	if (!content.has("scroll-down-button") ||
		!content.has("scroll-up-button") ||
		!content.has("scroll-slider-button"))
	{
		return false;
	}

	scroll_down = content.get("scroll-down-button");
	scroll_up = content.get("scroll-up-button");
	scroll_slider = content.get("scroll-slider-button");

	float scroll_area = scroll_up.get_y();
	scroll_area -= scroll_down.get_y();
	scroll_area -= scroll_down.get_height();
	scroll_area -= scroll_slider.get_height();

	if (scroll_area < 1.0f)
	{
		value = 0.0f;
	}
	else
	{
		float scroll_offset = scroll_up.get_y();
		scroll_offset -= scroll_slider.get_y();
		scroll_offset -= scroll_slider.get_height();
		value = scroll_offset / scroll_area;
	}

	return true;
}

bool autonomaus::GUICortex::try_get_vertical_scroll_area(
	const chomsky::GUIQuery& query,
	float visible_area,
	float& value) const
{
	chomsky::GUIContent content;
	if (try_get_child(query, content))
	{
		return try_get_vertical_scroll_area(content, visible_area, value);
	}

	return false;
}

bool autonomaus::GUICortex::try_get_vertical_scroll_area(
	const chomsky::GUIContent& content,
	float visible_area,
	float& value) const
{
	chomsky::GUIContent scroll_down;
	chomsky::GUIContent scroll_up;
	chomsky::GUIContent scroll_slider;
	if (!content.has("scroll-down-button") ||
		!content.has("scroll-up-button") ||
		!content.has("scroll-slider-button"))
	{
		return false;
	}

	scroll_down = content.get("scroll-down-button");
	scroll_up = content.get("scroll-up-button");
	scroll_slider = content.get("scroll-slider-button");

	float scroll_area = scroll_up.get_y();
	scroll_area -= scroll_down.get_y();
	scroll_area -= scroll_down.get_height();
	float ratio = scroll_area / scroll_slider.get_height();

	value = ratio * visible_area;
	return true;
}

bool autonomaus::GUICortex::do_try_get_content_under_point(
	const chomsky::GUIContent& content,
	const glm::ivec2& point,
	chomsky::GUIContent& result) const
{
	if (point.x < content.get_x() ||
		point.x > content.get_x() + content.get_width() ||
		point.y < content.get_y() ||
		point.y > content.get_y() + content.get_height())
	{
		return false;
	}

	bool child_match = false;
	for (auto& child: content)
	{
		if (do_try_get_content_under_point(*child.second, point, result))
		{
			child_match = true;
		}
	}

	// We want elements with higher Z (e.g., later children) to have higher priority.
	if (!child_match)
	{
		result = content;
	}
	return true;
}

autonomaus::GUICortex::PokeAction::PokeAction(
	GUICortex* cortex,
	const chomsky::GUIContent& content,
	const std::vector<std::string>& actions,
	bool ignore_tool_tip,
	const MotionSequence::Guard& guard) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->content = content;
	this->actions = actions;
	this->ignore_tool_tip = ignore_tool_tip;
	this->guard = guard;
}

autonomaus::GUICortex::PokeAction::~PokeAction()
{
	cancel();
	this->final_result.terminate();
}

autonomaus::Cortex::Result autonomaus::GUICortex::PokeAction::get_result()
{
	return this->final_result;
}

void autonomaus::GUICortex::PokeAction::update()
{
	switch (this->state)
	{
		case state_start:
			{
				MotionSequence sequence(*this->cortex);
				sequence.hover(this->content, guard);

				const float wait_time = Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
				sequence.wait<ToolTipCortex>(wait_time);

				this->current_result = this->cortex->perform(sequence);
				this->state = state_move;
			}
			break;
		case state_move:
			{
				if (this->current_result.get() == Result::Status::failure)
				{
					*this->status = Result::Status::failure;
					this->state = state_done;
				}
				else if (this->current_result.get() == Result::Status::success)
				{
					if (!ignore_tool_tip)
					{
						auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
						if (tool_tip == nullptr)
						{
							Log::info("(GUICortex::PokeAction) Tool tip cortex not valid. Bad logic!");
							*this->status = Result::Status::failure;
							this->state = state_done;
						}
						else if (tool_tip->match_any(this->actions))
						{
							MotionSequence sequence(*this->cortex);
							sequence.left_click(this->content, this->guard);

							this->current_result = this->cortex->perform(sequence);
							this->state = state_left_click;

							// A little bit ugly, but break early.
							break;
						}
					}

					MotionSequence sequence(*this->cortex);
					sequence.right_click(this->content, this->guard);

					const float wait_time = Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME");
					sequence.wait<RightClickMenuCortex>(wait_time);

					this->current_result = this->cortex->perform(sequence);
					this->state = state_right_click;
				}
			}
			break;
		case state_select:
		case state_left_click:
			if (this->current_result.get() != Result::Status::working ||
				!this->guard())
			{
				this->state = state_done;
				*this->status = this->current_result.get();
			}
			break;
		case state_right_click:
			if (this->current_result.get() == Result::Status::success)
			{
				auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
				if (right_click_menu == nullptr)
				{
					Log::info("(GUICortex::PokeAction) Right click menu cortex not valid. Bad logic!");
				}
				else
				{
					this->current_result = right_click_menu->select_any(this->actions);
					this->state = state_select;
				}
			}
			else if (this->current_result.get() == Result::Status::failure)
			{
				*this->status = Result::Status::failure;
				this->state = state_done;
			}
			break;
		case state_done:
		default:
			// Nothing.
			break;
	}
}

void autonomaus::GUICortex::PokeAction::cancel()
{
	if (this->state != state_done)
	{
		this->current_result.cancel();
		*this->status = Result::Status::failure;
		this->state = state_done;
	}
}
