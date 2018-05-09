// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <functional>
#include <set>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/makeXWindowCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"

const float autonomaus::MakeXWindowCortex::ACTION_WAIT_TIME_MIN =
	Config::get<float>("gui", "MakeXWindowCortex", "ACTION_WAIT_TIME_MIN");
const float autonomaus::MakeXWindowCortex::ACTION_WAIT_TIME_MAX =
	Config::get<float>("gui", "MakeXWindowCortex", "ACTION_WAIT_TIME_MAX");

struct autonomaus::MakeXWindowCortex::SelectAction : public Action
{
public:
	SelectAction(
		MakeXWindowCortex* cortex,
		const std::string& item,
		const std::string& category);
	~SelectAction();

	void cancel();

	Result get_result() override;
	void update() override;

	MakeXWindowCortex* cortex;
	std::string item;
	std::string category;

private:
	bool iterate_items(const std::function<bool(const chomsky::GUIContent&, const Memory&, ItemCortex*)>& func);
	bool is_item_visible();
	void select_item();

	Memory unknown_item;
	bool is_unknown_item_visible();
	void learn_unknown_item();

	enum
	{
		scroll_start,
		scroll_resume
	};
	void scroll(int state = scroll_start);

	void remember_item_category(const std::string& item);

	enum
	{
		state_start,
		state_open_categories,
		state_decide_category,
		state_select_category,
		state_wait,
		state_scroll,
		state_move,
		state_hover,
		state_click,
		state_failure,
		state_success,
		state_error,
		state_done
	};
	int state = state_start;
	int previous_state = state_failure;
	float exception_time = 0.0f;
	float wait_time = 0.0f;

	std::set<std::string> unvisited_categories;
	std::set<std::string> visited_categories;
	std::set<std::string> possible_categories;

	Result current_result;
	float hover_time = 0.0f;

	Result::StatusReference status;
	Result final_result;
};

autonomaus::MakeXWindowCortex::MakeXWindowCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::MakeXWindow", autonomaus, "*.main.windows[*]:make-x-window"),
	items_query("*.items.items[*].item"),
	selected_item_query("*.recipe.header.name"),
	categories_query("*.items.drop-down.button.text"),
	categories_option_query("*.items.drop-down.menu.values[*]"),
	quantity_input_query("*.recipe.quantity.value"),
	quantity_input_value_query("*.recipe.quantity.value.value"),
	make_button_query("*.recipe.quantity.make:button"),
	close_button_query("*.close-button:button"),
	scroll_bar_query("*.items.scroll-bar")

{
	on_poke(
		"select",
		[this](auto& message)
		{
			return this->select(message.get_string("item"), message.get_string("category"));
		});

	on_poke(
		"set_quantity",
		[this](auto& message)
		{
			return this->set_quantity(message.get_integer("value"));
		});

	on_poke(
		"make",
		[this](auto&)
		{
			return this->make();
		});

	on_poke(
		"close",
		[this](auto&)
		{
			return this->close();
		});

	on_pull(
		"current_category",
		[this](auto&)
		{
			Message result;
			result.set_string("result", this->get_current_category());
			return result;
		});

	on_pull(
		"current_item",
		[this](auto&)
		{
			Message result;
			result.set_string("result", this->get_current_item());
			return result;
		});

	on_pull(
		"current_quantity",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_current_quantity());
			return result;
		});
}

autonomaus::Cortex::Result autonomaus::MakeXWindowCortex::select(
	const std::string& item,
	const std::string& category)
{
	Log::info("(MakeXWindowCortex) Selecting item '{}'...", item);

	if (fuzzy_string_match(get_current_item(), item))
	{
		Log::info("(MakeXWindowCortex) Item already selected.");
		return Result::Status::success;
	}

	return perform(new SelectAction(this, item, category));
}

