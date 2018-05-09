// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/graphics/guiOverlay.hpp"

autonomaus::GUIOverlay::GUIOverlay(
	Renderer& renderer, const DrawState& draw_state) :
	Overlay(renderer, draw_state)
{
	// Nothing.
}

void autonomaus::GUIOverlay::set_draw_list(ImDrawList* value)
{
	this->draw_list = value;
}

void autonomaus::GUIOverlay::draw_text(
	const std::string& value,
	const glm::vec2& position,
	const glm::vec4& color,
	float scale)
{
	auto object = new DrawTextObject(this);
	object->text = value;
	object->position = position;
	object->color = color;
	object->scale = scale;

	add_draw(object);
}

void autonomaus::GUIOverlay::draw_text(
	const std::string& value,
	const glm::vec3& position,
	const glm::vec4& color,
	float scale)
{
	auto projected_position = glm::project(
		position,
		get_draw_state().get_camera_view_matrix(),
		get_draw_state().get_camera_projection_matrix(),
		get_draw_state().get_world_viewport());
	auto height = get_draw_state().get_gui_viewport().w;
	projected_position.y = height - projected_position.y;

	auto object = new DrawTextObject(this);
	object->text = value;
	object->position = glm::vec2(projected_position);
	object->color = color;
	object->scale = scale;

	add_draw(object);
}

void autonomaus::GUIOverlay::draw_path(
	const glm::vec2 points[],
	std::size_t count,
	const glm::vec4& color,
	float thickness,
	bool curved)
{
	auto object = new DrawPathObject(this);
	object->points.insert(object->points.begin(), points, points + count);
	object->color = color;
	object->thickness = thickness;
	object->curved = curved;

	add_draw(object);
}

void autonomaus::GUIOverlay::draw_rectangle(
	const glm::vec2& position,
	float width, float height,
	const glm::vec4& color)
{
	auto object = new DrawRectangleObject(this);
	object->position = position;
	object->width = width;
	object->height = height;
	object->color = color;
	object->thickness = HUGE_VALF;

	add_draw(object);
}

void autonomaus::GUIOverlay::draw_rectangle_outline(
	const glm::vec2& position,
	float width, float height,
	const glm::vec4& color,
	float thickness)
{
	auto object = new DrawRectangleObject(this);
	object->position = position;
	object->width = width;
	object->height = height;
	object->color = color;
	object->thickness = thickness;

	add_draw(object);
}

void autonomaus::GUIOverlay::unload_resources()
{
	// Nothing.
}

void autonomaus::GUIOverlay::load_resources()
{
	// Nothing.
}

void autonomaus::GUIOverlay::start_draw(int id, float age, float lifespan)
{
	if (age < get_fade_duration())
	{
		this->current_alpha = age / get_fade_duration();
	}
	else if (age > lifespan)
	{
		this->current_alpha = 1.0f - glm::min((age - lifespan) / get_fade_duration(), 1.0f);
	}
	else
	{
		this->current_alpha = 1.0f;
	}
}

void autonomaus::GUIOverlay::stop_draw()
{
	// Nothing.
}

autonomaus::GUIOverlay::DrawTextObject::DrawTextObject(GUIOverlay* overlay)
{
	this->overlay = overlay;
}

void autonomaus::GUIOverlay::DrawTextObject::draw()
{
	if (overlay->draw_list == nullptr)
	{
		return;
	}

	auto font = ImGui::GetIO().Fonts->Fonts[0];
	float font_size = font->FontSize * this->scale;
	auto text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, this->text.c_str());

	glm::vec2 offset;
	offset.x = text_size.x / 2.0f;
	offset.y = text_size.y / 2.0f;

	ImColor text_color(
		this->color.r,
		this->color.g,
		this->color.b,
		this->color.a * this->overlay->current_alpha);

	overlay->draw_list->PushTextureID(font->ContainerAtlas->TexID);
	overlay->draw_list->AddText(
		font, font_size,
		ImVec2(this->position.x - offset.x, this->position.y - offset.y),
		text_color, this->text.c_str());
	overlay->draw_list->PopTextureID();
}

autonomaus::GUIOverlay::DrawPathObject::DrawPathObject(GUIOverlay* overlay)
{
	this->overlay = overlay;
}

void autonomaus::GUIOverlay::DrawPathObject::draw()
{
	if (overlay->draw_list == nullptr)
	{
		return;
	}

	if (this->curved && this->points.size() >= 3)
	{
		int count = 0;
		overlay->draw_list->PathLineTo(ImVec2(this->points[0].x, this->points[0].y));
		for (int i = 1; i + 2 <= this->points.size(); i += 2)
		{
			ImVec2 a = ImVec2(this->points[i - 1].x, this->points[i - 1].y);
			ImVec2 b = ImVec2(this->points[i - 0].x, this->points[i - 0].y);
			ImVec2 c = ImVec2(this->points[i + 1].x, this->points[i + 1].y);

			overlay->draw_list->PathBezierCurveTo(a, b, c);
			count = i + 2;
		}

		for (int i = count; i < this->points.size(); ++i)
		{
			overlay->draw_list->PathLineTo(ImVec2(this->points[i].x, this->points[i].y));
		}
	}
	else
	{
		for (int i = 0; i < this->points.size(); ++i)
		{
			overlay->draw_list->PathLineTo(ImVec2(this->points[i].x, this->points[i].y));
		}
	}

	ImColor path_color(
		this->color.r,
		this->color.g,
		this->color.b,
		this->color.a * this->overlay->current_alpha);

	overlay->draw_list->PathStroke(path_color, false, this->thickness);
}

autonomaus::GUIOverlay::DrawRectangleObject::DrawRectangleObject(GUIOverlay* overlay)
{
	this->overlay = overlay;
}

void autonomaus::GUIOverlay::DrawRectangleObject::draw()
{
	if (overlay->draw_list == nullptr)
	{
		return;
	}

	ImColor rectangle_color(
		this->color.r,
		this->color.g,
		this->color.b,
		this->color.a * this->overlay->current_alpha);

	auto a = ImVec2(this->position.x, this->position.y);
	auto b = ImVec2(this->position.x + this->width, this->position.y + this->height);
	if (this->thickness == HUGE_VALF)
	{
		overlay->draw_list->AddRectFilled(a, b, rectangle_color);
	}
	else
	{
		overlay->draw_list->AddRect(a, b, rectangle_color, 0, 0, this->thickness);
	}
}
