// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_INPUT_MANAGER_HPP
#define AUTONOMAUS_INPUT_INPUT_MANAGER_HPP

#include <cmath>
#include <string>
#include <thread>
#include <vector>
#include "eiznebahn/mouse/mousePath.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"
#include "eiznebahn/input/inputSpoofer.hpp"
#include "eiznebahn/input/inputState.hpp"
#include "eiznebahn/input/inputQueue.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"

namespace autonomaus
{
	class Autonomaus;

	class InputManager
	{
	public:
		InputManager(
			autonomaus::Autonomaus& autonomaus,
			eiznebahn::InputSpoofer& input_spoofer);
		~InputManager();

		void load(const std::string& root);

		eiznebahn::InputQueue& get_queue();
		const eiznebahn::InputQueue& get_queue() const;

		const eiznebahn::InputState& get_state() const;

		eiznebahn::InputSpoofer& get_spoofer() const;

		void update();

		enum
		{
			preview_before,
			preview_after
		};
		void set_preview_mode(int mode);
		int get_preview_mode() const;
		void enable_previews();
		void disable_previews();
		bool get_is_preview_enabled() const;
		void preview(const eiznebahn::MouseButton& button);
		void preview(const eiznebahn::MousePath& path);

	private:
		Autonomaus* autonomaus;

		float previous_time = HUGE_VALF;

		eiznebahn::InputSpoofer* spoofer;
		eiznebahn::InputQueue queue;

#ifdef NDEBUG
		bool previews_enabled = false;
#else
		bool previews_enabled = true;
#endif
		int preview_mode = preview_before;
		glm::vec3 color = glm::vec3(0.0f, 1.0f, 1.0f);
		eiznebahn::InputState previous_state;
		GUIOverlay::Tag mouse_path_tag;

		GUIOverlay::Tag mouse_cursor_tag;

		typedef std::vector<eiznebahn::MousePath> MousePathCollection;
		std::vector<eiznebahn::MousePath> paths;
		void load_mouse_paths(const std::string& filename);

		void work();

	public:
		const MousePathCollection& get_paths() const;
	};
}

#endif
