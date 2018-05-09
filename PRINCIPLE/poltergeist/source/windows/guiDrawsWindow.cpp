// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "chomsky/chomsky.hpp"
#include "poltergeist/application.hpp"
#include "principle/deps/imgui/imgui.h"

poltergeist::Application::GUIDrawsWindow::GUIDrawsWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	open();
}

std::string poltergeist::Application::GUIDrawsWindow::get_title() const
{
	return "GUI Element Stream";
}

void poltergeist::Application::GUIDrawsWindow::update_gui()
{
	auto& element_stream = this->application->get_element_stream(application->current_framebuffer);
	ImGui::Text("Elements: %lu\n", element_stream.size());

	ImGui::Text("Stream: %-4d", this->application->current_framebuffer);
	ImGui::SameLine();
	if (ImGui::SmallButton("<-"))
	{
		auto iter = this->application->element_streams.find(this->application->current_framebuffer);
		int next_framebuffer = 0;
		if (iter == this->application->element_streams.begin())
		{
			next_framebuffer = this->application->element_streams.rbegin()->first;
		}
		else
		{
			--iter;
			next_framebuffer = iter->first;
		}

		this->application->current_framebuffer = next_framebuffer;
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("->"))
	{
		auto iter = this->application->element_streams.find(this->application->current_framebuffer);
		int next_framebuffer = 0;
		++iter;
		if (iter == this->application->element_streams.end())
		{
			next_framebuffer = this->application->element_streams.begin()->first;
		}
		else
		{
			next_framebuffer = iter->first;
		}

		this->application->current_framebuffer = next_framebuffer;
	}

	ImGui::Checkbox("Specialized Info", &this->show_specialized_info);
	ImGui::Checkbox("Element Info", &this->show_element_info);
	ImGui::Checkbox("Vertices", &this->show_vertex_info);

	auto& texture_manager = this->application->get_game_manager().get_texture_manager();
	auto& texture_resource_manager = this->application->get_game_manager().get_texture_resource_manager();
	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();

	std::size_t i = 0;
	for (auto& element: element_stream)
	{
		ImGui::Spacing();

		ImGui::PushID(i);
		if (this->application->current_focus == i &&
			this->application->previous_focus != i)
		{
			this->application->previous_focus = i;
			ImGui::SetScrollHere();
		}

		if (this->application->current_focus == i)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
		}

		if (element.get_is_textured())
		{
			auto texture = texture_manager.get_texture(element.get_texture_name());
			auto region = texture.get_region_from_name(element.get_texture_region_name());
			if (region != nullptr)
			{
				auto texture_resource = texture_resource_manager.get_texture(element.get_texture_name());
				int cropped_width =  std::min((int)ImGui::GetWindowWidth(), region->get_width());
				int cropped_height = std::min(region->get_height(), 48);
				float left = region->get_x();
				float right = region->get_x() + region->get_width();
				float cropped_right = region->get_x() + cropped_width;
				float top = region->get_y() + region->get_height();
				float cropped_top = region->get_y() + cropped_height;
				float bottom = region->get_y();

				left /= texture.get_width();
				right /= texture.get_width();
				cropped_right /= texture.get_width();
				top /= texture.get_height();
				cropped_top /= texture.get_height();
				bottom /= texture.get_height();

				ImGui::Image(
					texture_resource,
					ImVec2(cropped_width, cropped_height),
					ImVec2(left, bottom), ImVec2(cropped_right, cropped_top));
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Image(
						texture_resource,
						ImVec2(region->get_width(), region->get_height()),
						ImVec2(left, bottom), ImVec2(right, top));
					ImGui::EndTooltip();
					this->application->set_highlight(i);
				}
			}

			std::string label = element.get_key();
			if (label == "")
			{
				label = "(empty)";
			}
			else if (texture_dictionary.has(label))
			{
				label = texture_dictionary.get(label).get_value();
			}
			ImGui::Text("#%4lu: %s (texture %d, region %d)",
				i, label.c_str(),
				element.get_texture_name(), element.get_texture_region_name());

			if (this->application->current_focus == i)
			{
				ImGui::PopStyleColor();
			}

			ImGui::Button("Highlight");
			if (ImGui::IsItemHovered())
			{
				this->application->set_highlight(i);
			}

			ImGui::SameLine();
			if (ImGui::Button("Edit") && region != nullptr)
			{
				std::string key = element.get_key();
				if (!texture_dictionary.has(key) ||
					!texture_dictionary.get(key).has_tag("runtime"))
				{
					this->application->edit_texture_definition_window.clear();
					this->application->edit_texture_definition_window.add(
						texture.get_name(), region->get_name());
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Split Edit") && region != nullptr)
			{
				std::string key = element.get_key();
				if (!texture_dictionary.has(key) ||
					!texture_dictionary.get(key).has_tag("runtime"))
				{
					this->application->edit_texture_definition_window.clear();
					this->application->edit_texture_definition_window.add(*region);
				}
			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(
					"Forces creating a new texture key.\n\n"
					"For when a texture fingerprint accidentally\n"
					"matches a different texture.");
			}

			ImGui::SameLine();
			if (ImGui::Button("Batch Edit") && region != nullptr)
			{
				this->application->edit_texture_definition_window.add(
					texture.get_name(), region->get_name());
			}
		}
		else
		{
			ImGui::Text("#%4lu: (untextured)", i);

			if (this->application->current_focus == i)
			{
				ImGui::PopStyleColor();
			}

			ImGui::Button("Highlight");
			if (ImGui::IsItemHovered())
			{
				this->application->set_highlight(i);
			}
		}

		if (this->show_specialized_info && element.get_is_textured())
		{
			std::string key = element.get_key();
			if (texture_dictionary.has(key))
			{
				auto& definition = texture_dictionary.get(key);
				if (definition.has_tag("font") && !definition.has_tag("x-font-skip"))
				{
					auto texture = texture_manager.get_texture(element.get_texture_name());
					auto region = texture.get_region_from_name(element.get_texture_region_name());

					int cell_width = region->get_width() / chomsky::config::FONT_NUM_COLUMNS;
					int cell_height = region->get_height() / chomsky::config::FONT_NUM_ROWS;

					if (cell_width == 0 || cell_height == 0)
					{
						continue;
					}

					float s = element.get_vertex(chomsky::GUIElement::TOP_LEFT).texture_coordinate.s;
					float t = element.get_vertex(chomsky::GUIElement::TOP_LEFT).texture_coordinate.t;

					int x = s * texture.get_width() - region->get_x();
					int y = t * texture.get_height() - region->get_y();

					int row = x / cell_width;
					int column = y / cell_height;

					char c = column * chomsky::config::FONT_NUM_COLUMNS + row;
					switch (c)
					{
						case '\n':
							ImGui::BulletText("Character: (newline)");
							break;
						case ' ':
							ImGui::BulletText("Character: (space)");
							break;
						case '\xa0':
							ImGui::BulletText("Character: (non-breaking space)");
							break;
						default:
							ImGui::BulletText("Character: %c", c);
							break;
					}
				}
				else if (definition.get_value() == "item")
				{
					ImGui::BulletText("Name: %s", definition.has_data("item-name") ? definition.get_data("item-name").c_str() : "(unnamed)");
					ImGui::BulletText("Noted: %s", definition.has_tag("item-noted") ? "yes" : "no");
					ImGui::BulletText("Quantity: %s", definition.get_data("item-quantity").c_str());
					ImGui::Bullet();
					ImGui::TextWrapped("Key: %s", definition.get_data("item-key").c_str());
				}
			}
		}

		if (this->show_element_info)
		{
			ImGui::BulletText("Position: (%d, %d)", element.get_x(), element.get_y());
			ImGui::BulletText("Size: (%d, %d)", element.get_width(), element.get_height());
			ImGui::BulletText("Rotation: %d", (int)(element.get_rotation() * (180 / M_PI)));
			ImGui::BulletText(
					"Color: [%d, %d, %d, %d]",
					(int)(element.get_color().r * 255),
					(int)(element.get_color().g * 255),
					(int)(element.get_color().b * 255),
					(int)(element.get_color().a * 255));

			ImGui::SameLine();
			ImGui::ColorButton(ImVec4(
				element.get_color().r,
				element.get_color().g,
				element.get_color().b,
				element.get_color().a));
		}

		if (this->show_vertex_info)
		{
			for (std::size_t i = 0; i < chomsky::GUIElement::NUM_VERTICES; ++i)
			{
				auto vertex = element.get_vertex(i);
				ImGui::BulletText("vertex[%lu]:", i);
				ImGui::Indent();
				{
					ImGui::BulletText("Position: (%d, %d)", vertex.position.x, vertex.position.y);

					if (texture_manager.has_texture(element.get_texture_name()))
					{
						auto texture = texture_manager.get_texture(element.get_texture_name());
						ImGui::BulletText("Texture Coordinate (pixels): (%.0f, %.0f)",
							vertex.texture_coordinate.s * texture.get_width(),
							vertex.texture_coordinate.t * texture.get_height());
						ImGui::BulletText("Wrapped Texture Coordinate (pixels): (%.0f, %.0f)",
							vertex.wrapped_texture_coordinate.s * texture.get_width(),
							vertex.wrapped_texture_coordinate.t * texture.get_height());
						ImGui::BulletText("Atlas Min (pixels): (%.0f, %.0f)",
							vertex.atlas_min.s * texture.get_width(),
							vertex.atlas_min.t * texture.get_height());
						ImGui::BulletText("Atlas Max (pixels): (%.0f, %.0f)",
							vertex.atlas_max.s * texture.get_width(),
							vertex.atlas_max.t * texture.get_height());
					}
					else
					{
						auto texture = texture_manager.get_texture(element.get_texture_name());
						ImGui::BulletText("Texture Coordinate (texels): (%f, %f)",
							vertex.texture_coordinate.s,
							vertex.texture_coordinate.t);
						ImGui::BulletText("Wrapped Texture Coordinate (texels): (%f, %f)",
							vertex.wrapped_texture_coordinate.s,
							vertex.wrapped_texture_coordinate.t);
						ImGui::BulletText("Atlas Min (texels): (%.0f, %.0f)",
							vertex.atlas_min.s,
							vertex.atlas_min.t);
						ImGui::BulletText("Atlas Max (texels): (%.0f, %.0f)",
							vertex.atlas_max.s,
							vertex.atlas_max.t);
					}

					ImGui::BulletText(
							"Color: [%d, %d, %d, %d]",
							(int)(vertex.color.r * 255),
							(int)(vertex.color.g * 255),
							(int)(vertex.color.b * 255),
							(int)(vertex.color.a * 255));
					ImGui::SameLine();
					ImGui::ColorButton(ImVec4(
						vertex.color.r,
						vertex.color.g,
						vertex.color.b,
						vertex.color.a));
				}
				ImGui::Unindent();
			}
		}

		++i;
		ImGui::PopID();
	}
}
