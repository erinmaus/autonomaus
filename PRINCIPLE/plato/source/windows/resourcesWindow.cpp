// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <fstream>
#include <sstream>
#include "principle/common/gui.hpp"
#include "principle/common/loadingScreen.hpp"
#include "principle/deps/imgui/imgui.h"
#include "plato/application.hpp"
#include "plato/import/importer.hpp"
#include "thoreau/thoreau.hpp"

plato::Application::ResourcesWindow::ResourcesWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;

	this->tabs.emplace_back("Locations", [this] { show_locations(); });
	this->tabs.emplace_back("Skills", [this] { show_skills(); });
	this->tabs.emplace_back("Items", [this] { show_items(); });
	this->tabs.emplace_back("Currencies", [this] { show_currencies(); });
	this->tabs.emplace_back("Abilities", [this] { show_abilities(); });
	this->tabs.emplace_back("Quests", [this] { show_quests(); });
	this->tabs.emplace_back("Shops", [this] { show_shops(); });

	set_can_close(false);
}

std::string plato::Application::ResourcesWindow::get_title() const
{
	return "Mapp Resources";
}

void plato::Application::ResourcesWindow::update_gui()
{
	if (!this->application->brochure)
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Brochure not loaded. Woops.");
		return;
	}

	for (std::size_t i = 0; i < this->tabs.size(); ++i)
	{
		if (i != 0)
		{
			ImGui::SameLine();
		}

		bool is_active = (i == this->current_tab);
		if (is_active)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7, 0.4, 0.4, 1.0));
		}

		if (ImGui::Button(this->tabs[i].first.c_str()))
		{
			if (!is_active)
			{
				this->previous_tab = this->current_tab;
				this->filter.clear();
				this->current_resources.clear();
				this->current_locations.clear();
			}
			this->current_tab = i;
		}

		if (is_active)
		{
			ImGui::PopStyleColor();
		}
	}

	ImGui::Separator();
	ImGui::Spacing();

	if (this->current_tab < this->tabs.size())
	{
		this->tabs[this->current_tab].second();
	}
}

void plato::Application::ResourcesWindow::show_locations()
{
	ImGui::PushItemWidth(384);
	{
		ImGui::Text("Address Filter");
		ImGui::SameLine();
		if (ImGui::InputString("##address", this->filter))
		{
			populate_locations();
		}
	}
	ImGui::PopItemWidth();

	if (this->previous_tab != this->current_tab)
	{
		populate_locations();
		this->previous_tab = this->current_tab;
	}

	for (auto& location: this->current_locations)
	{
		show_locations(location);
	}
}

