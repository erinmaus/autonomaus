// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_GUI_DRAW_HOOK_HPP
#define AUTONOMAUS_GRAPHICS_GUI_DRAW_HOOK_HPP

#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/program.hpp"
#include "autonomaus/graphics/texture.hpp"
#include "autonomaus/graphics/mesh.hpp"
#include "autonomaus/graphics/drawHook.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"
#include "eiznebahn/input/inputState.hpp"

namespace autonomaus
{
	class GUIDrawHook : public DrawHook<GUIOverlay>
	{
	public:
		GUIDrawHook(
			touchstone::ImmediateDispatch& dispatch,
			DrawState& draw_state);
		~GUIDrawHook() = default;

		void invalidate() override;
		void load() override;
		void before_call(
			const touchstone::Context& context,
			std::uint32_t token) override;
		void after_call(
			const touchstone::Context& context,
			const touchstone::Call* call) override;
		void on_flip(const touchstone::Context& context) override;

		void update_input(const eiznebahn::InputState& input_state);
		bool wants_mouse_captured() const;
		bool wants_keyboard_captured() const;
		bool is_ready() const;

	private:
		void before_draw_overlay(ImDrawList& draw_list);
		void after_draw_overlay(ImDrawList& draw_list);
		void draw_before();
		void draw_after();

		void draw_imgui();
		void prepare_imgui_draw_state();
		void draw_imgui_draw_list(ImDrawList* list);

		ImDrawList before_draw_list;
		ImDrawList after_draw_list;

		static bool is_drawing_to_back_buffer(const touchstone::Context& context);
		bool is_first_draw = true;
		bool is_first_present = true;
		bool presented_world = false;
		bool is_valid = false;
		bool can_draw_gui = false;
		float previous_time;
		int previous_mouse_scroll = 0;

		int viewport_x = 0, viewport_y = 0, viewport_width = 1, viewport_height = 1;
		bool is_viewport_updated = false;
		void update_viewport(const touchstone::Context& context);

		std::unique_ptr<Program> program;
		std::unique_ptr<Texture> font;
		std::unique_ptr<Mesh> mesh;
	};
}

#endif