autonomaus::Cortex::Result autonomaus::MakeXWindowCortex::set_quantity(int value)
{
	Log::info("(MakeXWindowCortex) Setting quantity to {}...", value);

	chomsky::GUIContent quantity_input_value;
	if (!try_get_child(this->quantity_input_value_query, quantity_input_value))
	{
		Log::warn("(MakeXWindowCortex) Couldn't get current input value. Is the Make-X window open?");
		return Result::Status::failure;
	}

	auto current_value = std::atoi(quantity_input_value.get_text().to_string().c_str());
	if (current_value == value)
	{
		Log::info("(MakeXWindowCortex) Quantity already set to {}.", value);
		return Result::Status::success;
	}

	chomsky::GUIContent quantity_input;
	if (!try_get_child(this->quantity_input_query, quantity_input))
	{
		Log::warn("(MakeXWindowCortex) Couldn't get current input button. Bad logic!");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(
		quantity_input,
		[this] { return this->is_visible(); });
	sequence.wait(
		ACTION_WAIT_TIME_MAX,
		[this]
		{
			return this->get_current_quantity() == 0;
		});
	sequence.type(std::to_string(value), true, [this] { return this->is_visible(); });
	sequence.type("\n", true, [this] { return this->is_visible(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::MakeXWindowCortex::make()
{
	Log::info("(MakeXWindowCortex) Making {}x {}(s)...", get_current_quantity(), get_current_item());

	chomsky::GUIContent button;
	if (!try_get_child(this->make_button_query, button))
	{
		Log::warn("(MakeXWindowCortex) Couldn't make item. Is the Make-X window open?");
		return Result::Status::failure;
	}

	if (button.has("background"))
	{
		auto background = button.get("background");
		if (background.get_tags().has("disabled"))
		{
			Log::warn("(MakeXWindowCortex) Can't make item: requirements not met.");
			return Result::Status::failure;
		}
	}

	MotionSequence sequence(*this);
	sequence.left_click(button, [this] { return this->is_visible(); });
	sequence.wait(ACTION_WAIT_TIME_MAX, [this] { return !this->is_visible(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::MakeXWindowCortex::close()
{
	Log::info("(MakeXWindowCortex) Closing window...");

	chomsky::GUIContent button;
	if (!try_get_child(this->close_button_query, button))
	{
		Log::warn("(MakeXWindowCortex) Couldn't close window: window isn't open!");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(button, [this] { return this->is_visible(); });
	sequence.wait(ACTION_WAIT_TIME_MAX, [this] { return !this->is_visible(); });

	return perform(sequence);
}

std::string autonomaus::MakeXWindowCortex::get_current_category() const
{
	chomsky::GUIContent content;
	if (!try_get_child(this->categories_query, content))
	{
		return std::string();
	}

	return content.get_text().to_string();
}

std::string autonomaus::MakeXWindowCortex::get_current_item() const
{
	chomsky::GUIContent content;
	if (!try_get_child(this->selected_item_query, content))
	{
		return std::string();
	}

	std::string result;
	auto& text = content.get_text();
	for (std::size_t i = 0; i < text.count(); ++i)
	{
		auto segment = text.get(i);
		auto t = std::get<chomsky::GUIText::SEGMENT_STRING>(segment);
		result += trim_string(t);

		if (i + 1 < text.count())
		{
			result += " ";
		}
	}

	return result;
}

int autonomaus::MakeXWindowCortex::get_current_quantity() const
{
	chomsky::GUIContent quantity_input_value;
	if (!try_get_child(this->quantity_input_value_query, quantity_input_value))
	{
		return 0;
	}

	auto text = quantity_input_value.get_text().to_string();
	if (text.empty())
	{
		return 0;
	}

	return std::atoi(text.c_str());
}

autonomaus::MakeXWindowCortex::SelectAction::SelectAction(
	MakeXWindowCortex* cortex,
	const std::string& item,
	const std::string& category) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->item = item;
	this->category = category;
}

autonomaus::MakeXWindowCortex::SelectAction::~SelectAction()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	this->final_result.terminate();
}

void autonomaus::MakeXWindowCortex::SelectAction::cancel()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	this->state = state_done;
}

autonomaus::Cortex::Result autonomaus::MakeXWindowCortex::SelectAction::get_result()
{
	return this->final_result;
}

void autonomaus::MakeXWindowCortex::SelectAction::update()
{
	if (!this->cortex->is_visible())
	{
		if (this->state != state_error && this->state != state_done)
		{
			this->previous_state = this->state;
			this->state = state_error;

			const float wait_time = Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
			this->exception_time = get_current_time() + wait_time;
		}
	}

	switch (this->state)
	{
		case state_start:
			{
				if (this->cortex->has_memory(this->item))
				{
					auto memory = this->cortex->get(this->item);

					for (std::size_t i = 0; i < memory->length("category"); ++i)
					{
						this->possible_categories.insert(memory->get_string("category", i));
						printf("possible category: %s\n", memory->get_string("category", i).c_str());
					}
				}

				chomsky::GUIContent categories;
				if (!this->cortex->try_get_child(this->cortex->categories_query, categories))
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Couldn't get categories! Bad logic.");
					this->state = state_failure;
					break;
				}

				if (is_item_visible())
				{
					select_item();
				}
				else
				{
					auto name = categories.get_text().to_string();
					if (this->possible_categories.count(name) != 0 ||
						this->possible_categories.empty() ||
						(!this->category.empty() && fuzzy_string_match(name, this->category)))
					{
						this->possible_categories.erase(name);
						this->visited_categories.insert(name);

						scroll();
					}
					else
					{
						this->state = state_open_categories;
					}
				}
			}
			break;
		case state_open_categories:
			{
				MotionSequence sequence(*this->cortex);

				chomsky::GUIContent categories;
				if (!this->cortex->try_get_child(this->cortex->categories_query, categories))
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Couldn't get categories! Bad logic.");
					this->state = state_failure;
					break;
				}

				sequence.left_click(categories, [this] { return this->cortex->is_visible(); });
				sequence.wait(
					Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME"),
					[this]
					{
						chomsky::GUIContent root;
						if (!this->cortex->try_get_root(root))
						{
							return false;
						}

						std::vector<chomsky::GUIContent> categories;
						if (!this->cortex->categories_option_query.collect(root, categories)
							|| categories.empty())
						{
							return false;
						}

						return true;
					});
				this->current_result = this->cortex->perform(sequence);
				this->state = state_decide_category;
			}
			break;
		case state_decide_category:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::failure)
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Failed to click on category drop-down menu.");
					this->state = state_failure;
					break;
				}

				chomsky::GUIContent root;
				if (!this->cortex->try_get_root(root))
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Failed to get root. Bad logic!");
					this->state = state_failure;
					break;
				}

				std::vector<chomsky::GUIContent> categories;
				if (!this->cortex->categories_option_query.collect(root, categories) || categories.empty())
				{
					Log::warn("(MakeXWindowCortex::SelectAction) No categories. Bad logic!");
					this->state = state_failure;
					break;
				}

				for (auto& c: categories)
				{
					auto name = c.get_text().to_string();
					if (this->visited_categories.count(name))
					{
						continue;
					}

					if (this->possible_categories.count(name))
					{
						this->visited_categories.insert(name);

						MotionSequence sequence(*this->cortex);
						sequence.left_click(c, [this] { return this->cortex->is_visible(); });
						sequence.wait(ACTION_WAIT_TIME_MIN, [this, name] { return this->cortex->get_current_category() == name; });

						this->current_result = this->cortex->perform(sequence);
						this->state = state_select_category;
						break;
					}
				}

				if (this->state != state_decide_category)
				{
					break;
				}

				for (auto& c: categories)
				{
					auto name = c.get_text().to_string();
					if (this->visited_categories.count(name))
					{
						continue;
					}

					this->visited_categories.insert(name);

					MotionSequence sequence(*this->cortex);
					sequence.left_click(c, [this] { return this->cortex->is_visible(); });
					sequence.wait(ACTION_WAIT_TIME_MIN, [this, name] { return this->cortex->get_current_category() == name; });

					this->current_result = this->cortex->perform(sequence);
					this->state = state_select_category;
					break;
				}

				if (this->state == state_decide_category)
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Item '{}' not found.", this->item);
					this->state = state_failure;
					break;
				}
			}
			break;
		case state_select_category:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_wait;
					this->wait_time = get_current_time() + RNG::nextf(ACTION_WAIT_TIME_MIN, ACTION_WAIT_TIME_MAX);
				}
				else
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Failed to select specific category from drop-down menu.");
					this->state = state_failure;
				}
			}
			break;
		case state_wait:
			if (get_current_time() > this->wait_time)
			{
				scroll();
			}
			break;
		case state_scroll:
			if (is_item_visible())
			{
				this->current_result.cancel();
				select_item();
			}
			else if (is_unknown_item_visible())
			{
				this->current_result.cancel();
				learn_unknown_item();
			}
			else if (!this->current_result.pending())
			{
				this->state = state_open_categories;
			}
			break;
		case state_move:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->hover_time = get_current_time() + Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
					this->state = state_hover;
				}
				else
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Failed to move to unknown item.");
					this->state = state_failure;
					break;
				}
			}
			break;
		case state_hover:
			{
				auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
				auto items = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
				if (tool_tip == nullptr)
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Tool tip cortex not valid. Bad logic!");
					this->state = state_failure;
					break;
				}
				else if (items == nullptr)
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Item cortex not valid. Bad logic!");
					this->state = state_failure;
					break;
				}

				bool match = false;
				if (tool_tip->is_visible())
				{
					auto text = split_string(tool_tip->to_string(), "\n");
					if (text.size() >= 1)
					{
						auto name = text[0];
						if (!name.empty())
						{
							items->remember_item(unknown_item->get_string("runtime-key"));
							items->add_name(unknown_item->get_string("key"), name);
							remember_item_category(name);

							Log::info("(MakeXWindowCortex::SelectAction) Learned about item '{0}'.", name);
							match = true;
						}
					}
				}
				
				if (get_current_time() > this->hover_time || match)
				{
					if (is_unknown_item_visible())
					{
						learn_unknown_item();
					}
					else
					{
						scroll(scroll_resume);
					}
				}
			}
			break;
		case state_click:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_success;
				}
				else
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Couldn't select item '{}'.", this->item);
					this->state = state_failure;
				}
			}
			break;
		case state_failure:
			*this->status = Result::Status::failure;
			this->state = state_done;
			break;
		case state_success:
			*this->status = Result::Status::success;
			this->state = state_done;
			break;
		case state_error:
			if (this->cortex->is_visible())
			{
				this->state = this->previous_state;
			}
			else
			{
				auto& game_state = this->cortex->get_autonomaus().get_game_state();
				if (game_state.get_last_update_timestamp() > this->exception_time)
				{
					Log::warn("(MakeXWindowCortex::SelectAction) Make-X window no longer visible.");
					cancel();
				}
			}
			break;
		case state_done:
			// Nothing.
			break;
		default:
			Log::warn("(MakeXWindowCortex::SelectAction) Unhandled state {}. Bad logic!", this->state);
			cancel();
			break;
	}

}

