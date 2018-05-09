// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_WORLD_DRAW_HOOK_HPP
#define AUTONOMAUS_GRAPHICS_WORLD_DRAW_HOOK_HPP

#include "autonomaus/graphics/drawHook.hpp"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/worldOverlay.hpp"
#include "autonomaus/graphics/drawState.hpp"
#include "rosalind/common/re.hpp"

namespace autonomaus
{
	class WorldDrawHook : public DrawHook<WorldOverlay>
	{
	public:
		WorldDrawHook(
			touchstone::ImmediateDispatch& dispatch,
			DrawState& draw_state);
		~WorldDrawHook() = default;

		void before_call(
			const touchstone::Context& context,
			std::uint32_t token) override;
		void after_call(
			const touchstone::Context& context,
			const touchstone::Call* call) override;
		void on_flip(const touchstone::Context& context) override;

	private:
		void draw_before();
		void draw_after(int current_frame_buffer);

		bool cleared_frame_buffer = false;
		bool updated_view_transforms = false;
		bool is_drawing = false;
		int world_frame_buffer = 0;
		int world_viewport_x = 0, world_viewport_y = 0;
		int world_viewport_width = 0, world_viewport_height = 0;
		int view_transforms_uniform_buffer = 0;
		rosalind::re::ViewTransform view_transforms;

		static bool match_world_frame_buffer(
			const touchstone::Context& context);
		static bool has_depth_buffer(const touchstone::Context& context);
	};
}

#endif
