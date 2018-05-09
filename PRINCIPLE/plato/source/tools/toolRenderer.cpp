// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include "plato/tools/toolRenderer.hpp"

const float plato::ToolRenderer::NO_OUTLINE = 0.0f;

plato::ToolRenderer::ToolRenderer()
{
	this->draw_list.PushClipRectFullScreen();
	this->draw_list.PushTextureID(ImGui::GetIO().Fonts->Fonts[0]->ContainerAtlas->TexID);
}

void plato::ToolRenderer::draw()
{
	this->draw_list.PopTextureID();
	render(this->draw_list);

	this->draw_list.Clear();
	this->draw_list.PushClipRectFullScreen();
	this->draw_list.PushTextureID(ImGui::GetIO().Fonts->Fonts[0]->ContainerAtlas->TexID);
}

glm::vec2 plato::ToolRenderer::tile_to_screen(const thoreau::TileCoordinate& tile_coordinate)
{
	auto view = get_view_matrix();
	auto projection = get_projection_matrix();
	glm::vec4 viewport(0);
	viewport.z = get_viewport_width();
	viewport.w = get_viewport_height();

	glm::vec3 tile_position(tile_coordinate.x + 0.5f, tile_coordinate.y + 0.5f, 0.0f);
	auto position = glm::project(tile_position, view, projection, viewport);
	position.y = viewport.w - position.y;

	return glm::vec2(position);
}

void plato::ToolRenderer::draw_text(
	const thoreau::TileCoordinate& tile_coordinate,
	const std::string& text,
	const glm::vec4& color,
	float scale)
{
	draw_text(tile_to_screen(tile_coordinate), text, color, scale);
}

void plato::ToolRenderer::draw_text(
	const glm::vec2& position,
	const std::string& text,
	const glm::vec4& color,
	float scale)
{
	auto font = ImGui::GetIO().Fonts->Fonts[0];
	float font_size = font->FontSize * scale * std::floor(get_camera_scale()) / 4.0f;
	auto text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str());

	glm::vec2 offset;
	offset.x = text_size.x / 2.0f;
	offset.y = text_size.y / 2.0f;

	ImColor text_color(color.r, color.g, color.b, color.a);
	this->draw_list.PushTextureID(font->ContainerAtlas->TexID);
	this->draw_list.AddText(
		font, font_size,
		ImVec2(position.x - offset.x, position.y - offset.y),
		text_color, text.c_str());
	this->draw_list.PopTextureID();
}

void plato::ToolRenderer::draw_line(
	const thoreau::TileCoordinate& from,
	const thoreau::TileCoordinate& to,
	const glm::vec4& color,
	float thickness)
{
	draw_line(tile_to_screen(from), tile_to_screen(to), color, thickness);
}

void plato::ToolRenderer::draw_line(
	const glm::vec2& from,
	const glm::vec2& to,
	const glm::vec4& color,
	float thickness)
{
	auto i = ImVec2(from.x, from.y);
	auto j = ImVec2(to.x, to.y);

	ImColor line_color(color.r, color.g, color.b, color.a);
	this->draw_list.AddLine(i, j, line_color, std::max(thickness * std::floor(get_camera_scale()), 1.0f));
}

void plato::ToolRenderer::draw_rectangle(
	const thoreau::TileCoordinate& a,
	const thoreau::TileCoordinate& b,
	const glm::vec4& color,
	float outline)
{
	draw_rectangle(tile_to_screen(a), tile_to_screen(b), color, outline);
}

void plato::ToolRenderer::draw_rectangle(
	const glm::vec2& a,
	const glm::vec2& b,
	const glm::vec4& color,
	float outline)
{
	auto i = ImVec2(a.x, a.y);
	auto j = ImVec2(b.x, b.y);

	ImColor rectangle_color(color.r, color.g, color.b, color.a);
	if (outline != NO_OUTLINE)
	{
		this->draw_list.AddRect(
			i, j,
			rectangle_color,
			0, 0,
			std::max(outline * std::floor(get_camera_scale()), 1.0f));
	}
	else
	{
		this->draw_list.AddRectFilled(i, j, rectangle_color);
	}
}

void plato::ToolRenderer::draw_circle(
	const thoreau::TileCoordinate& center,
	float radius,
	const glm::vec4& color,
	float outline)
{
	draw_circle(tile_to_screen(center), radius, color, outline);
}

void plato::ToolRenderer::draw_circle(
	const glm::vec2& center,
	float radius,
	const glm::vec4& color,
	float outline)
{
	ImColor circle_color(color.r, color.g, color.b, color.a);

	if (outline != NO_OUTLINE)
	{
		this->draw_list.AddCircle(
			ImVec2(center.x, center.y),
			radius * std::floor(get_camera_scale()), circle_color,
			16,
			std::max(outline * std::floor(get_camera_scale()), 1.0f));
	}
	else
	{
		this->draw_list.AddCircleFilled(
			ImVec2(center.x, center.y),
			radius * std::floor(get_camera_scale()), circle_color,
			16);
	}
}

static void stencil_pass(const ImDrawList*, const ImDrawCmd*)
{
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NEVER, 0, 1);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
}

static void color_pass(const ImDrawList*, const ImDrawCmd*)
{
	glStencilFunc(GL_EQUAL, 1, 1);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
}

static void normal_pass(const ImDrawList*, const ImDrawCmd*)
{
	glDisable(GL_STENCIL_TEST);
}

void plato::ToolRenderer::draw_map_shape(
	const thoreau::Shape& shape,
	const glm::vec4& color,
	float outline)
{
	auto draw_path = [&]
	{
		for (std::size_t i = 0; i < shape.get_num_points(); ++i)
		{
			auto point = shape.get_point(i);
			auto point_screen = tile_to_screen(point);
			this->draw_list.PathLineTo(ImVec2(point_screen.x, point_screen.y));
		}
	};

	ImColor shape_color(color.r, color.g, color.b, color.a);
	if (outline != NO_OUTLINE)
	{
		draw_path();
		this->draw_list.PathStroke(
			shape_color, true,
			std::max(outline * std::floor(get_camera_scale()), 1.0f));
	}
	else
	{
		this->draw_list.AddCallback(stencil_pass, nullptr);
		draw_path();
		this->draw_list.PathFillConvex(shape_color);

		this->draw_list.AddCallback(color_pass, nullptr);
		draw_path();
		this->draw_list.PathFillConvex(shape_color);

		this->draw_list.AddCallback(normal_pass, nullptr);
	}
}
