// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <allegro5/allegro5.h>
#include "chomsky/io/gui/itemIconSilhouette.hpp"
#include "poltergeist/application.hpp"
#include "principle/deps/imgui/imgui.h"

poltergeist::Application::EditTextureDefinitionWindow::EditTextureDefinitionWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void poltergeist::Application::EditTextureDefinitionWindow::add(
	const kompvter::TextureRegion& region)
{
	auto hash = chomsky::TextureHash::hash(region);

	auto root_path = std::getenv("NECRONOMICON_DATA_PATH");
	std::string path;
	if (root_path == nullptr)
	{
		path = this->application->get_output_filename();
	}
	else
	{
		path = root_path;
		path += "/../samples/textures/";
		path += hash.to_string();
		path += ".dat";
	}

	if (path.empty())
	{
		return;
	}

	std::ofstream stream(path);
	mantorok::DATWriter writer(stream);
	mantorok::DATSerializer<kompvter::TextureRegion>::serialize(region, writer);
	if (stream.bad())
	{
		return;
	}

	Entry entry;
	entry.texture_name = 0;
	entry.region_name = 0;
	entry.key = hash.to_string();

	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	if (this->entries.empty() && texture_dictionary.has(entry.key))
	{
		auto& definition = texture_dictionary.get(entry.key);
		this->value = definition.get_value();
	}
	else
	{
		int block_width = region.get_width() / chomsky::TextureFingerprintManager::DEFAULT_NUM_COLUMNS;
		int block_height = region.get_height() / chomsky::TextureFingerprintManager::DEFAULT_NUM_ROWS;
		if (block_width == 0)
		{
			block_width = region.get_width();
		}
		if (block_height == 0)
		{
			block_height = region.get_height();
		}
		auto fingerprint = chomsky::TextureFingerprint::generate(
			region, block_width, block_height,
			chomsky::TextureFingerprintManager::DEFAULT_COLOR_RANGE);

		auto& definition = texture_dictionary.add(entry.key, fingerprint);
		definition.set_value(this->value);
	}
	this->entries.push_back(entry);

	principle::Window::open();
}

void poltergeist::Application::EditTextureDefinitionWindow::add(
	int texture_name, int region_name)
{
	auto& texture_manager = this->application->get_game_manager().get_texture_manager();
	auto& texture_fingerprint_manager = this->application->get_game_manager().get_texture_fingerprint_manager();
	auto texture = texture_manager.get_texture(texture_name);
	auto region = texture.get_region_from_name(region_name);

	if (region == nullptr)
	{
		return;
	}

	auto hash = chomsky::TextureHash::hash(*region);

	auto root_path = std::getenv("NECRONOMICON_DATA_PATH");
	std::string path;
	if (root_path == nullptr)
	{
		path = this->application->get_output_filename();
	}
	else
	{
		path = root_path;
		path += "/../samples/textures/";
		path += hash.to_string();
		path += ".dat";
	}

	if (path.empty())
	{
		return;
	}

	std::ofstream stream(path);
	mantorok::DATWriter writer(stream);
	mantorok::DATSerializer<kompvter::TextureRegion>::serialize(*region, writer);
	if (stream.bad())
	{
		return;
	}

	Entry entry;
	entry.texture_name = texture_name;
	entry.region_name = region_name;
	entry.key = hash.to_string();

	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	if (this->entries.empty() && texture_dictionary.has(entry.key))
	{
		auto& definition = texture_dictionary.get(entry.key);
		this->value = definition.get_value();
	}
	else
	{
		auto fingerprint = texture_fingerprint_manager.get(
			texture_name, region_name);
		auto& definition = texture_dictionary.add(entry.key, fingerprint);
		definition.set_value(this->value);
	}
	this->entries.push_back(entry);

	auto& texture_key_manager = this->application->get_game_manager().get_texture_key_manager();
	texture_key_manager.add(texture_name, region_name, entry.key);

	principle::Window::open();
}

