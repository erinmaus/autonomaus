// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "plato/gui/brochureWidgetManager.hpp"
#include "principle/deps/imgui/imgui.h"
#include "principle/common/gui.hpp"
#include "twoflower/relationships/requirement.hpp"

plato::BrochureWidgetManager::BrochureWidgetManager(
	twoflower::Brochure& brochure,
	mapp::Atlas& atlas,
	mapp::Game& game,
	Toolbox& toolbox)
{
	this->brochure = &brochure;
	this->atlas = &atlas;
	this->game = &game;
	this->toolbox = &toolbox;
}

twoflower::Brochure& plato::BrochureWidgetManager::get_brochure() const
{
	return *this->brochure;
}

plato::Toolbox& plato::BrochureWidgetManager::get_toolbox() const
{
	return *this->toolbox;
}

mapp::Atlas& plato::BrochureWidgetManager::get_atlas()
{
	return *this->atlas;
}

mapp::Game& plato::BrochureWidgetManager::get_game()
{
	return *this->game;
}

void plato::BrochureWidgetManager::add(
	const std::string& resource_type,
	ResourceView* view)
{
	this->resource_views[resource_type] = std::unique_ptr<ResourceView>(view);
}

void plato::BrochureWidgetManager::add(
	const std::string& resource_type,
	ResourceEditor* editor)
{
	this->resource_editors[resource_type] = std::unique_ptr<ResourceEditor>(editor);
}

void plato::BrochureWidgetManager::view(
	const twoflower::Resource& resource,
	bool use_default)
{
	ImGui::BeginGroup();
	ImGui::PushID("view");
	ImGui::PushID(resource.get_type().c_str());
	ImGui::PushID(resource.get_id());
	{
		auto iter = this->resource_views.find(resource.get_type());
		if (iter == this->resource_views.end() || use_default)
		{
			view_fallback(resource);
		}
		else
		{
			iter->second->update(*this, *this->brochure, resource);
		}
	}
	ImGui::PopID();
	ImGui::PopID();
	ImGui::PopID();
	ImGui::EndGroup();
}

void plato::BrochureWidgetManager::add(
	const std::string& action_type,
	const std::string& action_name,
	ActionView* view)
{
	auto key = std::make_pair(action_type, action_name);
	this->action_views[key] = std::unique_ptr<ActionView>(view);
}

void plato::BrochureWidgetManager::add(
	const std::string& action_type,
	const std::string& action_name,
	ActionEditor* editor)
{
	auto key = std::make_pair(action_type, action_name);
	this->action_editors[key] = std::unique_ptr<ActionEditor>(editor);
}

void plato::BrochureWidgetManager::edit(
	twoflower::Resource& resource,
	bool use_default)
{
	ImGui::BeginGroup();
	ImGui::PushID("edit");
	ImGui::PushID(resource.get_type().c_str());
	ImGui::PushID(resource.get_id());
	{
		auto iter = this->resource_editors.find(resource.get_type());
		if (iter == this->resource_editors.end() || use_default)
		{
			edit_fallback(resource);
		}
		else
		{
			iter->second->update(*this, *this->brochure, resource);
		}
	}
	ImGui::PopID();
	ImGui::PopID();
	ImGui::PopID();
	ImGui::EndGroup();
}

void plato::BrochureWidgetManager::view(
	const twoflower::Action& action,
	bool use_default)
{
	auto key = std::make_pair(action.get_type(), action.get_name());
	ImGui::BeginGroup();
	ImGui::PushID("edit");
	ImGui::PushID((action.get_type() + "." + action.get_name()).c_str());
	ImGui::PushID(action.get_id());
	{
		auto iter = this->action_views.find(key);
		if (iter == this->action_views.end() || use_default)
		{
			view_fallback(action);
		}
		else
		{
			iter->second->update(*this, *this->brochure, action);
		}
	}
	ImGui::PopID();
	ImGui::PopID();
	ImGui::PopID();
	ImGui::EndGroup();
}

void plato::BrochureWidgetManager::edit(
	twoflower::Action& action,
	bool use_default)
{
	auto key = std::make_pair(action.get_type(), action.get_name());
	ImGui::BeginGroup();
	ImGui::PushID("edit");
	ImGui::PushID((action.get_type() + "." + action.get_name()).c_str());
	ImGui::PushID(action.get_id());
	{
		auto iter = this->action_editors.find(key);
		if (iter == this->action_editors.end() || use_default)
		{
			edit_fallback(action);
		}
		else
		{
			iter->second->update(*this, *this->brochure, action);
		}
	}
	ImGui::PopID();
	ImGui::PopID();
	ImGui::PopID();
	ImGui::EndGroup();
}

