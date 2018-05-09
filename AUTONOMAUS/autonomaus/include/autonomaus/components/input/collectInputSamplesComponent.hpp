// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_INPUT_COLLECT_INPUT_SAMPLES_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_INPUT_COLLECT_INPUT_SAMPLES_COMPONENT_HPP

#include <functional>
#include <vector>
#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"
#include "chomsky/gui/guiContent.hpp"

namespace autonomaus
{
	class CollectInputSamplesComponent : public Component
	{
	public:
		explicit CollectInputSamplesComponent(Autonomaus& autonomaus);
		~CollectInputSamplesComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();
		void draw_mouse();
		void draw_keyboard();

		bool capturing_mouse_input = false;
		float capture_mouse_start = 0.0f;
		void capture_mouse_input(bool enable);
		void show_mouse_input();
		void show_gui_elements(GUIOverlay& overlay, int mouse_x, int mouse_y);
		void show_gui_contents(GUIOverlay& overlay, int mouse_x, int mouse_y);
		void save_mouse_input();

		bool capturing_keyboard_input = false;
		float capture_keyboard_start = 0.0f;
		void capture_keyboard_input(bool enable);
		void save_keyboard_input();

		void for_each_content_under_cursor(
			const chomsky::GUIContent& content,
			const std::function<void(const chomsky::GUIContent&)>& callback);

		int last_mouse_x = 0, last_mouse_y = 0;
		bool last_left_click = false;
		float last_mouse_movement = HUGE_VALF;
		bool show_gui_content = true, show_gui_element = false;
		GUIOverlay::Tag mouse_move_tag;
		GUIOverlay::Tag mouse_click_tag;

		std::map<int /* key code */, float /* timestamp */> keys;
	};
}

#endif
