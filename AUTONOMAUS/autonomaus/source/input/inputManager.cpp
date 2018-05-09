// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <boost/filesystem.hpp>
#include "autonomaus/core/rng.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"

autonomaus::InputManager::InputManager(
	Autonomaus& autonomaus, eiznebahn::InputSpoofer& input_spoofer)
{
	this->autonomaus = &autonomaus;
	this->spoofer = &input_spoofer;
}

autonomaus::InputManager::~InputManager()
{
	// Nothing.
}

void autonomaus::InputManager::load(const std::string& root)
{
	boost::filesystem::path mouse_paths_filename(root);
	mouse_paths_filename.append("paths.dat");

	load_mouse_paths(mouse_paths_filename.string());
}

eiznebahn::InputQueue& autonomaus::InputManager::get_queue()
{
	return this->queue;
}

const eiznebahn::InputQueue& autonomaus::InputManager::get_queue() const
{
	return this->queue;
}

const eiznebahn::InputState& autonomaus::InputManager::get_state() const
{
	return this->spoofer->get_input_state();
}

eiznebahn::InputSpoofer& autonomaus::InputManager::get_spoofer() const
{
	return *this->spoofer;
}

void autonomaus::InputManager::update()
{
	if (this->previews_enabled)
	{
		auto& overlay = this->autonomaus->get_gui_draw_hook().after();
		glm::vec2 mouse = glm::vec2(get_state().get_mouse_x(), get_state().get_mouse_y());
		glm::vec2 triangle[] =
		{
			glm::vec2(mouse.x, mouse.y),
			glm::vec2(mouse.x + 8, mouse.y + 24),
			glm::vec2(mouse.x + 8, mouse.y + 24),
			glm::vec2(mouse.x + 24, mouse.y + 8),
			glm::vec2(mouse.x + 24, mouse.y + 8),
			glm::vec2(mouse.x, mouse.y)
		};

		overlay.start(this->mouse_cursor_tag);
		overlay.draw_path(triangle, 6);
		overlay.stop();

		if (this->preview_mode == preview_after)
		{
			auto previous_x = this->previous_state.get_mouse_x();
			auto previous_y = this->previous_state.get_mouse_y();
			auto current_x = get_state().get_mouse_x();
			auto current_y = get_state().get_mouse_y();

			if (current_x != previous_x || current_y != previous_y)
			{
				overlay.start(this->mouse_path_tag, false, 0.25f);

				glm::vec2 path[] =
				{
					glm::vec2(previous_x, previous_y),
					glm::vec2(current_x, current_y),
				};
				overlay.draw_path(path, 2, glm::vec4(glm::normalize(this->color), 1.0f), 2.0f, false);

				overlay.stop();
			}

			for (int i = eiznebahn::MouseButton::button_min; i != eiznebahn::MouseButton::button_max; ++i)
			{
				if (i == eiznebahn::MouseButton::button_up ||
					i == eiznebahn::MouseButton::button_down)
				{
					continue;
				}

				if (this->previous_state.is_mouse_button_pressed(i) != get_state().is_mouse_button_pressed(i))
				{
					this->mouse_path_tag = GUIOverlay::Tag();
					this->color = glm::vec3(RNG::nextf(), RNG::nextf(), RNG::nextf());
					break;
				}
			}
		}
	}

	work();

	this->previous_state = get_state();
}

void autonomaus::InputManager::work()
{
	float delta;
	float current_time = get_current_time();
	if (this->previous_time == HUGE_VALF)
	{
		delta = 0.0f;
	}
	else
	{
		delta = (current_time - previous_time) * 1000.0f;
	}
	this->previous_time = current_time;

	this->queue.update(delta, *this->spoofer);
}

void autonomaus::InputManager::set_preview_mode(int mode)
{
	if (mode == preview_before || mode == preview_after)
	{
		this->preview_mode = mode;
	}
}

int autonomaus::InputManager::get_preview_mode() const
{
	return this->preview_mode;
}

bool autonomaus::InputManager::get_is_preview_enabled() const
{
	return this->previews_enabled;
}

void autonomaus::InputManager::enable_previews()
{
	this->previews_enabled = true;
}

void autonomaus::InputManager::disable_previews()
{
	this->previews_enabled = false;
}

void autonomaus::InputManager::preview(const eiznebahn::MouseButton& button)
{
	if (this->previews_enabled && autonomaus->get_is_debug_enabled())
	{
		auto& overlay = autonomaus->get_gui_draw_hook().after();

		GUIOverlay::Tag tag;
		overlay.start(tag);
		overlay.draw_rectangle_outline(
			glm::vec2(get_state().get_mouse_x() - 4, get_state().get_mouse_y() - 4),
			8, 8,
			glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
			4.0f);
		overlay.stop();
	}
}

void autonomaus::InputManager::preview(const eiznebahn::MousePath& path)
{
	if (this->previews_enabled && this->preview_mode == preview_before && autonomaus->get_is_debug_enabled())
	{
		std::vector<glm::vec2> points;
		std::vector<glm::vec2> shadow;
		for (auto point: path)
		{
			float x = std::get<eiznebahn::MousePath::X>(point);
			float y = std::get<eiznebahn::MousePath::Y>(point);

			points.emplace_back(x, y);
			shadow.emplace_back(x + 2, y + 2);
		}

		auto& overlay = autonomaus->get_gui_draw_hook().after();
		GUIOverlay::Tag tag;

		overlay.start(tag);
		overlay.draw_path(
			&shadow[0], shadow.size(),
			glm::vec4(0.0f, 0.0f, 0.0f, 0.5f), 4.0f, true);
		overlay.draw_path(
			&points[0], points.size(),
			glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 4.0f, true);
		overlay.stop();
	}
}

const autonomaus::InputManager::MousePathCollection&
autonomaus::InputManager::get_paths() const
{
	return this->paths;
}

void autonomaus::InputManager::load_mouse_paths(const std::string& filename)
{
	this->paths.clear();

	std::ifstream stream(filename);

	std::string command;
	stream >> command;
	while (stream.good() && command == "path")
	{
		eiznebahn::MousePath current;

		stream >> command;
		while (stream.good() && command == "sample")
		{
			int x, y;
			int duration;
			stream >> x >> y >> duration;

			current.add(x, y, duration);

			stream >> command;
		}
		
		if (!current.empty())
		{
			this->paths.emplace_back(std::move(current));
		}
	}
}