void plato::BrochureWidgetManager::view_fallback(
	const twoflower::Resource& resource)
{
	ImGui::Text("Resource %d:", resource.get_id());

	ImGui::Bullet();
	ImGui::TextWrapped("Name: %s", resource.get_name().c_str());

	ImGui::Bullet();
	ImGui::TextWrapped("Type: %s", resource.get_type().c_str());

	if (ImGui::CollapsingHeader("Actions"))
	{
		ImGui::BeginGroup();

		auto actions = this->brochure->actions(resource);
		for (auto action: actions)
		{
			ImGui::PushID(action.get_id());

			ImGui::Bullet();
			view_fallback(action);

			ImGui::PopID();
		}

		ImGui::EndGroup();
	}
}

void plato::BrochureWidgetManager::edit_fallback(
	twoflower::Resource& resource)
{
	bool update = false;

	ImGui::TextWrapped("Resource %d", resource.get_id());
	ImGui::Separator();

	ImGui::Text("Name");
	ImGui::SameLine();
	{
		std::string value = resource.get_name();
		if (ImGui::InputString("##name", value))
		{
			resource.builder().set_name(value);
			update = true;
		}
	}

	ImGui::Text("Type");
	ImGui::SameLine();
	{
		std::string value = resource.get_type();
		if (ImGui::InputString("##type", value))
		{
			resource.builder().set_type(value);
			update = true;
		}
	}

	if (update)
	{
		brochure->builder().update_resource(resource);
	}

	twoflower::Action action_pending_removal;
	if (ImGui::CollapsingHeader("Actions"))
	{
		ImGui::BeginGroup();

		if (ImGui::Button("Add"))
		{
			ImGui::OpenPopup("add_action");
		}

		auto actions = this->brochure->actions(resource);
		for (auto action: actions)
		{
			ImGui::PushID(action.get_id());

			twoflower::Action a = action;
			edit_fallback(a);

			ImGui::PopID();
		}

		if (ImGui::BeginPopup("add_action"))
		{
			auto current = brochure->actions().definitions();
			auto end = brochure->actions().end();
			twoflower::Action action;

			int selected_index = ImGui::GetStateStorage()->GetInt(
				ImGui::GetID("action_index"), 0);

			ImGui::ListBoxHeader("##Action");
			int index = 0;
			while (current != end)
			{
				std::string m = current->get_type() + "." + current->get_name();
				if (ImGui::Selectable(m.c_str(), selected_index == index))
				{
					selected_index = index;
				}

				if (selected_index == index)
				{
					action = *current;
				}

				++index;
				++current;
			}
			ImGui::ListBoxFooter();
			ImGui::GetStateStorage()->SetInt(ImGui::GetID("action_index"), selected_index);

			if (ImGui::Button("Add"))
			{
				if (brochure->has_action_definition(action.get_type(), action.get_name()))
				{
					brochure->builder().connect(action, resource);
				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::EndGroup();
	}

	ImGui::Spacing();
}

void plato::BrochureWidgetManager::view_fallback(
	const twoflower::Action& action)
{
	ImGui::TextWrapped(
		"Action %s.%s (id: %d)",
		action.get_type().c_str(),
		action.get_name().c_str(),
		action.get_id());

	ImGui::Indent();

	if (!action.get_task().empty())
	{
		ImGui::BulletText("Task: %s", action.get_task().c_str());
	}

	if (action.has_cost_multiplier())
	{
		ImGui::BulletText("Cost: %f", action.get_cost_multiplier());
	}

	auto requirements = this->brochure->requirements(action);
	for (auto requirement: requirements)
	{
		std::stringstream message;

		if (requirement.is_input() && requirement.is_output())
		{
			message << "requires & produces" << " ";
		}
		else if (requirement.is_input())
		{
			message << "requires" << " ";
		}
		else if (requirement.is_output())
		{
			message << "produces" << " ";
		}

		message << requirement.get_resource().get_type();
		if (requirement.get_count() > 0.0f &&
			requirement.get_count() != 1.0f)
		{
			message << "(s)";
		}
		message << " ";

		message << requirement.get_resource().get_name();
		message << " " << "(" << "id:" << " ";
		message << requirement.get_resource().get_id();
		message << ")" << " ";

		if (requirement.get_count() > 0.0f)
		{
			message << "x";

			int count = requirement.get_count();
			float remainder = requirement.get_count() - count;

			if (count > 0)
			{
				message << count << " ";
			}

			if (remainder != 0)
			{
				int denominator = 1.0f / remainder;
				message << "1/" << denominator << " ";
			}
		}

		ImGui::Bullet();
		ImGui::TextWrapped("%s", message.str().c_str());
	}
	ImGui::Unindent();
}

void plato::BrochureWidgetManager::edit_fallback(twoflower::Action& action)
{
	ImGui::Separator();
	ImGui::Spacing();

	if (!action.get_id())
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Cannot edit action definition %s.%s.",
			action.get_type().c_str(), action.get_name().c_str());
		return;
	}

	ImGui::Text(
		"Action %s.%s (%d)",
		action.get_type().c_str(),
		action.get_name().c_str(),
		action.get_id());

	ImGui::Text("Task");
	{
		auto task = action.get_task();
		if (ImGui::InputString("##task", task))
		{
			action.builder().set_task(task);
			brochure->builder().update_action(action);
		}
	}

	ImGui::Text("Cost");
	{
		float cost = action.get_cost_multiplier();
		if (ImGui::InputFloat("##cost", &cost))
		{
			action.builder().set_cost_multiplier(cost);
			brochure->builder().update_action(action);
		}

		ImGui::SameLine();
		if (ImGui::Button("Unset"))
		{
			action.builder().unset_cost_multiplier();
			brochure->builder().update_action(action);
		}
	}

	if (ImGui::CollapsingHeader("Requirements"))
	{
		if (ImGui::Button("Add Requirement"))
		{
			ImGui::OpenPopup("add_requirement");
		}

		auto requirements = brochure->requirements(action);
		if (requirements.begin() == requirements.end())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto requirement: requirements)
			{
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::PushID(requirement.get_id());

				ImGui::BulletText("Requirement (id: %d)", requirement.get_id());

				ImGui::SameLine();
				if (ImGui::Button("Remove"))
				{
					brochure->builder().remove_requirement(requirement);
				}

				ImGui::PushItemWidth(128);

				int resource_id = ImGui::GetStateStorage()->GetInt(
					ImGui::GetID("resource_id"),
					requirement.get_resource().get_id());
				ImGui::Text("Resource ID");
				ImGui::SameLine();
				if (ImGui::InputInt("##resource", &resource_id))
				{
					if (brochure->resources().has(resource_id))
					{
						requirement.builder().set_resource(
							brochure->resources().get(resource_id));
						brochure->builder().update_requirement(requirement);
					}

					ImGui::GetStateStorage()->SetInt(
						ImGui::GetID("resource_id"), resource_id);
				}
				ImGui::BulletText("Name: %s", requirement.get_resource().get_name().c_str());
				ImGui::BulletText("Type: %s", requirement.get_resource().get_type().c_str());

				bool updated_count = false;
				int count = requirement.get_count();
				float remainder = requirement.get_count() - count;
				int denominator;
				if (remainder == 0.0f)
				{
					denominator = 1;
				}
				else
				{
					denominator = 1.0f / remainder;
				}
				
				ImGui::Text("%20s", "Count:");
				ImGui::SameLine();
				if (ImGui::InputInt("##count", &count))
				{
					updated_count = true;
				}

				ImGui::Text("%20s", "Fraction (1/x):");
				ImGui::SameLine();
				if (ImGui::InputInt("##fraction", &denominator))
				{
					if (denominator == 0)
					{
						denominator = 1;
					}
					else
					{
						denominator = std::abs(denominator);
						updated_count = true;
					}
				}

				if (updated_count)
				{
					requirement.builder().set_count(count + std::abs(1.0f / denominator - 1.0f));
					brochure->builder().update_requirement(requirement);
				}

				bool is_input = requirement.is_input();
				if (ImGui::Checkbox("Input", &is_input))
				{
					requirement.builder().set_is_input(is_input);
					brochure->builder().update_requirement(requirement);
				}

				bool is_output = requirement.is_output();
				if (ImGui::Checkbox("Output", &is_output))
				{
					requirement.builder().set_is_output(is_output);
					brochure->builder().update_requirement(requirement);
				}

				ImGui::PopItemWidth();

				ImGui::PopID();
			}
		}

		if (ImGui::BeginPopup("add_requirement"))
		{
			int resource_id = ImGui::GetStateStorage()->GetInt(ImGui::GetID("resource_id"));
			ImGui::Text("Resource ID");
			if (ImGui::InputInt("##resource", &resource_id))
			{
				ImGui::GetStateStorage()->SetInt(
					ImGui::GetID("resource_id"), resource_id);
			}

			if (brochure->resources().has(resource_id))
			{
				auto resource = brochure->resources().get(resource_id);
				ImGui::BulletText("Name: %s", resource.get_name().c_str());
				ImGui::BulletText("Type: %s", resource.get_type().c_str());
			}
			else
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Resource not found.");
			}

			if (ImGui::Button("Add"))
			{
				if (brochure->resources().has(resource_id))
				{
					brochure->builder().connect(
						twoflower::Requirement(),
						action,
						brochure->resources().get(resource_id));
				}

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
}
