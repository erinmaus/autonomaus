// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "poltergeist/application.hpp"
#include "principle/deps/imgui/imgui.h"

poltergeist::Application::DiffWindow::DiffWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

std::string poltergeist::Application::DiffWindow::get_title() const
{
	return "Element Stream Diff";
}

void poltergeist::Application::DiffWindow::update_gui()
{
	ImGui::Text("Elements: %lu\n", this->application->element_diffs.size());
	{
		int range_begin = this->application->element_diff_range_begin;
		if (ImGui::InputInt("Start", &range_begin) && range_begin >= 0)
		{
			this->application->element_diff_range_begin = range_begin;
		}

		int range_end = this->application->element_diff_range_end;
		if (ImGui::InputInt("Stop", &range_end) && range_end >= 0)
		{
			this->application->element_diff_range_end = range_end;
		}
	}

	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	auto& texture_manager = this->application->get_game_manager().get_texture_manager();

	std::size_t i = 0;
	for (auto& diff: this->application->element_diffs)
	{
		if (!diff.element.get_is_textured())
		{
			continue;
		}

		ImGui::Spacing();
		ImGui::PushID(i);

		{
			int cropped_width =  std::min((int)ImGui::GetWindowWidth(), diff.texture->get_width());
			int cropped_height = std::min(diff.texture->get_height(), 48);
			float cropped_right = cropped_width;
			float cropped_top = cropped_height;
			cropped_right /= diff.texture->get_width();
			cropped_top /= diff.texture->get_height();

			ImGui::Image(
				diff.texture.get(),
				ImVec2(cropped_width, cropped_height),
				ImVec2(0.0f, 0.0f), ImVec2(cropped_right, cropped_top));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image(
					diff.texture.get(),
					ImVec2(diff.texture->get_width(), diff.texture->get_height()));
				ImGui::EndTooltip();
			}
		}

		std::string label = diff.element.get_key();
		if (label == "")
		{
			label = "(empty)";
		}
		else if (texture_dictionary.has(label))
		{
			label = texture_dictionary.get(label).get_value();
		}
		ImGui::Text("#%4lu: %s (texture %d, region %d, frame %lu)",
			i, label.c_str(),
			diff.element.get_texture_name(),
			diff.element.get_texture_region_name(),
			diff.frame);

		ImGui::SameLine();
		if (ImGui::Button("Edit"))
		{
			this->application->edit_texture_definition_window.clear();
			this->application->edit_texture_definition_window.add(diff.region);
		}

		ImGui::SameLine();
		if (ImGui::Button("Batch Edit"))
		{
			this->application->edit_texture_definition_window.add(diff.region);
		}

		++i;
		ImGui::PopID();
	}
}
