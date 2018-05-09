// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_GUI_OVERLAY_HPP
#define AUTONOMAUS_GRAPHICS_GUI_OVERLAY_HPP

#include <string>
#include <glm/glm.hpp>
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/overlay.hpp"

namespace autonomaus
{
	class GUIOverlay : public Overlay
	{
	public:
		GUIOverlay(
			Renderer& renderer, const DrawState& draw_state);
		GUIOverlay() = default;

		void set_draw_list(ImDrawList* value);

		void draw_text(
			const std::string& value,
			const glm::vec2& position,
			const glm::vec4& color = glm::vec4(1.0f),
			float scale = 1.0f);
		void draw_text(
			const std::string& value,
			const glm::vec3& position,
			const glm::vec4& color = glm::vec4(1.0f),
			float scale = 1.0f);
		void draw_path(
			const glm::vec2 points[],
			std::size_t count,
			const glm::vec4& color = glm::vec4(1.0f),
			float thickness = 4.0f,
			bool curved = true);
		void draw_rectangle(
			const glm::vec2& position,
			float width, float height,
			const glm::vec4& color = glm::vec4(1.0f));
		void draw_rectangle_outline(
			const glm::vec2& position,
			float width, float height,
			const glm::vec4& color = glm::vec4(1.0f),
			float thickness = 4.0f);

		void unload_resources() override;
		void load_resources() override;

	protected:
		struct DrawTextObject : public DrawObject
		{
			DrawTextObject(GUIOverlay* overlay);
			void draw() override;

			GUIOverlay* overlay = nullptr;
			std::string text;
			glm::vec2 position = glm::vec2(0.0f);
			glm::vec4 color = glm::vec4(1.0f);
			float scale = 1.0f;
		};

		struct DrawPathObject : public DrawObject
		{
			DrawPathObject(GUIOverlay* overlay);
			void draw() override;

			GUIOverlay* overlay = nullptr;
			std::vector<glm::vec2> points;
			glm::vec4 color = glm::vec4(1.0f);
			float thickness = 1.0f;
			bool curved = false;
		};

		struct DrawRectangleObject : public DrawObject
		{
			DrawRectangleObject(GUIOverlay* overlay);
			void draw() override;

			GUIOverlay* overlay = nullptr;
			glm::vec2 position = glm::vec2(0.0f);
			float width = 0.0f, height = 0.0f;
			glm::vec4 color = glm::vec4(1.0f);
			float thickness = HUGE_VALF;
		};

		float current_alpha;
		void start_draw(int id, float age, float lifespan) override;
		void stop_draw() override;

		ImDrawList* draw_list = nullptr;
	};
}

#endif
