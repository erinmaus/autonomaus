// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include <fstream>
#include "mantorok/io/datWriter.hpp"
#include "chomsky/io/world/query/materialSetMatch.hpp"
#include "chomsky/world/query/actorMaterialSetMatchExecutor.hpp"
#include "chomsky/world/query/staticMaterialSetMatchExecutor.hpp"
#include "chomsky/world/query/materialDefinition.hpp"
#include "principle/deps/imgui/imgui.h"
#include "zeitgeist/application.hpp"

zeitgeist::Application::MaterialSetMatchBuilderWindow::MaterialSetMatchBuilderWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void zeitgeist::Application::MaterialSetMatchBuilderWindow::add(
	const chomsky::MaterialSet& materials)
{
	MaterialSetMatch match;
	match.name = std::to_string(matches.size() + 1);

	auto& texture_manager = this->application->get_game_manager().get_texture_manager();
	auto& fingerprint_manager = this->application->get_game_manager().get_texture_fingerprint_manager();

	for (auto material: materials)
	{
		auto texture_name = std::get<chomsky::MaterialSet::TEXTURE>(material);
		auto region_name = std::get<chomsky::MaterialSet::REGION>(material);
		auto color = std::get<chomsky::MaterialSet::COLOR>(material);

		chomsky::MaterialDefinition definition;
		auto fingerprint = fingerprint_manager.get(texture_name, region_name);
		definition.set_fingerprint(fingerprint);
		definition.set_color(color);

		float fingerprint_area = fingerprint.get_width() * fingerprint.get_height();
		definition.set_max_dissimilarity(this->default_max_dissimilarity * (fingerprint_area / 2.0f));
		definition.set_max_difference_threshold(this->default_max_difference_threshold);
		definition.set_max_color_difference(this->default_max_color_difference / 255.0f);

		match.value.add(definition);

		auto texture_definition = texture_manager.get_texture(texture_name);
		auto texture_region = texture_definition.get_region_from_name(region_name);
		auto key = std::make_pair(texture_name, region_name);

		auto texture = std::make_shared<kvncer::Texture>(
			kvncer::Texture::component_format_rgba, false,
			texture_region->get_width(), texture_region->get_height());
		auto pixels = std::make_unique<std::uint8_t[]>(texture->get_width() * texture->get_height() * 4);
		texture_region->get_pixels(0, 0, texture->get_width(), texture->get_height(), pixels.get());
		texture->update(
			0, 0, texture->get_width(), texture->get_height(),
			kvncer::Texture::component_format_rgba,
			kvncer::Texture::component_type_unsigned_byte, pixels.get());

		match.materials.emplace(key, texture);
		match.texture_regions.push_back(key);
	}

	this->matches.push_back(match);

	open();
}

void zeitgeist::Application::MaterialSetMatchBuilderWindow::add(
	const thoreau::WorldCoordinate& location)
{
	auto& game_manager = this->application->get_game_manager();
	auto& materials_state = game_manager.get_actor_materials_state();
	if (!materials_state.has(location))
	{
		return;
	}

	auto& materials = materials_state.get(location);
	add(materials);
}

std::string zeitgeist::Application::MaterialSetMatchBuilderWindow::get_title() const
{
	return "Material Set Match Builder";
}

