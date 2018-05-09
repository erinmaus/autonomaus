// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <tuple>
#include "kvncer/graphics/texture.hpp"
#include "principle/deps/imgui/imgui.h"
#include "zeitgeist/application.hpp"

zeitgeist::Application::MaterialsSetWindow::MaterialsSetWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void zeitgeist::Application::MaterialsSetWindow::open(
	const chomsky::MaterialSet& materials)
{
	principle::Window::open();
	this->materials = materials;
}

std::string zeitgeist::Application::MaterialsSetWindow::get_title() const
{
	return "Materials";
}

void zeitgeist::Application::MaterialsSetWindow::update_gui()
{
	std::size_t count = this->materials.count();
	ImGui::Text("Materials: %lu\n", count);

	auto& texture_manager = this->application->get_game_manager().get_texture_manager();
	auto& texture_resource_manager = this->application->get_game_manager().get_texture_resource_manager();

	int current = 0;
	const kompvter::TextureRegion* test_texture_region = nullptr;
	for (auto material: this->materials)
	{
		auto texture_name = std::get<chomsky::MaterialSet::TEXTURE>(material);
		auto texture_region = std::get<chomsky::MaterialSet::REGION>(material);
		auto color = std::get<chomsky::MaterialSet::COLOR>(material);

		float texture_left, texture_right;
		float texture_top, texture_bottom;
		const kompvter::TextureRegion* region;
		kvncer::Texture* texture;
		if (texture_manager.has_texture(texture_name))
		{
			auto t = texture_manager.get_texture(texture_name);
			region = t.get_region_from_name(texture_region);
			texture_left = region->get_x();
			texture_left /= t.get_width();
			texture_right = region->get_x() + region->get_width();
			texture_right /= t.get_width();
			texture_top = region->get_y() + region->get_height();
			texture_top /= t.get_height();
			texture_bottom = region->get_y();
			texture_bottom /= t.get_height();
			texture = texture_resource_manager.get_texture(texture_name);
		}
		else
		{
			region = nullptr;
		}

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::PushID(current);
		ImGui::Text("Material %d", current);
		ImGui::Text("Texture %d (region %d)", texture_name, texture_region);
		if (region != nullptr && texture != nullptr)
		{
			ImGui::Image(
				texture,
				ImVec2(128, 128),
				ImVec2(texture_left, texture_top),
				ImVec2(texture_right, texture_bottom));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image(
					texture,
					ImVec2(region->get_width(), region->get_height()),
					ImVec2(texture_left, texture_top),
					ImVec2(texture_right, texture_bottom));
				ImGui::EndTooltip();
			}

			ImGui::SameLine();
			ImGui::Image(
				texture,
				ImVec2(128, 128),
				ImVec2(texture_left, texture_top),
				ImVec2(texture_right, texture_bottom),
				ImVec4(color.r, color.g, color.b, 1.0f));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Image(
					texture,
					ImVec2(region->get_width(), region->get_height()),
					ImVec2(texture_left, texture_top),
					ImVec2(texture_right, texture_bottom),
					ImVec4(color.r, color.g, color.b, 1.0f));
				ImGui::EndTooltip();
			}
		}

		ImGui::Text("Color: (%.0f, %.0f, %.0f)",
			color.r * 255,
			color.g * 255,
			color.b * 255);
		ImGui::SameLine();
		ImGui::ColorButton(ImVec4(color.r, color.g, color.b, 1.0f));

		ImGui::PopID();

		++current;
	}
}