bool autonomaus::MakeXWindowCortex::SelectAction::iterate_items(
	const std::function<bool(const chomsky::GUIContent&, const Memory&, ItemCortex*)>& func)
{
	chomsky::GUIContent root;
	if (!this->cortex->try_get_root(root))
	{
		return false;
	}

	std::vector<chomsky::GUIContent> item_contents;
	if (!this->cortex->items_query.collect(root, item_contents) || item_contents.empty())
	{
		return false;
	}

	auto item_cortex = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		return false;
	}

	for (auto& item_content: item_contents)
	{
		Memory item;
		if (!this->cortex->try_get_item(item_content, item))
		{
			continue;
		}

		if (func(item_content, item, item_cortex))
		{
			return true;
		}
	}

	return false;
}

bool autonomaus::MakeXWindowCortex::SelectAction::is_item_visible()
{
	return iterate_items(
		[this](auto& content, auto& memory, auto items)
		{
			auto key = memory->get_string("key");
			if (!items->has_name(key))
			{
				return false;
			}

			auto names = items->get_names(key);
			for (auto name: names)
			{
				if (fuzzy_string_match(name, this->item))
				{
					return true;
				}
			}

			return false;
		});
}

void autonomaus::MakeXWindowCortex::SelectAction::select_item()
{
	bool result = iterate_items(
		[this](auto& content, auto& memory, auto items)
		{
			auto key = memory->get_string("key");
			if (!items->has_name(key))
			{
				return false;
			}

			auto names = items->get_names(key);
			for (auto name: names)
			{
				if (fuzzy_string_match(name, this->item))
				{
					MotionSequence sequence(*this->cortex);
					sequence.left_click(content, [this] { return this->cortex->is_visible(); });

					this->remember_item_category(this->item);
					this->remember_item_category(name);

					this->current_result = this->cortex->perform(sequence);
					this->state = state_click;
					return true;
				}
			}

			return false;
		});

	if (!result)
	{
		Log::warn("(MakeXWindowCortex::SelectAction) Couldn't select item!");
		cancel();
	}
}

