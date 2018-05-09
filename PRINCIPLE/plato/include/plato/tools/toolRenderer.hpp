// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_TOOLS_TOOL_RENDERER_HPP
#define PLATO_TOOLS_TOOL_RENDERER_HPP

#include <glm/glm.hpp>
#include "principle/deps/imgui/imgui.h"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/shape.hpp"

namespace plato
{
	class ToolRenderer
	{
	public:
		static const float NO_OUTLINE;

		ToolRenderer();
		virtual ~ToolRenderer() = default;

		virtual glm::vec2 get_camera_offset() const = 0;
		virtual float get_camera_scale() const = 0;
		virtual int get_viewport_width() const = 0;
		virtual int get_viewport_height() const = 0;
		virtual glm::mat4 get_view_matrix() const = 0;
		virtual glm::mat4 get_projection_matrix() const = 0;

		void draw();

		glm::vec2 tile_to_screen(const thoreau::TileCoordinate& tile_coordinate);
		void draw_text(
			const thoreau::TileCoordinate& tile_coordinate,
			const std::string& text,
			const glm::vec4& color,
			float scale = 1.0f);
		void draw_text(
			const glm::vec2& position,
			const std::string& text,
			const glm::vec4& color,
			float scale = 1.0f);
		void draw_line(
			const thoreau::TileCoordinate& from,
			const thoreau::TileCoordinate& to,
			const glm::vec4& color,
			float thickness = 0.5f);
		void draw_line(
			const glm::vec2& from,
			const glm::vec2& to,
			const glm::vec4& color,
			float thickness = 0.5f);
		void draw_rectangle(
			const thoreau::TileCoordinate& a,
			const thoreau::TileCoordinate& b,
			const glm::vec4& color,
			float outline = 0.5f);
		void draw_rectangle(
			const glm::vec2& a,
			const glm::vec2& b,
			const glm::vec4& color,
			float outline = 0.5f);
		void draw_circle(
			const thoreau::TileCoordinate& center,
			float radius,
			const glm::vec4& color,
			float outline = 0.5f);
		void draw_circle(
			const glm::vec2& center,
			float radius,
			const glm::vec4& color,
			float outline = 0.5f);
		void draw_map_shape(
			const thoreau::Shape& shape,
			const glm::vec4& color,
			float outline = 0.5f);

	protected:
		virtual void render(ImDrawList& draw_list) = 0;

	private:
		ImDrawList draw_list;
	};
}

#endif
