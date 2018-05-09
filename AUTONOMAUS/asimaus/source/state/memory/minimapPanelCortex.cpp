// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/log.hpp"
#include "autonomaus/core/config.hpp"
#include "autonomaus/input/gestures/guiMoveToContentGesture.hpp"
#include "autonomaus/input/gestures/mousePressGesture.hpp"
#include "autonomaus/state/memory/homeTeleportWindowCortex.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/memory/minimapPanelCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"
#include "glooper/map/minimap.hpp"

struct autonomaus::MinimapPanelCortex::ClickTileAction : public Action
{
	ClickTileAction(
		MinimapPanelCortex* cortex,
		const thoreau::TileCoordinate& tile_coordinate,
		float radius);
	~ClickTileAction();

	Cortex::Result get_result() override;
	void update() override;

	bool start();
	void cancel();

	enum
	{
		state_move,
		state_click,
		state_done
	};
	int state = state_move;

	std::unique_ptr<Gesture> gesture;
	MinimapPanelCortex* cortex;
	const glooper::Minimap* minimap;
	Cortex::Result::StatusReference status;
	Cortex::Result result;

	thoreau::TileCoordinate tile_coordinate;
	float radius;
	glm::vec2 destination;

	bool ready = false;
	bool canceled = false;
};

autonomaus::MinimapPanelCortex::MinimapPanelCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::MinimapPanel", autonomaus, "*.main.panels[*].content:minimap-panel-content"),
	run_button_query("*.run-button:button.icon"),
	run_button_resting_query("*.run-button:button.resting"),
	run_button_energy_query("*.run-button:button.value"),
	home_teleport_button_query("*.home-teleport-button:button.icon"),
	minimap_query("*.minimap")
{
	on_poke(
		"enable_run",
		[this](auto)
		{
			return this->enable_run();
		});

	on_poke(
		"disable_run",
		[this](auto)
		{
			return this->disable_run();
		});

	on_pull(
		"is_running",
		[this](auto)
		{
			Message result;
			result.set_boolean("result", this->is_running());
			return result;
		});

	on_pull(
		"run_energy",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_run_energy());
			return result;
		});

	on_poke(
		"rest",
		[this](auto)
		{
			return this->rest();
		});

	on_pull(
		"is_resting",
		[this](auto)
		{
			Message result;
			result.set_boolean("result", this->is_resting());
			return result;
		});

	on_poke(
		"click_tile",
		[this](auto& message)
		{
			thoreau::TileCoordinate tile_coordinate;
			tile_coordinate.x = message.get_integer("x");
			tile_coordinate.y = message.get_integer("y");
			if (message.has("radius"))
			{
				return this->click_tile(tile_coordinate, message.get_single("radius"));
			}
			else
			{
				return this->click_tile(tile_coordinate);
			}
		});

	on_poke(
		"zoom_in",
		[this](auto& message)
		{
			int ticks = message.get_integer("ticks");
			return this->zoom_in(ticks);
		});

	on_poke(
		"zoom_out",
		[this](auto& message)
		{
			int ticks = message.get_integer("ticks");
			return this->zoom_out(ticks);
		});

	on_pull(
		"is_minimap_hidden",
		[this](auto)
		{
			Message result;
			result.set_boolean("result", this->is_minimap_hidden());
			return result;
		});

	on_pull(
		"minimap_rotation",
		[this](auto)
		{
			Message result;
			result.set_single("result", this->get_minimap_rotation());
			return result;
		});

	on_pull(
		"minimap_scale",
		[this](auto)
		{
			Message result;
			result.set_single("result", this->get_minimap_scale());
			return result;
		});

	on_pull(
		"minimap_center",
		[this](auto)
		{
			Message result;
			result.set_single("x", this->get_minimap_center().x);
			result.set_single("y", this->get_minimap_center().y);
			return result;
		});

	on_poke(
		"open_home_teleport_window",
		[this](auto)
		{
			return this->open_home_teleport_window();
		});

	on_poke(
		"teleport_last_destination",
		[this](auto& message)
		{
			return this->teleport_last_destination(message.get_string("destination"));
		});
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::enable_run()
{
	Log::info("(MinimapPanelCortex): Enabling run...");

	if (is_running())
	{
		Log::info("(MinimapPanelCortex): Run already enabled.");
		return Result::Status::success;
	}

	return toggle_run();
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::disable_run()
{
	Log::info("(MinimapPanelCortex): Disabling run...");

	if (!is_running())
	{
		Log::info("(MinimapPanelCortex): Run already disabled.");
		return Result::Status::success;
	}

	return toggle_run();
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::toggle_run()
{
	chomsky::GUIContent run_button;
	if (try_get_child(this->run_button_query, run_button))
	{
		MotionSequence sequence(*this);
		sequence.left_click(run_button);

		return perform(sequence);
	}

	Log::info("(MinimapPanelCortex): Toggling run failed! Minimap panel not visible?");
	return Result::Status::failure;
}

bool autonomaus::MinimapPanelCortex::is_running() const
{
	return has_memory("running");
}

bool autonomaus::MinimapPanelCortex::is_walking() const
{
	return has_memory("walking");
}

int autonomaus::MinimapPanelCortex::get_run_energy() const
{
	Memory memory = get("run-energy");
	return memory->get_integer("value");
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::rest()
{
	Log::info("(MinimapPanelCortex): Resting...");
	if (is_resting())
	{
		Log::info("(MinimapPanelCortex): Already resting.");
		return Result::Status::success;
	}

	chomsky::GUIContent run_button;
	if (try_get_child(this->run_button_query, run_button))
	{
		MotionSequence sequence(*this);
		sequence.right_click(run_button);

		const float wait_time = Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME");
		sequence.wait<RightClickMenuCortex>(wait_time);

		Message message;
		message.set_string("option", "Rest");
		sequence.poke<RightClickMenuCortex>("select", message);

		return perform(sequence);
	}

	Log::info("(MinimapPanelCortex): Resting failed! Minimap panel not visible?");
	return Result::Status::failure;
}

bool autonomaus::MinimapPanelCortex::is_resting() const
{
	return has_memory("resting");
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::click_tile(
	const thoreau::TileCoordinate& tile_coordinate,
	float radius)
{
	Log::info("(MinimapPanelCortex): Clicking tile ({}, {})...", tile_coordinate.x, tile_coordinate.y);
	return perform(new ClickTileAction(this, tile_coordinate, radius));
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::zoom_in(int ticks)
{
	Log::info("(MinimapPanelCortex) Zooming in {} ticks...", ticks);

	if (!is_visible())
	{
		Log::info("(MinimapPanelCortex): Minimap panel not visible.");
		return Result::Status::failure;
	}

	if (is_minimap_hidden())
	{
		Log::info("(MinimapPanelCortex): Minimap is hidden. (E.g., Are you at the Barrows..?)");
		return Result::Status::failure;
	}

	chomsky::GUIContent content;
	if (!try_get_child(this->minimap_query, content))
	{
		Log::info("(MinimapPanelCortex): Couldn't get minimap content. Bad logic!");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.scroll_up(
		content, ticks,
		[this] { return this->is_visible() && !this->is_minimap_hidden(); });
	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::zoom_out(int ticks)
{
	Log::info("(MinimapPanelCortex) Zooming out {} ticks...", ticks);

	if (!is_visible())
	{
		Log::info("(MinimapPanelCortex): Minimap panel not visible.");
		return Result::Status::failure;
	}

	if (is_minimap_hidden())
	{
		Log::info("(MinimapPanelCortex): Minimap is hidden. (E.g., Are you at the Barrows..?)");
		return Result::Status::failure;
	}

	chomsky::GUIContent content;
	if (!try_get_child(this->minimap_query, content))
	{
		Log::info("(MinimapPanelCortex): Couldn't get minimap content. Bad logic!");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.scroll_down(
		content, ticks,
		[this] { return this->is_visible() && !this->is_minimap_hidden(); });
	return perform(sequence);
}

bool autonomaus::MinimapPanelCortex::is_tile_clickable(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	glm::ivec2 unused;
	return try_minimap_to_panel(tile_coordinate, unused);
}

bool autonomaus::MinimapPanelCortex::is_minimap_hidden() const
{
	auto& minimap = get_autonomaus().get_map_state()->get_minimap();
	return !minimap.get_is_visible();
}

float autonomaus::MinimapPanelCortex::get_minimap_rotation() const
{
	auto& minimap = get_autonomaus().get_map_state()->get_minimap();
	return glm::degrees(minimap.get_rotation());
}

float autonomaus::MinimapPanelCortex::get_minimap_scale() const
{
	auto& minimap = get_autonomaus().get_map_state()->get_minimap();
	return glm::length(minimap.get_scale());
}

thoreau::TileCoordinate autonomaus::MinimapPanelCortex::get_minimap_center() const
{
	auto& minimap = get_autonomaus().get_map_state()->get_minimap();
	return minimap.get_center_tile();
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::open_home_teleport_window()
{
	chomsky::GUIContent home_teleport_button;
	if (try_get_child(this->home_teleport_button_query, home_teleport_button))
	{
		MotionSequence sequence(*this);
		sequence.left_click(home_teleport_button);

		return perform(sequence);
	}

	Log::info("(MinimapPanelCortex): Failed to open home teleport window! Minimap panel not visible?");
	return Result::Status::failure;
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::teleport_last_destination(
	const std::string& destination)
{
	chomsky::GUIContent home_teleport_button;
	if (try_get_child(this->home_teleport_button_query, home_teleport_button))
	{
		MotionSequence sequence(*this);
		sequence.invoke(
			[this]
			{
				auto home_teleport_cortex = get_autonomaus().get_userdata<HomeTeleportWindowCortex>();
				if (home_teleport_cortex != nullptr)
				{
					home_teleport_cortex->forget_last_destination();
				}
			});

		sequence.right_click(home_teleport_button);

		const float wait_time = Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME");
		sequence.wait<RightClickMenuCortex>(wait_time);

		Message message;
		message.set_string("option", destination);
		sequence.poke<RightClickMenuCortex>("select", message);

		sequence.invoke(
			[this, destination]
			{
				auto home_teleport_cortex = get_autonomaus().get_userdata<HomeTeleportWindowCortex>();
				if (home_teleport_cortex != nullptr)
				{
					home_teleport_cortex->set_last_destination(destination);
				}
			});

		return perform(sequence);
	}

	Log::info(
		"(MinimapPanelCortex): Teleporting to last destination failed!"
		" Minimap panel not visible?");
	return Result::Status::failure;
}

void autonomaus::MinimapPanelCortex::update()
{
	GUICortex::update();

	if (is_visible())
	{
		update_run();
	}
}

void autonomaus::MinimapPanelCortex::update_run()
{
	chomsky::GUIContent content;
	if (try_get_child(this->run_button_resting_query, content))
	{
		remember("resting", Memory());
	}
	else
	{
		forget("resting");
	}

	std::string run_energy;
	if (try_get_child(this->run_button_energy_query, content) &&
		try_get_text(content, run_energy))
	{
		Memory memory;
		memory->set_integer("value", std::atoi(run_energy.c_str()));
		remember("run-energy", memory);
	}

	if (try_get_child(this->run_button_query, content) &&
		content.get_tags().has("enabled"))
	{
		remember("running", Memory());
		forget("walking");
	}
	else
	{
		remember("walking", Memory());
		forget("running");
	}
}

bool autonomaus::MinimapPanelCortex::try_minimap_to_panel(
	const thoreau::TileCoordinate& tile_coordinate,
	glm::ivec2& result) const
{
	auto& minimap = get_autonomaus().get_map_state()->get_minimap();

	chomsky::GUIContent minimap_content;
	if (!try_get_child(this->minimap_query, minimap_content) ||
		minimap_content.get_num_elements() != 1)
	{
		return false;
	}

	auto& element = minimap_content.get_element(0);
	auto bottom_left_vertex = element.get_vertex(chomsky::GUIElement::BOTTOM_LEFT);
	auto size = glm::vec2(element.get_width(), element.get_height());
	auto offset = bottom_left_vertex.texture_coordinate * minimap.get_visible_size();
	auto new_destination = minimap.tile_to_minimap(tile_coordinate);

	if (new_destination.x < offset.x ||
		new_destination.x > offset.x + size.x ||
		new_destination.y < offset.y ||
		new_destination.y > offset.y + size.y)
	{
		return false;
	}

	glm::ivec2 point = new_destination + glm::vec2(element.get_x(), element.get_y()) - offset;
	chomsky::GUIContent content;
	if (!try_get_content_under_point(point, content) ||
		(content.get_name() != "minimap" && content.get_name() != "background"))
	{
		return false;
	}

	result = point;
	return true;
}

autonomaus::MinimapPanelCortex::ClickTileAction::ClickTileAction(
	MinimapPanelCortex* cortex,
	const thoreau::TileCoordinate& tile_coordinate,
	float radius) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->minimap = &cortex->get_autonomaus().get_map_state()->get_minimap();
	this->tile_coordinate = tile_coordinate;
	this->radius = radius;
}

autonomaus::MinimapPanelCortex::ClickTileAction::~ClickTileAction()
{
	cancel();
	this->result.terminate();
}

autonomaus::Cortex::Result autonomaus::MinimapPanelCortex::ClickTileAction::get_result()
{
	return this->result;
}

void autonomaus::MinimapPanelCortex::ClickTileAction::update()
{
	if (this->canceled)
	{
		*this->status = Result::Status::failure;
		return;
	}

	if (!this->ready)
	{
		if (!start())
		{
			cancel();
		}

		this->ready = true;
	}
	else if (*this->status == Result::Status::working)
	{
		switch (this->state)
		{
			case state_move:
				if (!gesture->pending())
				{
					this->gesture = std::make_unique<MousePressGesture>(
						this->cortex->get_autonomaus().get_input_manager(),
						eiznebahn::MouseButton::button_left,
						true);
					this->gesture->play();
					this->state = state_click;
				}
				else
				{
					auto new_destination = this->minimap->tile_to_minimap(this->tile_coordinate);
					auto distance = glm::length(this->destination - new_destination);
					if (distance > radius)
					{
						gesture->cancel();
						this->ready = false;
					}
				}
				break;
			case state_click:
				if (!gesture->pending())
				{
					*this->status = Result::Status::success;
					this->state = state_done;
				}
				break;
			case state_done:
				// Nothing.
				break;
		}
	}
}

bool autonomaus::MinimapPanelCortex::ClickTileAction::start()
{
	glm::ivec2 point;
	if (!this->cortex->try_minimap_to_panel(this->tile_coordinate, point))
	{
		Log::warn("(MinimapPanelCortex::ClickTileAction) Couldn't get panel-relative coordinate from tile.");
		return false;
	}

	auto& draw_state = this->cortex->get_autonomaus().get_draw_state();
	auto& input_manager = this->cortex->get_autonomaus().get_input_manager();
	glm::ivec2 click_bounds = glm::ivec2(this->radius * 2);
	glm::ivec2 click_position = point - glm::ivec2(this->radius);
	this->gesture = std::make_unique<GUIMoveToContentGesture>(
		input_manager,
		click_position.x, click_position.y,
		click_bounds.x, click_bounds.y,
		draw_state.get_gui_viewport().z, draw_state.get_gui_viewport().w);
	if (gesture->play() != Gesture::success)
	{
		Log::info("(MinimapPanelCortex) Couldn't play gesture!");
		this->gesture.reset();
		return false;
	}

	this->destination = this->minimap->tile_to_minimap(tile_coordinate);;
	return true;
}

void autonomaus::MinimapPanelCortex::ClickTileAction::cancel()
{
	if (this->gesture)
	{
		this->gesture->cancel();
		this->gesture.reset();
	}

	this->canceled = true;
}
