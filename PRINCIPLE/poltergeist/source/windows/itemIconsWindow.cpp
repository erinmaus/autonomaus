// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "poltergeist/application.hpp"
#include "principle/deps/imgui/imgui.h"

poltergeist::Application::ItemIconsWindow::ItemIconsWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

std::string poltergeist::Application::ItemIconsWindow::get_title() const
{
	return "Item Icons";
}

void poltergeist::Application::ItemIconsWindow::update_gui()
{
	auto& item_icon_resource_manager = this->application->get_game_manager().get_item_icon_resource_manager();

	ImGui::Text("Working Icons: %lu\n", item_icon_resource_manager.count());
	int i = 0;
	for (auto& icon: item_icon_resource_manager)
	{
		auto hash_string = icon.get_silhouette().to_string();

		ImGui::PushID(i);
		ImGui::Image(
			icon.get_texture(),
			ImVec2(icon.get_texture()->get_width(), icon.get_texture()->get_height()),
			ImVec2(0.0f, 1.0f),
			ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Text("%d: %.16s", i, hash_string.c_str());

		ImGui::SameLine();
		if (ImGui::Button("Edit"))
		{
			this->application->edit_texture_definition_window.clear();
			this->application->edit_texture_definition_window.add(icon.get_silhouette());
		}

		ImGui::SameLine();
		if (ImGui::Button("3D View"))
		{
			this->application->item_icon_3d_view_window.open(icon);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Pointless fun! :)");
		}

		ImGui::SameLine();
		ImGui::Button("View Hash");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::BeginChildFrame(0, ImVec2(256, 160));
			ImGui::TextWrapped("%s", hash_string.c_str());
			ImGui::EndChildFrame();
			ImGui::EndTooltip();
		}

		ImGui::SameLine();
		if (ImGui::Button("Copy Hash"))
		{
			ImGui::SetClipboardText(hash_string.c_str());
		}
		ImGui::PopID();

		ImGui::Separator();
		ImGui::Spacing();

		++i;
	}
}