void zeitgeist::Application::MaterialSetMatchBuilderWindow::update_gui()
{
	if (ImGui::Button("Clear"))
	{
		ImGui::OpenPopup(get_id("confirm_clear").c_str());
	}

	ImGui::SameLine();
	if (ImGui::Button("Generate Inclusive Match"))
	{
		if (!create_inclusive_match())
		{
			ImGui::OpenPopup(get_id("inclusive_match_failure").c_str());
		}
	}

	if (ImGui::CollapsingHeader("Defaults"))
	{
		ImGui::DragFloat3("Max Dissimilarity", &this->default_max_dissimilarity.x, 0.01f, 0.0f);
		ImGui::DragFloat3("Max Color Difference", &this->default_max_color_difference.x, 1.0f, 0.0f, 255.0f);
		ImGui::DragFloat3("Max Difference", &this->default_max_difference_threshold.x, 0.01f, 0.0f);
	}

	ImGui::Text("Queries");

	int id = 0;
	int pending_removal = -1;
	for (auto& match: this->matches)
	{
		ImGui::Spacing();
		ImGui::Separator();

		ImGui::PushID(id);

		if (ImGui::CollapsingHeader(match.name.c_str()))
		{
			char name[256];
			std::strncpy(name, match.name.c_str(), sizeof(name));
			if (ImGui::InputText("Name", name, sizeof(name)))
			{
				match.name = name;
			}

			ImGui::BeginGroup();
			int index = 0;
			int remove_index = -1;
			for (auto& definition: match.value)
			{
				ImGui::PushID(index);

				auto texture = match.materials[match.texture_regions[index]].get();
				ImGui::Image(
					texture,
					ImVec2(128, 128));
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Image(
						texture,
						ImVec2(texture->get_width(), texture->get_height()));
					ImGui::EndTooltip();
				}

				if (ImGui::Button("Remove"))
				{
					remove_index = index;
				}

				auto max_dissimilarity = definition.get_max_dissimilarity();
				if (ImGui::InputFloat3("Max Dissimilarity", &max_dissimilarity.x))
				{
					definition.set_max_dissimilarity(max_dissimilarity);
				}

				auto max_color_difference = definition.get_max_color_difference();
				max_color_difference *= 255.0f;
				if (ImGui::InputFloat3("Max Color Difference", &max_color_difference.x))
				{
					definition.set_max_color_difference(max_color_difference / 255.0f);
				}

				auto max_difference_threshold = definition.get_max_difference_threshold();
				if (ImGui::InputFloat3("Max Difference", &max_difference_threshold.x))
				{
					definition.set_max_difference_threshold(max_difference_threshold);
				}

				ImGui::PopID();

				++index;
			}
			ImGui::EndGroup();

			if (remove_index >= 0)
			{
				match.value.remove(remove_index);
				match.texture_regions.erase(match.texture_regions.begin() + remove_index);
			}
		}

		if (ImGui::Button("Export"))
		{
			auto filename = this->application->get_output_filename();
			if (!filename.empty())
			{
				std::ofstream stream(filename, std::ios::binary);
				mantorok::DATWriter writer(stream);
				mantorok::DATSerializer<chomsky::MaterialSetMatch>::serialize(
					match.value, writer);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Test (Animated)"))
		{
			chomsky::ActorMaterialSetMatchExecutor executor(
				&this->application->get_game_manager().get_animated_block(),
				this->application->get_game_manager().get_texture_fingerprint_manager());

			bool results = false;
			for (auto& i: this->application->get_game_manager().get_actor_materials_state())
			{
				if (executor.execute(i.first, match.value))
				{
					this->application->query_results_window.add_world(i.first);
					results = true;
				}
			}

			if (!results)
			{
				ImGui::OpenPopup(get_id("query_test_failure").c_str());
			}
			else
			{
				this->application->query_results_window.open();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Test (Static)"))
		{
			chomsky::StaticMaterialSetMatchExecutor executor(
				&this->application->get_game_manager().get_static_block(),
				this->application->get_game_manager().get_texture_fingerprint_manager());

			bool results = false;
			for (auto& i: this->application->get_game_manager().get_scenery_materials_state())
			{
				if (executor.execute(i.first, match.value))
				{
					this->application->query_results_window.add_tile(i.first);
					results = true;
				}
			}

			if (!results)
			{
				ImGui::OpenPopup(get_id("query_test_failure").c_str());
			}
			else
			{
				this->application->query_results_window.open();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			pending_removal = id;
		}

		ImGui::PopID();
		++id;
	}

	if (pending_removal >= 0)
	{
		this->matches.erase(this->matches.begin() + pending_removal);
	}

	if (ImGui::BeginPopup(get_id("confirm_clear").c_str()))
	{
		ImGui::Text("Are you sure you wish to clear the material queries?");
		if (ImGui::Button("Yes"))
		{
			this->matches.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(get_id("inclusive_match_failure").c_str()))
	{
		ImGui::Text("Failed to create minimum inclusive match for current queries.");

		ImGui::SameLine();
		if (ImGui::Button("Alright"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(get_id("query_test_failure").c_str()))
	{
		ImGui::Text("No matches found while testing query.");

		ImGui::SameLine();
		if (ImGui::Button("Alright"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

bool zeitgeist::Application::MaterialSetMatchBuilderWindow::create_inclusive_match()
{
	auto& fingerprint_manager = this->application->get_game_manager().get_texture_fingerprint_manager();

	typedef std::pair<int, int> TextureRegion;
	typedef std::pair<glm::vec3, glm::vec3> Color;
	typedef std::shared_ptr<kvncer::Texture> MaterialPointer;

	std::map<TextureRegion, Color> colors;
	std::map<TextureRegion, std::size_t> hits;
	std::map<TextureRegion, MaterialPointer> materials;

	for (auto& match: this->matches)
	{
		std::size_t index = 0;
		for (auto& definition: match.value)
		{
			auto texture = match.texture_regions[index];
			auto color = definition.get_color();

			auto iter = colors.find(texture);
			if (iter == colors.end())
			{
				colors[texture] = std::make_pair(color, color);
				hits[texture] = 1;
				materials[texture] = match.materials[texture];
			}
			else
			{
				auto& color_range = iter->second;
				color_range.first = glm::min(color_range.first, color);
				color_range.second = glm::max(color_range.second, color);

				++hits[texture];
			}

			++index;
		}
	}

	MaterialSetMatch result;
	result.name = "inclusive minimum";

	for (auto& hit: hits)
	{
		if (hit.second < this->matches.size())
		{
			continue;
		}

		auto texture_name = hit.first.first;
		auto region_name = hit.first.second;
		auto color = colors[hit.first];

		chomsky::MaterialDefinition definition;
		definition.set_fingerprint(fingerprint_manager.get(texture_name, region_name));
		definition.set_color((color.first + color.second) / 2.0f);
		definition.set_max_color_difference(color.second - color.first);
		definition.set_max_dissimilarity(this->default_max_dissimilarity);
		definition.set_max_difference_threshold(this->default_max_difference_threshold);

		result.texture_regions.push_back(hit.first);
		result.materials.emplace(hit.first, materials[hit.first]);
		result.value.add(definition);
	}

	if (result.value.empty())
	{
		return false;
	}

	this->matches.push_back(result);
	return true;
}