void poltergeist::Application::EditTextureDefinitionWindow::add(const chomsky::ItemIconSilhouette& silhouette)
{
	// TODO: Export sample.

	auto root_path = std::getenv("NECRONOMICON_DATA_PATH");
	std::string path;
	if (root_path != nullptr)
	{
		path = root_path;
		path += "/items/";
		path = this->application->get_output_filename(root_path);
	}
	else
	{
		path = this->application->get_output_filename();
	}

	if (path.empty())
	{
		return;
	}

	std::ofstream stream(path);
	mantorok::DATWriter writer(stream);
	mantorok::DATSerializer<chomsky::ItemIconSilhouette>::serialize(
		silhouette, writer);
	if (stream.bad())
	{
		return;
	}

	principle::Window::open();

	Entry entry;
	entry.texture_name = 0;
	entry.region_name = 0;
	entry.key = silhouette.to_string();

	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	if (!texture_dictionary.has(entry.key))
	{
		auto& definition = texture_dictionary.add(entry.key);
		definition.set_value("item");
	}

	this->entries.push_back(entry);
}

void poltergeist::Application::EditTextureDefinitionWindow::clear()
{
	this->entries.clear();

	this->value.clear();
	this->pending_input.clear();
}

void poltergeist::Application::EditTextureDefinitionWindow::close()
{
	clear();
	this->application->get_game_manager().save_texture_dictionary();
	for (auto& i: this->application->element_streams)
	{
		i.second.refresh();
	}
	principle::Window::close();
}

std::string poltergeist::Application::EditTextureDefinitionWindow::get_title() const
{
	return "Edit Texture Definition";
}