bool autonomaus::MakeXWindowCortex::SelectAction::is_unknown_item_visible()
{
	return iterate_items(
		[this](auto& content, auto& memory, auto items)
		{
			auto key = memory->get_string("key");
			return !items->has_name(key);
		});
}

void autonomaus::MakeXWindowCortex::SelectAction::learn_unknown_item()
{
	bool result = iterate_items(
		[this](auto& content, auto& memory, auto items)
		{
			auto key = memory->get_string("key");
			if (!items->has_name(key))
			{
				MotionSequence sequence(*this->cortex);
				sequence.hover(content, [this] { return this->cortex->is_visible(); });

				this->current_result = this->cortex->perform(sequence);
				this->state = state_move;
				this->unknown_item = memory;
				return true;
			}

			return false;
		});

	if (!result)
	{
		Log::warn("(MakeXWindowCortex::SelectAction) Couldn't learn about item!");
		cancel();
	}
}

void autonomaus::MakeXWindowCortex::SelectAction::remember_item_category(const std::string& item)
{
	std::string category = this->cortex->get_current_category();

	auto memory = this->cortex->get(item);
	if (memory->length("category") == 0)
	{
		// Oops, this looks redundant.
		memory->set_string("category", category);
	}
	else
	{
		for (std::size_t i = 0; i < memory->length("category"); ++i)
		{
			if (memory->get_string("category", i) == category)
			{
				return;
			}
		}

		memory->set_string("category", category, memory->length("category"));
	}

	this->cortex->remember(item, memory);
}

void autonomaus::MakeXWindowCortex::SelectAction::scroll(int state)
{
	chomsky::GUIContent scroll_bar;
	if (!this->cortex->try_get_child(this->cortex->scroll_bar_query, scroll_bar))
	{
		this->current_result = Result::Status::success;
		this->state = state_scroll;
		return;
	}
	else
	{
		float visible_area = scroll_bar.get_height();
		float total_area;
		this->cortex->try_get_vertical_scroll_area(scroll_bar, visible_area, total_area);
		if (total_area - visible_area < 16.0f)
		{
			this->current_result = Result::Status::success;
			this->state = state_scroll;
			return;
		}
	}

	MotionSequence sequence(*this->cortex);
	if (state == scroll_start)
	{
		sequence.drag(
			scroll_bar.get("scroll-slider-button"),
			scroll_bar.get("scroll-up-button"),
			1.0f,
			[this] { return this->cortex->is_visible(); });
	}
	sequence.drag(
		scroll_bar.get("scroll-slider-button"),
		scroll_bar.get("scroll-down-button"),
		4.0f,
		[this] { return this->cortex->is_visible(); });

	this->current_result = this->cortex->perform(sequence);
	this->state = state_scroll;
}