void plato::Application::ResourcesWindow::show_locations(const mapp::Location& parent)
{
	ImGui::PushID(parent.resource.get_id());
	ImGui::BulletText(
		"%s (id: %d)",
		parent.resource.get_name().c_str(),
		parent.resource.get_id());

	if (ImGui::Button("Add Child"))
	{
		this->application->tool_factory.add_new_child_location(parent);
	}

	ImGui::SameLine();
	if (ImGui::Button("Edit"))
	{
		this->application->tool_factory.edit(parent.resource);
	}

	ImGui::SameLine();
	if (ImGui::Button("Focus"))
	{
		auto center_x = -(parent.min.x + (parent.max.x - parent.min.x) / 2);
		center_x += this->application->get_display()->get_width() / 2;
		auto center_y = -(parent.min.y + (parent.max.y - parent.min.y) / 2);
		center_y += this->application->get_display()->get_height() / 2;
		this->application->camera_offset.x = center_x;
		this->application->camera_offset.y = center_y;
	}

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		pending_resource = parent.resource;
		ImGui::OpenPopup("confirm_remove");
	}

	auto children = this->application->atlas->children(parent);

	ImGui::Indent();
	for (auto& child: children)
	{
		show_locations(child);
	}
	ImGui::Unindent();

	if (ImGui::BeginPopup("confirm_remove"))
	{
		mapp::Location pending_location;
		if (!this->application->atlas->location(this->pending_resource, pending_location))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::TextWrapped(
			"Are you sure you wish to remove '%s' (id: %d)?",
			this->application->atlas->address(pending_location).c_str(),
			pending_location.resource.get_id());
		ImGui::TextColored(ImVec4(1, 0, 0, 1),
			"All children nodes will be erased,"
			" and any actions that depend on them!");

		if (ImGui::Button("Yes"))
		{
			this->application->atlas->remove(pending_location);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();
}

void plato::Application::ResourcesWindow::show_skills()
{
	show_resources(
		"skill",
		[this]
		{
			mapp::SkillDefinition definition;
			definition.name = this->pending_name;

			auto result = this->application->game->add(definition);
			this->application->tool_factory.edit(result.resource);
		},
		[this]
		{
			mapp::Skill skill;
			if (this->application->game->skill(this->pending_resource, skill))
			{
				this->application->game->remove(skill);
			}
		});
}

#include "plato/import/itemImporter.hpp"

void plato::Application::ResourcesWindow::show_items()
{
	if (ImGui::Button("Import"))
	{
		ItemImporter importer;
		import("item", importer);
	}

	ImGui::SameLine();
	show_resources(
		"item",
		[this]
		{
			mapp::ItemDefinition definition;
			definition.name = this->pending_name;

			auto result = this->application->game->add(definition);
			this->application->tool_factory.edit(result.resource);
		},
		[this]
		{
			mapp::Item item;
			if (this->application->game->item(this->pending_resource, item))
			{
				this->application->game->remove(item);
			}
		});
}

void plato::Application::ResourcesWindow::show_currencies()
{
	show_resources(
		"currency",
		[this]
		{
			mapp::CurrencyDefinition definition;
			definition.name = this->pending_name;

			auto result = this->application->game->add(definition);
			this->application->tool_factory.edit(result.resource);
		},
		[this]
		{
			mapp::Currency currency;
			if (this->application->game->currency(this->pending_resource, currency))
			{
				this->application->game->remove(currency);
			}
		});
}

#include "plato/import/spellImporter.hpp"

void plato::Application::ResourcesWindow::show_abilities()
{
	if (ImGui::Button("Import Spells"))
	{
		SpellImporter importer;
		import("spell", importer);
	}

	ImGui::SameLine();
	show_resources(
		"ability",
		[this]
		{
			mapp::AbilityDefinition definition;
			definition.name = this->pending_name;

			auto result = this->application->game->add(definition);
			this->application->tool_factory.edit(result.resource);
		},
		[this]
		{
			mapp::Ability ability;
			if (this->application->game->ability(this->pending_resource, ability))
			{
				this->application->game->remove(ability);
			}
		});
}

void plato::Application::ResourcesWindow::show_quests()
{
	show_resources(
		"quest",
		[this]
		{
			mapp::QuestDefinition definition;
			definition.name = this->pending_name;

			auto result = this->application->game->add(definition);
			this->application->tool_factory.edit(result.resource);
		},
		[this]
		{
			mapp::Quest quest;
			if (this->application->game->quest(this->pending_resource, quest))
			{
				this->application->game->remove(quest);
			}
		});
}

#include "plato/import/shopImporter.hpp"

void plato::Application::ResourcesWindow::show_shops()
{
	if (ImGui::Button("Import"))
	{
		ShopImporter importer;
		import("shop", importer);
	}

	ImGui::SameLine();
	show_resources(
		"shop",
		[this]
		{
			mapp::ShopDefinition definition;
			definition.name = this->pending_name;

			auto result = this->application->game->add(definition);
			this->application->tool_factory.edit(result.resource);
		},
		[this]
		{
			mapp::Shop shop;
			if (this->application->game->shop(this->pending_resource, shop))
			{
				this->application->game->remove(shop);
			}
		});
}

void plato::Application::ResourcesWindow::show_resources(
	const std::string& type,
	const std::function<void()>& add,
	const std::function<void()>& remove)
{
	if (ImGui::Button("Add"))
	{
		ImGui::OpenPopup("add_resource");
		this->pending_name.clear();
	}

	ImGui::SameLine();
	if (ImGui::Button("Refresh"))
	{
		populate_resources(type);
	}

	ImGui::Spacing();

	ImGui::PushItemWidth(192);
	{
		ImGui::Text("Filter");
		ImGui::SameLine();
		if (ImGui::InputString("##filter", this->filter))
		{
			populate_resources(type);
		}
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(128);
	{
		ImGui::Text("Limit");
		ImGui::SameLine();
		if (ImGui::InputInt("##limit", &this->limit))
		{
			populate_resources(type);
		}
	}
	ImGui::PopItemWidth();

	if (this->previous_tab != this->current_tab)
	{
		populate_resources(type);
		this->previous_tab = this->current_tab;
	}

	for (auto& resource: this->current_resources)
	{
		ImGui::PushID(resource.get_id());
		ImGui::Bullet();
		ImGui::TextWrapped("%s (id: %d)", resource.get_name().c_str(), resource.get_id());

		if (ImGui::Button("View"))
		{
			this->application->tool_factory.view(resource);
		}

		ImGui::SameLine();
		if (ImGui::Button("Edit"))
		{
			this->application->tool_factory.edit(resource);
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			this->pending_resource = resource;
			ImGui::OpenPopup("confirm_remove");
		}

		if (ImGui::BeginPopup("confirm_remove"))
		{
			ImGui::TextWrapped(
				"Are you sure you wish to remove '%s' (id: %d)?",
				this->pending_resource.get_name().c_str(),
				this->pending_resource.get_id());
			ImGui::TextColored(ImVec4(1, 0, 0, 1),
				"All actions that depend on this resource will also be removed!");

			if (ImGui::Button("Yes"))
			{
				remove();
				populate_resources(type);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("No"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	if (ImGui::BeginPopup("add_resource"))
	{
		ImGui::PushItemWidth(256);
		{
			ImGui::Text("Name");

			ImGui::SameLine();
			ImGui::InputString("##name", pending_name);
		}
		ImGui::PopItemWidth();

		if (ImGui::Button("Add"))
		{
			add();
			populate_resources(type);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void plato::Application::ResourcesWindow::import(
	const std::string& type,
	Importer& importer)
{
	std::string filename = this->application->get_input_filename();
	if (!filename.empty())
	{
		principle::LoadingScreen loading_screen(
			this->application->get_event_queue(),
			*this->application->get_display(),
			0, 0);

		int previous_count = 0;
		int count = Importer::import_from_file(
			filename,
			importer,
			*this->application->brochure,
			[&loading_screen, &previous_count, type](auto count)
			{
				loading_screen.update();
				{
					std::stringstream message;
					message << count << " " << type << "(s)" << " " << "imported.";
					loading_screen.set_message(message.str());
				}

				return !loading_screen.get_quit_pending();
			});

		std::printf("Imported %d %s(s) total.\n", count, type.c_str());
	}
}

void plato::Application::ResourcesWindow::populate_resources(const std::string& type)
{
	this->current_resources.clear();

	std::unordered_set<int> exact;

	auto resources = this->application->brochure->resources();
	int count = 0;
	for (auto i = resources.by_name(this->filter); i != resources.end(); ++i)
	{
		if (i->get_type() != type)
		{
			continue;
		}

		++count;
		if (count > limit && limit > 0)
		{
			break;
		}

		this->current_resources.push_back(*i);
		exact.insert(i->get_id());
	}

	for (auto i = resources.by_fuzzy_name(this->filter); i != resources.end(); ++i)
	{
		if (i->get_type() != type)
		{
			continue;
		}

		++count;
		if (count > limit && limit > 0)
		{
			break;
		}

		if (exact.count(i->get_id()))
		{
			continue;
		}

		this->current_resources.push_back(*i);
	}
}

void plato::Application::ResourcesWindow::populate_locations()
{
	this->current_locations.clear();

	if (!this->filter.empty())
	{
		auto locations = this->application->atlas->fuzzy_find(this->filter);
		if (!locations.empty())
		{
			this->current_locations = std::move(locations);
			return;
		}
	}

	this->current_locations.push_back(this->application->atlas->root());
}