void poltergeist::Application::EditTextureDefinitionWindow::update_gui()
{
	auto& texture_manager = this->application->get_game_manager().get_texture_manager();
	auto& texture_resource_manager = this->application->get_game_manager().get_texture_resource_manager();
	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();

	if ((this->entries.size() == 1 && ImGui::Button("Delete Entry") ||
		(this->entries.size() > 1 && ImGui::Button("Delete Entries"))))
	{
		ImGui::OpenPopup(get_id("confirm_remove").c_str());
	}

	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		this->application->get_game_manager().save_texture_dictionary();
	}

	char value[256] = { 0 };
	std::strcpy(value, this->value.c_str());
	if (ImGui::InputText("Value", value, sizeof(value)))
	{
		this->value = value;
		update_value();
	}

	char input[256] = { 0 };
	std::strcpy(input, this->pending_input.c_str());
	if (ImGui::InputText("Entry", input, sizeof(input)))
	{
		pending_input = input;
	}

	if (ImGui::Button("+ Tag"))
	{
		if (!this->pending_input.empty())
		{
			add_tag(this->pending_input);
			this->pending_input.clear();
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("+ Data") && !this->pending_input.empty())
	{
		auto split = this->pending_input.find('=');
		if (split != std::string::npos &&
			split != 0 &&
			split != this->pending_input.size() - 1)
		{
			auto left = this->pending_input.substr(0, split);
			auto right = this->pending_input.substr(split + 1);

			add_data(left, right);
			this->pending_input.clear();
		}
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Entry should be in the form key=value.\nSpaces aren't trimmed.");
	}

	if (ImGui::CollapsingHeader("Entries"))
	{
		ImGui::Checkbox("Minimal View", &this->minimal_entries_view);

		int remove_index = -1;
		int index = 0;
		for (auto& entry: this->entries)
		{
			auto& definition = texture_dictionary.get(entry.key);

			ImGui::PushID(index);
			ImGui::BulletText(
				"%.8s (texture: %2d, region: %4d)",
				entry.key.c_str(),
				entry.texture_name, entry.region_name);

			ImGui::Indent();
			ImGui::Button("Preview");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				auto texture = texture_manager.get_texture(entry.texture_name);
				auto region = texture.get_region_from_name(entry.region_name);

				if (region != nullptr)
				{
					auto texture_resource = texture_resource_manager.get_texture(entry.texture_name);
					float left = region->get_x();
					float right = region->get_x() + region->get_width();
					float top = region->get_y() + region->get_height();
					float bottom = region->get_y();

					left /= texture.get_width();
					right /= texture.get_width();
					top /= texture.get_height();
					bottom /= texture.get_height();

					ImGui::Image(
						texture_resource,
						ImVec2(region->get_width(), region->get_height()),
						ImVec2(left, bottom), ImVec2(right, top));
				}

				ImGui::EndTooltip();
			}

			ImGui::SameLine();
			ImGui::Button("View Key");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::BeginChildFrame(0, ImVec2(256, 160));
				ImGui::TextWrapped("%s", entry.key.c_str());
				ImGui::EndChildFrame();
				ImGui::EndTooltip();
			}

			ImGui::SameLine();
			if (ImGui::Button("Copy Key"))
			{
				ImGui::SetClipboardText(entry.key.c_str());
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove"))
			{
				remove_index = index;
			}
			ImGui::Unindent();

			ImGui::BeginGroup();
			if (definition.tags_begin() != definition.tags_end() &&
				(!this->minimal_entries_view || ImGui::CollapsingHeader("Tags")))
			{
				if (!this->minimal_entries_view)
				{
					ImGui::Text("Tags");
					ImGui::Separator();
				}

				std::string tag_pending_removal;
				for (auto i = definition.tags_begin(); i != definition.tags_end(); ++i)
				{
					ImGui::PushID(i->c_str());

					ImGui::Bullet();
					ImGui::SameLine();
					if (ImGui::SmallButton("-"))
					{
						tag_pending_removal = *i;
					}

					ImGui::SameLine();
					ImGui::TextWrapped("%s", i->c_str());

					ImGui::PopID();
				}

				if (!tag_pending_removal.empty())
				{
					definition.remove_tag(tag_pending_removal);
				}
			}
			ImGui::EndGroup();

			ImGui::BeginGroup();
			if (definition.data_begin() != definition.data_end() &&
				(!this->minimal_entries_view || ImGui::CollapsingHeader("Data")))
			{
				if (!this->minimal_entries_view)
				{
					ImGui::Text("Data");
					ImGui::Separator();
				}

				std::string data_pending_removal;
				for (auto i = definition.data_begin(); i != definition.data_end(); ++i)
				{
					ImGui::PushID(i->first.c_str());

					ImGui::Bullet();

					ImGui::SameLine();
					if (ImGui::SmallButton("-"))
					{
						data_pending_removal=  i->first;
					}

					ImGui::SameLine();
					ImGui::TextWrapped("%s = %s", i->first.c_str(), i->second.c_str());

					ImGui::PopID();
				}

				if (!data_pending_removal.empty())
				{
					definition.remove_data(data_pending_removal);
				}
			}
			ImGui::EndGroup();

			ImGui::PopID();
		}

		if (remove_index >= 0)
		{
			this->entries.erase(this->entries.begin() + remove_index);
		}
	}

	if (ImGui::BeginPopup(get_id("confirm_remove").c_str()))
	{
		if (this->entries.size() == 1)
		{
			ImGui::Text("Are you sure you wish to remove this entry?");
		}
		else
		{
			ImGui::Text("Are you sure you wish to remove these entries?");
		}

		if (ImGui::Button("Yes"))
		{
			auto& texture_key_manager = this->application->get_game_manager().get_texture_key_manager();
			for (auto& entry: this->entries)
			{
				texture_dictionary.remove(entry.key);
				texture_key_manager.remove(entry.key);
			}
			close();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void poltergeist::Application::EditTextureDefinitionWindow::update_value()
{
	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	for (auto& entry: this->entries)
	{
		auto& definition = texture_dictionary.get(entry.key);
		definition.set_value(this->value);
	}

	for (auto& i: this->application->element_streams)
	{
		i.second.refresh();
	}
}

void poltergeist::Application::EditTextureDefinitionWindow::add_tag(
	const std::string& tag)
{
	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	for (auto& entry: this->entries)
	{
		auto& definition = texture_dictionary.get(entry.key);
		definition.add_tag(tag);
	}
}

void poltergeist::Application::EditTextureDefinitionWindow::add_data(
	const std::string& key, const std::string& value)
{
	auto& texture_dictionary = this->application->get_game_manager().get_texture_dictionary();
	for (auto& entry: this->entries)
	{
		auto& definition = texture_dictionary.get(entry.key);
		definition.add_data(key, value);
	}
}
